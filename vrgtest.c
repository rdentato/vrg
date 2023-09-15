/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

// Define VRGMAIN in one (and only one) of the source files that include vrg.h
#define VRGMAIN

#include "vrg.h"

int main(int argc, char *argv[])
{
   // This is an informative message on the program (version, copyright ....)
   // that will be printed by vrgusage()
   vrgversion = "vrg test program (c) 2022 by me";

   // vrgargions() {...} encloses the actions to perform when an switch is specified
   vrgswitch(argc,argv) {

     // Each switch is a string divided in two parts seprated by tab (\t).
     // The first one is the switch, the second one is the description.

     // -h takes no option
     vrgcase("h\tPrint help") {
      // This code will be executed if the switch is specified.
      printf("You specified the '-h' switch with option '%s'\n",vrgoption);
     }

     // -o takes an optional option. You can tell it's optional because it is
     // enclosed in brackets: [...]
     vrgcase("o [optional]\tYou may or may not specify an option for '-o'") {
       // The variables vrglen and vrgargarg will help you get the switch argument.
       printf("You specified the '-o' switch with option '%s'\n",vrgoption);
     }

     // -m takes a mandatory argument. No brackets here, if the argument is missing 
     // an error will be printed.
     vrgcase("m mandatory\tYou must specify an argument for '-m'") {
      // You can specify the switch as `-m PIPPO` or `-mPIPPO`
      printf("You specified the '-m' switch with option '%s'\n",vrgoption);
     }

     vrgdefault {
      // Here you can deal with unknown argions. You may generate an error or just skip it
      // If no vrgdefault is 
      fprintf(stderr,"WARNING: unknown switch -%c\n",argv[vrgargn][1]);
     }
 
   }

   // The variable vrgargn will tell you if there are other arguments to be 
   // processed in the command line.
   if (vrgargn < argc) {
     printf("You have additional arguments:\n");
     for (int k = vrgargn; k < argc; k++) 
       printf(" %-2d '%s'\n", k, argv[k]);
   }
   
   // This function will print the list of switches and exit with an error.
   // Note that they will appear in reverse order.
   vrgusage();
}