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

#define PROG_NAME "cat"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "errprintf.h"

static void cat(FILE* file) {
	char ch;
	while ((ch = fgetc(file)) != EOF) putchar(ch);
}

int main(int argc, char* argv[]) {
	int ec = 0;
	int has_done = 0;
   for (int i = 1; i < argc; ++i) {
		FILE* file;
      if (strcmp("-u", argv[i]) == 0) continue;
      if (strcmp("-", argv[i]) == 0) file = stdin;
      else file = fopen(argv[i], "r");
		has_done = 1;
      if (!file) {
         errprintf("cat: %s", argv[i]);
			ec = 1;
			continue;
		}
		cat(file);
		fclose(file);
   }
	if (!has_done) cat(stdin);
	return ec;
}
