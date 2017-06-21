/* $Id: chipcom_mbox_c3000.h,v 3.3 1996/03/08 23:38:21 ljiang Exp $
 * $Source: /release/111/cvs/Xsys/partner/chipcom_mbox_c3000.h,v $
 *------------------------------------------------------------------
 * chipcom_mbox_c3000.h -- Cortez-specific Chipcom Mailbox Header File
 *
 * July 1994, John G. Kuzma
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The following file provides header information for the Cortez 
 * implementation of the Chipcom mailbox protocol.
 *------------------------------------------------------------------
 * $Log: chipcom_mbox_c3000.h,v $
 * Revision 3.3  1996/03/08  23:38:21  ljiang
 * CSCdi50986:  Add mailbox support for Microcom Terminator(c2511
 * derivative)
 *
 * Revision 3.2  1995/11/17  18:53:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/14  00:18:39  jkuzma
 * CSCdi37060:  Change version PDU sent to partner box for IPIPX and D-IBM
 *              images.
 *
 * Revision 2.1  1995/06/07  22:32:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __chipcom_mbox_c3000_h_
#define __chipcom_mbox_c3000_h_

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This file must be included AFTER chipcom_mbox.h, as it references constants 
 * defined in that file.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */


/*
 * Interrupt mask bits 
 */
#define ENABLE_MBOX_INTERRUPTS        0x03
#define DISABLE_MBOX_INTERRUPTS       0x00
#define DISABLE_MBOX_READ_INTR        0x01
#define DISABLE_MBOX_WRITE_INTR       0x02

/*
 * Cortez specific constants:
 */
#define MAILBOX_MODULE_DESCR_E        "Edge Rtr EntrSW" /* max 15 chars */
#define MAILBOX_MODULE_DESCR_D        "Edge Rtr DeskSW" /* max 15 chars */
#define MAILBOX_MODULE_DESCR_DIBM     "Edge Rtr DIBMSW" /* max 15 chars */
#define MAILBOX_MODULE_DESCR_IPIPX    "Edge Rtr IPSW"   /* max 15 chars */
#define MAILBOX_MODULE_DESCR_UNKNOWN  "Edge Rtr ??? SW" /* max 15 chars */
#define MAILBOX_MODULE_NUM_E          "R-CSE"           /* max 15 chars */
#define MAILBOX_MODULE_NUM_D          "R-CSD"           /* max 15 chars */
#define MAILBOX_MODULE_NUM_DIBM       "R-CSDI"          /* max 15 chars */
#define MAILBOX_MODULE_NUM_IPIPX      "R-CSIX"          /* max 15 chars */
#define MAILBOX_MODULE_NUM_UNKNOWN    "R-???"           /* max 15 chars */
#define MAILBOX_MODULE_NUM_EXT        ""                /* max  3 chars */
#define MAILBOX_MODULE_IDENTIFIER_E   81
#define MAILBOX_MODULE_IDENTIFIER_TR  83
#define MAILBOX_MODULE_VENDOR          2
#define MAILBOX_MODULE_CONFIG          0

/*
 * Constants.
 */
#define MAILBOX_CAPABILITY_SET  \
       (MAILBOX_CAP__MAC_CONFIG_N | MAILBOX_CAP__IP_CONFIG_N)

/*
 * Function prototypes for the Cancun version, aka, "Cortez".
 */
void chipcom_subsys_init(subsystype *subsys);
void mailbox_v2_init(void);
void mailbox_v2_start_output(ushort c);
void mailbox_v2_outbox_isr(void);
void mailbox_v2_inbox_isr(void);
void mailbox_v2_fence_interface(void);
uchar *mailbox_v2_fill_personality_PDU (struct mbox_personality_PDU *p,
                                               ushort flags);
inline boolean mailbox_v2_is_complex_port (hwidbtype *idb);

int  mailbox_v2_get_serial_conn(hwidbtype *idb);
int  mailbox_v2_get_serial_clock(hwidbtype *idb);

void chipcom_subsys_init (subsystype *subsys);

#endif __chipcom_mbox_c3000_h_
