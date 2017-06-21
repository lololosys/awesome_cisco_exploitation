/* $Id: sr_old_lenvmib.c,v 3.5.6.2 1996/07/03 20:41:41 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_old_lenvmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_lenvmib.c,v $
 * Revision 3.5.6.2  1996/07/03  20:41:41  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.5.6.1  1996/03/18  19:43:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/16  06:51:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.1  1996/03/07  09:40:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/03/01  14:42:43  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  15:45:45  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:24:02  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  09:20:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/28  21:16:53  sdurham
 * CSCdi36261:  CISCO-ENVMON-MIB and OLD-CISCO-ENV-MIB being loaded on ags
 *              improperly.
 *
 * Revision 2.2  1995/06/28  09:24:57  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:44:19  hampton
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
#include <ciscolib.h>
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_old_lenvmib.h"
#include "sr_old_lenvmib-mib.h"

#include "ec.h"
#include "../hes/env_api.h"


void
init_old_lenvmib (subsystype *subsys)
{
    if (envmib_load == TRUE) {
	load_mib(old_lenvmib_OidList, old_lenvmib_OidListNum);
	load_oid(old_lenvmib_oid_table);
    }
}


/*---------------------------------------------------------------------
 * Retrieve data from the lenv family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
lenv_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             arg;
    void           *dp;
    lenv_t         *data=NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_lenv_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_envPresent
      case I_envPresent:
	dp = &data->envPresent;
	break;
#endif				       /* I_envPresent */

#ifdef I_envTestPt1Descr
      case I_envTestPt1Descr:
	dp = MakeOctetString(data->envTestPt1Descr->octet_ptr, data->envTestPt1Descr->length);
	break;
#endif				       /* I_envTestPt1Descr */

#ifdef I_envTestPt1Measure
      case I_envTestPt1Measure:
	dp = &data->envTestPt1Measure;
	break;
#endif				       /* I_envTestPt1Measure */

#ifdef I_envTestPt2Descr
      case I_envTestPt2Descr:
	dp = MakeOctetString(data->envTestPt2Descr->octet_ptr, data->envTestPt2Descr->length);
	break;
#endif				       /* I_envTestPt2Descr */

#ifdef I_envTestPt2Measure
      case I_envTestPt2Measure:
	dp = &data->envTestPt2Measure;
	break;
#endif				       /* I_envTestPt2Measure */

#ifdef I_envTestPt3Descr
      case I_envTestPt3Descr:
	dp = MakeOctetString(data->envTestPt3Descr->octet_ptr, data->envTestPt3Descr->length);
	break;
#endif				       /* I_envTestPt3Descr */

#ifdef I_envTestPt3Measure
      case I_envTestPt3Measure:
	dp = &data->envTestPt3Measure;
	break;
#endif				       /* I_envTestPt3Measure */

#ifdef I_envTestPt4Descr
      case I_envTestPt4Descr:
	dp = MakeOctetString(data->envTestPt4Descr->octet_ptr, data->envTestPt4Descr->length);
	break;
#endif				       /* I_envTestPt4Descr */

#ifdef I_envTestPt4Measure
      case I_envTestPt4Measure:
	dp = &data->envTestPt4Measure;
	break;
#endif				       /* I_envTestPt4Measure */

#ifdef I_envTestPt5Descr
      case I_envTestPt5Descr:
	dp = MakeOctetString(data->envTestPt5Descr->octet_ptr, data->envTestPt5Descr->length);
	break;
#endif				       /* I_envTestPt5Descr */

#ifdef I_envTestPt5Measure
      case I_envTestPt5Measure:
	dp = &data->envTestPt5Measure;
	break;
#endif				       /* I_envTestPt5Measure */

#ifdef I_envTestPt6Descr
      case I_envTestPt6Descr:
	dp = MakeOctetString(data->envTestPt6Descr->octet_ptr, data->envTestPt6Descr->length);
	break;
#endif				       /* I_envTestPt6Descr */

#ifdef I_envTestPt6Measure
      case I_envTestPt6Measure:
	dp = &data->envTestPt6Measure;
	break;
#endif				       /* I_envTestPt6Measure */

#ifdef I_envTestPt1MarginVal
      case I_envTestPt1MarginVal:
	dp = &data->envTestPt1MarginVal;
	break;
#endif				       /* I_envTestPt1MarginVal */

#ifdef I_envTestPt2MarginVal
      case I_envTestPt2MarginVal:
	dp = &data->envTestPt2MarginVal;
	break;
#endif				       /* I_envTestPt2MarginVal */

#ifdef I_envTestPt3MarginPercent
      case I_envTestPt3MarginPercent:
	dp = &data->envTestPt3MarginPercent;
	break;
#endif				       /* I_envTestPt3MarginPercent */

#ifdef I_envTestPt4MarginPercent
      case I_envTestPt4MarginPercent:
	dp = &data->envTestPt4MarginPercent;
	break;
#endif				       /* I_envTestPt4MarginPercent */

#ifdef I_envTestPt5MarginPercent
      case I_envTestPt5MarginPercent:
	dp = &data->envTestPt5MarginPercent;
	break;
#endif				       /* I_envTestPt5MarginPercent */

#ifdef I_envTestPt6MarginPercent
      case I_envTestPt6MarginPercent:
	dp = &data->envTestPt6MarginPercent;
	break;
#endif				       /* I_envTestPt6MarginPercent */

#ifdef I_envTestPt1last
      case I_envTestPt1last:
	dp = &data->envTestPt1last;
	break;
#endif				       /* I_envTestPt1last */

#ifdef I_envTestPt2last
      case I_envTestPt2last:
	dp = &data->envTestPt2last;
	break;
#endif				       /* I_envTestPt2last */

#ifdef I_envTestPt3last
      case I_envTestPt3last:
	dp = &data->envTestPt3last;
	break;
#endif				       /* I_envTestPt3last */

#ifdef I_envTestPt4last
      case I_envTestPt4last:
	dp = &data->envTestPt4last;
	break;
#endif				       /* I_envTestPt4last */

#ifdef I_envTestPt5last
      case I_envTestPt5last:
	dp = &data->envTestPt5last;
	break;
#endif				       /* I_envTestPt5last */

#ifdef I_envTestPt6last
      case I_envTestPt6last:
	dp = &data->envTestPt6last;
	break;
#endif				       /* I_envTestPt6last */

#ifdef I_envTestPt1warn
      case I_envTestPt1warn:
	dp = &data->envTestPt1warn;
	break;
#endif				       /* I_envTestPt1warn */

#ifdef I_envTestPt2warn
      case I_envTestPt2warn:
	dp = &data->envTestPt2warn;
	break;
#endif				       /* I_envTestPt2warn */

#ifdef I_envTestPt3warn
      case I_envTestPt3warn:
	dp = &data->envTestPt3warn;
	break;
#endif				       /* I_envTestPt3warn */

#ifdef I_envTestPt4warn
      case I_envTestPt4warn:
	dp = &data->envTestPt4warn;
	break;
#endif				       /* I_envTestPt4warn */

#ifdef I_envTestPt5warn
      case I_envTestPt5warn:
	dp = &data->envTestPt5warn;
	break;
#endif				       /* I_envTestPt5warn */

#ifdef I_envTestPt6warn
      case I_envTestPt6warn:
	dp = &data->envTestPt6warn;
	break;
#endif				       /* I_envTestPt6warn */

#ifdef I_envFirmVersion
      case I_envFirmVersion:
	dp = MakeOctetString(data->envFirmVersion->octet_ptr, data->envFirmVersion->length);
	break;
#endif				       /* I_envFirmVersion */

#ifdef I_envTechnicianID
      case I_envTechnicianID:
	dp = MakeOctetString(data->envTechnicianID->octet_ptr, data->envTechnicianID->length);
	break;
#endif				       /* I_envTechnicianID */

#ifdef I_envType
      case I_envType:
	dp = MakeOctetString(data->envType->octet_ptr, data->envType->length);
	break;
#endif				       /* I_envType */

#ifdef I_envBurnDate
      case I_envBurnDate:
	dp = MakeOctetString(data->envBurnDate->octet_ptr, data->envBurnDate->length);
	break;
#endif				       /* I_envBurnDate */

#ifdef I_envSerialNumber
      case I_envSerialNumber:
	dp = MakeOctetString(data->envSerialNumber->octet_ptr, data->envSerialNumber->length);
	break;
#endif				       /* I_envSerialNumber */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}


lenv_t         *
k_lenv_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static lenv_t   lenvData;
    int level;
    static OctetString envFirmVersion, envTechnicianID, envType,
	envBurnDate, envSerialNumber, envTestPt1Descr, envTestPt2Descr,
	envTestPt3Descr, envTestPt4Descr, envTestPt5Descr,
	envTestPt6Descr;
    static ecshow_t snmp_env_buf;
    ecshow_t *show = &snmp_env_buf;

    if (!ecp || !ecp->ec_show)
	return(NULL);

    level = raise_interrupt_level(ALL_DISABLE);
    bcopy(ecp->ec_show, show, sizeof(ecshow_t));
    reset_interrupt_level(level);


    if (ecp->ec_state < EC_STATE_IDLE)
	lenvData.envPresent = ECM_PRES_NODATA;
    else
	lenvData.envPresent = ECM_PRES_AND_DATA;

    lenvData.envTestPt1Descr = &envTestPt1Descr;
    lenvData.envTestPt1Descr->octet_ptr = 
		show->s_test[ECTEST_T_AMB].t_description;
    lenvData.envTestPt1Descr->length = 
		strlen(show->s_test[ECTEST_T_AMB].t_description);
    lenvData.envTestPt1Measure = 
		getenvtemp(show->s_test[ECTEST_T_AMB].t_measured);

    lenvData.envTestPt2Descr = &envTestPt2Descr;
    lenvData.envTestPt2Descr->octet_ptr = 
		show->s_test[ECTEST_T_AIR].t_description;
    lenvData.envTestPt2Descr->length = 
		strlen(show->s_test[ECTEST_T_AIR].t_description);
    lenvData.envTestPt2Measure = 
		getenvtemp(show->s_test[ECTEST_T_AIR].t_measured);

    lenvData.envTestPt3Descr = &envTestPt3Descr;
    lenvData.envTestPt3Descr->octet_ptr = 
		show->s_test[ECTEST_V_P5].t_description;
    lenvData.envTestPt3Descr->length = 
		strlen(show->s_test[ECTEST_V_P5].t_description);
    lenvData.envTestPt3Measure = 
		getenvvoltage(show->s_test[ECTEST_V_P5].t_measured);

    lenvData.envTestPt4Descr = &envTestPt4Descr;
    lenvData.envTestPt4Descr->octet_ptr = 
		show->s_test[ECTEST_V_P12].t_description;
    lenvData.envTestPt4Descr->length = 
		strlen(show->s_test[ECTEST_V_P12].t_description);
    lenvData.envTestPt4Measure = 
		getenvvoltage(show->s_test[ECTEST_V_P12].t_measured);

    lenvData.envTestPt5Descr = &envTestPt5Descr;
    lenvData.envTestPt5Descr->octet_ptr = 
		show->s_test[ECTEST_V_N12].t_description;
    lenvData.envTestPt5Descr->length = 
		strlen(show->s_test[ECTEST_V_N12].t_description);
    lenvData.envTestPt5Measure = 
		getenvvoltage(show->s_test[ECTEST_V_N12].t_measured);

    lenvData.envTestPt6Descr = &envTestPt6Descr;
    lenvData.envTestPt6Descr->octet_ptr = 
		show->s_test[ECTEST_V_N5].t_description;
    lenvData.envTestPt6Descr->length = 
		strlen(show->s_test[ECTEST_V_N5].t_description);
    lenvData.envTestPt6Measure = 
		getenvvoltage(show->s_test[ECTEST_V_N5].t_measured);

    lenvData.envTestPt1MarginVal = 
		getenvtemp(show->s_test[ECTEST_T_AMB].t_shut_marg);
    lenvData.envTestPt2MarginVal =
		getenvtemp(show->s_test[ECTEST_T_AIR].t_shut_marg);
    lenvData.envTestPt3MarginPercent =
		getenvnumber(show->s_test[ECTEST_V_P5].t_shut_marg,0);
    lenvData.envTestPt4MarginPercent =
		getenvnumber(show->s_test[ECTEST_V_P12].t_shut_marg,0);
    lenvData.envTestPt5MarginPercent =
		getenvnumber(show->s_test[ECTEST_V_N12].t_shut_marg,0);
    lenvData.envTestPt6MarginPercent =
		getenvnumber(show->s_test[ECTEST_V_N5].t_shut_marg,0);

    lenvData.envTestPt1last =
		getenvtemp(show->s_test[ECTEST_T_AMB].t_shut_last);
    lenvData.envTestPt2last =
		getenvtemp(show->s_test[ECTEST_T_AIR].t_shut_last);
    lenvData.envTestPt3last =
		getenvvoltage(show->s_test[ECTEST_V_P5].t_shut_last);
    lenvData.envTestPt4last =
		getenvvoltage(show->s_test[ECTEST_V_P12].t_shut_last);
    lenvData.envTestPt5last =
		getenvvoltage(show->s_test[ECTEST_V_N12].t_shut_last);
    lenvData.envTestPt6last =
		getenvvoltage(show->s_test[ECTEST_V_N5].t_shut_last);

    lenvData.envTestPt1warn =
		show->s_test[ECTEST_T_AMB].t_warning[0] ?
		D_envTestPt1warn_warning : D_envTestPt1warn_noWarning;
    lenvData.envTestPt2warn =
		show->s_test[ECTEST_T_AIR].t_warning[0] ?
		D_envTestPt2warn_warning : D_envTestPt2warn_noWarning;
    lenvData.envTestPt3warn =
		show->s_test[ECTEST_V_P5].t_warning[0] ?
		D_envTestPt3warn_warning : D_envTestPt3warn_noWarning;
    lenvData.envTestPt4warn =
		show->s_test[ECTEST_V_P12].t_warning[0] ?
		D_envTestPt4warn_warning : D_envTestPt4warn_noWarning;
    lenvData.envTestPt5warn =
		show->s_test[ECTEST_V_N12].t_warning[0] ?
		D_envTestPt5warn_warning : D_envTestPt5warn_noWarning;
    lenvData.envTestPt6warn =
		show->s_test[ECTEST_V_N5].t_warning[0] ?
		D_envTestPt6warn_warning : D_envTestPt6warn_noWarning;

    lenvData.envFirmVersion = &envFirmVersion;
    lenvData.envFirmVersion->octet_ptr = show->s_revfw;
    lenvData.envFirmVersion->length = strlen(show->s_revfw);

    lenvData.envTechnicianID = &envTechnicianID;
    lenvData.envTechnicianID->octet_ptr = show->s_tech_id;
    lenvData.envTechnicianID->length = strlen(show->s_tech_id);

    lenvData.envType = &envType;
    lenvData.envType->octet_ptr = show->s_devnam;
    lenvData.envType->length = strlen(show->s_devnam);

    lenvData.envBurnDate = &envBurnDate;
    lenvData.envBurnDate->octet_ptr = show->s_burn_date;
    lenvData.envBurnDate->length = strlen(show->s_burn_date);

    lenvData.envSerialNumber = &envSerialNumber;
    lenvData.envSerialNumber->octet_ptr = show->s_serial_num;
    lenvData.envSerialNumber->length = strlen(show->s_serial_num);

    return (&lenvData);
}

/*
 * old_lenvmib subsystem header
 */

#define MAJVERSION_old_lenvmib 1
#define MINVERSION_old_lenvmib 0
#define EDITVERSION_old_lenvmib 0

SUBSYS_HEADER(old_envmib,
              MAJVERSION_old_lenvmib,
              MINVERSION_old_lenvmib,
              EDITVERSION_old_lenvmib,
              init_old_lenvmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
