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
   char* line = readline(stdin);
   enum ed_mode mode = ED_NORMAL;
   if (feof(stdin) || strcmp(line, "q") == 0) {
      buf_free(line);
      return ED_EXIT;
   } else if (*line == 'w') {
      char* s = line + 1;
      if (*s == 'q') mode = ED_EXIT, ++s;
      s = skip_ws(s);
      if (*s) {
         if (!write_file(s, *buffer, data->suppress)) puts("?"), mode = ED_NORMAL;
      } else {
         if (!data->filename || !write_file(data->filename, *buffer, data->suppress))
            puts("?"), mode = ED_NORMAL;
      }
      buf_free(line);
      return mode;
   }

   // parse numbers
   


   else if (strcmp(line, "a") == 0) {
      data->insert.line = buf_len(*buffer);
      mode = ED_INSERT;
   }
   else puts("?");
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
   if (data.filename) {
      FILE* file = fopen(data.filename, "r");
      if (file) {
         size_t num = 0;
         while (!feof(file)) {
            char* line = readline(file);
            buf_push(buffer, line);
            num += buf_len(line);
         }
         fclose(file);
         if (!data.suppress) printf("%zu\n", num - 1);
      }
      else if (!data.suppress) fprintf(stderr, "ed: %s: %s\n", data.filename, strerror(errno));
   }


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
