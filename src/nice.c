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

#define PROG_NAME "nice"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "errprintf.h"

static const int default_nice_value = 10;

int main(int argc, char** argv) {
   int nice_val = default_nice_value;
   char** parg = &argv[1];
   while (*parg && !strncmp("-n", *parg, 2)) {
      if (!strcmp("-n", *parg)) {
         if (parg[1]) {
            char* endp;
            nice_val = (int)strtol(*++parg, &endp, 10);
            if (*endp) {
               fprintf(stderr, "nice: invalid increment '%s'\n", *parg);
               return 1;
            }
         } else {
            goto print_usage;
         }
      } else {
         char* endp;
         nice_val = (int)strtol(*parg + 2, &endp, 10);
         if (*endp) {
            fprintf(stderr, "nice: invalid increment '%s'\n", *parg + 2);
            return 1;
         }
      }
      ++parg;
   }
   
   if (!*parg) {
   print_usage:
      fputs("Usage: nice [-n increment] utility [argument...]\n", stderr);
      return 1;
   }

   errno = 0;
   const int tmp = nice(nice_val);
   (void)tmp;
   if (errno) {
      errprintf("failed to set nice value to '%d'", nice_val);
   }

   execvp(*parg, parg);
   const int saved_errno = errno;
   errprintf("'%s'", *parg);
   return saved_errno == ENOENT ? 127 : 126;
}
