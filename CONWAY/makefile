########################################################################### 
# Compiler
########################################################################### 

CC = mpicc
CCFLAGS = -g -Wall -std=c99
ifeq ($(DEBUG),on)
	CCFLAGS += -DDEBUG
endif

########################################################################### 
# Files
########################################################################### 

C_FILES = conways_base.c pgm.c pprintf.c
O_FILES = conways_base.o pgm.o pprintf.o

########################################################################### 
# Targets
########################################################################### 

all: conways_base

conways_base: $(O_FILES)
	$(CC) -o conways_base $(O_FILES) $(LDFLAGS)

.PHONY: clean
clean:		
	/bin/rm -f core $(O_FILES) conways_base

conways_base.o: pgm.o pprintf.o

.c.o:		
	$(CC) $(CCFLAGS) -c -o $*.o $*.c

.cpp.o:		
	$(CC) $(CCFLAGS) -c -o $*.o $*.cpp

########################################################################### 
# Dependencies
########################################################################### 

# All of the object files depend on the globals, so rebuild everything if they
# change!
*.o: globals.h

# Nothing really depends on the pprintf prototypes, but just be safe
*.o: pprintf.h

# Conway depends on PGM utilities
conways_base.o: pgm.h pprintf.h
