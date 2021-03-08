#include <string.h>
#include <stdio.h>

inline static int check_special(const char* s) {
   const char* s2 = strrchr(s, '/');
   if (s2) s = s2 + 1;
   return strcmp(s, "[") == 0;
}

static int do_unary(char op, const char* str) {
   switch (op) {
   case 'n': return *str != '\0';
   case 'z': return *str == '\0';
   default:  return -1;
   }
}

static int do_binary(const char* s1, const char* s2, const char* op) {
   if (strcmp(op, "=") == 0) return strcmp(s1, s2) == 0;

   else return -1;
}

// TODO: finish this program
// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/test.htm

int main(int argc, char* argv[]) {
   const int is_special = check_special(argv[0]);
   if (is_special) {
      if (strcmp(argv[argc - 1], "]") == 0) --argc;
      else {
         fprintf(stderr, "%s: missing ']'\n", argv[0]);
         return 1;
      }
   }
   if (argc == 1) return 1;
   else if (argc == 2) return strlen(argv[1]) == 0 ? 1 : 0;
   else if (argc == 3) {
      if (strcmp(argv[1], "!") == 0) return strlen(argv[2]) == 0 ? 0 : 1;
      int i = do_unary(argv[1][1], argv[2]);
      if (i >= 0) return !i;
      else return 1; // TODO: error handling
   }
   else if (argc == 4) {
      int i = do_binary(argv[1], argv[3], argv[2]);
      if (i >= 0) return !i;
   }
}
