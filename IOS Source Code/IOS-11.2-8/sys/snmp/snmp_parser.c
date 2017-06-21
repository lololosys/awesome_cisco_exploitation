/* $Id: snmp_parser.c,v 3.7.18.7 1996/07/12 23:29:05 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/snmp/snmp_parser.c,v $
 *------------------------------------------------------------------
 * SNMP parser functions
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: snmp_parser.c,v $
 * Revision 3.7.18.7  1996/07/12  23:29:05  jjohnson
 * CSCdi62131:  Need a mechanism to support logical snmp entities
 * Branch: California_branch
 *
 * Revision 3.7.18.6  1996/07/03  17:37:19  jjohnson
 * CSCdi61958:  Remove support for undocumented snmp configuration commands
 * remove the last vestiges of support for wildcard snmp communities
 * Branch: California_branch
 *
 * Revision 3.7.18.5  1996/06/28  23:26:08  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.18.4  1996/06/17  23:36:42  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.7.18.3  1996/04/15  03:05:38  jjohnson
 * CSCdi53343:  Remove support for the SNMPv2 Simplified Security
 * Conventions
 * Branch: California_branch
 *
 * Revision 3.7.18.2  1996/03/28  16:47:00  jjohnson
 * CSCdi50399:  SNMP can drive CPU utilization to over 90%
 * Make SNMP processes default to low priority, and add a hidden config
 * command to allow customers to change it
 * Branch: California_branch
 *
 * Revision 3.7.18.1  1996/03/18  21:48:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/13  01:59:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.1  1996/02/20  17:13:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/07  16:15:24  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/01/23  11:55:47  mdb
 * CSCdi47339:  apply code review comments for CSCdi47065
 *
 * Revision 3.5  1996/01/22  07:15:19  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/16  00:14:37  sdurham
 * CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
 * cli support.
 *    moved all parser commands related to mib to appropriate mib
 *    subsystem.
 *
 * Revision 3.3  1995/11/18  00:21:37  jjohnson
 * CSCdi43548:  need mechanism to enable multiple traps for a subsystem
 *
 * Revision 3.2  1995/11/17  18:58:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:25:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/17  04:24:34  bchan
 * CSCdi36821:  SNMP trap process started without SNMP being configured
 *
 * Revision 2.4  1995/07/17  07:58:20  bchan
 * CSCdi36821:  SNMP trap process started without SNMP being configured
 *
 * Revision 2.3  1995/07/03  22:26:25  jjohnson
 * CSCdi35401:  Parser help for snmp-server packetsize is incorrect
 * Differentiate between the minimum, maximum, and default packetsize
 *
 * Revision 2.2  1995/06/20  07:13:59  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:48:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "parser.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../snmp/sr_snmpd.h"
#include "../wan/sr_frmib.h"
#include "sr_config.h"
#include "snmp_registry.h"
#include "../parser/parser_defs_snmp.h"
#include "../os/buffers.h"
#include "../snmp/sr_sysmib2.h"
#include "../snmp/sr_snmpv2mib.h"
#include "../snmp/sr_snmpmib2.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../os/techsupport.h"

/*
 * Forward References
 */
static void snmp_community(parseinfo *);
static void snmp_loghost(parseinfo *);

#define TRAP_ALL        -1L

static char *default_view = "everything";

/*
 * structure for maintaining snmpv1 configured communities
 */
#define	commtype struct comm
commtype {
    commtype *next;			/* ptr to next entry */
    char *community;			/* community string */
    char *viewname;			/* view entry name */
    int acl;				/* access control list */
    uchar access;			/* access type */
};

static queuetype snmpcommQ;

/*
 * structure for maintaining logical entities
 */
#define	enttype struct ent
enttype {
    enttype *next;			/* ptr to next entry */
    char *entity;			/* entity identifier */
};

static queuetype snmpentityQ;

/*
 * structure for maintaining snmpv1 configured nms (trap receivers)
 */
#define	nmstype struct nms
nmstype {
    nmstype *next;			/* ptr to next entry */
    uchar *community;			/* community string */
    ipaddrtype addr;			/* ip address */
    ulong flags;			/* traps allowed */
};

static queuetype snmphostQ;



/*
 * snmp_parse_config_init
 * initialize configuration goodies maintained by the parser code
 */
void
snmp_parse_config_init (void)
{
    queue_init(&snmpcommQ, 0);
    queue_init(&snmpentityQ, 0);
    queue_init(&snmphostQ, 0);
}

static const char *
snmp_priority_string (int priority)
{
    switch (priority) {
    case PRIO_HIGH: return "high";
    case PRIO_NORMAL: return "normal";
    default: return "low";
    }
}

/*
 * snmp_service
 * Parse the snmp-server command options
 * no snmp-server
 * [no] snmp-server community [str] [view [name]] [RO|RW] [N]
 * [no] snmp-server access-list N
 * snmp-server packet-size M
 * [no] snmp-server host name [str] [flags]
 * [no] snmp-server enable {trap | inform} [snmp | framerelay | tty | x25 ...]
 *      [trap-number | trap-name_string | ...]
 * [no] snmp-server trap-authentication
 * [no] snmp-server trap-timeout N
 * [no] snmp-server queue-length N
 * [no] snmp-server location str
 * [no] snmp-server contact str
 * [no] snmp-server priority {low | normal | high}
 * [no] snmp-server system-shutdown
 *   str is a required magic community string
 *   name is a viewname (see snmpv2 commands below)
 *   N is an access list for filtering incoming packets
 *   M is a packet size used to up the baby limit SNMP imposes
 *   name is a host name or address to send SNMP traps
 *   flags determine types of traps sent to host
 *
 * SNMPv2 specific snmp-server command options
 * [no] snmp-server access-policy <dstpartyname> <srcpartyname> <contextname>
 *      <privileges> 
 * [no] snmp-server context <contextname> <contextoid> <viewname> 
 * [no] snmp-server party <partyname> <partyoid>
 *      [<protocol> <address>]
 *      [packetsize <packetsize>]
 *      [local | remote]
 *      [authentication {md5 <key> [clock <clock>] [lifetime <lifetime>] |
 *			 snmpv1 <community>}]
 * [no] snmp-server view <viewname> <viewoid> {included | excluded}
 * [no] snmp-server userid <userid> [view <viewname>] [ro | rw]
 *	[password <password>]
 *
 * the "name" fields are all textual handles used to reference a given
 *	administrative record
 * the "oid" fields are text representations of an OBJECT IDENTIFIER.  These
 *	are in dotted decimal notation, with an optional text identifier.
 *	"1.3.6.1.2.1.1.1.0" == "sysDescr.0"
 * privileges is a numeric representation of access privileges
 * protocol address is a representation of the address of an entity.  The only
 *	supported protocol is "udp" with an address represented as a.b.c.d port
 * packetsize is the maximum size of a message to be sent to a given party
 * key is a 16-octet authentication key
 * community is a snmpv1-style community string
 * clock is the authentication clock of a party
 * lifetime is the lifetime of an authenticated message
 * userid is a string used to identify an approved SNMPv2 user
 * password is the user's password
 */


void snmp_service (parseinfo *csb)
{
    boolean hide_pw = FALSE;

    if (system_loading) {
	return;
    }

    if (csb->nvgen) {
	commtype *cp;
	nmstype *hp;

	if (!snmp_serving) {
	    return;
	}

	if (csb->flags & HIDE_PASSWORDS) {
	    hide_pw = TRUE;
	}

	switch (csb->which) {
	  case SNMP_DISABLE:
	    /* No NV generation */
	    break;
	  case SNMP_HOST:
	    for (hp = (nmstype *)snmphostQ.qhead; hp; hp = hp->next) {
		nv_write(TRUE, "%s %i %s",
			 csb->nv_command, hp->addr,
			 (hide_pw) ?
			 techsupport_blank_string : (char*)hp->community);
		if (hp->flags != TRAP_ALL) {
		    reg_invoke_snmp_host_nvgen(hp->flags);
                    nv_add(hp->flags & (1 << TRAP_SNMP), " snmp");
                }
	    }
	    break;
	  case SNMP_ENABLE:
	    /*
	     * NOTE: currently no handling for Informs
	     */
	    reg_invoke_Trap_cfg_nvwrt(csb);
	    break;
	    /*
	     * WARNING - the below case is ONLY for the mac security trap, and
	     * should not be used anywhere else without prior approval from the
	     * mib police!!
	     */
	  case SNMP_TRAP_FREQUENCY:
	    /*
	     * NOTE: currently only decay type
	     */
	    reg_invoke_Trap_freq_nvwrt(csb);
	    break;

	  case SNMP_COMMUNITY:
	    for (cp = (commtype *)snmpcommQ.qhead; cp; cp = cp->next) {
		nv_write(TRUE, "%s %s", csb->nv_command,
			 (hide_pw) ? techsupport_blank_string : cp->community);
		nv_add(strcmp(cp->viewname, default_view),
		       " view %s", cp->viewname);
		nv_add(cp->access == SR_READ_ONLY, " RO");
		nv_add(cp->access == SR_READ_WRITE, " RW");
		nv_add(cp->acl, " %d", cp->acl);
	    }
	    break;
	  case SNMP_TRAP_INTERFACE:
	    if (snmp_trap_idb != NULL) {
		nv_write(!(snmp_trap_idb->hwptr->status &
			   IDB_DELETED), "%s %s", csb->nv_command, 
			 snmp_trap_idb->namestring);
	    }
	    break;
	  case SNMP_PACKET:
	    nv_write(snmp_size != SNMP_MAX_MSG_SIZE_DEF, "%s %d",
		     csb->nv_command, snmp_size);
	    break;
	  case SNMP_AUTH:
	    if (snmpTrapData.snmpV2EnableAuthenTraps == SNMPv2_TRAPS_ENABLED 
			&& snmpData.snmpEnableAuthenTraps == 
			D_snmpEnableAuthenTraps_enabled)
		nv_write(TRUE, csb->nv_command);
	    else if (snmpTrapData.snmpV2EnableAuthenTraps == 
			SNMPv2_TRAPS_ENABLED) 
		nv_write(TRUE, "%s snmpv2", csb->nv_command);
	    else if (snmpData.snmpEnableAuthenTraps ==
			D_snmpEnableAuthenTraps_enabled)
	 	nv_write(TRUE, "%s snmpv1", csb->nv_command);
	    break;
	  case SNMP_TIMEOUT:
	    nv_write(snmp_def_timeout != SNMP_DEF_TIMEOUT, "%s %d",
		     csb->nv_command, snmp_def_timeout/ONESEC);
	    break;
	  case SNMP_QLEN:
	    nv_write(snmp_def_qlen != SNMP_DEF_QLEN, "%s %d",
		     csb->nv_command, snmp_def_qlen);
	    break;
	  case SNMP_LOCATION:
	    nv_write(snmp_location_string != NULL, "%s %s", 
		     csb->nv_command, snmp_location_string);
	    break;
	  case SNMP_CHASSIS:
	    nv_write(strcmp(snmp_chassis_string,
			    snmp_chassis_string_default),
		     "%s %s", csb->nv_command, snmp_chassis_string);
	    break;
	  case SNMP_CONTACT:
	    nv_write(snmp_contact_string != NULL, "%s %s",
		     csb->nv_command, snmp_contact_string);
	    break;
	  case SNMP_SHUTDOWN:
	    nv_write(snmp_shutdown, csb->nv_command);
	    break;
	  case SNMP_VIEW:
#ifdef SR_SNMPv2
	    snmp_write_views(csb->nv_command);
#endif                          /* SR_SNMPv2 */
	    break;
	  case SNMP_CONTEXT:
#ifdef SR_SNMPv2
	    snmp_write_contexts(csb->nv_command);
#endif                          /* SR_SNMPv2 */
	    break;
	  case SNMP_PARTY:
#ifdef SR_SNMPv2
	    snmp_write_parties(csb->nv_command, hide_pw);
#endif                          /* SR_SNMPv2 */
	    break;
	  case SNMP_ACCESS_POLICY:
#ifdef SR_SNMPv2
	    snmp_write_acls(csb->nv_command);
#endif                          /* SR_SNMPv2 */
	    break;
	  case SNMP_NOSPARSE:
	    nv_write(snmp_nosparse, "no %s", csb->nv_command);
	    break;
	case SNMP_PRIORITY:
	    nv_write(snmp_priority != SNMP_PRIORITY_DEF,
                     "%s %s", csb->nv_command,
                     snmp_priority_string(snmp_priority));
            break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /*
     * If turning off SNMP, destroy any handler processes
     */
    if (csb->which == SNMP_DISABLE) {
	snmp_serving = FALSE;
	reg_invoke_snmp_process(FALSE);
        reg_invoke_SnmpTrapFeProcess(FALSE);     /* remove the trap */
	return;
    }

    /*
     * Create SNMP processes if necessary
     */
    if (!snmp_serving) {
	snmp_serving = TRUE;
	reg_invoke_snmp_process(TRUE);
	reg_invoke_SnmpTrapFeProcess(TRUE);     /* trap handling */
	reg_invoke_snmp_priority(snmp_priority);
    }

    switch (csb->which) {
      case SNMP_COMMUNITY:
	snmp_community(csb);
	break;

      case SNMP_HOST:
	snmp_loghost(csb);
	break;

      case SNMP_ENABLE:
	{
	    uint	trap_subsys;
	    uint	trap_types;
	    /*
	     * NOTE- currently no handling for Informs
	     */
	    if (GETOBJ(int, 3) == SNMP_ENABLE_TRAPS) {
		trap_subsys = GETOBJ(int, 1);
		trap_types = GETOBJ(int, 2);
		if (trap_subsys == 0) {
		    /*
		     * all traps are being enabled
		     */
		    trap_subsys = TRAPS_ALL_SUBSYS_ENABLED;
		    trap_types = TRAPS_ALL_TYPES_ENABLED;
		}

		if (trap_types == 0) {
		    /*
		     * all traps for this subsystem are being enabled
		     */
		    trap_types = TRAPS_ALL_TYPES_ENABLED;
		}

		/* set each applicable trap type to to enabled or disabled */
		reg_invoke_Trap_cfg_set(csb->sense, trap_subsys, trap_types);
		if (trap_subsys & (1<<TRAP_SNMP)) {
		    if (trap_types & (1<<TRAP_SNMP_AUTHENTICATION)) {
			snmpData.snmpEnableAuthenTraps = csb->sense ?
			    D_snmpEnableAuthenTraps_enabled :
			    D_snmpEnableAuthenTraps_disabled;
			snmpTrapData.snmpV2EnableAuthenTraps = csb->sense ?
			    SNMPv2_TRAPS_ENABLED :
			    SNMPv2_TRAPS_DISABLED;
		    }
		}
	    }
	}
	break;

	/*
	 * WARNING - the below case is ONLY for the mac security trap, and
	 * should not be used anywhere else without prior approval from the
	 * mib police!!
	 */

      case SNMP_TRAP_FREQUENCY:
	{
	    uint	trap_subsys;
	    uint	trap_types;
	    /*
	     * NOTE- currently only decay
	     */
	    if (GETOBJ(int, 3) == SNMP_TRAP_FREQ_DECAY) {
		trap_subsys = GETOBJ(int, 1);
		trap_types = GETOBJ(int, 2);
		if (trap_subsys == 0) {
		    /*
		     * all traps are being enabled
		     */
		    trap_subsys = TRAPS_ALL_SUBSYS_ENABLED;
		    trap_types = TRAPS_ALL_TYPES_ENABLED;
		}

		if (trap_types == 0) {
		    /*
		     * all traps for this subsystem are being enabled
		     */
		    trap_types = TRAPS_ALL_TYPES_ENABLED;
		}

		/* set each applicable trap type to to enabled or disabled */
		reg_invoke_Trap_freq_set(csb->sense, trap_subsys, trap_types);
	    }
	}
	break;

      case SNMP_TRAP_INTERFACE:
	if (csb->sense) {
	    snmp_trap_idb = GETOBJ(idb,1);
	    if (snmp_trap_idb && snmp_trap_idb->ip_address == IPADDR_ZERO)
		 printf("\nWarning: %s does not currently have an ip address",
			snmp_trap_idb->namestring);
	} else {
	    snmp_trap_idb = NULL;
	}
	break;

      case SNMP_PACKET:
	if (csb->sense) {
	    if (GETOBJ(int,1) > SNMP_MAX_MSG_SIZE_MAX) {
		printf("\nSNMP packet size must be smaller than %d",
		       SNMP_MAX_MSG_SIZE_MAX);
	    } else {
		snmp_size = GETOBJ(int,1);
	    }
	} else {
	    snmp_size = SNMP_MAX_MSG_SIZE_DEF;
	}
	break;

      case SNMP_AUTH:
	switch (GETOBJ(int,1)) {
	case SNMP_AUTH_SNMPV1:
	    if (csb->sense)
		snmpData.snmpEnableAuthenTraps=D_snmpEnableAuthenTraps_enabled;
	    else
		snmpData.snmpEnableAuthenTraps=D_snmpEnableAuthenTraps_disabled;
	    break;
	case SNMP_AUTH_SNMPV2:
	    if (csb->sense)
		snmpTrapData.snmpV2EnableAuthenTraps = SNMPv2_TRAPS_ENABLED;
	    else
		snmpTrapData.snmpV2EnableAuthenTraps = SNMPv2_TRAPS_DISABLED;
	    break;
	default:
	    if (csb->sense) {
		snmpTrapData.snmpV2EnableAuthenTraps = SNMPv2_TRAPS_ENABLED;
		snmpData.snmpEnableAuthenTraps=D_snmpEnableAuthenTraps_enabled;
	    } else {
		snmpTrapData.snmpV2EnableAuthenTraps = SNMPv2_TRAPS_DISABLED;
		snmpData.snmpEnableAuthenTraps=D_snmpEnableAuthenTraps_disabled;
	    }
	    break;
	}
	break;

      case SNMP_TIMEOUT:
	if (csb->sense) {
	    snmp_def_timeout = GETOBJ(int,1) * ONESEC;
	} else {
	    snmp_def_timeout = SNMP_DEF_TIMEOUT;
	}
	break;

      case SNMP_QLEN:
	if (csb->sense) {
	    snmp_def_qlen = GETOBJ(int,1);
	} else {
	    snmp_def_qlen = SNMP_DEF_QLEN;
	}
	break;

      case SNMP_LOCATION:
	setstring(&snmp_location_string, GETOBJ(string,1));
	break;

      case SNMP_CHASSIS:
	setstring(&snmp_chassis_string,
		  (csb->sense) ? GETOBJ(string,1) :
		  		 snmp_chassis_string_default);
	break;

      case SNMP_CONTACT:
	setstring(&snmp_contact_string, GETOBJ(string,1));
	break;

      case SNMP_SHUTDOWN:
	snmp_shutdown = csb->sense;
	break;

      case SNMP_VIEW:
#ifdef SR_SNMPv2
	snmp_configure_view(csb);
#endif                          /* SR_SNMPv2 */
	break;

      case SNMP_CONTEXT:
#ifdef SR_SNMPv2
	snmp_configure_context(csb);
#endif                          /* SR_SNMPv2 */
	break;

      case SNMP_PARTY:
#ifdef SR_SNMPv2
	snmp_configure_party(csb);
#endif                          /* SR_SNMPv2 */
	break;

      case SNMP_ACCESS_POLICY:
#ifdef SR_SNMPv2
	snmp_configure_acl(csb);
#endif                          /* SR_SNMPv2 */
	break;

      case SNMP_NOSPARSE:
	snmp_nosparse = !(csb->set_to_default || csb->sense);
	break;

      case SNMP_PRIORITY:
        if (csb->sense) {
            snmp_priority = GETOBJ(int,1);
        } else {
            snmp_priority = SNMP_PRIORITY_DEF;
        }
	reg_invoke_snmp_priority(snmp_priority);
        break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


/*
 * snmp_int_service
 * Parse the snmp interface command options
 * [no] snmp trap-nolink  
 */

void snmp_int_service (parseinfo *csb)
{
    hwidbtype *hwidb = hwidb_or_null(csb->interface);

    if (system_loading)
	return;

    /*
     * Most snmp commands are prevented from nvgenning when 
     * snmp_serving == FALSE for the following reasons:
     * 1. to "delete" snmp commands from the config when
     *    snmp has been disabled using the [no] snmp-server configuration.
     * 2. Prevent access to uninitialised data-structures.
     *
     * The [no] snmp trap link-status needs to nvgen even if 
     * snmp_serving == FALSE for the following reasons:
     * 1. When "group-range" is configured for Group-Async interfaces,
     *    the Group-Async running-config is nvgenned to a private buffer,
     *    and for all Async interfaces in the group-range,
     *    each config command read from the buffer is applied.
     * 2. At system reload time, Group-Async commands may appear before
     *    snmp-server commands, so snmp_serving may be false at the moment
     *    but may change to TRUE shortly afterwards.
     *
     * As this command does not access any snmp-internal data structures
     * or require the snmp process, and because it is possible that
     * other general-interface configuration mechanisms may behave
     * (in the future) similar to the GroupAsync interfaces, this
     * command will be nvgenned for all interfaces even when
     * snmp_serving == FALSE, when "no snmp trap link-status is configured.
     * Change made in bugfix CSCdj13769
     */

    if (csb->nvgen) {
	switch (csb->which) {
	  case SNMP_NOLINK:
	    nv_write(hwidb->snmp_trap_nolink, "no %s", csb->nv_command); 
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case SNMP_NOLINK:
	hwidb->snmp_trap_nolink = !(csb->set_to_default || csb->sense);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


void show_snmp (parseinfo *csb)
{

    int  idx;
    char buffer[128];

    if (!snmp_serving) {
        printf("\n%%SNMP agent not enabled");
        return;
    }
    switch (csb->which) {
    case SNMP_SHOW_MIB:
        automore_enable(NULL);
        for (idx = 0; idx < NumObjects; idx++) {
	    MakeDotFromOID(&OidIndex[idx]->oid, buffer);
	    printf("\n%s", buffer);
            if (automore_quit())
                break;
        }
        automore_disable();
	break;
    case SNMP_SHOW_GLOBALS:
        if (snmp_chassis_string)
	    printf("\nChassis: %s", snmp_chassis_string);
        if (snmp_contact_string)
	    printf("\nContact: %s", snmp_contact_string);
        if (snmp_location_string)
	    printf("\nLocation: %s", snmp_location_string);
        printf("\n%u SNMP packets input"
	       "\n    %u Bad SNMP version errors"
	       "\n    %u Unknown community name"
	       "\n    %u Illegal operation for community name supplied"
	       "\n    %u Encoding errors"
	       "\n    %u Number of requested variables"
	       "\n    %u Number of altered variables"
	       "\n    %u Get-request PDUs"
	       "\n    %u Get-next PDUs"
	       "\n    %u Set-request PDUs"
	       "\n%u SNMP packets output"
	       "\n    %u Too big errors (Maximum packet size %d)"
	       "\n    %u No such name errors"
	       "\n    %u Bad values errors"
	       "\n    %u General errors"
	       "\n    %u Get-response PDUs"
	       "\n    %u SNMP trap PDUs",
	       snmpData.snmpInPkts,
	       snmpData.snmpInBadVersions,
	       snmpData.snmpInBadCommunityNames,
	       snmpData.snmpInBadCommunityUses,
	       snmpData.snmpInASNParseErrs,
	       snmpData.snmpInTotalReqVars,
	       snmpData.snmpInTotalSetVars,
	       snmpData.snmpInGetRequests,
	       snmpData.snmpInGetNexts,
	       snmpData.snmpInSetRequests,
	       snmpData.snmpOutPkts,
	       snmpData.snmpOutTooBigs, snmp_size,
	       snmpData.snmpOutNoSuchNames,
	       snmpData.snmpOutBadValues,
	       snmpData.snmpOutGenErrs,
	       snmpData.snmpOutGetResponses,
	       snmpData.snmpOutTraps);
	reg_invoke_snmp_show();
	break;
#ifdef  SR_SNMPv2
    case SNMP_SHOW_ACL:
	snmp_show_acls();
	break;
    case SNMP_SHOW_CONTEXT:
	snmp_show_contexts();
	break;
    case SNMP_SHOW_PARTY:
	snmp_show_parties();
	break;
    case SNMP_SHOW_VIEW:
	snmp_show_views();
	break;
#endif                          /* SR_SNMPv2 */

    }
}

/*
 * snmp_loghost
 * Build and delete entries of management hosts (trap receipients)
 */
static void snmp_loghost (parseinfo *csb)
{
    nmstype *lp;

    for (lp = snmphostQ.qhead; lp; lp = lp->next) {
	if (lp->addr == GETOBJ(paddr,1)->ip_addr) {
	    break;
	}
    }
    if (lp == NULL) {
	if (!csb->sense) {
	    printf("\nCannot find host %i", GETOBJ(paddr,1)->ip_addr);
	    return;
	}
	lp = malloc(sizeof(nmstype));
	if (lp == NULL) {
	    printf(nomemory);
	    return;
	}
	lp->addr = GETOBJ(paddr,1)->ip_addr;
	reg_invoke_snmp_add_host(lp->addr);
	enqueue(&snmphostQ, lp);
    } else {
	snmp_delete_trap_proxy(lp->addr, lp->community, lp->flags);
	if (!csb->sense) {
	    free(lp->community);
	    reg_invoke_snmp_delete_host(lp->addr);
	    unqueue(&snmphostQ, lp);
	    free(lp);
	    return;
	}
    }

    setstring((char **)&lp->community, GETOBJ(string,1));

    lp->flags = GETOBJ(int,1);
    if (lp->flags == 0L) {
	lp->flags = TRAP_ALL;
    }

    snmp_add_trap_proxy(lp->addr, lp->community, lp->flags);
}

/*
 * lookup_community
 * finds the record associated with the given community
 */
static commtype *
lookup_community (const char *community)
{
    commtype *comP;

    for (comP = snmpcommQ.qhead; comP; comP = comP->next) {
	if (strcmp(community, comP->community) == 0) {
	    break;
	}
    }
    return (comP);
}


/*
 * lookup_entity
 * finds the record associated with the given entity
 */
static enttype *
lookup_entity (const char *entity)
{
    enttype *entP;

    for (entP = snmpentityQ.qhead; entP; entP = entP->next) {
	if (strcmp(entity, entP->entity) == 0) {
	    break;
	}
    }
    return (entP);
}

/*
 * create_composite_community
 * creates a composite community string <community@entity>
 */
static void
create_composite_community (const char *community,
			    const char *entity,
			    char *composite)
{
    int length;

    length = strlen(community);
    memcpy(composite, community, length);
    composite[length] = '@';
    strcpy(&composite[length+1], entity);
}

/*
 * explode_composite_community
 * explodes a composite community string <community@entity>
 */
static void
explode_composite_community (const char *composite,
			     char *community,
			     char *entity)

{
    char *atsignP;
    char *fromP;
    char *toP;
    char datum;

    *community = '\0';
    *entity = '\0';
    atsignP = strrchr(composite, '@');
    fromP = (char *)composite;
    toP = community;
    while ((datum = *fromP)) {
	if (fromP == atsignP) {
	    *toP = '\0';
	    toP = entity;
	} else {
	    *toP++ = datum;
	}
	fromP++;
    }
    *toP = '\0';
}

/*
 * snmp_community
 * Build and delete entries of snmp communites
 * This function is complicated by the fact that there are two
 * different kinds of community strings, simple and composite.
 * a simple string is simply of the form <community> whereas a
 * composite string is of the form <community>@<entity>.  What
 * makes things tricky is that when a simple community is
 * configured, the cross-product of this community with all
 * known entities is implicitly configured.  But the explicit
 * configuration of a composite community overrides the implicit
 * configuration.  So for example if the system contained two
 * logical entities called "foobar" and "goober", if the user
 * configured the community "public" then party proxies are
 * created for "public", "public@foobar", and "public@goober".
 * If the user then configured the community "public@goober"
 * then the implicit proxy just created needs to be replaced with
 * a proxy that has the parameters specified when "public@goober"
 * was configured.  Later, if the user deletes "public@goober",
 * then the explicit proxy is removed, and is replaced with an
 * implicit proxy.  I told you it was complicated.
 */

static void
snmp_community (parseinfo *csb)
{
    commtype *comP;
    commtype *testP;
    enttype *entP;
    long privileges;
    char composite[128];
    char community[128];
    char entity[128];

    comP = lookup_community(GETOBJ(string,1));
    if (comP == NULL) {

	/*
	 * this is an unknown community
	 * if we are trying to delete it, then complain
	 * otherwise allocate a record for the new community
	 */
	if (!csb->sense) {
	    printf("\nCannot find community %s", GETOBJ(string,1));
	    return;
	}
	comP = malloc(sizeof(commtype));
	if (comP == NULL) {
	    return;
	}
	setstring(&comP->community, GETOBJ(string,1));
	enqueue(&snmpcommQ, comP);

	if (strchr(comP->community, '@')) {
	    /*
	     * this is a new composite community.  we need to see if
	     * an implicit configuration of this community needs
	     * to be overwritten
	     */
	    explode_composite_community(comP->community,
					community, entity);
	    testP = lookup_community(community);
	    if (testP && lookup_entity(entity)) {
		snmp_delete_community_proxy(comP->community, testP->viewname);
	    }
	}
    } else {

	/*
	 * this is a known community
	 * whether we are modifying or deleting the community
	 * we need to remove the proxy records.  note that if
	 * we are modifying the record, new proxy records will
	 * be added later below
	 */
	snmp_delete_community_proxy(comP->community, comP->viewname);
	if (strchr(comP->community, '@')) {
	    /*
	     * we are dealing with a composite community
	     * if we are modifying the community, no further
	     * work is required.  however if we are deleting
	     * an explicitly configured composite community,
	     * we need to replace it with an implicitly
	     * configured one
	     */
	    if (!csb->sense) {
		explode_composite_community(comP->community,
					    community, entity);
		testP = lookup_community(community);
		if (testP && lookup_entity(entity)) {
		    privileges = GET_MASK | GET_NEXT_MASK | BULK_MASK;
		    if (testP->access == SR_READ_WRITE)
			privileges |= SET_MASK;
		    snmp_add_community_proxy(comP->community, testP->viewname,
					     testP->acl, privileges);
		}
	    }
	} else {
	    /*
	     * we are removing proxies for a simple community.
	     * for each entity we create a composite community,
	     * and if that composite community has not been
	     * explicitly configured, we remove its proxy
	     */
	    for (entP = snmpentityQ.qhead; entP; entP = entP->next) {
		create_composite_community(comP->community,
					   entP->entity,
					   composite);
		if (lookup_community(composite) == NULL) {
		    snmp_delete_community_proxy(composite, comP->viewname);
		}
	    }
	}
	if (!csb->sense) {
	    unqueue(&snmpcommQ, comP);
	    free(comP->community);
	    free(comP->viewname);
	    free(comP);
	    return;
	}
    }

    /*
     * this is a new or modified community
     * update the view, access, and acl
     */
    if (GETOBJ(string,2)[0] == '\0') {
	setstring(&comP->viewname, default_view);
    } else {
	setstring(&comP->viewname, GETOBJ(string,2));
    }
    comP->access = GETOBJ(int,1);
    comP->acl = GETOBJ(int,2);
#ifdef SR_SNMPv2
    privileges = GET_MASK | GET_NEXT_MASK | BULK_MASK;
    if (comP->access == SR_READ_WRITE)
	privileges |= SET_MASK;
    snmp_add_community_proxy(comP->community, comP->viewname,
			     comP->acl, privileges);
    if (strchr(comP->community, '@') == NULL) {
	for (entP = snmpentityQ.qhead; entP; entP = entP->next) {
	    create_composite_community(comP->community,
				       entP->entity,
				       composite);
	    if (lookup_community(composite) == NULL) {
		snmp_add_community_proxy(composite, comP->viewname,
					 comP->acl, privileges);
	    }
	}
    }
#endif                          /* SR_SNMPv2 */
}

void
add_snmp_entity (const char *entity)
{
    enttype *entP;
    commtype *comP;
    char composite[128];
    long privileges;

    entP = lookup_entity(entity);
    if (entP)
	return;

    entP = malloc(sizeof(enttype));
    if (entP == NULL) {
	return;
    }
    setstring(&entP->entity, entity);
    enqueue(&snmpentityQ, entP);

    /*
     * need to create a composite proxy for every non-composite
     * community, provided the composte has not been explicitly
     * configured already
     */
    for (comP = snmpcommQ.qhead; comP; comP = comP->next) {
	if (strchr(comP->community, '@'))
	    continue;
	create_composite_community(comP->community, entity, composite);
	if (lookup_community(composite) == NULL) {
	    privileges = GET_MASK | GET_NEXT_MASK | BULK_MASK;
	    if (comP->access == SR_READ_WRITE)
		privileges |= SET_MASK;
	    snmp_add_community_proxy(composite, comP->viewname,
				     comP->acl, privileges);
	}
    }
}

void
delete_snmp_entity (const char *entity)
{
    enttype *entP;
    commtype *comP;
    char composite[128];

    entP = lookup_entity(entity);
    if (!entP)
	return;

    unqueue(&snmpentityQ, entP);
    free(entP->entity);
    free(entP);

    /*
     * need to remove a composite proxy for every non-composite
     * community, provided the composte was not explicitly
     * configured
     */
    for (comP = snmpcommQ.qhead; comP; comP = comP->next) {
	if (strchr(comP->community, '@'))
	    continue;
	create_composite_community(comP->community, entity, composite);
	if (lookup_community(composite) == NULL) {
	    snmp_delete_community_proxy(composite, comP->viewname);
	}
    }
}
