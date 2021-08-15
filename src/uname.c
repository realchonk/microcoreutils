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

#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define print(x) if (i == 0) { printf("%s", (x)); i = 1; } else printf(" %s", (x))

int main(int argc, char* argv[]) {
   int opt_m = 0, opt_n = 0, opt_r = 0, opt_s = 0, opt_v = 0;
   int option;
   while ((option = getopt(argc, argv, ":amnrsv")) != -1) {
      switch (option) {
      case 'a':
         opt_m = opt_n = opt_r = opt_s = opt_v = 1;
         break;
      case 'm': opt_m = 1; break;
      case 'n': opt_n = 1; break;
      case 'r': opt_r = 1; break;
      case 's': opt_s = 1; break;
      case 'v': opt_v = 1; break;
      default:
         fputs("Usage: uname [-amnrsv]\n", stderr);
         return 1;
      }
   }
   if (argc == 1) opt_s = 1;

   struct utsname u;
   if (uname(&u) != 0) {
      fprintf(stderr, "uname: failed to retrieve system information: %s\n", strerror(errno));
      return 1;
   }

   int i = 0;

   if (opt_s) {
      print(u.sysname);
   }
   if (opt_n) {
      print(u.nodename);
   }
   if (opt_r) {
      print(u.release);
   }
   if (opt_v) {
      print(u.version);
   }
   if (opt_m) {
      print(u.machine);
   }
   putchar('\n');
   return 0;
}
