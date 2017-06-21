/* $Id: rsh.h,v 3.2.62.1 1996/04/15 21:23:39 bcole Exp $
 * $Source: /release/112/cvs/Xsys/tcp/rsh.h,v $
 *------------------------------------------------------------------
 * rsh.h -- Prototypes for rsh.c
 *
 * April 1994, Marciano Pitargue
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsh.h,v $
 * Revision 3.2.62.1  1996/04/15  21:23:39  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:48:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:05:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TCP_RSH_H__
#define __TCP_RSH_H__

#define MAXCMDLEN	1024

#define CR      015

/* Stuff private to rcp/rsh */
extern void rsh_init(void);
extern void rsh_command(char *, tcbtype *, tcbtype *, rhost_entry *);

#endif /* __TCP_RSH_H__ */
