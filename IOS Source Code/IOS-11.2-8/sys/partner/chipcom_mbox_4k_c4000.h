/* $Id: chipcom_mbox_4k_c4000.h,v 3.4 1996/03/08 23:38:19 ljiang Exp $
 * $Source: /release/111/cvs/Xsys/partner/chipcom_mbox_4k_c4000.h,v $
 *------------------------------------------------------------------
 * chipcom_mbox_4k_c4000.h -- Margarita-specific Mailbox Header File
 *
 * February 1995, Cheng-Lee Nee
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The following file provides header information for the Cortez
 * implementation of the Chipcom mailbox protocol.
 *------------------------------------------------------------------
 * $Log: chipcom_mbox_4k_c4000.h,v $
 * Revision 3.4  1996/03/08  23:38:19  ljiang
 * CSCdi50986:  Add mailbox support for Microcom Terminator(c2511
 * derivative)
 *
 * Revision 3.3  1996/02/24  20:13:37  cnee
 * CSCdi49808:  Partner mailbox messages trashed under heavy mailbox
 * traffics
 *
 * Revision 3.2  1995/11/17  18:53:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:32:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __chipcom_mbox_4k_c4000_h_
#define __chipcom_mbox_4k_c4000_h_

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This file must be included AFTER chipcom_mbox.h, as it references constants 
 * defined in that file.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */


/*
 * Constants.
 */

/* Mailbox status register bit definition (active low) */
#define MARGARITA_INBOX_EMPTY		0x8000
#define MARGARITA_OUTBOX_FULL		0x4000

/* Mailbox read/write masks */
#define MARGARITA_MBOX_ENABLE		0x00
#define MARGARITA_MBOX_READ_DISABLE	0x01
#define MARGARITA_MBOX_WRITE_DISABLE	0x02
#define MARGARITA_MBOX_DISABLE		0x03

/*
 * Margarita specific constants:
 */
#define MAILBOX_MODULE_DESCR          "ONcore TRBR" /* max 15 chars */

/* External NIM slot number */
#define EXT_NIM_SLOT                  0             /* slot 1 */

/* External NIM types (Max 15 chars) */
#define MAILBOX_MODULE_NUM_1FDM       "6701CS-FDM"  /* 1-port FDDI */
#define MAILBOX_MODULE_NUM_1FDS       "6701CS-FDS"  /* 1-port FDDI */
#define MAILBOX_MODULE_NUM_1FSM       "6701CS-FSM"  /* 1-port FDDI */
#define MAILBOX_MODULE_NUM_1FSS       "6701CS-FSS"  /* 1-port FDDI */
#define MAILBOX_MODULE_NUM_4SER       "6704CS-SER"  /* quad serial */
#define MAILBOX_MODULE_NUM_2E         "6702CS-E"    /* dual Ethernet */
#define MAILBOX_MODULE_NUM_4BRI       "6704CS-BRI"  /* 4-port BRI */
#define MAILBOX_MODULE_NUM_8BRI       "6708CS-BRI"  /* 8-port BRI */
#define MAILBOX_MODULE_NUM_2TR        "6702CS-T"    /* dual token ring */
#define MAILBOX_MODULE_NUM_UNKNOWN    ""            /* none or unknown */

#define MAILBOX_MODULE_IDENTIFIER     0
#define MAILBOX_MODULE_VENDOR         2
#define MAILBOX_MODULE_CONFIG         3

/* Supported capabilities */
#define MAILBOX_CAPABILITY_SET  \
       (MAILBOX_CAP__MAC_CONFIG_N | \
        MAILBOX_CAP__IP_CONFIG_N | \
        MAILBOX_CAP__IP_GATEWAY_N)


/*
 * Function prototypes
 */
void chipcom_subsys_init (subsystype *subsys);
void mailbox_v2_init(void);
void mailbox_v2_start_output(ushort c);
forktype mailbox_v2_output(void);
forktype mailbox_v2_input(void);
void mailbox_v2_fence_interface(void);
uchar *mailbox_v2_fill_personality_PDU (struct mbox_personality_PDU *p,
                                               ushort flags);
inline boolean mailbox_v2_is_complex_port(hwidbtype *idb);

static boolean outbox_full (void);
static boolean outbox_disabled_or_full (void);
static boolean inbox_empty_or_disabled (void);
static uchar mailbox_v2_get_fddi_conn (hwidbtype *idb);
static uchar mailbox_v2_get_ether_conn (hwidbtype *idb);
static uchar mailbox_v2_get_serial_conn(hwidbtype *idb);
static uchar mailbox_v2_get_serial_clock(hwidbtype *idb);

#endif __chipcom_mbox_4k_c4000_h_
