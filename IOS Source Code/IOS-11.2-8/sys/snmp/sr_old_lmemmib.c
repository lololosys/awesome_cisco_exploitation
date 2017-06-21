/* $Id: sr_old_lmemmib.c,v 3.5.6.1 1996/07/03 20:43:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_old_lmemmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_lmemmib.c,v $
 * Revision 3.5.6.1  1996/07/03  20:43:33  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.5  1996/03/01  14:42:23  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/02/12  18:18:35  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.3  1995/12/14  08:27:37  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  19:00:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:25:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/12  07:01:15  tli
 * CSCdi42042:  More buffer system housekeeping
 *
 * Revision 2.2  1995/10/12  06:02:30  tli
 * CSCdi42042:  More buffer system housekeeping
 *
 * Revision 2.1  1995/06/07  22:50:00  hampton
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
#include "../snmp/snmp_api.h"
#include "sr_old_lmemmib.h"
#include "sr_old_lmemmib-mib.h"

#include "packet.h"
#include "../os/element.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "logger.h"
#include "../os/free.h"

void
init_old_lmemmib(void)
{
    load_mib(old_lmemmib_OidList, old_lmemmib_OidListNum);
    load_oid(old_lmemmib_oid_table);
}


/*---------------------------------------------------------------------
 * Retrieve data from the lmem family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
lmem_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             arg;
    void           *dp;
    lmem_t         *data=NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_lmem_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_freeMem
      case I_freeMem:
	dp = &data->freeMem;
	break;
#endif				       /* I_freeMem */

#ifdef I_bufferElFree
      case I_bufferElFree:
	dp = &data->bufferElFree;
	break;
#endif				       /* I_bufferElFree */

#ifdef I_bufferElMax
      case I_bufferElMax:
	dp = &data->bufferElMax;
	break;
#endif				       /* I_bufferElMax */

#ifdef I_bufferElHit
      case I_bufferElHit:
	dp = &data->bufferElHit;
	break;
#endif				       /* I_bufferElHit */

#ifdef I_bufferElMiss
      case I_bufferElMiss:
	dp = &data->bufferElMiss;
	break;
#endif				       /* I_bufferElMiss */

#ifdef I_bufferElCreate
      case I_bufferElCreate:
	dp = &data->bufferElCreate;
	break;
#endif				       /* I_bufferElCreate */

#ifdef I_bufferSmSize
      case I_bufferSmSize:
	dp = &data->bufferSmSize;
	break;
#endif				       /* I_bufferSmSize */

#ifdef I_bufferSmTotal
      case I_bufferSmTotal:
	dp = &data->bufferSmTotal;
	break;
#endif				       /* I_bufferSmTotal */

#ifdef I_bufferSmFree
      case I_bufferSmFree:
	dp = &data->bufferSmFree;
	break;
#endif				       /* I_bufferSmFree */

#ifdef I_bufferSmMax
      case I_bufferSmMax:
	dp = &data->bufferSmMax;
	break;
#endif				       /* I_bufferSmMax */

#ifdef I_bufferSmHit
      case I_bufferSmHit:
	dp = &data->bufferSmHit;
	break;
#endif				       /* I_bufferSmHit */

#ifdef I_bufferSmMiss
      case I_bufferSmMiss:
	dp = &data->bufferSmMiss;
	break;
#endif				       /* I_bufferSmMiss */

#ifdef I_bufferSmTrim
      case I_bufferSmTrim:
	dp = &data->bufferSmTrim;
	break;
#endif				       /* I_bufferSmTrim */

#ifdef I_bufferSmCreate
      case I_bufferSmCreate:
	dp = &data->bufferSmCreate;
	break;
#endif				       /* I_bufferSmCreate */

#ifdef I_bufferMdSize
      case I_bufferMdSize:
	dp = &data->bufferMdSize;
	break;
#endif				       /* I_bufferMdSize */

#ifdef I_bufferMdTotal
      case I_bufferMdTotal:
	dp = &data->bufferMdTotal;
	break;
#endif				       /* I_bufferMdTotal */

#ifdef I_bufferMdFree
      case I_bufferMdFree:
	dp = &data->bufferMdFree;
	break;
#endif				       /* I_bufferMdFree */

#ifdef I_bufferMdMax
      case I_bufferMdMax:
	dp = &data->bufferMdMax;
	break;
#endif				       /* I_bufferMdMax */

#ifdef I_bufferMdHit
      case I_bufferMdHit:
	dp = &data->bufferMdHit;
	break;
#endif				       /* I_bufferMdHit */

#ifdef I_bufferMdMiss
      case I_bufferMdMiss:
	dp = &data->bufferMdMiss;
	break;
#endif				       /* I_bufferMdMiss */

#ifdef I_bufferMdTrim
      case I_bufferMdTrim:
	dp = &data->bufferMdTrim;
	break;
#endif				       /* I_bufferMdTrim */

#ifdef I_bufferMdCreate
      case I_bufferMdCreate:
	dp = &data->bufferMdCreate;
	break;
#endif				       /* I_bufferMdCreate */

#ifdef I_bufferBgSize
      case I_bufferBgSize:
	dp = &data->bufferBgSize;
	break;
#endif				       /* I_bufferBgSize */

#ifdef I_bufferBgTotal
      case I_bufferBgTotal:
	dp = &data->bufferBgTotal;
	break;
#endif				       /* I_bufferBgTotal */

#ifdef I_bufferBgFree
      case I_bufferBgFree:
	dp = &data->bufferBgFree;
	break;
#endif				       /* I_bufferBgFree */

#ifdef I_bufferBgMax
      case I_bufferBgMax:
	dp = &data->bufferBgMax;
	break;
#endif				       /* I_bufferBgMax */

#ifdef I_bufferBgHit
      case I_bufferBgHit:
	dp = &data->bufferBgHit;
	break;
#endif				       /* I_bufferBgHit */

#ifdef I_bufferBgMiss
      case I_bufferBgMiss:
	dp = &data->bufferBgMiss;
	break;
#endif				       /* I_bufferBgMiss */

#ifdef I_bufferBgTrim
      case I_bufferBgTrim:
	dp = &data->bufferBgTrim;
	break;
#endif				       /* I_bufferBgTrim */

#ifdef I_bufferBgCreate
      case I_bufferBgCreate:
	dp = &data->bufferBgCreate;
	break;
#endif				       /* I_bufferBgCreate */

#ifdef I_bufferLgSize
      case I_bufferLgSize:
	dp = &data->bufferLgSize;
	break;
#endif				       /* I_bufferLgSize */

#ifdef I_bufferLgTotal
      case I_bufferLgTotal:
	dp = &data->bufferLgTotal;
	break;
#endif				       /* I_bufferLgTotal */

#ifdef I_bufferLgFree
      case I_bufferLgFree:
	dp = &data->bufferLgFree;
	break;
#endif				       /* I_bufferLgFree */

#ifdef I_bufferLgMax
      case I_bufferLgMax:
	dp = &data->bufferLgMax;
	break;
#endif				       /* I_bufferLgMax */

#ifdef I_bufferLgHit
      case I_bufferLgHit:
	dp = &data->bufferLgHit;
	break;
#endif				       /* I_bufferLgHit */

#ifdef I_bufferLgMiss
      case I_bufferLgMiss:
	dp = &data->bufferLgMiss;
	break;
#endif				       /* I_bufferLgMiss */

#ifdef I_bufferLgTrim
      case I_bufferLgTrim:
	dp = &data->bufferLgTrim;
	break;
#endif				       /* I_bufferLgTrim */

#ifdef I_bufferLgCreate
      case I_bufferLgCreate:
	dp = &data->bufferLgCreate;
	break;
#endif				       /* I_bufferLgCreate */

#ifdef I_bufferFail
      case I_bufferFail:
	dp = &data->bufferFail;
	break;
#endif				       /* I_bufferFail */

#ifdef I_bufferNoMem
      case I_bufferNoMem:
	dp = &data->bufferNoMem;
	break;
#endif				       /* I_bufferNoMem */

#ifdef I_bufferHgSize
      case I_bufferHgSize:
	dp = &data->bufferHgSize;
	break;
#endif				       /* I_bufferHgSize */

#ifdef I_bufferHgTotal
      case I_bufferHgTotal:
	dp = &data->bufferHgTotal;
	break;
#endif				       /* I_bufferHgTotal */

#ifdef I_bufferHgFree
      case I_bufferHgFree:
	dp = &data->bufferHgFree;
	break;
#endif				       /* I_bufferHgFree */

#ifdef I_bufferHgMax
      case I_bufferHgMax:
	dp = &data->bufferHgMax;
	break;
#endif				       /* I_bufferHgMax */

#ifdef I_bufferHgHit
      case I_bufferHgHit:
	dp = &data->bufferHgHit;
	break;
#endif				       /* I_bufferHgHit */

#ifdef I_bufferHgMiss
      case I_bufferHgMiss:
	dp = &data->bufferHgMiss;
	break;
#endif				       /* I_bufferHgMiss */

#ifdef I_bufferHgTrim
      case I_bufferHgTrim:
	dp = &data->bufferHgTrim;
	break;
#endif				       /* I_bufferHgTrim */

#ifdef I_bufferHgCreate
      case I_bufferHgCreate:
	dp = &data->bufferHgCreate;
	break;
#endif				       /* I_bufferHgCreate */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}



lmem_t         *
k_lmem_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static lmem_t   lmemData;

    lmemData.freeMem =  mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
					       MEMPOOL_ALTERNATE_INCLUDE);
    lmemData.bufferElFree = elements;
    lmemData.bufferElMax = INITELEMENTS;
    lmemData.bufferElHit = elementshit;
    lmemData.bufferElMiss = elementsmissed;
    lmemData.bufferElCreate = elementsmade;
    lmemData.bufferSmSize = small->size;
    lmemData.bufferSmTotal = small->total;
    lmemData.bufferSmFree = small->free;
    lmemData.bufferSmMax = small->maxfree;
    lmemData.bufferSmHit = small->hit;
    lmemData.bufferSmMiss = small->miss;
    lmemData.bufferSmTrim = small->trims;
    lmemData.bufferSmCreate = small->grows;
    lmemData.bufferMdSize = middle->size;
    lmemData.bufferMdTotal = middle->total;
    lmemData.bufferMdFree = middle->free;
    lmemData.bufferMdMax = middle->maxfree;
    lmemData.bufferMdHit = middle->hit;
    lmemData.bufferMdMiss = middle->miss;
    lmemData.bufferMdTrim = middle->trims;
    lmemData.bufferMdCreate = middle->grows;
    lmemData.bufferBgSize = big->size;
    lmemData.bufferBgTotal = big->total;
    lmemData.bufferBgFree = big->free;
    lmemData.bufferBgMax = big->maxfree;
    lmemData.bufferBgHit = big->hit;
    lmemData.bufferBgMiss = big->miss;
    lmemData.bufferBgTrim = big->trims;
    lmemData.bufferBgCreate = big->grows;
    lmemData.bufferLgSize = large->size;
    lmemData.bufferLgTotal = large->total;
    lmemData.bufferLgFree = large->free;
    lmemData.bufferLgMax = large->maxfree;
    lmemData.bufferLgHit = large->hit;
    lmemData.bufferLgMiss = large->miss;
    lmemData.bufferLgTrim = large->trims;
    lmemData.bufferLgCreate = large->grows;
    lmemData.bufferFail = small->failures + middle->failures +
	big->failures + large->failures + huge->failures;
    lmemData.bufferNoMem = small->nostorage + middle->nostorage +
	big->nostorage + large->nostorage + huge->nostorage;
    lmemData.bufferHgSize = huge->size;
    lmemData.bufferHgTotal = huge->total;
    lmemData.bufferHgFree = huge->free;
    lmemData.bufferHgMax = huge->maxfree;
    lmemData.bufferHgHit = huge->hit;
    lmemData.bufferHgMiss = huge->miss;
    lmemData.bufferHgTrim = huge->trims;
    lmemData.bufferHgCreate = huge->grows;

    return (&lmemData);
}
      
