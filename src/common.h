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

inline static const char* basename(const char* path) {
   const char* tmp = strrchr(path, '/');
   return tmp ? tmp + 1 : path;
}



#endif /* FILE_COMMON_H */
