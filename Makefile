#
# CMSC257 - Utility Library 
# Code for the CMSC257 Projects
#

# Make environment
CC=gcc
CFLAGS=-g -c -Wall 
LINKARGS=-lpthread


# Files
OBJECT_FILES=	csapp.o shellex.o

# Productions
all : my257sh

my257sh : $(OBJECT_FILES)
	$(CC) $(LINKARGS) $(OBJECT_FILES) -o $@

shellex.o : shellex.c csapp.h
	$(CC) $(CFLAGS) $< -o $@

csapp.c : csapp.c csapp.h
	$(CC) $(CFLAGS) $< -o $@

clean : 
	rm -v my257sh $(OBJECT_FILES)