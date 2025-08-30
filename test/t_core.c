
#include "cli.h"

struct clivars {
  int rays;
  char *model_file;
  char *output_file;
};

int cli() 
{
  cliopt("-h, --help\t\tPrint this help") {
    cliusage();
  }

  cliopt("--num-rays -n rays\tNumber of rays (positive integer)") {
    printf("Rays: '%s'\n",cliarg);
    clivars->rays = atoi(cliarg);
    if (clivars->rays <= 0) {
      climsg("ERROR: Argument for -n must be positive");
      cliusage();
    }
  }

  cliopt("--practice") {
    printf("practice '%s'\n",cliarg);
  }

  cliopt("--endargs, -e\tIgnore next args") {
    printf("endargs %d\n",clindx);
    return;
  }

  cliopt("-p [pippo]") {
    printf("P '%s'\n",cliarg);
  }

  cliopt("-r\t\t\tRetrace") {
    printf("retrace '%s'\n",cliarg);
  }

  cliopt("model\t\tModel file") {
    printf("model: '%s'\n",cliarg);
    clivars->model_file = cliarg;
  }

  cliopt("[outfile]\t\tOutput file") {
    printf("outfile: '%s'\n",cliarg);
  }

  cliopt() {
    if (cliarg[0] == '-') {
      climsg("Unknown option '%s' (ignored)",cliarg);
    }
    else {
      climsg("Additional arguments %d",clindx);
    }
  }
}

int main (int argc, char *argv[])
{
  struct clivars vars = {0};

  vars.rays = 1;

  int argn = cli("CLI Demo v.01 - (c) SoftySoft",&vars);

  fprintf(stderr,"rays: %d\n", vars.rays);
  fprintf(stderr,"model: %s\n", vars.model_file);
  fprintf(stderr,"Args left: %d\n", argc - argn);
}
