#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static void cat(FILE* file) {
	char ch;
	while ((ch = fgetc(file)) != EOF) putchar(ch);
}

int main(int argc, char* argv[]) {
	if (argc == 1) return cat(stdin), 0;
	int ec = 0;
	for (int i = 1; i < argc; ++i) {
		FILE* file;
      if (strcmp("-", argv[i]) == 0) file = stdin;
      else file = fopen(argv[i], "r");
		if (!file) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], argv[i], strerror(errno));
			ec = 1;
			continue;
		}
		cat(file);
		fclose(file);
	}
	return ec;
}
