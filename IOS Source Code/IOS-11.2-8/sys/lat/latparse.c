/* $Id: latparse.c,v 3.5.18.6 1996/08/28 12:56:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/lat/latparse.c,v $
 *------------------------------------------------------------------
 * January 1990, Bill Westfield.
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latparse.c,v $
 * Revision 3.5.18.6  1996/08/28  12:56:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.18.5  1996/08/07  09:01:10  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.18.4  1996/06/28  23:20:37  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.18.3  1996/06/17  23:34:43  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.5.18.2  1996/05/30  23:44:35  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.5.18.1  1996/03/18  20:39:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/07  10:04:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  01:07:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:13:49  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/01/22  06:36:36  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1996/01/10  06:49:35  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.2  1995/11/17  17:34:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/19  06:46:26  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  23:39:57  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  21:25:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "mtcsys.h"
#include "config.h"
#include "latls.h"
#include "latas.h"
#include "laths.h"
#include "latnd.h"
#include "latob.h"
#include "latfnc.h"
#include "latmsg.h"
#include "sysfnc.h"
#include "ttysrv.h"
#include "connect.h"
#include "address.h"
#include "name.h"
#include "latgroup.h"
#include "access.h"
#include "lat.h"
#include "lat_public.h"
#include "sys_registry.h"
#include "logger.h"
#include "parser.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../parser/parser_defs_lat.h"
#include "../parser/parser_defs_exec.h"
#include "../os/init.h"

#define MAXCHRS 40

/*
 * forward declarations
 */

/*
 * Forward references
 */
void nv_lat_groupmask(char *, byte *, boolean, boolean);
static void lat_service_command(parseinfo *);

/*
 * Local data
 */

static const char lat_name_len_err[] = 
  "\n%s name can be no longer than %d characters";

/*
 * externals
 */
extern boolean lat_enable;
extern boolean latfltr_debug;

/*
 * show_lat
 * Display LAT information
 */

void show_lat (parseinfo *csb)
{
    if (!lat_enable) {
	if (!csb->nvgen) {
	    printf(noasm);
	}
	return;
    }
    switch (GETOBJ(int,1)) {
      case SHOW_LAT_ACCESS:
	show_regexpacl(lat_accesslists, GETOBJ(int,2), "LAT");
	break;
      case SHOW_LAT_AS:
	show_lat_as();
	break;
      case SHOW_LAT_GROUPS:
	show_lat_groups();
	break;
      case SHOW_LAT_SERVICES:
	show_lat_ls();
	break;
      case SHOW_LAT_NODES:
	show_lat_nd(NULL);
	break;
      case SHOW_LAT_SESSIONS:
	show_lat_sessions(GETOBJ(int,2));
	break;
      case SHOW_LAT_TRAFFIC:
	show_lat_traffic();
	break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
	break;
    }
}

int create_lat_name (char *src, byte *dst, int len, char *descr)
{
    dst[0] = strlen(src);
    if (dst[0] < 1 || dst[0] >= len) {
	printf(lat_name_len_err, descr, len-1);
	return(-1);
    }
    bcopy(src, &dst[1], dst[0] + 1);
    uppercase((char *)&dst[1]);
    return(0);
}

void lat_connect_command (
     parseinfo *csb,
     boolean *abortflag)
{
    conntype *newcon;
    byte service[SV_STR+1];
    byte port[OB_STR+1];
    byte node[ND_STR+1];
    char *nodename = GETOBJ(string, 2);
    char *portname = GETOBJ(string, 3);
    boolean debug = GETOBJ(int, 1);
    ls_ptr ls;

    /*
     * Ensure that this connection is allowed
     */

    if (!connect_allowed(&lat_connvector))
      return;

    /*
     * See if this looks like a valid LAT service name.
     */

    if (create_lat_name(GETOBJ(string, 1), service, SV_STR, "service"))
      return;

    /*
     * We have the name in the format for LAT - now see if it is a valid
     * service and be sure that group codes permit this.
     */

    ls = ls_show(service, NULL, NULL, 0);
    
    if (!ls || !lat_compare_groups(ls->ls_nd->nd_acl, 
				   stdio->tty_lat_groupmask)) {
	printf(connect_unknown);
	return;
    }
    
    /*
     * At this point, we have determined that this is a valid LAT service
     * name. Now handle the other parameters.
     */

    if (nodename[0]) {
	if (create_lat_name(nodename, node, ND_STR, "node"))
	  return;
    } else
      node[0] = 0;

    if (portname[0]) {
	if (create_lat_name(portname, port, OB_STR, "port"))
	  return;
    } else
      port[0] = 0;

    /*
     * Now we try to connect to the remote host.
     */

    newcon = lat_open_connection(stdio, service, node, port,
			       debug ? LATOPEN_DEBUG : 0, NULL);

    if (newcon)
      connect_new_session(newcon, GETOBJ(string, 1), csb);
    				/* Start this new session */
}

/*
 * lat_command
 * parse LAT configuration commands.
 */
void lat_command (parseinfo *csb)
{
    latgroup *latg;
    boolean new = FALSE;
    byte setmask[32];
    byte clearmask[32];

    if (system_loading) {
	return;
    }

    if (!lat_enable) {
	if (! csb->nvgen) {
	    printf(noasm);
	}
    }
    if (csb->nvgen) {
	as_ptr as;

	switch (csb->which) {

	  case LAT_ACCESS:
	    regexpaccess_command(csb, lat_accesslists);
	    break;

	  case LAT_EXTRA_HOST:
	    nv_write(hs->hs_cb_h_rcv_extra != CB_H_RCV_EXTRA,
		     "%s %d", csb->nv_command,
		     hs->hs_cb_h_rcv_extra + 1);
	    break;

	  case LAT_EXTRA_SERVER:
	    nv_write(hs->hs_cb_s_rcv_extra != CB_S_RCV_EXTRA,
		     "%s %d", csb->nv_command,
		     hs->hs_cb_s_rcv_extra + 1);
	    break;

	  case LAT_GROUPL:
	    for (latg = (latgroup *) latgrpQ.qhead; latg; latg = latg->next) {
		nv_write(TRUE, "%s %s", csb->nv_command, latg->name);
		nv_lat_groupmask(NULL, latg->groups, FALSE, FALSE);
	    }
	    break;

	  case LAT_KATIMER:
	    nv_write(hs->hs_cb_ka_timer != CB_KA_SECS,
		     "%s %d", csb->nv_command,
		     hs->hs_cb_ka_timer);
	    break;

          case LAT_HDTIMER:
            nv_write(lat_host_delay != 0,
                     "lat host-delay %d", lat_host_delay);
            break;

	  case LAT_NODENAME:
	    if ((lat_interfaces != 0) &&
		strcasecmp((char *)&hs->hs_node[1], hostname))
		nv_write(TRUE, "%s %s", csb->nv_command, &hs->hs_node[1]);
	    break;

	  case LAT_RETRANSMIT:
	    nv_write(hs->hs_cb_retries != CB_DEF_TRY,
		     "%s %d", csb->nv_command,
		     hs->hs_cb_retries);
	    break;

	  case LAT_SVC:
	    for (as = as_list((byte *)""); as; as = as_list(as->as_service)) {
		/* Write the command to set the service identification */
		nv_write(as->as_ident[0] != 0, "%s %*s ident %*s", csb->nv_command,
			 LATSTRING(as->as_service),LATSTRING(as->as_ident));
		/* Write the command to set the static rating */
		nv_write(!(as->as_flags & AS_CLIENT),
			 "%s %*s rating %d", csb->nv_command,
			 LATSTRING(as->as_service),as->as_rating);
		/* Write the command to set the rotary group membership */
		nv_write(as->as_rotary,
			 "%s %*s rotary %d", csb->nv_command,
			 LATSTRING(as->as_service),as->as_rotary);
		/* Write the command to set the autocommand */
		nv_write(as->as_command != NULL,
			 "%s %*s autocommand %s", csb->nv_command,
			 LATSTRING(as->as_service),as->as_command);
		/* Write out the command to set the password */
		nv_write(as->as_password[0] != 0,
			 "%s %*s password %*s", csb->nv_command,
			 LATSTRING(as->as_service),LATSTRING(as->as_password));
		/* Write the command to enable the service */
		if (as->as_aid){         /* Dont write "enable" for CPT services */
		    continue;
		}
		nv_write((as->as_flags & AS_ENABLED),
			 "%s %*s enabled", csb->nv_command,
			 LATSTRING(as->as_service));
	    }
	    break;

	  case LAT_SERVICEG:
	    nv_lat_groupmask("service-group", hs->hs_acl_group, TRUE, TRUE);
	    break;

	  case LAT_SERVICE_ANNOUNCEMENTS:
	    nv_write(!(hs->hs_status & SVC_ANNC_EN),
		     "no %s", csb->nv_command);
	    break;

	  case LAT_SERVICE_RESPONDER:
	    nv_write(hs->hs_status & SVC_AGENT,
		     "%s", csb->nv_command);
	    break;

	  case LAT_SERVICET:
	    nv_write(hs->hs_as_timer != AS_MC_SEC,
		     "%s %d", csb->nv_command, hs->hs_as_timer);
	    break;

	  case LAT_VCTIMER:
	    nv_write(hs->hs_cb_vc_timer != CB_VC_MSEC,
		     "%s %d", csb->nv_command,
		     hs->hs_cb_vc_timer);
	    break;

	  case LAT_VC_SESSIONS:
	    nv_write(hs->hs_cb_max_ses != CB_MAX_SES,
		     "%s %d", csb->nv_command,
		     hs->hs_cb_max_ses);
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {

      case LAT_ACCESS:
	regexpaccess_command(csb, lat_accesslists);
	break;

      case LAT_EXTRA_HOST:	
	if (csb->sense)
	    hs->hs_cb_h_rcv_extra = GETOBJ(int, 1) - 1;
	else
	    hs->hs_cb_h_rcv_extra = CB_H_RCV_EXTRA;
	break;

      case LAT_EXTRA_SERVER:
	if (csb->sense)
	    hs->hs_cb_s_rcv_extra = GETOBJ(int, 1) - 1;
	else
	    hs->hs_cb_s_rcv_extra = CB_S_RCV_EXTRA;
	break;

      case LAT_GROUPL:
	latg = lat_find_group(GETOBJ(string,1));
	if (csb->sense) {
	    if (!latg) {
		latg = malloc(sizeof(latgroup));
		if (!latg) {
		    printf(nomemory);
		    return;
		}
		sstrncpy(latg->name, GETOBJ(string,1), LATGROUPNAMELEN);
		new = TRUE;
	    }
	} else {
	    if (GETOBJ(int,3) == 0) {

		/* Request to delete entire list */

		if (latg) {
		    unqueue(&latgrpQ, latg);
		    free(latg);
		} else {
		    printf("\nUnknown group-list '%s'", GETOBJ(string,1));
		}
		return;
	    }
	}
	if (!lat_parse_grouplist(csb, setmask, clearmask)) {
	    if (new) {
		free(latg);
	    }
	    return;
	}

        if (latfltr_debug){
           int i;

           buginf("\n LAT: Group List ACL changed, Mask = ");
           for (i = 0; i < 32; i++)
               buginf(" %x", setmask[i]);
           
        } 
	lat_apply_groupmask(latg->groups, setmask, clearmask);
	if (new) {
	    enqueue(&latgrpQ, latg);
	}
	break;

      case LAT_NODENAME:
	/*
	* Parse the node name
	*/
	if (csb->sense){
	    if (create_lat_name(GETOBJ(string,1), hs->hs_node,
				ND_STR, "node") == -1){
		return;
	    }
	    if (!str_check(hs->hs_node, ND_STR)) {
		printf("\n%% \"%*s\" is not a legal LAT node name", 
		       LATSTRING(hs->hs_node));
		lat_sethost();
	    }
	} else {
	    lat_sethost();
	}
	hs->hs_as_next_changes |= SVC_OTHER;
	break;

      case LAT_KATIMER:
	if (csb->sense) {
	    hs->hs_cb_ka_timer = GETOBJ(int,1);
	} else {
	    hs->hs_cb_ka_timer = CB_KA_SECS;
	}
	break;

      case LAT_HDTIMER:
        if (csb->sense){
          /*
           * Value, entered is in msecs, truncated to multiples of MSEC_TIC
           */
          if ((GETOBJ(int,1) >= MSEC_TIC) && (GETOBJ(int,1)<= CB_HD_MAX_MSEC))
             lat_host_delay = GETOBJ(int,1)/MSEC_TIC * MSEC_TIC;
          else 
            printf("Bad host-delay value, min = %d msecs, max = %d msecs", MSEC_TIC, CB_HD_MAX_MSEC );
        }
        else
          lat_host_delay = 0;
        break;

      case LAT_RETRANSMIT:
	if (csb->sense) {
	    hs->hs_cb_retries = GETOBJ(int,1);
	} else {
	    hs->hs_cb_retries = CB_DEF_TRY;
	}
	break;

      case LAT_SVC:
	lat_service_command(csb);
	break;

      case LAT_SERVICEG:
	new = FALSE;
	if (!csb->sense && (GETOBJ(int,3) == 0)) {
	    memset(clearmask, 0xff, 32);
	    memset(setmask, 0, 32);
	    setmask[0] = 1;
	    new = TRUE;
	} else {
	    new = lat_parse_grouplist(csb, setmask, clearmask);
	}
	if (new) {
            if (latfltr_debug){
               int i;

               buginf("\n LAT: Host ACL group changed, Mask = ");
               for (i = 0; i < 32; i++)
                   buginf(" %x", setmask[i]);
            }                           
	    lat_apply_groupmask(hs->hs_acl_group, setmask, clearmask);
	    hs->hs_as_next_changes |= SVC_GROUPS;
	}
	break;

      case LAT_SERVICET:
	if (csb->sense) {
	    hs->hs_as_timer = GETOBJ(int,1);
	} else {
	    hs->hs_as_timer = AS_MC_SEC;
	}
	break;

      case LAT_SERVICE_ANNOUNCEMENTS:
	if (csb->sense)
	  hs->hs_status |= SVC_ANNC_EN;
	else
	  hs->hs_status &= ~SVC_ANNC_EN;
	break;

      case LAT_SERVICE_RESPONDER:
	if (csb->sense)
	  hs->hs_status |= SVC_AGENT;
	else
	  hs->hs_status &= ~SVC_AGENT;
	break;

      case LAT_VCTIMER:
	if (csb->sense) {
	    hs->hs_cb_vc_timer = GETOBJ(int,1);
	} else {
	    hs->hs_cb_vc_timer = CB_VC_MSEC;
	}
	break;

      case LAT_VC_SESSIONS:
	if (csb->sense) {
	    hs->hs_cb_max_ses = GETOBJ(int,1);
	} else {
	    hs->hs_cb_max_ses = CB_MAX_SES;
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }


}

void lat_int_command (parseinfo *csb)
{
    idbtype *idb;
    byte node[ND_STR+1];
    byte service[SV_STR+1];
    ls_ptr ls;

    idb = csb->interface;
    if (system_loading) {
	return;
    }

    if (!lat_enable) {
	if (! csb->nvgen) {
	    printf(noasm);
	}
    }

    if (csb->nvgen) {
	switch (csb->which) {
	  case LAT_ENABLED:
	    if (nttylines && (idb->hwptr->status & IDB_ETHER)){
		nv_write(!idb->lat_running, "no %s", csb->nv_command);
	    } else {
		nv_write(idb->lat_running, csb->nv_command);
	    }
	    break;

	  case LAT_HOST:
	    for (ls = hs->hs_ls_head[0]; ls; ls = ls->ls_alpha[0]) {
		if (ls->ls_nd && (ls->ls_flags & LS_PERM) &&
		    ls->ls_nd->nd_idb == idb) {
		    nv_write(TRUE, "%s %*s %*s %e",
			     csb->nv_command,
			     LATSTRING(ls->ls_nd->nd_node),
			     LATSTRING(ls->ls_service),
			     ls->ls_nd->nd_host_addr);
		}
	    }
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {

      case LAT_ENABLED:
	if (csb->set_to_default)
	    if ((!router_enable) || (nttylines > 0))
		if (idb->hwptr->status & IDB_ETHER)
		    csb->sense = TRUE;
	if (csb->sense) {
	    if (!idb->lat_running) {
		idb->lat_running = TRUE;
		lat_interfaces++;
		(*(idb->hwptr->reset)) (idb->hwptr);
		lat_create_process();
	    }
	} else {
	    if (idb->lat_running) {
		idb->lat_running = FALSE;
		lat_interfaces--;
		(*(idb->hwptr->reset)) (idb->hwptr);
	    }
	}
	break;

      case LAT_HOST:
	if ((node[0] = strlen(GETOBJ(string,1))) > ND_STR - 1) {
	    printf(lat_name_len_err, "Node", ND_STR - 1);
	    return;
	}
	sstrncpy((char *)&node[1], GETOBJ(string,1), ND_STR);
	uppercase((char *)&node[1]);

	if ((service[0] = strlen(GETOBJ(string,2))) > SV_STR - 1) {
	    printf(lat_name_len_err, "Service", SV_STR - 1);
	    return;
	}
	sstrncpy((char *)&service[1], GETOBJ(string,2), SV_STR);
	uppercase((char *)&service[1]);

	if (csb->sense) {
	    if (ls_show(service, node, NULL, 0)) {
		printf("\nService already exists");
		return;
	    }
	    ls = ls_permanent(service, node, 
			      (word *)(GETOBJ(hwaddr,1)->addr), idb);
	    if (!ls) {
		lat_perror();
		return;
	    }
	    ls->ls_nd->nd_classes[0] = 1;
	    ls->ls_nd->nd_classes[1] = 2;
	} else {
	    if (!(ls = ls_show(service, node, NULL, 0))) {
		printf("\nService %s on node %s not found", &service[1],
		       &node[1]);
		return;
	    }
	    if (!(ls->ls_flags & LS_PERM)) {
		printf("\nService %s on node %s was learned dynamically",
		       &service[1], &node[1]);
		return;
	    }
	    ls_delete(ls);  /* Delete service */
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * lat_remote_modification - Handle the lat remote-modification command
 * With remote modification disable, LAT DataB slots cannot modify the
 * port characteristics. When enabled, the port characteristics are
 * modifiable by these DataBs.
 */

void lat_remote_modification (parseinfo *csb)
{
    tt_soc *tty;

    if (system_loading) {
        return;
    }

    if (!lat_enable) {
	if (! csb->nvgen) {
	    printf(noasm);
	}
    }

    if (csb->line1 < 0 || csb->line1 >= ALLlines ) {
       errmsg(&msgsym(INVLDLINE, PARSER), csb->line1);
       return;
    }

    tty = MODEMS[(int)csb->line1];

    if (csb->nvgen) {
	nv_write((tty->def_capabilities & REMOTE_MOD), "%s", csb->nv_command);
	return;
    }

    docaps (csb, REMOTE_MOD, FIRST_CAP);
}

/*
 * lat_ogroup_command - Handle the lat out-groups command
 *
 */

void lat_outgroup_command (parseinfo *csb)
{
    int i, j;
    byte setmask[32];
    byte clearmask[32];
    byte *ttymask;
    tt_soc *tty;
    int revert = TRUE;

    if (system_loading) {
        return;
    }

    if (!lat_enable) {
        if (! csb->nvgen) {
            printf(noasm);
	}
	return;
    }

    tty = MODEMS[csb->line1];
    if (csb->nvgen) {
	nv_lat_groupmask("out-group", tty->tty_lat_groupmask, TRUE, TRUE);
	return;
    }

    /*
     * Parse the group list, producing a mask of bits to clear, and a
     * mask of bits to set. If the list is invalid, get out - an error
     * will already have been reported.
     */

    if (!lat_parse_grouplist(csb, setmask, clearmask)){
	return;
    }

    /*
     * If this is a request to set the temporary characteristics,
     * trim the set request to only include bits that are already
     * set in the permanent characteristics. If there is no intersection
     * at all, revert back to original mask.
     */

    if (!csb->permanent) {
	tty = MODEMS[csb->line1];
	if (!tty->defaults)
	  tty_savedefaults(tty);
	for (i = 0; i < 32; i++){
	  setmask[i] &= tty->defaults->u_lat_groupmask[i + 1];
          if (setmask[i] != 0)
            revert = FALSE;
        }
        if (revert)
	  for (i = 0; i < 32; i++)       
	   setmask[i] |= tty->defaults->u_lat_groupmask[i + 1];
    }

    /*
     * Apply the changes to the group code masks
     */

    for (i = csb->line1; i < csb->line2; i++) {
	tty = MODEMS[i];
	if (!tty)
	  continue;
	if (csb->permanent && tty->defaults){
	  ttymask = tty->defaults->u_lat_groupmask;
          lat_apply_groupmask(ttymask, setmask, clearmask);
	}
	  
        ttymask = tty->tty_lat_groupmask;
        lat_apply_groupmask(ttymask, setmask, clearmask);

        if (latfltr_debug){
           int j;

           buginf("\n LAT: TTY%t, ACL group changed, Mask = ", tty->ttynum); 
           for (j = 0; j < 32; j++)
               buginf(" %x", setmask[j]);
        }    
    }
    
    /*
     * If this change was to the permanent list, go through all of the
     * permanent masks to build a new system-wide mask.
     */

    if (csb->permanent) {
        memset(hs->hs_idl_group, 0, GROUPS);	/* Zero out group code mask */
	for (i = 0; i < ALLlines; i++) {
	    tty = MODEMS[i];
	    if (!tty)
	      continue;
	    if (tty->defaults)
	      ttymask = tty->defaults->u_lat_groupmask;
	    else
	      ttymask = tty->tty_lat_groupmask;
	    for (j = 0; j < ttymask[0]; j++)
	      hs->hs_idl_group[j+1] |= ttymask[j+1];
	}
	for (i = 32; i > 0; i--)
	  if (hs->hs_idl_group[i])
	    break;
	hs->hs_idl_group[0] = i;
    }
}

/*
 * lat_service_command ()
 *
 * Parse the LAT service command.
 */

static void lat_service_command (parseinfo *csb)
{
    as_ptr as;
    byte service[SV_STR+1];
    byte ident[ID_STR-1];
    int	len;
    char *buf;

    /* First, look for the service name and convert it into
     * ASCIZ format
     */

    if ((service[0] = strlen(GETOBJ(string,1))) > SV_STR-1) {
	printf(lat_name_len_err, "Service", SV_STR-1);
	return;
    }

    sstrncpy((char *)&service[1], GETOBJ(string,1), SV_STR);
    uppercase((char *)&service[1]);

    /* We have fetched the service name, now look for it */

    as = as_show(service);

    /* Parse an optional keyword. If we find a keyword, dispatch to the
     * appropriate handler passing the service block address. Note that
     * a service must already exist for the keyword commands to be valid.
     */

    if (GETOBJ(int,1) != 0) {
	if (as == NULL) {
	    as = as_create(service, (byte *)"", 0, AS_CLIENT, 0);
	}
	if (as == 0) {
	    printf("\nInsufficient memory to create new LAT services.");
	    return;
	}
    }
    switch (GETOBJ(int,1)) {

      case 0:
	if (!csb->sense) {
	    if (!as) {
		printf("\nService %s does not exist.", &service[1]);
		return;
	    } else {
	        if (as->as_aid) {
                    printf("\nService %s is in use by translation.", &service[1]);
                    return;
	        }
	    }
	    free(as->as_command);		/* Delete any autocommand */
	    as_delete(as);			/* Now the service itself */

            /*
             * Update the bridging smf table if required
             */
            lat_mass_macaddrs_update(csb->sense);        
	break;
	} else {
	    printf("\nMissing parameter for LAT SERVICE command.");
	}
	break;

      case LAT_ENABLE_CMD:
	if (csb->sense) {
	    if (!as_set_flags(as, as->as_flags|AS_OFFERED|AS_ENABLED)) {
		printf("\nInsufficient memory to enable service.");
	    }
	}else{
	    if (!as->as_aid)
	        as_set_flags(as, as->as_flags & ~(AS_OFFERED|AS_ENABLED));
	}
        /*
         * Update the bridging smf table if required
         */
        lat_mass_macaddrs_update(csb->sense);        
	break;

      case LAT_IDENT:
	if ((ident[0] = strlen(GETOBJ(string,2))) > ID_STR-1) {
	    printf("\nIdentification string can be no longer than %d characters.",
		   ID_STR-1);
	}
	sstrncpy((char *)&ident[1], GETOBJ(string,2), ID_STR);
	if (!as_set_ident(as, ident)) {
	    printf("\nInsufficient memory to store new identification.");
	}
	break;

      case LAT_RATING:
	if (csb->sense) {
	    as->as_rating = GETOBJ(int,2);
	    as_set_flags(as, as->as_flags &~ AS_CLIENT);
	} else {
	    as_set_flags(as, as->as_flags | AS_CLIENT);
	}
	break;

      case LAT_AUTOCOMMAND:
	len = strlen(GETOBJ(string,2));
	if (len) {
	    buf = malloc(len+1);
	    if (buf) {
		strcpy(buf, GETOBJ(string,2));
		free(as->as_command);
		as->as_command = buf;
	    } else {
		printf(nomemory);
	    }
	} else {
	    free(as->as_command);
	    as->as_command = NULL;
	}
	break;

      case LAT_ROTARY:
	if (csb->sense) {
	    as->as_rotary = GETOBJ(int,2);
	} else {
	    as->as_rotary = 0;
	}
	reg_invoke_line_handoff();
	break;

      case LAT_PASSWORD:
	if ((len = strlen(GETOBJ(string,2))) > SV_STR - 1) {
	    printf("\nPassword length of %d is too long, maximum is %d",
		   len, SV_STR-1);
	} else {
	    if (len) {
		strcpy((char *)&as->as_password[1], GETOBJ(string,2));
		uppercase((char *)&as->as_password[1]);
	    }
	    as->as_password[0] = len;
	}
	break;

    }
}


void show_node (parseinfo *csb)
{
    char *name = GETOBJ(string,1);

    if (name != NULL) {
	uppercase(name);
    }

    switch (GETOBJ(int,1)) {
      case SHOW_NODE_COUNTERS:
	declat_show_nodes(name, declat_show_node_counters);
	break;
      case SHOW_NODE_STATUS:
	declat_show_nodes(name, declat_show_node_status);
	break;
      case SHOW_NODE_SUMMARY:
      default:
	declat_show_nodes(name, declat_show_node_summary);
	break;
    }
}

/*
 * lat_multiproto_open - Parse handler for the connect command
 *
 * This is the service routine for handling connect commands to LAT
 * services. We are called to determine whether a name is a valid LAT
 * service name for connect processing.
 */

conntype *lat_multiproto_open (csb, abortflag)
     parseinfo *csb;
     boolean *abortflag;
{
    byte service[SV_STR+1];
    ls_ptr ls;
    conntype *conn;

    /*
     * Ensure that the service name is a valid length, and then convert
     * it to a format suitable for the LAT engine.
     */

    service[0] = strlen(GETOBJ(string, 1));
    if (service[0] < 1 || service[0] >= SV_STR)
      return(NULL);

    bcopy(GETOBJ(string, 1), &service[1], service[0] + 1);
    uppercase((char *)&service[1]);

    /*
     * We have the name in the format for LAT - now see if it is a valid
     * service and be sure that group codes permit this.
     */

    ls = ls_show(service, NULL, NULL, 0);
    
    if (!ls || !lat_compare_groups(ls->ls_nd->nd_acl, 
				   stdio->tty_lat_groupmask))
      return(NULL);		/* No such service or access denied */

    /*
     * Attempt to open the connection
     */

    conn = lat_open_connection(stdio, service, NULL, NULL, 0, abortflag);

    return(conn);		/* Return a connection pointer, if any */

}

