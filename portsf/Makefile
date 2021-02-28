# NOTE: Target can't have same name as output or make thinks the target is up to
# date because it sees the artifact of that name and it hasn't changed.

# TODO
# Macros for compiler, optimizations, list of object file dependencies, include dir, obj file dir
# Macros for clean, clean all
# Macro to be parameterized for name of compiled target and target.c

all:
	clang -o example portsf.o ieee80.o process_lib.o example.c -I ../include

##makefile for portsf
#POBJS = ieee80.o portsf.o

## CFLAGS = -I ../include -D_DEBUG -g
## on strange 64 bit platforms must define CPLONG64
## CFLAGS = -Dunix -O2 -I ../include
#CFLAGS = -I ../include

#CC=clang

#.c.o:	$(CC) -c $(CFLAGS) $< -o $@ 

#.PHONY:	clean veryclean
#all:	libportsf.a

#clean:
#	-rm -f $(POBJS)

#veryclean:
#	-rm -f $(POBJS) 
#	rm -f libportsf.a; 

#libportsf.a:	$(POBJS)
#	ar -rc libportsf.a $(POBJS)
#	ranlib  libportsf.a

#install:	libportsf.a
#	cp libportsf.a ../lib
##
##	dependencies
##
#portsf.c:	../include/portsf.h ieee80.h


#example: portsf.o example.c 
#	$(CC) -o example example.c $(CFLAGS)
