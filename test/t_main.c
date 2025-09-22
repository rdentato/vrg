#include "cli.h"

int main(int argc, char **argv) {
  clioptions() {
    cliopt("-f [filename]") {
      printf("%s, %p\n", cliarg, cliarg);
    }
    cliopt() {}
  }
  return 0;
}
