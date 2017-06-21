/* $Id: clns_adj.h,v 3.2 1995/11/17 08:51:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/clns_adj.h,v $
 *------------------------------------------------------------------
 * clns_adj.h --  This file contains data structures and constants
 *                used in establishing OSI adjacencies.
 *  
 * October 1991, Dino Farinacci
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_adj.h,v $
 * Revision 3.2  1995/11/17  08:51:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:16:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *    This file contains the data structures and constants
 *    used in establishing OSI adjacencies.
 *
 *    Adjacencies may be established by the following routing protocols:
 *       1) ISIS
 *       2) ISO-IGRP
 *       3) ESIS
 *       4) DECnet Phase IV
 *       5) Static configuration
 *	 6) NLSP
 *
 */

#ifndef __CLNS_ADJ_H__
#define __CLNS_ADJ_H__

#include "address.h"
#include "../clns/clns.h"
#include "../iprouting/route.h"

#define CLNS_ADJ_DB_SIZE 256

struct clns_adjtype_ {
    struct clns_adjtype_ *next;       /* link to next adjacency */
    struct clns_adjtype_ *next_alias; /* Next cluster alias */
    NSAP_T     key;                   /* ES or IS system id, or prefix */
    hwaddrtype snpa;                  /* MAC-layer address for neighbor */
    idbtype    *idb;                  /* which interface */
    char       state;                 /* init, up, failed */
    char       adjacencyType;         /* ES, IS, L1_IS, L2_IS, unknown */
    int        protocol_source;       /* static, esis, igrpx, isis */
    char       neighbor_pak_fmt;      /* Phase IV / Phase V */
    sys_timestamp expiration_time;    /* time when adj expires, 0 = never */
    mgd_timer  isis_expiration_timer; /* Timer so ISIS knows immediately */
    long       uses;                  /* number of times we used this info */
    long       last_use;              /* when we last used neighbor */
    int        L1_lan_priority;       /* to determine level-1 DR */
    int        L2_lan_priority;       /* to determine level-2 DR */
    boolean    L2only;                /* IS this adjacency level 2 only ? */
    int        num_areas;             /* number of area addresses for adj */
    NSAP_T     area_address[MAX_AREA_ADDRESSES]; /* area address(es) for adj */
    ipaddrtype ip_address[MAXROUTES]; /* IP addresses in IIHes from neighbor */
    ipaddrtype ip_nexthop;	      /* IP address we can use as nexthop */
    ulong      ipx_net;		      /* IPX net number from neighbor */
    uchar      ipx_node[IEEEBYTES];   /* IPX node number from neighbor */
    uchar      ipx_version;           /* IPX LSP minor version */
    ulong      ipx_area_net[MAX_AREA_ADDRESSES];  /* Net part of area addr */
    ulong      ipx_area_mask[MAX_AREA_ADDRESSES]; /* Mask part of area addr */
    ulong      ipx_area_count;		/* Number of ipx areas */
    uchar      L1_circuit_id[SYSTEMID_LENGTH]; /* L1 LAN ID advertised by nbr*/
    uchar      L2_circuit_id[SYSTEMID_LENGTH]; /* L2 LAN ID advertised by nbr*/
    sys_timestamp create_time;          /* when this adj was created */
};

/*
 * Values for 'state' field.  Note that these correspond to the NLSP spec!
 */
#define CLNS_ADJ_UP_STATE     0
#define CLNS_ADJ_INIT_STATE   1
#define CLNS_ADJ_FAILED_STATE 2
#define CLNS_ADJ_MAXSTATE CLNS_ADJ_FAILED_STATE
#define CLNS_ADJ_NUMSTATES CLNS_ADJ_MAXSTATE+1

/*
 * Values for 'adjacencyType' field
 */
#define CLNS_ES_ADJ_TYPE      0x1
#define CLNS_IS_ADJ_TYPE      0x2
#define CLNS_L1_IS_ADJ_TYPE   0x4
#define CLNS_L2_IS_ADJ_TYPE   0x8
#define CLNS_PREFIX_ADJ_TYPE  0x10
#define CLNS_ALL_ADJ_TYPE     (CLNS_ES_ADJ_TYPE | CLNS_IS_ADJ_TYPE | \
			       CLNS_L1_IS_ADJ_TYPE | CLNS_L2_IS_ADJ_TYPE | \
			       CLNS_PREFIX_ADJ_TYPE)
 
#define CLNS_NLSP_IS_ADJ_TYPE 0x20
#define CLNS_NLSP_SVC_ADJ_TYPE 0x40	/* Not really in adj database */
#define CLNS_NLSP_XROUTE_ADJ_TYPE 0x80	/* Ditto */
#define CLNS_NLSP_SROUTE_ADJ_TYPE 0x100
#define CLNS_NLSP_ALL_ADJ_TYPE (CLNS_NLSP_IS_ADJ_TYPE | \
				CLNS_NLSP_SVC_ADJ_TYPE | \
				CLNS_NLSP_SROUTE_ADJ_TYPE | \
				CLNS_NLSP_XROUTE_ADJ_TYPE)

/*
 * Values for 'neighbor_pkt_format' field 
 */
#define CLNS_NO_FORMAT       0
#define CLNS_PHASE_IV_FORMAT 1
#define CLNS_PHASE_V_FORMAT  2

#endif __CLNS_ADJ_H__
