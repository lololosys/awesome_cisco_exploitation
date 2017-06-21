/* $Id: stdio.h,v 1.1.4.1 1996/05/09 15:00:10 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/stdio.h,v $
 *-----------------------------------------------------------------
 * stdio.h --  
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stdio.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:10  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:06  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:18  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:16  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:46  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:22  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _STDIO_
#define _STDIO_

typedef struct FILE {
	int fd;
	int valid;
	char *ptr;
	char *buf;
	int cnt;
	} FILE;

FILE *fopen();
char *fgets();
char *gets();

extern FILE _iob[];

#define getc(p)		((--(p)->cnt>=0)?((int)*(p)->ptr++):_filbuf(p))
#define getchar()	getc(stdin)
#define putchar(x)	putc((x),stdout)
#define stdin 		(&_iob[0])
#define stdout 		(&_iob[1])
#define stderr 		(&_iob[2])

#define OPEN_MAX 8
#define MAXLN 256

#define NULL 0
#define EOF  (-1)

typedef int iFunc();
typedef int *Addr;
#endif /* _STDIO_ */
