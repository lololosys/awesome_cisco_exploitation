/*
 *	P A R S E R _ D E F S _ T U N N E L . H
 *
 * $Id: parser_defs_tunnel.h,v 3.1 1996/02/01 04:58:35 hampton Exp $
 * $Source: /release/111/cvs/Xsys/ip/parser_defs_tunnel.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_tunnel.h,v $
 * Revision 3.1  1996/02/01  04:58:35  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:53:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Tunnel interface commands
 */
#define	TUNNEL_CMD_CARRY_SEC		1
#define	TUNNEL_CMD_SEQUENCING		2
#define	TUNNEL_CMD_CHECKSUM		3
#define	TUNNEL_CMD_KEY			4
#define	TUNNEL_CMD_MODE			5
#define	TUNNEL_CMD_DESTINATION		6
#define	TUNNEL_CMD_SOURCE		7
