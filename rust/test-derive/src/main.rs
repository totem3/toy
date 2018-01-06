#[macro_use]
extern crate nom;
#[macro_use]
extern crate derive_read_cstruct;

use nom::IResult;
use nom::le_u32;

trait ReadCStruct {
    type Item;
    fn parse(input: &[u8]) -> IResult<&[u8], Self::Item>;
}

#[derive(PartialEq, Debug, ReadCStruct)]
struct Person {
    age: u32,
}


fn main() {
    let input = &[0x10, 0, 0, 0];
    let person = Person::parse(input);
    println!("{:?}", person);
    assert_eq!(Person{age: 16}, person.unwrap().1);
}
