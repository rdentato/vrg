
#define VRGCLI
#include "vrg.h"

// CHeck if the both -t and -b are specified
int check_t_or_b(char *arg)
{
  static int found = 0;
  if (found == 1) vrgerror("Invalid '%s' argument. Only one between '-t' and '-b' can be specified.\n", arg);

  found = 1;
  return 1;
}

int main (int argc, char *argv[])
{

  vrgcli("version: 1.3RC\n  Demo for the vrgli function") {

    vrgarg("model\tModel file") {
      printf("model: '%s'\n",vrgarg);
    }

    vrgarg("-h, --help\tPrint this help") {
      vrgusage();
    }

    vrgarg("-b, --binary filename\tUse the binary format (mutually exclusive with '-t')",check_t_or_b) {
      printf("Binary format: '%s'\n",vrgarg);
    }

    vrgarg("-t, --text filename\tUse the text format (mutually exclusive with '-b')",check_t_or_b) {
      printf("Text format '%s'\n",vrgarg);
    }

    vrgarg() {
      if (vrgarg[0] == '-') vrgerror("Unknown option '%s'\n",vrgarg);
      else printf("Other argument %s\n",vrgarg);
    }

  }

  fprintf(stderr,"Args left: %d\n", argc - vrgargn);

}
