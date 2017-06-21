/* $Id: sr_imagemib.c,v 3.5 1996/02/28 20:49:40 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_imagemib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * January 1995, Yunsen Wang (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_imagemib.c,v $
 * Revision 3.5  1996/02/28  20:49:40  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  16:37:55  jjohnson
 * CSCdi45754:  snmp subsystem should not contain non-snmp mibs
 * move the rs232 mib, image mib, and terminal services mib into separate
 * subsystems.  move the cisco interfaces mib into the interfaces mib
 * subsystem.  consolidate the snmp timer subsystem into the snmp
 * subsystem since it really is core snmp technology.
 *
 * Revision 3.3  1995/12/14  08:27:33  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  18:59:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:20  hampton
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
#include "sr_imagemib.h"
#include "sr_imagemib-mib.h"



static void
init_imagemib(subsystype *subsys)
{
    load_mib(imagemib_OidList, imagemib_OidListNum);
    load_oid(imagemib_oid_table);
}









/*---------------------------------------------------------------------
 * Retrieve data from the ciscoImageEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ciscoImageEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ciscoImageEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ciscoImageIndex;


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
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &ciscoImageIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ciscoImageEntry_get(serialNum, contextInfo, arg, searchType, ciscoImageIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ciscoImageIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ciscoImageString
      case I_ciscoImageString:
	dp = MakeOctetString(data->ciscoImageString->octet_ptr, data->ciscoImageString->length);
	break;
#endif				       /* I_ciscoImageString */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}


ciscoImageEntry_t *
k_ciscoImageEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            ciscoImageIndex)
{
    int i;
    static OctetString string;
    static ciscoImageEntry_t ciscoImageEntryData;

    if (searchType == NEXT && ciscoImageIndex < 1)
	    ciscoImageIndex = 1;
    i = ciscoImageIndex - 1;
    if (ciscoImageIndex >= (long)cw_table_cnt || ciscoImageIndex < 1)
	return (NULL);

    ciscoImageEntryData.ciscoImageIndex = ciscoImageIndex;
    ciscoImageEntryData.ciscoImageString = &string;
    if ((string.length = strlen(cw_table[i].ptr)) > CW_MAX_DESCR_STRLEN)
	string.length = CW_MAX_DESCR_STRLEN;
    string.octet_ptr = cw_table[i].ptr;

    return (&ciscoImageEntryData);

}

/*
 * imagemib subsystem header
 */

#define MAJVERSION_imagemib 1
#define MINVERSION_imagemib 0
#define EDITVERSION_imagemib 0

SUBSYS_HEADER(imagemib,
              MAJVERSION_imagemib,
              MINVERSION_imagemib,
              EDITVERSION_imagemib,
              init_imagemib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
