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

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[]) {
   if (argc < 3) {
print_usage:
      puts("Usage: mv [-if] source_file target_file");
      puts("       mv [-if] source_file... target_dir");
      return 1;
   }
   int prompt = 0;
   int option;
   while ((option = getopt(argc, argv, ":if")) != -1) {
      switch (option) {
      case 'i':   prompt = 1; break;
      case 'f':   prompt = 0; break;
      case '?':
         printf("mv: unknown option: -%c\n", optopt);
         break;
      }
   }
   if ((argc - optind) < 2) goto print_usage;
   struct stat s;
   if ((argc - optind) == 2) {
      const char* src = argv[optind];
      const char* dest = argv[optind + 1];
      if (prompt && stat(dest, &s) >= 0) {
         char ch;
         printf("mv: overwrite '%s'? ", dest);
         while (scanf("%c\n", &ch) == 0);
         if (ch != 'y') return 0;
      }
      const int ec = rename(src, dest);
      if (ec  == -1) {
         printf("mv: cannot move '%s' to '%s': %s\n", src, dest, strerror(errno));
      }
      return errno;
   }
   else {

   }
}
