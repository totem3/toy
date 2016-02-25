extern crate getopts;
use getopts::Options;
use std::env;
use std::path::Path;
use std::fs;
use std::fs::{OpenOptions};
use std::io::prelude::*;

fn print_usage(program: &str, opts: Options) {
    let brief = format!("Usage: {} [options]", program);
    print!("{}", opts.usage(&brief));
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let program = args[0].clone();
    let mut opts = Options::new();
    opts.optflag("d", "dir", "directories");
    opts.optopt("a", "add", "add to favorite", "TARGET");
    opts.optflag("h", "help", "print this help");
    let matches = match opts.parse(&args[1..]) {
         Ok(m) => {m},
         Err(e) => { panic!(e.to_string())}
    };

    if matches.opt_present("h") {
        print_usage(&program, opts);
        return;
    }

    let is_add = matches.opt_present("a");
    let is_dir = true; // matches.opt_present("d"); now only dir option so always true. default in future

    if is_add {
        let add_value = matches.opt_str("a").unwrap();
        let dir_str = format!("{}/.fav", env::var("HOME").unwrap()); // if not set, just panic
        let dir = Path::new(&dir_str);
        if !dir.exists() {
            fs::create_dir(dir);
        }
        let path = format!("{}/directories", dir_str);
        let mut file = match OpenOptions::new().read(true).write(true).append(true).create(true).open(path) {
            Ok(f) => { f },
            Err(e) => { panic!(e.to_string()) },
        };
        file.write(format!("{}\n", add_value).as_ref());
    }

    if is_dir {
        let ds = get_directories();
    }
}

fn get_directories() {
    let dir_str = format!("{}/.fav", env::var("HOME").unwrap()); // if not set, just panic
    let dir = Path::new(&dir_str);
    if !dir.exists() {
        fs::create_dir(dir);
    }
    let path = format!("{}/directories", dir_str);
    let mut file = match OpenOptions::new().read(true).write(true).create(true).open(path) {
        Ok(f) => { f },
        Err(e) => { panic!(e.to_string()) },
    };

    let mut s = String::new();
    file.read_to_string(&mut s);
    print!("{}", s);
}
