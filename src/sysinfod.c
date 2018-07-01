#include <dbus/dbus-glib-bindings.h>

struct _SystemInfo
{
  GObject parent_instance;
  struct system_config *si;
};

typedef struct _SystemInfo SystemInfo;

struct _SystemInfoClass
{
  GObjectClass parent_class;
};

typedef struct _SystemInfoClass SystemInfoClass;

G_DEFINE_TYPE(SystemInfo, system_info, G_TYPE_OBJECT);

static gboolean si_get_config_value(SystemInfo *self, const char *key, GArray **val, GError **error);
static gboolean si_get_config_keys(SystemInfo *self, char ***keys, GError **error);

#include "dbus-glib-marshal-si.h"

static void
system_info_class_init(SystemInfoClass *klass)
{
}

static void
system_info_init(SystemInfo *self)
{
}

int main(int argc, const char **argv)
{

}
