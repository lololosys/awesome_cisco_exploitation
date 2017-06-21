/* $Id: vsvc_st.c,v 3.7.18.4 1996/09/04 23:54:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vsvc_st.c,v $
 *------------------------------------------------------------------
 * vsvc_st.h - Support for the `Streettalk' Service.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *
 * A BRIEF WORD ABOUT TERMINOLOGY
 * - ----- ---- ----- -----------
 * Streettalk uses a three part name in the form:
 *
 *     <item>@<domain>@<org>
 *
 * This code uses the variable names 'item_name', 'domain_name', and
 * 'org_name' to refer to the individual pieces of a Streetalk name,
 * and the variable 'full_name' to refer to the entire thing.  The
 * term 'group_name' refers to the combination "<domain>@<org>".
 *------------------------------------------------------------------
 * $Log: vsvc_st.c,v $
 * Revision 3.7.18.4  1996/09/04  23:54:46  snyder
 * CSCdi68120:  save a little memory here and there
 *              declare things that are const, const
 *              savings 300 out of data section
 *                       64 out of image
 * Branch: California_branch
 *
 * Revision 3.7.18.3  1996/08/07  09:04:10  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.7.18.2  1996/06/28  23:31:38  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.18.1  1996/03/18  22:33:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.2  1996/03/07  11:05:46  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.16.1  1996/02/20  21:48:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/07  16:16:56  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/02/01  06:11:40  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1995/12/08  04:56:31  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.4  1995/12/07  22:33:47  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/12/01  21:39:46  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.2  1995/11/17  17:59:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:46:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/08/25  11:45:45  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.7  1995/07/27 07:31:19  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.6  1995/07/24 22:33:06  hampton
 * Further work to parts of VINES to make it use the passive timer macros
 * for all its timers.  It no longer have any direct references to the
 * system clock.  [CSCdi37584]
 *
 * Revision 2.5  1995/07/07 20:40:56  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.4  1995/06/23 18:49:01  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.3  1995/06/20  20:11:31  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:25:09  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:16:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#include <string.h>
#include <ciscolib.h>
#include "config_register.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../os/clock.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"
#include "parser_defs_vines.h"
#include "../util/random.h"

#include "vsvc_st.h"
#include "vsvc_st.rpch"
#include "vsvc_st.rpcc"
#include "vsvc_vs.h"

/*
 * StreetTalk Constants
 */
#define VST_MIN_VERSION 40000		/* minimum version for colddump */

boolean streettalk_enabled;
static char *const vst_ni_names[] = {"0", "summary", "detail", 
                                     "need", "welcome"};
static char *vst_1st_orgname, *vst_2nd_orgname;
static boolean vst_needs_cold_dump;
static uchar vst_search_diameter;
static ulong vst_saved_change_id;

/*
 * The Streettalk specific timers.  Most of these indicate when a
 * message of a specific type needs to be sent.
 */
static sys_timestamp	vst_search_ends;
static sys_timestamp	vst_timer_search;
static sys_timestamp	vst_timer_summary;

/*
 * Message destinations.  Several of the above timers are for sending
 * messages to specific destinations.  The variables below maintin the
 * destination addresses.
 */
vst_msg_dest	vst_dest_welcome;
vst_msg_dest	vst_dest_detail;
vst_msg_dest	vst_dest_need;

/*
 * Domain@Org Information
 *
 * The complete set of domain/org combinations is maintained as an RB
 * Tree.  This tree has an additional dqueue thread that maintains a list
 * of all all domains in the orginization "Servers.  This provides for a
 * faster lookup of server names when performing address->name
 * translations.
 */
static rbTree* vines_groups;
static dqueue_t vst_org_servers;
static ulong		vst_localgroup_id;

/*
 * The queue of generic Streettalk messages to be forwarded.
 */
static queuetype 	vst_forwardQ;

/*
 * Streettalk "Handle" related data structures.
 */
static dqueue_t      vst_handleQ;	  /* Existing and partial handles */
static sys_timestamp vst_handle_timer;	  /* Next expiration pass */
static ushort        vst_handle_sequence; /* Embedded seq number in handle */

/*
 * Streettalk "Operation" Call related data structures.
 */
static dqueue_t      vst_operationQ;	  /* Outstanding OP messages */
static sys_timestamp vst_operation_timer; /* Next expiration pass */

/*
 * Forward declarations.
 */
static ushort vst_operate_send_call(vst_handle *, char *, char *, char *, ushort, ushort);


/**********************************************************************
 *
 *			    Streettalk
 *
 **********************************************************************/

/**************************************************
 *
 *         Group Tree Interface Routines
 *
 **************************************************/

/*
 * vst_GroupKey
 * Create a treeKey for a neighbor which is always unique.
 */
static treeKey
vst_GroupKey (char *domain_name, char *org_name)
{
    char local[max(VST_DOMAINLEN, VST_ORGLEN)];
    ulong hash = 0, *u;
    treeKey key;
    ushort i;

    u = (ulong *)&local;
    memset(local, 0, VST_DOMAINLEN);
    sstrncpy(local, domain_name, VST_DOMAINLEN);
    lowercase(local);
    for (i = 0; i < VST_DOMAINLEN/4; i++)
	hash += u[i];

    memset(local, 0, VST_ORGLEN);
    sstrncpy(local, org_name, VST_ORGLEN);
    lowercase(local);
    for (i = 0; i < VST_ORGLEN/4; i++)
	hash += u[i];

    key.l = hash;
    return key;
}

/*
 * vst_GroupCast
 * Do data type conversion between links and groups.
 */
static inline vst_groupentry*
vst_GroupCast (treeLink* link) { return(vst_groupentry*)link; }

/*
 * vst_UnprotectGroup
 * Allow a group to be freed or reused.
 */
static inline void
vst_UnprotectGroup (vst_groupentry* g) 
{ 
    RBTreeNodeProtect(&g->link,FALSE); 
}

/*
 * vst_ProtectGroup
 * Protect a group from being freed.
 */
static inline void
vst_ProtectGroup (vst_groupentry* g) 
{ 
    RBTreeNodeProtect(&g->link,TRUE); 
}

/*
 * vst_GroupProtected
 * Return protection status of group treeLink entry.
 */
static inline boolean
vst_GroupProtected (vst_groupentry* g)
{
    return RBTreeNodeProtected(&g->link);
}

/*
 * GetNextGroup - Search for the next group and return it w/o protection
 */
static vst_groupentry*
vst_GetNextGroup (vst_groupentry* group)
{
    vst_groupentry* g = group;

    if (vines_groups == NULL)
	return NULL;
    if (g == NULL) {
	g = vst_GroupCast(RBTreeFirstNode(vines_groups));
    } else {
	g = vst_GroupCast(RBTreeNextNode(&g->link));
    }
    return g;
}

/*
 * vst_GetFirstGroup
 * Get the first group from the tree.
 */
static inline vst_groupentry*
vst_GetFirstGroup (void)
{ 
    return vst_GetNextGroup(NULL); 
}

#ifdef KEEP_UNUSED_FUNCTION
/*
 * vst_GetBestGroup
 * Return the specified group OR the next greater group.
 */
static vst_groupentry*
vst_GetBestGroup (treeKey key)
{
    return vst_GroupCast(RBTreeBestNode(vines_groups,key));
}
#endif

/*
 * vst_FindGroup
 *
 * Find a group for the name domain@org.  Attempt to find the group as
 * maintained on the requested server.  If a server match can't be found,
 * and the server number requested is zero, then return the primary
 * server for this name.  Note: The groupname_thread is sorted in
 * order of priority, but there's no guarantee that the first entry
 * found in the tree is also the highest priority item.
 */
static vst_groupentry*
vst_FindGroup (char *domain_name, char *org_name, ulong serverid_wanted)
{
    vst_groupentry *first, *walking, *primary;
    ushort priority;
    dqueue_t *ptr;
    treeKey key;

    key = vst_GroupKey(domain_name, org_name);
    if (vines_groups == NULL)
	return (NULL);
    first = vst_GroupCast(RBTreeSearch(vines_groups,key));
    if (first == NULL)
	return(NULL);
    if (DQUEUE_EMPTY(first->groupname_thread)) { /* only entry */
	if (first->serverid == serverid_wanted)
	    return(first);
	return(NULL);
    }

    primary = first;
    priority = first->priority;
    RUN_DQUEUE(first->groupname_thread, ptr) {
	walking = ptr->parent;
	if (walking->serverid == serverid_wanted)
	    return(walking);
	if (walking->priority < priority) {
	    primary = walking;
	    priority = walking->priority;
	}
    }
    if (serverid_wanted != VINES_UNKNOWNNET)
	return(NULL);
    return(primary);
}

/*
 * vst_InsertGroup
 * Add a group to our table.
 */
static void
vst_InsertGroup (vst_groupentry* g) 
{
    vst_groupentry *existing, *prev;
    dqueue_t *ptr;
    treeKey key;

    key = vst_GroupKey(g->domain, g->org);
    if (vines_groups == NULL)
	return;
    existing = vst_FindGroup(g->domain, g->org, VINES_UNKNOWNNET);
    if (existing) {
	prev = existing;
	RUN_DQUEUE(existing->groupname_thread, ptr) {
	    if (existing->priority > g->priority)
		break;
	    prev = existing;
	}
	lw_insert(&g->groupname_thread, &prev->groupname_thread);
    }
    if (strcasecmp(g->org, vst_1st_orgname) == 0) {
	g->is_servername = TRUE;
	lw_insert(&g->servername_thread, &vst_org_servers);
    }
    RBTreeInsert(key,vines_groups,&g->link);
}

/*
 * vst_ReinitGroup
 * Release any resources in use for this group
 */
static void
vst_ReinitGroup (vst_groupentry* g)
{
				/* Currently no special resources */
}

/*
 * vst_CreateGroup
 * initialize a new group
 */
static vst_groupentry*
vst_CreateGroup (char *domain_name, char *org_name, ushort priority)
{
    vst_groupentry* g;

    if (vines_groups == NULL)
	return NULL;

    g = (vst_groupentry*) RBTreeGetFreeNode(vines_groups);
    if (g != NULL)
	vst_ReinitGroup(g);
    else
	g = malloc(sizeof(struct vst_groupentry_));
    if (g == NULL)
	return NULL;
    memset((uchar*)g, 0,sizeof(struct vst_groupentry_)); /* clear node */
    sstrncpy(g->org, org_name, VST_ORGLEN);
    sstrncpy(g->domain, domain_name, VST_DOMAINLEN);
    g->priority = priority;
    o_init(&g->servername_thread, g);
    o_init(&g->groupname_thread, g);
    vst_InsertGroup(g);
    return g;
}

/*
 * vst_DeleteGroup
 * Delete a group from our table.
 */
static void
vst_DeleteGroup (vst_groupentry* g)
{
    if ((vines_groups == NULL) || (g == NULL))
	return;
    RBTreeDelete(vines_groups,&g->link);
    lw_remove(&g->groupname_thread);
    lw_remove(&g->servername_thread);
}

/*
 * vst_WalkGroupTree
 *
 * all the specified function for each node in the tree.
 */
static inline boolean
vst_WalkGroupTree (treeProc proc, void *data)
{
    return(RBTreeForEachNode(proc, data, vines_groups, FALSE));
}

/*
 * vroute_WalkPartialTree
 *
 * Call the specified function for each node in the tree.
 */
static /* inline */ boolean
vst_WalkPartialGroupTree (treeProc proc, vst_groupentry *g, void *data)
{
    if (g)
	return(RBTreeForEachNodeTilFalse(proc, data, vines_groups,
					 &g->link, FALSE));
    return(RBTreeForEachNodeTilFalse(proc, data, vines_groups,
				     NULL, FALSE));
}


/**************************************************
 *
 *	       ST - Miscellaneous
 *
 *  General Routines that don't fall into a class.
 *
 **************************************************/

/*
 * vines_is_st_welcome
 *
 *
 */
boolean vines_is_st_welcome (paktype *pak)
{
    vinesipc2type *ipc2;
    vrpc_call *rpc;
    ushort *data;

    ipc2 = (vinesipc2type *)pak->transport_start;
    if (ipc2->type != VIPC_DATAGRAM)
	return(FALSE);

    rpc = (vrpc_call *)ipc2->data;
    if ((rpc->type != VRPC_CALL) || (rpc->procedure != VST_NI))
	return(FALSE);

    data = (ushort *)rpc->data;
    return(data[0] == VST_NI_WELCOME);
}

/*
 * vst_set_master_timer
 */
static inline void vst_set_master_timer (sys_timestamp when)
{
    mgd_timer_set_soonest(&vsvc_st_info.timer, when);
}

/*
 * vsvc_break_name
 *
 * Given a complete vines name, split it into its component parts with full
 * error checking.  The current data structures do not allow for easy checking
 * for the existance of the org name, so this code does not check for the
 * existance of the organization name.  It thus will never return the error
 * codes that indicate "the org doesn't exist", or "the org exists but the
 * group doesn't".  It will return the error that says "the combination of
 * domain@org" doesn't exist.
 */
ushort vsvc_break_name (
    char *full_name,
    char *item_name,
    char *domain_name,
    char *org_name)
{
    char *c, *start, *end;
    ushort length;

    if (strlen(full_name) >= VST_TOTALLEN)
	return(VST_ERR_NAMELONG);
    
    for (c = full_name; *c; c++) {
	if (isalnum(*c))
	    continue;
	if (strchr(VST_CHAR_ALLOWED, *c) != NULL)
	    continue;
	return(VST_ERR_BADNAME);
    }

    start = full_name;
    if (item_name) {
	end = strchr(start, '@');
	length = end - start;
	if (length > VST_ITEMLEN)
	    return(VST_ERR_OBJLONG);
	sstrncpy(item_name, full_name, length+1); /* allow for null */
	start = end + 1;
    }

    end = strchr(start, '@');
    length = end - start;
    if (length > VST_DOMAINLEN)
	return(VST_ERR_DOMLONG);
    sstrncpy(domain_name, start, length+1); /* allow for null */
    start = end + 1;

    end = strchr(start, '@');
    length = end - start;
    if (length > VST_ORGLEN)
	return(VST_ERR_ORGLONG);
    sstrncpy(org_name, start, length+1); /* allow for null */
    start = end + 1;

    end = strchr(start, '@');
    if (end != NULL)
	return(VST_ERR_NAMELONG);
    return(VSVC_NO_ERROR);
}

/*
 * vst_start_search
 *
 * Setup variables to start sending search messages looking for a
 * streettalk service to provide a cold dump.  Be nice and sent two
 * searches with a xzero hop count before increasing the diameter of the
 * search.
 */
static inline void vst_start_search (void)
{
    vst_search_diameter = 0;
    vst_send_search(FALSE);
    TIMER_START(vst_timer_search, VST_SEARCH_INTERVAL);
    TIMER_START(vst_search_ends, VST_SEARCH_MAX);
    vst_set_master_timer(vst_timer_search);
}

/*
 * vst_stop_search
 *
 * Change variables to stop sending search messages looking for a
 * streettalk service to provide a cold dump.
 */
static inline void vst_stop_search (void)
{
    TIMER_STOP(vst_timer_search);
    TIMER_STOP(vst_search_ends);
}

/**************************************************
 *
 *	       ST - Miscellaneous
 *
 *    Routines related to the various ID fields.
 *
 **************************************************/

/*
 * vst_bump_reinforce
 *
 * Increment the lower half of a ReinforceID.  The upper half is a constant
 * selected when a streettalk service starts running.  This is normally used
 * to increment the ReinforceID of this router before a new message is
 * generated.
 */
static inline void vst_bump_reinforce (serverentry *s)
{
    charlong *num;

    num = (charlong *)&s->st_reinforce_id;
    num->d.sword[1]++;
}

/*
 * vst_is_new_reinforce
 *
 * Compare two Reinforce IDs and determine if one is older than the
 * other.  If the two top halves are different then this is a new
 * instance of the streettalk service.  If they are the same, then the
 * bottom half is checked to see if this is a new message or a duplicate
 * message.
 */
static inline boolean
vst_is_new_reinforce (ulong local, ulong new)
{
    charlong stored, received;

    stored.d.lword = local;
    received.d.lword = new;

    /*
     * If the top halves of the received and stored RIDs are the same,
     * the bottom half is a strictly linear comparison.
     */
    if (stored.d.sword[0] == received.d.sword[0])
	return(stored.d.sword[1] < received.d.sword[1]);

    /*
     * What to do if the upper portion has changed.  I've had conflicting
     * answers from Banyan.  1) Only consider the message as new if the
     * top half of the received RID is higher than the top half of the
     * stored RID.  If the server has rebooted and picked a lower top
     * half than last time, that will be caught and corrected because of
     * the changed DBID or CID in a Detail message.  2) Accept any
     * message where the top half of the received RID is different from
     * the top half of the stored RID.
     */
    /* return(stored.d.sword[0] < received.d.sword[0]);	   #1 */
    return(TRUE);					/* #2 */
}

/*
 * vst_compare_ids
 *
 * Compare all the IDs in an incoming message to determine whether the
 * received information is new, old, etc.
 */
static enum vst_freshness
vst_compare_ids (serverentry *current, vst_server *newinfo)
{
    long diff;

    if (vines_extract_serverid(&newinfo->port) == vines_network)
	return(VST_LOCAL);

    if (current->st_database_id != newinfo->database_id) {
	/*
	 * Database IDs should never change once a group exists.  The
	 * group must have been deleted and recreated.
	 */
	return(VST_CHANGED);
    }

    if (current->st_change_id == 0)
	return(VST_CHANGED);
    diff = ((long)(newinfo->change_id - current->st_change_id));
    if (diff > 0) {
	/*
	 * Something has changed on the server.  Need to get new
	 * information for this server.
	 */
	return(VST_CHANGED);
    } else if (diff < 0) {
	return(VST_OLD);
    }

    if (vst_is_new_reinforce(current->st_reinforce_id, newinfo->reinforce_id)) {
	/*
	 * A new periodic broadcast.  No information has changed.
	 */
	return(VST_PERIODIC);
    } else if (current->st_reinforce_id == newinfo->reinforce_id) {
	/*
	 * A duplicate entry.
	 */
	return(VST_DUPLICATE);
    }
    return(VST_OLD);
}

/*
 * Convert the freshness enum into a string.
 */
static char *vst_freshness_tag (enum vst_freshness which)
{
    switch (which) {
      case VST_LOCAL:		return("this svr");
      case VST_MOVED:		return("moved");
      case VST_CHANGED:		return("changed");
      case VST_PERIODIC:	return("periodic");
      case VST_DUPLICATE:	return("duplicate");
      case VST_OLD:		return("old");
      default:			return("unknown");
    }
    return("unknown");
}

/*
 * vst_new_info
 *
 * Indicate which if the freshness valuds indicates that there is new
 * information available inthe message.
 */
static inline boolean vst_new_info (enum vst_freshness freshness)
{
    return((freshness == VST_CHANGED) || (freshness == VST_PERIODIC));
}

/*
 * vst_flush_old_group
 *
 * Flush all groups that no longer exist on a server.  This is called after
 * the receipt of a detail message to remove any groups that were not present
 * in that detail message.  Banyan doesn't provide for an incremental change
 * mechanism, just a current status mechanism.
 */
static boolean vst_flush_old_group (treeLink *x, void *data)
{
    serverentry* s;
    vst_groupentry *g;

    g = vst_GroupCast(x);
    s = (serverentry *)data;

    if (g->serverid != s->serverid)
	return(TRUE);
    if (g->change_id != s->st_change_id)
	vst_DeleteGroup(g);
    return(TRUE);
}

/*
 * vst_priority2misc
 *
 * Convert the locally stored form of precedence into the form used in
 * Streettalk messages.
 */
static ushort vst_priority2misc (ushort priority)
{
    if (priority == 0)
	return(VST_MISC_PRIMARY);
    return((priority << 12) & VST_MISC_PRECEDENCE);
}

/*
 * vst_misc2priority
 *
 * Convert the locally stored form of precedence into the form used in
 * Streettalk messages.
 */
static ushort vst_misc2priority (ushort misc)
{
    if (misc & VST_MISC_PRIMARY)
	return(0);
    return((misc & VST_MISC_PRECEDENCE) >> 12);
}

/**************************************************
 *
 *	       ST - Miscellaneous
 *
 *     Routines related name/address lookups.
 *
 **************************************************/

/*
 * vst_find_server_by_address
 *
 * Given a server's address, look through the list of all known groups for a
 * domain belonging to the organization "Servers" that is owned by this server.
 * This domain name will be the name of the server itself.  This routine will
 * get invoked indirectly via printf.
 */
char *vst_find_server_by_address (ulong serverid)
{
    vst_groupentry *g;
    dqueue_t *ptr;

    RUN_DQUEUE(vst_org_servers, ptr) {
	g = ptr->parent;
	if (g->serverid == serverid) {
	    return(g->domain);
	}
    }
    return(NULL);
}

/*
 * vst_find_server_by_address
 *
 * Given a server's name, look through the list of all known groups for a
 * domain belonging to the organization "Servers" with this domain name.  This
 * will allow you to extract the server number corresponding to this name.
 */
ulong vst_find_server_by_name (char *servername)
{
    vst_groupentry *g;
    dqueue_t *ptr;

    RUN_DQUEUE(vst_org_servers, ptr) {
	g = ptr->parent;
	if (strcasecmp(servername, g->domain) == 0) {
	    return(g->serverid);
	}
    }
    return(0);
}


/*************************************************
 *
 * 	       ST - Miscellaneous
 *
 *        Routines related to "Handles".
 *
 * Hopefully these routines will never be called.
 * Handle support was added before the "cisco
 * service" and proxy responses to search messages.
 * With those in place, a client should never
 * initiate a session directly with the router.
 **************************************************/

/*
 * vst_set_handle_timer
 *
 * Set the timer on a streettalk 'handle'.
 */
static inline void vst_set_handle_timer (vst_handle *handle, ulong offset)
{
    TIMER_START(handle->timer, offset);
    vst_set_master_timer(handle->timer);
}

/*
 * vst_find_handle
 *
 * Given a "Session Handle" number, look up the internal data structure for
 * that handle.
 */
static vst_handle *vst_find_handle (ulong session_handle)
{
    vst_handle *handle;
    dqueue_t *ptr;

    RUN_DQUEUE(vst_handleQ, ptr) {
	handle = ptr->parent;
	if (session_handle == handle->session_handle)
	    return(handle);
    }
    return(NULL);
}

/*
 * vst_find_valid_handle
 *
 * Given a "Session Handle" number, look up the internal data structure for
 * that handle.  Check to see that the handle found is valid for the specific
 * client port that provided it.  If so, return the handle.  If not,
 * invalidate the handle provided because it might be compromised, and return
 * NULL.
 */
static vst_handle *vst_find_valid_handle (
    vinesipcsocket *socket,
    ulong session_handle)
{
    vst_handle *handle;

    handle = vst_find_handle(session_handle);
    if (handle == NULL)
	return(NULL);

    /* Found the right handle.  Is this the right client port? */
    if ((socket->remote_net == handle->net) &&
	(socket->remote_host == handle->host) &&
	(socket->last_remote_port == handle->port)) {
	vst_set_handle_timer(handle, VST_HANDLE_NORMAL);
	return(handle);
    }
    handle->valid = FALSE;
    return(NULL);
}

/*
 * vst_setup_reply
 *
 * Given a session handle, perform all the common setup functions for sending
 * a query response to this client.  This includes looking up the handle data
 * structure, creating the connection structure if necessary, and allocating
 * the memory block for the response.  Is the response is an error message,
 * this route takes care of actually sending the error message.
 */
static void *vst_setup_reply (
    ulong session_handle,
    vst_handle **handle,
    vinesipcsocket **socket,
    ushort failure_code,
    ushort size)
{
    vst_handle *my_handle = NULL;
    dqueue_t *ptr;
    uchar *temp;

    /*
     * Find the data structure for the supplied session handle.
     */
    RUN_DQUEUE(vst_handleQ, ptr) {
	my_handle = ptr->parent;
	if (session_handle == my_handle->session_handle)
	    break;
    }
    if ((ptr == &vst_handleQ) ||
	(session_handle != my_handle->session_handle)) 
	return(NULL);
    *handle = my_handle;

    /*
     * Get a socket that can be used to send back to the station that
     * sent the original request.
     */
    *socket = vipc_get_conn_by_address(my_handle->net, my_handle->host);
    if (*socket == NULL)
	return(NULL);

    /* 
     * Take care of sending a failure response so the callers doesn't have
     * to have redundant code.
     */
    if (failure_code) {
	vrpc_send_abort(*socket, failure_code);
	return(NULL);
    }

    /*
     * Allocate the memory for sending a successful response.
     */
    temp = malloc(size);
    if (temp == NULL) {
	vrpc_send_abort(*socket, VSVC_ERR_NO_MEMORY);
	return(NULL);
    }
    return(temp);
}

/**************************************************
 *
 *	    ST - Delayed Message Handling
 *
 **************************************************/

/*
 * vst_set_gen_dest
 *
 * Add a destination address to the list of addresses wanting to receive
 * a particular type of message.  When the timer for this type of message
 * expires, the message will be sent to all destinations that have signed
 * up for it.  This routine caps the number of individual destinations at
 * some fixed value, and after than point will cause a broadcast message
 * to be sent.
 */
void vst_set_gen_dest (vst_msg_dest *dst_list, ulong newdest)
{
    ushort i;

    /*
     * Exit if already a broadcast
     */
    if (dst_list->broadcast) {
	if (vst_debug)
	    buginf("\nVST: destination list is broadcast)");
	return;
    }

    /*
     * Duplicate?
     */
    for (i = 0; i < dst_list->count; i++) {
	if (dst_list->dest[i] == newdest) {
	    if (vst_debug)
		buginf("\nVST: destination is already in list (entry %d)", i);
	    return;
	}
    }

    /*
     * Convert to a broadcast if limit has been reached.
     */
    if (dst_list->count == VST_MAX_DEST) {
	dst_list->broadcast = TRUE;
	dst_list->count = 0;
	if (vst_debug)
	    buginf("\nVST: Converting destination list to broadcast");
	return;
    }
    
    /*
     * Just record it
     */
    if (newdest == -1L) {
	dst_list->broadcast = TRUE;
	dst_list->count = 0;
    } else {
	dst_list->dest[dst_list->count++] = newdest;
    }
    if (!TIMER_RUNNING(dst_list->when)) {
	TIMER_START_JITTERED(dst_list->when, VST_XMIT_DELAY, VST_XMIT_JITTER);
	vst_set_master_timer(dst_list->when);
    }
    if (vst_debug)
	buginf("\nVST: Adding %#Z to destination list (entry %d)",
	       newdest, dst_list->count - 1);
}

/*
 * vst_clear_gen_dest
 *
 * Clear the destination address list for a particular type of message.
 * This routine should be called after each message that is generated by
 * the router.
 */
static inline void
vst_clear_gen_dest (vst_msg_dest *dst_list)
{
    ushort i;

    if (!dst_list)
	return;

    dst_list->broadcast = FALSE;
    dst_list->count = 0;
    for (i = 0; i < VST_MAX_DEST; i++)
	 dst_list->dest[i] = 0L;
    TIMER_STOP(dst_list->when);
}

/*
 * vst_propagate
 *
 * Duplicate the current message and queue up the duplicate for later
 * transmission.  The message will be delayed by the normal 30 to 37
 * seconds.  This routine is called from the top level "need info"
 * routine after the specific message type routines have indicated that
 * the message should be forwarded.
 */
static void vst_propagate (
    vinesipcsocket *socket,
    vst_ni_query *query)
{
    vsvc_rtn_info *routine;
    paktype *pak;
    uchar *c;

    routine = vsvc_find_routine(&vsvc_st_info, VST_NI);
    if (!routine)
	return;
    pak = vrpc_b_getbuffer(routine->query_len);
    if (!pak)
	return;

    c = (*routine->query_pack)(pak->application_start, (uchar *)query);
    pak->datagramsize = (c - pak->application_start);
    TIMER_START_JITTERED(pak->transmit_time, VST_XMIT_DELAY, VST_XMIT_JITTER);
    pak->if_output = NULL;
    pak->if_input = socket->last_idb_in->phys_idb;
    enqueue(&vst_forwardQ, pak);
    vst_set_master_timer(pak->transmit_time);
}

/*
 * vst_send_delayed_msgs
 *
 * Send a packet that was queued at time earlier time.  Valid packet
 * locations are pak->transmit_time which contains the time the packet
 * should be sent.
 */
static void vst_send_delayed_msgs (void)
{
    vinesidbtype *use, *exclude;
    vst_ni_query *ni_msg;
    paktype *pak;

    if (QUEUEEMPTY(&vst_forwardQ))
	return;

    while ((pak = dequeue(&vst_forwardQ)) != NULL) {
	if (SLEEPING(pak->transmit_time)) {
	    mgd_timer_set_soonest(&vsvc_st_info.timer, pak->transmit_time);
	    requeue(&vst_forwardQ, pak);
	    return;
	}

	if (vst_debug) {
	    ni_msg = (vst_ni_query *)pak->application_start;
	    buginf("\nVST: Sending pending %s message to %#Z",
		   vst_ni_names[ni_msg->data_type], -1L);
	}
	
	use = pak->if_output ? vines_getidb(pak->if_output) : NULL;
	exclude = pak->if_input ? vines_getidb(pak->if_input) : NULL;
	pak->if_output = pak->if_input = NULL;
	vrpc_send_broadcast_call(pak, use, exclude, 0, 1, VST_NI,
				 vsvc_st_info.well_known_port,
				 vsvc_st_info.transient_port, 0, 0);
    }
}

/*
 * vst_gen_delayed_msg
 *
 * See if this message generation timer has expired.  If it has, build
 * and send the appropriate packet.  If not, make sure that the
 * streettalk timer gets set so that this message will be checked again.
 */
static void vst_gen_delayed_msg (
    vst_msg_dest *dst_list,
    void fn(void))
{
    if (TIMER_RUNNING_AND_AWAKE(dst_list->when)) {
	(*fn)();
	vst_clear_gen_dest(dst_list);
	return;
    }
    mgd_timer_set_soonest(&vsvc_st_info.timer, dst_list->when);
}


/**************************************************
 *
 *	         ST - Lookup (0)
 *
 **************************************************/

/*
 * vst_lookup_rcv_call
 *
 * A client sent the router a request for some information.  Deflect the
 * lookup to the server that owns this group.
 */
ushort vst_lookup_rcv_call (
    vinesipcsocket *socket,
    vst_lookup_query *query,
    vst_lookup_reply *reply)
{
    char item_name[VST_ITEMLEN], domain_name[VST_DOMAINLEN], org_name[VST_ORGLEN];
    vst_handle *handle;
    ushort error;

    /*
     * First, validate the session handle
     */
    handle = vst_find_valid_handle(socket, query->handle);
    if (handle == NULL)
	return(VST_ERR_INVSESHAN);

    /*
     * Second, validate the name
     */
    error = vsvc_break_name(query->name, item_name, domain_name, org_name);
    if (error)
	return(error);

    /*
     * Now send an Operate query.
     */
    return(vst_operate_send_call(handle, item_name, domain_name, org_name,
				 query->class, VST_LOOKUP));
}

/*
 * vst_lookup_finished
 *
 * The Operate call sent by the above routine has finished.  Copy the returned
 * information into a lookup reply packet and send it.
 */
static void vst_lookup_finished (
    ulong session_handle,
    char *full_name,
    vst_st_record *st_info,
    ushort failure_code)
{
    vst_lookup_reply *reply;
    vinesipcsocket *socket;
    vst_handle *handle;

    /*
     * Setup for positive response.  Handles negative response and
     * failure modes.
     */
    reply = vst_setup_reply(session_handle, &handle, &socket,
			    failure_code, sizeof(vst_lookup_reply));
    if (reply == NULL)
	return;

    /*
     * Fill it in and send it
     */
    sstrncpy(reply->full_name, full_name, VST_TOTALLEN);
    bcopy(st_info, &reply->st_info, sizeof(vst_st_record));
    vst_lookup_send_reply(socket, reply);
    free(reply);
}

/**************************************************
 *
 *	         ST - Logout (14)
 *
 **************************************************/

/*
 * vst_logout_rcv_call
 *
 * The client is indicating its desire to log out of Streettalk.  Delete
 * the handle information for this client, if there is any.
 */
ushort vst_logout_rcv_call (
    vinesipcsocket *socket,
    vst_logout_query *query,
    vst_logout_reply *reply)
{
    vst_handle *handle;

    handle = vst_find_valid_handle(socket, query->handle);
    if (handle == NULL)
	return(VST_ERR_INVSESHAN);
    lw_remove(&handle->q);
    free(handle);
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	         ST - Get Ports (24)
 *
 **************************************************/

/*
 * vst_getports_rcv_call
 *
 * A client sent the router a request for some information.  Deflect the
 * lookup to the server that owns this group.
 */
ushort vst_getports_rcv_call (
    vinesipcsocket *socket,
    vst_getports_query *query,
    vst_getports_reply *reply)
{
    char item_name[VST_ITEMLEN], domain_name[VST_DOMAINLEN], org_name[VST_ORGLEN];
    vst_handle *handle;
    ushort error;

    /*
     * First, validate the session handle
     */
    handle = vst_find_valid_handle(socket, query->handle);
    if (handle == NULL)
	return(VST_ERR_INVSESHAN);

    /*
     * Second, validate the name
     */
    error = vsvc_break_name(query->name, item_name, domain_name, org_name);
    if (error)
	return(error);

    /*
     * Now send an Operate query to the station that owns this group.
     */
    return(vst_operate_send_call(handle, item_name, domain_name, org_name,
				 query->class, VST_GETPORTS));
}

/*
 * vst_getports_finished
 *
 * The Operate call sent by the above routine has finished.  Copy the returned
 * information into a getports reply packet and send it.
 */
static void vst_getports_finished (
    ulong session_handle,
    char *full_name,
    vst_st_record *st_info,
    ushort failure_code)
{
    vst_getports_reply *reply;
    vinesipcsocket *socket;
    vst_handle *handle;

    /*
     * Setup for positive response.  Handles negative response and
     * failure modes.
     */
    reply = vst_setup_reply(session_handle, &handle, &socket,
			    failure_code, sizeof(vst_getports_reply));
    if (reply == NULL)
	return;

    /*
     * Fill it in and send it
     */
    sstrncpy(reply->name, full_name, VST_TOTALLEN);
    reply->talk_port_count = 1;
    reply->talk_port[0].index = 0;
    bcopy(&st_info->class.service.service.port, &reply->talk_port[0].port,
	  sizeof(vinesaddress));
    vst_getports_send_reply(socket, reply);
    free(reply);
}

/**************************************************
 *
 *	      ST - Begin Session (28)
 *
 **************************************************/

/*
 * vst_begin_rcv_call
 *
 * The client is indicating its desire to log in to Streettalk.  This routine
 * will validate the user with his owning VINES Security service, and then
 * issue a Session Handle of the user is valid.
 */
ushort vst_begin_rcv_call (
    vinesipcsocket *socket,
    vst_begin_query *query,
    vst_begin_reply *reply)
{
    vst_handle *new_handle;
    char *string;

    if (vst_debug) {
	string = vsvc_print_nonce(&query->client_nonce);
	buginf("\nVST:   nonce %s", string);
    }

    /*
     * Create a partially filed out handle record and enqueue it.
     */
    new_handle = malloc(sizeof(vst_handle));
    if (new_handle == NULL) {
	return(VSVC_ERR_NO_MEMORY);
    }
    o_init(&new_handle->q, new_handle);
    new_handle->valid = FALSE;
    new_handle->net = socket->remote_net;
    new_handle->host = socket->remote_host;
    new_handle->port = socket->last_remote_port;
    bcopy(&query->client_nonce, &new_handle->nonce, VINES_NONCE_LEN);
    new_handle->username = NULL;
    vst_set_handle_timer(new_handle, VST_HANDLE_SHORT);
    lw_insert(&new_handle->q, &vst_handleQ);

    /*
     * Kick off the validation of this user.
     */
    vvs_validate_send_call(socket, &query->client_nonce);
    return(VSVC_DEFERRED_REPLY);
}

/*
 * vst_user_validated
 *
 * The VINES Security Validate call sent by the above routine has finished.
 * If the user is valid, finish filling in the local handle data structure and
 * return the session handle to the client.  Otherwise, delete the partial
 * session handle and pass the VS error back to the client.
 */
void vst_user_validated (ushort fail_code, char *username)
{
    vst_begin_reply *reply;
    vst_handle *handle;
    vinesipcsocket *socket;

    /*
     * Setup for positive response.  Handles negative response and
     * failure modes.
     */
    reply = vst_setup_reply(0L, &handle, &socket, fail_code,
			    sizeof(vst_lookup_reply));
    if (reply == NULL) {
	if (handle) {
	    lw_remove(&handle->q);
	    free(handle);
	}
	return;
    }

    handle->valid = TRUE;
    handle->session_handle = (vst_handle_sequence++ << 16) | 0x0100;
    setstring(&handle->username, username);

    reply->handle = handle->session_handle;
    vst_set_handle_timer(handle, VST_HANDLE_NORMAL);
    vst_begin_send_reply(socket, reply);
    free(reply);
}


/**************************************************
 *
 *	       ST - Cold Dump (34)
 *
 **************************************************/

/*
 * vst_cold_dump_rcv_reply
 *
 * A Cold Dump reply message was received in response to our request
 * message.  Parse the packet and insert all of the entries into the
 * local group tables.  This routine will also insert as much information
 * as possible into the server tables too.
 */
void vst_cold_dump_rcv_reply (
    vinesipcsocket *socket,
    vst_cold_dump_reply *reply)
{
    vst_cold_dump_server *server;
    ushort count, num_entries;
    serverentry *s;
    vst_groupentry *g;
    ulong serverid;
    ushort priority;

    num_entries = reply->server_count;
    for (count = 0; count < num_entries; count++) {
	server = &reply->server[count];
	serverid = vines_extract_serverid(&server->port);
	if (vst_debug) {
	    buginf("\nVST:   %#15Z                    DI %08x, CI %08x",
		   serverid, server->database_id, server->change_id);
	    buginf("\nVST:     %15s %15s  N  %08x, D  %04x, M %04x",
		   server->domain, server->org, server->netgroup_id,
		   server->domain_seq, server->misc_info);
	}
	
	/*
	 * Verify that this isn't a locally owned group.
	 */
	if (serverid == vines_network) {
	    if (vst_debug)
		buginf(", ignored");
	    continue;
	}
	if (strcmp(server->domain, vsvc_hostname()) == 0) {
	    if ((strcmp(server->org, vst_1st_orgname) == 0) ||
		(strcmp(server->org, vst_2nd_orgname) == 0)) {
		if (vst_debug)
		    buginf(", ignored");
		continue;
	    }
	}

	/*
	 * Copy all the information on the servers ID numbers.  Do not
	 * update the time on any entry except that of the station that
	 * sent the colddump.  He is the only station that we know is
	 * actually alive based upon the information received.
	 */
	if (!(s = vsvc_FindServer(serverid))) {
	    s = vsvc_CreateServer(serverid);
	    if (!s)
		continue;
	}
	bcopy(&server->port, &s->st_port, sizeof(vinesaddress));
	s->st_info_valid = TRUE;
	s->st_need_info = FALSE;
	if (serverid == socket->remote_net)
	    TIMER_START(s->st_last_update, VST_EXPIRATION);

	/*
	 * Copy all the information provided on the group.  If this is a
	 * domain in the organization "Servers", then some of this
	 * information can be added to the servers table.
	 */
	g = vst_FindGroup(server->domain, server->org, serverid);
	if (g == NULL) {
	    priority = vst_misc2priority(server->misc_info);
	    g = vst_CreateGroup(server->domain, server->org, priority);
	    if (g == NULL)
		continue;
	} else if (g->localgroup_id) {
	    continue;
	}
	g->serverid = serverid;
	g->netgroup_id   = server->netgroup_id;
	g->localgroup_id = 0;
	g->domain_seq    = server->domain_seq;
	TIMER_START(g->expires, VST_EXPIRATION);
	if (g->is_servername) {
	    s->st_database_id  = server->database_id;
	    s->st_change_id    = server->change_id;
	}
    }

    /*
     * Are we done?  If the "pass" entry is non-null, then there is more
     * data to be had.  Go ahead and ask for it now.
     */
    if (strlen(reply->pass)) {
	buginf("\nVST:     pass: `%s'", reply->pass);
	vst_cold_dump_send_call(socket->remote_net, reply->pass);
	return;
    }

    /*
     * Done.  Clear the search information.
     */
    vst_needs_cold_dump = FALSE;
    vst_stop_search();
}

/*
 * vst_cold_dump_insert_group
 *
 * This routine will be called once for each group in the group tables.
 * Its job is to insert this information into an outgoung Cold Dump Reply
 * message.  If the packet currently being built is full, it write the
 * last group name into the pass field so that the requestor will know to
 * ask for more.  It also returns false to break the look and have the
 * packet sent.
 *
 * Excerpt from mail from Jim Toga (toga@banyan.com):
 *
 * Only primarily location of groups are passed in the Cold dump.
 * The knowledge of shadows is passed via the 'normal' sync protocol.
 */
static boolean vst_cold_dump_insert_group (treeLink *x, void *data)
{
    char group_name[VST_DOMAINLEN + VST_ORGLEN];
    vst_cold_dump_server *server;
    vst_cold_dump_reply *reply;
    vinesipcsocket *socket;
    ushort misc_info;
    serverentry *s;
    vst_groupentry *g;

    socket = ((vst_aid *)data)->socket;
    reply = ((vst_aid *)data)->data;

    g = vst_GroupCast(x);
    if (g->priority != VST_HIGHEST_PRECEDENCE)
	return(TRUE);
    s = vsvc_FindServer(g->serverid);
    if (!s)
	return(TRUE);
    server = &reply->server[reply->server_count++];
    sstrncpy(server->org, g->org, VST_ORGLEN);
    sstrncpy(server->domain, g->domain, VST_DOMAINLEN);
    bcopy(&s->st_port, &server->port, sizeof(vinesaddress));
    misc_info = vst_priority2misc(g->priority);
    server->misc_info = misc_info;
    server->netgroup_id = g->netgroup_id;
    if (g->is_servername) {
	server->database_id = s->st_database_id;
	server->change_id   = s->st_change_id;
	server->domain_seq  = g->domain_seq;
    }
    if (vst_debug)
	buginf("\nVST:     %15s %15s  N %08x, D %04x, M %04x", g->domain,
	       g->org, g->netgroup_id, g->localgroup_id, g->domain_seq,
	       misc_info);

    /*
     * Set the pass field in case the next group has multiple servers
     * and can't fit into the current message.
     */
    sprintf(reply->pass, vst_group2name(g, group_name));
    if (reply->server_count == VST_COLDDUMP_MAX_GROUPS)
	return(FALSE);
    return(TRUE);
}

/*
 * vst_cold_dump_rcv_call
 *
 * Build and send a "Cold Dump" reply to someone.  This uses the tree
 * walking routines, and a helper routine.  The helper function will send
 * packets whenever the maximum number of entries for a packet is
 * reached.  It is designed so that there will always be data left over
 * in the packet for this routine to send, since this routine must return
 * either an error or data.
 */
ushort vst_cold_dump_rcv_call (
    vinesipcsocket *socket,
    vst_cold_dump_query *query,
    vst_cold_dump_reply *reply)
{
    char domain_name[VST_DOMAINLEN], org_name[VST_ORGLEN];
    vst_groupentry *g;
    vst_aid args;

    if (strlen(query->pass)) {
	vsvc_break_name(query->pass, NULL, domain_name, org_name);
	g = vst_FindGroup(domain_name, org_name, VINES_UNKNOWNNET);
    } else {
	g = NULL;
    }

    if (vst_debug) {
	buginf("\nVST: Sending Cold Dump Reply Message");
	if (strlen(query->pass))
	    buginf(", starting with %s", query->pass);
    }
    args.socket = socket;
    args.data = reply;
    reply->server_count = 0;
    if (vst_WalkPartialGroupTree(vst_cold_dump_insert_group, g, &args))
	reply->pass[0] = '\0'; /* successfully finished */
    return(VSVC_NO_ERROR);
}

/*
 * vst_cold_dump_send_call
 *
 * Request a complete dump of all network groups.  The serverid passed in
 * is used to determine which host this request will be sent to.  The
 * group_name string determines where in its tables the server hould
 * begin.  This is used to allow multi-pass calls to this routine.
 */
void vst_cold_dump_send_call (ulong serverid, char *group_name)
{
    vst_cold_dump_query *query;
    vinesipcsocket *socket;

    if (!serverid)
	return;
    socket = vipc_create_conn(serverid, VINES_SERVER, 0, 0, TRUE);
    if (!socket)
	return;
    query = malloc(sizeof(vst_cold_dump_query));
    if (!query)
	return;

    if (group_name)
	sstrncpy(query->pass, group_name, VST_TOTALLEN);
    vst_cold_dump_send_call_(socket, query);
    free(query);
}


/**************************************************
 *
 *	      ST - New Income (35)
 *
 **************************************************/

/****************************************
 *
 *	  ST - New Income - Summary
 *
 ****************************************/

/*
 * vst_rcv_ni_summary
 *
 * Process a received summary message.  This message is used as a
 * periodic keepalive message for streettalk.  If the router is about to
 * send its own streettalk message, then the forwarding of the received
 * message should be piggybacked onto the existing outgoing message.
 *
 * If the received message is a welcome message (same format as a
 * summary), it is used to list all known streettalk instances.  If there
 * are more than 40 servers, then multiple messages should be used.  A
 * welcome message is never forwarded.
 *
 * This routine returns true if the caller should forward the original
 * message.
 */
static boolean vst_rcv_ni_summary (
    vinesipcsocket *socket,
    vst_ni_summary *incoming,
    boolean is_summary)
{
    ushort i;
    vst_server *entry;
    serverentry* s;
    char *tag = NULL;
    boolean propagate;
    ulong serverid;
    enum vst_freshness freshness;
    
    /*
     * Process any information present in the message.  This will be a list of
     * servers that are presnt on the network, and a partial list of their
     * Streettalk IDs.  This will tell the router who is present, and whether
     * its current information is out of date.
     */
    propagate = FALSE;
    for (i = 0; i < incoming->server_count; i++) {
	entry = &incoming->server[i];
	serverid = vines_extract_serverid(&entry->port);
	if ((s = vsvc_FindServer(serverid)) != NULL) {
	    freshness = vst_compare_ids(s, entry);
	    tag = vst_freshness_tag(freshness);
	    if (vst_new_info(freshness)) {
		switch (freshness) {
		  case VST_CHANGED:
		    vst_set_gen_dest(&vst_dest_need, serverid);
		    bcopy(&entry->port, &s->st_port, sizeof(vinesaddress));
		    s->st_need_info    = TRUE;
		    s->st_database_id  = entry->database_id;
		    s->st_change_id    = entry->change_id;
		    /* Fall through */

		  case VST_PERIODIC:
		    s->st_reinforce_id = entry->reinforce_id;
		    if (is_summary || (s->serverid == socket->remote_net))
			TIMER_START(s->st_last_update, VST_EXPIRATION);
		    propagate = TRUE;
		    break;

		  default:
		    break;
		}
	    }
	} else if ((s = vsvc_CreateServer(serverid)) != NULL) {
	    /*
	     * Record this all new information, and set the timer to ask
	     * for a detail message to get the group information.
	     */
	    bcopy(&entry->port, &s->st_port, sizeof(vinesaddress));
	    s->st_info_valid   = TRUE;
	    s->st_database_id  = entry->database_id;
	    s->st_change_id    = entry->change_id;
	    s->st_reinforce_id = entry->reinforce_id;
	    s->st_need_info    = TRUE;
	    TIMER_START(s->st_last_update, VST_EXPIRATION);
	    vst_set_gen_dest(&vst_dest_need, serverid);
	    propagate = TRUE;
	    tag = "new entry";
	} else {
	    tag = "no memory";
	}
	
	if (vst_debug)
	    buginf("\nVST:   %#16Z DI %08x, CI %08x, RI %08x, H %04x, %s",
		   serverid, entry->database_id, entry->change_id,
		   entry->reinforce_id, entry->hop_count, tag);

	entry->hop_count++;
	if (entry->hop_count > VST_MAX_HOPS)
	    propagate = FALSE;
    }

    /*
     * Tell the caller whether or not this message needs to be propagated
     * to other servers.
     */
    return(propagate);
}

/*
 * vst_send_ni_summary
 *
 * Generate a summary message.  This is the keepalive mechanism used by
 * Streettalk.  It also includes all the local ID values so that the receivers
 * can detect an out-of-sync condition.
 */
void vst_send_ni_summary (void)
{
    vst_ni_query *ni_msg;
    vst_ni_summary *summary;
    vst_server *server;
    serverentry *self;

    /*
     * Basic Setup
     */
    if (!(self = vsvc_FindSelf(vst_debug, "ST")))
	return;
    ni_msg = malloc(sizeof(vst_ni_query));
    if (!ni_msg)
	return;
    vst_bump_reinforce(self);
    TIMER_START(self->st_last_update, VST_EXPIRATION);
    
    /*
     * Copy in all the information
     */
    ni_msg->data_type = VST_NI_SUMMARY;
    summary = &ni_msg->data.summary;
    summary->server_count = 1;

    server = &summary->server[0];
    bcopy(&self->st_port, &server->port, sizeof(vinesaddress));
    server->database_id  = self->st_database_id;
    server->change_id    = self->st_change_id;
    server->reinforce_id = self->st_reinforce_id;
    server->hop_count    = 0;

    vst_ni_send_call_(NULL, ni_msg);
    free(ni_msg);
}

/****************************************
 *
 *	  ST - New Income - Detail
 *
 ****************************************/

/*
 * vst_detail_extract_group
 *
 * Given a pointer into a detail message, extract the information for a single
 * group and update the local data structures.
 */
static void vst_detail_extract_group (
    ulong serverid,
    ulong change_id,
    vst_group *entry)
{
    vst_groupentry *g;
    ushort priority;

    if (vst_debug) {
	buginf("\nVST:     %15s %15s  N %08x, D %04x, M %04x",
	       entry->domain, entry->org, entry->netgroup_id,
	       entry->domain_seq, entry->misc_info); 
    }

    /*
     * Verify that this isn't a locally owned group.
     */
    if (strcmp(entry->domain, vsvc_hostname()) == 0) {
	if ((strcmp(entry->org, vst_1st_orgname) == 0) ||
	    (strcmp(entry->org, vst_2nd_orgname) == 0)) {
	    if (vst_debug)
		buginf(", ignored");
	    return;
	}
    }
	
    g = vst_FindGroup(entry->domain, entry->org, serverid);
    if (g == NULL) {
	priority = vst_misc2priority(entry->misc_info);
	g = vst_CreateGroup(entry->domain, entry->org, priority);
	if (g == NULL)
	    return;
    } else if (g->localgroup_id) {
	return;
    }

    g->serverid      = serverid;
    g->change_id     = change_id;
    g->netgroup_id   = entry->netgroup_id;
    g->localgroup_id = 0;
    g->domain_seq    = entry->domain_seq;
    TIMER_START(g->expires, VST_EXPIRATION);
}

/*
 * vst_rcv_ni_detail
 *
 * A detail message lists all the servers that are maintained on a
 * particular server.  This server is not necessarily the same server as
 * the one that sent the packet, as these messages are cascaded from
 * server to server.
 */
static boolean vst_rcv_ni_detail (
    vinesipcsocket *socket,
    vst_ni_detail *detail)
{
    ulong serverid, checksum;
    enum vst_freshness freshness;
    char *tag;
    serverentry* s;
    ushort i;

    /*
     * Determine the originator and the validity of this message.
     */
    checksum = 0;
    for (i = 0; i < detail->group_count; i++) {
	checksum *= 2;
	checksum ^= detail->group[i].netgroup_id;
    }
    serverid = vines_extract_serverid(&detail->source.port);
    if (!(s = vsvc_FindServer(serverid))) {
	s = vsvc_CreateServer(serverid);
    }
    freshness = vst_compare_ids(s, &detail->source);

    /*
     * A little debugging never hurt anyone.
     */
    if (vst_debug) {
	if (checksum != detail->checksum) {
	    tag = "invalid checksum";
	} else if (s != NULL) {
	    tag = vst_freshness_tag(freshness);
	} else {
	    tag = "no memory";
	}
	buginf("\nVST:   Groups maintained on %#Z (%s)", serverid, tag);
	buginf("\nVST:     IDs: DI %08x, CI %08x, RI %08x, H %04x, CK %08x",
	       detail->source.database_id, detail->source.change_id,
	       detail->source.reinforce_id, detail->source.hop_count,
	       detail->checksum);
    }
    if ((s == NULL) || (checksum != detail->checksum))
	return(FALSE);
    if (!vst_new_info(freshness))
	return(FALSE);

    /*
     * Copy all the information on the sending servers ID numbers
     */
    bcopy(&detail->source.port, &s->st_port, sizeof(vinesaddress));
    s->st_database_id  = detail->source.database_id;
    s->st_change_id    = detail->source.change_id;
    s->st_reinforce_id = detail->source.reinforce_id;
    s->st_info_valid   = TRUE;
    s->st_need_info    = FALSE;
    TIMER_START(s->st_last_update, VST_EXPIRATION);

    /*
     * Process any group information in the frame.
     */
    for (i = 0; i < detail->group_count; i++) {
	vst_detail_extract_group(serverid, s->st_change_id, &detail->group[i]);
    }

    /*
     * Clear any existing group names that were not in the last detail
     * message.  This can be safely done here because the information for
     * all groups existing on a single server will fit into one detail
     * message.
     */
    vst_WalkGroupTree(vst_flush_old_group, s);

    /*
     * Propagate the frame
     */
    return(++detail->source.hop_count <= VST_MAX_HOPS);
}

/*
 * vst_detail_insert_group
 *
 * Look at each node in the group tree to see if it is a locally
 * maintained group.  If so, insert the group information into the
 * outgoing detail message.
 */
static boolean vst_detail_insert_group (treeLink *x, void *data)
{
    vst_ni_detail *detail;
    vst_group *entry;
    vst_groupentry *g;
    vst_aid *aid;
    ushort misc_info;

    g = vst_GroupCast(x);
    aid = (vst_aid *)data;
    if (g->serverid != aid->server)
	return(TRUE);

    detail = aid->data;
    entry = &detail->group[detail->group_count++];

    sstrncpy(entry->org,    g->org,    VST_ORGLEN);
    sstrncpy(entry->domain, g->domain, VST_DOMAINLEN);
    misc_info = vst_priority2misc(g->priority);
    entry->misc_info = misc_info;
    entry->domain_seq = g->domain_seq;
    entry->netgroup_id = g->netgroup_id;

    detail->checksum *= 2;
    detail->checksum ^= g->netgroup_id;
    if (vst_debug)
	buginf("\nVST:     %15s %15s  N %08x, D %04x, M %04x", g->domain,
	       g->org, g->netgroup_id, g->domain_seq, misc_info);
    return(TRUE);
}

/*
 * vst_send_ni_detail
 *
 * This currently sends broadcast messages.  An enhancement would be to
 * send unicast messages back to the source.  The socket is available, so
 * this could be easily implemented.
 */
void vst_send_ni_detail (void)
{
    vst_ni_detail *detail;
    vst_ni_query *ni_msg;
    serverentry* self;
    vst_aid params;

    /*
     * General setup
     */
    if (!(self = vsvc_FindSelf(vst_debug, "ST")))
	return;
    ni_msg = malloc(sizeof(vst_ni_query));
    if (!ni_msg)
	return;
    vst_bump_reinforce(self);
    if (vst_dest_detail.broadcast)
	TIMER_START(self->st_last_update, VST_EXPIRATION);
    
    /*
     * Record the Streettalk ID information for this server
     */
    ni_msg->data_type = VST_NI_DETAIL;

    detail = &ni_msg->data.detail;
    bcopy(&self->st_port, &detail->source.port, sizeof(vinesaddress));
    detail->source.database_id  = self->st_database_id;
    detail->source.change_id    = self->st_change_id;
    detail->source.reinforce_id = self->st_reinforce_id;
    detail->source.hop_count    = 0;
    detail->checksum            = 0;
    detail->group_count         = 0;
    if (vst_debug) {
	buginf("\nVST: Sending detail from %#Z, DI %08x, CI %08x, RI %08x",
	       vines_network, self->st_database_id, self->st_change_id, 
	       self->st_reinforce_id);
    }

    /*
     * Record all the domain@org information on this server.
     */
    params.data   = &ni_msg->data.detail;
    params.server = vines_network;
    vst_WalkGroupTree(vst_detail_insert_group, &params);
    vst_ni_send_call_(NULL, ni_msg);
    free(ni_msg);
}

/****************************************
 *
 *	  ST - New Income - Need
 *
 ****************************************/

/*
 * vst_rcv_ni_need
 *
 * A need message is a request for a list of all the servers that are
 * maintained on a particular server.  The server desiring the response is not
 * necessarily the same server as the one that sent the packet, as these
 * messages are cascaded from server to server.
 */
static boolean vst_rcv_ni_need (
    vinesipcsocket *socket,
    vst_ni_need *need)
{
    ushort i, local_router;
    ulong serverid, sourceid;
    serverentry* s;
    boolean first, newentry;

    /*
     * Debug
     */
    sourceid = vines_extract_serverid(&need->origin);
    if (vst_debug) {
	buginf("\nVST:   %#Z is requesting information", sourceid);
	buginf("\nVST:     IDs: R %08x, H %04x", need->reinforce_id,
	       need->hop_count);
    }

    /*
     * Does this router already know about the sender of this need
     * message?  If not, glean what little information is available from
     * this message, and send a need back asking for the rest of that
     * servers information.
     */
    s = vsvc_FindServer(sourceid);
    if (s == NULL) {
	s = vsvc_CreateServer(sourceid);
	if (!s) {
	    if (vst_debug)
		buginf(" (no memory)");
	    return(TRUE);
	}
	bcopy(&need->origin, &s->st_port, sizeof(vinesaddress));
	s->st_reinforce_id = need->reinforce_id;
	s->st_info_valid   = TRUE;
	s->st_need_info    = TRUE;
	TIMER_START(s->st_last_update, VST_EXPIRATION);
	vst_set_gen_dest(&vst_dest_need, sourceid);
	newentry = TRUE;
    } else if (vst_is_new_reinforce(s->st_reinforce_id, need->reinforce_id)){
	s->st_reinforce_id = need->reinforce_id;
	newentry = FALSE;
    } else {
	if (vst_debug)
	    buginf(" (duplicate)");
	return(FALSE);
    }

    /*
     * Check to see if this message is requesting locally maintained
     * information.  Also print some debugging on what's being requested.
     */
    local_router = MAXUSHORT;
    first = TRUE;
    for (i = 0; i < need->wanted_count; i++) {
	serverid = vines_extract_serverid(&need->wanted[i]);
	if (serverid == vines_network)
	    local_router = i;
	if (vst_debug) {
	    if ((i % 4) == 0)
		buginf("\nVST:   %s", first ? "want" : "    ");
	    buginf("  %c%#15Z", (i == local_router) ? '*' : ' ', serverid);
	    first = FALSE;
	}
    }

    /*
     * Respond to a request for locally maintained information.
     */
    if (local_router != MAXUSHORT)
	vst_set_gen_dest(&vst_dest_detail, sourceid);

    return(++need->hop_count <= VST_MAX_HOPS);
}

/*
 * vst_need_insert_server
 *
 * Insert a servers address into the list of addresses for which this server
 * needs information.  If the current message limit is reached, send the need
 * and start on a new message.
 */
static boolean vst_need_insert_server (treeLink *x, void *data)
{
    vst_ni_query *ni_msg;
    vst_ni_need *need;
    vst_aid *params;
    serverentry *s;
    vinesipcsocket *socket;

    s = (serverentry *)x;
    if (!s->st_need_info)
	return(TRUE);
    params = data;
    socket = params->socket;
    ni_msg = params->data;
    need = &ni_msg->data.need;

    /*
     * If the data structures are full, then we must send this "packet's
     * worth" of data and then continue.  Having this here instead of at
     * the end of the function means that there should always be
     * something for the parent routine to send.
     */
    if (need->wanted_count == VST_NEED_COUNT) {
	vst_ni_send_call_(socket, ni_msg);
	need->wanted_count = 0;
	params->sent = TRUE;
    }

    bcopy(&s->st_port, &need->wanted[need->wanted_count++], sizeof(vinesaddress));
    s->st_need_info = FALSE;
    if (vst_debug)
	buginf("\nVST:     want %#Z", s->serverid);
    vst_set_gen_dest(&vst_dest_need, s->serverid);
    return(TRUE);
}

/*
 * vst_send_ni_need
 *
 * Build and send a "Need" message.  This routine uses the above message
 * constructor to do most of the dirty work.
 */
void vst_send_ni_need (void)
{
    vst_ni_query *ni_msg;
    vst_ni_need *need;
    serverentry *self;
    vst_aid params;

    if (!(self = vsvc_FindSelf(vst_debug, "ST")))
	return;
    ni_msg = malloc(sizeof(vst_ni_query));
    if (!ni_msg)
	return;
    ni_msg->data_type = VST_NI_NEED;

    vst_bump_reinforce(self);
    if (vst_debug) {
	buginf("\nVST: Generating new need message from %#Z, R %08x",
	       vines_network, self->st_reinforce_id);
    }
	
    need = &ni_msg->data.need;
    bcopy(&self->st_port, &need->origin, sizeof(vinesaddress));
    need->reinforce_id = self->st_reinforce_id;
    need->hop_count = 0;
    need->wanted_count = 0;

    params.socket = NULL;
    params.data   = (uchar *)ni_msg;
    params.sent   = FALSE;
    vsvc_WalkServerTree(vst_need_insert_server, &params);
    if (need->wanted_count != 0) {
	vst_ni_send_call_(NULL, ni_msg);
    } else if (vst_debug && (params.sent == FALSE)) {
	buginf(" (squelched)");
    }
		
    free(ni_msg);
}

/****************************************
 *
 *	  ST - New Income - Welcome
 *
 ****************************************/

/*
 * vst_rcv_ni_welcome
 *
 * See vst_rcv_ni_summary().
 */

/*
 * vst_welcome_insert_server
 *
 * Insert a servers address into the list of all known streettalk addresses.
 */
static boolean vst_welcome_insert_server (treeLink *x, void *data)
{
    vst_ni_query *ni_msg;
    vst_ni_welcome *welcome;
    vst_server *entry;
    vst_aid *params;
    serverentry *s;
    vinesipcsocket *socket;

    s = (serverentry *)x;
    if (!s->st_info_valid)
	return(TRUE);
    params = data;
    socket = params->socket;
    ni_msg = params->data;
    welcome = &ni_msg->data.welcome;

    /*
     * If the data structures are full, then we must send this "packet's
     * worth" of data and then continue.  Having this here instead of at
     * the end of the function means that there should always be
     * something for the parent routine to send.
     */
    if (welcome->server_count == VST_WELCOME_COUNT) {
	vst_ni_send_call_(socket, ni_msg);
	welcome->server_count = 0;
    }

    entry = &welcome->server[welcome->server_count++];
    bcopy(&s->st_port, &entry->port, sizeof(vinesaddress));
    entry->hop_count = 0;
    entry->reinforce_id = s->st_reinforce_id;
    entry->change_id = s->st_change_id;
    entry->database_id = s->st_database_id;
    if (vst_debug)
	buginf("\nVST:   %#16Z DI %08x, CI %08x, RI %08x, H %04x",
	       s->serverid, entry->database_id, entry->change_id,
	       entry->reinforce_id, entry->hop_count);
    return(TRUE);
}

/*
 * vst_send_ni_welcome
 *
 * Build and send a welcome message.  This message contains only the local ID
 * numbers, so that recipients can see that the router is still alive, and
 * they can see if their database is somehow out of sync with the local
 * database.
 */
void vst_send_ni_welcome (void)
{
    vst_ni_query *ni_msg;
    vst_ni_welcome *welcome;
    serverentry *self;
    vst_aid params;

    if (!(self = vsvc_FindSelf(vst_debug, "ST")))
	return;
    if (vst_debug)
	buginf("\nVST: Sending welcome message(s).");

    ni_msg = malloc(sizeof(vst_ni_query));
    if (!ni_msg)
	return;
    ni_msg->data_type = VST_NI_WELCOME;

    welcome = &ni_msg->data.welcome;
    welcome->server_count = 0;

    params.socket = NULL;
    params.data   = (uchar *)ni_msg;
    vsvc_WalkServerTree(vst_welcome_insert_server, &params);
    if (welcome->server_count != 0) {
	vst_ni_send_call_(NULL, ni_msg);
    }
    free(ni_msg);
}

/****************************************
 *
 *	     ST - New Income
 *
 ****************************************/

/*
 * vst_rcv_new_income
 *
 * The "New Income" dispatch routine.  This routine performs common work for
 * all four types of "New Income" messages, and the calls more specific
 * routines for each.  (Psst.  There was once an "Income" message, but it was
 * obsoleted by this message.)
 */
ushort vst_ni_rcv_call (
    vinesipcsocket *socket,
    vst_ni_query *query,
    vst_ni_reply *reply)
{
    boolean propagate;

    if (vst_debug) {
	if (query->data_type <= VST_NI_MAX) {
	    buginf(", type %s", vst_ni_names[query->data_type]);
	} else {
	    char number[20];
	    sprintf(number, "%d", query->data_type);
	    buginf(", type %s", number);
	}
    }

    switch (query->data_type) {
      case VST_NI_SUMMARY:
	propagate = vst_rcv_ni_summary(socket, &query->data.summary, TRUE);
	break;
      case VST_NI_DETAIL:
	propagate = vst_rcv_ni_detail(socket, &query->data.detail);
	break;
      case VST_NI_NEED:
	propagate = vst_rcv_ni_need(socket, &query->data.need);
	break;
      case VST_NI_WELCOME:
	vst_rcv_ni_summary(socket, &query->data.summary, FALSE);
	propagate = FALSE;
	break;
      default:
	propagate = FALSE;
	break;
    }

    if (propagate && socket->last_was_broadcast) {
	vst_propagate(socket, query);
    }

    return(VSVC_NO_ERROR);
}


/**************************************************
 *
 *	     ST - Return Details (36)
 *
 **************************************************/

/*
 * vst_rtndetail_rcv_call
 *
 * A Return Detail message lists all the information that is known about
 * a particular server.  This server providing this information is not
 * necessarily the same server that the information is describing.  The
 * sender is the server that was sent a query for this information.
 *
 * This routine uses the same group extraction routine as the `New
 * Income' Detail message.
 *
 * Note: This packet has a demultiplexor field that never seems to be
 * used.  If it is anything other than the value two (2), the unpacking
 * routine for this message will pitch it.  If anything other than the
 * value two (2) is valid, the data structure needs to be redefined, and
 * this code will need a switch statement added to it.
 */
ushort vst_rtndetail_rcv_call (
    vinesipcsocket *socket,
    vst_rtndetail_query *query,
    vst_rtndetail_reply *reply)
{
    vst_rtndetail_detail *detail = &query->data.detail;
    enum vst_freshness freshness;
    ulong serverid, checksum;
    char *tag;
    serverentry* s;
    ushort i;

    /*
     * Determine the originator and the validity of this message.
     */
    checksum = 0;
    for (i = 0; i < detail->group_count; i++) {
	checksum *= 2;
	checksum ^= detail->group[i].netgroup_id;
    }
    serverid = vines_extract_serverid(&detail->source.port);
    if (!(s = vsvc_FindServer(serverid))) {
	s = vsvc_CreateServer(serverid);
    }
    freshness = vst_compare_ids(s, &detail->source);

    /*
     * A little debugging never hurt anyone.
     */
    if (vst_debug) {
	if (checksum != detail->checksum) {
	    tag = "invalid checksum";
	} else if (s != NULL) {
	    tag = vst_freshness_tag(freshness);
	} else {
	    tag = "no memory";
	}
	buginf("\nVST:   Groups maintained on %#Z (%s)", serverid, tag);
	buginf("\nVST:     IDs: DI %08x, CI %08x, RI %08x, H %04x, CK %08x",
	       detail->source.database_id, detail->source.change_id,
	       detail->source.reinforce_id, detail->source.hop_count,
	       detail->checksum);
    }
    if ((s == NULL) || (checksum != detail->checksum))
	return(VSVC_NO_ERROR);
    if (!vst_new_info(freshness))
	return(VSVC_NO_ERROR);

    /*
     * Copy all the information on the sending servers ID numbers
     */
    bcopy(&detail->source.port, &s->st_port, sizeof(vinesaddress));
    s->st_database_id  = detail->source.database_id;
    s->st_change_id    = detail->source.change_id;
    s->st_reinforce_id = detail->source.reinforce_id;
    s->st_need_info    = FALSE;
    s->st_info_valid   = TRUE;
    TIMER_START(s->st_last_update, VST_EXPIRATION);

    /*
     * Process any group information in the frame.
     */
    for (i = 0; i < detail->group_count; i++) {
	vst_detail_extract_group(serverid, s->st_change_id, &detail->group[i]);
    }

    /*
     * Clear any existing group names that were not in the last detail
     * message.  This can be safely done here because the information for
     * all groups existing on a single server will fit into one detail
     * message.
     */
    vst_WalkGroupTree(vst_flush_old_group, s);

    /*
     * Propagate the frame
     */
    return(VSVC_NO_ERROR);
}

/*
 * vst_rtndetail_rcv_reply
 *
 * Receive a reply to a Return Details Message.  (Hah!)  There is no data
 * contained in a reply, so this routine is a no-op.  The only reason it is here
 * is to prevent the code from printing a debugging message that it is being
 * ignored.  Better to prevent customer questions than to answer them.
 */
void vst_rtndetail_rcv_reply (
    vinesipcsocket *socket,
    vst_rtndetail_reply *reply)
{
    /*
     * Doesn't do anything now, and never will!!!
     */
}

/*
 * vst_rtndetail_send_call
 *
 * This currently sends broadcast messages.  An enhancement would be to
 * send unicast messages back to the source.  The socket is available, so
 * this could be easily implemented.
 */
static void vst_rtndetail_send_call (vinesipcsocket *socket, ulong info_about)
{
    vst_rtndetail_detail *detail;
    vst_rtndetail_query *query;
    serverentry* server;
    vst_aid params;

    if (socket == NULL)
	return;

    server = vsvc_FindServer(info_about);
    if (server == NULL)
	return;

    /*
     * Get a packet
     */
    query = malloc(sizeof(vst_rtndetail_query));
    if (query == NULL)
	return;
    
    /*
     * Record the Streettalk ID information for this server.
     */
    query->data_type = VST_NI_DETAIL;

    detail = &query->data.detail;
    bcopy(&server->st_port, &detail->source.port, sizeof(vinesaddress));
    detail->source.hop_count    = 0;
    detail->source.database_id  = server->st_database_id;
    detail->source.change_id    = server->st_change_id;
    detail->source.reinforce_id = server->st_reinforce_id;
    detail->checksum            = 0;
    detail->group_count         = 0;
    if (vst_debug) {
	buginf("\nVST: Sending rtndetail for %#Z, DI %08x, CI %08x, RI %08x",
	       info_about, server->st_database_id, server->st_change_id, 
	       server->st_reinforce_id);
    }

    /*
     * Record all the domain@org information on this server.
     */
    params.data   = &query->data.detail;
    params.server = info_about;
    vst_WalkGroupTree(vst_detail_insert_group, &params);
    vst_rtndetail_send_call_(socket, query);
    free(query);
}


/**************************************************
 *
 *	     ST - Get Details (37)
 *
 **************************************************/

/*
 * vst_getdetail_rcv_call
 *
 * The router received a a GetDetail message from another server.  This
 * message is identical to the "New Income" Need message, but the response is
 * sent as a Return Details message.  I don't know why Banyan didn't just make
 * this a query/response transaction.
 */
ushort vst_getdetail_rcv_call (
    vinesipcsocket *socket,
    vst_getdetail_query *query,
    vst_getdetail_reply *reply)
{
    ulong info_about;
    boolean first;
    ushort i;

    /*
     * Walk through the list of information being requested and print some
     * debugging information.  We'll send everything on a second pass
     * of the list.  This makes the debugging easy to read.
     */
    if (vst_debug) {
	buginf("\nVST:   %#Z is requesting information", socket->remote_net);
	first = TRUE;
	for (i = 0; i < query->wanted_count; i++) {
	    info_about = vines_extract_serverid(&query->wanted[i]);
	    if (vst_debug) {
		if ((i % 4) == 0)
		    buginf("\nVST:   %s", first ? "want" : "    ");
		buginf("  %#15Z", info_about);
		first = FALSE;
	    }
	}
    }

    /*
     * Second walk through the list.  Send responses.
     */
    for (i = 0; i < query->wanted_count; i++) {
	info_about = vines_extract_serverid(&query->wanted[i]);
	vst_rtndetail_send_call(socket, info_about);
    }
    return(VSVC_NO_ERROR);
}


/**************************************************
 *
 *	       ST - Operate (16)
 *
 * This routine needs to come last because it calls
 * back to many of the other routines.  Either that,
 * or we will need a bunch of forward declarations.
 *
 **************************************************/

/*
 * vst_set_operation_timer
 *
 * Set the timer on a streettalk 'operation'
 */
static inline void vst_set_operation_timer (vst_operation *operation)
{
    TIMER_START(operation->timer, VST_OPERATION_TIME);
    vst_set_master_timer(operation->timer);
}

/*
 * vst_op_do_lookup
 *
 * Lookup the service name requested, and return some information about it.
 */
static ushort vst_op_do_lookup (
    vst_operate_query *query,
    vst_operate_lookup *qlookup,
    vst_operate_lookup *rlookup,
    char *fullname)
{
    vsvc_info *service;
    vst_st_record *record;
    vst_rec_svc *svc_data;
    vst_rec_assocrec *arec_data;
    vinesaddress address;

    for (service = vsvc_queue.qhead; service; service = service->next) {
	if (strcasecmp(service->svc_name, fullname) == 0)
	    break;
    }
    if (service == NULL)
	return(VST_ERR_NOSUCHOBJ);
    record = &rlookup->st_info;
    switch (query->assoc_number) {
      case VST_AREC_NONE:
	record->class_type = VST_REC_SERVICE;
	svc_data = &record->class.service.service;
	svc_data->category = VST_CAT_SS;
	sstrncpy(svc_data->description, service->description,
		 VST_DESCRIPTION);
	vsvc_create_address(&address, service->well_known_port);
	bcopy(&address, &svc_data->port, sizeof(vinesaddress));
	sstrncpy(svc_data->ssname, vsvc_hostname(), VST_DOMAINLEN);
	return(VSVC_NO_ERROR);
      case VST_AREC_PORTS:
	record->class_type = VST_REC_ASSOCREC;
	arec_data = &record->class.assocrec.assocrec;
	arec_data->data_count = 2 + sizeof(vinesaddress);
	*(ushort *)&arec_data->data = 1;	/* num ports */
	/* DRH - Finish me - If you need to */
	break;
      default:
	record->class_type = VST_REC_ASSOCREC;
	arec_data = &record->class.assocrec.assocrec;
	arec_data->data_count = 2;
	*(ushort *)&arec_data->data = 0;	/* num ports */
	break;
    }
    return(VST_ERR_UNKNOWN);
}

/*
 * vst_op_do_enumerate
 *
 * No, there are no items to be enumerated.  Leave all the information
 * as it was when we were called.  This allows multiple server
 * enumerations to succeed when the router is one of the middle servers
 * as they are each called in turn to enumerate.
 */
static ushort vst_op_do_enumerate (
    vst_operate_query *query,
    vst_operate_lookup *qlookup,
    vst_operate_lookup *rlookup,
    char *fullname)
{
    return(VSVC_NO_ERROR);
}

/*
 * vst_op_do_membership
 *
 * No, the name is not a member of a list, on a list, etc.  This is here so
 * that any queries about a users membership in the list
 * AdminList@<Router>@Servers will return false.  This will prevent anyone
 * from every trying to set or change something on the router.
 */
static ushort vst_op_do_membership (
    vst_operate_query *query,
    vst_operate_lookup *qlookup,
    vst_operate_lookup *rlookup,
    char *fullname)
{
    rlookup->cardinal[2] = 2;
    return(VSVC_NO_ERROR);
}

/*
 * vst_op_do_nicename
 *
 * ???
 */
static ushort vst_op_do_nicename (
    vst_operate_query *query,
    vst_operate_lookup *qlookup,
    vst_operate_lookup *rlookup,
    char *full_name)
{
    rlookup->pointer_count = 1;
    strcpy(rlookup->pointer[0].item, full_name);
    return(VSVC_NO_ERROR);
}

/*
 * vst_operate_rcv_call
 *
 * The router received an "Operate" query.  Find the requested information and
 * return it.  This is *the* workhorse call for Streettalk.  Any deflected
 * query windes up as an operate call between two servers.
 */
ushort vst_operate_rcv_call (
    vinesipcsocket *socket,
    vst_operate_query *query,
    vst_operate_reply *reply)
{
    vst_operate_lookup *qlookup, *rlookup;
    char full_name[VST_TOTALLEN];

    /*
     * Start by copying the lookup data structure from query to response.
     * The response version will then be modified to include the
     * information requested in the query.
     */
    qlookup = &query->lookup_info;
    rlookup = &reply->lookup_info;
    bcopy(qlookup, rlookup, sizeof(vst_operate_lookup));

    /*
     * Print debugging
     */
    vsvc_make_name(full_name, qlookup->item_name,
		       qlookup->domain_name, qlookup->org_name);
    if (vst_debug) {
	buginf("\nVST: operation %d requested by %s.",
	       query->procedure_id,
	       query->user_name ? query->user_name : "<no name>");
	buginf("\nVST:   class %d, name %s.", query->item_class, full_name);
    }

    /*
     * Gaaahhh!!!
     */
    switch (query->procedure_id) {
      case VST_OP_LOOKUP:
	return(vst_op_do_lookup(query, qlookup, rlookup, full_name));
      case VST_OP_ENUMERATE:
	return(vst_op_do_enumerate(query, qlookup, rlookup, full_name));
      case VST_OP_MEMBERSHIP:
	return(vst_op_do_membership(query, qlookup, rlookup, full_name));
      case VST_OP_NICENAME:
	return(vst_op_do_nicename(query, qlookup, rlookup, full_name));
      default:
	return(VSVC_ERR_INVALID);
    }

    return(VSVC_NO_ERROR);
}

/*
 * vst_operate_send_call
 *
 * Send an "Operate" query to another server.  This is *the* workhorse call
 * for Streettalk.  Any deflected query windes up as an operate call between
 * two servers.
 */
static ushort vst_operate_send_call (
    vst_handle *handle,
    char *item_name,
    char *domain_name,
    char *org_name,
    ushort class,
    ushort caller)
{
    vst_operate_lookup *lookup;
    vst_operate_query *query;
    vst_operation *record;
    vinesipcsocket *socket;
    vst_groupentry *primary, *group;
    dqueue_t *ptr;

    /*
     * Debugging.  What else.
     */
    if (vst_debug & VERBOSE_DEBUG)
	buginf("\nVST: Operate called with handle %08x, name %s:%s:%s, class %d",
	       handle->session_handle, item_name, domain_name, org_name, class);

    /*
     * Now find a reachable server for this group.  Walk the
     * groupname_thread since it is sorted in order of precedence
     * specified in the detail messages.
     */
    group = NULL;
    primary = vst_FindGroup(domain_name, org_name, VINES_UNKNOWNNET);
    RUN_DQUEUE_SELF(primary->groupname_thread, ptr) {
	group = ptr->parent;
	if (group->serverid == vines_network) {
	    /*
	     * This should probably do some real work, but at this point
	     * I don't think it this routine will ever be called now that
	     * search requests are proxied.  There's not much to do
	     * except check for AdminList@<RouterName>@Servers.  DRH
	     * 9/1/94
	     */
	    return(VST_ERR_NOSUCHOBJ);
	}
	if (vines_first_hop_test(group->serverid, VINES_SERVER)) {
	    /*
	     * This server is reachable.  Use it.
	     */
	    break;
	}
	group = NULL;
    }
    if (group == NULL)
	return(VST_ERR_NOSUCHOBJ);

    socket = vipc_create_conn(group->serverid, VINES_SERVER, 0, 0, TRUE);
    if (!socket)
	return(VSVC_ERR_NO_MEMORY);
    query = malloc(sizeof(vst_operate_query));
    if (!query)
	return(VSVC_ERR_NO_MEMORY);
    record = malloc(sizeof(vst_operation));
    if (!record) {
	free(query);
	return(VSVC_ERR_NO_MEMORY);
    }

    vst_set_operation_timer(record);
    record->handle = handle->session_handle;
    record->server = group->serverid;
    record->type = VST_OP_LOOKUP;
    record->caller = caller;
    vsvc_make_name(record->name, item_name, domain_name, org_name);
    record->q.parent = record;
    lw_insert(&record->q, &vst_operationQ);

    query->procedure_id = VST_OP_LOOKUP;
    sstrncpy(query->user_name, handle->username, VST_TOTALLEN);
    query->item_class = class;
    query->assoc_number = 0;
    lookup = &query->lookup_info;
    sstrncpy(lookup->org_name, org_name, VST_ORGLEN);
    sstrncpy(lookup->domain_name, domain_name, VST_DOMAINLEN);
    sstrncpy(lookup->item_name, item_name, VST_ITEMLEN);
    lookup->pointer_count = 0;
    lookup->cardinal_count = 0;
    lookup->misc_count = 1;
    lookup->misc[0].data_type = 4;
    vsvc_create_nonce(&lookup->misc[0].data.nonce.user_nonce);
    lookup->chaining_count = 0;
    vst_operate_send_call_(socket, query);
    free(query);
    return(VSVC_NO_ERROR);
}

/*
 * vst_operate_done
 *
 * This is the common routine for handling good and error returns from an
 * operate call.  Take the returned information, and feed it back to the
 * streettalk calls that caused the deflected operation to be performed ni the
 * first place.
 */
static void vst_operate_done (
    vinesipcsocket *socket,
    vst_operate_reply *reply,
    ushort reason)
{
    vst_operate_lookup *lookup;
    vst_operation *operation;
    vst_st_record *infoptr;
    dqueue_t *ptr;
    char name[VST_TOTALLEN], *nameptr;

    /*
     * Find the record of the last operate command
     */
    RUN_DQUEUE(vst_operationQ, ptr) {
	operation = ptr->parent;
	if (operation->server != socket->remote_net)
	    continue;
	lw_remove(ptr);
	break;
    }
    if (ptr == &vst_operationQ)
	return;

    /*
     * Finish off the client request.
     */
    operation = ptr->parent;
    if (reply) {
	lookup = &reply->lookup_info;
	vsvc_make_name(name, lookup->item_name, lookup->domain_name,
		       lookup->org_name);
	nameptr = name;
	infoptr = &lookup->st_info;
    } else {
	nameptr = NULL;
	infoptr = NULL;
    }
    switch (operation->caller) {
      case VST_LOOKUP:
	vst_lookup_finished(operation->handle, nameptr, infoptr, reason);
	break;
      case VST_GETPORTS:
	vst_getports_finished(operation->handle, nameptr, infoptr, reason);
	break;
      default:
	break;
    }
    free(operation);
}

/*
 * vst_operate_rcv_reply
 *
 * The good operate return wrapper.
 */
void vst_operate_rcv_reply (
    vinesipcsocket *socket,
    vst_operate_reply *reply)
{
    vst_operate_done(socket, reply, VSVC_NO_ERROR);
}

/*
 * vst_operate_rcv_abort
 *
 * The error operate return wrapper.
 */
void vst_operate_rcv_abort (
    vinesipcsocket *socket,
    ushort reason)
{
    vst_operate_done(socket, NULL, reason);
}

/**************************************************
 *
 *            ST - Search / ReturnAddress
 *
 **************************************************/

/*
 * vst_rcv_search
 *
 * A NetRPC search message has been received on the StreetTalk port.
 */
void vst_rcv_search (
    vinesipcsocket *socket,
    ushort user_num,
    uchar *user_string,
    uchar *info1,
    uchar *info2,
    uchar *info3)
{
    vsvc_send_returnaddress(socket, &vsvc_st_info);
}

/*
 * vst_send_search
 *
 * Well, send it!
 */
void vst_send_search (boolean inc_hop_count)
{
    vsvc_send_search(NULL, &vsvc_st_info, 0, NULL, NULL, NULL, NULL,
		     vst_search_diameter);
    if (inc_hop_count)
	vst_search_diameter++;
}

/*
 * vst_rcv_returnaddress
 *
 * A NetRPC returnaddress message has been received on the StreetTalk
 * port.  Call the appropriate action routine based upon the stare of
 * Streettalk.
 */
void vst_rcv_returnaddress (
    vinesipcsocket *socket,
    ushort proto_rev,
    ulong  service_rev,
    vinesaddress *service_address)
{
    /*
     * Call the appropriate ST routine
     */
    if (!vst_needs_cold_dump)
	return;

    if (service_rev >= VST_MIN_VERSION) {
	vst_cold_dump_send_call(socket->remote_net, NULL);
    }
}


/**************************************************
 *
 *	       ST - Miscellaneous
 *
 *  Routines for group/server creation/deletion.
 *
 **************************************************/

/*
 * vst_create_local_group
 *
 * Create and initialize a group that is maintined on this router.  The
 * server's network number is an argument only so that this routine may
 * be used by the VINES test code to generate arbitrary group names.
 */
void vst_create_local_group (
    char *domain_name,
    char *org_name,
    ulong serverid,
    ushort priority,
    boolean transmit)
{
    char group_name[VST_TOTALLEN]; /* needs to be overly long */
    serverentry *self;
    vst_groupentry *g;
    ulong group_id;
    short i;

    if (!(self = vsvc_FindSelf(vst_debug, "ST")))
	return;
    if (!(g = vst_FindGroup(domain_name, org_name, serverid)))
	g = vst_CreateGroup(domain_name, org_name, priority);
    memset(group_name, 0, VST_TOTALLEN);
    vst_make_group_name(group_name, domain_name, org_name);
    if (!g) {
	if (vst_debug)
	    buginf("\nVST: CLG cannot find or create %s.", group_name);
	return;
    }

    /*
     * Create a unique Group ID.  This needs to create the same ID
     * number each time the router is rebooted.  The algorithm was
     * made up, and has no relation to the algorithm Banyan uses.
     */ 
    group_id = 0;
    for (i = 0; i < strlen(group_name); i += 4) {
	group_id ^= *((ulong *)(group_name + i));
    }
    group_id = (((group_id * 47) + group_id) * 163) + group_id;
    g->netgroup_id = group_id ^ serverid;
    g->domain_seq  = VST_DSEQ_INIT_VALUE;
    g->serverid    = serverid;
    if (serverid == vines_network) {
	g->localgroup_id = vst_localgroup_id++;
    } else {
	g->localgroup_id = 0;
    }
    TIMER_START(g->expires, VST_EXPIRATION);
    self->st_change_id++;
    if (self->st_change_id == 0)
	self->st_change_id = 1;

    /*
     * Send a new detail?
     */
    if (transmit) {
	vst_set_gen_dest(&vst_dest_detail, -1L);
	vst_send_ni_detail();
    }
}

/*
 * vst_delete_local_group
 *
 * Delete a group that is maintined on this router.  The server's network
 * number is an argument only so that this routine may be used by the
 * VINES test code to generate arbitrary group names.
 */
void vst_delete_local_group (
    char *domain_name,
    char *org_name,
    ulong serverid,
    boolean transmit)
{
    serverentry *self;
    vst_groupentry *g;

    if (!(self = vsvc_FindSelf(vst_debug, "ST")))
	return;
    if (!(g = vst_FindGroup(domain_name, org_name, serverid)))
	return;

    vst_DeleteGroup(g);
    self->st_change_id++;
    if (self->st_change_id == 0)
	self->st_change_id = 1;

    /*
     * Send a new detail?
     */
    if (transmit) {
	vst_set_gen_dest(&vst_dest_detail, -1L);
	vst_send_ni_detail();
    }
}

/*
 * vst_delete_one_group
 *
 * This is a routine used by the tree walk code to delete all the
 * entries in the group table.
 */
static boolean vst_delete_one_group (treeLink *x, void *data)
{
    vst_groupentry *g;

    g = vst_GroupCast(x);
    vst_DeleteGroup(g);
    return(TRUE);
}

/*
 * vst_init_serverentry
 *
 * If at all possible, the Database ID should be the same from reboot to
 * reboot, but the Reinforce ID needs to be different from reboot to
 * reboot.  Build the database ID out of the server number and the server
 * name.  This should provide a consistent number that is unique across
 * the entire network..  Grab a random number and use it for the
 * Reinforce ID.
 */
static void vst_init_serverentry (void)
{
    vinesaddress *localport;
    serverentry* s;
    ulong db_id;
    char name[VST_DOMAINLEN];
    charlong reinforce_id;

    s = vsvc_FindSelf(vst_debug, "ST");
    if (s == NULL)
	return;

    /*
     * Create a unique Database ID.  This needs to create the same ID
     * number each time the router is rebooted.  The algorithm was
     * made up, and has no relation to the algorithm Banyan uses.
     */ 
    memset(name, 0, VST_DOMAINLEN);
    sstrncpy(name, vsvc_hostname(), VST_DOMAINLEN);
    db_id = *((ulong *)&name[0]) + *((ulong *)&name[4]) +
	    *((ulong *)&name[8]) + *((ulong *)&name[12]);
    db_id = (((db_id * 73) + db_id) * 199) + db_id;
    s->st_database_id  = db_id ^ vines_network;

    /*
     * The reinforce ID needs to have a random component.
     */
    process_sleep_for(0);
    reinforce_id.d.sword[0] = ((db_id >> 16) | (db_id & 0xFFFF)) + random_gen();
    reinforce_id.d.sword[1] = 1;
    s->st_reinforce_id = reinforce_id.d.lword;

    localport = vines_create_port(VINES_ADDR_NATIVE,
				  vsvc_st_info.well_known_port);
    bcopy(localport, &s->st_port, sizeof(vinesaddress));
    s->st_change_id    = vst_saved_change_id;
    s->st_need_info    = FALSE;
    s->st_info_valid   = TRUE;
    TIMER_START(s->st_last_update, VST_EXPIRATION);
}

/*
 * vst_insert_self
 *
 * Create an entry in the servers table for the router, and create an
 * entry in the group table for the name of the router.
 */
static void vst_insert_self (void)
{
    vst_init_serverentry();
    vst_create_local_group(vsvc_hostname(), vst_1st_orgname, vines_network,
			   VST_HIGHEST_PRECEDENCE, FALSE);
    vst_create_local_group(vsvc_hostname(), vst_2nd_orgname, vines_network,
			   VST_HIGHEST_PRECEDENCE, TRUE);
}

/*
 * vst_invalidate_entry
 */
static boolean vst_invalidate_entry (treeLink *x, void *data)
{
    serverentry* s;

    s = vsvc_ServerCast(x);
    memset(&s->st_port, 0, sizeof(vinesaddress));
    s->st_database_id  = 0;
    s->st_change_id    = 0;
    s->st_reinforce_id = 0;
    s->st_need_info    = FALSE;
    s->st_info_valid   = FALSE;
    TIMER_STOP(s->st_last_update);
    return(TRUE);
}

/*
 * vst_delete_self
 *
 * Delete the entry in the servers table that describes this router, And
 * the entry in the group table for this router's name.
 */
static void vst_delete_self (void)
{
    serverentry* s;

    vst_delete_local_group(vsvc_hostname(), vst_2nd_orgname,
			   vines_network, FALSE);
    vst_delete_local_group(vsvc_hostname(), vst_1st_orgname,
			   vines_network, TRUE);
    if (!(s = vsvc_FindServer(vines_network)))
	return;
    vst_saved_change_id = s->st_change_id;
    vst_invalidate_entry(&s->link, NULL);
}


/**************************************************
 *
 *	       ST - Display Routines
 *
 **************************************************/

static const char serverhdr[] =
 "\n  Server Name     Port                 Database Change   Reinforc LastMsg\n";
static const char grouphdr[] =
 "\n  Group Name                        Server Name     Netgroup L Pr DSeq Age\n";

/*
 * vst_display_forward_queue
 *
 * Are there any pending messages, and when is the first.  Don't bother
 * running the queue and printing them all.
 */
static inline void vst_display_forward_queue (void)
{
    paktype *pak;
    uchar time_string[20];
    ushort i;

    if (!QUEUEEMPTY(&vst_forwardQ)) {
	pak = (paktype *)vst_forwardQ.qhead;
	i = QUEUESIZE(&vst_forwardQ);
	sprint_ticks_in_dhms(time_string, TIME_LEFT_SLEEPING(pak->transmit_time));
	printf("\n  %d forwarded message%s pending, first in %s.",
	       i, (i > 1 ? "s" : ""), time_string);
    }
}

/*
 * vst_display_handles
 *
 * Display all currently existing Session Handles.  This will either be in use
 * by clients, or the clients are being verified as valid uses.  Old handles
 * are deleted immediately.
 */
static void vst_display_handles (void)
{
    vst_handle *handle;
    dqueue_t *q;

    if (DQUEUE_EMPTY(vst_handleQ))
	return;
    printf("\n\n  Session handles:");
    printf("\n    Handle    Valid  Port                Expires   User");
    RUN_DQUEUE(vst_handleQ, q) {
	handle = q->parent;
	/* vsvc_break_port(&handle->port, &net, &host, &port); */
	printf("\n    %08x   %3s   %08x:%04x:%04x     %-2d    %s",
	       handle->session_handle, (handle->valid ? "Yes" : "No"),
	       handle->net, handle->host, handle->port,
	       TIME_LEFT_SLEEPING(handle->timer)/ONEMIN, handle->username);
    }
}

/*
 * vst_display_operations
 *
 * Display all currently outstanding operations.
 */
static void vst_display_operations (void)
{
    vst_operation *operation;
    vst_handle *handle;
    dqueue_t *ptr;

    if (DQUEUE_EMPTY(vst_operationQ))
	return;
    printf("\n\n  Operations in progress:");
    printf("\n    Server           Handle    Type   User");
    RUN_DQUEUE(vst_operationQ, ptr) {
	operation = ptr->parent;
	handle = vst_find_handle(operation->handle);
	printf("\n    %#15Z  %08x  %-5d   %s",
	       operation->server, operation->handle, operation->type,
	       handle ? handle->username : "unknown");
    }
}

/*
 * vst_display_timer
 *
 * Display one timer in a pretty format.
 */
static void vst_display_timer (sys_timestamp timer, char *string)
{
    char time_string[20];

    if (TIMER_RUNNING(timer)) {
	sprint_ticks_in_dhms(time_string, TIME_LEFT_SLEEPING(timer));
	printf("\n  Next %s message generated in %s.", string, time_string);
    }
}

/*
 * vst_display_time_dest
 *
 * Display one timer and its list of destinations in a pretty format.
 */
static void vst_display_time_dest (vst_msg_dest *dst_list, char *string)
{
    char time_string[20];
    int i;

    if (TIMER_RUNNING(dst_list->when)) {
	sprint_ticks_in_dhms(time_string, TIME_LEFT_SLEEPING(dst_list->when));
	printf("\n  Next %s message generated in %s to", string, time_string);
	if (dst_list->broadcast) {
	    printf(" Broadcast");
	} else {
	    if (dst_list->count > 2)
		printf(":\n       ");
	    for (i = 0; i < dst_list->count; i++)
		printf("%s %#Z", (i==0 ? "" : ","), dst_list->dest[i]);
	}
    }
}

/*
 * vst_print_server_node
 *
 * Print a single entry from the servers tree.
 */
static boolean vst_print_server_node (treeLink *x, void *dummy)
{
    serverentry* s = vsvc_ServerCast(x);
    char time_string[20], port_string[30];
    ulong age;

    if (!s->st_info_valid)
	return(TRUE);
    if (TIMER_RUNNING(s->st_last_update)) {
	age = VST_EXPIRATION - TIME_LEFT_SLEEPING(s->st_last_update);
	sprint_ticks_in_dhms(time_string, age);
    } else {
	sprintf(time_string, "never");
    }
    vines_printf_address(port_string, &s->st_port);
    printf("\n %c%#15Z %20s %08x %08x %08x %s",
	   (s->st_need_info ? '*' : ' '), s->serverid, port_string,
	   s->st_database_id, s->st_change_id, s->st_reinforce_id,
	   time_string);
    return(TRUE);
}

/*
 * vst_print_group_node
 *
 * Print a single entry from the group tree.
 */
static boolean vst_print_group_node (treeLink *x, void *dummy)
{
    char group_name[VST_DOMAINLEN + VST_ORGLEN];
    vst_groupentry* g = vst_GroupCast(x);
    char time_string[20];
    ulong age;

    age = VST_EXPIRATION - TIME_LEFT_SLEEPING(g->expires);
    sprint_ticks_in_dhms(time_string, age);
    printf("\n  %31s %c %#15Z %08x %c %-2d %04x %s",
	   vst_group2name(g, group_name),
	   (g->is_servername ? '*' : ' '), g->serverid,
	   g->netgroup_id, (g->localgroup_id ? 'Y': 'N'), g->priority,
	   g->domain_seq, time_string);  
    return(TRUE);
}

/*
 * vst_display_group_tree
 *
 * Dump the domain@org tree information.  This doesn't show the information on
 * the database itself, just the tree information.
 */
void vst_display_group_tree (void)
{
    if (!streettalk_enabled)
	return;
    printf("\n\nGroup Tree Information:");
    RBTreeStats(vines_groups,"  Groups");
}


/**************************************************
 *
 *	ST - Routines Common Across Services
 *
 **************************************************/

/*
 * vst_new_neighbor
 *
 * A new neighbor has appeared on the network.  The router (and eveyone
 * else on the wire) needs to send it a welcome message.
 */
void vst_new_neighbor (ulong net, vinesidbtype *idb)
{
    if (!streettalk_enabled)
	return;

    if (vst_debug)
	buginf("\nVST: New neighbor server %#Z seen.", net);
    vst_set_gen_dest(&vst_dest_welcome, net);
    if (vst_needs_cold_dump) {
	vst_start_search();
    }
}

/*
 * vst_hostname_changed
 *
 * The router's hostname (or vines name) has changed.  Flush all existing
 * group entries belonging to this router, and rebuild them with the new
 * name.
 */
void vst_hostname_changed (void)
{
    serverentry *self;

    if (!streettalk_enabled)
	return;

    if (!(self = vsvc_FindSelf(vst_debug, "ST")))
	return;
    vst_WalkGroupTree(vst_flush_old_group, self);
    vst_create_local_group(vsvc_hostname(), vst_1st_orgname,
			   vines_network, VST_HIGHEST_PRECEDENCE, FALSE);
    vst_create_local_group(vsvc_hostname(), vst_2nd_orgname,
			   vines_network, VST_HIGHEST_PRECEDENCE, TRUE);
}

/*
 * vst_age_one_group
 *
 * Examine a single group to see if its expiration timer has fired.
 * If so, delete it.
 */
static boolean vst_age_one_group (treeLink *x, void *data)
{
    char group_name[VST_DOMAINLEN + VST_ORGLEN];
    vst_groupentry *g;

    g = vst_GroupCast(x);
    if (XSLEEPING(g->expires, VST_EXPIRATION))
	return(TRUE);
    if (vst_debug)
	buginf("\nVST: Aging out group %s from %#Z",
	       vst_group2name(g, group_name), g->serverid);
    vst_DeleteGroup(g);
    return(TRUE);
}

/*
 * vst_age_groups
 *
 * Age all members of the group database.  If their expiration timer
 * has fired, delete them.
 */
static void vst_age_groups (void) {
    vst_WalkGroupTree(vst_age_one_group, NULL);
}

/*
 * vst_periodic
 *
 * Perform any periodic processing for the StreetTalk service.  This
 * routine is called at least once every twelve hours.
 */
void vst_periodic (void)
{
    vst_operation *operation;
    vst_handle *handle;
    dqueue_t *ptr, *next;

    if (!streettalk_enabled)
	return;

    /*
     * Still looking for a server to get a cold dump from?
     */
    if (TIMER_RUNNING(vst_search_ends)) {
	if (!XSLEEPING(vst_search_ends, VST_SEARCH_MAX)) {
	    if (vst_debug)
		buginf("\nVST: Could not find peer to cold dump.");
	    vst_stop_search();
	} else {
	    if (!XSLEEPING(vst_timer_search, VST_SEARCH_INTERVAL)) {
		vst_send_search(TRUE);
		TIMER_UPDATE(vst_timer_search, VST_SEARCH_INTERVAL);
	    }
	    mgd_timer_set_soonest(&vsvc_st_info.timer, vst_timer_search);
	}
    }

    /*
     * Send any queued messages.  Updates the streettalk master timer
     * to account for any packets that still need to be sent.
     */
    vst_send_delayed_msgs();

    /*
     * Generate any message whose timer has expired, and update the
     * streettalk master timer to account for those that haven't.  Only
     * the `summary' timer should be reset after expirration to insure
     * that it is always be kept running.
     */
    vst_gen_delayed_msg(&vst_dest_welcome, vst_send_ni_welcome);
    vst_gen_delayed_msg(&vst_dest_detail,  vst_send_ni_detail);
    vst_gen_delayed_msg(&vst_dest_need,    vst_send_ni_need);
    if (!XSLEEPING(vst_timer_summary, VST_SUMMARY_INTERVAL)) {
	vst_send_ni_summary();
	TIMER_UPDATE(vst_timer_summary, VST_SUMMARY_INTERVAL);
	vst_age_groups();
    }
    mgd_timer_set_soonest(&vsvc_st_info.timer, vst_timer_summary);

    /*
     * 'Handle' processing
     */
    if (TIMER_RUNNING_AND_AWAKE(vst_handle_timer)) {
	TIMER_STOP(vst_handle_timer);
	RUN_DQUEUE_W_NEXT(vst_handleQ, ptr, next) {
	    handle = ptr->parent;
	    if (AWAKE(handle->timer)) {
		lw_remove(ptr);
		free(handle);
		continue;
	    }
	    vst_handle_timer = TIMER_SOONEST(vst_handle_timer, handle->timer);
	}
    }
    mgd_timer_set_soonest(&vsvc_st_info.timer, vst_handle_timer);
 
    /*
     * 'Operation' processing
     */
    if (TIMER_RUNNING_AND_AWAKE(vst_operation_timer)) {
	TIMER_STOP(vst_operation_timer);
	RUN_DQUEUE_W_NEXT(vst_operationQ, ptr, next) {
	    operation = ptr->parent;
	    if (AWAKE(operation->timer)) {
		vst_lookup_finished(operation->handle, NULL, NULL,
				    VSVC_ERR_TIMEOUT);
		lw_remove(ptr);
		free(operation);
		continue;
	    }
	    vst_operation_timer = TIMER_SOONEST(vst_operation_timer,
						operation->timer);
	}
    }
    mgd_timer_set_soonest(&vsvc_st_info.timer, vst_operation_timer);
}

/*
 * vst_age_service_info
 *
 * Age any Streettalk Specific information that is maintained on the
 * shared server entry data structures.  Return TRUE if the data
 * structure should be retained.
 */
boolean vst_age_service_info (serverentry *s)
{
    if (!streettalk_enabled)
	return(FALSE);

    if (s->st_info_valid == FALSE)
	return(FALSE);
    if (TIMER_RUNNING_AND_SLEEPING(s->st_last_update))
	return(TRUE);
    vst_WalkGroupTree(vst_flush_old_group, s);
    memset(&s->st_port, 0, sizeof(vinesaddress));
    s->st_database_id  = 0;
    s->st_change_id    = 0;
    s->st_reinforce_id = 0;
    s->st_need_info    = FALSE;
    TIMER_STOP(s->st_last_update);
    return(FALSE);
}

/*
 * vst_command
 *
 * Configuration for the StreetTalk service.
 */
void vst_command (parseinfo *csb)
{
    if (csb->nvgen) {
	if (!vines_running) {
	    return;
	}
	switch (csb->which) {
	  case VINES_STREETTALK:
	    nv_write(streettalk_enabled, "%s", csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case VINES_STREETTALK:
	if (streettalk_enabled == csb->sense)
	    break;
	streettalk_enabled = csb->sense;
	if (streettalk_enabled) {
	    vst_start();
	} else {
	    vst_stop();
	}
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * vst_init
 *
 * Boot time initializion of information that is maintained by the StreetTalk
 * service.  This routine will only ever be called once by the router.
 */
void vst_init (void)
{
    /*
     * Initialize the service.
     */
    streettalk_enabled = FALSE;
    vsvc_install_svc(&vsvc_st_info);
    vst_saved_change_id = 0;
    vst_1st_orgname = "Servers";
    vst_2nd_orgname = (configregister & CFG_OEM) ? "Routers" : "CiscoRouters";

    /*
     * Initialize the private Domain@Org tree
     */
    vines_groups = NULL;
    o_init(&vst_org_servers, NULL);

    /*
     * Functional Stuff: Handles and Operations
     */
    o_init(&vst_handleQ, NULL);
    o_init(&vst_operationQ, NULL);
    TIMER_STOP(vst_operation_timer);
    TIMER_STOP(vst_handle_timer);
    vst_handle_sequence = random_gen() & 0xFFFF;
}

/*
 * vst_start
 *
 * Run time initialize of information that is maintained by the StreetTalk
 * service.  This routine will be called each time the 'vines routing' command
 * is issued.
 */
void vst_start (void)
{
    if (!streettalk_enabled)
	return;

    /*
     * Database Synchronization Stuff
     */
    queue_init(&vst_forwardQ, 0);
    vst_clear_gen_dest(&vst_dest_need);
    vst_clear_gen_dest(&vst_dest_detail);
    vst_clear_gen_dest(&vst_dest_welcome);
    TIMER_START(vst_timer_summary, VST_SUMMARY_INTERVAL);
    vst_needs_cold_dump = TRUE;
    vst_start_search();

    /*
     * Functional Stuff: Handles & Operations
     */
    TIMER_STOP(vst_handle_timer);
    TIMER_STOP(vst_operation_timer);

    /*
     * Build the Domain@Org tree.  The local_id variable *must* be set first.
     */
    vst_localgroup_id = 1;
    vines_groups = RBTreeCreate("Vines", "VN", "Group Table", RBTreePrintKeyLong,
				&vst_debug);
    vst_insert_self();
}

/*
 * vst_stop
 *
 * Purge all information that is maintained by the StreetTalk service.  This
 * routine will be called each time the 'no vines routing' command is issued.
 */
void vst_stop (void)
{
    dqueue_t *ptr, *next;
    paktype *pak;

    /*
     * Database Synchronization stuff.
     */
    TIMER_STOP(vst_timer_summary);
    vst_stop_search();
    vst_clear_gen_dest(&vst_dest_need);
    vst_clear_gen_dest(&vst_dest_detail);
    vst_clear_gen_dest(&vst_dest_welcome);
    while ((pak = dequeue(&vst_forwardQ)) != NULL)
	datagram_done(pak);
    vst_needs_cold_dump = FALSE;

    /*
     * Functional Stuff: Handles and Operations
     */
    TIMER_STOP(vst_handle_timer);
    TIMER_STOP(vst_operation_timer);
    RUN_DQUEUE_W_NEXT(vst_handleQ, ptr, next) {
	lw_remove(ptr);
	free(ptr->parent);
    }
    RUN_DQUEUE_W_NEXT(vst_operationQ, ptr, next) {
	lw_remove(ptr);
	free(ptr->parent);
    }

    /*
     * Delete the private group tree.  We must walk it first because all
     * of the entries are multiply threaded.  Once walked, it can then be
     * deleted.
     */
    vst_delete_self();
    if (vines_groups != NULL) {
	vst_WalkGroupTree(vst_delete_one_group, NULL);
	vines_groups = RBTreeDestroy(vines_groups, &vst_debug);
    }

    /*
     * Invalidate all streettalk use of the common server tree.  Yank
     * the timers to force a pass over the common tree to see if it is
     * still in use or can be deleted.
     */
    vsvc_WalkServerTree(vst_invalidate_entry, NULL);
    mgd_timer_start(&vsvc_tree_timer, 0);
}

/*
 * vst_display
 *
 * Display all information related to the StreetTalk service.
 */
void vst_display (void)
{
    if (!streettalk_enabled)
 	return;

    printf("\n\n  Timer based events:");
    vst_display_forward_queue();
    vst_display_timer(vst_timer_search,      "search");
    vst_display_timer(vst_timer_summary,     "summary");
    vst_display_time_dest(&vst_dest_welcome, "welcome");
    vst_display_time_dest(&vst_dest_detail,  "detail");
    vst_display_time_dest(&vst_dest_need,    "need");
    if (vst_needs_cold_dump) {
	printf("\n  Needs a server for a full table dump.");
    }

    printf("\n");
    automore_header(serverhdr);
    vsvc_WalkServerTree(vst_print_server_node, NULL);

    automore_header(grouphdr);
    vst_WalkGroupTree(vst_print_group_node, NULL);

    automore_header(NULL);
    vst_display_handles();

    automore_header(NULL);
    vst_display_operations();
}
