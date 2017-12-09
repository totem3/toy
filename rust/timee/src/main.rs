extern crate time;
fn main() {
    let t = time::now();
    println!("{}", t.to_timespec().sec);
    println!("{:16}", t.to_timespec().sec);
}
