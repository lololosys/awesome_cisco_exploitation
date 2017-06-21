/* $Id: hostname.h,v 3.2.62.1 1996/03/18 21:30:01 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/hostname.h,v $
 *------------------------------------------------------------------
 * hostname.h - Prototypes and externs for hostname support
 *
 * March 1995, Scott Mackie
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hostname.h,v $
 * Revision 3.2.62.1  1996/03/18  21:30:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/14  01:21:30  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.2.26.1  1996/02/20  16:39:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:48:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:55:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */

extern void hostname_init(void);
extern boolean whoami(void);
extern void set_hostname(char *name);
extern void hostname_command(parseinfo *csb);

/*
 * Externs
 */

extern char *default_domain;		/* default domain name */
extern char *banner_motd;		/* message of the day banner */
extern char *banner_exec;		/* special banners: RU TEMP */
extern char *banner_incoming;
extern char *banner_temp;               /* temporary banner */
extern char *banner_login;
extern char *Eprompt;			/* EXEC prompt string */
extern char *Pprompt;			/* privileged EXEC prompt string */
