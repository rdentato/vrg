
#include "cli.h"

typedef struct {
  int rays;
  char *model_file;
  char *output_file;
} vars_t;

char *check_positive(char *s)
{
  int n = atoi(s);
  return ((n > 0) ? NULL : "Positive value expected for") ;  
}

int prgcli(vars_t *vars, int prg_argc, char *prg_argv[])
{

  clioptions("My xcli program (C) 2025 by me",prg_argc,prg_argv) {
    cliopt("-h, --help\t\t\t\tShow help") {
      cliusage(CLIEXIT);
    }

    cliopt("-x, --xray num-rays ($XRAYS,32)\tNumber of ray tracing along x", check_positive) {
      cli_trace("-x %s (%d)", cliarg, clindx);
      int rays = atoi(cliarg);
      vars->rays = rays;
    }

    cliopt("-y, --yray [num-rays]\t\t\tNumber of ray tracing along y (or optimize if missing)") {
      cli_trace("-X %s (%d)", cliarg, clindx);
    }

    cliopt("-z\t\t\t\t\tCompress output") {
      cli_trace("-z %s (%d)", cliarg, clindx);
    }

    cliopt("--compress type\t\t\t\tCompress output (type: lzh, zip, gz, z)") {
      cli_trace("--compress %s (%d)", cliarg, clindx);
    }

    cliopt("datafile\t\tThe input file") {
      cli_trace("datafile: %s (%d)", cliarg, clindx);
      vars->model_file = cliarg;
    }
    
    cliopt("[outfile]  (x.out)\tThe file containing results") {
      cli_trace("outfile: %s (%d)", cliarg, clindx);
      vars->output_file = cliarg;
    }

    cliopt() {
      cli_trace("Ignored: '%s'",cliarg);  
    }
  }
  return clindx;
}

int main (int argc, char *argv[])
{
  vars_t vars = {0};

  vars.rays = 1;

  int argn = prgcli(&vars, argc, argv);

  fprintf(stderr,"rays: %d\n", vars.rays);
  fprintf(stderr,"model: %s\n", vars.model_file);
  fprintf(stderr,"outfile: %s\n", vars.output_file);
  fprintf(stderr,"Args left: %d\n", argc - argn);
}
