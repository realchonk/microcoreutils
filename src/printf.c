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

#define PROG_NAME "printf"

#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "errprintf.h"
#include "buf.h"

inline static int isodigit(int ch) {
   return ch >= '0' && ch <= '7';
}

#define next_arg() (narg < argc ? argv[narg++] : (fputs("printf: expected argument\n", stderr), exit(1), ""))

int main(int argc, char* argv[]) {
   if (argc < 2) {
      fputs("Usage: printf format [argument...]\n", stderr);
      return 1;
   }

   int narg = 2;

   for (const char* s = argv[1]; *s; ++s) {
      if (*s == '\\') {
         ++s;
         char ch = '\0';
         if (isodigit(*s)) {
            int val = 0;
            for (int i = 0; i < 3 && isodigit(*s); ++i, ++s) {
               ch = ch * 8 + (*s - '0');
            }
         } else {
            switch (*s) {
            case 'a':   ch = '\a'; break;
            case 'b':   ch = '\b'; break;
            case 'c':   return 0;
            case 'f':   ch = '\f'; break;
            case 'n':   ch = '\n'; break;
            case 'r':   ch = '\r'; break;
            case 't':   ch = '\t'; break;
            case 'v':   ch = '\v'; break;
            case '"':   ch = '\"'; break;
            case '\'':  ch = '\''; break;
            case '\\':  break;
            default:
               putchar('\\');
               putchar(*s);
               continue;
            }
         }
         putchar(ch);
      } else if (*s == '%') {
         ++s;
         if (*s == '%') {
            putchar('%');
         }
         char* fmtstr = NULL;
         buf_push(fmtstr, '%');
         while (1) {
            switch (*s) {
            case '#':
            case '0':
            case '-':
            case ' ':
            case '+':
               buf_push(fmtstr, *s);
               continue;
            }
            break;
         }


         if (isdigit(*s)) {
            while (isdigit(*s)) {
               buf_push(fmtstr, *s++);
            }
         } else if (*s == '*') {
            const char* arg = next_arg();
            if (atoi(arg) != 0) {
               while (arg)
                  buf_push(fmtstr, *arg++);
            }
         }

         if (*s == '.') {
            buf_push(fmtstr, '.');
            ++s;
            while (isdigit(*s)) {
               buf_push(fmtstr, *s++);
            }
         }


         buf_push(fmtstr, *s);
         buf_push(fmtstr, '\0');

         switch (*s) {
         case 'c':
            printf(fmtstr, next_arg()[0]);
            break;
         case 'd':
         case 'u':
         case 'o':
         case 'x':
         case 'X':
            printf(fmtstr, atoi(next_arg()));
            break;
         case 's':
            printf(fmtstr, next_arg());
            break;
         }

         buf_free(fmtstr);
      } else {
         putchar(*s);
      }
   }
}
