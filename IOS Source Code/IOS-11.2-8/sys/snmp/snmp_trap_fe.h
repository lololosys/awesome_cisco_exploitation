/* $Id: snmp_trap_fe.h,v 3.2.60.1 1996/07/31 00:10:09 ramk Exp $
 * $Source: /release/112/cvs/Xsys/snmp/snmp_trap_fe.h,v $
 *------------------------------------------------------------------
 * Prototype decleartion of SnmpTrapFeInit and a macro to convert
 * ascii string to OID (example in ibm/sr_snasdlcmib.c
 *   
 *
 * Jan 1995, Barry Chan(Metaplex)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_trap_fe.h,v $
 * Revision 3.2.60.1  1996/07/31  00:10:09  ramk
 * CSCdi57452:  SNMP Trap overflow on boot-up
 * Branch: California_branch
 * Trap queue implemented using buffers allocated from system buffer pool.
 * Buginfs changed to errmsgs.
 *
 * Revision 3.2  1995/11/17  18:58:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/17  04:24:38  bchan
 * CSCdi36821:  SNMP trap process started without SNMP being configured
 *
 * Revision 2.2  1995/07/28  03:52:15  jjohnson
 * CSCdi36656:  snmp fails to generate a trap when line protocol changes
 * When an attempt is made to generate a linkUp or linkDown trap at
 * interrupt level, queue the request up to process level rather than
 * completely ignore it.
 *
 * Revision 2.1  1995/06/07  22:48:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void init_snmp_trap(void);
void SnmpTrapFeProcess(boolean);

void SnmpSendTrap_Generic(const OID* vblist,
                     const OID*   enterprise,
                     const char*  v2TrapOID,
                     const OID*   instanceOID,
                     unsigned short varbindCount,
                     int          genericTrapNum,
                     int          specificTrapNum);

/* This macro converts a string into OID, see example of sr_snasdlcmib.c */

#define OIDconversion(a) {LN ## a, (ulong *)ID ## a}
