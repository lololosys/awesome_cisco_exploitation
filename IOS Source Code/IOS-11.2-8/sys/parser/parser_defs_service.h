/*
 *	P A R S E R _ D E F S _ S E R V I C E . H
 *
 * $Id: parser_defs_service.h,v 3.2 1995/11/17 18:52:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/parser_defs_service.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_service.h,v $
 * Revision 3.2  1995/11/17  18:52:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:20:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/29  21:21:19  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "service udp-small-servers".
 *
 * Revision 2.2  1995/07/11  23:46:02  billw
 * CSCdi35800:  Need knob to supress user view of IP addresses
 *
 * Revision 2.1  1995/06/07  22:31:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Service defines */
enum {
    SERVICE_TCPKOUT = 1,
    SERVICE_TCPKIN,
    SERVICE_TELNETZERO,
    SERVICE_PADTOTCP,
    SERVICE_TCPTOPAD,
    SERVICE_PAD,
    SERVICE_NAGLE,
    SERVICE_SLAVECORE,
    SERVICE_SLAVELOG,
    SERVICE_FINGER,
    SERVICE_DECIMAL,
    SERVICE_CONFIG,
    SERVICE_PWD_ENCRYPT,
    SERVICE_COMPRESS_CFG,
    SERVICE_EXECWAIT,
    SERVICE_TIME_STAMPS,
    SERVICE_LINE_NUMBER,
    SERVICE_INTERNAL,
    SERVICE_PROMPT,
    SERVICE_LOG_BACKTRACE,
    SERVICE_TCPSERVERS,
    SERVICE_UDPSERVERS,
    SERVICE_CALLBACK,
    SERVICE_OLDSLIP,
    SERVICE_HIDEADDR,
    SERVICE_VTYLOG
};

/* Service Timestamps flag bits */

#define SERVICE_TIMESTAMP_ENAB 0x1     /* Timestamps enabled */
#define SERVICE_TIMESTAMP_DATETIME 0x2 /* User wants date/time timestamps */
#define SERVICE_TIMESTAMP_MSEC 0x4     /* User wants milliseconds */
#define SERVICE_TIMESTAMP_LOCAL 0x8    /* User wants local time */
#define SERVICE_TIMESTAMP_SHOWTZ 0x10  /* User wants time zone displayed */

/* Service Timestamps types */

enum {
    SERVICE_TIMESTAMPS_DEBUG,
    SERVICE_TIMESTAMPS_LOG,
    NUM_SERVICE_TIMESTAMPS	/* Number of services */
};
