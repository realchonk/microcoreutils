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

static int parse_mode(const char* s, unsigned* u) {
   unsigned val = 0;
   for (size_t i = 0; s[i]; ++i) {
      if (s[i] < '0' || s[i] > '7') return 0;
      val = val * 8 + (s[i] - '0');
   }
   *u = val;
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
      readdir(dir); // read '.'
      readdir(dir); // read '..'
      while ((ent = readdir(dir)) != NULL) {
         memcpy(buffer, path, len);
         buffer[len] = '/';
         strncpy(buffer + len + 1, ent->d_name, sizeof(ent->d_name));
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

// TODO: add support for extended syntax (like: a+wt or +x)

int main(int argc, char* argv[]) {
   if (argc < 2) {
print_usage:
      fputs("Usage: chmod [-R] mode file...\n", stderr);
      return 1;
   }
   recursive = 0;
   int option;
   while ((option = getopt(argc, argv, ":R")) != -1) {
      switch (option) {
      case 'R':   recursive = 1; break;
      default:    goto print_usage;
      }
   }
   if ((argc - optind) < 2) goto print_usage;

   unsigned mode;
   if (!parse_mode(argv[optind++], &mode)) goto print_usage;

   int rv = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      if (!do_chmod(path, mode)) rv = 1;
   }

   return rv;
}
