/* $Id: ipc_cbus_vip.h,v 3.3.12.2 1996/06/10 04:52:01 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_cbus_vip.h,v $
 *------------------------------------------------------------------
 * ipc_cbus_vip.h - CBUS VIPER Includes
 *
 * May 1995, Don Lobete
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_cbus_vip.h,v $
 * Revision 3.3.12.2  1996/06/10  04:52:01  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 *   - Use a more efficient scheme for slave-log port-name generation at
 *           both RP/RSP and VIP
 *   - Pull slave application details out of IPC CORE files
 *
 * Revision 3.3.12.1  1996/04/12  17:42:53  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/18  19:41:46  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.2  1995/11/17  17:28:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/30  00:21:34  dlobete
 * Placeholder for IPC.
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

#define IPC_MAX_SLOTS 5
 
/*
 * Structures for Master-Slave Initialization Scheme.
 */
typedef struct ipc_slave_cbus {
    int slot;
    int controller_type;
    hwidbtype *idb;
} ipc_slave_cbus;
 
typedef struct ipc_slave_list_elt {
    union {
        ipc_slave_cbus cbus;
    } type_of_slave;
    struct ipc_slave_list_elt *next;
} ipc_slave_list_elt;

/*
 * Prototypes
 */
extern boolean 	ipc_do_vip_init(void);
extern void 	ipc_do_vip_enable(ipc_port_id, ipc_seat);
extern boolean 	cbus_transmit_to_rp(ipc_message *, ipc_seat_data *);
extern void 	ipc_process_raw_pak(paktype *);
extern void	ipc_quiesce(void);
extern void	ipc_unquiesce(void);

/* end of file */
