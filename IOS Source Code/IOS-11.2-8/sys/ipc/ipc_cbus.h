/* $Id: ipc_cbus.h,v 3.3 1996/01/13 03:09:48 dlobete Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/ipc/ipc_cbus.h,v $
 *------------------------------------------------------------------
 * CBUS specific routines
 *
 * March 1995, Don Lobete
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_cbus.h,v $
 * Revision 3.3  1996/01/13  03:09:48  dlobete
 * CSCdi44696:  OIR and IPC troubles - selective sequence number synch
 * needed
 * Add slot granularity to IPC seat reset.
 *
 * Revision 3.2  1995/11/17  09:37:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:02:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SLOT_0_NAME     "CBUS: Slot 0"
#define SLOT_1_NAME     "CBUS: Slot 1"
#define SLOT_2_NAME     "CBUS: Slot 2"
#define SLOT_3_NAME     "CBUS: Slot 3"
#define SLOT_4_NAME     "CBUS: Slot 4"
 
#define SLOT_0_SEAT     0x01000000
#define SLOT_1_SEAT     0x01200000
#define SLOT_2_SEAT     0x01400000
#define SLOT_3_SEAT     0x01600000
#define SLOT_4_SEAT     0x01800000

#define IPC_CIP_ENCAP 0xbead

/*
 * Structures for Master-Slave Initialization Scheme.
 */
typedef struct ipc_slave_cbus {
    int slot;
    int controller_type;
    hwidbtype *idb;
}ipc_slave_cbus;

typedef struct ipc_slave_list_elt {
    union {
        ipc_slave_cbus cbus;
    }type_of_slave;
    struct ipc_slave_list_elt *next;
}ipc_slave_list_elt;

extern void ipc_cbus_rx(hwidbtype *hwidb);
extern boolean media_sniff_ipc (mcitype *cbus, hwidbtype *hwidb,
						ushort offset_to_sniff);
