#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

static bool recursive = false, prompt = false, force = false, initial = false;
static int opt_HLP = 'H';

static bool do_prompt(const char* msg, ...) {
   va_list ap;
   char first_ch, ch;
   va_start(ap, msg);
   vfprintf(stderr, msg, ap);
   va_end(ap);
   fputc(' ', stderr);
   
   first_ch = getchar();
   if (first_ch == '\n') return false;
   while ((ch = getchar()) != '\n');
   return tolower(first_ch) == 'y';
}

static bool copy(const char* dest_file, const char* source_file);
static bool fcopy(const char* dest_file, const char* source_file, const struct stat* st_source) {
   struct stat st_dest;
   int fd_dest = -1;
   if (stat(dest_file, &st_dest) == 0) {
      if (prompt && !do_prompt("cp: overwrite '%s'?", dest_file)) return true;
      fd_dest = open(dest_file, O_WRONLY | O_TRUNC);
      if (fd_dest < 0) {
         if (force) {
            if (unlink(dest_file) != 0) {
               fprintf(stderr, "cp: failed to remove '%s': %s\n", dest_file, strerror(errno));
               return false;
            }
         } else {
            fprintf(stderr, "cp: failed to access '%s': %s\n", dest_file, strerror(errno));
            return false;
         }
      }
   }
   if (fd_dest < 0) fd_dest = open(dest_file, O_WRONLY | O_CREAT, st_source->st_mode);
   if (fd_dest < 0) {
      fprintf(stderr, "cp: failed to open '%s' for write: %s\n", dest_file, strerror(errno));
      return false;
   }

   const int fd_src = open(source_file, O_RDONLY);
   if (fd_src < 0) {
      fprintf(stderr, "cp: failed to open '%s' for read: %s\n", source_file, strerror(errno));
      close(fd_dest);
      return false;
   }
   void* buffer = malloc(st_source->st_size);
   ssize_t num;
   while ((num = read(fd_src, buffer, st_source->st_size)) > 0) {
      while ((num -= write(fd_dest, buffer, num)) > 0);
   }

   close(fd_dest);
   close(fd_src);
   return true;
}
static bool copy_link(const char* dest_file, const char* source_file) {
   char* buffer = (char*)malloc(PATH_MAX + 1);
   if (!buffer) return perror("cp: failed to allocate buffer"), false;
   if (readlink(source_file, buffer, PATH_MAX) <= 0) {
      fprintf(stderr, "cp: failed to resolve symbolic link '%s': %s\n", source_file, strerror(errno));
      free(buffer);
      return false;
   }
   if (symlink(dest_file, buffer) != 0) {
      fprintf(stderr, "cp: failed to create symbolic link '%s': %s\n", dest_file, strerror(errno));
      free(buffer);
      return false;
   }
   free(buffer);
   return true;
}

static bool copy_dir(const char* dest_file, const char* source_file, const struct stat* st_source) {
   if (!recursive) {
      fprintf(stderr, "cp: -R not specified; omitting directory '%s'\n", source_file);
      return false;
   }
   struct stat st_dest;
   int error = stat(dest_file, &st_dest);
   if (!error && (st_dest.st_mode & S_IFMT) != S_IFDIR) {
      fprintf(stderr, "cp: '%s' already exists.\n", dest_file);
      return false;
   }
   if (error && mkdir(dest_file, st_source->st_mode) != 0) {
      fprintf(stderr, "cp: failed to create directory '%s': %s\n", dest_file, strerror(errno));
      return false;
   }

   struct dirent* ent;
   DIR* dir = opendir(source_file);
   if (!dir) {
      fprintf(stderr, "cp: failed to open directory '%s': %s\n", dest_file, strerror(errno));
      return false;
   }
   const size_t len_dest = strlen(dest_file);
   const size_t len_source = strlen(source_file);
   char* new_dest = (char*)malloc(len_dest + sizeof(ent->d_name) + 2);
   if (!new_dest) return perror("cp: failed to allocate buffer"), closedir(dir), false;
   char* new_source = (char*)malloc(len_source + sizeof(ent->d_name) + 2);
   if (!new_source) return perror("cp: failed to allocate buffer"), free(new_dest), closedir(dir), false;

   memcpy(new_dest, dest_file, len_dest);
   memcpy(new_source, source_file, len_source);
   new_dest[len_dest] = '/';
   new_source[len_source] = '/';

   bool ec = true;
   while ((ent = readdir(dir)) != NULL) {
      if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0) continue;
      strncpy(new_dest + len_dest + 1, ent->d_name, 0 + sizeof(ent->d_name));
      strncpy(new_source + len_source + 1, ent->d_name, 0 + sizeof(ent->d_name));
      if (!copy(new_dest, new_source)) ec = false;
   }

   free(new_source);
   free(new_dest);
   closedir(dir);
   return ec;
}

static bool copy(const char* dest_file, const char* source_file) {
   struct stat st_source;
   int error;
   if (opt_HLP == 'P' || (opt_HLP == 'H' && !initial)) error = lstat(source_file, &st_source);
   else error = stat(source_file, &st_source);
   if (error != 0) {
      fprintf(stderr, "cp: failed to access '%s': %s\n", source_file, strerror(errno));
      return false;
   }
   struct stat st_dest;
   if (stat(dest_file, &st_dest) != 0 && st_dest.st_ino == st_source.st_ino) {
      fprintf(stderr, "cp: '%s' and '%s' are the same file\n", source_file, dest_file);
      return false;
   }

   switch (st_source.st_mode & S_IFMT) {
   case S_IFLNK:  return copy_link(dest_file, source_file);
   case S_IFDIR:  return copy_dir(dest_file, source_file, &st_source);
   case S_IFREG: return fcopy(dest_file, source_file, &st_source);
   default:
      fputs("cp: special files are not supported\n", stderr);
      return false;
   }
   return false;
}

int main(int argc, char* argv[]) {
   int option;
   while ((option = getopt(argc, argv, ":fHiLPpR")) != -1) {
      switch (option) {
      case 'f': force = true; break;
      case 'i': prompt = true; break;
      case 'H':
      case 'L':
      case 'P': opt_HLP = option; break;
      case 'p': umask(0); break;
      case 'R': recursive = true; break;
      default: goto print_usage;
      }
   }
   if ((argc - optind) < 2) {
   print_usage:
      fputs("Usage: cp [-Pfip] source_file target_file\n", stderr);
      fputs("       cp [-Pfip] source_file... target\n", stderr);
      fputs("       cp -R [-H|-L|-P] [-fip] source_file... target\n", stderr);
      return 1;
   }
   const char* target = argv[argc - 1];
   const size_t len_target = strlen(target);
   struct stat st_target;
   int error = stat(target, &st_target) ? errno : 0;
   
   if (((argc - optind) > 2 && error)) {
      fprintf(stderr, "cp: failed to access target '%s': %s\n", target, strerror(errno));
      return 1;
   }
   if ((argc - optind) == 2) {
      const char* source_file = argv[optind];
      return !copy(target, source_file);
      if (error == ENOENT) return !copy(target, source_file);
      else {
         char* dest_file = (char*)malloc(len_target + PATH_MAX + 2);
         if (!dest_file) return perror("cp: failed to allocate buffer"), 1;
         memcpy(dest_file, target, len_target);
         dest_file[len_target] = '/';
         strncpy(dest_file + len_target + 1, source_file, PATH_MAX);
         initial = true;
         const int ec = !copy(dest_file, source_file);
         free(dest_file);
         return ec;
      }
   }


   char* dest_file = (char*)malloc(len_target + PATH_MAX + 2);
   if (!dest_file) return perror("cp: failed to allocate buffer"), 1;
   memcpy(dest_file, target, len_target);
   dest_file[len_target] = '/';

   int ec = 0;
   for (; optind < (argc - 1); ++optind) {
      const char* source_file = argv[optind];
      strncpy(dest_file + len_target + 1, source_file, PATH_MAX);
      initial = true;
      if (!copy(dest_file, source_file)) ec = 1;
   }
   free(dest_file);
   return ec;
}
