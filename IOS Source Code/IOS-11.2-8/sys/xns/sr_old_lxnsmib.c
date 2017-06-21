/* $Id: sr_old_lxnsmib.c,v 3.5.10.1 1996/07/03 20:45:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/sr_old_lxnsmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_lxnsmib.c,v $
 * Revision 3.5.10.1  1996/07/03  20:45:13  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.5  1996/02/21  21:45:21  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  15:58:52  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:30:32  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  19:25:12  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:34:16  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:30:23  hampton
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
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_old_lxnsmib.h"
#include "sr_old_lxnsmib-mib.h"

#include "packet.h"
#include "../xns/xns.h"

void
init_old_lxnsmib (subsystype *subsys)
{
    load_mib(old_lxnsmib_OidList, old_lxnsmib_OidListNum);
    load_oid(old_lxnsmib_oid_table);
}


/*---------------------------------------------------------------------
 * Retrieve data from the tmpxns family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
tmpxns_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             arg;
    void           *dp;
    tmpxns_t       *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_tmpxns_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_xnsInput
      case I_xnsInput:
	dp = &data->xnsInput;
	break;
#endif				       /* I_xnsInput */

#ifdef I_xnsLocal
      case I_xnsLocal:
	dp = &data->xnsLocal;
	break;
#endif				       /* I_xnsLocal */

#ifdef I_xnsBcastin
      case I_xnsBcastin:
	dp = &data->xnsBcastin;
	break;
#endif				       /* I_xnsBcastin */

#ifdef I_xnsForward
      case I_xnsForward:
	dp = &data->xnsForward;
	break;
#endif				       /* I_xnsForward */

#ifdef I_xnsBcastout
      case I_xnsBcastout:
	dp = &data->xnsBcastout;
	break;
#endif				       /* I_xnsBcastout */

#ifdef I_xnsErrin
      case I_xnsErrin:
	dp = &data->xnsErrin;
	break;
#endif				       /* I_xnsErrin */

#ifdef I_xnsErrout
      case I_xnsErrout:
	dp = &data->xnsErrout;
	break;
#endif				       /* I_xnsErrout */

#ifdef I_xnsFormerr
      case I_xnsFormerr:
	dp = &data->xnsFormerr;
	break;
#endif				       /* I_xnsFormerr */

#ifdef I_xnsChksum
      case I_xnsChksum:
	dp = &data->xnsChksum;
	break;
#endif				       /* I_xnsChksum */

#ifdef I_xnsNotgate
      case I_xnsNotgate:
	dp = &data->xnsNotgate;
	break;
#endif				       /* I_xnsNotgate */

#ifdef I_xnsHopcnt
      case I_xnsHopcnt:
	dp = &data->xnsHopcnt;
	break;
#endif				       /* I_xnsHopcnt */

#ifdef I_xnsNoroute
      case I_xnsNoroute:
	dp = &data->xnsNoroute;
	break;
#endif				       /* I_xnsNoroute */

#ifdef I_xnsNoencap
      case I_xnsNoencap:
	dp = &data->xnsNoencap;
	break;
#endif				       /* I_xnsNoencap */

#ifdef I_xnsOutput
      case I_xnsOutput:
	dp = &data->xnsOutput;
	break;
#endif				       /* I_xnsOutput */

#ifdef I_xnsInmult
      case I_xnsInmult:
	dp = &data->xnsInmult;
	break;
#endif				       /* I_xnsInmult */

#ifdef I_xnsUnknown
      case I_xnsUnknown:
	dp = &data->xnsUnknown;
	break;
#endif				       /* I_xnsUnknown */

#ifdef I_xnsFwdbrd
      case I_xnsFwdbrd:
	dp = &data->xnsFwdbrd;
	break;
#endif				       /* I_xnsFwdbrd */

#ifdef I_xnsEchoreqin
      case I_xnsEchoreqin:
	dp = &data->xnsEchoreqin;
	break;
#endif				       /* I_xnsEchoreqin */

#ifdef I_xnsEchoreqout
      case I_xnsEchoreqout:
	dp = &data->xnsEchoreqout;
	break;
#endif				       /* I_xnsEchoreqout */

#ifdef I_xnsEchorepin
      case I_xnsEchorepin:
	dp = &data->xnsEchorepin;
	break;
#endif				       /* I_xnsEchorepin */

#ifdef I_xnsEchorepout
      case I_xnsEchorepout:
	dp = &data->xnsEchorepout;
	break;
#endif				       /* I_xnsEchorepout */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}



tmpxns_t       *
k_tmpxns_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static tmpxns_t tmpxnsData;

    tmpxnsData.xnsInput = xns_stat[XNS_INPUT];
    tmpxnsData.xnsLocal = xns_stat[XNS_LOCALDEST];
    tmpxnsData.xnsBcastin = xns_stat[XNS_BCASTIN];
    tmpxnsData.xnsForward = xns_stat[XNS_FORWARDED];
    tmpxnsData.xnsBcastout = xns_stat[XNS_BCASTOUT];
    tmpxnsData.xnsErrin = xns_stat[XNS_ERRIN];
    tmpxnsData.xnsErrout = xns_stat[XNS_ERROUT];
    tmpxnsData.xnsFormerr = xns_stat[XNS_FORMATERROR];
    tmpxnsData.xnsChksum = xns_stat[XNS_CKSUMERR];
    tmpxnsData.xnsNotgate = xns_stat[XNS_NOTGATEWAY];
    tmpxnsData.xnsHopcnt = xns_stat[XNS_HOPCOUNT];
    tmpxnsData.xnsNoroute = xns_stat[XNS_NOROUTE];
    tmpxnsData.xnsNoencap = xns_stat[XNS_ENCAPSFAILED];
    tmpxnsData.xnsOutput = xns_stat[XNS_OUTPUT];
    tmpxnsData.xnsInmult = xns_stat[XNS_INMULT];
    tmpxnsData.xnsUnknown = xns_stat[XNS_UNKNOWN];
    tmpxnsData.xnsFwdbrd = xns_stat[XNS_FWDBRD];
    tmpxnsData.xnsEchoreqin = xns_stat[XNS_ECHOREQIN];
    tmpxnsData.xnsEchoreqout = xns_stat[XNS_ECHOREQOUT];
    tmpxnsData.xnsEchorepin = xns_stat[XNS_ECHOREPIN];
    tmpxnsData.xnsEchorepout = xns_stat[XNS_ECHOREPOUT];

    return (&tmpxnsData);
}

/*
 * old_lxnsmib subsystem header
 */

#define MAJVERSION_old_lxnsmib 1
#define MINVERSION_old_lxnsmib 0
#define EDITVERSION_old_lxnsmib 0

SUBSYS_HEADER(xnsmib,
              MAJVERSION_old_lxnsmib,
              MINVERSION_old_lxnsmib,
              EDITVERSION_old_lxnsmib,
              init_old_lxnsmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: xns",
              NULL);
      
