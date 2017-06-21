/* $Id: sr_old_latalkmib.c,v 3.4.8.2 1996/07/03 20:40:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/sr_old_latalkmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Yunsen Wang (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_latalkmib.c,v $
 * Revision 3.4.8.2  1996/07/03  20:40:36  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.4.8.1  1996/03/23  01:28:44  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.4  1996/02/29  22:39:13  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.3  1995/12/14  08:22:23  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  08:45:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:21:46  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:10:59  hampton
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

#include "master.h"
#include <interface.h>
#include "packet.h"
#include "../snmp/snmp_api.h"
#include "sr_old_latalkmib.h"
#include "sr_old_latalkmib-mib.h"
#include "atalk_private.h"

static tmpappletalk_t tmpappletalkData;

void
init_old_latalkmib (void)
{
    memset(&tmpappletalkData, 0, sizeof(tmpappletalkData));
    load_mib(old_latalkmib_OidList, old_latalkmib_OidListNum);
    load_oid(old_latalkmib_oid_table);
}








/*---------------------------------------------------------------------
 * Retrieve data from the tmpappletalk family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
tmpappletalk_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             arg;
    void           *dp;
    tmpappletalk_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_tmpappletalk_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_atInput
      case I_atInput:
	dp = &data->atInput;
	break;
#endif				       /* I_atInput */

#ifdef I_atLocal
      case I_atLocal:
	dp = &data->atLocal;
	break;
#endif				       /* I_atLocal */

#ifdef I_atBcastin
      case I_atBcastin:
	dp = &data->atBcastin;
	break;
#endif				       /* I_atBcastin */

#ifdef I_atForward
      case I_atForward:
	dp = &data->atForward;
	break;
#endif				       /* I_atForward */

#ifdef I_atBcastout
      case I_atBcastout:
	dp = &data->atBcastout;
	break;
#endif				       /* I_atBcastout */

#ifdef I_atChksum
      case I_atChksum:
	dp = &data->atChksum;
	break;
#endif				       /* I_atChksum */

#ifdef I_atNotgate
      case I_atNotgate:
	dp = &data->atNotgate;
	break;
#endif				       /* I_atNotgate */

#ifdef I_atHopcnt
      case I_atHopcnt:
	dp = &data->atHopcnt;
	break;
#endif				       /* I_atHopcnt */

#ifdef I_atNoaccess
      case I_atNoaccess:
	dp = &data->atNoaccess;
	break;
#endif				       /* I_atNoaccess */

#ifdef I_atNoroute
      case I_atNoroute:
	dp = &data->atNoroute;
	break;
#endif				       /* I_atNoroute */

#ifdef I_atNoencap
      case I_atNoencap:
	dp = &data->atNoencap;
	break;
#endif				       /* I_atNoencap */

#ifdef I_atOutput
      case I_atOutput:
	dp = &data->atOutput;
	break;
#endif				       /* I_atOutput */

#ifdef I_atInmult
      case I_atInmult:
	dp = &data->atInmult;
	break;
#endif				       /* I_atInmult */

#ifdef I_atRtmpin
      case I_atRtmpin:
	dp = &data->atRtmpin;
	break;
#endif				       /* I_atRtmpin */

#ifdef I_atRtmpout
      case I_atRtmpout:
	dp = &data->atRtmpout;
	break;
#endif				       /* I_atRtmpout */

#ifdef I_atNbpin
      case I_atNbpin:
	dp = &data->atNbpin;
	break;
#endif				       /* I_atNbpin */

#ifdef I_atNbpout
      case I_atNbpout:
	dp = &data->atNbpout;
	break;
#endif				       /* I_atNbpout */

#ifdef I_atAtp
      case I_atAtp:
	dp = &data->atAtp;
	break;
#endif				       /* I_atAtp */

#ifdef I_atZipin
      case I_atZipin:
	dp = &data->atZipin;
	break;
#endif				       /* I_atZipin */

#ifdef I_atZipout
      case I_atZipout:
	dp = &data->atZipout;
	break;
#endif				       /* I_atZipout */

#ifdef I_atEcho
      case I_atEcho:
	dp = &data->atEcho;
	break;
#endif				       /* I_atEcho */

#ifdef I_atEchoill
      case I_atEchoill:
	dp = &data->atEchoill;
	break;
#endif				       /* I_atEchoill */

#ifdef I_atDdpshort
      case I_atDdpshort:
	dp = &data->atDdpshort;
	break;
#endif				       /* I_atDdpshort */

#ifdef I_atDdplong
      case I_atDdplong:
	dp = &data->atDdplong;
	break;
#endif				       /* I_atDdplong */

#ifdef I_atDdpbad
      case I_atDdpbad:
	dp = &data->atDdpbad;
	break;
#endif				       /* I_atDdpbad */

#ifdef I_atNobuffer
      case I_atNobuffer:
	dp = &data->atNobuffer;
	break;
#endif				       /* I_atNobuffer */

#ifdef I_atArpreq
      case I_atArpreq:
	dp = &data->atArpreq;
	break;
#endif				       /* I_atArpreq */

#ifdef I_atArpreply
      case I_atArpreply:
	dp = &data->atArpreply;
	break;
#endif				       /* I_atArpreply */

#ifdef I_atArpprobe
      case I_atArpprobe:
	dp = &data->atArpprobe;
	break;
#endif				       /* I_atArpprobe */

#ifdef I_atUnknown
      case I_atUnknown:
	dp = &data->atUnknown;
	break;
#endif				       /* I_atUnknown */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}


tmpappletalk_t *
k_tmpappletalk_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    if(!atalk_running)
	return (NULL);

    tmpappletalkData.atInput = atalk_stat[ATALK_INPUT];
    tmpappletalkData.atLocal = atalk_stat[ATALK_LOCALDEST];
    tmpappletalkData.atBcastin = atalk_stat[ATALK_BCASTIN];
    tmpappletalkData.atForward = atalk_stat[ATALK_FORWARDED] \
                               + atalk_stat[ATALK_FASTFORWARD] \
                               + atalk_stat[ATALK_XFASTFORWARD];
    tmpappletalkData.atBcastout = atalk_stat[ATALK_BCASTOUT];
    tmpappletalkData.atChksum = atalk_stat[ATALK_CKSUMERR];
    tmpappletalkData.atNotgate = atalk_stat[ATALK_NOT_OPERATIONAL];
    tmpappletalkData.atHopcnt = atalk_stat[ATALK_HOPCOUNT];
    tmpappletalkData.atNoaccess = atalk_stat[ATALK_NOACCESS];
    tmpappletalkData.atNoroute = atalk_stat[ATALK_NOROUTE];
    tmpappletalkData.atNoencap = atalk_stat[ATALK_ENCAPSFAILED];
    tmpappletalkData.atOutput = atalk_stat[ATALK_OUTPUT];
    tmpappletalkData.atInmult = atalk_stat[ATALK_INMULT];
    tmpappletalkData.atRtmpin = atalk_stat[ATALK_RTMP_IN];
    tmpappletalkData.atRtmpout = atalk_stat[ATALK_RTMP_OUT];
    tmpappletalkData.atNbpin = atalk_stat[ATALK_NBP_IN];
    tmpappletalkData.atNbpout = atalk_stat[ATALK_NBP_OUT];
    tmpappletalkData.atAtp = atalk_stat[ATALK_ATP];
    tmpappletalkData.atZipin = atalk_stat[ATALK_ZIP_IN];
    tmpappletalkData.atZipout = atalk_stat[ATALK_ZIP_OUT];
    tmpappletalkData.atEcho = atalk_stat[ATALK_ECHO];
    tmpappletalkData.atEchoill = atalk_stat[ATALK_ECHO_ILL];
    tmpappletalkData.atDdpshort = atalk_stat[ATALK_DDP_SHORT];
    tmpappletalkData.atDdplong = atalk_stat[ATALK_DDP_LONG];
    tmpappletalkData.atDdpbad = atalk_stat[ATALK_DDP_WRONGSIZE];
    tmpappletalkData.atNobuffer = atalk_stat[ATALK_NO_BUFFERS];
    tmpappletalkData.atArpreq = atalk_stat[ATALK_ARP_REQUEST];
    tmpappletalkData.atArpreply = atalk_stat[ATALK_ARP_REPLY];
    tmpappletalkData.atArpprobe = atalk_stat[ATALK_ARP_PROBE];
    tmpappletalkData.atUnknown = atalk_stat[ATALK_UNKNOWN];

    return (&tmpappletalkData);
}
      
