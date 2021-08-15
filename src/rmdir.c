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

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


int main(int argc, char* argv[]) {
   if (argc < 2) {
print_usage:
      fputs("Usage: rmdir [-p] dir...\n", stderr);
      return 1;
   }
   int option;
   int parents = 0;
   while ((option = getopt(argc, argv, ":p")) != -1) {
      switch (option) {
      case 'p':   parents = 1; break;
      default:    goto print_usage;
      }
   }
   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      if (rmdir(path) < 0) {
         fprintf(stderr, "rmdir: %s: %s\n", path, strerror(errno));
         ec = 1;
         continue;
      }
      if (parents) {
         char* buffer = (char*)malloc(strlen(path) + 1);
         if (!buffer) {
            fprintf(stderr, "rmdir: %s\n", strerror(errno));
            ec = 1;
            continue;
         }
  
         char* end;
         strcpy(buffer, path);
         while ((end = strrchr(buffer, '/')) != NULL) {
            *end = '\0';
            if (rmdir(path) != 0) {
               fprintf(stderr, "rmdir: %s: %s\n", path, strerror(errno));
               ec = 1;
               break;
            }
         }

         free(buffer);
      }
   }
   return ec;
}
