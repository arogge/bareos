/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2020-2020 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#include <mutex>

#include "osinfo.h"

#include <CoreFoundation/CoreFoundation.h>
extern CFDictionaryRef _CFCopyServerVersionDictionary(void);
extern CFDictionaryRef _CFCopySystemVersionDictionary(void);
extern const CFStringRef _kCFSystemVersionProductNameKey;
extern const CFStringRef _kCFSystemVersionProductVersionKey;
extern const CFStringRef _kCFSystemVersionBuildVersionKey;

static char macos_version[256];
static bool macos_version_initialized = false;
static std::mutex init_mutex;

static bool GetMacOsVersionString(char*, size_t);

const char* GetOsInfoString()
{
  if (!macos_version_initialized) {
    const std::lock_guard<std::mutex> lock(init_mutex);
    if (!macos_version_initialized) {
      if (!GetMacOsVersionString(macos_version, sizeof(macos_version))) {
        return "MacOS unknown";
      }
      macos_version_initialized = true;
    }
  }
  return macos_version;
}

static bool GetMacOsVersionString(char* dst, size_t len)
{
  CFDictionaryRef dict = NULL;
  CFStringRef str = NULL;
  dict = _CFCopyServerVersionDictionary();
  if (dict == NULL) dict = _CFCopySystemVersionDictionary();
  if (dict == NULL) return false;

  str = CFStringCreateWithFormat(
      NULL, NULL, CFSTR("%@ Version %@ (Build %@)"),
      CFDictionaryGetValue(dict, _kCFSystemVersionProductNameKey),
      CFDictionaryGetValue(dict, _kCFSystemVersionProductVersionKey),
      CFDictionaryGetValue(dict, _kCFSystemVersionBuildVersionKey));
  if (!CFStringGetCString(str, dst, len, CFStringGetSystemEncoding())) {
    return false;
  }
  CFRelease(str);
  CFRelease(dict);
  return true;
}
