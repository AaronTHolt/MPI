########################################################################### 
# Compiler
########################################################################### 

CC = mpicc
CCFLAGS =-lm -g
LDFLAGS= -lm -g
ifeq ($(DEBUG),on)
	CCFLAGS += -DDEBUG
endif

########################################################################### 
# Files
########################################################################### 

C_FILES = hw7.1-holtat.c pgm.c pprintf.c
O_FILES = hw7.1-holtat.o pgm.o pprintf.o

########################################################################### 
# Targets
########################################################################### 

all: hw5.1-holtat

hw5.1-holtat: $(O_FILES)
	$(CC) -o hw7.1-holtat $(O_FILES) $(LDFLAGS)

.PHONY: clean
clean:		
	/bin/rm -f core $(O_FILES) hw5.1-holtat

hw5.1-holtat.o: pgm.o pprintf.o

.c.o:		
	$(CC) $(CCFLAGS) -c -pg -o $*.o $*.c

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
hw5.1-holtat.o: pgm.h pprintf.h
