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

#include <string.h>
#include <malloc.h>
#include "parse_size_match.h"
#include "edit.h"

// Parse a size matching fileset option.
bool ParseSizeMatch(const char* size_match_pattern,
                    struct s_sz_matching* size_matching)
{
  bool retval = false;
  char *private_copy, *bp;

  /*
   * Make a private copy of the input string.
   * As we manipulate the input and size_to_uint64
   * eats its input.
   */
  private_copy = strdup(size_match_pattern);

  // Empty the matching arguments.
  *size_matching = s_sz_matching{};

  /*
   * See if the size is a range e.g. there is a - in the
   * match pattern. As a size of a file can never be negative
   * this is a workable solution.
   */
  if ((bp = strchr(private_copy, '-')) != NULL) {
    *bp++ = '\0';
    size_matching->type = size_match_range;
    if (!size_to_uint64(private_copy, &size_matching->begin_size)) {
      goto bail_out;
    }
    if (!size_to_uint64(bp, &size_matching->end_size)) { goto bail_out; }
  } else {
    switch (*private_copy) {
      case '<':
        size_matching->type = size_match_smaller;
        if (!size_to_uint64(private_copy + 1, &size_matching->begin_size)) {
          goto bail_out;
        }
        break;
      case '>':
        size_matching->type = size_match_greater;
        if (!size_to_uint64(private_copy + 1, &size_matching->begin_size)) {
          goto bail_out;
        }
        break;
      default:
        size_matching->type = size_match_approx;
        if (!size_to_uint64(private_copy, &size_matching->begin_size)) {
          goto bail_out;
        }
        break;
    }
  }

  retval = true;

bail_out:
  free(private_copy);
  return retval;
}
