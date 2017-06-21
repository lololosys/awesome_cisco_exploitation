/* $Id: x25_parser.c,v 3.9.18.7 1996/06/28 23:32:48 hampton Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_parser.c,v $
 *------------------------------------------------------------------
 *	X 2 5 _ P A R S E R . C
 * Functions specific to interfaces which are called by the new command 
 * language parser.
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: x25_parser.c,v $
 * Revision 3.9.18.7  1996/06/28  23:32:48  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.18.6  1996/05/23  07:38:47  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.9.18.5  1996/05/17  12:17:22  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.4.3  1996/05/06  00:10:58  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.9.4.2  1996/04/06  01:06:17  pmorton
 * CSCdi45577:  QLLC w/PVCs regression with QLLC in CLS.
 * Build a map for managing PVCs, so that they can be claimed either by
 * CLS or by RSRB
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.4.1  1996/04/03  22:18:11  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.18.4  1996/05/13  14:48:36  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.9.18.3  1996/04/27  06:46:13  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.18.3  1996/04/26  00:05:22  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.18.2  1996/04/17  00:05:00  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.18.1  1996/02/23  21:32:53  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.9.18.2  1996/04/09  17:37:05  swenson
 * CSCdi45936:  Unable to remove X25 address from interface
 * Branch: California_branch
 *
 * Revision 3.9.18.1  1996/03/18  22:50:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/13  02:13:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/07  11:19:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  21:56:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/07  16:18:00  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/01  06:13:12  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/16  22:53:03  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Use lapb_update_n1 as a registry instead of a function call.
 *
 * Revision 3.6  1996/01/15  04:48:09  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Dynamically update N1 value whenever there is configuration change on
 * hardware MTU, LAPB/X25 modulo, or lapb compression.
 *
 * Revision 3.5  1995/11/22  21:47:06  widmer
 * CSCdi42888:  Cannot place  at beginning of cud field for x25 host
 * command
 * add %S to print out strings that are parsed by STRING macro.
 * Change x25 cud NV generation to use %S
 * Change STRING macro to escape quotes with \"
 *
 * Revision 3.4  1995/11/21  08:27:33  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.3  1995/11/17  18:07:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:52:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:53:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/02  22:02:01  carvalho
 * CSCdi39783:  CMNS call to ethernet destination fails if LLC2 not open.
 *
 * Revision 2.4  1995/08/07  23:30:52  carvalho
 * CSCdi35985:  Issue warning when an attempt is made to modify or delete
 * an X.25 map or PVC that is not associated with the current
 * subinterface.  Also enforce limit of one destination per protocol for
 * point-to-point X.25 subinterfaces.
 *
 * Revision 2.3  1995/07/20  07:38:23  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.2  1995/06/20 20:38:58  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:22:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "name.h"
#include "parser.h"
#include "../if/network.h"
#include "../ui/parse_util.h"
#include "ieee.h"

#include "../clns/clns.h"		/* We need these to get */
#include "../clns/clns_route.h"		/* prototype files for... */
#include "../clns/clns_externs.h"	/* ... nsap_string */
#include "parser_defs_x25.h"
#include "../ip/ip_registry.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../wan/dialer_registry.h"

#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"

#include "x25.h"
#include "lapb.h"
#include "x25_address.h"
#include "x25_switch.h"
#include "x25_route.h"
#include "x25_proto.h"
#include "x25_bfe.h"
#include "x25_pbp.h"
#include "x25_tcp.h"

extern queuetype x25rpQ;
extern x25routeentrytype *x25_routing_table;

static void x25_facility_command(parseinfo *csb);
static void x25_pvc_command(parseinfo *csb);
static void x25_pvc_switch(parseinfo *csb);
static void x25_pvc_no(parseinfo *csb);
static void x25_map_command(parseinfo *csb);

static char *badvalue = "\n%%Illegal value for %s - %d";
static char *notbfex25 = "\n%%Encapsulation must be bfex25";

/*
 * power_of_2 expedites routines parsing a power of two.
 * Returns TRUE if the passed value is a power of two, FALSE otherwise.
 * If second arg is non-NULL, it points to an ushort to hold the result.
 */
static boolean power_of_2 (int value, ushort *result)
{
    int power = firstrbit(value);
    int should_be_zero = value & ~(1 << power);

    if (should_be_zero != 0) {
	return FALSE;
    }
    if (result != NULL) {
	*result = power;
    }
    return TRUE;
}

/*
 *  x25_test_set
 *  Some critical parameters can't be set while the X.25 service is up;
 *  this procedure tests if these values can be set and if the resulting
 *  configuration would be legal; if both conditions are met, the
 *  new value will be set
 */
static void x25_test_set (hwidbtype *idb, ushort *var, ushort val, ushort cur)
{
    static char *cant_change = "\nParameter change held until a RESTART event";
    leveltype level;

    *var = val;

    if (cur != val) {
	/*
	 *  don't change the operating parameter while the service is up--
	 *  in the case of CONS if a single call has come through then
	 *  x25_upstate is true since LLC2 call exists, so you must check
	 *  if the hardware interface is up
	 *
	 *  if the service is not up, change the operating parameters
	 *  only if the configuration is valid
	 *
	 *  during system initialization, restart processing is deferred,
	 *  so it's always OK to change parameters until systeminit_flag
	 *  is set.
	 */
	level = raise_interrupt_level(NETS_DISABLE);

	if (systeminit_flag &&
	    (ISCMNS_IEEE(idb) ? hw_interface_up(idb) :
				(idb->x25_upstate || idb->x25_restart))) {
	    reset_interrupt_level(level);
	    printf(cant_change);
	} else {
	    if (x25_valid_config(idb, FALSE))
		x25_valid_config(idb, TRUE);
	    reset_interrupt_level(level);
	}
    }
}

/*
 *  x25_interface_commands
 *  For "x25 ..." commands, either
 *  generate an interface configuration command if needed -or-
 *  complete the parsing of a command
 */
void x25_interface_commands (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;
    ushort conditional_pout;
    ushort conditional_pin;

    if (reg_invoke_dialer_huntgroup_member(idb)) {
	if (!csb->nvgen) {
	    printf("\nCannot change X.25 configuration of hunt group member");
	}
	return;
    }

    if (csb->nvgen) {
	switch (csb->which) {
	  case X25_MODULO:
	    nv_write(idb->x25_admin_modulo != X25_DEFAULT_MODULO,
		     "%s %d", csb->nv_command, idb->x25_admin_modulo);
	    break;
	  case X25_IPS:
	    nv_write(idb->x25_admin_pin != X25_DEFAULT_IPS,
		     "%s %d", csb->nv_command, 1 << idb->x25_admin_pin);
	    break;
	  case X25_OPS:
	    nv_write(idb->x25_admin_pout != X25_DEFAULT_OPS,
		     "%s %d", csb->nv_command, 1 << idb->x25_admin_pout);
	    break;
	  case X25_SUPPRESS_SRC:
	    nv_write(idb->x25_flags & X25_SUPPR_SRC_ADDR, csb->nv_command);
	    break;
	  case X25_SUPPRESS_DST:
	    nv_write(idb->x25_flags & X25_SUPPR_DST_ADDR, csb->nv_command);
	    break;
	  case X25_ACCEPTREV:
	    nv_write(idb->x25_flags & X25_ACCEPTREVERSE, csb->nv_command);
	    break;
	  case X25_LINKSTART:
	    nv_write((idb->x25_flags & X25_LINKRESTART) == 0,
		     "no %s", csb->nv_command);
	    break;
	  case X25_IPTOS:
	    nv_write(idb->x25_flags & X25_IPPREC, csb->nv_command);
	    break;
	  case X25_SRCADDR:
	    nv_write(idb->x25_flags & X25_SUBST_SOURCE, csb->nv_command);
	    break;
	  case X25_PADACCESS:
	    nv_write(idb->x25_flags & X25_PADMAP, csb->nv_command);
	    break;
	  case X25_ADDRESS:
	    nv_write(idb->x25_address->length, "%s %*s", csb->nv_command,
		     idb->x25_address->length, idb->x25_address->x121_addr);
	    break;
	  case X25_BFE_EMER:
	    nv_write(ISBFE(idb) &&
		     (idb->x25_bfe_mode != BFE_NEVER) &&
		     (idb->x25_bfe_mode == GETOBJ(int,1)),
		     csb->nv_command);
	    break;
	  case X25_BFE_DECISION:
	    nv_write(ISBFE(idb) &&
		     (idb->x25_bfe_mode == BFE_DECISION) &&
		     (idb->x25_bfe_decision != BFE_DECISION_NO) &&
		     (idb->x25_bfe_decision == GETOBJ(int,1)),
		     csb->nv_command);
	    break;
	  case X25_DEFAULT:
	    nv_write(idb->x25_default != LINK_ILLEGAL, "%s %s",
		     csb->nv_command, print_linktype(idb->x25_default));
	    break;
	  case X25_FACILITY:
	    x25_facility_command(csb);
	    break;
	  case X25_HIC:
	    nv_write(idb->x25_admin_hic != X25_DEFAULT_HIC,
		     "%s %d", csb->nv_command, idb->x25_admin_hic);
	    break;
	  case X25_HOC:
	    nv_write(idb->x25_admin_hoc != X25_DEFAULT_HOC,
		     "%s %d", csb->nv_command, idb->x25_admin_hoc);
	    break;
	  case X25_HOLDTIME:
	    nv_write(idb->x25_holdtime != 0,
		     "%s %d", csb->nv_command, idb->x25_holdtime);
	    break;
	  case X25_HOLDQUEUE:
	    nv_write(idb->x25_holdmax != X25_HOLDQ,
		     "%s %d", csb->nv_command, idb->x25_holdmax);
	    break;
	  case X25_HTC:
	    nv_write(idb->x25_admin_htc !=
		     (ISCMNS_IEEE(idb) ? CMNS_DEFAULT_HTC : X25_DEFAULT_HTC),
		     "%s %d", csb->nv_command, idb->x25_admin_htc);
	    break;
	  case X25_IDLE:
	    nv_write(idb->x25_idle != X25_DEFAULT_IDLE,
		     "%s %d", csb->nv_command, idb->x25_idle);
	    break;
	  case X25_LIC:
	    nv_write(idb->x25_admin_lic != X25_DEFAULT_LIC,
		     "%s %d", csb->nv_command, idb->x25_admin_lic);
	    break;
	  case X25_LOC:
	    nv_write(idb->x25_admin_loc != X25_DEFAULT_LOC,
		     "%s %d", csb->nv_command, idb->x25_admin_loc);
	    break;
	  case X25_LTC:
	    nv_write(idb->x25_admin_ltc != X25_DEFAULT_LTC,
		     "%s %d", csb->nv_command, idb->x25_admin_ltc);
	    break;
	  case X25_NVC:
	    nv_write(idb->x25_nvc != X25_DEFAULT_NVC,
		     "%s %d", csb->nv_command, idb->x25_nvc);
	    break;
	  case X25_BFE_REMOTE:
	    if (ISBFE(idb))
		x25_bfe_nvgen(csb->nv_command, idb);
	    break;
	  case X25_T10:
	    nv_write((IS_DCE(idb, x25_dce) &&
		      (idb->x25_t0 != X25_DEFAULT_DCE_T0)),
		     "%s %d", csb->nv_command, idb->x25_t0);
	    break;
	  case X25_T11:
	    nv_write((IS_DCE(idb, x25_dce) &&
		      (idb->x25_t1 != X25_DEFAULT_DCE_T1)),
		     "%s %d", csb->nv_command, idb->x25_t1);
	    break;
	  case X25_T12:
	    nv_write((IS_DCE(idb, x25_dce) &&
		      (idb->x25_t2 != X25_DEFAULT_DCE_T2)),
		     "%s %d", csb->nv_command, idb->x25_t2);
	    break;
	  case X25_T13:
	    nv_write((IS_DCE(idb, x25_dce) &&
		      (idb->x25_t3 != X25_DEFAULT_DCE_T3)),
		     "%s %d", csb->nv_command, idb->x25_t3);
	    break;
	  case X25_T20:
	    nv_write(ISCMNS_IEEE(idb) ?
		     (idb->x25_t0 != CMNS_DEFAULT_DXE_T0) :
		     (IS_DTE(idb, x25_dce) &&
		      (idb->x25_t0 != X25_DEFAULT_DTE_T0)),
		     "%s %d", csb->nv_command, idb->x25_t0);
	    break;
	  case X25_T21:
	    nv_write(ISCMNS_IEEE(idb) ?
		     (idb->x25_t1 != CMNS_DEFAULT_DXE_T1) :
		     (IS_DTE(idb, x25_dce) &&
		      (idb->x25_t1 != X25_DEFAULT_DTE_T1)),
		     "%s %d", csb->nv_command, idb->x25_t1);
	    break;
	  case X25_T22:
	    nv_write(ISCMNS_IEEE(idb) ?
		     (idb->x25_t2 != CMNS_DEFAULT_DXE_T2) :
		     (IS_DTE(idb, x25_dce) &&
		      (idb->x25_t2 != X25_DEFAULT_DTE_T2)),
		     "%s %d", csb->nv_command, idb->x25_t2);
	    break;
	  case X25_T23:
	    nv_write(ISCMNS_IEEE(idb) ?
		     (idb->x25_t3 != CMNS_DEFAULT_DXE_T3) :
		     (IS_DTE(idb, x25_dce) &&
		      (idb->x25_t3 != X25_DEFAULT_DTE_T3)),
		     "%s %d", csb->nv_command, idb->x25_t3);
	    break;
	  case X25_TH:
	    nv_write(idb->x25_th != X25_DEFAULT_TH,
		     "%s %d", csb->nv_command, idb->x25_th);
	    break;
	  case X25_WIN:
	    nv_write(idb->x25_admin_win != X25_DEFAULT_WIN,
		     "%s %d", csb->nv_command, idb->x25_admin_win);
	    break;
	  case X25_WOUT:
	    nv_write(idb->x25_admin_wout != X25_DEFAULT_WOUT,
		     "%s %d", csb->nv_command, idb->x25_admin_wout);
	    break;
	  case X25_MAP:
	    x25_map_nvgen(csb);
	    break;
	  case X25_PVC:
	    x25_pvc_command(csb);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case X25_MODULO:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_modulo, X25_DEFAULT_MODULO,
			 idb->x25_modulo);
	} else {
	    if (GETOBJ(int,1) != X25_MOD8 && GETOBJ(int,1) != X25_MOD128) {
		printf(badvalue, "modulo", GETOBJ(int,1));
	    } else {
		x25_test_set(idb, &idb->x25_admin_modulo, GETOBJ(int,1),
			     idb->x25_modulo);
	    }
	}
	/*
	 * Update LAPB N1 value.
	 */
	reg_invoke_lapb_update_n1(idb);
	break;
      case X25_IPS:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_pin, X25_DEFAULT_IPS,
			 idb->x25_pin);
	} else {
	    if (GETOBJ(int,1) > X25MAXDATA ||
		!power_of_2(GETOBJ(int,1), &conditional_pin)) {
		printf(badvalue, "input packet size", GETOBJ(int,1));
	    } else {
		x25_test_set(idb, &idb->x25_admin_pin, conditional_pin,
			     idb->x25_pin);
	    }
	}
	break;
      case X25_OPS:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_pout, X25_DEFAULT_OPS,
			 idb->x25_pout);
	} else {
	    if (GETOBJ(int,1) > X25MAXDATA ||
		!power_of_2(GETOBJ(int,1), &conditional_pout)) {
		printf(badvalue, "output packet size", GETOBJ(int,1));
	    } else {
		x25_test_set(idb, &idb->x25_admin_pout, conditional_pout,
			     idb->x25_pout);
	    }
	}
	break;
      case X25_SUPPRESS_SRC:
        if (csb->sense) {
            idb->x25_flags &= ~X25_SUBST_SOURCE;
            idb->x25_flags |= X25_SUPPR_SRC_ADDR;
        } else {
            idb->x25_flags &= ~X25_SUPPR_SRC_ADDR;
	}
        break;
      case X25_SUPPRESS_DST:
        if (csb->sense) {
            idb->x25_flags |= X25_SUPPR_DST_ADDR;
        } else {
            idb->x25_flags &= ~X25_SUPPR_DST_ADDR;
	}
        break;
      case X25_ACCEPTREV:
        if (csb->sense) {
            idb->x25_flags |= X25_ACCEPTREVERSE;
        } else {
            idb->x25_flags &= ~X25_ACCEPTREVERSE;
	}
        break;
      case X25_LINKSTART:
        if (csb->sense) {
            idb->x25_flags |= X25_LINKRESTART;
        } else {
            idb->x25_flags &= ~X25_LINKRESTART;
	}
        break;
      case X25_IPTOS:
        if (csb->sense) {
            idb->x25_flags |= X25_IPPREC;
        } else {
            idb->x25_flags &= ~X25_IPPREC;
	}
        break;
      case X25_SRCADDR:
        if (csb->sense) {
            idb->x25_flags &= ~X25_SUPPR_SRC_ADDR;
            idb->x25_flags |= X25_SUBST_SOURCE;
        } else {
            idb->x25_flags &= ~X25_SUBST_SOURCE;
	}
        break;
      case X25_PADACCESS:
        if (csb->sense) {
            idb->x25_flags |= X25_PADMAP;
        } else {
            idb->x25_flags &= ~X25_PADMAP;
	}
        break;
      case X25_ADDRESS:
	if (csb->sense) {
	    bcopy(GETOBJ(paddr,1), idb->x25_address, sizeof(addrtype));
	} else if ((ISBFE(idb) || ISDDN(idb))) {
	    x25_ip_default(csb->interface, csb->interface->ip_address,
			   csb->interface->ip_nets_mask, FALSE, FALSE);
	} else {
	    idb->x25_address->type = ADDR_X121;
	    idb->x25_address->length = 0;
	    idb->x25_address->x121_addr[0] = '\0';
	}
	break;
      case X25_BFE_EMER:
	if (! ISBFE(idb)) {
	    printf(notbfex25);
	    return;
	}
	if (!csb->sense) {
	    idb->x25_bfe_mode = BFE_NEVER; /* default */
	} else {
	    idb->x25_bfe_mode = GETOBJ(int,1);
	}
	if (idb->x25_bfe_decision != BFE_DECISION) {
	    idb->x25_bfe_decision = BFE_DECISION_NO; /* default */
	}
	break;
      case X25_BFE_DECISION:
	if (! ISBFE(idb)) {
	    printf(notbfex25);
	    return;
	}
	if (idb->x25_bfe_mode != BFE_DECISION) {
	    printf("\n%%x25 bfe-emergency mode is not set to decision");
	    return;
	}
	if (!csb->sense) {
	    idb->x25_bfe_decision = BFE_DECISION_NO; /* default */
	} else {
	    idb->x25_bfe_decision = GETOBJ(int,1);
	}
	break;
      case X25_DEFAULT:
	if (csb->sense) {
	    idb->x25_default = GETOBJ(int,1);
	} else {
	    idb->x25_default = LINK_ILLEGAL;
	}
	break;
      case X25_FACILITY:
	x25_facility_command(csb);
	break;
      case X25_HIC:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_hic,
			 ISCMNS_IEEE(idb) ? CMNS_DEFAULT_HIC : X25_DEFAULT_HIC,
			 idb->x25_hic);
	} else {
	    x25_test_set(idb, &idb->x25_admin_hic, GETOBJ(int,1),
			 idb->x25_hic);
	}
	break;
      case X25_HOC:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_hoc,
			 ISCMNS_IEEE(idb) ? CMNS_DEFAULT_HOC : X25_DEFAULT_HOC,
			 idb->x25_hoc);
	} else {
	    x25_test_set(idb, &idb->x25_admin_hoc, GETOBJ(int,1),
			 idb->x25_hoc);
	}
	break;
      case X25_HOLDTIME:
	if (! csb->sense) {
	    idb->x25_holdtime = 0;
	} else {
	    idb->x25_holdtime = GETOBJ(int,1);
	}
	break;
      case X25_HOLDQUEUE:
	if (! csb->sense) {
	    idb->x25_holdmax = X25_HOLDQ;
	} else {
	    idb->x25_holdmax = GETOBJ(int,1);
	}
	break;
      case X25_HTC:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_htc,
			 ISCMNS_IEEE(idb) ? CMNS_DEFAULT_HTC : X25_DEFAULT_HTC,
			 idb->x25_htc);
	} else {
	    x25_test_set(idb, &idb->x25_admin_htc, GETOBJ(int,1),
			 idb->x25_htc);
	}
	break;
      case X25_IDLE:
	if (! csb->sense) {
	    idb->x25_idle = X25_DEFAULT_IDLE;
	} else {
	    idb->x25_idle = GETOBJ(int,1);
	}
	break;
      case X25_LIC:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_lic,
			 ISCMNS_IEEE(idb) ? CMNS_DEFAULT_LIC : X25_DEFAULT_LIC,
			 idb->x25_lic);
	} else {
	    x25_test_set(idb, &idb->x25_admin_lic, GETOBJ(int,1),
			 idb->x25_lic);
	}
	break;
      case X25_LOC:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_loc,
			 ISCMNS_IEEE(idb) ? CMNS_DEFAULT_LOC : X25_DEFAULT_LOC,
			 idb->x25_loc);
	} else {
	    x25_test_set(idb, &idb->x25_admin_loc, GETOBJ(int,1),
			 idb->x25_loc);
	}
	break;
      case X25_LTC:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_ltc,
			 ISCMNS_IEEE(idb) ? CMNS_DEFAULT_LTC : X25_DEFAULT_LTC,
			 idb->x25_ltc);
	} else {
	    x25_test_set(idb, &idb->x25_admin_ltc, GETOBJ(int,1),
			 idb->x25_ltc);
	}
	break;
      case X25_NVC:
	x25_map_update_if_nvc(idb,
		csb->sense ? GETOBJ(int,1) : X25_DEFAULT_NVC);
	break;
      case X25_BFE_REMOTE:
	if (! ISBFE(idb)) {
	    printf(notbfex25);
	    return;
	}
	x25_bfe_node(csb->sense, idb,
		     &GETOBJ(paddr,1)->ip_addr, &GETOBJ(paddr,2)->ip_addr);
	break;
      case X25_T10:
	if (! csb->sense) {
	    idb->x25_t0 = X25_DEFAULT_DCE_T0;
	} else {
	    idb->x25_t0 = GETOBJ(int,1);
	}
	break;
      case X25_T11:
	if (! csb->sense) {
	    idb->x25_t1 = X25_DEFAULT_DCE_T1;
	} else {
	    idb->x25_t1 = GETOBJ(int,1);
	}
	break;
      case X25_T12:
	if (! csb->sense) {
	    idb->x25_t2 = X25_DEFAULT_DCE_T2;
	} else {
	    idb->x25_t2 = GETOBJ(int,1);
	}
	break;
      case X25_T13:
	if (! csb->sense) {
	    idb->x25_t3 = X25_DEFAULT_DCE_T3;
	} else {
	    idb->x25_t3 = GETOBJ(int,1);
	}
	break;
      case X25_T20:
	if (! csb->sense) {
	    idb->x25_t0 = X25_DEFAULT_DTE_T0;
	} else {
	    idb->x25_t0 = GETOBJ(int,1);
	}
	break;
      case X25_T21:
	if (! csb->sense) {
	    idb->x25_t1 = X25_DEFAULT_DTE_T1;
	} else {
	    idb->x25_t1 = GETOBJ(int,1);
	}
	break;
      case X25_T22:
	if (! csb->sense) {
	    idb->x25_t2 = X25_DEFAULT_DTE_T2;
	} else {
	    idb->x25_t2 = GETOBJ(int,1);
	}
	break;
      case X25_T23:
	if (! csb->sense) {
	    idb->x25_t3 = X25_DEFAULT_DTE_T3;
	} else {
	    idb->x25_t3 = GETOBJ(int,1);
	}
	break;
      case X25_TH:
	if (! csb->sense) {
	    idb->x25_th = X25_DEFAULT_TH;
	} else {
	    if (GETOBJ(int,1) >= idb->x25_admin_modulo) {
		printf("\n%%Acknowledgement threshold must be < modulo");
	    } else {
		idb->x25_th = GETOBJ(int,1);
	    }
	}
	break;
      case X25_WIN:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_win, X25_DEFAULT_WIN,
			 idb->x25_win);
	} else {
	    if (GETOBJ(int,1) >= idb->x25_admin_modulo) {
		printf(badvalue, "input window", GETOBJ(int,1));
	    } else {
		x25_test_set(idb, &idb->x25_admin_win, GETOBJ(int,1),
			     idb->x25_win);
	    }
	}
	break;
      case X25_WOUT:
	if (! csb->sense) {
	    x25_test_set(idb, &idb->x25_admin_wout, X25_DEFAULT_WOUT,
			 idb->x25_wout);
	} else {
	    if (GETOBJ(int,1) >= idb->x25_admin_modulo) {
		printf(badvalue, "output window", GETOBJ(int,1));
	    } else {
		x25_test_set(idb, &idb->x25_admin_wout, GETOBJ(int,1),
			     idb->x25_wout);
	    }
	}
	break;
      case X25_MAP:
	x25_map_command(csb);
	break;
      case X25_PVC:
	x25_pvc_command(csb);
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    reg_invoke_interface_cfg_modified(idb);
}

/*
 *  x25_facility_command
 *  For "x25 facility ..." commands, either
 *  generate an interface configuration command if needed -or-
 *  complete the parsing of a command
 */
static void x25_facility_command (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;
    int throughput_in, throughput_out;
    rpoatype *rpoa;

    if (csb->nvgen) {
	switch (GETOBJ(int,10)) {
	  case FACIL_CUG:
	    nv_write(idb->x25_fcug > 0, "%s %d", csb->nv_command,
		     idb->x25_fcug);
	    break;
	  case FACIL_PACKET:
	    nv_write(idb->x25_fpin > 0 && idb->x25_fpout > 0,
		     "%s %d %d", csb->nv_command,
		     1 << idb->x25_fpin, 1 << idb->x25_fpout);
	    break;
	  case FACIL_REVERSE:
	    nv_write(idb->x25_freverse, csb->nv_command);
	    break;
	  case FACIL_RPOA:
	    if (idb->x25_rpoa != NULL) {
		nv_write(TRUE, "%s %s",
			 csb->nv_command, idb->x25_rpoa->name);
	    }
	    break;
	  case FACIL_THROUGHPUT:
	    nv_write(idb->x25_tput_in && idb->x25_tput_out, "%s %d %d",
		     csb->nv_command,
		     x25_facil_thru(idb->x25_tput_in),
		     x25_facil_thru(idb->x25_tput_out));
	    break;
	  case FACIL_TDELAY:
	    nv_write(idb->x25_tdelay > 0, "%s %d",
		     csb->nv_command, idb->x25_tdelay);
	    break;
	  case FACIL_WINDOW:
	    nv_write(idb->x25_fwin > 0 && idb->x25_fwout > 0, "%s %d %d",
		     csb->nv_command, idb->x25_fwin, idb->x25_fwout);
	    break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,10));
	    break;
	}
	return;
    }

    switch (GETOBJ(int,10)) {
      case FACIL_CUG:
	if (! csb->sense) {
	    idb->x25_fcug = 0;
	} else {
	    idb->x25_fcug = GETOBJ(int,1);
	}
	break;
      case FACIL_PACKET:
	if (! csb->sense) {
	    idb->x25_fpin = 0;
	    idb->x25_fpout = 0;
	} else {
	    if (GETOBJ(int,1) > X25MAXDATA ||
		!power_of_2(GETOBJ(int,1), NULL)) {
		printf(badvalue, "input packet size", GETOBJ(int,1));
	    } else if ((GETOBJ(int,2) > X25MAXDATA) ||
			!power_of_2(GETOBJ(int,2), NULL)) {
		printf(badvalue, "output packet size", GETOBJ(int,2));
	    } else {
		power_of_2(GETOBJ(int,1), &idb->x25_fpin);
		power_of_2(GETOBJ(int,2), &idb->x25_fpout);
	    }
	}
	break;
      case FACIL_REVERSE:
	idb->x25_freverse = csb->sense;
	break;
      case FACIL_RPOA:
	if (! csb->sense) {
	    idb->x25_rpoa = NULL;
	} else {
	    if ((rpoa = x25_rpoa_lookup(GETOBJ(string,1))) == NULL) {
		printf("\n%%Illegal RPOA name - %s", GETOBJ(string,1));
	    } else {
		idb->x25_rpoa = rpoa;
	    }
	}
	break;
      case FACIL_THROUGHPUT:
	if (! csb->sense) {
	    idb->x25_tput_in = 0;
	    idb->x25_tput_out = 0;
	} else {
	    if ((throughput_in = x25_facil_speed(idb, GETOBJ(int,1))) == 0) {
		printf(badvalue, "input throughput", GETOBJ(int,1));
	    } else if ((throughput_out =
			x25_facil_speed(idb, GETOBJ(int,2))) == 0) {
		printf(badvalue, "output throughput", GETOBJ(int,2));
	    } else {
		idb->x25_tput_in = throughput_in;
		idb->x25_tput_out = throughput_out;
	    }
	}
	break;
      case FACIL_TDELAY:
	if (! csb->sense) {
	    idb->x25_tdelay = 0;
	} else {
	    idb->x25_tdelay = GETOBJ(int,1);
	}
	break;
      case FACIL_WINDOW:
	if (! csb->sense) {
	    idb->x25_fwin = 0;
	    idb->x25_fwout = 0;
	} else {
	    if (GETOBJ(int,1) >= idb->x25_admin_modulo) {
		printf(badvalue, "input window size", GETOBJ(int,1));
	    } else {
		if (GETOBJ(int,2) >= idb->x25_admin_modulo) {
		    printf(badvalue, "output window size", GETOBJ(int,2));
		} else {
		    idb->x25_fwin = GETOBJ(int,1);
		    idb->x25_fwout = GETOBJ(int,2);
		}
	    }
	}
	break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,10));
	break;
    }
}

/*
 * copy_x25_fields
 * copy X.25 fields from one idb to another. Used to propagate info from 
 * dialer hunt groups to interfaces.
 */
void copy_x25_fields (hwidbtype *idb1, hwidbtype *idb2)
{
    if (idb1->x25_modulo != idb2->x25_modulo)
	x25_test_set(idb2, &idb2->x25_admin_modulo, idb1->x25_modulo, 
		     idb2->x25_modulo);
    if (idb1->x25_pin != idb2->x25_pin)
	x25_test_set(idb2, &idb2->x25_admin_pin, idb1->x25_pin, idb2->x25_pin);
    if (idb1->x25_pout != idb2->x25_pout)
	x25_test_set(idb2, &idb2->x25_admin_pout, idb1->x25_pout, 
		     idb2->x25_pout);
    if (idb1->x25_hic != idb2->x25_hic)
	x25_test_set(idb2, &idb2->x25_admin_hic, idb1->x25_hic, idb2->x25_hic);
    if (idb1->x25_hoc != idb2->x25_hoc)
	x25_test_set(idb2, &idb2->x25_admin_hoc, idb1->x25_hoc, idb2->x25_hoc);
    if (idb1->x25_htc != idb2->x25_htc)
	x25_test_set(idb2, &idb2->x25_admin_htc, idb1->x25_htc, idb2->x25_htc);
    if (idb1->x25_lic != idb2->x25_lic)
	x25_test_set(idb2, &idb2->x25_admin_lic, idb1->x25_lic, idb2->x25_lic);
    if (idb1->x25_loc != idb2->x25_loc)
	x25_test_set(idb2, &idb2->x25_admin_loc, idb1->x25_loc, idb2->x25_loc);
    if (idb1->x25_ltc != idb2->x25_ltc)
	x25_test_set(idb2, &idb2->x25_admin_ltc, idb1->x25_ltc, idb2->x25_ltc);
    if (idb1->x25_win != idb2->x25_win)
	x25_test_set(idb2, &idb2->x25_admin_win, idb1->x25_win, idb2->x25_win);
    if (idb1->x25_wout != idb2->x25_wout)
	x25_test_set(idb2, &idb2->x25_admin_wout, idb1->x25_wout, 
		     idb2->x25_wout);
    x25_map_update_if_nvc(idb2, idb1->x25_nvc);
    bcopy(idb1->x25_address, idb2->x25_address, sizeof(addrtype));
    idb2->lapb_dce = idb1->lapb_dce;
    idb2->x25_dce = idb1->x25_dce;
    idb2->x25_cud_pid_def = idb1->x25_cud_pid_def;
    idb2->x25_flags = idb1->x25_flags;
    idb2->x25_default = idb1->x25_default;
    idb2->x25_holdtime = idb1->x25_holdtime;
    idb2->x25_holdmax = idb1->x25_holdmax;
    idb2->x25_idle = idb1->x25_idle;
    idb2->x25_t0 = idb1->x25_t0;
    idb2->x25_t1 = idb1->x25_t1;
    idb2->x25_t2 = idb1->x25_t2;
    idb2->x25_t3 = idb1->x25_t3;
    idb2->x25_th = idb1->x25_th;
    idb2->x25_fcug = idb1->x25_fcug;
    idb2->x25_fpin = idb1->x25_fpin;
    idb2->x25_fpout = idb1->x25_fpout;
    idb2->x25_freverse = idb1->x25_freverse;
    idb2->x25_rpoa = idb1->x25_rpoa;
    idb2->x25_tput_in = idb1->x25_tput_in;
    idb2->x25_tput_out = idb1->x25_tput_out;
    idb2->x25_tdelay = idb1->x25_tdelay;
    idb2->x25_fwin = idb1->x25_fwin;
    idb2->x25_fwout = idb1->x25_fwout;

    /*
     * Copy LAPB fields too.
     */
    copy_lapb_fields(idb1, idb2);
}

/*
 *  x25_map_command
 *  complete the parsing of an encapsulation map, encapsulation PVC,
 *  PAD route/option map or CMNS route map command
 *
 *	OBJ(paddr,1) = X.121 address
 *	OBJ(paddr,N) = mapped addresses (2 <= N <= 10)
 *	OBJ(hwaddr,1) = MAC address (for CMNS map)
 *	OBJ(int,1) = VC number, 0 for non-PVC commands
 *	OBJ(int,N) = LINK_ service types (2 <= N <= 10)
 *	OBJ(int,11) = option flags
 *	OBJ(int,12), OBJ(int,13) = packet sizes
 *	OBJ(int,14), OBJ(int,15) = window sizes
 *	OBJ(int,16) = encapsulation method
 *	OBJ(int,17) = Closed User Group number
 *	OBJ(int,18) = nvc
 *	OBJ(int,19), OBJ(int,20) = throughputs
 *	OBJ(int,21) = delay
 *	OBJ(int,22) = idle time
 *	OBJ(string,1), OBJ(string,2) = NUID name and passowrd
 *	OBJ(string,1) = user format NUID
 *	OBJ(string,3) = RPOA id
 *
 *  Notes:
 *    for PAD maps, OBJ(int,2) is LINK_PAD, OBJ(paddr,2) is not defined
 *
 *    for CMNS maps, OBJ(int,2) is LINK_CMNS, OBJ(paddr,2) type is ADDR_CMNS,
 *    OBJ(hwaddr,1) type is STATION_IEEE48 (for non-serial media) or
 *    OBJ(paddr,1) type is, if defined, ADDR_X121 (for serial media)
 *
 *    for bridging maps, OBJ(int,2) is LINK_BRIDGE, OBJ(paddr,2) is not defined
 *
 *    for other encapsulation maps, OBJ(int,N) can have one of the LINK_ types
 *    {APOLLO, APPLETALK, COMPRESSED_TCP, CLNS, DECNET, IP, NOVELL, VINES, XNS}
 *    where the corresponding OBJ(paddr,N) is not defined for CLNS.
 */
#define DUPL_ADDR_CNT	1	/* IP and Compressed TCP use the same address */
static void x25_map_command (parseinfo *csb)
{
    static char *x121_required = "\n%%An X.121 address is required";
    static char *not_found = "\n%%Can't find address map";
    static char *inc_match = "\n%%Incomplete address match; cannot %s map";
    static char *pvc_explicit = "\n%%Can't implicitly %s a PVC";
    static char *define_temp = "\n%%Can't define %s; a temporary map exists";
    static char *delete_temp = "\n%%Can't delete a temporary map";
    static char *delete_str = "delete";
    static char *redefine_str = "redefine";

    hwidbtype *hwidb = csb->interface->hwptr;
    x25map *map;
    addrtype *primary_addr;
    addrtype *addr[X25_MAX_ADDRESSES_MAPPED];
    addrtype mac_addr;
    uint addr_cnt;
    uint map_services;
    int inx;
    ushort nvc;
    ushort pin, pout;
    uchar tput_in, tput_out;
    uchar nuidlen;
    uchar *nuid;
    rpoatype *rpoa;
    ushort method;
    leveltype level;
    link_t linktypes[X25_MAX_ADDRESSES_MAPPED];

    map = NULL;
    addr_cnt = 0;
    map_services = 0;

    /*
     *  validate a PVC specification (note that PVC deletion parsing is
     *  truncated after the "no x25 pvc <#>", so encapsulation PVC deletion
     *  is handled by x25_pvc_no)
     */
    if ((GETOBJ(int,1) != 0) && (! IS_LCI_PVC(hwidb, GETOBJ(int,1)))) {
	printf("\n%%PVC %d illegal for interface", GETOBJ(int,1));
	return;
    }

    /*
     *  determine the number and pointers to the primary and secondary
     *  addresses; note that the primary address may not be specified
     *  for a PVC or the "no x25 map ..." format
     */
    if (GETOBJ(int,2) == LINK_CMNS) {
	primary_addr = GETOBJ(paddr,2);
	primary_addr->type = ADDR_CMNS;	/* forced by parser/CLNS (*sigh*) */
	map_services = X25_MAPS_CMNS;

	if (ISCMNS_IEEE(hwidb)) {
	    addr_cnt = 1;
	    memset(&mac_addr, 0, sizeof(addrtype));
	    mac_addr.type = ADDR_MAC_KLUDGE;
	    mac_addr.length = ADDRLEN_MAC_KLUDGE;
	    ieee_copy(GETOBJ(hwaddr,1)->addr, mac_addr.mac_kludge_addr);
	    addr[0] = &mac_addr;
	} else {
	    /*
	     *  the X.121 address to send Calls to is optional
	     */
	    addr_cnt = 0;
	    if (ISOBJ(paddr,1) && (GETOBJ(paddr,1)->type == ADDR_X121)) {
		addr_cnt = 1;
		addr[0] = GETOBJ(paddr,1);
	    }
	}
    } else {
	primary_addr =
	    (ISOBJ(paddr,1) && (GETOBJ(paddr,1)->type == ADDR_X121)) ?
	    GETOBJ(paddr,1) : NULL;

	for (inx = 2; inx <= 2 + X25_MAX_ADDRESSES_MAPPED + DUPL_ADDR_CNT;
	     inx++) {
	    ushort service;
	    boolean is_defined;
	    addrtype *paddr;
	    int linktype;

	    linktype = LINK_ILLEGAL;
	    paddr = NULL;

	    /*
	     *  the parser macros can't be indexed (*SIGH*)
	     */
	    switch (inx) {
	      case 2:
		if ((is_defined = ISOBJ(int,2))) {
		    linktype = GETOBJ(int,2);
		    paddr = GETOBJ(paddr,2);
		}
		break;
	      case 3:
		if ((is_defined = ISOBJ(int,3))) {
		    linktype = GETOBJ(int,3);
		    paddr = GETOBJ(paddr,3);
		}
		break;
	      case 4:
		if ((is_defined = ISOBJ(int,4))) {
		    linktype = GETOBJ(int,4);
		    paddr = GETOBJ(paddr,4);
		}
		break;
	      case 5:
		if ((is_defined = ISOBJ(int,5))) {
		    linktype = GETOBJ(int,5);
		    paddr = GETOBJ(paddr,5);
		}
		break;
	      case 6:
		if ((is_defined = ISOBJ(int,6))) {
		    linktype = GETOBJ(int,6);
		    paddr = GETOBJ(paddr,6);
		}
		break;
	      case 7:
		if ((is_defined = ISOBJ(int,7))) {
		    linktype = GETOBJ(int,7);
		    paddr = GETOBJ(paddr,7);
		}
		break;
	      case 8:
		if ((is_defined = ISOBJ(int,8))) {
		    linktype = GETOBJ(int,8);
		    paddr = GETOBJ(paddr,8);
		}
		break;
	      case 9:
		if ((is_defined = ISOBJ(int,9))) {
		    linktype = GETOBJ(int,9);
		    paddr = GETOBJ(paddr,9);
		}
		break;
	      case (2 + X25_MAX_ADDRESSES_MAPPED + DUPL_ADDR_CNT):
		if ((is_defined = ISOBJ(int,10))) {
		    linktype = GETOBJ(int,10);
		    paddr = GETOBJ(paddr,10);
		}
		break;
	      default:
		is_defined = FALSE;
	    }

	    if (! is_defined)
		break;

	    if ((service = x25_map_link2service(linktype)) == 0) {
		printf("\n%%Unsupported protocol ID");
		return;
	    }
	    if (service == X25_MAPS_QLLC && paddr->type == LINK_ILLEGAL)
		service = X25_MAPS_QLLC_NATIVE;

	    if (service & map_services) {
		printf("\n%%Duplicate protocol specified");
		return;
	    }

	    /*
	     *  if we have a new service address, add it to the array
	     */
	    if ((service & X25_MAPE_NO_SVC_ADDRESS) == 0) {
		uint inx2;

		/*
		 *  verify that any two services that use the same
		 *  address type specify identical addresses
		 */
		for (inx2 = 0; inx2 < addr_cnt; inx2++) {
		    if (addr[inx2]->type == paddr->type) {
			if (bcmp(addr[inx2], paddr, addr[inx2]->length) == 0)
			    break;

			printf("\n%%Different addresses of the same type "
				"specified");
			return;
		    }
		}

		/*
		 *  add this address to the array if it isn't already mapped
		 */
		if (inx2 >= addr_cnt) {
		    linktypes[addr_cnt] = linktype;
		    addr[addr_cnt++] = paddr;
		}
	    }

	    map_services |= service;
	}
    }

    if (primary_addr && (primary_addr->type == ADDR_CMNS)) {
	/*
	 *  CMNS maps are strange; they're really routing information, so
	 *  they can be added if the NSAP isn't defined anywhere, but can
	 *  only be deleted from the correct interface
	 */

	if (!x25_routing) {
	    printf(not_routing_x25_msg);
	    return;
	}

	map = x25_map_find(NULL, primary_addr, X25_MATCH_EXACT, X25_MAPS_CMNS);

	if (! csb->sense) {
	    if (map == NULL) {
		printf(not_found);
	    } else if (((addr_cnt + 1) != map->map_addr_count) ||
		       (addr_cnt && ! x25_map_addr_match(map, addr[0]))) {
		printf(inc_match, delete_str);
	    } else {
		x25_map_delete(map, TRUE);
	    }
	} else {
	    if (map) {
		/*
		 *  see if we can convert a temporary map to permanent
		 */
		if ((map->map_status != X25_MAP_PERM) &&
		    (addr_cnt == 1) &&
		    (x25_map_addr_match(map, addr[0]))) {
		    map->map_status = X25_MAP_PERM;
		} else {
		    printf("\n%%NSAP already mapped on interface %s",
			   map->map_idb->namestring);
		}
	    } else if (addr_cnt &&
		       (addr[0]->type == ADDR_MAC_KLUDGE) &&
		       (x25_map_find(hwidb, addr[0],
				     X25_MATCH_EXACT, X25_MAPS_CMNS))) {
		printf("\n%%MAC already mapped");
	    } else if ((map = x25_map_create(csb->interface, X25_MAPS_CMNS,
					     addr_cnt + 1, primary_addr,
					     addr[0]))) {
		map->map_status = X25_MAP_PERM;
	    } else {
		printf(nomemory);
	    }
	}

	return;
    }

    /*
     *  if no X.121 address was specified, this must be a PVC or the
     *  deletion of a map; require an X.121 address for the encapsulated
     *  protocols that don't take a protocol-specific address (CLNS and
     *  bridging) or a PAD specification
     */
    if ((primary_addr == NULL) &&
	(map_services & X25_MAPE_NO_SVC_ADDRESS)) {
	printf(x121_required);
	return;
    }

    /*
     *  determine if we have a valid specification of an existing
     *  (non-transient) map
     */

    map = x25_map_find(hwidb, addr_cnt ? addr[0] : primary_addr,
			X25_MATCH_EXACT, map_services);

    if (map &&
	(map->map_status & X25_MAP_CONFIGURED_M) != X25_MAP_CONFIGURED_V) {
	printf((csb->sense) ? define_temp : delete_temp,
	       (GETOBJ(int,1) == 0) ? "map" : "PVC");
	return;
    }

    if (map == NULL) {
	/*
	 *  no existing map
	 */

	if (! csb->sense) {
	    printf(not_found);
	    return;
	}

	if (primary_addr == NULL) {
	    printf(x121_required);
	    return;
	}

	/*
	 *  limit point-to-point subinterfaces to one destination per protocol;
	 *  make sure none of the parsed addresses are already mapped
	 */

	for (inx = 0; map_services && (inx < addr_cnt); inx++) {
	    if ((csb->interface->subif_link_type == SUBIF_TYPE_P2P) &&
		(x25_map_type_check(hwidb, csb->interface,
				    linktypes[inx]) != NULL)) {
		printf("\n%%Point-to-point interface %s map already defined",
			link2name(linktypes[inx]));
		return;
	    }

	    if ((map = x25_map_find(hwidb, addr[inx],
				    X25_MATCH_EXACT, map_services)))
		map_services = 0;
	}
    } else {
	/*
	 *  the user has specified an existing (non-transient) map
	 */

	if (map->map_status == X25_MAP_PVC) {
	    /*
	     *  this routine is not invoked for deletion of a PVC, so if
	     *  this is a PVC, so the negative sense means the user is
	     *  trying to delete it using the "no x25 map ..." form
	     */
	    if (! csb->sense) {
		printf(pvc_explicit, delete_str);
		return;
	    }

	    /*
	     *  prevent re-definition of a PVC using the "x25 map ..." form
	     */
	    if (GETOBJ(int,1) == 0) {
		printf(pvc_explicit, redefine_str);
		return;
	    }
	} else if ((map_services & X25_MAPE_NO_MAP_TO_PVC) &&
		   GETOBJ(int,1)) {
	    printf("\n%%Can't re-define as a PVC");
	    return;
	}

	if (map->map_idb != csb->interface) {
	    printf("\n%%Wrong (sub)interface; this %s is configured on %s",
		(GETOBJ(int,1) == 0) ? "map" : "PVC",
		map->map_idb->namestring);
	    return;
	}

	/*
	 *  get the X.121 address if it wasn't specified, and
	 *  make sure the mapped services and addresses are identical
	 */
	if (primary_addr == NULL)
	    primary_addr = &map->map_address[X25_MAP_PRIMARY_ADDR_INX].addr;
	else if (! x25_map_addr_match(map, primary_addr))
	    map_services = 0;

	if (map_services != map->map_services)
	    map_services = 0;

	for (inx = 0; map_services && (inx < addr_cnt); inx++) {
	    if (! x25_map_addr_match(map, addr[inx])) {
		map_services = 0;
	    }
	}
    }

    if (map_services == 0) {
	printf((GETOBJ(int,1) != 0) ? pvc_explicit : inc_match,
		csb->sense ? redefine_str : delete_str);
	return;
    }

    /*
     *  handle deletion of a map
     */
    if (! csb->sense) {
	x25_map_delete(map, TRUE);
	return;
    }

    /*
     *  we're creating or re-defining a map; validate the map parameters
     */

    if ((GETOBJ(int,11) & X25MAP_PACKETSIZE) == 0) {
	pin = hwidb->x25_pin;
	pout = hwidb->x25_pout;
    } else {
	if ((GETOBJ(int,12) > X25MAXDATA) ||
	    !power_of_2(GETOBJ(int,12), &pin)) {
	    printf(badvalue, "input packet size", GETOBJ(int,12));
	}
	if ((GETOBJ(int,13) > X25MAXDATA) ||
	    !power_of_2(GETOBJ(int,13), &pout)) {
	    printf(badvalue, "output packet size", GETOBJ(int,13));
	}
    }

    if ((GETOBJ(int,1) != 0) &&
	(x25_payload(hwidb) < (1 << ((pin > pout) ? pin : pout)))) {
	printf("\nLower layer unable to support PVC max. packet sizes of %d/%d",
		1 << pin, 1 << pout);
	return;
    }

    if ((GETOBJ(int,11) & X25MAP_PASSIVE) &&
	(map_services & X25_MAPS_COMP_TCP) == 0) {
	printf("\n%%Passive is only valid for Compressed TCP operation");
	return;
    }

    method = X25_CUD_PID_DEFAULT;
    if (GETOBJ(int,11) & X25MAP_METHOD) {
	/*
	 *  check that non-Cisco encapsulation isn't attempted for
	 *  encapsulations that require it
	 */
	if ((map_services & X25_MAPE_CISCO_ONLY) &&
	    (GETOBJ(int,16) != X25METHOD_CISCO)) {
	    printf("\n%%Cisco encapsulation required");
	    return;
	}

	switch (GETOBJ(int,16)) {
	  case X25METHOD_CISCO:
	    method = X25_CUD_PID_CISCO;
	    break;
	  case X25METHOD_IETF:
	    method = X25_CUD_PID_IETF;
	    break;
	  case X25METHOD_SNAP:
	    method = X25_CUD_PID_SNAP;
	    break;
	  case X25METHOD_MULTI:
	    method = X25_CUD_PID_MULTI;
	    break;
	}
    }

    if ((GETOBJ(int,1) == 0) && (map_services & ~X25_MAPE_SINGLE_VC)) {
	nvc = (GETOBJ(int,11) & X25MAP_NVC) ? GETOBJ(int,18) : hwidb->x25_nvc;
    } else {
	nvc = 1;
    }

    if ((GETOBJ(int,11) & X25MAP_THROUGHPUT) == 0) {
	tput_in = tput_out = 0;
    } else {
	if ((tput_in = x25_facil_speed(hwidb, GETOBJ(int,19))) == 0) {
	    printf(badvalue, "input throughput", GETOBJ(int,19));
	    return;
	}
	if ((tput_out = x25_facil_speed(hwidb, GETOBJ(int,20))) == 0) {
	    printf(badvalue, "output throughput", GETOBJ(int,20));
	    return;
	}
    }

    nuidlen = 0;
    nuid = NULL;
    switch (GETOBJ(int,11) & (X25MAP_NUID | X25MAP_NUDATA)) {
      case 0:
	break;
      case X25MAP_NUID:
	nuidlen = 3 + strlen(GETOBJ(string,1)) + strlen(GETOBJ(string,2));

	if (nuidlen >= X25_MAX_NUID_SIZE) {
	    printf("\n%%NUID field lengths must be less than %u", 
		   X25_MAX_NUID_SIZE - 3); 
	    return;
	}

	if ((nuid = malloc(nuidlen)) == NULL) {
	    printf(nomemory);
	    return;
	}

	nuid[0] = 1;		/* cisco magic cookie */
	nuid[1] = strlen(GETOBJ(string,1));
	bcopy(GETOBJ(string,1), &nuid[2], nuid[1]);
	nuid[2 + nuid[1]] = strlen(GETOBJ(string,2));
	bcopy(GETOBJ(string,2), &nuid[2 + nuid[1] + 1], nuid[2 + nuid[1]]);
	break;
      case X25MAP_NUDATA:
	if ((nuidlen = strlen(GETOBJ(string,1))) >= X25_MAX_NUID_SIZE) {
	    printf("\n%%NUDATA field length must be less than %u", 
		   X25_MAX_NUID_SIZE); 
	    return;
	}

	if ((nuid = malloc(nuidlen)) == NULL) {
	    printf(nomemory);
	    return;
	}

	bcopy(GETOBJ(string,1), nuid, nuidlen);
	break;
      default:
	printf("%%NUID and NUDATA can not both be specified");
	return;
    }

    rpoa = NULL;
    if (GETOBJ(int,11) & X25MAP_RPOA) {
	if ((rpoa = x25_rpoa_lookup(GETOBJ(string,3))) == NULL) {
	    printf("\n%%Unknown RPOA name");
	    if (nuid)
		free(nuid);
	    return;
	}
    }

    /*
     *  issue any warnings
     */

    /*
     *  advise the user that multiprotocol encapsulation will be used
     *  if it seems that it may come as a surprise (e.g. a non-Cisco
     *  method is specified for a single service that requires
     *  multiprotocol encapsulation)
     */
    if ((GETOBJ(int,11) & X25MAP_METHOD) &&
	(GETOBJ(int,16) != X25METHOD_MULTI) &&
	(GETOBJ(int,16) != X25METHOD_CISCO) &&
	(map_services & X25_MAPE_FORCE_MULTI_ENC) &&
	(x25_map_service2link(map_services) != LINK_X25_MULTI_ENC))
	printf("\n%%Multiprotocol encapsulation will be used");

    /*
     *  everything looks ok; create/re-define the map
     */

    level = raise_interrupt_level(NETS_DISABLE);

    if (map) {
	/*
	 *  map is being re-defined; initialize most options
	 */
	map->map_fac.rev = FALSE;
	map->map_flags = X25_MAPF_DEFAULT;
	map->map_fac.cug = 0;
	map->map_fac.pin = 0;
	map->map_fac.pout = 0;
	map->map_fac.win = 0;
	map->map_fac.wout = 0;
	map->map_fac.tput_in = 0;
	map->map_fac.tput_out = 0;
	if (map->map_fac.nuid) {
	    free(map->map_fac.nuid);
	    map->map_fac.nuid = NULL;
	}
	map->map_fac.cisco_nuid = TRUE;
	map->map_fac.nuid_len = 0;
	map->map_fac.rpoa = NULL;
	map->map_fac.tdelay = 0;
	map->map_cud_pid_method = X25_CUD_PID_DEFAULT;
	map->map_idle = X25_MAP_DEFAULT_IDLE;

	if (GETOBJ(int,1) && map->map_status != X25_MAP_PVC)
	    x25_map_update_nvc(map, 0);
    } else {
	/*
	 *  create an address map for up to X25_MAX_ADDRESSES_MAPPED
	 *  addresses
	 */
	map = x25_map_create(csb->interface, map_services,
			     addr_cnt + 1, primary_addr,
			     addr[0], addr[1], addr[2], addr[3], addr[4],
			     addr[5], addr[6]);
	if (!map) {
	    reset_interrupt_level(level);
	    printf(nomemory);
	    if (nuid)
		free(nuid);
	    return;
	}
    }

    map->map_status = (GETOBJ(int,1) == 0) ? X25_MAP_PERM : X25_MAP_PVC;

    /*
     *  set the map options
     */

    if (GETOBJ(int,11) & X25MAP_BROADCAST) {
	map->map_flags |= X25_MAPF_BCAST;
    }

    if (GETOBJ(int,11) & X25MAP_PACKETSIZE) {
	map->map_fac.pin = pin;
	map->map_fac.pout = pout;
    }

    if (GETOBJ(int,11) & X25MAP_WINDOWSIZE) {
	map->map_fac.win = GETOBJ(int,14);
	map->map_fac.wout = GETOBJ(int,15);
    }

    if (map_services & X25_MAPS_COMP_TCP) {
	/*
	 *  initialize passive or active operation
	 */
	if (GETOBJ(int,11) & X25MAP_PASSIVE) {
	    map->map_flags |= X25_MAPF_PASSIVE;
	    map->map_thc->flags &= ~(THC_PERMACTIVE | THC_ACTIVE);
	} else {
	    map->map_thc->flags |= THC_PERMACTIVE | THC_ACTIVE;
	}
    }

    map->map_cud_pid_method = method;

    map->map_fac.rev = (GETOBJ(int,11) & X25MAP_REVERSE);

    if (GETOBJ(int,11) & X25MAP_ACCEPT_REVERSE) {
	map->map_flags |= X25_MAPF_REV_OK;
    }

    if (GETOBJ(int,11) & X25MAP_CUG) {
	map->map_fac.cug = GETOBJ(int,17);
    }

    if (map->map_maxnvc != nvc)
	x25_map_update_nvc(map, nvc);

    map->map_fac.tput_in = tput_in;
    map->map_fac.tput_out = tput_out;

    if (GETOBJ(int,11) & X25MAP_NUID) {
	map->map_fac.cisco_nuid = TRUE;
	map->map_fac.nuid_len = nuidlen;
	map->map_fac.nuid = nuid;
    } else if (GETOBJ(int,11) & X25MAP_NUDATA) {
	map->map_fac.cisco_nuid = FALSE;
	map->map_fac.nuid_len = nuidlen;
	map->map_fac.nuid = nuid;
    }

    map->map_fac.rpoa = rpoa;

    if (GETOBJ(int,11) & X25MAP_TDELAY) {
	map->map_fac.tdelay = GETOBJ(int,21);
    }

    if ((GETOBJ(int,11) & X25MAP_NO_INCOMING) || (GETOBJ(int,1) != 0)) {
	map->map_flags |= X25_MAPF_BAR_IN;
    }

    if ((GETOBJ(int,11) & X25MAP_NO_OUTGOING) || (GETOBJ(int,1) != 0)) {
	map->map_flags |= X25_MAPF_BAR_OUT;
    }

    if (GETOBJ(int,11) & X25MAP_IDLE) {
	map->map_idle = GETOBJ(int,22);
    }

    if (GETOBJ(int,11) & X25MAP_PBPCOMPRESS) {
	map->map_flags |= X25_MAPF_PBP;
    }

    /*
     *  finish processing a PVC
     */
    if (GETOBJ(int,1) != 0 ) {
	lcitype *lci;
	char *msg = NULL;

	lci = map->map_lci[0];

	if (lci == NULL) {
	    lci = x25_lci_enter(map->map_idb, GETOBJ(int,1), NULL);

	    if (lci) {
		/*
		 *  assocate the PVC with the map
		 */
		lci->lci_linktype = map->map_linktype;

		if (! x25_map_add_lci(lci, map)) {
		    x25_lci_delete(lci);
		    lci = NULL;
		}
	    }
	} else {
	    msg = "\n%%Re-defining PVC";
	}

	if (lci) {
	    /*
	     *  set the flow control values
	     */
	    if (map->map_fac.win && map->map_fac.wout) {
		lci->lci_win = map->map_fac.win;
		lci->lci_wout = map->map_fac.wout;
	    } else {
		lci->lci_win = hwidb->x25_win;
		lci->lci_wout = hwidb->x25_wout;
	    }
	    if (map->map_fac.pin && map->map_fac.pout) {
		lci->lci_pin = map->map_fac.pin;
		lci->lci_pout = map->map_fac.pout;
	    } else {
		lci->lci_pin = hwidb->x25_pin;
		lci->lci_pout = hwidb->x25_pout;
	    }

	    /*
	     *  handle a change in compression usage
	     */
	    if (map->map_flags & X25_MAPF_PBP) {
		if ((! lci->lci_pbp_compress) &&
		    (! x25_pbp_init(lci, FALSE))) {
		    x25_lci_delete(lci);
		    lci = NULL;
		}
	    } else {
		if (lci->lci_pbp_compress) {
		    no_x25_pbp_compression(lci);
		}
	    }
	}

	if (lci == NULL) {
	    map->map_status = X25_MAP_TEMP;
	    x25_map_delete(map, FALSE);
	    map = NULL;
	    msg = nomemory;
	} else {
	    lci->lci_outgoing = FALSE;
	    lci->lci_idle = 0;
	    lci->lci_cud_pid_method = X25_CUD_PID_NONE;

	    /*
	     *  set the PVC's data protocol identification method
	     */
	    if (map->map_cud_pid_method == X25_CUD_PID_DEFAULT) {
		if ((map->map_service_cnt > 1) ||
		    ((map->map_services & X25_MAPE_FORCE_MULTI_ENC) &&
		     (map->map_idb->hwptr->x25_cud_pid_def !=
			X25_CUD_PID_CISCO)))
		    lci->lci_data_pid_method = X25_DATA_PID_MULTI;
		else
		    lci->lci_data_pid_method = X25_DATA_PID_NONE;
	    } else {
		switch (GETOBJ(int,16)) {
		  case X25METHOD_CISCO:
		  case X25METHOD_IETF:
		    if (map->map_service_cnt == 1) {
			lci->lci_data_pid_method = X25_DATA_PID_NONE;
			break;
		    }
		    /* FALL THROUGH */
		  case X25METHOD_MULTI:
		    lci->lci_data_pid_method = X25_DATA_PID_MULTI;
		    break;
		  case X25METHOD_SNAP:
		    lci->lci_data_pid_method = X25_DATA_PID_SNAP;
		    break;
		}
	    }

	    /*
	     *  Reset the PVC if needed
	     */
	    if (hwidb->x25_upstate && (hwidb->x25_state == X25_R1)) {
		x25_sendcause(hwidb, lci, X25_RESET,
			      X25_RESET_NETWORK_OPERATIONAL, X25_DIAG_NO_INFO,
			      NULL);
	    }
	}

	reset_interrupt_level(level);
	if (map != NULL) {
	    reg_invoke_media_x25_pvc_setup(map->map_linktype, map, lci, TRUE);
	}
	if (msg)
	    printf(msg);
    } else {
	reset_interrupt_level(level);
    }
}

/*
 *  x25_global_command
 *  For global X.25 commands, either
 *  generate a class of configuration commands -or-
 *  complete the parsing of a command
 */
void x25_global_commands (parseinfo *csb)
{
    rpoatype *rp;
    hwidbtype *idb;
    int i;

    if (csb->nvgen) {
	switch (csb->which) {
	  case X25_XOT_CONFIRM_SVC_RESET:
	    nv_write(x25_xot_confirm_svc_reset, csb->nv_command);
	    break;
	  case X25_RPOA:
	    for (rp = (rpoatype *)x25rpQ.qhead; rp != NULL; rp = rp->next) {
		nv_write(TRUE, "%s %s", csb->nv_command, rp->name);
		for (i = 0; i < rp->nrpoa; i++) {
		    nv_add(TRUE, " %d", rp->rpoa[i]);
		}
	    }
	    break;
	  case X25_ROUTING:
	    if (x25_routing) {
		nv_write(TRUE, csb->nv_command);
		nv_add(x25_tcp_use_if_defs, " tcp-use-if-defs");
	    }
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case X25_XOT_CONFIRM_SVC_RESET:
	if (!csb->sense) {
	    x25_xot_confirm_svc_reset = FALSE;
	} else {
	    x25_xot_confirm_svc_reset = TRUE;
	}
	break;
      case X25_RPOA:
	if (! csb->sense) {
	    rp = x25_rpoa_lookup(GETOBJ(string,1));
	    if (rp == NULL) {
		printf("\n%%Can't find RPOA name to delete");
	    } else {
		unqueue(&x25rpQ, rp);
		FOR_ALL_HWIDBS(idb) {
		    if ((is_x25(idb) || ISCMNS_IEEE(idb))
			&& idb->x25_rpoa == rp) {
			idb->x25_rpoa = NULL;
		    }
		}
		x25_map_flush_rpoa(rp);
		free(rp->name);
		free(rp);
	    }
	} else {
	    if (x25_rpoa_lookup(GETOBJ(string,1)) != NULL) {
		printf("\n%%RPOA name already known");
	    } else {
		rp = malloc(sizeof(rpoatype) + GETOBJ(int,1) * sizeof(short));
		if (rp == NULL) {
		    return;
		}
		setstring(&rp->name, GETOBJ(string,1));
		for (i = 0; i < GETOBJ(int,1); i++) {
		    rp->rpoa[i] = csb->uiarray[i];
		}
		rp->nrpoa = GETOBJ(int,1);
		enqueue(&x25rpQ, rp);
	    }
	}
	break;	
      case X25_ROUTING:
	if (! csb->sense) {
	    x25_routing = FALSE;
	} else {
	    x25_routing = TRUE;
	    x25_tcp_use_if_defs = GETOBJ(int,1);
	    reg_invoke_ip_tcpdriver_start();
	}
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}
/*
 *  x25_routing_command
 *  For X.25 routing commands, either
 *  generate the configuration commands -or-
 *  complete the parsing of a command
 */

void x25_routing_commands (parseinfo *csb)
{
    x25routeentrytype *pt;
    uchar *caddr = NULL;
    uchar *faddr = NULL;
    uchar *taddr = NULL;
    uchar *cud = NULL;
    nametype *name;
    short j;
    int len;

    if (csb->nvgen) {
	switch (csb->which) {
	  case X25_ROUTE:
	    if (! x25_routing) {
		return;
	    }
	    for (pt = x25_routing_table; pt; pt = pt->next) {
		if (pt->type == X25_ROUTE_IPPT) {
		    continue;
		}
		nv_write(TRUE, "%s %s", csb->nv_command, pt->x121);
		nv_add((boolean)pt->repsrc, " substitute-source %s",
		       pt->repsrc);
		nv_add((boolean)pt->repdst, " substitute-dest %s", pt->repdst);
		nv_add((boolean)pt->cud, " cud %S", pt->cud);
		switch (pt->type) {
		  case X25_ROUTE_IF:
		    nv_add(TRUE, " interface %s", pt->idb->namestring);
		    x25_switch_tcp_nvparms(pt);
		    break;
		  case X25_ROUTE_IPADDR:
		    nv_add(TRUE, " ip");
		    for (j=0; j < pt->ipaddr_max; j++)
			nv_add(TRUE, " %i", pt->ipaddr[j]);
		    if (pt->source_idb)
			nv_add(TRUE, " xot-source %s",
			       pt->source_idb->namestring);
		    x25_switch_tcp_nvparms(pt);
		    break;
		  case X25_ROUTE_ALIAS:
		    nv_add(TRUE, " alias %s", pt->idb->namestring);
		    break;
		  case X25_ROUTE_FUNC:
		    nv_add(TRUE, " function");
		    nv_add(((int)pt->data == X25_FUNC_CHARGEN), " chargen");
		    nv_add(((int)pt->data == X25_FUNC_DISCARD), " discard");
		    nv_add(((int)pt->data == X25_FUNC_ECHO),    " echo");
		    nv_add(((int)pt->data == X25_FUNC_HELP),    " help");
		    break;
		}
	    }
	    break;
	  case X25_HOST:
	    for (name = (nametype *) nameQ.qhead; name != NULL;
		 name = name->next) {
		if (name->flags & NAM_PAD) {
		    int i;
		    nv_write(TRUE, "%s %s", csb->nv_command, name->name);
		    for (i = 0; i < MAX121ADDRESSES; i++) {
			nv_add(name->v.x121.adr[i].length," %s",
			       name->v.x121.adr[i].addr.x121_address);
		    }
		    nv_add((boolean)name->v.x121.userdata, " cud %S",
			   name->v.x121.userdata);
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
      case X25_ROUTE:
	setstring((char **)&caddr, GETOBJ(string,1));
	if (!x25_routing) {
	    printf(not_routing_x25_msg);
	} else if (! csb->sense) {
	    x25_add_route(csb->sense, caddr, NULL, NULL, NULL, NULL,
			  0, 0, GETOBJ(int,11), 0, 0L, 0L, 0L, 0L, 0L, 0L,
			  0, 0);
	} else {
	    if (GETOBJ(int,2) != X25_ROUTE_IPADDR &&
		GETOBJ(idb,1) != NULL &&
		GETOBJ(idb,1) != nullidb &&
		!is_x25(GETOBJ(idb,1)->hwptr) &&
		!ISCMNS_IEEE(GETOBJ(idb,1)->hwptr)) {
		printf("\n%%Must be running X.25 or CMNS - %s",
		       GETOBJ(idb,1)->namestring);
		free(caddr);
		return;
	    }
	    if (GETOBJ(string,2)[0] != '\0') {
		setstring((char **)&faddr, GETOBJ(string,2));
	    }
	    if (GETOBJ(string,3)[0] != '\0') {
		setstring((char **)&taddr, GETOBJ(string,3));
	    }
	    if (GETOBJ(string,4)[0] != '\0') {
		setstring((char **)&cud, GETOBJ(string,4));
	    } else {
		setstring((char **)&cud, "");
	    }
	    if (GETOBJ(int,2) == X25_ROUTE_FUNC) {
		x25_service_start(); 
	    }
	    x25_add_route(csb->sense, caddr, cud, faddr, taddr, GETOBJ(idb,1),
		(uchar *)GETOBJ(int,3), GETOBJ(int,2), GETOBJ(int,11),
		GETOBJ(int,8), GETOBJ(paddr,1)->ip_addr, 
		GETOBJ(paddr,2)->ip_addr, GETOBJ(paddr,3)->ip_addr,  
		GETOBJ(paddr,4)->ip_addr, GETOBJ(paddr,5)->ip_addr, 
		GETOBJ(paddr,6)->ip_addr,
		GETOBJ(int,5) * ONESEC, GETOBJ(int,6));
	}
	break;
      case X25_HOST:
	name = name_cache_lookup(GETOBJ(string,1), NULL, NAM_ANY);
	if (! csb->sense || name != NULL) {
	    /* If host exists or [no] option, flush the name entry */
	    name_delete(GETOBJ(string,1), NAM_PAD);
	}
	if (csb->sense) {
	    len = strlen(GETOBJ(string,1)) + 1;
	    name = malloc(sizeof(nametype) + len);
	    if (name == NULL) {
		return;
	    }
	    sstrncpy(name->name, GETOBJ(string,1), len);
	    name->flags = NAM_PERMANENT + NAM_PAD + NAM_ADDRVALID;
	    sstrncpy((char *) name->v.x121.adr[0].x121_addr,
		     (char *) GETOBJ(paddr,1)->x121_addr,
		     ADDRLEN_X121);
	    name->v.x121.adr[0].length = GETOBJ(paddr,1)->length;
	    if (GETOBJ(string,2)[0] != '\0') {
		setstring((char **)& name->v.x121.userdata, GETOBJ(string,2));
	    }
	    enqueue(&nameQ, name);
	}
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/* These functions do not follow normal usage convensions.
 *
 * Switched PVC specification:	x25_pvc_switch
 * 'no' PVC specification:	x25_pvc_no
 * config. generation of PVCs:	x25_pvc_nvgen
 */

/*
 *  x25_pvc_command
 *  Dispatch PVC commands
 */
static void x25_pvc_command (parseinfo *csb)
{
    hwidbtype *hwidb = csb->interface->hwptr;

    if (csb->nvgen) {
	x25_pvc_nvgen(csb);
	return;
    }

    if (!IS_LCI_PVC(hwidb, GETOBJ(int,1))) {
	printf("\n%%PVC LCI illegal - %d", GETOBJ(int,1));
	return;
    }

    if (!csb->sense)
	x25_pvc_no(csb);
    else
	x25_pvc_switch(csb);
}	

/************************************************************************
 *  x25_pvc_switch() handles configuration commands for local and remote
 *  switched PVCs; the two command forms are,
 *
 *  when OBJ(int,2) is set to X25_ROUTE_IPADDR):
 *	x25 pvc OBJ(int,1) tunnel OBJ(paddr,1) ...
 *	 ... interface serial OBJ(string,1) SWITCH-SPEC
 *
 *  and when OBJ(int,2) is set to X25_ROUTE_IF:
 *	x25 pvc OBJ(int,1) interface OBJ(idb,1) SWITCH-SPEC
 *
 *  where SWITCH-SPEC is:
 *    pvc OBJ(int,3) FLOW-SPEC
 *
 *  and FLOW-SPEC is:
 *    [packetsize OBJ(int,4) OBJ(int,5)] [windowsize OBJ(int,6) OBJ(int,7)]
 */
static void x25_pvc_switch (parseinfo *csb)
{
    idbtype *swidb = csb->interface;
    hwidbtype *idb = swidb->hwptr;
    hwidbtype *otheridb = NULL;
    lcitype *lci;
    lcitype *otherlci = NULL;
    x25routeentrytype *pt;
    ushort pin = 0;
    ushort pout = 0;

    /*
     *  make sure they gave us a valid VC
     */
    if (!IS_LCI_PVC(idb, GETOBJ(int,1))) {
	printf("\nInvalid VC number for a PVC");
	return;
    }

    if (!x25_routing) {
	printf("\nX.25 routing is not enabled");
	return;
    }

    /*
     *	validate the flow control parameters
     */
    if (ISOBJ(int,4)) {
	if (GETOBJ(int,4) > X25MAXDATA || !power_of_2(GETOBJ(int,4), &pin)) {
	    printf(badvalue, "input packet size", GETOBJ(int,4));
	    return;
	}
	if (GETOBJ(int,5) > X25MAXDATA || !power_of_2(GETOBJ(int,5), &pout)) {
	    printf(badvalue, "output packet size", GETOBJ(int,5));
	    return;
	}
    }
    if (ISOBJ(int,6)) {
	if (GETOBJ(int,6) >= idb->x25_modulo) {
	    printf(badvalue, "input window size", GETOBJ(int,6));
	    return;
	}
	if (GETOBJ(int,7) >= idb->x25_modulo) {
	    printf(badvalue, "output window size", GETOBJ(int,7));
	    return;
	}
    }

    /*
     *  prevent invalid configurations of a locally switched PVC
     */
    if (ISOBJ(idb,1)) {
	otheridb = GETOBJ(idb,1)->hwptr;

	if (otheridb == idb && GETOBJ(int,3) == GETOBJ(int,1)) {
	    printf("\n%%Invalid self-connecting PVC");
	    return;
	} else if (!is_x25(otheridb) ||
		   (otherlci = x25_lci_lookup(otheridb,
					GETOBJ(int,3), NULL)) == NULL) {
	    ;	/* EMPTY; the other interface may not be configured yet */
	} else if (otherlci->lci_rtp == NULL ||
		   otherlci->lci_swtype != X25_ROUTE_IF ||
		   idb != otherlci->lci_rtp->idb->hwptr ||
		   GETOBJ(int,1) != (int)otherlci->lci_rtp->pvc_dest_vc) {
	    printf("\n%%Invalid dest. PVC configuration");
	    return;
	}
    } else {
	/*
	 *  validate the remote PVC interface specification
	 */
	if (/*strlen("Serial")*/ 6 + strlen(GETOBJ(string,1)) > 16) {
	    printf("\n%%Invalid dest. PVC interface Serial%s",
		   GETOBJ(string,1));
	    return;
	}
    }
    
    /*
     *	set up the PVC
     */

    if ((lci = x25_pvc_setup(swidb, GETOBJ(int,1))) == NULL)
	return;

    lci->lci_linktype = (ISOBJ(idb,1)) ? LINK_X25 : LINK_X25TCP;
    lci->lci_swtype = GETOBJ(int,2);

    if ((lci->lci_rtp == NULL) &&
	((lci->lci_rtp = malloc(sizeof(x25routeentrytype))) == NULL)) {
	x25_lci_delete(lci);
	printf(nomemory);
    }
    pt = (x25routeentrytype *) lci->lci_rtp;

    /*
     *  if the PVC is currently connected, kill it
     */
    otherlci = ((lci->lci_swtype == X25_ROUTE_IF) && lci->lci_swdata) ?
	lci->lci_swdata : NULL;
    x25_flush_call(lci, X25_RESET_NETWORK_OUT_OF_ORDER,
		   X25_DIAG_LINK_OUT_OF_ORDER, FALSE);

    /*
     *	set the flow control parameters
     */

    if (pin) {
	lci->lci_pin = pin;
	lci->lci_pout = pout;
    } else {
	lci->lci_pin = idb->x25_pin;
	lci->lci_pout = idb->x25_pout;
    }

    if (ISOBJ(int,6)) {
	lci->lci_win = GETOBJ(int,6);
	lci->lci_wout = GETOBJ(int,7);
    } else {
	lci->lci_win = idb->x25_win;
	lci->lci_wout = idb->x25_wout;
    }

    /*
     *	fill in the remaining PVC configuration
     */

    pt->type = lci->lci_swtype;
    pt->pvc_dest_vc = GETOBJ(int,3);

    /*
     *  reset the PVC status and stop a connect timer
     */
    lci->lci_pvc_status = X25_PVC_DOWN;
    if (TIMER_RUNNING(lci->lci_timer) &&
	(lci->lci_state != X25_D2 && lci->lci_state != X25_D3)) {
	p_unqueue(&x25tQ, lci);
	TIMER_STOP(lci->lci_timer);
    }

    if (otheridb == NULL) {
	/*
	 *  remote (tunneled) PVC
	 */

	/* all s/w that can tunnel PVCs will do RESET confirmation over TCP */
	lci->lci_confirm_tcp_reset = TRUE;
	pt->ipaddr[0] = GETOBJ(paddr,1)->ip_addr;
	pt->ipaddr_max = 1;
	sprintf(pt->pvc_remote_if, "Serial%s", GETOBJ(string,1));
	pt->pvc_remote_if_type_len = 6;
	pt->pvc_remote_if_len =
		pt->pvc_remote_if_type_len + strlen(GETOBJ(string,1));
	pt->idb = NULL;
    } else {
	pt->ipaddr_max = 0;
	pt->idb = GETOBJ(idb,1);
    }

    /*
     *  if the X.25 service is up, try to start the PVC(s)
     */
    if (idb->x25_upstate && (idb->x25_state == X25_R1)) {
	if (otheridb == NULL) {
	    /*
	     *  for a remote (tunneled) PVC, start the connect timer
	     */
	    if (!TIMER_RUNNING(lci->lci_timer)) {
		if (x25_set_pvc_con_time(lci))
		    x25_addtimer(lci);
	    }
	} else {
	    /*
	     *  try to connect the locally switched PVC
	     */
	    x25_pvc_retry(lci);

	    /*
	     *  try to connect the other half of a broken connection to
	     *  determine its new status
	     */
	    if (otherlci->lci_pvc_status != X25_PVC_UP) {
		x25_pvc_retry(otherlci);
	    }
	}
    }
}

/*
 *  x25_pvc_no
 *  Delete a PVC
 */
static void x25_pvc_no (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;
    lcitype *lci;
    lcitype *otherlci;
    boolean isbusy;

    if ((lci = x25_lci_lookup(idb, GETOBJ(int,1), NULL)) == NULL)
	printf("\n%%VC %d not found", GETOBJ(int,1));
    else if (!IS_LCI_PVC(idb, GETOBJ(int,1)))
	printf("\n%%VC %d is not a PVC", GETOBJ(int,1));
    else if (lci->lci_pvc_status == X25_PVC_TCP_TRYING)
	printf("\n%%PVC %d is trying to establish a TCP connection; "
		"cannot be deleted at this time", GETOBJ(int,1));
    else if (lci->lci_idb != csb->interface)
	printf("\n%%Wrong (sub)interface; PVC %d is configured on %s",
		GETOBJ(int,1), lci->lci_idb->namestring);
    else {
	if (lci->lci_map) {
	    reg_invoke_media_x25_lci_busy(lci->lci_linktype,
		&lci->lci_map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr,
		&isbusy);
	    if (isbusy) {
		printf("\n%%X25: can't delete an active X.25 PVC");
		return;
	    }
	    reg_invoke_media_x25_pvc_setup(lci->lci_linktype,
					   lci->lci_map, lci,
					   csb->sense);
	}

	otherlci =
	    (lci->lci_linktype == LINK_X25 && lci->lci_swtype == X25_ROUTE_IF) ?
		lci->lci_swdata : NULL;

	if (lci->lci_map)
	    x25_map_delete(lci->lci_map, FALSE);

	x25_call_remove(lci, X25_RESET_NETWORK_OUT_OF_ORDER,
			X25_DIAG_LINK_OUT_OF_ORDER);

	/*
	 *  try to connect the other half of a broken connection to
	 *  determine its new status
	 */
	if (otherlci)
	    x25_pvc_retry(otherlci);
    }
}
