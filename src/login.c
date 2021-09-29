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

#define PROG_NAME "login"

#include "config.h"
#if HAVE_SETRESUID
#define _GNU_SOURCE
#endif
#include <sys/utsname.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include "errprintf.h"
#include "common.h"
#include "clearenv.h"

#if HAVE_CRYPT_H
#include <crypt.h>
#endif

#if HAVE_SHADOW_H
#include <shadow.h>
#endif

static bool check_passwd(struct passwd* user, const char* pwd) {
   if (!user) return false;
   if (!user->pw_passwd || !*user->pw_passwd) return true;
#if HAVE_SHADOW_H
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
#if HAVE_SETRESUID
      failed = setresgid(passwd->pw_gid, passwd->pw_gid, passwd->pw_gid) != 0 || initgroups(passwd->pw_name, passwd->pw_gid) != 0 || setresuid(passwd->pw_uid, passwd->pw_uid, passwd->pw_uid) != 0;
#else
      failed = setreuid(passwd->pw_uid, passwd->pw_uid) != 0 || setregid(passwd->pw_gid, passwd->pw_gid) != 0;
#endif
      if (failed) {
         errprintf("failed to setuid or setgid");
         goto begin;
      }
   }
   fix_env(passwd);
   execl(passwd->pw_shell, "-", NULL);
   errprintf("failed to launch login shell");
   goto begin;
}
