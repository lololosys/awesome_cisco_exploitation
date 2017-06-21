/* $Id: dci_diag_defs.h,v 3.1 1995/11/09 09:05:14 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/dci_diag_defs.h,v $
 *------------------------------------------------------------------
 * dci_diag_defs.h
 *
 * Copyright (c) 1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dci_diag_defs.h,v $
 * Revision 3.1  1995/11/09  09:05:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:36  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Amount of time to wait in ms for an DCI to reset:
 */

#define HARD_RESET_TIME 250
#define RESET_TIME      65
#define BUFLEN          30
#define MAXCTRLLERS     16
#define MAXPACKET       8160
#define DATA_LOCATION   0x80000
#define MEMD_BASE       0x100
#define MEMD_STEP       1       /* 1 == look at every word of memory */
#define AF_STEP         0x1
#define PKTLENGTH       60
#define MECBYTES        32
#define PKTWAIT         150

#define POOL0                   0
#define POOL7                   7

#define HARD_RESET              0
#define SOFT_RESET              1
#define FCI_ASSIGN_POOLINDEX    0
#define FCI_MAX_BUFFERSIZE      1
#define FCI_RX_BUFFER_OFFSET    2
#define FCI_RX_BUFFER_LIMIT     3
#define FCI_MAXUNITS 24
#define MCI_TYPE_FDDI 2                 /* 100mb fddi  */

#define FCI_MEMD_SELECT     8
#define FCI_MEMD_WRITE      9
#define FCI_MEMD_READ       10
#define FCI_CTRLR_CMD                   0x8002
#define FCI_CTRLR_ARGUMENT              0x8003

