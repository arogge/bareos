# BAREOS® - Backup Archiving REcovery Open Sourced
#
# Copyright (C) 2021-2021 Bareos GmbH & Co. KG
#
# This program is Free Software; you can redistribute it and/or modify it under
# the terms of version three of the GNU Affero General Public License as
# published by the Free Software Foundation and included in the file LICENSE.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#[=======================================================================[.rst:
FindRPC
-----------

Find RPC headers and libraries.
This will try glibc's integrated RPC first and fall back to
a pkgconfig approach to find libtirpc.
`
IMPORTED Targets
^^^^^^^^^^^^^^^^

The following :prop_tgt:`IMPORTED` targets may be defined:

``RPC::RPC``

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``RPC_FOUND``
  True if RPC found.
``RPC_INCLUDE_DIRS``
  Where to find rpc/rpc.h.
``RPC_LIBRARIES``
  List of libraries when using RPC.

#]=======================================================================]

find_path(
  RPC_INCLUDE_DIR
  NAMES rpc/rpc.h
)

include(FindPackageHandleStandardArgs)
if(RPC_INCLUDE_DIR)
  find_package_handle_standard_args(
    RPC
    REQUIRED_VARS RPC_INCLUDE_DIR
  )
else()
  # if we did not find rpc/rpc.h in the system's default path
  # we provably need libtirpc, so take a look with pkgconfig
  find_package(PkgConfig QUIET)
  pkg_check_modules(PC_RPC QUIET libtirpc)

  find_path(
    RPC_INCLUDE_DIR
    NAMES rpc/rpc.h
    HINTS ${PC_RPC_INCLUDEDIR} ${PC_RPC_INCLUDE_DIRS}
  )

  find_library(
    RPC_LIBRARY
    NAMES tirpc libtirpc
    HINTS ${PC_RPC_LIBDIR} ${PC_RPC_LIBRARY_DIRS}
  )

  if(PC_RPC_VERSION)
    set(RPC_VERSION_STRING ${PC_RPC_VERSION})
  endif()

  find_package_handle_standard_args(
    RPC
    REQUIRED_VARS RPC_LIBRARY RPC_INCLUDE_DIR
    VERSION_VAR RPC_VERSION_STRING
  )

endif()


mark_as_advanced(RPC_INCLUDE_DIR RPC_LIBRARY)

if(RPC_FOUND AND NOT TARGET RPC::RPC)
  add_library(RPC::RPC UNKNOWN IMPORTED)
  set_target_properties(
    RPC::RPC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
    "${RPC_INCLUDE_DIRS}"
  )
  if(RPC_LIBRARY)
    set_property(
      TARGET RPC::RPC
      APPEND
      PROPERTY IMPORTED_LOCATION "${RPC_LIBRARY}"
    )
  endif()
endif()
