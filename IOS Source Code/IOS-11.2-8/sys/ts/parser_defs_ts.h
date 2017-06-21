/*
 *	P A R S E R _ D E F S _ T S . H
 *
 * $Id: parser_defs_ts.h,v 3.1 1996/02/01 05:14:09 hampton Exp $
 * $Source: /release/111/cvs/Xsys/ts/parser_defs_ts.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_ts.h,v $
 * Revision 3.1  1996/02/01  05:14:09  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:53:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  23:40:12  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  22:31:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Terminal server flowcontrol commands
 */
#define	FLOWCONTROL_SOFTWARE	 1
#define	FLOWCONTROL_HARDWARE	 2
#define	FLOWCONTROL_NONE	 3
#define	FLOWCONTROL_IN		 4
#define	FLOWCONTROL_OUT		 5
#define	FLOWCONTROL_BOTH	 6

/*
 *  Telnet line commands
 */
#define	TN_TRANS		 1
#define	TN_SYNC			 2
#define	TN_SPEED		 3
#define	TN_REFUSE		 4
#define	TN_BREAKIP		 5
#define TN_IPBREAK               6

/*
 * Autobaud commands
 */
#define	AUTOBAUD_FULL_RANGE	 1
#define	AUTOBAUD_HIGH		 2
#define	AUTOBAUD_LOW		 3

/*
 *  Modem line commands
 */
#define PARSER_MODEM_CHAT               1
#define PARSER_MODEM_ANSWER             2
