/* $Id: ilmi_encode.h,v 3.2 1995/11/17 08:46:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm/ilmi_encode.h,v $
 *------------------------------------------------------------------
 * Placeholder for ILMI support
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_encode.h,v $
 * Revision 3.2  1995/11/17  08:46:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:12:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define FIRSTBYTE_HIBIT  	0x80
#define SECONDBYTE_HIBIT   0x8000
#define THIRDBYTE_HIBIT   0x800000
#define FOURTHBYTE_HIBIT   0x80000000

#define ILMI_OBJID_TAG 0x6
#define ILMI_SEQOF_TAG 0x30
#define ILMI_INT_TAG 0x2

#define ZEROINT_LEN 3
#define COMMSTR_LEN 6
#define NULL_VALUEBUF_LEN 2
#define ILMITRAP_AGENT_NAMEBUFLEN 6
#define TRAPNUM_BUFLEN 3
#define ATMFORUM_NAMEBUFLEN 9
#define VCCCHANGE_NAMEBUFLEN 14

#define ILMI_PORTSPECIFIC_LEN  2

