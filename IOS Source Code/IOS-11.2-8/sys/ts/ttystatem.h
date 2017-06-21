/* $Id: ttystatem.h,v 3.2.62.1 1996/08/12 16:08:41 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/ttystatem.h,v $
 *------------------------------------------------------------------
 * ttystatem.h - Definitions for tty state machines
 *
 * December 2, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains the definitions for tty state machines.
 *------------------------------------------------------------------
 * $Log: ttystatem.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:41  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:56:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:10:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TTYSTATEM_H__
#define __TTYSTATEM_H__

/*
 * Externs and prototypes
 */

extern queuetype *tty_statemQ;
extern boolean tty_runstates(conntype *, int);

#endif __TTYSTATEM_H__
