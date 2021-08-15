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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

int main(int argc, char* argv[]) {
   int use_utc0 = 0;
   const char* fmt = NULL;
   for (int i = 1; i < argc; ++i) {
      if (argv[i][0] == '+') {
         if (fmt) {
            fprintf(stderr, "date: extra operand '%s'\n", argv[i]);
            return 1;
         } else fmt = argv[i] + 1;
      }
      else if (strcmp(argv[i], "-u") == 0) use_utc0 = 1;
      else {
         fputs("Usage: date [-u] [+format]\n", stderr);
         return 1;
      }
   }
   if (!fmt) fmt = "%a %b %e %H:%M:%S %Z %Y";
   char buf[100];
   const time_t t = time(NULL);
   struct tm* tm;
   if (use_utc0) tm = gmtime(&t);
   else tm = localtime(&t);
   strftime(buf, sizeof(buf), fmt, tm);
   puts(buf);
   return 0;
}
