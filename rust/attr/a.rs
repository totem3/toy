use std::time::SystemTime;

trait Formatter {
    fn format(&self, s: String) -> String;
}

struct DefaultFormatter;
impl Formatter for DefaultFormatter {
    fn format(&self, s: String) -> String {
        format!("[INFO] {}", s)
    }
}

struct TimeFormatter;
impl Formatter for TimeFormatter {
    fn format(&self, s: String) -> String {
        let now = SystemTime::now();
        format!("[{:?}][INFO] {}", now, s)
    }
}

struct Logger<'a> {
    formatter: &'a Formatter,
}

impl<'a> Logger<'a> {
    fn new(formatter: &'a Formatter) -> Self {
        Logger { formatter: formatter }
    }

    fn log(&self, s: String) {
        let s = self.format(s);
        println!("{}", s);
    }

    fn format(&self, s: String) -> String {
        self.formatter.format(s)
    }
}

fn main() {
    let f = DefaultFormatter {};
    let default = Logger::new(&f);
    default.log("Default Formatter".to_string());

    let t = TimeFormatter {};
    let time = Logger::new(&t);
    time.log("Time Formatter".to_string());
}
