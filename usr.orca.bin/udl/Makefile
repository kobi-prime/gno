#
# This file is intended for use when building udl from within
# the GNO base distribution.  For building stand-alone on the
# IIgs or on another platform, use one of the other makefiles.
#
# Devin Reade, October 1997.
#
# $Id: Makefile,v 1.3 1998/03/08 17:55:53 gdr-ftp Exp $
#

PROG	= udl
SRCS	= udlgs.c udluse.c common.c globals.c
MAIN	= udlgs

# See the README for these macros.
CFLAGS	+= -DHAS_ATEXIT -DOVERFLOW_CHECK # -D__STACK_CHECK__
STACK	= 2048
BINDIR	= /usr/bin

.INCLUDE:	/src/gno/prog.mk

head.o:	def.h extern.h
