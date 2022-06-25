/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

// Define VRG_MAIN in one (and only one) of the source files that include vrg.h
#define VRG_MAIN
#include "vrg.h"

int main(int argc, char *argv[])
{
   // This is an informative message on the program (version, copyright ....)
   vrgver("vrg test program (c) 2022 by me");

   // vrgoptions() {...} encloses the actions to perform when an option is specified
   vrgoptions(argc,argv) {

     // Each option is a string divided in two parts seprated by tab (\t).
     // The first one is the option, the second one is the description.

     // -h takes no argument
     vrgopt("-h\tPrint help") {
      // This code will be executed if the option is specified.
      printf("You specified the '-h' option with argument '%.*s'\n",vrglen,vrgoptarg);
     }

     // -o takes an optional argument. You can tell it's optional because it is
     // enclosed in brackets: [...]
     vrgopt("-o [optional]\tYou might not specify it") {
       // The variables vrglen and vrgoptarg will help you get the option argument.
       printf("You specified the '-o' option with argument '%.*s'\n",vrglen,vrgoptarg);
     }

     // -m takes a mandatory argument. No brackets here, if the argument is missing 
     // an error will be printed.
     vrgopt("-m mandatory\tYou must have it") {
      // You can specify the option as `-m PIPPO` or `-mPIPPO`
      printf("You specified the '-m' option with argument '%.*s'\n",vrglen,vrgoptarg);
     }

     vrgoptdefault {
      // Here you can deal with unknown options. You may generate an error or just skip it
      fprintf(stderr,"WARNING: ignored option -%c\n",argv[vrgargn][1]);
     }
 
   }

   // The variable vrgargn will tell you if there are other arguments to be 
   // processed in the command line.
   if (vrgargn < argc) {
     printf("You have additional parameters:\n");
     for (int k = vrgargn; vrgargn < argc; k++) 
       printf("  '%s'\n",argv[k]);
   }
   
   // This function will print the list of flags (which is automatically contructed)
   // and exit with an error.
   vrghelp();
}