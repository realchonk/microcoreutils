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

#define PROG_NAME "mkdir"

#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "errprintf.h"

int main(int argc, char* argv[]) {
   if (argc < 2) {
      fputs("Usage: mkdir [-p] [-m mode] dir...\n", stderr);
      return 1;
   }

   int make_parents = 0;
   int mode = 0755;

   int option;
   while ((option = getopt(argc, argv, ":pm:")) != -1) {
      switch (option) {
      case 'p':   make_parents = 1; break;
      case 'm':
         mode = strtol(optarg, NULL, 8) & 0777;
         break;
      case ':':
         fputs("mkdir: -m needs an argument\n", stderr);
         break;
      case '?':
         fprintf(stderr, "mkdir: unknown option: -%c\n", optopt);
         break;
      }
   }
   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      if (make_parents) {
         char* buffer = (char*)malloc(strlen(path) + 1);
         if (!buffer) {
            errprintf("");
            ec = 1;
            continue;
         }

         char* end = buffer;
         while (1) {
            strcpy(buffer, path);
            end = strchr(end, '/');
            if (!end) break;
            *end = '\0';
            ++end;
            if (mkdir(buffer, mode) != 0) {
               errprintf("failed to create '%s'", path);
               ec = 1;
               goto failed;
            }

         }

         free(buffer);
      }
      if (mkdir(path, mode) != 0) {
         errprintf("failed to create '%s'", path);
         ec = 1;
      }
failed:;
   }
   return ec;
}

