use std::cmp::Ordering::{Less, Greater, Equal};
use std::mem;
use std::collections::VecDeque;

// A type implementing Indexed<K> that is used as value in a BinaryTree may be indexed by K,
// that is, lookup functions can take a key: K instead of the full value. This is necessary for
// implementing associative containers.
pub trait Indexed<K: ?Sized> {
    fn key(&self) -> &K;
}

impl<T> Indexed<T> for T
    where T: Ord
{
    fn key(&self) -> &T {
        self
    }
}

type Link<T> = Option<Box<Node<T>>>;

#[derive(Debug)]
struct Node<T: fmt::Debug> {
    value: T,
    left: Link<T>,
    right: Link<T>,
}

impl<T: fmt::Debug> Node<T> {
    fn new(value: T) -> Self {
        Node {
            value: value,
            left: None,
            right: None,
        }
    }
}

impl<T: fmt::Debug> fmt::Display for Node<T> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let left = match self.left {
            Some(ref v) => format!("{}", v),
            None => "None".to_string(),
        };
        let right = match self.right {
            Some(ref v) => format!("{}", v),
            None => "None".to_string(),
        };
        write!(f,
               "BinaryTree {:?}\n  left: {}\n  right: {}",
               self.value,
               left,
               right)
    }
}

fn dfs<T: fmt::Debug>(tree: &BinaryTree<T>) {
    fn _dfs<T: fmt::Debug>(node: &Node<T>, depth: usize) {
        // println!("{}{:?}", " ".repeat(depth), node.value);
        println!("{:?}", node.value);
        match node.left {
            Some(ref v) => {
                _dfs(v, depth + 2);
            }
            None => {
                // println!("{}・", " ".repeat(depth + 2));
            }
        }
        match node.right {
            Some(ref v) => {
                _dfs(v, depth + 2);
            }
            None => {
                // println!("{}・", " ".repeat(depth + 2));
            }
        }
    }
    match tree.root {
        Some(ref node) => _dfs(&node, 0),
        None => {}
    }
}

#[derive(Debug)]
pub struct BinaryTree<T: fmt::Debug> {
    root: Link<T>,
}

use std::fmt;
impl<T: fmt::Debug> fmt::Display for BinaryTree<T> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.root
            .as_ref()
            .ok_or(fmt::Error {})
            .and_then(|node| write!(f, "{}", node))
    }
}

impl<T: fmt::Debug> BinaryTree<T> {
    pub fn new() -> Self {
        BinaryTree { root: None }
    }

    // Get a reference to the link at which `key` is or should be located
    fn locate<K>(&self, key: &K) -> &Link<T>
        where T: Indexed<K>,
              K: Ord
    {
        let mut anchor = &self.root;
        while let Some(ref node) = *anchor {
            match node.value.key().cmp(key) {
                Less => anchor = &node.left,
                Greater => anchor = &node.right,
                Equal => return anchor,
            }
        }
        // No such entry, anchor is pointing to the insert position of value
        anchor
    }

    fn locate_mut<K>(&mut self, key: &K) -> &mut Link<T>
        where T: Indexed<K>,
              K: Ord
    {
        let mut anchor = &mut self.root;
        loop {
            // Not as simple as `locate`: The binding of `anchor` must
            // be removed before destructuring and re-assigning it in
            // order to avoid duplicate &muts
            match {
                      anchor
                  } {
                &mut Some(ref mut node) if key != node.value.key() => {
                    anchor = if key < node.value.key() {
                        &mut node.left
                    } else {
                        &mut node.right
                    }
                }

                other @ &mut Some(_) |
                other @ &mut None => return other,
            }
        }
    }

    pub fn lookup<K>(&self, key: &K) -> Option<&T>
        where T: Indexed<K>,
              K: Ord
    {
        self.locate(key).as_ref().map(|node| &node.value)
    }

    pub fn insert(&mut self, value: T) -> bool
        where T: Ord
    {
        let anchor = self.locate_mut(&value);
        match *anchor {
            Some(_) => false,
            None => {
                *anchor = Some(Box::new(Node::new(value)));
                true
            }
        }
    }

    pub fn delete<K>(&mut self, key: &K)
        where T: Indexed<K>,
              K: Ord
    {
        delete_node(self.locate_mut(key));
    }

    pub fn iter(&self) -> Iter<T> {
        Iter {
            current: &self.root,
            stack: Vec::new(),
        }
    }

    pub fn pre_order_iter(&self) -> PreOrderIter<T> {
        PreOrderIter {
            current: &self.root,
            stack: VecDeque::new(),
        }
    }
}

// Returns the next in-order successor in a subtree
fn successor<T: fmt::Debug>(mut next: &mut Link<T>) -> &mut Link<T> {
    loop {
        match {
                  next
              } {
            &mut Some(ref mut node) if node.left.is_some() => next = &mut node.left,
            other @ &mut Some(_) => return other,
            _ => unreachable!(),
        }
    }
}

// Removes a node by discarding it if it is a leaf, or by swapping it
// with its inorder successor (which, in this case, is always in a
// leaf) and then deleting the leaf.
fn delete_node<T: fmt::Debug>(link: &mut Link<T>) {
    if let Some(mut boxed_node) = link.take() {
        match (boxed_node.left.take(), boxed_node.right.take()) {
            (None, None) => (),
            (leaf @ Some(_), None) |
            (None, leaf @ Some(_)) => *link = leaf,
            (left, right) => {
                // take() followed by re-assignment looks like an awful hackjob, but appears
                // to be the only way to satisfy all cases in the match

                boxed_node.left = left;
                boxed_node.right = right;

                {
                    let node = &mut *boxed_node; // unbox
                    let next = successor(&mut node.right);
                    mem::swap(&mut node.value, &mut next.as_mut().unwrap().value);
                    delete_node(next);
                }
                *link = Some(boxed_node);
            }
        }
    }
}

impl<'a, T: 'a + fmt::Debug> IntoIterator for &'a BinaryTree<T> {
    type Item = &'a T;
    type IntoIter = Iter<'a, T>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

pub struct Iter<'a, T: 'a + fmt::Debug> {
    current: &'a Link<T>,
    stack: Vec<&'a Node<T>>,
}

impl<'a, T: 'a + fmt::Debug> Iterator for Iter<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        while let Some(node) = self.current.as_ref() {
            self.stack.push(node);
            self.current = &node.left;
        }
        self.stack
            .pop()
            .map(|node| {
                     self.current = &node.right;
                     &node.value
                 })
    }
}

pub struct PreOrderIter<'a, T: 'a + fmt::Debug> {
    current: &'a Link<T>,
    stack: VecDeque<&'a Node<T>>,
}

impl<'a, T: 'a + fmt::Debug> Iterator for PreOrderIter<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        while let Some(node) = self.current.as_ref() {
            self.stack.push_back(node);
            self.current = &node.left;
        }
        self.stack
            .pop_front()
            .map(|node| {
                     self.current = &node.right;
                     &node.value
                 })
    }
}

fn main() {
    let mut bt: BinaryTree<i32> = BinaryTree::new();
    bt.insert(1);
    bt.insert(3);
    bt.insert(5);
    bt.insert(9);
    bt.insert(4);
    bt.insert(2);
    println!("iter");
    for t in &bt {
        println!("{:?}", t);
    }

    println!("preorder iter");
    for t in bt.pre_order_iter() {
        println!("{}", t);
    }

    println!("--");
    dfs(&bt);
}
