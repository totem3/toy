#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct tree {
    struct tree *parent;
    struct tree *left;
    struct tree *right;
    int value;
} tree_t;

void set_value(tree_t *t, int value)
{
    t->value = value;
}

void insert(tree_t *t, tree_t *t2)
{
    if (t->value > t2->value) {
        t->left = t2;
        t2->parent = t;
    } else if (t->value < t2->value){
        t->right = t2;
        t2->parent = t;
    } else {
        t->value = t2->value;
    }
}

void rotateR(tree_t *t)
{
    tree_t *left = t->left;
    if (left == NULL) {
        return;
    }
    tree_t *parent = t->parent;
    if (parent == NULL) {
        left->parent = parent;
    } else {
        parent->right = left;
        left->parent = parent;
    }
    tree_t *left_right = left->right;
    if (left_right != NULL) {
        t->left = left_right;
        left_right->parent = t;
    }
    left->right = t;
    t->parent = left;
}

void rotateL(tree_t *t)
{
    tree_t *right = t->right;
    if (right == NULL) {
        return;
    }
    tree_t *parent = t->parent;
    if (parent == NULL) {
        right->parent = parent;
    } else {
        parent->left = right;
        right->parent = parent;
    }
    tree_t *right_left = right->left;
    if (right_left != NULL) {
        t->right = right_left;
        right_left->parent = t;
    }
    right->left = t;
    t->parent = right;
}

void rotateRL(tree_t *t)
{
    rotateR(t->right);
    rotateL(t);
}

void rotateLR(tree_t *t)
{
    rotateL(t->left);
    rotateR(t);
}

void print_tree(tree_t *t, int depth)
{
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
        if (t->left->left == NULL && t->left->right == NULL) {
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
        } else {
            print_tree(t->left, (depth+1));
        }
    }
    if (t->right != NULL) {
        if (t->right->left == NULL && t->right->right == NULL) {
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
        } else {
            print_tree(t->right, (depth+1));
        }
    }
}

int tree_height(tree_t *t)
{
    int left = 0;
    int right = 0;

    if (t->left != NULL) {
        left += tree_height(t->left);
    }
    if (t->right != NULL) {
        right += tree_height(t->right);
    }

    return (left >= right) ? left : right;
}

int bias(tree_t *t)
{
    int left = tree_height(t->left);
    int right = tree_height(t->right);
    return left - right;
}

int main()
{
    tree_t root = { .value = 10 };
    tree_t left = { .value = 5 };
    tree_t r    = { .value = 50 };
    tree_t rl   = { .value = 30 };
    tree_t rll  = { .value = 20 };
    tree_t rlr  = { .value = 40 };
    tree_t rr   = { .value = 100 };
    insert(&root, &left);
    insert(&root, &r);
    insert(&r, &rl);
    insert(&r, &rr);
    insert(&rl, &rll);
    insert(&rl, &rlr);
    print_tree(&root, 0);
    printf("--------------\n");
    rotateRL(&root);
    print_tree(&rl, 0);
    return 0;
}
