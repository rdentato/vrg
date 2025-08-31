//.  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
//.  SPDX-License-Identifier: MIT
#ifndef VRG_VERSION
#define VRG_VERSION 0x0010000B // 0.10.0-beta
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
