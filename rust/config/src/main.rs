#![feature(custom_derive)]
extern crate rustc_serialize;
extern crate toml;

use std::env;
use std::fs::File;
use std::io::Read;
use std::str;
use rustc_serialize::Decodable;

#[derive(Debug,RustcDecodable)]
struct Config {
    core: CoreConfig,
}

#[derive(Debug,RustcDecodable)]
struct CoreConfig {
    port: u16,
    bind: String,
}

fn main() {
    let path = env::args().nth(1).unwrap();
    let mut file = File::open(path).unwrap();
    let mut buf = Vec::new();
    let _ = file.read_to_end(&mut buf);

    let s = str::from_utf8(&buf[..]).unwrap();
    println!("{}", s);

    let mut parser = toml::Parser::new(s);
    let toml = parser.parse();

    if toml.is_none() {
        for err in parser.errors {
            println!("{}", err.desc);
        }
    }

    let toml = toml.unwrap();
    println!("toml {:?}", toml);
    let mut decoder = toml::Decoder::new(toml::Value::Table(toml));
    let config = Config::decode(&mut decoder);
    // let config = toml::Value::Table(toml);
    // let config = match toml::decode(config) {
    //     Some(t) => t,
    //     None => panic!("failed"),
    // };

    println!("config {:?}", config.unwrap());
}
