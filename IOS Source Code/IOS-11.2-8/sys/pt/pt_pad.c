/* $Id: pt_pad.c,v 3.5.20.4 1996/08/07 09:02:06 snyder Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_pad.c,v $
 *------------------------------------------------------------------
 * X.25 PAD specific Protocol Translator code
 *
 * November 1993, Scott Mackie, Eman Schaffer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_pad.c,v $
 * Revision 3.5.20.4  1996/08/07  09:02:06  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.20.3  1996/04/30  18:56:32  ravindra
 * CSCdi55465:  cant delete pvc translation entries
 * Branch: California_branch
 *
 * Revision 3.5.20.2  1996/04/05  22:02:00  ravindra
 * CSCdi52043:  Cant get VTY for X25 PVC translation when booting
 * Branch: California_branch
 * PT allocates a VTY line when PVC translation session demands for it
 * while reloading & flags an error to the user to increase the VTY lines
 * during an interactive session.
 *
 * Revision 3.5.20.1  1996/03/18  21:45:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.3  1996/03/13  01:52:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.22.2  1996/03/07  10:41:17  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.22.1  1996/02/20  17:00:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/01  06:08:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/11/22  21:47:00  widmer
 * CSCdi42888:  Cannot place  at beginning of cud field for x25 host
 * command
 * add %S to print out strings that are parsed by STRING macro.
 * Change x25 cud NV generation to use %S
 * Change STRING macro to escape quotes with \"
 *
 * Revision 3.3  1995/11/21  08:27:23  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.2  1995/11/17  18:59:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/24  05:07:23  eschaffe
 * CSCdi41300:  translate commands keyword options should nv-genn in lower
 * case
 *
 * Revision 2.2  1995/09/28  22:41:24  billw
 * CSCdi40511:  translate command does not support x25 regular expressions
 *
 * Revision 2.1  1995/06/07  22:35:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "interface_private.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "subsys.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "connect.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "parser_defs_pt.h"
#include "../ip/ip.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../x25/pad.h"
#include "../x25/x25_switch.h"
#include "../x25/x25_route.h"
#include "translate.h"
#include "../pt/pt_debug.h"
#include "access.h"
#include "pt_pad.h"
#include <string.h>
#include "../os/init.h"


/*
 * Local Storage and Definitions
 */
static int pvc_trans_entries = 0;
 
/*
 * X.25 PAD Protocol Translator subsystem header
 */

#define PT_PAD_MAJVERSION 1
#define PT_PAD_MINVERSION 0
#define PT_PAD_EDITVERSION  1

SUBSYS_HEADER(pt_pad,
	      PT_PAD_MAJVERSION, PT_PAD_MINVERSION, PT_PAD_EDITVERSION,
	      pt_pad_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: x25, pad",
	      "req: x25, pad");

/*
 * pt_pad_init
 *
 * Subsystem initialization point
 */

void pt_pad_init (subsystype *subsys)
{
    /*
     * Add parser commands for this subsystem
     */
    pt_pad_parser_init();

    /*
     * Add service points for this subsystem
     */
    reg_add_pt_translate_command(TRANS_PROT_X25,
				 pt_pad_command,
				 "pt_pad_command");

    reg_add_pt_add_alias(TRANS_X25,
				pt_pad_add_alias,
				"pt_pad_add_alias");

    reg_add_pt_compare_entry(TRANS_X25,
				    pt_pad_compare_entry,
				    "pt_pad_compare_entry");

    reg_add_pt_delete_alias(TRANS_X25,
				   pt_pad_delete_alias,
				   "pt_pad_delete_alias");

    reg_add_pt_show_translate(TRANS_X25,
				     pt_show_pad_transinfo,
				     "pt_show_pad_transinfo");

    reg_add_pt_nv_translate_entry(TRANS_X25,
					 pt_nv_pad_transinfo,
					 "pt_nv_pad_transinfo");

    reg_add_pt_translate_string(TRANS_X25, pt_pad_trans_string,
				"pt_pad_trans_string");
}

/*
 * pt_pad_command
 *
 * Registry service point for protocol translator parser chain
 */
int pt_pad_command (parseinfo *csb,
			     translatetype *trans,
			     int dir)
{
    int ok = TRUE;
    profiletype *profile;
    trans_x25info *x25info;
    char	*addr;
    char	*cud;
    char	*profile_str;
    int		pvc_set, pvc;
    int		accept_reverse = 0;
    int         printer;

    x25info = (dir == IN) ? &trans->in.x25 : &trans->out.x25;

    if (x25_enable == FALSE) {
	if (!csb->nvgen) {
	    printf(noasm);
	}
	return(TRANS_NONE);
    }

    /*  translate x25 <x121addr>  */
    if (dir == IN) {
	addr = GETOBJ(string,3);
	cud = GETOBJ(string,1);
	profile_str = GETOBJ(string,2);
	pvc_set = GETOBJ(int,2);
	pvc = GETOBJ(int,3);
	accept_reverse = GETOBJ(int,4);
	printer = GETOBJ(int,5);
    } else {
	addr = (char *)GETOBJ(paddr,2)->x121_addr;
	cud = GETOBJ(string,4);
	profile_str = GETOBJ(string,5);
	pvc_set = GETOBJ(int,7);
	pvc = GETOBJ(int,8);
	printer = 0;
    }

    /*  translate x25 <x121addr>  */
    strcpy((char *)x25info->x121addr, addr);
    /*  translate x25 <x121addr> cud <string> */
    strcpy((char *)x25info->cud, cud);
    /*  translate x25 <x121addr> profile <string>  */
    if (profile_str[0] != '\0') {
	profile = (profiletype *)pad_find_profile(profile_str);
	if (profile) {
	    x25info->profile = profile;
	} else {
	    ok = FALSE;
	    printf("\n%%Unknown profile name \"%s\"", profile_str);
	}
    }
    /*  translate x25 <x121addr> pvc <num>  */
    if (pvc_set) {
        idbtype *idb;
        if ((csb->flags & CONFIG_TERM) && \
    	    (pvc_trans_entries >= nvtylines) && (csb->sense)) {
    	    printf("\n%%Translate: Increase VTY's for X25 PVC translation");
    	    return(TRANS_NONE);
    	}
	idb = pick_x25_idb(x25info->x121addr, x25info->cud,
				    strlen((char *)x25info->cud));
	if ((idb == NULL) || !is_x25(idb->hwptr)) {
	    printf("\n%%Translate: X.25 is not available for PVC");
	    return(TRANS_NONE);
	}
	ok = FALSE;

	if (pvc && IS_LCI_PVC(idb->hwptr, pvc)) {
	    x25info->cudprefix = pvc;
	    if (dir == IN) {
		x25info->x25flags |= TRANSF_X25_PVC;
		ok = TRUE;
	    } else {
		printf("\n%%Translate: X25 PVCs not supported on output");
	    }
	} else {
	    printf("\n%%Translate: Illegal PVC %d for interface %s",
		   pvc, idb->namestring);
	}
    }
    /*  For accepting reverse charged call on a per call basis */
    /*  translate x25 <x121addr> accept-reverse  */
    if (accept_reverse) {
	x25info->x25flags |= TRANSF_X25_ACCEPTREVERSE;
    }

    /*
     * Requesting or not requesting reverse charge facility on a per call basis
     * translate inprotocol <in address> x25 <x121addr> reverse/no-reverse 
     */

    if (dir == OUT )	{
        if (GETOBJ(int,9)) {
	    x25info->x25flags |= TRANSF_X25_REVERSE;
        }

        if (GETOBJ(int,10)) {
	    x25info->x25flags |= TRANSF_X25_NOREVERSE;
        }
    }
    if (printer) {
	x25info->x25flags |= TRANSF_X25_PRINTER;

	if (dir == IN) {
	    /*
	     * The X25 local option "printer" implies the global option "quiet"
	     */
	    trans->flags |= TRANSF_GLOB_QUIET;
	}
    }
    if(pvc_set && ok ) 
    	if(csb->sense) 
    	    pvc_trans_entries++;
    	else 
    	    pvc_trans_entries--;
       
    return((ok) ? TRANS_X25 : TRANS_NONE);
}
/*
 * pt_show_pad_transinfo
 * 
 * Show PAD related translate information
 */
void pt_show_pad_transinfo (translatetype *trans, int dir)
{
    trans_x25info *x25 = (dir == IN) ? &trans->in.x25 : &trans->out.x25;

    printf("X25 %s", x25->x121addr);
    if (x25->cud[0]) printf(" Cud %s", x25->cud);
    if (x25->x25flags & TRANSF_X25_PVC)
	printf(" Pvc %d", x25->cudprefix);
    else if (x25->cudprefix)
	printf(" Prefix %x", x25->cudprefix);
    if (x25->profile) printf(" Profile %s", x25->profile->name);
    if (x25->x25flags & TRANSF_X25_ALL)
	printf("\n               ");
    if (x25->x25flags & TRANSF_X25_PRINTER)
	printf(" Printer");
    if (x25->x25flags & TRANSF_X25_ACCEPTREVERSE) printf(" Accept-reverse");
    if (x25->x25flags & TRANSF_X25_REVERSE) printf(" Reverse");
    if (x25->x25flags & TRANSF_X25_NOREVERSE) printf(" No-reverse");
}

/* 
 * pt_nv_pad_transinfo 
 * write PAD entry related translate configuration
 */
void pt_nv_pad_transinfo (translatetype *trans, int dir)
{
    trans_x25info *x25 = (dir == IN) ? &trans->in.x25 : &trans->out.x25;

    nv_add(TRUE, " x25 %s", x25->x121addr);
    nv_add(x25->cud[0]," cud %S", x25->cud);
    nv_add(x25->x25flags & TRANSF_X25_PRINTER, " printer");
    if (x25->x25flags & TRANSF_X25_PVC)
	nv_add(TRUE, " pvc %d", x25->cudprefix);
    else
	nv_add(x25->cudprefix, " prefix %x", x25->cudprefix);
    nv_add(x25->profile != NULL," profile %s", x25->profile->name);
    nv_add(x25->x25flags & TRANSF_X25_ACCEPTREVERSE," accept-reverse");
    nv_add(x25->x25flags & TRANSF_X25_REVERSE," reverse");
    nv_add(x25->x25flags & TRANSF_X25_NOREVERSE," no-reverse");
}

/*
 * pt_pad_add_alias
 *
 */

boolean pt_pad_add_alias (translatetype *trans)
{
    x25_routing = TRUE;
    if (trans->in.x25.x25flags & TRANSF_X25_PVC) {
	if (!x25pvc_cfork(trans))
	    return(FALSE);
    }
    else
	x25_add_route(TRUE, trans->in.x25.x121addr, trans->in.x25.cud,
		      NULL, NULL, NULL, trans, X25_ROUTE_IPPT,
		      1, 0, 0L, /* saddr, daddr, idb, ipaddr */ 0L, 0L,
		      0L, 0L, 0L, /* data, service, pos */
		      0, 0);
    
    /*
     * This always succeeds (apparently ;-)
     */
    return(TRUE);
}
/*
 * pt_pad_compare_entry
 *
 */

boolean pt_pad_compare_entry (translatetype *trans1, 
				      translatetype *trans2)
{
    if (strcmp((char *)trans1->in.x25.x121addr,
	       (char *)trans2->in.x25.x121addr) ||
	strcmp((char *)trans1->in.x25.cud,
	       (char *)trans2->in.x25.cud))
	return(FALSE);

    if ((trans1->in.x25.x25flags & TRANSF_X25_PVC) &&
	(trans1->in.x25.cudprefix != trans2->in.x25.cudprefix))
	return(FALSE);

    return(TRUE);
}
/*
 * pt_pad_delete_alias
 *
 */

boolean pt_pad_delete_alias (translatetype *trans)

{
    if (trans->in.x25.x25flags & TRANSF_X25_PVC) {
	/*
	 * remove the PVC and let the current translate session die
	 */
	idbtype *idb = pick_x25_idb(trans->in.x25.x121addr,
				    trans->in.x25.cud, 
				    strlen((char *)trans->in.x25.cud));
	lcitype *lci = (lcitype *) x25_lci_lookup(hwidb_or_null(idb),
						  trans->in.x25.cudprefix, 
						  NULL);
	if (lci)
	    x25_call_remove(lci, 0, 0);
	return(TRUE);
    }
    
    x25_add_route(FALSE, trans->in.x25.x121addr, trans->in.x25.cud,
		  NULL, NULL, NULL, trans, X25_ROUTE_IPPT,
		  0, 0, 0L, /* saddr, daddr, idb, ipaddr */ 0L, 0L,
		  0L, 0L, 0L, /* data, service, pos */
		  0, 0);
    /*
     * This always succeeds too!
     */
    return(TRUE);
}

/*
 * x25pvc_cfork
 * create a session for a PVC.  The PVC number is stored in the cudprefix
 * field of the translate data block.  Note that the x121address field is
 * not actually used, except possibly to select an interface.
 */
boolean x25pvc_cfork (translatetype *trans)
{ 
    int line;
    lcitype *lci;
    idbtype *idb;
    int maxline = nvtylines+VTYBase+1;

    line = get_vtyline(TELNET_PORT, &pad_connvector);	 /* get a vty */
    if (line <= 0) {
	printf("\n%%Translate:Creating extra VTY line for X25 PVC translation");
        /* 
         * This is to take care of x25 pvc translations
         * during VTY resource crunch
         */
        create_vtylines(maxline);
        line = get_vtyline(TELNET_PORT, &pad_connvector);	 /* get a vty */
        if (line <= 0) {
            printf("\n%%Translate: Can't get VTY for X25 PVC translation");
            return(FALSE);
        } 
    }

    idb = pick_x25_idb(trans->in.x25.x121addr, trans->in.x25.cud, 
		       strlen((char *)trans->in.x25.cud));
    if ((idb == NULL) || !is_x25(idb->hwptr)) {
	printf("\n%%Translate: X.25 is not available for PVC");
	return(FALSE);
    }
    lci = (lcitype *)x25_pvc_setup(idb, trans->in.x25.cudprefix);
    if (!lci) {
	printf("\n%%Translate: Can't setup PVC");
	return(FALSE);
    }
    lci->lci_linktype = LINK_PAD;
    if (!pad_makecall(lci, line, trans)) {
	printf("\n%%Translate: Can't create fork for PVC translation");    
	return(FALSE);
    }
    return(TRUE);
}

/*
 * pt_pad_trans_string
 * Take an int and return a string of the protocol name
 */
char *pt_pad_trans_string (int prot)
{
    return("x25");
}
