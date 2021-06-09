#   BAREOS® - Backup Archiving REcovery Open Sourced
#
#   Copyright (C) 2017-2017 Bareos GmbH & Co. KG
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

# * Find mysqlclient Find the native MySQL includes and library
#
# MySQL_INCLUDE_DIR - where to find mysql.h, etc. MySQL_LIBRARIES   - List of
# libraries when using MySQL. MySQL_FOUND       - True if MySQL found.

if(MySQL_INCLUDE_DIR)
  # Already in cache, be silent
  set(MySQL_FIND_QUIETLY TRUE)
endif(MySQL_INCLUDE_DIR)

find_path(MySQL_INCLUDE_DIR mysql.h /usr/local/include/mysql /usr/include/mysql)

set(MySQL_NAMES mysqlclient mysqlclient_r)
find_library(
  MySQL_LIBRARY
  NAMES ${MySQL_NAMES}
  PATHS /usr/lib /usr/local/lib
  PATH_SUFFIXES mysql
)

mark_as_advanced(MySQL_LIBRARY MySQL_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  MySQL REQUIRED_VARS MySQL_INCLUDE_DIR MySQL_LIBRARY
)

if(MySQL_FOUND)
  if(NOT TARGET MySQL::MySQL)
    add_library(MySQL::MySQL UNKNOWN IMPORTED)
    set_target_properties(
      MySQL::MySQL
      PROPERTIES IMPORTED_LOCATION "${MySQL_LIBRARY}"

                 INTERFACE_INCLUDE_DIRECTORIES "${MySQL_INCLUDE_DIR}"
    )
  endif()
endif()
