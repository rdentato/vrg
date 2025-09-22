//.  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
//.  SPDX-License-Identifier: MIT
#ifndef VRG_VERSION
#define VRG_VERSION 0x0021000B // 0.21.0-beta
#define VRG_jn(x,y)    VRG_exp(x ## y)
#define VRG_join(x,y)  VRG_jn(x, y)
#define VRG_exp(...) __VA_ARGS__
#define VRG_count(x1,x2,x3,x4,x5,x6,x7,x8,x9,xA,xN, ...) xN
#define VRG_nargs(...)    VRG_exp(VRG_count(__VA_ARGS__, A, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define VRG_ncommas(...)  VRG_exp(VRG_count(__VA_ARGS__, _, _, _, _, _, _, _, _, 1, _, _))
#define VRG_comma(...) ,
#define VRG_sel(x,...) \
   VRG_join(VRG_sel_, \
            VRG_join(VRG_ncommas(VRG_comma __VA_ARGS__ ()), VRG_ncommas(VRG_comma __VA_ARGS__ ))) (x)
#define VRG_sel_1_(x) 0
#define VRG_sel_11(x) x
#define VRG_sel___(x) x
#define vrg(f_,...)  VRG_join(f_, VRG_sel(VRG_nargs(__VA_ARGS__),__VA_ARGS__))(__VA_ARGS__)
#define VRG_frst(x,...) x
#define VRG_scnd(x,...) VRG_frst(__VA_ARGS__)
#define VRG_tail(x,...) __VA_ARGS__
#define VRG_tail2(x,...) VRG_tail(__VA_ARGS__)
#define VRG_precomma(...) VRG_comma
#define VRG_sel_n(x,...) \
   VRG_join(VRG_sel_ ,VRG_ncommas(VRG_exp(VRG_precomma VRG_frst(__VA_ARGS__) () VRG_scnd(__VA_ARGS__) ())))(x)
#define VRG_sel_1(x) x
#define VRG_sel__(x) _
#define vrg0(f_,...)  VRG_join(f_,VRG_sel_n(0,__VA_ARGS__))(__VA_ARGS__)
#define vrg1(f_,...)  VRG_join(f_,VRG_sel_n(1,VRG_tail(__VA_ARGS__)))(__VA_ARGS__)
#define vrg2(f_,...)  VRG_join(f_,VRG_sel_n(2,VRG_tail2(__VA_ARGS__)))(__VA_ARGS__)
#define vrg_(f_,...)  vrg0(f_,...)
#endif // VRG_VERSION_H
