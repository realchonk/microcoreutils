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

#define PROG_NAME "cmp"

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include "errprintf.h"
#include "openfile.h"

int main(int argc, char* argv[]) {
   const char* filename1;
   const char* filename2;
   bool list = false;
   bool suppress = false;
   int option;
   while ((option = getopt(argc, argv, ":ls")) != -1) {
      switch (option) {
      case 'l':
         list = true;
         break;
      case 's':
         suppress = true;
         break;
      default:
      print_usage:
         fputs("Usage: cmp [-l|-s] file1 file2\n", stderr);
         return 2;
      }
   }
   if ((list && suppress) || (argc - optind) != 2)
      goto print_usage;

   filename1 = argv[optind + 0];
   filename2 = argv[optind + 1];

   FILE* file1 = openfile_in(filename1);
   if (!file1) {
      if (!suppress)
         errprintf("failed to open '%s'", filename1);
      return 2;
   }
   FILE* file2 = openfile_in(filename2);
   if (!file2) {
      if (!suppress)
         errprintf("failed to open '%s'", filename2);
      closefile(file1);
      return 2;
   }

   size_t nl = 1;    // number of current line
   size_t nb = 1;    // number of current byte
   int ec = 0;
   int ch1, ch2;
   while (true) {
      ch1 = fgetc(file1);
      ch2 = fgetc(file2);

      if (ch1 == EOF || ch2 == EOF) {
         if (ch1 != ch2) {
            ec = 1;
            if (!suppress) {
               if (list) {
                  fprintf(stderr, "cmp: EOF on %s\n", ch1 == EOF ? filename1 : filename2);
               } else {
                  printf("%s %s differ: char %zu, line %zu\n", filename1, filename2, nb, nl);
               }
            }
            break;
         }
         break;
      } else if (ch1 != ch2){
         ec = 1;
         if (!suppress) {
            if (list) {
               printf("%zu %o %o\n", nb, ch1, ch2);
               continue;
            } else {
               printf("%s %s differ: char %zu, line %zu\n", filename1, filename2, nb, nl);
            }
         }
         break;
      }
      
      if (ch1 == '\n')
         ++nl;
      ++nb;
   }

   closefile(file1);
   closefile(file2);
   return ec;
}
