#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <scconf.h>

#include "sysinfo.h"
#include "entry.h"

struct sysinfo_certificate
{
  char *name;
  void *data;
  size_t len;
};

typedef struct sysinfo_certificate sysinfo_certificate;

struct system_config
{
  system_config_entry *root_entry;
  scconf_context *ssc;
  sysinfo_certificate *certs;
};

static int trace_level = SYSINFO_ERROR;

static void
default_trace_logger(int level, const char *msg)
{
  if (trace_level >= level)
    puts(msg);
}

static void (*g_trace_loger)(int, const char *) = default_trace_logger;

void
sysinfo_set_trace_level(int level)
{
  trace_level = level;
}

void
sysinfo_trace(int level, const char *fmt, ...)
{
  char buf[1024];
  va_list ap;

  va_start(ap, fmt);

  if (g_trace_loger)
  {
    vsnprintf(buf, sizeof(buf), fmt, ap);
    buf[sizeof(buf) - 1] = 0;
    g_trace_loger(level, buf);
  }

  va_end(ap);
}

int
sysinfo_init(struct system_config **sc_out)
{
  struct system_config *sc = calloc(1, sizeof(struct system_config));
  scconf_context *ssc;
  struct system_config_entry *entry;

  sysinfo_trace(SYSINFO_INFO, "Initializing configuration");

  if (!sc)
    return -1;

  sc->ssc = scconf_new("/etc/sysinfod.conf");

  if (ssc)
  {
    if (scconf_parse(ssc) <= 0)
    {
      scconf_free(ssc);
      sc->ssc = NULL;
    }
    else
    {
      sc->ssc = ssc;
      entry = calloc(1, sizeof(struct system_config_entry));

      if (entry)
      {
        entry_append_child(&sc->root_entry, entry);
        add_compver_entries(sc);
        add_cert_entries(sc);
        add_devinfo_entries(sc);

        *sc_out = sc;

        return 0;
      }

      scconf_free(sc->ssc);
    }
  }

  free(sc);

  return -1;
}
