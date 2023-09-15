/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**
** The technique to handle 0 arguments function has been taken from:
** https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
*/

/*
# Variadic functions

Say you want to define a variadic function with the following prototype:

    myfunc([int a [, char b [, void *c]]])

In other words, you want all arguments to be argional.

Simply, define your function with another name (say `my_func()`) and specify
how it should be called when invoked with 0, 1, 2 or 3 paramenters as shown 
in the example below.

Example:
  
    #include "vrg.h"

    int my_func(int a, char b, void *c);
    
    #define myfunc(...)     vrg(myfunc, __VA_ARGS__)
    #define myfunc01()       my_func(0,'\0',NULL)
    #define myfunc_1(a)      my_func(a,'\0',NULL)
    #define myfunc_2(a,b)    my_func(a,b,NULL)
    #define myfunc_3(a,b,c)  my_func(a,b,c)

Note that the `_1`, `_2`, etc, is appended to the name of the function.
Having 0 argument is a special case and `01` will be appended to the function name.

# Command line argions
 A minimal replacement of getarg.

 Define `VRGARGS` before including `vrg.h` only once, usually in the same source where `main()` is.

  #define VRGARGS
  #include "vrg.h"

... In the code: 

 vrgargions(argc,argv) {  // as received by main

   vrgarg("-f filename\tLoad file") { // explanation of the arg comes after \t
      // Will work both for `-f pippo` and `-fpippo`
      // Set what you need to set
      printf("file: `.*s`",vrglen, vrgargarg);
   }

   vrgarg("-o [filename]\tEnable output (on stdout by default)") {
       // Here filename is argional
   }

   vrgargdefault {
      // cought an unrecognized parameter.
   }
 }
 // At the end, the vrgargn variable contains the index of the argument
 // that is sill to be processed:
 //
 // Example (assuming -f takes an argument):
 //    prg -f fname pluto
 // vrgargn will be 3 pointing to "pluto" (argv[3])
 //

 vrghelp(); // Will print the list of defined argions


***/

#ifndef vrgversionSION
#define vrgversionSION 0x0001002C // 0.1.2-RC

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

// Variadic functions

#define vrg_cnt(_1,_2,_3,_4,_5,_6,_7,_8,_N, ...) _N
#define vrg_argn(...)   vrg_cnt(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define vrg_commas(...) vrg_cnt(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, _)
#define vrg_comma(...) ,
#define vrg_empty_(_0, _1, _2, _3) vrg_commas(vrg_cat5(vrg_empty_, _0, _1, _2, _3))
#define vrg_empty____0 ,
#define vrg_empty(...) vrg_empty_( vrg_commas(__VA_ARGS__)    , vrg_commas(vrg_comma __VA_ARGS__), \
                                   vrg_commas(__VA_ARGS__ ( )), vrg_commas(vrg_comma __VA_ARGS__ ( )) )

#define vrg_cat5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define vrg_cat3_(x,y,z)  x ## y ## z
#define vrg_cat3(x,y,z)   vrg_cat3_(x,y,z)

#define vrg(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)
#define VRG(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)

// Command line argions arguments

#ifdef VRGMAIN

typedef struct vrg_arglist_s {
  char  *def;
  struct vrg_arglist_s *next;
} vrg_argdef_t;

static vrg_argdef_t *vrg_arglist = NULL;

static int   vrgargn;
static char *vrgoption;

static int    vrg_count;
static int    vrg_argc;
static char **vrg_argv=NULL;

static char *vrg_emptystr = "";

static char *vrgversion = NULL;

static void vrgusage();

#define vrgerror(...) (fprintf(stderr,"" __VA_ARGS__), fputc('\n',stderr), vrgusage())

#define vrgswitch(vrg_argc_,vrg_argv_) \
  for (vrgargn = 0, vrg_argc = vrg_argc_, vrg_argv = vrg_argv_, vrg_count = 0, errno = 0 \
      ; vrg_next() \
      ; vrg_count = 0, vrgargn++) 

static int vrg_next()
{
  if (vrgargn == 0) return 1;
  if (vrgargn >= vrg_argc) return 0;

  char *cli_arg = vrg_argv[vrgargn];
  if (cli_arg[0] == '-') {
    if (cli_arg[1] == '-') {vrgargn++; return 0; }
    return 1;
  }
  return 0;
}

#define vrg_argnode() vrg_cat3(vrg_def_, __LINE__,)

#define vrgcase(vrg_arg_) static vrg_argdef_t vrg_argnode() = { .def = vrg_arg_ }; \
                          if (vrgargn == 0) { \
                            vrg_argnode().next = vrg_arglist; \
                            vrg_arglist = & vrg_argnode() ; \
                          } \
                          else if (!vrg_checkarg(&vrg_argnode()) || !(++vrg_count)) ; else

#define vrgdefault if (!vrgargn || vrg_count || !(++vrg_count)) ; else

static void vrgusage()
{
  // Find the basename for the program
  char *prgname = vrg_argv[0];
  while (*prgname) prgname++;
  while ((prgname > vrg_argv[0]) && (prgname[-1] !='\\') && (prgname[-1] != '/'))
    prgname--;

  // Find the longest definition for pretty alignment
  int max_def_len = 0;
  for (vrg_argdef_t *arg = vrg_arglist; arg; arg = arg->next) {
    char *t = arg->def;
    while (*t && *t != '\t') t++;
    if ((t - arg->def) > max_def_len) max_def_len = (t - arg->def);
  }
 
  fprintf(stderr,"Usage: %s [OPTIONS] ...\n",prgname);
  if (vrgversion) fprintf(stderr,"%s\n",vrgversion);

  for  (vrg_argdef_t *arg = vrg_arglist; arg; arg = arg->next) {
    // Split the definition up to the `\t`
    char *t = arg->def;
    while (*t && *t != '\t') t++;

    // Print the switch letter and option
    fprintf(stderr,"   -%.*s",(int)(t - arg->def), arg->def);

    // Print the description (if any)
    while(isspace(*t)) t++;
    if (*t) fprintf(stderr,"%*s  %s", (int)(max_def_len - (t-arg->def)+1),"",t);

    fputc('\n',stderr);
  }
  exit(1);
}


// x [data]\tdescription -y delta
static int vrg_checkarg(vrg_argdef_t *arg)
{
  char *cli_arg = vrg_argv[vrgargn];
  char *cur_argdef = arg->def; 

  if (cli_arg[1] != cur_argdef[0]) return 0;

  // check if the switch has further argument (possibly optional)
  int has_option = 0;  // -1 optional; 0 no; 1 mandatory

  cur_argdef++;
  while (*cur_argdef == ' ') cur_argdef++;
  if (*cur_argdef && (*cur_argdef != '\t')) {
    if (*cur_argdef == '[') has_option = -1;
    else has_option = 1;
  }

  // Get the switch options
  vrgoption = vrg_emptystr;

  // No option needed
  if (has_option == 0) return 1;

  // Check if options is given together with the swoitch ("-x23")
  cli_arg +=2;
  while (isspace(*cli_arg)) cli_arg++;
  if (*cli_arg != '\0') {
     vrgoption = cli_arg;
     return 1;
  }
  
  // Check if option is given as next argument ("-x 23")
  if (vrgargn < vrg_argc -1 ) { // there is at least another argument
    if (vrg_argv[vrgargn+1][0] != '-') { // and is not a switch!
      vrgoption = vrg_argv[++vrgargn];
      return 1;
    }
  }

  if (has_option > 0) // No option found but one was due! Exit with error.
     vrgerror("ERROR: Missing option for switch '-%c`",arg->def[0]);

  return 1;
}


#endif 
#endif