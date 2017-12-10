#[cfg(test)]
#[macro_use]
extern crate quickcheck;

use std::rc::Rc;
use std::collections::BinaryHeap;
use std::cmp::{Ord, Ordering};
use std::collections::HashMap;
use std::fmt;

type CodeInfo = HashMap<u8, u8>;
type RevCodeInfo = HashMap<u8, u8>;

struct HuffTree {
    value: u8,
    occurrences: usize,
    left: Option<Rc<HuffTree>>,
    right: Option<Rc<HuffTree>>,
}

fn index(data: &[u8], index: usize) -> u8 {
    let byte = data[index / 8];
    let shift = 7 - (index % 8);
    let bit = (byte & (1 << shift)) >> shift;
    bit
}

impl fmt::Debug for HuffTree {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "HuffTree({})", self.value)
    }
}

impl HuffTree {
    fn new(value: u8, occurrences: usize) -> HuffTree {
        HuffTree {
            value,
            occurrences,
            left: None,
            right: None,
        }
    }

    #[allow(unused)]
    fn to_string(&self, indent: u8) -> String {
        let mut space = String::new();
        let indent = indent + 2;
        for _ in 0..indent {
            space.push(' ');
        }
        let mut s = format!("HuffTree(v:{}, o:{})", self.value, self.occurrences);
        if let Some(ref left) = self.left {
            s = s + &format!("\n{}{}", space, left.to_string(indent + 2));
        }
        if let Some(ref right) = self.right {
            s = s + &format!("\n{}{}", space, right.to_string(indent + 2));
        }
        s
    }

    fn traverse(&self, bits: u8) -> Vec<(u8, u8)> {
        let mut stream = Vec::new();
        match self.left {
            Some(ref v) => {
                stream.append(&mut v.traverse(bits << 1));
            }
            None => {}
        };
        match self.right {
            Some(ref v) => {
                stream.append(&mut v.traverse(bits << 1 | 1));
            }
            None => {}
        };
        match self.left {
            Some(_) => {}
            None => {
                stream.push((self.value, bits));
            }
        }
        match self.right {
            Some(_) => {}
            None => stream.push((self.value, bits)),
        }
        stream
    }
}

impl PartialEq for HuffTree {
    fn eq(&self, other: &HuffTree) -> bool {
        self.occurrences == other.occurrences
    }
}

impl Eq for HuffTree {}


impl PartialOrd for HuffTree {
    fn partial_cmp(&self, other: &HuffTree) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for HuffTree {
    fn cmp(&self, other: &HuffTree) -> Ordering {
        other.occurrences.cmp(&self.occurrences)
    }
}

fn build_tree<'a>(data: &'a [u8]) -> (CodeInfo, RevCodeInfo) {
    let mut c = ::std::collections::HashMap::new();
    for v in data {
        let vv = c.entry(v).or_insert(0);
        *vv += 1;
    }
    let mut heap = BinaryHeap::new();
    for (v, o) in c {
        heap.push(HuffTree::new(*v, o));
    }
    let mut v = Vec::new();
    while let Some(h1) = heap.pop() {
        if v.is_empty() {
            if let Some(h2) = heap.pop() {
                let t = HuffTree {
                    value: 0,
                    occurrences: h1.occurrences + h2.occurrences,
                    left: Some(Rc::new(h2)),
                    right: Some(Rc::new(h1)),
                };
                v.push(t);
            } else {
                heap.push(h1);
                break;
            }
        } else {
            if let Some(h2) = v.pop() {
                let t = HuffTree {
                    value: 0,
                    occurrences: h1.occurrences + h2.occurrences,
                    left: Some(Rc::new(h1)),
                    right: Some(Rc::new(h2)),
                };
                v.push(t);
            } else {
                heap.push(h1);
                break;
            }
        }
    }
    let root = v.pop()
        .unwrap_or(HuffTree {
                       value: 0,
                       occurrences: 0,
                       left: None,
                       right: None,
                   });
    let vec = root.traverse(0);
    let mut t = HashMap::new();
    let mut r = HashMap::new();
    for (v, b) in vec {
        t.insert(v, b);
        r.insert(b, v);
    }
    (t, r)
}

fn bitwidth(v: &u8) -> usize {
    let mut v = v.clone();
    if v == 0 {
        1
    } else {
        let mut cnt = 1;
        while (v >> 1) != 0u8 {
            v >>= 1;
            cnt += 1;
        }
        cnt
    }
}

fn encode<'a>(data: &'a [u8], tree: &CodeInfo) -> Vec<u8> {
    let mut result = Vec::new();
    let mut bitpos = 0;
    for v in data {
        let code = tree.get(v).expect("unknwon code");
        let width = bitwidth(code);
        let cb_pos = bitpos % 8;
        let shift = (8 - (cb_pos as isize)) - (width as isize);
        let r = match cb_pos {
            0 => 0,
            _ => result.pop().unwrap_or(0),
        };
        if shift >= 0 {
            result.push(r | (code << shift));
        } else {
            result.push(r | (code >> (shift * -1)));
        }
        if cb_pos + width > 8 {
            let restwidth = cb_pos + width - 8;
            let mut or = 0;
            for _ in 0..restwidth {
                or = or << 1;
                or = or | 1;
            }
            let rest = code & or;
            result.push(rest << (8 - restwidth));
        }
        bitpos += width;
        for r in &result {
            print!("{:08b} ", r);
        }
    }
    result
}

fn decode<'a>(data: &'a [u8], tree: &CodeInfo, len: usize) -> Vec<u8> {
    let mut result = Vec::new();
    let mut i = 0;
    'outer: while i < data.len() * 8 {
        let mut byte = 0;
        'inner: while i < data.len() * 8 {
            byte = byte | index(data, i);
            i += 1;
            if let Some(v) = tree.get(&byte) {
                result.push(*v);
                break 'inner;
            } else {
                byte = byte << 1;
            }
        }
    }
    Vec::from(&result[0..len])
}

fn main() {
    let data = b"abcadbcdfabaa";
    let (tree, rev) = build_tree(data);
    let encoded = encode(data, &tree);
    let len = data.len();
    let decoded = decode(&encoded, &rev, len);
    let data = "abcadbcdfabaa";
}

#[cfg(test)]
mod tests {
    use super::{build_tree, encode, decode};

    quickcheck! {
        fn prop(s: Vec<u8>) -> bool {
            let data = &s[..];
            let (tree, rev) = build_tree(data);
            decode(&encode(data, &tree), &rev, data.len()) == data
        }
    }

    #[test]
    fn test_inversity() {
        let data = b"abcadbcdfabaa";
        let (tree, rev) = build_tree(data);
        assert_eq!(decode(&encode(data, &tree), &rev, data.len()), data);
    }

    #[test]
    fn test_compression() {
        let data = b"abcadbcdfabaa";
        let (tree, _) = build_tree(data);
        assert!(encode(data, &tree).len() < data.len());
    }
}
