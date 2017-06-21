/* $Id: sna_la_pri.c,v 3.2.60.4 1996/08/07 08:59:40 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sna_la_pri.c,v $
 *------------------------------------------------------------------
 * sna_la_pri.c -- SNA Local LU Address Prioritization
 *
 * March 1992, Anson Chen
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sna_la_pri.c,v $
 * Revision 3.2.60.4  1996/08/07  08:59:40  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/06/28  23:06:40  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/17  11:21:26  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:54:34  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  14:34:47  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  20:12:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/13  01:23:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  09:42:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/01/24  21:49:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:24:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:48:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "ibm.h"
#include "sna_la_pri.h"
#include "../if/priority.h"


/*
 * Function declarations
 */
/* neither used here nor defined anywhere...
extern int name2priority(char *);
extern boolean pakhexval(char *, char *);
*/


/*
 * Local Storage
 */
struct la_pri_array_ {
    la_pri_type *plist;
    ushort pdefault;
} la_pri_array[SNA_LA_PRI_MAXLIST];

struct sap_pri_array_ {
    sap_pri_type *plist;
    ushort pdefault;
} sap_pri_array[SAP_PRI_MAXLIST];


/*
 * priority_init
 * Initialize the priority data structure
 */

void snalapri_init (void)
{
    int i;

    for (i = 0; i < SNA_LA_PRI_MAXLIST; i++) {
	la_pri_array[i].plist = NULL;
	la_pri_array[i].pdefault = PRIORITY_NORMAL;
    }

    for (i = 0; i < SAP_PRI_MAXLIST; i++) {
	sap_pri_array[i].plist = NULL;
	sap_pri_array[i].pdefault = PRIORITY_NORMAL;
    }

}

/*
 * fid2_pri_classify (for sdlc)
 * Return priority level for a SNA FID2 packet.
 * (Currently, only dependend LUs are supported by this feature.)
 */

int fid2_pri_classify (hwidbtype *output, uchar *fid2th, uchar *addr)
{
    la_pri_type *plist;

    for (plist = la_pri_array[output->sna_la_prilist].plist; plist; 
		    plist = plist->next) {

	/* the fid2 address priorirtization feature is only
	   supported for dependent LUs, which means the lu on the
	   host end is always zero or one */
	if (plist->sdlc == TRUE)
		if (*addr != plist->sdlc_addr)
			continue;
	if((fid2th[2] == 0) || (fid2th[2] == 1)) {
	    if(plist->lu_address[0] == fid2th[3])
		return(plist->priority);
        }
	else if((fid2th[3] == 0) || (fid2th[3] == 1)) {
	    if(plist->lu_address[0] == fid2th[2])
		return(plist->priority);
        }
     }
     return(PRIORITY_NORMAL);
}


/*
 * general_fid2_pri_classify()
 */

int general_fid2_pri_classify (hwidbtype *output, uchar *dmac, uchar *smac,
                               uchar *dsap, uchar *ssap, uchar *fid2th)
{
    la_pri_type *plist;

    for (plist = la_pri_array[output->sna_la_prilist].plist; plist;
         plist = plist->next) {
        /*
         * fid2 address priorirtization feature is only
         * supported for dependent LUs, which means the lu on the
         * host end is always zero or one.
         */
        if (plist->param_mask & DMAC_MASK) {
            if (!ieee_equal(dmac, plist->dmac))
                continue;
        }
        if (plist->param_mask & SMAC_MASK) {
           if (((smac[0] & ~TR_RII) != plist->smac[0]) ||
               (bcmp(&smac[1], &plist->smac[1], 5)))
               continue;
        }
        if (plist->param_mask & DSAP_MASK) {
            if (*dsap != plist->dsap)
                continue;
        }
        if (plist->param_mask & SSAP_MASK) {
            if (*ssap != plist->ssap)
                continue;
        }
        if ((fid2th[2] == 0) || (fid2th[2] == 1)) {
            if (plist->lu_address[0] == fid2th[3])
                return(plist->priority);
        } else if((fid2th[3] == 0) || (fid2th[3] == 1)) {
            if (plist->lu_address[0] == fid2th[2])
                return(plist->priority);
        }
    }
    return(PRIORITY_NORMAL);
}

/*
 * rsrb_fid2_pri_classify
 * Return priority level for a SNA FID2 packet.
 * (Currently, only dependend LUs are supported by this feature.)
 */

int rsrb_fid2_pri_classify (
    hwidbtype *output,
    uchar *fid2th,
    tring_hdr *trh,
    sap_hdr *llc1)
{
	la_pri_type *plist;

	for (plist = la_pri_array[output->sna_la_prilist].plist; plist;
			plist = plist->next) {

	/* the fid2 address priorirtization feature is only
		supported for dependent LUs, which means the lu on the
		host end is always zero or one */
	if(plist->param_mask & DMAC_MASK) {
		if(!ieee_equal(trh->daddr, plist->dmac))
		    continue;
	}
	if(plist->param_mask & DSAP_MASK) {
		if(llc1->dsap != plist->dsap)
			continue;
	}
	if((fid2th[2] == 0) || (fid2th[2] == 1)) {
		if(plist->lu_address[0] == fid2th[3])
		return(plist->priority);
	}
	else if((fid2th[3] == 0) || (fid2th[3] == 1)) {
		if(plist->lu_address[0] == fid2th[2])
		return(plist->priority);
		}
	}
	return(PRIORITY_NORMAL);
}

#define LP_NTYPES 5

#ifdef NOBODY_CALLS_ME
char *const lp_keywords[LP_NTYPES] = {"DSAP", "DMAC", "SDLC", "SSAP", "SMAC"};
#endif /* NOBODY_CALLS_ME */

#define LP_DSAP_KEYWORD 0
#define LP_DMAC_KEYWORD 1
#define LP_SDLC_KEYWORD 2
#define LP_SSAP_KEYWORD 3
#define LP_SMAC_KEYWORD 4

/*
 * locaddr_pri_list_command
 * Set up an item in a local address prioritization list
 */

void locaddr_pri_list_command (parseinfo *csb)
{
    int number, priority;
    uchar lu_address;
    la_pri_type *plist, *pred;
	uchar param_mask, dsap, dmac[IEEEBYTES], sdlc_addr;
	uchar sdlc = FALSE;
	uchar dmac_dsap = FALSE;

    
    if (csb->nvgen) {
	char buff[64];
	int i;
	
	for (i = 0; i < SNA_LA_PRI_MAXLIST; i++) {
		if ((plist = la_pri_array[i].plist) == NULL)
			continue;
	if (plist) {
	    for (; plist; plist = plist->next) {
		nv_write(!plist->interface, "locaddr-priority-list %d", i); 
		sprintf(buff, "%x", plist->lu_address[0]);
		nv_add(TRUE, " %s", buff);
		nv_add(TRUE, " %s", priority2name(plist->priority));
		if(plist->sdlc == TRUE) {
			sprintf(buff, "%x", plist->sdlc_addr);
			nv_add(TRUE," sdlc %s", buff);
			continue;
		}
		if(plist->param_mask & DMAC_MASK) {
			sprintf(buff, "%e", plist->dmac);
			nv_add(TRUE," dmac %s", buff);
		}
		if(plist->param_mask & DSAP_MASK) {
			sprintf(buff, "%x", plist->dsap);
			nv_add(TRUE, " dsap %s", buff);
		}
	    }
	}
	}
	return;
    }

    
    /*
     * Get the priority list number
     */
    number = GETOBJ(int,1);

    /*
     * Get lu address
     */
    lu_address = (uchar)GETOBJ(int,2);
		
    /*
     * Get priority type
     */
    priority = GETOBJ(int,3);

	dsap = 0;
	ieee_zero(dmac);
	param_mask = 0;
	sdlc_addr = 0;

	/* see if the dmac/dsap keyword is present */
	if ((GETOBJ(int, 4) == TRUE) || (GETOBJ(int, 5) == TRUE)) {
		if ((GETOBJ(int, 4) == TRUE)) {
			ieee_copy(GETOBJ(hwaddr, 1)->addr, dmac);
			param_mask |= DMAC_MASK;
		}
		if ((GETOBJ(int, 5) == TRUE)) {
			dsap = GETOBJ(int, 6);
			param_mask |= DSAP_MASK;
		}
		dmac_dsap = TRUE;
	}
	if ((GETOBJ(int, 7) == TRUE)) {
		if (dmac_dsap == TRUE) {
			printf("\nConflict between priority list keywords");
			return;
		}
		sdlc_addr = GETOBJ(int, 8);
		sdlc = TRUE;
	}

    /*
     * Search the sna lu address prioritization array for an item that
     * matches on lu address.
     * If we find an item, update or delete it and return.
     * "pred" is the pointer to the last item in the list.
     */
    pred = NULL;
    for (plist = la_pri_array[number].plist; plist;
	 pred = plist, plist = plist->next) {
	 if (plist->lu_address[0] == lu_address) {
		if(((sdlc == TRUE) && (sdlc_addr == plist->sdlc_addr)) ||
		((param_mask & DMAC_MASK) && (ieee_equal(plist->dmac,dmac))) ||
		((param_mask & DSAP_MASK) && (dsap == plist->dsap)) ||
		((dmac_dsap == FALSE) && (sdlc == FALSE))) {
            if(plist->priority == priority) {
		if (!csb->sense) {
		    if (pred) {
			pred->next = plist->next;
		    } else {
			la_pri_array[number].plist = plist->next;
		    }
		    free(plist);
			return;
		}
	    } else { /* allow user to override an existing priority */
		if (csb->sense) {
		    plist->priority = priority;
		}
	    }
            return;
	}	    
    }
	}

    /*
     * Item not in list.  Create it at end of list.
     */
    if (!csb->sense) {
	return;
    }
    plist = malloc(sizeof(la_pri_type));
    if (!plist) {
	printf(nomemory);
	return;
    }

    plist->lu_address[0] = lu_address;
    plist->priority = priority;
	/* check to see if user has any option */
	if (dmac_dsap == TRUE) {
		if (param_mask & DMAC_MASK)
			ieee_copy(&dmac[0], &plist->dmac[0]);
		else
			ieee_zero(plist->dmac);
		if (param_mask & DSAP_MASK)
			plist->dsap = dsap;
		else
			plist->dsap = 0;
		plist->param_mask = param_mask;
	}
	if (sdlc == TRUE) {
		plist->sdlc_addr = sdlc_addr;
		plist->sdlc = TRUE;
	}
	else
		plist->sdlc = FALSE;


    if (pred) {
	pred->next = plist;
    } else {
	la_pri_array[number].plist = plist;
    }
}

/*
 * locaddr_pri_command
 * Set up a sna lu address priority list for an interface.
 */

void locaddr_pri_command (parseinfo *csb)
{
    hwidbtype *idb;

    idb = csb->interface->hwptr;
    if (csb->nvgen) {
	nv_write(idb->sna_la_prilist, "%s %d", csb->nv_command,
		 idb->sna_la_prilist);
	return;
    }

    if (is_bstun(idb)) {
       printf ("\nSNA LU address prioritization not supported with BSTUN.");
       return;
    }

    if (csb->sense) {
	idb->sna_la_prilist = GETOBJ(int,1);
    } else {
	idb->sna_la_prilist = 0;
    }
}


/*
 * ------------------------------------------------------------------------- 
 *
 * RSRP SAP prioritization 
 *
 */

/*
 * sap_pri_classify
 * Return priority level for a MAC/SAP combination.
 *
 */

int sap_pri_classify (hwidbtype *output, tring_hdr *trh, sap_hdr *llc1)
{
    sap_pri_type *plist;

    for (plist = sap_pri_array[output->sap_prilist].plist; plist; 
		    plist = plist->next) {
        if(plist->param_mask & DMAC_MASK) {
	    if(!ieee_equal(trh->daddr, plist->dmac))
	       continue;
        } 
        if(plist->param_mask & SMAC_MASK) {
            if(((trh->saddr[0] & ~TR_RII) != plist->smac[0]) ||
	          bcmp(&trh->saddr[1], &plist->smac[1], 5))
	       continue;
        }
	if(plist->param_mask & SSAP_MASK) {
	    if(llc1->ssap != plist->ssap)
	       continue;
        }
	if(plist->param_mask & DSAP_MASK) {
	    if(llc1->dsap != plist->dsap)
	       continue;
        }
	return(plist->priority);
     }
     return(PRIORITY_NORMAL);
}


/*
 * general_sap_pri_classify
 * Return priority level for a MAC/SAP combination.
 *
 */

int general_sap_pri_classify (hwidbtype *output, uchar *dmac,
                              uchar *smac, uchar *dsap, uchar *ssap)
{
    sap_pri_type *plist;

    for (plist = sap_pri_array[output->sap_prilist].plist; plist;
                    plist = plist->next) {
        if (plist->param_mask & DMAC_MASK) {
            if (!ieee_equal(dmac, plist->dmac))
                continue;
        }
        if (plist->param_mask & SMAC_MASK) {
            if (((smac[0] & ~TR_RII) != plist->smac[0]) ||
                (bcmp(&smac[1], &plist->smac[1], 5)))
                continue;
        }
        if (plist->param_mask & SSAP_MASK) {
            if (*ssap != plist->ssap)
                continue;
        }
        if (plist->param_mask & DSAP_MASK) {
            if (*dsap != plist->dsap)
                continue;
        }
        return(plist->priority);
     }
     return(PRIORITY_NORMAL);
}


/*
 * sap_pri_list_command
 * Set up an item in a local address prioritization list
 */

void sap_pri_list_command (parseinfo *csb)
{
    int number, priority;
    uchar dsap, ssap, smac[IEEEBYTES], dmac[IEEEBYTES];
    uchar param_mask;
    sap_pri_type *plist, *pred;
    
	if (csb->nvgen) {
    int i ;
    char buff[MAX_PEER_NAME];

	
	for (i = 0; i < SAP_PRI_MAXLIST; i++) {
		if ((plist = sap_pri_array[i].plist) == NULL)
			continue;
	for (; plist; plist = plist->next){
	     nv_write(!plist->interface, "sap-priority-list %d", i); 
	     nv_add(TRUE, " %s", priority2name(plist->priority));
         nv_add((plist->param_mask & SMAC_MASK)," smac %e", plist->smac);
         nv_add((plist->param_mask & DMAC_MASK)," dmac %e", plist->dmac);
         if(plist->param_mask & SSAP_MASK) {
			sprintf(buff, "%x", plist->ssap);
            nv_add(TRUE, " ssap %s", buff);
         }
         if(plist->param_mask & DSAP_MASK) {
			sprintf(buff, "%x", plist->dsap);
            nv_add(TRUE, " dsap %s", buff);
         }
	}
	}
	return;
	}
		
    /*
     * Get the priority list number
     */
	number = GETOBJ(int,1);

    /* initialize local variables */
    dsap = ssap = 0;
    ieee_zero(smac);
    ieee_zero(dmac);
    param_mask = 0;

    /*
     * Get priority type
     */
    priority = GETOBJ(int,2);
    
	if (GETOBJ(int,8)) {
		ssap = GETOBJ(int,4);
		param_mask |= SSAP_MASK;
	}
	if (GETOBJ(int,7)) {
		dsap = GETOBJ(int,3);
		param_mask |= DSAP_MASK;
	}
	if (GETOBJ(int,5)) {
		ieee_copy(GETOBJ(hwaddr,1)->addr, dmac);
	    param_mask |= DMAC_MASK;
	}
	if (GETOBJ(int,6)) {
		ieee_copy(GETOBJ(hwaddr,2)->addr, smac);
	    param_mask |= SMAC_MASK;
	}
    /*
     * Search the sna lu address prioritization array for an item that
     * matches on lu address.
     * If we find an item, update or delete it and return.
     * "pred" is the pointer to the last item in the list.
     */
    pred = NULL;
    for (plist = sap_pri_array[number].plist; plist;
	 pred = plist, plist = plist->next) {
	if ((ieee_equal(plist->dmac, dmac)) &&
	    (ieee_equal(plist->smac, smac)) &&
	    (plist->ssap == ssap) && (plist->dsap == dsap)) {
            if(plist->priority == priority) {
	       if (!csb->sense) {
		   if (pred)
		       pred->next = plist->next;
		   else
		       sap_pri_array[number].plist = plist->next;
		   free(plist);
	       }
	    }
	    else  /* allow user to override an existing priority */
	       if(csb->sense) {
	           plist->priority = priority;
		   plist->param_mask = param_mask;
               }
            return;

	}
    }

    /*
     * Item not in list.  Create it at end of list.
     */
    if (!csb->sense)
	return;
    plist = malloc(sizeof(sap_pri_type));
    if (!plist) {
	printf(nomemory);
	return;
    }

    plist->ssap = ssap;
    plist->dsap = dsap;
    ieee_copy(smac, plist->smac);
    ieee_copy(dmac, plist->dmac);
    plist->param_mask = param_mask;
    plist->priority = priority;

    if (pred)
	pred->next = plist;
    else
	sap_pri_array[number].plist = plist;
}

/*
 * sap_pri_command
 * Set up a sap priority list for an interface.
 */

void sap_pri_command (parseinfo *csb)
{
    hwidbtype *idb;

    idb = csb->interface->hwptr;
    if (csb->nvgen) {
        nv_write(idb->sap_prilist, "%s %d", csb->nv_command,
        idb->sap_prilist);
        return;
    }
    if (!csb->sense) {
	idb->sap_prilist = 0;
    } else {
	    idb->sap_prilist = GETOBJ(int,1);
	}
}
