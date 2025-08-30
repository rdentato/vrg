//.  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
//.  SPDX-License-Identifier: MIT

#include "vrg.h"

/* ---------- Fixed-arity stubs that expand to integer constants ---------- */
#define T_0()                        0
#define T_1(a)                       1
#define T_2(a,b)                     2
#define T_3(a,b,c)                   3
#define T_4(a,b,c,d)                 4
#define T_5(a,b,c,d,e)               5
#define T_6(a,b,c,d,e,f)             6
#define T_7(a,b,c,d,e,f,g)           7
#define T_8(a,b,c,d,e,f,g,h)         8
#define T_9(a,b,c,d,e,f,g,h,i)       9
#define T(...)                       vrg(T_, __VA_ARGS__)

/* Two-way split variant: 0 args -> U_0(), else -> U__(...) */
#define U_0()                        0xAA
#define U__(...)                     0xBB
#define U(...)                       vrg_(U_, __VA_ARGS__)

/* ---------- Helper macros to exercise edge cases ---------- */
#define NOTHING            /* expands to empty */
#define ONE                1
#define TWO                2
#define SPLIT              1,2                /* expands to two separate args */
#define PARENS_ONE         (42)               /* single arg starting with '(' */
#define CAST_ONE           (int)7             /* cast as first token */
#define TUPLE(a,b)         (a,b)              /* parenthesized comma as single arg */
#define MAKE_PAIR          (1,2)              /* expands to a single (1,2) token */
#define ID(x)              x                  /* passthrough */
#define EMPTY_MACRO()      /* empty */

/* ---------- 1) Direct arity mapping 0..9 ---------- */
_Static_assert(T()                                  == 0, "vrg: 0 args -> T_0");
_Static_assert(T(1)                                 == 1, "vrg: 1 arg  -> T_1");
_Static_assert(T(1,2)                               == 2, "vrg: 2 args -> T_2");
_Static_assert(T(1,2,3)                             == 3, "vrg: 3 args -> T_3");
_Static_assert(T(1,2,3,4)                           == 4, "vrg: 4 args -> T_4");
_Static_assert(T(1,2,3,4,5)                         == 5, "vrg: 5 args -> T_5");
_Static_assert(T(1,2,3,4,5,6)                       == 6, "vrg: 6 args -> T_6");
_Static_assert(T(1,2,3,4,5,6,7)                     == 7, "vrg: 7 args -> T_7");
_Static_assert(T(1,2,3,4,5,6,7,8)                   == 8, "vrg: 8 args -> T_8");
_Static_assert(T(1,2,3,4,5,6,7,8,9)                 == 9, "vrg: 9 args -> T_9");

/* ---------- 2) Tricky single-arg forms (start with '(') ---------- */
_Static_assert(T((int)42)                           == 1, "vrg: single cast arg");
_Static_assert(T((1,2))                             == 1, "vrg: single parenthesized expr with comma");
_Static_assert(T(((void*)0))                        == 1, "vrg: single parenthesized pointer");
_Static_assert(T(PARENS_ONE)                        == 1, "vrg: macro -> (42)");
_Static_assert(T(CAST_ONE)                          == 1, "vrg: macro -> (int)7");
_Static_assert(T(TUPLE(3,4))                        == 1, "vrg: tuple macro as single arg");
/* Two parenthesized args are still two args */
_Static_assert(T((a),(b))                           == 2, "vrg: two parenthesized args");

/* ---------- 3) Macros that expand to commas or empty ---------- */
/* SPLIT expands to “1,2” -> two args after expansion */
_Static_assert(T(SPLIT)                             == 2, "vrg: macro expanding to 2 args");
/* NOTHING expands to empty -> T() (0 args) */
_Static_assert(T(NOTHING)                           == 0, "vrg: macro expanding to empty -> 0 args");
/* EMPTY_MACRO() as argument: expands to empty -> 0 args */
_Static_assert(T(EMPTY_MACRO())                     == 0, "vrg: empty function-like macro -> 0 args");
/* ID(...) shouldn’t change counts */
_Static_assert(T(ID(5))                             == 1, "vrg: ID passthrough (1)");
_Static_assert(T(ID(5), ID(6))                      == 2, "vrg: ID passthrough (2)");
/* MAKE_PAIR expands to (1,2): single arg */
_Static_assert(T(MAKE_PAIR)                         == 1, "vrg: macro -> (1,2) is one arg");

/* ---------- 4) Two-way split: vrg_ (0 vs ≥1) ---------- */
_Static_assert(U()                                  == 0xAA, "vrg_: 0 args -> U_0");
_Static_assert(U(1)                                 == 0xBB, "vrg_: ≥1 args -> U__");
_Static_assert(U((int)1)                            == 0xBB, "vrg_: ≥1 args (cast) -> U__");
_Static_assert(U(1,2)                               == 0xBB, "vrg_: ≥1 args (multi) -> U__");
_Static_assert(U(SPLIT)                             == 0xBB, "vrg_: macro expanding to multi -> U__");
_Static_assert(U(NOTHING)                           == 0xAA, "vrg_: macro expanding to empty -> U_0");

/* ---------- 5) Raw counter sanity checks ---------- */
_Static_assert(VRG_nargs()                          == 1, "VRG_nargs: 1");
_Static_assert(VRG_nargs(a)                         == 1, "VRG_nargs: 1");
_Static_assert(VRG_nargs(a,b)                       == 2, "VRG_nargs: 2");
_Static_assert(VRG_nargs((int)x)                    == 1, "VRG_nargs: 1 cast");
_Static_assert(VRG_nargs((x,y))                     == 1, "VRG_nargs: 1 parenthesized comma");
_Static_assert(VRG_nargs(1,2,3,4,5,6,7,8,9)         == 9, "VRG_nargs: 9");

#define X_ 0
#define X1 1

_Static_assert(VRG_cat(X,VRG_ncommas()                 )         == 0, "VRG_ncommas: 0");
_Static_assert(VRG_cat(X,VRG_ncommas(a)                )         == 0, "VRG_ncommas: 0");
_Static_assert(VRG_cat(X,VRG_ncommas(a,b)              )         == 1, "VRG_ncommas: 1");
_Static_assert(VRG_cat(X,VRG_ncommas((int)x)           )         == 0, "VRG_ncommas: 0 cast");
_Static_assert(VRG_cat(X,VRG_ncommas((x,y))            )         == 0, "VRG_ncommas: 0 parenthesized comma");
_Static_assert(VRG_cat(X,VRG_ncommas(1,2,3,4,5,6,7,8,9))         == 0, "VRG_ncommas: 0");

/* ======================= 6) Nested vrg-in-vrg tests ======================= */
/* OUTER dispatcher (like T) */
#define OUT_0()                        100
#define OUT_1(a)                       101
#define OUT_2(a,b)                     102
#define OUT_3(a,b,c)                   103
#define OUT(...)                       vrg(OUT_, __VA_ARGS__)

/* INNER1 forwards args -> OUT sees the same arity */
#define IN1_0()                        /* empty -> 0 args to caller */
#define IN1_1(a)                       a
#define IN1_2(a,b)                     a,b
#define IN1_3(a,b,c)                   a,b,c
#define IN1(...)                       vrg(IN1_, __VA_ARGS__)

/* INNER2 collapses to a single parenthesized tuple (x,y,...) -> 1 arg */
#define IN2_0()                        (/*empty*/)          /* still 1 arg: () token */
#define IN2_1(a)                       (a)
#define IN2_2(a,b)                     (a,b)
#define IN2_3(a,b,c)                   (a,b,c)
#define IN2(...)                       vrg(IN2_, __VA_ARGS__)

/* INNER3 uses vrg_: 0 args -> empty (0 to caller), else passthrough */
#define IN3_0()                        /* empty */
#define IN3__(...)                     __VA_ARGS__
#define IN3(...)                       vrg_(IN3_, __VA_ARGS__)

/* ---- Assertions: INNER -> OUTER ---- */
_Static_assert(OUT(IN1())                   == 100, "nested: OUT(IN1()) -> OUT_0");
_Static_assert(OUT(IN1(7))                  == 101, "nested: OUT(IN1(1)) -> OUT_1");
_Static_assert(OUT(IN1(7,8))                == 102, "nested: OUT(IN1(1,2)) -> OUT_2");
_Static_assert(OUT(IN1(7,8,9))              == 103, "nested: OUT(IN1(1,2,3)) -> OUT_3");

/* IN2 collapses to one parenthesized token -> OUT always sees 1 arg */
_Static_assert(OUT(IN2())                   == 101, "nested: OUT(IN2()) -> OUT_1");
_Static_assert(OUT(IN2(1))                  == 101, "nested: OUT(IN2(1)) -> OUT_1");
_Static_assert(OUT(IN2(1,2))                == 101, "nested: OUT(IN2(1,2)) -> OUT_1");
_Static_assert(OUT(IN2(1,2,3))              == 101, "nested: OUT(IN2(1,2,3)) -> OUT_1");

/* IN3 uses vrg_: 0 args -> empty (0 to caller), else passthrough */
_Static_assert(OUT(IN3())                   == 100, "nested: OUT(IN3()) -> OUT_0");
_Static_assert(OUT(IN3(5))                  == 101, "nested: OUT(IN3(1)) -> OUT_1");
_Static_assert(OUT(IN3(5,6))                == 102, "nested: OUT(IN3(1,2)) -> OUT_2");

/* Deeper nesting: arity flows through expansions */
_Static_assert(OUT(IN1(IN1(1,2), 3))        == 103, "nested: OUT((1,2),3) -> OUT_3");
_Static_assert(OUT(IN1(IN2(1,2), 3))        == 102, "nested: OUT(( (1,2) ),3) -> OUT_2 (1+1)");
_Static_assert(OUT(IN2(IN1(1,2)))           == 101, "nested: OUT( (1,2) ) -> OUT_1");
_Static_assert(OUT(IN3(IN3()))              == 100, "nested: OUT( empty ) -> OUT_0");
_Static_assert(OUT(IN3(IN1(1,2)))           == 102, "nested: OUT(1,2) -> OUT_2");

/* Helper that returns constants like T, to check outer selection quickly */
#define C_0()                          
#define C_1(a)                         1
#define C_2(a,b)                       2,2
#define C_3(a,b,c)                     3,3,3
#define C(...)                         vrg(C_, __VA_ARGS__)

/* Mix with constant-producing inner to verify full path */
_Static_assert(OUT(C())                     == 100, "nested: OUT(C()) -> OUT_0");
_Static_assert(OUT(C(1))                    == 101, "nested: OUT(C(1)) -> OUT_1");
_Static_assert(OUT(C(1,2))                  == 102, "nested: OUT(C(1,2)) -> OUT_2");

/* Triple nesting */
_Static_assert(OUT(IN1(IN1(IN1(1),2),3))    == 103, "nested: OUT -> 3 args after expansions");
_Static_assert(OUT(IN2(IN2(1,2)))           == 101, "nested: OUT of tuple(tuple) -> 1 arg");

/* ======================= Optional runtime smoke test ====================== */

#include "tst.h"

tstsuite("Static assertions") {
  tstcheck("Static assertions succeded");
}