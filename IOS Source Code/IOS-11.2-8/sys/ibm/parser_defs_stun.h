/*
 *	P A R S E R _ D E F S _ S T U N . H
 *
 * $Id: parser_defs_stun.h,v 3.1 1996/02/01 04:58:18 hampton Exp $
 * $Source: /release/111/cvs/Xsys/ibm/parser_defs_stun.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_stun.h,v $
 * Revision 3.1  1996/02/01  04:58:18  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/27  03:11:31  vrawat
 * CSCdi33572:  AS/400 sdlc multidrop enviromnment
 *
 * Revision 2.1  1995/06/07  22:31:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* STUN command defines */
#define STUN_PEER		 0
#define STUN_POLL		 1
#define STUN_PRIMARY		 2
#define STUN_PROTOCOL		 3
#define STUN_SCHEMA		 4
#define STUN_KEEPALIVE           5
#define STUN_KEEPALIVE_COUNT     6
#define STUN_SLACK_FSM_TIMEOUT   7
#define STUN_QUICK_RESPONSE      8

/* STUN Interface subcommand defines */
#define STUNIF_PROXY		 0
#define STUNIF_GROUP		 1
#define STUNIF_ROUTE		 2
#define	NO_STUN			 3
#define	STUNIF_SDLCROLE		 4


/* STUN proxy poll types */
#define STUNIF_PROXY_DISCOVERY	 0
#define STUNIF_PROXY_PRIMARY	 1
#define STUNIF_PROXY_SECONDARY	 2

/* STUN route */
#define STUN_RTALL		 1
#define STUN_RTADDR		 2
#define STUN_TCP		 3
#define STUN_INTF		 4
#define STUN_INTFDIR		 5








