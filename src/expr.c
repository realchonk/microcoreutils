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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>
#include <errno.h>
#include "config.h"

#if !HAVE_STRCOLL
#define strcoll strcmp
#endif

static int narg;
static int argc;
static char** argv;

static size_t num_digits(long num) {
   size_t i = 0;
   if (num < 0) {
      num = -num;
      ++i;
   }
   while (num > 0) {
      num /= 10;
      ++i;
   }
   return i;
}
static const char* to_str(long num) {
   const size_t len = num_digits(num) + 1;
   char* buffer = malloc(len);
   if (!buffer) {
      fprintf(stderr, "expr: malloc(%zu) failed: %s\n", len, strerror(errno));
      exit(3);
   }
   snprintf(buffer, len, "%ld", num);
   return buffer;
}

struct expr {
   const char* str;
   long num;
   bool has_num;
};

static void try_parse(struct expr* expr) {
   char* endp;
   expr->num = strtol(expr->str, &endp, 10);
   expr->has_num = *endp == '\0';
}
static bool peek(struct expr* expr) {
   if (narg >= argc)
      return false;
   expr->str = argv[narg];
   try_parse(expr);
   return true;
}
static void skip(void) {
   ++narg;
}
static bool get(struct expr* expr) {
   if (!peek(expr))
      return false;
   skip();
   return true;
}

static bool parse_expr(struct expr*);
static bool paren(struct expr* expr) {
   struct expr tmp;
   if (!get(&tmp))
      return false;
   if (!strcmp(tmp.str, "(")) {
      if (!parse_expr(expr))
         return false;
      if (!get(&tmp) || !!strcmp(tmp.str, ")"))
         return false;
      else skip();
      return true;
   } else {
      *expr = tmp;
      return true;
   }
}
static bool match(struct expr* expr) {
   struct expr expr1, expr2, tmp;
   if (!paren(&expr1))
      return false;
   if (!peek(&tmp) || !!strcmp(tmp.str, ":")) {
      *expr = expr1;
      return true;
   }
   skip();
   if (!paren(&expr2))
      return false;
   regex_t regex;
   if (regcomp(&regex, expr2.str, 0))
      return false;
   regmatch_t matches[2];
   const char* text = expr1.str;
   int ec = regexec(&regex, text, 2, matches, 0);
   regfree(&regex);
   if (ec == REG_NOMATCH) {
      expr->has_num = true;
      expr->num = 0;
      expr->str = "0";
      return true;
   } else if (ec)
      return false;
   if (matches[1].rm_so != -1) {
      const regmatch_t backref = matches[1];
      const size_t len = backref.rm_eo - backref.rm_so + 1;
      char* buf = malloc(len);
      if (!buf) {
         fprintf(stderr, "expr: malloc(%zu) failed: %s\n", len, strerror(errno));
         exit(3);
      }
      strncpy(buf, text + backref.rm_so, len);
      buf[len - 1] = '\0';
      expr->str = buf;
      try_parse(expr);
   } else {
      const regmatch_t match = matches[0];
      expr->has_num = true;
      expr->num = match.rm_eo - match.rm_so;
      expr->str = to_str(expr->num);
   }
   return true;
}
static bool multiplicative(struct expr* expr) {
   struct expr expr1, expr2, tmp;
   if (!match(&expr1))
      return false;
   if (!peek(&tmp) || !(!strcmp(tmp.str, "*")
      || !strcmp(tmp.str, "/") || !strcmp(tmp.str, "%"))) {
      *expr = expr1;
      return true;
   }
   skip();
   if (!match(&expr2) || !expr1.has_num || !expr2.has_num)
      return false;
   const char op = *tmp.str;
   expr->has_num = true;
   switch (op) {
   case '*':
      expr->num = expr1.num * expr2.num;
      break;
   case '/':
      expr->num = expr1.num / expr2.num;
      break;
   case '%':
      expr->num = expr1.num % expr2.num;
      break;
   default:
      return false;
   }
   expr->str = to_str(expr->num);
   return true;
}
static bool additive(struct expr* expr) {
   struct expr expr1, expr2, tmp;
   if (!multiplicative(&expr1))
      return false;
   if (!peek(&tmp) || !(!strcmp(tmp.str, "+") || !strcmp(tmp.str, "-"))) {
      *expr = expr1;
      return true;
   }
   skip();
   if (!multiplicative(&expr2) || !expr1.has_num || !expr2.has_num)
      return false;
   const char op = *tmp.str;
   expr->has_num = true;
   switch (op) {
   case '+':
      expr->num = expr1.num + expr2.num;
      break;
   case '-':
      expr->num = expr1.num - expr2.num;
      break;
   default:
      return false;
   }
   expr->str = to_str(expr->num);
   return true;
}
static int cmp_op(const char* str) {
   if (!strcmp(str, "="))        return 1;
   else if (!strcmp(str, "!="))  return 2;
   else if (!strcmp(str, ">"))   return 3;
   else if (!strcmp(str, ">="))  return 4;
   else if (!strcmp(str, "<"))   return 5;
   else if (!strcmp(str, "<="))  return 6;
   else                          return 0;
}
static bool cmp(struct expr* expr) {
   struct expr expr1, expr2, op;
   if (!additive(&expr1))
      return false;
   int opi;
   if (!peek(&op) || !(opi = cmp_op(op.str))) {
      *expr = expr1;
      return true;
   }
   skip();
   if (!additive(&expr2))
      return false;
   long diff;
   if (expr1.has_num && expr2.has_num)
      diff = expr1.num - expr2.num;
   else diff = strcoll(expr1.str, expr2.str);
   switch (opi) {
   case 1:  expr->num = (diff == 0); break;
   case 2:  expr->num = (diff != 0); break;
   case 3:  expr->num = (diff >  0); break;
   case 4:  expr->num = (diff >= 0); break;
   case 5:  expr->num = (diff <  0); break;
   case 6:  expr->num = (diff <= 0); break;
   default: return false;
   }
   expr->has_num = true;
   expr->str = (expr->num ? "1" : "0");
   return true;
}
static bool eq2zero(const struct expr* e) {
   return e->has_num && !e->num;
}
static bool booland(struct expr* expr) {
   struct expr expr1, expr2, tmp;
   if (!cmp(&expr1))
      return false;
   if (!peek(&tmp) || !!strcmp(tmp.str, "&")) {
      *expr = expr1;
      return true;
   }
   skip();
   if (!cmp(&expr2))
      return false;
   const long e1 = !eq2zero(&expr1);
   const long e2 = !eq2zero(&expr2);
   if (e1 && e2) {
      *expr = expr1;
   } else {
      expr->has_num = true;
      expr->num = 0;
      expr->str = "0";
   }
   return true;
}
static bool boolor(struct expr* expr) {
   struct expr expr1, expr2, tmp;
   if (!cmp(&expr1))
      return false;
   if (!peek(&tmp) || !!strcmp(tmp.str, "|")) {
      *expr = expr1;
      return true;
   }
   skip();
   if (!cmp(&expr2))
      return false;
   const long e1 = !eq2zero(&expr1);
   const long e2 = !eq2zero(&expr2);
   if (e1) {
      *expr = expr1;
   } else if (e2) {
      *expr = expr2;
   } else {
      expr->has_num = true;
      expr->num = 0;
      expr->str = "0";
   }
   return true;
}
static bool parse_expr(struct expr* expr) {
   return boolor(expr);
}

int main(int a, char** b) {
   narg = 1;
   argc = a;
   argv = b;
   struct expr expr;
   if (!parse_expr(&expr)) {
      fputs("expr: invalid expression\n", stderr);
      return 2;
   }
   puts(expr.str);
   return !eq2zero(&expr);
}
