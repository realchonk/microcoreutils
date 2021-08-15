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

#include <signal.h>
#include <unistd.h>
#include <stdio.h>

static void sigint_handler(int sig) {
   (void)sig;
}

static FILE* open_file(const char* path, int append) {
   if (append) {
      FILE* file = fopen(path, "a");
      if (!file) file = fopen(path, "w");
      return file;
   }
   else return fopen(path, "w");
}

int main(int argc, char* argv[]) {
   int append = 0;
   int ignore_sigint = 0;
   int option;
   while ((option = getopt(argc, argv, ":ai")) != -1) {
      switch (option) {
      case 'a': append = 1; break;
      case 'i': ignore_sigint = 1; break;
      case '?':
         puts("Usage: tee [-ai] [file...]");
         break;
      }
   }
   if (ignore_sigint) signal(SIGINT, sigint_handler);
   const int num_files = argc - optind;
   FILE* files[num_files];
   int ec = 0;
   for (int i = 0; i < num_files; ++i) {
      files[i] = open_file(argv[optind + i], append);
      if (!files[i]) ec = 1;
   }
   char ch;
   while ((ch = getchar()) != EOF) {
      putchar(ch);
      for (int i = 0; i < num_files; ++i) {
         if (files[i]) fputc(ch, files[i]);
      }
   }
   for (int i = 0; i < num_files; ++i) {
      if (files[i]) fclose(files[i]);
   }
   return ec;
}
