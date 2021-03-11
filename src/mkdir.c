#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char* argv[]) {
   if (argc < 2) {
      fputs("Usage: mkdir [-p] [-m mode] dir...\n", stderr);
      return 1;
   }

   int make_parents = 0;
   int mode = 0755;

   int option;
   while ((option = getopt(argc, argv, ":pm:")) != -1) {
      switch (option) {
      case 'p':   make_parents = 1; break;
      case 'm':
         mode = strtol(optarg, NULL, 8) & 0777;
         break;
      case ':':
         fputs("mkdir: -m needs an argument\n", stderr);
         break;
      case '?':
         fprintf(stderr, "mkdir: unknown option: -%c\n", optopt);
         break;
      }
   }
   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      if (make_parents) {
         char* buffer = (char*)malloc(strlen(path) + 1);
         if (!buffer) {
            fprintf(stderr, "mkdir: %s\n", strerror(errno));
            ec = 1;
            continue;
         }

         char* end = buffer;
         while (1) {
            strcpy(buffer, path);
            end = strchr(end, '/');
            if (!end) break;
            *end = '\0';
            ++end;
            if (mkdir(buffer, mode) != 0) {
               fprintf(stderr, "mkdir: failed to create: '%s': %s\n", path, strerror(errno));
               ec = 1;
               goto failed;
            }

         }

         free(buffer);
      }
      if (mkdir(path, mode) != 0) {
         fprintf(stderr, "mkdir: failed to create '%s': %s\n", path, strerror(errno));
         ec = 1;
      }
failed:;
   }
   return ec;
}

