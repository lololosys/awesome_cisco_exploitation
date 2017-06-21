/* $Id: switch_def.h,v 3.2 1995/11/17 17:42:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/switch_def.h,v $
 *------------------------------------------------------------------
 * switch_def.h
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: switch_def.h,v $
 * Revision 3.2  1995/11/17  17:42:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:15:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:17:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* this used to be part of lif.h === */

#ifndef HAVE_SWITCH_DEF_H
#define HAVE_SWITCH_DEF_H

/* Central office switch types
*/
typedef enum
{
   NO_ISDN_SWITCH = 0,
   BRI_NET3_STYPE,
   PRI_4ESS_STYPE,
   PRI_5ESS_STYPE,
   BRI_5ESS_STYPE,
   PRI_DMS100_STYPE,
   BRI_DMS100_STYPE,
   VN2_STYPE,
   VN3_STYPE,
   KDD_STYPE,
   NTT_STYPE,
   PRI_1TR6_STYPE,
   BRI_1TR6_STYPE,
   BRI_NI1_STYPE,
   BRI_TS013_STYPE,     /* Australia switchtype */
   BRI_NZNET3_STYPE,    /* New Zealand NET3 subtype */
   BRI_NWNET3_STYPE,    /* Norway NET3 subtype */
   PRI_NTT_STYPE,
   PRI_NET5_STYPE,
   PRI_TS014_STYPE,
   CCITT_STYPE,
   NISDNSWITCHES
}
SWITCH_TYPE;

#define DEFAULT_ISDN_SWITCH	NO_ISDN_SWITCH

#endif
