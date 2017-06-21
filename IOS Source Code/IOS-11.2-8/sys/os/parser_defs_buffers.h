/*
 *	P A R S E R _ D E F S _ B U F F E R S . H
 *
 * $Id: parser_defs_buffers.h,v 3.1 1996/02/01 04:59:01 hampton Exp $
 * $Source: /release/111/cvs/Xsys/os/parser_defs_buffers.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_buffers.h,v $
 * Revision 3.1  1996/02/01  04:59:01  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:49:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Buffer parameter defines */
#define BUF_CMD_SMALL		 1
#define BUF_CMD_MIDDLE		 2
#define	BUF_CMD_BIG		 3
#define BUF_CMD_LARGE 		 4
#define BUF_CMD_HUGE 		 5
#define BUF_CMD_INIT		 6
#define BUF_CMD_MINFREE		 7
#define BUF_CMD_MAXFREE		 8
#define BUF_CMD_PERM		 9
#define BUF_CMD_SIZE 		10
#define BUF_CMD_VERYBIG 	11	
