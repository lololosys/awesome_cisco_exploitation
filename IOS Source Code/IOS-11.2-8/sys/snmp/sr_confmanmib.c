/* $Id: sr_confmanmib.c,v 3.8.8.1 1996/05/21 10:02:23 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_confmanmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * May 1995, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_confmanmib.c,v $
 * Revision 3.8.8.1  1996/05/21  10:02:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/02/28  17:22:27  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.7  1996/02/22  14:37:56  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/01/29  07:29:50  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/24  21:18:47  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.4  1995/12/17  18:37:13  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/18  00:21:40  jjohnson
 * CSCdi43548:  need mechanism to enable multiple traps for a subsystem
 *
 * Revision 3.2  1995/11/17  18:59:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/15  22:40:11  thille
 * Add placeholders for Bob's config history feature and MIB.
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
#include "snmp_api.h"
#include "snmp_registry.h"
#include "sr_confmanmib.h"
#include "sr_confmanmib-mib.h"
#include "config_history.h"

#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_snmp.h"

/* History Trap Control */

boolean confman_traps_enabled;

/* Internal Constants */

#define CONFIG_TRAP_NUMBER 1
#define TRAP_VARBIND_COUNT 3

static const OID trapOID = 
{LNciscoConfigManEvent, (ulong *)IDciscoConfigManEvent};

static const OID enterpriseOID =
{LNciscoConfigManEvent - 2, (ulong *)IDciscoConfigManEvent};

static const OID trap_varbinds[TRAP_VARBIND_COUNT] = {
    {LNccmHistoryEventCommandSource, (ulong *)IDccmHistoryEventCommandSource},
    {LNccmHistoryEventConfigSource, (ulong *)IDccmHistoryEventConfigSource},
    {LNccmHistoryEventConfigDestination,
	 (ulong *)IDccmHistoryEventConfigDestination}
};

static char trapOID_str[80];


/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the ccmHistory family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *ccmHistory_get (OID         *incoming,
			 ObjectInfo  *object,
			 int          searchType,
			 ContextInfo *contextInfo,
			 int          serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    ccmHistory_t    *data = NULL;


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
     if ( (arg == -1) || (data = k_ccmHistory_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ccmHistoryRunningLastChanged
    case I_ccmHistoryRunningLastChanged:
       dp = (void *) (&data->ccmHistoryRunningLastChanged);
       break;
#endif /* I_ccmHistoryRunningLastChanged */

#ifdef I_ccmHistoryRunningLastSaved
    case I_ccmHistoryRunningLastSaved:
       dp = (void *) (&data->ccmHistoryRunningLastSaved);
       break;
#endif /* I_ccmHistoryRunningLastSaved */

#ifdef I_ccmHistoryStartupLastChanged
    case I_ccmHistoryStartupLastChanged:
       dp = (void *) (&data->ccmHistoryStartupLastChanged);
       break;
#endif /* I_ccmHistoryStartupLastChanged */

#ifdef I_ccmHistoryMaxEventEntries
    case I_ccmHistoryMaxEventEntries:
       dp = (void *) (&data->ccmHistoryMaxEventEntries);
       break;
#endif /* I_ccmHistoryMaxEventEntries */

#ifdef I_ccmHistoryEventEntriesBumped
    case I_ccmHistoryEventEntriesBumped:
       dp = (void *) (&data->ccmHistoryEventEntriesBumped);
       break;
#endif /* I_ccmHistoryEventEntriesBumped */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ccmHistoryEventEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *ccmHistoryEventEntry_get (OID         *incoming,
				   ObjectInfo  *object,
				   int          searchType,
				   ContextInfo *contextInfo,
				   int          serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    ccmHistoryEventEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ccmHistoryEventIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &ccmHistoryEventIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        ccmHistoryEventIndex = MAX(1, ccmHistoryEventIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ccmHistoryEventEntry_get(serialNum, contextInfo, arg ,searchType, ccmHistoryEventIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->ccmHistoryEventIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ccmHistoryEventTime
    case I_ccmHistoryEventTime:
       dp = (void *) (&data->ccmHistoryEventTime);
       break;
#endif /* I_ccmHistoryEventTime */

#ifdef I_ccmHistoryEventCommandSource
    case I_ccmHistoryEventCommandSource:
       dp = (void *) (&data->ccmHistoryEventCommandSource);
       break;
#endif /* I_ccmHistoryEventCommandSource */

#ifdef I_ccmHistoryEventConfigSource
    case I_ccmHistoryEventConfigSource:
       dp = (void *) (&data->ccmHistoryEventConfigSource);
       break;
#endif /* I_ccmHistoryEventConfigSource */

#ifdef I_ccmHistoryEventConfigDestination
    case I_ccmHistoryEventConfigDestination:
       dp = (void *) (&data->ccmHistoryEventConfigDestination);
       break;
#endif /* I_ccmHistoryEventConfigDestination */

#ifdef I_ccmHistoryEventTerminalType
    case I_ccmHistoryEventTerminalType:
       dp = (void *) (&data->ccmHistoryEventTerminalType);
       break;
#endif /* I_ccmHistoryEventTerminalType */

#ifdef I_ccmHistoryEventTerminalNumber
    case I_ccmHistoryEventTerminalNumber:
       dp = (void *) (&data->ccmHistoryEventTerminalNumber);
       break;
#endif /* I_ccmHistoryEventTerminalNumber */

#ifdef I_ccmHistoryEventTerminalUser
    case I_ccmHistoryEventTerminalUser:
       dp = (void *) (CloneOctetString(data->ccmHistoryEventTerminalUser));
       break;
#endif /* I_ccmHistoryEventTerminalUser */

#ifdef I_ccmHistoryEventTerminalLocation
    case I_ccmHistoryEventTerminalLocation:
       dp = (void *) (CloneOctetString(data->ccmHistoryEventTerminalLocation));
       break;
#endif /* I_ccmHistoryEventTerminalLocation */

#ifdef I_ccmHistoryEventCommandSourceAddress
    case I_ccmHistoryEventCommandSourceAddress:
       dp = (void *) (IPToOctetString(data->ccmHistoryEventCommandSourceAddress));
       break;
#endif /* I_ccmHistoryEventCommandSourceAddress */

#ifdef I_ccmHistoryEventVirtualHostName
    case I_ccmHistoryEventVirtualHostName:
       dp = (void *) (CloneOctetString(data->ccmHistoryEventVirtualHostName));
       break;
#endif /* I_ccmHistoryEventVirtualHostName */

#ifdef I_ccmHistoryEventServerAddress
    case I_ccmHistoryEventServerAddress:
       dp = (void *) (IPToOctetString(data->ccmHistoryEventServerAddress));
       break;
#endif /* I_ccmHistoryEventServerAddress */

#ifdef I_ccmHistoryEventFile
    case I_ccmHistoryEventFile:
       dp = (void *) (CloneOctetString(data->ccmHistoryEventFile));
       break;
#endif /* I_ccmHistoryEventFile */

#ifdef I_ccmHistoryEventRcpUser
    case I_ccmHistoryEventRcpUser:
       dp = (void *) (CloneOctetString(data->ccmHistoryEventRcpUser));
       break;
#endif /* I_ccmHistoryEventRcpUser */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 **********************************************************************
 *
 * KERNEL CODE begins here.  Above is all generated by SNMP Research
 * compiler.  Below code does the real work in getting the router
 * information.  
 *
 * First come the utility helper routines.
 *
 **********************************************************************
 */

ccmHistory_t *k_ccmHistory_get (int          serialNum,
				ContextInfo *contextInfo,
				int          nominator)
{
    static ccmHistory_t ccmHistoryData;

    ccmHistoryData.ccmHistoryRunningLastChanged = running_last_changed;
    ccmHistoryData.ccmHistoryRunningLastSaved = running_last_saved;
    ccmHistoryData.ccmHistoryStartupLastChanged = startup_last_changed;
    ccmHistoryData.ccmHistoryMaxEventEntries = MAX_HISTORY;
    ccmHistoryData.ccmHistoryEventEntriesBumped = entries_bumped;
    return(&ccmHistoryData);
}

ccmHistoryEventEntry_t *k_ccmHistoryEventEntry_get (int serialNum,
						    ContextInfo *contextInfo,
						    int nominator,
						    int searchType,
						    long ccmHistoryEventIndex)
{
    static ccmHistoryEventEntry_t ccmHistoryEventEntryData;

    static OctetString     TerminalUser;
    static OctetString     TerminalLocation;
    static OctetString     VirtualHostName;
    static OctetString     File;
    static OctetString     RcpUser;

    confighistoryentry_t *entry;

    /* try to find an entry and give up if none */

    entry = config_history_find(ccmHistoryEventIndex, (searchType == EXACT));
    if (!entry)
	return(NULL);

    /* set return values from entry */

    ccmHistoryEventEntryData.ccmHistoryEventIndex = entry->serial_number;

    ccmHistoryEventEntryData.ccmHistoryEventTime = entry->timestamp;

    ccmHistoryEventEntryData.ccmHistoryEventCommandSource =
	entry->command_source;

    ccmHistoryEventEntryData.ccmHistoryEventConfigSource = entry->data_source;

    ccmHistoryEventEntryData.ccmHistoryEventConfigDestination = 
	entry->data_destination;

    ccmHistoryEventEntryData.ccmHistoryEventTerminalType = entry->terminal_type;

    ccmHistoryEventEntryData.ccmHistoryEventTerminalNumber = 
	entry->terminal_number;

    TerminalUser.octet_ptr = entry->terminal_user;
    TerminalUser.length = strlen(entry->terminal_user);
    ccmHistoryEventEntryData.ccmHistoryEventTerminalUser = &TerminalUser;

    TerminalLocation.octet_ptr = entry->terminal_location;
    TerminalLocation.length = strlen(entry->terminal_location);
    ccmHistoryEventEntryData.ccmHistoryEventTerminalLocation = 
	&TerminalLocation;

    ccmHistoryEventEntryData.ccmHistoryEventCommandSourceAddress = 
	entry->command_source_address;

    VirtualHostName.octet_ptr = entry->virtual_host_name;
    VirtualHostName.length = strlen(entry->virtual_host_name);
    ccmHistoryEventEntryData.ccmHistoryEventVirtualHostName = &VirtualHostName;

    ccmHistoryEventEntryData.ccmHistoryEventServerAddress =
	entry->server_address;

    File.octet_ptr = entry->file_name;
    File.length = strlen(entry->file_name);
    ccmHistoryEventEntryData.ccmHistoryEventFile = &File;

    RcpUser.octet_ptr = entry->rcp_user;
    RcpUser.length = strlen(entry->rcp_user);
    ccmHistoryEventEntryData.ccmHistoryEventRcpUser = &RcpUser;

    return(&ccmHistoryEventEntryData);
}


/*
 * snmp-server host command for writing to NVRAM
 */

static void config_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_CONFIG), " config");
}

/*
 * enable traps for snmp-server enable traps command
 */

static void config_trap_cfg_set (boolean enable, uint subsys, uint traps)
{
    if ((subsys & (1 << TRAP_CONFIG)))
        confman_traps_enabled = enable;
}

/*
 * snmp-server enable trap command for writing to NVRAM
 */

static void config_trap_cfg_nvwrt (parseinfo *csb)
{
    nv_write(confman_traps_enabled, "%s traps config", csb->nv_command);
}

/*
 * parser chain addition for snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_config_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_config,
	   cfg_snmp_host_config_exit, NONE,
	   OBJ(int,1), (1<<TRAP_CONFIG),
	   "config", "Allow SNMP config traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_config_entry, cfg_snmp_host_config);

/*
 * parser chain addition for snmp-server enable trap command
 */

LINK_EXIT(cfg_snmp_enable_config_exit, no_alt);
KEYWORD_OR(conf_snmp_enable_config, 
	   cfg_snmp_enable_config_exit, NONE,
           OBJ(int,1), (1<<TRAP_CONFIG),
           "config", "Enable SNMP config traps", PRIV_CONF);
LINK_POINT(cfg_snmp_enable_config_entry,
conf_snmp_enable_config);

/*
 * Parse chain registration array for config history
 */
static parser_extension_request config_chain_init_table[] = {
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_config_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_config_exit) },
    { PARSE_ADD_CFG_SNMP_ENABLE_CMD,
	  &pname(cfg_snmp_enable_config_entry) },
    { PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
        (dynamic_transition *) &pname(cfg_snmp_enable_config_exit) },
    { PARSE_LIST_END, NULL }
};

void ciscoConfigManEvent (int index)
{
    ulong       instance[1];
    int		i;
    OID	       *vbList[TRAP_VARBIND_COUNT+1];
    OID         instanceOID;

    /* Generate Trap */

    /* if trap disabled, all done */

    if (!confman_traps_enabled)
	return;

    /* create the varbind list, with the appropriate instancing */

    instance[0] = index;
    instanceOID.oid_ptr = instance;
    instanceOID.length = 1;

    for (i = 0; i < TRAP_VARBIND_COUNT; i++) {
        vbList[i] = CatOID((OID *) &trap_varbinds[i], &instanceOID);
    }
    vbList[i] = NULL;

    /* create the trap */

    snmp_trap(ENTERPRISE_TRAP, CONFIG_TRAP_NUMBER, vbList,
		 (OID *)&enterpriseOID, trapOID_str);

    /* free memory allocated by the CatOID service */

    for (i = 0; i < TRAP_VARBIND_COUNT; i++) {
	FreeOID(vbList[i]);
    }
}

static void init_confmanmib (subsystype *subsys)
{
    confman_traps_enabled = FALSE;

    load_mib(confmanmib_OidList, confmanmib_OidListNum);
    load_oid(confmanmib_oid_table);

    MakeDotFromOID((OID *)&trapOID, trapOID_str);
    register_snmp_trap(TRAP_CONFIG, trapOID_str);

    reg_add_snmp_host_nvgen(config_snmp_host_nvgen,
			    "config_snmp_host_nvgen");
    reg_add_Trap_cfg_set(config_trap_cfg_set, "config_trap_cfg_set");
    reg_add_Trap_cfg_nvwrt(config_trap_cfg_nvwrt, "config_trap_cfg_nvwrt");
    parser_add_command_list(config_chain_init_table, "config");

    reg_add_ciscoConfigManEvent(ciscoConfigManEvent, "ciscoConfigManEvent");
}

/*
 * confmanmib subsystem header
 */

#define MAJVERSION_confmanmib 1
#define MINVERSION_confmanmib 0
#define EDITVERSION_confmanmib 0

SUBSYS_HEADER(confmanmib,
              MAJVERSION_confmanmib,
              MINVERSION_confmanmib,
              EDITVERSION_confmanmib,
              init_confmanmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              "req: config_history");
      
