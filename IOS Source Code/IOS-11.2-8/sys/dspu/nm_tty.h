/* $Id: nm_tty.h,v 3.2 1995/11/17 09:06:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/nm_tty.h,v $
 *------------------------------------------------------------------
 * This module contains the prototype definitions for the
 * low level tty <-> sscp-pu session communication routines.
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nm_tty.h,v $
 * Revision 3.2  1995/11/17  09:06:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/22  07:57:32  bchan
 * CSCdi35751:  service point runcmd long data handling
 *
 * Revision 2.1  1995/06/07  20:30:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __NM_TTY_H
#define __NM_TTY_H

/*---- The one in h/ttysrv.h is wrong. */
#ifdef setup
#undef setup
#define setup(tty) ((*(tty->ovector->vsetup)) (tty))
#endif

#define OutputBufferSize 1024
#define MaxAutomoreLine  7

typedef struct NmOutbuffer NmOutbuffer;

struct NmOutbuffer{
    NmOutbuffer* next;
    unsigned short length;
    byte  buffer[OutputBufferSize];
};


void
nm_tty_init(tt_soc* tty);

#endif  /* __NM_TTY_H. */
/*---- End include file: nm_tty.h. */
