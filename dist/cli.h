//.  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
//.  SPDX-License-Identifier: MIT

//           .oooooo.   ooooo        ooooo 
//          d8P'  `Y8b  `888'        `888' 
//         888           888          888  
//         888           888          888  
//         888           888          888  
//         `88b    ooo   888       o  888  
//          `Y8bood8P'  o888ooooood8 o888o 

#ifndef CLI_VERSION
#define CLI_VERSION 0x0011002B

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//.  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
//.  SPDX-License-Identifier: MIT
#ifndef VRG_VERSION
#define VRG_VERSION 0x0011000B // 0.11.0-beta
#define VRG_jn(x,y)    VRG_exp(x ## y)
#define VRG_join(x,y)  VRG_jn(x, y)
#define VRG_exp(...) __VA_ARGS__
#define VRG_count(x1,x2,x3,x4,x5,x6,x7,x8,x9,xA,xN, ...) xN
#define VRG_nargs(...)   VRG_exp(VRG_count(__VA_ARGS__, A, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define VRG_ncommas(...) VRG_exp(VRG_count(__VA_ARGS__, _, _, _, _, _, _, _, _, 1, _, _))
#define VRG_comma(...) ,
#define VRG_fn_sel(ret,...) \
   VRG_join(VRG_fn_, \
            VRG_join(VRG_ncommas(VRG_comma __VA_ARGS__ ()), VRG_ncommas(VRG_comma __VA_ARGS__ ))) (ret)
#define VRG_fn_1_(ret) 0
#define VRG_fn_11(ret) ret
#define VRG_fn___(ret) ret
#define vrg(f_,...)  VRG_join(f_, VRG_fn_sel(VRG_nargs(__VA_ARGS__),__VA_ARGS__))(__VA_ARGS__)
#define vrg_(f_,...) VRG_join(f_, VRG_fn_sel(_                     ,__VA_ARGS__))(__VA_ARGS__)
#endif // VRG_VERSION_H

#ifndef CLI_STR_ERROR_MSG
#define CLI_STR_ERROR_MSG "Missing or invalid value for"
#endif

#ifndef CLI_STR_ERROR
#define CLI_STR_ERROR "ERROR"
#endif

#ifndef CLI_STR_DEFAULT
#define CLI_STR_DEFAULT " (default)"
#endif

#ifndef CLI_STR_USAGE
#define CLI_STR_USAGE "USAGE"
#endif

#ifndef CLI_STR_OPTIONS
#define CLI_STR_OPTIONS "OPTIONS"
#endif

#ifndef CLI_STR_COMMANDS
#define CLI_STR_COMMANDS "COMMANDS"
#endif

#ifndef CLI_STR_ARGUMENTS
#define CLI_STR_ARGUMENTS "ARGUMENTS"
#endif

#ifndef NDEBUG
#define cli_trace(...) (fflush(stdout),fprintf(stderr,"" __VA_ARGS__),fprintf(stderr," :%s:%d\n",__FILE__,__LINE__))
#else
#define cli_trace(...)
#endif

#define cli__trace(...)

#define cli_join(x,y) VRG_join(x,y)

#define CLI_OPT_FLAG_SHORT 0x80   // is -z
#define CLI_OPT_FLAG_LONG  0x40   // is --zorro
#define CLI_OPT_COMMAND    0x20   // is 'add'
#define CLI_OPT_ARGUMENT   0x10   // Takes an argument
#define CLI_OPT_OPTIONAL   0x04   // the argument is optional
#define CLI_OPT_ARG_ERROR  0x02   // Error: missing argument
#define CLI_OPT_FOUND      0x01   // this as been already found

typedef struct cli_option_s {
  struct  cli_option_s *next;
           char *def;
           char  short_minus;    // This is a trick so that a pointer to
           char  optname_short;  // `short_minus` is also a pointer to the
           char  short_nul;      // string "-x" (where `x` is `optname_short`)
  unsigned char  flags;
  unsigned char  optname_offset; 
  unsigned char  optname_len;
} cli_option_t;

#define cli_short_offset(opt_)  ((char *)&(opt_->short_minus))
#define cli_short_len(opt_)     2

static cli_option_t *cli_head = NULL;

// This works because, according to the C standard, a pointer to a
// structure is also a pointer to its first field (`next` in this case).
static cli_option_t *cli_tail = (cli_option_t *)&cli_head;

static char  *cli_emptystr = "";

static char **cliargv;
static int    cliargc;

static char  *cliarg;
static int    clindx;

static char  *cliprogname = NULL;
static char  *cliheader   = "";

static int cli_no_flags = 0;

static unsigned short cli_num_options   = 0;
static unsigned short cli_num_commands  = 0;
static unsigned short cli_num_arguments = 0;

static unsigned short cli_cmd_found = 0;

static int cli_default_errors = 0;

static char *clierrormsg = CLI_STR_ERROR_MSG;

typedef char * (*cli_chk_t)(char *);

#define cliisdefault()  (clindx == 0)

#define clierror(s,...)   cli_prt_error(1,s,__VA_ARGS__)
#define cliwarning(s,...) cli_prt_error(0,s,__VA_ARGS__)

#define cli_message(...) (fflush(stdout), fprintf(stderr,"%s: ",cliprogname), fprintf(stderr,"" __VA_ARGS__), fputc('\n',stderr))

#define cli_prt_error(x,s,...) \
  do { \
    char *cli_err = s;\
    if (cli_err) { \
      if (cli_err[0] == '\0') cli_err = clierrormsg; \
        cli_message(vrg(cli_error_arg_,cli_err,__VA_ARGS__));\
      if (x) exit(1); \
    } \
  } while(0)

#define cli_error_arg_2(s,a)    CLI_STR_ERROR ": %s '%s'%s\n",s,a,(cliisdefault()? CLI_STR_DEFAULT :"")
#define cli_error_arg_3(s,a,n)  CLI_STR_ERROR ": %s '%.*s'%s\n",s,n,a,(cliisdefault()? CLI_STR_DEFAULT :"")

static inline int cli_is_endchr(char c) {
  return c == '\0' || c == '\t' || c == '(' || c == ')';
}

static inline int cli_is_skipchr(char c) {
  char *skip = " .,|;:*?!@#/&%~=^";
  while(*skip) if (c == *skip++) return 1;
  return 0;
} 

static int cli_parse_short(cli_option_t *opt, char **cur) 
{
  char *d = *cur;
  opt->optname_short = '\0';

  while (cli_is_skipchr(*d)) d++;
  if (*d != '-' || !isalnum(d[1]) || isalnum(d[2])) return 0;

  opt->optname_short = d[1];
  opt->flags |= CLI_OPT_FLAG_SHORT;
  *cur = d+2;
  return 1;
}

static int cli_parse_long(cli_option_t *opt, char **cur) {
  char *d = *cur;
  int flags = 0;
  int offset;
  while (cli_is_skipchr(*d)) d++;
  cli__trace("Checking: %s",d);
  offset = d - opt->def;
  if (d[0] == '-' && d[1] == '-' && isalpha(d[2])) {
    flags = CLI_OPT_FLAG_LONG;
  }
  else if ((d[0] == '\'' || d[0] == '<') && isalpha(d[1])) {
    offset++;
    flags = CLI_OPT_COMMAND;
  }
  if (flags == 0) return 0;
  cli__trace("Checking2: %s",d);

  opt->optname_offset = offset;

  d += 2;
  do { d++; } while (*d == '-' || isalnum(*d));
  opt->optname_len = (d - opt->def) - opt->optname_offset;
  if (d[0] == '\'' || d[0] == '>') d++;
  opt->flags |= flags;
  *cur = d;
  cli__trace("got: %.*s",opt->optname_len,opt->def+opt->optname_offset);
  return 1;
}

static int cli_parse_argname(cli_option_t *opt, char **cur) {
  char *d = *cur;
  int offset;
  int flags = CLI_OPT_ARGUMENT;

  while (cli_is_skipchr(*d)) d++;

  if (*d == '[') {d++; flags |= CLI_OPT_OPTIONAL;}

  if (!isalpha(*d)) return 0;
  
  offset = d - opt->def;
  do {d++;} while (*d == '-' || isalnum(*d));
  if (opt->optname_len == 0) { // It's a positional argument
    opt->optname_offset = offset;
    opt->optname_len = (d - opt->def) - opt->optname_offset;
  }
  if (*d == ']') d++;

  opt->flags |= flags ;
  *cur = d;
  return 1;
}

static int cli_parse_default(cli_option_t *opt, char **cur, cli_chk_t cli_chk_fn) {
  char *d = *cur;
  static char defbuf[32];
  int i;

  while (cli_is_skipchr(*d)) d++; 
  if (*d != '(' ) return 0;

  cliarg = NULL;
  do { d++; } while (*d == ' ');
  if (*d == '$') {
    d++;
    i = 0;
    while(i<31 && (*d == '_' || isalnum(*d))) {defbuf[i++] = *d++;}
    defbuf[i] = '\0';
    cliarg = getenv(defbuf);
  }
  if (cliarg == NULL) {
    while(*d == ',' || isspace(*d)) d++;
    if (!cli_is_endchr(*d)) {
      i = 0;
      while(i<31 && !cli_is_endchr(*d)) {defbuf[i++] = *d++;}
      defbuf[i] = '\0';
      cliarg = defbuf;
    }
  }

  if (cliarg == NULL) cliarg = cli_emptystr;
 
  char *arg = cli_short_offset(opt); 
  int   len = cli_short_len(opt);
  if (opt->optname_len > 0) {
    arg = opt->def + opt->optname_offset;
    len = opt->optname_len;
  } 

  char *err_msg = cli_chk_fn(cliarg);
  if (err_msg) {
    cliwarning(err_msg,arg,len);
    opt->flags |= CLI_OPT_ARG_ERROR;
    cli_default_errors++;
  }

  *cur = d;
  return 1;
}

static int cli_opt_define(char *def, cli_option_t *opt, cli_chk_t cli_chk_fn) {
  *opt = (cli_option_t){0};

  cli_tail->next = opt;
  cli_tail  = opt;
  opt->def  = def;
  opt->next = NULL;
  opt->short_minus = '-';
  opt->short_nul   = '\0';
  opt->optname_offset = 0;
  opt->optname_len = 0;

  cli_parse_short(opt, &def);
  cli_parse_long(opt, &def);
  cli_parse_argname(opt, &def);

  if (opt->optname_len > 30) opt->optname_len = 30;

  if (opt->flags & (CLI_OPT_FLAG_SHORT | CLI_OPT_FLAG_LONG)) 
    cli_num_options++;
  else if (opt->flags & CLI_OPT_COMMAND) 
    cli_num_commands++;
  else
    cli_num_arguments++;

  return cli_parse_default(opt, &def, cli_chk_fn);
}

static char *cli_remove_slash(char *s)
{
  char *e = s; 
  while(*e) {
    if (*e == '/' || *e == '\\') s = e+1;
    e += 1;
  }
  return s;
}

static int cli_print_cmd(char *cmd)
{
  char *s = cmd;
  
  fputs("  ",stderr);
  for (s = cmd; *s && *s != '\'' && *s != '<'; s++) 
    fputc(*s,stderr);
  
  if (!*s) return 0;

  for (s++; *s && *s != '\'' && *s != '>'; s++) 
    fputc(*s,stderr);
  
  if (!*s) return 0;

  fprintf(stderr,"%s\n",s+1);
  return 1;
}      


#define CLIEXIT 1
#define cliusage(...)   cli_usage(__VA_ARGS__+0)

int cli_usage(int xt) {
  cli_option_t *opt;

  if (cliheader != NULL) fprintf(stderr,"%s\n",cliheader);
  fprintf(stderr, CLI_STR_USAGE ": %s", cliprogname);
  
  if (cli_num_commands > 0) fprintf(stderr," " CLI_STR_COMMANDS);
  if (cli_num_options > 0)  fprintf(stderr," " CLI_STR_OPTIONS);
  if (cli_num_arguments > 0) {
    for (opt = cli_head; opt != NULL; opt = opt->next) 
      if (!(opt->flags & (CLI_OPT_FLAG_SHORT | CLI_OPT_FLAG_LONG | CLI_OPT_COMMAND))) {
        int not_optional = !(opt->flags & CLI_OPT_OPTIONAL);
        fprintf(stderr," %s%.*s%s","["+not_optional,opt->optname_len, opt->def+opt->optname_offset,"]"+not_optional);
      }
  }

  if (cli_num_commands > 0) fprintf(stderr,"\n" CLI_STR_COMMANDS ":\n");
  for (opt = cli_head; opt != NULL;opt = opt->next) 
    if (opt->flags & CLI_OPT_COMMAND)
      cli_print_cmd(opt->def);

  if (cli_num_options > 0) fprintf(stderr,"\n" CLI_STR_OPTIONS ":\n");
  for (opt = cli_head; opt != NULL;opt = opt->next)
    if (opt->flags & (CLI_OPT_FLAG_SHORT | CLI_OPT_FLAG_LONG))
      fprintf(stderr,"  %s\n",opt->def);

  if (cli_num_arguments > 0) fprintf(stderr,"\n" CLI_STR_ARGUMENTS ":\n");
  for (opt = cli_head; opt != NULL;opt = opt->next)
    if (!(opt->flags & (CLI_OPT_FLAG_SHORT | CLI_OPT_FLAG_LONG | CLI_OPT_COMMAND)))
      fprintf(stderr,"  %s\n",opt->def);

  if (xt != 0) exit(xt);
  return(0);
}

static char *cli_chk_true(char *arg) {return NULL;}

// Get the next argument as the argument of the option. 
static char *cli_get_arg(cli_option_t *opt, char *arg)
{
  cliarg = cli_emptystr;
  // If next argument exists and is not a flag
  if (   ((clindx + 1) < cliargc) 
      && !(cliargv[clindx+1][0] == '-' && cliargv[clindx+1][1]!='\0'))
    cliarg = cliargv[++clindx];

  // If the arg is not optional and we didn't find one
  if (!(opt->flags & CLI_OPT_OPTIONAL) && cliarg == cli_emptystr) {
    clierror(clierrormsg,arg);
    opt->flags |= CLI_OPT_ARG_ERROR;
  }
  
  return cliarg;
}

// Repars is needed when the user puts together multiple short options
// like in `ps -aux` instead of `ps -a -u -x`.
static int cli_reparse_ndx = 1;
#define cli_no_reparse() (cli_reparse_ndx == 1)

static int cli_check_short(cli_option_t *opt, char *arg)
{
  if (cli_no_flags) return 0;
  if (arg[0] != '-' || arg[1] == '\0') return 0;

  if (arg[cli_reparse_ndx] != opt->optname_short) return 0;

  if (opt->flags & CLI_OPT_ARGUMENT) {
    if (arg[cli_reparse_ndx+1] != '\0') 
      cliarg = arg+cli_reparse_ndx+1;
    else
      cliarg = cli_get_arg(opt,arg);
    cli_reparse_ndx = 1;  
  }
  else if (arg[cli_reparse_ndx+1] != '\0') {
    cli_reparse_ndx++;
  } 
  else cli_reparse_ndx = 1;

  opt->flags |= CLI_OPT_FOUND;
  return 1;
}

static int cli_check_long(cli_option_t *opt, char *arg)
{
  if (cli_no_flags) return 0;

  if ((opt->flags & CLI_OPT_COMMAND) && (cli_cmd_found)) return 0; // cmds can only appear once
  if (strncmp(arg, opt->def+opt->optname_offset, opt->optname_len) != 0) return 0;
  if (opt->flags & CLI_OPT_COMMAND) cli_cmd_found = 1; // cmds can only appears as first argument (possibly after some '-' options)
  
  if (arg[opt->optname_len] != '\0' && arg[opt->optname_len] != '=') return 0;
  
  if (opt->flags & CLI_OPT_ARGUMENT) {
    if (arg[opt->optname_len] == '=') 
      cliarg = arg + opt->optname_len+1;
    else
      cliarg = cli_get_arg(opt,arg);
  }
  
  opt->flags |= CLI_OPT_FOUND;
  return 1;
}

static int cli_check_arg(cli_option_t *opt, char *arg)
{
  if (opt->flags & CLI_OPT_FOUND) return 0;
  if (opt->flags & (CLI_OPT_FLAG_SHORT | CLI_OPT_FLAG_LONG | CLI_OPT_COMMAND)) return 0;
  if (!cli_no_flags && (arg[0] == '-' && arg[1] != '\0')) return 0;
  cliarg = arg;
  opt->flags |= CLI_OPT_FOUND;
  cli_cmd_found = 1; // No commands after the first positional argumen
  return 1;
}
 
static int cli_check(cli_option_t *opt, cli_chk_t cli_chk_fn)
{
  char *arg = cliargv[clindx];
  opt->flags &= ~CLI_OPT_ARG_ERROR;
  if (!cli_check_short(opt,arg) &&
      !cli_check_long(opt,arg)  &&
      !cli_check_arg(opt,arg)   )     
    return 0;

  cli__trace("arg: %s",arg);
  char *err_msg;
  if ((err_msg = cli_chk_fn(cliarg)) != NULL) {
    cli__trace("EE: %s",err_msg);
    clierror(err_msg, arg);
    opt->flags |= CLI_OPT_ARG_ERROR;
  }
  return 1;
}

static int cli_last_check()
{
  for (cli_option_t *opt = cli_head; opt != NULL; opt = opt->next) {
    if (opt->flags & (CLI_OPT_FLAG_LONG | CLI_OPT_FLAG_SHORT | CLI_OPT_COMMAND))
      continue;

    if (!(opt->flags & CLI_OPT_FOUND) && !(opt->flags & CLI_OPT_OPTIONAL)) {
      clierror(clierrormsg,opt->def+opt->optname_offset, opt->optname_len);
      opt->flags |= CLI_OPT_ARG_ERROR;
    }
  }
  return 1;
}

static int cli_double_dash()
{
  if (cli_no_flags) return 0; // Already stopped checking for flags
  char *arg = cliargv[clindx];
  if (arg[0] != '-' || arg[1] != '-' || arg[2] != 0) return 0;
  cli_no_flags = 1;
  return 1; 
}

#define clioptions(...) vrg(cli_options_,__VA_ARGS__)
#define cli_options_0()                         cli_options_3(NULL, argc, argv)
#define cli_options_1(cli_header)               cli_options_3(cli_header, argc, argv)
#define cli_options_2(cli_arg_cnt,cli_arg_vct)  cli_options_3(NULL, cli_arg_cnt, cli_arg_vct)
#define cli_options_3(cli_header,cli_arg_cnt,cli_arg_vct)  \
{ \
  cliargc = cli_arg_cnt; \
  cliargv = cli_arg_vct; \
  if (cliprogname == NULL) cliprogname = cli_remove_slash(cliargv[0]);\
  if (cli_header != NULL) cliheader = cli_header; \
  clindx = 0; \
  cli_num_options   = 0; \
  cli_num_commands  = 0; \
  cli_num_arguments = 0; \
  cli_cmd_found     = 0; \
  int cli_opt_found, cli_k; \
  cli_loop:  \
  for ( cliarg = cli_emptystr, cli_opt_found = 0; \
       (clindx < cliargc) ; \
       (clindx += cli_no_reparse()), cliarg = cli_emptystr, cli_opt_found = 0) \
   if (cli_default_errors) cliusage(CLIEXIT); else \
   if (cli_double_dash()) continue; else

#define cli_new_opt VRG_join(cli_opt_,__LINE__)

#define cliopt(...) vrg(cli_opt_,__VA_ARGS__)
#define cli_opt_1(cli_def) cli_opt_2(cli_def, cli_chk_true)
#define cli_opt_2(cli_def, cli_chk) \
    static cli_option_t cli_new_opt; \
    if (cli_opt_found) continue; \
    else if (!( (clindx == 0 && cli_opt_define(cli_def, &cli_new_opt, cli_chk)) \
              ||(clindx >  0 && (cli_opt_found = cli_check(&cli_new_opt, cli_chk)) > 0))); \
         else

#define cli_opt_0()  \
    if (clindx == 0) continue; \
    if (cli_opt_found <= 0) break;\
  } \
  goto cli_last; cli_last: \
  if (clindx >= cliargc || cli_opt_found < 0) cli_last_check(); \
  else for (cliarg = cliargv[clindx], cli_k = 1; cli_k; cli_k++) \
         if (cli_k == 2) {clindx++; goto cli_loop;} \
         else

#define cliexit() if (!(cli_opt_found = -1)); else goto cli_last

#endif // CLI_VERSION
