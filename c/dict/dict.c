#include <stdio.h>
#include <string.h>
#include "./dict.h"
#include <unistd.h>

typedef struct tree {
  struct tree *left;
  struct tree *right;
  dict_entry_t entry;
} tree_t;

int count_node(tree_t *t)
{
    int n = 0;
    if (t->left != NULL) n++;
    if (t->right != NULL) n++;
    return n;
}

dict_entry_t* find(tree_t *t, const char *key)
{
    if (strcmp(t->entry.key, key) == 0) {
        return &t->entry;
    }
    if (t->left != NULL && 0 < strcmp(key, t->entry.key)) {
        return find(t->left, key);
    }
    if (t->right != NULL && 0 > strcmp(key, t->entry.key)) {
        return find(t->right, key);
    }
    return NULL;
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
    if (0 <= strcmp(node->entry.key, t->entry.key)) {
        if (t->left == NULL) {
            t->left = node;
        } else {
            insert_node(t->left, node);
        }
    } else if (0 > strcmp(node->entry.key, t->entry.key)) {
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
    sprintf(valbuf, "%d", t->entry.value);
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
            sprintf(lvalbuf, "%d", t->left->entry.value);
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
            sprintf(lvarbuf, "%d", t->right->entry.value);
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

int main_old()
{
    dict_entry_t entry;
    entry.key = "test";
    entry.value = 77;
    printf("key: %s, value: %d\n", entry.key, entry.value);

    printf("create first tree\n");
    tree_t t = {
        .left = NULL,
        .right = NULL,
        .entry = entry
    };
    printf("created first tree\n");
    printf("t.value: %d\n", t.entry.value);
    printf("create second tree\n");
    dict_entry_t entry2 = {
        .key = "ababa",
        .value = 200
    };
    tree_t t2;
    t2.left = NULL;
    t2.right = NULL;
    t2.entry = entry2;
    printf("t2->value: %d\n", t2.entry.value);
    dict_entry_t entry3 = {
        .key = "hoge",
        .value = 50
    };
    tree_t t3 = {
        .left = NULL,
        .right = NULL,
        .entry = entry3
    };
    dict_entry_t entry4 = {
        .key = "fuga",
        .value = 300
    };
    tree_t t4 = {
        .left = NULL,
        .right = NULL,
        .entry = entry4
    };
    printf("t3->value: %d\n", t3.entry.value);
    insert_node(&t, &t2);
    insert_node(&t, &t3);
    insert_node(&t, &t4);
    printf("t2 height: %d\n", tree_height(&t2));
    printf("t1 height: %d\n", tree_height(&t));
    print_tree(&t, 0);

    dict_entry_t *ent = find(&t, "fuga");
    if (ent != NULL) {
        printf("%d\n", ent->value);
    }
    return 0;
}

void insert(tree_t *dict, const char *key, int value, tree_t *a)
{
    dict_entry_t e = {
        .key = key,
        .value = value
    };
    if (strcmp(dict->entry.key, "") == 0) {
        dict->entry = e;
    } else {
        a->left = NULL;
        a->right = NULL;
        a->entry = e;
        insert_node(dict, a);
    }
}

dict_entry_t* get(tree_t *dict, const char *key)
{
    return find(dict, key);
}

int main()
{
    dict_entry_t e = {
        .key = "",
        .value = 0
    };
    tree_t dict = {
        .left = NULL,
        .right = NULL,
        .entry = e
    };

    int MAX = 200000;
    tree_t t[MAX];
    printf("sizeof tree_t %lu\n", sizeof(tree_t));
    char keybuf[MAX][5];
    int i;
    for (i = 0; i < MAX; i++) {
        /* fprintf(stderr,"%d\n", i); */
        sprintf(keybuf[i], "key%d", i);
        insert(&dict, keybuf[i], i, &t[i]);
    }
    sleep(10);

    printf("31 = %d\n", get(&dict, "key1")->value);
    dict_entry_t *result;
    result = get(&dict, "fuga");
    if (result != NULL) {
        printf("fuga: %d\n", result->value);
    } else {
        printf("value for key 'fuga' is NULL\n");
    }

    /* print_tree(&dict, 0); */
}
