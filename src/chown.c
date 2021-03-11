#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[]) {
   if (argc < 2) {
print_usage:
      fputs("Usage: chown [-h] owner[:group] file...\n", stderr);
      fputs("       chown -R [-H|-L|-P] owner[:group] file...\n", stderr);
      return 1;
   }
   int recursive = 0;
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

   for (; optind < argc; ++optind) {
      
   }

   if (group) free(owner);
   return 0;
}
