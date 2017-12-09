extern crate time;
extern crate rustc_serialize;

use rustc_serialize::base64::{FromBase64, ToBase64, Config, CharacterSet, Newline};

fn main() {
    let t = time::now();
    println!("{:?}", t.to_timespec());
    let pt = t.to_timespec().sec as i32;
    println!("{:?}", pt);
    let mut ip: *const i32 = &pt;
    let bp: *const u8 = ip as *const _;

    let bs: &[u8] = unsafe { std::slice::from_raw_parts(bp, std::mem::size_of::<i32>()) };
    println!("{:?}", bs);
    let c = Config {
        char_set: CharacterSet::Standard,
        newline: Newline::CRLF,
        pad: true,
        line_length: None,
    };
    let encoded = bs.to_base64(c);
    println!("{:?}", encoded);

    let decoded = &encoded[..].from_base64().unwrap();
    println!("{:?}", decoded);

}
