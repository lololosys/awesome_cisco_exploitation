/* $Id: sr_fsipmib.c,v 3.5.12.2 1996/05/21 09:48:07 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_fsipmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1995, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_fsipmib.c,v $
 * Revision 3.5.12.2  1996/05/21  09:48:07  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.12.1  1996/05/21  06:29:30  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5  1996/02/15  22:08:31  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/24  22:03:03  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.3  1996/01/18  15:45:44  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.2  1995/11/17  09:20:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  07:31:45  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:24:56  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:44:13  hampton
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
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "if_cmd.h"
#include "../snmp/snmp_api.h"
#include "sr_fsipmib.h"
#include "sr_fsipmib-mib.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the lFSIPEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lFSIPEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    lFSIPEntry_t    *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            locIfFSIPIndex;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
        if (instLength != 1) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &locIfFSIPIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_lFSIPEntry_get(serialNum, contextInfo, arg ,searchType, locIfFSIPIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->locIfFSIPIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_locIfFSIPIndex
    case I_locIfFSIPIndex:
       dp = (void *) (&data->locIfFSIPIndex);
       break;
#endif /* I_locIfFSIPIndex */

#ifdef I_locIfFSIPtype
    case I_locIfFSIPtype:
       dp = (void *) (&data->locIfFSIPtype);
       break;
#endif /* I_locIfFSIPtype */

#ifdef I_locIfFSIPrts
    case I_locIfFSIPrts:
       dp = (void *) (&data->locIfFSIPrts);
       break;
#endif /* I_locIfFSIPrts */

#ifdef I_locIfFSIPcts
    case I_locIfFSIPcts:
       dp = (void *) (&data->locIfFSIPcts);
       break;
#endif /* I_locIfFSIPcts */

#ifdef I_locIfFSIPdtr
    case I_locIfFSIPdtr:
       dp = (void *) (&data->locIfFSIPdtr);
       break;
#endif /* I_locIfFSIPdtr */

#ifdef I_locIfFSIPdcd
    case I_locIfFSIPdcd:
       dp = (void *) (&data->locIfFSIPdcd);
       break;
#endif /* I_locIfFSIPdcd */

#ifdef I_locIfFSIPdsr
    case I_locIfFSIPdsr:
       dp = (void *) (&data->locIfFSIPdsr);
       break;
#endif /* I_locIfFSIPdsr */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */


static boolean fsip_idb_tester (hwidbtype *idb, long* index2)
{
    return (idb->type == IDBTYPE_CBUSSERIAL);
}




lFSIPEntry_t *
k_lFSIPEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            locIfFSIPIndex)
{
    static lFSIPEntry_t lFSIPEntryData;
    register hwidbtype *idb;
    ushort signal_status;
    leveltype level;

    idb = ifmib_hwidb_get(serialNum, searchType, locIfFSIPIndex,
			  fsip_idb_tester, NULL);
    if (idb == NULL)
	return (NULL);

    /*
     * we have an FSIP controller, so grab its status
     */
    level = raise_interrupt_level(NETS_DISABLE);
    if ((CMD_RSP_OKAY != select_if(idb)) ||
	(CMD_RSP_OKAY != send_fsip_signal_sts_cmd(idb, &signal_status))) {
        reset_interrupt_level(level);
	return NULL;
    }
    reset_interrupt_level(level);

    lFSIPEntryData.locIfFSIPIndex = idb->snmp_if_index;

    /* FSIP_TYPE */
    if (idb->dte_interface)
	lFSIPEntryData.locIfFSIPtype = D_locIfFSIPtype_dte;
    else
	lFSIPEntryData.locIfFSIPtype = D_locIfFSIPtype_dce;

    /* FSIP_RTS */
    if (idb->dte_interface)
	lFSIPEntryData.locIfFSIPrts = (signal_status & FSIP_MASK_RTS) ?
	    D_locIfFSIPrts_up : D_locIfFSIPrts_down;
    else
	lFSIPEntryData.locIfFSIPrts = (signal_status & FSIP_MASK_CTS) ?
	    D_locIfFSIPrts_up : D_locIfFSIPrts_down;

    /* FSIP_CTS */
    if (idb->dte_interface)
	lFSIPEntryData.locIfFSIPcts = (signal_status & FSIP_MASK_CTS) ?
	    D_locIfFSIPcts_up : D_locIfFSIPcts_down;
    else
	lFSIPEntryData.locIfFSIPcts = (signal_status & FSIP_MASK_RTS) ?
	    D_locIfFSIPcts_up : D_locIfFSIPcts_down;

    /* FSIP_DTR */
    if (idb->dte_interface)
	lFSIPEntryData.locIfFSIPdtr = (signal_status & FSIP_MASK_DTR) ?	
	    D_locIfFSIPdtr_up : D_locIfFSIPdtr_down;
    else
	lFSIPEntryData.locIfFSIPdtr = (signal_status & FSIP_MASK_DCD) ?	
	    D_locIfFSIPdtr_up : D_locIfFSIPdtr_down;

    /* FSIP_DCD */
    if (idb->dte_interface)
	lFSIPEntryData.locIfFSIPdcd = (signal_status & FSIP_MASK_DCD) ?
	    D_locIfFSIPdcd_up : D_locIfFSIPdcd_down;
    else
	lFSIPEntryData.locIfFSIPdcd = (signal_status & FSIP_MASK_DTR) ?
	    D_locIfFSIPdcd_up : D_locIfFSIPdcd_down;

    /* FSIP_DSR */
    if (idb->dte_interface)
	lFSIPEntryData.locIfFSIPdsr = (signal_status & FSIP_MASK_DSR) ? 
	    D_locIfFSIPdsr_up : D_locIfFSIPdsr_down;
    else
	lFSIPEntryData.locIfFSIPdsr = (signal_status & FSIP_MASK_DTR) ? 
	    D_locIfFSIPdsr_up : D_locIfFSIPdsr_down;


    return (&lFSIPEntryData);
}



static void
init_fsipmib (subsystype *subsys)
{
    load_mib(fsipmib_OidList, fsipmib_OidListNum);
    load_oid(fsipmib_oid_table);
}

/*
 * fsipmib subsystem header
 */

#define MAJVERSION_fsipmib 1
#define MINVERSION_fsipmib 0
#define EDITVERSION_fsipmib 0

SUBSYS_HEADER(fsipmib,
              MAJVERSION_fsipmib,
              MINVERSION_fsipmib,
              EDITVERSION_fsipmib,
              init_fsipmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
