#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include "buf.h"

// Simple ED clone
// TODO: finish this program

static char* skip_ws(char* s) {
   while (isspace(*s)) ++s;
   return s;
}

enum ed_mode {
   ED_NORMAL,
   ED_INSERT,
   ED_EXIT,
};
struct ed_data {
   const char* filename;
   const char* prompt;
   int suppress;
   struct {
      size_t line;
   } insert;
};

static char* readline(FILE* file) {
   char* line = NULL;
   char ch;
   while ((ch = fgetc(file)) != EOF) {
      if (ch == '\n') break;
      else buf_push(line, ch);
   }
   buf_push(line, '\0');
   return line;
}
static bool write_file(const char* filename, char** buffer, int suppress) {
   FILE* file = fopen(filename, "w");
   if (!file) {
      if (!suppress) fprintf(stderr, "%s: %s\n", filename, strerror(errno));
      return false;
   }
   int num = 0;
   for (size_t i = 0; i < buf_len(buffer); ++i) {
      num += fprintf(file, "%s\n", buffer[i]);
   }
   fclose(file);
   if (!suppress) fprintf(stderr, "%d\n", num);
   return true;
}
static bool read_file(const char* filename, char*** buffer, int suppress) {
   FILE* file = fopen(filename, "r");
   if (!file) {
      if (!suppress) fprintf(stderr, "ed: %s: %s\n", filename, strerror(errno));
      return false;
   }
   size_t num = 0;
   char* line = NULL;
   char ch;
   while ((ch = fgetc(file)) != EOF) {
      if (ch == '\n') buf_push(line, '\0'), buf_push(*buffer, line), line = NULL;
      else buf_push(line, ch);
      ++num;
   }
   fclose(file);
   if (line) buf_push(line, '\0'), buf_push(*buffer, line);
   if (!suppress) printf("%zu\n", num);
   return true;
}



static enum ed_mode ed_insert(struct ed_data* data, char*** buffer) {
   char* line = readline(stdin);
   if (strcmp(line, ".") == 0) {
      buf_free(line);
      return ED_NORMAL;
   }
   buf_insert(*buffer, data->insert.line, line);
   ++data->insert.line;
   return ED_INSERT;
}
static enum ed_mode ed_normal(struct ed_data* data, char*** buffer) {
   printf("%s", data->prompt);
   fflush(stdout);
   char* line = readline(stdin);
   enum ed_mode mode = ED_NORMAL;
   char* s  = line;
   if (feof(stdin) || strcmp(line, "q") == 0) {
      buf_free(line);
      return ED_EXIT;
   }
   s = skip_ws(s);
   if (*s == '#') goto end;
   else if (*s == 'w') {
      ++s;
      if (*s == 'q') mode = ED_EXIT, ++s;
      s = skip_ws(s);
      if (*s) {
         if (!write_file(s, *buffer, data->suppress)) puts("?"), mode = ED_NORMAL;
      } else {
         if (!data->filename || !write_file(data->filename, *buffer, data->suppress))
            puts("?"), mode = ED_NORMAL;
      }
      goto end;
   }

   // parse numbers
   size_t first_num = buf_len(*buffer), second_num = buf_len(*buffer);

   if (*s == '$') {
      first_num = buf_len(*buffer);
      ++s;
   } else if (isdigit(*s)) {
      first_num = 0;
      while (isdigit(*s)) first_num = first_num * 10 + (*s++ - '0');
      if (first_num == 0) { puts("?"); goto end; }
   }
   s = skip_ws(s);
   if (*s == ',') {
      ++s;
      if (*s == '$') {
         ++s;
         second_num = buf_len(*buffer);
      } else if (isdigit(*s)) {
         second_num = 0;
         while (isdigit(*s)) second_num = second_num * 10 + (*s++ - '0');
      } else second_num = first_num;
      if (second_num == 0) { puts("?"); goto end; }
   } else second_num = first_num;

   if (first_num > second_num || second_num > buf_len(*buffer)) { puts("?"); goto end; }

   switch (*s) {
   case 'p':
      if (!buf_len(*buffer)) { puts("?"); goto end; }
      for (size_t i = first_num; i <= second_num; ++i) {
         puts((*buffer)[i - 1]);
      }
      break;
   case 'a':
      data->insert.line = first_num;
      mode = ED_INSERT;
      break;
   case 'i':
      data->insert.line = first_num - 1;
      mode = ED_INSERT;
      break;
   case 'd':
      buf_remove(*buffer, first_num - 1, second_num - first_num + 1);
      break;
   case 'c':
      buf_remove(*buffer, first_num - 1, 1);
      data->insert.line = first_num - 1;
      mode = ED_INSERT;
      break;
   default:
      puts("?");
      break;
   }
end:
   buf_free(line);
   return mode;
}

int main(int argc, char* argv[]) {
   struct ed_data data;
   data.filename = NULL;
   data.prompt = "";
   data.suppress = 0;
   int option;
   while ((option = getopt(argc, argv, "sp:")) != -1) {
      switch (option) {
      case 's': data.suppress = 1; break;
      case 'p': data.prompt = optarg; break;
      default:  return 1;
      }
   }
   const int narg = argc - optind;
   if (narg > 1) {
      fputs("Usage: ed [-p string] [-s] [file]\n", stderr);
      return 1;
   }
   else if (narg == 1) data.filename = argv[optind];

   char** buffer = NULL;
   if (data.filename) read_file(data.filename, &buffer, data.suppress);


   enum ed_mode mode = ED_NORMAL;

   while (1) {
      switch (mode) {
      case ED_NORMAL:
         mode = ed_normal(&data, &buffer);
         break;
      case ED_INSERT:
         mode = ed_insert(&data, &buffer);
         break;
      case ED_EXIT: goto end;
      }
   }
end:
   return 0;
}
