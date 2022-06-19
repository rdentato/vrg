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

    #include "utl.h"

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
 // when here the vrgargn variable contains the current argument
 // to be processed
 // Example:   prg -f fname pluto
 //            if -f takes fname as argument, vrgargn will be 3
 //

 vrghelp(); // Will print the list of defined options


***/

#ifndef VRG_VERSION
#define VRG_VERSION 0x0001000C

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define vrg_cnt(vrg1,vrg2,vrg3,vrg4,vrg5,vrg6,vrg7,vrg8,vrgN, ...) vrgN
#define vrg_argn(...)  vrg_cnt(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define vrg_cat0(x,y)  x ## y
#define vrg_cat(x,y)   vrg_cat0(x,y)

#define vrg(vrg_f,...) vrg_cat(vrg_f, vrg_argn(__VA_ARGS__))(__VA_ARGS__)

// Command line options arguments

typedef struct vrg_opt_s {
  struct vrg_opt_s *next;
  char *opt;
  char *desc;
} vrg_opt_t;

extern int    vrgargn;
extern int    vrg_argc;
extern char **vrg_argv;

extern char *vrgoptarg;
extern int   vrglen;
char        *vrg_ver;

#define vrgver(s) vrg_ver = (s)

extern vrg_opt_t *vrg_opt_list;

int vrg_isopt(char *opt);
int vrghelp();

int vrg_checkopt();

#define vrgoptions(vrg_argc_,vrg_argv_) \
  for (vrgargn = 0, vrg_argc = vrg_argc_, vrg_argv = vrg_argv_, errno = 0 , vrglen = 1\
      ; vrg_checkopt() \
      ; vrgargn++ ) 

#define vrgopt(vrg_opt_,vrg_desc_) \
  static vrg_opt_t vrg_cat(vrg_OPT_,__LINE__);\
  if (vrgargn == 0) { \
    vrg_cat(vrg_OPT_,__LINE__) .next = vrg_opt_list; \
    vrg_opt_list = &(vrg_cat(vrg_OPT_,__LINE__)); \
    vrg_cat(vrg_OPT_,__LINE__) .opt = vrg_opt_; \
    vrg_cat(vrg_OPT_,__LINE__) .desc = vrg_desc_; \
  } \
  else if (!vrg_isopt(vrg_cat(vrg_OPT_,__LINE__) .opt)) ; \
  else


#ifdef VRG_MAIN

#include "dbg.h"

int    vrgargn;
int    vrg_argc;
char **vrg_argv=NULL;

char *vrgoptarg;
int   vrglen;

char *vrgver = NULL;

vrg_opt_t *vrg_opt_list = NULL;

static int vrg_err(char *opt)
{
  char *s = opt;
  while (*s && !isspace(*s)) s++;
  fprintf(stderr,"ERROR: Invalid '%.*s'\n", (int)(s-opt),opt);
  return vrghelp();
}

int vrghelp()
{
  char *s = vrg_argv[0];
  while (*s) s++;
  while ((s > vrg_argv[0]) && (s[-1] !='\\') && (s[-1] != '/')) s--;
  if (vrg_ver && *vrg_ver) 
    fprintf(stderr, "%s\n",vrg_ver);
  fprintf(stderr, "Usage: %s [OPTIONS] ...\n",s);
  for (vrg_opt_t *opt=vrg_opt_list; opt; opt=opt->next) {
      fprintf(stderr, "   %s\t\t%s\n",opt->opt,opt->desc);
  }
  exit(1);
}

int vrg_checkopt()
{

  if (vrgargn == 0) return 1;

  if (vrglen == 0) {
    vrg_err(vrg_argv[vrgargn-1]);
    return 0;
  }
  vrglen = 0;

  if (vrgargn >= vrg_argc) return 0;

  if (!(vrg_argv[vrgargn][0] == '-' && vrg_argv[vrgargn][1]))
    return 0;

  
  if (vrg_isopt("--")) {
      vrgargn++; return 0;
  }

  if (vrg_isopt("-h")) {
      vrgargn++; 
      vrghelp(); return 1;
  }
  
  return 1;
}


// Example: "-o [filename]"
//          "myprg -fpippo"
//
int vrg_isopt(char *opt)
{
  int ret = 0;
  int opt_ndx=0;
  int arg_ndx=0;
  char *arg=vrg_argv[vrgargn];
 
  vrgoptarg = NULL; 

  if (arg == NULL) return 0;

  _dbgtrc("ISOPT: '%s' '%s'",opt,arg);

  while (opt[opt_ndx] && !isspace(opt[opt_ndx])) opt_ndx++ ;
  if (strncmp(opt,arg,opt_ndx) == 0) {
    ret = 1;
    arg_ndx = opt_ndx;
    while (opt[opt_ndx] && isspace(opt[opt_ndx])) opt_ndx++; // I'm on the arg.
    if (opt[opt_ndx]) { // Need arg
     _dbgtrc("OPT USE ARG (%c)",opt[opt_ndx]);
      if (arg[arg_ndx])
        vrgoptarg = arg+arg_ndx;
      else if (vrgargn+1 < vrg_argc) {
        if (vrg_argv[vrgargn+1][0] != '-')
          vrgoptarg = vrg_argv[++vrgargn];
      }
      if (vrgoptarg == NULL) vrgoptarg = "";

     _dbgtrc("OPT USE ARG: '%s'",vrgoptarg);

      if (*vrgoptarg == '\0' && opt[opt_ndx] != '[')  {
        errno = vrg_err(opt);
        ret = 0;
      }
    }
    vrglen = ret;
  }
  return ret;
}

#endif 
#endif