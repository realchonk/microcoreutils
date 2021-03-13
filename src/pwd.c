#include <stdlib.h>
#include <stdio.h>

// TODO: add -L, -P options

int main(int argc, char* argv[]) {
   if (argc > 1) {
      fputs("Usage: pwd\n", stderr);
      return 1;
   }
   char* pwd = getenv("PWD");
   if (!pwd) {
      fputs("pwd: PWD environement variable not defined!\n", stderr);
      return 1;
   }
   puts(pwd);
   return 0;
}

