/* $Id: atmsig_util.c,v 3.1.4.2 1996/07/10 22:20:38 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_util.c,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * March 1996, Andrew Wu 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_util.c,v $
 * Revision 3.1.4.2  1996/07/10  22:20:38  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:02:33  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/02  07:37:42  awu
 * Placeholder file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "address.h"
#include "../if/atm.h"
#include "atmsig_api.h"
#include "atmsig_private.h"
#include "../snmp/snmp_api.h"

static atm_qos_t
atmSig_qos_to_common (ushort uni_version, ushort sig_qos)
{
  /*
   * for uni3.0 and uni3.1, there is no conversion needed.
   */
  switch (uni_version) {
  case ATMSIG_VERSION_30:
  case ATMSIG_VERSION_31: 
  default:
    return(sig_qos);
    break;
  }
}

		     
/*
 * atmSig_find_trParamQos:
 * called to find out traffic parameters and QoS for a given vc on
 * the specified direction.
 *
 * INPUT:
 * vc_info:	ptr to the interested vc.
 * trParamQos:	ptr to the traffic param and qos info that will be
 *		returned.
 * tx_direction:	a boolean. true means the caller wants to
 *			the traffic param and qos for the tx
 *			direction. otherwise means for rx direction.
 */
boolean atmSig_find_trParamQos (vc_info_t *vc_info,
				atm_trParamQos_t *trParamQos,
				boolean tx_direction)
{
    atmSvcType *svc;
    idbtype	*swidb;
    hwidbtype	*hwidb;
    atm_db_t	*atm_db;

    if (vc_info == NULL)
      return(FALSE);
    swidb = vc_info->swidb;
    if (swidb == NULL)
      return(FALSE);
    hwidb = swidb->hwptr;
    if (hwidb == NULL)
      return(FALSE);
    atm_db = hwidb->atm_db;
    if (atm_db == NULL)
      return(FALSE);
    if (!IS_ATMVC_SVC(vc_info))
      return(FALSE);

    svc = atmSig_findSvcBlockByVC(vc_info->swidb, vc_info->vc);
    if (svc == NULL) {
	return(FALSE);
    }

    if (tx_direction) {			/* for the tx direction */
	if (svc->locallyInitiatedCall) {
	  trParamQos->qos =
	    atmSig_qos_to_common(atm_db->uni_version, svc->tos.txQosClassFwd);
	  trParamQos->qos = svc->tos.txQosClassFwd;
	  trParamQos->usr_cellrate.PeakClp0 =
	    svc->tos.userCellRate.fwdPeakClp0;
	  trParamQos->usr_cellrate.PeakClp01 =
	    svc->tos.userCellRate.fwdPeakClp01;
	  trParamQos->usr_cellrate.SusClp0 =
	    svc->tos.userCellRate.fwdSusClp0;
	  trParamQos->usr_cellrate.SusClp01 =
	    svc->tos.userCellRate.fwdSusClp01;
	  trParamQos->usr_cellrate.MaxBurstClp0 =
	    svc->tos.userCellRate.fwdMaxBurstClp0;
	  trParamQos->usr_cellrate.MaxBurstClp01 =
	    svc->tos.userCellRate.fwdMaxBurstClp01;
	  trParamQos->usr_cellrate.Tag =
	    svc->tos.userCellRate.FwdTag;
	} else {
	    trParamQos->qos =
	      atmSig_qos_to_common(atm_db->uni_version, svc->tos.rcvQosClassBackwd);
	    trParamQos->usr_cellrate.PeakClp0 =
	      svc->tos.userCellRate.bwdPeakClp0;
	    trParamQos->usr_cellrate.PeakClp01 =
	      svc->tos.userCellRate.bwdPeakClp01;
	    trParamQos->usr_cellrate.SusClp0 =
	      svc->tos.userCellRate.bwdSusClp0;
	    trParamQos->usr_cellrate.SusClp01 =
	      svc->tos.userCellRate.bwdSusClp01;
	    trParamQos->usr_cellrate.MaxBurstClp0 =
	      svc->tos.userCellRate.bwdMaxBurstClp0;
	    trParamQos->usr_cellrate.MaxBurstClp01 =
	      svc->tos.userCellRate.bwdMaxBurstClp01;
	    trParamQos->usr_cellrate.Tag =
	      svc->tos.userCellRate.BwdTag;
	}
    } else {				/* rx direction */
      	if (svc->locallyInitiatedCall) { /* make use of bwd */
	  trParamQos->qos =
	    atmSig_qos_to_common(atm_db->uni_version,
				 svc->tos.txQosClassBackwd);
	  trParamQos->usr_cellrate.PeakClp0 =
	    svc->tos.userCellRate.bwdPeakClp0;
	  trParamQos->usr_cellrate.PeakClp01 =
	    svc->tos.userCellRate.bwdPeakClp01;
	  trParamQos->usr_cellrate.SusClp0 =
	    svc->tos.userCellRate.bwdSusClp0;
	  trParamQos->usr_cellrate.SusClp01 =
	    svc->tos.userCellRate.bwdSusClp01;
	  trParamQos->usr_cellrate.MaxBurstClp0 =
	    svc->tos.userCellRate.bwdMaxBurstClp0;
	  trParamQos->usr_cellrate.MaxBurstClp01 =
	    svc->tos.userCellRate.bwdMaxBurstClp01;
	  trParamQos->usr_cellrate.Tag =
	    svc->tos.userCellRate.BwdTag;
	} else {			/* make use of fwd */
	  trParamQos->qos =
	    atmSig_qos_to_common(atm_db->uni_version, svc->tos.rcvQosClassFwd);
	    trParamQos->usr_cellrate.PeakClp0 =
	    svc->tos.userCellRate.fwdPeakClp0;
	  trParamQos->usr_cellrate.PeakClp01 =
	    svc->tos.userCellRate.fwdPeakClp01;
	  trParamQos->usr_cellrate.SusClp0 =
	    svc->tos.userCellRate.fwdSusClp0;
	  trParamQos->usr_cellrate.SusClp01 =
	    svc->tos.userCellRate.fwdSusClp01;
	  trParamQos->usr_cellrate.MaxBurstClp0 =
	    svc->tos.userCellRate.fwdMaxBurstClp0;
	  trParamQos->usr_cellrate.MaxBurstClp01 =
	    svc->tos.userCellRate.fwdMaxBurstClp01;
	  trParamQos->usr_cellrate.Tag =
	    svc->tos.userCellRate.FwdTag;
	} 
    }
    return(TRUE);
}





      
