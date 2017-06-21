/* $Id: rsp_rvip.c,v 3.6.18.8 1996/09/09 19:06:06 kao Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_rvip.c,v $
 *------------------------------------------------------------------
 * Versatile Interface Processor support code - RVIP version
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_rvip.c,v $
 * Revision 3.6.18.8  1996/09/09  19:06:06  kao
 * CSCdi61698:  show hardware reports 3 PAs after OIR
 * Branch: California_branch
 *
 * Revision 3.6.18.7  1996/08/28  13:14:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.18.6  1996/08/20  00:36:03  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.6.18.5  1996/08/07  18:02:36  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.6.18.4  1996/07/26  01:11:11  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.6.18.3  1996/05/22  13:51:31  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.6.18.2  1996/03/21  23:50:07  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.6.18.1  1996/03/18  22:02:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.8  1996/03/18  23:35:19  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.7  1996/03/14  01:03:36  fsunaval
 * CSCdi50801:  SW-VIP1: Show diag has wrong board type for FDDI PA
 * Branch: ELC_branch
 *
 * Revision 3.2.2.6  1996/03/13  09:24:21  rharagan
 * CSCdi51440:  Incomplete slot structure init for SVIP
 * Branch: ELC_branch
 * Fix small memory leak in remove ctrlr functions, svip and rvip, and
 * correctly setup pa_info element in svip slot structure.
 *
 * Revision 3.2.2.5  1996/03/06  20:50:18  fsunaval
 * CSCdi50792:  Distributing switch not working for fddi-pa.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.4  1996/02/10  00:18:42  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.3  1996/02/08  09:30:07  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.2  1996/01/16  02:35:49  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/23  04:41:55  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.6  1996/02/08  23:11:23  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.5  1996/02/01  06:09:56  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4.12.1  1996/01/24  23:04:19  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/26  23:11:37  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.3  1995/12/10  23:28:16  rpratt
 * CSCdi45343:  Chassis MIB enhancements for VIP & port adapters
 *
 * Revision 3.2  1995/11/17  18:49:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  14:46:53  getchell
 * Placeholder files for VIP development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 

#include "master.h"
#include "plural.h"
#include "subsys.h"
#include "logger.h"
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
#include "../if/rvip_debug.h"
#include "rsp_vip.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */


static uchar nRVIPs, nRVIPnets;
static uchar nRVIPfethernets, nRVIPethernets, nRVIPserialnets,
			 nRVIPtringnets, nRVIPfddinets;


/*****************************************************************************
 * Name: rvip_ctrlr_analyze
 *
 * Description: analyze part of initialization for an RVIP
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
static void rvip_ctrlr_analyze (slottype *slot)
{
    int port_num;
    ushort status;
    ushort interface_type;
    hwidbtype *hwidb;
    enum IDBTYPE type;
    ushort pa_indx;
    pa_info_slots *pa_info_ptr;

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

	interface_type = vip2standard_types(interface_type, &type);

	if (interface_type == FCI_TYPE_UNKNOWN) {
	    errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
		   port_num, interface_type);
	    continue;
	}

	/*
	 * Get the IDB for the RVIP interface
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
	    nRVIPfethernets++;
	    break;

	  case IDBTYPE_CBUSETHER:
	    rsp_ether_idb_init(hwidb, TRUE);
	    if (pa_info_ptr)
		pa_info_ptr->pa_type = FCI_TYPE_VIP_ETHER8;
	    nRVIPethernets++;
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
	    nRVIPserialnets++;
	    break;

	  case IDBTYPE_CTR:
	    rsp_vip_4r_idb_init(hwidb);
            if (pa_info_ptr)
                pa_info_ptr->pa_type = FCI_TYPE_VIP_TRING4;
	    nRVIPtringnets++;
	    break;

	  case IDBTYPE_FDDIT:
	    rsp_fddi_idb_init(hwidb, TRUE);
	    if (pa_info_ptr)
		pa_info_ptr->pa_type = FCI_TYPE_VIP_FDDI;
	    nRVIPfddinets++;
	    break;

	  default:
	    break;
	}
	if (pa_info_ptr)
	    pa_info_ptr->pa_ifcount++;
	hwidb->status |= IDB_VIP;
    }
    ++nRVIPs;
    return;
}


/*
 * invoked when an RVIP has been removed from the system
 */
static void
rvip_ctrlr_removed (slottype *sp)
{
    int ix;
    hwidbtype *hwidb;
    ushort pa_indx;
    pa_info_slots *pa_info_ptr;
    
    for (ix = 0; ix < sp->interface_count; ++ix) {
	hwidb = sp->interface_list[ix];
	if (hwidb) {
	    --nnets;
	    --nRVIPnets;
	    
	    pa_indx = (hwidb->slotunit < PA_VIRTUAL_SLOT_BOUNDARY) ? 0 : 1;
	    pa_info_ptr = sp->pa_info[pa_indx];
	    if (pa_info_ptr) {
                pa_info_ptr->pa_type = 0;
                pa_info_ptr->pa_ifcount = 0;
		if (pa_info_ptr->private_pa_info)
		    free(pa_info_ptr->private_pa_info);
		free(pa_info_ptr);
		sp->pa_info[pa_indx] = 0;
	    }
	    switch(hwidb->type) {
	    case IDBTYPE_FEIP:
		--nfethernets;
		--nethernets;
		--nRVIPfethernets;
		break;
		
	    case IDBTYPE_CBUSETHER:
		--nethernets;
		--nRVIPethernets;
		break;
		
	    case IDBTYPE_CBUSSERIAL:
		--nserialnets;
		--nRVIPserialnets;
		break;
		
	    case IDBTYPE_CTR:
		--nTRs;
		--nRVIPtringnets;
		break;
		
	    case IDBTYPE_FDDIT:
		--nfddinets;
		--nRVIPfddinets;
		break;
		
	    default:
		break;
	    }
	}
    }
    --nRVIPs;
    return;
}

/*
 * display count of RVIPs in the system
 */
static void
rvip_print_controllers (void)
{
    if (nRVIPs) {
	printf("%u VIP controller%s ", ArgAndPlural(nRVIPs, "", "s"));
	if (nRVIPfethernets)
	    printf("(%u FastEthernet)", nRVIPfethernets);
	if (nRVIPethernets)
	    printf("(%u Ethernet)", nRVIPethernets);
	if (nRVIPserialnets)
	    printf("(%u Serial)", nRVIPserialnets);
	if (nRVIPtringnets)
	    printf("(%u Token Ring)", nRVIPtringnets);
	if (nRVIPfddinets)
	    printf("(%u Fddi)", nRVIPfddinets);
	printf(".\n");
    }
}

/*****************************************************************************
 * Name: rvip_platform_init
 *
 * Description: platform specific VIP init
 *
 * Input: 
 *   subsys -- pointer to the subsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void rvip_platform_init (subsystype *subsys)
{
    nRVIPs = 0;
    nRVIPethernets = nRVIPserialnets =  nRVIPtringnets = nRVIPfethernets
	= nRVIPfddinets = 0;

    reg_add_ctrlr_analyze(FCI_RVIP_CONTROLLER,
			  rvip_ctrlr_analyze, "rvip_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_RVIP_CONTROLLER, vip_ctrlr_init, "vip_ctrlr_init");
    reg_add_if_init(FCI_RVIP_CONTROLLER, vip_if_init, "vip_if_init");
    reg_add_ctrlr_removed(FCI_RVIP_CONTROLLER,
			  rvip_ctrlr_removed, "rvip_ctrlr_removed");
    reg_add_print_controllers(rvip_print_controllers, 
			      "rvip__print_controllers");
    return;
}

/*
 * rvip_show_cbus
 */
void rvip_show_cbus (ushort slot_index)
{
    slottype *slotp;
    hwidbtype *hwidb;
    int ix;
    leveltype level;
    ushort vpld_ver, rom_ver;
    
    slotp = &slots[slot_index];
    /* 
     * If the card is disabled, we have no relevant
     * information to report.  Also, we will be unable to
     * communicate with it using the cbus.
     */
    if (slotp->flags & BOARD_DISABLED)
	return;

    /*
     * Find the first interface on the RVIP
     */
    hwidb = NULL;
    for (ix = 0; ix < MAX_CTRLR_INTERFACES; ix++) {
	hwidb = slotp->interface_list[ix];
	if (hwidb)
	    break;
    }
    
    if (!hwidb)
	return;

    vpld_ver = rom_ver = 0;
    mem_lock(hwidb);
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);
    send_pld_version_cmd(hwidb, &vpld_ver);
    send_rom_version_cmd(hwidb, &rom_ver);
    reset_interrupt_level(level);
    free(hwidb);
    
    printf("\n    %s", slotp->sw_version_str);
    printf("\n    FLASH ROM version %d.%d, VPLD version %d.%d", 
	   rom_ver  >> 8, rom_ver & 0xff,
	   vpld_ver >> 8, vpld_ver & 0xff);
}
