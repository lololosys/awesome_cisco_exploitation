/* $Id: pt_parse.c,v 3.8.4.3 1996/08/07 09:02:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_parse.c,v $
 *------------------------------------------------------------------
 * pt_parse.c  --  User interface for the "translate" facility.
 *
 * May 1990,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pt_parse.c,v $
 * Revision 3.8.4.3  1996/08/07  09:02:08  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.4.2  1996/05/01  01:59:51  ravindra
 * CSCdi49877:  deleting translate with x25 pvc results in corruption crash
 * Branch: California_branch
 *
 * Revision 3.8.4.1  1996/04/19  15:26:48  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.4  1996/03/19  08:31:21  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.2.6.3  1996/02/21  03:56:08  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.2  1995/12/17  02:15:37  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.2.6.1  1995/11/17  18:06:53  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.8  1996/03/11  22:00:57  swenson
 * CSCdi51039:  parsing of translate authentication options adds access
 * class
 * change which elements of uiarray[] are used for global translate
 * options to avoid conflicts introduced by patches for CSCdi35908.
 *
 * Revision 3.7  1996/03/06  09:31:27  billw
 * CSCdi44863:  AAA Accounting not done for one-step protocol translation
 * Also ensure tty->ustruct gets set for one-step PT sessions.
 *
 * Revision 3.6  1996/02/07  16:14:45  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/02/01  06:08:20  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/24  20:40:46  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.3  1995/11/22  21:25:52  eschaffe
 * CSCdi44220:  Update nameptr sharecount for host-name translate entries
 *
 * Revision 3.2  1995/11/17  18:59:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/24  05:07:25  eschaffe
 * CSCdi41300:  translate commands keyword options should nv-genn in lower
 * case
 *
 * Revision 2.6  1995/10/16  23:13:50  snyder
 * CSCdi42258:  spellink errors
 *              currenlty -> currently
 *
 * Revision 2.5  1995/07/24  23:55:35  eschaffe
 * CSCdi36312:  Memory leak in Middle and Big buffers
 *
 * Revision 2.4  1995/06/28  09:30:18  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/20  07:17:35  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:18:25  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:35:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"		/* includes master.h, etc */
#include "../lat/latsys.h"
#include "../lat/latas.h"
#include "../ip/ip.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "../h/cs_registry.regh"
#include "subsys.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "connect.h"
#include "../tcp/tcp.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "parser_defs_pt.h"
#include "../parser/parser_actions.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../tcp/telnet.h"
#include "../x25/pad.h"
#include "../x25/x25_switch.h"
#include "../x25/x25_route.h"
#include "translate.h"
#include "../pt/pt_debug.h"
#include "../os/async.h"
#include "login.h"
#include "aaa.h"
#include "../os/init.h"

/*
 * Protocol Translator subsystem header
 */

#define PT_MAJVERSION 1
#define PT_MINVERSION 0
#define PT_EDITVERSION  1

SUBSYS_HEADER(pt, PT_MAJVERSION, PT_MINVERSION, PT_EDITVERSION,
	      pt_init, SUBSYS_CLASS_MANAGEMENT,
	      NULL, NULL);

/*
 * translate_command
 * parse the translate command.  Currently this is either of the form
 *     "translate <ipaddress> <x121address>" or
 *     "translate <x121address> <ipaddress>"
 * Note that this is an interface subcommand!
 */

void translate_command (parseinfo *csb)
{
    translatetype *trans;
    int inprot = TRANS_NONE, outprot = TRANS_NONE;

    if (!protocolconversion) {
	if (!csb->nvgen) {
	    printf(noasm);
	}
	return;
    }

    if (csb->nvgen) {
	nv_translate();
	return;
    }

    trans = malloc_named(sizeof(translatetype), "TRANS CMD");
    if (trans == NULL) {	
	printf(nomemory);
	return;
    }
    /*
     * First parse the incoming protocol information.  Handle old style
     * translate commands specially, and remember they were old style.
     */
    inprot = reg_invoke_pt_translate_command(GETOBJ(int,1), csb, trans, IN);

    if (inprot == TRANS_NONE) {
	free(trans);
	return;
    }
    trans->type = TRANSTYPE(inprot, 0);
    if (csb->sense == FALSE) {
	translate_delete(trans);
	free(trans); 
	return;
    }

    /*
     * Now handle parsing of the outgoing protocol options.
     */
    outprot = reg_invoke_pt_translate_command(GETOBJ(int,6), csb, trans, OUT);
	
    if (outprot == TRANS_NONE) {
	free(trans);
	return;
    }
    if (inprot == outprot) {
	printf("\n%%Translate: can't translate between identical protocols");
	free(trans);
	return;
    }
    trans->type = TRANSTYPE(inprot, outprot);

    trans->accessclass = csb->uiarray[9];
    if (csb->uiarray[8]) {
	trans->flags |= TRANSF_GLOB_LOCAL;
    }
    if (csb->uiarray[7]) {
	trans->flags |= TRANSF_GLOB_LOGIN;
    }
    if (csb->uiarray[6]) {
	trans->flags |= TRANSF_GLOB_QUIET;
    }
    trans->maxallowed = csb->uiarray[5];
    if (csb->uiarray[4]) {
	trans->flags |= TRANSF_GLOB_SWAP;
    }

    /*
     * Now do whatever is necessary to tell the incoming connection
     * handler that the input address is to be handled by protocol translation
     * This includes deleting any old translation with same input params.
     * Don't delete a translation entry with a session that is already
     * active.
     */
    if (translate_delete(trans)) {
	/* SLIP/PPP is accepted as output protocol only.  
	 * Before we add it, do few sanity check on the address.
	 */
	if (outprot == TRANS_SLIP_PPP) {
	    if (!reg_invoke_pt_add_alias(outprot, trans)) {
		free(trans);
		return;
	    }
	}
	if (reg_invoke_pt_add_alias(inprot, trans)) {
	    enqueue(translateQ, trans);
	} else {
	    free(trans);
	    return;
	}
	/* For TCP outgoing connection only:
	 * If "host-name" option is used on the translation, lock the name ptr 
	 */
	if ((outprot == TRANS_TCP) && 
	    (trans->out.tcp.tcpflags & TRANSF_TCP_HOST_NAME)) {
	    if (trans->out.tcp.tcpnptr) {
		trans->out.tcp.tcpnptr->sharecount++;        
	    }
	}
    }
}  

/*
 * translate_delete
 *
 * seach through the queue of translations and find one that matches
 * (input protocols) the argument.  Delete this from the queue, and
 * delete any alias that might be lurking around...
 */

boolean translate_delete (translatetype *trans)
{
    translatetype *trans_look, *next;

    for (trans_look = (translatetype *)translateQ->qhead;
         trans_look;
	 trans_look = next) {
        next = trans_look->next;
        if (INPROT(trans->type) == INPROT(trans_look->type)) {
	    /*
	     * Check to see if this entry in the list matches the one we're
	     * looking for
	     */
            if (!reg_invoke_pt_compare_entry(INPROT(trans_look->type),
						    trans_look,
						    trans))
                continue;
	    /*
	     * Delete translation entries with PVC options but don't
	     * delete SVC translation entries with active connections.
	     */
            if ((trans_look->active) && !(trans_look->in.x25.x25flags & 
					  TRANSF_X25_PVC)) {
		printf("\n\nTranslate: Can't delete/add entry - Connection(s)"
		       " are currently active");
		return(FALSE);
	    }
	    /* For TCP outgoing connection only:
	     * If "host-name" option is used on the translation, unlock 
	     * the name ptr 
	     */
	    if ((OUTPROT(trans_look->type) == TRANS_TCP) &&
		(trans_look->out.tcp.tcpflags & TRANSF_TCP_HOST_NAME)) {
		if (trans_look->out.tcp.tcpnptr) {
		    trans_look->out.tcp.tcpnptr->sharecount--;        
		}
	    }
	    /*
	     * It matched. Remove the entry from the translation queue, remove
	     * any aliases bound with it and free the memory up.
	     */
	    unqueue(translateQ, trans_look);
	    reg_invoke_pt_delete_alias(INPROT(trans_look->type), 
				       trans_look);
	    /*
             * The PVC translation entry is currently in use by a process.
             * Let the process free the entry later on when the PVC is removed.
             */
	    if (!(trans_look->in.x25.x25flags & TRANSF_X25_PVC))
    	    	free(trans_look);
	    return(TRUE);
        }
    }
    return(TRUE);
}
/*
 * show_translate
 * Show information on protocol Translation.
 */
void show_translate (void)
{
    translatetype *trans;

    automore_enable(NULL);
    for (trans = (translatetype *)translateQ->qhead;trans; trans=trans->next) {
	printf("\n\nTranslate From: ");
	reg_invoke_pt_show_translate(INPROT(trans->type), trans, IN);
	printf("\n          To:   ");
	reg_invoke_pt_show_translate(OUTPROT(trans->type), trans, OUT);
	if ((trans->accessclass) || (trans->flags & TRANSF_GLOB_ALL))
	    printf("\n         ");
	if (trans->accessclass)
	    printf(" Access-class %d", trans->accessclass);
	if (trans->flags & TRANSF_GLOB_LOCAL)
	    printf(" Local");
	if (trans->flags & TRANSF_GLOB_LOGIN)
	    printf(" Login");
	if (trans->flags & TRANSF_GLOB_QUIET)
	    printf(" Quiet");
	if (trans->flags & TRANSF_GLOB_SWAP)
	    printf(" Swap");
	printf("\n          %d/%d users active, %d peak, %d total, %d failures",
	       trans->active, trans->maxallowed, trans->maxusers,
	       trans->totaluses, trans->failures);
	automore_conditional(TRANS_LINE_COUNT);
    }
    automore_disable();
}

/*
 * nv_translate
 * write out appropriate "translate" commands to nv ram.
*/
void nv_translate (void)
{
    translatetype *trans;

    /*
     * write out translate commands.
     */
    nv_write(translateQ->qhead != NULL, "!");
    for (trans= (translatetype *)translateQ->qhead;
	 trans;
	 trans = trans->next) {
	nv_write(TRUE, "translate");
	reg_invoke_pt_nv_translate_entry(INPROT(trans->type), 
						trans, IN);
	reg_invoke_pt_nv_translate_entry(OUTPROT(trans->type), 
						trans, OUT);
	nv_add(trans->flags & TRANSF_GLOB_LOCAL, " local");
	nv_add(trans->flags & TRANSF_GLOB_LOGIN, " login");
	nv_add(trans->flags & TRANSF_GLOB_QUIET, " quiet");
	nv_add(trans->accessclass, " access-class %d", trans->accessclass);
	nv_add(trans->maxallowed, " max-users %d", trans->maxallowed);
	nv_add(trans->flags & TRANSF_GLOB_SWAP, " swap");
    }
}

/*
 * pt_do_login
 *
 * Perhaps check for a password.  Diddle the IO vectors and
 * use the EXEC code...
 */
void pt_do_login (translatetype *trans, tt_soc *tty)
{
    char tty_str[8];
    char *rem;

    if ((trans->flags & TRANSF_GLOB_LOGIN) &&
        (!old_access_control || tty->password || tty->actype == AC_TACACS)) {
        if (!check_login(tty)) {
            startoutput(tty);
            process_sleep_for(2*ONESEC);   /* pause to read the message */
            trans->failures++;
            trans->active--;
            death(tty);                 /* we die now */
        }
        if (!(trans->flags & TRANSF_GLOB_QUIET)) {
            printf("\nPassword OK\n");
            startoutput(tty);
        }
    } else if (!old_access_control) {
	/* 
	 * For AAA security, we want to make sure that a user data structure
	 * is set up even if no login occurs.  This is used by accounting,
	 * possibly amoung other things.
	 */
	sprintf(&tty_str[0], "tty%d", tty->ttynum);
	rem = tty_get_remote_addr(tty);
	aaa_create_user(&tty->ustruct, NULL, NULL, tty_str, rem,
			AAA_AUTHEN_TYPE_NONE, AAA_SVC_PT, PRIV_USER);
    }
}

/*
 * pt_translate_string_default
 * 
 * Default case handing for translating a string
 */
static char *pt_translate_string_default(int prot)
{
    return("unknown protocol");
}

/*
 * pt_default_translate_command
 *
 * Default case handling for parser
 */  

int pt_default_translate_command (parseinfo *csb,
				  translatetype *trans,
				  int dir)
{
    if (dir == IN) {
	bad_parser_subcommand(csb, GETOBJ(int,1));
    } else {
	bad_parser_subcommand(csb, GETOBJ(int,6));
    }

    return(TRANS_NONE);
}	

/*
 * pt_default_add_alias
 *
 * Default case handling for alias additions
 */  

boolean pt_default_add_alias (translatetype *trans)
{
    return(FALSE);
}	

/*
 * pt_compare_entry
 *
 * Default case handling entry point for translate entry comparisons
 */  

boolean pt_default_compare_entry (translatetype *trans1,
				  translatetype *trans2)
{
    return(FALSE);
}

/*
 * pt_default_delete_alias
 *
 * Default case handling for alias deletions
 */  

boolean pt_default_delete_alias (translatetype *trans)
{
    return(FALSE);
}	

/*
 * pt_transto_default
 * Default error return for translating some input protocol
 * to some output protocol.  Returns -1 to indicate process creation failed.
 */

static int pt_transto_default (translatetype *trans, tt_soc *tty)
{
    return(-1);
}

/*
 * pt_init
 * Initialize various protocol translator stuff.
 */

void pt_init (subsystype *subsys)
{
    /*
     * Set up debugging flags
     */
    pt_debug_init();
    /*
     * Initialize the translator Queue
     */
    queue_init(translateQ,0);

    /*
     * Initialize translate parser support
     */
    pt_parser_init();

    /*
     * Register some default functions
     */
    reg_add_default_pt_translate_command(pt_default_translate_command,
					 "pt_default_translate_command");

    reg_add_default_pt_add_alias(pt_default_add_alias,
				 "pt_default_add_alias");

    reg_add_default_pt_compare_entry(pt_default_compare_entry,
				     "pt_default_compare_entry");

    reg_add_default_pt_delete_alias(pt_default_delete_alias,
				    "pt_default_delete_alias");

    reg_add_default_pt_tcpinput(pt_transto_default, 
				"pt_transto_default");

    reg_add_default_pt_x25input(pt_transto_default, 
				"pt_transto_default");

    reg_add_default_pt_latinput(pt_transto_default, 
				"pt_transto_default");

    reg_add_default_pt_translate_string(pt_translate_string_default,
					"pt_translate_string_default");
}
