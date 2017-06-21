/* $Id: pt_lat.c,v 3.4.20.1 1996/08/07 09:02:05 snyder Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_lat.c,v $
 *------------------------------------------------------------------
 * LAT specific Protocol Translator code
 *
 * November 1993, Eman Schaffer, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_lat.c,v $
 * Revision 3.4.20.1  1996/08/07  09:02:05  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/01  06:08:09  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/10  06:49:56  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.2  1995/11/17  18:59:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:34:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"		/* includes master.h, etc */
#include "../lat/latsys.h"
#include "../lat/latas.h"
#include "../lat/lat_public.h"
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
#include "translate.h"
#include "../pt/pt_debug.h"
#include "access.h"
#include "../os/init.h"

#include "pt_lat.h"


/*
 * Ugly externs required as the LAT code is unprototyped. Bleah.
 */
extern as_ptr as_create(byte *, byte *, aid_type, int, int);
extern int as_set_flags(as_ptr , int);
extern as_ptr as_delete(as_ptr);
extern as_ptr as_show(byte *);


/*
 * LAT Protocol Translator subsystem header
 */

#define PT_LAT_MAJVERSION 1
#define PT_LAT_MINVERSION 0
#define PT_LAT_EDITVERSION  1

SUBSYS_HEADER(pt_lat,
	      PT_LAT_MAJVERSION, PT_LAT_MINVERSION, PT_LAT_EDITVERSION,
	      pt_lat_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: lat",
	      "req: lat");

/*
 * pt_lat_init
 *
 * Subsystem initialization point
 */

void pt_lat_init (subsystype *subsys)
{
    /*
     * Add parser commands for this subsystem
     */
    pt_lat_parser_init();

    /*
     * Add service points for this subsystem
     */
    reg_add_pt_translate_command(TRANS_PROT_LAT,
				 pt_lat_command,
				 "pt_lat_command");

    reg_add_pt_add_alias(TRANS_LAT,
			 pt_lat_add_alias,
			 "pt_lat_add_alias");

    reg_add_pt_compare_entry(TRANS_LAT,
			     pt_lat_compare_entry,
			     "pt_lat_compare_entry");

    reg_add_pt_delete_alias(TRANS_LAT,
			    pt_lat_delete_alias,
			    "pt_lat_delete_alias");	

    reg_add_pt_show_translate(TRANS_LAT,
			      pt_show_lat_transinfo,
			      "pt_show_lat_transinfo");

    reg_add_pt_nv_translate_entry(TRANS_LAT,
				  pt_nv_lat_transinfo,
				  "pt_nv_lat_transinfo");

    reg_add_pt_translate_string(TRANS_LAT, pt_lat_trans_string,
				"pt_lat_trans_string");
}

/*
 * pt_lat_commmand
 *
 * Registry service point for protocol translator parser chain
 */

int pt_lat_command (parseinfo *csb,
			     translatetype *trans,
			     int dir)
{
    trans_latinfo *latinfo;
    char  *service;
    char  *node;
    char  *port;
    int   unadv;

    latinfo = (dir == IN) ? &trans->in.lat : &trans->out.lat;

    if (lat_enable == FALSE) {
	if (!csb->nvgen) {
	    printf(noasm);
	}
	return(TRANS_NONE);
    }

    if (dir == IN) {
	service = GETOBJ(string,1);
	node = GETOBJ(string,2);
	port = GETOBJ(string,3);
	unadv = GETOBJ(int,2);
    } else {
	service = GETOBJ(string,4);
	node = GETOBJ(string,5);
	port = GETOBJ(string,6);
	unadv = GETOBJ(int,7);
    }
    
    STRING_CHECK(service, latinfo->latservice, SV_STR, "service");
    STRING_CHECK(port, latinfo->latport, OB_STR, "port");
    STRING_CHECK(node, latinfo->latnode, ND_STR, "node");
    if (unadv) {
	latinfo->latflags |= TRANSF_LAT_UNADVERTISED;
    }

    return(TRANS_LAT);
}

/*
 * pt_show_lat_transinfo
 * 
 * Show LAT related translate information
 */
void pt_show_lat_transinfo (translatetype *trans, int dir)
{
    trans_latinfo *lat = (dir == IN) ? &trans->in.lat : &trans->out.lat;
    
    printf("LAT %*s", LATSTRING(lat->latservice));
    if (lat->latport[0]) 
	printf(" Port %*s", LATSTRING(lat->latport));
    if (lat->latnode[0]) 
	printf(" Node %*s", LATSTRING(lat->latnode));
    if (lat->latflags & TRANSF_LAT_UNADVERTISED) 
	printf (" Unadvertised");
}
/*
 * pt_nv_lat_transinfo
 * nv_add LAT related translate information
 */
void pt_nv_lat_transinfo (translatetype *trans, int dir)
{
    trans_latinfo *lat = (dir == IN) ? &trans->in.lat : &trans->out.lat;

    nv_add(TRUE, " lat %*s", LATSTRING(lat->latservice));
    nv_add(lat->latport[0]," port %*s", LATSTRING(lat->latport));
    nv_add(lat->latnode[0]," node %*s", LATSTRING(lat->latnode));
    nv_add(lat->latflags & TRANSF_LAT_UNADVERTISED, " unadvertised");
}
/*
 * pt_lat_add_alias
 *
 */
boolean pt_lat_add_alias (translatetype *trans)
{
    as_ptr as;
    
    as = as_show(trans->in.lat.latservice);
    if (!as) {
	/* This service is not yet created ,hence create it */
    	as = as_create(trans->in.lat.latservice, (byte *)"",
                   trans,
                   trans->in.lat.latflags &
                   TRANSF_LAT_UNADVERTISED ?
                   AS_CLIENT|AS_ENABLED :
                   AS_CLIENT|AS_OFFERED|AS_ENABLED,
                   0);

	/* This service cannot be created */
    	if (!as) {
	    printf("\n%%Translate: Can't set up LAT service %*s",
	       		LATSTRING(trans->in.lat.latservice));
	    return(FALSE);
	}
    } else {
	if (trans->in.lat.latflags & TRANSF_LAT_UNADVERTISED) {
            if (!as_set_flags(as, (as->as_flags|AS_ENABLED) & ~AS_OFFERED)) {
                printf("\n%%Translate: Insufficient memory to enable service.");
	        return(FALSE);
            }
	} else {
            if (!as_set_flags(as, as->as_flags|AS_OFFERED|AS_ENABLED)) {
                printf("\n%%Translate: Insufficient memory to enable service.");
	        return(FALSE);
            }
	}
        as->as_aid = trans;
    } 
    /*
     * The service has been created
     */
    trans->in.lat.latas = as;

     /*
      * Update the bridging smf table if required
      */
    lat_mass_macaddrs_update(TRUE); 
    return(TRUE);
}
/*
 * pt_lat_compare_entry
 *
 */

boolean pt_lat_compare_entry (translatetype *trans1, 
				      translatetype *trans2)
{
    if (strcmp((char *)trans1->in.lat.latservice,
	       (char *)trans2->in.lat.latservice))
	return(FALSE);

    return(TRUE);
}
/*
 * pt_lat_delete_alias
 *
 */
boolean pt_lat_delete_alias (translatetype *trans)
{
    as_ptr as;

    as = (as_ptr) as_show(trans->in.lat.latservice);
    if (as) {
	as_delete(as);

        /*
         * Update the bridging smf table if required
         */
        lat_mass_macaddrs_update(FALSE);        
    }
    return(TRUE);
}

/*
 * pt_lat_trans_string
 * Take an int and return a string of the protocol name
 */
char *pt_lat_trans_string (int prot)
{
    return("lat");
}
