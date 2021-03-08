#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static void cat(FILE* file) {
	char ch;
	while ((ch = fgetc(file)) != EOF) putchar(ch);
}

int main(int argc, char* argv[]) {
	int ec = 0;
	int has_done = 0;
   for (int i = 1; i < argc; ++i) {
		FILE* file;
      if (strcmp("-u", argv[i]) == 0) continue;
      if (strcmp("-", argv[i]) == 0) file = stdin;
      else file = fopen(argv[i], "r");
		has_done = 1;
      if (!file) {
			fprintf(stderr, "cat: %s: %s\n", argv[i], strerror(errno));
			ec = 1;
			continue;
		}
		cat(file);
		fclose(file);
   }
	if (!has_done) cat(stdin);
	return ec;
}
