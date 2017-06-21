/* $Id: pt_slip_ppp_parse.c,v 3.7.10.2 1996/08/07 09:02:09 snyder Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_slip_ppp_parse.c,v $
 *------------------------------------------------------------------
 * P T _ S L I P _ P A R S E . C
 *
 * PT Virtual Async SLIP PPP Global command parser
 *
 * April, 1994 Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pt_slip_ppp_parse.c,v $
 * Revision 3.7.10.2  1996/08/07  09:02:09  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/03/18  21:45:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  07:19:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/07  10:41:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  17:00:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/22  05:20:30  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.6  1996/02/13  08:15:42  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.5  1996/02/07  16:14:47  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:08:26  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/16  17:53:44  fox
 * CSCdi46568:  Can not set CHAP authentication on VTYs
 * Clear CHAP/PAP boolean when setting the other boolean.
 *
 * Revision 3.2  1995/11/17  19:00:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/10  17:05:04  eschaffe
 * CSCdi37604:  Spurious memory access messages on 4500 using protocol
 * translation
 *
 * Revision 2.1  1995/06/07  22:35:28  hampton
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
#include "logger.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "address.h"
#include "../if/network.h"
#include "aaa.h"
#include "../ts/parser_defs_slip.h"
#include "parser_defs_pt.h"
#include "../wan/parser_defs_ppp.h"
#include "../ts/if_slip.h"
#include "vtyasync.h"
#include "../ui/command.h"
#include "../ts/if_async.h"
#include "../if/ether.h"
#include "../os/init.h"

/*
 * Local Storage and Definitions
 */
boolean pt_vty_async_enabled = FALSE;
boolean pt_dynamic_routing = FALSE;
boolean pt_compress_passive = FALSE;
boolean pt_compress_on = FALSE;
boolean pt_ppp_authentication = FALSE;
boolean pt_ppp_use_tacacs = FALSE;
boolean pt_ipx_pppclient = FALSE;
uchar pt_auth_types[PPP_AUTH_NUM] = { 0 };
idbtype *pt_ipx_pppclient_owner = NULL;
char pt_ppp_aaa_method[32];
short pt_keepalive_period = KEEPALIVE_DEFAULT;
int pt_vtyasync_mtu = MAXETHERSIZE;


/* PT SLIP/PPP Configuration Parse command
 *
 */
void pt_vty_async_cfg_command (parseinfo *csb)
{
    int line;
    tt_soc *vty;

    if ((!protocolconversion)) {
        if (!csb->nvgen) {
            printf(noasm);
        }
        return;
    }
    if (csb->nvgen) {
	if (pt_vty_async_enabled) {
	    nv_write(pt_vty_async_enabled, "%s", csb->nv_command);
	    nv_write(pt_dynamic_routing, "%s dynamic-routing", 
		     csb->nv_command);
	    nv_write(pt_compress_on , "%s header-compression", 
		     csb->nv_command);
	    if (pt_compress_passive)
		nv_add(pt_compress_passive, " passive");
	    if (pt_ipx_pppclient)
		nv_write(pt_ipx_pppclient, "%s ipx ppp-client %s",
			 csb->nv_command, pt_ipx_pppclient_owner->namestring);
	    nv_write(pt_ppp_authentication, "%s ppp authentication", 
		     csb->nv_command);
	    if (pt_ppp_authentication) {
		int i;
		for (i = 0; (i < PPP_AUTH_NUM) && pt_auth_types[i]; i++) {
		    nv_add((pt_auth_types[i] == PPP_AUTH_PAP), " pap");
		    nv_add((pt_auth_types[i] == PPP_AUTH_CHAP), " chap"); 
		}
		nv_add(pt_ppp_aaa_method[0] && !old_access_control, " %s",
		       pt_ppp_aaa_method);
	    }
	    nv_write(pt_ppp_use_tacacs && old_access_control,
		"%s ppp use-tacacs", csb->nv_command);
	    nv_write(KEEPALIVE_DEFAULT != pt_keepalive_period, 
		     "%s keepalive %d", csb->nv_command, pt_keepalive_period);
	    nv_write(MAXETHERSIZE != pt_vtyasync_mtu,
		     "%s mtu %d", csb->nv_command, pt_vtyasync_mtu);
	}
	return;
    }

    pt_vty_async_enabled = TRUE;

    switch (csb->which) {
      case PT_VTY_ASYNC_ENABLED:
	/* no vty-async will turn off vty-async on the box for
	 * two-steps translation.
	 */
	if (!csb->sense) {
	    pt_vty_async_enabled = FALSE;
	    pt_compress_on = FALSE;
	    pt_compress_passive = FALSE;
	    pt_dynamic_routing = FALSE;
	    bzero(pt_auth_types, PPP_AUTH_NUM);
	    pt_ppp_authentication = FALSE;
	    pt_ppp_use_tacacs = FALSE;
	    pt_keepalive_period = KEEPALIVE_DEFAULT;
	    pt_vtyasync_mtu = MAXETHERSIZE;
	    pt_ipx_pppclient = FALSE;
	    pt_ipx_pppclient_owner = NULL;
            for (line = VTYBase; line < ALLlines; line++) {
                vty = MODEMS[line];
		/* Return only vtyasync sessions to normal vty operation */
		if (vty->statbits & SLIPMODE)
		    terminate_line(vty);
		/* Disable SLIP mode on the vty line */
		vty->def_capabilities &= ~MAYSLIP;
		vty->capabilities &= ~MAYSLIP;
            }
        }
	else {
	    pt_enable_vtyasync(ALLlines);
	}
	break;
      case PT_IPX_PPPCLIENT:
	if (csb->sense) {
	    pt_ipx_pppclient = TRUE;
	    pt_ipx_pppclient_owner = GETOBJ(idb,1);
	} else {
	    pt_ipx_pppclient = FALSE;
	    pt_ipx_pppclient_owner = NULL;
        }
      case PT_HEADER_COMP:
	if (csb->sense) {
	    pt_compress_on = TRUE; 
	    /* If header-compression passive */
	    if (GETOBJ(int,1)) {
		pt_compress_passive = TRUE;
	    }
	}
	else {
	    pt_compress_passive = FALSE;
	    pt_compress_on = FALSE;
	}
	break;

      case PT_DYNAMIC_ROUTING:
	if (csb->sense) {
	    pt_dynamic_routing = TRUE;
	    for (line = VTYBase; line < ALLlines; line++) {
		vty = MODEMS[line];
		vty->slip_flags |= SLIP_DYN_ROUTING;
	    }
	}
	else {
	    pt_dynamic_routing = FALSE;
	    for (line = VTYBase; line < ALLlines; line++) {
		vty = MODEMS[line];
		vty->slip_flags &= ~SLIP_DYN_ROUTING;
	    }
	}
	break;

      case PT_PPP_AUTHENTICATION:
	if (csb->sense) {
	    int i;

	    for (i = 0; i < PPP_AUTH_NUM; i++)
		pt_auth_types[i] = csb->uiarray[i];
	    pt_ppp_authentication = TRUE;
	    if (!old_access_control) {
		char *list_name;
		switch (GETOBJ(int,2)) {
		case 0:
		case 1:
		    pt_ppp_aaa_method[0] = '\0';
		    list_name = "default";
		    break;
		case 2:
		    list_name = GETOBJ(string,1);
		    sstrncpy(pt_ppp_aaa_method, list_name,
			     sizeof(pt_ppp_aaa_method));
		    break;
		default:
		    buginf("\n%% PPP: AAA: bad value in authentication list.");
		    list_name = NULL;
		    break;
		}
		if ((csb->resolvemethod == RES_MANUAL) && list_name &&
		    !aaa_is_list_defined(list_name, AAA_SVC_PPP)) {
		    printf("AAA: Warning authentication list \"%s\" is not "
			   "defined for PPP.\n", list_name);
		}
	    }
	}
	else {
	    bzero(pt_auth_types, PPP_AUTH_NUM);
	    pt_ppp_authentication = FALSE;
	    pt_ppp_aaa_method[0] = '\0';
	}
	break;

      case PT_PPP_USE_TACACS:
	if (csb->sense)
	    pt_ppp_use_tacacs = TRUE;
	else
	    pt_ppp_use_tacacs = FALSE;
	break;

      case PT_KEEPALIVE:
	if (csb->sense)
	    pt_keepalive_period = GETOBJ(int,1);
	else
	    pt_keepalive_period = KEEPALIVE_DEFAULT;
	break;

      case PT_INT_MTU:
	if (csb->sense) {
	    pt_vtyasync_mtu = GETOBJ(int,1);
	}
	else
	    pt_vtyasync_mtu = MAXETHERSIZE;
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}
