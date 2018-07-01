/**
  @file sysinfo_dbus_names.h

  Copyright (C) 2013 Jonathan Wilson

  @author Jonathan wilson <jfwfreo@tpgi.com.au>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License version 2.1 as
  published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this library; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#ifndef SYSINFO_DBUS_NAMES_H_
#define SYSINFO_DBUS_NAMES_H_

#define SYSINFOD_SERVICE                "com.nokia.SystemInfo"
#define SYSINFOD_INTERFACE              "com.nokia.SystemInfo"
#define SYSINFOD_PATH                   "/com/nokia/SystemInfo"
#define SYSINFOD_GET_CONFIG_KEYS        "GetConfigKeys" //returns array of string
#define SYSINFOD_GET_CONFIG_VALUE       "GetConfigValue" //takes string (key to use), returns array of byte

#endif /* SYSINFO_DBUS_NAMES_H_ */
