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

#ifndef FILE_OPENFILE_H
#define FILE_OPENFILE_H
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "buf.h"

inline static FILE* openfile_in(const char* filename) {
   return !strcmp(filename, "-") ? stdin : fopen(filename, "r");
}

inline static FILE* openfile_out(const char* filename) {
   return !strcmp(filename, "-") ? stdout : fopen(filename, "r");
}

inline static void closefile(FILE* file) {
   if (file != stdin && file != stdout)
      fclose(file);
}
inline static char* grep_readline(FILE* file) {
   char* buf = NULL;
   int ch;
   while ((ch = fgetc(file)) != EOF) {
      if (ch == '\n') {
         buf_push(buf, '\0');
         break;
      }
      buf_push(buf, ch);
   }
   if (!buf)
      return NULL;
   char* str = strdup(buf);
   buf_free(buf);
   return str;
}
inline static char* readfile(const char* path) {
   FILE* file = openfile_in(path);
   if (!file) {
      errprintf("%s", path);
      return NULL;
   }
   char* buf = NULL;
   int ch;
   while ((ch = fgetc(file)) != EOF) {
      buf_push(buf, ch);
   }
   if (buf[buf_len(buf)-1] == '\n') {
      buf[buf_len(buf)-1] = '\0';
   } else {
      buf_push(buf, '\0');
   }
   char* str = strdup(buf);
   buf_free(buf);
   return str;
}

#endif /* FILE_OPENFILE_H */
