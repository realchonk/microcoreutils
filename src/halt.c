#include <signal.h>
#include <stdio.h>

int main(void) {
   // TODO: add ability for reboot
   if (kill(1, SIGUSR1) < 0) {
      perror("Failed to kill init");
      return 1;
   }
   return 0;
}
