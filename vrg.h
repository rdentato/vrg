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

In other words, you want all arguments to be optional.

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

# Command line options
 A minimal replacement of getopt.

 Define `VRGOPTS` before including `vrg.h` only once, usually in the same source where `main()` is.

  #define VRGOPTS
  #include "vrg.h"

... In the code: 

 vrgoptions(argc,argv) {  // as received by main

   vrgopt("-f filename\tLoad file") { // explanation of the opt comes after \t
      // Will work both for `-f pippo` and `-fpippo`
      // Set what you need to set
      printf("file: `.*s`",vrglen, vrgoptarg);
   }

   vrgopt("-o [filename]\tEnable output (on stdout by default)") {
       // Here filename is optional
   }

   vrgoptdefault {
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

 vrghelp(); // Will print the list of defined options


***/

#ifndef VRG_VERSION
#define VRG_VERSION 0x0001001C

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

#define vrg_cat5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define vrg_empty_(_0, _1, _2, _3) vrg_commas(vrg_cat5(vrg_empty_, _0, _1, _2, _3))
#define vrg_empty____0 ,
#define vrg_comma(...) ,
#define vrg_empty(...) vrg_empty_( vrg_commas(__VA_ARGS__)    , vrg_commas(vrg_comma __VA_ARGS__), \
                                   vrg_commas(__VA_ARGS__ ( )), vrg_commas(vrg_comma __VA_ARGS__ ( )) )
#define vrg_cat3_(x,y,z)  x ## y ## z
#define vrg_cat3(x,y,z)   vrg_cat3_(x,y,z)

#define vrg(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)
#define VRG(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)


// Command line options arguments

#ifdef VRGOPTS

#ifndef VRGMAXOPTS
#define VRGMAXOPTS 16
#endif

static char *vrg_optlist[VRGMAXOPTS];

static char *vrg_ver;

#define vrgver(s) vrg_ver = (s)

static int vrg_isopt(char *opt);
static int vrghelp();
static int vrg_nxtopt();

#define vrgoptions(vrg_argc_,vrg_argv_) \
  for (vrgargn = 0, vrg_argc = vrg_argc_, vrg_argv = vrg_argv_, vrg_count = 0, errno = 0\
      ; vrg_nxtopt() \
      ; vrg_count = 0, vrgargn++ ) 

#define vrgopt(vrg_opt_) \
  if (vrgargn == 0) { \
    char *s; \
    vrg_optlist[vrg_numopts++] = vrg_opt_; \
    for(s=vrg_opt_;*s && *s!= '\t';s++); \
    if (vrg_maxlen < (s-vrg_opt_)) vrg_maxlen = (s-vrg_opt_); \
  } \
  else if (!vrg_isopt(vrg_opt_)); \
  else

#define vrgoptdefault \
  if ((vrg_count > 0) || \
      (vrgargn == 0) || \
      (vrgargn >= vrg_argc) || \
      (vrg_argv[vrgargn][0] != '-') || \
      (vrg_argv[vrgargn][1] == '\0') || \
      (vrg_argv[vrgargn][1] == ' ')); \
  else

static int   vrgargn;
static char *vrgoptarg;
static int   vrglen=-1;

static int vrg_maxlen = 0;
static int vrg_count;
static int vrg_argc;
static char **vrg_argv=NULL;

static int vrg_numopts=0;

static char *vrg_emptystr = "";

#define vrgerror(...) (fprintf(stderr,__VA_ARGS__),vrghelp())

static int vrg_err(char *opt)
{
  char *s = opt;
  while (*s && !isspace(*s)) s++;
  fprintf(stderr,"ERROR: Invalid '%.*s'\n", (int)(s-opt),opt);
  return vrghelp();
}

static int vrghelp()
{
  char *s = vrg_argv[0];
  while (*s) s++;
  while ((s > vrg_argv[0]) && (s[-1] !='\\') && (s[-1] != '/')) s--;
  if (vrg_ver && *vrg_ver) fprintf(stderr, "%s\n",vrg_ver);
  fprintf(stderr, "Usage: %s [OPTIONS] ...\n",s);
  for (int k=0; k<vrg_numopts;k++) {
    int j=vrg_maxlen;
    char *s=vrg_optlist[k];
    while (j>0 && *s && *s != '\t') { fputc(*s++,stderr); j--;}
    while (j>0) { fputc(' ',stderr); j--;}
    if (*s) s++;
    fprintf(stderr, "  %s\n",s);
  }
  exit(1);
}

static int vrg_nxtopt()
{
  return (vrgargn == 0)
         || ((vrgargn < vrg_argc) 
            && (vrg_argv[vrgargn][0] == '-') 
            && (vrg_argv[vrgargn][1] != '\0'));
}

// Example: "-o [filename]"
//          "myprg -fpippo"
//
static int vrg_isopt(char *opt)
{
  int opt_ndx=0;
  char *arg=vrg_argv[vrgargn];
 
  if (arg == NULL || arg[0]!= '-') return 0;

  vrgoptarg = vrg_emptystr; 
  vrglen = 0;

  if (opt[1]==arg[1]) {
    opt_ndx = 2;
    while (opt[opt_ndx] == ' ') opt_ndx++ ;
    if (opt[opt_ndx] != '\t') {
      if (arg[2] != 0)
        vrgoptarg = arg+2;
      else if (vrgargn+1 < vrg_argc)
        vrgoptarg = vrg_argv[++vrgargn];
  
      if ((vrgoptarg == vrg_emptystr) && (opt[opt_ndx] != '['))  {
         errno = vrg_err(opt);
         return 0;
      }
      vrglen = strlen(vrgoptarg);
    }
    vrg_count++;
    return 1;
  }
  return 0;
}

#endif 
#endif