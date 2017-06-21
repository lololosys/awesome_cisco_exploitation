/* $Id: parser_defs_arap.h,v 3.1.40.1 1996/09/10 18:51:26 asb Exp $
 * $Source: /release/112/cvs/Xsys/arap/parser_defs_arap.h,v $
 *------------------------------------------------------------------
 * Parser definitions for arap
 *
 * January 1994, Tim Kolar
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: parser_defs_arap.h,v $
 * Revision 3.1.40.1  1996/09/10  18:51:26  asb
 * CSCdi68276:  ARAP:  arap logging command requires debugging extensions
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:57:05  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:49:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *
 * 
 */

#define PARSER_ARAP_DEDICATED		1
#define PARSER_ARAP_ENABLED		(PARSER_ARAP_DEDICATED+1)
#define PARSER_ARAP_NOGUEST		(PARSER_ARAP_ENABLED+1)
#define PARSER_ARAP_NETACCESSLIST	(PARSER_ARAP_NOGUEST+1)
#define PARSER_ARAP_TIMELIMIT		(PARSER_ARAP_NETACCESSLIST+1)
#define PARSER_ARAP_NOSMARTBUFFER	(PARSER_ARAP_TIMELIMIT+1)
#define PARSER_ARAP_WARNINGTIME		(PARSER_ARAP_NOSMARTBUFFER+1)
#define PARSER_ARAP_ZONELIST		(PARSER_ARAP_WARNINGTIME+1)
#define PARSER_ARAP_LOGGING		(PARSER_ARAP_ZONELIST+1)
#define PARSER_ARAP_NETWORK		(PARSER_ARAP_LOGGING+1)
#define PARSER_ARAP_TACACS		(PARSER_ARAP_NETWORK+1)
#define PARSER_ARAP_MANUALPASSWORD	(PARSER_ARAP_TACACS+1)
#define PARSER_ARAP_CALLBACK	        (PARSER_ARAP_MANUALPASSWORD+1)
#define PARSER_ARAP_AUTHEN		(PARSER_ARAP_CALLBACK+1)
#define PARSER_ARAP_NOGUEST_IFNEEDED	(PARSER_ARAP_AUTHEN+1)
#define	PARSER_ARAP_LOGGING_DEBUGEXTENSIONS	(PARSER_ARAP_NOGUEST_IFNEEDED+1)
#define PARSER_MNP4_LOGGING             (PARSER_ARAP_LOGGING_DEBUGEXTENSIONS+1)
