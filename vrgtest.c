/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

// Define VRGCLI in one (and only one) of the source files that include vrg.h
#define VRGCLI

#include "vrg.h"

int main(int argc, char *argv[])
{
   
  // vrgcli() {...} encloses the actions to perform when flags and arguments are encountered specified
  // It takes as argument an optional informative message on the program (version, copyright ....)
  // that will be printed by vrgusage()
  vrgcli("vrg test program (c) 2022 by me") {

    // Each flag or argument is a string divided in two parts seprated by tab (\t).
    // The first part is the flag or argument name, the second one is the description.

    // The `vrgarg()` macro encloses the code that must be executed when a flag is found
    // Within the code, the `vrgarg` variable holds a pointer to the argument itself
    // (or the flag option).

    // -h takes no op
    vrgarg("-h\tPrint help") {
      // This code will be executed if the flag is specified.
      printf("You specified the '-h' flag with option '%s'\n",vrgarg);

      // This `vrgusage` function will print the list of flags and arguments in the same
      // order they are specified within `vrgcli` and then exit with an error.
      // You can optionally specify an error message to print before the Usage using 
      // the same syntx as `printf()`.
      vrgusage();
    }

    // -o takes an optional option. You can tell it's optional because it is
    // enclosed in brackets: [...]
    vrgarg("-o [optional]\tYou may or may not specify an option for '-o'") {
      // The variables vrglen and vrgargarg will help you get the flag argument.
      printf("You specified the '-o' flag with option '%s'\n",vrgarg);
    }

    // -m takes a mandatory argument. No brackets here, if the argument is missing 
    // an error will be printed.
    vrgarg("-m mandatory\tYou must specify an argument for '-m'") {
      // You can specify the flag as `-m PIPPO` or `-mPIPPO`
      printf("You specified the '-m' flag with option '%s'\n",vrgarg);
    }
  
    vrgarg("--\tEnd of vrgcli managed arguments") {
      // A simple break allows you to exit the cli handling
      // If you exit this way, no check on mandatory arguments
      // will be performed.
      break;  
    }

    // Positional arguments are usually defined after the flags but
    // you can specify them in whichever order you prefer on the command line.

    // This is the first positional order and it is mandatory
    // If a mandatory argument is not specified on the command line, 
    // an error will be printed.
    vrgarg("inputfile\tThe name of the file to process") {
      printf("File to be processed: `%s`\n",vrgarg);
    }

    // This is the second positional order and it is optional
    vrgarg("[outputfile]\tThe name of the output file") {
      printf("File to be created: `%s`\n",vrgarg);
    }

    // If no argment is passed, this code will be executed if none of the
    // previous ones are. This is useful to detect unexpected flags or 
    // multiple positional arguments at the end.
    // By default, unknown flag are just ignored
    vrgarg() {
      if (vrgarg[0] == '-') {
        // This will print the error message, the usage and will exit with an error
        vrgusage("ERROR: unknown flag '-%c'\n",vrgarg[1]);
      }
      
      fprintf(stderr,"Additional argument: '%s'\n",vrgarg);
    }
 
  }

  // The variable vrgargn will tell you if there are other arguments to be 
  // processed in the command line.
  if (vrgargn < argc) {
    printf("You have additional arguments:\n");
    for (int k = vrgargn; k < argc; k++) 
      printf(" %-2d '%s'\n", k, argv[k]);
  }
}