extern crate rustfft;
extern crate ndarray;
extern crate image;

use rustfft::FFTplanner;
use rustfft::num_complex::Complex;
use rustfft::num_traits::Zero;
use ndarray::{ArrayViewMut, ArrayViewMut2, Dimension};
use image::{ColorType, GenericImage, DynamicImage};

fn fft2(input: &mut ArrayViewMut2<Complex<f64>>, output: &mut ArrayViewMut2<Complex<f64>>) {
    let dimensions = input.raw_dim().into_pattern();
    let len = dimensions.0 * dimensions.1;
    let mut tmp = vec![Zero::zero(); len];
    let mut tmp_buf = ArrayViewMut::from_shape(dimensions, &mut tmp).unwrap();
    {
        let mut output_rows = tmp_buf.genrows_mut().into_iter();
        let mut planner = FFTplanner::new(false);
        let fft = planner.plan_fft(dimensions.0);
        for mut input_row in input.genrows_mut() {
            let mut output_row = output_rows.next().unwrap();
            fft.process(&mut input_row.as_slice_mut().unwrap(),
            &mut output_row.as_slice_mut().unwrap());
        }
    }

    let mut inputview = tmp_buf.reversed_axes();
    {
        let mut output_rows = output.genrows_mut().into_iter();
        let mut planner = FFTplanner::new(false);
        let fft = planner.plan_fft(dimensions.0);
        for mut input_row in inputview.genrows_mut() {
            let mut output_row = output_rows.next().unwrap();
            fft.process(&mut input_row.to_vec(),
                        &mut output_row.as_slice_mut().expect("output"));
        }
    }

    // let mut result1 = Vec::with_capacity(len);
    // let mut max = 0.;
    // for v in outputview.reversed_axes() {
    //     let mut real = v.re.abs();
    //     if real < 1.0 {
    //         real = 1.0;
    //     }
    //     real = real.log10();
    //     if max < real {
    //         max = real;
    //     }
    //     result1.push(real);
    // }
    // let mut result: Vec<u8> = Vec::with_capacity(len);
    // for v in result1 {
    //     let norm = ((v / max) * 255.).floor() as u8;
    //     result.push(norm);
    // }

    // // shift
    // let mut shifted: Vec<u8> = Vec::with_capacity(len);
    // shifted.resize(len, 0);
    // let view = ArrayView::from_shape(dimensions, &result).unwrap();
    // let width = dimensions.0;
    // let height = dimensions.1;
    // for (i, row) in view.genrows().into_iter().enumerate() {
    //     let row = row.as_slice().unwrap();
    //     let mut new_row = Vec::with_capacity(width);
    //     new_row.extend_from_slice(&row[width/2..]);
    //     new_row.extend_from_slice(&row[0..width/2]);
    //     let idx = width * ((i + (height/2)) % height);
    //     shifted[idx..idx+width].copy_from_slice(&new_row);
    // }
    // shifted
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

fn spectral(img: &DynamicImage) -> Vec<u8> {
    let dimensions = img.dimensions();
    let shape = (dimensions.0 as usize, dimensions.1 as usize);
    let pixels: Vec<u8> = img.to_luma().pixels().map(|a| a.data[0]).collect();
    let mut buffer: Vec<Complex<f64>> = pixels.iter().map(|a| Complex::new(*a as f64, 0.0)).collect();
    let mut outputbuffer = vec![Zero::zero(); buffer.len()];
    let mut input = ArrayViewMut::from_shape(shape, &mut buffer).unwrap();
    let mut output = ArrayViewMut::from_shape(shape, &mut outputbuffer).unwrap();
    fft2(&mut input, &mut output);
    let shifted = shift(&output);
    normalize(shifted)
}

fn main() {
    let image = image::open("gray.jpg").unwrap();
    let fimage = spectral(&image);
    let _ = image::save_buffer("spectral.jpg", &fimage, image.dimensions().0 as u32, image.dimensions().1 as u32, ColorType::Gray(8));
}
