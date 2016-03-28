typedef struct entry {
    char *key;
    int value;
} entry_t;

typedef struct tree {
    struct tree *parent;
    struct tree *left;
    struct tree *right;
    entry_t entry;
} tree_t;

void rotateR(tree_t *t);
void rotateL(tree_t *t);
void rotateRL(tree_t *t);
void rotateLR(tree_t *t);
void insert(tree_t *t, entry_t e);
void insert_tree(tree_t *t, tree_t *t2);
int tree_height (tree_t *t);
int bias (tree_t *t);
void print_tree (tree_t *t, int depth);
void traverse_tree (tree_t *t, void (*f)(tree_t *t));
