/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

/*
# Variadic functions

Say you want to define a variadic function with the following prototype:

    myfunc(int a [, char b [, void *c]])

In other words, you want `b` and `c` to be optional.

Simply, define your function with another name (say `my_func()`) and specify
how it should be called when invoked with 1, 2 or 3 paramenters as shown 
in the example below.

Example:

    #include "vrg.h"

    int my_func(int a, char b, void *c);
    
    #define myfunc(...)     vrg(myfunc, __VA_ARGS__)
    #define myfunc1(a)      my_func(a,'\0',NULL)
    #define myfunc2(a,b)    my_func(a,b,NULL)
    #define myfunc3(a,b,c)  my_func(a,b,c)

# Command line options
 A minimal replacement of getopt.

 void vrgerror(char *s)
 {
   // Will be called if an error occurs
   fprintf(stderr,"Missing argument for option: `%s`\n",s);
 }

... In the code: 

 vrgoptions(argc,argv) {  // as received by main

   vrgopt("-f filename","Load file") {
      // Will work both for `-f pippo` and `-fpippo`
      // Set what you need to set
      printf("file: `.*s`",vrglen, vrgoptarg);
   }

   vrgopt("-o [filename]", "Enable output (on stdout by default)") {
       // Here filename is optional
   }
 }
 // At the end, the vrgargn variable contains the number of the argument
 // that is sill to be processed
 // Example (assuming -f takes an argument):
//    prg -f fname pluto
 // vrgargn will be 3
 //

 vrghelp(); // Will print the list of defined options


***/

#ifndef VRG_VERSION
#define VRG_VERSION 0x0001000C

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

#define vrg_cnt(vrg1,vrg2,vrg3,vrg4,vrg5,vrg6,vrg7,vrg8,vrgN, ...) vrgN
#define vrg_argn(...)  vrg_cnt(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define vrg_cat0(x,y)  x ## y
#define vrg_cat(x,y)   vrg_cat0(x,y)

#define vrg(vrg_f,...) vrg_cat(vrg_f, vrg_argn(__VA_ARGS__))(__VA_ARGS__)

#ifndef VRGMAXOPTS
#define VRGMAXOPTS 16
#endif

extern int vrg_numopts;

char *vrg_optlist[VRGMAXOPTS];

// Command line options arguments
extern int vrg_maxlen;

extern int    vrgargn;
extern int    vrg_argc;
extern char **vrg_argv;

extern char *vrgoptarg;
extern int   vrglen;
char        *vrg_ver;

extern char *vrg_emptystr;

#define vrgver(s) vrg_ver = (s)

int vrg_isopt(char *opt);
int vrghelp();
int vrgerror(char *err);
int vrg_nxtopt();
void vrg_collect(char *opt);

#define vrgoptions(vrg_argc_,vrg_argv_) \
  for (vrgargn = 0, vrg_argc = vrg_argc_, vrg_argv = vrg_argv_, errno = 0\
      ; vrg_nxtopt() \
      ; vrgargn++ ) 

#define vrgopt(vrg_opt_) \
  if (vrgargn == 0) { \
    vrg_collect(vrg_opt_); \
  } \
  else if (!vrg_isopt(vrg_opt_)); \
  else

#ifdef VRG_MAIN

int vrg_maxlen = 0;

int    vrgargn;
int    vrg_argc;
char **vrg_argv=NULL;

int vrg_numopts=0;

char *vrgoptarg;
int   vrglen=-1;

char *vrgver = NULL;

char *vrg_emptystr = "";

static int vrg_err(char *opt)
{
  char *s = opt;
  while (*s && !isspace(*s)) s++;
  fprintf(stderr,"fatal error: invalid option '%.*s'\n", (int)(s-opt),opt);
  return vrghelp();
}

int vrgerror(char *err)
{
  fprintf(stderr,"%s\n", err);
  return vrghelp();
}

int vrghelp()
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

int vrg_nxtopt()
{
  return (vrgargn == 0)
         || ((vrgargn < vrg_argc) 
            && (vrg_argv[vrgargn][0] == '-') 
            && (vrg_argv[vrgargn][1] != '\0'));
}

void vrg_collect(char *opt)
{
  char *s = opt;
  vrg_optlist[vrg_numopts++] = opt;
  for(;*s && *s!= '\t';s++);
  if (vrg_maxlen < (s - opt)) vrg_maxlen = (s - opt);
}

// Example: "-o [filename]"
//          "myprg -fpippo"
//
int vrg_isopt(char *opt)
{
  int opt_ndx=0;
  char *arg=vrg_argv[vrgargn];
 
  if (arg == NULL || arg[0]!= '-') return 0;

  vrgoptarg = vrg_emptystr; 
  vrglen = 0;

  if (opt[1]==arg[1]) {
    opt_ndx = 2;
    while (opt[opt_ndx] == ' ') opt_ndx++ ;
    if (opt[opt_ndx] == '\t') return 1;

    if (arg[2] != 0)
      vrgoptarg = arg+2;
    else if (vrgargn+1 < vrg_argc)
      vrgoptarg = vrg_argv[++vrgargn];

    if ((vrgoptarg == vrg_emptystr) && (opt[opt_ndx] != '['))  {
       errno = vrg_err(opt);
       return 0;
    }
    vrglen = strlen(vrgoptarg);
    return 1;
  }
  return 0;
}

#endif 
#endif