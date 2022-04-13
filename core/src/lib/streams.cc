/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2022-2022 Bareos GmbH & Co. KG

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
#include <stdio.h>
#include "streams.h"

#define _(x) (x)

bool is_win32_stream(int stream)
{
  switch (stream) {
    case STREAM_WIN32_DATA:
    case STREAM_WIN32_GZIP_DATA:
    case STREAM_WIN32_COMPRESSED_DATA:
    case STREAM_ENCRYPTED_WIN32_DATA:
    case STREAM_ENCRYPTED_WIN32_GZIP_DATA:
    case STREAM_ENCRYPTED_WIN32_COMPRESSED_DATA:
      return true;
  }
  return false;
}

const char* stream_to_ascii(int stream)
{
  static char buf[20];

  switch (stream & STREAMMASK_TYPE) {
    case STREAM_UNIX_ATTRIBUTES:
      return _("Unix attributes");
    case STREAM_FILE_DATA:
      return _("File data");
    case STREAM_MD5_DIGEST:
      return _("MD5 digest");
    case STREAM_GZIP_DATA:
      return _("GZIP data");
    case STREAM_COMPRESSED_DATA:
      return _("Compressed data");
    case STREAM_UNIX_ATTRIBUTES_EX:
      return _("Extended attributes");
    case STREAM_SPARSE_DATA:
      return _("Sparse data");
    case STREAM_SPARSE_GZIP_DATA:
      return _("GZIP sparse data");
    case STREAM_SPARSE_COMPRESSED_DATA:
      return _("Compressed sparse data");
    case STREAM_PROGRAM_NAMES:
      return _("Program names");
    case STREAM_PROGRAM_DATA:
      return _("Program data");
    case STREAM_SHA1_DIGEST:
      return _("SHA1 digest");
    case STREAM_WIN32_DATA:
      return _("Win32 data");
    case STREAM_WIN32_GZIP_DATA:
      return _("Win32 GZIP data");
    case STREAM_WIN32_COMPRESSED_DATA:
      return _("Win32 compressed data");
    case STREAM_MACOS_FORK_DATA:
      return _("MacOS Fork data");
    case STREAM_HFSPLUS_ATTRIBUTES:
      return _("HFS+ attribs");
    case STREAM_UNIX_ACCESS_ACL:
      return _("Standard Unix ACL attribs");
    case STREAM_UNIX_DEFAULT_ACL:
      return _("Default Unix ACL attribs");
    case STREAM_SHA256_DIGEST:
      return _("SHA256 digest");
    case STREAM_SHA512_DIGEST:
      return _("SHA512 digest");
    case STREAM_SIGNED_DIGEST:
      return _("Signed digest");
    case STREAM_ENCRYPTED_FILE_DATA:
      return _("Encrypted File data");
    case STREAM_ENCRYPTED_WIN32_DATA:
      return _("Encrypted Win32 data");
    case STREAM_ENCRYPTED_SESSION_DATA:
      return _("Encrypted session data");
    case STREAM_ENCRYPTED_FILE_GZIP_DATA:
      return _("Encrypted GZIP data");
    case STREAM_ENCRYPTED_FILE_COMPRESSED_DATA:
      return _("Encrypted compressed data");
    case STREAM_ENCRYPTED_WIN32_GZIP_DATA:
      return _("Encrypted Win32 GZIP data");
    case STREAM_ENCRYPTED_WIN32_COMPRESSED_DATA:
      return _("Encrypted Win32 Compressed data");
    case STREAM_ENCRYPTED_MACOS_FORK_DATA:
      return _("Encrypted MacOS fork data");
    case STREAM_ACL_AIX_TEXT:
      return _("AIX Specific ACL attribs");
    case STREAM_ACL_DARWIN_ACCESS_ACL:
      return _("Darwin Specific ACL attribs");
    case STREAM_ACL_FREEBSD_DEFAULT_ACL:
      return _("FreeBSD Specific Default ACL attribs");
    case STREAM_ACL_FREEBSD_ACCESS_ACL:
      return _("FreeBSD Specific Access ACL attribs");
    case STREAM_ACL_HPUX_ACL_ENTRY:
      return _("HPUX Specific ACL attribs");
    case STREAM_ACL_IRIX_DEFAULT_ACL:
      return _("Irix Specific Default ACL attribs");
    case STREAM_ACL_IRIX_ACCESS_ACL:
      return _("Irix Specific Access ACL attribs");
    case STREAM_ACL_LINUX_DEFAULT_ACL:
      return _("Linux Specific Default ACL attribs");
    case STREAM_ACL_LINUX_ACCESS_ACL:
      return _("Linux Specific Access ACL attribs");
    case STREAM_ACL_TRU64_DEFAULT_ACL:
      return _("TRU64 Specific Default ACL attribs");
    case STREAM_ACL_TRU64_ACCESS_ACL:
      return _("TRU64 Specific Access ACL attribs");
    case STREAM_ACL_SOLARIS_ACLENT:
      return _("Solaris Specific POSIX ACL attribs");
    case STREAM_ACL_SOLARIS_ACE:
      return _("Solaris Specific NFSv4/ZFS ACL attribs");
    case STREAM_ACL_AFS_TEXT:
      return _("AFS Specific ACL attribs");
    case STREAM_ACL_AIX_AIXC:
      return _("AIX Specific POSIX ACL attribs");
    case STREAM_ACL_AIX_NFS4:
      return _("AIX Specific NFSv4 ACL attribs");
    case STREAM_ACL_FREEBSD_NFS4_ACL:
      return _("FreeBSD Specific NFSv4/ZFS ACL attribs");
    case STREAM_ACL_HURD_DEFAULT_ACL:
      return _("GNU Hurd Specific Default ACL attribs");
    case STREAM_ACL_HURD_ACCESS_ACL:
      return _("GNU Hurd Specific Access ACL attribs");
    case STREAM_XATTR_HURD:
      return _("GNU Hurd Specific Extended attribs");
    case STREAM_XATTR_IRIX:
      return _("IRIX Specific Extended attribs");
    case STREAM_XATTR_TRU64:
      return _("TRU64 Specific Extended attribs");
    case STREAM_XATTR_AIX:
      return _("AIX Specific Extended attribs");
    case STREAM_XATTR_OPENBSD:
      return _("OpenBSD Specific Extended attribs");
    case STREAM_XATTR_SOLARIS_SYS:
      return _(
          "Solaris Specific Extensible attribs or System Extended attribs");
    case STREAM_XATTR_SOLARIS:
      return _("Solaris Specific Extended attribs");
    case STREAM_XATTR_DARWIN:
      return _("Darwin Specific Extended attribs");
    case STREAM_XATTR_FREEBSD:
      return _("FreeBSD Specific Extended attribs");
    case STREAM_XATTR_LINUX:
      return _("Linux Specific Extended attribs");
    case STREAM_XATTR_NETBSD:
      return _("NetBSD Specific Extended attribs");
    default:
      sprintf(buf, "%d", stream);
      return (const char*)buf;
  }
}
