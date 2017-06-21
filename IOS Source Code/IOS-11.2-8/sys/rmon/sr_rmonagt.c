/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_rmonagt.c,v 3.3 1996/02/19 06:06:06 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/sr_rmonagt.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmonagt.c,v $
 * Revision 3.3  1996/02/19  06:06:06  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.2  1995/11/17  18:39:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:27  shaker
 * Bump version numbers from 2.x to 3.x.
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

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */


/*
 * Log: rmonagt.c,v 
 * Revision 1.1  1995/02/21  18:54:11  levi
 * Added stuff for cisco version of rmon monolithic agent
 *
 * Revision 1.8  1995/01/27  18:40:17  levi
 * Tried using signals to process packets in rmon_main task context, didn't work.
 *
 * Revision 1.7  1995/01/27  15:01:51  levi
 * Put vxworks rmon changes back into tree
 *
 * Revision 1.5  1995/01/11  17:11:11  levi
 * Updates for sunos version
 *
 * Revision 1.4  1994/12/22  21:30:44  levi
 * Fixes for VXWORKS
 *
 * Revision 1.3  1994/10/28  17:09:15  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.2  1994/10/26  21:38:27  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.1  1994/09/01  14:19:41  battle
 * New implementation of RMON.
 *
 * Revision 1.4  1994/06/13  14:56:16  battle
 * Added async library, GetObject, and byteswapping code.
 *
 * Revision 1.3  1994/04/22  15:13:33  battle
 * fixes to vxworks
 *
 * Revision 1.2  1993/11/10  19:30:45  battle
 * Additions to emanate to add shared library support and cvs headers.
 *
 */

#include "master.h"
#include "sr_rmonmib.h"
#include "../snmp/snmp_api.h"
#include "logger.h"
#include "../os/free.h"

/* this routine gets a mib variable and returns a varbind for it.
 *
 * This routine assumes a V2 domain! XXX
 * This routine doesn't do any security checking! XXX
 * This routine uses a null context! XXX
 * This routine doesn't give a real serial number! XXX
 */
VarBind        *
i_GetMib(object)
    OID            *object;
{
    int             i;
    ObjectInfo     *oip;
    VarBind        *vb;

    /* get the dispatch table entry for this object */
    if ((i = FindObjectClass(object, EXACT, SNMPv2_DOMAIN)) == -1) {
	vb = 0;
    }
    else {
	/* get a pointer to the dispatch table entry for this object */
	oip = GetOidElem(i);

	/* get the value */
	vb = (*(oip->get)) (object, oip, EXACT, (ContextInfo *) 0, 0);
    }

    /* return the varbind if successful, or null if not */
    return (vb);
}


/* return nonzero if we're low on memory */
int
HeapLowMem()
{
    return mempool_is_empty(MEMPOOL_CLASS_LOCAL);
}
      
