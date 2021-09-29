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

#define PROG_NAME "kill"

#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#include "errprintf.h"

struct signal_mapping {
   int signal;
   char name[16];
};


#define sig(n) { SIG##n, #n }
static struct signal_mapping signals[] = {
   sig(ABRT),
   sig(ALRM),
   sig(BUS),
   sig(CHLD),
   sig(CONT),
   sig(FPE),
   sig(HUP),
   sig(ILL),
   sig(INT),
   sig(KILL),
   sig(PIPE),
   sig(QUIT),
   sig(SEGV),
   sig(STOP),
   sig(TERM),
   sig(TSTP),
   sig(TTIN),
   sig(TTOU),
   sig(USR1),
   sig(USR2),
#ifdef SIGPOLL
   sig(POLL),
#endif
#ifdef SIGPROF
   sig(PROF),
#endif
#ifdef SIGSYS
   sig(SYS),
#endif
   sig(TRAP),
   sig(URG),
#ifdef SIGVTALRM
   sig(VTALRM),
#endif
   sig(XCPU),
   sig(XFSZ),
   { -1, "" },
};
#undef sig

static bool strnieq(const char* s1, const char* s2, size_t num) {
   for (size_t i = 0; i < num; ++i) {
      if (tolower(s1[i]) != tolower(s2[i])) return false;
      else if (!s1[i]) return true;
   }
   return true;
}

static int find_signal(const char* name) {
   size_t len = strlen(name);
   if (strcmp(name, "0") == 0) return 0;
   if (len > 3 && strnieq(name, "SIG", 3)) name += 3, len -= 3;
   for (size_t i = 0; signals[i].signal != -1; ++i) {
      if (strnieq(signals[i].name, name, -1)) return signals[i].signal;
   }
   return -1;
}

int main(int argc, char* argv[]) {
   if (argc <= 1) {
      fputs("Usage: kill [-s sigspec] pid... or kill -l [sigspec]\n", stderr);
      return 1;
   }
   int option;
   int sig = SIGTERM;
   while ((option = getopt(argc, argv, ":l:s:")) != -1) {
      switch (option) {
      case 'l': {
         char* endp;
         sig = (int)strtol(optarg, &endp, 10);
         if (*endp) {
            sig = find_signal(optarg);
            if (sig == -1) {
               unknown_signal_name:
               fprintf(stderr, "kill: unknown signal name '%s'\n", optarg);
               return 1;
            }
            printf("%d\n", sig);
            return 0;
         }
         if (sig == 0) goto unknown_signal_name;
         for (size_t i = 0; signals[i].signal != -1; ++i) {
            if (sig == signals[i].signal) {
               puts(signals[i].name);
               return 0;
            }
         }
         goto unknown_signal_name;
         
         return 0;
      }
      case 's':
         sig = find_signal(optarg);
         if (sig == -1) {
            fprintf(stderr, "kill: invalid signal '%s'\n", optarg);
            return 1;
         }
         break;
      case ':':
         if (optopt == 'l') {
            for (size_t i = 0; signals[i].signal != -1; ++i) {
               puts(signals[i].name);
            }
            return 0;
         } else {
            fprintf(stderr, "kill: option -%c requires an argument\n", optopt);
            return 1;
         }
         break;
      default: return 1;
      }
   }
   int ec = 0;
   for (; optind < argc; ++optind) {
      char* endp;
      const pid_t pid = (pid_t)strtol(argv[optind], &endp, 10);
      if (*endp != '\0') {
         fprintf(stderr, "kill: failed to parse pid: '%s'\n", argv[optind]);
         return 1;
      }
      if (kill(pid, sig) < 0) {
         errprintf("failed to kill process '%s'", argv[optind])
         ec = 1;
         continue;
      }
   }
   return ec;
}
