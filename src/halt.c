#include <signal.h>
#include <stdio.h>

int main(void) {
   // TODO: add ability for reboot
   if (kill(1, SIGTERM) < 0) {
      perror("Failed to kill init");
      return 1;
   }
   return 0;
}
