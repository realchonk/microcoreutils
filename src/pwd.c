#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
   int resolve_links = 0;
   int option;
   while ((option = getopt(argc, argv, ":LP")) != -1) {
      switch (option) {
      case 'P': resolve_links = 1; break;
      case 'L': resolve_links = 0; break;
      default: goto print_usage;
      }
   }
   int free_pwd = 0;
   char* pwd = getenv("PWD");
   if (!pwd) {
      pwd = (char*)malloc(PATH_MAX + 1);
      if (!pwd) {
         perror("pwd: Failed to allocate buffer");
         abort();
      }
      if (!getcwd(pwd, PATH_MAX)) {
         perror("pwd: Failed to get current working directory");
         free(pwd);
         return 1;
      }
   }
   int ec = 0;
   if (resolve_links) {
      char buf[PATH_MAX + 1];
      if (!realpath(pwd, buf)) {
         perror("pwd: Failed to resolve path");
         ec = 1;
      }
      else puts(buf);
   }
   else puts(pwd);
   if (free_pwd) free(pwd);
   return ec;
print_usage:
   fputs("Usage: pwd [-L|-P]\n", stderr);
   return 1;
}

