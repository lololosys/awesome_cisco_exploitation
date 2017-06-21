/* $Id: stdio.h,v 1.1.4.1 1996/05/09 14:55:48 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/catalyst/stdio.h,v $
 *------------------------------------------------------------------
 * stdio.h
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * Copyright (c) 1993 Xlnt Designs Inc
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stdio.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:48  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:26  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:44:08  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:47  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#)stdio.h	1.1 10/18/93 10/22/93 Copyright (c) 1993 Xlnt Designs Inc 
*/
/*
				stdio.h

	Defines the iob structure which is used in sprintf.
*/
# ifndef FILE
#define	BUFSIZ	1024
#define _SBFSIZ	8
#ifndef IOBUF
extern	struct	_iobuf {
	uInt32	_cnt;
	uChar *_ptr;
	uChar *_base;
	uInt32	_bufsiz;
	uInt16	_flag;
	char	_file;		/* should be short */
} _iob[];
#endif

#define _IOFBF	0
#define	_IOREAD	01
#define	_IOWRT	02
#define	_IONBF	04
#define	_IOMYBUF	010
#define	_IOEOF	020
#define	_IOERR	040
#define	_IOSTRG	0100
#define	_IOLBF	0200
#define	_IORW	0400
#ifndef	NULL
#define	NULL	0
#endif
#define	FILE	struct _iobuf
#define	EOF	(-1)

#define	stdin	(&_iob[0])
#define	stdout	(&_iob[1])
#define	stderr	(&_iob[2])

# endif
