/*
 *	P A R S E R _ D E F S _ H U G E . H
 *
 * $Id: parser_defs_xremote.h,v 3.1 1996/02/01 05:14:47 hampton Exp $
 * $Source: /release/111/cvs/Xsys/xremote/parser_defs_xremote.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_xremote.h,v $
 * Revision 3.1  1996/02/01  05:14:47  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:53:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * xremote commands
 */
#define	XREMOTE_BUFFERSIZE_CMD	 1
#define	XREMOTE_TFTP_BUFFERSIZE	 2
#define	XREMOTE_TFTP_RETRIES	 3
#define	XREMOTE_TFTP_HOST	 4

/*
 *  XRemote exec commands
 */
#define PARSER_XREMOTE_LAT	1
#define PARSER_XREMOTE_XDM	2
#define PARSER_XREMOTE_XTREK	3
#define PARSER_XREMOTE_FONT	4
#define	PARSER_XREMOTE_DEFAULT	5
#define PARSER_XREMOTE_DISABLE  6
