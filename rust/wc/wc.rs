use std::io::{Read, BufRead, BufReader};
use std::fs::File;

fn count<R: Read>(input: R) -> (usize, usize, usize) {
    let mut reader = BufReader::new(input);
    let mut bytes = 0;
    let mut words = 0;
    let mut lines = 0;
    loop {
        let mut buf = Vec::new();
        if let Ok(size) = reader.read_until(b'\n', &mut buf) {
            if size == 0 {
                break;
            }
            let mut on_word = !char::from(buf[0]).is_whitespace();
            let mut position = 0;
            let mut line = buf.iter();
            while let Some(lsize) = line.position(|c| {
                let c = char::from(*c);
                c.is_whitespace()
            }) {
                position += lsize;
                if on_word {
                    words += 1;
                }
                position += 1;
                if position < buf.len() {
                    on_word = !char::from(buf[position]).is_whitespace();
                }
            }
            bytes += size;
            if buf[buf.len() - 1] == b'\n' {
                lines += 1;
            } else if !char::from(buf[buf.len() - 1]).is_whitespace() {
                words += 1;
            }
        }
    }
    (bytes, words, lines)
}

fn print_wc(bytes: usize, words: usize, lines: usize, path: &str) {
    let byte_width = if bytes < 10usize.pow(7) {
        8
    } else {
        format!("{}", bytes).len() + 1
    };

    let word_width = if words < 10usize.pow(7) {
        8
    } else {
        format!("{}", words).len() + 1
    };

    let line_width = if lines < 10usize.pow(7) {
        8
    } else {
        format!("{}", lines).len() + 1
    };

    println!("{:line_width$}{:word_width$}{:byte_width$} {}",
             lines,
             words,
             bytes,
             path,
             line_width = line_width,
             word_width = word_width,
             byte_width = byte_width);
}

fn main() {
    let mut args = ::std::env::args();
    let _ = args.next();
    let args_count = args.len();

    if args_count == 0 {
        let stdin = ::std::io::stdin();
        let (bytes, words, lines) = count(stdin);
        print_wc(bytes, words, lines, "");
        return;
    }

    let mut bytes = 0;
    let mut words = 0;
    let mut lines = 0;
    for arg in args {
        let path = arg.clone();
        match File::open(path) {
            Ok(file) => {
                if let Ok(is_dir) = file.metadata().map(|m| m.file_type()).map(|ty| ty.is_dir()) {
                    if is_dir {
                        println!("error: is directory");
                        continue;
                    }
                }
                let count = count(file);
                bytes += count.0;
                words += count.1;
                lines += count.2;


                print_wc(count.0, count.1, count.2, &arg);
            }
            Err(e) => {
                println!("error: {}", e);
            }
        };
    }
    if args_count > 1 {
        print_wc(bytes, words, lines, "total")
    }
}
