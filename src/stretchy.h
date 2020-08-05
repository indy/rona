// Copyright (C) 2020 Inderjit Gill <email@indy.io>

// This file is part of Rona

// Rona is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Rona is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef STRETCHY_H
#define STRETCHY_H

//  modified stretchy buffer implementation to use FixedBlockAllocators
//  original implementation: https://github.com/nothings/stb/blob/master/stretchy_buffer.h

#define sb_free(alloc, a) ((a) ? rona_free((alloc), stb__sbraw(a)), 0 : 0)
#define sb_push(alloc, a, v) (stb__sbmaybegrow(alloc, a, 1), (a)[stb__sbn(a)++] = (v))
#define sb_count(a) ((a) ? stb__sbn(a) : 0)
#define sb_add(alloc, a, n)                                                                        \
  (stb__sbmaybegrow(alloc, a, n), stb__sbn(a) += (n), &(a)[stb__sbn(a) - (n)])
#define sb_last(a) ((a)[stb__sbn(a) - 1])

#define stb__sbraw(a) ((int*)(void*)(a)-2)
#define stb__sbm(a) stb__sbraw(a)[0]
#define stb__sbn(a) stb__sbraw(a)[1]

#define stb__sbneedgrow(a, n) ((a) == 0 || stb__sbn(a) + (n) >= stb__sbm(a))
#define stb__sbmaybegrow(alloc, a, n) (stb__sbneedgrow(a, (n)) ? stb__sbgrow(alloc, a, n) : 0)
#define stb__sbgrow(alloc, a, n) (*((void**)&(a)) = stb__sbgrowf((alloc), (a), (n), sizeof(*(a))))

static void* stb__sbgrowf(FixedBlockAllocator* alloc, void* arr, int increment, int itemsize) {
  int  dbl_cur = arr ? 2 * stb__sbm(arr) : 0;
  int  min_needed = sb_count(arr) + increment;
  int  m = dbl_cur > min_needed ? dbl_cur : min_needed;
  int* p = (int*)rona_realloc(alloc, arr ? stb__sbraw(arr) : 0, itemsize * m + sizeof(int) * 2);
  if (p) {
    if (!arr)
      p[1] = 0;
    p[0] = m;
    return p + 2;
  } else {
    RONA_ASSERT(p);
    return (void*)(2 * sizeof(int)); // try to force a NULL pointer exception later
  }
}

#endif // STRETCHY_H
