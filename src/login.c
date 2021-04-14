#define _GNU_SOURCE
#include <sys/utsname.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include "common.h"
#include "clearenv.h"

#if defined(__linux__)
#include <shadow.h>
#include <crypt.h>
#endif

static bool check_passwd(struct passwd* user, const char* pwd) {
   if (!user) return false;
   if (!user->pw_passwd || !*user->pw_passwd) return true;
#if defined(__linux__)
   if (strcmp(user->pw_passwd, "x") == 0) {
      struct spwd* shadow = getspnam(user->pw_name);
      if (!shadow) return false;
      else return strcmp(shadow->sp_pwdp, crypt(pwd, shadow->sp_pwdp)) == 0;
   } else
#endif
   return strcmp(user->pw_passwd, crypt(pwd, user->pw_passwd)) == 0;
}

static void fix_env(const struct passwd* pw) {
   clearenv();
   const size_t len_name = strlen(pw->pw_name);
   char* user = (char*)malloc(6 + len_name);
   memcpy(user, "USER=", 5);
   memcpy(user + 5, pw->pw_name, len_name);
   user[5 + len_name] = '\0';
   putenv(user);

   const size_t len_home = strlen(pw->pw_dir);
   char* home = (char*)malloc(6 + len_home);
   memcpy(home, "HOME=", 5);
   memcpy(home + 5, pw->pw_dir, len_home);
   home[5 + len_home] = '\0';
   putenv(home);

   const size_t len_shell = strlen(pw->pw_shell);
   char* shell = (char*)malloc(7 + len_shell);
   memcpy(shell, "SHELL=", 6);
   memcpy(shell + 6, pw->pw_shell, len_shell);
   shell[6 + len_shell] = '\0';
   putenv(shell);
}

// TODO: implement password-hiding on input

int main(void) {
   if (geteuid() != 0) {
      fputs("login: must be superuser.\n", stderr);
      return 1;
   }
   const char* hostname = "localhost";
   struct utsname un;
   if (uname(&un) == 0) hostname = un.nodename;
   struct passwd* passwd;
   char* user;
   char* pwd;
begin:
   while (1) {
      if (feof(stdin)) return 0;
      printf("%s login: ", hostname);
      fflush(stdout);
      user = readline(stdin);
      if (!user) continue;
      passwd = getpwnam(user);
      
      if (passwd && !*passwd->pw_passwd) break;
      fputs("Password: ", stdout);
      fflush(stdout);
      pwd = readline(stdin);

      if (check_passwd(passwd, pwd)) break;
      puts("\nLogin incorrect");

      buf_free(user);
      buf_free(pwd);
   }
   buf_free(user);
   buf_free(pwd);
   bool failed;
   if (passwd->pw_uid != getuid()) {
#if defined(__linux__) || defined(__FreeBSD__)
      failed = setresgid(passwd->pw_gid, passwd->pw_gid, passwd->pw_gid) != 0 || initgroups(passwd->pw_name, passwd->pw_gid) != 0 || setresuid(passwd->pw_uid, passwd->pw_uid, passwd->pw_uid) != 0;
#else
      failed = setreuid(passwd->pw_uid, passwd->pw_uid) != 0 || setregid(passwd->pw_gid, passwd->pw_gid) != 0;
#endif
      if (failed) {
         fprintf(stderr, "login: failed to setuid or setgid: %s\n", strerror(errno));
         goto begin;
      }
   }
   fix_env(passwd);
   execl(passwd->pw_shell, "-", NULL);
   fprintf(stderr, "login: failed to launch login shell: %s\n", strerror(errno));
   goto begin;
}
