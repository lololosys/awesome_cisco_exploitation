/* $Id: sr_cdlswmib.c,v 3.11.4.10 1996/09/08 21:10:25 jlautman Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/sr_cdlswmib.c,v $
 *------------------------------------------------------------------
 * Cisco DLSw MIB File
 *
 * October 1995, Ravi Periasamy
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_cdlswmib.c,v $
 * Revision 3.11.4.10  1996/09/08  21:10:25  jlautman
 * CSCdi64868:  DLSw MIB: ciscoDlswCircuitS1IfIndex should point to
 * interface index
 * Branch: California_branch
 *
 * Revision 3.11.4.9  1996/08/28  12:41:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.4.8  1996/07/31  01:37:58  jlautman
 * CSCdi64415:  dlsw mib Active Circuits and Circut Creates
 * reversed/TConnStat wrong
 * Branch: California_branch
 *
 * Revision 3.11.4.7  1996/06/18  23:10:00  ravip
 * CSCdi60356:  DLSw MIB: Incorrent value returned for circuit timestamps
 * Branch: California_branch
 *
 * Revision 3.11.4.6  1996/06/18  22:22:04  ravip
 * CSCdi60286:  DLSw MIB: Source/Target MAC/SAP information reversed in
 * remote rtr
 * Branch: California_branch
 *
 * Revision 3.11.4.5  1996/06/07  22:10:51  mzallocc
 * CSCdi59797:  Invalid zero length OID used producing error message.
 * Branch: California_branch
 *
 * Revision 3.11.4.4  1996/05/21  09:45:43  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.4.3  1996/05/21  06:27:17  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.11.4.2  1996/05/17  10:46:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.6.6  1996/04/26  07:34:56  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.6.5  1996/04/25  19:37:56  ravip
 * CSCdi54131:  DLSw MIB ciscoDlswTConnOper Table returns 0 for stats
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.4  1996/04/24  18:35:44  fbordona
 * CSCdi55588:  DLSw: Use managed timers for connecting dynamic peers
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.3  1996/04/03  14:02:07  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.6.2  1996/03/29  23:36:06  mzallocc
 * CSCdi53049:  Router writes to low mem when snmp get on
 * ciscoDlswTConnConfig*
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.1  1996/03/17  17:38:25  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11.4.1  1996/03/18  19:32:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.3  1996/03/16  06:39:48  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.14.2  1996/03/07  08:45:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.14.1  1996/02/20  00:46:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/08  22:31:41  mzallocc
 * CSCdi49393:  Router crash on get for ciscoDlswTConn*
 *
 * Revision 3.10  1996/03/07  18:46:49  ravip
 * CSCdi49435:  Incorrect value returned on SNMP get for
 *              ciscoDlswVirtualSegmentLFSi
 *
 * Revision 3.9  1996/03/06  23:00:08  ravip
 * CSCdi49441:  Number of active circuits wrong on SNMP get of certain
 * objects
 *
 * Revision 3.8  1996/02/28  17:45:12  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.7  1996/02/01  06:01:50  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/24  22:07:48  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.5  1995/12/14  15:28:43  ravip
 * CSCdi41809:  DLSW MIB ciruit counters and OIDs always set to 0
 *
 * Revision 3.4  1995/12/04  23:41:09  ravip
 * CSCdi41809:  DLSW MIB ciruit counters and OIDs always set to 0
 *
 * Revision 3.3  1995/12/01  23:40:21  ravip
 * CSCdi41808:  Dlsw Mib does not respond to get-next correctly
 *
 * Revision 3.2  1995/11/17  09:04:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:25:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/05  19:46:48  ravip
 * Placeholder for Cisco DLSw MIB Files
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
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_cdlswmib.h"
#include "sr_cdlswmib-mib.h"
#include "snmp_interface.h"

#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include "config.h"
#include "parser.h"
#include "../cls/dlc_registry.h"
#include "../ip/ip.h"
#include "../ui/debug.h"
#include "../util/fsm.h"
#include "../if/tring.h"
#include "../if/rif_util.h"
#include "../llc2/llc2.h"
#include "../srt/rsrb.h"
#include "../cls/clsi_msg.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ibm/netbios.h"
#include "../ibm/libibm_externs.h"
#include "../snmp/ifmibapi.h"

#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_core_fsm.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_csm_db.h"
#include "../dlsw/dlsw_csm.h"
#include "../dlsw/dlsw_debug.h"
#include "../dlsw/dlsw_dlu.h"


/*
 * MIB related constants
 */
#define DLSW_CIRCUIT_KEYSIZE   ((IEEEBYTES * 2) + 2)

/* lf sizes list */
#define     MAX_LFSIZE_LIST        63

static const uint  dlsw_std_lfsizes[] = {
            65535, 59551, 56559, 53567, 50575, 47583,
            44591, 41600, 38618, 32655, 29674,
            26693, 23711, 20730, 17749, 16956,
            16163, 15370, 14578, 13785, 12992,
            12199, 11407, 10997, 10587, 10178,
            9768, 9358, 8949, 8539, 8130, 7663,
            7197, 6730, 6264, 5798, 5331, 4865,
            4399, 4105, 3812, 3518, 3225, 2932,
            2638, 2345, 2052, 1979, 1906, 1833,
            1761, 1688, 1615, 1542, 1470, 1350,
            1231, 1112, 993, 873, 754, 635, 516
    };


/* functions */
static long get_pacing_support (void);
static dlsw_ring_t *get_next_port(long ifIndex, int searchType, long *cur_ifindex);
static dlsw_t *get_next_circuit(OctetString *ciscoDlswCircuitS1Mac,
                         OctetString *ciscoDlswCircuitS1Sap,
                         OctetString *ciscoDlswCircuitS2Mac,
                         OctetString *ciscoDlswCircuitS2Sap,
                         int searchType);
static uchar lpeer_get_dlsw_version_number (void);
static uchar lpeer_get_dlsw_release_number (void);
static uchar *lpeer_get_dlsw_vendor_id (void);
static char *lpeer_get_dlsw_version_string (void);
static peerentry *peer_get_next_config (int peer_index, int searchType);
static peerentry *peer_get_next_oper (OID *ciscoDlswTConnOperTDomain,
                               OctetString *ciscoDlswTConnOperRemoteTAddr,
                               int searchType);
static unsigned long get_peer_config_time (peerentry *peer);
static unsigned long get_peer_connect_time (peerentry *peer);
static unsigned long get_peer_disc_time (peerentry *peer);
static unsigned long get_dlsw_uptime (void);
static unsigned long syststamp_to_dlswtstamp (sys_timestamp dlsw_tstamp);
static int dlsw_to_mib_ckt_state (int dlsw_ckt_state);
static unsigned int get_peer_config_setup_type(peerentry *peer);


/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswNode family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswNode_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    ciscoDlswNode_t *data = NULL;


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
     if ( (arg == -1) || (data = k_ciscoDlswNode_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswVersion
    case I_ciscoDlswVersion:
       dp = (void *) (CloneOctetString(data->ciscoDlswVersion));
       break;
#endif /* I_ciscoDlswVersion */

#ifdef I_ciscoDlswVendorID
    case I_ciscoDlswVendorID:
       dp = (void *) (CloneOctetString(data->ciscoDlswVendorID));
       break;
#endif /* I_ciscoDlswVendorID */

#ifdef I_ciscoDlswVersionString
    case I_ciscoDlswVersionString:
       dp = (void *) (CloneOctetString(data->ciscoDlswVersionString));
       break;
#endif /* I_ciscoDlswVersionString */

#ifdef I_ciscoDlswStdPacingSupport
    case I_ciscoDlswStdPacingSupport:
       dp = (void *) (&data->ciscoDlswStdPacingSupport);
       break;
#endif /* I_ciscoDlswStdPacingSupport */

#ifdef I_ciscoDlswStatus
    case I_ciscoDlswStatus:
       dp = (void *) (&data->ciscoDlswStatus);
       break;
#endif /* I_ciscoDlswStatus */

#ifdef I_ciscoDlswUpTime
    case I_ciscoDlswUpTime:
       dp = (void *) (&data->ciscoDlswUpTime);
       break;
#endif /* I_ciscoDlswUpTime */

#ifdef I_ciscoDlswVirtualSegmentLFSize
    case I_ciscoDlswVirtualSegmentLFSize:
       dp = (void *) (&data->ciscoDlswVirtualSegmentLFSize);
       break;
#endif /* I_ciscoDlswVirtualSegmentLFSize */

#ifdef I_ciscoDlswResourceNBExclusivity
    case I_ciscoDlswResourceNBExclusivity:
       dp = (void *) (&data->ciscoDlswResourceNBExclusivity);
       break;
#endif /* I_ciscoDlswResourceNBExclusivity */

#ifdef I_ciscoDlswResourceMacExclusivity
    case I_ciscoDlswResourceMacExclusivity:
       dp = (void *) (&data->ciscoDlswResourceMacExclusivity);
       break;
#endif /* I_ciscoDlswResourceMacExclusivity */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswTrapControl family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswTrapControl_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    ciscoDlswTrapControl_t *data = NULL;


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
     if ( (arg == -1) || (data = k_ciscoDlswTrapControl_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswTrapCntlTConnPartnerReject
    case I_ciscoDlswTrapCntlTConnPartnerReject:
       dp = (void *) (&data->ciscoDlswTrapCntlTConnPartnerReject);
       break;
#endif /* I_ciscoDlswTrapCntlTConnPartnerReject */

#ifdef I_ciscoDlswTrapCntlTConnProtViolation
    case I_ciscoDlswTrapCntlTConnProtViolation:
       dp = (void *) (&data->ciscoDlswTrapCntlTConnProtViolation);
       break;
#endif /* I_ciscoDlswTrapCntlTConnProtViolation */

#ifdef I_ciscoDlswTrapCntlTConn
    case I_ciscoDlswTrapCntlTConn:
       dp = (void *) (&data->ciscoDlswTrapCntlTConn);
       break;
#endif /* I_ciscoDlswTrapCntlTConn */

#ifdef I_ciscoDlswTrapCntlCircuit
    case I_ciscoDlswTrapCntlCircuit:
       dp = (void *) (&data->ciscoDlswTrapCntlCircuit);
       break;
#endif /* I_ciscoDlswTrapCntlCircuit */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswTConnStat family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswTConnStat_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    ciscoDlswTConnStat_t *data = NULL;


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
     if ( (arg == -1) || (data = k_ciscoDlswTConnStat_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswTConnStatActiveConnections
    case I_ciscoDlswTConnStatActiveConnections:
       dp = (void *) (&data->ciscoDlswTConnStatActiveConnections);
       break;
#endif /* I_ciscoDlswTConnStatActiveConnections */

#ifdef I_ciscoDlswTConnStatCloseIdles
    case I_ciscoDlswTConnStatCloseIdles:
       dp = (void *) (&data->ciscoDlswTConnStatCloseIdles);
       break;
#endif /* I_ciscoDlswTConnStatCloseIdles */

#ifdef I_ciscoDlswTConnStatCloseBusys
    case I_ciscoDlswTConnStatCloseBusys:
       dp = (void *) (&data->ciscoDlswTConnStatCloseBusys);
       break;
#endif /* I_ciscoDlswTConnStatCloseBusys */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswTConnConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswTConnConfigEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    ciscoDlswTConnConfigEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ciscoDlswTConnConfigIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &ciscoDlswTConnConfigIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        ciscoDlswTConnConfigIndex = MAX(1, ciscoDlswTConnConfigIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoDlswTConnConfigEntry_get(serialNum, contextInfo, arg ,searchType, ciscoDlswTConnConfigIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->ciscoDlswTConnConfigIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswTConnConfigTDomain
    case I_ciscoDlswTConnConfigTDomain:
       dp = (void *) (CloneOID(data->ciscoDlswTConnConfigTDomain));
       break;
#endif /* I_ciscoDlswTConnConfigTDomain */

#ifdef I_ciscoDlswTConnConfigLocalTAddr
    case I_ciscoDlswTConnConfigLocalTAddr:
       dp = (void *) (CloneOctetString(data->ciscoDlswTConnConfigLocalTAddr));
       break;
#endif /* I_ciscoDlswTConnConfigLocalTAddr */

#ifdef I_ciscoDlswTConnConfigRemoteTAddr
    case I_ciscoDlswTConnConfigRemoteTAddr:
       dp = (void *) (CloneOctetString(data->ciscoDlswTConnConfigRemoteTAddr));
       break;
#endif /* I_ciscoDlswTConnConfigRemoteTAddr */

#ifdef I_ciscoDlswTConnConfigLastModifyTime
    case I_ciscoDlswTConnConfigLastModifyTime:
       dp = (void *) (&data->ciscoDlswTConnConfigLastModifyTime);
       break;
#endif /* I_ciscoDlswTConnConfigLastModifyTime */

#ifdef I_ciscoDlswTConnConfigEntryType
    case I_ciscoDlswTConnConfigEntryType:
       dp = (void *) (&data->ciscoDlswTConnConfigEntryType);
       break;
#endif /* I_ciscoDlswTConnConfigEntryType */

#ifdef I_ciscoDlswTConnConfigGroupDefinition
    case I_ciscoDlswTConnConfigGroupDefinition:
       dp = (void *) (CloneOID(data->ciscoDlswTConnConfigGroupDefinition));
       break;
#endif /* I_ciscoDlswTConnConfigGroupDefinition */

#ifdef I_ciscoDlswTConnConfigSetupType
    case I_ciscoDlswTConnConfigSetupType:
       dp = (void *) (&data->ciscoDlswTConnConfigSetupType);
       break;
#endif /* I_ciscoDlswTConnConfigSetupType */

#ifdef I_ciscoDlswTConnConfigSapList
    case I_ciscoDlswTConnConfigSapList:
       dp = (void *) (CloneOctetString(data->ciscoDlswTConnConfigSapList));
       break;
#endif /* I_ciscoDlswTConnConfigSapList */

#ifdef I_ciscoDlswTConnConfigAdvertiseMacNB
    case I_ciscoDlswTConnConfigAdvertiseMacNB:
       dp = (void *) (&data->ciscoDlswTConnConfigAdvertiseMacNB);
       break;
#endif /* I_ciscoDlswTConnConfigAdvertiseMacNB */

#ifdef I_ciscoDlswTConnConfigInitCirRecvWndw
    case I_ciscoDlswTConnConfigInitCirRecvWndw:
       dp = (void *) (&data->ciscoDlswTConnConfigInitCirRecvWndw);
       break;
#endif /* I_ciscoDlswTConnConfigInitCirRecvWndw */

#ifdef I_ciscoDlswTConnConfigOpens
    case I_ciscoDlswTConnConfigOpens:
       dp = (void *) (&data->ciscoDlswTConnConfigOpens);
       break;
#endif /* I_ciscoDlswTConnConfigOpens */

#ifdef I_ciscoDlswTConnConfigRowStatus
    case I_ciscoDlswTConnConfigRowStatus:
       dp = (void *) (&data->ciscoDlswTConnConfigRowStatus);
       break;
#endif /* I_ciscoDlswTConnConfigRowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswTConnOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswTConnOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    ciscoDlswTConnOperEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OID *           ciscoDlswTConnOperTDomain = NULL;
    int             ciscoDlswTConnOperTDomain_offset;
    OctetString *   ciscoDlswTConnOperRemoteTAddr = NULL;
    int             ciscoDlswTConnOperRemoteTAddr_offset;
    int             index;
    int             final_index;

    ciscoDlswTConnOperTDomain_offset = object->oid.length;
    ciscoDlswTConnOperRemoteTAddr_offset = ciscoDlswTConnOperTDomain_offset + GetVariableIndexLength(incoming, ciscoDlswTConnOperTDomain_offset);
    final_index = ciscoDlswTConnOperRemoteTAddr_offset + GetVariableIndexLength(incoming, ciscoDlswTConnOperRemoteTAddr_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ciscoDlswTConnOperRemoteTAddr_offset, &ciscoDlswTConnOperRemoteTAddr, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOID(incoming, ciscoDlswTConnOperTDomain_offset, &ciscoDlswTConnOperTDomain, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoDlswTConnOperEntry_get(serialNum, contextInfo, arg ,searchType, ciscoDlswTConnOperTDomain, ciscoDlswTConnOperRemoteTAddr)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->ciscoDlswTConnOperTDomain->length;
        for(i = 0; i < data->ciscoDlswTConnOperTDomain->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->ciscoDlswTConnOperTDomain->oid_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = data->ciscoDlswTConnOperRemoteTAddr->length;
        for(i = 0; i < data->ciscoDlswTConnOperRemoteTAddr->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->ciscoDlswTConnOperRemoteTAddr->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOID(ciscoDlswTConnOperTDomain);
        FreeOctetString(ciscoDlswTConnOperRemoteTAddr);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswTConnOperLocalTAddr
    case I_ciscoDlswTConnOperLocalTAddr:
       dp = (void *) (CloneOctetString(data->ciscoDlswTConnOperLocalTAddr));
       break;
#endif /* I_ciscoDlswTConnOperLocalTAddr */

#ifdef I_ciscoDlswTConnOperEntryTime
    case I_ciscoDlswTConnOperEntryTime:
       dp = (void *) (&data->ciscoDlswTConnOperEntryTime);
       break;
#endif /* I_ciscoDlswTConnOperEntryTime */

#ifdef I_ciscoDlswTConnOperConnectTime
    case I_ciscoDlswTConnOperConnectTime:
       dp = (void *) (&data->ciscoDlswTConnOperConnectTime);
       break;
#endif /* I_ciscoDlswTConnOperConnectTime */

#ifdef I_ciscoDlswTConnOperState
    case I_ciscoDlswTConnOperState:
       dp = (void *) (&data->ciscoDlswTConnOperState);
       break;
#endif /* I_ciscoDlswTConnOperState */

#ifdef I_ciscoDlswTConnOperConfigIndex
    case I_ciscoDlswTConnOperConfigIndex:
       dp = (void *) (&data->ciscoDlswTConnOperConfigIndex);
       break;
#endif /* I_ciscoDlswTConnOperConfigIndex */

#ifdef I_ciscoDlswTConnOperFlowCntlMode
    case I_ciscoDlswTConnOperFlowCntlMode:
       dp = (void *) (&data->ciscoDlswTConnOperFlowCntlMode);
       break;
#endif /* I_ciscoDlswTConnOperFlowCntlMode */

#ifdef I_ciscoDlswTConnOperPartnerVersion
    case I_ciscoDlswTConnOperPartnerVersion:
       dp = (void *) (CloneOctetString(data->ciscoDlswTConnOperPartnerVersion));
       break;
#endif /* I_ciscoDlswTConnOperPartnerVersion */

#ifdef I_ciscoDlswTConnOperPartnerVendorID
    case I_ciscoDlswTConnOperPartnerVendorID:
       dp = (void *) (CloneOctetString(data->ciscoDlswTConnOperPartnerVendorID));
       break;
#endif /* I_ciscoDlswTConnOperPartnerVendorID */

#ifdef I_ciscoDlswTConnOperPartnerVersionStr
    case I_ciscoDlswTConnOperPartnerVersionStr:
       dp = (void *) (CloneOctetString(data->ciscoDlswTConnOperPartnerVersionStr));
       break;
#endif /* I_ciscoDlswTConnOperPartnerVersionStr */

#ifdef I_ciscoDlswTConnOperPartnerInitPacingWndw
    case I_ciscoDlswTConnOperPartnerInitPacingWndw:
       dp = (void *) (&data->ciscoDlswTConnOperPartnerInitPacingWndw);
       break;
#endif /* I_ciscoDlswTConnOperPartnerInitPacingWndw */

#ifdef I_ciscoDlswTConnOperPartnerSapList
    case I_ciscoDlswTConnOperPartnerSapList:
       dp = (void *) (CloneOctetString(data->ciscoDlswTConnOperPartnerSapList));
       break;
#endif /* I_ciscoDlswTConnOperPartnerSapList */

#ifdef I_ciscoDlswTConnOperPartnerNBExcl
    case I_ciscoDlswTConnOperPartnerNBExcl:
       dp = (void *) (&data->ciscoDlswTConnOperPartnerNBExcl);
       break;
#endif /* I_ciscoDlswTConnOperPartnerNBExcl */

#ifdef I_ciscoDlswTConnOperPartnerMacExcl
    case I_ciscoDlswTConnOperPartnerMacExcl:
       dp = (void *) (&data->ciscoDlswTConnOperPartnerMacExcl);
       break;
#endif /* I_ciscoDlswTConnOperPartnerMacExcl */

#ifdef I_ciscoDlswTConnOperPartnerNBInfo
    case I_ciscoDlswTConnOperPartnerNBInfo:
       dp = (void *) (&data->ciscoDlswTConnOperPartnerNBInfo);
       break;
#endif /* I_ciscoDlswTConnOperPartnerNBInfo */

#ifdef I_ciscoDlswTConnOperPartnerMacInfo
    case I_ciscoDlswTConnOperPartnerMacInfo:
       dp = (void *) (&data->ciscoDlswTConnOperPartnerMacInfo);
       break;
#endif /* I_ciscoDlswTConnOperPartnerMacInfo */

#ifdef I_ciscoDlswTConnOperDiscTime
    case I_ciscoDlswTConnOperDiscTime:
       dp = (void *) (&data->ciscoDlswTConnOperDiscTime);
       break;
#endif /* I_ciscoDlswTConnOperDiscTime */

#ifdef I_ciscoDlswTConnOperDiscReason
    case I_ciscoDlswTConnOperDiscReason:
       dp = (void *) (&data->ciscoDlswTConnOperDiscReason);
       break;
#endif /* I_ciscoDlswTConnOperDiscReason */

#ifdef I_ciscoDlswTConnOperDiscActiveCir
    case I_ciscoDlswTConnOperDiscActiveCir:
       dp = (void *) (&data->ciscoDlswTConnOperDiscActiveCir);
       break;
#endif /* I_ciscoDlswTConnOperDiscActiveCir */

#ifdef I_ciscoDlswTConnOperInDataPkts
    case I_ciscoDlswTConnOperInDataPkts:
       dp = (void *) (&data->ciscoDlswTConnOperInDataPkts);
       break;
#endif /* I_ciscoDlswTConnOperInDataPkts */

#ifdef I_ciscoDlswTConnOperOutDataPkts
    case I_ciscoDlswTConnOperOutDataPkts:
       dp = (void *) (&data->ciscoDlswTConnOperOutDataPkts);
       break;
#endif /* I_ciscoDlswTConnOperOutDataPkts */

#ifdef I_ciscoDlswTConnOperInDataOctets
    case I_ciscoDlswTConnOperInDataOctets:
       dp = (void *) (&data->ciscoDlswTConnOperInDataOctets);
       break;
#endif /* I_ciscoDlswTConnOperInDataOctets */

#ifdef I_ciscoDlswTConnOperOutDataOctets
    case I_ciscoDlswTConnOperOutDataOctets:
       dp = (void *) (&data->ciscoDlswTConnOperOutDataOctets);
       break;
#endif /* I_ciscoDlswTConnOperOutDataOctets */

#ifdef I_ciscoDlswTConnOperInCntlPkts
    case I_ciscoDlswTConnOperInCntlPkts:
       dp = (void *) (&data->ciscoDlswTConnOperInCntlPkts);
       break;
#endif /* I_ciscoDlswTConnOperInCntlPkts */

#ifdef I_ciscoDlswTConnOperOutCntlPkts
    case I_ciscoDlswTConnOperOutCntlPkts:
       dp = (void *) (&data->ciscoDlswTConnOperOutCntlPkts);
       break;
#endif /* I_ciscoDlswTConnOperOutCntlPkts */

#ifdef I_ciscoDlswTConnOperCURexSents
    case I_ciscoDlswTConnOperCURexSents:
       dp = (void *) (&data->ciscoDlswTConnOperCURexSents);
       break;
#endif /* I_ciscoDlswTConnOperCURexSents */

#ifdef I_ciscoDlswTConnOperICRexRcvds
    case I_ciscoDlswTConnOperICRexRcvds:
       dp = (void *) (&data->ciscoDlswTConnOperICRexRcvds);
       break;
#endif /* I_ciscoDlswTConnOperICRexRcvds */

#ifdef I_ciscoDlswTConnOperCURexRcvds
    case I_ciscoDlswTConnOperCURexRcvds:
       dp = (void *) (&data->ciscoDlswTConnOperCURexRcvds);
       break;
#endif /* I_ciscoDlswTConnOperCURexRcvds */

#ifdef I_ciscoDlswTConnOperICRexSents
    case I_ciscoDlswTConnOperICRexSents:
       dp = (void *) (&data->ciscoDlswTConnOperICRexSents);
       break;
#endif /* I_ciscoDlswTConnOperICRexSents */

#ifdef I_ciscoDlswTConnOperNQexSents
    case I_ciscoDlswTConnOperNQexSents:
       dp = (void *) (&data->ciscoDlswTConnOperNQexSents);
       break;
#endif /* I_ciscoDlswTConnOperNQexSents */

#ifdef I_ciscoDlswTConnOperNRexRcvds
    case I_ciscoDlswTConnOperNRexRcvds:
       dp = (void *) (&data->ciscoDlswTConnOperNRexRcvds);
       break;
#endif /* I_ciscoDlswTConnOperNRexRcvds */

#ifdef I_ciscoDlswTConnOperNQexRcvds
    case I_ciscoDlswTConnOperNQexRcvds:
       dp = (void *) (&data->ciscoDlswTConnOperNQexRcvds);
       break;
#endif /* I_ciscoDlswTConnOperNQexRcvds */

#ifdef I_ciscoDlswTConnOperNRexSents
    case I_ciscoDlswTConnOperNRexSents:
       dp = (void *) (&data->ciscoDlswTConnOperNRexSents);
       break;
#endif /* I_ciscoDlswTConnOperNRexSents */

#ifdef I_ciscoDlswTConnOperCirCreates
    case I_ciscoDlswTConnOperCirCreates:
       dp = (void *) (&data->ciscoDlswTConnOperCirCreates);
       break;
#endif /* I_ciscoDlswTConnOperCirCreates */

#ifdef I_ciscoDlswTConnOperCircuits
    case I_ciscoDlswTConnOperCircuits:
       dp = (void *) (&data->ciscoDlswTConnOperCircuits);
       break;
#endif /* I_ciscoDlswTConnOperCircuits */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswTConnTcpConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswTConnTcpConfigEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    ciscoDlswTConnTcpConfigEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ciscoDlswTConnConfigIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &ciscoDlswTConnConfigIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        ciscoDlswTConnConfigIndex = MAX(1, ciscoDlswTConnConfigIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoDlswTConnTcpConfigEntry_get(serialNum, contextInfo, arg ,searchType, ciscoDlswTConnConfigIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->ciscoDlswTConnConfigIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswTConnTcpConfigKeepAliveInt
    case I_ciscoDlswTConnTcpConfigKeepAliveInt:
       dp = (void *) (&data->ciscoDlswTConnTcpConfigKeepAliveInt);
       break;
#endif /* I_ciscoDlswTConnTcpConfigKeepAliveInt */

#ifdef I_ciscoDlswTConnTcpConfigTcpConnections
    case I_ciscoDlswTConnTcpConfigTcpConnections:
       dp = (void *) (&data->ciscoDlswTConnTcpConfigTcpConnections);
       break;
#endif /* I_ciscoDlswTConnTcpConfigTcpConnections */

#ifdef I_ciscoDlswTConnTcpConfigMaxSegmentSize
    case I_ciscoDlswTConnTcpConfigMaxSegmentSize:
       dp = (void *) (&data->ciscoDlswTConnTcpConfigMaxSegmentSize);
       break;
#endif /* I_ciscoDlswTConnTcpConfigMaxSegmentSize */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswTConnTcpOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswTConnTcpOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    ciscoDlswTConnTcpOperEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OID *           ciscoDlswTConnOperTDomain = NULL;
    int             ciscoDlswTConnOperTDomain_offset;
    OctetString *   ciscoDlswTConnOperRemoteTAddr = NULL;
    int             ciscoDlswTConnOperRemoteTAddr_offset;
    int             index;
    int             final_index;

    ciscoDlswTConnOperTDomain_offset = object->oid.length;
    ciscoDlswTConnOperRemoteTAddr_offset = ciscoDlswTConnOperTDomain_offset + GetVariableIndexLength(incoming, ciscoDlswTConnOperTDomain_offset);
    final_index = ciscoDlswTConnOperRemoteTAddr_offset + GetVariableIndexLength(incoming, ciscoDlswTConnOperRemoteTAddr_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ciscoDlswTConnOperRemoteTAddr_offset, &ciscoDlswTConnOperRemoteTAddr, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOID(incoming, ciscoDlswTConnOperTDomain_offset, &ciscoDlswTConnOperTDomain, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoDlswTConnTcpOperEntry_get(serialNum, contextInfo, arg ,searchType, ciscoDlswTConnOperTDomain, ciscoDlswTConnOperRemoteTAddr)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->ciscoDlswTConnOperTDomain->length;
        for(i = 0; i < data->ciscoDlswTConnOperTDomain->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->ciscoDlswTConnOperTDomain->oid_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = data->ciscoDlswTConnOperRemoteTAddr->length;
        for(i = 0; i < data->ciscoDlswTConnOperRemoteTAddr->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->ciscoDlswTConnOperRemoteTAddr->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOID(ciscoDlswTConnOperTDomain);
        FreeOctetString(ciscoDlswTConnOperRemoteTAddr);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswTConnTcpOperKeepAliveInt
    case I_ciscoDlswTConnTcpOperKeepAliveInt:
       dp = (void *) (&data->ciscoDlswTConnTcpOperKeepAliveInt);
       break;
#endif /* I_ciscoDlswTConnTcpOperKeepAliveInt */

#ifdef I_ciscoDlswTConnTcpOperPrefTcpConnections
    case I_ciscoDlswTConnTcpOperPrefTcpConnections:
       dp = (void *) (&data->ciscoDlswTConnTcpOperPrefTcpConnections);
       break;
#endif /* I_ciscoDlswTConnTcpOperPrefTcpConnections */

#ifdef I_ciscoDlswTConnTcpOperTcpConnections
    case I_ciscoDlswTConnTcpOperTcpConnections:
       dp = (void *) (&data->ciscoDlswTConnTcpOperTcpConnections);
       break;
#endif /* I_ciscoDlswTConnTcpOperTcpConnections */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswIfEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswIfEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    ciscoDlswIfEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ifIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoDlswIfEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->ifIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswIfRowStatus
    case I_ciscoDlswIfRowStatus:
       dp = (void *) (&data->ciscoDlswIfRowStatus);
       break;
#endif /* I_ciscoDlswIfRowStatus */

#ifdef I_ciscoDlswIfVirtualSegment
    case I_ciscoDlswIfVirtualSegment:
       dp = (void *) (&data->ciscoDlswIfVirtualSegment);
       break;
#endif /* I_ciscoDlswIfVirtualSegment */

#ifdef I_ciscoDlswIfSapList
    case I_ciscoDlswIfSapList:
       dp = (void *) (CloneOctetString(data->ciscoDlswIfSapList));
       break;
#endif /* I_ciscoDlswIfSapList */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswCircuitStat family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswCircuitStat_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    ciscoDlswCircuitStat_t *data = NULL;


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
     if ( (arg == -1) || (data = k_ciscoDlswCircuitStat_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswActiveCircuits
    case I_ciscoDlswActiveCircuits:
       dp = (void *) (&data->ciscoDlswActiveCircuits);
       break;
#endif /* I_ciscoDlswActiveCircuits */

#ifdef I_ciscoDlswCircuitCreates
    case I_ciscoDlswCircuitCreates:
       dp = (void *) (&data->ciscoDlswCircuitCreates);
       break;
#endif /* I_ciscoDlswCircuitCreates */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoDlswCircuitEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoDlswCircuitEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    ciscoDlswCircuitEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ciscoDlswCircuitS1Mac = NULL;
    int             ciscoDlswCircuitS1Mac_offset;
    OctetString *   ciscoDlswCircuitS1Sap = NULL;
    int             ciscoDlswCircuitS1Sap_offset;
    OctetString *   ciscoDlswCircuitS2Mac = NULL;
    int             ciscoDlswCircuitS2Mac_offset;
    OctetString *   ciscoDlswCircuitS2Sap = NULL;
    int             ciscoDlswCircuitS2Sap_offset;
    int             index;
    int             final_index;

    ciscoDlswCircuitS1Mac_offset = object->oid.length;
    ciscoDlswCircuitS1Sap_offset = ciscoDlswCircuitS1Mac_offset + GetVariableIndexLength(incoming, ciscoDlswCircuitS1Mac_offset);
    ciscoDlswCircuitS2Mac_offset = ciscoDlswCircuitS1Sap_offset + 1;
    ciscoDlswCircuitS2Sap_offset = ciscoDlswCircuitS2Mac_offset + GetVariableIndexLength(incoming, ciscoDlswCircuitS2Mac_offset);
    final_index = ciscoDlswCircuitS2Sap_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToFixedOctetString(incoming, ciscoDlswCircuitS2Sap_offset, &ciscoDlswCircuitS2Sap, searchType, &carry, 1)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, ciscoDlswCircuitS2Mac_offset, &ciscoDlswCircuitS2Mac, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToFixedOctetString(incoming, ciscoDlswCircuitS1Sap_offset, &ciscoDlswCircuitS1Sap, searchType, &carry, 1)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, ciscoDlswCircuitS1Mac_offset, &ciscoDlswCircuitS1Mac, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoDlswCircuitEntry_get(serialNum, contextInfo, arg ,searchType, ciscoDlswCircuitS1Mac, ciscoDlswCircuitS1Sap, ciscoDlswCircuitS2Mac, ciscoDlswCircuitS2Sap)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->ciscoDlswCircuitS1Mac->length;
        for(i = 0; i < data->ciscoDlswCircuitS1Mac->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->ciscoDlswCircuitS1Mac->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->ciscoDlswCircuitS1Sap->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->ciscoDlswCircuitS1Sap->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = data->ciscoDlswCircuitS2Mac->length;
        for(i = 0; i < data->ciscoDlswCircuitS2Mac->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->ciscoDlswCircuitS2Mac->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->ciscoDlswCircuitS2Sap->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->ciscoDlswCircuitS2Sap->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(ciscoDlswCircuitS1Mac);
        FreeOctetString(ciscoDlswCircuitS1Sap);
        FreeOctetString(ciscoDlswCircuitS2Mac);
        FreeOctetString(ciscoDlswCircuitS2Sap);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoDlswCircuitS1IfIndex
    case I_ciscoDlswCircuitS1IfIndex:
       dp = (void *) (&data->ciscoDlswCircuitS1IfIndex);
       break;
#endif /* I_ciscoDlswCircuitS1IfIndex */

#ifdef I_ciscoDlswCircuitS1DlcType
    case I_ciscoDlswCircuitS1DlcType:
       dp = (void *) (&data->ciscoDlswCircuitS1DlcType);
       break;
#endif /* I_ciscoDlswCircuitS1DlcType */

#ifdef I_ciscoDlswCircuitS1RouteInfo
    case I_ciscoDlswCircuitS1RouteInfo:
       dp = (void *) (CloneOctetString(data->ciscoDlswCircuitS1RouteInfo));
       break;
#endif /* I_ciscoDlswCircuitS1RouteInfo */

#ifdef I_ciscoDlswCircuitS1CircuitId
    case I_ciscoDlswCircuitS1CircuitId:
       dp = (void *) (CloneOctetString(data->ciscoDlswCircuitS1CircuitId));
       break;
#endif /* I_ciscoDlswCircuitS1CircuitId */

#ifdef I_ciscoDlswCircuitS1Dlc
    case I_ciscoDlswCircuitS1Dlc:
       dp = (void *) (CloneOID(data->ciscoDlswCircuitS1Dlc));
       break;
#endif /* I_ciscoDlswCircuitS1Dlc */

#ifdef I_ciscoDlswCircuitS2Location
    case I_ciscoDlswCircuitS2Location:
       dp = (void *) (&data->ciscoDlswCircuitS2Location);
       break;
#endif /* I_ciscoDlswCircuitS2Location */

#ifdef I_ciscoDlswCircuitS2TDomain
    case I_ciscoDlswCircuitS2TDomain:
       dp = (void *) (CloneOID(data->ciscoDlswCircuitS2TDomain));
       break;
#endif /* I_ciscoDlswCircuitS2TDomain */

#ifdef I_ciscoDlswCircuitS2TAddress
    case I_ciscoDlswCircuitS2TAddress:
       dp = (void *) (CloneOctetString(data->ciscoDlswCircuitS2TAddress));
       break;
#endif /* I_ciscoDlswCircuitS2TAddress */

#ifdef I_ciscoDlswCircuitS2CircuitId
    case I_ciscoDlswCircuitS2CircuitId:
       dp = (void *) (CloneOctetString(data->ciscoDlswCircuitS2CircuitId));
       break;
#endif /* I_ciscoDlswCircuitS2CircuitId */

#ifdef I_ciscoDlswCircuitOrigin
    case I_ciscoDlswCircuitOrigin:
       dp = (void *) (&data->ciscoDlswCircuitOrigin);
       break;
#endif /* I_ciscoDlswCircuitOrigin */

#ifdef I_ciscoDlswCircuitEntryTime
    case I_ciscoDlswCircuitEntryTime:
       dp = (void *) (&data->ciscoDlswCircuitEntryTime);
       break;
#endif /* I_ciscoDlswCircuitEntryTime */

#ifdef I_ciscoDlswCircuitStateTime
    case I_ciscoDlswCircuitStateTime:
       dp = (void *) (&data->ciscoDlswCircuitStateTime);
       break;
#endif /* I_ciscoDlswCircuitStateTime */

#ifdef I_ciscoDlswCircuitState
    case I_ciscoDlswCircuitState:
       dp = (void *) (&data->ciscoDlswCircuitState);
       break;
#endif /* I_ciscoDlswCircuitState */

#ifdef I_ciscoDlswCircuitPriority
    case I_ciscoDlswCircuitPriority:
       dp = (void *) (&data->ciscoDlswCircuitPriority);
       break;
#endif /* I_ciscoDlswCircuitPriority */

#ifdef I_ciscoDlswCircuitFCSendGrantedUnits
    case I_ciscoDlswCircuitFCSendGrantedUnits:
       dp = (void *) (&data->ciscoDlswCircuitFCSendGrantedUnits);
       break;
#endif /* I_ciscoDlswCircuitFCSendGrantedUnits */

#ifdef I_ciscoDlswCircuitFCSendCurrentWndw
    case I_ciscoDlswCircuitFCSendCurrentWndw:
       dp = (void *) (&data->ciscoDlswCircuitFCSendCurrentWndw);
       break;
#endif /* I_ciscoDlswCircuitFCSendCurrentWndw */

#ifdef I_ciscoDlswCircuitFCRecvGrantedUnits
    case I_ciscoDlswCircuitFCRecvGrantedUnits:
       dp = (void *) (&data->ciscoDlswCircuitFCRecvGrantedUnits);
       break;
#endif /* I_ciscoDlswCircuitFCRecvGrantedUnits */

#ifdef I_ciscoDlswCircuitFCRecvCurrentWndw
    case I_ciscoDlswCircuitFCRecvCurrentWndw:
       dp = (void *) (&data->ciscoDlswCircuitFCRecvCurrentWndw);
       break;
#endif /* I_ciscoDlswCircuitFCRecvCurrentWndw */

#ifdef I_ciscoDlswCircuitFCLargestRecvGranted
    case I_ciscoDlswCircuitFCLargestRecvGranted:
       dp = (void *) (&data->ciscoDlswCircuitFCLargestRecvGranted);
       break;
#endif /* I_ciscoDlswCircuitFCLargestRecvGranted */

#ifdef I_ciscoDlswCircuitFCLargestSendGranted
    case I_ciscoDlswCircuitFCLargestSendGranted:
       dp = (void *) (&data->ciscoDlswCircuitFCLargestSendGranted);
       break;
#endif /* I_ciscoDlswCircuitFCLargestSendGranted */

#ifdef I_ciscoDlswCircuitFCHalveWndwSents
    case I_ciscoDlswCircuitFCHalveWndwSents:
       dp = (void *) (&data->ciscoDlswCircuitFCHalveWndwSents);
       break;
#endif /* I_ciscoDlswCircuitFCHalveWndwSents */

#ifdef I_ciscoDlswCircuitFCResetOpSents
    case I_ciscoDlswCircuitFCResetOpSents:
       dp = (void *) (&data->ciscoDlswCircuitFCResetOpSents);
       break;
#endif /* I_ciscoDlswCircuitFCResetOpSents */

#ifdef I_ciscoDlswCircuitFCHalveWndwRcvds
    case I_ciscoDlswCircuitFCHalveWndwRcvds:
       dp = (void *) (&data->ciscoDlswCircuitFCHalveWndwRcvds);
       break;
#endif /* I_ciscoDlswCircuitFCHalveWndwRcvds */

#ifdef I_ciscoDlswCircuitFCResetOpRcvds
    case I_ciscoDlswCircuitFCResetOpRcvds:
       dp = (void *) (&data->ciscoDlswCircuitFCResetOpRcvds);
       break;
#endif /* I_ciscoDlswCircuitFCResetOpRcvds */

#ifdef I_ciscoDlswCircuitDiscReasonLocal
    case I_ciscoDlswCircuitDiscReasonLocal:
       dp = (void *) (&data->ciscoDlswCircuitDiscReasonLocal);
       break;
#endif /* I_ciscoDlswCircuitDiscReasonLocal */

#ifdef I_ciscoDlswCircuitDiscReasonRemote
    case I_ciscoDlswCircuitDiscReasonRemote:
       dp = (void *) (&data->ciscoDlswCircuitDiscReasonRemote);
       break;
#endif /* I_ciscoDlswCircuitDiscReasonRemote */

#ifdef I_ciscoDlswCircuitDiscReasonRemoteData
    case I_ciscoDlswCircuitDiscReasonRemoteData:
       dp = (void *) (CloneOctetString(data->ciscoDlswCircuitDiscReasonRemoteData));
       break;
#endif /* I_ciscoDlswCircuitDiscReasonRemoteData */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */

ciscoDlswNode_t *
k_ciscoDlswNode_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   static ciscoDlswNode_t ciscoDlswNodeData;
   static uchar  cisco_dlsw_ver[2];
   static OctetString ciscoDlswVersionOS;
   static OctetString ciscoDlswVendorIDOS;
   static OctetString ciscoDlswVersionStringOS;

   char *vendorId, *version;
   int len;
   
   uint   box_max_lfsize = 516;
   int    ii;

   if (!(reg_invoke_dlsw_running(0, 0, 0))) {
      return(NULL);
   }

   cisco_dlsw_ver[0] = lpeer_get_dlsw_version_number();
   cisco_dlsw_ver[1] = lpeer_get_dlsw_release_number();
   ciscoDlswVersionOS.octet_ptr = cisco_dlsw_ver;
   ciscoDlswVersionOS.length = sizeof(cisco_dlsw_ver);
   ciscoDlswNodeData.ciscoDlswVersion = &ciscoDlswVersionOS;

   vendorId = lpeer_get_dlsw_vendor_id();
   len = vendorId ? SNAP_OUIBYTES : 0;
   ciscoDlswVendorIDOS.octet_ptr = vendorId;
   ciscoDlswVendorIDOS.length = len;
   ciscoDlswNodeData.ciscoDlswVendorID = &ciscoDlswVendorIDOS;

   version = lpeer_get_dlsw_version_string();
   len = version ? strlen(version) : 0;
   ciscoDlswVersionStringOS.octet_ptr = version;
   ciscoDlswVersionStringOS.length = len;
   ciscoDlswNodeData.ciscoDlswVersionString = &ciscoDlswVersionStringOS;

   ciscoDlswVersionStringOS.octet_ptr = lpeer_get_dlsw_version_string();
   ciscoDlswVersionStringOS.length =
                    strlen(ciscoDlswVersionStringOS.octet_ptr);
   ciscoDlswNodeData.ciscoDlswVersionString = &ciscoDlswVersionStringOS;

   ciscoDlswNodeData.ciscoDlswStdPacingSupport = get_pacing_support();
   if (reg_invoke_dlsw_running(0, 0, 0)) {
       ciscoDlswNodeData.ciscoDlswStatus = D_ciscoDlswStatus_active;
       ciscoDlswNodeData.ciscoDlswUpTime = get_dlsw_uptime();
   } else {
       ciscoDlswNodeData.ciscoDlswStatus = D_ciscoDlswStatus_inactive;
       ciscoDlswNodeData.ciscoDlswUpTime = 0;
   }

   for(ii = 0; ii < MAX_LFSIZE_LIST; ii++) {
      if (dlsw_std_lfsizes[ii] <= BOX_MTU_SIZE) {
         box_max_lfsize = dlsw_std_lfsizes[ii];
         break;
      }
   }
   ciscoDlswNodeData.ciscoDlswVirtualSegmentLFSize = box_max_lfsize;
   ciscoDlswNodeData.ciscoDlswResourceNBExclusivity = dlsw_is_local_nb_excl();
   ciscoDlswNodeData.ciscoDlswResourceMacExclusivity = dlsw_is_local_mac_excl();
   return(&ciscoDlswNodeData);
}

ciscoDlswTrapControl_t *
k_ciscoDlswTrapControl_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
#ifdef NOT_YET
   static ciscoDlswTrapControl_t ciscoDlswTrapControlData;

   /*
    * put your code to retrieve the information here
    */

   ciscoDlswTrapControlData.ciscoDlswTrapCntlTConnPartnerReject = ;
   ciscoDlswTrapControlData.ciscoDlswTrapCntlTConnProtViolation = ;
   ciscoDlswTrapControlData.ciscoDlswTrapCntlTConn = ;
   ciscoDlswTrapControlData.ciscoDlswTrapCntlCircuit = ;
   return(&ciscoDlswTrapControlData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

ciscoDlswTConnStat_t *
k_ciscoDlswTConnStat_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   static ciscoDlswTConnStat_t ciscoDlswTConnStatData;

   if (!(reg_invoke_dlsw_running(0, 0, 0))) {
      return(NULL);
   }
   ciscoDlswTConnStatData.ciscoDlswTConnStatActiveConnections = 
                                             conn_active_connection;
   ciscoDlswTConnStatData.ciscoDlswTConnStatCloseIdles = 
                                             conn_close_idle;
   ciscoDlswTConnStatData.ciscoDlswTConnStatCloseBusys = 
                                             conn_close_busy;
   return(&ciscoDlswTConnStatData);
}

static OID* GetDomain(peerentry *peer) {
  static unsigned long oid_vals[LNciscoDlswTConnTcpConfigMaxSegmentSize-1];
  static OID domainOID = 
             {LNciscoDlswTConnTcpConfigMaxSegmentSize-1, oid_vals};
      
  bcopy(IDciscoDlswTConnTcpConfigMaxSegmentSize, oid_vals,sizeof(oid_vals));
  switch (peer->xport_type) {
    case DLSW_TCP:
      break;
          
    case DLSW_FST:
      domainOID.oid_ptr[LNciscoDlswTConnTcpConfigMaxSegmentSize-2] += 1;
      break;

    case DLSW_IF:
      domainOID.oid_ptr[LNciscoDlswTConnTcpConfigMaxSegmentSize-2] += 2;
      break;

    case DLSW_IF_LAN:
      domainOID.oid_ptr[LNciscoDlswTConnTcpConfigMaxSegmentSize-2] += 3;
      break;

    case DLSW_FR:
      domainOID.oid_ptr[LNciscoDlswTConnTcpConfigMaxSegmentSize-2] += 4;
      break;
        
    default:
  };

  return &domainOID;
};

ciscoDlswTConnConfigEntry_t *
k_ciscoDlswTConnConfigEntry_get(serialNum, contextInfo, nominator, searchType, ciscoDlswTConnConfigIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ciscoDlswTConnConfigIndex;
{
   static ciscoDlswTConnConfigEntry_t ciscoDlswTConnConfigEntryData;
   static peerentry peer_info;
   peerentry *peer = NULL;
   static ipaddrtype  local_ip_addr;
   static OctetString ciscoDlswTConnConfigLocalTAddrOS;
   static OctetString ciscoDlswTConnConfigRemoteTAddrOS;
   static OctetString ciscoDlswTConnConfigSapListOS;
   static OctetString emptyOS = {NULL, 0};

   if (!(reg_invoke_dlsw_running(0, 0, 0))) {
      return(NULL);
   }
   peer = peer_get_next_config(ciscoDlswTConnConfigIndex, searchType);
   if (peer == NULL) {
       return NULL;
   }
   local_ip_addr = peer_get_my_ipaddr();
   bcopy(peer, &peer_info, sizeof(peerentry));

   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigIndex = 
     peer_info.peer_index;
   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigTDomain = 
     GetDomain(peer);
   
   ciscoDlswTConnConfigLocalTAddrOS.octet_ptr = (uchar *) &local_ip_addr;
   ciscoDlswTConnConfigLocalTAddrOS.length = sizeof(local_ip_addr);
   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigLocalTAddr = 
     &ciscoDlswTConnConfigLocalTAddrOS;

   ciscoDlswTConnConfigRemoteTAddrOS.octet_ptr = (uchar *) &peer_info.ipaddr;
   ciscoDlswTConnConfigRemoteTAddrOS.length = sizeof(peer_info.ipaddr);
   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigRemoteTAddr =
     &ciscoDlswTConnConfigRemoteTAddrOS;

   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigLastModifyTime = 
                                 get_peer_config_time(&peer_info);
   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigEntryType = 
                                 D_ciscoDlswTConnConfigEntryType_individual;
   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigGroupDefinition = 
                                 &nullOID;
   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigSetupType = 
                                 get_peer_config_setup_type(&peer_info);

   if (lpeer->cap.supp_saps) {
     ciscoDlswTConnConfigSapListOS.octet_ptr = lpeer->cap.supp_saps;
     ciscoDlswTConnConfigSapListOS.length = SAP_CV_BYTES;
     ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigSapList =
       &ciscoDlswTConnConfigSapListOS;
   } else {
     ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigSapList = &emptyOS;
   }

   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigAdvertiseMacNB = 
                                 D_ciscoDlswTConnConfigAdvertiseMacNB_true;
   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigInitCirRecvWndw = 
                                 peer->cap.initial_pacing_window;
   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigOpens = 
				 peer_info.config_opens;
   ciscoDlswTConnConfigEntryData.ciscoDlswTConnConfigRowStatus = 
                                 D_ciscoDlswTConnConfigRowStatus_active;
   return(&ciscoDlswTConnConfigEntryData);

}

ciscoDlswTConnOperEntry_t *
k_ciscoDlswTConnOperEntry_get(serialNum, contextInfo, nominator, searchType, ciscoDlswTConnOperTDomain, ciscoDlswTConnOperRemoteTAddr)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OID * ciscoDlswTConnOperTDomain;
   OctetString * ciscoDlswTConnOperRemoteTAddr;
{
   static ciscoDlswTConnOperEntry_t ciscoDlswTConnOperEntryData;
   peerentry *peer = NULL;
   static peerentry peer_info;
   static ipaddrtype local_ip_addr;
   static uchar  partner_dlsw_ver[2];
   static OctetString ciscoDlswTConnOperLocalTAddrOS;
   static OctetString ciscoDlswTConnOperRemoteTAddrOS;
   static OctetString ciscoDlswTConnOperPartnerVersionOS;
   static OctetString ciscoDlswTConnOperPartnerVendorIDOS;
   static OctetString ciscoDlswTConnOperPartnerVersionStrOS;
   static OctetString ciscoDlswTConnOperPartnerSapListOS;
   
   if (!(reg_invoke_dlsw_running(0, 0, 0))) {
      return(NULL);
   };
   peer = peer_get_next_oper(ciscoDlswTConnOperTDomain,
                             ciscoDlswTConnOperRemoteTAddr,
                             searchType);
   if (peer != NULL) {
      local_ip_addr = peer_get_my_ipaddr();
      bcopy(peer, &peer_info, sizeof(peerentry));
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperTDomain = GetDomain(peer);
         
      ciscoDlswTConnOperLocalTAddrOS.octet_ptr = (uchar *) &local_ip_addr;
      ciscoDlswTConnOperLocalTAddrOS.length = sizeof(local_ip_addr);
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperLocalTAddr =
        &ciscoDlswTConnOperLocalTAddrOS;

      ciscoDlswTConnOperRemoteTAddrOS.octet_ptr = (uchar *) &peer_info.ipaddr;
      ciscoDlswTConnOperRemoteTAddrOS.length = sizeof(peer_info.ipaddr);
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperRemoteTAddr =
         &ciscoDlswTConnOperRemoteTAddrOS;

      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperEntryTime = 
                                  get_peer_config_time(&peer_info);
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperConnectTime = 
                                  get_peer_connect_time(&peer_info);
      switch (peer_info.conn_state) {
        case PEER_WAIT_READ:
          ciscoDlswTConnOperEntryData.ciscoDlswTConnOperState = 
                                 D_ciscoDlswTConnOperState_connecting;
          break;
        case PEER_WAIT_CAP:
          ciscoDlswTConnOperEntryData.ciscoDlswTConnOperState = 
                                 D_ciscoDlswTConnOperState_initCapExchange;
          break;
        case PEER_CONN:
        case PEER_XPORT_BUSY:
          ciscoDlswTConnOperEntryData.ciscoDlswTConnOperState = 
                                 D_ciscoDlswTConnOperState_connected;
          break;
        case PEER_DISC:
        default:
          ciscoDlswTConnOperEntryData.ciscoDlswTConnOperState = 
                                 D_ciscoDlswTConnOperState_disconnected;
          break;
      } 
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperConfigIndex = 
                                 peer_info.peer_index;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperFlowCntlMode = 
                                  D_ciscoDlswTConnOperFlowCntlMode_pacing;
      partner_dlsw_ver[0] = peer_info.cap.version_number;
      partner_dlsw_ver[1] = peer_info.cap.release_number;

      ciscoDlswTConnOperPartnerVersionOS.octet_ptr = partner_dlsw_ver;
      ciscoDlswTConnOperPartnerVersionOS.length = sizeof(partner_dlsw_ver);
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerVersion =
         &ciscoDlswTConnOperPartnerVersionOS;

      ciscoDlswTConnOperPartnerVendorIDOS.octet_ptr = peer_info.cap.vendor_id;
      ciscoDlswTConnOperPartnerVendorIDOS.length = SNAP_OUIBYTES;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerVendorID =
        &ciscoDlswTConnOperPartnerVendorIDOS;

      ciscoDlswTConnOperPartnerVersionStrOS.octet_ptr = peer_info.cap.version_string;
      ciscoDlswTConnOperPartnerVersionStrOS.length = strlen(peer_info.cap.version_string);
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerVersionStr =
        &ciscoDlswTConnOperPartnerVersionStrOS;

      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerInitPacingWndw = 
                                  peer_info.cap.initial_pacing_window;

      ciscoDlswTConnOperPartnerSapListOS.octet_ptr = peer_info.cap.supp_saps;
      ciscoDlswTConnOperPartnerSapListOS.length = SAP_CV_BYTES;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerSapList =
        &ciscoDlswTConnOperPartnerSapListOS;

      if (peer_info.cap.nb_exclusive) {
         ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerNBExcl = 
                                     D_ciscoDlswTConnOperPartnerNBExcl_true;
      } else {
         ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerNBExcl = 
                                     D_ciscoDlswTConnOperPartnerNBExcl_false;
      }
      if (peer_info.cap.mac_exclusive) {
         ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerMacExcl = 
                                     D_ciscoDlswTConnOperPartnerMacExcl_true;
      } else {
         ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerMacExcl = 
                                     D_ciscoDlswTConnOperPartnerMacExcl_true;
      }
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerNBInfo = 
                                  D_ciscoDlswTConnOperPartnerNBInfo_complete;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperPartnerMacInfo = 
                                  D_ciscoDlswTConnOperPartnerMacInfo_complete;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperDiscTime = 
                                  get_peer_disc_time(&peer_info);
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperDiscReason = 
                                  D_ciscoDlswTConnOperDiscReason_other;

      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperDiscActiveCir =
                             peer_info.disc_active_circuits;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperInDataPkts =
                             peer_info.in_data_packets;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperOutDataPkts =
                             peer_info.out_data_packets;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperInDataOctets =
                             peer_info.in_data_octets;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperOutDataOctets =
                             peer_info.out_data_octets;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperInCntlPkts =
                             peer_info.in_cntl_packets;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperOutCntlPkts =
                             peer_info.out_cntl_packets;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperCURexSents =
                             peer_info.cur_ex_sents;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperICRexRcvds =
                             peer_info.icr_ex_rcvds;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperCURexRcvds =
                             peer_info.cur_ex_rcvds;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperICRexSents =
                             peer_info.icr_ex_sents;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperNQexSents =
                             peer_info.nq_ex_sents;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperNRexRcvds =
                             peer_info.nr_ex_rcvds;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperNQexRcvds =
                             peer_info.nq_ex_rcvds;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperNRexSents =
                             peer_info.nr_ex_sents;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperCirCreates =
                             peer_info.circuit_creates;
      ciscoDlswTConnOperEntryData.ciscoDlswTConnOperCircuits =
                             peer_info.num_ckts;

      return(&ciscoDlswTConnOperEntryData);
   } else {
      return(NULL);
   }

}

ciscoDlswTConnTcpConfigEntry_t *
k_ciscoDlswTConnTcpConfigEntry_get(serialNum, contextInfo, nominator, searchType, ciscoDlswTConnConfigIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ciscoDlswTConnConfigIndex;
{
#ifdef NOT_YET
   static ciscoDlswTConnTcpConfigEntry_t ciscoDlswTConnTcpConfigEntryData;

   /*
    * put your code to retrieve the information here
    */

   ciscoDlswTConnTcpConfigEntryData.ciscoDlswTConnTcpConfigKeepAliveInt = ;
   ciscoDlswTConnTcpConfigEntryData.ciscoDlswTConnTcpConfigTcpConnections = ;
   ciscoDlswTConnTcpConfigEntryData.ciscoDlswTConnTcpConfigMaxSegmentSize = ;
   ciscoDlswTConnTcpConfigEntryData.ciscoDlswTConnConfigIndex = ;
   return(&ciscoDlswTConnTcpConfigEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

ciscoDlswTConnTcpOperEntry_t *
k_ciscoDlswTConnTcpOperEntry_get(serialNum, contextInfo, nominator, searchType, ciscoDlswTConnOperTDomain, ciscoDlswTConnOperRemoteTAddr)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OID * ciscoDlswTConnOperTDomain;
   OctetString * ciscoDlswTConnOperRemoteTAddr;
{
#ifdef NOT_YET
   static ciscoDlswTConnTcpOperEntry_t ciscoDlswTConnTcpOperEntryData;

   /*
    * put your code to retrieve the information here
    */

   ciscoDlswTConnTcpOperEntryData.ciscoDlswTConnTcpOperKeepAliveInt = ;
   ciscoDlswTConnTcpOperEntryData.ciscoDlswTConnTcpOperPrefTcpConnections = ;
   ciscoDlswTConnTcpOperEntryData.ciscoDlswTConnTcpOperTcpConnections = ;
   ciscoDlswTConnTcpOperEntryData.ciscoDlswTConnOperTDomain = ;
   ciscoDlswTConnTcpOperEntryData.ciscoDlswTConnOperRemoteTAddr = ;
   return(&ciscoDlswTConnTcpOperEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

ciscoDlswIfEntry_t *
k_ciscoDlswIfEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
{
   static ciscoDlswIfEntry_t ciscoDlswIfEntryData;
   dlsw_ring_t *dlsw_ring_tmp;
   long   cur_ifindex;
   static OctetString ciscoDlswIfSapListOS;
   static uchar SapList[SAP_CV_BYTES];
   ulong ii;

   if (!(reg_invoke_dlsw_running(0, 0, 0))) {
      return(NULL);
   }

   cur_ifindex = ifIndex;

   dlsw_ring_tmp = get_next_port(ifIndex, searchType, &cur_ifindex);

   if (dlsw_ring_tmp != NULL) {
      ciscoDlswIfEntryData.ciscoDlswIfRowStatus = D_ciscoDlswIfRowStatus_active;
      ciscoDlswIfEntryData.ciscoDlswIfVirtualSegment = dlsw_ring_tmp->mtu_size;

      for (ii = 0; ii < SAP_CV_BYTES; ii++)
         SapList[ii] = ~0;
      ciscoDlswIfSapListOS.octet_ptr = SapList;
      ciscoDlswIfSapListOS.length = SAP_CV_BYTES;
      ciscoDlswIfEntryData.ciscoDlswIfSapList = &ciscoDlswIfSapListOS;

      ciscoDlswIfEntryData.ifIndex = cur_ifindex;
      return(&ciscoDlswIfEntryData);
   } else {
      return(NULL);
   }
}

ciscoDlswCircuitStat_t *
k_ciscoDlswCircuitStat_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   static ciscoDlswCircuitStat_t ciscoDlswCircuitStatData;

   if (!(reg_invoke_dlsw_running(0, 0, 0))) {
      return(NULL);
   }
   ciscoDlswCircuitStatData.ciscoDlswActiveCircuits=dlsw_get_active_circuits();
   ciscoDlswCircuitStatData.ciscoDlswCircuitCreates=dlsw_get_circuit_creates();
   return(&ciscoDlswCircuitStatData);
}

ciscoDlswCircuitEntry_t *
k_ciscoDlswCircuitEntry_get(serialNum, contextInfo, nominator, searchType, ciscoDlswCircuitS1Mac, ciscoDlswCircuitS1Sap, ciscoDlswCircuitS2Mac, ciscoDlswCircuitS2Sap)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * ciscoDlswCircuitS1Mac;
   OctetString * ciscoDlswCircuitS1Sap;
   OctetString * ciscoDlswCircuitS2Mac;
   OctetString * ciscoDlswCircuitS2Sap;
{
   static ciscoDlswCircuitEntry_t ciscoDlswCircuitEntryData;
   static dlsw_t dlsw_circuit_info;
   static peerentry peer_info;

   dlsw_t *dlsw_ptr;

   static OctetString ciscoDlswCircuitS1MacOS;
   static OctetString ciscoDlswCircuitS1SapOS;
   static OctetString ciscoDlswCircuitS1RouteInfoOS;
   static OctetString ciscoDlswCircuitS1CircuitIdOS;
   static OctetString ciscoDlswCircuitS2MacOS;
   static OctetString ciscoDlswCircuitS2SapOS;
   static OctetString ciscoDlswCircuitS2CircuitIdOS;
   static OctetString ciscoDlswCircuitS2TAddressOS;

   static OctetString emptyOS = {NULL, 0};
  
   static unsigned long oid_vals[] = { 1, 3, 6, 1, 4, 1, 9, 10, 9, 1, 6 };
   #define LNcircuitS1DlcOID (sizeof(oid_vals)/sizeof(oid_vals[0]))
   static OID circuitS1DlcOID = {LNcircuitS1DlcOID, oid_vals};

   if (!(reg_invoke_dlsw_running(0, 0, 0))) {
      return(NULL);
   }

   dlsw_ptr = get_next_circuit(ciscoDlswCircuitS1Mac,
                               ciscoDlswCircuitS1Sap,
                               ciscoDlswCircuitS2Mac,
                               ciscoDlswCircuitS2Sap,
                               searchType);
   if (dlsw_ptr == NULL) {
       return(NULL);
   }

   /* 
    * copy the dlsw_ptr info into the static structure, incase the 
    * dlsw_ptr gets freed 
    */
   bcopy (dlsw_ptr, &dlsw_circuit_info, sizeof(dlsw_t));

   if (dlsw_circuit_info.my_ckt) {
       ciscoDlswCircuitS1MacOS.octet_ptr =
                       dlsw_circuit_info.dlc_id.origin_mac_address;
       ciscoDlswCircuitS1SapOS.octet_ptr =
                       &dlsw_circuit_info.dlc_id.origin_link_sap;
       ciscoDlswCircuitS2MacOS.octet_ptr =
                       dlsw_circuit_info.dlc_id.target_mac_address;
       ciscoDlswCircuitS2SapOS.octet_ptr =
                       &dlsw_circuit_info.dlc_id.target_link_sap;
   } else {
       ciscoDlswCircuitS1MacOS.octet_ptr =
                       dlsw_circuit_info.dlc_id.target_mac_address;
       ciscoDlswCircuitS1SapOS.octet_ptr =
                       &dlsw_circuit_info.dlc_id.target_link_sap;
       ciscoDlswCircuitS2MacOS.octet_ptr =
                       dlsw_circuit_info.dlc_id.origin_mac_address;
       ciscoDlswCircuitS2SapOS.octet_ptr =
                       &dlsw_circuit_info.dlc_id.origin_link_sap;
   }
   ciscoDlswCircuitS1MacOS.length = IEEEBYTES;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitS1Mac =
                   &ciscoDlswCircuitS1MacOS;
   ciscoDlswCircuitS2MacOS.length = IEEEBYTES;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitS2Mac =
                   &ciscoDlswCircuitS2MacOS;
   ciscoDlswCircuitS1SapOS.length = 1;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitS1Sap =
                   &ciscoDlswCircuitS1SapOS;
   ciscoDlswCircuitS2SapOS.length = 1;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitS2Sap =
                   &ciscoDlswCircuitS2SapOS;

   if (dlsw_circuit_info.port_id_dgra->port_type == CLS_TBRIDGE) {
       ciscoDlswCircuitEntryData.ciscoDlswCircuitS1IfIndex = 0;
   } else {
       ciscoDlswCircuitEntryData.ciscoDlswCircuitS1IfIndex =
                   dlsw_circuit_info.port_id_dgra->swidb->hwptr->snmp_if_index;
   }

   if (dlsw_circuit_info.port_id_dgra->port_type == CLS_LLC) {
       ciscoDlswCircuitEntryData.ciscoDlswCircuitS1DlcType = 
                   D_ciscoDlswCircuitS1DlcType_llc;
   } else if (dlsw_circuit_info.port_id_dgra->port_type == CLS_SDLC) {
       ciscoDlswCircuitEntryData.ciscoDlswCircuitS1DlcType = 
                   D_ciscoDlswCircuitS1DlcType_sdlc;
   } else if (dlsw_circuit_info.port_id_dgra->port_type == CLS_QLLC) {
       ciscoDlswCircuitEntryData.ciscoDlswCircuitS1DlcType = 
                   D_ciscoDlswCircuitS1DlcType_qllc;
   } else {     /* other */
       ciscoDlswCircuitEntryData.ciscoDlswCircuitS1DlcType = 
                   D_ciscoDlswCircuitS1DlcType_other;
   }

   ciscoDlswCircuitS1RouteInfoOS.octet_ptr = dlsw_circuit_info.local_rif;
   ciscoDlswCircuitS1RouteInfoOS.length = dlsw_circuit_info.local_rif_len;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitS1RouteInfo =
                   &ciscoDlswCircuitS1RouteInfoOS;

   ciscoDlswCircuitS1CircuitIdOS.octet_ptr =
                   (uchar *) &dlsw_circuit_info.our_dlc_corr;
   ciscoDlswCircuitS1CircuitIdOS.length =
                   sizeof(dlsw_circuit_info.our_dlc_corr);
   ciscoDlswCircuitEntryData.ciscoDlswCircuitS1CircuitId =
     &ciscoDlswCircuitS1CircuitIdOS;

   /* create the oid for the dlc type.
      Note this needs to change when we add/compile ciscoDlswSdlcLsEntries
        in the dlsw mib.
      Note this code assumes the oids for other dlcs are ciscoDlswMIBObjects
      + (6 (ie ciscoDlswSdlc) - CLS_SDLC). The mib does not define the other
      dlc oids. So I am taking some liberty here. */
   oid_vals[LNcircuitS1DlcOID-1] = 6; /* ciscoDlswSdlc OBJECT IDENTIFIER ::= { ciscoDlswMIBObjects 6 }  -- SDLC */
   oid_vals[LNcircuitS1DlcOID - 1] += 
                   dlsw_ptr->port_id_dgra->port_type - CLS_SDLC;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitS1Dlc = &circuitS1DlcOID;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitS2Location = 
                   D_ciscoDlswCircuitS2Location_remote;

      bcopy((char *) dlsw_circuit_info.peer_handle, &peer_info, 
                     sizeof(peerentry));
      ciscoDlswCircuitEntryData.ciscoDlswCircuitS2TDomain = 
                   GetDomain((peerentry *) dlsw_circuit_info.peer_handle);
      ciscoDlswCircuitS2TAddressOS.octet_ptr = 
                  (uchar *) &peer_info.ipaddr;
      ciscoDlswCircuitS2TAddressOS.length = sizeof(peer_info.ipaddr);
      ciscoDlswCircuitEntryData.ciscoDlswCircuitS2TAddress =
        &ciscoDlswCircuitS2TAddressOS;

   ciscoDlswCircuitS2CircuitIdOS.octet_ptr =
                   (uchar *) &dlsw_circuit_info.their_dlc_corr;
   ciscoDlswCircuitS2CircuitIdOS.length =
                   sizeof(dlsw_circuit_info.their_dlc_corr);
   ciscoDlswCircuitEntryData.ciscoDlswCircuitS2CircuitId =
                   &ciscoDlswCircuitS2CircuitIdOS;

   if (dlsw_circuit_info.my_ckt) {
      ciscoDlswCircuitEntryData.ciscoDlswCircuitOrigin = 
                   D_ciscoDlswCircuitOrigin_s1;
   } else {
      ciscoDlswCircuitEntryData.ciscoDlswCircuitOrigin = 
                   D_ciscoDlswCircuitOrigin_s2;
   }
   ciscoDlswCircuitEntryData.ciscoDlswCircuitEntryTime = 
               syststamp_to_dlswtstamp(dlsw_circuit_info.circuit_entry_time);
   ciscoDlswCircuitEntryData.ciscoDlswCircuitStateTime = 
               syststamp_to_dlswtstamp(dlsw_circuit_info.circuit_state_time);

   ciscoDlswCircuitEntryData.ciscoDlswCircuitState = 
                   dlsw_to_mib_ckt_state(dlsw_circuit_info.state);
   ciscoDlswCircuitEntryData.ciscoDlswCircuitPriority = 
                   D_ciscoDlswCircuitPriority_unsupported;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCSendGrantedUnits = 
                   dlsw_circuit_info.fc_send_granted_units;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCSendCurrentWndw = 
                   dlsw_circuit_info.fc_send_curr_window;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCRecvGrantedUnits = 
                   dlsw_circuit_info.fc_recv_granted_units;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCRecvCurrentWndw = 
                   dlsw_circuit_info.fc_recv_curr_window;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCLargestRecvGranted = 
                   dlsw_circuit_info.fc_largest_recv_granted;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCLargestSendGranted = 
                   dlsw_circuit_info.fc_largest_send_granted;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCHalveWndwSents = 
                   dlsw_circuit_info.fc_half_window_sents;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCResetOpSents = 
                   dlsw_circuit_info.fc_reset_window_sents;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCHalveWndwRcvds = 
                   dlsw_circuit_info.fc_half_window_rcvds;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitFCResetOpRcvds = 
                   dlsw_circuit_info.fc_reset_window_rcvds;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitDiscReasonLocal = 
                   D_ciscoDlswCircuitDiscReasonLocal_endStationDiscRcvd;
   ciscoDlswCircuitEntryData.ciscoDlswCircuitDiscReasonRemote = 
                   D_ciscoDlswCircuitDiscReasonRemote_unknown;

   ciscoDlswCircuitEntryData.ciscoDlswCircuitDiscReasonRemoteData =
     &emptyOS;

   return(&ciscoDlswCircuitEntryData);
}



static void
init_cdlswmib (subsystype *subsys)
{
    load_mib(cdlswmib_OidList, cdlswmib_OidListNum);
    load_oid(cdlswmib_oid_table);
}

/*
 * cdlswmib subsystem header
 */

#define MAJVERSION_cdlswmib 1
#define MINVERSION_cdlswmib 0
#define EDITVERSION_cdlswmib 0

SUBSYS_HEADER(cdlswmib,
              MAJVERSION_cdlswmib,
              MINVERSION_cdlswmib,
              EDITVERSION_cdlswmib,
              init_cdlswmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              "req: dlsw");






/***********************************************************************
 *          Routines to access local core data structures for the mib
 ***********************************************************************/


static long get_pacing_support (void)
{

    /* Right now we support only receive window adaptive pacing */
    return(ADAPTIVE_RECV_WINDOW);

}


static boolean dlsw_port_tester (hwidbtype *idb, long* dummy)
{
    dlsw_ring_t *dlsw_ring_tmp;

    for (dlsw_ring_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
                 dlsw_ring_tmp;
                 dlsw_ring_tmp = dlsw_ring_tmp->next)  {
        if (dlsw_ring_tmp->swidb->hwptr == idb) {
            return TRUE;
        }
    }

    return FALSE;
}


static dlsw_ring_t *get_next_port(long ifIndex, int searchType, long *cur_ifindex)
{
    dlsw_ring_t *dlsw_ring_tmp;
    hwidbtype *hwidb;


    hwidb = ifmib_2indices_get(0, searchType, ifIndex,
			       dlsw_port_tester, cur_ifindex);

    if (hwidb == NULL) {
        return NULL;
    };
    
    *cur_ifindex = hwidb->snmp_if_index;

    for (dlsw_ring_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
                 dlsw_ring_tmp;
                 dlsw_ring_tmp = dlsw_ring_tmp->next)  {
        if (dlsw_ring_tmp->swidb->hwptr == hwidb) {
            return dlsw_ring_tmp;
        }
    }

    return(NULL);

}



static dlsw_t *get_next_circuit(OctetString *ciscoDlswCircuitS1Mac,
                         OctetString *ciscoDlswCircuitS1Sap,
                         OctetString *ciscoDlswCircuitS2Mac,
                         OctetString *ciscoDlswCircuitS2Sap,
                         int searchType)
{

    dlsw_t   *dlsw_ptr, *best_dlsw_ptr = NULL;
    char     dlsw_circuit_key[DLSW_CIRCUIT_KEYSIZE];   /* mac and sap */
    char     get_key[DLSW_CIRCUIT_KEYSIZE];
    char     best_key[DLSW_CIRCUIT_KEYSIZE];
    int      rc, rc1;
    int      ii;

    /* form the key */
    memset(dlsw_circuit_key, 0, DLSW_CIRCUIT_KEYSIZE);
    memset(get_key, 0, DLSW_CIRCUIT_KEYSIZE);

    bcopy(ciscoDlswCircuitS1Mac->octet_ptr, get_key, 
          ciscoDlswCircuitS1Mac->length);
    if (ciscoDlswCircuitS1Sap->length) {
        get_key[IEEEBYTES] = *ciscoDlswCircuitS1Sap->octet_ptr;
    }
    bcopy(ciscoDlswCircuitS2Mac->octet_ptr, &get_key[IEEEBYTES + 1],
          ciscoDlswCircuitS2Mac->length);
    if (ciscoDlswCircuitS2Sap->length) {
        get_key[(IEEEBYTES * 2) + 1] = *ciscoDlswCircuitS2Sap->octet_ptr;
    }

    for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
        dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
        for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
            if (dlsw_ptr->my_ckt) {
               bcopy(dlsw_ptr->dlc_id.origin_mac_address,
                  dlsw_circuit_key, 
                  IEEEBYTES);
               if (ciscoDlswCircuitS1Sap->length) {
                   dlsw_circuit_key[IEEEBYTES] = 
                                      dlsw_ptr->dlc_id.origin_link_sap;
               }
               bcopy(dlsw_ptr->dlc_id.target_mac_address,
                  &dlsw_circuit_key[IEEEBYTES + 1],
                  IEEEBYTES);
               if (ciscoDlswCircuitS2Sap->length) {
                   dlsw_circuit_key[(IEEEBYTES * 2) + 1] = 
                                      dlsw_ptr->dlc_id.target_link_sap;
               }
            } else {
               bcopy(dlsw_ptr->dlc_id.target_mac_address,
                  dlsw_circuit_key, 
                  IEEEBYTES);
               if (ciscoDlswCircuitS2Sap->length) {
                   dlsw_circuit_key[IEEEBYTES] =
                                      dlsw_ptr->dlc_id.target_link_sap;
               }
               bcopy(dlsw_ptr->dlc_id.origin_mac_address,
                  &dlsw_circuit_key[IEEEBYTES + 1],
                  IEEEBYTES);
               if (ciscoDlswCircuitS1Sap->length) {
                   dlsw_circuit_key[(IEEEBYTES * 2) + 1] = 
                                      dlsw_ptr->dlc_id.origin_link_sap;
               }
            }
            rc = memcmp(get_key, dlsw_circuit_key, DLSW_CIRCUIT_KEYSIZE);
            if (rc == 0) {
                /* best match */
                return dlsw_ptr;
            }

            if (searchType == NEXT) {
                if (rc < 0) {
                    if (best_dlsw_ptr) {
                        rc1 = memcmp(dlsw_circuit_key,
                                     best_key,
                                     DLSW_CIRCUIT_KEYSIZE);
                        if (rc1 < 0) {
                            best_dlsw_ptr = dlsw_ptr;
                            bcopy(dlsw_circuit_key, best_key, 
                                  DLSW_CIRCUIT_KEYSIZE);
                        }
                    } else {
                        best_dlsw_ptr = dlsw_ptr;
                        bcopy(dlsw_circuit_key, best_key, 
                              DLSW_CIRCUIT_KEYSIZE);
                    }
                }
            }
        }
    }

    return best_dlsw_ptr;

}



/***********************************************************************
 *          Routines to access local peer data structures for the mib
 ***********************************************************************/


static uchar lpeer_get_dlsw_version_number (void)
{

    if (lpeer) {
        return(lpeer->cap.version_number);
    } else {
        return 0;
    }

}


static uchar lpeer_get_dlsw_release_number (void)
{

    if (lpeer) {
        return(lpeer->cap.release_number);
    } else {
        return 0;
    }

}


static uchar *lpeer_get_dlsw_vendor_id (void)
{

    if (lpeer) {
        return(lpeer->cap.vendor_id);
    } else {
        return 0;
    }

}


static char *lpeer_get_dlsw_version_string (void)
{

    if (lpeer) {
        return(lpeer->cap.version_string);
    } else {
        return 0;
    }

}



static peerentry *peer_get_next_config (int peer_index, int searchType) 
{
    int       ii;
    peerentry *peer = NULL;
    peerentry *best_peer = NULL;

    /* Connected DLX peers. */
    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[ii].qhead)) {
            for (; peer; peer = peer->next) {
                if ((peer->remove) || (peer->peer_type == PROM_PEER) ||
                         (peer->peer_type == DEMAND_PEER) || 
                         (peer->peer_type == PEER_MAX_TYPE)) {
                    continue;
                }
                if (peer->peer_index == peer_index) {
                    /* best match */
                    return peer;
                } else if (peer->peer_index > peer_index) {
                    if (searchType == NEXT) {
                        if ((best_peer == NULL) || 
                                 (peer->peer_index < best_peer->peer_index)) {
                            best_peer = peer;
                        }
                    }
                }
            }
        }
    }


    /* Connected DLSw peers. */
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if ((peer->remove) || (peer->peer_type == PROM_PEER) ||
                     (peer->peer_type == DEMAND_PEER) || 
                     (peer->peer_type == PEER_MAX_TYPE)) {
                continue;
            }
            if (peer->peer_index == peer_index) {
                /* best match */
                return peer;
            } else if (peer->peer_index > peer_index) {
                if (searchType == NEXT) {
                    if ((best_peer == NULL) || 
                             (peer->peer_index < best_peer->peer_index)) {
                        best_peer = peer;
                    }
                }
            }
        }
    }


    /* Disconnected peers. */
    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if ((peer->remove) || (peer->peer_type == PROM_PEER) ||
                     (peer->peer_type == DEMAND_PEER) || 
                     (peer->peer_type == PEER_MAX_TYPE)) {
                continue;
            }
            if (peer->peer_index == peer_index) {
                /* best match */
                return peer;
            } else if (peer->peer_index > peer_index) {
                if (searchType == NEXT) {
                    if ((best_peer == NULL) || 
                             (peer->peer_index < best_peer->peer_index)) {
                        best_peer = peer;
                    }
                }
            }
        }
    }
    return (best_peer);
}



static peerentry *peer_get_next_oper (OID *ciscoDlswTConnOperTDomain,
                               OctetString *ciscoDlswTConnOperRemoteTAddr,
                               int searchType)
{
    ipaddrtype     key_ipaddr;
    peerentry      *peer = NULL;
    peerentry      *best_peer = NULL;
    int            ii;

    if (ciscoDlswTConnOperRemoteTAddr && 
        ciscoDlswTConnOperRemoteTAddr->length > 0) {
      key_ipaddr = GETLONG(ciscoDlswTConnOperRemoteTAddr->octet_ptr);
    } else {
      bzero(&key_ipaddr, sizeof(key_ipaddr));
    };
    
    /* Connected DLX peers. */
    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[ii].qhead)) {
            for (; peer; peer = peer->next) {
                if (peer->remove) {
                    continue;
                }
                if (peer->ipaddr == key_ipaddr) {
                    /* best match */
                    return peer;
                } else if (peer->ipaddr > key_ipaddr) {
                    if (searchType == NEXT) {
                        if ((best_peer == NULL) || 
                                 (peer->ipaddr < best_peer->ipaddr)) {
                            best_peer = peer;
                        }
                    }
                }
            }
        }
    }


    /* Connected DLSw peers. */
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->remove) {
                continue;
            }
            if (peer->ipaddr == key_ipaddr) {
                /* best match */
                return peer;
            } else if (peer->ipaddr > key_ipaddr) {
                if (searchType == NEXT) {
                    if ((best_peer == NULL) || 
                             (peer->ipaddr < best_peer->ipaddr)) {
                        best_peer = peer;
                    }
                }
            }
        }
    }


    /* Disconnected peers. */
    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->remove) {
                continue;
            }
            if (peer->ipaddr == key_ipaddr) {
                /* best match */
                return peer;
            } else if (peer->ipaddr > key_ipaddr) {
                if (searchType == NEXT) {
                    if ((best_peer == NULL) || 
                             (peer->ipaddr < best_peer->ipaddr)) {
                        best_peer = peer;
                    }
                }
            }
        }
    }
    return (best_peer);
}


static inline unsigned long TimeTicks(unsigned long milliseconds)
{
  /* converts milliseconds to snmp timeticks */
  #define SYSUPTIME_UNITS 100     /* Units in Hz */
  #define SYSUPTIME_SCALE (ONESEC/SYSUPTIME_UNITS) /* Units in msclock ticks */
  return (milliseconds/SYSUPTIME_SCALE) & MAXULONG;
}


static unsigned long get_dlsw_uptime ()
{
    unsigned long timeTicks = 0;
    if (TIMER_RUNNING(dlsw_start_time)) {
        timeTicks = TimeTicks(ELAPSED_TIME(dlsw_start_time));
    };
    return timeTicks;
}

static unsigned long get_peer_config_time (peerentry *peer)
{
    unsigned long timeTicks = 0;
    if (TIMER_RUNNING(peer->config_tstamp)) {
        timeTicks = get_dlsw_uptime() - 
                    TimeTicks(ELAPSED_TIME(peer->config_tstamp));
    };
    return timeTicks;
}

static unsigned long get_peer_connect_time (peerentry *peer)
{
    unsigned long timeTicks = 0;
    if (TIMER_RUNNING(peer->connect_tstamp)) {
        timeTicks = get_dlsw_uptime() - 
                    TimeTicks(ELAPSED_TIME(peer->connect_tstamp));
    };
    return timeTicks;
}

static unsigned long get_peer_disc_time (peerentry *peer)
{
    unsigned long timeTicks = 0;
    if (TIMER_RUNNING(peer->disc_tstamp)) {
        timeTicks = get_dlsw_uptime() - 
                    TimeTicks(ELAPSED_TIME(peer->disc_tstamp));
    };
    return timeTicks;
}

static unsigned long syststamp_to_dlswtstamp (sys_timestamp dlsw_tstamp)
{
    unsigned long timeTicks = 0;
    if (TIMER_RUNNING(dlsw_tstamp)) {
        timeTicks = get_dlsw_uptime() - 
                    TimeTicks(ELAPSED_TIME(dlsw_tstamp));
    };
    return timeTicks;
}


static int dlsw_to_mib_ckt_state (int dlsw_ckt_state)
{

    switch (dlsw_ckt_state) {
      case dlswst_disconnect:
        return D_ciscoDlswCircuitState_disconnected;
        break;
      case dlswst_loc_resolve:
      case dlswst_rem_resolve:
        return D_ciscoDlswCircuitState_resolvePending;
        break;
      case dlswst_ckt_start:
        return D_ciscoDlswCircuitState_circuitStart;
        break;
      case dlswst_ckt_pending:
        return D_ciscoDlswCircuitState_circuitPending;
        break;
      case dlswst_established:
        return D_ciscoDlswCircuitState_circuitEstablished;
        break;
      case dlswst_contact_pend:
        return D_ciscoDlswCircuitState_contactPending;
        break;
      case dlswst_connect_pend:
        return D_ciscoDlswCircuitState_connectPending;
        break;
      case dlswst_connected:
        return D_ciscoDlswCircuitState_connected;
        break;
      case dlswst_disc_pending:
      case dlswst_close_pend:
        return D_ciscoDlswCircuitState_disconnectPending;
        break;
      case dlswst_halt_pending:
        return D_ciscoDlswCircuitState_haltPending;
        break;
      case dlswst_halt_pend_noack:
        return D_ciscoDlswCircuitState_haltPendingNoack;
        break;
      case dlswst_ckt_restart:
        return D_ciscoDlswCircuitState_circuitRestart;
        break;
      case dlswst_restart_pend:
        return D_ciscoDlswCircuitState_restartPending;
        break;
      default:
        return 0;
        break;
    }
    return 0;

}

static unsigned int get_peer_config_setup_type(peerentry *peer)
{
   if ((lpeer->conn_mode == PASSIVE_MODE) ||
       (peer->conn_mode == PASSIVE_MODE)) {
           return D_ciscoDlswTConnConfigSetupType_passive;
   } else switch (peer->peer_type) {;
		case DYNAMIC_PEER:
       		    return D_ciscoDlswTConnConfigSetupType_activeOnDemand;
      		default: 
           	    return D_ciscoDlswTConnConfigSetupType_activePersistent;
	  }
}

      
