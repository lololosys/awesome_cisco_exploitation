/* $Id: sr_appndlurmib.c,v 3.1.4.3 1996/08/05 07:25:07 bchan Exp $
 * $Source: /release/112/cvs/Xsys/appn/sr_appndlurmib.c,v $
 *------------------------------------------------------------------
 * DLUR snmn sub-agent code
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_appndlurmib.c,v $
 * Revision 3.1.4.3  1996/08/05  07:25:07  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/07/17  06:52:26  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:39:13  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/05/12  08:09:33  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.2.1  1996/03/21  03:30:57  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  06:50:22  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file is machine generated
 * Do not attempt to edit it or place it under CVS control
 */


#include "master.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_appndlurmib.h"
#include "sr_appndlurmib-mib.h"
#include "../entity/entity_api.h"

/*
 * Start of system-independent routines
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
 */




#ifdef HAVE_STDLIB_H
#endif	/* HAVE_STDLIB_H */

#ifdef HAVE_MALLOC_H
#endif	/* HAVE_MALLOC_H */


/*---------------------------------------------------------------------
 * Retrieve data from the dlurNodeCapabilities family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
dlurNodeCapabilities_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    dlurNodeCapabilities_t *data = NULL;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and
     * the single instance element be 0.
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
     if ( (arg == -1) || (data = k_dlurNodeCapabilities_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_dlurReleaseLevel
    case I_dlurReleaseLevel:
       dp = (void *) (CloneOctetString(data->dlurReleaseLevel));
       break;
#endif /* I_dlurReleaseLevel */

#ifdef I_dlurANSsupport
    case I_dlurANSsupport:
       dp = (void *) (&data->dlurANSsupport);
       break;
#endif /* I_dlurANSsupport */

#ifdef I_dlurMultiSubnetSupport
    case I_dlurMultiSubnetSupport:
       dp = (void *) (&data->dlurMultiSubnetSupport);
       break;
#endif /* I_dlurMultiSubnetSupport */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dlurPuEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
dlurPuEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    dlurPuEntry_t   *data;
    SR_UINT32   buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        dlurPuIndex;

    data = NULL;
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

    if ( (InstToInt(incoming, 0 + object->oid.length, &dlurPuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_dlurPuEntry_get(serialNum, contextInfo, arg ,searchType, dlurPuIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->dlurPuIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_dlurPuIndex
    case I_dlurPuIndex:
       dp = (void *) (&data->dlurPuIndex);
       break;
#endif /* I_dlurPuIndex */

#ifdef I_dlurPuName
    case I_dlurPuName:
       dp = (void *) (CloneOctetString(data->dlurPuName));
       break;
#endif /* I_dlurPuName */

#ifdef I_dlurPuStatus
    case I_dlurPuStatus:
       dp = (void *) (&data->dlurPuStatus);
       break;
#endif /* I_dlurPuStatus */

#ifdef I_dlurPuANSsupport
    case I_dlurPuANSsupport:
       dp = (void *) (&data->dlurPuANSsupport);
       break;
#endif /* I_dlurPuANSsupport */

#ifdef I_dlurPuLocation
    case I_dlurPuLocation:
       dp = (void *) (&data->dlurPuLocation);
       break;
#endif /* I_dlurPuLocation */

#ifdef I_dlurPuDlusSessnStatus
    case I_dlurPuDlusSessnStatus:
       dp = (void *) (&data->dlurPuDlusSessnStatus);
       break;
#endif /* I_dlurPuDlusSessnStatus */

#ifdef I_dlurPuActiveDlusName
    case I_dlurPuActiveDlusName:
       dp = (void *) (CloneOctetString(data->dlurPuActiveDlusName));
       break;
#endif /* I_dlurPuActiveDlusName */

#ifdef I_dlurPuDefPrimDlusName
    case I_dlurPuDefPrimDlusName:
       dp = (void *) (CloneOctetString(data->dlurPuDefPrimDlusName));
       break;
#endif /* I_dlurPuDefPrimDlusName */

#ifdef I_dlurPuDefBackDlusName
    case I_dlurPuDefBackDlusName:
       dp = (void *) (CloneOctetString(data->dlurPuDefBackDlusName));
       break;
#endif /* I_dlurPuDefBackDlusName */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dlurDlusEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
dlurDlusEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    dlurDlusEntry_t *data;
    SR_UINT32   buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        dlurDlusIndex;

    data = NULL;
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

    if ( (InstToInt(incoming, 0 + object->oid.length, &dlurDlusIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_dlurDlusEntry_get(serialNum, contextInfo, arg ,searchType, dlurDlusIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->dlurDlusIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_dlurDlusIndex
    case I_dlurDlusIndex:
       dp = (void *) (&data->dlurDlusIndex);
       break;
#endif /* I_dlurDlusIndex */

#ifdef I_dlurDlusName
    case I_dlurDlusName:
       dp = (void *) (CloneOctetString(data->dlurDlusName));
       break;
#endif /* I_dlurDlusName */

#ifdef I_dlurDlusSessnStatus
    case I_dlurDlusSessnStatus:
       dp = (void *) (&data->dlurDlusSessnStatus);
       break;
#endif /* I_dlurDlusSessnStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}
/*
 * Start of system-dependent routines
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
 */


SubAgentFunc* dlurRp = NULL;
SubAgentFunc* dlurCip = NULL;;


dlurNodeCapabilities_t *
k_dlurNodeCapabilities_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   SubAgentFunc* func;
   func = snmp_extract_logical_entity(contextInfo) ? dlurCip : dlurRp;
   if (func){
       return func->t_dlurNodeCapabilities_get(serialNum, contextInfo, nominator);
   }
   return NULL;
}

dlurPuEntry_t *
k_dlurPuEntry_get(serialNum, contextInfo, nominator, searchType, dlurPuIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 dlurPuIndex;
{
   SubAgentFunc* func;
   func = snmp_extract_logical_entity(contextInfo) ? dlurCip : dlurRp;
   if (func){
       return func->t_dlurPuEntry_get(serialNum, contextInfo, nominator, searchType, dlurPuIndex);
   }
   return NULL;
}

dlurDlusEntry_t *
k_dlurDlusEntry_get(serialNum, contextInfo, nominator, searchType, dlurDlusIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 dlurDlusIndex;
{
   SubAgentFunc* func;
   func = snmp_extract_logical_entity(contextInfo) ? dlurCip : dlurRp;
   if (func){
       return func->t_dlurDlusEntry_get(serialNum, contextInfo, nominator, searchType, dlurDlusIndex);
   }
   return NULL;
}



static void
init_appndlurmib (subsystype *subsys)
{
    load_mib(appndlurmib_OidList, appndlurmib_OidListNum);
    load_oid(appndlurmib_oid_table);
}


/*
 * appndlurmib subsystem header
 */


#define MAJVERSION_appndlurmib 1
#define MINVERSION_appndlurmib 0
#define EDITVERSION_appndlurmib 0

SUBSYS_HEADER(appndlurmib,
              MAJVERSION_appndlurmib,
              MINVERSION_appndlurmib,
              EDITVERSION_appndlurmib,
              init_appndlurmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              "req: snmp");
      
