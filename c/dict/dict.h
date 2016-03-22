typedef struct {
    const char *key;
    int value;
} dict_entry_t;

typedef struct {
    dict_entry_t *entries;
} dict_t;

void insert(dict_t dict, const char *key, int value);
int get(dict_t dict, const char *key);
