/* $Id: community.c,v 3.4.20.6 1996/08/28 12:52:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/community.c,v $
 *------------------------------------------------------------------
 * community.c -- IP community list related functions.
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: community.c,v $
 * Revision 3.4.20.6  1996/08/28  12:52:01  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.20.5  1996/07/29  21:33:12  rchandra
 * CSCdi64437:  BGP: community display is not user-friendly
 * Branch: California_branch
 * - add user friendly community display
 * - add update throttling code (CSCdi64451)
 * - allow 'set community <additive>' during redistribution into BGP
 *
 * Revision 3.4.20.4  1996/05/04  01:21:25  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.4.20.3  1996/04/16  19:01:27  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/03/28  23:59:14  rchandra
 * CSCdi52098:  BGP: exact keyword not accepted by route-map in community
 * match
 * Branch: California_branch
 * - fix my silly bug in handling exact community matching
 * - limit the range of valid community to 0xFFFF0000
 * - fix a cosmetic bug in show ip community-list
 *
 * Revision 3.4.20.1  1996/03/18  20:27:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.2  1996/03/07  09:49:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/02/20  01:02:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/01  06:05:09  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/11  07:43:08  rchandra
 * CSCdi45046:  BGP: prefer external route over confederation external
 * route
 * - prefer external route over the confederation external route
 * - do not drop routes with 'no-export' community in the sub-AS boundary
 * - add 'local-AS' special community
 *
 * Revision 3.2  1995/11/17  17:32:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/26  00:14:13  rchandra
 * CSCdi39369:  BGP: show ip bgp community must display route with any
 * community
 * - also make the internet community value in community-list to NVGEN.
 *
 * Revision 2.1  1995/06/07  21:05:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../iprouting/msg_iproute.c"	/* Not a typo, see logger.h */
#include "parser_defs_community.h"
#include "../util/sorted_array.h"
#include "community.h"
#include "iprouting_debug.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "bgp.h"
#include "bgp_private.h"

/*
 * The community list space is divided into standard and extended with each
 * list supporting 100 elements.
 *
 * The standard list can contain communities in the new and old format along
 * with well-known community strings. The extended community list can contain
 * community strings expressed as a regular expression. 
 *
 * The regular expression will be considered as a text and consumed as it it.
 * This forces the user to enter
 *
 * (1) all the "well-known" community strings in the ascii string format.
 * (2) the communities in the ascending order.
 * (3) community format should correspond to the format used by router during
 *	display. If the user has configured "ip bgp new-format", then the
 *	communities in the reg-exp should be in new-format. The converse is
 *	also true.
 * Failure to do so will result in reg-exp not matching the community strings
 * in the received update.
 */

/*
 * Globals
 */
boolean new_community_format = FALSE;

/*
 * local storage
 */
static char		bgp_router_com_string[BGP_COMMUNITY_ASCIISIZE];
static regexpacctype	*community_regexps[MAX_EXT_COMLIST+1];
static comlist_item	*comlist_array[MAX_STD_COMLIST+1];

/*
 * ASCII_COMMUNITY_SIZE is based on the size of these well-known community
 * strings.
 */
static const id_string community_items[] = {
	{COMMUNITY_NOEXPORT, "no-export"},
	{COMMUNITY_NOADV, "no-advertise"},
	{COMMUNITY_LOCALAS, "local-AS"},	
	{COMMUNITY_INTERNET, "internet"},
	{0, NULL}
};

/*
 * Extern declarations.
 */
extern message msgsym(INSUFMEM, BGP);


/*
 * ip_community_format
 */
void
ip_community_format (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(new_community_format, "ip bgp-community new-format");
	return;
    }

    new_community_format = csb->sense ? TRUE : FALSE;
}

/*
 * community_command
 * Parse the community-list command.
 * If the community-list is configured it works in three stages, it is
 * explained in cfg_ip_community-list.h
 */
void
ip_community_command (parseinfo *csb)
{
    int list, action;
    boolean permit;
    comlist_item *comptr, *next, *last;
    static comlist_item com_entry;
    ulong community;

    action = GETOBJ(int, 4);

    if (action == PARSE_COMMUNITYLIST_INIT) {
	com_entry.count = 0;
	return;
    }
    
    if (action == PARSE_COMMUNITYLIST_ENTRY) {
	if (com_entry.count < MAX_COMLIST_ENTRY) {
	    if (GETOBJ(int,3) != 0)
		community = GETOBJ(int,3);
	    else
		community = (GETOBJ(int,5) << 16) + GETOBJ(int,6);
	    sorted_array_add(com_entry.community, &com_entry.count, community);
	} else
	    printf("\n%% Cannot have more than %d community attributes",
		   MAX_COMLIST_ENTRY);
	return;
    }

    if (action == PARSE_COMMUNITYLIST_END) {
	list = GETOBJ(int, 1);
	permit = GETOBJ(int, 2);

	if (list > MAX_STD_COMLIST)
	    return;

	last = NULL;
	comptr = comlist_array[list];
	if (csb->sense) {
	    for (; comptr; comptr = comptr->next) {
		if ((com_entry.count == comptr->count) &&
		    (comptr->grant == permit) &&
		    !bcmp(com_entry.community, comptr->community, 
			 COMMUNITY_LEN(comptr->count)))
		    return;
		last = comptr;
	    }

	    comptr = malloc(sizeof(comlist_item));
	    if (!comptr)
		return;

	    comptr->count = com_entry.count;
	    comptr->grant = permit;
	    bcopy(com_entry.community, comptr->community, 
		  COMMUNITY_LEN(com_entry.count));

	    if (last)
		last->next = comptr;
	    else
		comlist_array[list] = comptr;
	} else {
	    comptr = comlist_array[list];
	    comlist_array[list] = NULL;
	    for (; comptr; comptr = next) {
		next = comptr->next;
		free(comptr);
	    }
	}
	bgp_community_list_callback(list);     /* force BGP to purge cache */
	return;
    }
    if (action == PARSE_COMMUNITYLIST_REXP) {
        list = regexpaccess_command(csb, community_regexps);
        if (list)
            bgp_community_list_callback(list);
	else
	    printf("\n%% Could not accept the entry. Duplicate or No memory");
	return;
    }
}

/*
 * ip_print_community_string
 * 
 * Print community in the aa:nn format
 */
static int 
ip_print_community_string (char *bufptr, id_string const idarray[],
			   ulong community)
{
    id_string const *idstr;
    ushort asnumber, len = 0;

    for (idstr = idarray; idstr->str; idstr++) {
	if (idstr->id == community) {
	    if (bufptr)
		len = sprintf(bufptr, "%s", idstr->str);
	    else
		printf("%s", idstr->str);
	    return(len);
	}
    }

    if (new_community_format) {
	asnumber = community >> 16;
	if (bufptr)
	    len = sprintf(bufptr, "%d:%d", asnumber,
			  (community - (asnumber << 16)));
	else
	    printf("%d:%d", asnumber, (community - (asnumber << 16)));
    } else {
	if (bufptr)
	    len = sprintf(bufptr, "%d", community);
	else
	    printf("%d", community);
    }
    return(len);
}

/*
 * ip_community_print_array
 *
 * Helper routine to print out all information in a community array.
 */
void
ip_community_print_array (ulong *community, int count)
{
    int i;

    for (i = 0; i < count; ++i) {
	if (i > 0)
	    printf(" ");
	ip_print_community_string(NULL, community_items, community[i]);
    }
}

/*
 * ip_community_show_list
 *
 * Print out the contents of a given (or all) community list(s).
 */
void
ip_community_show_list (parseinfo *csb)
{
    int list, i;
    comlist_item *comptr;

    automore_enable(NULL);
    list = GETOBJ(int, 1);

    for (i = 1; i <= MAX_STD_COMLIST; ++i) {
	if (comlist_array[i] == NULL || (list > 0 && list != i))
	    continue;
	printf("\nCommunity standard list %d", i);
	comptr = comlist_array[i];
	while (comptr) {
	    printf("\n     %s ", (comptr->grant ? "permit" : "deny"));
	    ip_community_print_array(comptr->community, comptr->count);
	    comptr = comptr->next;
	}
    }
    show_regexpacl(community_regexps, list, "Community extended");
}

/*
 * nv_add_community_string
 *
 * NVgen community in the aa:nn format
 */
static void
nv_add_community_string(id_string const idarray[], ulong community)
{
    id_string const *idstr;
    ushort asnumber;

    for (idstr = idarray; idstr->str; idstr++) {
	if (idstr->id == community) {
	    nv_add(TRUE, "%s", idstr->str);
	    return;
	}
    }
    
    if (new_community_format) {
	asnumber = community >> 16;
	nv_add(TRUE, "%d:%d", asnumber, (community - (asnumber << 16)));
    } else
	nv_add(TRUE, "%d", community);
}

/*
 * ip_community_nv_array
 *
 * Helper routine that prints out community information
 */
void
ip_community_nv_array (ulong *community, int count)
{
    int i;

    for (i = 0; i < count; ++i) {
	nv_add(count > 0, " ");
	nv_add_community_string(community_items, community[i]);
    }
}

/*
 * ip_community_nvgen
 */
void
ip_community_nvgen (parseinfo *csb) 
{
    int i;
    comlist_item *comptr;

    for (i = 1; i <= MAX_STD_COMLIST; ++i) {
	comptr = comlist_array[i];
	for (; comptr; comptr = comptr->next) {
	    nv_write(TRUE, "%s %d %s", csb->nv_command, i, 
		     (comptr->grant ? "permit" : "deny"));
	    ip_community_nv_array(comptr->community, comptr->count);
	}
    }
    regexpaccess_command(csb, community_regexps);
}

/*
 * bgp_com2string
 *
 * Converts the community list into an ascii string.
 */
void
bgp_com2string(ulong *comm, int comm_count, char *comm_str, int size)
{
    int   ix;
    char  *strptr = comm_str;
 
    for (ix = 0; ix < comm_count; ix++) {
        if ((ix * ASCII_COMMUNITY_SIZE) > size)
            goto buf_overflow;
	if (ix)
	    strptr += sprintf(strptr, "%s", " ");
        strptr += ip_print_community_string(strptr, community_items,
					     comm[ix]);
    }
    *strptr = '\0';
    return;
 
buf_overflow:
    errmsg(&msgsym(INSUFMEM, BGP), "community");
    return;
}


/*
 * ip_community_check
 *
 * Check if the com_array satisfy the community-list.
 * Supports exact match as well as the check for the presence of comlist
 * entries in comarray.
 *
 * Return FALSE if not satisfied else return grant.
 */
boolean
ip_community_check (ulong *com_array, int comcount, int list, boolean exact) 
{
    comlist_item *comptr;

    if (list == 0)
	return(TRUE);

    if (list > MAX_EXT_COMLIST) {
	errmsg(&msgsym(BADCOMLIST, IPRT), list);
	return(FALSE);
    }

    /*
     * Community attribute value 0 stands for the internet community. It
     * will the first community if present. All routes are members of it.
     * Entries are all sorted and enables us to use bcmp for exact match.
     * If not exact match, then brute force.
     */
     if (list <= MAX_STD_COMLIST) {
	 for (comptr = comlist_array[list]; comptr; comptr = comptr->next) {
	     if (*(comptr->community) == COMMUNITY_INTERNET)
		 return(comptr->grant);
	     if (exact) {
		 if ((comcount == comptr->count) &&
		     !bcmp(comptr->community, com_array,
			   COMMUNITY_LEN(comcount)))
		     return(comptr->grant);
	     } else {
		 if (sorted_array_partial_compare(comptr->community,
						  comptr->count, com_array,
						  comcount))
		     return(comptr->grant);
	     }
	 }
     }
     else if (list <= MAX_EXT_COMLIST) {
	 bgp_com2string(com_array, comcount, bgp_router_com_string,
			BGP_COMMUNITY_ASCIISIZE);
	 return(regexp_accesscheck(bgp_router_com_string, list,
				   community_regexps));
     }
     return(FALSE);
}
