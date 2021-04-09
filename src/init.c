#include <sys/reboot.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>
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

static char* read_file(const char* path, const char* defval, size_t* len_out) {
   size_t len;
   char* text;
   struct stat st;
   if (lstat(path, &st) != 0) {
   fallback:
      len = strlen(defval);
      text = (char*)malloc(len + 1);
      if (!text) {
         perror("Failed to allocate buffer");
         abort();
      }
      memcpy(text, defval, len + 1);
      goto end;
   }
   len = st.st_size;
   
   int fd = open(path, O_RDONLY);
   if (fd < 0) goto fallback;

   text = (char*)malloc(len + 1);
   if (!text) {
      perror("Failed to allocate buffer");
      abort();
   }
   if (read(fd, text, len) < 0) { close(fd); goto fallback; }
   text[len] = '\0';

   close(fd);

end:
   if (len_out) *len_out = len;
   return text;
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
   size_t hostname_len;
   char* hostname = read_file("/etc/hostname", "linux", &hostname_len);
   if (sethostname(hostname, hostname_len) < 0) perror("Failed to set hostname");
   
   // Setup some environement variables
   free(hostname);
   putenv("USER=root");
   putenv("SHELL=/bin/sh");
   putenv("EUID=0");
   putenv("EGID=0");

   // Start single-user mode
   running = true;
   while (running) {
      pid_t pid = fork();
      if (pid == 0) {
         execl("/bin/sh", "-", NULL);
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


