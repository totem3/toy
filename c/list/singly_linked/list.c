#include <stdio.h>
#include <stdlib.h>
#include "./list.h"

int insert(List *list, Node *node)
{
    Node *n = list->head;
    list->head = node;
    node->next = n;

    return 0;
}

Node *get(List *list, int index)
{
    int i;
    Node *n = list->head;
    for (i = 0; i < index; i++) {
        if (n == NULL) {
            return NULL;
        }
        n = n->next;
    }
    return n;
}

int remove_node(List *list, Node *node)
{
    Node *n = list->head;
    while(n != NULL && n->next != NULL) {
        if (n->next == node) {
            n->next = node->next;
        }
        n = n->next;
    }
    return -1;
}

Node *new_node(int value)
{
    Node *node = calloc(sizeof(Node), 1);
    node->value = value;
    return node;
}

List *new_list()
{
    List *list = calloc(sizeof(List), 1);
    return list;
}
