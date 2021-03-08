#include <stdio.h>

struct count_result {
   
};

// TODO: finish this program
int main(int argc, char* argv[]) {
   if (argc < 2) {
      puts("Usage: wc [-c|-m] [-lw] [file...]");
      return 1;
   }

   int option;
   while ((option = getopt(argc, argv, ":cmlw")) != -1) {
      switch (option) {
      
      }
   }
}
