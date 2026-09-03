#!/bin/bash
#   BAREOS® - Backup Archiving REcovery Open Sourced
#
#   Copyright (C) 2024-2024 Bareos GmbH & Co. KG
#
#   This program is Free Software; you can redistribute it and/or
#   modify it under the terms of version three of the GNU Affero General Public
#   License as published by the Free Software Foundation and included
#   in the file LICENSE.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
#   02110-1301, USA.

wrapped_cmd="$(dirname "$0")/localfile-wrapper.sh"

set -Eeuo pipefail

get_filesize() {
  if [ "$(uname)" = "FreeBSD" ]; then
    stat -f %z "$1"
  else
    stat "--format=%s" "$1"
  fi
}
echo "$@" >>"${tmp:-/tmp}/worm.txt"

case "$1" in
  options)
    cat << '_EOT_'
worm_staging_path
_EOT_
    "${wrapped_cmd}" options
    ;;
  testconnection)
    [ -d "${worm_staging_path}" ]
    exec "${wrapped_cmd}" "$@"
    ;;
  list)
    if [ -e "${worm_staging_path}/$2" ]; then
      printf "0000 %d\n" "$(get_filesize "${worm_staging_path}/$2")"
    else
      exec "${wrapped_cmd}" "$@"
    fi
    ;;
  stat)
    if [ $3 = "0000" ] && [ -e "${worm_staging_path}/$2" ]; then
      get_filesize "${worm_staging_path}/$2"
    else
      exec "${wrapped_cmd}" "$@"
    fi
    ;;
  upload)
    if [ $3 = "0000" ] && ! [ -e "${worm_staging_path}/$2" ]; then
      exec cat >"${worm_staging_path}/$2"
    else
      "${wrapped_cmd}" "$@"
      ret=$?
      rm -f "${worm_staging_path}/$2"
      exit $?
    fi
    ;;
  download)
    if [ $3 = "0000" ] && [ -e "${worm_staging_path}/$2" ]; then
      cat "${worm_staging_path}/$2"
    else
      exec "${wrapped_cmd}" "$@"
    fi
    ;;
  remove)
    if [ $3 = "0000" ] && [ -e "${worm_staging_path}/$2" ]; then
      rm -f "${worm_staging_path}/$2"
    else
      exec "${wrapped_cmd}" "$@"
    fi
    ;;
  *)
    exit 2
    ;;
esac
