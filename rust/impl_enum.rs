enum Hoge {
    Foo,
    Bar,
    Baz,
}

trait Greeting {
    fn greet(&self);
}

impl Greeting for Hoge {
    fn greet(&self) {
        let hello = match *self {
            Hoge::Foo => "foo",
            Hoge::Bar => "bar",
            Hoge::Baz => "baz",
        };
        println!("{}", hello);
    }
}

fn main() {
    Hoge::Foo.greet();
    Hoge::Bar.greet();
    Hoge::Baz.greet();
}
