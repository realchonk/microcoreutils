#include <unistd.h>
#include <stdio.h>

int main(void) {
   char* tty = ttyname(STDIN_FILENO);
   if (tty) return puts(tty), 0;
   perror("tty: Failed to get ttyname for stdin");
   return 1;
}
