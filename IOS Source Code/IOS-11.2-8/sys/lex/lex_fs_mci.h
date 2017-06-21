/* $Id: lex_fs_mci.h,v 3.2 1995/11/17 17:43:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lex_fs_mci.h,v $
 *------------------------------------------------------------------
 * lex_fs_mci.h - LEX high-end fastswitching definitions
 *
 * May 4, 1994, Andy Harvey
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex_fs_mci.h,v $
 * Revision 3.2  1995/11/17  17:43:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:40:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __LEX_FS_MCI_H__
#define __LEX_FS_MCI_H__

#include "../lex/lexfast.h"

#define LEX_RECEIVE_OFFSET	(MCI_OFFSETSIZE + 1)

/* Prototypes */
boolean lex_fs (hwidbtype *);
boolean lex_ip_fastswitch (hwidbtype *);
boolean lex_nov_fastswitch(hwidbtype *);

/*
 * Place here for now, but check later.
 */
ushort fastiphdrchk(ushort *, ulong);

#endif
