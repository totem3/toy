use std::fs::File;
use std::result::Result;
use std::io::Error;

fn open_file() -> Result<(), Error> {
    try!(File::create("hoge"));
    Ok(())
}

fn main() {
    let foo = open_file();
}
