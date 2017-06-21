/* $Id: rsp_svip.c,v 3.1.62.13 1996/09/09 19:06:08 kao Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_svip.c,v $
 *------------------------------------------------------------------
 * Versatile Interface Processor support code - SVIP version
 *
 * November 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_svip.c,v $
 * Revision 3.1.62.13  1996/09/09  19:06:08  kao
 * CSCdi61698:  show hardware reports 3 PAs after OIR
 * Branch: California_branch
 *
 * Revision 3.1.62.12  1996/08/28  13:14:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.62.11  1996/08/20  00:36:05  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.1.62.10  1996/08/19  19:56:02  ptran
 * Add ESA's idb to VIP and RSP hwidb/swidb chain.
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 3.1.62.9  1996/08/07  18:02:42  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.1.62.8  1996/07/26  01:11:12  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.1.62.7  1996/07/18  15:52:58  ptran
 * Free up all pa_info when svip is removed.
 * CSCdi62289:  show diag still show EPA info after the EPA has been
 * removed
 * Branch: California_branch
 *
 * Revision 3.1.62.6  1996/07/18  15:45:05  ptran
 * Backout the previous commit (3.1.62.5), it was committed under the wrong
 * bug id CSCdi56030.
 * Branch: California_branch
 *
 * Revision 3.1.62.5  1996/07/18  00:23:18  ptran
 * CSCdi56030:  Remove the support of bitflip mode from EPA driver.
 * Branch: California_branch
 *
 * Revision 3.1.62.4  1996/07/10  02:37:56  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/05/22  13:51:35  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/04/11  01:24:07  izhu
 * CSCdi53923:  SW-VIP2: Show diag doesnt pickup FDDI
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:50:12  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.5  1996/03/18  23:35:26  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.10.4  1996/03/13  09:24:22  rharagan
 * CSCdi51440:  Incomplete slot structure init for SVIP
 * Branch: ELC_branch
 * Fix small memory leak in remove ctrlr functions, svip and rvip, and
 * correctly setup pa_info element in svip slot structure.
 *
 * Revision 3.1.10.3  1996/02/27  22:46:56  fsunaval
 * CSCdi50071:  Addition to the SVIP family
 * Branch: ELC_branch
 *
 * Revision 3.1.10.2  1996/02/13  23:45:58  mbeesley
 * CSCdi48865:  Fix compilation of rsp_svip.c
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:18:45  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/27  20:14:25  getchell
 * Placeholder for VIPER development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "plural.h"
#include "subsys.h"
#include "logger.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "rsp_bufhdr.h"
#include "rsp_if.h"
#include "rsp_qasic.h"
#include "rsp_pcmap.h"
#include "sys_registry.h"
#include "packet.h"
#include "../hes/cbus_registry.h"
#include "media_registry.h"
#include "../hes/if_hes_common.h"
#include "rsp_common_if.h"
#include "../if/ether.h"
#include "../hes/if_cmd.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "lovenote.h"
#include "../hes/if_hes_ether.h"
#include "../hes/if_feip.h"
#include "../hes/if_vip.h"
#include "../hes/if_vip_4t.h"
#include "../ui/debug.h"
#include "../if/svip_debug.h"
#include "rsp_vip.h"

static uchar nSVIPs, nSVIPnets;
static uchar nSVIPfethernets, nSVIPethernets, nSVIPserialnets,
             nSVIPtringnets, nSVIPfddinets;


/*****************************************************************************
 * Name: svip_ctrlr_analyze
 *
 * Description: analyze part of initialization for an SVIP
 *   This function is called when a board is first discovered and initialized.
 *   It is called the first time the board is plugged in, or when the router
 *   is restarted.  It is *NOT* called for boards that are installed and
 *   running when another new board is inserted.
 *
 *   This function is responsible for discovering the interfaces
 *   on the VIP (via the CBUS -- select command), and creating
 *   and initializing the CDB and the IDB for those interfaces.
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP with the
 *     following fields filled in:
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the IP
 *       cmd_q_ptr -- pointer to the command queue for the IP
 *
 * Output:
 *   The following fields in the slottype (pointed to by slot) are changed
 *     interface_count -- number of interfaces on this IP
 *     interface_list -- list of pointers to IDB's for the interfaces 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void svip_ctrlr_analyze (slottype *slot)
{
    int port_num;
    ushort status;
    ushort interface_type;
    hwidbtype *hwidb;
    enum IDBTYPE type;
    ushort pa_indx; 
    pa_info_slots *pa_info_ptr;

    /*
     * Allocate space for the crash context
     */
    if (slot->crash_cntxt == NULL) {
	slot->crash_cntxt = malloc_named(DBUS_CRASH_LEN, "VIP crash context");
    }

    /*
     * We loop through the interfaces on the VIP selecting them as we go. 
     * We use the select command to tell if the interface is there.  If the
     * select command is successful, it returns the interface type.
     * If things go OK, we initialize the port.
     */
    for (port_num = 0; port_num < MAX_CTRLR_INTERFACES; ++port_num) {
	status = send_select_cmd(slot, port_num, &interface_type);
	if (status != FCI_RSP_OKAY) {
	    char name[CARDNAMELEN + 5]; /* room for name + slot number */

	    sprintf(name, "%s%u", slot->card_name, slot->slot_num);
	    errmsg(&msgsym(CMD, CBUS), "select", status, name);
	    break;
	}

	/* Allow for holes in the interface space */
	if (interface_type == FCI_TYPE_UNKNOWN){
	    slot->interface_list[port_num] = NULL;
            continue;
	}

	if (interface_type == FCI_TYPE_OUT_OF_RANGE)
	    break;

	/* 
	 * Get idb type and fci_type from VIP fci_type
	 */
	interface_type = vip2standard_types(interface_type, &type);

	if (interface_type == FCI_TYPE_UNKNOWN) {
	    errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
		   port_num, interface_type);
	    continue;
	}
	/*
	 * Get the IDB for the SVIP interface
	 * If we find a matching one, we can just use it,
	 * otherwise we init a new one.
	 */
	hwidb = rsp_get_matching_idb(slot, port_num,
				     interface_type, type);
	if (hwidb == NULL) {
	    break;		/* rsp_get_matching_idb() complains */
	}			/* about malloc() failure */

	/*
	 * Set up common fields.
         */
	slot->interface_list[port_num] = hwidb;
	slot->interface_count = 1 + port_num;
	pa_indx = (hwidb->slotunit < PA_VIRTUAL_SLOT_BOUNDARY) ? 0 : 1;
	if (!slot->pa_info[pa_indx]) {
	    slot->pa_info[pa_indx] =
		malloc_named(sizeof(pa_info_slots), "VIP PA info");
	    pa_info_ptr = slot->pa_info[pa_indx];
	    if (pa_info_ptr) {
	    	pa_info_ptr->pa_type = -1;
	    	pa_info_ptr->pa_ifcount = 0;
	    }
	}
	pa_info_ptr = slot->pa_info[pa_indx];

	switch(type) {
	    
	case IDBTYPE_FEIP:
	    rsp_fastether_idb_init(hwidb, TRUE);
	    if (pa_info_ptr) {
		pa_info_ptr->pa_type = interface_type;
	    }
	    nSVIPfethernets++;
	    break;
	    
	case IDBTYPE_CBUSETHER:
	    rsp_ether_idb_init(hwidb, TRUE);
	    if (pa_info_ptr)
		pa_info_ptr->pa_type = FCI_TYPE_VIP_ETHER8;
	    nSVIPethernets++;
	    break;
	    
	case IDBTYPE_CBUSSERIAL:
	    rsp_serial_idb_init(hwidb, TRUE);
	    if (pa_info_ptr) {
		pa_info_ptr->pa_type = FCI_TYPE_VIP_SERIAL4;
		if (!pa_info_ptr->private_pa_info) {
		    pa_info_ptr->private_pa_info = 
			malloc(sizeof(vip4t_privdata_t));
		}
	    }
	    nSVIPserialnets++;
	    break;
	    
	case IDBTYPE_CTR:
	    rsp_vip_4r_idb_init(hwidb);
	    if (pa_info_ptr)
		pa_info_ptr->pa_type = FCI_TYPE_VIP_TRING4;
	    nSVIPtringnets++;
	    break;
	    
	case IDBTYPE_FDDIT:
	    rsp_fddi_idb_init(hwidb, TRUE);
	    if (pa_info_ptr)
		pa_info_ptr->pa_type = FCI_TYPE_VIP_FDDI;
	    nSVIPfddinets++;
	    break;
	    
        case IDBTYPE_CYLEPA:
            rsp_esa_idb_init(hwidb, TRUE);
	    if (pa_info_ptr)
		pa_info_ptr->pa_type = FCI_TYPE_VIP_ESA;
            break;
	default:
	    break;
	}
	if (pa_info_ptr)
	    pa_info_ptr->pa_ifcount++;
	hwidb->status |= IDB_VIP;
    }
    ++nSVIPs;
    return;
}


/*
 * invoked when an SVIP has been removed from the system
 */
static void
svip_ctrlr_removed (slottype *sp)
{
    int ix;
    hwidbtype *hwidb;
    pa_info_slots *pa_info_ptr;

    /*
     * Free the slot's crash context
     */
    if (sp->crash_cntxt != NULL) {
	free(sp->crash_cntxt);
	sp->crash_cntxt = NULL;
	sp->crash_count = 0;
	sp->crash_timestamp = 0;
    }

    for (ix = 0; ix < SLOT_NUM_VIP_PAS; ix++) {
        pa_info_ptr = sp->pa_info[ix];
        if (pa_info_ptr) {
            if (pa_info_ptr->private_pa_info)
                free(pa_info_ptr->private_pa_info);
            free(pa_info_ptr);
            sp->pa_info[ix] = 0;
        }
    }

    for (ix = 0; ix < sp->interface_count; ++ix) {
	hwidb = sp->interface_list[ix];
	if (hwidb) {
	    --nnets;
	    --nSVIPnets;

	    switch(hwidb->type) {
	      case IDBTYPE_FEIP:
		--nfethernets;
		--nethernets;
		--nSVIPfethernets;
		break;

	      case IDBTYPE_CBUSETHER:
		--nethernets;
		--nSVIPethernets;
		break;
		
	    case IDBTYPE_CBUSSERIAL:
		--nserialnets;
		--nSVIPserialnets;
		break;
		
	    case IDBTYPE_CTR:
		--nTRs;
		--nSVIPtringnets;
		break;
		
	    case IDBTYPE_FDDIT:
		--nfddinets;
		--nSVIPfddinets;
		break;
		
	    default:
		break;
	    }
	}
    }
    --nSVIPs;
    return;
}

/*
 * display count of SVIPs in the system
 */
static void
svip_print_controllers (void)
{
    if (nSVIPs) {
	printf("%u VIP2 controller%s ", ArgAndPlural(nSVIPs, "", "s"));
	if (nSVIPfethernets)
	    printf("(%u FastEthernet)", nSVIPfethernets);
	if (nSVIPethernets)
	    printf("(%u Ethernet)", nSVIPethernets);
	if (nSVIPserialnets)
	    printf("(%u Serial)", nSVIPserialnets);
	if (nSVIPtringnets)
	    printf("(%u Token Ring)", nSVIPtringnets);
	if (nSVIPfddinets)
	    printf("(%u Fddi)", nSVIPfddinets);
	printf(".\n");
    }
}

/*****************************************************************************
 * Name: svip_platform_init
 *
 * Description: platform specific VIP init
 *
 * Input: 
 *   subsys -- pointer to the subsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void svip_platform_init (subsystype *subsys)
{
    nSVIPs = 0;
    nSVIPethernets = nSVIPserialnets =  nSVIPtringnets = nSVIPfethernets = 0;

    reg_add_ctrlr_analyze(FCI_SVIP_CONTROLLER,
			  svip_ctrlr_analyze, "svip_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_SVIP_CONTROLLER, vip_ctrlr_init, "vip_ctrlr_init");
    reg_add_if_init(FCI_SVIP_CONTROLLER, vip_if_init, "vip_if_init");
    reg_add_ctrlr_removed(FCI_SVIP_CONTROLLER,
			  svip_ctrlr_removed, "svip_ctrlr_removed");
    reg_add_print_controllers(svip_print_controllers, 
			      "svip__print_controllers");
    return;
}

/*
 * svip_show_cbus
 */
void svip_show_cbus (ushort slot_index)
{
    slottype *slotp;
    hwidbtype *hwidb;
    int ix;
    leveltype level;
    ushort rom_ver;
    
    slotp = &slots[slot_index];
    /* 
     * If the card is disabled, we have no relevant
     * information to report.  Also, we will be unable to
     * communicate with it using the cbus.
     */
    if (slotp->flags & BOARD_DISABLED)
	return;

    /*
     * Find the first interface on the SVIP
     */
    hwidb = NULL;
    for (ix = 0; ix < MAX_CTRLR_INTERFACES; ix++) {
	hwidb = slotp->interface_list[ix];
	if (hwidb)
	    break;
    }
    
    if (!hwidb)
	return;

    rom_ver = 0;
    mem_lock(hwidb);
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);
    send_rom_version_cmd(hwidb, &rom_ver);
    reset_interrupt_level(level);
    free(hwidb);
    
    printf("\n    %s", slotp->sw_version_str);
    printf("\n    ROM Monitor version %d.%d", 
	   rom_ver  >> 8, rom_ver & 0xff);
}

/* end of file */
