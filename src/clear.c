#include <unistd.h>

int main(void) {
   // TODO: implement proper clearing with terminfo
   write(STDOUT_FILENO, "\e[3J\033c", 6);
   return 0;
}
