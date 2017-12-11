#[derive(Debug)]
struct BinTree {}

#[cfg(test)]
mod tests {
    use super::BinTree;

    #[test]
    fn test_traverse() {
        let tree = BinTree {};
        assert_eq!(tree, 1)
    }
}
