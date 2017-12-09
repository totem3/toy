fn f(i: &i32) {
    println!("{}", i);
}

fn main() {
    let b = Box::new(1i32);
    f(&*b);
}
