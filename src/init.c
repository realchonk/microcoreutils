#include <sys/reboot.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
   if (getpid() != 1) {
      fputs("init: already running\n", stderr);
      return 1;
   }
   // Create /proc, /sys, /dev, /dev/pts
   mkdir("/proc", 755);
   mkdir("/sys", 755);
   mkdir("/dev", 755);
   mkdir("/dev/pts", 755);

   // Mount /proc, /sys, /dev/pts
   if (mount(NULL, "/proc", "proc", 0, NULL) < 0) perror("Failed to mount /proc");
   if (mount(NULL, "/sys", "sysfs", 0, NULL) < 0) perror("Failed to mount /sys");
   mount(NULL, "/dev", "devtmpfs", 0, NULL); // May already be mounted by kernel
   if (mount(NULL, "/dev/pts", "devpts", 0, NULL) < 0) perror("Failed to mount /dev/pts");

   // Setup some environement variables
   putenv("USER=root");
   putenv("SHELL=/bin/sh");

   // Start single-user mode
   while (1) {
      pid_t pid = fork();
      if (pid == 0) {
         execl("/bin/sh", "-", NULL);
         perror("init: failed to exec /bin/sh");
         return 1;
      }
      int wstatus;
      waitpid(pid, &wstatus, 0);
      if (!WIFEXITED(wstatus) || WEXITSTATUS(wstatus) != 0) break;
   }

   // Write all changes to disk
   sync();

   // TODO: find portable solution
   reboot(RB_POWER_OFF);

   return 0;
}


