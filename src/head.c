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

#define PROG_NAME "head"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "errprintf.h"

static void head(FILE* file, unsigned n) {
   unsigned i = 0;
   char ch;
   while (i < n) {
      ch = fgetc(file);
      if (ch == EOF) break;
      else if (ch == '\n') ++i;
      putchar(ch);
   }
}

int main(int argc, char* argv[]) {
   unsigned long n = 10;
   int option;
   while ((option = getopt(argc, argv, ":n:")) != -1) {
      switch (option) {
      case 'n':
         n = strtoul(optarg, NULL, 10);
         break;
      case ':':
         puts("head: '-n' requires an argument");
         break;
      }
   }
   if (optind == argc) return head(stdin, n), 0;
   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      FILE* file = NULL;
      if (strcmp(path, "-") == 0) file = stdin;
      else file = fopen(path, "r");
      if (!file) {
         errprintf("failed to open '%s'", path);
         ec = 1;
         continue;
      }
      head(file, n);
      if (file != stdin) fclose(file);
   }
   return ec;
}
