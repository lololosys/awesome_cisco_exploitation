/* $Id: channelmib_utils.c,v 3.2.60.3 1996/05/21 09:47:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/channelmib_utils.c,v $
 *------------------------------------------------------------------
 * channelmib_utils.c:  Utilities for cip related mibs.
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: channelmib_utils.c,v $
 * Revision 3.2.60.3  1996/05/21  09:47:37  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.60.2  1996/05/21  06:29:00  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.2.60.1  1996/03/18  19:39:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:44:59  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:15:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:59:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/17  07:31:41  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/27  20:46:13  motto
 * Code Review updates:
 *   - Make use of snmp_if_index instead of hw_if_index
 *     using snmp functions in sr_core.c.
 *   - Source line length adjustments.
 *   - Change VLAN to LAN for CIP internal LANs.
 *     This was basically renaming the MIBS,
 *     mib source files, and makefiles.
 *     This also involved changing the
 *     CIPCSNA mib references to virtual
 *     LANs on the CIP.
 *   - Fix query bug in sr_ciplanmib.c for
 *     CIP internal LAN adapters.
 * CSCdi33408:  CIP SNA SNMP MIB support
 *
 * Revision 2.1  1995/06/07  20:38:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../snmp/snmp_api.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_snmp.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "channelmib_utils.h"
#include "interface_private.h"
#include "snmp_interface.h" 
#include "../snmp/ifmib_registry.h"
#include "../snmp/snmp_registry.h"

/*
 * CIP snmpidb test function
 */
static boolean cip_hwidb_test ( hwidbtype *hwidb, long* user_data) {
    return (hwidb->type == IDBTYPE_CIP);
}

/*
 * Retrieve the correct CIP hwidb given an ifIndex and searchtype 
 */
hwidbtype *cip_get_hwidb (int searchType, long ifIndex) {
    return reg_invoke_ifmib_hwidb_get(MAXINT, searchType, ifIndex,
				      cip_hwidb_test, NULL);
}


void InitOctetString (OctetString *os, unsigned char *srcStr, int srcLen)
{
  if (os) {
    os->octet_ptr = srcStr;
    os->length    = srcLen;
  }
}

void index_to_path_device (long index, long *path, long *device)
{
  /* This routine splits out the path and device parts from the subchannel 
     index and returns the path and device.
  */
  *path = index >> 8;
  *device = index & 0xFF;
}

long path_device_to_index (long path, long device)
{
  /* This routine combines the path and device into a subchannel index */
  return (path << 8) + device;
}

/*
 * do_channel_trap
 * Make varBindings and generate channel trap
 */
void do_channel_trap (const OID*       vbList,
                      const OID*       entOID,
                      char*            v2TrapOID,
                      OID*             indexOID,
                      int              vbLength,
                      int              genericTrapNum,
                      int              specificTrapNum,
                      void             *dataP[] )
{
  int       vbIndex;
  int       objectIndex;
  OID       *vbObject;
  OID       enterprise = *entOID;
  VarBind   *previous = NULL;
  VarBind   *current;
  VarBind   *varBinds = NULL;

  for (vbIndex = 0; vbIndex < vbLength; vbIndex++) {  

      vbObject = CatOID((OID *) &vbList[vbIndex], &indexOID[vbIndex]);
       if (vbObject) {
           objectIndex = find_object(vbObject, EXACT);

           if (objectIndex != -1) {
               current = MakeVarBind(OidIndex[objectIndex], 
                             &indexOID[vbIndex], dataP[vbIndex]);

	       if (current != NULL) {
		  /*
		   * if this is the first VarBind, make it the head of
		   * the list, otherwise attach it to the end
		   */
		   if (varBinds == NULL)
		       varBinds = current;
		   else
		       previous->next_var = current;
		   current->next_var = NULL;
		   previous = current;
	       }
           }
           FreeOID(vbObject);
       }
       else {
           while (varBinds != NULL) {
                  current = varBinds->next_var;
                  FreeVarBind(varBinds);
                  varBinds = current;
           }
           return;
       }
  }

  /* Make the call to generate the trap!  It will free the varbinds. */
  do_trap(genericTrapNum, specificTrapNum, varBinds, &enterprise, v2TrapOID);
}

boolean chan_link_fail_traps_enabled;
/* 
 * Parse chain for chan_link_fail snmp-server host command 
 */
LINK_EXIT(cfg_snmp_host_chan_link_fail_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_chan_link_fail, cfg_snmp_host_chan_link_fail_exit, NONE,
	   OBJ(int,1), (1<<TRAP_CHAN_LINK_FAIL),
	   "channel-failures", "Allow channel link failure event traps", 
           PRIV_CONF);
LINK_POINT(cfg_snmp_host_chan_link_fail_entry, cfg_snmp_host_chan_link_fail);

/* 
 * Parse chain for channel-failures snmp-server enable traps command 
 */
LINK_EXIT(cfg_snmp_enable_chan_link_fail_exit, no_alt);
KEYWORD_OR(conf_snmp_enable_chan_link_fail, cfg_snmp_enable_chan_link_fail_exit, NONE, OBJ(int,1),
          (1<<TRAP_CHAN_LINK_FAIL), "channel-failures", "Enable SNMP channel-failures traps", PRIV_CONF);
LINK_POINT(cfg_snmp_enable_chan_link_fail_entry, conf_snmp_enable_chan_link_fail);

/* 
 * Parse chain registration array for CHAN_LINK_FAIL Trap Control
 */
static parser_extension_request chan_link_fail_chain_init_table[] = {
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname (cfg_snmp_host_chan_link_fail_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_chan_link_fail_exit) },
    { PARSE_ADD_CFG_SNMP_ENABLE_CMD, &pname(cfg_snmp_enable_chan_link_fail_entry) },
    { PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
      (dynamic_transition *) &pname(cfg_snmp_enable_chan_link_fail_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * nvgen for CHAN_LINK_FAIL snmp-server host command
 */
static void chan_link_fail_snmp_host_nvgen (ulong flags) 
{
    nv_add(flags & (1 << TRAP_CHAN_LINK_FAIL), " channel-failures");
}

/*
 * cfg  CHAN_LINK_FAIL snmp-server enable traps command
 */ 
static void chan_link_fail_traps_cfg_set (boolean enable, uint subsys, uint traps) 
{
    if ((subsys & (1 << TRAP_CHAN_LINK_FAIL))) {
        chan_link_fail_traps_enabled = enable;
    }
}

/*
 * nv_write for CHAN_LINK_FAIL snmp-server enable traps command
 */
static void chan_link_fail_traps_cfg_nvwrt (parseinfo *csb) 
{
    nv_write(chan_link_fail_traps_enabled, "%s traps channel-failures", csb->nv_command);
}




/*
 * init_chanmib_utils
 * chan mib utilities subsys initialization
 */ 
static void init_chanmib_utils (subsystype *subsys)
{
    chan_link_fail_traps_enabled = FALSE;
    parser_add_command_list(chan_link_fail_chain_init_table, "chan_link_fail");
    reg_add_snmp_host_nvgen(chan_link_fail_snmp_host_nvgen, "chan_link_fail_snmp_host_nvgen");
    reg_add_Trap_cfg_set(chan_link_fail_traps_cfg_set, "chan_link_fail_traps_cfg_set");
    reg_add_Trap_cfg_nvwrt(chan_link_fail_traps_cfg_nvwrt, "chan_link_fail_traps_cfg_nvwrt");
}


/*
 * snallcmib subsystem header
 */
#define MAJVERSION_channelmib_utils 1
#define MINVERSION_channelmib_utils 0
#define EDITVERSION_channelmib_utils 0

SUBSYS_HEADER(channelmib_utils,
              MAJVERSION_channelmib_utils,
              MINVERSION_channelmib_utils,
              EDITVERSION_channelmib_utils,
              init_chanmib_utils,
              SUBSYS_CLASS_MANAGEMENT,
              "req: snmp cip", NULL);    
































