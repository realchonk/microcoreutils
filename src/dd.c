#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>

struct dd_options {
   const char* input;
   const char* output;
   size_t ibs;
   size_t obs;
   size_t skip;
   size_t seek;
   size_t count;
   bool has_count;
};

static void print_usage(void) {
   fputs("Usage: dd [if=file] [of=file] [count=num] [bs=expr] [ibs=epxr] [obs=expr] [skip=num] [count=num]\n", stderr);
   exit(1);
}
static bool stroeq_impl(const char* key, size_t len_key, const char* opt) {
   const size_t len_opt = strlen(opt);
   if (len_opt != len_key) return false;
   else return memcmp(key, opt, len_key) == 0;
}
static size_t parse_size(const char* s, const char* opt) {
   size_t sz = 0;
   if (!isdigit(*s)) goto error;
   while (isdigit(*s)) sz = sz * 10 + (*s++ - '0');
   if (*s) {
error:
      fprintf(stderr, "dd: option '%s' expects a positive integer as a value.\n", opt);
      exit(1);
   } else return sz;
}
static size_t parse_expr2(const char** s, const char* opt) {
   size_t sz = 0;
   if (!isdigit(**s)) {
      fprintf(stderr, "dd: option '%s' expects an expression as a value.\n", opt);
      exit(1);
   }
   while (isdigit(**s)) sz = sz * 10 + (*(*s)++ - '0');
   switch (**s) {
   case 'b':   return ++*s, sz * 512;
   case 'k':   return ++*s, sz * 1024;
   default:    return sz;
   }
}
static size_t parse_expr(const char* s, const char* opt) {
   size_t sz = parse_expr2(&s, opt);
   while (*s == 'x') {
      ++s;
      sz *= parse_expr2(&s, opt);
   }
   if (*s) {
      fprintf(stderr, "dd: option '%s' expects an expression as a value.\n", opt);
      exit(1);
   } else return sz;
}

#define stroeq(opt) stroeq_impl(arg, len_key, opt)
static struct dd_options parse_opts(int argc, char* argv[]) {
   struct dd_options opts;
   opts.input = NULL;
   opts.output = NULL;
   opts.ibs = 512;
   opts.obs = 512;
   opts.skip = 0;
   opts.seek = 0;
   opts.count = -1;
   opts.has_count = false;

   for (int i = 1; i < argc; ++i) {
      const char* arg = argv[i];
      if (!*arg || *arg == '-' || *arg == '=') print_usage();
      char* value = strchr(arg, '=');
      if (!value) print_usage();
      const size_t len_key = value - arg;
      ++value;
      if (stroeq("if")) opts.input = value;
      else if (stroeq("of")) opts.output = value;
      else if (stroeq("ibs")) opts.ibs = parse_expr(value, "ibs");
      else if (stroeq("obs")) opts.obs = parse_expr(value, "obs");
      else if (stroeq("bs")) opts.ibs = opts.obs = parse_expr(value, "bs");
      else if (stroeq("skip")) opts.skip = parse_size(value, "skip");
      else if (stroeq("seek")) opts.seek = parse_size(value, "seek");
      else if (stroeq("count")) opts.count = parse_size(value, "count"), opts.has_count = true;
      else print_usage();
   }

   return opts;
}
#undef stroeq

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
int main(int argc, char* argv[]) {
   const struct dd_options opts = parse_opts(argc, argv);
   int in, out, ec = 0;
   if (opts.input) in = open(opts.input, O_RDONLY);
   else in = STDIN_FILENO;
   
   if (in < 0) {
      fprintf(stderr, "dd: failed to open '%s' for read: %s\n", opts.input, strerror(errno));
      return 1;
   }

   if (opts.output) {
      out = open(opts.output, O_WRONLY | O_TRUNC);
      if (out < 0) {
         puts("fail");
         out = open(opts.output, O_WRONLY | O_CREAT, 0666);
      }
   } else out = STDOUT_FILENO;

   if (out < 0) {
      close(in);
      fprintf(stderr, "dd: failed to open '%s' for write: '%s'\n", opts.output, strerror(errno));
      return 1;
   }

   char* buf;
   if (opts.ibs != opts.obs) buf = (char*)malloc(max(opts.ibs, opts.obs) * 2);
   else buf = (char*)malloc(opts.ibs);
   if (!buf) {
      perror("dd: failed to allocate buffer");
      ec = 1;
      goto end;
   }

   ssize_t total_rd = 0, total_wr = 0;
   if (opts.ibs == opts.obs) {
      ssize_t rd;
      do {
         rd = read(in, buf, opts.ibs);
         total_rd += rd;
         total_wr += write(out, buf, rd);
      } while (rd == (ssize_t)opts.ibs);
   } else if (opts.ibs < opts.obs) {
      ssize_t num = 0;
      bool eof = false;
      while (!eof) {
         do {
            const ssize_t rd = read(in, buf + num, opts.ibs);
            total_rd += rd;
            if (rd < (ssize_t)opts.ibs && opts.input) eof = true;
            num += rd;
         } while (num < (ssize_t)opts.obs && !eof);
         const ssize_t wr = write(out, buf, opts.obs);
         total_wr += wr;
         num -= wr;
         memmove(buf, buf + wr, num);
         if (eof) {
            total_wr += write(out, buf, num);
         }
      }
   } else {
      ssize_t num = 0;
      bool eof = false;
      while (!eof) {
         const ssize_t rd = read(in, buf + num, opts.ibs);
         total_rd += rd;
         if (rd < (ssize_t)opts.ibs) eof = true;
         do {
            const ssize_t wr = write(out, buf, opts.obs);
            total_wr += wr;
            num -= wr;
            memmove(buf, buf + wr, num);
         } while (num >= (ssize_t)opts.obs);
         if (eof && num > 0) {
            total_wr += write(out, buf, num);
         }
      }
   }

   printf("total written: %zd\ntotal read: %zd\n", total_wr, total_rd);


end:
   if (opts.input) close(in);
   if (opts.output) close(out);
   return ec;
}





