#include <assert.h>
#include <string.h>

#include "sysinfo.h"
#include "entry.h"

static char *
entry_get_key(system_config_entry *entry)
{
  int len = 0;
  system_config_entry *e;
  char *key;
  char *p;

  if (!entry->name)
    return strdup("<root>");

  for (e = entry; e && e->name; e = e->parent)
    len += strlen(e->name) + 1;

  if (!(key = malloc(len + 1)))
    return NULL;

  key[len] = 0;
  p = &key[len];

  for (e = entry; e && e->name; e = e->parent)
  {
    int l = strlen(e->name);

    p -= l;
    memcpy(p, e->name, l);
    *p-- = '/';
  }

  return key;
}

void
entry_append_child(system_config_entry **entry, system_config_entry *child)
{
  char *key;
  system_config_entry *i;

  key = entry_get_key(child);

  if (key)
  {
    sysinfo_trace(SYSINFO_INFO, "Adding config key '%s'", key);
    free(key);
  }

  for (i = *entry; i; i = i->next )
    entry = &i->next;

  *entry = child;
}

system_config_entry *
alloc_config_entry(const char *name)
{
  system_config_entry *entry;

  assert(name != NULL);
  assert(strlen(name) < 64);

  entry = calloc(1, sizeof(system_config_entry));

  if (!entry)
    return NULL;

  entry->name = strdup(name);

  return entry;
}
