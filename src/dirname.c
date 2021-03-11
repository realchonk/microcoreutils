#include <string.h>
#include <stdio.h>

// For documentation see: https://pubs.opengroup.org/onlinepubs/9699919799/utilities/dirname.html

static int is_slash(const char* str, size_t len) {
   for (size_t i = 0; i < len; ++i) {
      if (str[i] != '/') return 0;
   }
   return 1;
}

int main(int argc, char* argv[]) {
   if (argc != 2) {
      fputs("Usage: dirname string\n", stderr);
      return 1;
   }
   char* str = argv[1];
   const size_t len = strlen(str);
   char* end = &str[len - 1];
   if (strcmp(str, "//") == 0) goto step_6;
   if (is_slash(str, len)) {
      puts("/");
      return 0;
   }

   while (*end == '/') *end-- = '\0';

   if (strchr(str, '/') == NULL) {
      puts(".");
      return 0;
   }

   while (*end != '/') *end-- = '\0';
step_6:
   while (*end == '/') *end-- = '\0';
   if (*str == '\0') puts("/");
   else puts(str);
   return 0;
}
