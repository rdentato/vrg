
#include "cli.h"

int prgcli(int argc, char *argv[])
{
  clioptions("My tar program (C) 2025 by me") {
    cliopt("-h\t\tShow help") {
      cliusage(CLIEXIT);
    }

    cliopt("-x\t\tExtract") {
      cli_trace("-x %s (%d)", cliarg, clindx);
    }

    cliopt("-v\t\tVerbose") {
      cli_trace("-v %s (%d)", cliarg, clindx);
    }

    cliopt("-z\t\tCompress") {
      cli_trace("-z %s (%d)", cliarg, clindx);
    }

    cliopt("-f tarfile\tInput file") {
      cli_trace("-f %s (%d)", cliarg, clindx);
    }

    cliopt("[out-dir]\tOutput directory") {
      cli_trace("out-dir: %s (%d)", cliarg, clindx);
    }

    cliopt() {
      cli_trace("Ignored: '%s'",cliarg);  
    }
  }
  return clindx;
}

int main (int argc, char *argv[])
{
  int argn = prgcli(argc, argv);
  fprintf(stderr,"Next arg: %d/%d\n",argn,argc);
}
