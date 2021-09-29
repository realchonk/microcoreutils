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

#define PROG_NAME "env"

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include "errprintf.h"
#include "clearenv.h"

// For some reason glibc-2.33 on Gentoo doesn't declare this:
extern char** environ;


static bool is_env(const char* s) {
   size_t i = 0;
   if (s[i] != '_' && !isalpha(s[i])) return false;
   ++i;
   while (s[i] == '_' || isalpha(s[i])) ++i;
   return s[i] == '=';
}

int main(int argc, char* argv[]) {
   int option;
   while ((option = getopt(argc, argv, "i")) != -1) {
      switch (option) {
      case 'i':
         clearenv();
         break;
      default: return 1;
      }
   }
   for (; optind < argc; ++optind) {
      if (is_env(argv[optind])) putenv(argv[optind]);
      else {
         execvp(argv[optind], argv + optind);
         const int saved_errno = errno;
         errprintf("'%s'", argv[optind])
         return saved_errno == ENOENT ? 127 : 126;
      }
   }
   for (size_t i = 0; environ[i]; ++i) {
      puts(environ[i]);
   }
   return 0;
}
