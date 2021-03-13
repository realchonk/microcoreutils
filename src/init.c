#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
   while (1) {
      pid_t pid = fork();
      if (pid == 0) {
         execl("/bin/sh", "/bin/sh", NULL);
         perror("init: failed to exec /bin/sh");
         return 1;
      }
      waitpid(pid, NULL, 0);
   }

   return 0;
}


