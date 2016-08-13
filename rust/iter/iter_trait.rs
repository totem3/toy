trait Named {
    fn name(&self) -> String;
}

#[derive(Debug)]
struct Person {
    name: String,
}

#[derive(Debug)]
struct Dog {
    name: String,
}

impl Named for Person {
    fn name(&self) -> String {
        self.name.clone()
    }
}

impl Named for Dog {
    fn name(&self) -> String {
        self.name.clone()
    }
}

fn main() {
    let p = Person { name: "person".to_string() };
    let d = Dog { name: "dog".to_string() };
    // Error
    // let v: Vec<Named> = vec![p, d];
    {
        // need block
        let v: Vec<&Named> = vec![&p, &d];
        for n in v {
            println!("{}", n.name());
        }
    }

    {
        let v2: Vec<Box<Named>> = vec![Box::new(p), Box::new(d)];
        for n in v2 {
            println!("{}", n.name());
        }
    }
}
