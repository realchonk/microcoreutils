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
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
   bool force = false, symbolic = false;
   int opt_LP = '\0';
   int option;
   while ((option = getopt(argc, argv, ":fsLP")) != -1) {
      switch (option) {
      case 'f': force = true; break;
      case 's': symbolic = true; break;
      case 'L':
      case 'P': opt_LP = option; break;
      default:  goto print_usage;
      }
   }
   if ((argc - optind) < 2) {
   print_usage:
      fputs("Usage: ln [-sf] [-L|-P] source_file target_file\n", stderr);
      fputs("       ln [-sf] [-L|-P] source_file... target_dir\n", stderr);
      return 1;
   }
   const char* target = argv[argc - 1]; 
   struct stat st_target;
   int error = stat(target, &st_target) ? errno : 0;
   

   if ((argc - optind) == 2 && (st_target.st_mode & S_IFMT) != S_IFDIR) {
      const char* path = argv[optind];
      if (symbolic) {
         if (symlink(path, target) != 0) {
            fprintf(stderr, "ln: failed to create symbolic link '%s': %s\n", target, strerror(errno));
            return 1;
         } else return 0;
      } else {
         if (link(path, target) != 0) {
            fprintf(stderr, "ln: failed to create hard-link '%s': %s\n", target, strerror(errno));
            return 1;
         } else return 0;
      }
   }
   if (error) {
      fprintf(stderr, "ln: failed to access '%s': %s\n", target, strerror(error));
      return 1;
   } else if ((st_target.st_mode & S_IFMT) != S_IFDIR) {
      fprintf(stderr, "ln: target '%s': %s\n", target, strerror(ENOTDIR));
      return 1;
   }
   const size_t len_target = strlen(target);
   char* buffer = malloc(len_target + PATH_MAX + 2);
   if (!buffer) {
      perror("ln: failed to allocate buffer");
      return 1;
   }
   int ec = 0;
   
   memcpy(buffer, target, len_target);
   buffer[len_target] = '/';

   for (; optind < (argc - 1); ++optind) {
      struct stat st;
      const char* path = argv[optind];
      strncpy(buffer + len_target + 1, path, PATH_MAX);
      error = stat(buffer, &st) ? errno : 0;
      if (!error && !force) {
         fprintf(stderr, "ln: file '%s' already exists.\n", buffer);
         ec = 1;
         continue;
      }
      if (st.st_ino == st_target.st_ino) {
         fprintf(stderr, "ln: target_dir '%s' names the same file as '%s'\n", target, path);
         ec = 1;
         continue;
      }
      if (!error && unlink(buffer) != 0) {
         fprintf(stderr, "ln: failed to unlink '%s': %s\n", buffer, strerror(errno));
         ec = 1;
         continue;
      }
      if (symbolic) {
         if (symlink(path, buffer) != 0) {
            fprintf(stderr, "ln: failed to create symbolic link '%s': %s\n", buffer, strerror(errno));
            ec = 1;
         }
         continue;
      }
      struct stat st_src;
      if (lstat(path, &st_src) != 0 && (st_src.st_mode & S_IFMT) == S_IFLNK) {
         if (linkat(AT_FDCWD, path, AT_FDCWD, buffer, opt_LP == 'P' ? 0 : AT_SYMLINK_FOLLOW) != 0) {
            fprintf(stderr, "ln: failed to create hard-link '%s': %s\n", buffer, strerror(errno));
            ec = 1;
         }
         continue;
      }
      if (link(path, buffer) != 0) {
         fprintf(stderr, "ln: failed to create hard-link '%s': %s\n", buffer, strerror(errno));
         ec = 1;
         continue;
      }
   }
   free(buffer);
   return ec;
}
