typedef struct _node {
    int value;
    struct _node *next;
} Node;

typedef struct _list {
    Node *head;
} List;

int insert(List *list, Node *node);
int remove_node(List *list, Node *node);
Node *get(List *list, int index);
Node *new_node(int value);
List *new_list();
