/* $Id: fddi.h,v 3.6.12.2 1996/04/22 18:12:58 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/if/fddi.h,v $
 *------------------------------------------------------------------
 * fddi.h -- Fiber Distributed Data Interface 
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddi.h,v $
 * Revision 3.6.12.2  1996/04/22  18:12:58  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.6.12.1  1996/03/18  20:13:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.2  1996/02/20  00:56:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/13  20:25:24  hampton
 * Move platform specific defines from common include files to platform
 * specific include files.  [CSCdi48844]
 *
 * Revision 3.5.16.1  1996/01/24  22:18:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/06  18:29:49  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.4  1995/11/26  08:13:12  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              Catalyst switches now support the use of the Cisco
 *              multicast 0x01000C000000 for ISL vLAN encapsulated
 *              packets.
 *
 * Revision 3.3  1995/11/17  09:27:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:49:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:02:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/09  00:52:30  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.2  1995/06/13  03:28:11  alam
 * CSCdi35570:  Remove extraneous functions
 * Use llc_process_test and llc_process_xid for FDDI as well as ethernet
 * and FR.
 * Remove FDDI packet debug.
 *
 * Revision 2.1  1995/06/07  20:52:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FDDI_H__
#define __FDDI_H__

#include "ieee.h"

#define FDDI_ENCAPBYTES             13
#define FDDI_8025_ENCAPBYTES        13
#define FDDI_SAP_ENCAPBYTES         16
#define FDDI_SNAP_ENCAPBYTES        21
#define FDDI_ISO3_ENCAPBYTES	    15
#define FDDI_APOLLO_ENCAPBYTES      51

#define FDDI_ENCAPWORDS             7
#define FDDI_SAP_ENCAPWORDS         8
#define FDDI_SNAP_ENCAPWORDS       11

/*
 * FDDI specific header.  Used by LLC2.
 */
typedef struct fddi_hdr_ {
    uchar  fc;
    uchar  daddr[IEEEBYTES];
    uchar  saddr[IEEEBYTES];
    uchar  data[0];
} fddi_hdr;

typedef struct _fddirif_t {
    uchar fc;
    uchar daddr[6];
    uchar saddr[6];
    uchar rc_blth;
    uchar rc_dlf;
    ushort PACKED(rd[0]);
} fddirif_t;

/*
 * Novell uses a weird encapsulation on fddi that uses only a MAC header,
 * and does not have a dsap or lsap.  Instead, an XNS datagram immediately
 * follows.  Fortunately, Novell uses FFFF as the XNS checksum, and we can
 * check for this to recognize Novell packets on an 
 */
typedef struct fddi_novell_ {
    uchar  fc;
    uchar  daddr[IEEEBYTES];
    uchar  saddr[IEEEBYTES];
    uchar data[0];		/* 1st word is checksum */
} fddi_novell;

/*
 * 802 SNAP encapsulation for apollo on FDDI.
 */
typedef struct fddi_apollo_ {
    uchar  fc;
    uchar  daddr[IEEEBYTES];
    uchar  saddr[IEEEBYTES];
    uchar  dsap;
    uchar  ssap;
    uchar  control;
    uchar  oui[SNAP_OUIBYTES];
    uchar  type[2];
    uchar  private[30];
} fddi_apollo;

#define FDDI_IP_MTU             4352   /* Per RFC1188 */

#define FDDI_GROUP		0x80
#define FDDI_LOCAL		0x40

#define FDDI_FRAME_SIZE		4500

#define FDDI_PREAMBLE		2
#define FDDI_SD			1
#define FDDI_FC			1
#define FDDI_DA			6
#define FDDI_SA			6    
#define FDDI_FCS    		4
#define FDDI_EFS		2
#define FDDI_LLC                3
#define FDDI_STATUS             1

#define FDDI_INFO_SIZE      	(FDDI_FRAME_SIZE  - (FDDI_PREAMBLE + \
						    FDDI_SD +	\
						    FDDI_FC +	\
						    FDDI_DA +	\
						    FDDI_SA +	\
						    FDDI_FCS +	\
						    FDDI_EFS))

#define FDDI_BUFFERSIZE_BYTES  	(FDDI_INFO_SIZE + (FDDI_FC + \
						   FDDI_DA + \
						   FDDI_SA + \
						   FDDI_FCS + \
						   1))

/*
 * This is FDDI_BUFFERSIZE_BYTES minus the maxmtu
 */
#define FDDI_OVERHEAD_BYTES     (FDDI_BUFFERSIZE_BYTES - \
				  (FDDI_INFO_SIZE - \
				   (FDDI_LLC + FDDI_SNAP)))

#define FDDI_MINDGRAM		(FDDI_FC + FDDI_DA + FDDI_SA + FDDI_LLC)

#define FDDI_MAXDGRAM		(FDDI_FRAME_SIZE - (FDDI_PREAMBLE + \
						    FDDI_SD + \
						    FDDI_FCS + \
						    FDDI_EFS))
#define FDDI_ASYNCHRONOUS_FRAME	0x00
#define FDDI_SYNCHRONOUS_FRAME  0x80    
#define FDDI_48BIT_ADDRESS	0x40
#define FDDI_LLC_FRAME		0x10
#define FDDI_PRIORITY_ZERO	0x00

#define FDDI_LLC_FC_BYTE        (FDDI_ASYNCHRONOUS_FRAME | \
				 FDDI_48BIT_ADDRESS |      \
				 FDDI_LLC_FRAME |          \
				 FDDI_PRIORITY_ZERO)

#define MAXFDDISIZE		4500

#define DEFAULT_T_TVX_TIME	2500		/* 2500 microseconds */
#define DEFAULT_T_TVX_MIN_TIME	3400		/* 3400 microseconds */
#define DEFAULT_T_TVX_MAX_TIME	5200


#define T_MIN_SYMBOLS		100000
#define T_MAX_SYMBOLS		4125000
#define FDDI_SYMBOLS_SECOND	25000000
#define FDDI_BYTES_SECOND	(FDDI_SYMBOLS_SECOND / 2)

#define TVX_MIN_SYMBOLS		62500           /* equal to 2.5 milliseconds */
#define CISCO_DEFAULT_TVX		(TVX_MIN_SYMBOLS / 2)

#define DEFAULT_PHY_A_TVAL	0x0008
#define DEFAULT_PHY_B_TVAL	0x020C

#define WRAP_A_FAIL        	0x0001
#define WRAP_B_FAIL        	0x0002
#define THRU_A_FAIL        	0x0004
#define FDDI_SHUTDOWN_DONE      0x8000


/*
 * Routing information indicator definitions:
 */

#define FDDI_RII   0x80
#define FDDIT_RII  0x01

/*
 * FDDI options.
 */
#define FDDI_DUPLICATE_ADDRESS_CHK	0x0001
#define FDDI_SMT_FRAMES			0x0002
#define FDDI_LEM_SUPPORTED		0x0004
#define FDDI_CMT_ASSIST_SUPPORTED	0x0008
#define FDDI_VERSION_128		0x0010
#define FDDI_BRIDGE_CAM_SUPPORTED	0x0020 /* used to be FDDI_FDDIT */
#define FDDI_PHY_GET_LS_SUPPORTED	0x0040
#define FDDI_ENCAPSULATE		0x0080
#define FDDI_FAST_CMT			0x0100
#define FDDI_FAST_CMT_ENB               0x0200

enum {
    FDDI_HW_CLAIMS=IDB_TYPE_BASE_FDDI+1,
    FDDI_HW_BEACONS,
    FDDI_HW_TRACE_COUNT,

    FDDI_HW_SI_CLAIMS,
    FDDI_HW_SI_BEACONS,
    FDDI_HW_SI_TRACE_COUNT,

    FDDI_HW_CFM_STATE,
    FDDI_HW_TRT,
    FDDI_HW_TRT_NEG,
    FDDI_HW_BURST_COUNT,
    FDDI_HW_FRINC,
    FDDI_HW_TVX,
    FDDI_HW_OPERATIONAL,
    FDDI_HW_SHUTDOWN_REASONS,

    FDDI_HW_CLAIMS_STAT,
    FDDI_HW_BEACONS_STAT,
    FDDI_HW_TRACE_COUNT_STAT,
    FDDI_HW_CLEAR_STATS
};

/*
 * Definitions for fddi cam support for various platforms.
 * Slave reads to CAM to find out whether an address exists or not are very
 * costly from performance point of view. For FDDI-PAs (mif68840), we need
 * to populate CAM the first three times and then every 16k packets if the
 * CAM is still not rejecting packets based on Src-Dest addr match. This is
 * not needed on FIP as we get an indication FDDIT_SA_FND. So we don't have
 * to go through this exercise on FIP.
 * We give three chances for an incoming packet go through refresh cam path.
 * Note:  the arp packet does not go through this path.
 * Do every 16k afterwards to avoid the cam flush out by other entries.
*/
#define FDDI_CAM_REFRESH_CHECK           0x3ff
#define FDDI_CAM_INITIAL_REFRESH_PERIOD  (FDDI_CAM_REFRESH_CHECK*3)
#define FDDI_CAM_REFRESH_PERIOD          0x3fff

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

void fddi_idb_enqueue(hwidbtype *hwidb);
void fddi_idb_dequeue(hwidbtype *hwidb);

extern void start_smt_input_proc(void);

#endif /* __FDDI_H__ */
