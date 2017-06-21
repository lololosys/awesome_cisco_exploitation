/* $Id: texti.h,v 3.4.18.2 1996/06/06 04:25:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/texti.h,v $
 *------------------------------------------------------------------
 * texti.h - external declarations and defines for texti
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: texti.h,v $
 * Revision 3.4.18.2  1996/06/06  04:25:28  widmer
 * CSCdi59599:  Add printf style formatting to parse_line
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/06/05  15:05:45  widmer
 * CSCdi57346:  Add printf style formatting to readline prompt
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/07  21:15:10  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.3  1995/12/10  21:13:03  widmer
 * CSCdi45105:  Running-configs line exec-timeout and history change at
 * login
 * Add argument to readline() to specify whether command should be added
 * to history, change existing readline() calls to use TRUE, change
 * askstring() to call readline() with FALSE.
 *
 * Revision 3.2  1995/11/17  18:52:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:19:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:59:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TEXTI_H__
#define __TEXTI_H__

/* Flags */
#define	ESCAPE_CHAR	0x01
#define	TERMINAL_ESCAPE	0x02
#define	KILLED_BEFORE	0x04
#define	JUST_KILLED	0x08
#define	YANKED_BEFORE	0x10
#define	JUST_YANKED	0x20


/* used for killing */
#define	NEW		   0
#define	APPEND		   1
#define	PREPEND		   2
#define	OLD		   3

/* to the max */
#define	MAX_KILL	  10

#define	NOECHO		stdio->tty_editinfo.noecho
#define	BEEP		printc(BELL)
#define	GET_CHAR_STRING(ch, str)\
    if (ch == TAB) { \
	/*           12345678 */\
	sprintf(str,"        ");\
    } else {\
	if (((uchar)ch < 32) || (ch == 127)) {\
	    sprintf(str, "^%c", (ch + 64) & 0x7f);\
	} else {\
	    sprintf(str, "%c", ch);\
	}\
    }

#define	SCREEN_WIDTH	  	stdio->tty_width
#define	SCROLL_JUMP	   	10
#define	MALLOC(size)		malloc(size)

#define	FREE(pointer)		TI_free((void **) &(pointer))
#define	Printc(ch)		if (NOECHO == FALSE) printc(ch)
#define	Putc(ch)		if (NOECHO == FALSE) putc(stdio,ch)
#define	Printf			if (NOECHO == FALSE) printf

#define	CONTROL_C	  3		/* ^C */
#define	CONTROL_D	  4		/* ^D */
#define	TAB		  9		/* ^I (TAB) */
#define	INSERT_NEWLINE	 15		/* ^O */
#define	ESCAPE		 27		/* Escape, ^3, ^[ */

#define	STRING_INSERT	  0
#define	STRING_REPLACE	  1

boolean	texti(char*, int, char*, char*);
boolean	readline(char*, int, const char*, char*, boolean, const char*, ...);
boolean readline_args(char *, int, const char *, char *, boolean,
		      const char *, va_list);

/*  Init & Reset  */
void	texti_init(tt_soc*);
void	texti_reset(tt_soc*);


#endif		/* __TEXTI_H__ */
