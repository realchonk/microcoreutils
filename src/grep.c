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

#define PROG_NAME "grep"

#include "config.h"
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>
#include "errprintf.h"
#include "openfile.h"
#include "buf.h"

static char* grep_readline(FILE* file) {
   char* buf = NULL;
   int ch;
   while ((ch = fgetc(file)) != EOF) {
      if (ch == '\n') {
         buf_push(buf, '\0');
         break;
      }
      buf_push(buf, ch);
   }
   if (!buf)
      return NULL;
   char* str = strdup(buf);
   buf_free(buf);
   return str;
}
static char* readfile(const char* path) {
   FILE* file = openfile_in(path);
   if (!file) {
      errprintf("%s", path);
      return NULL;
   }
   char* buf = NULL;
   int ch;
   while ((ch = fgetc(file)) != EOF) {
      buf_push(buf, ch);
   }
   if (buf[buf_len(buf)-1] == '\n') {
      buf[buf_len(buf)-1] = '\0';
   } else {
      buf_push(buf, '\0');
   }
   char* str = strdup(buf);
   buf_free(buf);
   return str;
}

enum grep_mode {
   GREP_BRE,
   GREP_ERE,
   GREP_FIX,
};

struct grep_pattern {
   bool is_file;
   const char* str;
   regex_t regex;
};

static bool print_linenum = false;
static bool suppress_err = false;
static bool inverse = false;
static int pmode = 'p';
static struct grep_pattern* patterns = NULL;
static char*(*fix_strstr)(const char*, const char*) = NULL;
static bool(*match_func)(const char*) = NULL;

#if !HAVE_STRCASESTR
static char* my_strcasestr(const char* hay, const char* needle) {
   const size_t len_hay = strlen(hay);
   const size_t len_needle = strlen(needle);
   if (len_needle > len_hay)
      return NULL;

   const size_t len = len_hay - len_needle;

   // `<=` is intended. 
   for (size_t i = 0; i <= len; ++i) {
      if (strcasecmp(hay + i, needle) == 0)
         return (char*(hay + i);
   }
   return NULL;
}
#endif

static bool regex_match(const char* line) {
   for (size_t i = 0; i < buf_len(patterns); ++i) {
      if (regexec(&patterns[i].regex, line, 0, NULL, 0) == 0)
         return true;
   }
   return false;
}

static bool fix_match(const char* line) {
   for (size_t i = 0; i < buf_len(patterns); ++i) {
      if (fix_strstr(line, patterns[i].str))
         return true;
   }
   return false;
}

static bool run_grep(FILE* file, const char* filename, bool print_filename) {
   size_t num_matched = 0;

   size_t linenum = 0;
   char* line;

   bool(*f)(const char*);

   while ((line = grep_readline(file)) != NULL) {
      ++linenum;

      const bool line_matched = match_func(line);

      if (line_matched ^ inverse) {
         ++num_matched;
         switch (pmode) {
         case 'p':
            if (print_filename)
               printf("%s:", filename);

            if (print_linenum)
               printf("%zu:", linenum);

            puts(line);
            break;
         case 'l':
            puts(filename);
            return true;
         case 'q':
            return true;
         case 'c':
            break;
         }
      }

   }

   if (pmode == 'c') {
      if (print_filename)
         printf("%s:", filename);
      printf("%zu\n", num_matched);
   }

   return num_matched != 0;
}

int main(int argc, char* argv[]) {
   bool icase = false;
   enum grep_mode rmode = GREP_BRE;
   struct grep_pattern gp;
   int ec;

   char* basename = strrchr(argv[0], '/');
   if (!basename)
      basename = argv[0];
   
   if (!strcmp(basename, "egrep")) {
      rmode = GREP_ERE;
   } else if (!strcmp(basename, "fgrep")) {
      rmode = GREP_FIX;
   }

   int option;
   while ((option = getopt(argc, argv, ":EFclqinsvxe:f:")) != -1) {
      switch (option) {
      case 'E':
         rmode = GREP_ERE;
         break;
      case 'F':
         rmode = GREP_FIX;
         break;
      case 'c':
      case 'l':
      case 'q':
         pmode = option;
         break;
      case 'i':
         icase = true;
         break;
      case 's':
         suppress_err = true;
         break;
      case 'v':
         inverse = true;
         break;
      case 'x':
         fputs("grep: unimplemented option '-x'\n", stderr);
         return 2;
      case 'e':
         gp.is_file = false;
         gp.str = optarg;
         buf_push(patterns, gp);
         break;
      case 'f':
         gp.is_file = true;
         gp.str = optarg;
         buf_push(patterns, gp);
         break;
      case ':':
         fprintf(stderr, "grep: expected argument for '-%c'\n", optopt);
         return 2;
      case '?':
         fprintf(stderr, "grep: invalid option '-%c'\n", optopt);
         return 2;
      }
   }

   if (!patterns) {
      if ((argc - optind) > 0) {
         gp.is_file = false;
         gp.str = argv[optind++];
         buf_push(patterns, gp);
      } else {
         fputs("grep: expected pattern argument.\n", stderr);
         return 2;
      }
   }

   if (rmode == GREP_FIX) {
      if (icase) {
         fix_strstr = strcasestr;
      } else {
         fix_strstr = strstr;
      }
      match_func = fix_match;
   } else {
      match_func = regex_match;
      int cflags = 0;
      if (rmode == GREP_ERE) {
         cflags |= REG_EXTENDED;
      }
      if (icase) {
         cflags |= REG_ICASE;
      }

      // Compile regex patterns
      for (size_t i = 0; i < buf_len(patterns); ++i) {
         struct grep_pattern* p = &patterns[i];
         if (p->is_file) {
            char* text = readfile(p->str);
            if (!text) {
               for (size_t j = 0; j < i; ++j) {
                  regfree(&patterns[j].regex);
               }
               return 2;
            }
            ec = regcomp(&p->regex, text, cflags);
            free(text);
         } else {
            ec = regcomp(&p->regex, p->str, cflags);
         }
         if (ec != 0) {
            const size_t num = regerror(ec, &p->regex, NULL, 0);
            char* buf = malloc(num);
            regerror(ec, &p->regex, buf, num);
            fprintf(stderr, "grep: %s\n", buf);
            free(buf);
            for (size_t j = 0; j < i; ++j) {
               regfree(&patterns[j].regex);
            }
            return 2;
         }
      }
   }

   if ((argc - optind) == 0) {
      --optind;
      argv[optind] = "-";
   }

   const bool print_filenames = (argc - optind) > 1;

   ec = 1;
   for (; optind < argc; ++optind) {
      FILE* file = openfile_in(argv[optind]);
      if (!file) {
         ec = 2;
         if (!suppress_err)
            errprintf("%s", argv[optind]);
         continue;
      }

      const char* filename;
      if (!strcmp(argv[optind], "-")) {
         filename = "(standard input)";
      } else {
         filename = argv[optind];
      }
      
      const bool matched = run_grep(file, filename, print_filenames);
      closefile(file);
      if (matched) {
         if (ec == 1 || pmode == 'q')
            ec = 0;
         if (pmode != 'p' && pmode != 'c')
            break;
      }
   }

   if (rmode != GREP_FIX) {
      // Free regexes
      for (size_t i = 0; i < buf_len(patterns); ++i) {
         regfree(&patterns[i].regex);
      }
   }

   return ec;
}
