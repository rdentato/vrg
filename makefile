#  (C) by Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the MIT license:
#  https://opensource.org/licenses/MIT
#

# If you know exactly how many options there are (or you 
# have more than 16 options, which is the default), you 
# can specify it by defining VRGMAXOPTS
CFLAGS = -O2 -Wall # -DVRGMAXOPTS=3

vrgtest: vrg.h vrgtest.c
	gcc $(CFLAGS) -o vrgtest vrgtest.c