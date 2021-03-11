#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

struct count_result {
   size_t words;
   size_t lines;
   size_t bytes;
};

static struct count_result count(FILE* file) {
   struct count_result r = { 0 };
   char ch, last_ch = '\0';
   while ((ch = fgetc(file)) != EOF) {
      if (isspace(ch) && !isspace(last_ch))
         ++r.words;
      if (ch == '\n') ++r.lines;
      ++r.bytes;
      last_ch = ch;
   }
   return r;
}

#define print(x) { if (i == 0) { i = 1; printf("%zu", (x)); } else printf(" %zu", (x)); }
#define print_result(r) \
   if (opt_l) print(r.lines); \
   if (opt_w) print(r.words); \
   if (opt_c || opt_m) print(r.bytes);

int main(int argc, char* argv[]) {
   int opt_c = 0, opt_m = 0, opt_l = 0, opt_w = 0;
   int option;
   while ((option = getopt(argc, argv, ":cmlw")) != -1) {
      switch (option) {
      case 'c':   opt_c = 1; break;
      case 'm':   opt_m = 1; break;
      case 'l':   opt_l = 1; break;
      case 'w':   opt_w = 1; break;
      default:    goto print_usage;
      }
   }
   if ((opt_c + opt_m + opt_l + opt_w) == 0)
      opt_l = opt_w = opt_m = 1;
   int i = 0;
   if (optind == argc) {
      struct count_result r = count(stdin);
      print_result(r);
      putchar('\n');
      return 0;
   }
   const int print_total = (argc - optind) > 1;
   int ec = 0;
   struct count_result total = { 0 };
   for (; optind < argc; ++optind) {
      i = 0;
      const char* path = argv[optind];
      FILE* file;
      if (strcmp(path, "-") == 0) file = stdin;
      else file = fopen(path, "r");
      if (!file) {
         fprintf(stderr, "wc: failed to open '%s': %s\n", path, strerror(errno));
         ec = 1;
         continue;
      }
      const struct count_result r = count(file);
      print_result(r);
      printf(" %s\n", path);
      if (file != stdin) fclose(file);

      total.lines += r.lines;
      total.words += r.words;
      total.bytes += r.bytes;
   }

   if (print_total) {
      i = 0;
      print_result(total);
      puts(" total");
   }

   return ec;
print_usage:
   puts("Usage: wc [-c|-m] [-lw] [file...]");
   return 1;
}
