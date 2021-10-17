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

#define PROG_NAME "mv"

#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "errprintf.h"
#include "prompt.h"

static bool is_tty(void) {
   return ttyname(STDIN_FILENO) != NULL;
}

#define is_dir(st) (((st).st_mode & S_IFMT) == S_IFDIR)

#define is_writable(path) (access((path), W_OK) == 0)

static char* make_sub_path(const char* dir, const char* ent) {
   const size_t len_dir = strlen(dir);
   const size_t len_ent = strlen(ent);
   const size_t len_buf = len_dir + len_ent + 2;
   char* buf = malloc(len_buf);
   snprintf(buf, len_buf, "%s/%s", dir, ent);
   return buf;
}

int main(int argc, char* argv[]) {
   bool prompt = false;
   int option;
   while ((option = getopt(argc, argv, ":if")) != -1) {
      switch (option) {
      case 'i':
         prompt = true;
         break;
      case 'f':
         prompt = false;
         break;
      case '?':
         printf("mv: unknown option: -%c\n", optopt);
         break;
      }
   }

   const int narg = argc - optind;
   struct stat st_dest, st_src;

   if (narg < 2) {
      fputs("Usage: mv [-if] source_file target_file\n", stderr);
      fputs("       mv [-if] source_file... target_dir\n", stderr);
      return 1;
   }

   const char* src;
   const char* dest = argv[argc - 1];
   const int has_dest = stat(dest, &st_dest) == 0;

   if (narg == 2 && (!has_dest || !is_dir(st_dest))) {
      src = argv[optind];

      // if the destination exists and one of the following:
      // - `-i` was specified
      // - the destination is not writable and stdin is a tty
      // then print a prompt
      if (has_dest && (prompt || (!is_writable(dest) && is_tty()))) {
         if (!do_prompt(true, "overwrite '%s'", dest))
            return 0;
      }
      if (rename(src, dest) != 0) {
         errprintf("cannot move '%s' to '%s'", src, dest);
         return 1;
      }
      return 0;
   }

   if (!has_dest) {
      fprintf(stderr, "mv: no such directory '%s'.\n", dest);
      return 1;
   }

   if (!is_dir(st_dest)) {
      fprintf(stderr, "mv: '%s' is not a directory.\n", dest);
      return 1;
   }

   for (int i = optind; i < (argc - 1); ++i) {
      src = argv[i];
      char* dest_path = make_sub_path(dest, src);

      if (rename(src, dest_path) != 0) {
         errprintf("failed to move '%s' to '%s", src, dest_path);
         return 1;
      }

      free(dest_path);
   }

   return 0;
}
