use std::process::{Command,Stdio};
use std::io::{Write,Read};

fn main() {
    let mut ps = Command::new("ps").arg("aux").stdout(Stdio::piped()).spawn().unwrap();
    let mut grep = Command::new("grep").arg("[v]im").stdin(Stdio::piped()).stdout(Stdio::piped()).spawn().unwrap();

    if let Some(ref mut stdout) = ps.stdout {
        if let Some(ref mut stdin) = grep.stdin {
            let mut buf : Vec<u8> = Vec::new();
            stdout.read_to_end(&mut buf).unwrap();
            stdin.write_all(&buf).unwrap();
        }
    }

    let res = grep.wait_with_output().unwrap().stdout;
    let s = String::from_utf8(res).unwrap();
    println!("{}", s);
}
