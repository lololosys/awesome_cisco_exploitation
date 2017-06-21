/* $Id: gdp.h,v 3.2.60.1 1996/04/15 21:17:49 bcole Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/gdp.h,v $
 *------------------------------------------------------------------
 * gdp.h -- Gateway Discovery Protocol data structures
 *
 * Split from gdp.c, September 12 1990
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdp.h,v $
 * Revision 3.2.60.1  1996/04/15  21:17:49  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:33:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  03:21:09  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.1  1995/06/07 21:06:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#ifndef __GDP_H__
#define __GDP_H__

/*
 * Default parameters
 */
#define GDP_PRIORITY	100		/* a number, any number */
#define GDP_REPORTTIME	5		/* send update every 5 seconds */
#define GDP_XREPORTTIME	0		/* send update every 0 seconds */
#define GDP_HOLDTIME	15		/* believe me until 15 seconds */

#define GDP_MIN_INTERVAL (2*ONESEC)	/* Minimum computed periodicity between
					   observed updates */

/*
 * GDP header
 */
#define GDPHEADERBYTES	4
#define GDP_VERSION	1
#define GDP_OPC_REPORT	1
#define GDP_OPC_QUERY	2

typedef struct {
    uchar version;
    uchar opcode;
    uchar count;
    uchar reserved;
} gdptype;

/*
 * GDP address information.  Follows GDP header.
 */
#define GDPDATABYTES(count) (8*(count))

typedef struct {
    ipaddrtype address;
    ushort priority;
    ushort holdtime;
} gdpinfo;

/*
 * A local data structure for storing proxy information
 */
typedef struct gproxy {
    struct gproxy *next;
    ipaddrtype address;
    ushort priority;
    ushort holdtime;
} gproxy;

/*
 * structure to hold candidates for default router (for gdp client)
 */

typedef struct gdp_candidate {
    struct gdp_candidate *next;
    ipaddrtype address;
    sys_timestamp lastupdate;
    sys_timestamp expiretime;
    idbtype *source_idb;
    int source;
    int priority;
    int interval;
} gdp_candidate;

#define GDP_SOURCE_GDP  1
#define GDP_SOURCE_RIP  2
#define GDP_SOURCE_IGRP 3
#define GDP_SOURCE_IRDP 4

#define GDP_RIP_PRIORITY 50
#define GDP_IGRP_PRIORITY 60

#define GDP_MIN_INTERVAL (2*ONESEC)	/* Minimum computed periodicity between
					   observed updates */

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * gdpclient.c
 */
extern watched_queue *gdp_clientQ;
extern boolean gdp_gdp_enable;
extern boolean gdp_igrp_enable;
extern boolean gdp_rip_enable;
extern boolean gdp_irdp_enable;
extern boolean gdp_irdp_multicast;
extern void gdpclient_init(void);
extern void ip_gdpclient_command(parseinfo *);
extern ipaddrtype gdp_pickrouter(ipaddrtype);

/*
 * gdpclient_chain.c
 */
extern void gdpclient_parser_init(void);

/*
 * gdp_chain.c
 */
extern void gdp_parser_init(void);

#endif __GDP_H__
