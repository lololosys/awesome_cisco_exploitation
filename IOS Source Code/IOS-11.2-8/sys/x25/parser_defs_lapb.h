/*
 *	P A R S E R _ D E F S _ L A P B . H
 *
 * $Id: parser_defs_lapb.h,v 3.1 1996/02/01 05:14:42 hampton Exp $
 * $Source: /release/111/cvs/Xsys/x25/parser_defs_lapb.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_lapb.h,v $
 * Revision 3.1  1996/02/01  05:14:42  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* LAPB defines */
#define LAPB_CMD_MODULO		 0
#define LAPB_CMD_T1		 1
#define LAPB_CMD_T3		 2
#define LAPB_CMD_T4		 3
#define LAPB_CMD_N1		 4
#define LAPB_CMD_N2		 5
#define LAPB_CMD_K		 6
#define LAPB_CMD_PROTOCOL	 7
#define LAPB_CMD_HOLDQ		 8

#define VIP_HACK
#ifdef VIP_HACK
#define LAPB_IPC_ALLOWED	 9	/*need to "ipc master ." before allowing
					 *code to use IPC - its a hack but
					 *what you going to do - wait till its
					 *fixed?
					 *at present ipc fails if the remote
					 *end is not in listen (registered).
				         *what you going to do - wait till its
                                         *fixed? */
#endif
