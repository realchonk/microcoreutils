#ifndef FILE_CLEARENV_H
#define FILE_CLEARENV_H
#include <string.h>
#include <stdlib.h>

#if !defined(__linux__)

extern char** environ;
static void clearenv(void) {
   while (environ && *environ) {
      const char* env = *environ;
      char* name = (char*)malloc(strlen(env));
      size_t i;
      for (i = 0; env[i] != '='; ++i) {
         name[i] = env[i];
      }
      name[i] = '\0';
      unsetenv(name);
      free(name);
   }
}

#endif

#endif /* FILE_CLEARENV_H */
