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

#define PROG_NAME "ls"

#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include "errprintf.h"
#include "common.h"
#include "buf.h"


static int(*sorter)(const void*, const void*) = NULL;
static int show_hidden = 0, list = 0, follow_links = 0, print_slash = 0, reverse_sort = 0, print_inode = 0;

struct ls_result {
   char* path;
   const char* name; // points to (path + n)
   struct stat st;
};
static int sort_none(const void* p1, const void* p2) {
   (void)p1;
   (void)p2;
   return -1;
}
static int sort_name(const void* p1, const void* p2) {
   const struct ls_result* e1 = p1;
   const struct ls_result* e2 = p2;
   const int r = strcmp(e1->name, e2->name);
   return reverse_sort ? -r : r;
}
static int sort_t(const void* p1, const void* p2) {
   const struct ls_result* e1 = p1;
   const struct ls_result* e2 = p2;
   int r = e1->st.st_mtime - e2->st.st_mtime;
   if (r == 0) r = strcmp(e1->name, e2->name);
   return reverse_sort ? -r : r;
}
static int sort_S(const void* p1, const void* p2) {
   const struct ls_result* e1 = p1;
   const struct ls_result* e2 = p2;
   int r = e1->st.st_size - e2->st.st_size;
   if (r == 0) r = strcmp(e1->name, e2->name);
   return reverse_sort ? -r : r;
}
static int sort_c(const void* p1, const void* p2) {
   const struct ls_result* e1 = p1;
   const struct ls_result* e2 = p2;
   int r = e1->st.st_ctime - e2->st.st_ctime;
   if (r == 0) r = strcmp(e1->name, e2->name);
   return reverse_sort ? -r : r;
}
static int sort_u(const void* p1, const void* p2) {
   const struct ls_result* e1 = p1;
   const struct ls_result* e2 = p2;
   int r = e1->st.st_atime - e2->st.st_atime;
   if (r == 0) r = strcmp(e1->name, e2->name);
   return reverse_sort ? -r : r;
}

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
   mode[1] = st->st_mode & S_IRUSR ? 'r' : '-';
   mode[2] = st->st_mode & S_IWUSR ? 'w' : '-';
   mode[3] = "-xSs"[!!(st->st_mode & S_IXUSR) + (!!(st->st_mode & S_ISUID) << 1)];
   mode[4] = st->st_mode & S_IRGRP ? 'r' : '-';
   mode[5] = st->st_mode & S_IWGRP ? 'w' : '-';
   mode[6] = "-xSs"[!!(st->st_mode & S_IXGRP) + (!!(st->st_mode & S_ISGID) << 1)];
   mode[7] = st->st_mode & S_IROTH ? 'r' : '-';
   mode[8] = st->st_mode & S_IWOTH ? 'w' : '-';
   mode[9] = "-xTt"[!!(st->st_mode & S_IXOTH) + ((((st->st_mode & S_IFMT) == S_IFDIR) && (st->st_mode & S_ISVTX)) << 1)];
   mode[10] = '\0';
}

static void print_entry(const char* name, struct stat* st, const char* path) {
   if (print_inode) printf("%ju ", (uintmax_t)st->st_ino);
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

      struct tm tm = *localtime(&st->st_atime);
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
         const ssize_t new_len = readlink(path, link_target, lt_len);
         if (new_len <= 0) {
            perror("Failed to readlink");
            return;
         }
         link_target[new_len] = '\0';
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
}


static struct ls_result* do_ls(const char* path) {
   struct ls_result trs;
   struct ls_result* result = NULL;
   struct stat st;
   if (stat(path, &st) != 0) {
      errprintf("cannot access '%s'", path);
      return NULL;
   }
   if (is_directory(&st)) {
      const size_t len = strlen(path);
      DIR* dir;
      struct dirent* ent;
      struct stat new_st;
      char* buffer = (char*)malloc(len + 260);
      if (!buffer) {
         errprintf("failed to allocate buffer");
         return NULL;
      }
      if ((dir = opendir(path)) == NULL) {
         free(buffer);
         errprintf("failed to access '%s'", path);
         return NULL;
      }
      while ((ent = readdir(dir)) != NULL) {
         if (!show_hidden && ent->d_name[0] == '.')
            continue;
         if ((strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) && show_hidden != 'a') continue;
         memcpy(buffer, path, len);
         buffer[len] = '/';
         strncpy(buffer + len + 1, ent->d_name, 0 + sizeof(ent->d_name)); // '0 + ...' to suppress warnings on gcc
         int ec;
         if (follow_links) ec = stat(buffer, &new_st);
         else ec = lstat(buffer, &new_st);
         if (ec != 0) {
            errprintf("failed to access '%s'", buffer);
            return NULL;
         }
         trs.st = new_st;
         trs.path = strdup(buffer);
         trs.name = basename(trs.path);
         buf_push(result, trs);
      }
      free(buffer);
      closedir(dir);
   } else {
      trs.st = st;
      trs.path = strdup(path);
      trs.name = basename(trs.path);
      buf_push(result, trs);
   }
   return result;
}

static bool do_ls2(const char* path) {
   struct ls_result* entries = do_ls(path);
   if (!entries) return false;
   const size_t len = buf_len(entries);
   qsort(entries, len, sizeof(struct ls_result), sorter);
   for (size_t i = 0; i < len; ++i) {
      print_entry(entries[i].name, &entries[i].st, entries[i].path);
      free(entries[i].path);
   }
   buf_free(entries);
   return true;
}

// TODO: add additional options
// see: https://pubs.opengroup.org/onlinepubs/9699919799/utilities/ls.html

int main(int argc, char* argv[]) {
   int option;
   sorter = sort_name;
   while ((option = getopt(argc, argv, ":1likqrsAaCmx1FpHLRdSftcu")) != -1) {
      switch (option) {
      case '1': break;
      case 'A':
      case 'a': show_hidden = option; break;

      case 'i': print_inode = 1; break;

      case 'l': list = 1; break;
      case 'H': follow_links = 0; break;
      case 'L': follow_links = 1; break;

      case 'p': print_slash = 1; break;
        
      case 'r': reverse_sort = 1; break;
      case 'f': show_hidden = 'a'; sorter = sort_none; break;
      case 't': sorter = sort_t; break;
      case 'S': sorter = sort_S; break;
      case 'c': sorter = sort_c; break;
      case 'u': sorter = sort_u; break;

      case '?': goto print_usage;
      default:
         fprintf(stderr, "ls: the '-%c' option is currently not supported!\n", option);
         return 1;
      }
   }
   
   if (optind == argc) return do_ls2(".");

   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      if (!do_ls2(path)) ec = 1;
   }      
   return ec;
print_usage:
   fputs("Usage: ls [-ir] [-l] [-a|-A] [-1] [-p] [-H|-L] [-S|-f|-t|-c|-u] [file...]\n", stderr);
   return 1;
}

