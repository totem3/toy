#include "./tree.h"
#include <string.h>

int count_node(tree_t *t)
{
    int n = 0;
    if (t->left != NULL) n++;
    if (t->right != NULL) n++;
    return n;
}

int tree_height(tree_t *t)
{
    int height++;
    if (t->left != NULL) {
        height++;
    }
}

void insert_node(tree_t *t, tree_t *node)
{
    if (node->value <= t->value) {
        if (t->left == NULL) {
            t->left = node;
        } else {
            insert_node(t->left, node);
        }
    } else if (node->value > t->value) {
        if (t->right == NULL) {
            t->right = node;
        } else {
            insert_node(t->right, node);
        }
    }
}

void print_tree(tree_t *t)
{
    printf("%d\n", t->value);
    if (t->left != NULL) {
        if (t->right != NULL) {
            printf("├%d\n", t->left->value);
            printf("└%d\n", t->right->value);
        } else {
            printf("└%d\n", t->left->value);
        }
    } else {
        if (t->right != NULL) {
            printf("└%d\n", t->right->value);
        }
    }
}

void traverse(tree_t *t, void (*f)(tree_t *))
{
    if (t->left != NULL) {
        traverse(t->left, f);
    }
    if (t->right != NULL) {
        traverse(t->right, f);
    }
    f(t);
}
