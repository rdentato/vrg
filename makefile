#  (C) by Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the MIT license:
#  https://opensource.org/licenses/MIT
#

# If you know exactly how many options there are (or you 
# have more than 16 options, which is the default), you 
# can specify it by defining VRGMAXOPTS
CFLAGS = -O2 -Wall # -DVRGMAXOPTS=3

all: vrgtest vrgtest2 vrgtest3 vrgtest4 vrgtest5

vrgtest: vrg.h vrgtest.c
	gcc $(CFLAGS) -o vrgtest vrgtest.c

vrgtest2: vrg.h vrgtest2.c
	gcc $(CFLAGS) -o vrgtest2 vrgtest2.c

vrgtest3: vrg.h vrgtest3.c
	gcc $(CFLAGS) -o vrgtest3 vrgtest3.c

vrgtest4: vrg.h vrgtest4.c
	gcc $(CFLAGS) -o vrgtest4 vrgtest4.c

vrgtest5: vrg.h vrgtest5.c
	gcc $(CFLAGS) -o vrgtest5 vrgtest5.c
