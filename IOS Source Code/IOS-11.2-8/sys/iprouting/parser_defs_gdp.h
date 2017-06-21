/*
 *	P A R S E R _ D E F S _ G D P . H
 *
 * $Id: parser_defs_gdp.h,v 3.1 1996/02/01 04:58:41 hampton Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/parser_defs_gdp.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_gdp.h,v $
 * Revision 3.1  1996/02/01  04:58:41  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:50:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define GDP_CMD_ENABLE		 0
#define GDP_CMD_ADDRESS		 1
#define GDP_CMD_DEFAULTROUTE	 2
#define GDP_CMD_HOLDTIME	 3
#define GDP_CMD_PRIORITY	 4
#define GDP_CMD_REPORTTIME	 5
#define GDP_CMD_RIP		 6
#define GDP_CMD_IGRP		 7
#define GDP_CMD_GDP		 8
#define GDP_CMD_IRDP		 9
#define GDP_CMD_GLOBAL		10
#define	GDP_CMD_IRDP_MULTICAST	11
