#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>

static bool use_kilo = false;
static bool summarize = false;
static bool show_all = false;
static char HL = ' ';

#define geterr() strerror(errno)

static size_t traverse(const char* path, bool top) {
   struct stat st;
   int ec;
   if (HL == 'L' || (HL == 'H' && top))
      ec = stat(path, &st);
   else ec = lstat(path, &st);
   if (ec) {
      fprintf(stderr, "du: failed to stat '%s': %s\n", path, geterr());
      return 0;
   }
   const bool is_dir = st.st_mode & S_IFDIR;
   size_t size = st.st_blocks;

   if (is_dir) {
      const size_t len_path = strlen(path);
      DIR* dir = opendir(path);
      struct dirent* ent;
      if (!dir) {
         fprintf(stderr, "du: failed to open directory '%s': %s\n", path, geterr());
         return 0;
      }
      char buffer[len_path + 262];
      memcpy(buffer, path, len_path);
      buffer[len_path] = '/';
      while ((ent = readdir(dir)) != NULL) {
         if (!strcmp(".", ent->d_name) || !strcmp("..", ent->d_name))
            continue;
         strncpy(buffer + len_path + 1, ent->d_name, 260);
         size += traverse(buffer, false);
      }
   }


   if ((summarize && top) || (!summarize && (is_dir || show_all || top))) {
      printf("%-7zu %s\n", size / (use_kilo ? 2 : 1), path);
   }
   return size;
}

int main(int argc, char* argv[]) {
   int option;
   while ((option = getopt(argc, argv, "askxHL")) != -1) {
      switch (option) {
      case 'a':
         show_all = true;
         break;
      case 'k':
         use_kilo = true;
         break;
      case 's':
         summarize = true;
         break;
      case 'H':
      case 'L':
         HL = option;
         break;
      case 'x':
         fputs("du: option '-x' is not implemented!\n", stderr);
         return 1;
      default:
         puts("Usage: du [-a|-s] [-k] [-H|-L] [file...]");
         return 0;
      }
   }
   if (show_all && summarize) {
      fputs("du: cannot both summarize and show all entries\n", stderr);
      return 1;
   }

   if (optind < argc) {
      for (; optind < argc; ++optind) {
         traverse(argv[optind], true);
      }
   } else {
      traverse(".", true);
   }
   return 0;
}
