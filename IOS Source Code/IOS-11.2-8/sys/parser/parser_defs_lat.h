/*
 *	P A R S E R _ D E F S _ L A T . H
 *
 * $Id: parser_defs_lat.h,v 3.3.20.1 1996/05/30 23:45:07 hampton Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_defs_lat.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_lat.h,v $
 * Revision 3.3.20.1  1996/05/30  23:45:07  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/29  07:01:00  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:51:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  LAT commands
 */
#define	LAT_ACCESS		 1
#define	LAT_AUTOCOMMAND		 2
#define	LAT_ENABLE_CMD		 3
#define	LAT_ENABLED		 4
#define	LAT_GROUPL		 5
#define	LAT_GROUP_LIST		 6
#define	LAT_GROUP_DISABLED	 7
#define	LAT_GROUP_ENABLED	 8
#define	LAT_GROUP_SET		 9
#define	LAT_GROUP_ALL		10
#define	LAT_HOST		11
#define	LAT_IDENT		12
#define	LAT_KATIMER		13
#define LAT_NODENAME		14
#define	LAT_PASSWORD		15
#define	LAT_RATING		16
#define	LAT_RETRANSMIT		17
#define	LAT_ROTARY		18
#define	LAT_SVC			19
#define	LAT_SERVICEG		20
#define	LAT_SERVICET		21
#define	LAT_VCTIMER	   	22
#define	LAT_OGROUP		23
#define LAT_SERVICE_RESPONDER	24
#define LAT_SERVICE_ANNOUNCEMENTS 25
#define LAT_VC_SESSIONS		26
#define LAT_EXTRA_HOST		27
#define LAT_EXTRA_SERVER	28
#define LAT_HDTIMER             29
#define LAT_REMOTE_MODIFY       30

/*
 * LAT show commands
 */
enum {
    SHOW_LAT,
    SHOW_LAT_ACCESS,
    SHOW_LAT_AS,
    SHOW_LAT_GROUPS,
    SHOW_LAT_NODES,
    SHOW_LAT_SERVERS,
    SHOW_LAT_SERVICES,
    SHOW_LAT_SESSIONS,
    SHOW_LAT_TRAFFIC,
};
