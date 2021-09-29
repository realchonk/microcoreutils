//  Copyright (C) 2021 Benjamin Stürz
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef FILE_ERRPRINTF_H
#define FILE_ERRPRINTF_H
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifndef PROG_NAME
#error "PROG_NAME is not defined"
#endif

inline static void errprintf(const char* fmt, ...) {
   const int saved_errno = errno;
   va_list ap;
   va_start(ap, fmt);
   
   fputs(PROG_NAME ": ", stderr);
   vfprintf(stderr, fmt, ap);
   if (saved_errno) {
      fprintf(stderr, ": %s\n", strerror(errno));
   } else {
      fputc('\n', stderr);
   }
}

#endif /* FILE_ERRPRINTF_H */
