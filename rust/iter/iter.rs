#[derive(Debug)]
pub struct Person {
    age: u32,
    name: String,
}

#[derive(Debug)]
pub struct Group {
    members: Vec<Person>,
}

impl Group {
    fn iter(&self) -> Iter {
        Iter { iter: self.members.iter() }
    }

    fn iter_mut(&mut self) -> IterMut {
        IterMut { iter: self.members.iter_mut() }
    }
}

impl IntoIterator for Group {
    type Item = Person;
    type IntoIter = ::std::vec::IntoIter<Person>;
    fn into_iter(self) -> Self::IntoIter {
        self.members.into_iter()
    }
}

struct Iter<'a> {
    iter: std::slice::Iter<'a, Person>,
}

impl<'a> Iterator for Iter<'a> {
    type Item = &'a Person;
    fn next(&mut self) -> Option<Self::Item> {
        self.iter.next()
    }
}

struct IterMut<'a> {
    iter: std::slice::IterMut<'a, Person>,
}

impl<'a> Iterator for IterMut<'a> {
    type Item = &'a mut Person;
    fn next(&mut self) -> Option<Self::Item> {
        self.iter.next()
    }
}

fn main() {
    let p1 = Person {
        age: 1,
        name: "foo".to_string(),
    };
    let p2 = Person {
        age: 2,
        name: "bar".to_string(),
    };
    let p3 = Person {
        age: 3,
        name: "baz".to_string(),
    };
    let mut group = Group { members: vec![p1, p2, p3] };
    for p in group.iter() {
        println!("Iter		{:?}", p);
    }
    for p in group.iter_mut() {
        p.age = p.age + 1;
        println!("IterMut		{:?}", p);
    }
    for p in group {
        println!("Iterator	{:?}", p);
    }
}
