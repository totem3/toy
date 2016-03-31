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
    if (parent != NULL) {
        parent->right = left;
    }
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
    if (parent != NULL) {
        parent->left = right;
    }
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

void insert(tree_t *t, entry_t *entry)
{
    tree_t *t2 = malloc(sizeof(tree_t*));
    t2->entry = entry;

    insert_tree(t, t2);
}

void _insert_tree(tree_t *t, tree_t *t2, int *change, tree_t **v)
{
    if (t->entry == NULL || 0 == strcmp(t->entry->key, t2->entry->key)) {
        entry_t *e = malloc(sizeof(*e));
        e->key = t2->entry->key;
        e->value = t2->entry->value;
        t->entry = e;
    } else if (0 < strcmp(t->entry->key, t2->entry->key)) {
        if (t->left == NULL) {
            t->left = t2;
            t2->parent = t;
            int c = 1;
            memcpy(change, &c, sizeof(c));
            memcpy(v, &t, sizeof(*v));
        } else {
            _insert_tree(t->left, t2, change, v);
        }
    } else if (0 > strcmp(t->entry->key, t2->entry->key)) {
        if (t->right == NULL) {
            t->right = t2;
            t2->parent = t;
            int c = 1;
            memcpy(change, &c, sizeof(c));
            memcpy(v, &t, sizeof(*v));
        } else {
            _insert_tree(t->right, t2, change, v);
        }
    } else {
    }
}
void insert_tree(tree_t *t, tree_t *t2)
{
    tree_t *v = NULL;
    int c = 0;
    _insert_tree(t, t2, &c, &v);

    while (c) {
        int b = bias(t);
        int f = 0;
        if (b == 1 || b == -1) {
        } else if (b == 0) {
            memcpy(&c, &f, sizeof(c));
        } else if (b == 2) {
            switch (bias(t->left)) {
                case 1:
                    rotateR(t);
                    memcpy(&c, &f, sizeof(c));
                    break;
                case -1:
                    rotateLR(t);
                    memcpy(&c, &f, sizeof(c));
                    break;
            }
        } else if (b == -2) {
            switch (bias(t->right)) {
                case -1:
                    rotateL(t);
                    memcpy(&c, &f, sizeof(c));
                    break;
                case 1:
                    rotateRL(t);
                    memcpy(&c, &f, sizeof(c));
                    break;
            }
        }
        if (t == NULL || t->parent == NULL) {
            break;
        }
        t = t->parent;
    }

}

void _print_tree(tree_t *t, int depth)
{
    char buffer[depth*2+3+10];
    int i;
    for (i = 0; i < depth; i++) {
        char *buf = &buffer[i*2];
        if (i == (depth)) {
            memcpy(buf, "+-", 2);
        } else {
            memcpy(buf, "| ", 2);
        }
    }
    char *buf = &buffer[i*2];
    memcpy(buf, "+ ", 2);
    buf = &buffer[i*2+2];
    char valbuf[10];
    sprintf(valbuf, "%s:%d", t->entry->key, t->entry->value);
    memcpy(buf, valbuf, 10);
    printf("%s\n", buffer);
    if (t->left != NULL) {
        /* if (t->left->left == NULL && t->left->right == NULL) { */
            /* char lbuffer[depth*2+3+10]; */
            /* int j; */
            /* for (j = 0; j < depth; j++) { */
            /*     char *lbuf = &lbuffer[j*2]; */
            /*     if (j == (depth)) { */
            /*         memcpy(lbuf, "+-", 2); */
            /*     } else { */
            /*         memcpy(lbuf, "| ", 2); */
            /*     } */
            /* } */
            /* char *lbuf = &lbuffer[j*2]; */
            /* memcpy(lbuf, "+-", 2); */
            /* lbuf = &lbuffer[j*2+2]; */
            /* char lvalbuf[10]; */
            /* sprintf(lvalbuf, "%s:%d", t->left->entry.key, t->left->entry.value); */
            /* memcpy(lbuf, lvalbuf, 10); */
            /* printf("%s\n", lbuffer); */
        /* } else { */
            _print_tree(t->left, (depth+1));
        /* } */
    }
    if (t->right != NULL) {
        /* if (t->right->left == NULL && t->right->right == NULL) { */
            /* char rbuffer[depth*2+3+10]; */
            /* int j; */
            /* for (j = 0; j < depth; j++) { */
            /*     char *rbuf = &rbuffer[j*2]; */
            /*     if (j == (depth)) { */
            /*         memcpy(rbuf, "+-", 2); */
            /*     } else { */
            /*         memcpy(rbuf, "| ", 2); */
            /*     } */
            /* } */
            /* char *rbuf = &rbuffer[j*2]; */
            /* memcpy(rbuf, "+-", 2); */
            /* rbuf = &rbuffer[j*2+2]; */
            /* char lvarbuf[10]; */
            /* sprintf(lvarbuf, "%s:%d", t->right->entry.key, t->right->entry.value); */
            /* memcpy(rbuf, lvarbuf, 10); */
            /* printf("%s\n", rbuffer); */
        /* } else { */
            _print_tree(t->right, (depth+1));
        /* } */
    }
}

void print_tree(tree_t *t)
{
    _print_tree(t, 0);
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

entry_t* find(tree_t *t, const char *key)
{
    if (t == NULL || t->entry == NULL) {
        return NULL;
    }
    int a = strcmp(t->entry->key, key);
    if (0 < a) {
        if (t->left != NULL) {
            return find(t->left, key);
        } else {
            return NULL;
        }
    } else if (0 > a) {
        if (t->right != NULL) {
            return find(t->right, key);
        } else {
            return NULL;
        }
    } else if (0 == a) {
        return t->entry;
    }
    return NULL;
}

entry_t* get(dict_t *d, const char *key)
{
    return find(d->tree, key);
}

int set(dict_t *d, const char *key, int value)
{
    entry_t *e = malloc(sizeof(*e));
    e->key = key;
    e->value = value;
    insert(d->tree, e);
    return value;
}

tree_t new_tree(const char *key, int value)
{
    entry_t *e = malloc(sizeof(*e));
    tree_t *t = malloc(sizeof(*t));
    e->key = key;
    e->value = value;
    t->entry = e;
    return *t;
}

dict_t new_dict()
{
    dict_t *d = malloc(sizeof(*d));
    tree_t *t = malloc(sizeof(*t));
    d->tree = t;
    return *d;
}
