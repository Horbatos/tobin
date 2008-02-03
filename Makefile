######################################################################
##
## Copyright (C) 2008,  Simon Kagstrom
##
## Filename:      Makefile
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   Makefile for struct
##
## $Id:$
##
######################################################################
CC = gcc
LD = gcc

all: tobin binto

%.o: %.c
	$(CC) -g -Wall -c $< -o $@

tobin: struct.o struct-pack.o
	$(LD) $+ -o $@

binto: struct.o struct-unpack.o
	$(LD) $+ -o $@

clean:
	rm -f *.o tobin binto *~
