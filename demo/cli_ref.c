#include "cli.h"
#include <stdio.h>
#include <stdlib.h>

static char *is_positive(char *s) {
  char *end; long v = strtol(s, &end, 10);
  if (*end) return "Not an integer";
  return (v > 0) ? NULL : "Must be > 0";
}

int main(int argc, char **argv) {
  int verbose=0, xrays=0, temp=0, adding = -1;
  const char *in=NULL, *out=NULL;

  clioptions("demo tool", argc, argv) {
    cliopt("-h\t\t\t\tShow help") { cliusage(CLIEXIT); }

    cliopt("-v, --verbose\t\t\tIncrease verbosity") { verbose++; }

    cliopt("-x, --xrays n\t\t\tNumber of rays", is_positive) { xrays = atoi(cliarg); }

    cliopt("-T, --temperature temp (42)\tSet temperature (default 42)") {
      int t = atoi(cliarg);
      if (cliisdefault()) temp = t;      // initialization path
      else                temp = t;      // user override path (same here)
    }

    cliopt("<add> item\tAdd a single item") {
      /* ... handle "add" ... */
      adding = clindx;
    }

    cliopt("input\t\tInput file") { in = cliarg; }
    cliopt("[output]\tOutput file") { out = cliarg; }

    cliopt() {                            // unknown/extra
      if (cliarg[0] == '-') clierror("Unknown option", cliarg);
      // else: tolerate extra positionals or stop with cliexit()
    }
  }

  printf("add='%s' verbose=%d xrays=%d temp=%d in='%s' out='%s'\n",
         adding>=0?argv[adding]:"(none)", verbose, xrays, temp, in?in:"(none)", out?out:"(none)");
  return 0;

}