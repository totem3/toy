use std::process;

fn main() {
    let cmd = process::Command::new("ls").args(&["-l", "-a"]).output().unwrap();
    println!("{}", String::from_utf8(cmd.stdout).unwrap());
    println!("{}", String::from_utf8(cmd.stderr).unwrap());
}
