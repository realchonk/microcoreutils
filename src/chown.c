#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>

#ifndef true
#define true 1
#define false 0
#endif

static int parse_number(const char* s, unsigned* u) {
   unsigned val = 0;
   for (size_t i = 0; s[i]; ++i) {
      if (!isdigit(s[i])) return 0;
      val = val * 10 + (s[i] - '0');
   }
   *u = val;
   return true;
}

static int getusrinfo(uid_t* uid, gid_t* gid, const char* user, const char* group) {
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

static int recursive;
static int do_chown(const char* path, uid_t uid, gid_t gid) {
   struct stat st;
   if (stat(path, &st) != 0) {
      fprintf(stderr, "chown: failed to stat '%s': %s\n", path, strerror(errno));
      return false;
   }
   int rv = true;
   if (recursive && (st.st_mode & S_IFMT) == S_IFDIR) {
      // path is a directory
      const size_t len = strlen(path);
      DIR* dir;
      struct dirent* ent;
      char* buffer = (char*)malloc(len + 260);
      if (!buffer) {
         fprintf(stderr, "chown: failed to allocate buffer: %s\n", strerror(errno));
         return false;
      }
      if ((dir = opendir(path)) == NULL) {
         fprintf(stderr, "chown: failed to access '%s': %s\n", path, strerror(errno));
         return false;
      }
      readdir(dir); // read '.'
      readdir(dir); // read '..'
      while ((ent = readdir(dir)) != NULL) {
         memcpy(buffer, path, len);
         buffer[len] = '/';
         strncpy(buffer + len + 1, ent->d_name, sizeof(ent->d_name));
         if (chown(buffer, uid, gid) != 0) {
            fprintf(stderr, "chown: failed to change owner of '%s': %s\n", buffer, strerror(errno));
            rv = false;
         }
      }
   }
   if (chown(path, uid, gid) != 0) {
      fprintf(stderr, "chown: failed to change owner of '%s': %s\n", path, strerror(errno));
      return false;
   }
   return rv;
}

// TODO: add support for -H, -L, -P options

int main(int argc, char* argv[]) {
   if (argc < 2) {
print_usage:
      fputs("Usage: chown [-h] owner[:group] file...\n", stderr);
      fputs("       chown -R [-H|-L|-P] owner[:group] file...\n", stderr);
      return 1;
   }
   recursive = 0;
   int opt_h = 0, opt_upper = 0;
   int option;
   while ((option = getopt(argc, argv, ":RHLPh")) != -1) {
      switch (option) {
      case 'R':   recursive = 1; break;
      case 'h':   opt_h = 1; break;
      case 'H':   opt_upper = 'H'; break;
      case 'L':   opt_upper = 'L'; break;
      case 'P':   opt_upper = 'P'; break;
      default:    goto print_usage;
      }
   }
   if ((argc - optind) < 2) goto print_usage;

   char* owner = argv[optind++];
   char* group = NULL;
   char* end;
   if ((end = strchr(owner, ':')) != NULL) {
      group = end + 1;
      char* tmp = (char*)malloc(end - owner + 2);
      if (!tmp) {
         fprintf(stderr, "chmod: failed to allocate memory: %s\n", strerror(errno));
         return 1;
      }
      strncpy(tmp, owner, end - owner);
      owner = tmp;
   }

   uid_t uid;
   gid_t gid = -1;
   if (!getusrinfo(&uid, &gid, owner, group)) return 1;

   printf("uid=%s, gid=%s\n", owner, group);

   int rv = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      if (!do_chown(path, uid, gid)) rv = 1;
   }

   if (group) free(owner);
   return rv;
}
