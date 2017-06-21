/* $id: if_aip.c,v 3.12.8.14 1996/08/28 12:44:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_aip.c,v $
 *------------------------------------------------------------------
 * if_aip.c: Holds the actual AIP driver code
 *
 * November, 1993, Rick Li Fo Sjoe
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_aip.c,v $
 * Revision 3.12.8.14  1996/08/28  12:44:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.8.13  1996/08/13  02:17:30  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.12.8.12  1996/08/03  23:24:05  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.12.8.11  1996/07/03  00:01:34  rzagst
 * CSCdi55228:  spurious messages when PLIM is oversubscribed
 * Branch: California_branch
 *
 * Revision 3.12.8.10  1996/06/29  22:04:30  rzagst
 * CSCdi59683:  OAM cells are not transmitted after ATM int was shut/noshut
 * Branch: California_branch
 *
 * Revision 3.12.8.9  1996/05/31  19:00:39  rzagst
 * CSCdi59199:  VCs with peak = average not = default fail to be created
 * Branch: California_branch
 *
 * Revision 3.12.8.8  1996/05/31  15:30:47  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.12.8.7  1996/05/28  20:36:41  shaker
 * CSCdi58549:  Notify users of non EOIR capable hardware
 * Branch: California_branch
 * Let users know why we're resetting the interface.
 *
 * Revision 3.12.8.6  1996/05/13  18:41:06  rzagst
 * CSCdi57247:  Burst size handled improperly between AIP and 4500
 * Branch: California_branch
 *
 * Revision 3.12.8.5  1996/05/09  14:28:42  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.12.8.4  1996/05/05  17:08:35  rzagst
 * CSCdi54165:  Cant create pvcs during subinterface shutdown
 * Branch: California_branch
 *
 * Revision 3.12.8.3  1996/04/30  23:15:53  sdurham
 * CSCdi46300:  Interface missing from MIB
 * Branch: California_branch
 *          add atm and aal5 layers to the ifmib
 *
 * Revision 3.12.8.2.2.1  1996/04/27  07:02:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.12.8.2  1996/04/25  23:12:01  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.12.8.1.8.1  1996/04/08  01:46:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.12.8.1  1996/03/18  19:41:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.12.10.2  1996/03/25  02:19:28  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.12.10.1  1996/03/22  09:36:22  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.10.4.2  1996/03/08  22:31:46  cakyol
 * pull in awu's CBUS-OUTHUNG bug fix manually.  This happens
 * when a rate q is configured on the AIP.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.10.4.1  1996/02/27  20:41:57  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.10  1996/01/26  03:10:26  awu
 * CSCdi43779:  Declaring or removing a PVC can cause loss of all SVCs.
 *  -change the way of setting up a rateq so that resetting the interf card
 *   is being bypassed.
 *
 * Revision 3.9.14.1  1996/01/24  21:46:06  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.9.4.1  1996/02/15  19:12:56  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.9  1995/12/16  02:39:24  awu
 * CSCdi45544:  atm pvc-s remain inactive after interface down/up.
 *
 * Revision 3.8  1995/12/11  22:21:12  awu
 * CSCdi31527:  number of current VC in show atm vc differs from show int
 * atm.
 *
 * Revision 3.7  1995/11/27  01:49:43  weismann
 * CSCdi34084:  peak/ave/burst is not always displayed properly in show
 * atm vc. Fix cellq corner case for minimum. Fix debug, show outputs.
 * Use flags for defaults.
 *
 * Revision 3.6  1995/11/23  19:57:46  bbenson
 * CSCdi42518:  vc creation fails because of wrong max_vpibits in ILMI
 * Fixes bits per VPI/VCI reported by the ILMI.
 *
 * Revision 3.5  1995/11/23  10:39:32  weismann
 * CSCdi34084:  peak/ave/burst is not always displayed properly in show
 *              atm vc. Fixed atm debug,show output. Handle atm vc
 *              defaults with flags.
 *
 * Revision 3.4  1995/11/22  20:44:47  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.3  1995/11/17  09:17:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:00:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/09/20  18:24:39  awu
 * CSCdi38728:  system shoud not block when requesting a bulk vc stats.
 *
 * Revision 2.7  1995/08/10  23:30:21  wfried
 * CSCdi37608:  oam f5 cells generated by 4500 are not recognized by
 * analyzer
 * Changed ATM code to pass VC pointer for OAM cells and other related
 * changes
 *
 * Revision 2.6  1995/07/06  01:08:39  mcieslak
 * The sequence of initialization was incorrect such that the
 * initialization for the AIP was trying to do an add to a registry
 * that had not been created yet by the cbus_mci initialization.
 * Ensure that cbus_mci is initialized prior to aip_init.
 * CSCdi36654:  %SYS-3-REGNONE: Registry 7 service 26 doesnt exist -
 * add/delete/inv
 *
 * Revision 2.5  1995/07/05  18:38:06  awu
 * CSCdi35839:  cle int a x/0 cause TEARDOWN_VC error and PVC 0/5 dead.
 *
 * Revision 2.4  1995/07/01  05:58:05  awu
 * CSCdi36167:  ATM calls printf from interrupt level.
 *
 * Revision 2.3  1995/06/28  09:24:33  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/26  18:02:35  ronnie
 * CSCdi35427:  shutting down AIP in an LEC router triggers CPUHOG and VC
 * reject
 *
 * Revision 2.1  1995/06/07  20:40:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "../ui/debug.h"
#include "if_hes_common.h"
#include "address.h"
#include "logger.h"
#include "../if/network.h"
#include "../hes/dgbus.h"
#include "../hes/dbus.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/cbus_registry.h"
#include "../wan/smds.h"
#include "../if/atm_debug.h"
#include "../if/atm.h"
#include "../atm/parser_defs_atm.h"
#include "../hes/if_aip.h"
#include "if_cmd.h"
#include "msg_aip.c"			/* Not a typo, see logger.h */
#include "msg_cbus.c"			/* Not a typo, see logger.h */
#include "msg_oir.c"			/* Not a typo, see logger.h */
#include "../atm/msg_atm.c"		/* Not a typo, see logger.h */

#include "../atm/atm_oam.h"

/*
 * All the 'glue' routines to communicate info. to the AIP.
 */

/*****************************************************************************
 * Name: aip_writemem
 *
 * Description: write a block of data to AIP memory
 *   CAUTION: this function writes data 2 bytes at a time.  If you attempt to
 *            write an odd number of bytes an extra byte will be written (i.e.
 *            if you try to write 3 bytes 4 will actually be written)
 *
 * Assumptions:
 *   Interrupts are disabled by the caller.
 *   Controller is already selected.
 *   
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the AIP interface
 *   mem -- pointer into AIP memory at which to write
 *   data -- pointer to the data in local memory
 *   len -- number of bytes of data to write (see caution above)
 *
 * Return Value: 
 *   boolean: TRUE = block written successfully, FALSE = block not written
 *
 *****************************************************************************/
static boolean
aip_writemem (hwidbtype *hwidb, ulong mem, ushort *data, uint len)
{
    boolean rc;
    int byte_count;
    ushort status;
    /*
     * Select the address in the AIP to which to write
     */
    status = send_aip_addr_select_cmd(hwidb,mem);
    if (status != FCI_RSP_OKAY) {
	rc = FALSE;
    } else {
	rc = TRUE;
	/*
	 * Loop writing ushort values to the AIP (AIP autoincrements
	 * the address for us)
	 */
	for (byte_count = 0; byte_count < len; byte_count += sizeof(ushort)) {
	    status = send_aip_mem_write_cmd(hwidb,*data);
	    if (status != FCI_RSP_OKAY) {
		rc = FALSE;
		break;
	    } else {
		data++;
	    }
	}
    }
    return(rc);
}

/*****************************************************************************
 * Name: aip_readmem
 *
 * Description: read a block of data from AIP memory
 *   CAUTION: this function reads data 2 bytes at a time.  If you attempt to
 *            read an odd number of bytes an extra byte will be read (i.e.
 *            if you try to read 3 bytes 4 will actually be read)
 *
 * Assumptions:
 *   Interrupts are disabled by the caller.
 *   Controller is already selected.
 *   
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the AIP interface
 *   mem -- pointer into AIP memory at which to read
 *   data -- pointer to where to place the data in local memory
 *   len -- number of bytes of data to read (see caution above)
 *
 * Return Value: 
 *   boolean: TRUE = block read successfully, FALSE = block not read
 *
 *****************************************************************************/
static boolean
aip_readmem (hwidbtype *hwidb, unsigned mem, ushort *data, uint len)
{
    boolean rc;
    int byte_count;
    ushort status;

    /*
     * Select the address in the AIP from which to read
     */
    status = select_if(hwidb);		/* Select the AIP in question */
    if (status != CMD_RSP_OKAY) {
	return (FALSE);
    }
    status = send_aip_addr_select_cmd(hwidb,mem);
    if (status != FCI_RSP_OKAY) {
	rc = FALSE;
    } else {
	rc = TRUE;
	/*
	 * Loop read ushort values from the AIP (AIP autoincrements
	 * the address for us)
	 */
	for (byte_count = 0; byte_count < len; byte_count += sizeof(ushort)) {
	    status = send_aip_mem_read_cmd(hwidb,data);
	    if (status != FCI_RSP_OKAY) {
		rc = FALSE;
		break;
	    } else {
		data++;
	    }
	}
    }
    return(rc);
}

/*****************************************************************************
 * Name: aip_sendcmd
 *
 * Description: sends a command block to the AIP and causes it to be executed
 *   CAUTION: commands should be an even number of bytes long
 *  
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the AIP interface
 *   cmd -- pointer to the command in local memory
 *   len -- number of bytes of command (see caution above)
 *   waitforsts -- a boolean that indicates whether to 
 *     (FALSE) -- just send the command and return
 *     (TRUE)  -- send the command and wait for status to return
 *
 * Return Value: 
 *   status -- status returned by the command.
 *
 *****************************************************************************/
static uint
aip_sendcmd (hwidbtype *hwidb, ushort *cmd, uint len, boolean waitforsts)
{
    atm_db_t *aip = hwidb->atm_db;
    sys_timestamp Tm;
    leveltype level;
    ushort status;

    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);
    if (aip_writemem(hwidb, aip->mema_cmd, cmd, len) == FALSE) {
	aip->cur_status = AIP_STS_CMD_FAIL;
	reset_interrupt_level(level);
    } else {
	aip->cur_status = AIP_STS_BUSY;
        status = send_aip_cmdparse_cmd(hwidb);
	if (status != FCI_RSP_OKAY) {
	    aip->cur_status = AIP_STS_CMD_FAIL;
    	    reset_interrupt_level(level);
	} else {
	    reset_interrupt_level(level);
	    if (!waitforsts) {
		aip->cur_status = AIP_STS_OK;
	    } else {
                GET_TIMESTAMP(Tm);
		while (CLOCK_IN_INTERVAL(Tm, AIP_CMD_WAIT*ONESEC)) {
		    if (aip->cur_status != AIP_STS_BUSY) {
			break;
		    }
		}
		if (aip->cur_status == AIP_STS_BUSY) {
		    aip->cur_status = AIP_STS_TIMEOUT;
		}
	    }
	}
    }
    return(aip->cur_status);
}

/*
 * aip_eoir_capable_hw_version
 * Return true if this AIP hardware is EOIR capable
 */
static boolean aip_eoir_capable_hw_version (hwidbtype *hwidb)
{
    ushort hw_version = dbus_hw_version(pslot_to_dbus(hwidb->slot));

    if (hw_version == AIP_NO_EOIR_VERSION)
	return(FALSE);

    return(TRUE);
}

/*****************************************************************************
 * Name: aip_disable
 *
 * Description: Called from aip_reset() to shut the AIP down. Note that there
 *   is only one interface on the controller currently and that the entire
 *   controller is reset here.
 *  
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the AIP interface
 *
 * Return Value: 
 *   None.
 *
 *****************************************************************************/
static void
aip_disable (hwidbtype *hwidb)
{
    leveltype level;
    atm_db_t *aip = hwidb->atm_db;
    ushort status;
    idbtype *swidb = hwidb->firstsw;
    boolean eoir_reset;
    dgtype *dgbus;

    if (atm_event_debug)
	buginf("\naip_disable(%s): state=%d", hwidb->hw_namestring,
            hwidb->state);

    if (hwidb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), hwidb, hwidb->hw_namestring);
	return;
    }

    if ( (hwidb->oir_flags & EOIR_RESET) &&
	 aip_eoir_capable_hw_version(hwidb) )
	eoir_reset = TRUE;
    else
	eoir_reset = FALSE;

    hwidb->counters.resets++;
    level = raise_interrupt_level(NETS_DISABLE);
    if (!eoir_reset) {
	hwidb->atm_lineup = FALSE;
	aip->flags &= ~ATM_FLAG_UP;
	aip->config_status = ATM_CONFIG_CHANGED;
	/*
	 * If AS was enabled on this interface then we gotta flush out the
	 * caches to make sure that switching doesn't continue after the
	 * interface is shutdown.
	 */
	reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
    }

    /*
     * Select the interface in question.  From hereon we deal only with
     * AIP specific issues.
     */
    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(AIPRSET, CBUS), swidb->namestring, status,
	       "select");
	reset_interrupt_level(level);
	return;
    }

    if (!eoir_reset) {
	/*
	 * Disable the interface and give the firmware a chance to finish.
	 */
	status = send_if_disable_cmd_delay(hwidb, IF_DISABLE_TIME);
	if (status != FCI_RSP_OKAY) {
	    errmsg(&msgsym(AIPRSET, CBUS), swidb->namestring, status,
		   "disable");
	    reset_interrupt_level(level);
	    return;
	}

	/*
	 * Select, reset and then restart (load) the controller.
	 * Clears out the current configuration including all VCs.
	 */
	dgbus = pslot_to_dbus(hwidb->cbus_ctrlr_index);
	dbus_ctrlr_reset(dgbus);
	dbus_slot_enable(dgbus);
	slot_enabled_led_on(hwidb->cbus_ctrlr_index);
	DELAY(FCI_SOFT_RESET_TIME);
    }

    aip_restart_init(hwidb);

    if (!eoir_reset) {
	select_if(hwidb);
	send_aip_cmdblock_cmd(hwidb,&aip->mema_cmd);
    }

    reset_interrupt_level(level);

    if (!eoir_reset) {
	aip->flags &= ~ATM_FLAG_UP;
	aip->config_status = ATM_CONFIG_CHANGED;

	/*
	 * Flush hold queue -- ring down -- no Upstream or Downstream
	 * neighbors.
	 */
	holdq_reset(hwidb);
    }
}

/*
 * aip_send_config:	Sends the current config in the AIP struct. to
 *		the AIP.
 *	See the hes/if_aip.h file for a description of the RateQ register
 *	values.  
 * OUTPUT/RETURN:
 * INPUT:
 */
static boolean
aip_send_config (hwidbtype *hwidb)
{
    aip_cmd_cfg_t aipcfg;
    atm_db_t *aip = hwidb->atm_db;
    unsigned i;

    if (atm_event_debug) {
	buginf("\nconfig(%s)", hwidb->hw_namestring);
    }

    aip->config.mtu = aip->config.mode & ATM_CFG_MODE_AAL34 ?
        ATM_SMDS_ENCAPBYTES : ATM_SNAP_ENCAPBYTES;
    aip->config.mtu += hwidb->maxmtu + ETHER_ARPA_ENCAPBYTES;
    memset(&aipcfg, 0, sizeof(aip_cmd_cfg_t));
    for (i = 0; i < AIP_RATEQ_NO; i++) {
	unsigned scaler, preload, Peak_Rate=aip->rateq[i].rate;

	if (!Peak_Rate)
	    continue;
	/* 
	 * CAUTION:  The code below requires a bucket & roll of toilet
	 * paper.  It's fully ugly.  Proceed at your own risk.
	 *
	 * Essentially, this code calculates what the PRELOAD value should
	 * be, given a specific SCALER.  There are 4 SCALER values.  The
	 * PRELOAD range is 0 .. 255.  We loop through the calculation,
	 * using different SCALERs until we have a PRELOAD value that 
	 * is allowable.  Then the Rate-Queue register is configured for
	 * PEAK rate requested by the user.
	 * original formula from manual is:
	 * abs value(255 - [424/PeakRate(Mbps)*[1000/TCLK(ns)]*[1/Prescaler]])
	 * so 424/PeakRate(Mbps) = 424*1000/PeakRate(Kbps)
	 * 1000/50ns = 20
	 * giving:
	 * abs value(255 - (424 * 1000 * 20)/(PeakRate(Kbps) * prescaler))
	 * where prescalers are 4,16,64,256
	 *
	 * Magic nos.:
	 *	50	Nanoseconds in a clock cycle(20Mhz)
	 *              used in formula as 1000/50ns = 20
	 *	424	bits in a cell(53 * 8)
	 *	1000	Work in Kbps.  rateq[i].rate is in Kbps already.
	 *
	 * Highly recommened reading is the SARA/FRED spec.
	 */
	for (scaler=0; scaler < 4 ; scaler++) {
	    preload = 255 - (424*20*1000)/(Peak_Rate*(4<<(scaler*2)));
	    if ((424*20*1000) % (Peak_Rate*(4<<(scaler*2))))
		preload--;
	    if (preload < 256)
		break;
	}
	if (scaler < 4) {
	    aip->config.rateq[i] = AIP_RATEQ_ENABLE | 
		(scaler << 8) | preload;
	} else {
	    if (atm_errors_debug)
		buginf("\nATM(%s): Config. scaler error.  Rateq %d, rate %d", 
			hwidb->hw_namestring, i, Peak_Rate);
	}
    }
    bcopy(&aip->config, &aipcfg.aip_cfg, sizeof(atm_config));
    aipcfg.aip_cmd.cmd = AIP_CMD_CONFIG;

    if(aip_sendcmd(hwidb, (ushort *)&aipcfg, sizeof(aip_cmd_cfg_t), TRUE)) {
	return(FALSE);
    }
    return(TRUE);
}

/*****************************************************************************
 * Description: Accumulates cBus errors from the AIP
 *   Called by aip_get_cbus_errors
 *
 * Assumptions:
 *   1. interrupts are disabled
 *   2. interface has been selected
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
static void
aip_get_cbus_errors (hwidbtype *hwidb)
{
    long counts;

    if (hwidb->type == IDBTYPE_AIP) {
	counts = send_errors_cmd(hwidb, FCI_ERR_NOBUFFERS);
	hwidb->counters.input_resource += counts;
    
	counts = send_errors_cmd(hwidb, FCI_ERR_CRC);
	hwidb->counters.input_crc += counts;
	hwidb->counters.input_error += counts;
    
	counts = send_errors_cmd(hwidb, FCI_ERR_FRAME);
	hwidb->counters.input_frame += counts;
	hwidb->counters.input_error += counts;

	counts = send_errors_cmd(hwidb, FCI_ERR_AIP_TX_DROP);
	hwidb->atm_db->out_ts_drop += counts; 
    }
}

/*
 * aip_cstate:	Called whenever a CD transition was detected.
 */
static void
aip_cstate (hwidbtype *hwidb)
{
    if (atm_event_debug) {
	buginf("\naip_cstate(%s): state=%d", hwidb->hw_namestring,
		hwidb->state);
    }
    hwidb->atm_lineup = (hwidb->state == IDBS_UP) ? TRUE : FALSE;
    if (hwidb->state == IDBS_UP) {
	reg_invoke_hwif_comingup(hwidb);
    } else {
	reg_invoke_hwif_goingdown(hwidb);
    }

    /*
     * If the line is down, inform upper layers so that they can do
     * cleanup actions
     */
    /*
     * We really do not need to worry too much about so called upper layers.
     * SSCOP entity should/will timeout if it does not receive its keepalives
     * when the link loses signal long enough and should notify signalling
     * entity to perform necesaary cleanup. For the PVC's, we don't/shouldn't
     * have much to do either when only the "link" is down.
     */
    hwidb->atm_db->config_status = ATM_CONFIG_CHANGED;
    if(!hwidb->atm_lineup) {
        reg_invoke_media_hw_reset(hwidb);
    }
    
}

/*
 * aip_get_rateq: Called to fetch a particular rate-queue and it's speed.
 * OUTPUT/RETURN:
 * 	uint index value into the array of ratequeues available on this int.
 * INPUT:
 *	Speed, in Mbps, requested
 *
 */
static uint
aip_get_rateq (atm_db_t *aip, uint speed)
{
    uint ndx;

    for(ndx = 0; ndx < AIP_RATEQ_NO; ndx++) {
	if (aip->rateq[ndx].state != RATE_QUEUE_DISABLED &&
	  aip->rateq[ndx].rate == speed) {
	    return(ndx);
    	}
    }
    return(AIP_BAD_RATEQ);
}

/*
 * aip_set_rateq: Called to set a ratequeue to a particular speed.
 * OUTPUT/RETURN:
 *	rateq ndx.  -1 == UNSUCCESFUL
 * INPUT:
 *	Speed(Kbps)
 */
static uint
aip_set_rateq (atm_db_t *aip, uint speed)
{
    uint ndx, new_trate = 0;
    atm_setup_status_t status;
    rateq_param_t rq_param;
    
    for(ndx = 0; ndx < AIP_RATEQ_NO; ndx++) {

	if(aip->rateq[ndx].state == RATE_QUEUE_DISABLED) {
	    rq_param.rq_id = ndx;
	    rq_param.rq_rate = speed;
	    rq_param.rq_state = RATE_QUEUE_DYNAMIC;
	    atm_update_rateq(aip, &rq_param);
	    status = (*aip->funcs.config_rateq)(aip->hwidb, ndx);
	     /*
	      * Since "delayed" implies the config will be sent
	      * down to the driver following an interface card
	      * reset, we assume the driver will execute this
	      * config sucfessfully in that case.
	      */
	    if (status == ATM_SETUP_SUCCESSFUL ||
		status == ATM_SETUP_DELAYED) {
		new_trate = atm_rateq_ratesum(aip); /* in Kbps */
		if (new_trate > (aip->plimrate * ATM_MBPS2KBPS))
		    if (atm_errors_debug) {
			buginf("Interface %s: Total rateq allocation %uKbps "
				 "exceeded maximum plim rate of %uMbps.",
				 aip->hwidb->hw_namestring,new_trate,
				 aip->plimrate);
		    }
		return(ndx);
	    } else {			/* we failed to setup the rateq */
		rq_param.rq_id = ndx;
		rq_param.rq_rate = 0;
		rq_param.rq_state = RATE_QUEUE_DISABLED;
		atm_update_rateq(aip, &rq_param);
	    }
	}
	
    }
    return(AIP_BAD_RATEQ);		/* No available rate-queues */
}

/*
 * aip_send_rateq_config: called to request aip set up a rateq.
 * INPUT:
 *	hwidb: interface ptr
 *	rateq_no:	which rateq to be set up.
 * Output/RETURN:
 *	status of the execution of the reqest.
 */	
static atm_setup_status_t
aip_send_rateq_config (hwidbtype *hwidb, ushort rateq_no)
{
    boolean status;
    atm_db_t *aip = hwidb->atm_db;

    if (aip->config_status == ATM_CONFIG_CHANGED) {
        if (atm_event_debug) {
            buginf("\naip_send_rateq_config(%s): rateq %d setup delayed. AIP con
fig. in progress",
                hwidb->hw_namestring, rateq_no);
        }
        return(ATM_SETUP_DELAYED);
    }
    status = aip_send_config(hwidb);
    /*
     * although aip_send_config() only returns TRUE or FALSE,
     * we are doing more generic testing of 'if (status)', rather
     * than 'if (status == TRUE)'.
     */
    if (status)
	return (ATM_SETUP_SUCCESSFUL);
    else 
	return(ATM_SETUP_FAILED);
}


/* aip_verify_qos_params: Called to allow the platform dependent code
 * to verify the qos parameters
 */
uint
aip_verify_qos_params (hwidbtype *hwidb,unsigned peakrate,unsigned avgrate,
		      unsigned burstcells)
{
    atm_db_t *atm;
    uint result,i,rate;
    
    /* should never happen */
    if (hwidb == NULL)
	return(FALSE);
    
    /*again should not happen */
    atm = hwidb->atm_db;
    if (atm == NULL)
	return(FALSE);

    /* initialize to pass */
    result = ATM_TRAFFIC_SHAPING_VERIFIED;
    
    /* first check if there is an available rate queue
     * or one we can already use
     */
    if (peakrate == 0) {
	rate = atm->plimrate*ATM_MBPS2KBPS;
    } else {
	rate = peakrate;
    }
    
    for (i = 0;i< AIP_RATEQ_NO;i++) {
	if (atm->rateq[i].state == RATE_QUEUE_DISABLED ||
	    atm->rateq[i].rate == rate)
	    break;
    }

    if (i == AIP_RATEQ_NO)
	result |= ATM_OUT_OF_RATE_QUEUES;
    
    /* Next check the burst cell size
     * Note: a burstcell value of zero indicates to use default
     */
    if ((burstcells != 0) && ((burstcells < (AIP_MIN_CQ * AIP_MIN_BURST_SIZE))
			      || (burstcells > (AIP_MAX_CQ
						* AIP_MIN_BURST_SIZE)))) {
	if (atm_errors_debug) {
	    buginf ("Burst cell size value: %d is out of range: must be "
		    "between %d and %d",burstcells,
		    AIP_MIN_CQ *AIP_MIN_BURST_SIZE,
		    AIP_MAX_CQ * AIP_MIN_BURST_SIZE);
	}
	result |= ATM_BURST_ERROR;
    }

    /* now burst is checked so return if using peak,average defaults*/
    if (peakrate == 0 && avgrate == 0 &&
	(result == ATM_TRAFFIC_SHAPING_VERIFIED ||
	 result == ATM_OUT_OF_RATE_QUEUES))
	return(result);
    
    /* check peakrate for platform */
    if ((peakrate > (atm->plimrate) * ATM_MBPS2KBPS) ||
	(peakrate < AIP_MIN_PEAK_RATE)) {
	if (atm_errors_debug) {
	    buginf ("Peak rate value: %d is out of range: must be "
		    "between %d and %d",peakrate,
		    AIP_MIN_PEAK_RATE,
		    atm->plimrate * ATM_MBPS2KBPS);
	}
	    
	result |= ATM_PEAKRATE_ERROR;
    }

    /*
     * check average rate for platform
     * this duplicates the aip_setup_vc code which passes
     * questionable values to the aip if the user doesn't configure
     * the right values which could only come from reading the manual
     * also the units in the parser prevent you from using a lot of
     * the average rate values- since divide is done
     */
    if (avgrate != peakrate) {
	if ((((peakrate / avgrate) - 1) > AIP_MAX_TIQ) ||
	    (((peakrate / avgrate) - 1) < AIP_MIN_TIQ)) {
	    if (atm_errors_debug) {
		buginf ("Average rate value: %d is out of range: must be "
			"between %d and %d",avgrate,
			(((atm->plimrate * ATM_MBPS2KBPS) % 64) == 0) ?
			((atm->plimrate * ATM_MBPS2KBPS)/64) :
			((atm->plimrate * ATM_MBPS2KBPS)/64) + 1,
			atm->plimrate * ATM_MBPS2KBPS);
	    }
	    result |= ATM_AVGRATE_ERROR;
	}
    }
    
    return(result);

}
	

    
/*
 * aip_setup_vc:	Called to initialize a VCD on the AIP and allow
 *		communication over the specified VCI/VPI to a remote host.
 * NOTE: 	(1) CRC32 calculations are disabled.
 *		(2) Congestion recovers after every cell
 * OUTPUT/RETURN:
 *	None.
 * INPUT:
 *	hwidbtype ptr. to the AIP we create the VC on.
 *	vcinfo ptr. of the VC we wish to create.
 */
static boolean
aip_setup_vc (hwidbtype *hwidb, vc_info_t *vcinfo)
{
    aip_cmd_setup_vc_t aipvc;
    atm_db_t *aip = hwidb->atm_db;
    unsigned rateq = ATM_VC_RATEQ0;
    int cellq, tiq = 0;
    ushort burst;
    

    if (atm_event_debug)
	buginf("\naip_setup_vc(%s): vc:%d vpi:%d vci:%d", 
		hwidb->hw_namestring, vcinfo->vc, vcinfo->vpi, vcinfo->vci);

    if (!(aip->flags & ATM_FLAG_UP)) {
	if (atm_event_debug) {
	    buginf("\naip_setup_vc(%s): vc%d creation delayed.  AIP DISABLED",
		hwidb->hw_namestring, vcinfo->vc);
	}
	return(TRUE);
    }
    if (vcinfo->swidb->subif_state != SUBIF_STATE_UP) {
	if(atm_event_debug || atm_errors_debug) {
	   buginf("\naip_setup_vc(%s): vc%d creation delayed: AIP sub-interface shutdown",
	     vcinfo->swidb->namestring,vcinfo->vc);
	}
      return(TRUE);
    }

    if ((aip->config_status != ATM_CONFIG_OK) &&
	(aip->configs_pending != 0) &&  
	(aip->config_status != ATM_VCS_UP) &&
	(aip->config_status != ATM_VCS_NOT_UP)) {
	if (atm_event_debug) {
	    buginf("\naip_setup_vc(%s): vc%d creation delayed.  AIP config. in progress",
		hwidb->hw_namestring, vcinfo->vc);
	}
	return(TRUE);
    }

    memset(&aipvc, 0, sizeof(aip_cmd_setup_vc_t)); /* Start with a clean slate */

    aipvc.vcd = vcinfo->vc;
    aipvc.enctype = vcinfo->etype;
    aipvc.gfc_vpi = vcinfo->vpi << 4;  /* GFC = 0, VPI=VPI (15..12)VCI = VCI */
    aipvc.gfc_vpi |= (vcinfo->vci & 0xF000) >> 12;
    aipvc.vci_pti = (vcinfo->vci & 0x0FFF) << 4;
    if (ISVCAAL34(vcinfo)) {
    	aipvc.vcmode = AIP_CCMODE_SWCONG | AIP_SWCONG_NORMAL;
    } else {
    	aipvc.vcmode = AIP_VCM_CRC32 | AIP_CCMODE_SWCONG | AIP_SWCONG_NORMAL;
    }
    aipvc.mid_low = vcinfo->mid_start;
    aipvc.mid_high = vcinfo->mid_end;
    aipvc.betag = 0;		/* Start BeTAG @ ZERO ALWAYS! */
    /*
     * If Peak & Avg. rates aren't specified, then just put this guy into
     * the fastest RateQ by setting flag for default (for hot swap actions).
     */
    if (vcinfo->peakrate == 0 && vcinfo->avgrate == 0)
        vcinfo->flags |= VC_FLAG_DEFAULT_RATE;

    if (vcinfo->flags & VC_FLAG_DEFAULT_RATE) {
        if ((rateq = aip_get_rateq(aip, aip->plimrate*1000)) == 
            AIP_BAD_RATEQ) {
            if ((rateq = aip_set_rateq(aip, aip->plimrate*1000)) == 
                AIP_BAD_RATEQ) {
                errmsg(&msgsym(AIPFAILSETUPVC,AIP), hwidb->hw_namestring,
                      vcinfo->vc, "Cannot be assigned to a RateQ");
                return(FALSE);
	    }
	}
        vcinfo->peakrate=aip->plimrate*1000;
        vcinfo->avgrate=aip->plimrate*1000;
    } else {
    	/*
    	 * Let's do a Peak rate first... 
    	 * Here we just find a RateQ that comes close to the Peak that we need.
     	 */
	if ((rateq = aip_get_rateq(aip, vcinfo->peakrate)) == AIP_BAD_RATEQ) {
	    if ((rateq = aip_set_rateq(aip, vcinfo->peakrate)) == 
                AIP_BAD_RATEQ) {
               errmsg(&msgsym(AIPFAILSETUPVC,AIP), hwidb->hw_namestring,
                      vcinfo->vc, "Cannot assign the vc to a RateQ");
               if (atm_errors_debug)
                   buginf("\n%%No rate-queues to assign VCD %d, Peak %dKbps",
                          vcinfo->vc, vcinfo->peakrate);
		return(FALSE);
	    }
	}
    	/* 
    	 * Ok... full blown.  User wants Peak & Avg. rate metering.  We have
    	 * to calculate both TIQ & CQ values for this VC.
    	 */
	if (vcinfo->avgrate > 0 && vcinfo->avgrate != vcinfo->peakrate) {
	    /*
	     * config. the TIQ to provide the VC 'Average Rate'.
	     */
	   
            tiq = (aip->rateq[rateq].rate / vcinfo->avgrate) - 1;
            if (tiq < AIP_MIN_TIQ || tiq > AIP_MAX_TIQ) {
                errmsg(&msgsym(AIPFAILSETUPVCAVG, AIP),
                       hwidb->hw_namestring, vcinfo->vc, "Average Rate out of ra
nge",vcinfo->peakrate/(AIP_MAX_TIQ+1),vcinfo->peakrate/(AIP_MIN_TIQ+1),
vcinfo->peakrate);
                if (atm_errors_debug)
                    buginf("\naip_setup_vc(%s): Avg rate err. VC %d peak %d
avg. %d rateq rate %d Average Rate expected range [%d .. %d]",
                           hwidb->hw_namestring, vcinfo->vc, vcinfo->peakrate,
                           vcinfo->avgrate, aip->config.rateq[rateq],
                           vcinfo->peakrate/(AIP_MAX_TIQ+1),
                           vcinfo->peakrate/(AIP_MIN_TIQ+1));
                return(FALSE);
            }
        }
    }
   /*
    * Config. the CellQ to provide the VC 'Burst Rate'.
    */
    /* on aip burst is in groups of 32 cell bursts
    * note: burstcells value is already rounded up to nearest
     * multiple of 32 cells
     */
    /* burst rate can't be zero unless not entered - use default
     * or default flag is already set in reset case
     */
    
    if (((vcinfo->flags & VC_FLAG_DEFAULT_BURST) != 0) ||
	(vcinfo->burstcells == 0)) {
	uint default_mtu;

	/* get interface mtu size to calculate burst size to burst mtu*/
	if (vcinfo->swidb != NULL) {
	    default_mtu = vcinfo->swidb->sub_mtu;
	} else {
	    default_mtu = hwidb->maxmtu;
	}
	default_mtu += (ETHER_ARPA_ENCAPBYTES +	ATM_SNAP_ENCAPBYTES);

	/* default is enough cells to burst mtu */
	burst = default_mtu/ATM_PAYLOAD_BYTES_IN_A_CELL;

	/* add one for integer truncation to make default > mtu*/
	if ((default_mtu % ATM_PAYLOAD_BYTES_IN_A_CELL) != 0)
	    burst++;

	/* round up to resolution of platform burst parameter */
	if ((burst % AIP_MIN_BURST_SIZE) != 0) {
	    burst += AIP_MIN_BURST_SIZE;
	}
	/* value is stored in cells so compute 32 multiple of cells */
	vcinfo->burstcells = (burst / AIP_MIN_BURST_SIZE) * AIP_MIN_BURST_SIZE;
    }

    cellq = vcinfo->burstcells/AIP_MIN_BURST_SIZE;
    
    if (cellq < AIP_MIN_CQ || cellq > AIP_MAX_CQ){
        errmsg(&msgsym(AIPFAILSETUPVC, AIP), hwidb->hw_namestring,
        vcinfo->vc, "BurstCells out of range");
        if (atm_errors_debug)
            buginf("\naip_setup_vc(%s): CQ err. VC %d CQ=%d, MTU=%d",
                  hwidb->hw_namestring, vcinfo->vc,
                  cellq, aip->config.mtu);
            return(FALSE);
    }

    aipvc.cellquota = cellq;		/* Set the CQ value */
    aipvc.vcmode |= tiq;		/* Slam in TIQ value */
    aipvc.vcmode |= (rateq << ATM_VC_RATEQ_SHIFT); /* Select RateQ */

    aipvc.aip_cmd.cmd = AIP_CMD_SETUP_VC;
    /*
     * Send this command packet down to the AIP for VC setup.
     * return FALSE if the aip isn't up and running on the next
     * aip_periodic PVCs will be re-tried and if aip is then
     * up will be created
     */
    if ((aip->config_status != ATM_CONFIG_OK) &&
	(aip->config_status != ATM_VCS_UP) &&
	(aip->config_status != ATM_VCS_NOT_UP)) {
	return(FALSE);
    }
    
    if (aip_sendcmd(hwidb, (ushort *)&aipvc, sizeof(aip_cmd_setup_vc_t),
		    TRUE)) {
	errmsg(&msgsym(AIPREJCMD, AIP), hwidb->hw_namestring, "Setup VC",
	       aip->cur_status);
	return(FALSE);
    }

    vcinfo->vcmode = aipvc.vcmode;		/* Save it for display */
    /*
     * Successful... Mark this VC as ACTIVE now
     */
    vcinfo->flags |= VC_FLAG_ACTIVE;
    aip->rateq[rateq].num_vc++;		/* Put one in this bucket */
    GET_TIMESTAMP(vcinfo->last_updown_time); /* make the uptime */
    return(TRUE);
}

/*
 * aip_teardown_vc:	Calls AIP to disable/shutdown a specific ATM
 *		connection, free up the VCI/VPI/VCDs for another connection.
 * OUTPUT/RETURN:
 * INPUT:
 *	hwidbtype ptr. to the AIP we teardown the VC.
 *	vcinfo ptr. of the VC we wish to teardown.
 */
static boolean
aip_teardown_vc (hwidbtype *hwidb, vc_info_t *vcinfo)
{
    aip_cmd_setup_vc_t aipvc;
    atm_db_t *aip = hwidb->atm_db;
    ushort rateq;

    if (atm_event_debug)
	buginf("\naip_teardown_vc(%s): vc:%d vpi:%d vci:%d", 
		hwidb->hw_namestring, vcinfo->vc, vcinfo->vpi, vcinfo->vci);

    /* we request AIP to teardown the VCC only if it is not being disabled */
    if (!(aip->flags & ATM_FLAG_UP)) {
	if (atm_event_debug) {
	    buginf("\naip_teardown_vc(%s): vc%d deletion delayed. AIP DISABLED",
               hwidb->hw_namestring, vcinfo->vc);
	}
    } else {
	aipvc.aip_cmd.cmd = AIP_CMD_TEARDOWN_VC;
	aipvc.vcd = vcinfo->vc;
	aipvc.enctype = vcinfo->etype;
	aipvc.gfc_vpi = vcinfo->vpi << 4;  /* GFC = 0, VPI=VPI
					      (15..12)VCI = VCI */

	aipvc.gfc_vpi |= (vcinfo->vci & 0xF000) >> 12;
	aipvc.vci_pti = (vcinfo->vci & 0x0FFF) << 4;

	if (aip_sendcmd(hwidb, (ushort *)&aipvc, sizeof(aip_cmd_setup_vc_t),
			TRUE)) {

	    errmsg(&msgsym(AIPREJCMD, AIP), hwidb->hw_namestring,
		   "Teardown VC", aip->cur_status);
	    if (atm_errors_debug) {

		buginf("\naip_teardown_vc(%s): Return value %d",
		       hwidb->hw_namestring, aip->cur_status);
	    }
	    return(FALSE);
	}
    }
    /*
     * Successful... Mark this VC as INACTIVE now
     */
    vcinfo->flags &= ~VC_FLAG_ACTIVE;
    rateq = (vcinfo->vcmode >> ATM_VC_RATEQ_SHIFT) & (AIP_RATEQ_NO - 1);
    aip->rateq[rateq].num_vc--;
    if (aip->rateq[rateq].num_vc == 0 && 
      aip->rateq[rateq].state == RATE_QUEUE_DYNAMIC) {
	aip->rateq[rateq].state = RATE_QUEUE_DISABLED;
    }
    vcinfo->vcmode = 0;
    GET_TIMESTAMP(vcinfo->last_updown_time);
    return(TRUE);
}

/*
 * aip_enable(): Called from aip_reset to enable the AIP
 */
static void
aip_enable (hwidbtype *hwidb)
{
    leveltype level;
    ushort ndx;
    ushort status;
    atm_db_t *aip = hwidb->atm_db;
    vc_info_t *vc;
    boolean eoir_reset;

    if ( (hwidb->oir_flags & EOIR_RESET) &&
	 aip_eoir_capable_hw_version(hwidb) )
	eoir_reset = TRUE;
    else
	eoir_reset = FALSE;

    if (atm_event_debug)
	buginf("\naip_enable(%s)", hwidb->hw_namestring);

    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    if (hwidb->state == IDBS_ADMINDOWN || !aip) {
	if (eoir_reset)
	    send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
	reset_interrupt_level(level);
	return;
    }

    if (!eoir_reset) {
	switch (aip->plimtype) {
	case ATM_PLIM_DS3:
	    send_aip_ds3scramble_cmd(hwidb, (aip->flags & ATM_FLAG_DS3SCRAM));
	    send_aip_framing_cmd(hwidb,aip->ds3_framing);
	    send_clocktype_cmd(hwidb,aip->txclock);
	case ATM_PLIM_E3:
	    send_aip_ds3scramble_cmd(hwidb, (aip->flags & ATM_FLAG_DS3SCRAM));
	    send_aip_framing_cmd(hwidb,aip->e3_framing);
	    send_clocktype_cmd(hwidb,aip->txclock);
	    break;
	case ATM_PLIM_SONET:
	    send_clocktype_cmd(hwidb,aip->txclock);
	    send_aip_sonet_cmd(hwidb,aip->sonetmode);
	    break;
	default:
	    break;
	}
    }

    /*
     * Enable reception.  It is possible for this command to fail if there
     * insufficient receive buffers allocated.
     */
    cbus_mci_txlimit(hwidb);

    send_aip_setver_cmd(hwidb);

    /* 
     * Issue AIP 'Enable' command to accept config. and go active
     */
    if (!eoir_reset) {
	status = send_if_enable_cmd(hwidb);
    } else {
	status = send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
    }
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(AIPRSETU, CBUS), hwidb->mci_index, status, "enable");
	reset_interrupt_level(level);
	return;
    }

    /*
     * KLUDGE:  For now we assume that the AIP always comes up.  Later,
     * 	we must go through the cbus_mci_carrier routine to determine carrier
     *  state before allowing the interface to come up.
     *  AIP must generate a DCD interrupt to the RP to signal that Carrier
     *  is available so we switch hardware state.
     */
    /*
     * Set loopback state if it should be looped
     */
    if (hwidb->loopback) {
	send_status_cmd(hwidb,hwidb->loopback ? FCI_STATUS_LOOPLOCAL :
	    FCI_STATUS_LOOPREMOTE);
    }

    reset_interrupt_level(level);

    aip->flags |= ATM_FLAG_UP;

    /*
     * For each previous connection, issue AIP commands to enable
     * the PVC again.
     * NOTE: This only applies in PVCs.  VCs must be reconfigured
     * 	on the AIP when enabling the card.
     */
    if (!eoir_reset) {
	if (hwidb->state != IDBS_UP)
	    return;
	if (atm_event_debug)
	    buginf("\naip_enable(%s): restarting VCs: %d",
		   hwidb->hw_namestring,
		   aip->num_vc);
	for (ndx = 1; ndx < aip->config.max_intf_vcs; ndx++) {
	    vc = atm_getvc_all(aip, ndx);
	    if (vc == NULL)
		continue;
	    if ((vc->flags & VC_FLAG_PVC) && 
		(vc->swidb->subif_state == SUBIF_STATE_UP) &&
		(!ISVCACTIVE(vc))) {
		atm_activate_pvc(hwidb, vc);
	    }
	    /* Either no vc's come up or all will so we can toggle
	     * this config to ATM_VCS_UP if one comes up
	     *
	     */
	    if (ISVCACTIVE(vc)) {
		aip->config_status = ATM_VCS_UP;
	    }
	    if ((aip->config_status != ATM_VCS_UP) &&
		(aip->configs_pending == 0)) {
		aip->config_status = ATM_VCS_NOT_UP;
	    }
	}
	if (aip->config_status == ATM_VCS_UP) {
	    if (atm_event_debug) {
		buginf("\naip_enable(%s): AIP config. status"
		       " set to ATM_VCS_UP",
		       hwidb->hw_namestring);
	    }
	}
	if (aip->config_status == ATM_VCS_NOT_UP) {
	    if (atm_event_debug) {
		buginf("\naip_enable(%s): AIP config. status"
		       " set to ATM_VCS_NOT_UP",
		       hwidb->hw_namestring);
	    }
	}
	    
    }
    hwidb->atm_lineup = TRUE;
}

/*****************************************************************************
 * Name: aip_get_plim_info
 *
 * Description: Called from aip_reset() and the RSP's aip_ctrlr_analyze to
 *   determine the type of plim present and set the appropriate rate in the
 *   atm data block.
 *  
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the AIP interface
 *      (hwidb->atm_db is expected to be initialized.)
 *
 * Return Value: 
 *   None.
 *
 *****************************************************************************/
void
aip_get_plim_info (hwidbtype *hwidb)
{
    atm_db_t *aip = hwidb->atm_db;
    leveltype level;
    idbtype *swidb = hwidb->firstsw;

    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    aip->plimtype = ATM_PLIM_INVALID;
    send_applique_cmd(hwidb,0,&aip->plimtype);
    switch(aip->plimtype) {
    case ATM_PLIM_4B5B:
	aip->plimrate = PLIM_4B5B_RATE;
	break;
    case ATM_PLIM_SONET:
	aip->plimrate = PLIM_SONET_RATE;
	break;
    case ATM_PLIM_E3:
	aip->plimrate = PLIM_E3_RATE;
	break;
    case ATM_PLIM_DS3:
	aip->plimrate = PLIM_DS3_RATE;
	break;
    case ATM_PLIM_INVALID:
    default:
	aip->plimrate = 0;		/* No traffic allowed */
	errmsg(&msgsym(AIPINVPLIM, CBUS), hwidb->hw_namestring);
	break;
    }

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	switch(aip->plimtype) {
	case ATM_PLIM_4B5B:
	    set_default_interface_bandwidth(swidb,
					    VISIBLE_AIP_BANDWIDTH_4B5B);
	    set_default_interface_delay(swidb, AIP_4B5B_DELAY);
	    break;
	case ATM_PLIM_SONET:
	    set_default_interface_bandwidth(swidb,
					    VISIBLE_AIP_BANDWIDTH_SONET);
	    set_default_interface_delay(swidb, AIP_SONET_DELAY);
	    break;
	case ATM_PLIM_E3:
	    set_default_interface_bandwidth(swidb, VISIBLE_AIP_BANDWIDTH_E3);
	    set_default_interface_delay(swidb, AIP_E3_DELAY);
	    break;
	case ATM_PLIM_DS3:
	    set_default_interface_bandwidth(swidb, VISIBLE_AIP_BANDWIDTH_DS3);
	    set_default_interface_delay(swidb, AIP_DS3_DELAY);
	    break;
        default:
	    break;
	}
    }

    reset_interrupt_level(level);
}

/*
 * aip_reset: Called from the [NO] SHUTDOWN command and from CLEAR INTERFACE
 *	to reinitialize the AIP. RESET causes all VC to be closed down and lost
 * 	by the AIP. If !ADMINDOWN then we have to reconstruct the VCs
 *	after doing a RESET.
 *	This code first goes through a formal SHUTDOWN sequence to clear
 *	out the interface.  It then checks to see if the interface is to
 *	be left SHUTDOWN or brought up.  If it's to be brought up, then 
 *	the interface is re-enabled and all PVCs are reconfigured as is the
 *	AIP with the config. parameters.
 * OUTPUT/RETURN:
 *	None.
 * INPUT:
 *	idb->state is check to see if the interface is declared DOWN or UP
 */
static void
aip_reset (hwidbtype *hwidb)
{
    atm_db_t *aip = hwidb->atm_db;
    leveltype level;
    boolean eoir_reset;
    idbtype *swidb;

    if (hwidb->oir_flags & EOIR_RESET) {
	if (aip_eoir_capable_hw_version(hwidb))
	    eoir_reset = TRUE;
	else {
	    ushort hw_version;

	    eoir_reset = FALSE;
	    hw_version = dbus_hw_version(pslot_to_dbus(hwidb->slot));
	    errmsg(&msgsym(NOEOIR, OIR), hwidb->hw_namestring,
		   "hardware", (hw_version >> 8), (hw_version & 0xFF));
	}
    } else {
	eoir_reset = FALSE;
    }

    if (!eoir_reset) {
	/*
	 * Determine the plim type and rate and act appropriatly.
	 */
	aip_get_plim_info(hwidb);
	if (aip->plimrate == 0) {
	    /* No traffic allowed, force card down. */
	    hwidb->state = IDBS_DOWN;
	}
    }
    if (atm_event_debug) {
	buginf("\nRESET(%s): PLIM type is %d, Rate is %dMbps", 
		hwidb->hw_namestring, aip->plimtype, 
		aip->plimrate);
    }

    /*
     * Shut the AIP down.
     */
    aip_disable(hwidb);

    /*
     * Invoke callback functions to inform them of the reset.
     */
    if (!eoir_reset) {
	reg_invoke_media_hw_reset(hwidb);
    }

    /*
     * Disable interrupts and reselect the interface.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    /*
     * At this point, the AIP was reset & is sitting in the 'shutdown'
     * state.  We decide now whether to bring the card up again or to
     * leave it down.
     */
    if (hwidb->state == IDBS_ADMINDOWN) {
	if (eoir_reset) {
	    /* Re-active interface from its nap. */
	    send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
	}
	reset_interrupt_level(level);
	return;
    }
    
    /* 
     * Ok... We've decided to bring it back up.  We have to send down
     * the configuration commands to tell it how to config. memory, etc...
     * Send configuration commands to AIP 
     */
    if (!eoir_reset) {
	if (!aip->mema_cmd) {	
	    errmsg(&msgsym(AIPCFG, CBUS), hwidb->hw_namestring, aip->mema_cmd,
		   AIP_STS_CONFIG_FAIL);
	    reset_interrupt_level(level);
	    return;
	}
	
	aip_send_config(hwidb);
/*	aip->configs_pending++; */
	GET_TIMESTAMP(aip->config_time);
    } else {
	/*
	 * Don't wait around for periodic function to bring the interface
	 * back on line.
	 */
	TIMER_STOP(hwidb->xmittimer);
	aip->config_status = ATM_CONFIG_IN_PROGRESS;
	if (atm_event_debug) {
	    buginf("\naip_enable(%s): AIP config. status"
		   " set to ATM_CONFIG_IN_PROGRESS",hwidb->hw_namestring);
        }
	aip_enable(hwidb);
    }

    /*
     * add interesting addresses to bridge table if tbridging
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	reg_invoke_media_interesting_addresses(swidb);
    }

    reset_interrupt_level(level);
}

/*
 * aip_periodic:	Called from atm_periodic() to perform periodic
 *		functions...
 *	These include:
 *		Timed configuration changes to avoid flapping.
 *		...
 * OUTPUT/RETURN:
 * INPUT:
 */
static void
aip_periodic (hwidbtype *hwidb)
{
    atm_db_t *aip = hwidb->atm_db;

    /*
     * Don't perform this function if "aip_if_init" hasn't occurred on
     * the RSP.
     */
    if (platform_cbus_reset_in_progress(hwidb))
	return;
 
    if (hwidb->state != IDBS_ADMINDOWN) {
    	switch (aip->config_status) {
    	case ATM_CONFIG_CHANGED:
	    if (CLOCK_OUTSIDE_INTERVAL(aip->config_time, 
	      AIP_CONFIG_DELAY*ONESEC)) {
		/* we have to wait until all configs
		 * have been passed to aip before we
		 * can enable it so countdown must go to zero
		 */
		if (aip->configs_pending != 0) {
		    aip->configs_pending = 0;  
		} else {
		    aip_reset(hwidb);
		    aip->config_status = ATM_VCS_NOT_UP;
			if (atm_event_debug) {
			    buginf("\naip_periodic(%s): AIP config. status"
				   " set to ATM_VCS_NOT_UP",
				   hwidb->hw_namestring);
			}
		    aip_enable(hwidb); 
                   /* if all pvcs are up we can go to  */
		    if (aip->config_status == ATM_VCS_UP) 
			aip->config_status = ATM_CONFIG_OK;
			if (atm_event_debug) {
			    buginf("\naip_periodic(%s): AIP config. status"
				   " set to ATM_CONFIG_OK",
				   hwidb->hw_namestring);
			}
		}
	    }
	    break;
    	case ATM_CONFIG_ENABLE:
	    if (CLOCK_OUTSIDE_INTERVAL(aip->config_time, 
				       AIP_CONFIG_DELAY*ONESEC)) {
		/* we have to wait until all configs
		 * have been passed to aip before we
		 * can enable it so countdown must go to zero
		 *
		 */
		if (aip->configs_pending != 0) {
		    aip->configs_pending = 0; 
		} else {
		    aip->config_status = ATM_CONFIG_IN_PROGRESS;
		    if (atm_event_debug) {
			buginf("\naip_periodic(%s): AIP config. status"
			       " set to ATM_CONFIG_IN_PROGRESS",
			       hwidb->hw_namestring);
		    }
		    aip_enable(hwidb);
		    if (aip->config_status == ATM_VCS_UP) {
			aip->config_status = ATM_CONFIG_OK;
			if (atm_event_debug) {
			    buginf("\naip_periodic(%s): AIP config. status"
				   " set to ATM_CONFIG_OK",
				   hwidb->hw_namestring);
			}
		    }
		}
	    }
	    break;
	default:
	    if (aip->config_status != ATM_CONFIG_OK)
		aip_enable(hwidb);
	    if (aip->config_status == ATM_VCS_UP) {
		aip->config_status = ATM_CONFIG_OK;
		if (atm_event_debug) {
		    buginf("\naip_periodic(%s): AIP config. status"
			   " set to ATM_CONFIG_OK",
			   hwidb->hw_namestring);
		}
	    }
	    break;
	}
    }
}

/*
 * aip_love_note_common:
 *	Called to perform the common processing for an AIP interrupt.
 *      Presumably, the obvious AIP function, Carrier Detect, has already
 *	been taken care of. This routine handles the command completion
 *	love notes from the AIP.
 */
void
aip_love_note_common (hwidbtype *hwidb,
		      ushort asr)
{
    unsigned aipsts;
    atm_db_t *aip = hwidb->atm_db;

    if (aip->cur_status != AIP_STS_BUSY)
	return;		/* If not waiting for a completion, return */

    aipsts = (asr & AIP_COND_STS_MASK) >> 8; /* Get status */
    switch (aipsts) {
    case AIP_CMD_OK:
	aip->cur_status = AIP_STS_OK;
	break;
    case AIP_CMD_FAIL:
	aip->cur_status = AIP_STS_CMD_FAIL;
	break;
    case AIP_CONFIG_FAIL:
	aip->cur_status = AIP_STS_CONFIG_FAIL;
	break;
    case AIP_INVLD_PLIM:		/* AIP has an invalid PLIM */
	errmsg(&msgsym(AIPINVPLIM, CBUS), hwidb->hw_namestring);
	hwidb->state = IDBS_ADMINDOWN;
	aip_reset(hwidb);
	break;
    default:
   	if (atm_errors_debug)
	    buginf("\naip_love_note_common(%s): UNKNOWN asr=0x%x",
		   hwidb->hw_namestring, asr);
	break;
    }
}

/*
 * aip_encaps:	Called to do Interface specific encapsulation stuff.
 * 	for an ATM packet.
 * OUTPUT/RETURN:
 *	Packet is encapsulated with AIP specific encapsulation info.
 * INPUT:
 *	*aip -> ptr. to the ATM struct.
 *	*pak -> Ptr. to the packet to encaps.
 *	*vc -> Ptr. to the VC associated with this.
 *	dm == Descriptor mode value. 
 *		0 == Default to normal VC traffic.  Fabricate dm from vc.
 *		!0 == OAM/ILMI cell processing ... Use dm as specified.
 */
static unsigned
aip_encaps (atm_db_t *aip, paktype *pak, vc_info_t *vc, ushort dm)
{
    atm_encap_t *aipenc;
    vcd_t vcnum;
    ushort enctype;

    pak->datagramstart -= ATM_ENCAP_HDRBYTES;
    pak->datagramsize += ATM_ENCAP_HDRBYTES;
    aipenc = (atm_encap_t *)pak->datagramstart;
    if(!dm) {				/* Standard descriptor */
    	vcnum = vc->vc;		/* Slam in the VC no. for the AIP */
	enctype = AIP_DM_EOM_EN;
	if (ISVCAAL34(vc)) {
	    enctype |= AIP_DM_PKT_AAL34;
	} else {
	    enctype |= AIP_DM_PKT_AAL5;
	}
    } else {
	enctype = dm;
	dm &= ATM_DM_PKT_MSK;
	if ((dm == AIP_DM_PKT_OAM_DM) || (dm == AIP_DM_PKT_OAM_VC)) {
	    vcnum = 0;		/* Default to 0 for OAM stuff */
	} else {
	    vcnum = vc->vc;	/* Set it if requested */
	}
    }
    PUTSHORT(&aipenc->vcnum, vcnum);
    PUTSHORT(&aipenc->enctype, enctype);

    return(ATM_ENCAP_HDRBYTES);
}

/*
 * aip_raw_input: 	Called from the AIP_RECEIVE macro, indirectly from
 *		atm_pakalign() to process a RAW cell type.  This may be
 *		OAM F4, F5, or an AIP -> RP communication message 
 * OUTPUT/RETURN:
 * INPUT:
 */
static void
aip_raw_input (hwidbtype *hwidb, paktype *pak)
{
    unsigned *u_oam;

    u_oam = (unsigned *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
    /*
     * Check for AIP->RP comm. message.  If not, proceed to F4/F5 processing
     */
    if (*u_oam == AIP_RP_COMM_MSG) {		/* AIP->RP comm. message */
	datagram_done(pak);
	return;
    } 

    process_enqueue_pak(atm_oam_packetQ, pak);
}

/*
 * aip_get_stats:	Called to fetch current statistics from the AIP
 * OUTPUT/RETURN:
 *	If a specific VC was specified, then the VC info structure is 
 *	updated to reflect the total # of packets that were AS'ed.
 * 	If a NULL VC was specified, then we send the request to the AIP
 *	but do not wait for a response.  We've requested all stats be
 *	returned, and the AIP will return them in special buffers.
 *	aip_raw_input() will deal with these packets as necessary.
 * INPUT:
 *	*hwidb -> Ptr. to hwidb in question
 *	*vc -> Ptr. fo specific VC, or NULL for all VCs
 */
static boolean
aip_get_stats(hwidbtype *hwidb, vc_info_t *vc)
{
    aip_getvc_stats_t aipcmd;
    atm_db_t *aip = hwidb->atm_db;
    leveltype level;
    uint cmdlen = sizeof(aip_getvc_stats_t);
    boolean result;

    memset(&aipcmd, 0, sizeof(aip_getvc_stats_t));
    aipcmd.aip_cmd.cmd = AIP_CMD_GET_STATS;
    if (vc) {	
    /*
     * We're after a specific VC. Wait for OK, then collect stats.
     */
	aipcmd.vcd = vc->vc;
	aipcmd.vpi = vc->vpi;
	aipcmd.vci = vc->vci;
	result = aip_sendcmd(hwidb, (ushort *)&aipcmd, cmdlen, TRUE);
    } else
	/*
	 * request for bulk status. memset above already sets vcd to 0
	 */	 
	result = aip_sendcmd(hwidb, (ushort *)&aipcmd, cmdlen, FALSE);

    if (result) 
	return(FALSE);		/* Badness.... No stats available */

    if (vc) {
    /* 
     * Ok. Collect stats
     */
    	level = raise_interrupt_level(NETS_DISABLE);
    	if(!aip_readmem(hwidb, aip->mema_cmd, (ushort *)&aipcmd, cmdlen)) {
	    aip->cur_status = AIP_STS_CMD_FAIL;
	    reset_interrupt_level(level);
	    return(FALSE);
    	}
	reset_interrupt_level(level);
	vc->in_pkts = ((aipcmd.rx_pkt_hi << 16) | aipcmd.rx_pkt_lo);
	vc->out_pkts = ((aipcmd.tx_pkt_hi << 16) | aipcmd.tx_pkt_lo);
	vc->in_bytes += (aipcmd.rx_byte_hi << 16) | aipcmd.rx_byte_lo;
	vc->out_bytes += (aipcmd.tx_byte_hi << 16) | aipcmd.tx_byte_lo;
	if (!(ISVCAAL34(vc))) {
	    aip->aal5.ifOutOctets += (aipcmd.tx_byte_hi << 16) |
		aipcmd.tx_byte_lo;
	    aip->aal5.ifInOctets += (aipcmd.rx_byte_hi << 16) |
		aipcmd.rx_byte_lo;
	}
	if (atm_event_debug) {
	    buginf("\nAIP Stats(%s): VCD# %d: RX PKT: %d, TX PKT: %d",
		hwidb->hw_namestring, vc->vc,
		   ((aipcmd.rx_pkt_hi << 16) | aipcmd.rx_pkt_lo),
		   ((aipcmd.tx_pkt_hi << 16) | aipcmd.tx_pkt_lo));
	}
    }
    return(TRUE);
}

/*
 * aip_setup:		Called to set the default config. for a newly
 *		discoverd AIP card.
 * ASSUMPTIONS:
 *	atm_db_t struct was pre-initialized to ZEROs!
 * OUTPUT/RETURN:
 *	none
 * INPUT:
 *	hwidbtype ptr.
 */
void aip_setup (hwidbtype *hwidb)
{
    atm_db_t *aip = hwidb->atm_db;
    idbtype *swidb = hwidb->firstsw;

    holdq_init(hwidb, AIP_HOLDQ);

    aip->flags |= ATM_FLAG_PVC;		/* initially we're doing PVC only */

    if (!aip->config.tx_buff && !aip->config.rx_buff) {
    	aip->config.tx_buff = ATM_TXBUFF_DEFAULT;
    	aip->config.rx_buff = ATM_RXBUFF_DEFAULT;
    	aip->config.mode = ATM_CFG_MODE_AAL5;
    	aip->config.exq = ATM_EXCQ_DEFAULT;
    	aip->config.raw_queue_size = ATM_RAWQ_DEFAULT;
    	aip->config.max_intf_vcs = ATM_MAX_VC_DEFAULT;
    	aip->config.rx_vc = aip->config.max_intf_vcs; /* Same as TX VCs */
    	aip->config.vp_filter = ATM_VP_FILTER_DEFAULT;
    	aip->config.mode = ATM_CFG_MODE_AAL5;
    	aip->config.mtu = hwidb->maxmtu + ETHER_ARPA_ENCAPBYTES + ATM_SMDS_HDRBYTES;
	aip->config.mid_per_vc = ATM_MID_DEFAULT;
    	aip->config.rt_vc_per_vp = aip->def_config.rt_vc_per_vp = ATM_VC_PER_VP_DEFAULT;

	/* max_vci_bits is dependent on the vc per vp */
	aip->def_config.max_vci_bits = aip->config.max_vci_bits
	    = atm_calc_vci_bits(ATM_VC_PER_VP_DEFAULT);

	/* max_vpi_bits is dependent on total vp/vc bits and max_vci_bits */
	aip->def_config.max_vpi_bits = aip->config.max_vpi_bits
	    = atm_calc_vpi_bits(AIP_TOTAL_VPIVCI_BITS,
				aip->config.max_vci_bits);

	aip->def_config.max_burst = AIP_MAX_CQ;

	aip->def_config.min_peak_rate = AIP_MIN_PEAK_RATE;

	aip->mtu_reject_call = FALSE;
	/* platform dependent value to use to display burst cells from
	   the configured value */
	aip->burstcells_factor = AIP_MIN_BURST_SIZE;

    }

    aip->funcs.reset = aip_reset;
    aip->funcs.love_note = (void *)aip_love_note;
    aip->funcs.rawinput = aip_raw_input;
    aip->funcs.setupvc = aip_setup_vc;
    aip->funcs.teardownvc = aip_teardown_vc;
    aip->funcs.config = aip_send_config;
    aip->funcs.config_rateq = aip_send_rateq_config;
    aip->funcs.encapsulate = aip_encaps;
    aip->funcs.periodic = aip_periodic;
    aip->funcs.getvc_stats = aip_get_stats;
    aip->funcs.sniff = aip_sniff;
    aip->funcs.qos_params_verify = aip_verify_qos_params;
    aip->ds3_framing =
	aip->ds3_def_framing =
	    AIP_DS3_FRAMING_DEFAULT;
    aip->e3_framing =
	aip->e3_def_framing =
	    AIP_E3_FRAMING_DEFAULT;
    aip->ratequeue_no = AIP_RATEQ_NO;
    aip->config_delay = AIP_CONFIG_DELAY;
    aip->out_ts_drop = 0;

    hwidb->lineaction = aip_cstate;
    hwidb->reset = aip_reset;

    /*
     * set MAXMTU & MAXDGRAM at the same time that the rx_offset field
     * is initialized by calling set_maxdgram().
     */
    hwidb->buffersize = aip->config.mtu;
    hwidb->max_pak_size_def = aip->config.mtu;
    if_maxdgram_adjust(hwidb, aip->config.mtu);
    set_default_interface_bandwidth(swidb, VISIBLE_AIP_BANDWIDTH);
    set_default_interface_delay(swidb, AIP_DELAY);

    aip->config_status = ATM_CONFIG_CHANGED; /* Force first config. to card */
    if (atm_event_debug) {
	buginf("\naip_setup(%s): AIP config. status"
	       " set to ATM_CONFIG_CHANGED",
	       hwidb->hw_namestring);
    }
    GET_TIMESTAMP(aip->config_time);		/* Set current time */
    aip->configs_pending++;
}

/*
 * aip_init:	Called to init. the registry states to support the AIP
 *	specific driver code.
 */
static void
aip_init (subsystype* subsys) 
{
    reg_add_media_hw_setup(aip_setup, "aip_setup");
    reg_add_cbus_get_ip_errors(FCI_TYPE_ATM, 
			       aip_get_cbus_errors, "aip_get_cbus_errors");
    aip_platform_init(subsys);
}

/*
 * AIP subsystem header
 */

#define AIP_MAJVERSION 1
#define AIP_MINVERSION 0
#define AIP_EDITVERSION  1

SUBSYS_HEADER(aip, AIP_MAJVERSION, AIP_MINVERSION, AIP_EDITVERSION,
	      aip_init, SUBSYS_CLASS_DRIVER,
	      "seq: cbus_mci",
	      "req: cbus_mci, atm");
