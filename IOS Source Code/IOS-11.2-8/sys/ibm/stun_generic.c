/* $Id: stun_generic.c,v 3.5.18.4 1996/06/28 23:06:43 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ibm/stun_generic.c,v $
 *------------------------------------------------------------------
 * stun_generic.c -- GENERIC protocol support, for user to 
 * define the protocols running on the line.
 *
 * March 8, 1991 - Joel P. Bion
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stun_generic.c,v $
 * Revision 3.5.18.4  1996/06/28  23:06:43  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.18.3  1996/06/25  21:38:25  fbordona
 * CSCdi60992:  stun schema cnt offset 0 length 1 format hexidecimal
 *              Fix typo causing error on reload.
 * Branch: California_branch
 *
 * Revision 3.5.18.2  1996/05/17  11:21:53  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.2  1996/04/26  07:54:49  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.4.1  1996/04/03  14:35:07  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.18.1  1996/03/18  20:12:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  09:42:49  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  00:55:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:12:10  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:03:35  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/11/17  09:24:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:36:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:46:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/08  16:47:25  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.2  1995/07/06 01:32:56  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.1  1995/06/07 20:49:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_stun.h"
#include "../ip/ip.h"
#include "ibm.h"
#include "ibm_externs.h"
#include "ibm_public.h"

stun_protocol_info generic_protocol_info;

/*
 * Print out a generic frame.
 */
static void stun_generic_print_frame (
    paktype *pak,
    int group,
    char *prefix,
    hwidbtype *idb)
{
    char *addrp;
    stun_protocol_info *gpi;
    char buff[MAX_PEER_NAME];
    int i;
    int size = pak->datagramsize;
    int c;
    uchar addr = 0;
    char the_data[21];
    char the_time[40];
      
    gpi = stun_pinfo[group];
    addrp = (char *) pak->datagramstart;

    sprint_dhms(the_time, stun_lastframe);
    GET_TIMESTAMP(stun_lastframe);

    if (!stun_print_check(group, (gpi->onlyall ? (&addr) : 
				  ((uchar *) addrp +
				   gpi->addroffset))))
        return;

    if (!gpi->onlyall) {
	addrp += gpi->addroffset;
	stun_sprintf_addr((uchar *) addrp, FALSE, group, buff);
	addrp += gpi->addrlength;
	size -= gpi->addroffset + gpi->addrlength; 
    } else {
	buff[0] = '\0';
    }
    for (i = 0; ((i < 10) && (size > 0)); i++, addrp++, size--) {
	c = (*addrp & 0xf0) >> 4;
	the_data[i * 2] = ((char) ((c > 9) ? ((int) 'a' +
					      ((int) c) - 10) :
				   ((int) '0' + ((int) c))));
	c = (*addrp & 0x0f);
	the_data[i * 2 + 1] = ((char) ((c > 9) ? ((int) 'a' +
						  ((int) c) - 10) :
				       ((int) '0' + ((int) c))));
    }
    the_data[i * 2] = '\0';
    
    buginf("\nSTUN %s: %s %15s %s %s %s Data: %s", gpi->name,
	   the_time, idb->hw_namestring, prefix,
	   (*buff ? ("Addr: ") : ("")), buff, the_data);
}



static void stun_generic_long_periodic (
    hwidbtype *idb)
{
    return;
}


static void stun_generic_show (void)
{
    return;
}

static void stun_generic_nv_globals (stun_protocol_info *spi, parseinfo *csb)
{
    switch (csb->which) {
      case STUN_POLL:
        break;
      case STUN_PRIMARY:
        break;
      case STUN_SCHEMA:
        if (spi->onlyall) {
            nv_write(TRUE, "%s %s basic ", csb->nv_command, spi->name);
        } else {
            nv_write(TRUE, "%s %s offset %d length %d format %s",
		     csb->nv_command,
		     spi->name, spi->addroffset, spi->addrlength,
                     (spi->addrfmt == 'd' ? "decimal" :
                      (spi->addrfmt == 'o' ? "octal" :
                       "hexadecimal")));
        }
        break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

static void stun_generic_nv_interface (
    stun_protocol_info *spi, hwidbtype *idb)
{
}

static void stun_generic_shutint (
    hwidbtype *idb)
{
}


static int stun_generic_serial_input (
    hwidbtype *idb,
    paktype *pak,
    stunpeertype *stp)
{
    return(STUN_OP_PASS);
}


static boolean stun_generic_network_input (
    hwidbtype *idb,
    paktype *pak,
    stunpeertype *stp,
    ushort opcode)
{
    return(FALSE);
}

/*
 * Little show hook -- a few char. output for line. Header = TRUE for
 * header line. False for data.
 */
static void stun_generic_little_show_hook (
    boolean header,
    hwidbtype *idb,
    stunpeertype *stp)
{
    return;
}

static boolean stun_generic_parse_global (parseinfo *csb)
{
    return(FALSE);
}
    
static boolean stun_generic_parse_interface (parseinfo *csb)
{
    return(FALSE);
}


static boolean stun_generic_peer_change_hook (
     hwidbtype *idb,
     stunpeertype *stp,
     boolean exists)
{
    return(TRUE);
}

stun_protocol_info *stun_new_generic (
    char *name,
    int addrlen,
    int addroff,
    char addrfmt,
    boolean onlyall,
    boolean neverall,
    boolean local_ack_capable,
    boolean local_ack_required)
{
    stun_protocol_info *newg;
    
    if ((newg = malloc(sizeof(stun_protocol_info))) == NULL)
      return(NULL);
    sstrncpy(newg->name, name, NAMEDTHINGNAMELEN+1);
    newg->addrlength = addrlen;
    newg->addroffset = addroff;
    newg->addrfmt = addrfmt;
    newg->permanent = FALSE;
    newg->prototype = GENERIC_PTYPE;
    newg->onlyall = onlyall;
    newg->neverall = neverall;
    newg->uses_tg = FALSE;
    newg->local_ack_capable = local_ack_capable;
    newg->local_ack_required = local_ack_required;
    newg->serial_forward_hook = stun_generic_serial_input;
    newg->network_forward_hook = stun_generic_network_input;
    newg->shut_interface = stun_generic_shutint;
    newg->long_periodic_hook = stun_generic_long_periodic;
    newg->show_hook = stun_generic_show;
    newg->little_show_hook = stun_generic_little_show_hook;
    newg->global_parse_hook = stun_generic_parse_global;
    newg->interface_parse_hook = stun_generic_parse_interface;
    newg->global_nv_hook = stun_generic_nv_globals;
    newg->interface_nv_hook_1 = stun_generic_nv_interface;
    newg->interface_nv_hook_2 = stun_generic_nv_interface;
    newg->print_frame = stun_generic_print_frame;
    newg->peer_change_hook = stun_generic_peer_change_hook;
    return(newg);
}

static void stun_noaddress_nv_globals (
    stun_protocol_info *spi,
    parseinfo *csb)
{
    return;
}

stun_protocol_info *stun_noaddress_predefined_init (char **namep)
{
    stun_protocol_info *res;

    res = stun_new_generic("basic", 0, 0, 'd', TRUE, FALSE, FALSE, FALSE);
    res->permanent = TRUE;
    res->global_nv_hook = stun_noaddress_nv_globals;
    *namep = &(res->name[0]);
    return(res);
}
