#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

int main(int argc, char* argv[]) {
   int use_utc0 = 0;
   const char* fmt = NULL;
   for (int i = 1; i < argc; ++i) {
      if (argv[i][0] == '+') {
         if (fmt) {
            fprintf(stderr, "date: extra operand '%s'\n", argv[i]);
            return 1;
         } else fmt = argv[i] + 1;
      }
      else if (strcmp(argv[i], "-u") == 0) use_utc0 = 1;
      else {
         fputs("Usage: date [-u] [+format]\n", stderr);
         return 1;
      }
   }
   if (!fmt) fmt = "%a %b %e %H:%M:%S %Z %Y";
   char buf[100];
   const time_t t = time(NULL);
   struct tm* tm;
   if (use_utc0) tm = gmtime(&t);
   else tm = localtime(&t);
   strftime(buf, sizeof(buf), fmt, tm);
   puts(buf);
   return 0;
}
