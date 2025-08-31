
#include "cli.h"

typedef struct {
  int rays;
  char *model_file;
  char *output_file;
 } vars_t;

int mycli(int argc, char **argv, vars_t *vars) 
{

  clioptions("CLI Demo v.01 - (c) SoftySoft") {
    cliopt("-h, --help\t\tPrint this help") {
      cliusage();
    }
  
    cliopt("-n | --num-rays rays\tNumber of rays (positive integer)") {
      printf("Rays: '%s'\n",cliarg);
      vars->rays = atoi(cliarg);
      if (vars->rays <= 0) {
        clierror("Argument for -n must be positive",cliarg);
        cliusage();
      }
    }
  
    cliopt("--practice") {
      printf("practice '%s'\n",cliarg);
    }
  
    cliopt("-e --endargs\t\tIgnore next args") {
      printf("endargs %d\n",clindx);
      return clindx;
    }
  
    cliopt("-p [pippo]") {
      printf("P '%s'\n",cliarg);
    }
  
    cliopt("-r\t\t\tRetrace") {
      printf("retrace '%s'\n",cliarg);
    }
  
    cliopt("model\t\tModel file") {
      printf("model: '%s'\n",cliarg);
      vars->model_file = cliarg;
    }
  
    cliopt("[outfile]\t\tOutput file") {
      printf("outfile: '%s'\n",cliarg);
    }
  
    cliopt() {
      if (cliarg[0] == '-') {
        fprintf(stderr,"Unknown option '%s' (ignored)\n",cliarg);
      }
      else {
        fprintf(stderr,"Additional arguments %d\n",argc-clindx);
      }
    }
  }
  return clindx;
}

int main (int argc, char *argv[])
{
  vars_t vars = {0};

  vars.rays = 1;

  int argn = mycli(argc, argv, &vars);

  fprintf(stderr,"rays: %d\n", vars.rays);
  fprintf(stderr,"model: %s\n", vars.model_file);
  fprintf(stderr,"Args left: %d\n", argc - argn);
}
