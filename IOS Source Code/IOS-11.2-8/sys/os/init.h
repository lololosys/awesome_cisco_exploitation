/* $Id: init.h,v 3.3.62.2 1996/08/12 16:05:24 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/init.h,v $
 *------------------------------------------------------------------
 * init.h - Prototypes and defines for system initialization
 *
 * March 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: init.h,v $
 * Revision 3.3.62.2  1996/08/12  16:05:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.62.1  1996/08/07  09:01:52  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  18:48:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:02  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:46:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:56:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern int init_errors;

/*
 * Prototypes
 */

extern boolean system_init(boolean loading);
extern forktype init_process(boolean loading);
extern boolean systeminitBLOCK(queuetype *qptr);
extern void show_controllers(parseinfo *csb);

/*
 * External variables
 */

extern const char noasm[];
