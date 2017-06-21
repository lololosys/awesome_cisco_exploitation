/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_type.h,v 3.2 1995/11/17 19:01:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_type.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_type.h,v $
 * Revision 3.2  1995/11/17  19:01:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:51:17  hampton
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

/* sr_type.h,v 1.2 1995/02/08 19:38:44 battle Exp */

/*
 * sr_type.h,v
 * Revision 1.2  1995/02/08  19:38:44  battle
 * added copyright, Id, and Log
 *
 */

#ifndef SR_TYPE_H
#define SR_TYPE_H

#ifdef SIXTY_FOUR_BIT_LONGS
typedef int          SR_INT32;
typedef unsigned int SR_UINT32;
#else /* SIXTY_FOUR_BIT_LONGS */
typedef long          SR_INT32;
typedef unsigned long SR_UINT32;
#endif /* SIXTY_FOUR_BIT_LONGS */

#endif /* SR_TYPE_H */

