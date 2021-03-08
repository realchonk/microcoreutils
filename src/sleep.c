#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
   if (argc != 2) {
   print_usage:
      puts("Usage: sleep time");
      return 1;
   }
   char* endp;
   unsigned long n = strtoul(argv[1], &endp, 10);
   if (*endp) goto print_usage;
   sleep(n);
   return 0;
}
