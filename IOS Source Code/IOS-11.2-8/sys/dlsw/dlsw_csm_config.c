/* $Id: dlsw_csm_config.c,v 3.8.6.5 1996/08/16 06:55:49 ioakley Exp $
 *------------------------------------------------------------------
 * DLSw CSM Configuration Command Processing Routines
 *
 * August 1994, Frank Bordonaro 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Called by DLSw parser macros for DLSw CSM configuration commands
 *------------------------------------------------------------------
 * $Log: dlsw_csm_config.c,v $
 * Revision 3.8.6.5  1996/08/16  06:55:49  ioakley
 * CSCdi58903:  Add dlsw touch timer support for sna/netbios
 *              reachability cache entries.
 * Branch: California_branch
 *
 * Revision 3.8.6.4  1996/08/09  21:55:16  ravip
 * CSCdi59181:  Best path not chosen for the first circuit
 * Branch: California_branch
 *
 * Revision 3.8.6.3  1996/05/30  14:43:26  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.8.6.2  1996/05/17  10:45:06  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.4.5  1996/05/15  00:38:51  ravip
 * CSCdi50707:  dlsw the reachability search is linear, should be binary
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.4.4  1996/05/10  15:00:45  ioakley
 * CSCdi57325:  Remove dlsw reliance on port_number/slot_number
 *              and identify all dlsw ports by associated swidb.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.4.3  1996/04/26  07:33:56  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.4.2  1996/04/03  14:01:09  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.4.1  1996/03/17  17:37:51  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.8.6.1  1996/03/18  19:31:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.3  1996/03/16  06:38:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.20.2  1996/03/07  08:44:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/02/20  00:45:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/03/06  18:09:13  ioakley
 * CSCdi50102:  Prevent memlock related crashes from occurring in
 *              "show dlsw reachability" cache command processing.
 *
 * Revision 3.7  1996/02/07  16:10:57  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/02/01  06:01:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/02/01  02:56:58  fbordona
 * CSCdi47930:  DLSw session does not reconnect after peer powered off.
 *              Only attempt one peer connection at at time to avoid
 *              starving the dlsw background process.
 *
 * Revision 3.4  1995/11/29  19:44:53  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.3  1995/11/17  09:02:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:55:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/07  05:31:08  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.4  1995/06/30  18:52:21  widmer
 * CSCdi36635:  nv_add string should not contain newline
 * Fix dlsw NV generation
 *
 * Revision 2.3  1995/06/15  15:54:25  ravip
 * CSCdi35890:  dlsw allroute-* command broken
 *              print nv_command instead of the string
 *              check for dlsw_peer_active for local peer instead
 *                    of checking dlsw_running before nv_add
 *
 * Revision 2.2  1995/06/13  17:48:28  ravip
 * CSCdi35261:  DLSw should permit ARB from target
 *              Nerd knob to control ARB and SRB of SNA and Netbios frames
 *              Store test frames if search is pending for the dmac
 *
 * Revision 2.1  1995/06/07  20:26:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../util/fsm.h"
#include "../if/rif_util.h"
#include "../llc2/llc2.h"
#include "../cls/clsi_msg.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ibm/netbios.h"
#include "../ibm/libibm_externs.h"

#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_csm_db.h"
#include "../dlsw/dlsw_csm.h"
#include "../dlsw/dlsw_debug.h"


/*
 * Externs
 */
extern boolean  dlsw_started;

/*
 * File Scope function prototypes
 */
static void csm_nvgen_timers (parseinfo *csb);
static void csm_nvgen_touch_timers (parseinfo *csb);
static void csm_nvgen_statics (parseinfo *csb,
                               db_master_t *lcache,
                               db_master_t *rcache);
static void csm_parse_timers (parseinfo *csb);
static void csm_parse_touch_timers (parseinfo *csb);
static void csm_parse_statics (parseinfo *csb);
static char *csm_cache_status_to_string (int status);
static char *csm_cache_direction_to_string (int reach_direction);

void csm_show_mac_cache (void);
void csm_show_name_cache (void);
void csm_show_cache (int, char *, int);


/**********************************************************************
 *
 * CSM configuration commands.
 *
 *********************************************************************/

/*
 * dlsw_csm_command()
 *
 * global dlsw csm commands.
 */

void dlsw_csm_command (parseinfo *csb)
{
    if (system_loading)
        return;

    if (!bridge_enable) {
        printf("\n%%DLSw not supported in this release");
        return;
    }

    if (csb == NULL) {
        printf("\nDLSw: parseinfo NULL pointer");
	return;
    }
    if (!(dlsw_peer_active(0, 0, 0)))
      return;
    if (csb->nvgen) {
        switch (csb->which) {
          case CSM_MAC_ADDR:
            csm_nvgen_statics(csb, LCACHE_MAC, RCACHE_MAC);
            break;

          case CSM_NETBIOS_NAME:
            csm_nvgen_statics(csb, LCACHE_NAME, RCACHE_NAME);
            break;

          case CSM_DUP_PATH:
            nv_write(csm_load_balance, "%s", csb->nv_command);
            break;

          case CSM_EXPLQ_DEPTH:
            nv_write(((dlsw_ctlQ.maximum != CSM_DEF_EXPLQ_DEPTH) &&
		      (dlsw_ctlQ.maximum != 0)),
		     "%s %d", csb->nv_command, dlsw_ctlQ.maximum);
            break;

          case CSM_TIMERS:
            csm_nvgen_timers(csb);
            break;

	  case CSM_TOUCH_TIMERS:
	    csm_nvgen_touch_timers(csb);
            break;

          case CSM_ALLROUTE_SNA:
            nv_write((dlsw_sna_xcast_flag != CSM_DEF_SNA_XCAST_FLAG),
		     "%s", csb->nv_command);
            break;

          case CSM_ALLROUTE_NETBIOS:
            nv_write((dlsw_nb_xcast_flag != CSM_DEF_NB_XCAST_FLAG),
		     "%s", csb->nv_command);
            break;

          default:
            bad_parser_subcommand(csb, csb->which);
            break;

        }
        return;
    }

    switch (csb->which) {
      case CSM_MAC_ADDR:
      case CSM_NETBIOS_NAME:
        csm_parse_statics(csb);
        break;

      case CSM_DUP_PATH:
        csm_load_balance = csb->sense;
        break;

      case CSM_EXPLQ_DEPTH:
        if (csb->sense)
            dlsw_ctlQ.maximum = GETOBJ(int,1);
        else
            dlsw_ctlQ.maximum = CSM_DEF_EXPLQ_DEPTH;
        break;

      case CSM_TIMERS:
        csm_parse_timers(csb);
        break;

      case CSM_TOUCH_TIMERS:
	csm_parse_touch_timers(csb);
	break;

      case CSM_ALLROUTE_SNA:
        if (csb->sense) {
            dlsw_sna_xcast_flag = RC_BROADBIT;
        } else {
            dlsw_sna_xcast_flag = CSM_DEF_SNA_XCAST_FLAG;
        }
        break;

      case CSM_ALLROUTE_NETBIOS:
        if (csb->sense) {
            dlsw_nb_xcast_flag = RC_BROADBIT;
        } else {
            dlsw_nb_xcast_flag = CSM_DEF_NB_XCAST_FLAG;
        }
        break;

      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}


/*
 * csm_nvgen_statics()
 */

static char *nvgen_fmt[] = {
    "%s %s remote-peer ip %i",
    "%s %e remote-peer ip %i",
    "%s %s remote-peer interface %s",
    "%s %e remote-peer interface %s",
    "%s %s ring %d",
    "%s %e ring %d",
    "%s %s rif %s",
    "%s %e rif %s"
};
    

static void csm_nvgen_statics (parseinfo *csb,
			       db_master_t *lcache,
			       db_master_t *rcache)
{
    int             ii;
    lcache_t        *cache_ptr;
    record_handle   rhandle;
    peerhandle_t    ph;
    reach_entry_t   *reach;
    uchar	    *rif_str;

    if (!dlsw_started)
      return;
    rhandle = read_next_record(rcache, NULL, NULL, (void **) &cache_ptr);
    while (rhandle != NULL) {
        if (cache_ptr->cache_type == USER_CONF) {
            for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
	        if (cache_ptr->r_entry[ii].flags != 0) {
		    if (cache_ptr->reach_direction == REMOTE) {
			ph = cache_ptr->r_entry[ii].prp_id.peer_id;
			switch (peer_handle_to_xport(ph)) {
			case DLSW_TCP:
			case DLSW_FST:
                            nv_write(csb->sense, 
				     nvgen_fmt[(rcache == RCACHE_MAC ? 1 : 0)],
				     csb->nv_command, RECORD_KEY(rhandle),
			             peer_handle_to_ip(ph));
			    break;
			case DLSW_IF:
                            nv_write(csb->sense, 
				     nvgen_fmt[(rcache == RCACHE_MAC ? 3 : 2)],
				     csb->nv_command, RECORD_KEY(rhandle),
				     peer_handle_to_int(ph));
			    break;
			default:
			    buginf("\nconfig: unknown dlsw peer xport type");
			}
                    } else {    /* LOCAL */
                        buginf("\nconfig: no command for local");
                    }
                } 
            }
        }
	rhandle = read_next_record(rcache, rhandle, NULL,
                                   (void **) &cache_ptr);
    }
    rhandle = read_next_record(lcache, NULL, NULL, (void **) &cache_ptr);
    while (rhandle != NULL) {
        if (cache_ptr->cache_type & USER_CONF) {
            for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
		reach = &cache_ptr->r_entry[ii];
	        if (reach->flags != 0) {
		    if (cache_ptr->reach_direction == LOCAL) {
			if (cache_ptr->cache_type & USER_CONF_RIF) {
			    /* store rif information */
			    rif_str = string_getnext();
			    rif_str = get_rif_str(reach->rif, rif_str);
			    nv_write(csb->sense,
				     nvgen_fmt[(lcache == LCACHE_MAC ? 7 : 6)],
				     csb->nv_command, RECORD_KEY(rhandle),
				     rif_str);
			} else {
			    /* store ring information */
			    nv_write(csb->sense,
				     nvgen_fmt[(lcache == LCACHE_MAC ? 5 : 4)],
				     csb->nv_command, RECORD_KEY(rhandle),
				     (cache_ptr->cache_type & SAP_PEND ?
					reach->prp_id.ring_id :
				        portid2ringid(reach->prp_id.port_id)));
			}
                    } else {    /* remote */
                        buginf("\nconfig: no command for remote");
                    }
                } 
            }
        }
	rhandle = read_next_record(lcache, rhandle, NULL,
                                   (void **) &cache_ptr);
    }

    printf("\n");   /* leave a blank line */
    return;
}


/*
 * csm_nvgen_timers()
 */

static void csm_nvgen_timers (parseinfo *csb)
{
    nv_write(csm_sna_expl_to != CSM_DEF_SNA_EXPL_TO,
	     "%s sna-explorer-timeout %d", csb->nv_command, 
	     (csm_sna_expl_to / ONESEC));
    nv_write(csm_sna_cache_to != CSM_DEF_SNA_CACHE_TO,
	     "%s sna-cache-timeout %d", csb->nv_command, 
	     (csm_sna_cache_to / ONESEC));
    nv_write(csm_sna_cache_verify_to != CSM_DEF_SNA_VERIFY_INT,
	     "%s sna-verify-interval %d", csb->nv_command, 
	     (csm_sna_cache_verify_to / ONESEC));
    nv_write(csm_sna_expl_rt != CSM_DEF_SNA_RETRY_INT,
	     "%s sna-retry-interval %d", csb->nv_command, 
	     (csm_sna_expl_rt / ONESEC));
    nv_write(csm_nb_expl_to != CSM_DEF_NB_EXPL_TO,
	     "%s netbios-explorer-timeout %d", csb->nv_command, 
	     (csm_nb_expl_to / ONESEC));
    nv_write(csm_nb_cache_to != CSM_DEF_NB_CACHE_TO,
	     "%s netbios-cache-timeout %d", csb->nv_command, 
	     (csm_nb_cache_to / ONESEC));
    nv_write(csm_nb_cache_verify_to != CSM_DEF_NB_VERIFY_INT,
	     "%s netbios-verify-interval %d", csb->nv_command, 
	     (csm_nb_cache_verify_to / ONESEC));
    nv_write(csm_nb_expl_rt != CSM_DEF_NB_RETRY_INT,
	     "%s netbios-retry-interval %d", csb->nv_command, 
	     (csm_nb_expl_rt / ONESEC));
    nv_write(csm_negative_cache_to != CSM_DEF_NEG_CACHE_TO,
	     "%s icannotreach-block-time %d", csb->nv_command, 
	     (csm_negative_cache_to / ONESEC));
    nv_write(csm_expl_wait_rt != CSM_DEF_EXPL_WAIT_INT,
	     "%s explorer-wait-time %d", csb->nv_command, 
	     (csm_expl_wait_rt / ONESEC));
    nv_write(csm_expl_delay != CSM_DEF_EXPL_DELAY,
             "%s explorer-delay-time %d", csb->nv_command,
             (csm_expl_delay / ONEMIN));
}

/*
 * csm_nvgen_touch_timers()
 */
static void csm_nvgen_touch_timers (parseinfo *csb)
{
    nv_write(((csm_touch_sna_cache_active_to !=
	     CSM_DEF_TOUCH_SNA_CACHE_ACTIVE_TO) &&
	     (csm_touch_sna_cache_reset_to !=
	     CSM_DEF_TOUCH_SNA_CACHE_RESET_TO)),
	     "%s touch-sna-cache-timeout %d %d", csb->nv_command,
	     (csm_touch_sna_cache_active_to / ONESEC),
	     (csm_touch_sna_cache_reset_to /ONESEC));
    nv_write(((csm_touch_nb_cache_active_to !=
	     CSM_DEF_TOUCH_NB_CACHE_ACTIVE_TO) &&
	     (csm_touch_nb_cache_reset_to !=
	     CSM_DEF_TOUCH_NB_CACHE_RESET_TO)),
	     "%s touch-nb-cache-timeout %d %d", csb->nv_command,
	     (csm_touch_nb_cache_active_to / ONESEC),
	     (csm_touch_nb_cache_reset_to /ONESEC));
}

/*
 * csm_parse_timers()
 */

static void csm_parse_timers (parseinfo *csb)
{
    switch (GETOBJ(int,1)) {
      case CSM_SNA_EXPL_T:
	if (csb->sense)
            csm_sna_expl_to = GETOBJ(int,2) * ONESEC;
	else 
            csm_sna_expl_to = CSM_DEF_SNA_EXPL_TO;
        break;

      case CSM_SNA_CACHE_T:
	if (csb->sense)
            csm_sna_cache_to = GETOBJ(int,2) * ONESEC;
        else
            csm_sna_cache_to = CSM_DEF_SNA_CACHE_TO;
        break;

      case CSM_SNA_VERIFY_T:
	if (csb->sense)
            csm_sna_cache_verify_to = GETOBJ(int,2) * ONESEC;
        else
            csm_sna_cache_verify_to = CSM_DEF_SNA_VERIFY_INT;
        break;

      case CSM_SNA_RETRY_T:
	if (csb->sense)
            csm_sna_expl_rt = GETOBJ(int,2) * ONESEC;
	else
            csm_sna_expl_rt = CSM_DEF_SNA_RETRY_INT;
        break;

      case CSM_NB_EXPL_T:
	if (csb->sense)
            csm_nb_expl_to = GETOBJ(int,2) * ONESEC;
	else
            csm_nb_expl_to = CSM_DEF_NB_EXPL_TO;
        break;

      case CSM_NB_CACHE_T:
	if (csb->sense)
            csm_nb_cache_to = GETOBJ(int,2) * ONESEC;
	else
            csm_nb_cache_to = CSM_DEF_NB_CACHE_TO;
        break;

      case CSM_NB_VERIFY_T:
	if (csb->sense)
            csm_nb_cache_verify_to = GETOBJ(int,2) * ONESEC;
	else
            csm_nb_cache_verify_to = CSM_DEF_NB_VERIFY_INT;
        break;

      case CSM_NB_RETRY_T:
	if (csb->sense)
            csm_nb_expl_rt = GETOBJ(int,2) * ONESEC;
	else
            csm_nb_expl_rt = CSM_DEF_NB_RETRY_INT;
        break;

      case CSM_NEG_CACHE_T:
	if (csb->sense)
            csm_negative_cache_to = GETOBJ(int,2) * ONESEC;
	else
            csm_negative_cache_to = CSM_DEF_NEG_CACHE_TO;
        break;

      case CSM_EXPL_WAIT_T:
        if (csb->sense)
            csm_expl_wait_rt = GETOBJ(int, 2) * ONESEC;
        else 
            csm_expl_wait_rt = CSM_DEF_EXPL_WAIT_INT;
        break;

      case CSM_EXPL_DELAY_T:
        if (csb->sense)
            csm_expl_delay = GETOBJ(int, 2) * ONEMIN;
        else
            csm_expl_delay = CSM_DEF_EXPL_DELAY;
        break;

      default:
        bad_parser_subcommand(csb, GETOBJ(int,1));
        break;
    }
}

/*
 * csm_parse_touch_timers()
 */

static void csm_parse_touch_timers (parseinfo *csb)
{
    switch (GETOBJ(int,1)) {
    case CSM_TOUCH_SNA_CACHE_T:
	if (csb->sense) {
	    csm_touch_sna_cache_active_to = GETOBJ(int,2) * ONESEC;
	    csm_touch_sna_cache_reset_to = GETOBJ(int,3) * ONESEC;
        }
        else { 
	    csm_touch_sna_cache_active_to = CSM_DEF_TOUCH_SNA_CACHE_ACTIVE_TO;
	    csm_touch_sna_cache_reset_to = CSM_DEF_TOUCH_SNA_CACHE_RESET_TO;
        }
	break;

    case CSM_TOUCH_NB_CACHE_T:
	if (csb->sense) {
	    csm_touch_nb_cache_active_to = GETOBJ(int,2) * ONESEC;
	    csm_touch_nb_cache_reset_to = GETOBJ(int,3) * ONESEC;
        }
        else { 
	    csm_touch_nb_cache_active_to = CSM_DEF_TOUCH_NB_CACHE_ACTIVE_TO;
	    csm_touch_nb_cache_reset_to = CSM_DEF_TOUCH_NB_CACHE_RESET_TO;
        }
	break;
	    
    case CSM_EXPL_WAIT_T:
        if (csb->sense)
            csm_expl_wait_rt = GETOBJ(int, 2) * ONESEC;
        else 
            csm_expl_wait_rt = CSM_DEF_EXPL_WAIT_INT;
        break;

      default:
        bad_parser_subcommand(csb, GETOBJ(int,1));
        break;
    }
}

/*
 * csm_parse_statics()
 */

static void csm_parse_statics (parseinfo *csb)
{
    cache_action_t action;
    object_type_t map_type;
    config_obj_u obj;
    uchar rif_len;
    ulong ii;
    
    if (csb->sense)
        action = SET;
    else
        action = CLEAR;

    switch (GETOBJ(int,2)) {
      case CSM_RES2RIF:
        map_type = RIF_STRING;
	rif_len = GETOBJ(int,4);

        /* copy the rif into obj */
        for (ii = 0; ii < rif_len; ii++)
            obj.rif[ii] = csb->uiarray[ii];
        break;

      case CSM_RES2RING:
        map_type = RING_ID;
        obj.ring_id = GETOBJ(int,3);
        break;

      case CSM_RES2IPADDR:
        map_type = PEER_ID;
        obj.peer_id = peer_ip_to_handle(GETOBJ(paddr,1)->ip_addr);
        if (!obj.peer_id) {
            printf("\n%%No remote peer defined with ip address %i", 
                   GETOBJ(paddr,1)->ip_addr);
            return;
        }
        break;

      case CSM_RES2INTERFACE:
        map_type = PEER_ID;
        obj.peer_id = peer_hwidb_to_handle(GETOBJ(idb,1)->hwptr);
        if (!obj.peer_id) {
            printf("\n%%No remote peer defined for interface %s",
                   GETOBJ(idb,1)->namestring);
            return;
        }
        break;

      default:
        bad_parser_subcommand(csb, GETOBJ(int,2));
        return;
    }
    if (GETOBJ(int,1) == CSM_MAC_ADDR) {
        csm_update_mac_entry(action, USER_CONFIGURED, GETOBJ(hwaddr,1)->addr,
                             NULL, REACHABLE, map_type, obj);
    } else {
	if (str_has_wc(GETOBJ(string,1))) {
	    printf("\n%%Wildcards are not allowed in static NetBios name entries");
	    return;
	}
        csm_update_nbname_entry(action, USER_CONFIGURED, GETOBJ(string,1),
                                REACHABLE, map_type, obj);
    }
}

/*
 *********************************************************************
 * CSM Debug Functions
 *********************************************************************
 */

/*
 * show_csm_debug()
 */

void show_csm_debug ()
{
    if (dlsw_csm_debug_verbose || dlsw_csm_debug_event || dlsw_csm_debug_error)
      {
        printf("\nDLSw reachability debugging is on at ");
        if (dlsw_csm_debug_verbose) {
            printf("verbose");
        } else if (dlsw_csm_debug_event) {
            printf("event");
        } else {
            printf("error");
	}
        printf(" level for ");
        if (dlsw_csm_debug_all) {
            printf("all protocol");
        } else if (dlsw_csm_debug_sna) {
            printf("SNA");
        } else {
            printf("NetBIOS");
	}
        printf(" traffic");
    }
}

/*
 * csm_debug()
 */

void csm_debug (int level, int prot, boolean printit, boolean sense)
{
    dlsw_csm_debug_verbose = (level == DLSW_DEBUG_CSM_VERBOSE) ? sense : FALSE;
    dlsw_csm_debug_event = (level == DLSW_DEBUG_CSM_EVENTS) ? sense : FALSE;
    dlsw_csm_debug_error = (level == DLSW_DEBUG_CSM_ERRORS) ? sense : FALSE;
    dlsw_csm_debug_sna = (prot == DLSW_DEBUG_SNA_REACH) ? sense : FALSE;
    dlsw_csm_debug_netb = (prot == DLSW_DEBUG_NETB_REACH) ? sense : FALSE;
    dlsw_csm_debug_all = (prot == DLSW_DEBUG_ALL_REACH) ? sense : FALSE;

    if (printit) {
        if (sense) {
            show_dlsw_debug();
	} else {
            printf("DLSw reachability debugging is off");
	}
    }
}


/*
 * csm_debug_all()
 */

void csm_debug_all (boolean printit, boolean sense)
{
    csm_debug(DLSW_DEBUG_CSM_EVENTS, DLSW_DEBUG_ALL_REACH, printit, sense);
}

/*
 * Clear all reachability cache
 */
void parse_clear_cache(parseinfo *csb)
{
    clear_csm_cache(csb);
}

/*
 * show csm cache for the given cache db
 */
static void print_cache_header (db_master_t *db_num)
{
    if (db_num == LCACHE_MAC) {
        printf("\nDLSw Remote MAC address reachability cache list");
        printf("\nMac Addr        status     Loc.    port                 rif");
    } else if (db_num == RCACHE_MAC) {
        printf("\nDLSw Local MAC address reachability cache list");
        printf("\nMac Addr        status     Loc.    peer");
    } else if (db_num == LCACHE_NAME) {
        printf("\nDLSw Local NetBIOS Name reachability cache list");
        printf("\nNetBIOS Name    status     Loc.    port                 rif");
    } else {    /* RCACHE_NAME */
        printf("\nDLSw Remote NetBIOS Name reachability cache list");
        printf("\nNetBIOS Name    status     Loc.    peer");
    }
}

static void print_nb_bogus_name (char *nb_bogus_name)
{
    char            hexa_char[] = "0123456789abcdef";
    char            *str;
    char            *tmp_str;
    int             ii, space_cnt;

    space_cnt = 0;
    while (nb_bogus_name[space_cnt] == '\0')
        space_cnt++;
    ii = space_cnt;
    str = string_getnext();
    tmp_str = str;
    for (; ii < 15; ii++) {
        *tmp_str++ = hexa_char[(nb_bogus_name[ii] >> 4) & 0x0f];
        *tmp_str++ = hexa_char[nb_bogus_name[ii] & 0x0f];
    }
    /* Reduce space_cnt by length of '0..' which is 3 */
    if (space_cnt > 3) {
       space_cnt -= 3;
    } else {
         space_cnt = 0;
    }
    while (space_cnt) {
        *tmp_str++ = ' ';
        space_cnt--;
    }
    *tmp_str = '\0';
    printf("\n0..%s ", str);
}


static void print_cache_entry (db_master_t *db_num, record_handle rhandle,
                        lcache_t *cache_ptr, lcache_t *scache_ptr)
{
    int             ii;
    boolean         first_record;
    char            *rif_str;
    char            *peerid_str;
    dlsw_ring_t     *dgra_tmp;
    leveltype       status;
    char            *name;

    status = raise_interrupt_level(NETS_DISABLE);
    mem_lock(rhandle);
    bcopy(cache_ptr, scache_ptr, sizeof(lcache_t));
    if (scache_ptr->reach_direction == LOCAL) {
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
	    bcopy(&cache_ptr->r_entry[ii],
		  &scache_ptr->r_entry[ii], sizeof(reach_entry_t));
	    if (!(scache_ptr->cache_type & SAP_PEND))
                mem_lock(scache_ptr->r_entry[ii].prp_id.port_id);
        }
    } else {    /* direction is REMOTE */
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
	    bcopy(&cache_ptr->r_entry[ii],
		  &scache_ptr->r_entry[ii], sizeof(reach_entry_t));
            /* peer_id is actually a pointer to the peer structure */
            mem_lock((void *) scache_ptr->r_entry[ii].prp_id.peer_id);
        }
    }
    reset_interrupt_level(status);

    automore_conditional(NDLSW_LINES);
    if ((db_num == LCACHE_MAC) || (db_num == RCACHE_MAC)) {
        printf("\n%e  %s  %s",
		RECORD_KEY(rhandle),
		csm_cache_status_to_string(scache_ptr->status),
		csm_cache_direction_to_string(scache_ptr->reach_direction));
    } else {    /* LCACHE_NAME or RCACHE_NAME */
        name = RECORD_KEY(rhandle);
        if (name[0] == '\0') {
            print_nb_bogus_name(name);
        } else {
            printf("\n%15s ", name);  
        }       
        printf("%s  %s",
                csm_cache_status_to_string(scache_ptr->status),
                csm_cache_direction_to_string(scache_ptr->reach_direction));
    }
    first_record = TRUE;
    for (ii = 0; ii < scache_ptr->no_in_list; ii++) {
	rif_str = NULL;
	if (scache_ptr->r_entry[ii].flags != 0) {
	    if (scache_ptr->reach_direction == LOCAL) {
                rif_str = string_getnext();
		if (rif_str == NULL) {
		    /* change to errmsg */
		    buginf("\nCSM: Error getting buf from string_getnext");
                } else {
		    if (RIF_LENGTH(scache_ptr->r_entry[ii].rif[0]))
			rif_str = get_rif_str(scache_ptr->r_entry[ii].rif, 
                                              rif_str);
		    else
			rif_str = "--no rif--";
                }
		dgra_tmp = scache_ptr->r_entry[ii].prp_id.port_id;
		if (first_record) {
		    if (scache_ptr->cache_type & SAP_PEND) {
	                printf("  InActRg-%03x    %s", 
                                  (ulong)dgra_tmp,
				  rif_str);
                    } else if (dgra_tmp->port_type != CLS_TBRIDGE) {
	                printf("  %s     %s", 
                              dgra_tmp->swidb->hwptr->hw_namestring,
			      rif_str);
                    } else {
	                printf("  TBridge-%03x    %s", 
                                  dgra_tmp->trans_bridge_group,
				  rif_str);
                    }
		    first_record = FALSE;
                } else {
		    if (scache_ptr->cache_type & SAP_PEND) {
		        printf("\n%35cInActRg-%03x     %s", ' ',
                                  (ulong)dgra_tmp,
				  rif_str);
                    } else if (dgra_tmp->port_type != CLS_TBRIDGE) {
		        printf("\n%35c%s      %s", ' ',
                              dgra_tmp->swidb->hwptr->hw_namestring,
			      rif_str);
                    } else {
		        printf("\n%35cTBridge-%03x     %s", ' ',
                                  dgra_tmp->trans_bridge_group,
				  rif_str);
                    }
                }
            } else {
		peerid_str = string_getnext();
		if (peerid_str == NULL) {
		    /* change to errmsg */
		    buginf("\nCSM: Error getting buf from string_getnext");
                } else {
		    peer_handle_to_str(scache_ptr->r_entry[ii].prp_id.peer_id, 
			       peerid_str);
                }
                if (peerid_str[1] != 0) {
                    if (first_record) {
                        if (scache_ptr->r_entry[ii].mtu_size)
                            printf("  %s max-lf(%d)", &peerid_str[5],
                                   scache_ptr->r_entry[ii].mtu_size);
                        else
                            printf("  %s", &peerid_str[5]);
                        first_record = FALSE;
                    } else {
                        if (scache_ptr->r_entry[ii].mtu_size)
                            printf("\n%35c%s max-lf(%d)", ' ', &peerid_str[5],
                                   scache_ptr->r_entry[ii].mtu_size);
                        else
                            printf("\n%35c%s", ' ', &peerid_str[5]);
                    }
                } else {
                    if (first_record) {
                        printf("  %s", "No peer info.");
                        first_record = FALSE;
                    } else {
                        printf("\n%35c%s", ' ', "No peer info.");
                    }
                }
            }
        }
    }

    /* free the locked memory */
    if (scache_ptr->reach_direction == LOCAL) {
	if (!(scache_ptr->cache_type & SAP_PEND)) {
            for (ii = 0; ii < scache_ptr->no_in_list; ii++) 
                free(scache_ptr->r_entry[ii].prp_id.port_id);
        }
    } else {    /* direction is REMOTE */
        for (ii = 0; ii < scache_ptr->no_in_list; ii++) {
            free((void *) scache_ptr->r_entry[ii].prp_id.peer_id);
        }
    }
}


void csm_show_cache (int flag, char *match, int match_valid)
{

    db_master_t     *db_num;
    lcache_t        *cache_ptr;
    lcache_t        *scache_ptr;       /* pointer to snapshot of cache */
    record_handle   rhandle;

    if (!dlsw_started) {
      return;
    }

    switch (flag) {
      case LCACHE_MAC_FLAG:
        db_num = LCACHE_MAC;
        break;
      case RCACHE_MAC_FLAG:
        db_num = RCACHE_MAC;
        break;
      case LCACHE_NAME_FLAG:
        db_num = LCACHE_NAME;
        break;
      default:   /* RCACHE_NAME_FLAG */
        db_num = RCACHE_NAME;
    }
    automore_enable(NULL);
    scache_ptr = malloc_named(sizeof(lcache_t), "SHOW CSM REACH SNAPSHOT");
    if (!scache_ptr) {
        buginf("\nCSM: Error in malloc for show cache snapshot");
	return; 
    }
    if (!match_valid) {
        print_cache_header(db_num);
        rhandle = read_next_record(db_num, NULL, NULL, (void **) &cache_ptr);
        while (rhandle != NULL) {
            print_cache_entry(db_num, rhandle, cache_ptr, scache_ptr);
            if (rhandle->del_flag) {
                /* CSM freed it !!! */
                free(rhandle);
                printf("\nCSM Cache may have changed.... Retry command");
                rhandle = NULL;     /* exit out of the loop */
            } else {
                free(rhandle);
                rhandle = read_next_record(db_num, rhandle, NULL,
                                       (void **) &cache_ptr);
            }
        }

        printf("\n");   /* leave a blank line */
    } else {      /* search for a specific key */

        if ((db_num == LCACHE_MAC) || (db_num == RCACHE_MAC))
          rhandle = seek_record(db_num, match, (void **) &cache_ptr);
        else
          /* search for netbios name - use pad characters if necessary */
          rhandle = seek_nb_name_record(db_num, match, (void **) &cache_ptr);

        if (rhandle) {
            print_cache_header(db_num);
            print_cache_entry(db_num, rhandle, cache_ptr, scache_ptr);
            printf("\n");
            free(rhandle);
        } 
    }

    free(scache_ptr);
    automore_disable();
    return;
}



char *csm_cache_status_to_string(int csm_cstatus)
{

    switch (csm_cstatus) {
      case FOUND:
	return ("FOUND    ");
      case NOT_FOUND:
	return ("NOT_FOUND");
      case UNCONFIRMED:
	return ("UNCONFIRM");
      case SEARCHING:
	return ("SEARCHING");
      case VERIFY:
        return ("VERIFY   ");
      default:
	return ("ERROR    ");
    }

}



char *csm_cache_direction_to_string(int cache_direction)
{

    switch (cache_direction) {
      case LOCAL:
	return ("LOCAL ");
      case REMOTE:
	return ("REMOTE");
      default:
	return ("ERROR ");
    }

}



