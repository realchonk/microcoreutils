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

#define PROG_NAME "pathchk"

#include <sys/stat.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>

static void warn(const char* fmt, ...) {
   va_list ap;
   va_start(ap, fmt);

   fputs(PROG_NAME ": ", stderr);
   vfprintf(stderr, fmt, ap);
   fputs(".\n", stderr);

   va_end(ap);
}

static bool check_length(const char* path, size_t mx) {
   if (strlen(path) > mx) {
      warn("'%s': file name is longer than %zu", path, mx);
      return false;
   }
   return true;
}

static bool check_comp_lengths(const char* path, char* buffer, size_t mx) {
   const char* comp = strtok(buffer, "/");
   while (comp) {
      if (strlen(comp) > mx) {
         warn("'%s': component '%s' is longer than %lu", path, comp, mx);
         return false;
      }
      comp = strtok(NULL, "/");
   }
   return true;
}

static bool run_normal(const char* path) {
   // is path longer than PATH_MAX?
   if (!check_length(path, PATH_MAX))
      return false;

   // does path contain any component which is longer than NAME_MAX?
   char* buffer = strdup(path);
   if (!check_comp_lengths(path, buffer, NAME_MAX)) {
      free(buffer);
      return false;
   }

   // does path contain any directory that is not searchable?
   if (*path) {
      strcpy(buffer, path);
      char* end = buffer;
      while ((end = strchr(end + 1, '/')) != NULL) {
         *end = '\0';

         struct stat st;
         if (stat(buffer, &st) == 0) {
            if ((st.st_mode & S_IFMT) != S_IFDIR) {
               warn("'%s': '%s' is not a directory", path, buffer);
               free(buffer);
               return false;
            }
            if (access(buffer, X_OK) != 0) {
               warn("'%s': '%s' is not searchable", path, buffer);
               free(buffer);
               return false;
            }
         }

         *end = '/';
      }
   }
   free(buffer);

   // I don't know how to perform the last check

   return true;
}

static bool ispfname(int ch) {
   return isalnum(ch) || ch == '/' || ch == '.' || ch == '_' || ch == '-';
}

static bool run_strong(const char* path) {
   // is path longer than _POSIX_PATH_MAX
   if (!check_length(path, _POSIX_PATH_MAX))
      return false;

   // does path contain any component which is longer than _POSIX_NAME_MAX?
   char* buffer = strdup(path);
   bool tmp = check_comp_lengths(path, buffer, _POSIX_NAME_MAX);
   free(buffer);
   if (!tmp)
      return false;

   // does path contain any characters that are not in the portable filename charset?
   for (size_t i = 0; path[i]; ++i) {
      if (!ispfname(path[i])) {
         warn("'%s': '%c' is not in the portable filename character set", path, path[i]);
      }
   }
}

static bool run_extra(const char* path) {
   // is path empty?
   if (!*path) {
      warn("empty filename");
      return false;
   }

   // does path begin with a '-'?
   if (*path == '-') {
      warn("'%s' begins with a '-'", path);
      return false;
   }

   return true;
}

int main(int argc, char** argv) {
   int option;
   bool strong = false;
   bool extra = false;
   while ((option = getopt(argc, argv, ":pP")) != -1) {
      switch (option) {
      case 'p':
         strong = true;
         break;
      case 'P':
         extra = true;
         break;
      case '?':
         fprintf(stderr, "pathchk: invalid option '-%c'.\n", optopt);
         return 1;
      }
   }

   const int narg = argc - optind;
   if (narg < 1) {
      fputs("Usage: pathchk [-pP] pathname...\n", stderr);
      return 1;
   }


   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];

      if (strong) {
         ec |= !run_strong(path);
      } else {
         ec |= !run_normal(path);
      }

      if (extra) {
         ec |= !run_extra(path);
      }

   }
   return ec;
}
