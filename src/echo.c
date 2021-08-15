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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static int parse_xdigit(char ch) {
   if (isdigit(ch)) return ch - '0';
   else if (islower(ch)) return ch - 'a' + 10;
   else if (isupper(ch)) return ch - 'A' + 10;
   else return 0;
}
static int isoctal(char ch) {
   return ch >= '0' && ch <= '7';
}

static void print_str(const char* s) {
   size_t i = 0;
   int tmp;
   while (s[i] != '\0') {
      char ch = s[i++];
      if (ch == '\\') {
         ch = s[i++];
         if (ch == '\0') {
            putchar('\\');
            continue;
         }
         switch (ch) {
         case 'a':   putchar('\a'); break;
         case 'b':   putchar('\b'); break;
         case 'c':   exit(0);
         case 'f':   putchar('\f'); break;
         case 'n':   putchar('\n'); break;
         case 'r':   putchar('\r'); break;
         case 't':   putchar('\t'); break;
         case 'v':   putchar('\v'); break;
         case '\\':  putchar('\\'); break;
         case '\'':  putchar('\''); break;
         case '\"':  putchar('\"'); break;
         case 'x':
            ch = s[i++];
            if (!isxdigit(ch)) {
               printf("\\x%c", ch);
               continue;
            }
            tmp = parse_xdigit(ch);
            ch = s[i++];
            if (!isxdigit(ch)) {
               putchar(tmp);
               putchar(ch);
               continue;
            }
            tmp = tmp * 10 + parse_xdigit(ch);
            putchar(tmp);
            break;
         case '0':
            ch = s[i++];
            tmp = 0;
            for (unsigned j = 0; j < 3 && isoctal(s[i]); ++j, ++i) {
               tmp = tmp * 8 + (s[i] - '0');
            }
            putchar(tmp);
            break;
         default:
            putchar('\\');
            putchar(ch);
            break;
         }
      }
      else putchar(ch);
   }
}

int main(int argc, char* argv[]) {
   if (argc == 1) goto end;
   
   print_str(argv[1]);
   for (int i = 2; i < argc; ++i) {
      putchar(' ');
      print_str(argv[i]);
   }
end:
   putchar('\n');
   return 0;
}
