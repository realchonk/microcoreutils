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

#define PROG_NAME "renice"

#include <sys/resource.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include "errprintf.h"

int main(int argc, char** argv) {
   const char* what;
   int which = PRIO_PROCESS;
   int inc;
   bool has_inc = false;
   int option;
   while ((option = getopt(argc, argv, ":pgun:")) != -1) {
      char* endp;
      switch (option) {
      case 'p':
         which = PRIO_PROCESS;
         what = "process";
         break;
      case 'g':
         which = PRIO_PGRP;
         what = "process group";
         break;
      case 'u':
         which = PRIO_USER;
         what = "user";
         break;
      case 'n':
         inc = (int)strtol(optarg, &endp, 10);
         if (*endp) {
            fprintf(stderr, "renice: invalid number '%s'\n", optarg);
            return 1;
         }
         has_inc = true;
         break;
      default:
         goto print_usage;
      }
   }
   const int narg = argc - optind;
   if (!has_inc) {
      fputs("renice: expected increment operand.\n", stderr);
      return 1;
   } else if (narg < 1) {
   print_usage:
      fputs("Usage: renice [-g|-p|-u] -n increment ID...\n", stderr);
      return 1;
   }


   int ec = 0;
   for (int i = optind; i < argc; ++i) {
      const char* id = argv[i];
      char* endp;
      int who;
      if (which == PRIO_PROCESS || which == PRIO_PGRP) {
         who = (int)strtol(id, &endp, 10);
         if (*endp) {
            fprintf(stderr, "renice: invalid %s ID '%s'.\n", what, id);
            ec = 1;
            continue;
         }
      } else {
         if (isdigit(*id)) {
            who = (int)strtol(id, &endp, 10);
            if (*endp) {
               fprintf(stderr, "renice invalid user ID '%s'.\n", id);
               ec = 1;
               continue;
            }
         } else {
            // get UID from name
            struct passwd* pwd = getpwnam(id);
            if (!pwd) {
               errprintf("failed to resolve user name '%s'", id);
               ec = 1;
               continue;
            }
            who = pwd->pw_uid;
         }
      }
      if (setpriority(which, who, inc) < 0) {
         errprintf("failed to set nice value for %s '%s'", what, id);
         ec = 1;
      }
   }
   return ec;
}

