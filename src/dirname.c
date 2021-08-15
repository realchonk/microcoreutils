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

// For documentation see: https://pubs.opengroup.org/onlinepubs/9699919799/utilities/dirname.html

static int is_slash(const char* str, size_t len) {
   for (size_t i = 0; i < len; ++i) {
      if (str[i] != '/') return 0;
   }
   return 1;
}

int main(int argc, char* argv[]) {
   if (argc != 2) {
      fputs("Usage: dirname string\n", stderr);
      return 1;
   }
   char* str = argv[1];
   const size_t len = strlen(str);
   char* end = &str[len - 1];
   if (strcmp(str, "//") == 0) goto step_6;
   if (is_slash(str, len)) {
      puts("/");
      return 0;
   }

   while (*end == '/') *end-- = '\0';

   if (strchr(str, '/') == NULL) {
      puts(".");
      return 0;
   }

   while (*end != '/') *end-- = '\0';
step_6:
   while (*end == '/') *end-- = '\0';
   if (*str == '\0') puts("/");
   else puts(str);
   return 0;
}
