#include <unistd.h>

int main(void) {
   // TODO: implement proper clearing with terminfo
   return write(STDOUT_FILENO, "\e[3J\033c", 6) == 6 ? 0 : 1;
}
