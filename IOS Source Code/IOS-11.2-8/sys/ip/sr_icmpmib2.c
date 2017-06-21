/* $Id: sr_icmpmib2.c,v 3.5.10.2 1996/07/06 05:54:02 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/sr_icmpmib2.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_icmpmib2.c,v $
 * Revision 3.5.10.2  1996/07/06  05:54:02  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.5.10.1  1996/06/18  01:46:07  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/22  21:55:10  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  15:48:30  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:25:03  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  09:36:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:26:53  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:00:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1993 by SNMP Research, Incorporated.
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

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_icmpmib2.h"
#include "ip.h"
#include "sr_icmpmib2-mib.h"
#include "packet.h"

void
init_icmpmib2 (subsystype *subsys)
{
    load_mib(icmpmib2_OidList, icmpmib2_OidListNum);
    load_oid(icmpmib2_oid_table);
}








/*---------------------------------------------------------------------
 * Retrieve data from the icmp family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
icmp_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    icmp_t          *data = NULL;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and the single
     * instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    switch (searchType) {
      case EXACT:
        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
          arg = object->nominator;
        }
        break;

      case NEXT:
        if (instLength <= 0) {
          arg = object->nominator;
        }
        break;

    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_icmp_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_icmpInMsgs
    case I_icmpInMsgs:
#endif /* I_icmpInMsgs */

#ifdef I_icmpInErrors
    case I_icmpInErrors:
#endif /* I_icmpInErrors */

#ifdef I_icmpInDestUnreachs
    case I_icmpInDestUnreachs:
#endif /* I_icmpInDestUnreachs */

#ifdef I_icmpInTimeExcds
    case I_icmpInTimeExcds:
#endif /* I_icmpInTimeExcds */

#ifdef I_icmpInParmProbs
    case I_icmpInParmProbs:
#endif /* I_icmpInParmProbs */

#ifdef I_icmpInSrcQuenchs
    case I_icmpInSrcQuenchs:
#endif /* I_icmpInSrcQuenchs */

#ifdef I_icmpInRedirects
    case I_icmpInRedirects:
#endif /* I_icmpInRedirects */

#ifdef I_icmpInEchos
    case I_icmpInEchos:
#endif /* I_icmpInEchos */

#ifdef I_icmpInEchoReps
    case I_icmpInEchoReps:
#endif /* I_icmpInEchoReps */

#ifdef I_icmpInTimestamps
    case I_icmpInTimestamps:
#endif /* I_icmpInTimestamps */

#ifdef I_icmpInTimestampReps
    case I_icmpInTimestampReps:
#endif /* I_icmpInTimestampReps */

#ifdef I_icmpInAddrMasks
    case I_icmpInAddrMasks:
#endif /* I_icmpInAddrMasks */

#ifdef I_icmpInAddrMaskReps
    case I_icmpInAddrMaskReps:
#endif /* I_icmpInAddrMaskReps */

#ifdef I_icmpOutMsgs
    case I_icmpOutMsgs:
#endif /* I_icmpOutMsgs */

#ifdef I_icmpOutErrors
    case I_icmpOutErrors:
#endif /* I_icmpOutErrors */

#ifdef I_icmpOutDestUnreachs
    case I_icmpOutDestUnreachs:
#endif /* I_icmpOutDestUnreachs */

#ifdef I_icmpOutTimeExcds
    case I_icmpOutTimeExcds:
#endif /* I_icmpOutTimeExcds */

#ifdef I_icmpOutParmProbs
    case I_icmpOutParmProbs:
#endif /* I_icmpOutParmProbs */

#ifdef I_icmpOutSrcQuenchs
    case I_icmpOutSrcQuenchs:
#endif /* I_icmpOutSrcQuenchs */

#ifdef I_icmpOutRedirects
    case I_icmpOutRedirects:
#endif /* I_icmpOutRedirects */

#ifdef I_icmpOutEchos
    case I_icmpOutEchos:
#endif /* I_icmpOutEchos */

#ifdef I_icmpOutEchoReps
    case I_icmpOutEchoReps:
#endif /* I_icmpOutEchoReps */

#ifdef I_icmpOutTimestamps
    case I_icmpOutTimestamps:
#endif /* I_icmpOutTimestamps */

#ifdef I_icmpOutTimestampReps
    case I_icmpOutTimestampReps:
#endif /* I_icmpOutTimestampReps */

#ifdef I_icmpOutAddrMasks
    case I_icmpOutAddrMasks:
#endif /* I_icmpOutAddrMasks */

#ifdef I_icmpOutAddrMaskReps
    case I_icmpOutAddrMaskReps:
#endif /* I_icmpOutAddrMaskReps */

	 return (MakeVarBind(object, &ZeroOid, SR_OFFSET(data, arg)));

    default:
       return ((VarBind *) NULL);

     }      /* switch */

}





icmp_t *
k_icmp_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static icmp_t icmpData;

   icmpData.icmpInMsgs = icmp_traffic.inputs;
   icmpData.icmpInErrors = icmp_traffic.checksumerr;
   icmpData.icmpInDestUnreachs = icmp_traffic.unreachrcv;
   icmpData.icmpInTimeExcds = icmp_traffic.ttlrcv;
   icmpData.icmpInParmProbs = icmp_traffic.paramrcv;
   icmpData.icmpInSrcQuenchs = icmp_traffic.quenchrcv;
   icmpData.icmpInRedirects = icmp_traffic.redirectrcv;
   icmpData.icmpInEchos = icmp_traffic.echorcv;
   icmpData.icmpInEchoReps = icmp_traffic.echoreprcv;
   icmpData.icmpInTimestamps = icmp_traffic.timestrcv;
   icmpData.icmpInTimestampReps = icmp_traffic.timestreprcv;
   icmpData.icmpInAddrMasks = icmp_traffic.maskreqrcv;
   icmpData.icmpInAddrMaskReps = icmp_traffic.maskreprcv;
   icmpData.icmpOutMsgs = icmp_traffic.outputs;
   icmpData.icmpOutErrors = 0;
   icmpData.icmpOutDestUnreachs = icmp_traffic.unreachsent;
   icmpData.icmpOutTimeExcds = icmp_traffic.ttlsent;
   icmpData.icmpOutParmProbs = icmp_traffic.paramsent;
   icmpData.icmpOutSrcQuenchs = icmp_traffic.quenchsent;
   icmpData.icmpOutRedirects = icmp_traffic.redirectsent;
   icmpData.icmpOutEchos = icmp_traffic.echosent;
   icmpData.icmpOutEchoReps = icmp_traffic.echorepsent;
   icmpData.icmpOutTimestamps = icmp_traffic.timestsent;
   icmpData.icmpOutTimestampReps = icmp_traffic.timestrepsent;
   icmpData.icmpOutAddrMasks = icmp_traffic.maskreqsent;
   icmpData.icmpOutAddrMaskReps = icmp_traffic.maskrepsent;

   return(&icmpData);
}


/*
 * icmpmib2 subsystem header
 */

#define MAJVERSION_icmpmib2 1
#define MINVERSION_icmpmib2 0
#define EDITVERSION_icmpmib2 0

SUBSYS_HEADER(icmpmib2,
              MAJVERSION_icmpmib2,
              MINVERSION_icmpmib2,
              EDITVERSION_icmpmib2,
              init_icmpmib2,
              SUBSYS_CLASS_MANAGEMENT,
              subsys_req_iphost,
              NULL);
      
