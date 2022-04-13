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

#include <stdint.h>

typedef enum
{
  size_match_none,
  size_match_approx,
  size_match_smaller,
  size_match_greater,
  size_match_range
} b_sz_match_type;

struct s_sz_matching {
  b_sz_match_type type{size_match_none};
  uint64_t begin_size{};
  uint64_t end_size{};
};

bool ParseSizeMatch(const char* size_match_pattern,
                    struct s_sz_matching* size_matching);
