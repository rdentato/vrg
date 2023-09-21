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
cur_def
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

#ifndef VRG_VERSION
#define VRG_VERSION 0x0001002C // 0.1.2-RC

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

// Variadic functions



#define vrg_cnt(_1,_2,_3,_4,_5,_6,_7,_8,_9,_N, ...) _N
#define vrg_argn(...)   vrg_cnt(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define vrg_commas(...)  vrg_cnt(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, _)
#define vrg_comma(...) ,
#define vrg_empty_(_0, _1, _2, _3) vrg_commas(vrg_cat5(vrg_empty_, _0, _1, _2, _3))
#define vrg_empty____0 ,
#define vrg_empty(...) vrg_empty_( vrg_commas(__VA_ARGS__)    , vrg_commas(vrg_comma __VA_ARGS__), \
                                   vrg_commas(__VA_ARGS__ ( )), vrg_commas(vrg_comma __VA_ARGS__ ( )) )

#define vrg_cat5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define vrg_cat3_(x,y,z)  x ## y ## z
#define vrg_cat3(x,y,z)   vrg_cat3_(x,y,z)

// There are two of them because if one of the arguments of `vrg` is defined with `vrg`, the
// macro expansion would stop. In that case define one with `vrg` and the other with `VRG`.
// Hopefully two will be enough.
#define vrg(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)
#define VRG(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)

#define vrg_zcommas(...) vrg_cnt(__VA_ARGS__, __, __, __, __, __, __, __, __, 01)
#define vrg_(vrg_f,...) vrg_cat3(vrg_f, vrg_zcommas(__VA_ARGS__) , )(__VA_ARGS__)

// Command line argions arguments

#ifdef VRGMAIN

//   prog  -b 23 -a pippo pluto
//         \___/ \/ \_________/
//          \     \  \____________ argument
//           \     \______________ flag
//            \___________________ flag with an option



typedef struct vrg_def_s {
  char  *def;
  struct vrg_def_s *next;
  char   pos;
  char   tab;     // up to the \t charcter
  char   hasarg;  //  -1 optional; 0 none; 1 mandatory
  char   found;
} vrg_def_t;

static vrg_def_t *vrg_arglist = NULL;

static int   vrgargn = 0;
static char *vrgarg;

static int    vrg_argfound = 0;
static int    vrg_pos;
static int    vrg_argc;
static char **vrg_argv=NULL;

#define VRG_CLI_HAS_FLAGS    0x01
#define VRG_CLI_HAS_DEFAULT  0x02
#define VRG_CLI_HAS_ARGS     0x04

static int vrg_has = 0;

static char *vrg_emptystr = "";

static char *vrg_help = NULL;


#define vrgusage(...) vrg_(vrgusage,__VA_ARGS__)
#define vrgusage__(...) (fprintf(stderr,"" __VA_ARGS__), vrgusage01())
static int vrgusage01();

#define vrgcli(...) vrg(vrgcli,__VA_ARGS__)
#define vrgcli01()  vrgcli_3(NULL,argc,argv)
#define vrgcli_1(s) vrgcli_3(s,argc,argv)

#define vrgcli_3(s,argc_,argv_) \
  for (vrgargn = vrg_invalid(NULL), errno = 0, vrg_argfound = 0, vrg_help = s, vrg_argc = argc_, vrg_argv = argv_, vrg_pos = 0  \
      ; ((vrgargn < vrg_argc) || vrg_checkmandatory()) && ((vrgargn == 1) ? !(vrg_pos=0) : 1) \
      ; vrg_argfound ? vrg_argfound = 0 : vrgargn++ ) 

#define vrg_defnode() vrg_cat3(vrg_def_, __LINE__,)

#define vrgarg(...) vrg(vrgarg,__VA_ARGS__)

#define vrgarg01() if (vrgargn==0 || vrgargn >= vrg_argc || vrg_argfound || !(vrgarg = vrg_argv[vrgargn])) vrg_has |= VRG_CLI_HAS_DEFAULT; \
                   else

#define vrgarg_1(def_) static vrg_def_t vrg_defnode() = { .def = def_, .pos = -1, .hasarg = 0 }; \
                                 if (vrgargn == 0) vrg_setnode(&vrg_defnode());\
                            else if (vrgargn >= vrg_argc) { vrg_checkmandatory(); break; }\
                            else if (!vrg_checkarg(&vrg_defnode(), vrg_argv[vrgargn])) ; \
                            else

#define vrgarg_2(def_, chk_)             vrgarg_1(def_) if (!(chk_(vrgarg) || vrg_invalid(&vrg_defnode()))); else
#define vrgarg_3(def_, chk_, a_)         vrgarg_1(def_) if (!(chk_(vrgarg, a_) || vrg_invalid(&vrg_defnode()))); else
#define vrgarg_4(def_, chk_, a_, b_)     vrgarg_1(def_) if (!(chk_(vrgarg, a_, b_) || vrg_invalid(&vrg_defnode()))); else
#define vrgarg_5(def_, chk_, a_, b_, c_) vrgarg_1(def_) if (!(chk_(vrgarg, a_, b_, c_) || vrg_invalid(&vrg_defnode()))); else

static int vrg_invalid(vrg_def_t *node)
{
  if (node == NULL) return 0;
  if ((vrgarg == NULL || *vrgarg == '\0') && (node->hasarg <= 0)) return 1;
  char *s = node->def;
  while(*s && !isspace(*s)) s++;
  vrgusage__("ERROR: Invalid value '%s' for %s %.*s\n", vrgarg, (node->def)[0] == '-'? "flag" : "argument", (int)(s - node->def), node->def);
  return (0);
}

static void vrg_setnode(vrg_def_t *node)
{
  char *cur_def = node->def;
  node->next = vrg_arglist;
  node->found = 1;
  vrg_arglist = node; 
  while(isspace(cur_def[0])) cur_def++;
  if (cur_def[0] == '-' && cur_def[1] != '\0')  { // it's a flag
    vrg_has |= VRG_CLI_HAS_FLAGS;
    cur_def += 2;
    while(*cur_def == ' ') cur_def++;
    if (*cur_def == '[') node->hasarg = -1; // optional argument
    else if (*cur_def != '\0' && *cur_def != '\t') node->hasarg = 1; // mandatory argument
  }
  else { // it's a positional argument
    vrg_has |= VRG_CLI_HAS_ARGS;
    node->hasarg = -1;
    if (cur_def[0] != '[') { // and it's mandatory
      node->hasarg = 1;
      node->found = 0;
    }
    node->pos = vrg_pos++;
  }
}

// -x [data]\tdescription -y delta
// file 
static int vrg_checkarg(vrg_def_t *node, char *cli_arg)
{
  char *cur_def = node->def; 

  vrgarg = vrg_emptystr;
  // It's a flag and the definition matches.
  if (cur_def[0] == '-') { // checking for a flag

    // The argument is a not a flag or it's not the same flag
    if (cli_arg[0] != '-' || cli_arg[1] != cur_def[1]) return 0; 

    if (node->hasarg) { // look for an option
      if (cli_arg[2] != '\0') 
         vrgarg = cli_arg+2;  // option is attached to the flag -x32
      else if ((vrgargn+1 < vrg_argc) && vrg_argv[vrgargn+1][0] != '-')
         vrgarg = vrg_argv[++vrgargn];
  
      if (vrgarg == vrg_emptystr && node->hasarg>0)
        vrgusage("Missing argument for %.2s\n",cur_def);
    }
  }
  else { // Checking for an agument

    // but a flag has been found.
    if (cli_arg[0] == '-') return 0;

    // It's an argument but the pos doesnt match!
    if (vrg_pos != node->pos) return 0;
    
    vrgarg = cli_arg;
    node->found = 1;
    vrg_pos++;
  }

  vrgargn++; 
  vrg_argfound = 1;
  return 1;
}

static int vrg_checkmandatory()
{
  vrg_def_t *node = vrg_arglist;
  int errors=0;
  char *s=NULL;
  while (node) {
     if (!node->found) {
       errors++;
       s = node->def;
       while (*s && *s != '\t') s++;
       fprintf(stderr,"Error: missing '%.*s' argument\n", (int)(s-node->def),node->def) ;
     };
     node = node->next;
  }
  if (errors) vrgusage();
  return 0;
}

static int vrgusage01()
{
  vrg_def_t *node = vrg_arglist;
  vrg_def_t *inverted = NULL;
  vrg_def_t *tmp_node;
  char *s=NULL;
  int max_tab = 0;

  while (node) {
    s = node->def;
    while (*s && *s != '\t') s++;
    node->tab = s - node->def; 
    if (node->tab > max_tab) max_tab = node->tab;

    tmp_node = node->next;
    node->next = inverted;
    inverted = node;
    node = tmp_node;
  }
  vrg_arglist = inverted;

  char *prgname = vrg_argv[0];
  while (*prgname) prgname++;
  while ((prgname > vrg_argv[0]) && (prgname[-1] !='\\') && (prgname[-1] != '/'))
    prgname--;

  fflush(stdout);
  fprintf(stderr, "USAGE:\n  %s ",prgname);

  if (vrg_has & VRG_CLI_HAS_FLAGS) fprintf(stderr, "[OPTIONS] ");

  if (vrg_has & VRG_CLI_HAS_ARGS) {
    for (node = vrg_arglist; node ; node = node->next) {
      if (node->def[0] != '-') fprintf(stderr,"%.*s ",node->tab,node->def);
    }
  }

  if (vrg_has & VRG_CLI_HAS_DEFAULT) fprintf(stderr,"...");

  fprintf(stderr,"\n");
  if (vrg_help && *vrg_help) fprintf(stderr,"  %s\n",vrg_help);

  if (vrg_has & VRG_CLI_HAS_ARGS) {
    fprintf(stderr,"\nARGUMENTS:\n");
    for (node = vrg_arglist; node ; node = node->next) {
      if (node->def[0] != '-') {
        fprintf(stderr,"  %.*s%*s%s\n",node->tab,node->def,max_tab - node->tab, "", node->def + node->tab);
      }
    }
  }

  if (vrg_has & VRG_CLI_HAS_FLAGS) {
    fprintf(stderr,"\nOPTIONS:\n");
    for (node = vrg_arglist; node ; node = node->next) {
      if (node->def[0] == '-') {
        fprintf(stderr,"  %.*s%*s%s\n",node->tab,node->def,max_tab - node->tab, "", node->def + node->tab);
      }
    }    
  }

  exit(1);
}

#endif 
#endif