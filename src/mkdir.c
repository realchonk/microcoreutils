#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char* argv[]) {
   if (argc < 2) {
      puts("Usage: mkdir [-p] [-m mode] dir...");
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
         puts("mkdir: -m needs an argument");
         break;
      case '?':
         printf("mkdir: unknown option: -%c\n", optopt);
         break;
      }
   }
   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* name = argv[optind];
      if (make_parents) {
         // TODO
      }
      const int r = mkdir(name, mode);
      if (r != 0) {
         printf("mkdir: failed to create '%s': %s\n", name, strerror(errno));
         ec = 1;
      }
   }
   return ec;
}

