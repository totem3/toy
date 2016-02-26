use std::net::{TcpListener, TcpStream};
use std::thread;
use std::io::Write;
use std::time::Duration;

fn handle_client(mut stream: TcpStream) {
    loop {
        stream.write("testtest\n".as_ref());
        let sec = Duration::new(1,0);
        thread::sleep(sec);
    }
}
fn main() {
    let listener = TcpListener::bind("127.0.0.1:8888").unwrap();
    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                thread::spawn(move || {
                    handle_client(stream)
                });
            },
            Err(e) => {
                println!("{}", e)
            },
        }
    }
    drop(listener);
}
