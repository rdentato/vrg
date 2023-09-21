
#define VRGMAIN
#include "vrg.h"

int isint(char *arg)
{
  if (arg == NULL || *arg == '\0') return 0;
  for(;*arg; arg++) if (*arg < '0' || '9' < *arg) return 0;
  return 1;
}

int isgreaterthan(char *arg, int n)
{
  if (arg == NULL || *arg == '\0') return 0;
  if (atoi(arg)<= n) return 0;
  return 1;
}

int isbool(char *arg) 
{
  if (arg == NULL || *arg == '\0') return 0;
  if (strchr("1TtYy",*arg)) return 1;
  if (strchr("0FfNn",*arg)) return 1;
  return 0;
}

int isfile(char *arg) 
{
  if (arg == NULL || *arg == '\0') return 0;
  FILE *f = fopen(arg,"rb");
  if (f == NULL) return 0;
  fclose(f);
  return 1;
}


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

    vrgarg("-r\tRetrace") {
      printf("r '%s'\n",vrgarg);
    }

    vrgarg("[outfile]\tOutput file") {
      printf("outfile: '%s'\n",vrgarg);
    }

    vrgarg() {
      if (vrgarg[0] == '-') vrgusage("Error: Unknown option '-%c'\n",vrgarg[1]);
      else printf("Other argument %s\n",vrgarg);
    }

  }

  fprintf(stderr,"Args left: %d\n", argc - vrgargn);

}