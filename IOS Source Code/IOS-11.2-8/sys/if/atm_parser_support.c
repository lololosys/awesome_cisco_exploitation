/* $Id: atm_parser_support.c,v 3.1.4.18 1996/09/11 17:38:57 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/atm_parser_support.c,v $
 *------------------------------------------------------------------
 * atm_parser_support.c: support for platform indpendent parser
 *                       commands
 *
 * April 1996, Rob Zagst
 *
 * Copyright (c) 1996 - 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_parser_support.c,v $
 * Revision 3.1.4.18  1996/09/11  17:38:57  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.1.4.17  1996/09/08  03:03:27  rzagst
 * CSCdi62883:  debug atm packet shows incorrect message for oam cells
 * Branch: California_branch
 * Prevent displaying of oam cells if debug atm packet is running, create
 * a separate atm_debug_swidb to hold the oam interface, and add a flag
 * to indicate whether inarp is running or not on the particular vc.
 *
 * Revision 3.1.4.16  1996/08/08  02:30:34  anna
 * CSCdi64538:  ATM: rfc1483 some (vpi,vci) are not working properly
 * Branch: California_branch
 * Prevent VCI 3 and 4 from being configured for PVCs.
 *
 * Revision 3.1.4.15  1996/08/05  21:33:20  jwjang
 * CSCdi65054:  atm uni-version command disappear after reboot
 * Branch: California_branch
 *
 * Revision 3.1.4.14  1996/06/18  00:36:33  rzagst
 * CSCdi60123:  VC-per-VP command should only reject new value for PVCs
 * Branch: California_branch
 *
 * Revision 3.1.4.13  1996/06/09  06:05:54  achopra
 * CSCdi51530:  ATM ARP client misregisters if no address is configured
 * Branch: California_branch
 *
 * Revision 3.1.4.12  1996/06/07  22:01:15  schrupp
 * CSCdi56175:  concurrent use of LANE and RFC1577, RFC1577 is not working
 * Branch: California_branch
 *
 * Revision 3.1.4.11  1996/06/07  17:11:14  rzagst
 * CSCdi56880:  Cannot create SVC connection on pt-to-pt subinterface
 * Branch: California_branch
 *
 * Revision 3.1.4.10  1996/06/04  00:17:44  rzagst
 * CSCdi59164:  command show atm map not accurate
 * Branch: California_branch
 *
 * Revision 3.1.4.9  1996/05/31  15:30:55  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.1.4.8  1996/05/24  21:09:47  schrupp
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
 * Revision 3.1.4.7  1996/05/21  16:39:48  jwjang
 * CSCdi57875:  show int atm display wrong uni version
 *              added ATMUNI_VERSION_DEFAULT == ATMUNI_VERSION_30
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/05/13  18:41:15  rzagst
 * CSCdi57247:  Burst size handled improperly between AIP and 4500
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/05/12  19:49:36  rzagst
 * CSCdi55659:  ATMSIG: IPoverATM NSAP sub-intrfcs can not co-exist with
 * ESI sub-int
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/05/11  19:34:31  jwjang
 * CSCdi57376:  sscop keepalive timer is 30 sec in uni 3.1 sometimes
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/05/09  14:29:42  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.4.1.2.3  1996/05/04  01:34:16  jwjang
 * CSCdi56712:  atm uni-version cmd is allowed while interface is up
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.4.1.2.2  1996/05/01  17:25:52  jwjang
 * CSCdi56347:  add atm uni-version interface command
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.4.1.2.1  1996/04/27  07:03:33  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.4.2  1996/05/06  16:58:24  dino
 * CSCdi56685:  BT wants an easier way to find multipoint VCs in
 *              "show atm vc"
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:12:58  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/04/23  01:55:34  rzagst
 * Fix spurious alignment problem.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1.2.6  1996/04/21  21:15:51  rzagst
 * ATM Modularity: Code review action item: change config.vc to more
 * meaningful variable name.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1.2.5  1996/04/13  01:27:50  rzagst
 * ATM Modularity: Place atm arp in a separate subsystem.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1.2.4  1996/04/11  17:17:26  lmercili
 * Code review comments.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1.2.3  1996/04/10  18:27:24  rzagst
 * ATM Modularity: Update atm vc-per-vp command per code review action
 * items.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1.2.2  1996/04/05  19:32:51  rzagst
 * ATM Modularity: Move commands to separate file.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1.2.1  1996/04/05  17:11:34  rzagst
 * ATM Modularity: Complete code review action items
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1  1996/04/05  01:26:14  rzagst
 * Placeholder file for ATM Modularity: Move atm parser command support
 * code
 * to separate file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "ether.h"
#include "config.h"
#include "parser.h"
#include "../ui/parse_util.h"
#include "../parser/parser_defs_exec.h"
#include "../atm/parser_defs_atm.h"
#include "../lane/parser_defs_lane.h"
#include "atm_debug.h"
#include "atm.h"
#include "static_map.h"
#include "../atm/atm_oam.h"
#include "../lane/lane.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_private.h"
#include "../atm/atmsig_smap.h"
#include "../wan/smds.h"
#include "../atm/ilmi_public.h"
#include "../atm/atm_registry.h"

/*
 * atm_interface_commands:	Called to do all ATM command processing.
 * OUTPUT/RETURN:
 *	None.
 * INPUT:
 *	csb = Ptr. to the Parse block with command & arguments.
 *	csb->which = ATM interface Command type
 *	csb->sense = [NO] value(0 = NO), (1 = !NO)
 */
void
atm_interface_commands (parseinfo *csb)
{
    idbtype *idb, *tmp_idb;
    hwidbtype *hwidb;
    atm_db_t *atm;
    int protocol = 0, flags = 0;
    vcd_t vcnum;
    vc_info_t *vc;
    hwaddrtype atm_addr, *hwconf_tmp;
    char nsapstr[MAX_ATM_NSAP_ASCII_LEN];
    user_vc_type_t user_vc_type;
    atmSig_nsap_info *nsap_ptr;

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
	case ATM_CMD_PVC:
	    for(vcnum = 1; vcnum < atm->config.max_intf_vcs; vcnum++) {
		if((vc = atm_getvc_all(atm, vcnum))) {
		    if(IS_ATMVC_SVC(vc) ||
		       (vc->flags & VC_FLAG_TEMP))
			continue;
		    if(vc->swidb != idb)	/* Only do this SWIDB */
			continue;
		    nv_write(TRUE, "%s %d %d %d ", csb->nv_command,
			     vcnum, vc->vpi, vc->vci);
		    switch(vc->flags & VC_FLAG_AAL_MASK) {
		    case ATM_VC_AAL5SNAP:
			nv_add(TRUE, "aal5snap");
			break;
		    case ATM_VC_AAL5NLPID:
			nv_add(TRUE, "aal5nlpid");
			break;
		    case ATM_VC_AAL5FRNLPID:
			nv_add(TRUE, "aal5frnlpid");
			break;
                    case ATM_VC_AALQSAAL:
                        nv_add(TRUE, "qsaal");
                        break;
		    case ATM_VC_AAL5MUX:
			switch (vc->etype) {
			case TYPE_IP10MB:
			    nv_add(TRUE, "aal5mux ip");
			    break;
			case TYPE_DECNET:
			    nv_add(TRUE, "aal5mux decnet");
			    break;
			case TYPE_CLNS:
			    nv_add(TRUE, "aal5mux clns");
			    break;
			case TYPE_CHAOS:
			    nv_add(TRUE, "aal5mux chaos");
			    break;
			case TYPE_ETHERTALK:
			    nv_add(TRUE, "aal5mux appletalk");
			    break;
			case TYPE_NOVELL1:
			    nv_add(TRUE, "aal5mux novell");
			    break;
			case TYPE_VINES:
			    nv_add(TRUE, "aal5mux vines");
			    break;
			case TYPE_APOLLO:
			    nv_add(TRUE, "aal5mux apollo");
			    break;
                        case TYPE_XNS:
                            nv_add(TRUE, "aal5mux xns");
                            break;
			}
			break;
		    case ATM_VC_AAL5ILMI:
			nv_add(TRUE, "ilmi");
			break;
		    case ATM_VC_AAL34SMDS:
			nv_add(TRUE, "aal34smds");
			break;
		    }
		    nv_add(vc->mid_start || vc->mid_end, " %d", vc->mid_start);
		    nv_add(vc->mid_end, " %d", vc->mid_end);
                    nv_add(!(vc->flags & VC_FLAG_DEFAULT_RATE), " %d",
			   vc->peakrate);
                    nv_add(!(vc->flags & VC_FLAG_DEFAULT_RATE), " %d",
			   vc->avgrate);
		    if ((vc->flags & VC_FLAG_DEFAULT_BURST) == 0) {
			ushort cellq;
			cellq = vc->burstcells / atm->burstcells_factor;
			if (vc->burstcells % atm->burstcells_factor != 0) {
			    cellq++;
			}
			nv_add(TRUE," %d",cellq);
		    }
                    if(vc->inarp) {
			nv_add(TRUE, " inarp");
			if ((vc->inarp/ONEMIN) != ATM_VC_INARP) {
			    nv_add(TRUE, " %d", (vc->inarp/ONEMIN));
			}
		    }
		    nv_add(vc->oam > 0, " oam %d", vc->oam);
		}
	    }
	    break;
	case ATM_CMD_MTP_SIG:
	    nv_write(idb->atmsig_mtp_flag, "%s", csb->nv_command);
	    break;
	case ATM_CMD_MTP_INTERVAL:
	    nv_write(atm->bc_interval != ATM_MTP_INTERVAL_DEFAULT, "%s %d",
		     csb->nv_command, atm->bc_interval / ONESEC);
	    break;
	case ATM_CMD_IDLETIMEOUT:
	    nv_write(atm->idle_time != ATM_IDLETIME_DEFAULT, "%s %d",
		     csb->nv_command, atm->idle_time / ONESEC);
	    break;
	case ATM_CMD_SMDS:		/* Give interface an SMDS addr. */
	    nv_write((uint)idb->vaddr, 
		     "%s %E", csb->nv_command, idb->vaddr->addr);
	    break;
	case ATM_CMD_MTU:
	    nv_write(atm->config.mtu != ATM_DEFAULT_MTU, "%s %d",
		     csb->nv_command, atm->config.mtu);
	    break;
	case ATM_CMD_VC_PER_VP:
	    nv_write(atm->config.rt_vc_per_vp != ATM_VC_PER_VP_DEFAULT, "%s %d",
		     csb->nv_command, atm->config.rt_vc_per_vp);
	    break;

	case ATM_CMD_UNI_VERSION:
	    if (atm->ignore_peer_uni) {
	      switch (atm->uni_version) {
	      case ATMUNI_VERSION_30:
		nv_write(TRUE,"%s 3.0",csb->nv_command);
		break;
	      case ATMUNI_VERSION_31:
		nv_write(TRUE,"%s 3.1",csb->nv_command);
		break;
	      default:
		break;
	      }
	    }
	    break;

        case ATM_CMD_RATEQ:
	{
	    uint ndx;
	    for(ndx = 0; ndx < atm->ratequeue_no; ndx++) {
		if(atm->rateq[ndx].state == RATE_QUEUE_PERMANENT) {
		    nv_write(TRUE, csb->nv_command);
		    nv_add(TRUE, " %d %d",
			   ndx, (atm->rateq[ndx].rate/1000));
		}
	    }
	}
	    break;
	case ATM_CMD_NSAP:
	    if (idb->nsap != NULL) {
	      /* Search through nsap list
	       * Nvgen only if not ESI generated
	       * Match nsap in case LANE has something registered
	       * on this idb.
	       */
	      for (nsap_ptr = atm->nsap_list_head; nsap_ptr != NULL;
		   nsap_ptr = nsap_ptr->next) {

		if ( (nsap_ptr->idb == idb) &&
		     (!nsap_ptr->useIlmiPrefix) &&
		     (bcmp(idb->nsap->addr, nsap_ptr->nsap.addr, 
			   STATIONLEN_ATMNSAP) == 0) ) {

		  atm_printnsap(idb->nsap->addr, nsapstr);
		  nv_write( !atmSig_nullNsap(idb->nsap->addr), "%s %s", 
			   csb->nv_command, nsapstr );
		  break;
		}
	      }
	    }
	    break;
	case ATM_CMD_E3FRAMING:
	    nv_write((atm->e3_framing != atm->e3_def_framing) &&
		     (atm->e3_framing == GETOBJ(int,1)), "%s", csb->nv_command);
	    break;
	case ATM_CMD_DS3FRAMING:
	    nv_write((atm->ds3_framing != atm->ds3_def_framing) &&
		     (atm->ds3_framing == GETOBJ(int,1)), "%s", csb->nv_command);
	    break;
	case ATM_CMD_CLOCKING:
	    nv_write(atm->txclock != ATM_CLOCK_LINE, "%s",
		     csb->nv_command);
	    break;
	case ATM_CMD_SONET:
	    if(atm->plimtype == ATM_PLIM_SONET) {
	   	nv_write(atm->sonetmode != ATM_SONET_STS3C, "%s",
			 csb->nv_command);
	    }
	    break;
	case ATM_CMD_MULTICAST:
	{
	    atm_multicast_t *multic = atm_get_multicast(idb);
	    if (multic) {
		nv_write(TRUE, "%s %E", csb->nv_command, 
		     multic->mca.addr);
	    }
	}
	break;
	
	case ATM_CMD_DS3SCRAMBLE:
	    nv_write(atm->flags & ATM_FLAG_DS3SCRAM, "%s", csb->nv_command);
	    break;
        case ATM_CMD_CLASS:
	    if (atm->def_class) {
		nv_write(TRUE, "%s %s", csb->nv_command,
                     atm->def_class->name);
	    }
            break;	
	case ATM_CMD_MTUREJECT:
	    nv_write(atm->mtu_reject_call !=
		     ATM_MTU_REJECT_DEFAULT, "%s",
		     csb->nv_command);
	    break;
	case ATM_CMD_SIG_TRAF_SHAPE:
	    nv_write(atm->atmsig_tr_shape_strict !=
		     ATMSIG_STRICT_TRAFFIC_SHAPING_DEFAULT,
		     "%s", csb->nv_command);
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
    case ATM_CMD_PVC:
	if ((GETOBJ(int, 3) == ATM_OAM_F4_SEGMENT) ||
	    (GETOBJ(int, 3) == ATM_OAM_F4_ENDTOEND)) {
	    printf ("\n%%Invalid VCI of %d requested: "
		    "(%s): Not creating vc:%d:%u:%u",GETOBJ(int,3),
		    hwidb->hw_namestring,GETOBJ(int,1),
		    GETOBJ(int,2),GETOBJ(int,3));
	    return;
	}
	vc = atm_getvc_deleted(atm,GETOBJ(int,1));
	if (vc && IS_VC_DELETED(vc) && IS_PVC_DOWN(vc)) {
	    printf ("ATM: VCD not yet available for re-use, hardware "
		    "hasn't completed deletion yet.");
	    return;
	}
	
	vc = atm_getvc_all(atm, GETOBJ(int,1));
	if (csb->sense) {
	    uint result;
	    /*
	     *  If we already have this VCD defined, we have to undo it
	     */
	    if (vc) {
		/*
		 *  But if this is an SVC, then we must just error out.
		 *  Bad things will happen if we pull down an SVC without
		 *  going through signalling.
		 */
		if (IS_ATMVC_SVC(vc)) {
		    printf("\n%%ATM: VCD %u is already in use for an SVC",
			   vc->vc);
		    return;
		}
		atm_remove_vc(hwidb, GETOBJ(int,1));
	    }

	    protocol = GETOBJ(int, 10);
	    flags = GETOBJ(int,4) | VC_FLAG_PVC;

	    user_vc_type = GETOBJ(int,20);

	    /* peak rate can't be zero unless not entered - use defaults*/
	    if (GETOBJ(int,5) == 0) {
		flags |= VC_FLAG_DEFAULT_RATE;
	    }

	    /* burst cell size can't be zero unless not entered -
	     * use defaults
	     */
	    if (GETOBJ(int,7) ==0) {
		flags |= VC_FLAG_DEFAULT_BURST;
	    }

            /* check qos parameters and print error message if not valid */
	    result = (*atm->funcs.qos_params_verify)
		(hwidb,GETOBJ(int,5),GETOBJ(int,6),
		 (GETOBJ(int,7)*atm->burstcells_factor));
 
	    if ((result & ATM_OUT_OF_RATE_QUEUES) != 0) {
		printf ("\nNot creating vc:%d interface: %s is out"
			" of rate queues.",
			GETOBJ(int,1),hwidb->hw_namestring);
		return;
	    }

	    if ((result & ATM_PEAKRATE_ERROR) != 0) {
		printf ("\nInvalid peak rate of %d requested: "
			"(%s): Not creating vc:%d:%u:%u",GETOBJ(int,5),
			hwidb->hw_namestring,GETOBJ(int,1),
			GETOBJ(int,2),GETOBJ(int,3));
		return;
	    }
 
	    if ((result & ATM_AVGRATE_ERROR) != 0) {
		printf("\nInvalid average rate of %d requested: (%s): "
		       "Not creating vc:%d:%u:%u",
		       GETOBJ(int,6),hwidb->hw_namestring,
		       GETOBJ(int,1),GETOBJ(int,2),GETOBJ(int,3));
		return;
	    }
 
	    if ((result & ATM_BURST_ERROR) != 0) {
		printf("\nInvalid burst size of %d cells requested:"
		       " (%s): Not creating vc:%d:%u:%u",
		       GETOBJ(int,7),hwidb->hw_namestring,
		       GETOBJ(int,1),GETOBJ(int,2),GETOBJ(int,3));
		return;
	    }
     
	    atm_create_vc(hwidb, GETOBJ(int,1), GETOBJ(int,2), 
			  GETOBJ(int,3), atm_addr, 0, 1, 
			  hwidb->maxmtu, flags, protocol,
			  user_vc_type,
			  idb, 
			  GETOBJ(int,5), 	/* peak */
			  GETOBJ(int,6),	/* avg */
			  	                /* burst */
			  (GETOBJ(int,7) * atm->burstcells_factor),
			  GETOBJ(int,8), 	/* mid-low */
			  GETOBJ(int,9),	/* mid-high */
			  GETOBJ(int,11), 	/* InARP */
			  GETOBJ(int,12));	/* OAM */

	} else {
	    if (!vc) {
		printf("\n%%ATM: VC %u is not configured", GETOBJ(int,1));
		return;
	    }
	    /* If VC is SVC, don't remove it with PVC command */
	    if (IS_ATMVC_SVC(vc)) {
		printf("\n%%ATM: VCD %u is SVC",
		       vc->vc);
		return;
	    }
	    atm_remove_vc(hwidb, GETOBJ(int,1));
	}
	break;
    case ATM_CMD_SMDS:		/* Give interface an SMDS addr. */
	if(!(atm->config.mode & ATM_CFG_MODE_AAL34)) {
	    printf("\n%% AAL3/4 not enabled");
	    return;
	}
	if (csb->sense) {
	    if(!(uint)idb->vaddr) {
		idb->vaddr = malloc(sizeof(hwaddrtype));
		if (!idb->vaddr) {
		    printf(nomemory);
		    return;
		}
	    }
	    bcopy(GETOBJ(hwaddr,1)->addr, idb->vaddr->addr,
		  STATIONLEN_SMDS64);
	    if(!SMDS_ISINDIV(idb->vaddr->addr[0])) {
		printf("\n%%ATM: SMDS address MUST be UNIcast type!");
		free(idb->vaddr);
		idb->vaddr = NULL;
		return;
	    }
	    idb->arp_smds = TRUE;
	} else {
	    if(idb->vaddr)
		free(idb->vaddr);
	    idb->vaddr = NULL;
	    idb->arp_smds = FALSE;
	    reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
	    reg_invoke_ar_cache_clear(idb);    
	}
	break;
    case ATM_CMD_MTP_SIG:
	if (csb->sense) 
	    idb->atmsig_mtp_flag = TRUE;
	else 
	    idb->atmsig_mtp_flag = FALSE; 
	break;
    case ATM_CMD_MTP_INTERVAL:
	if (csb->sense) 
	    atm->bc_interval = GETOBJ(int,1) * ONESEC;
	else 
	    atm->bc_interval = ATM_MTP_INTERVAL_DEFAULT;
	break;
    case ATM_CMD_IDLETIMEOUT:
	if (csb->sense) 
	    atm->idle_time = GETOBJ(int,1) * ONESEC;
	else 
	    atm->idle_time = ATM_IDLETIME_DEFAULT;
	break;
    case ATM_CMD_MTU:
	if (csb->sense) {
	    atm->config.mtu = GETOBJ(int, 1);
	    hwidb->maxmtu = atm->config.mtu;
	} else {
	    atm->config.mtu = ATM_DEFAULT_MTU;
	    hwidb->maxmtu = atm->config.mtu;
	}
	atm_newconfig(atm);
	break;
    case ATM_CMD_VC_PER_VP:
	if (csb->sense) {
	    uint vc_per_vp = GETOBJ(int,1);
	    vcd_t vcnum;
	    vc_info_t *vc_info;
	    ushort max_vpi_bits,max_vci_bits,max_vpi;
		
	    /*
	     * calculate the new max vci bits and vpi bits based
	     * on this new vc-per-vp
	     */
	    max_vci_bits = atm_calc_vci_bits(vc_per_vp);
		
	    max_vpi_bits = atm_calc_vpi_bits(
		(atm->def_config.max_vpi_bits +
		 atm->def_config.max_vci_bits), max_vci_bits);

	    /*
	     * calculate the highest possible vpi & vci allowed
	     * with the new vpi/vci max bits if this command was
	     * executed
	     */
	    max_vpi = atm_calc_max_vpivci(max_vpi_bits);
	    
	    /* Do we already have a higher vc configured
	     * on a vp than the max we now ask for or a higher vp
	     * than the new max_vpi_bits will support?
	     */
	    for (vcnum = 0; vcnum < atm->config.max_intf_vcs;vcnum++) {
		vc_info = atm_getvc(atm, vcnum);
		if (!vc_info) {
		    continue;
		}
		if (IS_ATMVC_SVC(vc_info)) {
			continue;
		}
		if (vc_info->vci > vc_per_vp - 1) {
		    printf ("VC-PER-VP not changed: This vc-per-vp allows a maximum vci of %d which is smaller than already configured VCD %u,VPI %u,VCI %u.",vc_per_vp - 1,vc_info->vc,vc_info->vpi,vc_info->vci);
		    return;
		}
		if (vc_info->vpi > max_vpi) {
		    printf ("VC-PER-VP not changed: This vc-per-vp allows a maximum vpi of %d which is smaller than already configured VCD %u,VPI %u,VCI %u.",max_vpi,vc_info->vc,vc_info->vpi,vc_info->vci);
		    return;
		}
	    }
	    /* We have accepted the vc-per-vp value so 
	     * calculate the max_vci_bits and max_vpi_bits
	     * needed by ILMI.
	     * NOTE: vc_per_vp must be a power of two (checked above).
	     */
	    atm->config.max_vci_bits = max_vci_bits;
		
	    atm->config.max_vpi_bits = max_vpi_bits;

	    atm->config.rt_vc_per_vp = vc_per_vp;
	}

	atm_newconfig(atm);
	break;
    case ATM_CMD_RATEQ:
    {
	uint rateq = GETOBJ(int, 1), rate = GETOBJ(int, 2);
	unsigned new_trate = 0;
	atm_setup_status_t status;
	rateq_param_t rq_param;
 
	if(rateq >= atm->ratequeue_no) {
	    printf("\n%%Bad Ratequeue specified, range is 0 .. %d",
		   atm->ratequeue_no-1);
	    return;
	}
	if(rate < ATM_RATEQ_MIN || rate > atm->plimrate) {
	    printf("\n%%Bad Rate %d specified, range is %dMbps .. %dMbps",
		   rate, ATM_RATEQ_MIN, atm->plimrate);
	    return;
	}
		
	switch (atm->rateq[rateq].state) {
	case RATE_QUEUE_PERMANENT:
	    if (atm->rateq[rateq].num_vc > 0) {
		printf("\n%%Rateq %d is currently being usd by %d VC's. It's not allowed to be changed",
		       rateq, atm->rateq[rateq].num_vc);
		return;
	    } 
	    break;

	case RATE_QUEUE_DYNAMIC:
	    printf("\n%%Rateq %d is currently being used as a DYNAMIC one. It's not allowed to be changed", rateq);
	    return;
	    break;
	case RATE_QUEUE_DISABLED:
	    break;
	default:
	    return;
	    break;
	}

	rq_param.rq_id = rateq;
	if (csb->sense) {
	    rq_param.rq_rate = rate*ATM_MBPS2KBPS;
	    rq_param.rq_state = RATE_QUEUE_PERMANENT;
	} else {
	    rq_param.rq_rate = 0;
	    rq_param.rq_state = RATE_QUEUE_DISABLED;
	}
	atm_update_rateq(atm, &rq_param);
	status = (*atm->funcs.config_rateq)(hwidb, rateq);
	/*
	 * Since ATM_SETUP_DELAYED implies that the config will be sent
	 * down to the driver following an interface card
	 * reset, we assume the driver/ucode shall execute the
	 * config successfully then.
	 */
	if (status == ATM_SETUP_FAILED) {
	    printf("\n%%ATM driver failed to %sconfigure rateq %d",
		   csb->sense? "":"de", rateq);
	    /*
	     * the driver fails to setup the config, 
	     * so we simply disable it.
	     */
	    rq_param.rq_id = rateq;
	    rq_param.rq_rate = 0;	/* Disable the Rateq */
	    rq_param.rq_state = RATE_QUEUE_DISABLED;
	    atm_update_rateq(atm, &rq_param);
	}
	new_trate = atm_rateq_ratesum(atm); /* in Kbps */
	if(new_trate > (atm->plimrate * ATM_MBPS2KBPS)) {
	    printf("\n%%WARNING(%s): Total rateq allocation %dKbps exceeded maximum plim rate of %dMbps.",
		   hwidb->hw_namestring, new_trate, atm->plimrate);
	}
		
    }
	break;
      case ATM_CMD_NSAP:
	{
	  /* csb->sense = FALSE, "no" form of command */
	  if (!csb->sense) {
	    
	    /* Validate idb->nsap */
	    if (idb->nsap == NULL) {
	      printf("\n%%ATM no nsap to delete");
	      return;
	    }
	    
	    /* Don't remove this NSAP if generated from LANE */
	    if (is_atm_lane(idb)) {
	      printf("\n%%ATM NSAP command conflicts with LANE client.");
	      return;
	    }

	    /* Search through nsap list
	     * Cancel only if not ESI generated
	     * Match nsap in case LANE has something registered
	     * on this idb.
	     */

	    for (nsap_ptr = atm->nsap_list_head; nsap_ptr != NULL;
		 nsap_ptr = nsap_ptr->next) {
	      
	      if ( (nsap_ptr->idb == idb) &&
		   (!nsap_ptr->useIlmiPrefix) &&
		   (bcmp(idb->nsap->addr, nsap_ptr->nsap.addr, 
			 STATIONLEN_ATMNSAP) == 0) ) {

		/* Cancel NSAP */
		atmSmap_cancelAtmAddress(idb);
		reg_invoke_atm_addr_change(idb, csb->sense);
		free(idb->nsap);
		idb->nsap = NULL; 
		return;
	      }
	    }

	    /* Not found, using ESI, or using LANE */
	    printf("\n%%ATM nsap not deleted");
	    return;

	  }
	  
	  /* csb->sense = TRUE, adding command */
	  if (is_atm_lane(idb)) {
	    printf("\n%%ATM NSAP command conflicts with LANE client.");
	    return;
	  }

 	  /* Don't allow NSAP if interface is point-to-point */
	    if (is_p2p(idb)) {
 	      printf ("\nATM: cannot configure NSAP on point-to-point"
 		      " sub-interfaces.");
 	      return;
	  }

	  /* Don't allow NSAP if already configured on this or 
	   * or any other interface
	   */
	  hwconf_tmp = GETOBJ(hwaddr,1);
	  FOR_ALL_HWIDBS(hwidb) {
	    if (!hwidb->status & IDB_ATM)
	      continue;
	    FOR_ALL_SWIDBS_ON_HW(hwidb, tmp_idb) {
	      if (tmp_idb->nsap) {
		if ( bcmp(tmp_idb->nsap->addr, hwconf_tmp->addr,
			  STATIONLEN_ATMNSAP) == 0 ) {
		  printf("\n%%ATM:nsap already configured on %s", 
			   tmp_idb->namestring);
		  return;
		}
	      }
	    }
	  }

	  /* Cancel current NSAP if it exists
	   * Match nsap in case LANE has something registered
	   * on this idb.
	   */
	  if (idb->nsap != NULL) {

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
		  printf("\n%%ATM removing nsap");
		if ( (nsap_ptr->useIlmiPrefix) && 
		    (nsap_ptr->ilmiAddrRegistered) ) {
		  atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressInValid);
		}
		atmSmap_cancelAtmAddress(idb);
	      }
	    }
	  }

	  /* Enter new NSAP */
	  if (idb->nsap == NULL) {
	    idb->nsap = malloc( sizeof(hwaddrtype) );
	    if (idb->nsap == NULL) {
	      return;
	    }
	  }
	  bzero(idb->nsap, sizeof(hwaddrtype));
	  bcopy(GETOBJ(hwaddr,1), idb->nsap, sizeof(hwaddrtype));
	  if ( atmSig_nullNsap(idb->nsap->addr) ) {
	    printf("\n%%Atm null nsap address not configured");
	    free(idb->nsap);
	    idb->nsap = NULL; 
	    return;
	  }
	  atmSmap_registerAtmAddress(idb);
	  reg_invoke_atm_addr_change(idb, csb->sense);
	}
	break;
    break;
    case ATM_CMD_E3FRAMING:
	if (csb->sense) {
	    switch (GETOBJ(int,1)) {
	    case ATM_FRAMING_E3G751PLCP:
	    case ATM_FRAMING_E3G751ADM:
	    case ATM_FRAMING_E3G832ADM:
		atm->e3_framing = GETOBJ(int,1);
		break;
	    default:
		printf("\n%%Framing not allowed on E3 PLIM");
		return;
	    }
	} else {
	    atm->e3_framing = atm->e3_def_framing;
	}
	atm_newconfig(atm);
	break;
    case ATM_CMD_DS3FRAMING:
	if (csb->sense) {
	    switch (GETOBJ(int,1)) {
	    case ATM_FRAMING_DS3CBITADM:
	    case ATM_FRAMING_DS3CBITPLCP:
	    case ATM_FRAMING_DS3M23ADM:
	    case ATM_FRAMING_DS3M23PLCP:
		atm->ds3_framing = GETOBJ(int,1);
		break;
	    default:
		printf("\n%%Framing not allowed on DS3 PLIM");
		return;
	    }
	} else {
	    atm->ds3_framing = ATM_FRAMING_DS3CBITADM;
	}
	atm_newconfig(atm);
	break;
    case ATM_CMD_CLOCKING:
	if(atm->plimtype == ATM_PLIM_4B5B) {
	    printf("\n%%Clocking is allowed on the E3 & SONET PLIMs");
	    return;
	}
	if (csb->sense) {
	    atm->txclock = GETOBJ(int,1);
	} else {
	    atm->txclock = ATM_CLOCK_LINE;
	}
	atm_newconfig(atm);
	break;
    case ATM_CMD_SONET:
	if(atm->plimtype != ATM_PLIM_SONET) {
	    printf("\n%%SONET command is only allowed on SONET PLIMs");
	    return;
	}
	if (csb->sense) {
	    atm->sonetmode = GETOBJ(int,1);
	} else {
	    atm->sonetmode = ATM_SONET_STS3C;
	}
	atm_newconfig(atm);
	break;
    case ATM_CMD_MULTICAST:
	if (csb->sense) {
	    atm_add_multicast(idb, GETOBJ(hwaddr,1));
	} else {
	    atm_delete_multicast(idb);
	}
	break;

      case ATM_CMD_UNI_VERSION:
	/* ask user to shut down the interface before changing
	   uni version. but don't do this during bootup */
	if (atm->uni_version_init && (hwidb->state != IDBS_ADMINDOWN) ) {
          printf("\nPlease shut down the interface first");
          return;
	}
	atm->uni_version_init = TRUE;
	if (csb->sense)  {
	  atm->ignore_peer_uni = TRUE;
	  atm->uni_version = GETOBJ(int,1);
	  atm_newconfig(atm);
	}
	else {
	  atm->ignore_peer_uni = FALSE;
	  atm->uni_version = ATMUNI_VERSION_DEFAULT;
	}
	break;

    case ATM_CMD_DS3SCRAMBLE:
 	if (atm->plimtype != ATM_PLIM_DS3) {
 	    printf("\n%%ds3-scramble option is allowed on DS3 PLIMs only");
 	    return;
 	}
	if (csb->sense) {
	    atm->flags |= ATM_FLAG_DS3SCRAM;
	} else {
	    atm->flags &= ~ATM_FLAG_DS3SCRAM;
	}
	atm_newconfig(atm);
	break;
    case ATM_CMD_CLASS:
    {
	mapclass_type *class;
	char *class_name;

	class_name = GETOBJ(string, 1);

	if (csb->sense) {
	    class =  mapclass_lookup(class_name, ET_ATM);
	    if (!class) {
		/* creates an invisible class  */
		class = mapclass_create(class_name, ET_ATM, FALSE);
		if (!class) {
		    printf("\n%%Can not create mapclass: %s.",
			   class_name);
		    return;
		}
		enqueue(mapclassQ, class);
	    }
	    class->ref_count++;
	    atm->def_class = class;
	} else {
	    if (!atm->def_class) {
                printf("\n%%ATM class is not configured.");
                return;
	    }
	    atm->def_class->ref_count--;
	    atm->def_class = NULL;

	}
    }
    break;
    case ATM_CMD_MTUREJECT:
	/* write selected behavior into atm config struct */
	if (csb->sense) {
	    atm->mtu_reject_call = TRUE;
	} else {
	    atm->mtu_reject_call = FALSE;
	}
	atm_newconfig(atm);
	break;
    case ATM_CMD_SIG_TRAF_SHAPE:
	if (csb->sense)
	    atm->atmsig_tr_shape_strict = TRUE;
	else
	    atm->atmsig_tr_shape_strict = FALSE;
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

static const char vchead[] =
"                                     AAL /         Peak   Avg.  Burst       \nInterface     VCD   VPI   VCI Type  Encapsulation  Kbps   Kbps  Cells Status";

/*
 * atm_show_commands: Called to show the individual ATM vars. 
 * stats, VCs, config, etc.
 */
void
atm_show_commands (parseinfo *csb)
{
    idbtype *sidb;
    hwidbtype *idb=NULL;
    atm_db_t *atm=NULL;
    atmSvcType *svc;
    unsigned ndx, vcnum;
    vc_info_t *vc;

    switch(csb->which) {
    case SHOW_ATM_VC:			/* Show ACTIVE VC info.*/
	vcnum = GETOBJ(int,1);
	automore_enable(NULL);
	if(!vcnum)
    	    automore_header(vchead);
	FOR_ALL_HWIDBS(idb) {
	    if(!is_atm(idb)) 
		continue;
	    atm = idb->atm_db;
	    if (vcnum) {
	    	if(!(vc = atm_getvc_all(atm, vcnum)))
		    continue;
		if (ISVCACTIVE(vc))
		    (*atm->funcs.getvc_stats)(idb, vc);
		ATM_INAS(atm, vc);
		ATM_OUTAS(atm, vc);

	    	printf("\n%s: VCD: %u, VPI: %u, VCI: %u, etype:0x%x", 
			vc->swidb->namestring, 
			vcnum, vc->vpi, vc->vci, vc->etype);
		switch(vc->flags & VC_FLAG_AAL_MASK) {
		case ATM_VC_AAL5SNAP:
		    printf(", AAL5 - LLC/SNAP");
		    break;
		case ATM_VC_AAL5NLPID:
		    printf(", AAL5 - NLPID");
		    break;
		case ATM_VC_AAL5FRNLPID:
		    printf(", AAL5 - FR/NLPID");
		    break;
		case ATM_VC_AAL34SMDS:
		    printf(", AAL3/4 - SMDS");
		    break;
		case ATM_VC_AALQSAAL:
		    printf(", AAL5 - SAAL");
		    break;
		case ATM_VC_AAL5LANE:
		    switch(vc->user_vc_type) {
		    case LANE_LEC_DIRECT:
		    case LANE_LEC_DISTRIBUTE:
		    case LANE_LEC_CONFIGURE:
		    case LANE_LEC_SEND:
		    case LANE_LEC_TR_SEND:
		    case LANE_LEC_FORWARD:
		    case LANE_LEC_TR_FORWARD:
			printf(", LANE-LEC");
			break;
		    case LANE_LES_DIRECT:
		    case LANE_LES_DISTRIBUTE:
			printf(", LANE-LES");
			break;
		    case LANE_BUS_SEND:
		    case LANE_BUS_TR_SEND:
		    case LANE_BUS_FORWARD:
		    case LANE_BUS_TR_FORWARD:
			printf(", LANE-BUS");
			break;
		    case LANE_CONFIGURE:
			printf(", LANE-LECS");
			break;
		    case LANE_LEC_DATA:
		    case LANE_LEC_TR_DATA:
			printf(", LANE-DATA");
			break;
		    }
		    break;
		case ATM_VC_AAL5ILMI:
		    printf(", AAL5 - ILMI");
		    break;
		default:
		    printf(", AAL5 - MUX");
		    break;
		}
		printf(", Flags: 0x%x", vc->flags);
	    	printf("\nPeakRate: %u, Average Rate: %u",
			vc->peakrate, vc->avgrate);
		printf(", Burst Cells: %u, VCmode: 0x%x",
			vc->burstcells, vc->vcmode);
		if((vc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL34SMDS) {
		    printf("\nMID start: %u, MID end: %u", 
			vc->mid_start, vc->mid_end);
		}
		if(vc->oam) {
		    printf("\nOAM frequency: %u second(s)", vc->oam);
		} else {
		    printf("\nOAM DISABLED");
		}
		if(vc->inarp) {
		    printf(", InARP frequency: %u minute(s)", 
			(vc->inarp/ONEMIN));
		} else {
		    printf(", InARP DISABLED");
		}
		printf("\nInPkts: %u, OutPkts: %u, InBytes: %u, OutBytes: %u",
			vc->total_in_pkts, vc->total_out_pkts, 
			vc->in_bytes, vc->out_bytes);
		printf("\nInPRoc: %u, OutPRoc: %u, Broadcasts: %u",
			vc->inpkts, vc->outpkts, vc->broadcasts);
		printf("\nInFast: %u, OutFast: %u",
			vc->infast, vc->outfast);
		printf(", InAS: %u, OutAS: %u", vc->in_as, vc->out_as);
		printf("\nOAM F5 cells sent: %u, OAM cells received: %u",
			vc->out_oam, vc->in_oam);

		printf("\nStatus: %8s", ISVCACTIVE(vc)?"ACTIVE":"INACTIVE");

		if (IS_ATMVC_SVC(vc) && ISVCACTIVE(vc)) {
		    printf(", TTL: %7u",
			   TIME_LEFT_SLEEPING(vc->idle_timer)/ONEMIN);
		}
 
                if (IS_ATMVC_SVC(vc) && ISVCACTIVE(vc)) {
		    atmSig_show_vc(vcnum, idb);
                }
		continue;
	    }
	    for(vcnum = 1; vcnum < atm->config.max_intf_vcs; vcnum++) {
		if(!(vc = atm_getvc_all(atm, vcnum)))
		    continue;
		if (IS_ATMVC_SVC(vc) && !ISVCACTIVE(vc))
                    continue;
		printf("\n%12s%-5d %-5u %-5u ",
	    	        vc->swidb->namestring, vc->vc, vc->vpi, vc->vci);

 		/*
 		 * Print SVC (point-to-point), MSVC (multipoint), or PVC.
 		 */
 		if (IS_ATMVC_SVC(vc)) {
		    svc = atmSig_findSvcBlockByVC(vc->swidb, vcnum);
 		    printf("%6s", (svc && svc->multiPoint) ? "MSVC" : " SVC");
 		} else {
 		    printf("%6s", " PVC");
 		}
		switch(vc->flags & VC_FLAG_AAL_MASK) {
		case ATM_VC_AAL5SNAP:
		    printf("%13s", "AAL5-SNAP");
		    break;
		case ATM_VC_AAL5NLPID:
		    printf("%13s", "AAL5-NLPID");
		    break;
		case ATM_VC_AAL5FRNLPID:
		    printf("%13s", "AAL5-FR/NLPID");
		    break;
		case ATM_VC_AAL5MUX:
		    printf("%13s", "AAL5-MUX");
		    break;
                case ATM_VC_AALQSAAL:
                    printf("%13s", "AAL5-SAAL");
                    break;
		case ATM_VC_AAL34SMDS:
		    printf("%13s", "AAL3/4-SMDS");
		    break;
		case ATM_VC_AAL5LANE:
		    switch(vc->user_vc_type) {
		    case LANE_LEC_DIRECT:
		    case LANE_LEC_DISTRIBUTE:
		    case LANE_LEC_CONFIGURE:
		    case LANE_LEC_SEND:
		    case LANE_LEC_TR_SEND:
		    case LANE_LEC_FORWARD:
		    case LANE_LEC_TR_FORWARD:
			printf("%13s", "LANE-LEC");
			break;
		    case LANE_LES_DIRECT:
		    case LANE_LES_DISTRIBUTE:
			printf("%13s", "LANE-LES");
			break;
		    case LANE_BUS_SEND:
		    case LANE_BUS_TR_SEND:
		    case LANE_BUS_FORWARD:
		    case LANE_BUS_TR_FORWARD:
			printf("%13s", "LANE-BUS");
			break;
		    case LANE_CONFIGURE:
			printf("%13s", "LANE-LECS");
			break;
		    case LANE_LEC_DATA:
		    case LANE_LEC_TR_DATA:
			printf("%13s", "LANE-DATA");
			break;
		    }
		    break;
		case ATM_VC_AAL5ILMI:
		    printf("%13s", "AAL5-ILMI");
		    break;
		}
		printf(" %-6u %-6u%-6u ", vc->peakrate, vc->avgrate,
			vc->burstcells);
		printf("%8s", ISVCACTIVE(vc)? "ACTIVE":"INACTIVE");
	    }
	    vcnum = 0;
	}
    	automore_disable();
	break;
    case SHOW_ATM_TRAFFIC:		/* Show ATM traffic */
	printf("\n%u Input packets", atm_counters[ATM_INPUT]);
	printf("\n%u Output packets", atm_counters[ATM_OUTPUT]);
	printf("\n%u Broadcast packets", atm_counters[ATM_BROADCAST]);
	printf("\n%u Packets received on non-existent VC", atm_counters[ATM_INPUT_NOVC]);
	printf("\n%u Packets attempted to send on non-existent VC", atm_counters[ATM_OUT_NOVC]);
	printf("\n%u OAM cells received", atm_counters[ATM_OAM_INPUT_CELLS]);
	printf("\n%u OAM cells sent", atm_counters[ATM_OAM_OUTPUT_CELLS]);
	/*
	 * RLFS: 2/23/94
	 * Until we collect these, don't print them & confuse people.
	 *
	printf("\n%u Packets with CRC errors", atm_counters[ATM_CRC_ERRS]);
	printf("\n%u Cells lost\n", atm_counters[ATM_CELL_LOSS]);
	*/
	break;
    case SHOW_ATM_INT:			/* Show ATM Interface specifics */
    	sidb = GETOBJ(idb,1);
	if (sidb) {
	    idb = sidb->hwptr;
	} else {
	    printf("\nerror: source idb not set");
	    return;
	}
	if(!is_atm(idb)) {
	    printf("\nInterface %s is not an ATM interface", 
		idb->hw_namestring);
	    return;
	}
    	atm = idb->atm_db;
	printf("\nATM interface %s:", idb->hw_namestring);
	printf("\nAAL enabled: %s %s", 
	    	(atm->config.mode & ATM_CFG_MODE_AAL5) ? " AAL5" :
	    	"", (atm->config.mode & ATM_CFG_MODE_AAL34) ? " AAL3/4" : "");
	printf(", Maximum VCs: %u, Current VCCs: %u",
	       atm->config.max_intf_vcs,
	    	atm->num_vc);
	printf("\nTx buffers %u, Rx buffers %u", atm->config.tx_buff, 
	    	atm->config.rx_buff);
	printf(", Exception Queue: %u, Raw Queue: %u", atm->config.exq,
		atm->config.raw_queue_size);
	printf("\nVP Filter: 0x%x, VCIs per VPI: %u",
		atm->config.vp_filter, atm->config.rt_vc_per_vp);
	printf(", Max. Datagram Size:%u", atm->config.mtu);
    	if(atm->config.mode & ATM_CFG_MODE_AAL34) {
	    printf(", MIDs/VC: %u", atm->config.mid_per_vc);
	}
	printf("\nPLIM Type:");
	switch(atm->plimtype) {
	default:
	case ATM_PLIM_INVALID:
	    printf("INVALID");
	    break;
	case ATM_PLIM_DS3:
	    printf("DS3 - %dMbps, Framing is %s",
		   atm->plimrate, atm_framing_string(idb));
	    break;
	case ATM_PLIM_E3:
	    printf("E3 - %dMbps, Framing is %s",
		   atm->plimrate, atm_framing_string(idb));
            break;
	case ATM_PLIM_4B5B:
	    printf("4B5B - %dMbps", atm->plimrate);
	    break;
	case ATM_PLIM_SONET:
	    printf("SONET - %dMbps", atm->plimrate);
	    break;
	}
	switch(atm->txclock) {
	default:
	case ATM_CLOCK_LINE:
	    printf(", TX clocking: LINE");
	    break;
	case ATM_CLOCK_INTERNAL:
	    printf(", TX clocking: INTERNAL");
	    break;
	}
	if (atm->plimtype == ATM_PLIM_DS3) {
	    printf(", Scrambling:");
	    if (atm->flags & ATM_FLAG_DS3SCRAM) {
		printf(" ON");
	    } else {
		printf(" OFF");
	    }
	}
	printf("\n%u input, %u output, %u IN fast, %u OUT fast",
	    	atm->inpkts, atm->outpkts, atm->infast, atm->outfast);
	if (atm->out_ts_drop != UNSUPPORTED_COUNT)
	    printf(", %u out drop", atm->out_ts_drop);
	for(ndx = 0; ndx < atm->ratequeue_no; ndx++) {
	    switch(atm->rateq[ndx].state) {
	    case RATE_QUEUE_DYNAMIC:
		printf("\nRate-Queue %d set to %uKbps, reg=0x%x DYNAMIC", 
			ndx, atm->rateq[ndx].rate, 
			atm->config.rateq[ndx]);
		if (atm->rateq[ndx].num_vc) {
		    printf(", %u VCCs", atm->rateq[ndx].num_vc);
		}
		break;
	    case RATE_QUEUE_PERMANENT:
		printf("\nRate-Queue %d set to %uKbps, reg=0x%x PERMANENT", 
			ndx, atm->rateq[ndx].rate, 
			atm->config.rateq[ndx]);
		break;
	    case RATE_QUEUE_DISABLED:
		break;
	    }
	}
	FOR_ALL_SWIDBS_ON_HW(idb, sidb) {
	    if(sidb->vaddr) {
		atm_multicast_t *multic = atm_get_multicast(sidb);

		printf("\n%s: AAL3/4-SMDS address %E",
			sidb->namestring, sidb->vaddr->addr);
		if(multic) {
		    printf(" Multicast %E", multic->mca.addr);
		} else {
		    printf(" No Multicast address");
		}
	    }
	}
	if(atm->config_status == ATM_CONFIG_OK)
	    printf("\nConfig. is ACTIVE");
	else
	    printf("\nNew Config. is ACTIVE in %d seconds",
		atm->config_delay - (ELAPSED_TIME(atm->config_time)/ONESEC));
	break;
	
    case SHOW_ATM_MAP:			/* Show ATM static mapping */
	{
	    maplist_type *list;
	    staticmap_type *map;
	    char buf[64];
	    char nsapbuf[MAX_ATM_NSAP_ASCII_LEN];
	    uint i;
	    boolean connection_up = FALSE;
	    atm_params_type *clparams =  NULL;

	    automore_enable(NULL);
  	    FOR_ALL_MAPLISTS(list) {
		mem_lock(list);	/* lock the list for the print */
  		if (list->visible) {
		    printf("\nMap list %s :", list->name);
		    printf(" PERMANENT");
		} else {
		    if(!list->ref_count) {	/* Don't display if EMPTY */
			continue;
		    }
		    printf("\nMap list %s :", list->name);
		    printf(" DYNAMIC");
		}
		FOR_ALL_MAPS_IN_LIST(i, list, map) {
		    if (map->encap == ET_ATM) {
			mem_lock(map); /* lock the map for the print */
		    	print_proto_addr(NULL, &map->addr, map->protocol, buf);
		    	printf("\n%s maps to", buf);
		    	switch(map->hwconf.type) {
		    	case STATION_ATMVC:
			    printf(" VC %u", *(ushort *)map->hwconf.addr);
			    break;
		    	case STATION_ATMNSAP:
			    atm_printnsap(map->hwconf.addr, nsapbuf);
			    printf(" NSAP %s", nsapbuf);
			    break;
		    	case STATION_SMDS64:
			    printf(" SMDS %E", map->hwconf.addr);
			    break;
		    	default:	/* Should never happen -SHB 7/13/94 */
			    break;
		    	}
			if ( (map->class) || 
			    (((atm_params_type *)map->params)->broadcast) ||
			    (((atm_params_type *)map->params)->aal5mux) ||
			    (map->hwaddr.type != STATION_ILLEGAL) ||
			    ((map->atm_map) && 
			      (map->atm_map->bc_hwaddr.type != 
			       STATION_ILLEGAL)) ) 
			  printf("\n\t");
			
			/*
			 * Check the mapclass and/or the static maplist for
			 * "broadcast", and "aal5mux". 
			 * 
			 */
		    	if ((map->class) && (map->class->params)) {
		    	    printf(", class %s", map->class->name);

			   clparams = (atm_params_type *)map->class->params;

		    	   if ((clparams->broadcast) || 
			       (((atm_params_type *)map->params)->broadcast))
		    	       printf(", broadcast");
		    	   if ((clparams->aal5mux) ||
			       (((atm_params_type *)map->params)->aal5mux))
			       printf(", aal5mux");
			} else {
		    	   if (((atm_params_type *)map->params)->broadcast)
		    	       printf(", broadcast");
		    	   if (((atm_params_type *)map->params)->aal5mux)
		    	      printf(", aal5mux");
                        }
                    	if (map->hwaddr.type != STATION_ILLEGAL) {
  			   connection_up = TRUE;
                           printf(", connection up, VC %u",
					*(ushort *)map->hwaddr.addr);
		    	}
			
			if (map->atm_map) {
			  if (map->atm_map->bc_hwaddr.type != STATION_ILLEGAL){
  			    connection_up = TRUE;
			    printf(", multipoint connection up, VC %u",
				   *(ushort *)map->atm_map->bc_hwaddr.addr);
			  }
			}
			if ( (connection_up) && (map->sm_swidb) )
			    printf(", %s", map->sm_swidb->namestring);
			free(map);
                    }
		}
		printf("\n");
		free(list);
	    }
	    automore_disable();
	}
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/* 
  *  Add code to enable user to filter atm packets on a per interface
  *  or a per vc basis.
  *  debug_swidb and debug_vc globals declared in this file
  *
  */
void 
atm_debug_command (parseinfo *csb)
{
 
    debug_command(csb);


    /* most debug command processing is done in debug_command 
     * but to allow filtering on a per-interface or a per-sub
     * interface basis we must provide additional handling for
     * the packet and oam commands.
     * GETOBJ(int,1) contains TRUE/FALSE whether an interface
     *    was selected
     * GETOBJ(idb,1) contains a software interface
     * GETOBJ(int,2) contains TRUE/FALSE whether a vc was selected
     * GETOBJ(int,3) contains the selected vc
     * The selected interface is stored in global atm_debug_swidb(oam)
     * or atm_debug_pkt_swidb(packet)
     * The selected vc is stored in global atm_debug_vc
     */
    switch(csb->which) {
    case DEBUG_ATMPACKET:
	if (GETOBJ(int,1)) {
	    atm_debug_swidb[ATM_PACKET_FILTERING] = GETOBJ(idb,1);
	} else {
	    atm_debug_swidb[ATM_PACKET_FILTERING] = NULL;
	    if (atm_packet_debug)
		printf ("\nDisplaying all ATM packets");
	}
	if (GETOBJ(int,2)) {
	    atm_debug_vc = GETOBJ(int,3);
	} else {
	    atm_debug_vc = NO_VC;
	}    
	if (atm_packet_debug) {
	    if (atm_debug_swidb[ATM_PACKET_FILTERING] != NULL) {
		printf ("\nDisplaying packets on interface %s ",
			atm_debug_swidb[ATM_PACKET_FILTERING]->namestring);
		if (atm_debug_vc != ((vcd_t) NO_VC))
		    printf ("VCD %u only",atm_debug_vc);
		else
		    printf ("only");
	    }
	}
	break;
    case DEBUG_ATMOAM:
	if (GETOBJ(int,1)) {
	    atm_debug_swidb[ATM_OAM_FILTERING] = GETOBJ(idb,1);
	    printf ("\nDisplaying oam data from interface %s only",
		    atm_debug_swidb[ATM_OAM_FILTERING]->namestring);
	    printf ("\nOAM AIS & PERF filtering can only be done on a per "
		    "interface basis.");
	} else {
	    atm_debug_swidb[ATM_OAM_FILTERING] = NULL;
	    if (atm_oam_debug)
		printf ("\nDisplaying all ATM oam data");
	}
	break;
    }
}

/*
 * atm_encaps_show_protocol_attr
 *
 * Registry case called out of network.c, that prints information for
 * the "show interface" command.
 */
void atm_encaps_show_protocol_attr (idbtype *swidb)
{
    if (swidb->nsap && !atmSig_nullNsap(swidb->nsap->addr)) {
	char nsapstr[MAX_ATM_NSAP_ASCII_LEN];

	atm_printnsap(swidb->nsap->addr, nsapstr);
	printf("\n  NSAP address: %s", nsapstr);
    }

    printf("\n  Encapsulation ATM");
    if (is_atm_lane(swidb)) {
	printf("-LANE");
    }
}

/*
 * atm_show_packet: To show packet contents.
 * OUTPUT/RETURN:
 * INPUT:
 */
void
atm_show_packet (hwidbtype *hwidb, paktype *pak, boolean outpkt)
{
    vcd_t vcnum;
    ushort pakenc;
    vc_info_t *vc;
    atm_db_t *atm;
    union iso {
	snap_hdr	snap;
	sap_hdr	sap;
    } *iso;
    ushort *pk;
    unsigned ndx, lns;
    ushort *epk;
    atm_aal5mux_t *mux;
    atm_aal5nlpid_t *nlpid;
    atm_aal5lane_t *lane;
    boolean	oam_pkt;


    if(!atm_packet_debug && !atm_oam_debug)
	return;

    vcnum = GETSHORT(pak->datagramstart);
    atm = hwidb->atm_db;
    /* this should never happen */
    if (atm == NULL) 
      return;
    if (vcnum != 0) {
	vc = atm_getvc(atm, vcnum);
    } else {
	vc = NULL;
    }
    
    /* since OAM cells are marked by making the vc number 0 */
    if (vcnum != 0) {
	if (atm_debug_swidb[ATM_PACKET_FILTERING] != NULL &&
	    atm_debug_swidb[ATM_PACKET_FILTERING] != vc->swidb)
	    return;
    } else {
	if (atm_debug_swidb[ATM_OAM_FILTERING] != NULL &&
	    atm_debug_swidb[ATM_OAM_FILTERING]->hwptr != hwidb)
	    return;
    }
    if (atm_packet_debug) {
	if (atm_debug_vc != ((ushort)NO_VC) && atm_debug_vc != vcnum)
	    return;
    }
    pakenc = (GETSHORT(pak->datagramstart +2));
    if (outpkt)
	pakenc &= ATM_DM_PKT_MSK;
    iso = (union iso *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
    pk = (ushort *)(pak->datagramstart + ATM_ENCAP_HDRBYTES + SAP_HDRBYTES);
    epk = (ushort *)((uchar *)pak->datagramstart + pak->datagramsize);
    mux = (atm_aal5mux_t *)pak->datagramstart;
    nlpid = (atm_aal5nlpid_t *)pak->datagramstart;
    lane = (atm_aal5lane_t *)pak->datagramstart;
    oam_pkt = FALSE;

    /*
     * Check for OAM cells 
     */
    if (outpkt) {
      if(pakenc == AIP_DM_PKT_OAM_VC || pakenc == AIP_DM_PKT_OAM_DM) {
	  oam_pkt = TRUE;
      }
    } else {
      if(pakenc == ATM_ENCTYPE_RAW) {
	  oam_pkt = TRUE;
      }
    }

    if(!vc && !oam_pkt) {
      buginf("\n%s(%s): ", hwidb->hw_namestring, (outpkt) ? "O" : "I");
      if(outpkt)
    	buginf("VCD:0x%x DM:0x%x", vcnum, pakenc);
      else
    	buginf("VCD:0x%x Type:0x%x", vcnum, pakenc);

      buginf("Inactive or not configured VCD");
      goto atm_show_inactive;
    }

    if (oam_pkt) {
	return;
    }

    if (!atm_packet_debug)
      return;
    buginf("\n%s(%s): \n", vc->swidb->namestring, (outpkt) ? "O" : "I");
    if(outpkt)
    	buginf("VCD:0x%x DM:0x%x", vcnum, pakenc);
    else
    	buginf("VCD:0x%x Type:0x%x", vcnum, pakenc);


    switch(vc->flags & VC_FLAG_AAL_MASK) {
    case ATM_VC_AAL5SNAP:
	if(GETSHORT (&iso->sap.dsap) == (SAP_SNAP | (SAP_SNAP << 8))) {
            buginf(" SAP:%02x%02x CTL:%02x ",
                iso->snap.dsap, iso->snap.ssap, iso->snap.control);
            pk = (ushort *)(pak->datagramstart + ATM_SNAP_ENCAPBYTES);
            buginf("OUI:%02x%02x%02x TYPE:%04x", iso->snap.oui[0], 
		iso->snap.oui[1], iso->snap.oui[2], GETSHORT(&iso->snap.type));
        } else {
            buginf(" SAP:%02x%02x CTL:%02x ",
                iso->snap.dsap, iso->snap.ssap, iso->snap.control);
            pk = (ushort *)(pak->datagramstart + SAP_HDRBYTES +
                ATM_ENCAP_HDRBYTES);
        }
	break;
    case ATM_VC_AAL5MUX:
	buginf(", MUX");
	if (!outpkt) {
	    buginf("ETYPE:0x%04x", GETSHORT(&mux->encap.enctype));
	} else {
	    buginf("ETYPE:0x%04x", vc->etype);
	}
	pk = (ushort *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
	break;
    case ATM_VC_AALQSAAL:
        buginf(", SAAL, ");
        if (!outpkt) {
            buginf("ETYPE:0x%04x", GETSHORT(&mux->encap.enctype));
        } else {
            buginf("ETYPE:0x%04x", vc->etype);
        }
        pk = (ushort *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
        break; 
    case ATM_VC_AAL5ILMI:
	buginf(", ILMI,");
	pk = (ushort *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
	break; 
    case ATM_VC_AAL5NLPID:
        buginf(" NLPID:0x%04x", nlpid->nlpid);
        pk = (ushort *)(pak->datagramstart + ATM_NLPID_ENCAPBYTES);
	break;
    case ATM_VC_AAL5LANE:
	buginf(", LANE, ");
	if (outpkt) {
	    buginf("ETYPE:0x%04x LECID:0x%04x ", 
		   vc->etype, 
		   lane->lecid);
	} else {
	    buginf("ETYPE:0x%04x LECID:0x%04x ", 
                   lane->encap.enctype, 
                   lane->lecid);
	}
        pk = (ushort *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
	break;
    case ATM_VC_AAL34SMDS:
	buginf(", SMDS");
        pk = (ushort *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
	break;
    default:
	buginf("*Unknown encapsulation*");
    }

atm_show_inactive:
    buginf(" Length:0x%x", pak->datagramsize);
    for (lns = 0; lns < 3; lns++) {
	buginf("\n");
    	for (ndx = 0; ndx < 15 && pk < epk; ndx++, pk++) {
	    buginf("%04x ", GETSHORT(pk));
	}
	if (pk >= epk)
	    break;
    }
    buginf("\n");
}
