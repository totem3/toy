#include "iniparse.c"

int main(void) {
    init_config();
    init_section();
    if (!yyparse()) {
        return 1;
    }
    ini_entry_t *entry = list_entry(config._list.next, ini_entry_t, _list);
    printf("%s: %s=%s\n", entry->section, entry->key, entry->value);
    entry = list_entry(entry->_list.next, ini_entry_t, _list);
    printf("%s: %s=%s\n", entry->section, entry->key, entry->value);
    return 0;
}
