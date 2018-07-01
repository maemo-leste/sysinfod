sbin_PROGRAMS = sysinfod

lib_LTLIBRARIES = libsysinfo.la

libsysinfo_la_CFLAGS = $(SYSINFO_CFLAGS) -I../include -Wall -Werror
libsysinfo_la_LDFLAGS = $(SYSINFO_LIBS) -lscconf -avoid-version -Wl, no-undefined
libsysinfo_la_SOURCES = entry.c sysinfo.c

sysinfod_CFLAGS = $(SYSINFOD_CFLAGS) -I../include -Wall -Werror
sysinfod_LDFLAGS = $(SYSINFOD_LIBS)

BUILT_SOURCES = dbus-glib-marshal-si.h

dbus-glib-marshal-si.h: sysinfod.xml
	$(DBUS_BINDING_TOOL) --prefix=si --mode=glib-server $< > xgen-$(@F)		\
	&& ( cmp -s xgen-$(@F) $@ || cp xgen-$(@F) $@ )					\
	&& rm -f xgen-$(@F)

sysinfod_SOURCES = sysinfod.c

CLEANFILES = $(BUILT_SOURCES)

MAINTAINERCLEANFILES = Makefile.in
