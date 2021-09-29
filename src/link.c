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

#define PROG_NAME "link"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "errprintf.h"

int main(int argc, char** argv) {
   if (argc != 3) {
      fputs("Usage: link file1 file2\n", stderr);
      return 1;
   }
   const char* file1 = argv[1];
   const char* file2 = argv[2];
   if (link(file1, file2) != 0) {
      errprintf("cannot create link '%s' to '%s'", file2, file1);
      return 1;
   }
   return 0;
}
