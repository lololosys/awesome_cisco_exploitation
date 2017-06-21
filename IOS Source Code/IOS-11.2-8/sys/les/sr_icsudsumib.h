/* $Id: sr_icsudsumib.h,v 3.2 1996/03/29 03:50:15 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/sr_icsudsumib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1996, David Williams (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_icsudsumib.h,v $
 * Revision 3.2  1996/03/29  03:50:15  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:34  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
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

#ifndef _icsudsumib_h_
#define _icsudsumib_h_

/*
 * Don't know why these weren't defined in sr_icsudsumibsupp.h, they expose
 *   OIDs needeb by the Trap generating routines.
 */
#define N_ciscoICsuDsuMIBNotificationPrefix
#define N_ciscoICsuDsuT1LoopStatusNotification
#define N_ciscoICsuDsuSw56kLoopStatusNotification

#include "../snmp/snmp_api.h"
#include "sr_icsudsumibdefs.h"
#include "sr_icsudsumibsupp.h"
#include "sr_icsudsumibpart.h"
#include "sr_icsudsumibtype.h"


extern const ObjectInfo icsudsumib_OidList[];
extern const uint       icsudsumib_OidListNum;

/* BitMask of values for ciscoICsuDsuLoopStatus */
#define B_ciscoICsuDsuLoopStatus_lossOfSignal       1
#define B_ciscoICsuDsuLoopStatus_lossOfFrame        2
#define B_ciscoICsuDsuLoopStatus_detectedRAI        4
#define B_ciscoICsuDsuLoopStatus_detectedAIS        8
#define B_ciscoICsuDsuLoopStatus_placedInLoopback   16

/* BitMask of values for dsx1LineStatus */
#define B_dsx1LineStatus_NoAlarm            1
#define B_dsx1LineStatus_RcvFarEndLOF       2
#define B_dsx1LineStatus_XmtFarEndLOF       4
#define B_dsx1LineStatus_RcvAIS             8
#define B_dsx1LineStatus_XmtAIS             16
#define B_dsx1LineStatus_LossOfFrame        32
#define B_dsx1LineStatus_LossOfSignal       64
#define B_dsx1LineStatus_LoopbackState      128
#define B_dsx1LineStatus_T16AIS             256
#define B_dsx1LineStatus_RcvFarEndLOMF      512
#define B_dsx1LineStatus_XmtFarEndLOMF      1024
#define B_dsx1LineStatus_RcvTestCode        2048
#define B_dsx1LineStatus_OtherFailure       4096

/*
 * Various Maximum string lengths as defined in mib
 */
#define MAX_CSU_DSU_REVISION_LENGTH  16
#define MAX_USER_PATTERN_LENGTH      24

/*
 * snmpv1 Enterprise trap numbers
 */
#define T1_LOOP_STATUS_V1_SPEC_TRAP   1
#define SW56_LOOP_STATUS_V1_SPEC_TRAP 2

#endif  /* _icsudsumib_h_ */
      
