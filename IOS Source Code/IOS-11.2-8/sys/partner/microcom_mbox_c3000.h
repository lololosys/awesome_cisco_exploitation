/* $Id: microcom_mbox_c3000.h,v 3.2 1996/03/09 00:47:38 ljiang Exp $
 * $Source: /release/111/cvs/Xsys/partner/microcom_mbox_c3000.h,v $
 *------------------------------------------------------------------
 * microcom_mbox_c3000.h -- Microcom/Cisco Terminator-specific Mailbox 
 *			    Header File
 *
 * August 1995, Li Jiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: microcom_mbox_c3000.h,v $
 * Revision 3.2  1996/03/09  00:47:38  ljiang
 * CSCdi50986:  Add a new procedure to fix a mailbox timeout problem
 *
 * Revision 3.1  1996/03/09  00:36:00  ljiang
 * CSCdi50986:  Add mailbox support for  Microcom Terminator(c2511
 * derivative)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __microcom_mbox_c3000_h_
#define __microcom_mbox_c3000_h_

/*
 *  Constants
 */

/* Mailbox status register bit definition (active low) */
/* Daughter Car Full Flag-- OBX. Initially '0' */
#define TERMINATOR_INBOX_EMPTY           0x0400

/* Carrier Card Full Flag-DBX. initially '1' */
#define TERMINATOR_OUTBOX_FULL           0x0800

/* Mailbox read/write masks */
#define TERMINATOR_MBOX_ENABLE           0x00
#define TERMINATOR_MBOX_READ_DISABLE     0x01
#define TERMINATOR_MBOX_WRITE_DISABLE    0x02
#define TERMINATOR_MBOX_DISABLE          0x03

/* Terminator specific constants: */
#define MAILBOX_MODULE_DESCR          "Microcom ComSvr" /* max 15 chars */
#define MAILBOX_MODULE_NUM            "CS-1716"/* max 15 chars */
#define MAILBOX_MODULE_NUM_EXT        "" /* max  3 chars */

#define MAILBOX_MODULE_IDENTIFIER	103
#define MAILBOX_MODULE_VENDOR     	11
#define MAILBOX_MODULE_CONFIG         	0

/* Supported capabilities */
#define MAILBOX_CAPABILITY_SET  \
       (MAILBOX_CAP__MAC_CONFIG_N | \
        MAILBOX_CAP__IP_CONFIG_N | \
        MAILBOX_CAP__IP_GATEWAY_N)


/* 
 * Function prototypes 
 */

static boolean outbox_full (void);
void microcom_subsys_init(subsystype *subsys);
void microcom_mailbox_v2_init(void);
void microcom_mailbox_v2_start_output(ushort c);
forktype mailbox_v2_output(void);
forktype mailbox_v2_input(void);
void microcom_mailbox_v2_fence_interface(void);
uchar *microcom_mailbox_v2_fill_personality_PDU (struct mbox_personality_PDU *p,
                                               ushort flags);
inline boolean microcom_mailbox_v2_is_complex_port(hwidbtype *idb);

static boolean outbox_disabled_or_full (void);
static boolean inbox_empty_or_disabled (void);

static uchar mailbox_v2_get_serial_conn (hwidbtype *idb);
static uchar mailbox_v2_get_serial_clock (hwidbtype *idb);

#endif __microcom_mbox_c3000_h_

