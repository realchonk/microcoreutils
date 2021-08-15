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
#include <string.h>
#include <stdio.h>

static int contains(const char* set, char ch) {
   for (size_t i = 0; set[i]; ++i) {
      if (set[i] == ch) return 1;
   }
   return 0;
}

static void substitute(const char* set, const char* rep, char* ch, size_t rep_len) {
   for (size_t i = 0; set[i]; ++i) {
      if (set[i] == *ch) {
         if (i < rep_len) *ch = rep[i];
         else *ch = rep[rep_len - 1];
      }
   }
}

// TODO: add extended values (like '[:space:]')

int main(int argc, char* argv[]) {
   if (argc < 3) {
print_usage:
      fputs("Usage: tr [-c|-C] [-s] string1 string2\n", stderr);
      fputs("       tr -s [-c|-C] string1\n", stderr);
      fputs("       tr -d [-c|-C] string1\n", stderr);
      fputs("       tr -ds [-c|-C] string1 string2\n", stderr);
      return 1;
   }
   int delete = 0, complement = 0, unrepeat = 0;
   int option;
   while ((option = getopt(argc, argv, ":cCds")) != -1) {
      switch (option) {
      case 'c':
      case 'C': complement = 1; break;
      case 'd': delete = 1; break;
      case 's': unrepeat = 1; break;
      default: goto print_usage;
      }
   }
   const int nargs = argc - optind;
   if (nargs < 1 || nargs > 2 || (nargs != (2 - (delete ^ unrepeat)))) goto print_usage;
   const char* set = argv[optind];
   const char* rep = argv[optind + 1] ? argv[optind + 1] : argv[optind];
   const size_t rep_len = strlen(rep);

   if (rep && rep[0] == '\0') {
      fputs("tr: when not truncating string2 must not be empty!\n", stderr);
      return 1;
   }


   char last_ch, ch = '\0';
   if (complement) {
      while (last_ch = ch, (ch = getchar()) != EOF) {
         if (!delete && !unrepeat && !contains(set, ch)) ch = rep[rep_len - 1];
         else if (delete && !contains(set, ch)) continue;
         else if (unrepeat && ch == last_ch && !contains(rep, ch)) continue;

         putchar(ch);
      }
   } else {
      while (last_ch = ch, (ch = getchar()) != EOF) {
         if (!delete && !unrepeat) substitute(set, rep, &ch, rep_len);
         else if (delete && contains(set, ch)) continue;
         else if (unrepeat && ch == last_ch && contains(rep, ch)) continue;

         putchar(ch);
      }
   }
   return 0;
}
