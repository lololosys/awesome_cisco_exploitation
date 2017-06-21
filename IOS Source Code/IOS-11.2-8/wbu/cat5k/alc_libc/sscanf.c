/* $Id: sscanf.c,v 1.1.4.1 1996/05/09 14:55:59 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/sscanf.c,v $
 *------------------------------------------------------------------
 * sscanf.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * copyright (c) 1993 Xlnt Designs Inc
 * copyright (c) 1990 Regents of the University of California.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sscanf.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:59  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:30  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:32  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:38  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:42  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:53:51  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)sscanf.c	1.2 10/22/93 11/10/93 copyright (c) 1993 Xlnt Designs Inc";
#endif
/*
 * copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 *      Revision History (XDI - XLNT Designs Inc.)
 *
 * Jun-11-92    AMP
 *	Changed the arglist of sscanf so that the va_alist contained all
 *	args; this is the preferred method of vararg passing, and mixing
 *	args and varargs could cause problems on some risc processors.  
 *
 */

#include "types.h"
#include "stdio.h"
/*#include "stdarg.h"*/
#include "varargs.h"

/* ARGSUSED */
static uInt32
eofread(cookie, buf, len)
	void *cookie;
	char *buf;
	uInt32 len;
{

	return (0);
}

uInt32
sscanf(va_alist)
	va_dcl
{
	char *str;
	char *fmt;
	uInt32 ret;
	va_list ap;
	FILE f;

	va_start(ap);
	str = va_arg(ap, char *);
	fmt = va_arg(ap, char *);
	f._flag = _IOREAD;
	f._base = f._ptr = (unsigned char *)str;
	f._bufsiz = f._cnt = strlen(str);
	ret = __svfscanf(&f, fmt, ap);
	va_end(ap);
	return (ret);
}
