/*
 *      P A R S E R _ D E F S _ D L S W . H
 * 
 * $Id: parser_defs_dlsw.h,v 3.1.40.5 1996/09/09 19:09:25 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/parser_defs_dlsw.h,v $
 *
 * July 1994-1995, Frank Bordonaro 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_dlsw.h,v $
 * Revision 3.1.40.5  1996/09/09  19:09:25  akhanna
 * CSCdi65724:  Cannot stop keepalives on promiscous peer
 * Branch: California_branch
 *
 * Revision 3.1.40.4  1996/08/16  06:56:13  ioakley
 * CSCdi58903:  Add dlsw touch timer support for sna/netbios
 *              reachability cache entries.
 * Branch: California_branch
 *
 * Revision 3.1.40.3  1996/08/09  21:55:54  ravip
 * CSCdi59181:  Best path not chosen for the first circuit
 * Branch: California_branch
 *
 * Revision 3.1.40.2  1996/07/31  18:20:05  akhanna
 * CSCdi61887:  dlsw icanreach saps
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/07/23  06:29:53  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:57:27  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:50:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/13  17:47:20  ravip
 * CSCdi35261:  DLSw should permit ARB from target
 *              Nerd knob to control ARB and SRB of SNA and Netbios frames
 *              Store test frames if search is pending for the dmac
 *
 * Revision 2.1  1995/06/07  22:29:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * DLSw system global command constants
 */
#define DLSW_DISABLE            0
#define DLSW_NORNR              1

/*
 * DLSw CSM command constants
 */
#define CSM_DUP_PATH             0
#define CSM_EXPLQ_DEPTH          1
#define CSM_TIMERS               2
#define CSM_MAC_ADDR             3
#define CSM_NETBIOS_NAME         4
#define CSM_ALLROUTE_SNA         5
#define CSM_ALLROUTE_NETBIOS     6
#define CSM_TOUCH_TIMERS         7

/*
 * CSM Timer Command Types
 */
#define CSM_SNA_EXPL_T           0
#define CSM_SNA_CACHE_T          1
#define CSM_SNA_VERIFY_T         2
#define CSM_SNA_RETRY_T          3
#define CSM_NB_EXPL_T            4
#define CSM_NB_CACHE_T           5
#define CSM_NB_VERIFY_T          6
#define CSM_NB_RETRY_T           7
#define CSM_NEG_CACHE_T          8
#define CSM_EXPL_WAIT_T          9
#define CSM_EXPL_DELAY_T         10
#define CSM_MAX_TIMERS           11

/*
 * CSM Static MAC Addr and NetBios Name Mapping Command Types
 */
#define CSM_RES2RIF              0
#define CSM_RES2RING             1
#define CSM_RES2IPADDR           2
#define CSM_RES2INTERFACE        3
#define CSM_RES2PGROUP           4

/*
 * CSM Touch Timer Command Types
 */
#define CSM_TOUCH_SNA_CACHE_T    0
#define CSM_TOUCH_NB_CACHE_T     1

/* 
 * DLSw peer command constants 
 */
#define PEER_LOCAL		 0
#define PEER_REMOTE		 1
#define PEER_PORT_LIST           2
#define PEER_ICANREACH           3
#define PEER_ICNR                4
#define PEER_POD_DEFAULTS        5
#define PEER_FORCE_SSP           6
#define PEER_RING_LIST           7
#define PEER_BGROUP_LIST         8

#define PEER_BGROUP_ASSIGN       9
#define PEER_PROM_DEFAULTS       10
#define DLSW_SPAN_MAXINDEX 64   /* maximum bridge group number as defined
				 * by transparent bridging code.  Needs to
				 * track symbol 'SPAN_MAXINDEX' as define
				 * in 'srt/span.h' and './parser_defs_bridge.h */
/* 
 * DLSW transport type
 */
#define DLSW_UNKNOWN_XPORT       0
#define DLSW_TCP 		 1
#define DLSW_FST		 2
#define DLSW_IF 		 3
#define DLSW_IF_LAN 		 4
#define DLSW_FR                  5

/*
 * DLSw resources that can be locally reached by this router
 */
#define ICR_MAC_EXCL             0
#define ICR_NB_EXCL              1
#define ICR_MAC                  2
#define ICR_NBNAME               3
#define ICR_SAPS                 4

/*
 * DLSw resources that can NOT be locally reached by this router
 */
#define PEER_ICNR_SAP            0
