/* $Id: at_static.c,v 3.5.26.3 1996/07/10 22:19:00 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_static.c,v $
 *------------------------------------------------------------------
 * at_static.c  -- AppleTalk Static Routing subroutines
 *
 * October  1993, Grisha Kotlyar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_static.c,v $
 * Revision 3.5.26.3  1996/07/10  22:19:00  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.5.26.2  1996/03/23  01:27:40  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.5.26.1  1996/03/18  18:53:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.1  1996/03/07  08:29:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/01/18  19:39:30  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/03  11:45:36  dwong
 * CSCdi46301:  ALIGN-3-SPURIOUS Spurious memory access made at 0x60492240
 *
 * Revision 3.3  1995/11/17  08:43:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:50:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/15  23:00:11  kleung
 * CSCdi40441:  AT static route still used even if next hop interface is
 * shutdown
 *
 * Revision 2.3  1995/07/25  00:17:30  kleung
 * CSCdi37595:  Extra space in NVRAM for AppleTalk static route
 * configuration.
 *
 * Revision 2.2  1995/07/24  22:30:37  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.1  1995/06/07 20:09:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include <interface.h>
#include "packet.h"
#include "config.h"
#include "atalk_private.h"
#include "at_static.h"
#include "at_domain.h"

#define FALSE  0
#define TRUE   1

#define ONLY_ONE 1
/*#define AT_STATIC_DEBUG 1*/

a_static  *a_static_head;  /* Head of the static list */
a_static  *a_static_tail;  /* Tail of the static list */

/*
************  Private ***********************
*/
static boolean at_static_CreateZone(a_static *, uchar *, ushort);
static void    at_static_PutZone(a_static *, char *, ushort);
static void    at_static_DeleteZoneList(a_static *);
static boolean at_static_ErrForExtended(ushort,ushort);
static boolean at_static_ErrForNonExtended(ushort);
static void    at_static_updateroute(a_static *, ushort);
static boolean at_static_sanitycheck(ushort, ushort, ataddress *, ushort);


/*
 * Routine:        atstatic_redist_all(boolean distflag)
 *
 * Purpose:        
 *
 * Called From:    atstatic_Initialize()
 */
static void
atstatic_redist_all (boolean distflag)
{
    rbTree	*tree;

    tree = atroute_GetPTree(AT_PATH_STATIC);
    if (tree == NULL)
	return;

    RBTreeForEachNode(atroute_path_redist_rbshell, (void *)distflag,
		      tree, FALSE);
}

/*
 * Routine:        atstatic_Initialize()
 *
 * Purpose:        Initializes static module in AppleTalk
 *
 * Called From:    atalk_init()
 */
void atstatic_Initialize (void)
{
    /* Initialiaze static path tree */
    atroute_InitPathTree(AT_PATH_STATIC, "AT STATIC Path Tree");
    /* Register Static protocol for IGRP */
    atroute_RegisterProtocol(AT_PATH_STATIC,
			     AT_ROUTE_LEAK_STATIC,
			     AT_ROUTE_UNLEAK_STATIC,
			     AT_ROUTE_LEAKED_STATIC,
			     NULL, NULL,
			     atstatic_redist_all,
			     NULL);
    /* Mark that static initialization is done */
    atalk_static  = TRUE;
    a_static_head = NULL;
    a_static_tail = NULL;
}

/*
 * Routine:        atalk_static_command(parseinfo *)
 *
 * Purpose:        Gets new parser info and processes it.
 *
 * Called From:    appletalk_command()
 */
void atalk_static_command (parseinfo *csb)
{

#ifdef AT_STATIC_DEBUG  
    printf("\n atalk_static_command is called ");
#endif

    if(!atalk_static)
	return;

    /* Check for "wr term" */
    if (csb->nvgen) {
	if(atalk_running)
	    nv_atalk_static(csb);
	return;
   }
   
    /* If not for NvRam  then do all assignments */
    atalk_static_assign(csb);
}

/*
 * Routine:        nv_atalk_static(parseinfo *)
 *
 * Purpose:        Writes the whole static table into NVRam.
 *
 * Called From:    atalk_static_command()
 */
void nv_atalk_static (parseinfo *csb)
{
    a_static *as;
    a_static_zonelist *z;

#ifdef AT_STATIC_DEBUG
    printf("\n nv_static_debug is called ");
#endif

    as = a_static_head;
    while (as != NULL) {
	z = as->z_hdr;
	if(as->end == 0) {
	    while (z) {
		nv_write(TRUE,"%s network %d to %d.%d %s zone ",
			 csb->nv_command, as->start,
			 as->nextaddr->net, as->nextaddr->node,
			 as->floating ? "floating" : "");
		atalk_nv_printzone(z->zonename);
		z = z->next;
	    }
	} else {
	    while (z) {
		nv_write(TRUE,"%s cable-range %d-%d to %d.%d %szone ",
			 csb->nv_command, as->start, as->end,
			 as->nextaddr->net, as->nextaddr->node,
			 as->floating ? "floating " : "");
		atalk_nv_printzone(z->zonename);
		z = z->next;
	    }
	}
	as = as->next;
    }
}

/*
 * Routine:     atalk_static_assign (parseinfo *)
 * 
 * Purpose:     Kicks off all the static assignments out of csb
 *
 * Called From: atalk_static_command()
 */
boolean atalk_static_assign (parseinfo *csb)
{
    ushort    start;
    ushort    end;
    ataddress nextaddr;
    ushort    hops;
    ushort    test_flag = 0;
    boolean   floating;
    uchar     zonename[ZONELEN];
    ushort    zonelen;
    boolean   Error;

#ifdef AT_STATIC_DEBUG
  printf("\n atalk_static_assign is called");
#endif

    start = (ushort)GETOBJ(int,1);
    end   = (ushort)GETOBJ(int,2);
    

    Error = ((end != 0) ? 
	    at_static_ErrForExtended(start,end) : 
	    at_static_ErrForNonExtended(start));
    if(Error) {
	if(end == 0)
	    printf("\n Bad appletalk network address :  %d  ", start);
	else
	    printf("\n Bad AppleTalk cable range : %d-%d ", start,end);
	return(FALSE);
    }
	
	    
    if (GETOBJ(paddr,1)->type == ADDR_APPLE) {
	nextaddr.net  = GETOBJ(paddr,1)->apple_addr.net;
	nextaddr.node = GETOBJ(paddr,1)->apple_addr.host;
    } else {
	return(FALSE);
    }
        
    if (GETOBJ(int,3) == TRUE) { /* there's an optional hop cnt */
        hops = (ushort)GETOBJ(int,4);
	test_flag = 1;
    } else {
        hops = 1; /* default */
    }
    
    if(GETOBJ(int,5)) { /* the entry is a floating one */
	floating = TRUE;
    }
    else
	floating = FALSE;

    if (!csb->sense)  {
	/* It is "no appletalk static cable/net to ... " command */
	return(at_static_noprefix(start,end,&nextaddr));
    }

    zonelen = atalk_hex_filter(GETOBJ(string,1), (char *)zonename, ZONELEN);
    if (zonelen == 0) 
	return(FALSE);
    

    /* Insert this entry into the static module */
    if (!at_static_insert(start,end,&nextaddr,hops,zonename,zonelen,test_flag,
			  floating))
	return(FALSE);
    
    return(TRUE);
}

void at_static_validate_route(ushort Start, ushort End)
{

    a_static *as;

    as = at_find_static(Start);
    if(!as || !as->idb)
	return;

    if(as->floating) {
	if (!at_insert_static_route(as)) {
	     buginf("\n AT: could not create static entry");
	     return;
	 }
	at_static_AddAllZones(as);
	as->flags = AT_STATIC_ACTIVE;

    }
	


}


/*
 * Routine:     at_static_insert(start,end,nextaddr, hops,zonename, zonelen)
 *
 * Purpose:     Inserts static entry into the static and AppleTalk tables
 *
 * Called From: atalk_static_assign()
 */
boolean at_static_insert (ushort start, ushort end, ataddress *nextaddr, 
			  ushort hops, uchar *zonename, ushort zonelen,
			  ushort test_flag,
			  boolean floating)
{
    a_static *as;
    atalkidbtype  *idb;
    ushort   flags;
    atalk_rdb_t            *r;
    rbTree                 *r_tree;


#ifdef AT_STATIC_DEBUG
    printf("\n at_static_insert is called ");
#endif

    /* 
     * Make a sanity check for this static entry 
     * before processing it
     */

    if(!floating) {
	if(!at_static_sanitycheck(start, end, nextaddr, test_flag)) 
	    return(FALSE);
    }

    /* 
     * First find out whether nextaddr is on the known AppleTalk idb.
     * If it is, then mark this as potentially ACTIVE entry
     */
    if ((idb = at_get_nextaddr_idb(nextaddr)) == NULL) {
	printf("\nAT-STATIC: Warning -> Unknown AT address : %d.%d ", 
	       nextaddr->net, nextaddr->node);
	flags = AT_STATIC_NONACTIVE;	
    } else
	flags = AT_STATIC_ACTIVE;

    /* Now see if this static entry already exists */
    if( (as = at_find_static(start)) == NULL) {
	/* Does not exist, therefore create a new one. */
	as = at_static_new(start, end, nextaddr, hops, idb, flags, 
			   test_flag, floating);
	if(as == NULL){
#ifdef AT_STATIC_DEBUG 	    
	    printf("\n Static assignment failed");
#endif
	    return(FALSE);
      	}  
	/* Now insert static entry to the main AppleTalk routing table */
	if (flags == AT_STATIC_ACTIVE) {
	    if (!at_insert_static_route(as)) {
		buginf("\n AppleTalk: could not create static routing entry ");
		at_static_delete(as);
		return(TRUE);
	    }
	}
    }  else {    /* If static entry exists for this net == start */
	/* Now check if all the parameters match */

	if(!at_static_match(start,end,nextaddr,as)) {
	    printf("\n AT Static entry conflict; static entry is ignored");
	    return(FALSE);
	} else {

	    /* Check for updating hops */
	    if(hops != as->hops && at_static_FindZone(as, zonename)){ 
		at_static_updateroute(as,hops);
		return(TRUE);
	    }
	}
    }
    

    /* First check if this zonename is already there */
    if (at_static_FindZone(as,zonename)) {
	return(FALSE);
    }
	
    /* 
     * Now let's not bother with not active entries.
     * The only thing left is to remember a new zone into static table
     */
    if (flags != AT_STATIC_ACTIVE) {
	
	at_static_PutZone (as, zonename, zonelen);
	return(TRUE);
    }
	
    /*
     * Now we are left with only active entries
     * (with known ApleTalk idb)
     * Let's put a new zone to the main Zone Table
     */

    /* First check if this zonename is already there */
    if (at_static_FindZone(as,zonename)) {
	return(FALSE);
    }

    at_static_CreateZone(as, zonename, zonelen);

    /* Remember the rdb pointer */
    r_tree = atroute_GetTree();
    r = atroute_LookupRoute(r_tree, as->start, TRUE);
    if(r == NULL || r->rpath.type != AT_PATH_STATIC)
	return(FALSE);
    as->r = r;

    /* Now redistribute */
    atroute_path_redist_rbshell((treeLink *) as->r->best_path, FALSE);

    return(TRUE); 
}


/*
 * Routine:       at_static_collect_routes(idbtype *)
 *
 * Purpose:       This routine collects(re-activates)
 *                all non-active static routes
 *                pointing to this idb.
 *                
 * Called From:   maint_StateHandler()
 */
void at_static_collect_routes (atalkidbtype *idb)
{
    a_static  *ar = a_static_head;
#ifdef AT_STATIC_DEBUG
    printf("\n at_static_collect_routes is called ");
#endif

    while (ar) {
	if (ar->flags != AT_STATIC_ACTIVE &&
	    idb->atalk_enabled && idb->atalk_operational &&
	    ar->nextaddr->net >= idb->atalk_cablestart && 
	    (((idb->atalk_cableend == 0) &&
	      (ar->nextaddr->net == idb->atalk_cablestart)) || 
	     (ar->nextaddr->net <= idb->atalk_cableend))) {

	    ar->idb = idb;  
	    if (!at_insert_static_route(ar)) {
	      buginf("\n AT: could not create static entry");
	      return;
	    }
	    at_static_AddAllZones(ar);
	    ar->flags = AT_STATIC_ACTIVE;
	    if (ar->r)
	        atroute_path_redist_rbshell((treeLink *) ar->r->best_path, FALSE);
        }
	ar = ar->next;
    }
}

/*
 *
 * Routine:     at_static_AddAllZones(a_static *as)
 *
 * Purpose:     Adds all zones out of the zone list
 *              of this static entry to the main zone table
 *
 * Called From: at_static_collect_routes()
 */
void at_static_AddAllZones(a_static *as)
{
    a_static_zonelist *z;
    atalk_rdb_t       *r;
    rbTree            *tree;
    

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_AddAllZones is called ");
#endif

    tree    = atroute_GetTree();

    /* Now find routing entry */
    r = atroute_FindRoute(tree,as->start, FALSE);
    if (r == NULL) {
#ifdef AT_STATIC_DEBUG    
	printf("\n atalk: Could not find static route for zone update ");
#endif

	return;
    }

    /* Get the header of this static entry zone list */
    z = as->z_hdr;

    /* Go through all zone entries */
    while (z) {
	atzip_AddZone(r, z->zonename, z->zonelen);
	z = z->next;
    }
    ReleaseRoute(r);
}

/* 
 * Routine:       at_get_nextaddr_idb (nextaddr)
 *
 * Purpose:       Gets idb based on ataddress of nextaddr
 *
 * Called From:   at_static_insert()
 */
atalkidbtype *at_get_nextaddr_idb (ataddress *nextaddr)
{
    rbTree         *r_tree;
    atalk_rdb_t    *rt;
    atalkidbtype        *idb;
    
#ifdef AT_STATIC_DEBUG 
    printf("\n at_get_nextaddr_idb is called");
#endif

    r_tree  = atroute_GetTree();
   
    /* Find the route with protecting */
    rt  = atroute_FindRoute(r_tree, nextaddr->net, TRUE);
    if (rt == NULL) {
	return(NULL);
    }
    if(rt->rpath.type != AT_PATH_CONNECTED) {
	ReleaseRoute(rt);
	return(NULL);
    }
    idb     = rt->rpath.idb;

    ReleaseRoute(rt);
    return(idb);
}

/*
 * Routine:       at_find_static(net)
 *
 * Purpose:       Finds a static entry based on the beginning of 
 *                the range
 *
 * Called From:   at_static_insert(), at_static_noprefix()
 */
a_static *at_find_static(ushort net)
{
    a_static   *as;

#ifdef AT_STATIC_DEBUG  
    printf("\n at_find_static is called ");
#endif

    as = a_static_head;

    while(as) {
	if(as->start == net)
	    return(as);
	as = as->next;
    }
    return(NULL);  
}

/*
 * Routine:         atstatic_SetPathState
 * 
 * Purpose:         Sets the state of this path as requested.
 *                  A pointer to this function is installed on 
 *                  all STATIC paths created by this module.
 *
 * Called From:     at_insert_static_route()
 */
static boolean
atstatic_SetPathState (atalk_pdb_t* p,  at_state_t state)
{
    atalk_rdb_t	*r;
    at_state_t	ostate;

    if (p == NULL || p->type != AT_PATH_STATIC)
	return (FALSE);

    ostate  = p->state;
    r = p->rdb;
    /* Basically we are interested 
     * only in at_state_delete state
     * so we can do redistribution of static route
     */
    switch (state) {
      case at_state_good:
      case at_state_suspect:
      case at_state_bad:
	p->state = state;
	break;
      case at_state_delete:
	atroute_path_unredistribute(p);
	break;

      default:
	break;
    }
    if (p->state != ostate) {
	if (at_debug(appletalk_debug, ATALK_NULL_MASK))
	    buginf("\nAT: State change for %#A: %s -> %s",
		   atalk_CableRange(r), atalk_state_name(ostate),
		   atalk_state_name(p->state));
	p->action = at_change_path;
    }
    atroute_BestPath(p);
    return (TRUE);
}

/* 
 * Routine:      at_insert_static_route(as)
 *
 * Purpose:      Creates and adds a new static path 
 *               for the static entry.
 *               Also creates neighbor entry (if it does not yet exist)
 * 
 * Called From:  at_static_insert(), at_static_collect_routes()
 */
boolean at_insert_static_route ( a_static *as)
{
    atalk_metric_t         metric;
    atalk_pdb_t            *p;
    rbTree                 *p_tree;
    neighborentry          *neighbor;
    ataddrtype             nextaddr;
    atalk_rdb_t            *r;
    rbTree                 *r_tree;

    extern atalk_rdb_t *atroute_LookupRoute(rbTree *, ushort, boolean);

#ifdef AT_STATIC_DEBUG
    printf("\n at_insert_static_route is called ");
#endif
    if (!as || !as->idb)
        return(FALSE);

    if(!as->idb->atalk_operational)
	return(FALSE);
    memset(&metric, 0, sizeof(metric));
    atrtmp_SetMetric(&metric, as->hops);
    if(as->floating) {
	metric.metric  = AT_METRIC_FLOATING;
    }
    else {
	metric.metric      = AT_METRIC_STATIC;  /* Override standard metric. */
    }

    metric.type        = AT_PATH_STATIC;
    /*  
     * First create a neighbor entry if it does not exist
     */
    nextaddr  = atalk_address(as->nextaddr->net, (uchar)as->nextaddr->node);
    neighbor  = atmaint_FindNeighbor(nextaddr, as->idb);
    if (neighbor == NULL) {	/* didn't find it, so create */
	neighbor = atmaint_CreateNeighbor(nextaddr, as->idb);
	if(neighbor == NULL) {
	    return(FALSE);
	}
	GET_TIMESTAMP(neighbor->rtmp_firstheard);
	neighbor->rtmp_state = neighborUp;
	GET_TIMESTAMP(neighbor->rtmp_restarted);
    }
    neighbor->static_entry = TRUE;
    atmaint_ProtectNeighbor(neighbor);
    
    p_tree  = atroute_GetPTree(AT_PATH_STATIC);
    p = atroute_AddPath(p_tree, as->idb,  as->start, as->end, 
			neighbor, atstatic_SetPathState, NULL,
			0 , AT_PATH_STATIC, &metric);
    if(p == NULL) {
	if(as->floating)
	    return(TRUE);
	else
	    return(FALSE);
    }
    /* Now let's make sure that we did write the end of 
     * of the cable range correctly, as atroute_AddPath may not do it 
     */

    r_tree = atroute_GetTree();
    r = atroute_LookupRoute(r_tree, as->start, TRUE);
    if(r == NULL) {
	return(FALSE);
	}
    if(r->rpath.type == AT_PATH_STATIC) {
	r->dstrngstr = as->start;
	r->dstrngend  = as->end;
    }
    
    /* Now remember this rdb route */
    as->r = r;

    return(TRUE);

}

/*
 * Routine:      at_static_new(start,end,nextaddr,hops,idb, flags)
 *
 * Purpose:      Creates new static entry in the local static table.
 *               Returns the pointer to the created entry.
 *
 * Caled From:   at_static_insert()
 */
a_static *at_static_new(ushort start, ushort end, ataddress *nextaddr, 
			ushort hops, atalkidbtype *idb, ushort flags, 
			ushort test_flag, boolean floating)
{
    a_static *as;

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_new is called ");
#endif

    as = malloc(sizeof(a_static));

    if (as == NULL)
	return(NULL);

    /* now assign all routing parameters */
    
    
    if((as->nextaddr = malloc(sizeof(ataddress))) == NULL) {
	free(as);
	return(NULL);
    }
    as->nextaddr->net  = nextaddr->net;
    as->nextaddr->node = nextaddr->node;
    as->nextaddr->spec = 0;
    as->start = start;
    as->end   = end;
    as->hops   = hops;
    as->test_entry   = test_flag;
    as->idb   = idb;
    as->flags = flags;
    as->z_hdr = NULL; /* Zone list is empty */
    as->floating = floating;

    if(a_static_tail == NULL) {
	/* this is the very first entry in the list */
	a_static_head  = as;
	as->prev       = NULL;
    } else {
	as->prev = a_static_tail;
	a_static_tail->next = as;
    }

    as->next = NULL;
    a_static_tail = as;

    return(as);
}

/* 
 * Routine:       at_static_FindZone(as,zone)
 *
 * Purpose:       Tries to find the zone in the static entry's
 *                zone table. Returns TRUE if zone name matched.
 *
 * Called From:   at_static_CreateZone()
 */
boolean at_static_FindZone (a_static *as, char *zone)
{

    a_static_zonelist *z;

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_FindZone is called");
#endif

    z = as->z_hdr;
    
    while (z) {
	if(strcmp(z->zonename, zone) == 0)
	    return(TRUE);
	z = z->next;
    }
    return(FALSE);
}

/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * Routine:       at_static_CreateZone(as,zonename,zonelen)
 *
 * Purpose:       Adds zone to both static and main AppleTalk zone 
 *                lists.
 * 
 * Called From:   at_static_insert(),at_static_AddAllZones()
 */
static boolean
at_static_CreateZone (a_static *as, uchar *zonename, ushort zonelen)
{
   
    atalk_rdb_t   *r;
    rbTree        *tree;
    

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_CreateZone is called ");
#endif

    tree    = atroute_GetTree();

    /* Now find routing entry */
    r = atroute_FindRoute(tree,as->start, FALSE);
    if (r == NULL) {
#ifdef AT_STATIC_DEBUG    
	buginf("\n atalk: Could not find static route for zone update ");
#endif

	return(FALSE);
    }

    atzip_AddZone(r,zonename, zonelen);
    ReleaseRoute(r);
    at_static_PutZone (as, zonename, zonelen);
    return(TRUE);
}

/*
 * Routine:       at_static_PutZone(as,zonename,zonelen)
 *
 * Purpose:       Creates and Fills out the a_static_zonelist entry
 *
 * Called From:   at_static_insert(), at_static_CreateZone()
 */
static void at_static_PutZone (a_static *as, char *zonename, ushort zonelen)
{

    a_static_zonelist  *z, *z_entry;

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_PutZone is caled");
#endif

    z_entry  = malloc(sizeof(a_static_zonelist));

    if( z_entry == NULL)
	return;

    sstrncpy(z_entry->zonename, zonename, (zonelen + 1));
    z_entry->zonelen = zonelen;
    z_entry->next = NULL;

    z = as->z_hdr;

    if(z == NULL) { /* If zone list is empty */
	as->z_hdr = z_entry;
	return;
    }
    while(z->next) {
	z = z->next;
    }
    z->next = z_entry;
}

/* 
 * Routine:          at_static_noprefix(start,end,nextaddr)
 *
 * Purpose:          Takes care of the removing a static entry
 *                   from both the static table and routing table
 *
 * Called From:      atalk_static_assign()
 */
boolean  at_static_noprefix (ushort start, ushort end, ataddress *nextaddr)
{
    a_static *as;

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_noprefix  is called ");
#endif

    as = at_find_static(start);
    if(as == NULL)
	{
	    printf("\n Static entry is not found ");
	    return(FALSE);
	}
    /*if( !(as = at_find_static(start)))
	return(FALSE);
     */

    /* Now check if entry matches */
    if(!at_static_match(start, end, nextaddr, as)) {
	printf("\n Static entry is not found ");
	return(FALSE);
    }

    /* Delete this static entry from the routing table */
 
    at_static_PathAndRoute_Delete(as);
  
    /* And delete this entry from the static list */
    at_static_delete(as);

    return(TRUE);
} 

/*
 * Routine:       at_static_PathAndRoute_Delete(as)
 * 
 * Purpose:       Deletes static entry from the main 
 *                routing table
 *
 * Called From:   at_static_noprefix()
 */
void at_static_PathAndRoute_Delete (a_static *as)
{
    rbTree        *p_tree;
    atalk_pdb_t   *p;

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_PathAndRoute_Delete is called ");
#endif    


#ifdef AT_STATIC_DEBUG  
    printf("\n Deleting route %d", as->start);
#endif

    p_tree = atroute_GetPTree(AT_PATH_STATIC);
    p = atroute_LookupPath(p_tree,as->start);
    if(!p)
	return;

    atroute_DeletePath(p);
    
    /* Reset the zone list */
    atmaint_ResetRequestInfo(p->rdb);

    at_static_DeleteNeighbor(as);

}


/*
 * Routine:      at_static_neighbors_match (ataddress *)
 *
 * Purpose:      Counts how many static entries with this nextaddr
 *
 * Called From:  at_static_PathAndRoute_Delete()
 */
int at_static_neighbors_match (ataddress *next_key)
{
    a_static  *as = a_static_head;
    int       match = 0;

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_neighborss_match is called ");
#endif

    while (as)  {
	if(as->nextaddr->net  == next_key->net &&
           as->nextaddr->node == next_key->node)
	    match++;
	as = as->next;
    }
    return(match);

}

/* 
 * Routine:      at_static_DeleteNeighbor(as)
 *
 * Purpose:      Deletes AppleTalk neighbor from 
 *               the neighbor list
 * Called From:  at_static_PathAndRoutes_Delete()
 */
void at_static_DeleteNeighbor(a_static  *as)
{
    neighborentry          *neighbor;
    ataddrtype             nextaddr;
    int           	   NumberStaticNeighbors;


#ifdef AT_STATIC_DEBUG
    printf("\n at_static_DeleteNeighbor is called ");
#endif

    /* Now we have to take care of the neighbor entry*/
    NumberStaticNeighbors = at_static_neighbors_match(as->nextaddr);
    nextaddr  = atalk_address(as->nextaddr->net, (uchar)as->nextaddr->node);
    neighbor  = atmaint_FindNeighbor(nextaddr, as->idb);
    if (neighbor != NULL) {	/* found it, so delete it */
	atmaint_UnprotectNeighbor(neighbor);
	
	/*
	 * If there is only one neighbor (it is our entry)
	 * then it is no longer a static neighbor
	 */ 
	if(NumberStaticNeighbors == ONLY_ONE) {
	    neighbor->static_entry = FALSE;
	    neighbor->state = neighborDown;
	}
	
	
    }
}

/*
 * Routine:        at_static_match(start,end,nextaddr,as)
 *
 * Purpose:        Verifies that static entry matches to what 
 *                 is in the static table
 * Called From:    at_static_noprefix()
 */
boolean  at_static_match (ushort start, ushort end, ataddress *nextaddr, a_static *as)
{
#ifdef AT_STATIC_DEBUG
    printf("\n at_static_match is called ");
#endif

    if(as->start == start && as->end == end && as->nextaddr->net == nextaddr->net && 
       as->nextaddr->node == nextaddr->node)
	return(TRUE);

    return(FALSE);
}

/*
 * Routine:      at_static_delete(as)
 *
 * Purpose:      Deletes a static entry from the static table
 *
 * Called From:  at_static_insert(), at_static_noprefix()
 */
void at_static_delete (a_static *as)
{
#ifdef AT_STATIC_DEBUG 
    printf("\n at_static_delete is called" );
#endif

    if(as == a_static_head ) {
	if(as == a_static_tail) {
      
	    a_static_head = NULL;
	    a_static_tail = NULL;
      
	}
	else {
	    a_static_head = as->next;
	    a_static_head->prev = NULL;
	}
    
    }
    else if (as == a_static_tail) {
	a_static_tail = as->prev;
	a_static_tail->next = NULL;
    }    
    else {
	as->prev->next = as->next;
	as->next->prev = as->prev;
    }
    at_static_DeleteZoneList(as);
    free(as->nextaddr);
    free(as);
}

/* Routine:          at_static_DeleteZoneList(as)
 *
 * Purpose:          Deletes the whole Zone List pertaining
 *                   to this static entry.
 * Called From:      at_static_delete()
 */
static void at_static_DeleteZoneList (a_static *as)
{
    a_static_zonelist *z, *z_prev;

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_DeleteZoneList is called");
#endif    
    z = as->z_hdr;
    while(z) {	
	z_prev = z;
	z = z->next;
	free(z_prev);
    }
}


static boolean at_static_ErrForExtended(ushort start, ushort end)
{
    if(start == 0 || start >= ATALK_STARTUP_START || end < start)
	return(TRUE);
    return(FALSE);

}

static boolean at_static_ErrForNonExtended(ushort net)
{
    if(net == 0 || net >= ATALK_STARTUP_START)
	return(TRUE);
    return(FALSE);
}


/*
 * Routine:        at_static_nonactive(idb)
 *
 * Purpose:        Marks all entries pointing to this idb
 *                 as NONACTIVE
 * Called From:    atalkif_DisablePort()
 */
void at_static_nonactive(atalkidbtype *idb)
{

    a_static *as = a_static_head;
    
#ifdef AT_STATIC_DEBUG
    printf("\n at_static_nonactive is called");
#endif
    while(as) {
	if(as->idb == idb)  {
	    as->idb   = NULL;
	    as->flags = AT_STATIC_NONACTIVE;
	    at_static_PathAndRoute_Delete(as);
	}
	as = as->next;
    }
	
}

static void at_static_updateroute(a_static *as, ushort hops)
{

    boolean     valid;
    neighborentry          *neighbor;
    ataddrtype             nextaddr;
    atalk_pdb_t *p;
    rbTree      *tree;

    if(!as)
	return;
    if(!(as->flags == AT_STATIC_ACTIVE))
	return;

    tree = atroute_GetPTree(AT_PATH_STATIC);
    
    nextaddr  = atalk_address(as->nextaddr->net, (uchar)as->nextaddr->node);
    neighbor  = atmaint_FindNeighbor(nextaddr, as->idb);
    if(neighbor == NULL)
	return;
    
    p = atroute_GetMatchingPath(tree, as->start, as->end, as->idb, neighbor,
				&valid);
    if(p && valid) {
	atrtmp_SetMetric(&p->metric, hops);
	p->metric.metric      = AT_METRIC_STATIC; /* Override standard metric */
	GET_TIMESTAMP(p->timer);
	
	atroute_NewPathMetric(p->rdb, p);
	as->hops = hops;
    } 
}



/* 
 * Routine : at_static_sanitycheck(start,end, nextaddr, test_flag)
 * 
 * Purpose : Checks if this range is overlapping with any connected routes
 *           
 */
static boolean
at_static_sanitycheck(ushort start, ushort end,
		      ataddress *next, ushort test_flag)
{
 
    atalk_rdb_t   *r;
    rbTree        *tree;
    a_static *as = a_static_head;
    ataddrtype next_addr;
    atalkidbtype *idb;

#ifdef AT_STATIC_DEBUG
    printf("\n at_static_sanitycheck is called ");
#endif    

    if (atutil_RangesOverlap(start,end, next->net, next->net)) {
	printf("\n Illegal Static Entry");
	return (FALSE);
    }

    /* Now go through all the static entries
       and make sure it is not in conflict 
       with any of them (even with non-active ones)
     */
    while(as) {
	if (atutil_RangesOverlap(as->start, as->end, start, end))  {
	    if(as->start == start && as->end == end)
		return(TRUE);
	    else {
		printf("\n Static Entry is overlapping with static net(cable) %d-%d",
		       as->start, as->end);
		return(FALSE);
	    }
	}
	as = as->next;
    }
	
    tree    = atroute_GetTree();

    for (r = KeepFirstRoute(tree); r != NULL; r = KeepNextRoute(tree, r)) {
	/* Look for any overlapping entry */
	if (atutil_RangesOverlap(r->dstrngstr, r->dstrngend, start, end)) {
            /* Static route for testing purposes are exempted */
            if (test_flag) {
                ReleaseRoute(r);
	        return (TRUE);
            }
	    printf("\n Static Entry is overlapping with net(cable) %d-%d",
		   r->dstrngstr, r->dstrngend);
            ReleaseRoute(r);
	    return (FALSE);
	}
    }
    next_addr = atalk_address(next->net, next->node);
    FOR_ALL_ATIDBS(idb) {
	if (idb->atalk_enabled && idb->atalk_operational) {
            if (atalkif_MyAddress(idb) == next_addr && !test_flag) {
	        printf("\n Static Entry's next hop conflicts with address %a",
		   next_addr);
                return (FALSE);
            }
        }
    }

    if (atalk_DomainRouter_running) {
	if (atdomain_InsideInboundRemap(start, end)) {
	    printf("\n Static Entry is overlapping with one of the "
		   "inbound remapping ranges");
	    return (FALSE);
	}
    }
    return (TRUE);
}

/*
 * Routine:     at_testroute(network)
 *
 * Purpose:     Check if static route is for testing purpose.  Returns
 *              TRUE if test_entry flag is set.
 *
 * Called From: rtmp_GetNextRoute()
 */
boolean
at_testroute (ushort network)
{
    a_static *as;

    if ((as = at_find_static(network)) != NULL) {
        if (as->test_entry) 
	    return (TRUE);
    } 

    return (FALSE);
}
