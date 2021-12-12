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

#define PROG_NAME "cut"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "errprintf.h"


int main(int argc, char* argv[]) {
   if (argc < 2) {
print_usage:
      fputs("Usage: cut -b list [-n] [file...]\n"
            "       cut -c list [file...]\n"
            "       cut -f list [-d delim] [-s] [file...]\n",
            stderr);
      return 1;
   }
   int main_opt = '\0';
   const char* list = NULL;
   const char* delim = NULL;
   bool dont_split = false;
   bool suppress = false;
   int option;
   while ((option = getopt(argc, argv, ":b:c:d:f:ns")) != -1) {
      switch (option) {
      case 'b':
      case 'c':
      case 'f':
         if (main_opt)
            goto print_usage;
         main_opt = option;
         list = optarg;
         break;
      case 'd':
         delim = optarg;
         break;
      case 's':
         suppress = true;
         break;
      case 'n':
         dont_split = true;
         break;
      default:
         goto print_usage;
      }
   }

   if (!main_opt
      || (dont_split && main_opt != 'b')
      || ((delim != NULL || suppress) && main_opt != 'f')
      || (delim == NULL && main_opt == 'f')
      ) {
      goto print_usage;
   }

   fputs("This program is not complete!\n", stderr);

   return 1;

}
