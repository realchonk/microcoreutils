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

#define PROG_NAME "mkfifo"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "errprintf.h"

int main(int argc, char* argv[]) {
   mode_t mode = 0777;
   int option;
   while ((option = getopt(argc, argv, ":m:")) != -1) {
      char* endp;
      switch (option) {
      case 'm':
         mode = (mode_t)strtoul(optarg, &endp, 8);
         if (*endp) {
            fprintf(stderr, "mkfifo: invalid mode '%s'\n", optarg);
            return 1;
         }
         break;
      case ':':
         fprintf(stderr, "mkfifo: expected argument for option '-%c'\n", optopt);
         return 1;
      case '?':
         fprintf(stderr, "mkfifo: invalid option '-%c'\n", optopt);
         return 1;
      }
   }

   int ec = 0;
   for (; optind < argc; ++optind) {
      if (mkfifo(argv[optind], mode) != 0) {
         errprintf("failed to create FIFO '%s'", argv[optind]);
         ec = 1;
      }
   }
   return ec;
}
