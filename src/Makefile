CFLAGS = -g -Wall --std=c99 -I ../include
CC = clang

# TODO
# Macros for compiler, optimizations, list of object file dependencies, include dir, obj file dir
# Macros for clean, clean all
# Macro to be parameterized for name of compiled target and target.c

processing:
	$(CC) -c processing_lib.c -o processing_lib.o

all: processing_lib.o example.c
	$(CC) -o example portsf.o ieee80.o processing_lib.o example.c $(CFLAGS) 

report-peak: processing_lib.o report_peak.c
	$(CC) -o report_peak portsf.o ieee80.o processing_lib.o report_peak.c $(CFLAGS) 

gain: processing_lib.o gain.c
	$(CC) -o gain portsf.o ieee80.o processing_lib.o gain.c $(CFLAGS) 

##makefile for portsf
POBJS = ieee80.o portsf.o

.c.o:	$(CC) -c $(CFLAGS) $< -o $@ 

# .PHONY:	clean veryclean
# all:	libportsf.a

clean:
	-rm -f $(POBJS)

veryclean:
	-rm -f $(POBJS) 
	rm -f libportsf.a; 

libportsf.a:	$(POBJS)
	ar -rc libportsf.a $(POBJS)
	ranlib  libportsf.a

install:	libportsf.a
	cp libportsf.a ../lib
#
#	dependencies
#
portsf.c:	../include/portsf.h ieee80.h

## CFLAGS = -I ../include -D_DEBUG -g
## on strange 64 bit platforms must define CPLONG64
## CFLAGS = -Dunix -O2 -I ../include
#CFLAGS = -I ../include

#CC=clang

#example: portsf.o example.c 
#	$(CC) -o example example.c $(CFLAGS)
