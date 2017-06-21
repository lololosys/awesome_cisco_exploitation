/* $Id: cons_parse.c,v 3.5.18.3 1996/09/10 03:08:30 snyder Exp $
 * $Source: /release/112/cvs/Xsys/x25/cons_parse.c,v $
 *------------------------------------------------------------------
 * cons_parse.c -- CONS configuration
 *
 * May 1991, Babak Nader
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cons_parse.c,v $
 * Revision 3.5.18.3  1996/09/10  03:08:30  snyder
 * CSCdi68568:  more constant opportuniites
 *              160 out of data
 *               72 out of image
 * Branch: California_branch
 *
 * Revision 3.5.18.2  1996/08/07  09:04:44  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.18.1  1996/03/18  22:49:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  02:13:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  21:54:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  20:15:15  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.4  1996/02/07  16:17:48  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1996/02/01  06:12:50  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  18:06:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/20  07:38:05  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.1  1995/06/07 23:20:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_cons.h"
#include "../if/network.h"
#include "x25.h"
#include "../decnet/decnet.h"
#include "../clns/clns.h"
#include "../llc2/llc2.h"
#include "../ui/common_strings.h"

extern queuetype x25rpQ;

extern x25_enable;

/*
 * cons_intfc_enable_cmd
 * process the cons interface enable disable command
 * interface cons enable:
 * 	must have specified NET already
 * 	cmns_intfc_enabled flag in the idb set to true
 * interface cons disable:
 * 	set a flag in the idb to false 
 */


static void cons_intfc_enable_cmd (parseinfo *csb)
{
    hwidbtype *idb;
	
    idb = csb->interface->hwptr;
    if (csb->nvgen) {
	nv_write(idb->cmns_intfc_enabled, csb->nv_command);
	return;
    }

    if (csb->sense) {
	/*
	 * Register some functions
	 */
	reg_add_raw_enqueue(LINK_CMNS, cons_enqueue, "cons_enqueue");
	if (!idb->cmns_intfc_enabled) {
	    if (x25_enable) {
		idb->cmns_intfc_enabled = TRUE;
		x25_setup(NULL, idb);
	    } else {
		  printf("\nX.25 not supported in this software");
	    }
	}
    }else{
	if (idb->cmns_intfc_enabled){
	    idb->cmns_intfc_enabled = FALSE;
	}
    }
}

/*
 * cons_intfc_command
 * Given a token and a parsetype block, check for cons 
 * interface subcommand(s).
 *
 */
void cons_intfc_command (parseinfo *csb)
{
    switch (csb->which) {
      case CONS_ENABLE:
	cons_intfc_enable_cmd(csb);
	break;

	/* for future development */

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
    return;
}

/*
 * cons_interface
 * Output information for the show interface command
 *
 */
void cons_interface (register idbtype *swidb)
{
    hwidbtype *idb = swidb->hwptr;

    printf("\n%s is %s, line protocol is %s", swidb->namestring,
	print_idbstate(swidb), interface_up(swidb) ? "up" : "down");

    if (!ISIEEEMSK(idb)) {
        if (is_x25(idb))
	    x25_interface(idb);
	else 
            printf("\n  CMNS/X25 protocol processing disabled");
	return;
    }
    if (!idb->cmns_intfc_enabled) {
        printf("\n  CMNS protocol processing disabled");
	return;
    }

    printf("\n  Hardware address is %e (bia %e), state %s", 
	idb->hardware, idb->bia,
	x25_prstate(idb, (lcitype *)NULL)); 

    printf("\n      Modulo %d, idle %d, timer %u, nvc %d",
	idb->x25_modulo, idb->x25_idle,
	TIME_LEFT_SLEEPING(idb->x25_timer) / ONESEC, idb->x25_nvc);

    printf("\n      Window size: input %d, output %d,",
	idb->x25_win, idb->x25_wout);

    printf(" Packet size: input %d, output %d\n      Timer: TH %d",
	1 << idb->x25_pin, 1 << idb->x25_pout,
	idb->x25_th);

    printf("\n      Channels: ");
    if (idb->x25_lic)
	printf("Incoming-only %d-%d, ", idb->x25_lic, idb->x25_hic);
    else
	printf("Incoming-only none, ");
    if (idb->x25_ltc)
	printf("Two-way %d-%d, ", idb->x25_ltc, idb->x25_htc);
    else
	printf("Two-way none, ");
    if (idb->x25_loc)
	printf("Outgoing-only %d-%d", idb->x25_loc, idb->x25_hoc);
    else
	printf("Outgoing-only none");

    printf("\n      RESTARTs %d/%d CALLs %d+%d/%d+%d/%d+%d DIAGs %d/%d",
	idb->x25_restartssent, idb->x25_restartsrcvd,
	idb->x25_callssent, idb->x25_callssentfailed,
	idb->x25_callsrcvd, idb->x25_callsrcvdfailed,
	idb->x25_callsfrwd, idb->x25_callsfrwdfailed,
	idb->x25_diagssent, idb->x25_diagsrcvd);
}



#define CONS_NUM_CR	6
static char *const cons_if_types[] = {"CMNS-DTE", "CMNS-DCE", "CMNS-DXE"};


/*
 * show_llc2_cons
 * Output CMNS information for the show llc2 command
 *
 */
void show_llc2_cons (llctype *llc2)
{
    static const char banner[] = "\n\n   CMNS Connections to: ";
    idbtype *swidb = llc2->idb;
    hwidbtype *idb = swidb->hwptr;
    considbtype *considb = llc2->considb;
    char c;

    if (considb && validmem(considb)) {
	c = ISDCE(idb, considb) ? '1' : '2';
	automore_conditional(CONS_NUM_CR);
	printf(banner);
	printf("\n      Address %02x%02x.%02x%02x.%02x%02x via %s",
		llc2->llc2_dte[0], llc2->llc2_dte[1], llc2->llc2_dte[2],
		llc2->llc2_dte[3], llc2->llc2_dte[4], llc2->llc2_dte[5],
		swidb ? swidb->namestring : "");
	printf("\n      Protocol is %s", 
		considb->x25_upstate ? "up" : "down");
  	printf("\n      Interface Type %s RESTARTS %d/%d", 
		cons_if_types[considb->cmns_dce],
		considb->x25_restartssent, considb->x25_restartsrcvd);
	printf("\n      Timers: T%c0 %d T%c1 %d T%c2 %d T%c3 %d",
		c, idb->x25_t0, c, idb->x25_t1, c, idb->x25_t2,
		c, idb->x25_t3);
    }
}
#undef CONS_NUM_CR


void show_cmns (parseinfo *csb)
{
    automore_enable(NULL);
    show_swidb_chain(ISOBJ(idb,1) ? GETOBJ(idb,1) : NULL, cons_interface, 0);
    automore_disable();
}
