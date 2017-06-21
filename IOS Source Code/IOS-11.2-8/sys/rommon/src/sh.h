/* $Id: sh.h,v 3.2.58.1 1996/03/21 23:27:53 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/sh.h,v $
 *------------------------------------------------------------------
 * sh.h
 *
 * February 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the monitor shell.
 *------------------------------------------------------------------
 * $Log: sh.h,v $
 * Revision 3.2.58.1  1996/03/21  23:27:53  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:08  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __SH_H__
#define __SH_H__

/*
** Defines for the monitor shell.
*/
#define MAXARG 16
#define ARGVBUFSIZ 128

#define MOREKLUDGE 1

/*
** This structure is where a command line is parsed and
** held when it is passed to a application program.  Argv
** must be first in this structure.
*/
struct shstuff {
  char *argv[MAXARG+1];      /* array of argv buf ptrs (MUST be first) */
  char *cmdptr;              /* command buffer pointer */
  char *argvptr;             /* argv buffer pointer */
  int argc;                  /* argument count */
  jmp_buf shparserrjmp;      /* jump buffer for shell parse errors */
  int retval;                /* the return value from a program */
  char argvbuf[ARGVBUFSIZ];  /* the actual argument buffer */
  char aliasflag, loadonly;
};

typedef enum {
  W_REG,     /* a regular word with no special meaning */
  W_PIPE,    /* the shell pipe character */
  W_BKGRND,  /* the shell background character */
  W_ENDCMD,  /* the shell command terminator character */
  W_REDOUT,  /* the shell redirect output character */
  W_APPEND,  /* the shell redirect output append condition */
  W_REDIN,   /* the shell redirect input character */
  W_EOLN,    /* end of a command line */
  W_EOF,     /* end of file condition */
  W_VOID,    /* void word */
} SH_TOKEN;  /* shell token */

typedef enum {
    NEUTRAL,     /* start of word */
    GRTRTHAN,    /* one '>' char encountered */
    INSQUOTE,    /* in single quoted string */
    INDQUOTE,    /* in double quoted string */
    INSHVAR,     /* in a shell variable word */
    INBRACES,    /* in braces '{}' */
    INWORD,      /* within a word */
    INCOMMENT,   /* in a comment string */
    INPIPELINE,  /* in a pipeline */
} SH_STATE;

/*
** The monitor supports execution of limited script files.
** A monitor script file must have the string "#monsh" on the first
** line of the file.  The define below is the first four
** characters of this special string in hex.
*/
#define MONSH_MAGIC 0x236d6f6e

/* monitor.c */
extern int invoke(struct shstuff *shp);

/* shcmd.c */
extern void shcmdline(struct shstuff *shp);

/* parsetoken.c */
extern SH_TOKEN parsetoken(struct shstuff *shp, SH_STATE state);

#endif  /* __SH_H__ */
