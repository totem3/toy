use std::rc::Rc;

trait Named {
    fn name(&self);
}

// Vec<T> の T は Sized でなければならない
// error[E0277]: the trait bound `Named: std::marker::Sized` is not satisfied
// #[derive(Debug,Clone)]
// struct Group {
//     members: Vec<Named>,
// }

// BoxとRcは全然違うということは踏まえたうえで
// cloneなどをderiveする場合、Box< trait object > ではダメ。cloneできないから。
// Box<Named> のsizeがわからないとcloneできない
// error[E0277]: the trait bound `Named: std::marker::Sized` is not satisfied
// = note: `Named` does not have a constant size known at compile-time
// = note: required because of the requirements on the impl of `std::clone::Clone` for `Box<Named>`
// = note: required because of the requirements on the impl of `std::clone::Clone` for `std::vec::Vec<Box<Named>>`
// = note: required by `std::clone::Clone::clone`
//
// debugも同様
// #[derive(Debug,Clone)]
// struct Group {
//     members: Vec<Box<Named>>,
// }

// RcはDebugがDeriveできないが, Cloneはできる
#[derive(Clone)]
struct Group {
    members: Vec<Rc<Named>>,
}

#[derive(Debug,Clone)]
struct Person {
    name: String,
    age: u32,
}

impl Named for Person {
    fn name(&self) {
        println!("{}", self.name);
    }
}

#[derive(Debug,Clone)]
struct Dog {
    name: String,
}

impl Named for Dog {
    fn name(&self) {
        println!("{}", self.name);
    }
}

fn main() {
    let p = Person {
        age: 28,
        name: "Person".to_string(),
    };
    let d = Dog { name: "Wanko".to_string() };

    let p2 = Rc::new(Person {
        age: 30,
        name: "Person2".to_string(),
    });
    let p3 = Rc::new(Person {
        age: 31,
        name: "Person3".to_string(),
    });

    let g = Group { members: vec![p2, p3] };
    for v in g.members {
        v.name();
    }

    {
        let vec: Vec<&Named> = vec![&p, &d];
        for v in vec {
            v.name()
        }
    }

    {
        let p = p.clone();
        let d = d.clone();
        let vec2: Vec<Box<Named>> = vec![Box::new(p), Box::new(d)];
        for v in vec2 {
            v.name();
        }
    }

    // 普通にやると型は合わない
    // error[E0308]: mismatched types
    //    |         let vec3 = vec![Rc::new(p), Rc::new(d)];
    //    |                                             ^ expected struct `Person`, found struct `Dog`
    // {
    //     let vec3 = vec![Rc::new(p), Rc::new(d)];
    //     for v in vec3 {
    //         v.name();
    //     }
    // }

    // trait objectにcastすることはできない
    // error: cast to unsized type: `Person` as `Named`
    // {
    //     let vec3 = vec![Rc::new(p as Named), Rc::new(d as Named)];
    //     for v in vec3 {
    //         v.name();
    //     }
    // }

    // trait objectのpointerであればcastできる
    {
        let vec3 = vec![Rc::new(&p as &Named), Rc::new(&d as &Named)];
        for v in vec3 {
            v.name();
        }
    }

    // 型を明示すればcastしなくても大丈夫
    {
        let vec3: Vec<Rc<Named>> = vec![Rc::new(p), Rc::new(d)];
        for v in vec3 {
            v.name();
        }
    }

}
