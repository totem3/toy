#include "./avltree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void rotateR(tree_t *t)
{
    tree_t *parent = t->parent;
    tree_t *left = t->left;
    tree_t *right = t->right;
    tree_t *lr = t->left->right;

    if (left == NULL) {
        return;
    }

    left->right = t;
    t->parent = left;
    left->parent = parent;
    t->left = lr;
    if (lr != NULL) {
        lr->parent = t;
    }
}

void rotateL(tree_t *t)
{
    tree_t *parent = t->parent;
    tree_t *left = t->left;
    tree_t *right = t->right;
    tree_t *rl = t->right->left;

    if (right == NULL) {
        return;
    }

    right->left = t;
    t->parent = right;
    right->parent = parent;
    t->right = rl;
    if (rl != NULL) {
        rl->parent = t;
    }
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

int tree_height(tree_t *t)
{
    int left = 1;
    int right = 1;

    if (t == NULL) {
        return 0;
    }

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

void insert(tree_t *t, entry_t entry)
{
    tree_t *t2 = malloc(sizeof(tree_t*));
    t2->entry = entry;

    insert_tree(t, t2);
}

void insert_tree(tree_t *t, tree_t *t2)
{
    if (0 < strcmp(t->entry.key, t2->entry.key)) {
        if (t->left == NULL) {
            t->left = t2;
            t2->parent = t;
        } else {
            insert_tree(t->left, t2);
            int b = bias(t);
            if (b < -1) {
                rotateL(t);
            }
            if (b > 1) {
                rotateR(t);
            }
        }
    } else if (0 > strcmp(t->entry.key, t2->entry.key)) {
        if (t->right == NULL) {
            t->right = t2;
            t2->parent = t;
        } else {
            insert_tree(t->right, t2);
            int b = bias(t);
            if (b < -1) {
                rotateL(t);
            }
            if (b > 1) {
                rotateR(t);
            }
        }
    } else {
        t->entry.value = t2->entry.value;
    }
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

void traverse_tree(tree_t *t, void (*f)(tree_t *t))
{
    if (t->left != NULL) {
        traverse_tree(t->left, f);
    }
    if (t->right != NULL) {
        traverse_tree(t->right, f);
    }
    f(t);
}
