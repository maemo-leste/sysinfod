#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <cal.h>
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

static int
add_devinfo_entries(struct system_config *sc)
{
  int res;
  system_config_entry *device_entry;
  system_config_entry *entry;
  struct cal_phone_info phone_info;
  unsigned long len;
  void *buf;
  struct cal *cal_out;

  res = cal_init(&cal_out);

  if (res < 0)
  {
    sysinfo_error("%s: cal init failed (%d)", "read_devinfo", res);
    return res;
  }

  res = cal_read_block(cal_out, "phone-info", &buf, &len, 0);

  if (res < 0)
  {
    sysinfo_error("Phone info (block %s) read failed (%d)", "phone-info", res);
    cal_finish(cal_out);
    return res;
  }

  if (len != sizeof(struct cal_phone_info))
  {
    sysinfo_error("Phone info size mismatch (expected %zu, actual %lu)",
                  sizeof(struct cal_phone_info), len);
  }

  if (len > sizeof(struct cal_phone_info))
    len = sizeof(struct cal_phone_info);

  memcpy(&phone_info, buf, len);
  free(buf);

  res = cal_read_block(cal_out, "sw-release-ver", &buf, &len, CAL_FLAG_USER);

  if (res < 0)
  {
    sysinfo_error("Software release version (block %s) read failed (%d)",
                  "sw-release-ver", res);
    cal_finish(cal_out);
    return res;
  }

  cal_finish(cal_out);

  device_entry = alloc_config_entry("device");

  if (!device_entry)
    return -1;

  add_config_entry(sc->root_entry, device_entry);

  entry = alloc_strn_entry("sw-release-ver", buf, len);
  free(buf);

  if (!entry)
    return -1;

  add_config_entry(device_entry, entry);
  entry = alloc_strn_entry("hw-version", phone_info.hw_version,
                           sizeof(phone_info.hw_version));

  if (!entry)
    return -1;

  add_config_entry(device_entry, entry);
  entry = alloc_strn_entry("production-sn", phone_info.production_sn,
                           sizeof(phone_info.production_sn));

  if ( !entry )
    return -1;

  add_config_entry(device_entry, entry);
  entry = alloc_strn_entry("product-code", phone_info.product_code,
                           sizeof(phone_info.product_code));

  if (!entry)
    return -1;

  add_config_entry(device_entry, entry);
  entry = alloc_strn_entry("order-number", phone_info.order_number,
                           sizeof(phone_info.order_number));

  if (!entry)
    return -1;

  add_config_entry(device_entry, entry);
  entry = alloc_strn_entry("basic-product-code", phone_info.basic_product_code,
                           sizeof(phone_info.basic_product_code));

  if (!entry)
    return -1;

  add_config_entry(device_entry, entry);

  return 0;
}

static int
add_compver_entries(struct system_config *sc)
{
  system_config_entry *component_entry;
  char comp_ver_buf[1024];
  int res = 0;
  FILE *fp = fopen("/proc/component_version", "r");

  if (!fp)
    return -errno;

  component_entry = alloc_config_entry("component");

  if (!component_entry)
  {
    res = -ENOMEM;
    goto out;
  }

  add_config_entry(sc->root_entry, component_entry);

  while (fgets(comp_ver_buf, sizeof(comp_ver_buf), fp))
  {
    system_config_entry *entry;
    char *name;
    char *p;
    char *val;

    if (strlen(comp_ver_buf) == 1023) /* WTF? */
    {
      res = -EINVAL;
      remove_config_entry(component_entry);
      goto out;
    }

    name = strtok(comp_ver_buf, " \t\n\r");

    if (!name)
      continue;

    p = &name[strlen(name) + 1];

    while (*p && isspace(*p))
      p++;

    val = strtok(p, "\n\r");

    if (val)
      entry = alloc_str_entry(name, val);
    else
      entry = alloc_str_entry(name, "");

    if (!entry)
    {
      res = -ENOMEM;
      remove_config_entry(component_entry);
      goto out;
    }

    add_config_entry(component_entry, entry);
  }

out:
  fclose(fp);

  return res;
}

int
sysinfo_init(struct system_config **sc_out)
{
  struct system_config *sc = calloc(1, sizeof(struct system_config));
  struct system_config_entry *entry;

  sysinfo_trace(SYSINFO_INFO, "Initializing configuration");

  if (!sc)
    return -1;

  sc->ssc = scconf_new("/etc/sysinfod.conf");

  if (sc->ssc)
  {
    if (scconf_parse(sc->ssc) <= 0)
    {
      scconf_free(sc->ssc);
      sc->ssc = NULL;
    }
    else
    {
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
