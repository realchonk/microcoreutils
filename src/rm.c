#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static int has_permission(int euid, int egid, int uid, int gid, int mode) {
   if (euid == uid) return (mode & 0200) == 0200;
   else if (egid == gid) return (mode & 0020) == 0020;
   else return (mode & 0002) == 0002;
}

static int do_prompt(void) {
   const int ch = getchar();
   if (ch == '\n') return 0;
   while (getchar() != '\n');
   return (ch == 'y') || (ch == 'Y');
}

int main(int argc, char* argv[]) {
   if (argc < 2) {
   print_usage:
      fputs("Usage: rm [-iRr] file...\n", stderr);
      fputs("       rm -f [-iRr] [file...]\n", stderr);
      return 1;
   }
   int force = 0;
   int recursive = 0;
   int prompt = 0;
   int option;
   while ((option = getopt(argc, argv, ":iRrf")) != -1) {
      switch (option) {
      case 'f':   force = 1; prompt = 0; break;
      case 'R':
      case 'r':   recursive = 1; break;
      case 'i':   prompt = 1; force = 0; break;
      default:    goto print_usage;
      }
   }
   if (optind == argc) {
      if (force) return 0;
      else goto print_usage;
   }

   const int euid = geteuid();
   const int egid = getegid();

   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      struct stat statbuf;
      if (stat(path, &statbuf) != 0) {
         if (!force) {
            fprintf(stderr, "rm: cannot remove '%s': %s\n", path, strerror(errno));
            ec = 1;
         }
         continue;
      }

      const int mode = statbuf.st_mode & 0777;
      const int uid = statbuf.st_uid;
      const int gid = statbuf.st_gid;

      if (prompt || (!has_permission(euid, egid, uid, gid, mode) && !force)) {
         fprintf(stderr, "rm: remove '%s'? ", path);
         if (!do_prompt()) continue;
      }


      if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
         // 'path' is a directory
         if (!recursive) {
            fprintf(stderr, "rm: cannot remove '%s': %s\n", path, strerror(EISDIR));
            ec = 1;
            continue;
         }
         // TODO: implement recursive remove
         fputs("Recursive rm is not implemented!\n", stderr);
         continue;
      }

      if (unlink(path) != 0) {
         fprintf(stderr, "rm: cannot remove '%s': %s\n", path, strerror(errno));
         ec = 1;
      }
   }
   return ec;
}
