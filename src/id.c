#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>

static int get_groups(const char* user, gid_t gid, gid_t** groups, int* ngroups) {
   *ngroups = 0;
   getgrouplist(user, gid, NULL, ngroups);
   *groups = (gid_t*)malloc(sizeof(gid_t) * *ngroups);
   const int r = getgrouplist(user, gid, *groups, ngroups);
   if (r == -1) fprintf(stderr, "id: failed to get group list for '%s'\n", user);
   return r;
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
      int ngroups;
      gid_t* groups;
      if (get_groups(pw->pw_name, pw->pw_gid, &groups, &ngroups) == -1) return 1;
      if (ngroups == 0) return 0;
      if (opt_n) {
         printgrpinfo2(groups[0]);
         for (int i = 1; i < ngroups; ++i) {
            putchar(' ');
            printgrpinfo2(groups[i]);
         }
      }
      else {
         printf("%u", groups[0]);
         for (int i = 1; i < ngroups; ++i) {
            printf(" %u", groups[i]);
         }
      }
      putchar('\n');
   }
   else {
      struct group* grp = getgrgid(pw->pw_gid);
      if (!grp) {
         fprintf(stderr, "id: '%s': %s\n", pw->pw_name, strerror(errno));
         return 1;
      }
      int ngroups;
      gid_t* groups;
      if (get_groups(pw->pw_name, pw->pw_gid, &groups, &ngroups) == -1) return 1;
      printf("uid=%u(%s) gid=%u(%s) groups=", pw->pw_uid, pw->pw_name, grp->gr_gid, grp->gr_name);
      if (ngroups > 0) {
         printgrpinfo(groups[0]);
         for (int i = 1; i < ngroups; ++i) {
            putchar(',');
            printgrpinfo(groups[i]);
         }
      }
      putchar('\n');
   }

   return 0;
print_usage:
   fputs("Usage: id [user]\n", stderr);
   fputs("       id -G [-n] [user]\n", stderr);
   fputs("       id -g [-nr] [user]\n", stderr);
   fputs("       id -u [-nr] [user]\n", stderr);
   return 1;
}
