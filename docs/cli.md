# `cli` — Programmer’s Manual (single-header C library for Command-Line Interfaces)

> A tiny, declarative way to parse command lines in C.
> Unlike `getopt()`, each option is fully specified **as a string** and you attach **the code to run** when that flag/argument is seen. Flags and positional args can appear in **any order**.

---

## 1) Quick start

```c
#include "cli.h"

int main(int argc, char **argv) {
  int verbose = 0, xrays = 0;
  const char *infile = NULL, *outfile = NULL;

  clioptions("my program", argc, argv) {
    cliopt("-h\tThis help") {
      cliusage(CLIEXIT);              // print auto-generated help and exit
    }

    cliopt("-v, --verbose\tBe verbose") {
      verbose++;
    }

    cliopt("-x, --xrays n\tActivate n rays for the x-ray function") {
      xrays = atoi(cliarg);           // cliarg is the matched argument
    }

    cliopt("-O, --optimize [level]\tOptimize up to the specified level") {
      int level = cliarg ? atoi(cliarg) : 1; // optional arg
      /* ... */
    }

    cliopt("data-file\tThe input file") {     // required positional
      infile = cliarg;
    }

    cliopt("[output-file]\tThe output file") { // optional positional
      outfile = cliarg;
    }

    cliopt("-T, --temperature temp (42)\tSet temperature (default 42)") {
      /* Runs when default is materialized AND when user passes a value.
         Detect which with cliisdefault(). */
      if (!cliisdefault()) { /* ... */ }
    }

    cliopt("-L, --luminosity lum ($LUMEN,100)\tLuminosity from env or 100") {
      /* If $LUMEN is set, its value is used; otherwise 100. */
    }

    cliopt("<add>\tAdd items") {              // command (must be first arg)
      /* ... */
    }

    cliopt("<list> type\tList items of a given type") {
      /* ... */
    }

    cliopt("<sort> [direction]\tSort with optional direction") {
      /* ... */
    }

    cliopt() {                                // mandatory catch-all, last
      if (cliarg[0] == '!') cliexit();        // stop scanning
      /* else: unknown/extra arg → ignore or error */
    }
  }

  /* clindx holds index of next unprocessed argv[] (for your own parsing) */
  /* ... do the rest of the program ... */
  return 0;
}
```

---

## 2) Concepts & flow

* **Declarative spec**: each `cliopt("spec\tDescription") { /* handler */ }` declares *both* how the option looks and what the program should do when it appears.
* **Order independence**: flags and positionals can be interleaved arbitrarily.
* **Immediate actions**: the handler block executes as soon as a match is found while scanning `argv`.
* **Default clause**: a final `cliopt()` with empty spec **must** exist and be last; it runs for anything not matched so far.
* **Auto-help**: `cliusage()` is generated from your specs and descriptions (text after `\t`).
* **Short-option grouping**: `-xvz` ≡ `-x -v -z`. If one of them **takes an argument**, it must be the last in the group (e.g., `-vx3` ok if `-x` takes `3`).
* **--**: double dash stops the flags and commands to be recognized but continues to process positional arguments. Use `cliexit()` if you want to exit the CLI altogether.

---

## 3) The option-string format

Use a tab `\t` to separate the **spec** (left) from the **help text** (right).

### 3.1 Tokens (left side of `\t`)

* **Short flag**: `-v`
* **Long flag**: `--verbose`
* **Both**: `-v, --verbose`
* **Required argument** (for a flag): append a name: `-x n` or `--xrays n`
* **Optional argument** (for a flag): put name in brackets: `-O [level]`
* **Required positional**: a bare name: `data-file`
* **Optional positional**: `[output-file]`
* **Command**: `<add>` (must appear as the **first** non-program arg)
* **Command arguments**: append after the command: `<list> type`, `<sort> [direction]`
* **Default value**: `(42)` → if no user value is provided, 42 is used
* **Env default**: `($LUMEN,100)` → if `$LUMEN` set use it, else `100`

You may combine pieces, e.g.:

```
"-L, --luminosity lum ($LUMEN,100)\tLuminosity"
"-T, --temperature temp (42)\tDefault 42"
```

### 3.2 Formal grammar (EBNF, left side)

```
Spec        := (Command | Positional | Flag) "\t"+ Help?
Flag        := ShortLong OptArg?
ShortLong   := Short ("," WS? Long)? | Long | ShortGroup
Short       := "-" Letter
ShortGroup  := "-" Letter Letter{1,}
Long        := "--" Ident
OptArg      := WS (ArgReq | ArgOpt) Default?
ArgReq      := Ident
ArgOpt      := "[" Ident "]"
Default     := WS? "(" ( EnvDefault | Literal ) ")"
EnvDefault  := "$" Ident ("," Literal)?
Positional  := PosReq | PosOpt
PosReq      := Ident
PosOpt      := "[" Ident "]"
Command     := "<" Ident ">" (WS ArgReq | WS ArgOpt)?
Ident       := [A-Za-z-][A-Za-z0-9-]*
Letter      := [A-Za-z0-9]
Literal     := any text up to ')'
WS          := spaces
```

---

## 4) Runtime API (what you use inside handlers)

* `char *cliarg`: pointer to the argument value if present; Points to a constant empty string for an optional arg not supplied. For positionals/commands, it’s the matched token (or command’s argument when applicable).
* `int clindx`: index into `argv` of the *next* item to process. You can `return clindx;` or stash it to process trailing values yourself.
* `int cliisdefault()`: true if the current handler is executing to *materialize a default* (from `(42)` or `($ENV,fb)`), false if it’s for a user-provided value.
* `void cliusage(int mode)`:  prints the auto-generated usage/help text. If called with `CLIEXIT`, it **exits** the program; otherwise it returns after printing.
* `cliexit()`: stop parsing immediately (returns out of the `clioptions` scanning loop to your code).
* `void clierror(const char *msg, const char *arg)` : print `msg` (optionally mentioning `arg`) and exit with an error status.

> **Note**: `cliusage()` and `clierror()` handle formatting consistently with how you wrote the specs.

---

## 5) Validators

Attach a validator after the spec string. Its signature is:

```c
char *validator(char *arg);
/* return NULL if OK; non-NULL => error message to show */
```

Example:

```c
static char *is_positive(char *arg) {
  int n = atoi(arg);
  return (n > 0) ? NULL : "Argument must be positive";
}

clioptions("myprogram", argc, argv) {
  cliopt("-x, --xrays n\tNumber of rays", is_positive) {
    /* This runs only if the validator passes */
    xrays = atoi(cliarg);
  }
  cliopt() { /* ... */ }
}
```

No validator? You can handle it manually:

```c
cliopt("-x, --xrays n\tNumber of rays") {
  int n = atoi(cliarg);
  if (n <= 0) clierror("Argument must be positive", cliarg);
  xrays = n;
}
```

---

## 6) Commands

* Declare with `<cmd>` as a *spec*.
* Commands must appear as the **first** argument after the program name to be recognized as such. They can be preceded by flags.
* You can give command-specific args: `<add> item`, `<sort> [direction]`.
* The handler block runs when the command token is encountered.

Typical pattern:

```c
cliopt("<list> [filter]\tList items, optionally filtered") {
  const char *flt = cliarg; // Empty string "" if omitted
  /* ... */
}
```

---

## 7) Defaults and environment

* `(42)` sets a default if the user didn’t supply the argument.
* `($NAME,fb)` tries environment variable `NAME`; if unset, uses `fb`.
* When the library *applies* a default, it runs your handler with `cliarg` pointing to the chosen string and `cliisdefault()` true. This lets you centralize initialization logic in one place.

---

## 8) Grouping short options

* `-abc` is expanded as `-a -b -c`.
* If one of them **takes an argument**, it must be **last**, and the arg may be attached or separate:

  * `-x23` or `-x 23` (ok if `-x` expects a value)
  * `-vx3` (ok when `-x` takes `3`)
  * `-xv` (ambiguous if `-x` needs an arg → **not allowed**)

---

## 9) Error handling & exit behavior

* `cliusage(CLIEXIT)` prints help and exits (typically with `0`).
* `clierror(msg, arg)` prints a formatted error and exits with a **non-zero** status.
* Missing required args, unknown options (if you choose to treat them as errors), or validator failures should call `clierror()`.

Typical unknown-option handling in the default clause:

```c
cliopt() {
  if (cliarg[0] == '-') clierror("Unknown option", cliarg);
  /* else it's an extra positional → accept or store for later */
}
```

---

## 10) Accessing remaining args

`clindx` gives you the current scan position in `argv`. After the `clioptions { ... }` block finishes (either naturally or because you called `cliexit()`), any unprocessed items are at/after `argv[clindx]`. This is useful for sub-parsers or when you allow free-form trailing arguments.

---

## 11) Patterns & recipes

### 11.1 Toggle with `-v` (count verbosity)

```c
int verbosity = 0;
cliopt("-v, --verbose\tIncrease verbosity") { verbosity++; }
```

### 11.2 Optional numeric level (`-O [level]`, default 1)

```c
int optlevel = 0;
cliopt("-O, --optimize [level] (1)\tOptimize") {
  if (cliarg) optlevel = atoi(cliarg);
}
```

### 11.3 Required input, optional output

```c
const char *in=NULL, *out=NULL;
cliopt("input\tInput file")   { in  = cliarg; }
cliopt("[output]\tOutput file"){ out = cliarg; }
```

### 11.4 Default from env (fallback 100)

```c
int lum = 0;
cliopt("-L, --luminosity lum ($LUMEN,100)\tLuminosity") {
  lum = atoi(cliarg); // runs once for default (cliisdefault()==1) or user value
}
```

---

## 12) Diagnostics & usage text

* The help text is auto-generated from every spec’s right-hand side (after `\t`).
* Keep descriptions concise and imperative.
* Example output is shaped roughly as:

  ```
  Usage: prog [options] data-file [output-file]
    -h                    This help
    -v, --verbose         Be verbose
    -x, --xrays n         Activate n rays for the x-ray function
    -O, --optimize [lvl]  Optimize up to the specified level
    ...
  Commands:
    add                   Add items
    list type             List items of a given type
    sort [direction]      Sort with optional direction
  ```

---

## 13) Portability & constraints

* Header-only; include `"cli.h"`. It is assumed it will be included only by one source file: the one where all the CLI parsing is done.
* Works with standard C compilation units; no global state required other than what **you** maintain in your handlers.
* Assumes typical `main(int argc, char **argv)` conventions and `getenv`, `atoi`, etc.
* Handlers are ordinary C blocks with full access to your program’s variables.

---

## 14) Gotchas & best practices

* **Always include a final `cliopt()`** with no spec; make it last.
* If you define **commands**, ensure they are checked before general positionals so they don’t get mistaken for filenames.
* For missing **optional arguments**, check `cliarg[0]` for `0`.
* When using defaults, gate side-effects if needed:

  ```c
  cliopt("-T temp (42)") {
    if (!cliisdefault()) apply_runtime_change(atoi(cliarg));
    else initial_temperature = atoi(cliarg);
  }
  ```
* Keep validators small and composable; wrap multiple checks into one function if needed.

---

## 15) Complete example with validators, defaults, commands

```c
#include "cli.h"
#include <stdio.h>
#include <stdlib.h>

static char *is_positive(char *s) {
  char *end; long v = strtol(s, &end, 10);
  if (*end) return "Not an integer";
  return (v > 0) ? NULL : "Must be > 0";
}

int main(int argc, char **argv) {
  int verbose=0, xrays=0, temp=0, adding = 0;
  const char *in=NULL, *out=NULL;

  clioptions("demo tool", argc, argv) {
    cliopt("-h\t\tShow help") { cliusage(CLIEXIT); }

    cliopt("-v, --verbose\tIncrease verbosity") { verbose++; }

    cliopt("-x, --xrays n\tNumber of rays", is_positive) { xrays = atoi(cliarg); }

    cliopt("-T, --temperature temp (42)\tSet temperature (default 42)") {
      int t = atoi(cliarg);
      if (cliisdefault()) temp = t;      // initialization path
      else                temp = t;      // user override path (same here)
    }

    cliopt("<add> item\tAdd a single item") {
      /* ... handle "add" ... */
      adding = 1;
    }

    cliopt("input\tInput file") { in = cliarg; }
    cliopt("[output]\tOutput file") { out = cliarg; }

    cliopt() {                            // unknown/extra
      if (cliarg[0] == '-') clierror("Unknown option", cliarg);
      // else: tolerate extra positionals or stop with cliexit()
    }
  }

  printf("add=%d verbose=%d xrays=%d temp=%d in=%s out=%s\n",
         adding, verbose, xrays, temp, in?in:"(none)", out?out:"(none)");
  return 0;
}
```

---

## 16) Reference (cheat sheet)

* **Blocks**

  * `clioptions(desc, [argc, argv]) { ... }`
  * `cliopt("spec\tHelp" [, validator]) { ... }`
  * `cliopt() { ... }`  // default/fallback; **must be last**

* **Runtime**

  * `char *cliarg;`          // current value (or a pointer to "" for missing optional)
  * `int  clindx;`          // index of next unprocessed argv
  * `int  cliisdefault(void);`      // true when running due to a default
  * `void cliusage(int mode);`      // prints usage; `CLIEXIT` to exit
  * `void cliexit(void);`           // stop parsing immediately
  * `void clierror(const char *msg, const char *arg);` // print error & exit
  * `void cliwarning(const char *msg, const char *arg);` // print error NO exit
  * `char *cliprogname;`  // Holds the name of the executable (argv[0] if NULL)
  * `#define CLIEXIT ...`            // pass to cliusage() to also exit

* **Spec features**

  * Short/long: `-v`, `--verbose`, `-v, --verbose`
  * Args: required `-x n`, optional `-O [level]`
  * Positionals: `name`, `[name]`
  * Commands: `<cmd>`, `<cmd> arg`
  * Defaults: `(42)`, `($ENV,fb)`
  * Grouping: `-abc`; if arg-taking flag present, it must be last.

