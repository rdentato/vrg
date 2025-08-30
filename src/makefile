#  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
#  SPDX-License-Identifier: MIT

SRC=../src
DIST=../dist

dist: $(DIST)/vrg.h $(DIST)/cli.h

$(DIST)/vrg.h: $(SRC)/vrg.h
	sed -e '/^\/\/ /d' -e '/^\/\/$$/d' -e '/^ *$$/d' $(SRC)/vrg.h > $(DIST)/vrg.h

$(DIST)/cli.h: $(DIST)/vrg.h $(SRC)/cli.h
	sed -e '/^#include \"vrg.h\"/{r ../dist/vrg.h' -e 'd}' $(SRC)/cli.h > $(DIST)/cli.h 

clean_dist:
	rm -f $(DIST)/cli.h $(DIST)/vrg.h 
