/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_diag.h,v 3.2 1995/11/17 18:59:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_diag.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_diag.h,v $
 * Revision 3.2  1995/11/17  18:59:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:48:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */
/*
 * Revision History:
 *
 * 14-Aug-1992 DLB
 *
 *   Removed NOTRACE, added APALL and APALWAYS, see comments below.
 *
 * 21-Jul-1993 DLB
 *   Added prototypes.
 *
 */

#ifndef _diag_h
#define _diag_h

#include "sr_proto.h"

#ifdef DEBUG
extern int      LogLevel;
extern int SetLogLevel SR_PROTOTYPE((int));
extern int GetLogLevel SR_PROTOTYPE(());
#endif /* DEBUG */

#ifndef DEBUG

#define SR_FNAME(name)
#define DPRINTF( X )
#define	DMEMDUMP(flags, src, len)

#else				/* DEBUG */
#define SR_FNAME(name)    static char FName[] = name;
#define DPRINTF( X )   dprintf X; dlfprint(__LINE__, __FILE__)

#endif				/* DEBUG */

#define APWARN  0x1000
#define APERROR 0x2000


#define APTRACE		0x0080	/* Trace packet through application */


#define APALL		0x7FFF	/* OR of all the above DO NOT PASS TO DPRINTF */

#define APALWAYS	0x0000	/* always print this message PASS TO DPRINTF */


extern  int dprintf SR_PROTOTYPE((int va_alist, const char *format, ...));
extern  void dlfprint SR_PROTOTYPE((int line,char *file));

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* ! _diag_h */
