#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
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

static bool get_mode(const char* filename, mode_t* mode) {
   struct stat st;
   if (stat(filename, &st) < 0) {
      fprintf(stderr, "chmod: %s: %s\n", filename, strerror(errno));
      return false;
   }
   *mode = st.st_mode;
   return true;
}

static bool parse_mode(const char* s, mode_t* u) {
   mode_t val = 0;
   for (size_t i = 0; s[i]; ++i) {
      if (s[i] < '0' || s[i] > '7') return false;
      val = val * 8 + (s[i] - '0');
   }
   *u = val;
   return true;
}

static void mode_add(mode_t* mode, int mask, bool value) {
   if (value) *mode |= mask;
   else *mode &= ~mask;
}
static bool strcnt(const char* s, char ch) {
   while (*s) {
      if (*s == ch) return true;
      ++s;
   }
   return false;
}
static bool parse_mode2(const char* s, mode_t* mode, const mode_t um) {
   bool u = false, g = false, o = false, a = false;
   char op = '\0';

   // rwxXst
   char perms[6] = { 0 };
   while (strcnt("ugoa", *s)) {
      switch (*s) {
      case 'u': u = true; break; // owner
      case 'g': g = true; break; // group
      case 'o': o = true; break; // other users
      case 'a': a = true; break; // all users
      }
      ++s;
   }
   do {
      if (!strcnt("+-", *s)) return false;
      op = *s++;
      // TODO: add support for u+g etc and =
      
      if (!*s) return false;
      while (strcnt("rwxXst", *s)) {
         switch (*s) {
         case 'r': perms[0] = op; break;
         case 'w': perms[1] = op; break;
         case 'x': perms[2] = op; break;
         case 'X': perms[3] = op; break;
         case 's': perms[4] = op; break;
         case 't': perms[5] = op; break;
         }
         ++s;
      }
   } while (*s);

   if (u + g + o + a == 0) {
      if (perms[0]) mode_add(mode, 0444 & ~um, perms[0] == '+');
      if (perms[1]) mode_add(mode, 0222 & ~um, perms[1] == '+');
      if (perms[2]) mode_add(mode, 0111 & ~um, perms[2] == '+');
      if (perms[3]) mode_add(mode, S_ISGID & ~um, perms[3] == '+');
      if (perms[4]) mode_add(mode, S_ISUID & ~um, perms[4] == '+');
      if (perms[5]) mode_add(mode, S_ISVTX & ~um, perms[5] == '+');
      return true;
   }
   
   if (u) {
      if (perms[0]) mode_add(mode, S_IRUSR, perms[0] == '+');
      if (perms[1]) mode_add(mode, S_IWUSR, perms[1] == '+');
      if (perms[2]) mode_add(mode, S_IXUSR, perms[2] == '+');
      if (perms[3]) mode_add(mode, S_ISGID, perms[3] == '+');
      if (perms[4]) mode_add(mode, S_ISUID, perms[4] == '+');
      if (perms[5]) mode_add(mode, S_ISVTX, perms[5] == '+');
   }
   if (g) {
      if (perms[0]) mode_add(mode, S_IRGRP, perms[0] == '+');
      if (perms[1]) mode_add(mode, S_IWGRP, perms[1] == '+');
      if (perms[2]) mode_add(mode, S_IXGRP, perms[2] == '+');
      if (perms[3]) mode_add(mode, S_ISGID, perms[3] == '+');
      if (perms[4]) mode_add(mode, S_ISUID, perms[4] == '+');
      if (perms[5]) mode_add(mode, S_ISVTX, perms[5] == '+');
   }
   if (o) {
      if (perms[0]) mode_add(mode, S_IROTH, perms[0] == '+');
      if (perms[1]) mode_add(mode, S_IWOTH, perms[1] == '+');
      if (perms[2]) mode_add(mode, S_IXOTH, perms[2] == '+');
      if (perms[3]) mode_add(mode, S_ISGID, perms[3] == '+');
      if (perms[4]) mode_add(mode, S_ISUID, perms[4] == '+');
      if (perms[5]) mode_add(mode, S_ISVTX, perms[5] == '+');
   }

   return true;
}


static int recursive;
static int do_chmod(const char* path, mode_t mode) {
   struct stat st;
   if (stat(path, &st) != 0) {
      fprintf(stderr, "chmod: failed to stat '%s': %s\n", path, strerror(errno));
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
         fprintf(stderr, "chmod: failed to allocate buffer: %s\n", strerror(errno));
         return false;
      }
      if ((dir = opendir(path)) == NULL) {
         free(buffer);
         fprintf(stderr, "chmod: failed to access '%s': %s\n", path, strerror(errno));
         return false;
      }
      memcpy(buffer, path, len);
      buffer[len] = '/';
      while ((ent = readdir(dir)) != NULL) {
         if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0) continue;
         strncpy(buffer + len + 1, ent->d_name, 0 + sizeof(ent->d_name)); // '0 + ...' to suppress warnings on gcc
         if (chmod(buffer, mode) != 0) {
            fprintf(stderr, "chmod: failed to change mode for '%s': %s\n", buffer, strerror(errno));
            rv = false;
         }
      }
      free(buffer);
   }
   if (chmod(path, mode) != 0) {
      fprintf(stderr, "chmod: failed to change mode for '%s': %s\n", path, strerror(errno));
      return false;
   }
   return rv;
}

int main(int argc, char* argv[]) {
   if (argc < 2) {
print_usage:
      fputs("Usage: chmod [-R] mode file...\n", stderr);
      return 1;
   }
   recursive = 0;
   int narg = 1;
   while (narg < argc && strcmp(argv[narg], "-R") == 0) recursive = 1, ++narg;
   if (narg >= argc) goto print_usage;
   const char* mode_str = argv[narg++];
   const mode_t um = umask(0);
   umask(um);

   int rv = 0;
   bool had_file = false;
   for (; narg < argc; ++narg) {
      const char* str = argv[narg];
      if (strcmp(str, "-R") == 0) { recursive = 1; continue; }
      mode_t mode;
      had_file = true;
      if (!get_mode(str, &mode)) { rv = 1; continue; }
      if (!parse_mode(mode_str, &mode) && !parse_mode2(mode_str, &mode, um)) goto print_usage;
      if (!do_chmod(str, mode)) rv = 1;
   }

   if (!had_file) goto print_usage;

   return rv;
}
