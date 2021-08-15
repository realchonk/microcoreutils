//  Copyright (C) 2021 Benjamin Stürz
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <sys/reboot.h>
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
#include "config.h"

#if HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#else
#warning "init is currently only available for Linux"
#endif

static bool running;
static pid_t pid_shell;
static void signal_handler(int sig) {
   switch (sig) {
   case SIGTERM:
   case SIGKILL:
   case SIGUSR1:
      kill(pid_shell, SIGKILL);
      running = false;
      break;
   }
}
#if HAVE_SETHOSTNAME
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
#else
static bool init_hostname(const char* path) {
   (void)path;
   return false;
}
#endif

int main(void) {
   if (getpid() != 1) {
      fputs("init: already running\n", stderr);
      return 1;
   }

   // Register signal handlers
   signal(SIGUSR1, signal_handler);
   signal(SIGTERM, signal_handler);
   signal(SIGKILL, signal_handler);

#ifdef __linux__
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
#endif

   // Set the hostname
   if (!init_hostname("/etc/hostname")) perror("init: failed to set hostname");

#ifdef __linux__
   // Setup some environement variables
   putenv("TERM=linux");
#endif

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

#ifdef __linux__
   // TODO: find portable solution
   reboot(RB_POWER_OFF);
#endif

   return 0;
}


