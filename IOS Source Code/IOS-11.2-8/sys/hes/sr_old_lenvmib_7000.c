/* $Id: sr_old_lenvmib_7000.c,v 3.4.26.2 1996/05/21 06:29:31 mordock Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_old_lenvmib_7000.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_lenvmib_7000.c,v $
 * Revision 3.4.26.2  1996/05/21  06:29:31  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.4.26.1  1996/03/18  19:43:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.1  1996/03/07  09:40:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/18  15:45:47  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:24:03  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  09:20:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/28  09:25:00  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/16  04:27:41  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:44:24  hampton
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
#include "sr_old_lenvmib_7000.h"

#include "../snmp/chassismib_registry.h"
#include "../os/clock.h"
#include "envm.h"

void
init_old_lenvmib_7000 (subsystype *subsys)
{
    load_mib(old_lenvmib_7000_OidList, old_lenvmib_7000_OidListNum);
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
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void           *dp;
    lenv_t         *data=NULL;


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


#define ECM_7000 3
    lenvData.envPresent = ECM_7000;

    lenvData.envTestPt1Descr = &envTestPt1Descr;
    lenvData.envTestPt1Descr->octet_ptr = 
		show->s_test[ECTEST_T_INLET].t_description;
    lenvData.envTestPt1Descr->length = 
		strlen(show->s_test[ECTEST_T_INLET].t_description);
    lenvData.envTestPt1Measure = 
		CELSIUS(show->s_test[ECTEST_T_INLET].t_measured);

    lenvData.envTestPt2Descr = &envTestPt2Descr;
    lenvData.envTestPt2Descr->octet_ptr = 
		show->s_test[ECTEST_T_EXHAUST].t_description;
    lenvData.envTestPt2Descr->length = 
		strlen(show->s_test[ECTEST_T_EXHAUST].t_description);
    lenvData.envTestPt2Measure = 
		CELSIUS(show->s_test[ECTEST_T_EXHAUST].t_measured);

    lenvData.envTestPt3Descr = &envTestPt3Descr;
    lenvData.envTestPt3Descr->octet_ptr = 
		show->s_test[ECTEST_V_P5].t_description;
    lenvData.envTestPt3Descr->length = 
		strlen(show->s_test[ECTEST_V_P5].t_description);
    lenvData.envTestPt3Measure = 
		convert_millivolts(show->s_test[ECTEST_V_P5].t_measured,
		ECTEST_V_P5);

    lenvData.envTestPt4Descr = &envTestPt4Descr;
    lenvData.envTestPt4Descr->octet_ptr = 
		show->s_test[ECTEST_V_P12].t_description;
    lenvData.envTestPt4Descr->length = 
		strlen(show->s_test[ECTEST_V_P12].t_description);
    lenvData.envTestPt4Measure = 
		convert_millivolts(show->s_test[ECTEST_V_P12].t_measured,
		ECTEST_V_P12);

    lenvData.envTestPt5Descr = &envTestPt5Descr;
    lenvData.envTestPt5Descr->octet_ptr = 
		show->s_test[ECTEST_V_N12].t_description;
    lenvData.envTestPt5Descr->length = 
		strlen(show->s_test[ECTEST_V_N12].t_description);
    lenvData.envTestPt5Measure = 
		convert_millivolts(show->s_test[ECTEST_V_N12].t_measured,
		ECTEST_V_N12);

    lenvData.envTestPt6Descr = &envTestPt6Descr;
    lenvData.envTestPt6Descr->octet_ptr = 
		show->s_test[ECTEST_V_P24].t_description;
    lenvData.envTestPt6Descr->length = 
		strlen(show->s_test[ECTEST_V_P24].t_description);
    lenvData.envTestPt6Measure = 
		convert_millivolts(show->s_test[ECTEST_V_P24].t_measured,
		ECTEST_V_P24);

    lenvData.envTestPt1MarginVal = 
		CELSIUS(env_temp_bounds(ECTEST_T_INLET,SHUTDOWN));
    lenvData.envTestPt2MarginVal =
		CELSIUS(env_temp_bounds(ECTEST_T_EXHAUST,SHUTDOWN));
    lenvData.envTestPt3MarginPercent = 0; /* upsupported */
    lenvData.envTestPt4MarginPercent = 0; /* upsupported */
    lenvData.envTestPt5MarginPercent = 0; /* upsupported */
    lenvData.envTestPt6MarginPercent = 0; /* upsupported */

    lenvData.envTestPt1last = last_showp ?
		CELSIUS(last_showp->s_test[ECTEST_T_INLET].t_measured) : 0;
    lenvData.envTestPt2last = last_showp ?
		CELSIUS(last_showp->s_test[ECTEST_T_EXHAUST].t_measured) : 0;
    lenvData.envTestPt3last = last_showp ?
		convert_millivolts(last_showp->s_test[ECTEST_V_P5].t_measured, 
		ECTEST_V_P5) : 0;
    lenvData.envTestPt4last = last_showp ?
		convert_millivolts(last_showp->s_test[ECTEST_V_P12].t_measured,
		ECTEST_V_P12) : 0;
    lenvData.envTestPt5last = last_showp ?
		convert_millivolts(last_showp->s_test[ECTEST_V_N12].t_measured,
		ECTEST_V_N12) : 0;
    lenvData.envTestPt6last = last_showp ?
		convert_millivolts(last_showp->s_test[ECTEST_V_P24].t_measured,
		ECTEST_V_P24) : 0;

    lenvData.envTestPt1warn =
		(show->s_test[ECTEST_T_INLET].t_state == NORMAL) ?
		D_envTestPt1warn_noWarning : D_envTestPt1warn_warning;
    lenvData.envTestPt2warn =
		(show->s_test[ECTEST_T_EXHAUST].t_state == NORMAL) ?
		D_envTestPt2warn_noWarning : D_envTestPt2warn_warning;
    lenvData.envTestPt3warn =
		(show->s_test[ECTEST_V_P5].t_state == NORMAL) ?
		D_envTestPt3warn_noWarning : D_envTestPt3warn_warning;
    lenvData.envTestPt4warn =
		(show->s_test[ECTEST_V_P12].t_state == NORMAL) ?
		D_envTestPt4warn_noWarning : D_envTestPt4warn_warning;
    lenvData.envTestPt5warn =
		(show->s_test[ECTEST_V_N12].t_state == NORMAL) ?
		D_envTestPt5warn_noWarning : D_envTestPt5warn_warning;
    lenvData.envTestPt6warn =
		(show->s_test[ECTEST_V_P24].t_state == NORMAL) ?
		D_envTestPt6warn_noWarning : D_envTestPt6warn_warning;

    lenvData.envFirmVersion = &envFirmVersion;
    lenvData.envFirmVersion->octet_ptr = NULL;
    lenvData.envFirmVersion->length = 0;

    lenvData.envTechnicianID = &envTechnicianID;
    lenvData.envTechnicianID->octet_ptr = NULL;
    lenvData.envTechnicianID->length = 0;

    lenvData.envType = &envType;
    lenvData.envType->octet_ptr = NULL;
    lenvData.envType->length = 0;

    lenvData.envBurnDate = &envBurnDate;
    lenvData.envBurnDate->octet_ptr = NULL;
    lenvData.envBurnDate->length = 0;

    lenvData.envSerialNumber = &envSerialNumber;
    lenvData.envSerialNumber->octet_ptr = NULL;
    lenvData.envSerialNumber->length = 0;

    return (&lenvData);
}

/*
 * old_lenvmib_7000 subsystem header
 */

#define MAJVERSION_old_lenvmib_7000 1
#define MINVERSION_old_lenvmib_7000 0
#define EDITVERSION_old_lenvmib_7000 0

SUBSYS_HEADER(old_envmib,
              MAJVERSION_old_lenvmib_7000,
              MINVERSION_old_lenvmib_7000,
              EDITVERSION_old_lenvmib_7000,
              init_old_lenvmib_7000,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
