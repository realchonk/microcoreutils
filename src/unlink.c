#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[]) {
   if (argc != 2) {
      puts("Usage: unlink file");
      return 1;
   }
   if (unlink(argv[1]) != 0) {
      printf("unlink: cannot unlink '%s': %s\n", argv[1], strerror(errno));
      return 1;
   }
   return 0;
}
