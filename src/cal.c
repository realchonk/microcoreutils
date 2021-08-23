#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// TODO: make it understand localization options (LC_*)

static const char* months[12] = {
   "January",
   "February",
   "March",
   "April",
   "May",
   "June",
   "July",
   "August",
   "September",
   "Oktober",
   "November",
   "December",
};

static int is_leap_year(int y) {
   if (y < 1752) return y % 4 == 0;
   else if (y % 400 == 0) return 1;
   else if (y % 100 == 0) return 0;
   else return y % 4 == 0;
}
static int weekday(int d, int m, int y) {
   const int ym100 = y % 100;
   const int yc = (ym100 + (ym100 / 4)) % 7;
   const int mc = "033614625035"[m] - '0';
   const int cent = y / 100;
   int cc;
   bool lyc;
   if (y < 1752 || (y == 1752 && m < 9)) {
      cc = (18 - cent) % 7;
      if (y % 400 == 0) lyc = 1;
      else if (y % 100 == 0) lyc = 0;
      else lyc = y % 4 == 0;
   } else {
      cc = 6 - (cent % 4) * 2;
      lyc = y % 4 == 0;
   }
   return (yc + mc + cc + d - lyc) % 7;
}

static int days_per_month(int m, int y) {
   return m == 1 ? 28 + is_leap_year(y) : 30 + (m <= 6 ? !(m & 1) : m & 1);
}

static void print_offset(const char* s, int tot, bool trailing) {
   const int len = strlen(s);
   const int off = tot / 2 - len / 2;
   printf("%*s%s", off, "", s);
   if (trailing)
      printf("%*s", tot - (off + len), "");
}

static int print_row(int d, int m, int y) {
   const int dpm = days_per_month(m, y);
   if (d > dpm) {
      printf("%*s", 21, "");
      return 0;
   }
   const int wd1 = weekday(d, m, y);
   printf("%*s", wd1 * 3, "");
   int i;
   for (i = 0; i < (7 - wd1) && (i + d <= dpm); ++i) {
      printf("%2d ", i + d);
   }
   const int wdi = weekday(d + i - 1, m, y);
   if (wdi != 6) {
      printf("%*s", (6 - wdi) * 3, "");
   }
   return i + d; 
}

static void print_month(int m, int y) {
   print_offset(months[m], 16, false);
   printf(" %d\n", y);
   puts("Su Mo Tu We Th Fr Sa");
   
   if (m != 9 && y != 1752) {
      int d = 1;
      for (int i = 0; i < 6 && d >= 0; ++i) {
         d = print_row(d, m, y);
         putchar('\n');
      }
   } else {
      puts("    1  2 14 15 16 17");
      puts("18 19 20 21 22 23 24");
      puts("25 26 27 28 29 30   \n\n");
   }
}

static void print_year(int y) {
   printf("%31s%04d\n\n", "", y);
   for (int i = 0; i < 4; ++i) {
      const int m = i * 3;
      print_offset(months[m + 0], 20, true);
      printf("   ");
      print_offset(months[m + 1], 20, true);
      printf("   ");
      print_offset(months[m + 2], 20, false);
      putchar('\n');
      int d[3];
      d[0] = d[1] = d[2] = 1;
      puts("Su Mo Tu We Th Fr Sa   Su Mo Tu We Th Fr Sa   Su Mo Tu We Th Fr Sa");
      for (int i = 0; i < 6; ++i) {
         d[0] = print_row(d[0], m + 0, y);
         printf("  ");
         d[1] = print_row(d[1], m + 1, y);
         printf("  ");
         d[2] = print_row(d[2], m + 2, y);
         putchar('\n');
      }
   }
}

static void print_usage(void) {
   fputs("Usage: cal [[month] year]\n", stderr);
}

// Usage: cal [[month] year]
int main(int argc, char** argv) {
   tzset();
   if (argc == 1) {
      const time_t t = time(NULL);
      struct tm* tm = localtime(&t);
      print_month(tm->tm_mon, tm->tm_year + 1900);
      return 0;
   } else if (argc == 2) {
      char* endp = NULL;
      const long y = strtol(argv[1], &endp, 10);
      if (*endp || y < 0)
         return print_usage(), 1;
      print_year(y);
      return 0;
   } else if (argc == 3) {
      char* endp = NULL;
      const long m = strtol(argv[1], &endp, 10);
      if (*endp || m < 1 || m > 12)
         return print_usage(), 1;
      const long y = strtol(argv[2], &endp, 10);
      if (*endp)
         return print_usage(), 1;
      print_month(m - 1, y);
      return 0;
   } else {
      print_usage();
      return 1;
   }
}
