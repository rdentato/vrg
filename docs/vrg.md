# Programmer’s Manual for `vrg()` and `vrg_()`

This manual explains how to use the `vrg()` and `vrg_()` macros to emulate **optional arguments** and **macro overloading by arity** in portable C (C11 preprocessor). It also dives into how the selection works, edge cases, and how to extend the max arity.

> **Scope:** The API lives entirely in macros. There’s no linker/runtime cost—selection happens at preprocess time.

---

## 1) Quick Start

### 1.1 Overload by number of arguments: `vrg()`

Use `vrg(prefix_, ...)` to dispatch to `prefix_0`, `prefix_1`, …, `prefix_9` depending on how many arguments you pass.

```c
#include "vrg.h"

/* Implementation target with defaults handled here */
static int my_func(int a, char b, void *c);

/* Public “variadic” front: choose by arity */
#define myfunc(...)  vrg(myfunc_, __VA_ARGS__)

/* Arity-specialized entry points */
#define myfunc_0()             my_func(0, '\0', NULL)
#define myfunc_1(a)            my_func(a, '\0', NULL)
#define myfunc_2(a,b)          my_func(a, b, NULL)
#define myfunc_3(a,b,c)        my_func(a, b, c)

/* Usage */
myfunc();                  // -> myfunc_0() -> my_func(0,'\0',NULL)
myfunc(42);                // -> myfunc_1(42)
myfunc(42, 'x');           // -> myfunc_2(42,'x')
myfunc(1, 'y', &obj);      // -> myfunc_3(1,'y',&obj)
```

**What it does:** At preprocess time, `vrg()` **counts** the number of arguments and *token-pastes* that count onto your `prefix_`, then calls that macro.

---

### 1.2 Only distinguish “no args” vs “one or more”: `vrg_()`

Use `vrg_(prefix_, ...)` to pick **`prefix_0`** when there are **no arguments**, and **`prefix__`** (two underscores) for **1+ arguments**.

```c
#include "vrg.h"

static int your_func(int a, char b, void *c);

/* Public front */
#define yourfunc(...)  vrg_(yourfunc_, __VA_ARGS__)

/* Two cases only */
#define yourfunc_0()        your_func(0,'\0',NULL)   // zero-arg default
#define yourfunc__(...)     your_func(__VA_ARGS__)   // pass-through (1+ args)

/* Usage */
yourfunc();                 // -> yourfunc_0()
yourfunc(5);                // -> yourfunc__(5)
yourfunc(5, 'q');           // -> yourfunc__(5,'q')
```

**When to use:** When you don’t care *how many* arguments there are—only whether there are **none** or **some**.

---

## 2) API Reference

### 2.1 Public selectors

* `vrg(f_, ...)`

  * Expands to `f_0(__VA_ARGS__)` for zero arguments; `f_1(__VA_ARGS__)`, …, up to `f_9`.
  * **Limit:** up to **9** arguments out of the box (configurable; see §7.1).

* `vrg_(f_, ...)`

  * Expands to `f_0()` for zero arguments; `f__(__VA_ARGS__)` for **one or more** arguments.

### 2.2 Required arity targets

* For `vrg(f_, ...)`, you should define the arity-specific macros you intend to support:

  * e.g., `f_0`, `f_1`, `f_2`, … up to your max arity.
  * If a call resolves to an undefined `f_k`, you’ll get a **preprocessor/compile error**.

* For `vrg_(f_, ...)`, define:

  * `f_0` (zero-arg case)
  * `f__` (the “one or more args” case)

---

## 3) Design Patterns & Recipes

### 3.1 Optional defaults routed to one implementation

Keep one real function and initialize missing arguments via the arity macros.

```c
/* One canonical implementation */
static int open_file_impl(const char *path, const char *mode, int flags);

/* User-facing macro chooser */
#define open_file(...) vrg(open_file_, __VA_ARGS__)

/* Defaults: mode="r", flags=0 */
#define open_file_1(path)            open_file_impl(path, "r", 0)
#define open_file_2(path, mode)      open_file_impl(path, mode, 0)
#define open_file_3(path, mode, f)   open_file_impl(path, mode, f)
#define open_file_0()                open_file_impl(NULL, "r", 0) /* if you want */
```

### 3.2 Overloaded “constructors” for a struct

```c
typedef struct { int x, y; } point_t;
static inline point_t point_xy(int x, int y) { return (point_t){x,y}; }

#define point(...) vrg(point_, __VA_ARGS__)
#define point_0()              point_xy(0,0)
#define point_1(x)             point_xy(x,0)
#define point_2(x,y)           point_xy(x,y)

/* Usage */
point();         // {0,0}
point(3);        // {3,0}
point(3,4);      // {3,4}
```

### 3.3 Thin wrappers: “none vs some” (`vrg_`)

```c
/* A logging facade where no-arg means “newline only” */
void log_impl(const char *fmt, ...);

#define logln(...) vrg_(logln_, __VA_ARGS__)
#define logln_0()          log_impl("\n")
#define logln__(...)       log_impl(__VA_ARGS__), log_impl("\n")
```

### 3.4 Interoperate with casts and macro arguments

```c
#define W(x) printf("[%d]\n", (x))

#define show(...) vrg(show_, __VA_ARGS__)
#define show_0()           W(0)
#define show_1(a)          W(a)
#define show_2(a,b)        (W(a), W(b))

show((int)7);   // -> show_1((int)7)   OK
show(1, (int)2) // -> show_2(1,(int)2) OK
```

> **Note:** Whether the first argument is cast or not does not change the selected arity in practice; the internal tests only exist to reliably detect the **zero-argument** case.

---

## 4) How It Works (Under the Hood)

You don’t have to understand this to use it, but it helps when customizing or debugging.

### 4.1 Building blocks

* **Token join with extra expansion**

  ```c
  #define VRG_exp(...)    __VA_ARGS__
  #define VRG_jn(x,y)     VRG_exp(x ## y)
  #define VRG_join(x,y)   VRG_jn(x, y)
  ```

  Some preprocessors (notably MSVC) need the `VRG_exp` “extra expansion” so that pasted tokens reflect previous macro expansions.

* **Argument counting (up to 9)**

  ```c
  #define VRG_count(x1,x2,x3,x4,x5,x6,x7,x8,x9,xA,xN, ...) xN
  #define VRG_nargs(...)   VRG_exp(VRG_count(__VA_ARGS__, A, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
  ```

  This returns `1..9` for 1..9 args. **When there are no args, it returns `1`** (on purpose). We’ll fix that shortly with the “comma trick.”

* **Comma trick**

  ```c
  #define VRG_ncommas(...) VRG_exp(VRG_count(__VA_ARGS__, _, _, _, _, _, _, _, _, 1, _, _))
  #define VRG_comma(...)   ,
  ```

  We compute two probes:

  * `VRG_ncommas(VRG_comma __VA_ARGS__ ())`
  * `VRG_ncommas(VRG_comma __VA_ARGS__)`

  and then paste their results into a two-character pattern: `1_`, `11`, or `__`.

  The key property exploited is how the preprocessor treats a macro name followed (or not) by parentheses, and how commas behave at top level vs inside parentheses. After enumerating cases, only the **no-argument** case yields the pattern `1_`. Everything else (1+ args), whether the first arg is cast or not, yields `11` or `__`—which we treat the same.

* **Selector**

  ```c
  #define VRG_fn_sel(ret,...) \
    VRG_join(VRG_fn_, \
      VRG_join(VRG_ncommas(VRG_comma __VA_ARGS__ ()), \
               VRG_ncommas(VRG_comma __VA_ARGS__))) (ret)

  #define VRG_fn_1_(ret) 0   /* no args -> 0 */
  #define VRG_fn_11(ret) ret /* 1+ args -> ret */
  #define VRG_fn___(ret) ret /* 1+ args -> ret */
  ```

  So:

  * **No arguments:** returns `0`
  * **1+ arguments:** returns `ret`

  Now choose `ret` to fit the flavor:

  * In `vrg(f_, ...)`, we pass `ret = VRG_nargs(__VA_ARGS__)`, giving `0` (after fixup) … or `1..9`.
  * In `vrg_(f_, ...)`, we pass `ret = _`, making the two outcomes `0` or `_`.

* **Final public macros**

  ```c
  #define vrg(f_,...)  VRG_join(f_, VRG_fn_sel(VRG_nargs(__VA_ARGS__),__VA_ARGS__))(__VA_ARGS__)
  #define vrg_(f_,...) VRG_join(f_, VRG_fn_sel(_,                      __VA_ARGS__))(__VA_ARGS__)
  ```

---

## 5) Rules, Limits, and Portability

### 5.1 Arity limit (default: 9)

Out of the box, `vrg()` supports up to **9** arguments. Calls with more will fall into the “other cases,” and the count will no longer be correct. Extend it if needed (see §7.1).

### 5.2 Top-level commas must separate arguments

The preprocessor splits arguments on **top-level commas**. If you need to pass something containing commas, wrap it in parentheses so it’s one argument:

```c
/* Struct literal -> wrap it */
call((struct S){1,2});          // one argument
call((int[2]){1,2}, 3);         // two arguments: array literal + 3
```

Strings are fine; commas inside string literals do **not** split arguments.

### 5.3 Works with casts

The internal detection takes casts into account to keep **zero-argument detection** reliable. In practice, both casted and uncasted single arguments still select arity 1. You don’t need to care; it “just works.”

### 5.4 MSVC/Clang/GCC compatibility

The `VRG_exp` step ensures **MSVC `cl`** performs the extra expansion needed for correct token pasting. The technique is known to work across major compilers that support standard variadic macros (C99+).

### 5.5 Side effects and multiple use of `__VA_ARGS__`

`__VA_ARGS__` appears more than once in the selector machinery, but **only in macro space**. There is **no runtime duplication** of side effects. Still, keep your arguments syntactically valid in all positions (e.g., balance parentheses), because they’re re-inserted into macro probes.

---

## 6) Diagnostics & Common Pitfalls

* **“Unknown identifier `f_5`”**
  You called `vrg(f_, ...)` with 5 args, but didn’t define `f_5`. Define it or restrict callers.

* **“Too many arguments”**
  You exceeded the configured max arity (default 9). Extend `VRG_count` (see below).

* **Unbalanced parentheses**
  Because your arguments are reused in macro probes, stray parentheses can break expansion. Make sure each argument is a syntactically valid token sequence by itself.

* **Commas inside arguments**
  Always parenthesize comma-containing constructs (initializer lists, macro-expanding to multiple comma-separated items) so they stay a **single** argument.

---

## 7) Customization

### 7.1 Extending to more than 9 arguments

You must:

1. Extend the `VRG_count` arity list, and
2. Adjust `VRG_nargs` and `VRG_ncommas` sequences accordingly.

For example, to support up to **16** args:

```c
/* 1) Extend VRG_count to enough slots */
#define VRG_count(x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,xA,xN, ...) xN

/* 2) Extend VRG_nargs: the “suffix ladder” is: A, 16, 15, ..., 1, 0 */
#define VRG_nargs(...) \
  VRG_exp(VRG_count(__VA_ARGS__, A, 16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))

/* 3) VRG_ncommas needs at least enough slots to detect a single comma. 
      Leave the tail “..., 1, _, _” intact and pad with _ as needed.
*/
#define VRG_ncommas(...) \
  VRG_exp(VRG_count(__VA_ARGS__, _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_, 1, _, _))
```

You can now define `f_0` .. `f_16` and call `vrg(f_, ...)` with up to 16 args.

> **Tip:** If you never need exact counts beyond deciding between **0** and **1+**, prefer `vrg_()`—it doesn’t need extension.

---

## 8) Practical Examples

### 8.1 Small “overloaded” API

```c
/* Append to a vector with convenient defaults */
int vec_pushn(void *vec, const void *data, size_t n);

#define push(...) vrg(push_, __VA_ARGS__)
#define push_1(v)              vec_pushn(v, NULL, 1)   /* reserve 1 */
#define push_2(v, x)           vec_pushn(v, &(x), 1)   /* push one */
#define push_3(v, p, n)        vec_pushn(v, (p), (n))  /* push many */

push(v);                // reserve one slot
push(v, value);         // push one value
push(v, values, n);     // push n values
```

### 8.2 A “printf-like” wrapper with optional tag

```c
void log_impl(const char *tag, const char *fmt, ...);

#define log(...) vrg(log_, __VA_ARGS__)
#define log_1(msg)             log_impl("INFO", "%s", (msg))
#define log_2(tag, msg)        log_impl((tag), "%s", (msg))
#define log_3(tag, fmt, ...)   log_impl((tag), (fmt), __VA_ARGS__)

log("start");                       // INFO: start
log("DBG", "x=%d", x);              // DBG: x=...
```

### 8.3 “No args vs some args” batch command

```c
void run_all(void);
void run_one(const char *name);

#define run(...) vrg_(run_, __VA_ARGS__)
#define run_0()         run_all()
#define run__(...)      run_one(__VA_ARGS__)

run();                 // run_all()
run("task42");         // run_one("task42")
```

---

## 9) FAQ

**Q:** *Why does `VRG_nargs()` return 1 when there are zero arguments?*
**A:** That’s a deliberate quirk to keep the counting simple across compilers. The framework then **corrects** the zero-arg case via the “comma trick” and `VRG_fn_1_ -> 0`. Net effect: you get **0** for no args and **1..9** otherwise.

**Q:** *Does a cast on the first argument break detection?*
**A:** No. The probes handle cast/non-cast uniformly for arity selection. You’ll still get the right `f_k`.

**Q:** *Any runtime overhead?*
**A:** None. It’s pure preprocessor work; the emitted code is exactly the chosen `f_k(__VA_ARGS__)`.

**Q:** *Can I call functions or do work in the selector?*
**A:** No—selection happens before compilation. Keep “work” in the arity-specific macros/functions.

**Q:** *Do I need C23 `__VA_OPT__`?*
**A:** No. This library targets portable C11 preprocessors and avoids `__VA_OPT__`.

---

## 10) Troubleshooting Checklist

* **Compile error near `VRG_join` or `##`:**
  Make sure you included the header **before** using `vrg()`/`vrg_()`. Also check that your `f_k` macro names (including underscores) match *exactly*.

* **“Macro pasting produces invalid tokens”:**
  Look for stray punctuation in your prefix (e.g., spaces in `my func_`). The prefix must be a clean token like `myfunc_`.

* **Arity mismatches:**
  If you pass 4 args, you must define `f_4`. If you don’t want to support certain arities, trap them intentionally:

  ```c
  #define f_4(...)  _Static_assert(0, "f_4 not supported")
  ```

* **Arguments with commas:**
  Parenthesize them to make them one argument.

---

## 11) Reference: What’s in the Header

* **Joining & expansion:** `VRG_join`, `VRG_jn`, `VRG_exp`
* **Counting:** `VRG_count`, `VRG_nargs`, `VRG_ncommas`, `VRG_comma`
* **Selector core:** `VRG_fn_sel`, `VRG_fn_1_`, `VRG_fn_11`, `VRG_fn___`
* **Public API:** `vrg(f_, ...)`, `vrg_(f_, ...)`

You normally only use **`vrg`** and **`vrg_`** and define your `prefix_0`, `prefix_1`, … or `prefix__` macros.

---

## 12) Best Practices

* **Route to one real function** that applies defaults; keep logic centralized.
* **Keep `f_k` tiny** (ideally one-liners) to avoid duplication and accidental divergence between arities.
* **Be explicit about max supported arity** in comments; extend if/when needed.
* **Parenthesize complex arguments** (struct/array literals, comma-containing macros).
* **Name consistently:** choose a stable `prefix_` (e.g., `parse_`, `vecpush_`, `open_`).

---

## 13) Minimal Self-Test (drop-in)

```c
/* Expect which macro fires; compile to see preprocessed output or run to observe prints */
#include <stdio.h>
#include "vrg.h"

#define PFX(...) vrg(PFX_, __VA_ARGS__)
#define PFX_0()        puts("0")
#define PFX_1(a)       puts("1")
#define PFX_2(a,b)     puts("2")
#define PFX_3(a,b,c)   puts("3")

#define QFX(...) vrg_(QFX_, __VA_ARGS__)
#define QFX_0()        puts("Q0")
#define QFX__(...)     puts("Q+")
int main(void) {
  PFX();               // 0
  PFX(1);              // 1
  PFX((int)2);         // 1
  PFX(1,2);            // 2
  PFX((int)1,2);       // 2
  QFX();               // Q0
  QFX(7);              // Q+
  QFX(7,8);            // Q+
  return 0;
}
```

---

## 14) Summary

* Use **`vrg()`** when you want **exact arity selection** (`_0.._9`).
* Use **`vrg_()`** when you only care about **zero** vs **one or more** (`_0` vs `__`).
* Parenthesize arguments that contain commas.
* Extend the internal counters if you need more than 9 arguments.
* Enjoy zero-runtime-cost, portable “optional arguments” in C.
