//.  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
//.  SPDX-License-Identifier: MIT
#ifndef VRG_VERSION
#define VRG_VERSION 0x0010000B // 0.10.0-beta

//    oooooo     oooo ooooooooo.     .oooooo.    
//     `888.     .8'  `888   `Y88.  d8P'  `Y8b   
//      `888.   .8'    888   .d88' 888           
//       `888. .8'     888ooo88P'  888           
//        `888.8'      888`88b.    888     ooooo 
//         `888'       888  `88b.  `88.    .88'  
//          `8'       o888o  o888o  `Y8bood8P'   
 
// # Variadic functions
// 
// Say you want to define a variadic function with the following prototype:
// 
//     myfunc([int a [, char b [, void *c]]])
// 
// In other words, you want all arguments to be optional.
// 
// You can use the `vrg` macros to define your function and write specific 
// functions/macros one for when it is invoked with 0, 1, 2 or 3 paramenters.
// 
// Example1:
// 
//     #include "vrg.h"
//
//     /* Name your actual function differently (`my_func()`)*/
//     int my_func(int a, char b, void *c);
//
//     /* Use `vrg` to define your desired function (`myfunc()`) */
//     #define myfunc(...)  vrg(myfunc_, __VA_ARGS__)
//
//     /* Specify how to call the actual function when invoked with 0,1,2, ... arguments */
//     #define myfunc_0()       my_func(0,'\0',NULL)
//     #define myfunc_1(a)      my_func(a,'\0',NULL)
//     #define myfunc_2(a,b)    my_func(a,b,NULL)
//     #define myfunc_3(a,b,c)  my_func(a,b,c)
// 
// Note that the `0', `1`, `2`, etc, is appended to the name of the function.
// 
// Example2: Only discriminate between 0 and n parameters
// 
//     #include "vrg.h"
//
//     /* Name your actual function differently (`your_func()`)*/
//     int your_func(int a, char b, void *c);
//
//     /* Use `vrg_` to define your desired function (`yourfunc()`) */
//     #define yourfunc(...)  vrg_(yourfunc_, __VA_ARGS__)
//
//     /* Specify how to call the actual function when invoked with 0,1,2, ... arguments */
//     #define yourfunc_0()       your_func(0,'\0',NULL)
//     #define yourfunc__(...)    your_func(__VA_ARGS__)
// 


// ## Joining
// Creating a token from two parts mys be done in two steps to ensure proper expansion
// of the arguments.
#define VRG_jn(x,y)    VRG_exp(x ## y)
#define VRG_join(x,y)  VRG_jn(x, y)

// ## Extra Expansion
// Some compiler (notably MS cl) requires an extra step of expansion to ensure arguments
// are properly evaluated
#define VRG_exp(...) __VA_ARGS__

// ## Counting arguments
// The macro function `VRG_nargs()` counts how many arguments are present. If no
// argument is there it still returns 1.
// The macro function `VRG_ncommas()` counts how many commas are are present. 
// It returns 1 if there is only one comma and `_` in all other cases.
// The macro `VRG_count` allows up to 9 arguments for a function.

#define VRG_count(x1,x2,x3,x4,x5,x6,x7,x8,x9,xA,xN, ...) xN
#define VRG_nargs(...)   VRG_exp(VRG_count(__VA_ARGS__, A, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define VRG_ncommas(...) VRG_exp(VRG_count(__VA_ARGS__, _, _, _, _, _, _, _, _, 1, _, _))

// ## A comma
#define VRG_comma(...) ,

// ## Counting commas
//
// Unfortunately, just counting the number of arguments with `VRG_count()` is not enough
// because if there's no argument, the counting macro will still return 1.
// To discriminate when there is no argument from the case where there is one argument,
// we'll also count how many commas are present.
//
// To understand how everything works, you should focus on these two key expressions:
//  - `VRG_comma __VA_ARGS__ ()`
//  - 'VRG_comma __VA_ARGS__`
//
//  1- `VRG_ncommas(VRG_comma __VA_ARGS__ ())`. There are several cases:
//
//     a- No arguments: `__VA_ARGS__` is empty and the expression expands to `VRG_comma()` 
//        which expands to a single comma `,`. The macro `VRG_ncommas()` will expand to `1`
//     b- There is one argument and it does NOT start with '(' (i.e. is not type casted):
//        Then it will expand to `VRG_comma x ()` which will not expand further (because
//        `VRG_comma` by itself is not defined) and `VRG_ncommas` will expand to `_`
//     c- There is one argument and it does start with '(' (i.e. is type casted).
//        Then it will expand to `VRG_comma (t)x ()` which will become `, x ()` and
//        `VRG_ncommas` will expand to `1`
//     d- There are two arguments and the first one does NOT start with '(' (no type cast): 
//        Then it will expand to `VRG_comma x,y ()` and `VRG_ncommas` will expand to `1`
//     e- There are two arguments and the first one does  start with '(' (type cast): 
//        Then it will expand to `VRG_comma (x),y ()` which will expand to `,,y()` and
//        `VRG_ncommas` will expand to `_`
//     f- In all the other cases, the expressions exapnds to `_` because there is more than 1 comma.
//
//  2- 'VRG_ncommas(VRG_comma __VA_ARGS__)`. Note that the difference with expression 1
//     is that there is no `()` at the end. Considering the same cases as above:
//
//     a- No arguments: `__VA_ARGS__` is empty and the expression expands to `VRG_comma`. The macro
//        `VRG_ncommas()` will expand to `_`
//     b- There is one argument and it does NOT start with '(' (i.e. is not type casted):
//        Then it will expand to `VRG_comma x` and `VRG_ncommas` will expand to `_`
//     c- There is one argument and it does start with '(' (i.e. is type casted).
//        Then it will expand to `VRG_comma (t)x` which will become `, x` and
//        `VRG_ncommas` will expand to `1`
//     d- There are two arguments and the first one does NOT start with '(' (no type cast): 
//        Then it will expand to `VRG_comma x,y` and `VRG_ncommas` will expand to `1`
//     e- There are two arguments and the first one does  start with '(' (type cast): 
//        Then it will expand to `VRG_comma (x),y` which will expand to `,,y` and
//        `VRG_ncommas` will expand to `_`
//     f- In all the other cases, the expressions exapnds to `_` because there is more than 1 comma.
//
//  Putting together these cases with the expression, we'll get:
//  
//  `VRG_join(VRG_ncommas(VRG_comma __VA_ARGS__ ()), VRG_ncommas(VRG_comma __VA_ARGS__ ))`
//                    │                                     │
//                    ╰────────────────╮   ╭────────────────╯
//                                     │   │
//                                     ▽   ▽
//        a- No arguments:            `1` `_`
//        b- One argument (no cast):  `_` `_`
//        c- One argument (cast):     `1` `1`
//        d- Two arguments (no cast): `1` `1`
//        e- Two arguments (cast):    `_` `_`
//        e- All the other cases:     `_` '_' 
//
//  we have only three possible cases:
// 
//   - `1_` no arguments.
//   - `11` one casted argument or two arguments.
//   - `__` In all other cases.
//
//  The macro `VRG_fn_sel` uses this expression to select the right function to call. 

#define VRG_fn_sel(ret,...) \
   VRG_join(VRG_fn_, \
            VRG_join(VRG_ncommas(VRG_comma __VA_ARGS__ ()), VRG_ncommas(VRG_comma __VA_ARGS__ ))) (ret)

//  `VRG_fn_1_(ret)` will always return `0`
//  `VRG_fn_11(ret)` and `VRG_fn_11(ret)` will return their argument.
// The `ret` parameter is there to support the `vrg` and `vrg_` variants. 
#define VRG_fn_1_(ret) 0
#define VRG_fn_11(ret) ret
#define VRG_fn___(ret) ret

// Use `vrg()` to define functions f_0, ..., f_9 for 0, ..., 9 arguments
#define vrg(f_,...)  VRG_join(f_, VRG_fn_sel(VRG_nargs(__VA_ARGS__),__VA_ARGS__))(__VA_ARGS__)

// Use `vrg_()` to define functions f_0 for no arguments and f__ for any number of arguments
#define vrg_(f_,...) VRG_join(f_, VRG_fn_sel(_                     ,__VA_ARGS__))(__VA_ARGS__)

#endif // VRG_VERSION_H
