#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include "buf.h"

static gid_t* getusrgroups(const char* user) {
   FILE* file = fopen("/etc/group", "r");
   if (!file) return NULL;
   const size_t len_user = strlen(user);
   gid_t* gids = NULL;
   char* line = NULL;
   char ch;
   while (1) {
      ch = fgetc(file);
      if (ch == '\n' || ch == EOF) {
         if (!line || *line == '#') goto cont;
         buf_push(line, '\0');
         const char* s = strrchr(line, ':') + 1;
         const char* end = s + strlen(s);
         if (!s || !*s) goto cont;

         const char* tmp;
         do {
            tmp = strchr(s, ',');
            if (!tmp) tmp = end;
            const size_t len_s = tmp - s;
            
            if (len_user == len_s && memcmp(user, s, len_s) == 0) {
               const size_t len_name = strchr(line, ':') - line;
               char* name = strndup(line, len_name);
               struct group* grp = getgrnam(name);
               if (grp) buf_push(gids, grp->gr_gid);
               free(name);
               break;
            }
            s = tmp + 1;
         } while (tmp != end);

         cont:
         buf_free(line);
         if (ch == EOF) break;
      } else buf_push(line, ch);
   }
   fclose(file);
   return gids;
}

static int printgrpinfo(gid_t gid) {
   struct group* grp = getgrgid(gid);
   if (grp == NULL) {
      fprintf(stderr, "id: %s\n", strerror(errno));
      return -1;
   }
   printf("%d(%s)", grp->gr_gid, grp->gr_name);
   return 0;
}

static void printgrpinfo2(gid_t gid) {
   struct group* grp = getgrgid(gid);
   if (grp) printf("%s", grp->gr_name);
   else printf("%u", gid);
}

int main(int argc, char* argv[]) {
   int opt_G = 0, opt_g = 0, opt_n = 0, opt_r = 0, opt_u = 0;
   int option;
   while ((option = getopt(argc, argv, ":Ggunr")) != -1) {
      switch (option) {
      case 'G':   opt_G = 1; break;
      case 'g':   opt_g = 1; break;
      case 'n':   opt_n = 1; break;
      case 'r':   opt_r = 1; break;
      case 'u':   opt_u = 1; break;
      default:    goto print_usage;
      }
   }
   if (opt_g + opt_G + opt_u > 1) goto print_usage;
   if (!(opt_g || opt_G || opt_u) && (opt_r || opt_n)) goto print_usage;
   struct passwd* pw = NULL;
   if (argc == optind) {
      pw = getpwuid(geteuid());
      if (!pw) {
         fprintf(stderr, "id: failed to get info for '%u'\n", geteuid());
         return 1;
      }
   }
   else {
      if (isdigit(argv[optind][0])) {
         char* endp = NULL;
         unsigned long uid = strtoul(argv[optind], &endp, 10);
         if (*endp != '\0') {
            fprintf(stderr, "id: not a valid uid: '%s'\n", argv[optind]);
            return 1;
         }
         pw = getpwuid((uid_t)uid);
      }
      else pw = getpwnam(argv[optind]);
      if (!pw) {
         fprintf(stderr, "id: failed to get info for '%s'\n", argv[optind]);
         return 1;
      }
   }

   if (opt_u) {
      if (opt_n) puts(pw->pw_name);
      else printf("%d\n", pw->pw_uid);
   }
   else if (opt_g) {
      if (opt_n) {
         struct group* grp = getgrgid(pw->pw_gid);
         if (!grp) {
            fprintf(stderr, "id: '%s': %s\n", pw->pw_name, strerror(errno));
            return 1;
         }
         puts(grp->gr_name);
      }
      else printf("%d\n", pw->pw_gid);
   }
   else if (opt_G) {
      gid_t* groups = getusrgroups(pw->pw_name);
      if (!groups) return 0;
      if (opt_n) {
         printgrpinfo2(pw->pw_gid);
         for (size_t i = 0; i < buf_len(groups); ++i) {
            if (groups[i] == pw->pw_gid) continue;
            putchar(' ');
            printgrpinfo2(groups[i]);
         }
      }
      else {
         printf("%u", pw->pw_gid);
         for (size_t i = 0; i < buf_len(groups); ++i) {
            if (groups[i] == pw->pw_gid) continue;
            printf(" %u", groups[i]);
         }
      }
      putchar('\n');
      buf_free(groups);
   }
   else {
      struct group* grp = getgrgid(pw->pw_gid);
      if (!grp) {
         fprintf(stderr, "id: '%s': %s\n", pw->pw_name, strerror(errno));
         return 1;
      }
      gid_t* groups = getusrgroups(pw->pw_name);
      printf("uid=%u(%s) gid=%u(%s) groups=", pw->pw_uid, pw->pw_name, grp->gr_gid, grp->gr_name);
      printgrpinfo(pw->pw_gid);
      for (size_t i = 0; i < buf_len(groups); ++i) {
         if (groups[i] == pw->pw_gid) continue;
         putchar(',');
         printgrpinfo(groups[i]);
      }
      putchar('\n');
      buf_free(groups);
   }

   return 0;
print_usage:
   fputs("Usage: id [user]\n", stderr);
   fputs("       id -G [-n] [user]\n", stderr);
   fputs("       id -g [-nr] [user]\n", stderr);
   fputs("       id -u [-nr] [user]\n", stderr);
   return 1;
}
