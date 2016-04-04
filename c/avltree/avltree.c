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
    left->parent = parent;
    if (parent != NULL) {
        if (parent->left == t) {
            parent->left = left;
        } else {
            parent->right = left;
        }
    }
    t->parent = left;
    t->left = lr;
    if (lr != NULL) {
        lr->parent = t;
    }
    set_height(t);
    set_height(left);
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
    right->parent = parent;
    if (parent != NULL) {
        if (parent->left == t) {
            parent->left = right;
        } else {
            parent->right = right;
        }
    }
    t->parent = right;
    t->right = rl;
    if (rl != NULL) {
        rl->parent = t;
    }
    set_height(t);
    set_height(right);
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

void set_height(tree_t *t)
{
    if (t == NULL) return;

    int l = (t->left == NULL) ? 0 : t->left->height;
    int r = (t->right == NULL) ? 0 : t->right->height;
    t->height = 1 + ((l >= r) ? l : r);
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
    int l = (t->left == NULL) ? 0 : t->left->height;
    int r = (t->right == NULL) ? 0 : t->right->height;
    return l - r;
}

void insert(tree_t *t, entry_t *entry)
{
    tree_t *t2 = (tree_t*)malloc(sizeof(tree_t*));
    memset(t2, 0, sizeof(tree_t));
    t2->entry = entry;

    insert_tree(t, t2);
}

void _insert_tree(tree_t *t, tree_t *t2, int *change, tree_t **v)
{
    if (t->entry == NULL || 0 == strcmp(t->entry->key, t2->entry->key)) {
        t->entry = t2->entry;
    } else if (0 < strcmp(t->entry->key, t2->entry->key)) {
        if (t->left == NULL) {
            t->left = t2;
            t2->parent = t;
            memset(change, 0, sizeof(int));
            memset(change, 1, 1);
            memcpy(v, &t, sizeof(tree_t*));
        } else {
            _insert_tree(t->left, t2, change, v);
        }
    } else if (0 > strcmp(t->entry->key, t2->entry->key)) {
        if (t->right == NULL) {
            t->right = t2;
            t2->parent = t;
            memset(change, 0, sizeof(int));
            memset(change, 1, 1);
            memcpy(v, &t, sizeof(tree_t*));
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
        int b = bias(v);
        set_height(v);
        if (b == 1 || b == -1) {
        } else if (b == 0) {
            /* memset(&c, 0, sizeof(int)); */
        } else if (b == 2) {
            switch (bias(v->left)) {
                case 1:
                    rotateR(v);
                    /* memset(&c, 0, sizeof(int)); */
                    break;
                case -1:
                    rotateLR(v);
                    /* memset(&c, 0, sizeof(int)); */
                    break;
            }
        } else if (b == -2) {
            switch (bias(v->right)) {
                case -1:
                    rotateL(v);
                    /* memset(&c, 0, sizeof(int)); */
                    break;
                case 1:
                    rotateRL(v);
                    /* memset(&c, 0, sizeof(int)); */
                    break;
            }
        }
        if (v == NULL || v->parent == NULL) {
            break;
        }
        v = v->parent;
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
    entry_t *e = (entry_t*)malloc(sizeof(entry_t));
    tree_t *t = malloc(sizeof(tree_t));
    memset(e, 0, sizeof(entry_t));
    memset(t, 0, sizeof(tree_t));
    e->key = key;
    e->value = value;
    t->entry = e;
    t->height = 1;
    insert_tree(d->tree, t);
    while(d->tree->parent != NULL) {
        d->tree = d->tree->parent;
    }
    return value;
}

tree_t *new_tree(const char *key, int value)
{
    entry_t *e = (entry_t*)malloc(sizeof(entry_t));
    memset(e, 0, sizeof(entry_t));
    tree_t *t = (tree_t*)malloc(sizeof(tree_t));
    memset(t, 0, sizeof(tree_t));
    e->key = key;
    e->value = value;
    t->entry = e;
    t->height = 1;
    return t;
}

dict_t *new_dict()
{
    dict_t *d = (dict_t *)malloc(sizeof(dict_t));
    tree_t *t = (tree_t *)malloc(sizeof(tree_t));
    memset(d, 0, sizeof(dict_t));
    memset(t, 0, sizeof(tree_t));
    d->tree = t;
    t->height = 1;
    return d;
}
