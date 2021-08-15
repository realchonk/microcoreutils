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

#ifndef FILE_COMMON_H
#define FILE_COMMON_H
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include "buf.h"

inline static int is_directory(const struct stat* st) {
   return (st->st_mode & S_IFMT) == S_IFDIR;
}

inline static int parse_number(const char* s, unsigned* u) {
   unsigned val = 0;
   for (size_t i = 0; s[i]; ++i) {
      if (!isdigit(s[i])) return 0;
      val = val * 10 + (s[i] - '0');
   }
   *u = val;
   return true;
}
inline static int getusrinfo(uid_t* uid, gid_t* gid, const char* user, const char* group) {
   unsigned tmp;
   if (isdigit(user[0])) {
      if (parse_number(user, &tmp)) return (*uid = tmp), true;
      else return false;
   } else {
      struct passwd* pw = getpwnam(user);
      if (!pw) {
         fprintf(stderr, "chown: failed to get user info for '%s': %s\n", user, strerror(errno));
         return false;
      }
      *uid = pw->pw_uid;
   }
   if (group != NULL) {
      if (isdigit(group[0])) {
         if (parse_number(group, &tmp)) return (*gid = tmp), true;
         else return false;
      } else {
         struct group* grp = getgrnam(group);
         if (!grp) {
            fprintf(stderr, "chown: failed to get group info for '%s': %s\n", group, strerror(errno));
            return false;
         }
         *gid = grp->gr_gid;
      }
   }
   return true;
}

#ifndef _GNU_SOURCE
inline static const char* basename(const char* path) {
   const char* tmp = strrchr(path, '/');
   return tmp ? tmp + 1 : path;
}
#endif


inline static char* readline(FILE* file) {
   char* line = NULL;
   char ch;
   while ((ch = fgetc(file)) != EOF) {
      if (ch == '\n') break;
      else buf_push(line, ch);
   }
   buf_push(line, '\0');
   return line;
}

inline static bool strcnt(const char* s, char ch) {
   while (*s) {
      if (*s == ch) return true;
      ++s;
   }
   return false;
}
#endif /* FILE_COMMON_H */
