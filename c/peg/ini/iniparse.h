#include "list.h"
typedef struct ini_entry {
    char* section;
    const char* key;
    const char* value;
    struct list_head _list;
} ini_entry_t;

ini_entry_t config;
static char* c_section;
static void init_section(void)
{
    c_section = (char*)malloc(128);
    strcpy(c_section, "global");
}
static int init_config(void)
{
    INIT_LIST_HEAD(&config._list);
    return 0;
}
