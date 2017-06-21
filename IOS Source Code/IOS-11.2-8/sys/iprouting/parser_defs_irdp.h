/*
 *	P A R S E R _ D E F S _ I R D P . H
 *
 * $Id: parser_defs_irdp.h,v 3.1 1996/02/01 04:58:45 hampton Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/parser_defs_irdp.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_irdp.h,v $
 * Revision 3.1  1996/02/01  04:58:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  IRDP commands
 */
#define IRDP_CMD_ADDRESS                 1
#define IRDP_CMD_PREFERENCE              2
#define IRDP_CMD_HOLDTIME                3
#define IRDP_CMD_MINADVERT               4
#define IRDP_CMD_MAXADVERT               5
#define	IRDP_CMD_MULTICAST		 6
#define IRDP_CMD                         7


