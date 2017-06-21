/* $Id: termcapglo.h,v 3.2 1995/11/17 18:50:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/termcap/termcapglo.h,v $
 *------------------------------------------------------------------
 * TERMCAPGLO.H - Definition of termcap globals structure.
 * 
 * June 1991, Robert Snyder
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: termcapglo.h,v $
 * Revision 3.2  1995/11/17  18:50:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TERMCAPGLO_H_
#define __TERMCAPGLO_H_	-1	/* Indicate file has been read */

#define MAXRETURNSIZE 64

struct termcap_globals_struct_ {
    char *tbuf;
    int hopcount;
    char result[MAXRETURNSIZE];
    char added[10];
    char *UP;
    char *BC;
};

#define TermGloPtr(x) (conn->proto.tn.termcapdata->x)

#endif  __TERMCAPGLO_H_
