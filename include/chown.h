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

#ifndef FILE_CHOWN_H
#define FILE_CHOWN_H

static int opt_h, opt_upper, recursive, initial;
static bool do_chown(const char* path, uid_t uid, gid_t gid) {
   int rv = true;
   struct stat st;
   int error;
   if (opt_h || recursive) error = lstat(path, &st);
   else error = stat(path, &st);
   if (error != 0) {
      fprintf(stderr, "chown: failed to access '%s': %s\n", path, strerror(errno));
      return false;
   }
   if ((st.st_mode & S_IFMT) == S_IFLNK) {
      if (opt_upper == 'P' || opt_h) {
         if (lchown(path, uid, gid) != 0) {
            fprintf(stderr, "chown: failed to change ower for '%s': %s\n", path, strerror(errno));
            return false;
         }
         return true;
      }
      if (opt_upper == 'L' || (opt_upper == 'H' && initial)) {
         if (opt_h) {
            if (lchown(path, uid, gid) != 0) {
               fprintf(stderr, "chown: failed to change ower for '%s': %s\n", path, strerror(errno));
               return false;
            }
         }
         if (stat(path, &st) != 0) {
            fprintf(stderr, "chown: failed to resolve link '%s': %s\n", path, strerror(errno));
            return false;
         }
      }
   }
   if (recursive && (st.st_mode & S_IFMT) == S_IFDIR) {
      // path is a directory
      const size_t len = strlen(path);
      DIR* dir;
      struct dirent* ent;
      char* buffer = (char*)malloc(len + sizeof(ent->d_name) + 4);
      if (!buffer) {
         fprintf(stderr, "chown: failed to allocate buffer: %s\n", strerror(errno));
         return false;
      }
      if ((dir = opendir(path)) == NULL) {
         fprintf(stderr, "chown: failed to access '%s': %s\n", path, strerror(errno));
         return false;
      }
      memcpy(buffer, path, len);
      buffer[len] = '/';
      while ((ent = readdir(dir)) != NULL) {
         if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0) continue;
         strncpy(buffer + len + 1, ent->d_name, 0 + sizeof(ent->d_name)); // '0 + ...' to suppress warnings on gcc
         initial = false;
         if (!do_chown(buffer, uid, gid)) {
            //fprintf(stderr, "chown: failed to change owner of '%s': %s\n", buffer, strerror(errno));
            rv = false;
         }
      }
      free(buffer);
      closedir(dir);
   }
   if (opt_h) error = lchown(path, uid, gid);
   else error = chown(path, uid, gid);
   if (error != 0) {
      fprintf(stderr, "chown: failed to change owner of '%s': %s\n", path, strerror(errno));
      return false;
   }
   return rv;
}

#endif /* FILE_CHOWN_H */
