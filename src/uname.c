#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define print(x) if (i == 0) { printf("%s", (x)); i = 1; } else printf(" %s", (x))

int main(int argc, char* argv[]) {
   int opt_m = 0, opt_n = 0, opt_r = 0, opt_s = 0, opt_v = 0;
   int option;
   while ((option = getopt(argc, argv, "amnrsv")) != -1) {
      switch (option) {
      case 'a':
         opt_m = opt_n = opt_r = opt_s = opt_v = 1;
      }
   }
   if (argc == 1) opt_s = 1;

   struct utsname u;
   if (uname(&u) != 0) {
      fprintf(stderr, "uname: failed to retrieve system information: %s\n", strerror(errno));
      return 1;
   }

   int i = 0;

   if (opt_s) {
      print(u.sysname);
   }
   if (opt_n) {
      print(u.nodename);
   }
   if (opt_r) {
      print(u.release);
   }
   if (opt_v) {
      print(u.version);
   }
   if (opt_m) {
      print(u.machine);
   }
   putchar('\n');
   return 0;
}
