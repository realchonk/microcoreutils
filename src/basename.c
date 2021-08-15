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

#include <string.h>
#include <stdio.h>

static int is_slash(const char* str, size_t len) {
   for (size_t i = 0; i < len; ++i) {
      if (str[i] != '/') return 0;
   }
   return 1;
}

int main(int argc, char* argv[]) {
   if (argc <= 1 || argc > 3) {
      fputs("Usage: basename string [suffix]\n", stderr);
      return 1;
   }
   char* str = argv[1];
   if (*str == '\0') {
      puts(str);
      return 0;
   }
   size_t len = strlen(str);

   if (is_slash(str, len)) {
      puts("/");
      return 0;
   }

   char* end = &str[len - 1];
   while (*end == '/') *end-- = '\0';

   char* tmp = strrchr(str, '/');
   if (tmp != NULL) str = tmp + 1;

   if (argc == 3) {
      const char* suffix = argv[2];
      const size_t len_suffix = strlen(suffix);
      len = strlen(str);
      if (len > len_suffix && memcmp(&str[len - len_suffix], suffix, len_suffix) == 0)
         str[len - len_suffix] = '\0';
   }

   puts(str);
}
