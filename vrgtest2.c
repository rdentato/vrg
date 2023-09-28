
#define VRGCLI
#include "vrg.h"

// CHeck if the string is an integer
int isint(char *arg)
{
  if (arg == NULL || *arg == '\0') return 0;
  for(;*arg; arg++) if (*arg < '0' || '9' < *arg) return 0;
  return 1;
}

// Check if the string is an integer greater than n
int isgreaterthan(char *arg, int n)
{
  if (arg == NULL || *arg == '\0') return 0;
  if (atoi(arg)<= n) return 0;
  return 1;
}

// Check if is a boolean option any string starting with ('1', 'Y', 'y', 'T', 't') 
// is considered true, any string starting with ('0', 'N', 'n', 'F', 'f') is 
// considered false.
int isbool(char *arg) 
{
  if (arg == NULL || *arg == '\0') return 0;
  if (strchr("1TtYy",*arg)) return 1;
  if (strchr("0FfNn",*arg)) return 1;
  return 0;
}

// check if the specified file exists and is readable.
int isfile(char *arg) 
{
  if (arg == NULL || *arg == '\0') return 0;
  FILE *f = fopen(arg,"rb");
  if (f == NULL) return 0;
  fclose(f);
  return 1;
}
char *usage_en="# Usage\n";

int main (int argc, char *argv[])
{

  vrgcli("version: 1.3RC\n  Demo for the vrgli function") {

    vrgarg("model\tModel file",isfile) {
      printf("model: '%s'\n",vrgarg);
    }

    vrgarg("-h\tPrint this help") {
      vrgusage();
    }

    vrgarg("-n rays\tNumber of rays (positive integer)", isgreaterthan, 0) {
      printf("Rays: '%s'\n",vrgarg);
    }

    vrgarg("-t [trace]\tTrace (boolean)", isbool) {
      printf("Trace '%s'\n",vrgarg);
    }

    vrgarg("-p") {
      printf("practice '%s'\n",vrgarg);
    }

    vrgarg("-r\tRetrace") {
      printf("retrace '%s'\n",vrgarg);
    }

    vrgarg("[outfile]\tOutput file") {
      printf("outfile: '%s'\n",vrgarg);
    }

    vrgarg() {
      if (vrgarg[0] == '-') vrgerror("Unknown option '-%c'\n",vrgarg[1]);
      else printf("Other argument %s\n",vrgarg);
    }

  }

  fprintf(stderr,"Args left: %d\n", argc - vrgargn);

}
