/* $Id: latshow.c,v 3.3.28.2 1996/07/29 19:02:19 snyder Exp $
 * $Source: /release/112/cvs/Xsys/lat/latshow.c,v $
 *------------------------------------------------------------------
 * latshow.c -- cisco LAT show code - displays LAT information
 *
 * February 17, 1992, Nick Thille
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * LAT technology licensed from Meridian Technology, Copyright 1989.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latshow.c,v $
 * Revision 3.3.28.2  1996/07/29  19:02:19  snyder
 * CSCdi64402:  declare arrays in lat that should be const, const
 *              save 1084 data, 68 out of image
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/03/18  20:39:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  10:04:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  06:36:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  17:34:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "mtcsys.h"
#include "config.h"
#include "latsys.h"
#include "latls.h"
#include "latas.h"
#include "laths.h"
#include "latnd.h"
#include "latob.h"
#include "latfnc.h"
#include "sysfnc.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "latgroup.h"
#include "connect.h"


/*
 * declat_show_ls_summary
 * Show one learned service in detail like DECServer does
 */

void declat_show_ls_summary (
     ls_ptr ls,
     nd_ptr nd,
     const char *hdr)

{ /* declat_show_ls_summary */

    printf("%s\n%*s%*c%s%*s", hdr,
	   LATSTRING(ls->ls_service), (20 - ls->ls_service[0]), ' ',
	   (nd->nd_flags) ? "Unknown      " : "Available    ",
	   LATSTRING(ls->ls_ident));
} /* declat_show_ls_summary */


/*
 * declat_show_one_ls
 * Show one learned service in detail like DECServer does
 */

static const char declat_ls1_hdr[] = "\n\nService ";
static const char declat_ls2_hdr[] =
    "\nNode Name           Status   Rating  Identification\n";
static const char declat_ip_hdr[] =
    "\nHostname            Status           IP Address\n";


void declat_show_one_ls (
     char *args)

{
    ls_ptr ls;
    nd_ptr nd;
    nametype *cache_ptr;
    int errcode;

    ipaddrtype tmp_ip_addr = 0;
    char *state;
    
    boolean header_printed = FALSE;
    byte last_service[SV_STR+1];
    short ctr = 1;

    uppercase(args);
    sstrncpy((char *)&last_service[1], args, SV_STR+1);
    last_service[0] = strlen(args);

    ls = ls_show(last_service, NULL, NULL, ND_UNKNOWN+ND_UNREACH+ND_INIT);

    if (!ls) {
	ls = ls_show(last_service, NULL, NULL, 0);
    }
    if (ls) {
        nd = ls->ls_nd;
        if (lat_compare_groups(nd->nd_acl, stdio->tty_lat_groupmask)) {

 	    header_printed = TRUE;
	    automore_enable(NULL); /* turn on automore */

    	    if (nd->nd_flags == 0) {
                state = "Available    \n";
    	    } else if (nd->nd_flags & ND_UNKNOWN) {
    		state = "Unknown      \n";
    	    } else if ((nd->nd_flags & ND_INIT) &&
                       (!nd->nd_flags & ND_UNREACH)) {
    		state = "Initializing \n";
            } else {
    		state = "Unreachable  \n";
    	    }

    	    printf("\n%s%*s - %s%s",declat_ls1_hdr, LATSTRING(last_service),
		   state, declat_ls2_hdr);


            ctr = 1;
            while (TRUE) {  /* Step through printing services */ 
    	        ls = ls_scan(last_service, NULL, ctr);
    	        if (!ls)
    	            break;
	        ctr++;

    	        nd = ls->ls_nd;
    	        if (lat_compare_groups(nd->nd_acl, stdio->tty_lat_groupmask)) {
                    printf("\n%*s%*c%s%-4d  %*s  ", LATSTRING(nd->nd_node),
                           (20 - nd->nd_node[0]), ' ',
                           (nd->nd_flags == 0)? "Reachable  ":"Unreachable",
                            ls->ls_rating, LATSTRING(ls->ls_ident));
                } /* if had right groups for this ls */

            } /* while true */

	    printf("\n");        /* match decserver with blank line */
	    automore_disable();    /* stop automore here */

        } else {    /* He doesn't have right groups */
            ls = NULL;
	  }
    } /* ls not NULL */

    if (header_printed) {  /* If we printed a LAT service out exit */
        return;
    }

        /* No services printed, check cache for available IP host. */

    cache_ptr = name_cache_lookup(args,NULL,NAM_IP);
    if (cache_ptr != NULL) {
	if (!(cache_ptr->flags & NAM_REVALIDATE)) { /* Host in cache */

	    for (ctr = 0; (ctr < MAXIPADDRESSES); ctr++)
	        if (cache_ptr->v.ip.adr[ctr].type == ADDR_IP) {
		    tmp_ip_addr = cache_ptr->v.ip.adr[ctr].addr.ip_address;
		    break;
		} /* Found an IP address */

	    printf("\n\n\nIP Host %s - Available\n"
		   "%s\n%s%*cReachable        ", args, 
		   declat_ip_hdr, args, (20-strlen(args)), ' ');

	    if (tmp_ip_addr != 0) {
		printf("%i\n", tmp_ip_addr);
	    } else {
		printf("\n");
	    }

	    return;		/* Got IP host from cache, we're done*/

	} /* REVALIDATE flags not set */

          /* Got pointer, but revalidate set */

        for (ctr = 0; (ctr < MAXIPADDRESSES); ctr++)
	    if (cache_ptr->v.ip.adr[ctr].type == ADDR_IP) {
                tmp_ip_addr = cache_ptr->v.ip.adr[ctr].addr.ip_address;
 	        break;
	    } /* Found an IP address */

        printf("\n\n\nIP Host %s - Unknown\n"
	       "\nHostname            Status           IP Address\n"
	       "\n%s%*cUnknown          ", args, args, (20-strlen(args)), ' ');

	if (tmp_ip_addr != 0) {
	    printf("%i\n", tmp_ip_addr);
	} else {
	    printf("\n");
	}

	return;			/* Got IP host from cache, we're done*/

    } /* Got a cachepointer from probe_cache */

        /* No IP host in cache, do a name_lookup and see if exist. */

    cache_ptr = name_lookup(args, &errcode, NAM_IP);
    if (cache_ptr != NULL) {

          /* Got pointer, but had to lookup in DNS */

        for (ctr = 0; (ctr < MAXIPADDRESSES); ctr++)
	    if (cache_ptr->v.ip.adr[ctr].type == ADDR_IP) {
                tmp_ip_addr = cache_ptr->v.ip.adr[ctr].addr.ip_address;
 	        break;
	    } /* Found an IP address */

        printf("\n\n\nIP Host %s - Unknown\n"
	       "\nHostname            Status           IP Address\n"
	       "\n%s%*cUnknown          ", args, args, (20-strlen(args)), ' ');

	if (tmp_ip_addr != 0) {
	    printf("%i\n", tmp_ip_addr);
	} else {
	    printf("\n");
	}

    } else { 
                 /* Never found anything */
	printf("\nLocal -711- Service %s not known", args);
    } /* Never found anything */

} /* declat_show_one_ls */
    
    

/*
 * declat_show_ls
 * Show learned services like DECServer does
 */

static const char ls_summary_hdr[] = "\nService Name        Status       Identification\n";


void declat_show_ls (parseinfo *csb)
{
    ls_ptr ls;
    nd_ptr nd;
    const char *hdr;
    char *servicename = GETOBJ(string, 1);
    boolean header_printed = FALSE;
    byte last_service[SV_STR];
    
    if (*servicename) { /* if user asked for one service call detail routine */
        declat_show_one_ls(servicename);
	return;
    }
                    /* otherwise, give one liner for all services */

    last_service[0] = 0;

    automore_enable(NULL);
    
    while (TRUE) {
        ls = ls_list(last_service, (byte *)"", 0, ND_UNREACH);
    	if (!ls)
	    break;
    	nd = ls->ls_nd;
    
    	str_move(last_service, ls->ls_service);
    
    	if (lat_compare_groups(nd->nd_acl, stdio->tty_lat_groupmask)) {
	    if (!header_printed)
		hdr = ls_summary_hdr;
	    else
		hdr = NULL;

	    declat_show_ls_summary(ls, nd, hdr);

	    header_printed = TRUE;

    	}  /* He has the right groups */

    } /* While true */

    if (header_printed)
	printf("\n");

} /* declat_show_ls */


/*
 * declat_show_node_counters
 * Show node counters for a given node like DECserver does  
*/

void declat_show_node_counters (
     nd_ptr nd,
     char *hdr_ptr)

{
    printf(
      "\nNode: %*s\n"
      "\nSeconds Since Zeroed:   %-10d      Multiple Node Addresses:   %-10d"
      "\nMessages Received:      %-10d      Duplicates Received:       %-10d"
      "\nMessages Transmitted:   %-10d      Messages Re-transmitted:   %-10d"
      "\nSlots Received:         %-10d      Illegal Messages Received: %-10d"
      "\nSlots Transmitted:      %-10d      Illegal Slots Received:    %-10d"
      "\nBytes Received:         %-10d      Solicitations Accepted:    %-10d"
      "\nBytes Transmitted:      %-10d      Solicitations Rejected:    %-10d"
      "\n",
	   LATSTRING(nd->nd_node),
	   nd->nd_stats.co_seconds, nd->nd_stats.co_multiple_node,
	   nd->nd_stats.co_msgs_recv, nd->nd_stats.co_duplicates,
	   nd->nd_stats.co_msgs_xmit, nd->nd_stats.co_retransmit,
	   nd->nd_stats.co_slot_recv, nd->nd_stats.co_msgs_bad,
	   nd->nd_stats.co_slot_xmit, nd->nd_stats.co_slot_bad,
	   nd->nd_stats.co_byte_recv, nd->nd_stats.co_solicit_accept,
	   nd->nd_stats.co_byte_xmit, nd->nd_stats.co_solicit_reject);
}


/*
 * declat_show_node_status
 * Show details of a given node like DECserver
 */

void declat_show_node_status (
     nd_ptr nd,
     char *hdr_ptr)

{ /* declat_show_node_status */

    byte tmp_nd_acl[GROUPS];
    char *hdr = "\nService Name        Status   Rating  Identification\n";
    ls_ptr ls;
    short ctr = 0;
    short idx = 0;

				/* Copy over the ACL before printing */
    bcopy((byte *) &nd->nd_acl, (byte *) &tmp_nd_acl, GROUPS);

    printf(
      
      "\n\n\nNode: %*s%*c                 Address:  "
      "%02x-%02x-%02x-%02x-%02x-%02x"
      "\nLAT Protocol:     V%d.%d                  Data Link Frame Size:  %4d"
      "\n" 
      "\nIdentification: %s"
      "\n"
      "\nNode Groups:",
	   LATSTRING(nd->nd_node), (17 - nd->nd_node[0]), ' ', 
	   ((uchar *)nd->nd_host_addr)[0], ((uchar *)nd->nd_host_addr)[1],
	   ((uchar *)nd->nd_host_addr)[2], ((uchar *)nd->nd_host_addr)[3],
	   ((uchar *)nd->nd_host_addr)[4], ((uchar *)nd->nd_host_addr)[5], 
	   nd->nd_prot_ver, nd->nd_prot_eco, (nd->nd_dll_max + 18),
	   (nd->nd_ident[0] != 0) ? (char *) &nd->nd_ident[1] : NULL);

    if (tmp_nd_acl) {
	ctr = print_lat_groups(tmp_nd_acl, 12, 15, TRUE);
	printf("\n");
    }
    else 
	printf("   0\n");

    ls = nd->nd_ls[0];

    while ((validmem(ls)) && (ls->ls_nd == nd)) {

	ls = nd->nd_ls[0];	               /* start at root of list */
	ctr = 0;		               /* With a counter at 0 */

	while (validmem(ls) && (ctr < idx)) {  /* index down list */
	    ls = ls->ls_host[0];
	    ctr++;
	} /* index down list */

	idx++;			               /* update index for next time */
	   
	if (ls && validmem(ls) && ls->ls_nd == nd) {
	    printf("%s\n%*s%*c%s%-3d  %*s", hdr,
		   LATSTRING(ls->ls_service), (20 - ls->ls_service[0]), ' ',
		   (nd->nd_flags) ? "Unknown     " : "Available   ",
		   ls->ls_rating,
		   LATSTRING(ls->ls_ident));
	    hdr = NULL;
	} /* if ls valid, then print it*/

    } /* while ls valid */

} /* declat_show_node_status */


/*
 * declat_show_node_summary
 * Show summary of nodes like DECserver
 */

void declat_show_node_summary (
     nd_ptr nd,
     char *hdr_ptr)

{
    char *state;

    if (nd->nd_usage > 0) {

/*   Don't know about this, so don't do it.
	printf("%3d", nd->nd_usage);
*/
	state = "Connected      ";
    } else {
	if (nd->nd_flags & ND_UNKNOWN)
	  state = "Unknown      ";
	else if (nd->nd_flags & ND_UNREACH)
	  state = "Unreachable  ";
	else state = "Reachable    ";
    }

    printf("%s\n%*s%*c%s%*s", hdr_ptr, LATSTRING(nd->nd_node), 
	   (20-nd->nd_node[0]), ' ', state, 
	   LATSTRING(nd->nd_ident));
}



/*
 * declat_show_nodes
 * Show counters, status, or summary of a node or all nodes like a DECserver
 */

void declat_show_nodes (
     char *name,
     void (*funct_ptr) (nd_ptr, char *))

{ /* declat_show_summary_nodes */

    byte nd_name[20] = {0};	/* for name in latstring format */
    nd_ptr nd;			/* node pointer for lookups */
    boolean printed= FALSE;	/* have any nodes been printed  */
    int ctr;			/* temp integer variable */
    char *hdr;			/* temp var to avoid poison free probs */

    automore_enable(NULL);

    if (name[0] != 0) {	/* we got passed a name */
	ctr = str_convert(nd_name, (byte *) name, 20, 0);
	nd = nd_show(nd_name, NULL);
	if (nd == NULL) 	/* No node by that name */
	    printf("\nLocal -710- Node %*s not known", LATSTRING(nd_name));
	else {
	    if (funct_ptr == declat_show_node_summary)
		hdr = "\nNode Name           Status       Identification\n";
	    else
		hdr = NULL;
	    (*funct_ptr)(nd, hdr); /* node exsits */
	    if (funct_ptr != declat_show_node_counters)
		printf("\n");
	}

	return;			/* we're done, get out */
    } /* we got passed a name */

				/*  No name passed in - loop through nodes */

    nd = nd_list(nd_name);	/* get first node */

    for (; nd != NULL; nd = nd_list(nd_name)) {
	str_move(nd_name, nd->nd_node);
	if (lat_compare_groups(nd->nd_acl, stdio->tty_lat_groupmask)) {
	    if ((!printed) && (funct_ptr == declat_show_node_summary)) 
		hdr = "\nNode Name           Status       Identification\n";
            else
		hdr = NULL;
	    printed = TRUE;
	    (*funct_ptr)(nd, hdr);
	} /* allowed to see this one */
    } /* loop through the nodes */

    if ((printed) && (funct_ptr != declat_show_node_counters))
	printf("\n");
    automore_disable ();

} /* declat_show_summary_nodes */
