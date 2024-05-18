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

# Command line options
 A minimal replacement of getarg.

 Define `VRGARGS` before including `vrg.h` only once, usually in the same source where `main()` is.

  #define VRGCLI
  #include "vrg.h"

... In the code:

 vrgcli("Version 1.0", argc,argv) {  // as received by main

   vrgarg("-f filename\tLoad file") { // explanation of the arg comes after \t
      // Will work both for `-f pippo` and `-fpippo`
      // Set what you need to set
      printf("file: `.*s`",vrglen, vrgargarg);
   }

   vrgarg("-o [filename]\tEnable output (on stdout by default)") {
       // Here filename is argional
   }

   vrgarg() { // Default handler (if none of the above triggers)
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

 vrgusage(); // Will print an error message (if specified) and the usage. Then will exit.
 vrgerror(); // Will print an error message (if specified). Then will exit.

 Read README.md for full details.
***/

#ifndef VRG_VERSION
#define VRG_VERSION 0x0009000C // 0.9.0-RC

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Variadic functions

#define vrg_cnt(_1,_2,_3,_4,_5,_6,_7,_8,_9,_N, ...) _N
#define vrg_argn(...)    vrg_cnt(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define vrg_commas(...)  vrg_cnt(__VA_ARGS__,    0, 0, 0, 0, 0, 0, 0, 0, _)
#define vrg_comma(...) ,
#define vrg_empty_(_0, _1, _2, _3) vrg_commas(vrg_cat5(vrg_empty_, _0, _1, _2, _3))
#define vrg_empty____0 ,
#define vrg_empty(...) vrg_empty_( vrg_commas(__VA_ARGS__)    , vrg_commas(vrg_comma __VA_ARGS__), \
                                   vrg_commas(__VA_ARGS__ ( )), vrg_commas(vrg_comma __VA_ARGS__ ( )) )

#define vrg_cat5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define vrg_cat3(_0, _1, _2)         vrg_cat5(_0, _1, _2, , )

// There are two of them because if one of the arguments of `vrg` is defined with `vrg`, the
// macro expansion would stop. In that case define one with `vrg` and the other with `VRG`.
// Hopefully two will be enough.
#define vrg(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)
#define VRG(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)

// If you want just to distinguish between having or not having arguments you cam use `vrg_`:
// #define vf(...) vrg_(vf,__VA_ARGS__)
// #define vf01()    printf("ARGS: none\n");
// #define vf__(...) printf("ARGS: " __VA_ARGS__)

#define vrg_zcommas(...) vrg_cnt(__VA_ARGS__, __, __, __, __, __, __, __, __, 01)
#define vrg_(vrg_f,...) vrg_cat3(vrg_f, vrg_zcommas(__VA_ARGS__) , )(__VA_ARGS__)
#define VRG_(vrg_f,...) vrg_cat3(vrg_f, vrg_zcommas(__VA_ARGS__) , )(__VA_ARGS__)

// Command line options arguments

#ifdef VRGCLI

#ifndef VRG_STR_USAGE
#define VRG_STR_USAGE        "USAGE"
#endif

#ifndef VRG_STR_ERROR       
#define VRG_STR_ERROR        "ERROR"
#endif

#ifndef VRG_STR_OPTIONS
#define VRG_STR_OPTIONS      "OPTION"
#endif

#ifndef VRG_STR_INVALID     
#define VRG_STR_INVALID      "Invalid value '%V' for %T '%N'"
#endif

#ifndef VRG_STR_INV_ARGUMENT
#define VRG_STR_INV_ARGUMENT "argument"
#endif

#ifndef VRG_STR_INV_OPTION  
#define VRG_STR_INV_OPTION   "option"
#endif

#ifndef VRG_STR_NO_ARGOPT  
#define VRG_STR_NO_ARGOPT    "Missing argument for option '%.*s'"
#endif

#ifndef VRG_STR_NO_ARGUMENT  
#define VRG_STR_NO_ARGUMENT  "Missing argument '%.*s'"
#endif

#ifndef VRG_STR_ARGUMENTS
#define VRG_STR_ARGUMENTS    "ARGUMENTS"
#endif

//   prog  -b 23 -a  pippo pluto
//         ╰─┬─╯ ╰┬╯╰────┬─────╯
//           │    │      ╰────────── arguments
//           │    ╰───────────────── flag
//           ╰────────────────────── flag with an argument

//  "-h|--help\tPrint usage details"
//  "-f|--file filename\nThe file to open"
//  "--no-duplicates [Y/N]\tEliminates duplicates (defaults to 'Y')"

// Private version of vrg()
#define vRG(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)

#define VRG_ARG_IS_MANDATORY    0x01
#define VRG_ARG_IS_OPTIONAL     0x02
#define VRG_ARG_IS_OPTWITHARG   0x03
#define VRG_ARG_FOUND           0x80
#define VRG_ARG_IS_SHORTOPT     0x10
#define VRG_ARG_IS_LONGOPT      0x20
#define VRG_ARG_IS_OPTION       0x30
#define VRG_ARG_IS_POSITIONAL   0x40

#define vrg_argdef_set(n,f) ((n)->argis |= VRG_ARG_ ## f)
#define vrg_argdef_check(n,f) ((n)->argis & VRG_ARG_ ## f)

// The definitions of CLI arguments are stored in a linked list. Each node of the list
// holds information on the type of argument specified by `vrgarg()`

typedef struct vrg_def_s {
    char             *def;
    struct vrg_def_s *next;
    signed char       pos;    // Which positional argument this is (-1 for flags)
    unsigned char     tab;    // up to the \t charcter
    unsigned char     argis;  // See flags above
    unsigned char     fst;    // first char of the (long) flag or argument name
    unsigned char     lst;    // lentgth of the (long) flag or argument name
} vrg_def_t;

static vrg_def_t *vrg_arglist = NULL;

static int   vrgargn = 0;
static char *vrgarg;

static int    vrg_argfound = 0;
static int    vrg_pos;

// These are usually seto to the value of `argc` and `argv` passed to `main()`
static int    vrg_argc;
static char **vrg_argv=NULL;

// We encode some of the CLI characteristic to ease the generation of `vrgusage()` text.
static int vrg_cli_has = 0;

#define VRG_CLI_HAS_OPTIONS     0x01   // The CLI has at least one option
#define VRG_CLI_HAS_DEFAULT     0x02   // The CLI handles unknown arguments
#define VRG_CLI_HAS_ARGS        0x04   // The CLI has positional arguments
#define VRG_CLI_HAS_NOMOREOPT   0x10   // Encountered '--' in the args, no more options checked

// To simplify reading, the bit masking operation are abstracted away by the `set` and `check` functions
#define vrg_cli_set(F)   (vrg_cli_has |= (VRG_CLI_ ## F))
#define vrg_cli_check(F) (vrg_cli_has  & (VRG_CLI_ ## F))

static char *vrg_emptystr  = "";
static char *vrg_help      = NULL;

#define vrgerror(...) do { fflush(stdout); fprintf(stderr, VRG_STR_ERROR ": " __VA_ARGS__); exit(1); } while(0)

#define vrgusage(...) vrg_(vrgusage,__VA_ARGS__)
#define vrgusage__(...) (fflush(stdout), fprintf(stderr,"" __VA_ARGS__), vrgusage01())

// static int vrgusage01();

#define vrgcli(...) vRG(vrgcli,__VA_ARGS__)
#define vrgcli01()  vrgcli_3(NULL,argc,argv)
#define vrgcli_1(s) vrgcli_3(s,argc,argv)

#define vrgcli_3(hlp_, argc_, argv_) \
  for (vrgargn = 0, vrg_pos = 0, vrg_argfound = vrg_invalid(NULL), vrg_help = hlp_, vrg_argc = argc_, vrg_argv = argv_, vrgarg=vrg_emptystr  \
      ; ((vrgargn < vrg_argc) || vrg_check_mandatory()) && ((vrgargn == 1) ? !(vrg_pos=0) : 1) \
      ; vrg_argfound ? vrg_argfound = 0 : vrgargn++ )

#define vrgarg(...) vRG(vrgarg,__VA_ARGS__)
                                                                           //╭─ Just assign vrgarg to current argument
#define vrgarg01() if (vrgargn == 0 || vrgargn >= vrg_argc || vrg_argfound || !(vrgarg = vrg_argv[vrgargn])) \
                       vrg_cli_set(HAS_DEFAULT); \
                   else

#define vrg_argdef vrg_cat3(vrg_def_, __LINE__,) // Each invocation of vrgarg() will create a unique node.
//               ╰──────────────────────╮  
#define vrgarg_1(def_) static vrg_def_t vrg_argdef = { .def = def_, .pos = -1, .argis = 0,  .tab = 0 }; \
                            if (vrgargn == 0) vrg_add_argdef(&vrg_argdef); \
                       else if (vrgargn >= vrg_argc) { vrg_check_mandatory(); break; } \
                       else if (!vrg_checkarg(&vrg_argdef, vrg_argv[vrgargn])) ; \
                       else

#define vrgarg_2(def_, chk_)             vrgarg_1(def_) if (!(chk_(vrgarg) || vrg_invalid(&vrg_argdef))); else
#define vrgarg_3(def_, chk_, a_)         vrgarg_1(def_) if (!(chk_(vrgarg, a_) || vrg_invalid(&vrg_argdef))); else
#define vrgarg_4(def_, chk_, a_, b_)     vrgarg_1(def_) if (!(chk_(vrgarg, a_, b_) || vrg_invalid(&vrg_argdef))); else
#define vrgarg_5(def_, chk_, a_, b_, c_) vrgarg_1(def_) if (!(chk_(vrgarg, a_, b_, c_) || vrg_invalid(&vrg_argdef))); else


static int vrg_invalid(vrg_def_t *node)
{
  if (node == NULL) return 0;
  if ((vrgarg == NULL || *vrgarg == '\0') && (vrg_argdef_check(node,IS_MANDATORY) == 0)) return 1;

  fputs(VRG_STR_ERROR ": ", stderr);
  for (char *m = VRG_STR_INVALID; *m; m++) {
      if (m[0] == '%' && m[1]) {
          m++;
          switch (*m) {
              case 'V' : fprintf(stderr,"%s",vrgarg); break;
              case 'T' : fprintf(stderr,"%s",node->def[0] == '-'? VRG_STR_INV_OPTION : VRG_STR_INV_ARGUMENT); break;
              case 'N' : fprintf(stderr,"%.*s",node->lst, node->def); break;
              case '%' : fputc('%',stderr); break;
              default  : fprintf(stderr,"%%%c",*m);
          }
      } 
      else fputc(*m,stderr);
  }
  fputc('\n',stderr);
  exit(1);
}

static void vrg_set_option_def(vrg_def_t *node, char *cur_def)
{
  cur_def++;
  if (*cur_def != '-') {
      vrg_argdef_set(node,IS_SHORTOPT);
      node->fst = cur_def - node->def;
      node->lst = node->fst + 1;
      // Move forward in case there's a long format version for this option
      while (*cur_def && *cur_def != '\t' && *cur_def != '-') cur_def++;
      if (*cur_def == '-') cur_def++;
  }

  if (*cur_def == '-') { // check for long
      cur_def++;
      if (*cur_def && *cur_def != ' ' && *cur_def != '\t') { // Found a long one
          vrg_argdef_set(node,IS_LONGOPT);         
          // compute the length
          node->fst = cur_def - node->def;
          while (*cur_def && *cur_def != ' ' && *cur_def != '\t') cur_def++;
          node->lst = cur_def - node->def;
      }
  }

  // Check if this option has an argument
  while (*cur_def == ' ') cur_def++;
  if (*cur_def == '[') vrg_argdef_set(node,IS_OPTIONAL); // optional argument
  else if (*cur_def != '\0' && *cur_def != '\t') vrg_argdef_set(node,IS_MANDATORY); // mandatory argument
}

static void vrg_add_argdef(vrg_def_t *node)
{
  char *cur_def = node->def;
  
  node->next = vrg_arglist;
  vrg_arglist = node;

  while(isspace(*cur_def)) cur_def++;
  node->def = cur_def;
  
  if (cur_def[0] == '-' && cur_def[1] != '\0')  { // it's an option
      vrg_cli_set(HAS_OPTIONS);
      vrg_set_option_def(node, cur_def);
  }
  else { // it's a positional argument
      vrg_cli_set(HAS_ARGS);
      node->pos = vrg_pos++;
      node->fst = cur_def - node->def;

      vrg_argdef_set(node,IS_POSITIONAL);

      if (*cur_def == '[') vrg_argdef_set(node,IS_OPTIONAL); // and it's optional
      else vrg_argdef_set(node,IS_MANDATORY);

      while (*cur_def && !isspace(*cur_def))  cur_def++;

      node->lst = cur_def - node->def;
  }
}

// -x [data]\tdescription -y delta
// file
static int vrg_checkarg(vrg_def_t *node, char *cli_arg)
{
  char *cur_def = node->def;

  vrgarg = vrg_emptystr;

  if (!vrg_cli_check(HAS_NOMOREOPT) && (cli_arg[0] == '-')) { // found a flag

      if ((cli_arg[1] == '-') ) { // Double dash '--'
          if (cli_arg[2] == '\0') { // Stop options
              vrg_cli_set(HAS_NOMOREOPT); // Signalling the end of options.
              vrgargn++;
              vrg_argfound = 1;
              return 0;
          }
          if (!vrg_argdef_check(node,IS_LONGOPT)) return 0;
          cli_arg += 2;
          int n;
          for (n = node->fst; *cli_arg && (n < node->lst) && (node->def[n] == *cli_arg); n++) cli_arg++;
          if (n < node->lst) return 0;
          if (*cli_arg == '=') cli_arg++;
          else if (*cli_arg != '\0') return 0;
      }
      else {
          if (!vrg_argdef_check(node,IS_SHORTOPT)) return 0; // The node is a not a flag ...
          if (cli_arg[1] != cur_def[1]) return 0; // ... or it's not the same flag
          cli_arg += 2;
      }

      // Now `cli_arg` is right after the end of the option. for example:
      //  -x     --x-rays=
      //    ^             ^   
      if (vrg_argdef_check(node,IS_OPTWITHARG)) { // look for an argument to the option
          if (cli_arg[0] != '\0' || cli_arg[-1] == '=') // option argument is attached to the option itself -x32 --x-rays=32
              vrgarg = cli_arg;  
          else if ((vrgargn+1 < vrg_argc) && vrg_argv[vrgargn+1][0] != '-') // take next argument (if any) but ignore flags
              vrgarg = vrg_argv[++vrgargn];

          if (vrgarg == vrg_emptystr && vrg_argdef_check(node,IS_MANDATORY))
              vrgerror(VRG_STR_NO_ARGOPT "\n",node->lst,cur_def);

          // while (isspace(*vrgarg)) vrgarg++;
      }
  }
  else { // Checking for an agument

     // but a flag has been found.
      if (!vrg_cli_check(HAS_NOMOREOPT) && cli_arg[0] == '-') return 0;

      // It's an argument but the current pos doesnt match!
      if (vrg_pos != node->pos) return 0;

      vrgarg = cli_arg;
      vrg_argdef_set(node,FOUND);
      vrg_pos++;
  }

  vrgargn++;
  vrg_argfound = 1;
  return 1;
}

static int vrg_check_mandatory()
{
  vrg_def_t *node = vrg_arglist;
  int errors=0;
  while (node) {
      if (vrg_argdef_check(node,IS_POSITIONAL) && vrg_argdef_check(node,IS_MANDATORY) && !vrg_argdef_check(node,FOUND)) {
          errors++;
          fprintf(stderr,VRG_STR_ERROR ": " VRG_STR_NO_ARGUMENT "\n", node->lst,node->def);
      }
      node = node->next;
  }
  if (errors) exit(1);
  return 0;
}

#define vrgusage01() exit(vrghelp())

static int vrghelp()
{
  vrg_def_t *node = vrg_arglist;
  vrg_def_t *inverted = NULL;
  vrg_def_t *tmp_node;
  char *s=NULL;
  int max_tab = 0;

  fflush(stdout);

  // Just print the help message passed to vrgcli()
  if (vrg_help && *vrg_help == '#') {
      while (*vrg_help && *vrg_help != '\n') vrg_help++;
      if (*vrg_help) vrg_help++;
      fprintf(stderr,"%s\n",vrg_help);
      exit(1);
  }

  // Invert the list so that arguments are in the same order 
  // they were specified in `vrgcli()` and compute the space
  // needed to align the definition part.
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

  fprintf(stderr, "%s:\n  %s ",VRG_STR_USAGE, prgname);

  if (vrg_cli_check(HAS_OPTIONS)) fprintf(stderr, "[%s] ",VRG_STR_OPTIONS);

  if (vrg_cli_check(HAS_ARGS)) {
      for (node = vrg_arglist; node ; node = node->next) {
          if (node->def[0] != '-')
              fprintf(stderr,"%.*s ",node->tab,node->def);
      }
  }

  if (vrg_cli_check(HAS_DEFAULT)) fprintf(stderr,"...");

  fprintf(stderr,"\n");
  if (vrg_help && *vrg_help) fprintf(stderr,"  %s\n",vrg_help);

  if (vrg_cli_check(HAS_ARGS)) {
      fprintf(stderr,"\n%s:\n",VRG_STR_ARGUMENTS);
      for (node = vrg_arglist; node ; node = node->next) {
          if (node->def[0] != '-') {
              fprintf(stderr,"  %.*s%*s%s\n",node->tab,node->def,max_tab - node->tab, "", node->def + node->tab);
          }
      }
  }

  if (vrg_cli_check(HAS_OPTIONS)) {
    fprintf(stderr,"\n%s:\n", VRG_STR_OPTIONS);
    for (node = vrg_arglist; node ; node = node->next) {
        if (node->def[0] == '-') {
            fprintf(stderr,"  %.*s%*s%s\n",node->tab,node->def,max_tab - node->tab, "", node->def + node->tab);
        }
    }
  }

  return 1;
}

#endif
#endif