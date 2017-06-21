/* $Id: atmdriver.c,v 1.1.4.3 1996/07/02 23:45:03 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/atmdriver.c,v $
 *-----------------------------------------------------------------
 * atmdriver.c -- ATM driver interface
 *
 * November 1994, sakumar
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmdriver.c,v $
 * Revision 1.1.4.3  1996/07/02  23:45:03  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 1.1.4.2  1996/06/18  16:45:03  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/09  14:54:41  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:51  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/03/30  02:17:38  sakumar
 * Changes to support sending of FLUSH message on Cat5K ATM LANE module.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:10:09  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:30  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:07  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:37  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.70.2.1  1995/11/27  18:11:49  sakumar
 * Added new RSAR command: VC_MODIFY that used by aalsap_lec_map_bus_vcds(),
 * aalsap_change_vcd_clbit() aalsap_bind_vlan_to_vc() and aalsap_bus_map_bus_vcds()
 * instead of VC_OPEN.
 *
 * This fixes a problem in which if the aalsap_change_vcd_clbit() was called
 * while the RSAR was re-assembling a FROM_BUS_TO_BIGA VC packet, using VC OPEN
 * could corrupt the RSAR's Reassembly TimeOut queue and potentially hang the RSAR.
 *
 * Revision 1.70  1995/11/04  00:19:46  sakumar
 * Removed debugging printfs.
 *
 * Revision 1.69  1995/10/27  20:26:48  sakumar
 * IPC packet counter was being incremented incorrectly in TXSAR ISR handler.
 *
 * Revision 1.68  1995/10/16  23:35:56  sakumar
 * Added support for:
 *     (1) Topology Change
 *     (2) LES, BUS, LECS on Catalyst 5000 ATM module with BUS implemented
 *         in fast RSAR->TSAR firmware.
 *
 * Revision 1.67  1995/10/13  06:33:13  rlowe
 * Cleaned up some ifdefs.
 *
 * Revision 1.66  1995/10/10  16:36:55  sakumar
 * Cleaned up RSAR and atmdriver by removing canned Synergy Headers in
 * Frame Buffer. The RSAR now dynamically creates the Synergy Header
 * from VC Info. Also, in-band IPC does not use the canned Synergy
 * Header. This checkin frees up space in the Frame Buffer, which
 * is needed for SAR-to-SAR messaging queues.
 *
 * Revision 1.65  1995/10/04  01:30:47  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.64  1995/09/27  23:27:25  sakumar
 * Removed references to TSAR flush holding queues since we will be
 * dropping packets during the flush state. Added support for LEC
 * to set the VPC along with the state in the TSAR CAM.
 *
 * Revision 1.63  1995/09/22  23:24:39  sakumar
 * RSAR performance changes. (Construct Synergy Header on the fly and
 * DMA it to frame buffer; DMA TBD to frame buffer instead of two 4
 * word writes).
 *
 * The Incoming VCI (ivci) info on VC open changed to provide
 * (1) VLAN (2) VPC (3) C/L and (4) card instance instead of ptr
 * to Synergy Header.
 *
 * Revision 1.62  1995/09/15  20:36:29  sakumar
 * Pulled out a bunch of debugging printfs.
 *
 * Revision 1.61  1995/09/14  17:39:48  rbau
 * read pmd type directly from the pmd serial eeprom.
 *
 * Revision 1.60  1995/09/08  23:09:06  sakumar
 * Sets up PMD type by asking the LCP (and PMD EEPROM).
 *
 * Revision 1.59  1995/09/02  01:14:10  rlowe
 * Per Sanjay's request, fix calc of number of vpi bits.
 *
 * Revision 1.58  1995/08/30  19:14:10  sakumar
 * (1) Support for new PMC PMD.
 * (2) vc_per_vp command.
 *
 * Revision 1.57  1995/07/25  22:03:25  sakumar
 * Added function aalsap_change_vcd_clbit() which will set/unset the
 * Conditional Learn bit for BUS VCs. This function is called by LANE
 * when a topology change happens. At start, clear C_L bit. At end set C_L bit.
 *
 * Revision 1.56  1995/06/27  23:24:45  sakumar
 * Enabled LCP_CardInstanceNbr() instead of a hardcoded value.
 *
 * Revision 1.55  1995/06/22  18:00:56  sakumar
 * Changes to support LEC Ctrl Packet on Data Direct VC to ACP even before
 * bind has been done.
 *
 * Also added a (dummy) function atmSyn_sniff() for IOS compatibility.
 *
 * Revision 1.54  1995/06/17  02:21:11  sakumar
 * In aalsap_is_vpc_active() also check for overflow value for vpc.
 *
 * Revision 1.53  1995/06/15  21:02:55  rahuja
 * 	Added get_vcd_from_vpc, get_vcd_from_vpc calls in atmdriver.c
 * 	Started ipctest from atmtest.c
 *
 * Revision 1.52  1995/06/15  09:25:45  rlowe
 * Move boot download functionality to boot-1.  Condition PMON to SARs
 * on MFG jumper instead of DBG jumper.  Condition boot download on
 * SPARE jumper until checksums are done.  Cleanup makefiles.
 * Lock s-bus when resetting BIGA/SARs to avoid hung frame buffer.
 * Explicitly hold/release reset on BIGA.  Force LCP reset whenever
 * LCP driver re-initialized.
 *
 * Revision 1.51  1995/06/14  22:49:12  rlowe
 * Change ADVR_RxSarPollSvc, ADVR_TxSarPollSvc to have "fOnce" flag.
 *
 * Revision 1.50  1995/06/10  01:13:54  rlowe
 * #if out some debug prints (buginf's).
 *
 * Revision 1.49  1995/06/09  11:43:51  rlowe
 * Make SAR resets atomic with respect to Frame Buffer accesses.  Fix
 * for HW bug where Frame-Buf arbitration could hang, thus causing 68k
 * (ACP) bus errors.
 *
 * Revision 1.48  1995/06/09  06:28:03  rlowe
 * Integrate to latest LCP driver (message model, startup & HW handshakes).
 * Control whether or not ATM driver has its own LCP status poll at startup.
 *
 * Revision 1.47  1995/06/09  01:58:58  sakumar
 * Added Statistics gathering functionality.
 *
 * Revision 1.46  1995/06/08  17:00:07  rahuja
 * 	Checked in Rick's changes to unconditionally poll for sar command
 * completion
 *
 * Revision 1.45  1995/06/01  22:34:16  sakumar
 * Bunch of changes made in the last few days.
 *
 * Revision 1.44  1995/05/27  05:02:26  sakumar
 * Fix so that interrupts are (re-)unmasked whenever we use IOS edisms
 * replacement (buzz loop device queue poll).  This ensures that ISRs
 * can keep running even though we are buzzing for a particular queue
 * (element) response.  Thus this mechanism is NOT the same as device
 * polling where interrupts are NOT enabled.
 *
 * Revision 1.43  1995/05/27  02:44:47  sakumar
 * Fix IOS init-time run-to-completion by NOT allowing ATM driver
 * edisms (use driver polling mode instead) until IOS init is done.
 *
 * Revision 1.42  1995/05/26  10:15:54  rlowe
 * Change atm_drv_fillsyhdr to call LCP_CardInstanceNbr instead of
 * hard-coding the card instance number in the Synergy header (for IPC).
 *
 * Revision 1.41  1995/05/26  08:42:58  sakumar
 * Adjust platform specific startup/init for driver and LANE integration.
 *
 * Revision 1.40  1995/05/26  01:34:10  sakumar
 * Ensure that ADVR_Std_TSar_LeArpUep is initialized to NULL.
 *
 * Revision 1.39  1995/05/26  01:25:29  sakumar
 * Added functionality for LANE Support. (1) Upcall to initiate LE-ARP etc.
 *
 * Revision 1.38  1995/05/19  00:01:16  sakumar
 * Detect and handle SAR timeouts.
 *
 * Revision 1.37  1995/05/18  01:11:49  rlowe
 * Cleanup startup time settings of MFP for SAR interrupt inputs.  Had race
 * condition where MFP was being (re-)groomed while TSAR was asserting interrupt.
 *
 * Revision 1.36  1995/05/13  22:42:07  rlowe
 * Support Rev 2 in-band downloading, and shift image start address to
 * $40004000.
 *
 * Revision 1.35  1995/05/04  10:38:38  rlowe
 * Integrate in-band ipc & download to Boot-2 for testing, using TEST_DOWNLOAD
 * flag.
 *
 * Revision 1.34  1995/05/02  16:48:08  sakumar
 * A VC is assigned a VC Info Structure in the RXSAR's VCR only if it is a
 * LANE DATA DIRECT VC or if it is the Signalling VC. All other VCs have
 * their VC Info structure assigned in the Secondary Port RAM.
 *
 * Revision 1.33  1995/05/02  05:01:16  rlowe
 * Move some global counter definitions from datatest.c to atmdvr_gdata.c/h.
 * Eliminate some atm driver startup code from atmdvr_poll.c; all startup now
 * in atmdriver.c.
 *
 * Revision 1.32  1995/05/02  04:28:17  rlowe
 * Fix upcall registration and use, so that ATM driver will work even if
 * NO upcall is current (fcn pointer is NULL).  Add upcall registration
 * to Syn_ipc.c test file.
 *
 * Revision 1.31  1995/05/02  01:28:45  rlowe
 * Cleanup files a bit; ensure polling mode api's available in atm driver
 * interface file.
 *
 * Revision 1.30  1995/05/02  01:06:57  rlowe
 * Re-structure code to handling polling (boot case) as well as interrupt
 * mode (kernel case).  Isolate IOS-isms (such as edisms) a bit more.
 * Provide polling mode APIs.
 *
 * Revision 1.29  1995/04/21  16:42:48  sakumar
 * Added support for (1) runtime specification of active VPI/VCI bits.
 *                   (2) binding of VC with LECID (for FROM BUS VC)
 * Cleaned up aalsap_open_vcc().
 *
 * Revision 1.28  1995/04/12  22:04:06  sakumar
 * Changed VCD to VPC and
 *         vcd to vpc.
 *
 * Revision 1.27  1995/04/07  22:44:41  sakumar
 * Regardless of whether or not send_nmp succeeds, the ACP->NMP IPC buffer
 * is released by the SAR alone (because single producer, single consumer).
 *
 * Revision 1.26  1995/04/07  20:00:08  rahuja
 * 	Added support for inband ipc
 *
 * Revision 1.25  1995/03/30  22:28:02  sakumar
 * cleaned up building of the Synergy header. Added vlan and syn_vpc as
 * aalsap_open_vcc parameters.
 *
 * Revision 1.24  1995/03/30  17:59:21  sakumar
 * Added Syn_ipc.c (currently just does and echo)
 *
 * Revision 1.23  1995/03/29  22:49:27  sakumar
 * Changes to support Echo Suppresion and CTRL packet in Data VC (in RXSAR)
 * Use latest SAR code with these changes.
 *
 * Revision 1.22  1995/03/27  23:33:14  sakumar
 * Changed ACP->NMP IPC so only one interrupt from RXSAR per packet.
 * Added ISR support for TXSAR->ACP Events (for IPC).
 *
 * Revision 1.21  1995/03/25  02:32:05  sakumar
 * IPC (one way from ACP to BIGA via RXSAR) now works.
 *
 * Revision 1.20  1995/03/21  23:00:14  sakumar
 * Merged (with BIGA stuff) now works.
 *
 * Revision 1.19  1995/03/18  01:45:09  sakumar
 * Merged atmdriver with IOS environment
 *
 * Revision 1.18  1995/03/17  18:51:36  sakumar
 * ATM Driver now works with IDBs.
 *
 * Revision 1.17  1995/03/03  07:56:41  rlowe
 * Clean up and solidify MFP interrupt controller setup and usage.
 * Provide MFP interrupt controller polling routines for low-level code.
 *
 * Revision 1.16  1995/02/24  18:36:26  sakumar
 * Updated file to use paktype and do a bcopy to/from frame buffer memory.
 *
 * Revision 1.15  1995/02/21  16:31:33  sakumar
 * Fixed memory pointer bug in datatest.c. Now it works within IOS.
 *
 * Revision 1.14  1995/02/16  07:34:46  rlowe
 * Fix bugs from initial integration with IOS (bad pointer, printf string).
 *
 * Revision 1.13  1995/02/16  03:48:05  rlowe
 * Changes for initial integration of ATM driver with IOS.
 *
 * Revision 1.12  1995/02/10  23:59:40  sakumar
 * Added code to support reporting of ACP receive buffer error counts (due to
 * reassembly timeout, crc error, length mismatch etc.)
 *
 * Revision 1.11  1995/02/10  16:38:32  sakumar
 * txsar_cmd() is now used for data transmit. Removed some printfs.
 *
 * Revision 1.10  1995/02/06  19:02:43  rahuja
 * Changed OK and ERR to ACP_OK, ACP_ERR
 *
 * Revision 1.9  1994/12/20  22:51:11  rahuja
 * Called ntc_init
 *
 * When IVCI Map Entry is in VCR, the VCR Entry pointer table entry now points
 * to the VCR (previusly, the driver didn't update this, so the SAR ended up
 * using the Secondary Memory for IVCI Map Entry even though the initial SET
 * gave a pointer into the VCR.
 *
 * Revision 1.7  1994/12/16  00:29:11  rahuja
 * Integrated and tested atm driver on acp
 *
 * Revision 1.6  1994/12/14  22:25:03  sakumar
 * added sar interrupt functions
 *
 * Revision 1.5  1994/12/10  02:25:48  rlowe
 * Yet more changes for a working test.
 *
 * Revision 1.4  1994/12/10  00:36:46  rlowe
 * Added Makefile, fixed up #defines/#includes for atmdriver.c
 *
 * Revision 1.3  1994/12/09  22:09:36  rahuja
 * Integrated with txsar
 *
 * Revision 1.2  1994/12/09  18:02:10  rahuja
 * Integrated txsar
 *
 * Revision 1.1  1994/12/06  22:06:31  sakumar
 * Now this file is in a new place (acp/drivers/atmdriver)
 *
 * Revision 1.5  1994/12/06  22:03:46  sakumar
 * Added code to allow IVCI Map Entries to live in VCR.
 *
 * Revision 1.4  1994/12/01  23:24:28  sakumar
 * Changed to have compile time switches come in from Makefile rather than
 * defines in the source files.
 *
 * Revision 1.3  1994/11/30  21:06:45  sakumar
 * Added support for different address spaces for ACP and SARs.
 *
 * Revision 1.2  1994/11/19  01:56:58  sakumar
 * Added CVS header
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef LANGUAGE_C
#define     LANGUAGE_C
#endif

#include	"../../../sys/h/master.h"

#include    "basic_types.h"
#include    "param.h"
#include    "queue_if.h"
#include    "buffer_if.h"
#include    "ovcimap.h"
#include    "cam_if.h"
#include    "shram.h"
#include    "spram.h"
#include	"sar_if.h"
#include    "ivcimap.h"
#include	"atmdrv_if.h"
#include	"acp_basic_types.h"
#include	"acp_hw_defs.h"
#include	"acp_c_defs.h"
#include    "lo_wdog.h"
#include	"mfp_hw_defs.h"
#include	"mfp_acp_defs.h"
#include	"atmdvr_gdata.h"
#include	"lcp_if.h"
#include    "lec_if.h"
#include	"bdg_if.h"
#include	"ntc_client.h"



/*
**	PROTOTYPES
*/
UINT32 wait_for_interrupt ();
void atm_drv_fillsyhdr(tVPC from_vpc, tVPC to_vpc, UINT16 vlan, UINT16 c_l);


extern tpSARCMD txsar_fillcmd(UINT32  cmd);
extern void txsar_postcmd(tpSARCMD pCmd);




/*
**	GLOBALS
*/
UINT32 pmd_chip_type = PMD_PMC; /* Default */


/*
*******************************************************************************
*
*   FUNCTION:
*           atmdrv_interrupt_rxsar
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/

void 
atmdrv_interrupt_rxsar()
{
   /* Interrupt RXSAR */
	*(UINT32 *) (ACP_INTR2_SAR_REG(ACP_I2S_RX)) = ACP_INTR2_SAR_DAT(0,1);

}/*end atmdrv_interrupt_rxsar*/


/*
*******************************************************************************
*
*   FUNCTION:
*           atmdrv_interrupt_txsar
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/
 
void 
atmdrv_interrupt_txsar()
{
   /* Interrupt TXSAR */
	*(UINT32 *) (ACP_INTR2_SAR_REG(ACP_I2S_TX)) = ACP_INTR2_SAR_DAT(0,1);

}/*end atmdrv_interrupt_txsar */

 

/*
*******************************************************************************
*
*   FUNCTION:
*           VCR_ivci_map_info_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
* 		Initialize Structure that tracks which VC maps are in the VCR
*
*******************************************************************************
*/

void
VCR_ivci_map_info_init()
{
	int num_entries, i, j;
	int ivci_map_entry_size;


	/* Link up the info structures */
	for(i = 0; i < (MAX_VCR_IVCI_MAP_ENTRIES - 1); i++) {
		vcr_imap_ptr[i].next = i + 1;
	}
	vcr_imap_ptr[MAX_VCR_IVCI_MAP_ENTRIES - 1].next = -1;

	/* round up structure size to multiple of 4 bytes */
	ivci_map_entry_size = (sizeof(tIMAPENTRY) + 3) & ~0x3;

	/* Calculate number of ivci Map Entries that can be accommodated in VCR */
	num_entries = 
    (VCR_IVCI_MAP_AREA_END_ADDR-VCR_IVCI_MAP_AREA_START_ADDR)/ivci_map_entry_size;

	head_empty_list = num_entries;
	if (num_entries >= MAX_VCR_IVCI_MAP_ENTRIES) {
		num_entries = MAX_VCR_IVCI_MAP_ENTRIES;
		head_empty_list = -1;
	}

	for(i = 0, j = VCR_IVCI_MAP_AREA_START_ADDR; i < num_entries; 
												i++, j += ivci_map_entry_size) {
		vcr_imap_ptr[i].pMapEntry = (tpIMAPENTRY) j;
	}
	vcr_imap_ptr[num_entries -1].next = -1;

	head_avail_list = 0;

#ifdef DEBUG_PRINTF
	printf("head_avail_list: %d, head_empty_list %d, num_entries %d\n",
			head_avail_list, head_empty_list, num_entries);
#endif /* DEBUG_PRINTF */

}/*end VCR_ivci_map_info_init */


/*
*******************************************************************************
*
*   FUNCTION:
*           get_pVCR_ivci_map_entry
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
* 	Function called by the ATM driver to assign a VC Map location in the VCR.
* If it fails, it returns a 0.
*
*******************************************************************************
*/

tpIMAPENTRY
get_pVCR_ivci_map_entry()
{
	int i;

	i = head_avail_list;

	if (i == -1) {
		/* No space available for VCR ivci map entry */
		return((tpIMAPENTRY) NULLP);
	}

	/* Update head index */
	head_avail_list = vcr_imap_ptr[head_avail_list].next;

	/* Update free index */
	vcr_imap_ptr[i].next = head_empty_list;
	head_empty_list = i;

	return(vcr_imap_ptr[i].pMapEntry);

}/*end get_pVCR_ivci_map_entry */


/*
*******************************************************************************
*
*   FUNCTION:
*           rel_VCR_ivci_map_entry
*
*   PARAMETERS:
*			tpIMAPENTRY pMapEntry
*   RETURN:
*
*   ALGORITHMS
*
* Release a VC Map Table entry that has been assigned in the RXSAR's VCR.
*
*******************************************************************************
*/

void
rel_VCR_ivci_map_entry( tpIMAPENTRY pMapEntry )
{
	int i;

	if (head_empty_list == -1) {
		printf("\nrel_pVCR_ivci_map_entry(): Internal Error\n");
		return;
	}

	i = head_empty_list;
	head_empty_list = vcr_imap_ptr[i].next;

	vcr_imap_ptr[i].pMapEntry = pMapEntry;
	vcr_imap_ptr[i].next = head_avail_list;

	head_avail_list = i;

}/*end rel_VCR_ivci_map_entry */


/*
*******************************************************************************
*
*   FUNCTION:
*		ADVR_RxSar_Handler
*
*   PARAMETERS:
*		UINT8	fOnce	-- set TRUE if at most ONE inbound packet
*				   can be handled, eg. if polling.
*
*   RETURN:
*		TRUE if an inbound packet was handled, else FALSE.
*
*   ALGORITHMS
*		This routine does the RxSAR service, whether for
*		interrupt-level processing or polling mode activity.
*
*******************************************************************************
*/

UINT8
ADVR_RxSar_Handler (UINT8 fOnce)
{
	tpSARCMD pResp;
	tpBUFFER pRcvBuf;
	tpBUFHDR pBufHdr;
	UINT8    RC = FALSE ;


	/* First check for command completions */
	while ((pResp = (tSARCMD *) qmgr_deq(acp_Rxspram_info.rxspram_pAcpRespq)) != NULL) {
		pResp = (tpSARCMD) MAP_SP_RX_ACP(pResp);
		pResp->cmd_wakeup = 1;
	}

	/* Check for ACP Packets Received */
	while ((pRcvBuf = (tpBUFFER)qmgr_deq(acp_Rxspram_info.rxspram_pRxToAcpDataq))
 																			!= NULL) {
		pRcvBuf = (tpBUFFER) MAP_SH_SAR_ACP(pRcvBuf);
		pBufHdr = (tpBUFHDR) MAP_SH_SAR_ACP(&pRcvBuf->buf_hdr);
		switch (pBufHdr->bufh_status) {
			case RCV_PKT_STAT_OK_C:
                if (AtmdrvGcb.sap_entry[0][pBufHdr->bufh_vpc].vcstate != VPC_INACTIVE) {
                    
                        /* Make Upcall to Data Packet Recipient */
                        (AtmdrvGcb.sap_entry[0][pBufHdr->bufh_vpc].sap_uep)
									(AtmdrvGcb.sap_entry[0][pBufHdr->bufh_vpc].vcd,
								    0,  /* XXX Event? */
								  	pBufHdr->bufh_length,
									MAP_SH_SAR_ACP(&pRcvBuf->buf_data));
                }
				break;

			case RCV_PKT_STAT_TIMEOUT_ERROR_C:
				rto_err++;
				break;
			case RCV_PKT_STAT_BUF_OVFL_ERROR_C:
				buf_ovfl_err++;
				break;
			case RCV_PKT_STAT_LEN_MISMATCH_ERROR_C:
				len_err++;
				break;
			case RCV_PKT_STAT_CRC_ERROR_C:
				crc_err++;
				break;
			case RCV_PKT_STAT_BIGA_Q_OVFL_ERROR_C:
			case RCV_PKT_STAT_ERROR_C:
			default:
				break;
		}

		/* put received buffer back on acp buffer q */
		qmgr_enq(acp_Rxspram_info.rxspram_pAcpBufPool, (tpQELEM) MAP_SH_ACP_SAR(pRcvBuf));

		RC = TRUE ;

			/* This allows a hole and must NOT be used once interrupt */
			/* mode is used (ie. is for Boot/download polling ONLY).  */
			/* Consider a datagram interrupt followed by SAR cmd ack. */
			/* With fOnce we exit after clearing interrupts AND just  */
			/* handling the datagram.  Thus SAR cmd ack is dropped.   */

		if (fOnce)		/* Let single packet be processed. */
			break ;
	}

	return (RC) ;

}	/* end ADVR_RxSar_Handler */


/*
*******************************************************************************
*
*   FUNCTION:
*           rxsar_isr
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*
*   ALGORITHMS
*
*******************************************************************************
*/

void
rxsar_isr ()
{
	(void) ADVR_RxSar_Handler (FALSE) ;

}	/* end rxsar_isr */


/*
*******************************************************************************
*
*   FUNCTION:
*           rxsar_cmd
*
*   PARAMETERS:
*           UINT32 cmd
*			void * cmd_data
*
*   RETURN:
*			ACP_OK
*			ACP_ERR
*
*   ALGORITHMS
*
*******************************************************************************
*/

UINT32
rxsar_cmd(UINT32 cmd, void * cmd_data)
{
	tpSARCMD pCmd;
	int retval;
	tpVPCPARM vpc_parm, pCmdParm;
	tpRX_NMP_REQ rx_nmp_parm, pRxNmpParm;
	tpRX_SET_VPI_VCI_BITS_REQ p_vp_per_vc, pVpPerVc;

	
	/* Get a command Buffer */
	pCmd = (tpSARCMD) qmgr_deq(acp_Rxspram_info.rxspram_pAcpCmdPool);
	if (pCmd == NULL) {
#ifdef DEBUG_PRINTF
		printf("\nrxsar_cmd(): Couldn't get Command buffer; Command Failed\n");
#endif /* DEBUG_PRINTF */
		return(ACP_ERR);
	}

	pCmd = (tpSARCMD) MAP_SP_RX_ACP(pCmd);
	
	/* Set Command Byte */
	pCmd->cmd_code = (cmd >> 8);
	
	/* Clear wakeup byte (in command header) to 0 */
	pCmd->cmd_wakeup = 0;

	/* Clear status (i.e. response) byte to 0 */
	pCmd->cmd_status = 0;

	/* Format Command (based on command type) */
	switch (cmd) {
		case RXSAR_RESTART_C:			/* HACK XXX */
		case RXSAR_CLEAR_VPCMAP_C:
		case RXSAR_SET_VPCMAP_C:
		case RXSAR_MODIFY_VPCMAP_C:
			vpc_parm = (tpVPCPARM) cmd_data;
			pCmdParm = (tpVPCPARM) pCmd->cmd_parmv;
			/* Copy cmd info into cmd buffer */
			pCmdParm->vpcp_vpc = vpc_parm->vpcp_vpc;
			pCmdParm->vpcp_lecid = vpc_parm->vpcp_lecid; 
			pCmdParm->vpcp_vcstate = vpc_parm->vpcp_vcstate;
			pCmdParm->vpcp_qos = vpc_parm->vpcp_qos;
			pCmdParm->vpcp_pVcMapEntry = vpc_parm->vpcp_pVcMapEntry;
			pCmdParm->vpcp_vlan_cl =  vpc_parm->vpcp_vlan_cl;
			pCmdParm->vpcp_cid_vpc =  vpc_parm->vpcp_cid_vpc;
			pCmdParm->vpcp_pRVCStat =  vpc_parm->vpcp_pRVCStat;
			pCmdParm->vpcp_pTVCStat =  vpc_parm->vpcp_pTVCStat;
			break;
		case RXSAR_SEND_TO_NMP_C:
			rx_nmp_parm = (tpRX_NMP_REQ) cmd_data;
			pRxNmpParm = (tpRX_NMP_REQ) pCmd->cmd_parmv;
			/* Copy cmd into cmd buffer */
			pRxNmpParm->rx_nmp_pbuf = rx_nmp_parm->rx_nmp_pbuf;
			pRxNmpParm->rx_nmp_len = rx_nmp_parm->rx_nmp_len;
			pRxNmpParm->rx_nmp_vlan_cl  = rx_nmp_parm->rx_nmp_vlan_cl;
			pRxNmpParm->rx_nmp_cid_vpc  = rx_nmp_parm->rx_nmp_cid_vpc;
			break;
		case RXSAR_SET_VPI_VCI_BITS_C:
			p_vp_per_vc = (tpRX_SET_VPI_VCI_BITS_REQ) cmd_data;
			pVpPerVc = (tpRX_SET_VPI_VCI_BITS_REQ) pCmd->cmd_parmv;
			/* Copy cmd into cmd buffer */
			pVpPerVc->rx_active_vpi_vci_mask = p_vp_per_vc->rx_active_vpi_vci_mask;
			pVpPerVc->rx_num_vpi_shift_bits = p_vp_per_vc->rx_num_vpi_shift_bits;
			break;
		default:
			/* Return cmd buffer to command pool */
			qmgr_enq(acp_Rxspram_info.rxspram_pAcpCmdPool, (void *) MAP_SP_ACP_RX(pCmd));
#ifdef DEBUG_PRINTF
			printf("\nrxsar_cmd(): Unknown Command Code: 0x%x\n", cmd);
#endif /* DEBUG_PRINTF */
			return(ACP_ERR);
	}


	/* Enqueue Command */
	qmgr_enq(acp_Rxspram_info.rxspram_pAcpCmdq, (void *) MAP_SP_ACP_RX(pCmd));
	
	/* Interrupt RXSAR */
	atmdrv_interrupt_rxsar();

	/* Give up the processor */
	ADVR_DismissOnRxSar ( (void *) atmdrv_BLOCK, pCmd);

/*
* Note: The SARs DO NOT change the wakeup byte. After the SAR puts the command 
*		 buffer on the Response queue, it interrupts the ACP. The interrupt
*		 handler on the ACP dequeues the response, and sets the wakup byte to 1.
* 		 Then when the ACP scheduler runs the TEST function, it finds the wakeup
*		 byte set to 1 and so the scheduler wakes up this process
*/

	/* Command Completed, check response code */
	if (pCmd->cmd_status == SAR_CMD_SUCCESS_C) {
		retval = ACP_OK;
	} else if (pCmd->cmd_wakeup == 0) { /* Command timed out */
		ADVR_CmdTimeOut (RXSAR_INTR, pCmd) ;
	} else {
#ifdef DEBUG_PRINTF
		printf("rxsar_cmd: Command Failed, Error Status: %d\n", pCmd->cmd_status);
#endif /* DEBUG_PRINTF */
		retval = ACP_ERR;
	}

	/* Return cmd buffer to command pool */
	qmgr_enq(acp_Rxspram_info.rxspram_pAcpCmdPool, (void *) MAP_SP_ACP_RX(pCmd));

	return (retval);

}/*end rxsar_cmd */


/*
*******************************************************************************
*
*   FUNCTION:
*           txsar_cmd
*
*   PARAMETERS:
*           UINT32 cmd
*           void * cmd_data
*
*   RETURN:
*           ACP_OK
*           ACP_ERR
*
*   ALGORITHMS
*
*		This procdure calls the primitve tx sar routines.  The primitves
*	are blocking and return after the tx sar posts the response.
*
*******************************************************************************
*/


UINT32
txsar_cmd(UINT32 cmdcode, void * cmd_data)
{
UINT32      i;
tpSARCMD 	pCmd = NULLP;
tpQDESC     pCmdBufPool = acp_Txspram_info.txspram_pAcpCmdPool;

	switch(cmdcode) {

	case SAR_NOP_C:								/* no-operation comamnd		*/
		
		if( (pCmd = txsar_post_nopcmd()) == NULLP ) {
			printf("\n\rtxsar_cmd: txsar_post_nopcmd failed");
			goto err_exit;
		}

		break;

	case TXSAR_RESTART_C:

		break;

	case TXSAR_SET_VPCMAP_C:					/* vpc set command			*/
{
tpVPCPARM   pVpcCmdp;

		pVpcCmdp = (tpVPCPARM)cmd_data;

		if( (pCmd = txsar_post_vpccmd(0,				/* wakeup	*/
							TXSAR_SET_VPCMAP_C,			/* cmd		*/
						  	pVpcCmdp->vpcp_vpc,			/* vpcval	*/
							pVpcCmdp->vpcp_vcstate		/* vcstate	*/
							)) == NULLP ) {

			printf("\n\rtxsar_cmd: txsar_post_vpcsetcmd failed");
			goto err_exit;
		}
}
		break;

	case TXSAR_GET_VPCMAP_C:					/* get vpc command			*/
{
tpVPCPARM   pVpcCmdp;
 
        pVpcCmdp = (tpVPCPARM)cmd_data;
 
        if( (pCmd = txsar_post_vpccmd(0,                /* wakeup   */
                            TXSAR_SET_VPCMAP_C,         /* cmd      */
                            pVpcCmdp->vpcp_vpc,         /* vpcval   */
                            pVpcCmdp->vpcp_vcstate      /* vcstate  */
                            )) == NULLP ) {
 
            printf("\n\rtxsar_cmd: txsar_post_vpcsetcmd failed");
            goto err_exit;
        }
}
		break;

    case TXSAR_CLEAR_VPCMAP_C:                   /* clear vpc command         */
{
tpVPCPARM   pVpcCmdp;
 
        pVpcCmdp = (tpVPCPARM)cmd_data;
 
        if( (pCmd = txsar_post_vpccmd(0,                /* wakeup   */
                            TXSAR_CLEAR_VPCMAP_C,       /* cmd      */
                            pVpcCmdp->vpcp_vpc,         /* vpcval   */
                            pVpcCmdp->vpcp_vcstate      /* vcstate  */
                            )) == NULLP ) {
 
            printf("\n\rtxsar_cmd: txsar_post_vpcsetcmd failed");
            goto err_exit;
        }
}
		break;

	case TXSAR_SEND_REQUEST_C:					/* get vpc command			*/
{
tpTXSEND_REQ    pSreq;

		pSreq = (tpTXSEND_REQ)cmd_data;

		if( (pCmd = txsar_post_sendcmd(0, 
							   pSreq->txsend_vpc,
							   pSreq->txsend_len,
							   pSreq->txsend_pbuf
							   )) == NULLP ) {
	        printf("\n\r txtxsar_cmd: txsar_post_sendcmd failed");
			goto err_exit;
    	}
}
	break;

	case TXSAR_INIT_CAM_C:						/* initialize cam			*/

	    if( (pCmd = txsar_post_caminit()) == NULLP) {
 
        	printf("\n\r txtxsar_cmd: txsar_post_caminit failed");
        	goto err_exit;
		}

		break;

	case TXSAR_CREATE_CAM_C:				/* create cam entry				*/
{
tpCAMPARM	pCamReq = (tpCAMPARM)cmd_data;
 
		if( (pCmd = txsar_post_camcmd( TXSAR_CREATE_CAM_C, 
									   pCamReq )) == NULLP ) {

        	printf("\n\r txtxsar_cmd: TXSAR_CREATE_CAM_C failed");
        	goto err_exit;

		} 

		/*
		**	txsar_post_camcmd is a blocking call.  When we return here,
		**	the sar has acted upon the command, and the response should
		**	have the cam index value
		**	Copy the index value to the callers req block
		*/

		pCamReq->camp_index = ((tpCAMPARM)(pCmd->cmd_parmv))->camp_index;
}
	break;



	case TXSAR_GET_CAM_C:						/* get cam entry			*/
{
tpCAMPARM	pCamReq = (tpCAMPARM)cmd_data;
 
		if( (pCmd = txsar_post_camcmd( TXSAR_GET_CAM_C, 
									   pCamReq )) == NULLP ) {

        	printf("\n\r txtxsar_cmd: TXSAR_GET_CAM_C failed");
        	goto err_exit;
		}

        /*
        **  txsar_post_camcmd is a blocking call.  When we return here,
        **  the sar has acted upon the command, and the response should
        **  have the cam index value
        **  Copy the index value to the callers req block
        */
 
        pCamReq->camp_index = ((tpCAMPARM)(pCmd->cmd_parmv))->camp_index;

        /* Report back the current CAM state too */
        pCamReq->camp_state = ((tpCAMPARM)(pCmd->cmd_parmv))->camp_state;

}
		break;

	case TXSAR_CHANGE_CAM_STATE_C:				/* Change the CAM state		 */
{
tpCAMPARM	pCamReq = (tpCAMPARM)cmd_data;
 
		if( (pCmd = txsar_post_camcmd( TXSAR_CHANGE_CAM_STATE_C, 
									   pCamReq )) == NULLP ) {

        	printf("\n\r txtxsar_cmd: TXSAR_CHANGE_CAM_STATE_C failed");
        	goto err_exit;
		}

        /*
        **  txsar_post_camcmd is a blocking call.  When we return here,
        **  the sar has acted upon the command, and the response should
        **  have the cam index value
        **  Copy the index value to the callers req block
        */
 
        pCamReq->camp_index = ((tpCAMPARM)(pCmd->cmd_parmv))->camp_index;

}
		break;


	case TXSAR_CLEAR_CAM_C:						/* clear cam entry			*/
{
tpCAMPARM   pCamReq = (tpCAMPARM)cmd_data;
 
        if( (pCmd = txsar_post_camcmd( TXSAR_CLEAR_CAM_C,
                                       pCamReq )) == NULLP ) {
 
            printf("\n\r txtxsar_cmd: TXSAR_CLEAR_CAM_C failed");
            goto err_exit;
        }
 
        /*
        **  txsar_post_camcmd is a blocking call.  When we return here,
        **  the sar has acted upon the command, and the response should
        **  have the cam index value
        **  Copy the index value to the callers req block
        */
 
        pCamReq->camp_index = ((tpCAMPARM)(pCmd->cmd_parmv))->camp_index;
}
		break;

	case TXSAR_SET_VPI_VCI_BITS_C:	/* Set active VPI / VCI bits */
{
		tpTX_SET_VPI_VCI_BITS_REQ  p1 = (tpTX_SET_VPI_VCI_BITS_REQ) cmd_data;
		tpTX_SET_VPI_VCI_BITS_REQ  p2;

		if( (pCmd = txsar_fillcmd(TXSAR_SET_VPI_VCI_BITS_C)) == NULLP ) {
        	goto err_exit;
    	}
 
    	pCmd->cmd_wakeup = 0;
    	pCmd->cmd_length = mCMDOFFSET + sizeof(tTX_SET_VPI_VCI_BITS_REQ);
 
    	p2 = (tpTX_SET_VPI_VCI_BITS_REQ)pCmd->cmd_parmv;
		p2->tx_active_vci_mask = p1->tx_active_vci_mask; 
		p2->tx_num_active_vci_bits = p1->tx_num_active_vci_bits; 
 
    	/*
    	**  Post command with txsar
    	*/
    	txsar_postcmd(pCmd);
 
		break;
}

	case TXSAR_SET_THROTTLE_C:
{
		tpTX_SET_THROTTLE_PARM_REQ  p1 = (tpTX_SET_THROTTLE_PARM_REQ) cmd_data;
		tpTX_SET_THROTTLE_PARM_REQ  p2;

		if ( (pCmd = txsar_fillcmd(TXSAR_SET_THROTTLE_C)) == NULLP ) {
            goto err_exit;
        }
		pCmd->cmd_wakeup = 0;
        pCmd->cmd_length = mCMDOFFSET + sizeof(tTX_SET_THROTTLE_PARM_REQ);
 
        p2 = (tpTX_SET_THROTTLE_PARM_REQ)pCmd->cmd_parmv;
		p2->tx_throttle_parm = p1->tx_throttle_parm; 
 
        /*
        **  Post command with txsar
        */
        txsar_postcmd(pCmd);


}
		break;
		


	default:

		printf("\n\r txsar_cmd: unknown command: %d", cmdcode);
		goto err_exit;

	}/*switch cmdcode*/


    /* Command Completed, check response code */
 
	if( (pCmd->cmd_status != SAR_CMD_SUCCESS_C) ) {
		if (pCmd->cmd_wakeup == 0) {
			ADVR_CmdTimeOut (TXSAR_INTR, pCmd) ;
		}
#ifdef DEBUG_PRINTF
		printf("\n\rtxsar_cmd STATUS failed for : pCmd:%x", pCmd);
		printf("\n\rtxsar_cmd: status:%d", pCmd->cmd_status);
#endif /* DEBUG_PRINTF */
    	bufmgr_freebuf( pCmdBufPool, (tpBUFFER)(MAP_SP_ACP_TX(pCmd)) );
        goto err_exit;
	}


    /*
    **  Response verified, free command buffer
    */
 
    bufmgr_freebuf( pCmdBufPool, (tpBUFFER)(MAP_SP_ACP_TX(pCmd)) );
    return ACP_OK;

err_exit:
	return ACP_ERR;

}/*end txsar_cmd */



/*
*******************************************************************************
*
*   FUNCTION:
*		ADVR_TxSar_Handler
*
*   PARAMETERS:
*		UINT8	fOnce	-- set TRUE if at most ONE inbound packet
*				   can be handled, eg. if polling.
*
*   RETURN:
*		TRUE if an inbound packet was handled, else FALSE.
*
*   ALGORITHMS
*		This routine does the TxSAR service, whether for
*		interrupt-level processing or polling mode activity.
*
*******************************************************************************
*/

UINT8
ADVR_TxSar_Handler (UINT8 fOnce)
{
tpQDESC     pAcpTxRespQ = acp_Txspram_info.txspram_pAcpRespq;
tpQDESC     pTxToAcpEventq = acp_Txspram_info.txspram_pTxToAcpEventq;
tpQDESC     pAcpToTxEventRespq = acp_Txspram_info.txspram_pAcpToTxEventRespq;
tpSARCMD    pRespBuf, pEvntBuf;
UINT8		*pData;
UINT32		length;
UINT8		RC		= FALSE ;
	
	/* 
	**
	**	First check for command completions 
	**
	*/

	while( (pRespBuf = qmgr_deq(pAcpTxRespQ)) != NULLP ) {
		
		/* convert response to acp address space */

		pRespBuf = (tpSARCMD) MAP_SP_TX_ACP(pRespBuf);
		pRespBuf->cmd_wakeup = 1;
		
	}/*while*/

	/* 
	**
	**	Now check if any Events received from TXSAR 
	**
	*/

	while( (pEvntBuf = qmgr_deq(pTxToAcpEventq) ) != NULL) {

		/* 	
		**	convert Event buffer ptr to ACP Address space 
		*/
		pEvntBuf = (tpSARCMD) MAP_SP_TX_ACP(pEvntBuf);

		/*	
		**	Process the event code 
		*/
		if(pEvntBuf->cmd_code == 
				((TXSAR_DATA_IND_C  & CMDCODE_MASK_C) >> 8)) {

		tpTXSAR_DIND	pDind;				/* data indication parameters	*/

			pDind = (tpTXSAR_DIND)pEvntBuf->cmd_parmv;
			pData = (UINT8 *)MAP_SH_SAR_ACP(pDind->txdind_pbuf);
			length = (UINT32)pDind->txdind_len;
			
			/*
			**	 Process the ipc data indication.  Note this
			**	routine should copy the data.
			*/

			ADVR_DispatchInPktUpCall
					(pEvntBuf->cmd_code, pData, length) ;

#ifdef DEBUG_PRINTF
			buginf("\n\r ADVR_TxSar_Handler: IPC message from NMP");
        	buginf("\n\r 	 cmd_code %d:", pEvntBuf->cmd_code);
			buginf("\n\r     data:   %02x %02x %02x %02x %02x %02x", 
								pData[0], pData[1], pData[2],
								pData[3], pData[4], pData[5]);
#endif /* DEBUG_PRINTF */
		
		} else if ( (pEvntBuf->cmd_code ==
						((TXSAR_START_LEARP_IND_C & CMDCODE_MASK_C) >> 8)) ||
					(pEvntBuf->cmd_code ==
						((TXSAR_CLEAR_MAC_IND_C & CMDCODE_MASK_C) >> 8)) ) {


		tpLEARP_PARAM	pMsg = (tpLEARP_PARAM)pEvntBuf->cmd_parmv;
		tpCAM_ENTRY 	pCamEntry = NULLP;

			pMsg->opcode = pEvntBuf->cmd_code;
			pMsg->opcode = (pMsg->opcode << 8);
			pMsg->learp_came = (void *) MAP_SP_TX_ACP(pMsg->learp_came);

			pCamEntry = (tpCAM_ENTRY)pMsg->learp_came;

			/*
			**	Post a message to the LEC task.  
			*/

#ifdef DEBUG_PRINTF
			buginf("\n\r ADVR_TxSar_Handler: start learp event from txsar");
			buginf("\n\r     mac:   %02x.%02x.%02x.%02x.%02x.%02x", 
								pMsg->learpp_dmacaddr.maddr[0],
								pMsg->learpp_dmacaddr.maddr[1],
								pMsg->learpp_dmacaddr.maddr[2],
								pMsg->learpp_dmacaddr.maddr[3],
								pMsg->learpp_dmacaddr.maddr[4],
								pMsg->learpp_dmacaddr.maddr[5]);
			buginf("\n\r     color: %02x", pMsg->learpp_vlanid);
        	buginf("\n\r 	 pCamEntry 0x%x:", pMsg->learp_came);
        	buginf("\n\r 	 state %d:", ((tpCAM_ENTRY)(pMsg->learp_came))->came_macstate);
        	buginf("\n\r 	 pacing %d:", ((tpCAM_ENTRY)(pMsg->learp_came))->came_pacing_counter);
        	buginf("\n\r 	 index %d:", ((tpCAM_ENTRY)(pMsg->learp_came))->came_index);
#endif /* DEBUG_PRINTF */

			ADVR_Dispatch_lec_arpevent(pMsg);


		} else {

			buginf("\n\rADVR_TxSar_Handler: invalid event code from txsar:0x%x",
                    * (UINT32*) pEvntBuf);

		} 


		/* 
		**
		**	Send Event Response back to TXSAR 
		**
		*/

		qmgr_enq(pAcpToTxEventRespq, (tpQELEM) MAP_SP_ACP_TX(pEvntBuf));
		atmdrv_interrupt_txsar();

		RC = TRUE ;

			/* This allows a hole and must NOT be used once interrupt */
			/* mode is used (ie. is for Boot/download polling ONLY).  */
			/* Consider a datagram interrupt followed by SAR cmd ack. */
			/* With fOnce we exit after clearing interrupts AND just  */
			/* handling the datagram.  Thus SAR cmd ack is dropped.   */

		if (fOnce)		/* Let single packet be processed. */
			break ;

	}/* while events from txsar */

	return (RC) ;

}	/* end ADVR_TxSar_Handler */


/*
*******************************************************************************
*
*   FUNCTION:
*		txsar_isr
*
*   PARAMETERS:	NONE
*
*   RETURN:	NONE
*
*   ALGORITHMS
*		ATM driver entry point for TxSAR interrupts.
*
*******************************************************************************
*/

void
txsar_isr()
{
	(void) ADVR_TxSar_Handler (FALSE) ;

}	/* end txsar_isr */


/*
*******************************************************************************
*
*   FUNCTION:
*		ADVR_RegInPktUpCall
*
*   PARAMETERS:
*		pFunc	-- pointer to client's upcall routine
*				   (for now, just have IPC client)
*
*   RETURN:	NONE
*
*   ALGORITHMS
*		Registers the client upcall routine to use for a TxSAR pkt.
*
*******************************************************************************
*/

void
ADVR_RegInPktUpCall (void * pFunc)
{
	ADVR_Std_TSar_Upcall = (tADVR_PktUpCall *) pFunc ;

}	/* end ADVR_RegInPktUpCall */


/*
*******************************************************************************
*
*   FUNCTION:
*       ADVR_RegLearpUpCall
*
*   PARAMETERS:
*       pFunc   -- pointer to client's upcall routine
*                  lec enq handler
*
*   RETURN: NONE
*
*   ALGORITHMS
*       Registers the client upcall routine to use for start le arp event
*
*******************************************************************************
*/


void
ADVR_RegLearpUpCall (void * pFunc)
{

	ADVR_Std_TSar_LeArpUep = (tADVR_LearpUpCall *) pFunc ;

}	/* end ADVR_RegLearpUpCall */


/*
*******************************************************************************
*
*   FUNCTION:
*		ADVR_DispatchInPktUpCall
*
*   PARAMETERS:
*		UINT8	CmdCode	-- used to distinguish packet clients
*				   (for now, just have IPC client)
*		UINT8 *	pData	-- pointer to received data/packet buffer
*		UINT32	length	-- length of received data/packet buffer
*
*   RETURN:	NONE
*
*   ALGORITHMS
*		Finds and calls the client upcall routine for a given packet.
*
*******************************************************************************
*/

void
ADVR_DispatchInPktUpCall (UINT8 CmdCode, UINT8 * pData, UINT32 length)
{
		/* For now, we just have a single client, IPC.  This can */
		/* be called from interrupt or polling mode, though.     */
		/* Return the upcall routine registered at startup.      */

	CmdCode = 0 ;		/* Avoid compiler warnings re: unreferenced. */

	if (ADVR_Std_TSar_Upcall != NULL)
		(* ADVR_Std_TSar_Upcall) (pData, length) ;

}	/* end ADVR_DispatchInPktUpCall */


/*
*******************************************************************************
*
*   FUNCTION:
*       ADVR_Dispatch_lec_arpevent
*
*   PARAMETERS:
*       
*
*   RETURN: NONE
*
*   ALGORITHMS
*       Invokes learp event handler in alc_lec.c
*
*******************************************************************************
*/
 
void
ADVR_Dispatch_lec_arpevent(tpLEARP_PARAM  pMsg)
{

	if(ADVR_Std_TSar_LeArpUep != NULLP) {
		(* ADVR_Std_TSar_LeArpUep) (pMsg);
	} 

}


/*
*******************************************************************************
*
*   FUNCTION:
*           atm_get_vpc
*
*   PARAMETERS:
*           tVCI vci
*			VPI vpi
*
*   RETURN:
*           tVPC	vpc
*
*   ALGORITHMS
*   	returns normalised vpc
*
*******************************************************************************
*/
 
tVPC
atm_get_vpc(tVCI vci, tVPI vpi) 
{

	if (vci > ( (1<< num_active_vci_bits)-1) ) {
		return (0);
	}
	if (vpi > ( (1<< num_active_vpi_bits)-1) ) {
		return(0);
	}

	return ((vci | (vpi << num_active_vci_bits)));

}/*end atm_get_vpc */



/*
*******************************************************************************
*
*   FUNCTION:
*           aalsap_open_vcc
*
*   RETURN:
*
*   ALGORITHMS
*
*******************************************************************************
*/

int
aalsap_open_vcc (UINT16 portindex, 
				 UINT16 vcd,
				 tVPI vpi, 
				 tVCI vci, 
				 tQOS qos, 
				 UINT16 txsar_state, 
				 UINT16 rxsar_state,
				 UINT16 vlan,
				 UINT16 lecid,
				 void (*aal_user_uep)(UINT16  vcd,
									  UINT16  event,
									  UINT32  length,
									  void    *pData) )
{
	tVPC vpc;
	tVPCPARM CmdParm;
	tpIMAPENTRY rx_vc_info_ptr;
	UINT8	*pCHAR;
	UINT32	i;

	if (portindex >= MAX_PORT) {
		return(0);
	}

	/* Reduce VCI and VPI. If out of range return 0 (indicating error) */
	vpc = atm_get_vpc(vci, vpi);
	if (vpc == 0) {
		return(0);
	}

	/* Check Current State of VPC, if busy, then return 0 */
	if (AtmdrvGcb.sap_entry[portindex][vpc].vcstate != VPC_INACTIVE) {
		return(0);
	} 

	/* Set the pointers to per-VC statistics area (for SARs) */
	CmdParm.vpcp_pRVCStat = &rxsar_pRxSpram->rxstat_vc[vpc];
	CmdParm.vpcp_pTVCStat = &txsar_pTxSpram->txstat_vc[vpc];

	/* Open VC with RX SAR */
   	CmdParm.vpcp_vpc = vpc;
	CmdParm.vpcp_lecid = acp_pRxSpram->MapTable[vpc].imap_lecid = lecid;

	switch(rxsar_state) {

		case VPC_DEST_ACP:
			CmdParm.vpcp_vcstate = VPC_DEST_ACP;
			break;

		case VPC_DEST_BIGA:
		case VPC_SNAP_PVC_TO_BIGA:

			/* Note: The actual state for this VC is setup in aalsap_bind_vlan_to_vc() */
			/* The RXSAR should send data from this VC to the Synergy bus only after   */
			/* the VLAN number is known.											   */

			CmdParm.vpcp_vcstate = (rxsar_state | VPC_WT_TL_BIND_BIT);

			atm_drv_fillsyhdr(vpc, vpc, vlan, 0);

			CmdParm.vpcp_vlan_cl = acp_pRxSpram->MapTable[vpc].imap_vlan_cl;

			CmdParm.vpcp_cid_vpc = acp_pRxSpram->MapTable[vpc].imap_cid_vpc;
			break;

		case VPC_DEST_BIGA_FROM_BUS:

			/* Note: The actual state for this VC is setup in aalsap_lec_map_bus_vcds() */
			/* The RXSAR should send data from this VC to the Synergy bus only after 	*/
			/* the LECID and VLAN number is known.									 	*/

			CmdParm.vpcp_vcstate =(VPC_DEST_BIGA_FROM_BUS | VPC_WT_TL_BIND_BIT);
			/* 
			 * Note: The index value in Synergy Hdr will be changed by
			 * 	     a call to aalsap_lec_map_bus_vcds().
	 		 */
			atm_drv_fillsyhdr(vpc, vpc, vlan, 1);

			CmdParm.vpcp_vlan_cl = acp_pRxSpram->MapTable[vpc].imap_vlan_cl;

			CmdParm.vpcp_cid_vpc = acp_pRxSpram->MapTable[vpc].imap_cid_vpc;
			break;

		case VPC_RCV_FOR_BUS:
			/* Note: The actual state for this VC is setup in aalsap_bus_map_bus_vcds()   */
            /* The RXSAR should send data from this VC to the TSAR only after the 	 	  */
            /* outgoing VC information is known.                       				 	  */

            CmdParm.vpcp_vcstate =(VPC_RCV_FOR_BUS | VPC_WT_TL_BIND_BIT);
            /*
			 * Since this packet goes to the TSAR, this call is only needed to set
			 * the outgoing VPC in the VC Info Table.
             */
            atm_drv_fillsyhdr(vpc, vpc, vlan, 0);

            CmdParm.vpcp_cid_vpc = vpc; /* the real outgoing vpc value will be filled later */
			break;
	

		default:
#ifdef DEBUG_PRINTF
			printf("\naalsap_open_vcc: Illegal Destination\n");
#endif /* DEBUG_PRINTF */
			return(0);

	}/*switch */

    /* Assign a memory area for RXSAR VC Information (ivci) struct.
	 * If VC is destination ACP, then assign VC Info in Secondary port memory
	 * otherwise try to assign VC Info in VCR.
	 * The only exception to this is the Signalling VC (VPI==0, VCI==5) which
	 * is assigned space in the VCR even though it is destination ACP.
	 */

	if (rxsar_state == VPC_DEST_ACP && vpc != 5) {
    	rx_vc_info_ptr = NULLP;
	} else {
    	rx_vc_info_ptr = get_pVCR_ivci_map_entry();
	}
 
    if (rx_vc_info_ptr != NULLP) { 
        CmdParm.vpcp_pVcMapEntry =
		AtmdrvGcb.sap_entry[portindex][vpc].pVCR_ivci_map_entry = rx_vc_info_ptr;

		/* Map table entry pointer points to VCR Memory */
		acp_pRxSpram->MapEntryPtrTable[vpc] = rx_vc_info_ptr;
    } else {
        /* Use Secondary Port Memory if no space available in VCR */ 
        CmdParm.vpcp_pVcMapEntry = &rxsar_pRxSpram->MapTable[vpc];
		AtmdrvGcb.sap_entry[portindex][vpc].pVCR_ivci_map_entry = NULLP;

		/* Map table entry pointer points to Secondary Memory */
		acp_pRxSpram->MapEntryPtrTable[vpc] = &rxsar_pRxSpram->MapTable[vpc];
    }

    if (ACP_ERR == rxsar_cmd(RXSAR_SET_VPCMAP_C, &CmdParm)) {
		if (AtmdrvGcb.sap_entry[portindex][vpc].pVCR_ivci_map_entry != NULLP) {
			rel_VCR_ivci_map_entry(rx_vc_info_ptr);
        	AtmdrvGcb.sap_entry[portindex][vpc].pVCR_ivci_map_entry = NULLP;

			/* Restore Map table entry pointer to point to Secondary Memory */
			acp_pRxSpram->MapEntryPtrTable[vpc] = &rxsar_pRxSpram->MapTable[vpc];

			printf("\n\raalsap_open_vcc: rxsar_cmd FAILED");
		}
		return(0);
	}

	/* Open VC with TX SAR (If it fails, then close VC with RXSAR before returning) */


	CmdParm.vpcp_vcstate = txsar_state;

    if (ACP_ERR == txsar_cmd(TXSAR_SET_VPCMAP_C, &CmdParm)) {
		CmdParm.vpcp_vcstate = VPC_INACTIVE;
		if(ACP_ERR==rxsar_cmd(RXSAR_CLEAR_VPCMAP_C, &CmdParm)) {
			printf("\n\r aalsap_open_vcc: RXSAR_CLEAR_VPCMAP_C failed");
			/* call system exit */
		}
		if (AtmdrvGcb.sap_entry[portindex][vpc].pVCR_ivci_map_entry != NULLP) {
			rel_VCR_ivci_map_entry(rx_vc_info_ptr);
        	AtmdrvGcb.sap_entry[portindex][vpc].pVCR_ivci_map_entry = NULLP;

			/* Restore Map table entry pointer to point to Secondary Memory */
			acp_pRxSpram->MapEntryPtrTable[vpc] = &rxsar_pRxSpram->MapTable[vpc];
		}

		/* Initialize Rx VC MAP in Secondary Memory to zero */
		pCHAR = (UINT8 *) MAP_SP_RX_ACP(&rxsar_pRxSpram->MapTable[vpc]);
        for (i = 0; i < sizeof(tIMAPENTRY); i++) {
            *pCHAR++ = 0;
        }

		return(0);
	}


	/* store User's Upcall Entry Point in Driver VC Table */
	AtmdrvGcb.sap_entry[portindex][vpc].sap_uep = aal_user_uep;
	/* Update VC State */
	AtmdrvGcb.sap_entry[portindex][vpc].vcstate = rxsar_state;
	/* Store VCD number */
	AtmdrvGcb.sap_entry[portindex][vpc].vcd = vcd;
	/* Store VPC number in vcd_vpc_map Table */
	vcd_vpc_map[portindex][vcd] = vpc;

	/*
	**	Clear stats for this vc
	*/
	memset((char *)&acp_pRxSpram->rxstat_vc[vpc], 0x00, sizeof(tRXSARVCSTAT));
	memset((char *)&acp_pTxSpram->txstat_vc[vpc], 0x00, sizeof(tTXSARVCSTAT));

	return(1);

}/*end aalsap_open_vcc */

/*
*******************************************************************************
*
*   FUNCTION:
*		get_vpc_from_vcd:		Returns a vcpi given a vcd
*
*   PARAMETERS:
*			UINT16	vcd
*   RETURN:
*           0xffff No mapping
*
*   ALGORITHMS
*
*
*******************************************************************************
*/

tVPC
get_vpc_from_vcd(UINT16  portindex, UINT16  vcd)
{

	if(vcd >= MAX_DEFAULT_VPC) {
		return 0xffff;
	} else {
		return (vcd_vpc_map[portindex][vcd]);
	}

}/*end get_vpc_from_vcd*/



/*
*******************************************************************************
*
*   FUNCTION:
*       get_vcd_from_vpc       Returns a vcd given a vpci
*
*   PARAMETERS:
*           tVPC    vpc
*   RETURN:
*
*           0xffff No mapping
*
*   ALGORITHMS
*
*
*******************************************************************************
*/

UINT16
get_vcd_from_vpc(UINT16 portindex, tVPC  vpci)
{

    if( (vpci >= MAX_DEFAULT_VPC) || 
	 	(AtmdrvGcb.sap_entry[portindex][vpci].vcstate == VPC_INACTIVE) ) {
        return 0xffff;
    } else {
        return (AtmdrvGcb.sap_entry[portindex][vpci].vcd);
    }

}


UINT32
aalsap_is_vpc_active(tVPC  vpc)
{

    if ( (vpc >= MAX_DEFAULT_VPC) || 
		(AtmdrvGcb.sap_entry[0][vpc].vcstate == VPC_INACTIVE) ) {
		return FALSE;
	} else {
		return TRUE;
	}
}

/*
*******************************************************************************
*
*   FUNCTION:
*           aalsap_lec_map_bus_vcds: For BUS Multicast VC from a Client perspective,
*								 associates a different outgoing VC with 
*                                incoming VC. Also binds the VLAN and LECID.
* 			NOTE: Both VCs must ALREADY be open at this time.
*
*   PARAMETERS:
*
*   RETURN:
*           1 means OK
*           0 means call failed.
*
*   ALGORITHMS
*       
*
*******************************************************************************
*/
int 
aalsap_lec_map_bus_vcds(UINT16 portindex,
                    UINT16 from_bus_vcd,    /* The VC to be modified  */
                    UINT16 to_bus_vcd,      /* VCD Encoded in Syn Hdr */
					UINT16 vlan,
					UINT16 lecid)
{
	tVPC from_bus_vpc, to_bus_vpc;
	tVPCPARM CmdParm;
	UINT32 i;
	
	if (portindex >= MAX_PORT) {
		return(0);
	}

	from_bus_vpc = vcd_vpc_map[portindex][from_bus_vcd];
	if (from_bus_vpc == 0xffff) {
		printf("\n\raalsap_lec_map_bus_vcds: from_bus_vcd: %d not setup", from_bus_vcd);
		return(0);
	}
	to_bus_vpc = vcd_vpc_map[portindex][to_bus_vcd];
	if (to_bus_vpc == 0xffff) {
		printf("\n\raalsap_lec_map_bus_vcds: to_bus_vcd: %d not setup", to_bus_vcd);
		return(0);
	}

	/* Setup Synergy Header, since BUS VC, set conditional-learn parameter */
    atm_drv_fillsyhdr(from_bus_vpc,to_bus_vpc,vlan,1);

	/* First inform RX SAR of LECID */
	CmdParm.vpcp_vpc = from_bus_vpc;
	CmdParm.vpcp_lecid =
    acp_pRxSpram->MapTable[from_bus_vpc].imap_lecid = lecid;

	CmdParm.vpcp_vcstate = VPC_DEST_BIGA_FROM_BUS;
	CmdParm.vpcp_pVcMapEntry = acp_pRxSpram->MapEntryPtrTable[from_bus_vpc];
	CmdParm.vpcp_vlan_cl = acp_pRxSpram->MapTable[from_bus_vpc].imap_vlan_cl;
	CmdParm.vpcp_cid_vpc = acp_pRxSpram->MapTable[from_bus_vpc].imap_cid_vpc;


	/* Set the pointers to per-VC statistics area (for RXSAR) */
	CmdParm.vpcp_pRVCStat = &rxsar_pRxSpram->rxstat_vc[from_bus_vpc];

	if (ACP_ERR == rxsar_cmd(RXSAR_MODIFY_VPCMAP_C, &CmdParm)) {
		printf("\n\raalsap_lec_map_bus_vcds: rxsar_cmd FAILED");
		return(0);
	}

	/* Now inform TX SAR of LECID */

	acp_pTxSpram->LecTable[vlan].lece_lecid = lecid;
	acp_pTxSpram->LecTable[vlan].lece_busvpc = to_bus_vpc;

	return(1);
}



/*
**      aalsap_bus_map_bus_vcds: Map INCOMING TO BUS VC to 
**                   OUTGOING FROM BUS VC.
**      NOTE: Both VCs must ALREADY be open at this time.
**      Return: 1 means OK
**              0 means call failed.
*/
int
aalsap_bus_map_bus_vcds(UINT16 portindex,
                    UINT16 in_vcd,    	/* The VC to be modified     INCOMING TO BUS FROM NETWORK */
                    UINT16 out_vcd)		/* VCD to send packet out on OUTGOING FROM BUS TO NETWORK */
{
    tVPC in_vpc, out_vpc;
    tVPCPARM CmdParm;
   
    if (portindex >= MAX_PORT) {
        return(0);
    }

    in_vpc = vcd_vpc_map[portindex][in_vcd];
    if (in_vpc == 0xffff) {
        printf("\n\raalsap_bus_map_bus_vcds: in_vcd: %d not setup", in_vcd);
        return(0);
    }
    out_vpc = vcd_vpc_map[portindex][out_vcd];
    if (out_vpc == 0xffff) {
        printf("\n\raalsap_bus_map_bus_vcds: out_vcd: %d not setup", out_vcd);
        return(0);
    }

    /* Inform RX SAR of VC bindings */
    CmdParm.vpcp_vpc = in_vpc;

    CmdParm.vpcp_vcstate = VPC_RCV_FOR_BUS;
    CmdParm.vpcp_pVcMapEntry = acp_pRxSpram->MapEntryPtrTable[in_vpc];
    CmdParm.vpcp_cid_vpc = out_vpc;

    /* Set the pointers to per-VC statistics area (for RXSAR) */
    CmdParm.vpcp_pRVCStat = &rxsar_pRxSpram->rxstat_vc[in_vpc];

    if (ACP_ERR == rxsar_cmd(RXSAR_MODIFY_VPCMAP_C, &CmdParm)) {
        printf("\n\raalsap_bus_map_bus_vcds: rxsar_cmd FAILED");
        return(0);
    }

    return(1);
}



/*
*******************************************************************************
*
*   FUNCTION:
*           aalsap_bind_vlan_to_vc: Associate VLAN with NON-BUS data VC.
*           NOTE: VC must ALREADY be open at this time.
*				  *** Important *** DO NOT use this function for BUS VCs.
*									use aalsap_lec_map_bus_vcds() instead.
*
*   PARAMETERS:
*
*   RETURN:
*           1 means OK
*           0 means call failed.
*
*   ALGORITHMS
*
*
*******************************************************************************
*/
int
aalsap_bind_vlan_to_vc( UINT16 portindex,
                    	UINT16 vcd,
						UINT16 vlan,
                        UINT32 vcstate)
{
	tVPC vpc;
	tVPCPARM CmdParm;
	UINT32 i;
	
	if (portindex >= MAX_PORT) {
		return(0);
	}

	vpc = vcd_vpc_map[portindex][vcd];
	if (vpc == 0xffff) {
		printf("\n\raalsap_bind_vlan_to_vc: vcd: %d not setup", vcd);
		return(0); /* VCD hasn't been setup yet */
	}

	atm_drv_fillsyhdr(vpc, vpc, vlan, 0);

	/* Now inform RX SAR of vlan number */
	CmdParm.vpcp_vpc = vpc;
    CmdParm.vpcp_lecid = acp_pRxSpram->MapTable[vpc].imap_lecid; /* not needed */
 
    CmdParm.vpcp_vcstate = vcstate;
    CmdParm.vpcp_pVcMapEntry = acp_pRxSpram->MapEntryPtrTable[vpc];
	CmdParm.vpcp_vlan_cl = acp_pRxSpram->MapTable[vpc].imap_vlan_cl;
	CmdParm.vpcp_cid_vpc = acp_pRxSpram->MapTable[vpc].imap_cid_vpc;
 
	/* Set the pointers to per-VC statistics area (for RXSAR) */
	CmdParm.vpcp_pRVCStat = &rxsar_pRxSpram->rxstat_vc[vpc];


    if (ACP_ERR == rxsar_cmd(RXSAR_MODIFY_VPCMAP_C, &CmdParm)) {
        printf("\n\raalsap_bind_vlan_to_vc: rxsar_cmd FAILED");
        return(0);
    }


	return(1);
}

/*
*******************************************************************************
*
*   FUNCTION:
*           aalsap_change_vcd_clbit: For BUS Multicast VC from a Client 
*									 perspective, changes the Conditional
*									 Learn bit from on to off or vice versa.
*           NOTE1: Both VCs must ALREADY be open at this time.
*
*   PARAMETERS:
*
*   RETURN:
*           1 means OK
*           0 means call failed.
*
*   ALGORITHMS
*
*
*******************************************************************************
*/
int
aalsap_change_vcd_clbit(UINT16 portindex,
                    	UINT16 from_bus_vcd,    /* The VC to be modified  */
                    	UINT16 to_bus_vcd,      /* VCD Encoded in Syn Hdr */
                    	UINT16 vlan,
						UINT16 c_l)
{
	tVPC from_bus_vpc, to_bus_vpc;
    tVPCPARM CmdParm;
	int i;
   
    if (portindex >= MAX_PORT) {
        return(0);
    }
	
	from_bus_vpc = get_vpc_from_vcd(portindex, from_bus_vcd);
	if (from_bus_vpc == 0xffff) {
        printf("\n\raalsap_change_vcd_clbit: from_bus_vcd: %d not setup", 
					from_bus_vcd);
        return(0);
    }
	to_bus_vpc = get_vpc_from_vcd(portindex, to_bus_vcd);
    if (to_bus_vpc == 0xffff) {
        printf("\n\raalsap_change_vcd_clbit: to_bus_vcd: %d not setup", 
					to_bus_vcd);
        return(0);
    }

	/* Setup Synergy Header */
	atm_drv_fillsyhdr(from_bus_vpc, to_bus_vpc, vlan, c_l);

	/* Now inform RX SAR of vlan number */
	CmdParm.vpcp_vpc = from_bus_vpc;
	CmdParm.vpcp_lecid = acp_pRxSpram->MapTable[from_bus_vpc].imap_lecid;

	CmdParm.vpcp_vcstate = VPC_DEST_BIGA_FROM_BUS;
	CmdParm.vpcp_pVcMapEntry = acp_pRxSpram->MapEntryPtrTable[from_bus_vpc];
	CmdParm.vpcp_vlan_cl = acp_pRxSpram->MapTable[from_bus_vpc].imap_vlan_cl;
	CmdParm.vpcp_cid_vpc = acp_pRxSpram->MapTable[from_bus_vpc].imap_cid_vpc;

	/* Set the pointers to per-VC statistics area (for RXSAR) */
	CmdParm.vpcp_pRVCStat = &rxsar_pRxSpram->rxstat_vc[from_bus_vpc];

	if (ACP_ERR == rxsar_cmd(RXSAR_MODIFY_VPCMAP_C, &CmdParm)) {
		printf("\n\raalsap_change_vcd_clbit: rxsar_cmd FAILED");
		return(0);
	}
	return(1);
}



/*
*******************************************************************************
*
*   FUNCTION:
*           aalsap_close_vcc
*
*   PARAMETERS:
*			tVPC  vpc
*			UINT16 portindex
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*       
*
*******************************************************************************
*/

void
aalsap_close_vcc(tVPC  vpc, UINT16 portindex)
{
	tVPCPARM CmdParm;
	UINT8	*pCHAR;
	UINT32	i;
	UINT16  vcd;

	if (portindex >= MAX_PORT) {
		return;
	}

    /* Check Current State of VPC, if already inactive, then return */
    if (AtmdrvGcb.sap_entry[portindex][vpc].vcstate == VPC_INACTIVE) {
        return;
    }

	/* Close VC with RXSAR */
   	CmdParm.vpcp_vpc = vpc;
	CmdParm.vpcp_vcstate = VPC_INACTIVE;
	CmdParm.vpcp_pVcMapEntry = &rxsar_pRxSpram->MapTable[vpc];

    if (ACP_ERR == rxsar_cmd(RXSAR_CLEAR_VPCMAP_C, &CmdParm)) {
		printf("aalsap_close_vcc: Could not close RXSAR VC!\n");
		/* call system exit */
	}

	/* Initialize Rx VC MAP in Secondary Memory to zero */
	pCHAR = (UINT8 *) MAP_SP_RX_ACP(&rxsar_pRxSpram->MapTable[vpc]);
    for (i = 0; i < sizeof(tIMAPENTRY); i++) {
		*pCHAR++ = 0;
	}

	/* If ivci map entry in VCR, release it */
	if (AtmdrvGcb.sap_entry[portindex][vpc].pVCR_ivci_map_entry != NULLP) {
		rel_VCR_ivci_map_entry(AtmdrvGcb.sap_entry[portindex][vpc].pVCR_ivci_map_entry);

		AtmdrvGcb.sap_entry[portindex][vpc].pVCR_ivci_map_entry = NULLP;

		/* Restore Map table entry pointer to point to Secondary Memory */
		acp_pRxSpram->MapEntryPtrTable[vpc] = &rxsar_pRxSpram->MapTable[vpc];
	}

	/* Close VC with TXSAR */
    if (ACP_ERR == txsar_cmd(TXSAR_CLEAR_VPCMAP_C, &CmdParm)) {
		printf("aalsap_close_vcc: Could not close TXSAR VC!\n");
		/* call system exit */
	}


	vcd_vpc_map[portindex][AtmdrvGcb.sap_entry[portindex][vpc].vcd] = 0xffff;
    AtmdrvGcb.sap_entry[portindex][vpc].vcstate = VPC_INACTIVE;

}/*end aalsap_close_vcc */


/*
*******************************************************************************
*
*   FUNCTION:
*           aalsap_vc_per_vp: Modify number of VCI per VPI
*
*   PARAMETERS:
*
*   RETURN:
*           1 means OK
*           0 means call failed due to invalid num_vcs value
*
*   ALGORITHMS
*
*
*******************************************************************************
*/
int
aalsap_vc_per_vp(UINT32 num_vcs)
{
    tRX_SET_VPI_VCI_BITS_REQ RCmdParm;
    tTX_SET_VPI_VCI_BITS_REQ TCmdParm;
	UINT32 i;

	/* Check validity of num_vcs */
	/* ... TBD ... */

	if(num_vcs < 32 || num_vcs > 1024) {
		return(0);
	}

	num_active_vci_bits = 0;
	i = num_vcs;
	while (i != 1) {
		i = i>>1;
		num_active_vci_bits++;
	}

	num_active_vpi_bits = num_active_vpi_vci_bits - num_active_vci_bits;

	if (num_vcs != (1 << num_active_vci_bits)) {
		/* num_vcs not a power of 2 */
		return(0);
	}

    /* First inform RX SAR of change in vc per vp */
    RCmdParm.rx_active_vpi_vci_mask = (((1<<num_active_vci_bits)-1)<<4) |
									 (((1<<num_active_vpi_bits)-1)<<20);
	
    RCmdParm.rx_num_vpi_shift_bits = (16 - num_active_vci_bits);

    if (ACP_ERR == rxsar_cmd(RXSAR_SET_VPI_VCI_BITS_C, &RCmdParm)) {
        printf("\n\aalsap_vc_per_vp: rxsar_cmd FAILED");
        return(0);
    }

    /* Now inform TX SAR of change in vc per vp */
	TCmdParm.tx_active_vci_mask = ((1<<num_active_vci_bits)-1);
   
    TCmdParm.tx_num_active_vci_bits = num_active_vci_bits;
 
    if (ACP_ERR == txsar_cmd(TXSAR_SET_VPI_VCI_BITS_C, &TCmdParm)) {
        printf("\n\aalsap_vc_per_vp: txsar_cmd FAILED");
        return(0);
    }


	return(1);
}


/*
*******************************************************************************
*
*   FUNCTION:
*           atm_drv_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*		Initialize vci maps, shram, spram, cam, ntc and sars.
*
*******************************************************************************
*/
 

void
atm_drv_init()
{
UINT32 i, j;
UINT32* pLCP ;			/* HACK-O-MATIC */
volatile UINT8	*pReg ;

#ifdef DEBUG_PRINTF
	printf("\n\r atm_drv_init");
#endif /* DEBUG_PRINTF */

        /* Ensure TSAR/RSAR are in reset.   This could also be a good */
	/* place to download them ....                                */
#ifdef DEBUG_PRINTF
	printf("\n\r atm_drv_init: putting sar's into reset\n\r");
#endif /* DEBUG_PRINTF */
    pReg = (UINT8 *) (ACP_HW_IOREG_BASE) ;
	BDG_ResetFrBufUsr (ACP_WRRG_TSAR_RESET_) ;
	BDG_ResetFrBufUsr (ACP_WRRG_RSAR_RESET_) ;

	/* Initialize driver registration data structures etc. */
	ADVR_Std_TSar_Upcall   = NULL ;
	ADVR_Std_TSar_LeArpUep = NULL ;

	/* This flag is set to FALSE if we want the kernel-mode driver */
	/* to have edisms-ing right away.  This breaks IOS init during */
	/* NVRAM config load, since config commands may invoke the ATM */
	/* driver (and edisms will break run-to-completion of the init */
	/* process).  Thus we want polling mode first, until IOS confg */
	/* has been fully loaded.  Then, some platform-specific system */
	/* (IOS) code can set this flag back to FALSE.                 */
	fADVR_ForceEvPoll      = TRUE ;		/* Was FALSE */

	/* Initialize Structure that keeps track of which VC Maps are in VCR */
	VCR_ivci_map_info_init();

	/* Initialize number of active VPI and VCI bits */
	num_active_vpi_vci_bits = VPI_BITS_C + VCI_BITS_C;
	num_active_vpi_bits = VPI_BITS_C;
	num_active_vci_bits = VCI_BITS_C;

	/* Initialize throttle parameter */
	AtmdrvGcb.throttle_parm = MAX_OC3_MBITS_PER_SEC;

    /* 
	**
	**	INITIALIZE Shared ram
	**
	*/

    if (ACP_ERR == shmem_init()) {
        printf("Memory initialization Error\n");
        return;
    }  

    /* 
	**
	**	INITIALIZE RX spram
	**
	*/

    if (ACP_ERR == rx_mem_init()) {
        printf("Memory initialization Error\n");
        return;
    }  

    /* 
	**
	**	INITIALIZE TX spram
	**
	*/

    if (ACP_ERR == tx_mem_init()) {
        printf("Memory initialization Error\n");
        return;
    }  

	/* Setup Synergy Header for IPC */
	atm_drv_fillsyhdr(0, 0, 0, 0);

#ifdef DEBUG_PRINTF
    /* Print Memory Addresses */
    spram_log();
#endif /* DEBUG_PRINTF */

	/* Initialize all entries in VC Table to Inactive State */
	pAtmdrvGcb = &AtmdrvGcb;

	for(i = 0; i < MAX_PORT; i++) {
		for(j = 0; j < MAX_DEFAULT_VPC_C; j++) {
			AtmdrvGcb.sap_entry[i][j].vcstate = VPC_INACTIVE;
			AtmdrvGcb.sap_entry[i][j].sap_uep = NULLP;

			vcd_vpc_map[i][j] =  0xffff;
		}
	}

	/*
	**
	**	INITIALIZE BIGA's
	**
	*/

	(void)biga_init();
	
#ifdef	ACP_REV0_LCP
	/*
	**
	**	Wait for sage / lcp to come online
	**  This is a HACK until the LCP driver is finished.
	**  DO NOT DO THIS IN THE FINAL PRODUCT !!!
	**
	*/
	pLCP = (UINT32 *)(0x28000200) ;
	pReg [ACP_WRRG_LCP_RESET_] = ON_ ;
	for (i = 0 ; i < 16 ; ++ i)  { }
	*pLCP = 0 ;
	pReg [ACP_WRRG_LCP_RESET_] = OFF_ ;
	while(1) {
    UINT32  status1 ;
    UINT32  status2 ;

		status1 = *pLCP ;
		status2 = *(pLCP + 1) ;

#ifdef DEBUG_PRINTF
        printf("\n\r bigatst_checkinit: status1 = %x ", status1);
        printf("\n\r bigatst_checkinit: status2 = %x ", status2);
#endif /* DEBUG_PRINTF */
        if( (status1 & 0x00ffffff)  == 0x00868008) {
#ifdef DEBUG_PRINTF
            printf("\n\r bigatst_checkinit: status1 = %x ", status1);
            printf("\n\r bigatst_checkinit: status2 = %x ", status2);
#endif /* DEBUG_PRINTF */
			break;
		}

		PUNCH_WD;
	}
	
#endif

	/*
	**
	**	INITIALIZE PMD CHIP
	**
	*/
	pmd_chip_type = rd_pmd_type();

	switch (pmd_chip_type) {
		case PMD_NTCa:
		case PMD_NTCb:
			ntc_init();
			break;
		case PMD_PMC:
        	syn_atm_pmc_oc3_init(); /* Init PMC Chip */
			break;
		case PMD_NO_EEPROM:
		default:
			buginf("\n\r *** WARNING: PMD EEPROM invalid; assuming type PMC");
			pmd_chip_type = PMD_PMC;
        	syn_atm_pmc_oc3_init(); /* Init PMC Chip */
			break;
	}



	/*
	**
	** INITIALIZE SARS (groom MFP for them too).
	**
	*/

	MFP_MaskIntrW   ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), TRUE);
	MFP_EnableIntrW ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), TRUE);

#ifdef DEBUG_PRINTF
	printf("\n\r atm_drv_init: dropping sar's out of reset");
#endif /* DEBUG_PRINTF */
        /* Drop TSAR/RSAR out of reset. */
    pReg [ACP_WRRG_TSAR_RESET_] = OFF_ ;					/* For TxSAR. */
    pReg [ACP_WRRG_RSAR_RESET_] = OFF_ ;					/* For RxSAR. */

	/* ------------------------------------------------------- */
	/* Assume that interrupts have been masked off at the MFP. */
	/* Temporarily sync to PMON-based serial-line downloads.   */
	/* A set of init-only interrupt ISRs have been bound.      */
	/* Allow interrupts NOW, then re-mask when init done.      */
	/* Caller will then bind real ISRs and un-mask interrupts. */
	/* ------------------------------------------------------- */
	/* Since this is system start-up code, assume that spin    */
	/* wait is okay.  We are NOT punching the watchdog here... */
	/* ------------------------------------------------------- */
	sar_intr_status = 0;
	crc_err		= 0 ;
	rto_err		= 0 ;
	buf_ovfl_err	= 0 ;
	len_err		= 0 ;

#if	0
	while( sar_intr_status != (RXSAR_INTR | TXSAR_INTR) ) {
		wait_for_interrupt();
	}
#else
	wait_for_interrupt () ;
#endif
	
	sar_intr_status = 0;

    /*
    **
    **  INITIALIZE CAM
    **
    */
 
#ifdef DEBUG_PRINTF
    printf("\n\r atm_drv_init: Initializing CAMs");
#endif /* DEBUG_PRINTF */
#ifdef MOVED_TO_INIT_ALC
    if(cam_init() != ACP_OK) {
        printf("\n\r atm_drv_init: cam_init failed");
		while(1) {
			;							/* crash and burn */
		}
        return;
    }
#endif /* MOVED_TO_INIT_ALC */

	return;

}/*end atm_drv_init */



void
atm_drv_fillsyhdr(tVPC from_vpc, tVPC to_vpc, UINT16 vlan, UINT16 c_l)
{
	acp_pRxSpram->MapTable[from_vpc].imap_vlan_cl = (vlan << 1) | c_l;
	acp_pRxSpram->MapTable[from_vpc].imap_cid_vpc = 
									(LCP_CardInstanceNbr() <<13) | to_vpc;
}/*end atm_drv_fillsyhdr*/



int
aalsap_send(tVPC vpc, UINT16 portindex, UINT32 datalen, void *pData)
{
/*
**
** aalsap_send:  Send data on indicated SAP.
**        Note:  Data is copied from DRAM to Frame Buffer in this function.
**
** Return code: OK                                    :  0
**              Couldn't allocate command/data buffer : -1
**              Bad port                              : -2
**              VC not active                         : -3
**              Packet too long                       : -4
**
*/

	tTXSEND_REQ	Sreq;
	tpBUFFER	pBuf = NULLP;
	tpQDESC     pAcpDataPool = acp_Shram_info.shram_pAcpToSarPool;

	/* Check portindex (in Synergy ATM Card, has to be 0) */
    if (portindex >= MAX_PORT) {
		return(-2);
    }

	/* Check length */
	if (datalen > ACP_TX_BUFSZ) {
		return(-4);
	}

    /* Check Current State of VPC, if INACTIVE, then return -3 */
    if (AtmdrvGcb.sap_entry[portindex][vpc].vcstate == VPC_INACTIVE) {
		return(-3);
    } 

	/* Get buffer in Frame Buffer Memory */
	if ((pBuf = (tpBUFFER) bufmgr_getbuf(pAcpDataPool)) == NULLP) {
		return(-1);
	}
	pBuf = (tpBUFFER) MAP_SH_SAR_ACP(pBuf);

    /* Copy data from paktype buffer in ACP DRAM to Frame Memory buffer */
    bcopy(pData, &pBuf->buf_data, datalen);
 
    Sreq.txsend_vpc = vpc;
    Sreq.txsend_len = datalen;
    Sreq.txsend_pbuf = (void *) &pBuf->buf_data;
 
    if (txsar_cmd(TXSAR_SEND_REQUEST_C, (void *)&Sreq) == ACP_ERR) {
        bufmgr_freebuf(pAcpDataPool, (tpBUFFER) MAP_SH_ACP_SAR(pBuf));
        return(-1);
    }
 
    bufmgr_freebuf(pAcpDataPool, (tpBUFFER) MAP_SH_ACP_SAR(pBuf));
    return(0);
 
}

int
aalsap_nmp_send(UINT16 portindex, UINT32 datalen, void *pData)
{
/*
**
** aalsap_nmp_send: Send data packet to NMP.
**            Note: Data is copied from DRAM to Frame Buffer in this function.
**
** Return code: OK										:  0
**				Resource unavailable					: -1
**				Bad port								: -2
**				Packet too long							: -3
**
*/

	tRX_NMP_REQ	RNreq;
	tpAcpToNmpIpcBuffer	pBuf = NULLP;
	tpQDESC     pAcpToNmpIpcPool = acp_Rxspram_info.rxspram_pAcpToNnpIpcPool;


    /* Check portindex (in Synergy ATM Card, has to be 0) */
    if (portindex >= MAX_PORT) {
        return(-2);
    }
 
    /* Check length */
    if (datalen > ACP_NMP_IPC_BUFSZ) {
        return(-3);
    }
 
    /* Get buffer in Frame Buffer Memory */
    if ((pBuf = (tpAcpToNmpIpcBuffer)qmgr_deq(pAcpToNmpIpcPool)) == NULLP) {
        return(-1);
    }

    RNreq.rx_nmp_pbuf = pBuf;
    RNreq.rx_nmp_len = datalen;

    RNreq.rx_nmp_vlan_cl =  acp_pRxSpram->MapTable[0].imap_vlan_cl;
    RNreq.rx_nmp_cid_vpc =  acp_pRxSpram->MapTable[0].imap_cid_vpc;

    pBuf = (tpAcpToNmpIpcBuffer) MAP_SH_SAR_ACP(pBuf);
 
    /* Copy data from paktype buffer in ACP DRAM to Frame Memory buffer */
	/* Leave 12 bytes for the Synergy Header at the beginning */
    bcopy(pData, (UINT8*) (((UINT8*)(&(pBuf->buf_data)))+12), datalen);
 
    if (rxsar_cmd(RXSAR_SEND_TO_NMP_C, (void *)&RNreq) == ACP_ERR) {
        return(-1);
    }
 
	/* 
	 * NOTE: Whether or not the command succeeded, DO NOT free pBuf. It is 
	 *		 freed by RXSAR after it has been transmitted on the Synergy Bus 
	 *		 by BIGA/SAGE.
	 */
	
	acp_pRxSpram->rxstat_gl.rxgl_packet_nmp++;

    return(0);
}

/*
*******************************************************************************
*
*   FUNCTION:
*           aalsap_set_tsar_throttle: Pace global output of TSAR
*
*   PARAMETERS:
*
*   RETURN:
*           1 means OK
*           0 means call failed
*
*   ALGORITHMS
*
*
*******************************************************************************
*/
int
aalsap_set_tsar_throttle(UINT32 txsar_throttle_parm)
{
    tTX_SET_THROTTLE_PARM_REQ TCmdParm;

	TCmdParm.tx_throttle_parm = txsar_throttle_parm;

    if (ACP_ERR == txsar_cmd(TXSAR_SET_THROTTLE_C, &TCmdParm)) {
        printf("\n\aalsap_set_tsar_throttle: txsar_cmd FAILED");
        return(0);
    }

	return (1);
}



void
atm_drv_isr (UINT32 Which)
{
	switch(Which) {

	case ACP_I2S_RX:
		sar_intr_status |= RXSAR_INTR;
#if 0
		buginf("\n\ratm_drv_isr: RX interrupt");
#endif 
		rxsar_isr();
		break;

	case ACP_I2S_TX:
		sar_intr_status |= TXSAR_INTR;
#if 0
		buginf("\n\ratm_drv_isr: TX interrupt");
#endif
		txsar_isr();
		break;
	}

	
}/*atm_drv_isr*/


UINT32
wait_for_interrupt ()
{
#if	1
		/* Enable (but mask off) interrupts for the Tx/Rx SARs. */
		/* Then poll on them until they are BOTH true.  Then    */
		/* clear the pending interrupts and return.             */

#ifdef DEBUG_PRINTF
	printf ("\nwait_for_interrupt: entered\n") ;
#endif /* DEBUG_PRINTF */

	MFP_PollForIntr ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), FALSE, FALSE, FALSE) ;

#ifdef DEBUG_PRINTF
	printf ("\nwait_for_interrupt: exited\n") ;
#endif /* DEBUG_PRINTF */
#else
	while (bt_PollCheck (atm_drv_isr) <= 0)
		{ }
#endif

	return 0;

}/*wait_for_interrupt*/


/*
*******************************************************************************
*
*   FUNCTION:
*		ADVR_CmdTimeOut
*
*   PARAMETERS:
*		pFunc	-- pointer to client's upcall routine
*				   (for now, just have IPC client)
*
*   RETURN:	NEVER
*
*   ALGORITHMS
*		Handles SAR command timeouts.
*
*******************************************************************************
*/

void
ADVR_CmdTimeOut (UINT8 WhichSar, tpSARCMD pCmd)
{
	UINT8	MyBuf [80] ;

#ifdef DEBUG_PRINTF
	sprintf (MyBuf, "\n\rADVR_CmdTimeOut:  %csar timeout, pCmd $%x cmd %u\n",
			((WhichSar == RXSAR_INTR) ? 'r' : 't'), pCmd, pCmd->cmd_code);
	buginf (MyBuf) ;
#endif /* DEBUG_PRINTF */

	MFP_HwtStdTimeout () ;		/* This should force a reset.  Maybe we'll */
								/* do some logging, etc. in future.   RKL. */

}	/* end ADVR_CmdTimeOut */


/*******************************************************************************
*
*   FUNCTION:
*		ADVR_RxSarPollSvc
*
*   PARAMETERS:
*		NONE
*
*   RETURN:
*		TRUE if there were any RxSAR inbound packets, else FALSE.
*
*   ALGORITHMS
*		ATM driver polling mode entry point to do/check RxSAR
*		service.  Always clear any pending interrupts FIRST, so
*		we can't drop/miss any.
*
*******************************************************************************/

UINT8
ADVR_RxSarPollSvc (u_int8 fOnce)
{
	MFP_ClrPendIntr (MFP_IMSK_RSAR, FALSE) ;

	return (ADVR_RxSar_Handler (fOnce)) ;

}	/* end ADVR_RxSarPollSvc */


/*******************************************************************************
*
*   FUNCTION:
*		ADVR_TxSarPollSvc
*
*   PARAMETERS:
*		NONE
*
*   RETURN:
*		TRUE if there were any TxSAR inbound packets, else FALSE.
*
*   ALGORITHMS
*		ATM driver polling mode entry point to do/check TxSAR
*		service.  Always clear any pending interrupts FIRST, so
*		we can't drop/miss any.
*
*******************************************************************************/

UINT8
ADVR_TxSarPollSvc (u_int8 fOnce)
{
	MFP_ClrPendIntr (MFP_IMSK_TSAR, FALSE) ;

	return (ADVR_TxSar_Handler (fOnce)) ;

}	/* end ADVR_TxSarPollSvc */


/*
*******************************************************************************
*
*   FUNCTION:
*		ADVR_RxSarPollEvWait
*		ADVR_TxSarPollEvWait
*
*   PARAMETERS:
*           void * pFunc	-- unused, for compatibility with IOS "edisms".
*           tpSARCMD pCmd
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*		Do SAR service requests through polling interfaces to low-
*		level handlers (ie. ISRs).  Repeat until some outstanding
*		SAR transaction is acknowledged to the ACP by an interrupt.
*		MFP interrupt line polling routine punches HW watchdog for us.
*
*******************************************************************************
*/
void
ADVR_RxSarPollEvWait (void * pFunc, tpSARCMD pCmd)
{
	UINT32	SR ;


	SR = splx (7) ;			/* RKL */

	ADVR_RxSarPollSvc (FALSE) ;

	while (pCmd->cmd_wakeup == 0)
	{
#ifdef	COMMENTED_OUT		/* This should work, but who knows ? */
		MFP_PollForIntr (MFP_IMSK_RSAR, FALSE, FALSE, TRUE) ;
#endif
		ADVR_RxSarPollSvc (FALSE) ;
	}

	(void) splx (SR) ;			/* RKL */

}	/* ADVR_RxSarPollEvWait */

void
ADVR_TxSarPollEvWait (void * pFunc, tpSARCMD pCmd)
{
	UINT32	SR ;


	SR = splx (7) ;			/* RKL */

	ADVR_TxSarPollSvc (FALSE) ;

	while (pCmd->cmd_wakeup == 0)
	{
#ifdef	COMMENTED_OUT		/* This should work, but who knows ? */
		MFP_PollForIntr (MFP_IMSK_TSAR, FALSE, FALSE, TRUE) ;
#endif
		ADVR_TxSarPollSvc (FALSE) ;
	}

	(void) splx (SR) ;			/* RKL */

}	/* ADVR_TxSarPollEvWait */


/*
*******************************************************************************
*
*   FUNCTION:
*		ADVR_DismissOnRxSar
*		ADVR_DismissOnTxSar
*
*   PARAMETERS:
*           void * pFunc	-- unused, for compatibility with IOS "edisms".
*           tpSARCMD pCmd
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*		Filter out requests to block until a SAR is done with a transaction.
*		Blocking can be either tight-loop/polling, or dismissing the current
*		thread until an interrupt/up-call wakes it back up.
*
*		A global flag in the driver allows us to use polling even when
*		the driver support library used is targeted for interrupt/kernel
*		mode instead of polling/boot mode.
*
*		Note that dismissing through the polling/boot-specific support
*		library will end up calling back in here to polling-mode wait
*		routines above.
*
*		If we are in forced polling mode, unmask at the MFP to allow
*		the SAR ISRs to be hit from interrupt-level.  This forced poll
*		is meant to be an IOS edisms replacement, not an interrupt
*		handling replacement.
*
*******************************************************************************
*/

void
ADVR_DismissOnRxSar (void * pFunc, tpSARCMD pCmd)
{
	if (fADVR_ForceEvPoll)
	{
		ADVR_RxSarPollEvWait (pFunc, pCmd) ;
		MFP_UnmaskIntrW (MFP_IMSK_RSAR, FALSE, FALSE) ;
	}
	else	/* Use polling or interrupt support library. */
	{
		ADVR_DismissOnRxSar2 (pFunc, pCmd) ;
	}

}	/* ADVR_DismissOnRxSar */

void
ADVR_DismissOnTxSar (void * pFunc, tpSARCMD pCmd)
{
	if (fADVR_ForceEvPoll)
	{
		ADVR_TxSarPollEvWait (pFunc, pCmd) ;
		MFP_UnmaskIntrW (MFP_IMSK_TSAR, FALSE, FALSE) ;
	}
	else	/* Use polling or interrupt support library. */
	{
		ADVR_DismissOnTxSar2 (pFunc, pCmd) ;
	}

}	/* ADVR_DismissOnTxSar */


/*******************************************************************************
*
*   FUNCTION:
*		ADVR_ChgEvPollFlag
*
*   PARAMETERS:
*		fNew	-- new value of driver "force event polling" flag.
*
*   RETURN:
*		Previous value of driver "force event polling" flag.
*
*   ALGORITHMS
*		Driver has a flag to "force event polling".  This is mainly used
*		at start-up when the driver support library uses interrupts rather
*		than polling.  A typical example is when start-up code needs to
*		do a SAR transaction (ie. cam setup) but O/S interrupt handling is
*		not yet enabled.
*
*		This routine is not really useful if the driver support library
*		already uses polling instead of interrupts.
*
*******************************************************************************/

UINT8
ADVR_ChgEvPollFlag (UINT8 fNew)
{
	UINT8	fOld ;


	fOld = fADVR_ForceEvPoll ;
	fADVR_ForceEvPoll = fNew ;

	return (fOld) ;

}	/* ADVR_ChgEvPollFlag */

