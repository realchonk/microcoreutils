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

#define PROG_NAME "split"

#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "errprintf.h"

// Calculate 26^n for size_t
static size_t pow26uz(size_t n) {
   size_t res = 1;
   while (n--) {
      res *= 26;
   }
   return res;
}

static char* create_filename(const char* prefix, size_t num, size_t len_suffix) {
   const size_t len_prefix = strlen(prefix);
   const size_t len = len_prefix + len_suffix + 1;
   char* buffer = malloc(len);

   strncpy(buffer, prefix, len_prefix);
   for (size_t i = 0; i < len_suffix; ++i) {
      buffer[i + len_prefix] = num / pow26uz(i) % 26 + 'a';
   }
   buffer[len - 1] = '\0';

   return buffer;
}

int main(int argc, char* argv[]) {
   const char* filename = "-";
   const char* prefix = "x";
   size_t suffix_length = 2;
   size_t chunk_size = 1000;
   bool chunks_lines = true;
   char* endp;
   int option;
   while ((option = getopt(argc, argv, ":a:b:l:")) != -1) {
      switch (option) {
      case 'a':
         suffix_length = (size_t)strtoumax(optarg, &endp, 10);
         if (*endp) {
            fprintf(stderr, "split: invalid suffix length: %s\n", optarg);
            return 1;
         }
         break;
      case 'b':
         chunk_size = (size_t)strtoumax(optarg, &endp, 10);
         chunks_lines = false;
         switch (*endp) {
         case 'k':
            chunk_size <<= 10;
            break;
         case 'm':
            chunk_size <<= 20;
            break;
         case '\0':
            break;
         default:
            fprintf(stderr, "split: invalit byte size: %s\n", optarg);
            return 1;
         }
         break;
      case 'l':
         chunk_size = (size_t)strtoumax(optarg, &endp, 10);
         chunks_lines = true;
         if (*endp) {
            fprintf(stderr, "split: invalid line count: %s\n", optarg);
            return 1;
         }
         break;
      default:
      print_usage:
         fputs("Usage: split [-l line_count] [-a suffix_length] [file [name]]\n", stderr);
         fputs("       split -b n[k|m] [-a suffix_length] [file [name]]\n", stderr);
         return 1;
      }
   }
   const int narg = argc - optind;
   if (narg > 0) {
      filename = argv[optind];
      if (narg == 2) {
         prefix = argv[optind + 1];
      } else goto print_usage;
   }

   if (strlen(prefix) + suffix_length >= NAME_MAX) {
      fputs("split: filename too long.\n", stderr);
      return 1;
   }

   FILE* input;
   if (!strcmp(filename, "-")) {
      input = stdin;
   } else {
      input = fopen(filename, "r");
   }

   if (!input) {
      errprintf("failed to open '%s'", filename);
      return 1;
   }

   char* buffer = malloc(chunks_lines ? 100 : chunk_size);
   if (!buffer) {
      errprintf("failed to allocate a buffer");
      return 1;
   }
   
   size_t cfile = 0;
   while (!feof(input)) {
      if (cfile == 676) {
         fputs("split: exceeded maximum of 676 files.\n", stderr);
         return 1;
      }
      char* path = create_filename(prefix, cfile++, suffix_length);
      FILE* output = NULL;

      if (chunks_lines) {
         for (size_t n = 0; n < chunk_size; ++n) {
            do {
               if (!fgets(buffer, 100, input))
                  goto end_for;


               if (!output) {
                  output = fopen(path, "w");
                  if (!output) {
                     errprintf("failed to open '%s'", path);
                     fclose(input);
                     free(buffer);
                     free(path);
                     return 1;
                  }
               }

               fputs(buffer, output);
            } while (!strchr(buffer, '\n'));
         }
         end_for:
      } else {
         size_t n = fread(buffer, 1, chunk_size, input);
         if (n == 0)
            break;

         output = fopen(path, "w");
         if (!output) {
            errprintf("failed to open '%s'", path);
            fclose(input);
            free(buffer);
            free(path);
            return 1;
         }

         fwrite(buffer, 1, n, output);

         if (n < chunk_size) {
            fclose(output);
            break;
         }
      }
      free(path);
      if (output)
         fclose(output);
   }

   free(buffer);
   fclose(input);
   return 0;
}

