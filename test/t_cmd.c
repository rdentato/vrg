
#include "cli.h"

int prgcli(int argc, char *argv[])
{

  clioptions("My cmd program (C) 2025 by me") {
    cliopt("-h, --help [topic]\t\tShow help on a specific topic") {
      cli_trace("help %s (%d)", cliarg, clindx);
      cliusage(CLIEXIT);
    }

    cliopt("<add>\t\t\tAdd items") {
      cli_trace("add %s (%d)", cliarg, clindx);
    }

    cliopt("<list> [type]\t\tList items of the specified type") {
      cli_trace("list %s (%d)", cliarg, clindx);
    }

    cliopt("<check> type\t\tCheck if items are of the specified type") {
      cli_trace("check %s (%d)", cliarg, clindx);
    }

    cliopt("-v, --verbose [level]\t\tVerbose") {
      cli_trace("-v %s (%d)", cliarg, clindx);
    }

    cliopt("-x, --xray\t\tVerbose") {
      cli_trace("-x %s (%d)", cliarg, clindx);
    }

    cliopt("-z, --compress type\t\tCompress(zip, lzh, z)") {
      cli_trace("-z %s (%d)", cliarg, clindx);
    }

    cliopt("[item] ... \t\tThe items to process") {
      cli_trace("First item %s (%d)", cliarg, clindx);
    }

    cliopt() {
      cli_trace("Other item: '%s'",cliarg);  
    }
  }
  return clindx;
}

int main (int argc, char *argv[])
{
  int argn = prgcli(argc, argv);
  fprintf(stderr,"Next arg: %d/%d\n",argn,argc);
}
