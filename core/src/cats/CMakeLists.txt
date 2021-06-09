#   BAREOS® - Backup Archiving REcovery Open Sourced
#
#   Copyright (C) 2017-2020 Bareos GmbH & Co. KG
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
message("Entering ${CMAKE_CURRENT_SOURCE_DIR}")


add_definitions(-D_BDB_PRIV_INTERFACE_)

add_library(bareossql SHARED)
target_sources(bareossql PRIVATE
    bvfs.cc
    cats.cc
    sql.cc
    sql_create.cc
    sql_delete.cc
    sql_find.cc
    sql_get.cc
    sql_list.cc
    sql_pooling.cc
    sql_query.cc
    sql_update.cc
)

set_target_properties(
  bareossql PROPERTIES VERSION "${BAREOS_NUMERIC_VERSION}"
                       SOVERSION "${BAREOS_VERSION_MAJOR}"
)
target_link_libraries(bareossql bareoscats bareos)

# TODO: install default db backend

# if we have only one database backend, we do not create the dummy libbareoscats
# but instead we create a symlink to the db backend lib

list(LENGTH db_backends nr_db_backends)

add_library(bareoscats SHARED)
target_sources(bareoscats PRIVATE cats_backends.cc)
target_link_libraries(bareoscats bareos)
set_target_properties(
  bareoscats PROPERTIES VERSION ${BAREOS_NUMERIC_VERSION}
                        SOVERSION "${BAREOS_VERSION_MAJOR}"
)
install(TARGETS bareoscats DESTINATION ${libdir})

if(HAVE_SQLITE3)
  add_library(bareoscats-sqlite3 MODULE)
  target_sources(bareoscats-sqlite3 PRIVATE sqlite.cc cats_backends.cc)
  target_link_libraries(bareoscats-sqlite3 bareossql bareos sqlite3)
  install(TARGETS bareoscats-sqlite3 DESTINATION ${backenddir})

  install(
    DIRECTORY ddl
    DESTINATION ${scriptdir}
    FILES_MATCHING
    PATTERN *sqlite3*.sql
  )
endif()

if(mysql)
  find_package(MySQL REQUIRED)
  add_library(bareoscats-mysql MODULE)
  target_sources(bareoscats-mysql PRIVATE mysql.cc mysql_batch.cc cats_backends.cc)
  target_link_libraries(bareoscats-mysql PRIVATE bareossql bareos MySQL::MySQL)
  install(TARGETS bareoscats-mysql DESTINATION ${backenddir})

  install(
    DIRECTORY ddl
    DESTINATION ${scriptdir}
    FILES_MATCHING
    PATTERN *mysql*.sql
  )
endif()

if(postgresql)
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  find_package(PostgreSQL REQUIRED)
endif()
  add_library(bareoscats-postgresql MODULE)
  target_sources(bareoscats-postgresql PRIVATE postgresql.cc postgresql_batch.cc cats_backends.cc)
  target_link_libraries(
    bareoscats-postgresql PRIVATE bareossql bareos PostgreSQL::PostgreSQL
  )
  install(TARGETS bareoscats-postgresql DESTINATION ${backenddir})

  install(
    DIRECTORY ddl
    DESTINATION ${scriptdir}
    FILES_MATCHING
    PATTERN *postgresql*.sql
  )
endif()

install(TARGETS bareossql DESTINATION ${libdir})

install(
  FILES create_bareos_database
        update_bareos_tables
        make_bareos_tables
        grant_bareos_privileges
        drop_bareos_tables
        drop_bareos_database
        make_catalog_backup.pl
        make_catalog_backup
        delete_catalog_backup
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
              WORLD_READ WORLD_EXECUTE
  DESTINATION ${scriptdir}
)

install(
  DIRECTORY ddl
  DESTINATION ${scriptdir}
  PATTERN version.map
  PATTERN *.in EXCLUDE
  PATTERN *.sql EXCLUDE
)

if(${nr_db_backends} EQUAL 1)
  install(CODE "set(libdir \"${libdir}\")")
  install(CODE "set(backenddir \"${backenddir}\")")
  install(CODE "set(default_db_backend \"${default_db_backend}\")")
  install(
    CODE "set(CMAKE_SHARED_LIBRARY_SUFFIX \"${CMAKE_SHARED_LIBRARY_SUFFIX}\")"
  )
  install(CODE "set(VERSION \"${BAREOS_NUMERIC_VERSION}\")")
  install(CODE "set(SRC_DIR \"${PROJECT_SOURCE_DIR}\")")

  install(
    SCRIPT ${PROJECT_SOURCE_DIR}/cmake/bareos-symlink-default-db-backend.cmake
  )
endif()
