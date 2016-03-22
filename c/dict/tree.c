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
    int lheight = 1;
    int rheight = 1;
    if (t->left != NULL) {
        lheight += tree_height(t->left);
    }
    if (t->right != NULL) {
        rheight += tree_height(t->right);
    }
    return (lheight >= rheight) ? lheight : rheight;
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

void print_tree(tree_t *t, int depth)
{
    // depth を渡していくのがいいかも
    // depth 1のノードの子供は| +-
    // depth 2のノードの子供は| | +-
    // 規則的にできそう
    char buffer[depth*2+3+10];
    int i;
    for (i = 0; i < depth; i++) {
        char *buf = &buffer[i*2];
        if (i == (depth - 1)) {
            memcpy(buf, "+-", 2);
        } else {
            memcpy(buf, "| ", 2);
        }
    }
    char *buf = &buffer[i*2];
    memcpy(buf, "+ ", 2);
    buf = &buffer[i*2+2];
    char valbuf[10];
    sprintf(valbuf, "%d", t->value);
    memcpy(buf, valbuf, 10);
    printf("%s\n", buffer);
    if (t->left != NULL) {
        if (tree_height(t->left) == 1) {
            char lbuffer[depth*2+3+10];
            int j;
            for (j = 0; j < depth; j++) {
                char *lbuf = &lbuffer[j*2];
                if (j == (depth - 1)) {
                    memcpy(lbuf, "+-", 2);
                } else {
                    memcpy(lbuf, "| ", 2);
                }
            }
            char *lbuf = &lbuffer[j*2];
            memcpy(lbuf, "+-", 2);
            lbuf = &lbuffer[j*2+2];
            char lvalbuf[10];
            sprintf(lvalbuf, "%d", t->left->value);
            memcpy(lbuf, lvalbuf, 10);
            printf("%s\n", lbuffer);
            // +- value
        } else {
            print_tree(t->left, (depth+1));
            // +-+ value
            // | +- left
            // | +- right
        }
    }
    if (t->right != NULL) {
        if (tree_height(t->right) == 1) {
            char rbuffer[depth*2+3+10];
            int j;
            for (j = 0; j < depth; j++) {
                char *rbuf = &rbuffer[j*2];
                memcpy(rbuf, "| ", 2);
            }
            char *rbuf = &rbuffer[j*2];
            memcpy(rbuf, "+-", 2);
            rbuf = &rbuffer[j*2+2];
            char lvarbuf[10];
            sprintf(lvarbuf, "%d", t->right->value);
            memcpy(rbuf, lvarbuf, 10);
            printf("%s\n", rbuffer);
            // +- value
        } else {
            print_tree(t->right, (depth+1));
            // +-+ value
            // | +- right
            // | +- right
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
