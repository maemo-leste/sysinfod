#include <assert.h>
#include <string.h>
#include <errno.h>

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

void
free_config_entry(system_config_entry *entry)
{
  if (entry->free)
    entry->free(entry);

  if ( entry->name )
    free(entry->name);

  free(entry);
}

void
add_config_entry(system_config_entry *parent, system_config_entry *entry)
{

  assert(parent != NULL);

  entry->parent = parent;
  entry_append_child(&parent->children, entry);
}

static int
simple_entry_get_value(system_config_entry *entry, void **val, size_t *len)
{
  size_t l = strlen((const char *)entry->data);

  sysinfo_trace(SYSINFO_TRACE, "%s: Read string (%s)", entry->name,
                (char *)entry->data);
  *val = malloc(l);

  if (!*val)
    return -ENOMEM;

  memcpy(*val, entry->data, l);

  *len = l;

  return 0;
}

static void
simple_entry_free(system_config_entry *entry)
{
  free(entry->data);
}

system_config_entry *
alloc_strn_entry(const char *name, const char *value, size_t len)
{
  system_config_entry *entry;

  assert(name != NULL);
  assert(value != NULL);

  entry = alloc_config_entry(name);

  if (!entry)
    return NULL;

  entry->data = strndup(value, len);

  if (entry->data)
  {
    entry->get_value = simple_entry_get_value;
    entry->free = simple_entry_free;
  }
  else
  {
    free_config_entry(entry);
    entry = NULL;
  }

  return entry;
}
