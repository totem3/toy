extern crate rustfft;
extern crate ndarray;
extern crate image;

use rustfft::{FFTplanner, FFTnum};
use rustfft::num_complex::Complex;
use rustfft::num_traits::Zero;
use ndarray::{Array, Array2, ArrayView, ArrayViewMut, ArrayViewMut2, Dimension};
use image::{ColorType, GenericImage, DynamicImage};
use std::fmt::Debug;

fn _fft<T: FFTnum>(input: &mut [Complex<T>], output: &mut [Complex<T>], inverse: bool) {
    let mut planner = FFTplanner::new(inverse);
    let len = input.len();
    let fft = planner.plan_fft(len);
    fft.process(input, output);
}

fn fft<T: FFTnum>(input: &mut [Complex<T>], output: &mut [Complex<T>]) {
    _fft(input, output, false);
}

fn ifft<T: FFTnum + From<u32>>(input: &mut [Complex<T>], output: &mut [Complex<T>]) {
    _fft(input, output, true);
    for v in output.iter_mut() {
        *v = v.unscale(T::from(input.len() as u32));
    }
}

fn fft2(input: &mut ArrayViewMut2<Complex<f64>>, output: &mut ArrayViewMut2<Complex<f64>>) {
    fftnd(input, output, &[0,1]);
}

fn ifft2(input: &mut ArrayViewMut2<Complex<f64>>, output: &mut ArrayViewMut2<Complex<f64>>) {
    ifftnd(input, output, &[1,0]);
}

fn fftn<D: Dimension>(input: &mut ArrayViewMut<Complex<f64>, D>, output: &mut ArrayViewMut<Complex<f64>, D>, axis: usize) {
    _fftn(input, output, axis, false);
}

fn ifftn<D: Dimension>(input: &mut ArrayViewMut<Complex<f64>, D>, output: &mut ArrayViewMut<Complex<f64>, D>, axis: usize) {
    _fftn(input, output, axis, true);
}

fn _fftn<D: Dimension>(input: &mut ArrayViewMut<Complex<f64>, D>, output: &mut ArrayViewMut<Complex<f64>, D>, axis: usize, inverse: bool) {
    if inverse {
        mutate_lane(input, output, ifft, axis)
    } else {
        mutate_lane(input, output, fft, axis)
    }
}

fn fftnd<D: Dimension>(input: &mut ArrayViewMut<Complex<f64>, D>, output: &mut ArrayViewMut<Complex<f64>, D>, axes: &[usize]) {
    _fftnd(input, output, axes, false);
}

fn ifftnd<D: Dimension>(input: &mut ArrayViewMut<Complex<f64>, D>, output: &mut ArrayViewMut<Complex<f64>, D>, axes: &[usize]) {
    _fftnd(input, output, axes, true);
}

fn _fftnd<D: Dimension>(input: &mut ArrayViewMut<Complex<f64>, D>, output: &mut ArrayViewMut<Complex<f64>, D>, axes: &[usize], inverse: bool) {
    let len = axes.len();
    for i in 0..len {
        let axis = axes[i];
        _fftn(input, output, axis, inverse);
        if i < len - 1 {
            let mut outrows = output.genrows_mut().into_iter();
            for mut row in input.genrows_mut() {
                let mut outrow = outrows.next().unwrap();
                row.as_slice_mut().unwrap().copy_from_slice(outrow.as_slice_mut().unwrap());
            }
        }
    }
}

fn shift<T:Default+ Copy+ Clone>(view: &ArrayViewMut2<T>) -> Vec<T> {
    let dimensions = view.raw_dim().into_pattern();
    let len = dimensions.0 * dimensions.1;
    let width = dimensions.0;
    let height = dimensions.1;
    let mut shifted: Vec<T> = Vec::with_capacity(len);
    shifted.resize(len, T::default());
    for (i, row) in view.genrows().into_iter().enumerate() {
        let row = row.as_slice().unwrap();
        let mut new_row = Vec::with_capacity(width);
        new_row.extend_from_slice(&row[width/2..]);
        new_row.extend_from_slice(&row[0..width/2]);
        let idx = width * ((i + (height/2)) % height);
        shifted[idx..idx+width].copy_from_slice(&new_row);
    }
    shifted
}

fn normalize(view: Vec<Complex<f64>>) -> Vec<u8> {
    let max = view.iter().max_by(|a, b| (a.re.abs()).partial_cmp(&b.re.abs()).unwrap()).unwrap().re.abs();
    let max = max.log10();
    let mut result: Vec<u8> = Vec::with_capacity(view.len());
    for v in view {
        let mut real = v.re.abs();
        if real < 1.0 {
            real = 1.0;
        }
        real = real.log10();
        let norm = ((real / max) * 255.).floor() as u8;
        result.push(norm);
    }
    result
}

fn spectral(img: &DynamicImage) -> Vec<Complex<f64>> {
// fn spectral(img: &DynamicImage) -> Vec<u8> {
    let dimensions = img.dimensions();
    let shape = (dimensions.0 as usize, dimensions.1 as usize);
    let pixels: Vec<u8> = img.to_luma().pixels().map(|a| a.data[0]).collect();
    let mut buffer: Vec<Complex<f64>> = pixels.iter().map(|a| Complex::new(*a as f64, 0.0)).collect();
    let mut outputbuffer = vec![Zero::zero(); buffer.len()];
    let mut input = ArrayViewMut::from_shape(shape, &mut buffer).unwrap();
    let mut output = ArrayViewMut::from_shape(shape, &mut outputbuffer).unwrap();
    fft2(&mut input, &mut output);
    shift(&output)
    // let shifted = shift(&output);
    // normalize(shifted)
}

fn mutate_lane<T: Zero + Clone + Debug, D: Dimension>(input: &mut ArrayViewMut<T, D>, output: &mut ArrayViewMut<T, D>, f: fn(&mut [T], &mut [T]) -> (), axis: usize) {
    if axis > 0 {
        input.swap_axes(0, axis);
        output.swap_axes(0, axis);
        {
            let mut outrows = output.genrows_mut().into_iter();
            for mut row in input.genrows_mut() {
                let mut outrow = outrows.next().unwrap();
                let mut vec = row.to_vec();
                let mut out = vec![Zero::zero(); outrow.len()];
                f(&mut vec, &mut out);
                for i in 0..outrow.len() {
                    outrow[i] = out.remove(0);
                }
            }
        }
        input.swap_axes(0, axis);
        output.swap_axes(0, axis);
    } else {
        let mut outrows = output.genrows_mut().into_iter();
        for mut row in input.genrows_mut() {
            let mut outrow = outrows.next().unwrap();
            f(&mut row.as_slice_mut().unwrap(), &mut outrow.as_slice_mut().unwrap());
        }
    }
}

fn low_pass_filter_inplace(mut input: &mut ArrayViewMut2<Complex<f64>>, r: usize) {
    let len = input.shape().into_iter().product();
    let mut filter = Vec::with_capacity(len);
    let icenter = input.shape()[0] / 2;
    let jcenter = input.shape()[1] / 2;
    for i in 0..input.shape()[0] {
        for j in 0..input.shape()[1] {
            let idiff = if i >= icenter { i - icenter } else { icenter - i };
            let jdiff = if j >= jcenter { j - jcenter } else { jcenter - j };
            if idiff.pow(2) + jdiff.pow(2) < r.pow(2) {
                filter.push(Complex::new(1., 0.));
            } else {
                filter.push(Complex::new(0., 0.));
            }
        }
    }
    let filter = ArrayView::from_shape(input.shape(), &filter).unwrap();
    *input *= &filter;
}

fn high_pass_filter(input: Array2<Complex<f64>>, r: usize) -> Array2<Complex<f64>> {
    let len = input.shape().into_iter().product();
    let mut filter = Vec::with_capacity(len);
    let icenter = input.shape()[0] / 2;
    let jcenter = input.shape()[1] / 2;
    for i in 0..input.shape()[0] {
        for j in 0..input.shape()[1] {
            let idiff = if i >= icenter { i - icenter } else { icenter - i };
            let jdiff = if j >= jcenter { j - jcenter } else { jcenter - j };
            if idiff.pow(2) + jdiff.pow(2) < r.pow(2) {
                filter.push(Complex::new(0., 0.));
            } else {
                filter.push(Complex::new(1., 0.));
            }
        }
    }
    let filter = ArrayView::from_shape(input.shape(), &filter).unwrap();
    input * filter
}
fn low_pass_filter(input: Array2<Complex<f64>>, r: usize) -> Array2<Complex<f64>> {
    let len = input.shape().into_iter().product();
    let mut filter = Vec::with_capacity(len);
    let icenter = input.shape()[0] / 2;
    let jcenter = input.shape()[1] / 2;
    for i in 0..input.shape()[0] {
        for j in 0..input.shape()[1] {
            let idiff = if i >= icenter { i - icenter } else { icenter - i };
            let jdiff = if j >= jcenter { j - jcenter } else { jcenter - j };
            if idiff.pow(2) + jdiff.pow(2) < r.pow(2) {
                filter.push(Complex::new(1., 0.));
            } else {
                filter.push(Complex::new(0., 0.));
            }
        }
    }
    let filter = ArrayView::from_shape(input.shape(), &filter).unwrap();
    input * filter
}

fn main() {
    let mut args = ::std::env::args();
    let x = args.nth(1).unwrap();
    let filter_size = x.parse::<usize>().unwrap();

    let img = image::open("gray.png").unwrap();
    let dimensions = img.dimensions();
    let shape = (dimensions.0 as usize, dimensions.1 as usize);
    let pixels: Vec<u8> = img.to_luma().pixels().map(|a| a.data[0]).collect();
    let mut buffer: Vec<Complex<f64>> = pixels.iter().map(|a| Complex::new(*a as f64, 0.0)).collect();
    let mut outputbuffer = vec![Zero::zero(); buffer.len()];
    let mut resultbuffer = vec![Zero::zero(); buffer.len()];
    let mut input = ArrayViewMut::from_shape(shape, &mut buffer).unwrap();
    let mut output = ArrayViewMut::from_shape(shape, &mut outputbuffer).unwrap();
    {
        let mut result = ArrayViewMut::from_shape(shape, &mut resultbuffer).unwrap();
        fft2(&mut input, &mut output);
        let mut shifted_buffer = shift(&output);
        let mut shifted = ArrayViewMut::from_shape(shape, &mut shifted_buffer).unwrap();
        low_pass_filter_inplace(&mut shifted, filter_size);
        let shifted = shifted.view_mut();
        let mut unshifted_buffer = shift(&shifted);
        let mut unshifted = ArrayViewMut::from_shape(shape, &mut unshifted_buffer).unwrap();
        ifft2(&mut unshifted, &mut result);
    }
    let result: Vec<u8> = resultbuffer.into_iter().map(|x| x.re as u8).collect();
    let filename = format!("high_pass_filter_{}.jpg", filter_size);
    let _ = image::save_buffer(filename, &result, shape.0 as u32, shape.1 as u32, ColorType::Gray(8));
}

fn _main() {
    let mut args = ::std::env::args();
    let x = args.nth(1).unwrap();
    let filter_size = x.parse::<usize>().unwrap();
    let image = image::open("gray.jpg").unwrap();
    let mut fimage = spectral(&image);
    // let _ = image::save_buffer("filtered.jpg", &fimage, image.dimensions().0, image.dimensions().1, ColorType::Gray(8));
    // let fimageview = Array::from_shape_vec((image.dimensions().0 as usize, image.dimensions().1 as usize), fimage).unwrap();
    // let mut fimage: Vec<Complex<f64>> = vec![];
    // let image_array = low_pass_filter(fimageview, filter_size);
    // for row in image_array.genrows() {
    //     fimage.extend(&row);
    // }
    // let filtered_spectrum:Vec<u8> = fimage.iter().map(|x| x.re as u8).collect();
    // let _ = image::save_buffer("filtered.jpg", &filtered_spectrum, image.dimensions().0, image.dimensions().1, ColorType::Gray(8));
    {
        // let mut image_array = ArrayViewMut::from_shape((image.dimensions().0 as usize, image.dimensions().1 as usize), &mut fimage).unwrap();
        // let fimage = shift(&mut image_array);
    }
    // let mut fimage:Vec<Complex<f64>> = fimage.into_iter().map(|x| Complex::new(x as f64, 0.0)).collect();
    let mut image_array = ArrayViewMut::from_shape((image.dimensions().0 as usize, image.dimensions().1 as usize), &mut fimage).unwrap();
    let mut result_vec = vec![Zero::zero(); image_array.shape()[0] * image_array.shape()[1]];
    {
        let mut result = ArrayViewMut::from_shape((image_array.shape()[0], image_array.shape()[1]), &mut result_vec).unwrap();
        ifft2(&mut image_array, &mut result);
    }
    let fimage: Vec<u8> = result_vec.into_iter().map(|x| x.re as u8).collect();
    println!("{:?}", &fimage);
    // let filename = format!("filter_{}.jpg", filter_size);
    let filename = "hoge.jpg";
    let _ = image::save_buffer(filename, &fimage, image.dimensions().0, image.dimensions().1, ColorType::Gray(8));
}

#[cfg(test)]
mod test {
    use super::{fft, ifft, fft2, ifft2, mutate_lane, low_pass_filter, shift};
    use ndarray::{Array, ArrayViewMut, Axis, Dimension};
    use rustfft::FFTplanner;
    use rustfft::num_complex::Complex;
    use rustfft::num_traits::Zero;

    #[test]
    fn test_dimensions() {
        // 一番内側が後！二次元ならy,x
        let a = array![[1,2,3], [4,5,6]];
        assert_eq!(a.shape(), &[2,3]);
        let a = a.reversed_axes();
        assert_eq!(a.shape(), &[3,2]);

        let a = array![
            [
                [ 0,  1,  2],
                [ 3,  4,  5]
            ],
            [
                [ 6,  7,  8],
                [ 9, 10, 11]
            ]
        ];
        assert_eq!(a.shape(), &[2,2,3]);
    }

    #[test]
    fn swap_axes() {
        let mut a = array![[1,2,3], [4,5,6]];
        // slice取れる
        for mut v in a.genrows_mut() {
            assert!(v.as_slice_mut().is_some());
        }
        a.swap_axes(0,1);
        assert_eq!(a, array![[1,4],[2,5],[3,6]]);
        // slice取れない
        for mut v in a.genrows_mut() {
            assert!(v.as_slice_mut().is_none());
        }
    }

    #[test]
    fn test_slice_mut() {
        // slice_mutはsliceを取るものじゃなくてsliceしたArrayViewを取るもの
        // let mut a = array![[1,2,3], [4,5,6]];
        // let slice = a.slice_mut(s![..2,..1]);
        // assert_eq!(slice, array![[1,4]]);
    }

    #[test]
    fn test_col_slicing_mutate() {
        fn f(input: &mut [u8], output: &mut [u8]) {
            for i in 0..output.len() {
                output[i] = input[i] * ((i % 256) as u8);
            }
        }

        fn g<D: Dimension>(input: &mut ArrayViewMut<u8, D>, output: &mut ArrayViewMut<u8, D>, f: fn(&mut [u8], &mut [u8]) -> (), axis: usize) {
            if axis > 0 {
                input.swap_axes(0, axis);
                output.swap_axes(0, axis);
                {
                    let mut outrows = output.genrows_mut().into_iter();
                    for mut row in input.genrows_mut() {
                        let mut outrow = outrows.next().unwrap();
                        let mut vec = row.to_vec();
                        let mut out = vec![0; outrow.len()];
                        f(&mut vec, &mut out);
                        for i in 0..outrow.len() {
                            outrow[i] = out[i];
                        }
                    }
                }
                input.swap_axes(0, axis);
                output.swap_axes(0, axis);
            } else {
                let mut outrows = output.genrows_mut().into_iter();
                for mut row in input.genrows_mut() {
                    let mut outrow = outrows.next().unwrap();
                    f(&mut row.as_slice_mut().unwrap(), &mut outrow.as_slice_mut().unwrap());
                }
            }
        }

        let mut a = vec![0,1,2,3,4,5,6,7,8,9,10,11];
        let mut b = vec![0; 12];
        {
            let mut view = ArrayViewMut::from_shape((3,4), &mut a).unwrap();
            let mut outview = ArrayViewMut::from_shape((3,4), &mut b).unwrap();
            g(&mut view, &mut outview, f, 1);
        }
        assert_eq!(b, vec![0,0,0,0,4,5,6,7,16,18,20,22]);

        let mut a = vec![0,1,2,3,4,5,6,7,8,9,10,11];
        let mut b = vec![0; 12];
        {
            let mut view = ArrayViewMut::from_shape((3,4), &mut a).unwrap();
            let mut outview = ArrayViewMut::from_shape((3,4), &mut b).unwrap();
            g(&mut view, &mut outview, f, 0);
        }
        assert_eq!(b, vec![0,1,4,9,0,5,12,21,0,9,20,33]);
    }

    #[test]
    fn test_mutate_col() {
        fn f(input: &mut [u8], output: &mut [u8]) {
            let avg = input.iter().fold(0, |a, b| a + b) / input.len() as u8;
            for i in 0..output.len() {
                output[i] = input[i] + avg;
            }
        }
        let mut a = vec![0,1,2,
                         3,4,5,
                         6,7,8];
        let mut b = vec![0; 9];
        {
            let mut input = ArrayViewMut::from_shape((3,3), &mut a).unwrap();
            let mut output = ArrayViewMut::from_shape((3,3), &mut b).unwrap();
            mutate_lane(&mut input, &mut output, f, 1);
        }
        assert_eq!(b, vec![ 3,  5,  7,
                            6,  8, 10,
                            9, 11, 13]);
    }

    #[test]
    fn test_mutate_twice() {
        fn f(input: &mut [u8], output: &mut [u8]) {
            let avg = input.iter().fold(0, |a, b| a + b) / input.len() as u8;
            for i in 0..output.len() {
                output[i] = input[i] + avg;
            }
        }
        let mut a = vec![0,1,2,
                         3,4,5,
                         6,7,8];
        let mut b = vec![0; 9];
        {
            let mut input = ArrayViewMut::from_shape((3,3), &mut a).unwrap();
            let mut output = ArrayViewMut::from_shape((3,3), &mut b).unwrap();
            mutate_lane(&mut input, &mut output, f, 0);
        }
        assert_eq!(b, vec![ 1,  2,  3,
                            7,  8,  9,
                           13, 14, 15]);
        let mut a = b;
        let mut b = vec![0; 9];
        {
            let mut input = ArrayViewMut::from_shape((3,3), &mut a).unwrap();
            let mut output = ArrayViewMut::from_shape((3,3), &mut b).unwrap();
            mutate_lane(&mut input, &mut output, f, 1);
        }
        assert_eq!(b, vec![ 8, 10, 12,
                           14, 16, 18,
                           20, 22, 24]);
    }

// 45+0i, -4.499999999999995+12.363648387545796i, -4.500000000000002+5.362891166673942i
// -4.499999999999993+2.5980762113533165i, -4.4999999999999964+0.7934714131880969i, -4.499999999999997-0.793471413188092i
// -4.499999999999993-2.5980762113533165i, -4.5-5.36289116667394i, -4.499999999999992-12.363648387545796i
// 9.000000000000032+0.000000000000007105427357601002i, 80.99999999999999+0.0000000000000008881784197001252i, 71.99999999999999+0.000000000000015987211554602254i
// 63.00000000000001-0.0000000000000035527136788004994i, 53.999999999999986+0.0000000000000026326510883636403i, 44.999999999999986+0.0000000000000012366151173399847i
// 35.99999999999999-0.0000000000000035527136788004994i, 27.000000000000014-0.0000000000000035208295080637655i, 18.000000000000014-0.000000000000017223826671942234i

    #[test]
    fn test_inverse() {
        let a:Vec<Complex<f64>> = vec![1.,2.,3.,4.,5.,6.,7.,8.,9.].into_iter().map(|i|Complex::from(i)).collect();
        let mut b = vec![Zero::zero(); 9];
        let mut b2 = vec![Zero::zero(); 9];
        fft(&mut a.clone(), &mut b);
        ifft(&mut b, &mut b2);
        for (a, b) in a.iter().zip(b2) {
            assert!((a.re-b.re).abs() < 0.0001)
        }
    }

    #[test]
    fn test_inverse2() {
        let mut a:Vec<Complex<f64>> = vec![1.,2.,3.,4.,5.,6.,7.,8.,9.].into_iter().map(|i|Complex::from(i)).collect();
        let mut input = ArrayViewMut::from_shape((3,3), &mut a).unwrap();
        let mut b = vec![Zero::zero(); 9];
        let mut output = ArrayViewMut::from_shape((3,3), &mut b).unwrap();
        let mut b2 = vec![Zero::zero(); 9];
        {
            let mut output2 = ArrayViewMut::from_shape((3,3), &mut b2).unwrap();
            fft2(&mut input, &mut output);
            ifft2(&mut output, &mut output2);
        }
        let a:Vec<Complex<f64>> = vec![1.,2.,3.,4.,5.,6.,7.,8.,9.].into_iter().map(|i|Complex::from(i)).collect();
        for (a, b) in a.iter().zip(b2) {
            assert!((a.re-b.re).abs() < 0.0001)
        }
    }

    #[test]
    fn test_shift() {
        let mut vec = vec![1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16];
        let mut view = ArrayViewMut::from_shape((4,4), &mut vec).unwrap();
        let shifted = shift(&view);
        let expected = vec![1,   2,  3,  4,
                            5,   6,  7,  8,
                            9,  10, 11, 12,
                            13, 14, 15, 16];
        let expected = vec![11, 12,  9, 10,
                            15, 16, 13, 14,
                             3,  4,  1,  2,
                             7,  8,  5,  6];
        assert_eq!(shifted, expected);
    }

    #[test]
    fn test_shift_twice() {
        let mut vec = vec![1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16];
        let mut view = ArrayViewMut::from_shape((4,4), &mut vec).unwrap();
        let mut shifted = shift(&view);
        let mut view = ArrayViewMut::from_shape((4,4), &mut shifted).unwrap();
        let mut shifted = shift(&view);
        let expected = vec![1,   2,  3,  4,
                            5,   6,  7,  8,
                            9,  10, 11, 12,
                            13, 14, 15, 16];
        assert_eq!(shifted, expected);
    }
}
