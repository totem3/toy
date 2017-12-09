fn main() {
    let vec = vec![1, 2, 3, 4, 5];
    let len = vec.len();
    print!("{{");
    for (k, v) in vec.iter().enumerate() {
        print!("{}", v);
        if k != (len - 1) {
            print!(", ");
        }
    }
    println!("}}");
}
