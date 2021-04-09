#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

// For some reason glibc-2.33 on Gentoo doesn't declare this:
extern char** environ;

static bool is_env(const char* s) {
   size_t i = 0;
   if (s[i] != '_' && !isalpha(s[i])) return false;
   ++i;
   while (s[i] == '_' || isalpha(s[i])) ++i;
   return s[i] == '=';
}

int main(int argc, char* argv[]) {
   int option;
   while ((option = getopt(argc, argv, "i")) != -1) {
      switch (option) {
      case 'i':
         clearenv();
         break;
      default: return 1;
      }
   }
   for (; optind < argc; ++optind) {
      if (is_env(argv[optind])) putenv(argv[optind]);
      else {
         execvp(argv[optind], argv + optind);
         fprintf(stderr, "env: '%s': %s\n", argv[optind], strerror(errno));
         return errno == ENOENT ? 127 : 126;
      }
   }
   for (size_t i = 0; environ[i]; ++i) {
      puts(environ[i]);
   }
   return 0;
}
