
#include "cli.h"

struct clivars {
  int rays;
  char *model_file;
  char *output_file;
};

int cli() 
{
  cliopt("model\t\tModel file") {
    printf("model: '%s'\n",cliarg);
    clivars->model_file = cliarg;
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

  int argn = cli("CLI Demo v.01 - (c) SoftySoft",&vars);

  vars.rays = 1;

  fprintf(stderr,"rays: %d\n", vars.rays);
  fprintf(stderr,"model: %s\n", vars.model_file);
  fprintf(stderr,"Args left: %d\n", argc - argn);
}
