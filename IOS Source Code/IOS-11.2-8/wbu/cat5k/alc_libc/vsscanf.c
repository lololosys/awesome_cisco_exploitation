/* $Id: vsscanf.c,v 1.1.4.1 1996/05/09 14:56:12 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/vsscanf.c,v $
 *------------------------------------------------------------------
 * vfscanf.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * copyright (c) 1993 Xlnt Designs Inc
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vsscanf.c,v $
 * Revision 1.1.4.1  1996/05/09  14:56:12  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:43  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:51  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:56  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:54:05  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)vsscanf.c	1.2 10/22/93 11/10/93 copyright (c) 1993 Xlnt Designs Inc";
#endif

#include "types.h"
#include "stdio.h"
#include "varargs.h"

/* like sscanf, but takes a pointer to a vararg list rather than a variable
 * number of arguments */
uInt32
vsscanf(str, fmt, ap)
	char *str;
	char *fmt;
	va_list ap;
{
	uInt32 ret;
	FILE f;

	f._flag = _IOREAD;
	f._base = f._ptr = (unsigned char *)str;
	f._bufsiz = f._cnt = strlen(str);
	ret = __svfscanf(&f, fmt, ap);
	return (ret);
}
