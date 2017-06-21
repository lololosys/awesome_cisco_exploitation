/* $Id: regexp.h,v 3.2.62.1 1996/03/18 22:30:55 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/util/regexp.h,v $
 *------------------------------------------------------------------
 * $Log: regexp.h,v $
 * Revision 3.2.62.1  1996/03/18  22:30:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/02/20  21:45:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:56:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 */

/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */
#define NSUBEXP  10
struct regexp_ {
	char *startp[NSUBEXP];	
	char *endp[NSUBEXP];    
	char regstart;		/* Internal use only. */
	char reganch;		/* Internal use only. */
	char regatom;		/* Internal use only. */
	char *regmust;		/* Internal use only. */
	int regmlen;		/* Internal use only. */
	char program[1];	/* Unwarranted chumminess with compiler. */
};

/*
 * The first byte of the regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.
 */
#define	MAGIC	0234


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * regexp.c
 */
extern char *reg(int, int *);
extern char *regatom(int *);
extern char *regbranch(int *);
extern char *regnode(char);
extern char *regpiece(int *);
extern char *regprop(char *);
extern int regexec(regexp *, char *);
extern int regmatch(char *);
extern int regrepeat(char *);
extern int regtry(regexp *, char *);
extern regexp *regcomp(char *);
extern void regc(char);
extern void regdump(regexp *);
extern void regerror(char *);
extern void reginsert(char, char *);
extern void regoptail(char *, char *);
extern void regtail(char *, char *);

/*
 * regsub.c
 */
extern int regsub(regexp *, char *, char *, int);
