########################################################################### 
# Compiler
########################################################################### 

CC = gcc
CFLAGS = 
LDFLAGS = -fopenmp

ifeq ($(DEBUG),on)
	CFLAGS += -DDEBUG
endif

########################################################################### 
# File Declarations
########################################################################### 
BINARIES = serial hw9.1-holtat-openmp-manual hw9.2-holtat-openmp-compiletime hw9.3-holtat-openmp-runtime

# C, H, and Object Files
C_FILES = serial.c hw9.1-holtat-openmp-manual.c hw9.2-holtat-openmp-compiletime.c hw9.3-holtat-openmp-runtime.
H_FILES = 
O_FILES = serial.o hw9.1-holtat-openmp-manual.o hw9.2-holtat-openmp-compiletime.o hw9.3-holtat-openmp-runtime.o

########################################################################### 
# Main Targets
########################################################################### 
all: $(BINARIES)

serial: serial.c 
	$(CC) -o serial serial.c $(LDFLAGS)

hw9.1-holtat-openmp-manual: hw9.1-holtat-openmp-manual.c
	$(CC) -o hw9.1-holtat-openmp-manual hw9.1-holtat-openmp-manual.c $(LDFLAGS)

hw9.2-holtat-openmp-compiletime: hw9.2-holtat-openmp-compiletime.c
	$(CC) -o hw9.2-holtat-openmp-compiletime hw9.2-holtat-openmp-compiletime.c $(LDFLAGS)

hw9.3-holtat-openmp-runtime: hw9.3-holtat-openmp-runtime.c
	$(CC) -o hw9.3-holtat-openmp-runtime hw9.3-holtat-openmp-runtime.c $(LDFLAGS)

clean:		
	/bin/rm -rf core $(O_FILES) $(BINARIES)

# Suffixes

.cpp.o:		
	$(CC) $(CFLAGS) -c -o $*.o $*.cpp

.c.o:		
	$(CC) $(CFLAGS) -c -o $*.o $*.c

# Dependencies
