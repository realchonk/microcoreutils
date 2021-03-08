#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

static void head(FILE* file, unsigned n) {
   unsigned i = 0;
   char ch;
   while (i < n) {
      ch = fgetc(file);
      if (ch == EOF) break;
      else if (ch == '\n') ++i;
      putchar(ch);
   }
}

int main(int argc, char* argv[]) {
   unsigned long n = 10;
   int option;
   while ((option = getopt(argc, argv, ":n:")) != -1) {
      switch (option) {
      case 'n':
         n = strtoul(optarg, NULL, 10);
         break;
      case ':':
         puts("head: '-n' requires an argument");
         break;
      }
   }
   if (optind == argc) return head(stdin, n), 0;
   int ec = 0;
   for (; optind < argc; ++optind) {
      const char* path = argv[optind];
      FILE* file = NULL;
      if (strcmp(path, "-") == 0) file = stdin;
      else file = fopen(path, "r");
      if (!file) {
         printf("head: failed to open '%s': %s\n", path, strerror(errno));
         ec = 1;
         continue;
      }
      head(file, n);
      if (file != stdin) fclose(file);
   }
   return ec;
}
