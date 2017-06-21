/* $Id: sr_mempoolmib.c,v 3.2.20.2 1996/04/13 06:05:39 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/os/sr_mempoolmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_mempoolmib.c,v $
 * Revision 3.2.20.2  1996/04/13  06:05:39  dkatz
 * CSCdi53190:  Init time memory charged to *Dead* process
 * Branch: California_branch
 * Ensure that memory allocated by the ephemeral Init process is charged
 * to *Init*.  Copy the allocator PC from the allocated block into the
 * fragment when fragmenting memory.  Add an option to "show memory" to
 * display the allocating process with each block.
 *
 * Revision 3.2.20.1  1996/04/02  05:37:47  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1996/02/20  06:15:32  jjohnson
 * CSCdi49356:  Need MIB variable for total DRAM
 * Fully instrument the memory pools
 *
 * Revision 3.1  1996/02/13  18:27:22  jjohnson
 * Initial placeholder
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
#include "sr_mempoolmib.h"
#include "sr_mempoolmib-mib.h"
#include "logger.h"
#include "region.h"
#include "free.h"
#include "free_private.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the ciscoMemoryPoolEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoMemoryPoolEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    ciscoMemoryPoolEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ciscoMemoryPoolType;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &ciscoMemoryPoolType, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoMemoryPoolEntry_get(serialNum, contextInfo, arg ,searchType, ciscoMemoryPoolType)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->ciscoMemoryPoolType;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoMemoryPoolName
    case I_ciscoMemoryPoolName:
       dp = (void *) (CloneOctetString(data->ciscoMemoryPoolName));
       break;
#endif /* I_ciscoMemoryPoolName */

#ifdef I_ciscoMemoryPoolAlternate
    case I_ciscoMemoryPoolAlternate:
       dp = (void *) (&data->ciscoMemoryPoolAlternate);
       break;
#endif /* I_ciscoMemoryPoolAlternate */

#ifdef I_ciscoMemoryPoolValid
    case I_ciscoMemoryPoolValid:
       dp = (void *) (&data->ciscoMemoryPoolValid);
       break;
#endif /* I_ciscoMemoryPoolValid */

#ifdef I_ciscoMemoryPoolUsed
    case I_ciscoMemoryPoolUsed:
       dp = (void *) (&data->ciscoMemoryPoolUsed);
       break;
#endif /* I_ciscoMemoryPoolUsed */

#ifdef I_ciscoMemoryPoolFree
    case I_ciscoMemoryPoolFree:
       dp = (void *) (&data->ciscoMemoryPoolFree);
       break;
#endif /* I_ciscoMemoryPoolFree */

#ifdef I_ciscoMemoryPoolLargestFree
    case I_ciscoMemoryPoolLargestFree:
       dp = (void *) (&data->ciscoMemoryPoolLargestFree);
       break;
#endif /* I_ciscoMemoryPoolLargestFree */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */





ciscoMemoryPoolEntry_t *
k_ciscoMemoryPoolEntry_get(serialNum, contextInfo, nominator, searchType, ciscoMemoryPoolType)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ciscoMemoryPoolType;
{
   static ciscoMemoryPoolEntry_t ciscoMemoryPoolEntryData;
   static OctetString poolname;
   mempool *pool = NULL;
   boolean found;
   boolean got_lost;
   ulong used;
   ulong free;
   ulong largefree;
   ulong alternate;
   ulong lowfree;

   if (ciscoMemoryPoolType < 1)
       ciscoMemoryPoolType = 1;

   found = FALSE;

   while (ciscoMemoryPoolType < MEMPOOL_CLASS_MAX && !found) {
       pool = mempools[ciscoMemoryPoolType];
       if (pool && (pool->class == ciscoMemoryPoolType)) {
	   found = TRUE;
       } else {
	   ciscoMemoryPoolType++;
       }
   }

   if (!found)
       return(NULL);

   got_lost = spillmem(pool, FALSE, FALSE, FALSE, &used, &free, &largefree,
		       &lowfree);
   if (pool->alternate)
       alternate = pool->alternate->class;
   else
       alternate = 0;
   ciscoMemoryPoolEntryData.ciscoMemoryPoolType = pool->class;
   poolname.octet_ptr = pool->name;
   poolname.length = strlen(pool->name);
   ciscoMemoryPoolEntryData.ciscoMemoryPoolName = &poolname;
   ciscoMemoryPoolEntryData.ciscoMemoryPoolAlternate = alternate;
   ciscoMemoryPoolEntryData.ciscoMemoryPoolValid = got_lost ?
       D_ciscoMemoryPoolValid_false : D_ciscoMemoryPoolValid_true;
   ciscoMemoryPoolEntryData.ciscoMemoryPoolUsed = used;
   ciscoMemoryPoolEntryData.ciscoMemoryPoolFree = free;
   ciscoMemoryPoolEntryData.ciscoMemoryPoolLargestFree = largefree;
   return(&ciscoMemoryPoolEntryData);
}



static void
init_mempoolmib (subsystype *subsys)
{
    load_mib(mempoolmib_OidList, mempoolmib_OidListNum);
    load_oid(mempoolmib_oid_table);
}

/*
 * mempoolmib subsystem header
 */

#define MAJVERSION_mempoolmib 1
#define MINVERSION_mempoolmib 0
#define EDITVERSION_mempoolmib 0

SUBSYS_HEADER(mempoolmib,
              MAJVERSION_mempoolmib,
              MINVERSION_mempoolmib,
              EDITVERSION_mempoolmib,
              init_mempoolmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
