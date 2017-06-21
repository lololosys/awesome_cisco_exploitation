/* $Id: sr_rptrmib.h,v 3.3 1995/12/16 00:14:18 sdurham Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/les/sr_rptrmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * November 1994, Steven Garcia (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rptrmib.h,v $
 * Revision 3.3  1995/12/16  00:14:18  sdurham
 * CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
 * cli support.
 *    moved all parser commands related to mib to appropriate mib
 *    subsystem.
 *
 * Revision 3.2  1995/11/17  17:42:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:15:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/28  09:28:54  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:39:45  hampton
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

#ifndef _rptrmib_h_
#define _rptrmib_h_

/* enable set processing */
#define SETS

/* allow OID structures to be defined for the ciscoRptrIllegalSrcAddrTrap ID */
#define N_ciscoRptrIllegalSrcAddrTrap

#define N_cisco2505RptrGroup
#define N_cisco2507RptrGroup
#define N_cisco2516RptrGroup
#define N_ciscoUnknownRptrGroup
#define N_snmpDot3RptrMgt

#include "../snmp/snmp_api.h"
#include "sr_rptrmibdefs.h"
#include "sr_rptrmibsupp.h"
#include "sr_rptrmibpart.h"
#include "sr_rptrmibtype.h"

#undef N_cisco2505RptrGroup
#undef N_cisco2507RptrGroup
#undef N_cisco2516RptrGroup
#undef N_ciscoUnknownRptrGroup
#undef N_snmpDot3RptrMgt

extern const ObjectInfo rptrmib_OidList[];
extern const uint       rptrmib_OidListNum;

/*
 * rfc1516 specific trap values
 */
#define RPTRHEALTH_TRAPNUM     1
#define RPTRGROUPCHANGE        2 /* not needed, but here for completeness */
#define RPTRRESETEVENT_TRAPNUM 3
#define RPTR_ILLEGAL_ADDR_TRAPNUM 4 

/*
 * RPTR_SNMP_TEXT enumerations must match the array structure 
 * rptr_text in sr_rptrmib.c
 */
enum RPTR_SNMP_TEXT {
    HEALTHTEXT_INITIALIZING = 0,
    HEALTHTEXT_OPERATIONAL,
    GROUPDESCR_8PORT,
    GROUPDESCR_14PORT,
    GROUPDESCR_16PORT,
    MAX_RPTR_SNMP_TEXT
};


/* Individual traps for repeater */
enum { 
    TRAP_REPEATER_HEALTH,
    TRAP_REPEATER_RESET,
    TRAP_REPEATER_ILLEGAL_ADDRESS
};

typedef struct _rptrMIBNotificationEnable_t {
    boolean rptrMIBEnableHealthNotification;
    boolean rptrMIBEnableResetNotification;
}  rptrMIBNotificationEnable_t;

/*
 * Function prototypes
 */
extern void generate_rptr_trap(int);
extern void generate_illegal_addr_trap(int, int);
extern void init_rptrmib(subsystype *);
extern void hub_mib_parser_init(void);

#endif  /* _rptrmib_h_ */

      
