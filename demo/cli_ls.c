#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- State collected from CLI ------------------------------------------- */

typedef struct {
  int all;                 // -a
  int almost_all;          // -A
  int long_format;         // -l
  int human;               // -h (with -l)
  int sort_size;           // -S
  int sort_time;           // -t
  int reverse;             // -r
  int recursive;           // -R
  int list_dirs_as_files;  // -d
  int classify;            // -F / --classify
  int append_slash;        // -p (suffix / to dirs only)
  int inode;               // -i
  int one_per_line;        // -1
  int columns;             // -C
  int commas;              // -m
  int numeric_ids;         // -n
  int no_owner;            // -g (suppress owner)
  int no_group;            // -o (suppress group)
  int hide_nonprint;       // -q
  int time_access;         // -u (use atime)
  int time_status;         // -c (use ctime/status)
  int sort_ext;            // -X (sort by extension)
  int sort_version;        // -v (natural version)
  int deref_cmdline;       // -H
  int deref_all;           // -L
  int deref_never;         // -P
  int grp_dirs_first;      // --group-directories-first
  const char *color;       // --color[=WHEN] (auto|always|never)
  const char *block_size;  // --block-size=SIZE
  const char *time_style;  // --time-style=STYLE
  const char *quoting;     // --quoting-style=STYLE
  int width;               // --width=COLS
} ls_opts_t;

static ls_opts_t opt = {0};

/* collected FILE operands */
#define MAX_FILES 1024
static const char *files[MAX_FILES];
static int nfiles = 0;

/* ---- Validators ---------------------------------------------------------- */

static char *is_color_when(char *s) {
  if (!s) return NULL; // optional; missing is fine
  if (!strcmp(s, "auto") || !strcmp(s, "always") || !strcmp(s, "never")) return NULL;
  return "WHEN must be auto|always|never";
}

static char *is_uint(char *s) {
  if (!s || !*s) return "Missing number";
  for (const char *p=s; *p; ++p) if (*p<'0'||*p>'9') return "Not a positive integer";
  return NULL;
}

/* ---- Example CLI for `ls` ----------------------------------------------- */

int main(int argc, char **argv) {
  clioptions("ls - list directory contents", argc, argv) {
    /* Help */
    cliopt("-h, --help\t\tShow this help") { cliusage(CLIEXIT); }

    /* Basic listing style & filters */
    cliopt("-a, --all\t\tDo not ignore entries starting with .")        { opt.all = 1; }
    cliopt("-A, --almost-all\tLike -a but exclude . and ..")           { opt.almost_all = 1; }
    cliopt("-l\t\t\tUse a long listing format")                            { opt.long_format = 1; }
    cliopt("-H\t\t\tFollow symlinks on command line")                      { opt.deref_cmdline = 1; }
    cliopt("-L\t\t\tFollow all symlinks")                                  { opt.deref_all = 1; opt.deref_never = 0; }
    cliopt("-P\t\t\tNever follow symlinks")                                { opt.deref_never = 1; opt.deref_all = 0; }
    cliopt("-d\t\t\tList directories themselves, not their contents")      { opt.list_dirs_as_files = 1; }
    cliopt("-R, --recursive\tList subdirectories recursively")         { opt.recursive = 1; }

    /* Sorting & ordering */
    cliopt("-S\t\t\tSort by file size")                                    { opt.sort_size = 1; }
    cliopt("-t\t\t\tSort by modification time")                            { opt.sort_time = 1; }
    cliopt("-u\t\t\tUse access time for -t/-l")                            { opt.time_access = 1; opt.time_status = 0; }
    cliopt("-c\t\t\tUse status time for -t/-l")                            { opt.time_status = 1; opt.time_access = 0; }
    cliopt("-X\t\t\tSort alphabetically by entry extension")               { opt.sort_ext = 1; }
    cliopt("-v\t\t\tNatural sort of (version) numbers")                    { opt.sort_version = 1; }
    cliopt("-r, --reverse\t\tReverse sort order")                        { opt.reverse = 1; }

    /* Formatting */
    cliopt("-1\t\t\tList one file per line")                               { opt.one_per_line = 1; opt.columns = 0; }
    cliopt("-C\t\t\tList entries by columns")                              { opt.columns = 1; opt.one_per_line = 0; }
    cliopt("-m\t\t\tFill width with a comma separated list of entries")    { opt.commas = 1; }
    cliopt("-n\t\t\tList numeric user and group IDs")                      { opt.numeric_ids = 1; }
    cliopt("-g\t\t\tLike -l but do not list owner")                        { opt.no_owner = 1; opt.long_format = 1; }
    cliopt("-o\t\t\tLike -l but do not list group")                        { opt.no_group = 1; opt.long_format = 1; }
    cliopt("-i, --inode\t\tPrint the index number of each file")         { opt.inode = 1; }
    cliopt("-q\t\t\tHide non-printable chars (show as ?)")                 { opt.hide_nonprint = 1; }
    cliopt("-F, --classify\tAppend indicator (one of */=>@|) to entries"){ opt.classify = 1; }
    cliopt("-p\t\t\tAppend / indicator to directories only")               { opt.append_slash = 1; }

    /* GNU-style long options with optional/required args */
    cliopt("--color [when]\tColorize output (auto|always|never)", is_color_when) {
      opt.color = cliarg ? cliarg : "auto";  // default to auto if omitted
    }
    cliopt("--group-directories-first\tGroup directories before files") { opt.grp_dirs_first = 1; }
    cliopt("--block-size size\tScale sizes by SIZE (e.g., 1K, 1M, 1G)") { opt.block_size = cliarg; }
    cliopt("--time-style style\tTime/date style (e.g., full-iso, long-iso)") { opt.time_style = cliarg; }
    cliopt("--quoting-style style\tSet quoting style (literal, shell, etc.)") { opt.quoting = cliarg; }
    cliopt("--width cols\t\tAssume screen width COLS", is_uint)          { opt.width = atoi(cliarg); }

    /* Positional operands: FILE... (accept many) */
    cliopt("FILE\tFile or directory to list") {
      if (nfiles < MAX_FILES) files[nfiles++] = cliarg;
      else clierror("Too many files", cliarg);
    }

    /* End of options marker */
    cliopt() {
      if (strcmp(cliarg, "--") == 0) { cliexit(); }     // stop option parsing; remaining are files
      if (cliarg[0] == '-') clierror("Unknown option", cliarg);
      // treat any other token as another FILE operand
      if (nfiles < MAX_FILES) files[nfiles++] = cliarg;
      else clierror("Too many files", cliarg);
    }
  }

  /* ---- Demonstration: print what was parsed ------------------------------ */
  printf("Parsed %d file(s).\n", nfiles);
  for (int i = 0; i < nfiles; ++i) printf("  FILE[%d] = %s\n", i, files[i]);
  printf("Flags: a=%d A=%d l=%d h=%d S=%d t=%d r=%d R=%d d=%d F=%d p=%d i=%d 1=%d C=%d m=%d n=%d g=%d o=%d q=%d u=%d c=%d X=%d v=%d H=%d L=%d P=%d\n",
         opt.all,opt.almost_all,opt.long_format,opt.human,opt.sort_size,opt.sort_time,opt.reverse,opt.recursive,
         opt.list_dirs_as_files,opt.classify,opt.append_slash,opt.inode,opt.one_per_line,opt.columns,opt.commas,
         opt.numeric_ids,opt.no_owner,opt.no_group,opt.hide_nonprint,opt.time_access,opt.time_status,opt.sort_ext,
         opt.sort_version,opt.deref_cmdline,opt.deref_all,opt.deref_never);
  printf("--color=%s --group-dirs-first=%d --block-size=%s --time-style=%s --quoting-style=%s --width=%d\n",
         opt.color?opt.color:"(none)", opt.grp_dirs_first,
         opt.block_size?opt.block_size:"(none)",
         opt.time_style?opt.time_style:"(none)",
         opt.quoting?opt.quoting:"(none)",
         opt.width);

  return 0;
}
