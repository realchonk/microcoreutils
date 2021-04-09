#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include "common.h"

// TODO: add additional options
// see: https://pubs.opengroup.org/onlinepubs/9699919799/utilities/ls.html

static int show_hidden = 0, list = 0, follow_links = 0, print_slash = 0;

static void print_mode(char* mode, const struct stat* st) {
   switch (st->st_mode & S_IFMT) {
   case S_IFBLK: mode[0] = 'b'; break;
   case S_IFCHR: mode[0] = 'c'; break;
   case S_IFDIR: mode[0] = 'd'; break;
   case S_IFIFO: mode[0] = 'p'; break;
   case S_IFLNK: mode[0] = 'l'; break;
   case S_IFREG: mode[0] = '-'; break;
   case S_IFSOCK:mode[0] = 's'; break;
   default:      mode[0] = '?'; break;
   }
   mode[1] = st->st_mode & 0400 ? 'r' : '-';
   mode[2] = st->st_mode & 0200 ? 'w' : '-';
   mode[3] = st->st_mode & 0100 ? 'x' : '-';
   mode[4] = st->st_mode & 0040 ? 'r' : '-';
   mode[5] = st->st_mode & 0020 ? 'w' : '-';
   mode[6] = st->st_mode & 0010 ? 'x' : '-';
   mode[7] = st->st_mode & 0004 ? 'r' : '-';
   mode[8] = st->st_mode & 0002 ? 'w' : '-';
   mode[9] = st->st_mode & 0001 ? 'x' : '-';
   mode[10] = '\0';
}

static int print_entry(const char* name, struct stat* st, const char* path) {
   if (list) {
      char mode[11];
      print_mode(mode, st);

      // add the "total: num" line

      char* owner = NULL;
      char* group = NULL;
      int free_owner = 0, free_group = 0;

      struct passwd* pwd = getpwuid(st->st_uid);
      struct group* grp = getgrgid(st->st_gid);

      if (pwd) owner = pwd->pw_name;
      else {
         owner = (char*)malloc(16);
         snprintf(owner, 15, "%u", st->st_uid);
         free_owner = 1;
      }

      if (grp) group = grp->gr_name;
      else {
         group = (char*)malloc(16);
         snprintf(group, 15, "%u", st->st_gid);
         free_group = 1;
      }

      struct tm tm = *localtime(&st->st_atim.tv_sec);
      //strftime(time, sizeof(time), "%c", tm);
      char* time = asctime(&tm);
      time[strlen(time) - 1] = '\0';

      switch (mode[0]) {
      case 'c':
      case 'b':
         printf("%s %u %s %s %s %s %s\n", mode, (unsigned)st->st_nlink, owner, group, "not supported", time, name);
         break;
      case 'l': {
         const size_t lt_len = st->st_size ? st->st_size : PATH_MAX;
         char* link_target = malloc(lt_len + 1);
         readlink(path, link_target, lt_len);
         link_target[lt_len] = '\0';
         printf("%s %u %s %s %8u %s %s -> %s\n", mode, (unsigned)st->st_nlink, owner, group, (unsigned)st->st_size, time, name, link_target);
         free(link_target);
         break;
      }
      case 'd':
         printf("%s %u %s %s %8u %s %s%s\n", mode, (unsigned)st->st_nlink, owner, group, (unsigned)st->st_size, time, name, print_slash ? "/" : "");
         break;
      default:
         printf("%s %u %s %s %8u %s %s\n", mode, (unsigned)st->st_nlink, owner, group, (unsigned)st->st_size, time, name);
         break;
      }
      if (free_owner) free(owner);
      if (free_group) free(group);
   } else printf("%s%s\n", name, print_slash && (st->st_mode & S_IFMT) == S_IFDIR ? "/" : "");
   return 1;
}



static int do_ls(const char* path) {
   struct stat st;
   if (stat(path, &st) != 0) {
      fprintf(stderr, "ls: cannot access '%s': %s\n", path, strerror(errno));
      return 1;
   }
   if (is_directory(&st)) {
      const size_t len = strlen(path);
      DIR* dir;
      struct dirent* ent;
      struct stat new_st;
      char* buffer = (char*)malloc(len + 260);
      if (!buffer) {
         fprintf(stderr, "ls: failed to allocate buffer: %s\n", strerror(errno));
         return 1;
      }
      if ((dir = opendir(path)) == NULL) {
         free(buffer);
         fprintf(stderr, "ls: failed to access '%s': %s\n", path, strerror(errno));
         return 1;
      }
      while ((ent = readdir(dir)) != NULL) {
         if (!show_hidden && ent->d_name[0] == '.')
            continue;
         if ((strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) && show_hidden != 'a') continue;
         memcpy(buffer, path, len);
         buffer[len] = '/';
         strncpy(buffer + len + 1, ent->d_name, sizeof(ent->d_name));
         int ec;
         if (follow_links) ec = stat(buffer, &new_st);
         else ec = lstat(buffer, &new_st);
         if (ec != 0) {
            fprintf(stderr, "ls: failed to access '%s': %s\n", buffer, strerror(errno));
            return 1;
         }
         if (!print_entry(ent->d_name, &new_st, buffer)) return 1;
      }
      free(buffer);
   } else {
      if (!print_entry(path, &st, path)) return 1;
   }
   return 0;
}

// TODO: Implement sorting

int main(int argc, char* argv[]) {
   int option;
   while ((option = getopt(argc, argv, ":likqrsAaCmx1FpHLRdSftcu")) != -1) {
      switch (option) {
      case 'A':
      case 'a': show_hidden = option; break;

      case 'l': list = 1; break;
      case 'H': follow_links = 0; break;
      case 'L': follow_links = 1; break;

      case 'p': print_slash = 1; break;
         
      case '?': goto print_usage;
      default:
         fprintf(stderr, "ls: the '-%c' option is currently not supported!\n", option);
         return 1;
      }
   }

   if (optind == argc) return do_ls(".");

   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      if (do_ls(path) != 0) ec = 1;
   }      
   return ec;
print_usage:
   fputs("Usage: ls [-l] [-a|-A] [file...]\n", stderr);
   return 1;
}

