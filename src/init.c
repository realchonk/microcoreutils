#include <sys/reboot.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

static bool running;
static pid_t pid_shell;
static void signal_handler(int sig) {
   switch (sig) {
   case SIGTERM:
   case SIGKILL:
      kill(pid_shell, SIGKILL);
      running = false;
      break;
   }
}
static bool init_hostname(const char* path) {
   FILE* file = fopen(path, "r");
   if (!file) return false;
   char buffer[HOST_NAME_MAX + 1];
   if (!fgets(buffer, sizeof(buffer), file)) return fclose(file), false;
   fclose(file);

   const size_t len = strlen(buffer) - 1;
   if (buffer[len] == '\n') buffer[len] = '\0';
   return sethostname(buffer, len) == 0;
}

int main(void) {
   if (getpid() != 1) {
      fputs("init: already running\n", stderr);
      return 1;
   }

   // Register signal handlers
   signal(SIGTERM, signal_handler);
   signal(SIGKILL, signal_handler);

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

   // Set the hostname
   if (!init_hostname("/etc/hostname")) perror("init: failed to set hostname");

   // Setup some environement variables
   putenv("TERM=linux");

   // Start single-user mode
   running = true;
   while (running) {
      pid_t pid = fork();
      if (pid == 0) {
         execl("/bin/login", "-", NULL);
         perror("init: failed to exec /bin/sh");
         return 1;
      }
      pid_shell = pid;
      int wstatus;
      waitpid(pid, &wstatus, 0);
   }

   // TODO: terminate all processes

   // Write all changes to disk
   sync();

   // TODO: find portable solution
   reboot(RB_POWER_OFF);

   return 0;
}


