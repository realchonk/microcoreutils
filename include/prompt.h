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

#ifndef FILE_PROMPT_H
#define FILE_PROMPT_H
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef PROG_NAME
#error PROG_NAME is not defined
#endif

// Prints a prompt and waits for a response.
// If the response is 'y' or 'Y' it returns true;
// if the response is empty it returns def_val;
// otherwise it returns false.
static bool do_prompt(bool def_val, const char* fmt, ...) {
   va_list ap;
   va_start(ap, fmt);

   char tmp[3] = "yn";
   if (def_val) {
      tmp[0] = 'Y';
   } else {
      tmp[1] = 'N';
   }

   fputs(PROG_NAME ": ", stderr);
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, " [%s]? ", tmp);

   char ch;
   while (scanf("%c", &ch) == 0);

   va_end(ap);
   return (ch == '\n') ? def_val : ((ch == 'y') || (ch == 'Y'));
}

#endif /* FILE_PROMPT_H */
