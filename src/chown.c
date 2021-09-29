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

#define PROG_NAME "chown"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include "common.h"
#include "chown.h"
#include "errprintf.h"

static int recursive, opt_h = 0, opt_upper = 0, initial;

int main(int argc, char* argv[]) {
   if (argc < 2) {
print_usage:
      fputs("Usage: chown [-h] owner[:group] file...\n", stderr);
      fputs("       chown -R [-H|-L|-P] owner[:group] file...\n", stderr);
      return 1;
   }
   int option;
   recursive = 0;
   opt_upper = 'P';
   while ((option = getopt(argc, argv, ":RHLPh")) != -1) {
      switch (option) {
      case 'R':   recursive = 1; opt_upper = 'P'; break;
      case 'h':   opt_h = 1; break;
      case 'H':   opt_upper = 'H'; break;
      case 'L':   opt_upper = 'L'; break;
      case 'P':   opt_upper = 'P'; break;
      default:    goto print_usage;
      }
   }
   if ((argc - optind) < 2 || (opt_upper && !recursive)) goto print_usage;
   
   char* owner = argv[optind++];
   char* group = NULL;
   char* end;
   if ((end = strchr(owner, ':')) != NULL) {
      group = end + 1;
      char* tmp = (char*)malloc(end - owner + 2);
      if (!tmp) {
         errprintf("failed to allocate memory");
         return 1;
      }
      strncpy(tmp, owner, end - owner);
      owner = tmp;
   }

   uid_t uid;
   gid_t gid = -1;
   if (!getusrinfo(&uid, &gid, owner, group)) return 1;

   int rv = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      initial = true;
      if (!do_chown(path, uid, gid)) rv = 1;
   }

   if (group) free(owner);
   return rv;
}
