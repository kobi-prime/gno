# From:	@(#)Makefile	8.1 (Berkeley) 7/19/93
# $Id: Makefile,v 1.3 1998/04/16 06:32:26 gdr-ftp Exp $

PROG=	login
MAN1=	login.1
MAN5=	login.access.5
SRCS=	login.c
#SRCS=	login.c login_access.c login_fbtab.c

#CFLAGS+=-DLOGIN_ACCESS -DSKEY -DLOGALL

#.if defined(KLOGIN_PARANOID)
#CFLAGS+=-DKLOGIN_PARANOID
#.endif

DPADD=	${LIBUTIL} ${LIBCRYPT}
LDADD=	-lutil -lcrypt
#DPADD=	${LIBUTIL} ${LIBSKEY} ${LIBMD} ${LIBCRYPT}
#LDADD=	-lutil -lskey -lmd -lcrypt

#.if exists(${DESTDIR}/usr/lib/libkrb.a) && defined(MAKE_EBONES)
#CFLAGS+=-DKERBEROS
#SRCS+=	klogin.c
#DPADD+=	${LIBKRB} ${LIBDES}
#LDADD+=	-lkrb -ldes
#DISTRIBUTION=	krb
#.endif

BINOWN=	root
BINMODE=4555
INSTALLFLAGS=-fschg

DEBUG=	25
OPTIMIZE=79
STACK=	768
BINDIR	= /usr/sbin
HAS_BSD_MANPAGE	= true

.INCLUDE: /src/gno/prog.mk
