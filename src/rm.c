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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include "config.h"

#if HAVE_SYS_SYSMACROS_H
#include <sys/sysmacros.h>
#endif

static int has_permission(int euid, int egid, int uid, int gid, int mode) {
   if (euid == uid) return (mode & 0200) == 0200;
   else if (egid == gid) return (mode & 0020) == 0020;
   else return (mode & 0002) == 0002;
}

static int do_prompt(void) {
   const int ch = getchar();
   if (ch == '\n') return 0;
   while (getchar() != '\n');
   return (ch == 'y') || (ch == 'Y');
}

static int euid, egid, recursive, force, prompt;
static bool delete_file(const char*);
static bool recursive_delete(const char* path) {
   DIR* dir;
   struct dirent* ent;
   if ((dir = opendir(path)) == NULL) {
      fprintf(stderr, "rm: failed to open '%s': %s\n", path, strerror(errno));
      return 0;
   }

   const size_t len = strlen(path);
   char* buffer = (char*)malloc(len + 260);
   bool rv = true;
   while ((ent = readdir(dir)) != NULL) {
      if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
      memcpy(buffer, path, len);
      buffer[len] = '/';
      strncpy(buffer + len + 1, ent->d_name, 256);
      
      if (!delete_file(buffer) && !force)
         rv = false;

   }
   free(buffer);
   return rv;
}

static bool delete_file(const char* path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0) {
      if (!force) {
         fprintf(stderr, "rm: cannot remove '%s': %s\n", path, strerror(errno));
         return false;
      }
      return true;
   }
   const int mode = statbuf.st_mode & 0777;
   const int uid = statbuf.st_uid;
   const int gid = statbuf.st_gid;
   if (prompt || (!has_permission(euid, egid, uid, gid, mode) && !force)) {
      fprintf(stderr, "rm: remove '%s'? ", path);
      if (!do_prompt()) return true;
   }


   if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
      // 'path' is a directory
      if (!recursive) {
         fprintf(stderr, "rm: cannot remove '%s': %s\n", path, strerror(EISDIR));
         return false;
      }
      if (!recursive_delete(path)) return false;
      if (rmdir(path) != 0) {
         fprintf(stderr, "rm: cannot remove '%s': %s\n", path, strerror(errno));
         return false;
      }
      return true;
   }

   if (unlink(path) != 0) {
      fprintf(stderr, "rm: cannot remove '%s': %s\n", path, strerror(errno));
      return false;
   }
   return true;
}


int main(int argc, char* argv[]) {
   if (argc < 2) {
   print_usage:
      fputs("Usage: rm [-iRr] file...\n", stderr);
      fputs("       rm -f [-iRr] [file...]\n", stderr);
      return 1;
   }
   force = 0;
   recursive = 0;
   prompt = 0;
   int option;
   while ((option = getopt(argc, argv, ":iRrf")) != -1) {
      switch (option) {
      case 'f':   force = 1; prompt = 0; break;
      case 'R':
      case 'r':   recursive = 1; break;
      case 'i':   prompt = 1; force = 0; break;
      default:    goto print_usage;
      }
   }
   if (optind == argc) {
      if (force) return 0;
      else goto print_usage;
   }

   euid = geteuid();
   egid = getegid();

   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      if (!delete_file(path) && !force) ec = 1;
   }
   return ec;
}
