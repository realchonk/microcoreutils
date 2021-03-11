#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


int main(int argc, char* argv[]) {
   if (argc < 2) {
print_usage:
      fputs("Usage: rmdir [-p] dir...\n", stderr);
      return 1;
   }
   int option;
   int parents = 0;
   while ((option = getopt(argc, argv, ":p")) != -1) {
      switch (option) {
      case 'p':   parents = 1; break;
      default:    goto print_usage;
      }
   }
   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      if (rmdir(path) < 0) {
         fprintf(stderr, "rmdir: %s: %s\n", path, strerror(errno));
         ec = 1;
         continue;
      }
      if (parents) {
         char* buffer = (char*)malloc(strlen(path) + 1);
         if (!buffer) {
            fprintf(stderr, "rmdir: %s\n", strerror(errno));
            ec = 1;
            continue;
         }
  
         char* end;
         strcpy(buffer, path);
         while ((end = strrchr(buffer, '/')) != NULL) {
            *end = '\0';
            if (rmdir(path) < 0) {
               fprintf(stderr, "rmdir: %s: %s\n", path, strerror(errno));
               ec = 1;
               break;
            }
         }

         free(buffer);
      }
   }
   return ec;
}
