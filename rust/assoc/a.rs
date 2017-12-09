use std::fmt::Debug;
trait HasValue {
    type T: Debug;
    fn get_value(&self) -> Self::T;
}

#[derive(Debug)]
struct Person {
    age: i32,
}

impl HasValue for Person {
    type T = i32;
    fn get_value(&self) -> Self::T {
        self.age
    }
}

#[derive(Debug)]
struct Hoge;

impl HasValue for Hoge {
    type T = String;
    fn get_value(&self) -> Self::T {
        "HOGEHOGEHOGEHOGE".to_string()
    }
}

fn print_value<H: HasValue>(v: &H) {
    println!("{:?}", v.get_value());
}

fn main() {
    let p = Person { age: 100 };
    let h = Hoge;
    let vs = vec![&p];
    for v in vs {
        print_value(v);
    }
}
