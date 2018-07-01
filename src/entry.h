#ifndef ENTRY_H
#define ENTRY_H

#include <stdlib.h>

typedef struct system_config_entry system_config_entry;

struct system_config_entry
{
  char *name;
  void *data;
  int (*get_value)(system_config_entry *entry, void **val, size_t *len);
  void (*free)(struct system_config_entry *);
  system_config_entry *parent;
  system_config_entry *children;
  system_config_entry *next;
};

system_config_entry *alloc_config_entry(const char *name);
void entry_append_child(system_config_entry **entry, system_config_entry *child);
void add_config_entry(system_config_entry *parent, system_config_entry *entry);

#endif // ENTRY_H
