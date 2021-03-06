/*
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Modified for GNO (Apple IIgs) by Steve Reeves, January 1998
 *
 * Added conditional compilation based on these macros:
 *
 *   __ORCAC__ for constructs unacceptable to the compiler,
 *   __GNO__ for changes not related to the compiler,
 *   __STDC__ or __P() from <sys/cdefs.h> for ISO C prototyping,
 *   __STACK_CHECK__ for reporting stack space usage,
 *   NOID for excluding copyright and version control strings
 *
 * $Id: colcrt.c,v 1.1 1998/02/17 03:12:30 gdr-ftp Exp $
 *
 */

#ifndef NOID
#ifndef lint
static char copyright[] =
"@(#) Copyright (c) 1980, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)colcrt.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */
#endif /* NOID */

#include <stdio.h>
#ifdef __GNO__
#include <sys/cdefs.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <gno/gno.h>
#endif

/*
 * colcrt - replaces col for crts with new nroff esp. when using tbl.
 * Bill Joy UCB July 14, 1977
 *
 * This filter uses a screen buffer, 267 half-lines by 132 columns.
 * It interprets the up and down sequences generated by the new
 * nroff when used with tbl and by \u \d and \r.
 * General overstriking doesn't work correctly.
 * Underlining is split onto multiple lines, etc.
 *
 * Option - suppresses all underlining.
 * Option -2 forces printing of all half lines.
 *
 * For GNO: Option -V prints version information and exits.
 */

#ifndef __ORCAC__
char	page[267][132];
#else
char	*page;
#endif

int	outline = 1;
int	outcol;

char	suppresul;
char	printall;

FILE	*f;

static void usage __P((void));
#ifdef __STDC__
int plus(char c, char d);
void pflush(int ol);
void move(int l, int m);
#endif

#if defined(__GNO__)  &&  defined(__STACK_CHECK__)
/* Interface to check on how much stack space a C program uses. */
#ifndef _GNO_GNO_H_
#include <gno/gno.h>
#endif
static void report_stack(void)
{
	fprintf(stderr,"\n ==> %d stack bytes used <== \n", _endStackCheck());
}
#endif

int
#ifndef __STDC__
main(argc, argv)
	int argc;
	char *argv[];
#else
main(int argc, char *argv[])
#endif
{
	register int c;
	register char *cp, *dp;

#if defined(__GNO__)  &&  defined(__STACK_CHECK__)
	_beginStackCheck();
	atexit(report_stack);
#endif

	argc--;
	argv++;
	while (argc > 0 && argv[0][0] == '-') {
		switch (argv[0][1]) {
			case '\0':
				suppresul = 1;
				break;
			case '2':
				printall = 1;
				break;
#ifdef __GNO__
			case 'V':
				puts("colcrt v1.0");
				exit(0);
#endif
			default:
#ifdef __GNO__
				fprintf(stderr, "%s: illegal option -- %c\n",
						__prognameGS(), argv[0][1]);
#endif
				usage();
		}
		argc--;
		argv++;
	}
#ifdef __ORCAC__
	/* Dynamically allocate the page buffer, so we can use the
		small memory model; buffer MUST be initialized to zeroes */
	if ((page = calloc(1, 267 * 132L)) == NULL)
		err(1, NULL);
#endif
	do {
		if (argc > 0) {
#ifndef __GNO__
			close(0);
			if (!(f = fopen(argv[0], "r"))) {
#else
			if (!(f = freopen(argv[0], "r", stdin))) {
#endif
				fflush(stdout);
				err(1, argv[0]);
			}
			argc--;
			argv++;
		}
		for (;;) {
			c = getc(stdin);
			if (c == -1) {
				pflush(outline);
				fflush(stdout);
				break;
			}
			switch (c) {
				case '\n':
					if (outline >= 265)
						pflush(62);
					outline += 2;
					outcol = 0;
					continue;
				case '\016':
				case '\017':
					continue;
				case '\033':
					c = getc(stdin);
					switch (c) {
						case '9':
							if (outline >= 266)
								pflush(62);
							outline++;
							continue;
						case '8':
							if (outline >= 1)
								outline--;
							continue;
						case '7':
							outline -= 2;
							if (outline < 0)
								outline = 0;
							continue;
						default:
							continue;
					}
				case '\b':
					if (outcol)
						outcol--;
					continue;
				case '\t':
					outcol += 8;
					outcol &= ~7;
					outcol--;
					c = ' ';
				default:
					if (outcol >= 132) {
						outcol++;
						continue;
					}
#ifndef __ORCAC__
					cp = &page[outline][outcol];
#else
					cp = page + 132L*outline + outcol;
#endif
					outcol++;
					if (c == '_') {
						if (suppresul)
							continue;
						cp += 132;
						c = '-';
					}
					if (*cp == '\0') {
						*cp = c;
						dp = cp - outcol;
						for (cp--; cp >= dp && *cp == '\0'; cp--)
							*cp = ' ';
					} else
						if (plus(c, *cp) || plus(*cp, c))
							*cp = '+';
						else if (*cp == ' ' || *cp == '\0')
							*cp = c;
					continue;
			}
		}
	} while (argc > 0);
	fflush(stdout);
	exit(0);
}

static void
usage __P((void))
{
#ifndef __GNO__
	fprintf(stderr, "usage: colcrt [ - ] [ -2 ] [ file ... ]\n");
#else
	fprintf(stderr, "usage: %s [ - ] [ -2 ] [ -V ] [ file ... ]\n",
			__prognameGS());
#endif
	exit(1);
}

#ifndef __STDC__
plus(c, d)
	char c, d;
#else
int
plus(char c, char d)
#endif
{

	return (c == '|' && d == '-' || d == '_');
}

int first;

#ifndef __STDC__
pflush(ol)
	int ol;
#else
void
pflush(int ol)
#endif
{
	register int i, j;
	register char *cp;
	char lastomit;
	int l;

	l = ol;
	lastomit = 0;
	if (l > 266)
		l = 266;
	else
		l |= 1;
	for (i = first | 1; i < l; i++) {
		move(i, i - 1);
		move(i, i + 1);
	}
	for (i = first; i < l; i++) {
#ifndef __ORCAC__
		cp = page[i];
#else
		cp = page + 132L*i;
#endif
		if (printall == 0 && lastomit == 0 && *cp == '\0') {
			lastomit = 1;
			continue;
		}
		lastomit = 0;
#ifndef __GNO__
		printf("%s\n", cp);
#else
		puts(cp);
#endif
	}
#ifndef __ORCAC__
	bcopy(page[ol], page, (267 - ol) * 132);
	bzero(page[267- ol], ol * 132);
#else
	bcopy(page + 132L*ol, page, (267 - ol) * 132L);
	bzero(page + 132L*(267-ol), ol * 132L);
#endif
	outline -= ol;
	outcol = 0;
	first = 1;
}

#ifndef __STDC__
move(l, m)
	int l, m;
#else
void
move(int l, int m)
#endif
{
	register char *cp, *dp;

#ifndef __ORCAC__
	for (cp = page[l], dp = page[m]; *cp; cp++, dp++) {
#else
	for (cp = page + 132L*l, dp = page + 132L*m; *cp; cp++, dp++) {
#endif
		switch (*cp) {
			case '|':
				if (*dp != ' ' && *dp != '|' && *dp != '\0')
					return;
				break;
			case ' ':
				break;
			default:
				return;
		}
	}
	if (*cp == '\0') {
#ifndef __ORCAC__
		for (cp = page[l], dp = page[m]; *cp; cp++, dp++)
#else
		for (cp = page + 132L*l, dp = page + 132L*m; *cp; cp++, dp++)
#endif
			if (*cp == '|')
				*dp = '|';
			else if (*dp == '\0')
				*dp = ' ';
#ifndef __ORCAC__
		page[l][0] = '\0';
#else
		*(page + 132L*l) = '\0';
#endif
	}
}
