/* $Id: x25_bfe.c,v 3.3.20.4 1996/09/10 03:08:32 snyder Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_bfe.c,v $
 *------------------------------------------------------------------
 * x25_bfe.c - X25 BFE emergency node table support
 *
 * February 1992, Babak Nader
 *
 *	Copyright (c) 1987-1997 by cisco Systems, Inc.
 *	All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_bfe.c,v $
 * Revision 3.3.20.4  1996/09/10  03:08:32  snyder
 * CSCdi68568:  more constant opportuniites
 *              160 out of data
 *               72 out of image
 * Branch: California_branch
 *
 * Revision 3.3.20.3  1996/08/28  13:23:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.20.2  1996/06/28  23:32:42  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/03/18  22:50:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:19:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:55:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/01  06:13:06  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  18:07:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/02  22:01:53  carvalho
 * CSCdi39783:  CMNS call to ethernet destination fails if LLC2 not open.
 *
 * Revision 2.2  1995/09/04  22:02:27  gglick
 * CSCdi37951:  X.25/BFE X.121 address generation incorrect
 *
 * Revision 2.1  1995/06/07  23:22:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "address.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser_defs_x25.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_proto.h"
#include "x25_bfe.h"
#include "x25_debug.h"
#include "logger.h"
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */

/* include parser informaition for BFE command */


bfetype *x25_bfe_hash_table[X25_BFE_HASH_SIZE];


/*
 * Initialize hash table
 */

void x25_bfe_hash_init (void)
{
    register int i;

    for (i = 0; i < X25_BFE_HASH_SIZE; i++)
	x25_bfe_hash_table[i] = (bfetype *)NULL;
}

/*
 * Lookup host address in hash table.
 * Return pointer to bfe block or null.
 */

bfetype *x25_bfe_lookup (
    register hwidbtype *idb,
    register ipaddrtype *h_add)
{
    register bfetype *l;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    l = x25_bfe_hash_table[X25_BFE_HASH(h_add, sizeof(ipaddrtype))];
    while (l) {
	if (l->idb == idb &&
	    bcmp((uchar *)&l->host_ipaddr, (uchar *)h_add,
		 sizeof(l->host_ipaddr)) == 0) {
	    reset_interrupt_level(level);
	    return (l);
	}
	l = l->bfe_next;
    }
    reset_interrupt_level(level);
    return ((bfetype *)NULL);
}

/*
 * Enter addresses into hash table.
 */

bfetype *x25_bfe_enter (
    hwidbtype *idb,
    ipaddrtype *h_add,
    ipaddrtype *r_add)
{
    register int hash;
    register bfetype *l;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    l = malloc(sizeof(bfetype));
    if (l) {
	bcopy(h_add, &l->host_ipaddr, sizeof(ipaddrtype));
	bcopy(r_add, &l->rbfe_ipaddr, sizeof(ipaddrtype));
	bfe_ip_parse(l->rbfe_ipaddr, &l->rbfe_rvn);
	l->idb = idb;
	hash = X25_BFE_HASH(h_add, sizeof(ipaddrtype));
	l->bfe_next = x25_bfe_hash_table[hash];
	x25_bfe_hash_table[hash] = l;
    }
    reset_interrupt_level(level);
    return (l);
}


/*
 * Remove addresses from hash table given host address
 */

void x25_bfe_delete (
    register hwidbtype *idb,
    register ipaddrtype *h_add)
{
    bfetype *bfep;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    bfep = x25_bfe_lookup(idb, h_add);
    if (bfep != (bfetype *)NULL)
        x25_bfenode_delete(idb, bfep);
    else
      errmsg(&msgsym(NOTFINDBH, X25), *h_add);

    reset_interrupt_level(level);
}


/*
 * x25_bfenode_delete
 * Remove node data given pointer to address block.
 */

void x25_bfenode_delete (hwidbtype *idb, register bfetype *hp)
{
    register uint hash;
    register bfetype **entry;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Now delink it from the host address hash table.
     */
    hash = X25_BFE_HASH(&hp->host_ipaddr, sizeof(hp->host_ipaddr));
    entry = &x25_bfe_hash_table[hash];
    while (*entry) {
	if (*entry == hp) {
	    *entry = hp->bfe_next;
	    break;
	}
	entry = &((*entry)->bfe_next);
    }

    reset_interrupt_level(level);

    free(hp);
}

/*
 * x25_bfe_sendcall
 * save information and send call again.
 */
static boolean x25_bfe_process_call (
    hwidbtype *hwidb,
    lcitype *lci,
    bfetype *bfe,
    uchar cause, 
    uchar diag,
    boolean conf)
{
    x25map *map;

    map = lci->lci_map;

    if (map) {
	int prec;
	ushort map_status;

	/* 
	 *  save the information needed to place a new Call and make
	 *  sure the map doesn't get deleted, then process the Clear
	 */
	prec = lci->lci_prec;
	map_status = map->map_status;
	map->map_status = X25_MAP_PERM;
	x25_map_remove_lci(lci);
	map->map_status = map_status;

    	/* 
     	 * process clear for the original lci 
     	 */
    	if (conf)
	     sendpacket(hwidb, lci, X25_CLEAR_CONFIRMATION, 0, NULL);
        x25_call_remove(lci, cause, diag);

	/*
	 * Enter a new address map for the new call
	 */

	if ((lci = x25_lci_enter(hwidb->firstsw, X25_NO_VC_ASSIGNED, NULL))) {
	    lci->lci_linktype = LINK_IP;
	    lci->lci_prec = prec;

	    if (x25_map_add_lci(lci, map)) {
		map->map_bfe = bfe;	/* Set the bfe for the address map */
		bcopy(hwidb->x25_address, &lci->lci_src_addr, sizeof(addrtype));
		bcopy(&map->map_address[X25_MAP_PRIMARY_ADDR_INX].addr,
		      &lci->lci_dst_addr, sizeof(addrtype));
		if (x25_sendcall(lci, NULL, 0))
		    return(TRUE);
	    }

	    /*
	     *  Call failed
	     */
	    x25_call_remove(lci, 0, 0);
	}

	if (map)
	    x25_map_delete(map, FALSE);

	hwidb->x25_callssentfailed++;
    }

    return(FALSE);
}

/*
 *
 * x25_process_atif 
 * Address Tranlation information is requested from
 * the BFE. Clear the call and Process another call
 * with the facility included.
 */
boolean x25_process_atif (
    hwidbtype *idb,
    lcitype *olci,
    uchar cause,
    uchar diag,
    boolean conf)
{
    bfetype *bfe;
    x25map *map = olci->lci_map;
    addrtype proto_addr;
    ipaddrtype h_ipaddr;

    bcopy(&map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr, &proto_addr,
	  sizeof(addrtype));
    h_ipaddr = proto_addr.ip_addr;

    /* check diag values */

    if (diag == X25_DIAG_BLACK_FAIL_ADDR_TRANS) {
	/* 
	 * check for Desicion or Always
	 *
	 */
	if (idb->x25_bfe_status & BFE_EMW_OPEN) {
	    /* For BFE Decision Emergency window must be open */
	    if (idb->x25_bfe_decision == BFE_DECISION_ASK) {
		    /*
		     * Router: Decision ask
		     * BFE :  Always
		     * Send Message out 
		     */
		    if (!(idb->x25_bfe_status & BFE_EMERG_MODE)) {
		    	if ((x25_debug || x25event_debug) &&
        	        	(x25_vcnum == 0 ||
				x25_vcnum == olci->lci_lci)) {
		        	buginf("\n%s: BFE is in Emergency mode but",
				    	idb->hw_namestring);
		        	buginf(" ROUTER is NOT in Emergency Mode,");
		        	buginf(" Use BFE EXEC command to Enter"); 
		    	}
		    } else {
	                /*
			 * In emergency so
			 * lookup host IP address in BFE table 
			 */
                       bfe = x25_bfe_lookup(idb, &h_ipaddr);
	               if (bfe != (bfetype *)NULL)
	                   return(x25_bfe_process_call(
		              idb, olci, bfe, cause, diag, conf));
		    }
	    } else if (idb->x25_bfe_decision == BFE_DECISION_YES || 
		   idb->x25_bfe_mode == BFE_ALWAYS) {
		    /*
		     * Router: Decision yes | always
		     * BFE :   Decsion
	             * lookup host IP address in BFE table
		     */
                    bfe = x25_bfe_lookup(idb, &h_ipaddr);
	            if (bfe != (bfetype *)NULL)
		        return(x25_bfe_process_call(
		           idb, olci, bfe, cause, diag, conf));
	    } 
	} else {
	       if (((idb->x25_bfe_mode == BFE_ALWAYS) ||
		  (idb->x25_bfe_decision == BFE_DECISION_YES)) &&
		  (idb->x25_bfe_status & BFE_EMERG_MODE)) {
	            /*
		     * Router : Always | Decision yes
		     * BFE :	Always 
	             * lookup host IP address in BFE table
		     */
                    bfe = x25_bfe_lookup(idb, &h_ipaddr);
	            if (bfe != (bfetype *)NULL)
	                return(x25_bfe_process_call(
		           idb, olci, bfe, cause, diag, conf));
	       } else if (idb->x25_bfe_decision == BFE_DECISION_ASK) {
		    /*
		     * Router: Decision ask
		     * BFE :  Always
		     * Send Message out 
		     */
		    if (!(idb->x25_bfe_status & BFE_EMERG_MODE)) {
		    	if ((x25_debug || x25event_debug) &&
        	        	(x25_vcnum == 0 ||
				x25_vcnum == olci->lci_lci)) {
		        	buginf("\n%s: BFE is in Emergency mode but",
				    	idb->hw_namestring);
		        	buginf(" ROUTER is NOT in Emergency Mode,");
		        	buginf(" Use BFE EXEC command to Enter"); 
		    	}
		    } else {
	                /*
			 * In emergency so
			 * lookup host IP address in BFE table 
			 */
                       bfe = x25_bfe_lookup(idb, &h_ipaddr);
	               if (bfe != (bfetype *)NULL)
	                   return(x25_bfe_process_call(
		              idb, olci, bfe, cause, diag, conf));
		    }
	       }
	}
    } 
    if (diag == X25_DIAG_BLACK_FAIL_NO_WIND &&
       (idb->x25_bfe_status & BFE_EMERG_MODE)) {
       /*
	* BFE must be in Decsion mode since
	* Emergency mode window is open
	*/
       if (idb->x25_bfe_decision == BFE_DECISION_ASK) {
	     /*
	      * Router: Decision ask
	      * BFE :   Decsion
	      * Send Message out 
	      */
	     if ((x25_debug || x25event_debug) &&
		 (x25_vcnum == 0 || x25_vcnum == olci->lci_lci)) {
		 buginf("\n%s: Router in Emergency mode but",
			 idb->hw_namestring);
		 buginf(" BFE is NOT in Emergency Mode,");
		 buginf(" Use BFE EXEC command to Leave");
	     }

       } else if (idb->x25_bfe_decision == BFE_DECISION_YES) {
	     /*
	      * 
	      * Router: Decision yes
	      * BFE :   Decsion
	      * Hmm. lookup host IP address in BFE table
	      * Enter BFE into emergency mode again.
	      */
	     idb->x25_bfe_status &= ~BFE_EMERG_MODE;
             bfe = x25_bfe_lookup(idb, &h_ipaddr);
	     if (bfe != (bfetype *)NULL)
	         return(x25_bfe_process_call(
		       idb, olci, bfe, cause, diag, conf));
       } else if (idb->x25_bfe_mode == BFE_ALWAYS) {
	     /*
	      * router : Always 
	      * BFE :  Decision
	      */
	      bfe = (bfetype *)NULL;
	      if (idb->x25_bfe_status & BFE_EMW_OPEN)
                   bfe = x25_bfe_lookup(idb, &h_ipaddr);
	      idb->x25_bfe_status &= ~BFE_EMERG_MODE;
	      return(x25_bfe_process_call(idb, olci, bfe,
		      cause, diag, conf));
       }
    } 
    return(FALSE);
}

/*
 * BFE exec command to enter and leave decision mode
 */
void bfe_command (parseinfo *csb)
{
    hwidbtype *idb;
    leveltype level;
  
    idb = GETOBJ(idb,1)->hwptr;
    if (idb->x25_bfe_decision != BFE_DECISION_ASK) {
      printf("x25 bfe-emergency not in decision mode\n");
      return;
    }
    switch (GETOBJ(int,1)) {
      case PARSER_BFE_ENTER:
  	level = raise_interrupt_level(NETS_DISABLE);
	idb->x25_bfe_status |= BFE_EMERG_MODE;
        reset_interrupt_level(level);
	break;
      case PARSER_BFE_LEAVE:
  	level = raise_interrupt_level(NETS_DISABLE);
	idb->x25_bfe_status &= ~BFE_EMERG_MODE;
        reset_interrupt_level(level);
	break;
    }
}

/*
* BFE  - hash pick routine.
* p-free avoiding... so walk the
* hash table by indexing.
*
*/
static bfetype *bfe_hashpick (
    bfetype *hp,
    int index)
{
    bfetype *ptr;

    for (ptr = hp; ptr; ptr = ptr->bfe_next)
         if (index-- == 0)
             break;
    return(ptr);
}

/*
 * Display information about BFE node and remote BFE addresses
 */

#define	LINECNT 20
static const char x25bfehdr[] = "Entry\t\tREMOTE-RED\t\tREMOTE-BLACK\t\tINTERFACE";

void show_x25bfe (void)
{
    int i, j;
    register bfetype *bfe;
    int entry = 0;

    automore_enable(x25bfehdr);
    for (i = 0; i < X25_BFE_HASH_SIZE; i++) {
	for (j = 0; (bfe = bfe_hashpick(x25_bfe_hash_table[i], j)); j++) {
	    mem_lock(bfe);
	    entry++;
	    printf("\n%-3d\t\t%i\t\t%i\t\t%s", entry,
		bfe->host_ipaddr, bfe->rbfe_ipaddr,
		bfe->idb->hw_namestring);
	    free(bfe);
	}
    }
    automore_disable();
}

void x25_bfe_nvgen (char *nv_command, hwidbtype *hwidb)
{
    int hash_bucket;
    int pick;
    bfetype *bfe;

    for (hash_bucket = 0; hash_bucket < X25_BFE_HASH_SIZE ; hash_bucket++) {
	for (pick = 0;
	     (bfe = bfe_hashpick(x25_bfe_hash_table[hash_bucket], pick));
	     pick++) {
	    if (hwidb != bfe->idb)
		continue;

	    nv_write(TRUE, "%s %i remote-black %i",
		     nv_command, bfe->host_ipaddr, bfe->rbfe_ipaddr);
	}
    }
}

/*
 * X25 bfe add and delete element
 * to the list.
 */
void x25_bfe_node (
    boolean add,
    hwidbtype *idb,
    ipaddrtype *h_add,
    ipaddrtype *r_add)
{

    if (!add) 
      x25_bfe_delete(idb, h_add);
    else
      x25_bfe_enter(idb, h_add, r_add);
}


/*
 * x25_bfe_diag process BFE diags
 * appropraitly.
 *
 */
void x25_bfe_diag (hwidbtype *idb, paktype *pak)
{

     uchar *diag;
     packettype *p = (packettype *)pak->datagramstart;

     if (!ISBFE(idb))
	return;

     diag =  (uchar *)p + X25HEADERBYTES;
     switch (*diag) {
	case X25_DIAG_BLACK_ENT_EMERG:
	     if (x25_debug || x25event_debug) 
		 buginf("\n%s: BFE Requested To Enter Emergency Mode",
		     idb->hw_namestring);
	     if (idb->x25_bfe_mode == BFE_ALWAYS) {
	     	 if (x25_debug || x25event_debug)
		     buginf("\n%s: Entering Emergency Mode Of Operation",
		         idb->hw_namestring);
	      	 idb->x25_bfe_status |= BFE_EMERG_MODE; 
	     } else {
		 if (idb->x25_bfe_decision == BFE_DECISION_YES) {
	     	     if (x25_debug || x25event_debug)
		         buginf("\n%s: Entering Emergency Mode Of Operation",
			     idb->hw_namestring);
		     idb->x25_bfe_status |= BFE_EMERG_MODE;
		 }
		 if ((idb->x25_bfe_decision == BFE_DECISION_ASK) &&
		    !(idb->x25_bfe_status & BFE_EMERG_MODE)) {
		     buginf("%s: Use BFE EXEC command to Enter",
			     idb->hw_namestring);
		 }
	     }
	     break;
	case X25_DIAG_BLACK_LEAVE_EMERG:
	     if (x25_debug || x25event_debug)
		 buginf("\n%s: BFE Requested to Leave Emergency Mode",
		     idb->hw_namestring);
	     if (idb->x25_bfe_mode == BFE_ALWAYS) {
	     	 if (x25_debug || x25event_debug)
		     buginf("\n%s: Leaving Emergency Mode Of Operation",
		         idb->hw_namestring);
	      	 idb->x25_bfe_status &= ~BFE_EMERG_MODE; 
	     } else {
		 if (idb->x25_bfe_decision == BFE_DECISION_YES) {
		     buginf("\n%s: Leaving Emergency Mode Of Operation",
			     idb->hw_namestring);
		     idb->x25_bfe_status &= ~BFE_EMERG_MODE;
		 }
		 if ((idb->x25_bfe_decision == BFE_DECISION_ASK) &&
		    (idb->x25_bfe_status & BFE_EMERG_MODE)) {
		     buginf("%s: Use BFE EXEC command to Leave",
			     idb->hw_namestring);
		 }
	     } 
	     break;
	case X25_DIAG_BLACK_EMERG_WINDOW_OP:
	     if (x25_debug || x25event_debug)
		 buginf("\n%s: BFE Requested to Open Emergency Window",
		    idb->hw_namestring);
	     if (idb->x25_bfe_mode != BFE_NEVER) {
	        if (idb->x25_bfe_status & BFE_EMW_OPEN) {
	      	     if (x25_debug || x25event_debug)
		         buginf("\n%s: Emergency Window already open",
		    	    idb->hw_namestring);
	     	} else {
	      	     if (x25_debug || x25event_debug)
		         buginf("\n%s: Opening Emergency Window",
		    	    idb->hw_namestring);
	      	     idb->x25_bfe_status |= BFE_EMW_OPEN; 
		}
	     }
	     break;
	default: /* no_op */
	     break;
     } /* diag */
}

