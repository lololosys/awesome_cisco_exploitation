/* $Id: ilmi_ui.c,v 3.1.62.7 1996/06/18 06:31:02 kartik Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_ui.c,v $
 *------------------------------------------------------------------
 * ILMI Related UI.
 *
 * October 1995, Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_ui.c,v $
 * Revision 3.1.62.7  1996/06/18  06:31:02  kartik
 * Branch: California_branch
 * CSCdi50421:  show atm ilmi command returns error when no interface
 * provided
 *              Added more details to show.
 *
 * Revision 3.1.62.6  1996/06/09  06:05:49  achopra
 * CSCdi51530:  ATM ARP client misregisters if no address is configured
 * Branch: California_branch
 *
 * Revision 3.1.62.5  1996/06/07  22:02:52  schrupp
 * CSCdi56175:  concurrent use of LANE and RFC1577, RFC1577 is not working
 * Branch: California_branch
 *
 * Revision 3.1.62.4  1996/06/07  17:11:04  rzagst
 * CSCdi56880:  Cannot create SVC connection on pt-to-pt subinterface
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/05/24  21:08:47  schrupp
 * CSCdi58635:  ATM: ESI cleanup
 * Branch: California_branch
 *
 *  - Deregister addresses when a new prefix is added by ILMI
 *  - Deregister addresses when ESI/NSAP is changed from command line
 *  - Reregister addresses even if the new prefix is the same as the old
 *  - Deregister addresses when subinterface is shut
 *  - Reregister addresses when subinterface is no shutD
 *  - Don't allow qsaal and ilmi pvc's on subinterfaces
 *  - Init atm_db->peeruni_ifname
 *
 * Revision 3.1.62.2  1996/05/12  19:49:28  rzagst
 * CSCdi55659:  ATMSIG: IPoverATM NSAP sub-intrfcs can not co-exist with
 * ESI sub-int
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/04/25  23:02:51  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/30  03:35:12  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1  1995/11/09  11:00:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *

 * Revision 2.1  1995/10/17  20:26:52  kartik
 * Placeholder for Rhino Branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include "types.h"
#include "address.h"
#include "media_registry.h"
#include "sys_registry.h"
#include "stdarg.h"
#include "interface_private.h"
#include "../if/atm.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../parser/parser_defs_exec.h"
#include "../atm/parser_defs_ilmi.h"
#include "../if/atm.h"
#include "../if/atm_debug.h"
#include "ilmi_public.h"
#include "ilmi_private.h"
#include "ilmi_api.h"
#include "../lane/lane.h"
#include "atmsig_api.h"
#include "atmsig_private.h"
#include "atmsig_smap.h"
#include "atm_registry.h"

/* ------------------------------------------------------------------
 * Show ILMI information for the specified interface.
 *
 */

void ilmi_show_commands (parseinfo *csb)
{
    hwidbtype *idb;
    idbtype *sidb;
    
    switch (csb->which) {
    case ILMI_SHOW_ATM_IF_ILMI:
	sidb = GETOBJ(idb,1);
	if (sidb) {
	    idb = sidb->hwptr;
	     if(!is_atm(idb)) {
		 printf("\nInterface %s is not an ATM interface",
			idb->hw_namestring);
		 return;
	     }
	} else {
	    idb = NULL;
	}
	reg_invoke_atm_ilmi_show_command(idb);
	break;

    default:
	/* invalid option */
	break;
   }
}
void
ilmi_intf_config_commands (parseinfo *csb)
{
    idbtype *idb, *tmp_idb;
    hwidbtype *hwidb;
    int interval;
    atm_db_t *atm;
    char esistr[MAX_ATM_NSAP_ASCII_LEN];
    atmSig_nsap_info *nsap_ptr;
    hwaddrtype *hwconf_tmp;

    idb = csb->interface;
    if (idb) {
	hwidb = idb->hwptr;
    } else {
	printf("\nerror: source idb not set");
	return;
    }
    atm = hwidb->atm_db;

/*
 * csb->nvgen == TRUE when the 'write [term || mem || net]' command is
 * entered.  Otherwise, it's false to indicate that a command is being
 * specified by the user.
 */
    if (csb->nvgen) {
	switch(csb->which) {
	case ILMI_IF_DISABLE_ENTERPRISE_TRAPS:
	    nv_write(atm->disable_ilmi_traps == TRUE, "%s", csb->nv_command);
	    break;
		    
	case ILMI_IF_CONFIG_ADDRESS_REG:
	    nv_write(atm->address_reg == FALSE, "no %s", csb->nv_command);
	    break;
	case ILMI_IF_CONFIG_KEEPALIVE:
	    nv_write(atm->ilmipoll_interval != ATM_ILMIPOLL_DEFAULT, "%s %d",
		     csb->nv_command, atm->ilmipoll_interval / ONESEC);
	    break;
        case ILMI_IF_CONFIG_ESI:
	    if (idb->nsap != NULL) {
	      /* Search through nsap list
	       * Nvgen only if ESI generated
	       * Match esi in case LANE has something registered
	       * on this idb.
	       */
	      for (nsap_ptr = atm->nsap_list_head;nsap_ptr != NULL;
		   nsap_ptr = nsap_ptr->next) {

		if ( (nsap_ptr->idb == idb) &&
		     (nsap_ptr->useIlmiPrefix) &&
		     (bcmp(&idb->nsap->addr[ILMI_ESI_OFFSET], 
			   &nsap_ptr->nsap.addr[ILMI_ESI_OFFSET], 
			   ILMI_ESI_SIZE) == 0) ) {

		  atm_printesi( &(nsap_ptr->nsap.addr[ILMI_ESI_OFFSET]), 
			       esistr);
		  nv_write( !atmSig_nullEsi
			   (&nsap_ptr->nsap.addr[ILMI_ESI_OFFSET]),
			   "%s %s", csb->nv_command, esistr );
		  break;
		}
	      }
	    }
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
/*
 * csb->nvgen == FALSE.  Go process user specific command
 */
    switch(csb->which) {
    case ILMI_IF_DISABLE_ENTERPRISE_TRAPS:
	if (csb->sense)
	    atm->disable_ilmi_traps = TRUE;
	else
	    atm->disable_ilmi_traps = FALSE;
	break;
		
    case ILMI_IF_CONFIG_ADDRESS_REG:
	if (csb->sense) 
	    atm->address_reg = TRUE;
	else 
	    atm->address_reg = FALSE;
	break;
    case ILMI_IF_CONFIG_KEEPALIVE:
	if (csb->sense) {
            interval = GETOBJ(int,1) * ONESEC;
            if(!interval) {
		atm->ilmipoll_interval = 0;
                reg_invoke_atm_ilmi_destroy_event(hwidb->hw_if_index,
						  ilmiPeriodicPoll,0);
	    }
            else {
                interval = ((interval < ATM_ILMIPOLL_MIN) ?
			    ATM_ILMIPOLL_MIN : interval);
                atm->ilmipoll_interval = interval;
                reg_invoke_atm_ilmi_create_event(hwidb->hw_if_index,
						 ilmiPeriodicPoll,interval,0);
	    }	
	}
	else {
	    atm->ilmipoll_interval = 0;
	    reg_invoke_atm_ilmi_destroy_event(hwidb->hw_if_index,
					      ilmiPeriodicPoll,0);
	}
	break;

    case ILMI_IF_CONFIG_ESI:
	{
	  if (!csb->sense) {

	    /* Validate NSAP */
	    if (idb->nsap == NULL) {
	      printf("\n%%ATM no esi to delete");
	      return;
	    }

	    /* Don't remove this NSAP if generated from LANE */
	    if (is_atm_lane(idb)) {
	      printf("\n%%ATM NSAP command conflicts with LANE client.");
	      return;
	    }

	    /* Don't remove this NSAP unless generated from ESI command */
	    hwconf_tmp = GETOBJ(hwaddr,1);
	    for (nsap_ptr = atm->nsap_list_head; nsap_ptr != NULL;
		 nsap_ptr = nsap_ptr->next) {

	      if ( (nsap_ptr->idb == idb) &&
		   (nsap_ptr->useIlmiPrefix) &&
		   (bcmp(&idb->nsap->addr[ILMI_ESI_OFFSET], 
			 &nsap_ptr->nsap.addr[ILMI_ESI_OFFSET], 
			 ILMI_ESI_SIZE) == 0) ) {

		/* Cancel NSAP */
		if (nsap_ptr->ilmiAddrRegistered) {
		  atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressInValid);
		}
		atmSmap_cancelAtmAddress(idb);
		reg_invoke_atm_addr_change(idb, csb->sense);
		free(idb->nsap);
		idb->nsap = NULL; 
		return;
	      }
	    }
	     
	    /* ESI not deleted */
	    printf("\n%%ATM esi not deleted");
	    return;

	  }

	  /* csb->sense = TRUE */
	  /* verify ilmi active */
	  if (!idb->hwptr->atm_db->address_reg) {
	    printf("\n%%ILMI: address registration NOT active");
	    return;
	  }

	  if (is_atm_lane(idb)) {
	    printf("\n%%ATM NSAP command conflicts with LANE client.");
	    return;
	  }

	  if (is_p2p(idb)) {
	      printf ("\nATM: cannot configure ESI/NSAP on point-to-point"
		      " sub-interface.");
	      return;
	  }
 	  
    
	  /* Don't allow ESI if already configured on this or 
	   * or any other interface
	   */
	  hwconf_tmp = GETOBJ(hwaddr,1);
	  FOR_ALL_HWIDBS(hwidb) {
	    if (!hwidb->status & IDB_ATM)
	      continue;
	    FOR_ALL_SWIDBS_ON_HW(hwidb, tmp_idb) {
	      if (tmp_idb->nsap) {
		if ( bcmp(&tmp_idb->nsap->addr[ILMI_ESI_OFFSET], 
			  &hwconf_tmp->addr[ILMI_ESI_OFFSET], 
			  ILMI_ESI_SIZE) == 0 ) {
		  printf("\n%%ATM:esi already configured on %s", 
			   tmp_idb->namestring);
		  return;
		}
	      }
	    }
	  }


	  /* Cancel current NSAP if it exists */
	  if (idb->nsap != NULL) {

	    /* Find nsap_ptr, if from ESI, dereg */
	    for (nsap_ptr = atm->nsap_list_head;nsap_ptr != NULL;
		 nsap_ptr = nsap_ptr->next) {

	      if ( (nsap_ptr->idb == idb) &&
		   (bcmp(idb->nsap->addr, nsap_ptr->nsap.addr, 
			 STATIONLEN_ATMNSAP) == 0) ) {
		break;
	      }
	    }

	    if (nsap_ptr) {
	      if (!atmSig_nullNsap(idb->nsap->addr) ) {
		if (atm_event_debug)
		  printf("\n%%ATM esi removing nsap");
		if ( (nsap_ptr->useIlmiPrefix) && 
		    (nsap_ptr->ilmiAddrRegistered) ) {
		  atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressInValid);
		}
		atmSmap_cancelAtmAddress(idb);
	      }
	    }
	  }

	  /* Enter new ESI */
	  if (idb->nsap == NULL) {
	    idb->nsap = malloc( sizeof(hwaddrtype) );
	    if (idb->nsap == NULL) {
	      return;
	    }
	  }
	  bzero(idb->nsap, sizeof(hwaddrtype));
	  bcopy(GETOBJ(hwaddr,1), idb->nsap, sizeof(hwaddrtype)); 
	  bzero(idb->nsap->addr, ILMI_PREFIX_SIZE);
	  idb->nsap->type = STATION_ATMNSAP;
	  idb->nsap->length = STATIONLEN_ATMNSAP;
	  if ( atmSig_nullNsap(idb->nsap->addr) ) {
	    printf("\n%%ATM: null esi address not configured");
	    free(idb->nsap);
	    idb->nsap = NULL; 
	    break;
	  }

	  atmSmap_fillEsiInNsapInfo(idb, atm);
	  reg_invoke_atm_addr_change(idb, csb->sense);
	  break;
	}

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


