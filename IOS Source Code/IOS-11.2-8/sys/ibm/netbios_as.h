/* $Id: netbios_as.h,v 3.2.60.2 1996/07/11 19:19:16 syiu Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_as.h,v $
 *------------------------------------------------------------------
 * netbios_as.h - NBF Access Server definitions
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as.h,v $
 * Revision 3.2.60.2  1996/07/11  19:19:16  syiu
 * CSCdi62549:  NBFCP disabled on Vaccess interfaces
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  20:11:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:23:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  14:21:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:23:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:23:05  syiu
 * Initial placeholder for NBF (NetBIOS Frames) support.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * LLC 1 and 2 definitions
 */
#define N_MASK 0xFE 
#define PF_BIT 0x01

#define NETBIOS_MAX_SESSIONS 255
#define NETBIOS_HASH_SESS_LEN 0x100
#define NETBIOS_NBF_QUEUE_TIMEOUT (3*ONESEC)
#define NETBIOS_NAME_PROJ_DISPERSE   1
#define NETBIOS_NAME_PROJ_REASSEMBLE 2

#define IS_NETBIOS_ACCESS(idb) (is_async(idb) || is_isdn(idb) || (idb->type == IDBTYPE_VACCESS))
#define IS_NETBIOS_LAN(idb) (is_ethernet(idb) || is_tokenring(idb))

/*
 * NetBIOS NCB Return Codes (some)
 */
#define NETBIOS_NCB_GOOD_RETURN 0x00
#define NETBIOS_NCB_DUP_LOCAL_NAMETABLE 0x0D
#define NETBIOS_NCB_NAMETABLE_FULL 0x0E
#define NETBIOS_NCB_NAME_NOT_FOUND 0x15
#define NETBIOS_NCB_NAME_INUSE 0x16
#define NETBIOS_NCB_NAME_CONFLICT_DETECTED 0x19
#define NETBIOS_NCB_INTERFACE_BUSY 0x21
#define NETBIOS_NCB_NAME_DEF 0x30
#define NETBIOS_NCB_NO_SYSRESOURCE 0x35

/*
 * NetBIOS Name Query data2 return codes
 */
#define NETBIOS_NAME_QUERY_NO_LISTEN 0x00
#define NETBIOS_NAME_QUERY_NO_RESOURCE 0xFF

/*
 * Checks whether NETBIOS frame is an I-Frame or U-Frame.
 * condition TRUE is I-Frame, U-Frame otherwise
 */
#define NETBIOS_IS_IFRAME(nbh) \
((nbh->command >= NETBIOS_ADD_GROUP_QUERY) && \
  (nbh->command <= NETBIOS_TERMINATE_TRACE_REM_LOC))

struct netbios_sess_entry_ {
 uchar dstnum;                 /* Destination number of current NetBIOS pkt */
 hwidbtype *dstidb;            /* Target device of sender SESSION_CONFIRM */
 llctype *dst_llc;             /* LLC connection on target device */
 uchar dstmacaddr[IEEEBYTES];  /* Destination MAC address of sender SESSION_CONFIRM */
};

struct netbios_lan_sess_entry_ {
 struct netbios_lan_sess_entry_ *next;
 uchar srcnum;      /* Source session number of current NetBIOS packet */
 uchar dstnum;      /* Destination session number of current NetBIOS packet */
 hwidbtype *srcidb; /* Receiving LAN device of SESSION_INITIALIZE */
 hwidbtype *dstidb; /* Target device of sender SESSION_CONFIRM */
 llctype *dst_llc;  /* LLC connection on target device */
 uchar srcmacaddr[IEEEBYTES]; /* Source MAC address of all nodes on this LAN*/
 uchar dstmacaddr[IEEEBYTES]; /* Dest  MAC address to send to */
};

typedef struct netbios_nbfcp_name_proj_entry_ {
 uchar netbios_name[NETBIOS_MAX_NAME_LEN];
 uchar reason;
} netbios_nbfcp_name_proj_entry;
