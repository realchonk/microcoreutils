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

#define PROG_NAME "sed"

#include "config.h"
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>
#include "errprintf.h"
#include "openfile.h"
#include "buf.h"

static bool suppress_defout = false;
static char** script = NULL;

static char** sed_readfile(FILE* file) {
   char** buf = NULL;
   char* line;
   while ((line = grep_readline(file)) != NULL)
      buf_push(buf, line);
   return buf;
}

static void free_buffer(char** buf) {
   for (size_t i = 0; i < buf_len(buf); ++i)
      free(buf[i]);
   buf_free(buf);
}

static bool process_file(char** buf) {
}

int main(int argc, char* argv[]) {
   int option;
   while ((option = getopt(argc, argv, ":ne:f:")) != -1) {
      switch (option) {
      case 'n':
         suppress_defout = true;
         break;
      case 'e':
      {
         char* str = strdup(optarg);
         if (!str) {
            errprintf("failed to strdup(\"%s\")", optarg);
            return 1;
         }
         buf_push(script, str);
         break;
      }
      case 'f':
      {
         FILE* file = fopen(optarg, "r");
         if (!file) {
            errprintf("failed to open file '%s'", optarg);
            return 1;
         }
         script = sed_readfile(file);
         fclose(file);
         break;
      }
      case ':':
         fprintf(stderr, "sed: expected argument for option '-%c'\n", optopt);
         return 1;
      case '?':
         fprintf(stderr, "sed: invalid option '-%c'\n", optopt);
         return 1;
      }
   }

   bool failed = false;
   if ((argc - optind) == 0) {
      char** buf = sed_readfile(stdin);
      failed |= !process_file(buf);
      free_buffer(buf);
   } else {
      for (; optind < argc; ++optind) {
         FILE* file = openfile_in(argv[optind]);
         if (!file) {
            errprintf("failed to open file '%s'", argv[optind]);
            free_buffer(script);
            return 1;
         }
         char** buf = sed_readfile(file);
         failed |= !process_file(buf);
         free_buffer(buf);
         closefile(file);
      }
   }

   free_buffer(script);

   return failed;
}
