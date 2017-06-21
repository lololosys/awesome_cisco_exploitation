/* $Id: ios_atmdriver.c,v 3.1.6.7 1996/08/13 02:23:03 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/ios_atmdriver.c,v $
 *------------------------------------------------------------------
 * ios_atmdriver.c
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ios_atmdriver.c,v $
 * Revision 3.1.6.7  1996/08/13  02:23:03  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.6.6  1996/08/01  20:20:51  awu
 * CSCdi62194:  %SYS-3-INVMEMINT:Invalid memory action; %SYS-2-MALLOCFAIL:
 * Memory al
 * Branch: California_branch
 *  -introduce reg_invoke_atm_vc_activated()/deactivated(), for the
 *   callback of activating/deactivating a PVC.
 *
 * Revision 3.1.6.5  1996/07/02  23:44:34  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 3.1.6.4  1996/06/29  22:35:58  rzagst
 * CSCdi59683:  OAM cells are not transmitted after ATM int was shut/noshut
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/06/18  16:44:01  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/06/05  02:00:08  sakumar
 * Added dummy function atmSyn_verify_qos_params() and defined
 * atm->burstcells_factor to 1 to prevent Cat5k ATM module crashes.
 * This module currently does not support QOS.
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/05/09  14:41:08  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.52.4  1996/05/02  20:27:50  sakumar
 * Initialize def_config parameters for init VPI/VCI bits configuration.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.52.3  1996/05/01  22:55:20  venkat
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * Cleaned up code (within #if 0 and endif)
 *
 * Revision 3.1.52.2  1996/04/29  22:21:46  sakumar
 * Changes to Cat5K ATM HW specific code for IOS to link after latest sync.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.52.1  1996/04/27  07:25:22  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.40.1  1996/04/08  02:08:56  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.24.5  1996/03/27  21:55:25  sakumar
 * Got rid of some compiler warnings.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.4  1996/03/26  21:15:42  sakumar
 * Use total_in_pkts and total_out_pkts instead of in_pkts and out_pkts.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.3  1996/03/23  04:54:42  rlowe
 * More sync-time fun and games.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.2  1996/03/22  23:40:11  rlowe
 * Give relative paths to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.1  1996/03/22  09:43:41  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.2.5  1996/03/21  00:21:15  sakumar
 * Fix for PVC setup when line-state changes.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.2.4  1996/03/20  14:15:49  sakumar
 * Tracking changes in PVC setup in atm.c for Cat5K ATM Module.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.2.3  1996/03/20  10:48:48  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Also fold old Synergy atmtest.c content into vcdtest.c
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.2.2  1996/03/20  03:12:01  sakumar
 * Apply ATM line card 2.1 fixes to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.2.1  1996/03/13  03:18:36  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1  1996/03/11  08:15:23  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
 *
 * File: ios_atmdriver.c
 *
 * Description: IOS hooks to ATM driver interface
 *
 * ALL RIGHTS RESERVED. THESE SOURCE FILES ARE THE SOLE PROPERTY
 * OF CISCO SYTEMS, Inc. AND CONTAIN CONFIDENTIAL  AND PROPRIETARY
 * INFORMATION.  REPRODUCTION OR DUPLICATION BY ANY MEANS OF ANY
 * PORTION OF THIS SOFTWARE WITHOUT PRIOR WRITTEN CONSENT OF
 * CISCO SYSTEMS, Inc. IS EXPRESSLY FORBIDDEN.
 *
 */

/* Switch */
/* #define SAR_RESP_TIMEOUT_ENABLED */


/* Includes */
#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "sched.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../ui/parse_util.h"
#include "../parser/parser_defs_exec.h"
#include "address.h"
#include "logger.h"
#include "../os/buffers.h"
#include "../if/network.h"
#include "../wan/smds.h"
#include "../if/atm_debug.h"
#include "../if/atm.h"
#include "../atm/atm_oam.h"
#include "../atm/ilmi_public.h"        /* needed by atm_registry.h */
#include "../atm/atm_registry.h"
#include "../les/if_les.h"
#include "../os/timer_generic.h"
#include "../lane/lane.h"
#include "../parser/parser_defs_parser.h"

#include    "../../wbu/cat5k/alc_cominc/basic_types.h"
#include    "../../wbu/cat5k/alc_cominc/param.h"
#include    "../../wbu/cat5k/alc_cominc/queue_if.h"
#include    "../../wbu/cat5k/alc_cominc/buffer_if.h"
#include    "../../wbu/cat5k/alc_cominc/ivcimap.h"
#include    "../../wbu/cat5k/alc_cominc/ovcimap.h"
#include    "../../wbu/cat5k/alc_cominc/cam_if.h"
#include    "../../wbu/cat5k/alc_cominc/shram.h"
#include    "../../wbu/cat5k/alc_cominc/spram.h"
#include	"../../wbu/cat5k/alc_cominc/sar_if.h"
#include    "../../wbu/cat5k/alc_cominc/atmdrv_if.h"
#include    "../../wbu/cat5k/sysif/lec_ipc.h"
#include 	"ios_atmdriver.h"
#include 	"../../wbu/cat5k/alc_drivers/atmdvr_gdata.h"
#include 	"../../wbu/cat5k/alc_include/lcp_if.h"
#include	"../../wbu/cat5k/alc_include/ntc_client.h"



/* Globals */

hwidbtype * atmSyn_hwidb; 	/* Setup by atmSyn_init_idb() for use in receive function*/
boolean atmSyn_debugPacket;
void atmSyn_line_state_change (int frame_status);
void SynALC_led_init (void) ;
void ipc_init (void) ;
void Syn_ipc_init (void) ;


/*
 * Synergy ATM Card ATM Subsystem Header
 */
#define ATMSYN_MAJVERSION 1
#define ATMSYN_MINVERSION 0
#define ATMSYN_EDITVERSION 1

SUBSYS_HEADER(atmSyn, ATMSYN_MAJVERSION, ATMSYN_MINVERSION, ATMSYN_EDITVERSION,
			  atmSyn_subsys_init, SUBSYS_CLASS_DRIVER,
			  NULL,
			  NULL);

void atmSyn_dump_buffer(uchar *pdata, int length)
{
	int i, j;

	j = 0;
	buginf("\natmSyn_dump_buffer:\n");
    while(length) {
		for(i=0; i<16 && length; i++, length--) {
			buginf("%02x ", pdata[j+i]);
		}
		j += 16;
		buginf("\n");
	}
}

/*
 * Synergy ATM Card Subsystem Initialization
 */
void
atmSyn_subsys_init (subsystype *subsys)		/* Subsystem struc ptr */
{
	/*
     * Register the NIM IDs for the various BSI FDDI NIMs
     */
    reg_add_analyze_interface (XX_ATM_SYN,
                               atmSyn_analyze_interface,
                               "atmSyn_analyze_interface");
    /*
     * Register hardware setup function
     */
    reg_add_media_hw_setup (atmSyn_setup, "atmSyn_setup");

    /*
     * Add final startup routine
     */
    reg_add_if_final_init (atmSyn_if_final_init, "atmSyn_if_final_init");

    atmSyn_analyze_interface (0) ;

}

void
atmSyn_invoke_media_hw_reset(void)
{

	reg_invoke_media_hw_reset(atmSyn_hwidb);

}


/*
 * Analyze Synergy ATM Card in the given slot  (XXX ??)
 */
void
atmSyn_analyze_interface(int slot) /* Slot number */
{
	atmSyn_init_idb(slot, natmnets);

#ifdef COMMENTED_OUT
	sscop_init(); 		/* XXX remove this if driver started normally */
	atmSig_init();		/* XXX remove this if driver started normally */
#endif
	ipc_init();
	Syn_ipc_init();

	/*
     * Register interrupt handlers
     */
	/* XXX Currently hardwired */

	/*
     * Add this NIM to the number of ATM interfaces
     */
    natmnets++;
}



hwidbtype *
atmSyn_init_idb(int slot,  /* XXX Box slot number */
				int	unit)  /* XXX ATM unit number */
{

	hwidbtype	*idb;
	ulong		Val = 0 ;
    uchar c;
	
	/* 
	 * Allocate hardware IDB memory block
	 */
	idb = idb_create();
	if (!idb) {
		buginf("\natmSyn_init_idb: Could not allocate hwidb");
        return (NULL);
	}	

	/* 
	 * Initialize the low-level shared-memory structures (IOS Independent)
	 */
	if ( (c = ALC_InitAtmDriver ()) == FALSE) {
		buginf("\natmSyn_init_idb: ATM Driver inititialization failed");
        return (NULL);
    }

	/*
	 * Register upcall with NTC driver for line state change
	 */
	if (pmd_chip_type == PMD_PMC) {
		pmc_Reg_line_state_change_upcall (atmSyn_line_state_change) ;
	} else {
		ntc_Reg_line_state_change_upcall (atmSyn_line_state_change) ;
	}

	/*
	 * Initialize hardware IDB
	 */
    idb->unit = unit;
    idb->subunit = unit;
    idb->slot = slot;
	idb->type = IDBTYPE_SYNALC_ATM;
    idb->name = IFNAME_ATM;
    idb->status = IDB_ATM | IDB_SYNC ;
    idb->counters.resets = 0; 
	if ((LCP_Ntwk_MacAddr ((tLCP_MacAddrU *) idb->bia, & Val) == NULL) ||
		(LCP_Ntwk_MacAddr ((tLCP_MacAddrU *) idb->hardware, & Val) == NULL))
	{
		buginf("\natmSyn_init_idb: Could not get network-side MAC addr blk");
        return (NULL);
	}
    idb->typestring  = "Catalyst 5000 ATM";
#ifdef XXX /* not sure what to fill in here */
    idb->type = IDBTYPE_ATMZR;
    idb->nim_type = get_nim_type (slot);
    idb->nim_version = get_nim_version (slot);
    idb->error_threshold = 5000;
    idb->error_count = 0;
#endif /* XXX */

    /* Encapsulation parameters */
    idb->maxmtu_def = idb->maxmtu = ACP_RX_BUFSZ; /* XXX */
    idb->span_encapsize = idb->encsize;			  /* XXX where was encsize setup ? */


	/* IDB functions */
    idb->soutput = atmSyn_output;
    idb->reset = atmSyn_reset;
    idb->shutdown = atmSyn_shutdown;
    idb->lineaction = atmSyn_line_action;
    idb->linestate = atmSyn_line_state;
    idb->enable = atmSyn_enable;
    idb->clear_support = atmSyn_clear_counters;
    idb->system_configured = atmSyn_system_configured;
    idb->show_controller = atmSyn_show_controller;
    idb->fastsend = atmSyn_fastsend;
	idb->set_maxdgram = atmSyn_set_maxdgram;

    /* Time stamps */
    TIMER_STOP (idb->lastinput);
    TIMER_STOP (idb->lastoutput);
    TIMER_STOP (idb->lastreset);

#ifndef	COMMENTED_OUT
	reg_invoke_media_atm_setup (idb) ;
	/* atm_setup    (idb) ; */
	atmSyn_setup (idb) ;
#endif

#ifdef ACP_IOS_102 /* done in atm_setup() in IOS 10.3 */
	idb->atm_lineup = TRUE;
#endif /* ACP_IOS_102 */


	idb->atm_db->flags |= ATM_FLAG_UP;


	/*
	 * Enqueue newly created IDB
 	 */
	idb_enqueue(idb);

	atmSyn_hwidb = idb;

	/* XXX OK to do this here? */
	net_cstate(idb, IDBS_UP);

	return(idb);
}


void
atmSyn_setup (hwidbtype *idb)
{
	atm_db_t            *atm;       /* Configuration database ptr */
    idbtype         	*swidb;     /* Interface software IDB ptr */
	int 				phy_media_type; /* Physical Media Type */

    /*
     * Initialize local auxiliar variables
     */
    atm = idb->atm_db;
    swidb = idb->firstsw;

    /*
     * Set output hold queue size
     */
    holdq_init (idb, ATMSYN_HOLDQ_SZ);

	/*
	 * Set options
	 */
	atm->flags |= ATM_FLAG_PVC;
    atm->burstcells_factor = 1; /* We don't support QOS */

	/*
	 * Init ATM database structure
	 */
	atm->config.mtu = ACP_TX_BUFSZ - ATM_ENCAP_HDRBYTES;
	atm->config.max_intf_vcs = MAX_DEFAULT_VPC;		/* Total number of VCs */
	atm->config.mode = ATM_CFG_MODE_AAL5; 			/* AAL5 only */
	atm->config.rx_buff = NUM_RXSAR_ACP_BUFS;
	atm->config.tx_buff = NUM_ACP_TX_DATA_BUFS;
	atm->config.exq = ATM_EXCQ_DEFAULT;
	atm->config.raw_queue_size = ATM_RAWQ_DEFAULT;
    atm->def_config.rt_vc_per_vp =
	atm->config.rt_vc_per_vp = (1 << VCI_BITS_C);	/* Number of VCIs per VPI */
	atm->config.vp_filter = 0; 						/* XXX ?? */
	atm->config.rx_vc = atm->config.max_intf_vcs;	/* Same as TX */
	atm->config.mid_per_vc = 0;						/* Since AAL5 only */

	atm->vc_hash_tbl = NULLP;	/* Will be allocated when first VC created */

	/*
     * Set Physical Layer characterictics
	 */
	atm->plimtype = ATM_PLIM_SONET;
	atm->plimrate = PLIM_SONET_RATE;

	/* Get physical media type */
	phy_media_type = rd_pmd_mode();
	switch (phy_media_type) {
		case PMD_MODE_MMF:
			atm->medium = ATM_MEDIA_MULTI_FIBER;
			break;
		case PMD_MODE_SMF:
			atm->medium = ATM_MEDIA_SINGLE_FIBER;
			break;
		case PMD_MODE_UTP:
			atm->medium = ATM_MEDIA_UTP;
			break;
		case PMD_MODE_INVALID:
		default:
			buginf("\n\r *** WARNING: PMD EEPROM invalid; assuming Multi-Mode Fiber");
			atm->medium = ATM_MEDIA_MULTI_FIBER;
			break;
	}

	/*
	 * Init interface function pointers
	 */
    atm->funcs.reset = atmSyn_reset;
    atm->funcs.rawinput = atmSyn_raw_input;
    atm->funcs.setupvc = atmSyn_setup_vc;
    atm->funcs.teardownvc = atmSyn_teardown_vc;
    atm->funcs.encapsulate = atmSyn_encaps;
    atm->funcs.periodic = atmSyn_periodic;
    atm->funcs.sniff = atmSyn_sniff;
    atm->funcs.qos_params_verify = atmSyn_verify_qos_params;

#ifndef ACP_IOS_102
    atm->funcs.getvc_stats = atmSyn_get_vc_stats;
#endif /* NOT ACP_IOS_102 */

	atm->def_config.max_vpi_bits = atm->config.max_vpi_bits = VPI_BITS_C;
	atm->def_config.max_vci_bits = atm->config.max_vci_bits = VCI_BITS_C;

    /*
     * Set software IDB attributes
     */
	set_default_interface_bandwidth(swidb, ATMSYN_VISIBLE_BANDWIDTH_SONET);
	set_default_interface_delay(swidb, ATMSYN_SONET_DELAY);

    idb->buffersize = atm->config.mtu;
    idb->max_pak_size_def = atm->config.mtu;
    if_maxdgram_adjust(idb, atm->config.mtu);
 
    /*
     * Set indication that configuration changed
     * so that NIM gets the update.
     */
    atm->config_status = ATM_CONFIG_CHANGED;
    GET_TIMESTAMP (atm->config_time);
}


boolean
atmSyn_setup_vc(hwidbtype *idb,			/* Interface HW IDB ptr */
				vc_info_t *vcinfo)		/* VC info ptr */
{
    tVPC retcode;
    UINT16 rxsar_state; 

	if (vcinfo->vpi >= (1 << num_active_vpi_bits) ||
		vcinfo->vci >= (1 << num_active_vci_bits)) {

		/* VPI or VCI is outside range */
		buginf("\natmSyn_setup_vc: VPI/VCI out of range");
		
		return(FALSE);
	
	}

	/*
	 * Determine RXSAR State (i.e. where RXSAR must send the packet)
	 */
	switch(vcinfo->flags & VC_FLAG_AAL_MASK) {
		case ATM_VC_AALQSAAL:
			rxsar_state = VPC_DEST_ACP;
			break;
		case ATM_VC_AAL5LANE:
    		switch (vcinfo->user_vc_type) {
    			case LANE_LEC_DATA:
					rxsar_state = VPC_DEST_BIGA;	
					break;
    			case LANE_LEC_FORWARD:
			    case LANE_LEC_SEND:
					rxsar_state = VPC_DEST_BIGA_FROM_BUS;	
					break;
			    case LANE_BUS_SEND:
					rxsar_state = VPC_RCV_FOR_BUS;
					break;
    			case LANE_LEC_DIRECT:
    			case LANE_LEC_DISTRIBUTE:
    			case LANE_LEC_CONFIGURE:
    			case LANE_LES_DIRECT:
			    case LANE_LES_DISTRIBUTE:
			    case LANE_CONFIGURE:
			    case LANE_BUS_FORWARD:
					rxsar_state = VPC_DEST_ACP;	
					break;
    			default:
        			return(FALSE);
    		}
			break;
		case ATM_VC_AAL5ILMI:
			rxsar_state = VPC_DEST_ACP;
			break;
        case ATM_VC_AAL5SNAP:
            rxsar_state = VPC_SNAP_PVC_TO_BIGA;
            break;
		default:
			/* For Synergy, we only accept the above 3 cases */
			return(FALSE);
	}

    if ( (retcode = aalsap_open_vcc(0,                      /* UINT16 portindex   */
									vcinfo->vc,				/* VCD				  */
                                    vcinfo->vpi,            /* tVPI vpi           */
    								vcinfo->vci,            /* tVCI vci           */
                                    0,                      /* tQOS qos           */
                                    VPC_TX_ACTIVE,          /* txsar_state		  */
									rxsar_state,            /* rxsar_state		  */
									0,                      /* VLAN               */
									0xaabb,					/* LECID 			  */
                                    atmSyn_proc_input_pkt) ) == 1 ) {

		/* Mark VC as active */
		vcinfo->flags |= VC_FLAG_ACTIVE;

	    if ((vcinfo->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5SNAP) {
            alc_mark_PVC_as_aal5snap(vcinfo->vc);
        }

        /*
         * If Peak & Avg. rates aren't specified, then just put this guy into
         * the fastest RateQ by setting flag for default.
         */
        if (vcinfo->peakrate == 0 && vcinfo->avgrate == 0) {
            vcinfo->flags |= (VC_FLAG_DEFAULT_RATE | VC_FLAG_DEFAULT_BURST);
        }
		return(TRUE);
	} else {
		return(FALSE);
	}
}


boolean
atmSyn_teardown_vc (hwidbtype *idb,		/* Interface HW IDB ptr */
           			vc_info_t *vcinfo)	/* VC struct ptr */
{
	tVPC vpc;
    UINT16 vlan;

    if ((vcinfo->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5SNAP) {
        if (0 != (vlan = alc_get_vlan_from_PVC(vcinfo->vc))) {
            if (alc_unbind_PVC_from_vlan(vlan, vcinfo->vc) <= 0) {
                buginf("\natmSyn_teardown_vc: Error in deleting PVC");
            }
        }
    }

	vpc = atm_get_vpc(vcinfo->vci, vcinfo->vpi);
	aalsap_close_vcc(vpc, 0);

	vcinfo->flags &= ~VC_FLAG_ACTIVE;

	return(TRUE);
}


/*
 * a t m S y n _ e n c a p s  ( )
 *
 * Returns: platform-specific encapsulation length
 */
unsigned
atmSyn_encaps (	atm_db_t *atmdb,      /* ATM structure ptr */
          		paktype *pak,         /* Packet header ptr */
          		vc_info_t *vc,        /* Associated VC struct ptr */
          		ushort txopt)         /* Transmit options */
{

	tVPC vpc;
 
    /*
     * Prepend ATM encapsulation (VcId/Encaps)
     */
    pak->datagramstart -= ATM_ENCAP_HDRBYTES;
    pak->datagramsize += ATM_ENCAP_HDRBYTES;

    /*
     * Check for special OAM/ILMI traffic
     */
    if (txopt & TXOPT_OAM) {
        vpc	= 0;          /* Default to 0 for OAM stuff */
    } else {
		/* XXX can we use vc->vc ? */
		vpc = atm_get_vpc(vc->vci, vc->vpi);

    }

    /* Put the VPC number at start of packet */
    ((atm_encap_t *)pak->datagramstart)->vcnum = vpc;
 
    return (ATM_ENCAP_HDRBYTES);
}


void
atmSyn_output (hwidbtype *idb)           /* Interface HW IDB ptr */
{

	paktype		*pak;
	tVPC		vpc;
	UINT32		datalen;
	int			retcode;
	void	    *pData;

	/* Send out all packets on queue before returning */
	while (idb->state == IDBS_UP) {
		pak = (*idb->oqueue_dequeue)(idb);
		if (pak == NULL) {	/* Nothing to transmit */
			return;
		}

		vpc = ((atm_encap_t *)pak->datagramstart)->vcnum; /* put in by atmSyn_encaps */
		datalen = pak->datagramsize - ATM_ENCAP_HDRBYTES;
		pData = pak->datagramstart + ATM_ENCAP_HDRBYTES;

		if ( (retcode = aalsap_send(vpc, 0, datalen, pData)) ) {
			/* Error */
			switch(retcode) {
				case -1:
					buginf("\natmSyn_output: no buffers, dropping");
					break;
				case -2:
					buginf("\natmSyn_output: bad port, dropping packet");
					break;
				case -3:
					buginf("\natmSyn_output: bad VC, dropping packet");
					break;
				case -4:
					buginf("\natmSyn_output: packet too long, dropping");
					break;
				default:
					buginf("\natmSyn_output: unknown retcode $d", retcode);
					break;
			}
		} 
		datagram_done(pak);
	}
}


void
atmSyn_proc_input_pkt(UINT16 vcd,
					  UINT16 event,
					  UINT32 length,
					  void   *pData)
{
	vc_info_t	*vc;	/* VC Info ptr */
	iqueue_t	iqueue;	/* Queue to process packet */
	paktype		*pak;

	if ( (pak = getbuffer(length + ATM_ENCAP_HDRBYTES)) ) {
		/* Copy data to pak buffer (in DRAM) from Frame Buffer memory */
		bcopy (pData, (pak->datagramstart + ATM_ENCAP_HDRBYTES), length);
		pak->datagramsize = length;
	} else {
		/* Couldn't get packet */
		return;
	}

	if (atmSyn_debugPacket) {
		atmSyn_dump_buffer((pak->datagramstart + ATM_ENCAP_HDRBYTES), length);
	}

	/* Get pointer to VC Info structure */
	if (!(vc = atm_getvc(atmSyn_hwidb->atm_db, vcd))) {
		datagram_done(pak);
		return;
	}

	/* Stuff vc info pointer into idb */
	atmSyn_hwidb->atm_db->fast_vc_info = vc;

	/*
	 * Update packet header
	 * NOTE: On entry, datagramstart points to start of packet; 
	 *       datagramsize has been set to length in AAL5 trailer.
	 */
	pak->datagramsize += ATM_ENCAP_HDRBYTES;
	pak->if_input = vc->swidb;
	((atm_encap_t *) (pak->datagramstart))->vcnum = vcd;
	((atm_encap_t *) (pak->datagramstart))->enctype = vc->etype;

	iqueue = (*atmSyn_hwidb->parse_packet)
 							   (atmSyn_hwidb, pak, NULL);	/* atm_parse_packet() */

	if (iqueue) {
		(*iqueue) (atmSyn_hwidb, pak);						/* atm_input() */
	} else {
		buginf("\natmSyn_proc_input_pkt: Packet for unknown VC dropped");
		datagram_done(pak);
	}
}

void
atmSyn_if_final_init()
{ }


/*
 * a t m S y n _ l i n e _ s t a t e  ( )
 *
 * Report line state state.
 */
boolean
atmSyn_line_state (hwidbtype *idb)       /* Interface HW IDB ptr */
{
    return (idb->atm_lineup);
}

void 
atmSyn_system_configured(hwidbtype *idb) 
{ 
	/* Start the LED State machine */
	SynALC_led_init();

    if (idb->state != IDBS_ADMINDOWN) { 
        atmSyn_reset(idb);
    }
}


boolean
atmSyn_get_vc_stats(hwidbtype *idb, vc_info_t *vc)
{ 
	tVPC vpc;
	int ndx;
	vc_info_t *vc1;

	/*
	 * Check if a specific VC is being addressed
	 */
	if (vc && (-1 != (int) vc) ) {
		/*
		 * Report VC statistics
		 */
		vpc = atm_get_vpc(vc->vci, vc->vpi);

		if (aalsap_is_vpc_active(vpc) == TRUE) {
			vc->total_in_pkts = acp_pRxSpram->rxstat_vc[vpc].rxvc_packet_input;
			vc->total_out_pkts =acp_pTxSpram->txstat_vc[vpc].txvc_packet_output;
			vc->in_bytes = acp_pRxSpram->rxstat_vc[vpc].rxvc_ntccell_input*48;
			vc->out_bytes = acp_pTxSpram->txstat_vc[vpc].txvc_ntccell_output*48;
		} else {
			return(FALSE);
		}
	} else {
		/*
	 	 * All VCs status requested
	 	 */
		if (idb->atm_db->vc_hash_tbl) {
			for(ndx = 0; ndx < ATM_HASH_DEFAULT; ndx++) {
        		vc1 = (vc_info_t *)idb->atm_db->vc_hash_tbl[ndx];
        		for(; vc1 && (-1 != (int) vc1); vc1 = vc1->next_vc) {
					/* Yes, this is a recursive call. Got any better ideas? */
					if (FALSE == atmSyn_get_vc_stats(idb, vc1)) {
						buginf("\natmSyn_get_vc_stats: VPC VCD mismatch!");
					}
				}
			}
		}
	}
 
	return(TRUE);
}

void
atmSyn_line_action(hwidbtype *idb)
{
    /*
     * Update line state
     */
    idb->atm_lineup = (idb->state == IDBS_UP) ? TRUE : FALSE;

	/*
	 * The AIP does this, but not the c4000 atmzr. However, if this
	 * is not done, then atm_reset is not called when the line goes down.
	 * As a result, old SVCs stay around preventing lane from coming up.
	 * If the line is down, inform upper layers so that they can do
	 * cleanup actions
	 */
	if(!idb->atm_lineup) {
		reg_invoke_media_hw_reset(idb);
	}
}
 
void
atmSyn_shutdown(hwidbtype *idb, int newstate)
{
    /*
     * Reset NIM and update interface state
     */
    if (newstate == IDBS_ADMINDOWN) {
        /* Inform upper layers that some cleanup may be necessary */
        reg_invoke_media_hw_reset (idb);
    }
    net_cstate(idb, newstate);
}
 
void
atmSyn_reset(hwidbtype *idb)
{
    /*
     * Inform upper layers that some cleanup may be necessary
     */
    reg_invoke_media_hw_reset (idb);
 
    /*
     * Make sure interface is not admin down
     */
    if (idb->state == IDBS_ADMINDOWN)
        return;

	/* Now check to see if interface can be brought back up */
	if (pmd_chip_type == PMD_PMC) {
    	if (pmc_get_framer_status() == IN_FRAMING) {
       		net_cstate(idb, IDBS_UP);
            atmSyn_config(idb); 
    	} else {
        	net_cstate(idb, IDBS_DOWN);
    	}
	} else {
    	if (ntc_get_framer_status() == IN_FRAMING) {
       		net_cstate(idb, IDBS_UP);
    	} else {
        	net_cstate(idb, IDBS_DOWN);
    	}
	}
 
    idb->counters.resets++;
}
 
void
atmSyn_periodic(hwidbtype *idb)
{ 
    atm_db_t *atm = idb->atm_db;
 
    if (idb->state != IDBS_ADMINDOWN) {
	switch (atm->config_status) {
	case ATM_CONFIG_CHANGED:
	    atmSyn_config(idb);
	    break;
	default:
	    atm->config_status = ATM_CONFIG_OK;
	    break;
        }
    }	

    /* Copy global statistics from SAR's Secondary Port RAM to hwidb */
    idb->counters.inputs = acp_pRxSpram->rxstat_gl.rxgl_packet_input;

    /* Note this is CELLS dropped due to no buffer available */
    idb->counters.input_resource = acp_pRxSpram->rxstat_gl.rxgl_cell_no_buf_err;

    idb->counters.input_error = acp_pRxSpram->rxstat_gl.rxgl_packet_rto_err +
	                        acp_pRxSpram->rxstat_gl.rxgl_packet_crc_err + 
				acp_pRxSpram->rxstat_gl.rxgl_packet_bufovl_err +
				acp_pRxSpram->rxstat_gl.rxgl_packet_bigaovl_err;

    idb->counters.outputs = acp_pTxSpram->txstat_gl.txgl_upacket_output;

    /* Cell Stats only for PMC */
    if (pmd_chip_type == PMD_PMC) {
	idb->counters.rx_cumbytes = AtmdrvGcb.rxcellcount*48;
	idb->counters.tx_cumbytes = AtmdrvGcb.txcellcount*48;
    }
}

/* This function is called by the ntc interrupt handler if line state changes */
void
atmSyn_line_state_change(int frame_status)
{
	enum IDB_STATES	newstate; /* New interface state */

	newstate = (frame_status == IN_FRAMING) ? IDBS_UP : IDBS_DOWN;

	if ( (atmSyn_hwidb->state != newstate) &&
		 (atmSyn_hwidb->state != IDBS_ADMINDOWN) ) {
		net_cstate(atmSyn_hwidb, newstate);
		atmSyn_hwidb->counters.transitions++;

        if (newstate == IDBS_UP) {
            atmSyn_config(atmSyn_hwidb); 
        }
	}
}

boolean
atmSyn_show_controller(hwidbtype *idb, parseinfo *p)
{
	printf("\nCatalyst 5000 ATM card Statistics");
	if (pmd_chip_type == PMD_PMC) {
		printf("\nNumber of Cells Received: %d ", AtmdrvGcb.rxcellcount);
	}
	printf("\nNumber of Frames Received: %d ",
                acp_pRxSpram->rxstat_gl.rxgl_packet_input);
	printf("\nNumber of received Cells dropped due to Unknown VC: %d ",
                acp_pRxSpram->rxstat_gl.rxgl_cell_unknownvc_err);
	printf("\nNumber of received Cells dropped because buffer unavailable: %d ",
                acp_pRxSpram->rxstat_gl.rxgl_cell_no_buf_err);
	printf("\nNumber of received Frames dropped due to Re-Assembly Timeout: %d "
,
                acp_pRxSpram->rxstat_gl.rxgl_packet_rto_err);
	printf("\nNumber of received Frames dropped due to CRC Error: %d ",
                acp_pRxSpram->rxstat_gl.rxgl_packet_crc_err);
	printf("\nNumber of received Frames dropped due to Buffer Overflow: %d ",
                acp_pRxSpram->rxstat_gl.rxgl_packet_bufovl_err);
	printf("\nNumber of received Frames dropped due to BIGA Overflow: %d ",
                acp_pRxSpram->rxstat_gl.rxgl_packet_bigaovl_err);
	printf("\nNumber of received Frames dropped due to bad AAL5 length: %d ",
                acp_pRxSpram->rxstat_gl.rxgl_packet_bad_len_err);

	if (pmd_chip_type == PMD_PMC) {
		printf("\n\nNumber of Cells Transmitted: %d ", AtmdrvGcb.txcellcount);
	}
	printf("\nNumber of Frames Transmitted: %d ",
                acp_pTxSpram->txstat_gl.txgl_upacket_output);
	printf("\nNumber of Broadcast Frames Transmitted: %d ",
                acp_pTxSpram->txstat_gl.txgl_bpacket_output);
	printf("\nNumber of Floods to color : %d ",
                acp_pTxSpram->txstat_gl.txgl_unknownflood_output);
	printf("\nNumber of start LEARP Events: %d ",
                acp_pTxSpram->txstat_gl.txgl_start_learp);

	printf("\n\nNumber of IPC Frames Received: %d ",
                acp_pTxSpram->txstat_gl.txgl_packet_acp);
	printf("\nNumber of IPC Frames Transmitted: %d ",
				acp_pRxSpram->rxstat_gl.rxgl_packet_nmp);

    printf("\n\nPMD Error Counts:");
    printf("\nSection BIP-8 errors (B1): %d", AtmdrvGcb.B1_err);
    printf("\nLine BIG-8/24 errors (B2): %d", AtmdrvGcb.B2_err);
    printf("\n    Line FEBE errors (Z2): %d", AtmdrvGcb.Z2_err);
    printf("\n   Path BIP-8 errors (B3): %d", AtmdrvGcb.B3_err);
    printf("\n    Path FEBE errors (G1): %d", AtmdrvGcb.G1_err);
    printf("\n   Correctable HCS errors: %d", AtmdrvGcb.CHCS_err);
    printf("\n Uncorrectable HCS errors: %d", AtmdrvGcb.UHCS_err);
    printf("\n  Number of OOF Intr rcvd: %d", AtmdrvGcb.OOF_count);
    printf("\n  Number of LOF Intr rcvd: %d", AtmdrvGcb.LOF_count);

	return(FALSE);
}

/*
 * When the CLI causes a new config command, the periodic polling
 * functions catches that and this function is called.
 */
void
atmSyn_config (hwidbtype *idb)
{
    ushort		indx;
    vc_info_t	*vc;
    atm_db_t	*atm;

	/* Currently only for the vc_per_vp command */
	if (0 == aalsap_vc_per_vp(idb->atm_db->config.rt_vc_per_vp)) {
		buginf("\natmSyn_config: VCI per VPI command failed");
	}

    /*
     * Reopen Active PVCs
     */
    atm = idb->atm_db;
    atm->config_status = ATM_CONFIG_IN_PROGRESS;
    for (indx=1; indx<atm->config.max_intf_vcs; indx++) {
        vc = atm_getvc (atm, indx);
        if (vc && (vc->flags & VC_FLAG_PVC)) {
            vc->flags |= VC_FLAG_ACTIVE;
		    atm->num_vc++;
		    atm_oam_enable_timer(vc,vc->oam);
		    reg_invoke_atm_vc_activated(vc);
        }
        if (vc && !(vc->flags & VC_FLAG_PVC) && ISVCACTIVE(vc)) {
            vc->flags &= ~VC_FLAG_ACTIVE;
            atm->num_vc--; /* Take SVCs down */
        }
    }
	atm->config_status = ATM_CONFIG_OK;
}


ushort *
atmSyn_sniff (hwidbtype *hwidb, paktype *pak, uchar *data,
             ushort *sniff_buffer, int offset, int num_shorts)
{
    return((ushort *)pak->datagramstart + offset);
}

/*
 * The following functions are not implemented
 */

void
dummy_break(void) {}

boolean 
atmSyn_set_maxdgram(hwidbtype *idb,             /* Interface HW IDB ptr */
                    int buffersize,             /* Max buffer size */
                    int maxdgram)       
{ return (TRUE); }

void 
atmSyn_enable(hwidbtype *idb) 
{ buginf("\natmSyn_enable: function not implemented"); dummy_break(); }

void 
atmSyn_clear_counters(hwidbtype *idb) 
{ buginf("\natmSyn_clear_counters: function not implemented"); dummy_break(); }

void 
atmSyn_fastsend(hwidbtype *idb, paktype *pak) 
{ buginf("\natmSyn_fastsend: function not implemented"); dummy_break(); }

void
atmSyn_raw_input(hwidbtype *idb, paktype *pak)
{ buginf("\natmSyn_raw_input: function not implemented"); dummy_break(); }

uint
atmSyn_verify_qos_params(hwidbtype *hwidb,  /* atm interface hw type */
                        unsigned peakrate, /* proposed peak cell rate */
                        unsigned avgrate,  /* proposed sustainable cell rate*/
                        unsigned burstcells) /* proposed burst cell size */
{
    /* We don't support QOS yet */
    return(0);
}



/*
 * ATM driver wrappers for IOS "edisms".
 */

void
ADVR_DismissOnRxSar2 (void * pFunc, tpSARCMD pCmd)
{
	edisms ((blockproc *) pFunc, (UINT32) pCmd) ;

}	/* ADVR_DismissOnRxSar2 */

void
ADVR_DismissOnTxSar2 (void * pFunc, tpSARCMD pCmd)
{
	edisms ((blockproc *) pFunc, (UINT32) pCmd) ;

}	/* ADVR_DismissOnTxSar2 */


/*
*******************************************************************************
*
*   FUNCTION:
*           atmdrv_BLOCK
*
*   PARAMETERS:
*           tpSARCMD pCmd
*
*   RETURN:
*           TRUE
*			FALSE
*
*   ALGORITHMS
* 	Scheduler test for process that blocked in the ATM driver. The wakeup field
* 	is set to 1 by the Interrupt Handler.
*
*******************************************************************************
*/

tBOOLEAN
atmdrv_BLOCK(tpSARCMD pCmd)
{
	if (pCmd->cmd_wakeup){
		return(FALSE);					/* Wake up, response received. */ 
	}
	else {
		 return(TRUE);					/* Keep blocking, timer runs. */
	}
}/*end atmdrv_BLOCK */



/***************************************************
 * FUNCTIONS TO SUPPORT AAL5 SNAP PVCs
 ***************************************************/


/*
** alc_is_vlan_on_PVCs: Is this VLAN configured to run over PVCs?
**     return 0 if:
**         - vlan not configured at all
**         - vlan configured to run LANE
**         - vlan not configured to run over PVCs
**     else return 1
**
**     Note: Needs at least 1 PVC to be setup for this VLAN to return 1.
*/
int
alc_is_vlan_on_PVCs (UINT16 vlan)
{
    if (vlan >= MAX_COLOR || vlan == 0) {
        return (0);
    }
    if (ALC_PVC_LLC_ENCAP_C ==
         (acp_pTxSpram->LecTable[vlan].lece_busvpc & ALC_PVC_ENCAP_MASK_C)) {
        return (1);
    } else {
        return (0);
    }
}

/*
** alc_bind_PVC_to_vlan: Assign a PVC to a vlan
**     Error return (negative value) if:
**         - vcd out of range
**         - vcd not setup as aal5snap
**         - vlan already served by LANE
**     else return 1
**
**     Notes: If first PVC for VLAN, then sends port create msg to NMP.
*/
int
alc_bind_PVC_to_vlan (UINT16 vlan, UINT16 vcd)
{
    tVPC vpc;
    UINT16 tmp_vlan;

    if (vlan >= MAX_COLOR || vlan == 0) {
        return (ALC_ERR_BAD_COLOR_RANGE);
    }

    if (!alc_is_PVC_aal5snap(vcd)) {
        return (ALC_ERR_PVC_NOT_AAL5SNAP);
    }

    if (!aalsap_bind_vlan_to_vc( 0, vcd, vlan, VPC_SNAP_PVC_TO_BIGA)) {
        return (ALC_ERR_OTHER);
    }

    /* Is this VLAN already served by LANE ? */
    if ( (acp_pTxSpram->LecTable[vlan].lece_lecid != 0) ||
         ((acp_pTxSpram->LecTable[vlan].lece_busvpc & ~ALC_PVC_ENCAP_MASK_C) 
           != 0 ) ) {
        return (ALC_ERR_VLAN_ON_LANE);
    }

    /* Is this PVC already setup on a vlan ? */
    tmp_vlan = alc_get_vlan_from_PVC(vcd);
    if (tmp_vlan != 0) {
        /*
         * This PVC is already bound to a vlan, if vlan that 
         * we want then ok, otherwise return error
         */
        if (tmp_vlan == vlan) {
            return (ALC_RET_OK);
        } else {
            return (ALC_ERR_PVC_ON_OTHER_VLAN);
        }
    }

    /* 
     * All the checks passed, so add pvc to vlan 
     */

    /* first setup the PVC Info structure */
    vpc = get_vpc_from_vcd(0, vcd);
    /* higher order bits previously set so OR in vlan number */
    acp_pTxSpram->PVCTable[vpc].pvc_vlan |= vlan; 
    acp_pTxSpram->PVCTable[vpc].pvc_vpc = vpc;
    acp_pTxSpram->PVCTable[vpc].pvc_next = (struct pvc_info *) 0;

    /* Now add the PVC Info structure to end of PVCs linked list */
    if (alc_is_vlan_on_PVCs(vlan)) {

        tpPVC_info pPVC_Info;

        /* add PVC to end of linked list */
        pPVC_Info = MAP_SP_TX_ACP(acp_pTxSpram->LecTable[vlan].lece_pvc);

        while (pPVC_Info->pvc_next) {
            pPVC_Info = MAP_SP_TX_ACP(pPVC_Info->pvc_next);
        }
        pPVC_Info->pvc_next = MAP_SP_ACP_TX(&(acp_pTxSpram->PVCTable[vpc]));

    } else {
        /* If first PVC assigned to VLAN */
        acp_pTxSpram->LecTable[vlan].lece_lecid = 0;
        acp_pTxSpram->LecTable[vlan].lece_busvpc = ALC_PVC_LLC_ENCAP_C;
        acp_pTxSpram->LecTable[vlan].lece_pvc =
            MAP_SP_ACP_TX(&(acp_pTxSpram->PVCTable[vpc]));
  
        /* Send port create request to NMP */
	    leipc_send_spt_req(0, LEIPC_PORT_CREATE_REQ, vlan);
         
    }

    return (ALC_RET_OK);
}


/*
** alc_unbind_PVC_from_vlan: Remove a PVC from a vlan
**     Error return (negative value) if:
**         - vcd out of range
**         - vcd not setup as aal5snap
**         - vcd not part of this vlan
**         - vlan not setup to run over PVCs
**     else return 1
**
**     Notes: If deleting last PVC for VLAN, then send port destroy msg to NMP.
*/
int 
alc_unbind_PVC_from_vlan (UINT16 vlan, UINT16 vcd)
{
    tVPC vpc;

    if (vlan >= MAX_COLOR || vlan == 0) {
        return (ALC_ERR_BAD_COLOR_RANGE);
    }
   
    if (!alc_is_vlan_on_PVCs(vlan)) {
        return(ALC_ERR_VLAN_NOT_PVC_SUPP);
    }
    
    /* Return error if PVC does not belong to this vlan */
    if (vlan != alc_get_vlan_from_PVC(vcd)) {
        return (ALC_ERR_VLAN_PVC_MISMATCH);
    }

    vpc = get_vpc_from_vcd(0, vcd);
    if (acp_pTxSpram->LecTable[vlan].lece_pvc == 
           MAP_SP_ACP_TX(&(acp_pTxSpram->PVCTable[vpc]) ) ){
        /* If the PVC to be deleted is the first one in the linked list */
        acp_pTxSpram->LecTable[vlan].lece_pvc = 
                          acp_pTxSpram->PVCTable[vpc].pvc_next;
    } else {

        tpPVC_info pPVC_Info;

        pPVC_Info = MAP_SP_TX_ACP(acp_pTxSpram->LecTable[vlan].lece_pvc);
        while (pPVC_Info->pvc_next != 
                   MAP_SP_ACP_TX(&(acp_pTxSpram->PVCTable[vpc])) ) {
            pPVC_Info = MAP_SP_TX_ACP(pPVC_Info->pvc_next);
        }
        pPVC_Info->pvc_next = acp_pTxSpram->PVCTable[vpc].pvc_next;
    }

    acp_pTxSpram->PVCTable[vpc].pvc_next = NULL;
    acp_pTxSpram->PVCTable[vpc].pvc_vpc = 0;
    acp_pTxSpram->PVCTable[vpc].pvc_vlan = 0; 

    /* If this was last PVC assigned to the VLAN then send port down to NMP */
    if (acp_pTxSpram->LecTable[vlan].lece_pvc == 0 ) {
        acp_pTxSpram->LecTable[vlan].lece_lecid = 0;
        acp_pTxSpram->LecTable[vlan].lece_busvpc = 0;

        leipc_send_spt_req(0, LEIPC_PORT_DESTROY_REQ, vlan);
    }

    return (ALC_RET_OK);
}

/*
** alc_get_vlan_from_PVC: Returns vlan number given PVC's vcd
**     Error return 0 if:
**         - vcd out of range
**         - vcd not setup as aal5snap
**         - vcd not bound to any vlan
**     else return vlan_number
*/
UINT16 
alc_get_vlan_from_PVC (UINT16 vcd)
{
    tVPC vpc;
 
    if (0xffff == (vpc = get_vpc_from_vcd(0, vcd))) {
        /* vcd out of range */
        return (0);
    }

    if (!alc_is_PVC_aal5snap(vcd)) {
        return (0);
    }

    return (acp_pTxSpram->PVCTable[vpc].pvc_vlan & ~ALC_PVC_ENCAP_MASK_C);
}

/*
** alc_get_PVC_from_vlan: Returns PVC vcd from vlan. If prev_vcd (2nd parameter)
**                        is zero, then the first PVC is returned. If not
**                        zero, then the next PVC after prev_vcd is returned.
**     Error return 0 if:
**         - vlan not setup to run over PVCs
**         - prev_vcd out of range
**         - prev_vcd not setup as aal5snap
**         - prev_vcd not part of vlan
**     else return vcd
*/
UINT16 
alc_get_PVC_from_vlan (UINT16 vlan, UINT16 prev_vcd)
{
    tVPC vpc;
    UINT16 vcd;
    tVPC prev_vpc;
    tpPVC_info pPVC_Info;

    if (vlan >= MAX_COLOR || vlan == 0) {
        return (0);
    }

    if (!alc_is_vlan_on_PVCs(vlan)) {
        return (0);
    }

    if (0 == acp_pTxSpram->LecTable[vlan].lece_pvc) {
        return (0);
    }

    if (prev_vcd == 0) {
        pPVC_Info = MAP_SP_TX_ACP(acp_pTxSpram->LecTable[vlan].lece_pvc);
    } else {
        /* send vcd of PVC following prev_vcd */
        /* if prev_vcd 0, then return vcd of first PVC */
        if (vlan != alc_get_vlan_from_PVC(prev_vcd) ) {
            return (0);
        } 

        if (0xffff == (prev_vpc = get_vpc_from_vcd (0, prev_vcd))) {
            return (0);
        }
        if (acp_pTxSpram->PVCTable[prev_vpc].pvc_next == 0) {
            return (0);
        }
        pPVC_Info = MAP_SP_TX_ACP(acp_pTxSpram->PVCTable[prev_vpc].pvc_next);
    }

    vpc = pPVC_Info->pvc_vpc;
    vcd = get_vcd_from_vpc(0, vpc);

    if (0xffff == vcd) {
        /* vpc out of range */
        return (0);
    }

    return (vcd);
}

/*
** alc_is_PVC_aal5snap: Is PVC of type aal5snap?
**     return 0 if:
**         - vcd out of range
**         - vcd not of type aal5snap
**     else return 1 
*/
int 
alc_is_PVC_aal5snap (UINT16 vcd) {
    tVPC vpc;
 
    if (0xffff == (vpc = get_vpc_from_vcd(0, vcd))) {
        /* vcd out of range */
        return (0);
    }
 
    /* Check the upper 4 bits of the pvc_vlan field */
    if (ALC_PVC_LLC_ENCAP_C == 
            (acp_pTxSpram->PVCTable[vpc].pvc_vlan & ALC_PVC_ENCAP_MASK_C)) {
        return (1);
    } else {
        return (0);
    }
}

/*
** alc_delete_PVCs_for_vlan: Delete all PVCs for this VLAN.
**     Error return 0 if:
**         - vlan not configured at all
**         - vlan configured to run LANE
**         - vlan not configured to run over PVCs
**     else return 1 
**
**     Note: Sends port delete message to NMP too.
*/
int 
alc_delete_PVCs_for_vlan (UINT16 vlan)
{
    UINT16 vcd;

    if (!alc_is_vlan_on_PVCs(vlan)) {
        return (0);
    }
    while (0 != (vcd = alc_get_PVC_from_vlan(vlan, 0)) ) { 
        if (alc_unbind_PVC_from_vlan (vlan, vcd) <= 0) {
            buginf("\nalc_delete_PVCs_for_vlan: Internal error\n"); 
            return (0);
        }
    }
    return (1);
}

/*
** alc_mark_PVC_as_aal5snap: Done when PVC is setup initially
**     Error return 0 if:
**         - vcd out of range
**     else return 1
**
*/
int 
alc_mark_PVC_as_aal5snap (UINT16 vcd)
{
    tVPC vpc;

    if (0xffff == (vpc = get_vpc_from_vcd(0, vcd))) {
        /* vcd out of range */
        return (0);
    }

    /* 
     * For now, just set the upper bits in the pvc_vlan field to indicate
     * that this PVC is used as aal5snap. The actual vlan number will be
     * assigned when the bind is done.
     */
    acp_pTxSpram->PVCTable[vpc].pvc_vlan |= ALC_PVC_LLC_ENCAP_C;
    return (1);
}

/*
** alc_get_PVC_vlan: Get the next vlan that is configured to run over PVCs.
**     Error return 0 if:
**         - no such vlan exists
**     else return vlan_number
**
**     Notes: If parameter 0, then function returns the lowest numbered VLAN
**            that is configured to use PVCs, else it returns the next higher 
**            number VLAN that is configured to run over PVCs.
*/
UINT16
alc_get_PVC_vlan (UINT16 vlan)
{
    vlan++;

    while (vlan < MAX_COLOR) {
        if (alc_is_vlan_on_PVCs(vlan)) {
            return (vlan);
        }
        vlan ++;
    }

    return (0);
}
