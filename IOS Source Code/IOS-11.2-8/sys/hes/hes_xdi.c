/* $Id: hes_xdi.c,v 3.5.26.7 1996/08/28 12:44:06 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/hes_xdi.c,v $
 *------------------------------------------------------------------
 * High-End platforms common FDDI SMT functions.
 *
 * June 1994, Walter R. Friedrich
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hes_xdi.c,v $
 * Revision 3.5.26.7  1996/08/28  12:44:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.26.6  1996/08/16  00:49:52  ogrady
 * CSCdi54594:  SRB on FDDI propogates broadcast storms
 *              Branch: California_branch
 *              Very small bridge packets not stripped from ring.  Make
 *              routine that inits FIP hold register common instead of
 *              platform specific.
 *
 * Revision 3.5.26.5  1996/07/23  13:22:38  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.5.26.4  1996/06/07  06:59:01  fsunaval
 * CSCdi54755:  Fixes for SMT conformance tests.
 * Branch: California_branch
 * 1.Fix trace test and other minor MIB values.
 *
 * Revision 3.5.26.3  1996/05/01  09:15:15  shaker
 * CSCdi54444:  Booting causes FIP FDDI to beacon
 * Branch: California_branch
 * Wait around in the process that starts CMT for it to finish
 * so that we know that no other code disables interrupts,
 * preventing us from bringing the MAC online, causing the
 * ring to beacon.
 *
 * Revision 3.5.26.2  1996/03/21  22:40:12  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.5.26.1  1996/03/18  19:40:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.5  1996/02/08  07:21:07  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.4  1996/01/25  12:09:44  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.3.2.3  1996/01/15  10:19:05  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.2  1995/12/26  19:56:30  fsunaval
 * CSCdi46111:  Add parser support for FDDI PA and other changes
 * Branch: ELC_branch
 * 1.Add missing diffs from previous commit for init_pas.c
 *   (NMI interrupts).
 * 2.Add FDDI PA parser support (sh cont, test pas fddi).
 * 3.Do a little code cleanup of hes_fddi.c and rsp_fip.c specifically
 *   in regards to the way trt and tvx are passed down.
 *
 * Revision 3.3.2.1  1995/12/23  04:39:55  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.5  1996/01/18  22:47:09  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/21  16:56:33  hampton
 * Remove old timer related declarations or obsolete conditionally
 * compiled code.  [CSCdi45965]
 *
 * Revision 3.3  1995/11/17  09:16:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/07  05:33:44  richl
 * CSCdi38315:  makefile rework
 *              move code to tring_inline and to rif_util in libutil.a
 *
 * Revision 2.1  1995/06/07  20:40:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*********************************************************************
        Cisco High-End platforms XDI interface Module

        File:                   hes_xdi.c
        Created:                02/14/94

        Version:
        Last Modified:

        This module includes all or part of the XDI interface files
        which contain platform specific code:

                cspintif.c,
                csppcm.c,
                cspsysif.c,
                csplocal.c,
                fbmfrmif.c,
                mibsysif.c

        Modification History:
*********************************************************************/

#include        "master.h"
#include        "logger.h"
#include	"../les/msg_fddi.c"	/* Not a typo, see logger.h */
#include	<ciscolib.h>
#include        "interface_private.h"
#include        "packet.h"
#include        "../if/fddi.h"
#include 	"../if/fddi_private.h"
#include        "mgd_timers.h"
#include        "../hes/if_fci.h"
#include	"../hes/cbus_registry.h"
#include	"../hes/if_cmd.h"

#include        "../xdi/xdihdrsif.h"
#include        "../xdi/fddihdr.h"
#include        "../xdi/smtmsg.h"
#include        "../xdi/csphdr.h"
#include        "../xdi/cspglbl.h"
#include        "../xdi/cspproto.h"
#include        "../xdi/mibglbl.h"
#include        "../xdi/fbmhdr.h"
#include        "../xdi/fbmframe.h"
#include        "../xdi/fbmglbl.h"

#include        "../if/if_fddi.h"
#include        "hes_fddi_public.h"
#include	"../hes/if_fip.h"
#include        "../if/rif_inline.h"
#include        "../if/network.h"
#include        "if_hes_common.h"

/*********************************************************************
        Variables
*********************************************************************/
extern boolean		fddi_smt_packet_debug;
extern boolean		fddi_debug;


/*********************************************************************
        Function Prototypes
*********************************************************************/
extern void fddi_smt_debug(paktype *, boolean);


#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_cspintif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Connection Services Process Module

	Interrupt/Event/Invocation Interface Routines

	THIS MODULE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_cspintif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module provides the interface routines to handle event
	conditions that must be reported to CSP.

	Modification History:
        *** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"cspmacro.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"cspproto.h"
*/


/*********************************************************************
	Interrupt Interface Routines
*********************************************************************/

uInt32
#ifdef __STDC__
InitCSPInterruptsMulti (uInt16 stnID)
#else
InitCSPInterruptsMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Initialize system for processing CSP interrupts. If
			explicit processing is required to enable interrupts
			for this station, it may be done here or during the
			station's start of day procedure.
Parameters:	None.
Input:		None.
Output:		Set system signal processing.
Return:		0 if successful, error number if failed.
*********************************************************************/
{
	return(0);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_cspsysif.c if7_v2.2.1:cset.0036:4:4 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Connection Services Process Module

	System Interface Routines

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_cspsysif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0036		02/10/93
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	All functions listed here are used by CSP to access
	environment capabilities.

	Modification History:
        *** SMT 7.2 ***
		Based on 6.2.3 multistation code.
	930203.007	LJP
		The ANSI definition of ConfigureMACMulti() did not use the
		correct parameter names. They now match the traditional C
		parameters.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"cspmacro.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"smtmsg.h"
#include	"cspproto.h"
*/

/*********************************************************************
	Variables
*********************************************************************/
mgd_timer               CSPTimer;
Flag                    RTTimerStarted = FALSE;
Flag			MACInterruptsEnabled;

/*
* Conversion of S0-2 Endec LS index to the Line State index
* as understood by the XDI code (defined in file hes_xdi.h).
*/
uInt16          ENDECLSIndex2XDILSMask[] = { /* indexed by CRSR LS bits */
                    0,          /* LSU_STATUS	*/
                    0,          /* NLS_STATUS	*/
                    MLS,        /* MLS_STATUS	*/
                    SILS,       /* ILS_STATUS	*/
                    HLS,        /* HLS_STATUS	*/
                    QLS,        /* QLS_STATUS	*/
                    ALS         /* ALS_STATUS	*/
};


/*********************************************************************
        Environment Interface Routines
*********************************************************************/

uInt32
#ifdef __STDC__
ReadSystemTimer (void)
#else
ReadSystemTimer ()
#endif
/*********************************************************************
Function:       Read the current number of microseconds remaining before
                        the next CSP time value expires in the system timer .
Parameters:     None.
Input:          Implementor defined.
Output:         Implementor defined.
Return:         Number of microseconds remaining.
Notes:          None.
Modification History:
*********************************************************************/
{
        /* Get CSP RT timer due time */
        return(1000 * mgd_timer_left_sleeping(&CSPTimer));
}

void
#ifdef __STDC__
SetSystemTimer (uInt32 interval)
#else
SetSystemTimer (interval)
                uInt32                  interval;
#endif
/*********************************************************************
Function:       Set the system timer to time the next CSP timer event.
Parameters:     interval = number of microseconds until timer expiration.
Input:          Implementor defined.
Output:         Implementor defined.
Return:         No value returned.
Notes:          If the interval has the value of 0, then the timer is
                        to be turned off.
Modification History:
*********************************************************************/
{
        /* Set CSP timer */
        if (interval == 0)
            mgd_timer_stop(&CSPTimer);
        else
            mgd_timer_start(&CSPTimer, 1 + interval/1000);

        return;
}

/*********************************************************************
	Station Interface Routines
*********************************************************************/

Flag
#ifdef __STDC__
StationPathTestMulti (uInt16 stnID)
#else
StationPathTestMulti (stnID)
		uInt16                  stnID;
#endif
/*********************************************************************
Function:	Perform a station path test.
Parameters:	stnID		= index of station to use.
Input:		None.
Output:		None.
Return:		TRUE if path test passes
			FALSE if path test fails
Modification History:
*********************************************************************/
{
    hwidbtype   *idb;
    leveltype   level;
    ushort 	response, mode;
    fddisb      *fsb;

    /* Initialize local variables */
    idb = STADATA (stnID, hwIDB[0]);

    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
    if (fddi_debug)
        buginf("\nIn StationPathTestMulti");

    /* Disable interrupts */
    level = raise_interrupt_level(NETS_DISABLE);

    /* Select interface */
	select_if(idb);

    /* Select FORMAC Mode register */
	send_fip_fm_select_cmd(idb, FDDI_FM_RW_MODE_REG);

	/* Take the formac off-line */
    if (fddi_debug)
        buginf("\n\tTake FORMAC off-line");
	send_fip_fm_select_cmd(idb, FDDI_FM_RW_MODE_REG); /* Again, why? */
	send_fip_fm_write_cmd(idb, FM_MODE_OFF_LINE);

	/* Set the configured interrupt mask */
	send_fip_fm_select_cmd(idb, FDDI_FM_RW_INT_MASK);

	mode = FDDIT_FORMAC_BASE_MASK;
	if (idb->fddi_options & FDDI_DUPLICATE_ADDRESS_CHK)
	    mode |= FDDIT_DUP_ADDR_MASK;
	send_fip_fm_write_cmd(idb, mode);
 
	/* Reset the FDDI hold register i.e. the correct transmit offset */
	hes_fddi_set_hold_reg(idb, get_fip_byte_rxoffset(idb));
 
    /* Force into CF_Isolated */
	select_endec(idb, FDDI_EN_RW_CR1, ENDECA);
	send_fip_en_write_cmd(idb, ENDEC_TB_BUS | ENDEC_REPEAT);

	select_endec(idb, FDDI_EN_RW_CR1, ENDECB);
	send_fip_en_write_cmd(idb, ENDEC_TA_BUS | ENDEC_REPEAT);
    CSPDATA (stnID, portData[0].R_Val) =
        ((phytype *)(idb->phy[PHY_A]))->rval = 0;
    CSPDATA (stnID, portData[1].R_Val) =
        ((phytype *)(idb->phy[PHY_B]))->rval = 0;
    reg_invoke_fddi_ringstate_leds(idb);

	/* Put the formac on-line */
    if (fddi_debug)
        buginf("\n\tPut FORMAC on-line");
	send_fip_fm_select_cmd(idb, FDDI_FM_RW_MODE_REG);
	mode = FDDIT_MODE;
	mode |= FM_MODE_RA_BUS;
	send_fip_fm_write_cmd(idb, mode);

	/* Force the FORMAC into claim listen mode */
	send_fip_fm_select_cmd(idb, FDDI_FM_W_CLAIM_LISTEN);
	send_fip_fm_write_cmd(idb, 0);


    /* Wait FDDI_LOOPBACK_DELAY and then check if internal path came up properly */
    usecdelay(FDDI_LOOPBACK_DELAY);
    select_if(idb);
	send_carrier_cmd(idb, &response);
	if (!response) {
		if (fddi_debug)
			buginf("\n\tInternal data path test FAILED");
		fsb->fddi_shutdown_reasons |= THRU_A_FAIL;
		reset_interrupt_level (level);
		idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
		return(FALSE);
	} else {
		if (fddi_debug)
			buginf("\n\tInternal data path test PASSED");
		fsb->fddi_shutdown_reasons &= ~THRU_A_FAIL;
		reset_interrupt_level (level);
		idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
		return(TRUE);
        }
}

void
#ifdef __STDC__
BypassRequestMulti (uInt16 stnID, uInt16 bypassState)
#else
BypassRequestMulti (stnID, bypassState)
		uInt16                  stnID;
		uInt16                  bypassState;
#endif
/*********************************************************************
Function:	Implement the SM_PM_BYPASS.request primitive to change
			the state of the bypass relay.
Parameters:	stnID		= index of station to use.
			bypassState	= one of the defined values:
							INSERTED to insert the relay
							DEINSERTED to deinsert the relay
Input:		None.
Output:		None.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	hwidbtype *idb;
	leveltype level;
	ushort    appl_state;

	/* Initialize local variables */
	idb = STADATA(stnID, hwIDB[0]);

	/* Disable interrupts */
	level = raise_interrupt_level(NETS_DISABLE);

    if (fddi_debug)
        buginf("\nIn BypassRequestMulti, bypassState = %d", bypassState);


	/* Select the bypass switch -- and throw it.  */
	select_if(idb);
	send_fip_applique_select_cmd(idb, BYPASS_SWITCH);
	if (bypassState == DEINSERTED)
	    appl_state = APPLIQUE_BYPASS;
	else
	    appl_state = APPLIQUE_CONNECT;
	send_fip_applique_write_cmd(idb, appl_state);

	/* Restore interrupts */
	reset_interrupt_level(level);
}

void
#ifdef __STDC__
ConfigureAttachMulti (uInt16 stnID, uInt16 CF_State)
#else
ConfigureAttachMulti (stnID, CF_State)
		uInt16                  stnID;
		uInt16                  CF_State;
#endif
/*********************************************************************
Function:	Set the configuration hardware for the attachment
			ports to the CFM configuration given.
Parameters:	stnID		= index of station to use.
			CF_State	= state for hardware configuration as
							defined in the CFM state machine.
Input:		None.
Output:		None.
Return:		No value returned.
Notes:		The implementer may use the given CFM state configuration
			or the CFM state configuration specified by the CFM
			state (portData[A port index].cmState and
			portData[B port index].cmState, or
			portData[S port index].cmState) for the attachment ports.

			Non-concatenation of Frames
			===========================
			To avoid concatenation of frames during a configuration
			change, the SMT document requires that a minimum of 16
			Idle symbols be generated on all ports changing their
			configuration state. To support this operation, this
			routine should perform the following actions:

			1) Disable interrupts.
			2) FOR EACH port involved in configuration change:
				IF (new configuration required)
					THEN
						Read current xmit line state
						IF (xmitLS == PDRLS)
							THEN
								SET xmitLS = ILS
						ENDIF
						SET new configuration
						Restore xmit line state if needed
				ENDIF
			3) Restore interrupts
Modification History:
*********************************************************************/
{
	/* ##
	 * Still need to add loopback for AGS+ boards to make sure they are not
	 * connected the wrong way.
	 * ## */
	hwidbtype *idb;
	leveltype level;
        StnDCBType *staData;
	fddisb *fsb;
	

	/* Initialize local variables */
	idb = STADATA(stnID, hwIDB[0]);
        staData = &STAINST(stnID);

	if (fddi_debug)
	    buginf("\n%s: In ConfigureAttachMulti", idb->hw_namestring);

	/* Disable interrupts */
	level = raise_interrupt_level(NETS_DISABLE);

	/* Select interface */
	select_if(idb);

        /* Program Data Path according to new configuration */
        switch (CF_State)
        {
        case CF_isolated:
        if (fddi_debug)
            buginf("\n\tCF_State = CF_Isolated");
	    select_endec(idb, FDDI_EN_RW_CR1, ENDECA);
	    send_fip_en_write_cmd(idb, ENDEC_TB_BUS | ENDEC_REPEAT);

	    select_endec(idb, FDDI_EN_RW_CR1, ENDECB);
	    send_fip_en_write_cmd(idb, ENDEC_TA_BUS | ENDEC_REPEAT);
        CSPDATA (stnID, portData[0].R_Val) =
            ((phytype *)(idb->phy[PHY_A]))->rval = 0;
        CSPDATA (stnID, portData[1].R_Val) =
            ((phytype *)(idb->phy[PHY_B]))->rval = 0;
        reg_invoke_fddi_ringstate_leds(idb);
	    break;

        case CF_wrap_a:
        case CF_c_wrap_a:
        if (fddi_debug)
            buginf("\n\tCF_State = CF_wrap_a");
	    select_endec(idb, FDDI_EN_RW_CR1, ENDECA);
	    send_fip_en_write_cmd(idb, ENDEC_TB_BUS | ENDEC_THROUGH);

	    select_endec(idb, FDDI_EN_RW_CR0, ENDECA);
	    send_fip_en_write_cmd(idb, ENDEC_ENABLE_REPEAT_FILTER);

	    select_endec(idb, FDDI_EN_RW_CR1, ENDECB);
	    send_fip_en_write_cmd(idb, ENDEC_TA_BUS | ENDEC_REPEAT);
        CSPDATA (stnID, portData[1].R_Val) =
            ((phytype *)(idb->phy[PHY_B]))->rval = 0;
        reg_invoke_fddi_ringstate_leds(idb);
	    break;

        case CF_wrap_b:
        case CF_c_wrap_b:
        if (fddi_debug)
            buginf("\n\tCF_State = CF_wrap_b");
	    select_endec(idb, FDDI_EN_RW_CR1, ENDECA);
	    send_fip_en_write_cmd(idb, ENDEC_TB_BUS | ENDEC_REPEAT);

	    select_endec(idb, FDDI_EN_RW_CR1, ENDECB);
	    send_fip_en_write_cmd(idb, ENDEC_TA_BUS | ENDEC_THROUGH);

	    select_endec(idb, FDDI_EN_RW_CR0, ENDECB);
	    send_fip_en_write_cmd(idb, ENDEC_ENABLE_REPEAT_FILTER);
        CSPDATA (stnID, portData[0].R_Val) =
            ((phytype *)(idb->phy[PHY_A]))->rval = 0;
        reg_invoke_fddi_ringstate_leds(idb);
	    break;

        case CF_thru:
        if (fddi_debug)
            buginf("\n\tCF_State = CF_thru");
	    select_endec(idb, FDDI_EN_RW_CR1, ENDECA);
	    send_fip_en_write_cmd(idb, ENDEC_TB_BUS | ENDEC_THROUGH);

	    select_endec(idb, FDDI_EN_RW_CR0, ENDECA);
	    send_fip_en_write_cmd(idb, ENDEC_ENABLE_REPEAT_FILTER);

	    select_endec(idb, FDDI_EN_RW_CR1, ENDECB);
	    send_fip_en_write_cmd(idb, ENDEC_TA_BUS | ENDEC_THROUGH);

	    select_endec(idb, FDDI_EN_RW_CR0, ENDECB);
	    send_fip_en_write_cmd(idb, ENDEC_ENABLE_REPEAT_FILTER);
        reg_invoke_fddi_ringstate_leds(idb);
	    break;

	default:
	    break;
	}

        if (staData->prevCFState != CF_State) {
	    /* Update IDB CFM state to allow user message to be printed */
	    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
            fsb->cfm_state = CF_State;
	    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI); 
            /* Report Interface State Change if appropriate */
	    if (idb->state != IDBS_ADMINDOWN) {
        	if (staData->prevCFState == CF_isolated) {
		    net_cstate(idb, IDBS_UP);
		    rif_update(idb, 0, idb->bit_swapped_hardware, NULL, 
 			       NULL, RIF_INTERFACE);
		    idb->counters.transitions++;
		} else if (CF_State == CF_isolated) {
		    net_cstate(idb, IDBS_DOWN);
		    idb->counters.transitions++;
		}
	    }
            /* Update CFM state history variable */
            staData->prevCFState = CF_State;
	}

	/* Restore interrupts */
	reset_interrupt_level(level);
}

/*********************************************************************
	PORT Interface Routines
*********************************************************************/

uInt16
#ifdef __STDC__
ReadLineStateMulti (uInt16 stnID, uInt16 portID)
#else
ReadLineStateMulti (stnID, portID)
		uInt16                  stnID;
		uInt16                  portID;
#endif
/*********************************************************************
Function:	Return the current value of the line state history
			including the current line state being received. Only
			those line states of interest to SMT should be included:
			QLS, HLS, MLS, SILS, NLS (if noise timer is not in HW)
			and ILS. If support for SILS is provided by hardware, then
			ILS should not be reported.
Parameters:	stnID	= index of station to use.
			portID	= index of PORT to read.
Input:		None.
Output:		Line state history is cleared.
Return:		Bit vector value indicating line states.
Modification History:
*********************************************************************/
{
	/*
	 * The implementation on the High-End will return only the
	 * current line state, since it is only used by the
	 * ECM state machine to check for Stuck Optical
	 * Bypass conditions (all PCM is done in microcode).
	 */
	hwidbtype *idb;
	leveltype level;
	uInt16 ls;
	ushort status;

	/* Initialize local variables */
	idb = STADATA(stnID, hwIDB[0]);

	/* Disable interrupts */
	level = raise_interrupt_level(NETS_DISABLE);

	/* Select interface */
	status = select_if(idb);
	if (status != FCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    return(0);
	}

	/* Read line state register */
	select_fiber_status(idb, portID ? ENDECB : ENDECA);
	status = send_fip_applique_read_cmd(idb, &ls);
	if (status != FCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    return(0);
	}

	/* Restore interrupts */
	reset_interrupt_level(level);

	/* Return the value read from the endec */
	return(ENDECLSIndex2XDILSMask[ls]);
}

void
#ifdef __STDC__
SendLineStateMulti (uInt16 stnID, uInt16 portID, uInt16 ls)
#else
SendLineStateMulti (stnID, portID, ls)
                uInt16                  stnID;
                uInt16                  portID;
                uInt16                  ls;
#endif
/*********************************************************************
Function:       Implement the SM_PH_LINE-STATE.request primitive to cause
                        the PORT to transmit a specified symbol stream.
Parameters:     stnID   = index of station to use.
                        portID  = index of PORT to change.
                        ls              = defined value indicating symbol stream to transmit.
Input:          None.
Output:         Sets port to transmit appropriate line state.
Return:         No value returned.
Modification History:
*********************************************************************/
{
        /*
         *  This function is only used in the Cisco High-End platforms
	 * to implement the scrubbing function.
         */
	hwidbtype *idb;
	leveltype level;
	ushort    arg;

	/* Initialize local variables */
	idb = STADATA(stnID, hwIDB[0]);

	/* Disable interrupts */
	level = raise_interrupt_level(NETS_DISABLE);

	/* Select interface */
	select_if(idb);

	/* Write Transmit register */
	select_endec(idb, FDDI_EN_RW_CR0, portID);
	switch (ls) {
	case QLS:
	    arg = ENDEC_FORCE_QUIET;
	    break;
	case HLS:
	    arg = ENDEC_FORCE_HALT;
	    break;
	case ILS:
	    arg = ENDEC_FORCE_IDLE;
	    break;
	case MLS:
	    arg = ENDEC_FORCE_MASTER;
	    break;
	case PDRLS:
	    arg = ENDEC_ENABLE_REPEAT_FILTER;
	    break;
	default:
	    reset_interrupt_level(level);
	    return;
	}
	send_fip_en_write_cmd(idb, arg);	

	/* Restore interrupts */
	reset_interrupt_level(level);
}


/*********************************************************************
	MAC Interface Routines
*********************************************************************/

void
#ifdef __STDC__
MACSetInterruptsMulti (uInt16 stnID, uInt16 macID, uInt16 state)
#else
MACSetInterruptsMulti (stnID, macID, state)
		uInt16                  stnID;
		uInt16                  macID;
		uInt16                  state;
#endif
/*********************************************************************
Function:	Change the MAC's interrupt settings.
Parameters:	stnID	= index of station to use.
			macID	= index of MAC to reset.
			state	= ENABLE to allow all interrupts required by
						the core code.
					  DISABLE to allow only Ring_Op and Rmode
						interrupts (initial state).
Input:		None.
Output:		MAC interrupt setting changed as required.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * We don't have direct control over the MAC interrupts.
	 * Most of them are handled by the microcode. Only
	 * the Mismatch in the TReq and TBid interrupt will be
	 * reported to RMT if the flag MACInterruptsEnabled is
	 * set.
	 */
	MACInterruptsEnabled = state;

	return;
}

void
#ifdef __STDC__
MACResetRequestMulti (uInt16 stnID, uInt16 macID)
#else
MACResetRequestMulti (stnID, macID)
		uInt16                  stnID;
		uInt16                  macID;
#endif
/*********************************************************************
Function:	Implement the SM_MA_CONTROL.request primitive for the
			control_action parameter set to Reset.
Parameters:	stnID	= index of station to use.
			macID	= index of MAC to reset.
Input:		None.
Output:		None.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * This function calls the fddi_reset function which
	 * besides resetting the chip initializes its address,
	 * TRT and some other SMT related parameters.
	 */
/*	fddi_reset(STADATA (stnID, hwIDB[macID]));
*/
}

void
#ifdef __STDC__
MACClaimRequestMulti (uInt16 stnID, uInt16 macID)
#else
MACClaimRequestMulti (stnID, macID)
		uInt16                  stnID;
		uInt16                  macID;
#endif
/*********************************************************************
Function:	Implement the SM_MA_CONTROL.request primitive for the
			control_action parameter set to Claim. The MAC is put
			into states T4:CLAIM/R0:LISTEN.
Parameters:	stnID	= index of station to use.
			macID	= index of MAC to start claiming.
Input:		None.
Output:		None.
Return:		No value returned.
Modification History:
*********************************************************************/
{
        hwidbtype *idb;
        leveltype level;

        /* Initialize local variables */
        idb = STADATA(stnID, hwIDB[macID]);


        if (fddi_debug)
            buginf("\nIn MACClaimRequestMulti");

        /* Disable interrupts */
        level = raise_interrupt_level(NETS_DISABLE);

        /* Select interface */
	select_if(idb);

	/* Force the FORMAC into claim listen mode */
	send_fip_fm_select_cmd(idb, FDDI_FM_W_CLAIM_LISTEN);
	send_fip_fm_write_cmd(idb, 0);

        /* Restore interrupts */
        reset_interrupt_level(level);
}

void
#ifdef __STDC__
SetBeaconMulti (uInt16 stnID, uInt16 macID, uChar beaconType,
	FDDILongAddressType DA, uInt16 infoLength, uChar * beaconInfo)
#else
SetBeaconMulti (stnID, macID, beaconType, DA, infoLength, beaconInfo)
		uInt16                  stnID;
		uInt16                  macID;
		uChar                   beaconType;
		FDDILongAddressType     DA;
		uInt16                  infoLength;
		uChar                  *beaconInfo;
#endif
/*********************************************************************
Function:	Change the beacon frame sent out by the MAC. This will NOT
			cause the MAC to beacon.
Parameters:	stnID		= index of station to use.
			macID		= index of MAC to change.
			beaconType	= value to place in beacon type field.
			DA			= destination address to use.
			infoLength	= length of beacon info field (must be signed).
			beaconInfo	= buffer containing beacon info.
Input:		None.
Output:		Changes beacon frame to be transmitted.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * All Beacon transmission is done in microcode.
	 * Both Beacon frame types are setup at initialization time,
	 * and the microcode does know how to pick the right type
	 * of Beacon for transmission.
	 *
	 * The only thing that is required from software is
	 * to determine the time to start transmitting Directed
	 * Beacons.
	 * The microcode starts the Trace process automatically
	 * after the transmission of Directed Beacons.
	 */
	hwidbtype	*idb;
	fddisb          *fsb;

	/* Check for Directed Beacons request */
	if (beaconType == BCN_DIRECTED) {
	    
	    /*
	     * Notify microcode that it is time to send
	     * Directed Beacons.
	     */
	    idb = STADATA(stnID, hwIDB[macID]);
	    cbus_daughter_cmd(idb, FCI_FDDI_START_TRACE, 0);
	    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
	    fsb->fddi_trace_count++;
	    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
	}
}

void
#ifdef __STDC__
BeaconRequestMulti (uInt16 stnID, uInt16 macID)
#else
BeaconRequestMulti (stnID, macID)
		uInt16                  stnID;
		uInt16                  macID;
#endif
/*********************************************************************
Function:	Implement the SM_MA_CONTROL.request primitive for the
			control_action parameter set to Beacon.
Parameters:	stnID	= index of station to use.
			macID	= index of MAC to start beaconing.
Input:		None.
Output:		None.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
	/*
	 * All Beacon frame handling is done by the microcode.
	 */
	return;
}

uInt16
#ifdef __STDC__
ChangeMACAddressMulti (uInt16 stnID, uInt16 macID,
	FDDILongAddressType newAddress)
#else
ChangeMACAddressMulti (stnID, macID, newAddress)
		uInt16                  stnID;
		uInt16                  macID;
		FDDILongAddressType     newAddress;
#endif
/*********************************************************************
Function:	Change the MAC's address to a new long address.
Parameters:	stnID		= index of station to use.
			macID		= index of MAC to start beaconing.
			newAddress	= buffer for new address in use by MAC.
Input:		None.
Output:		Sets new address in MAC if possible.
			newAddress	= new MAC address if changed.
Return:		0				= address changed successfully.
			RMT_DUP_JAM_A	= address NOT changed, do Jam_A_Actions
			RMT_DUP_JAM_B	= address NOT changed, do Jam_B_Actions
			RMT_DUP_LEAVE	= address NOT changed, do Leave_Actions
			Any other value is interpreted as RMT_DUP_LEAVE
Notes:		Due to the various addressing schemes that can be devised
			for FDDI nodes, this routine has been made as flexible as
			possible. In the RMT state machine, the Change_Actions
			footnote says to perform the address change if a local
			address is in use. This implementation allows the implementer
			to decide when, if at all, to change the address. Each time
			a duplicate address is detected and RMT_DUP_CHANGE is
			the current policy, this routine will be called. The
			implementer can then decide how many times the address can
			be changed. If the address is not changed, then the
			alternate policy to use is specified in the returned value.

			FDDILongAddressType is an array so the parameter newAddress
			is actually a pointer.

			If the address is changed, RMT will notify the MIB of the
			new address in use.
Modification History:
*********************************************************************/
{
	/*
	 * JAM actions not supported.
	 */
	return(RMT_DUP_LEAVE);
}

void
#ifdef __STDC__
SetMACInputMulti (uInt16 stnID, uInt16 macID, Flag state)
#else
SetMACInputMulti (stnID, macID, state)
		uInt16                  stnID;
		uInt16                  macID;
		Flag                    state;
#endif
/*********************************************************************
Function:	Perform special configuration control for Jam_A_Actions.
			This function will (dis)connect the MAC's input from
			the data path.
Parameters:	stnID	= index of station to use.
			macID	= index of MAC to change.
			state	= MAC input connection state
					  TRUE if connected.
					  FALSE if not connected.
Input:		None.
Output:		None.
Return:		No value returned.
Notes:		If the station does not have the hardware capability to
			leave the MAC operational and have its output active with
			the input disconnect, then the Jam_A_Actions policy should
			not be allowed.
Modification History:
*********************************************************************/
{
	/*
	 * JAM actions not supported.
	 */
	return;
}

void
#ifdef __STDC__
ConfigureMACMulti (uInt16 stnID, uInt16 macID, Flag state, uInt16 pathID,
	uInt32 TVXLB, uInt32 T_MaxLB, uInt32 MaxT_Req)
#else
ConfigureMACMulti (stnID, macID, state, pathID, TVXLB, T_MaxLB, MaxT_Req)
		uInt16                  stnID;
		uInt16                  macID;
		Flag                    state;
		uInt16                  pathID;
		uInt32                  TVXLB;
		uInt32                  T_MaxLB;
		uInt32                  MaxT_Req;
#endif
/*********************************************************************
Function:	This routine implements the MAC CFM footnotes and
			configures the MAC for operation on the specified path.
Parameters:	stnID	= index of station to use.
			macID	= CSP index of MAC to configure.
			state	= ENABLE if the MAC is to be placed in the data path
						and activated.
					  DISABLE if the MAC is to be removed and turned off.
			pathID	= CSP index of the path to configure the MAC onto.
			TVXLB	= lower bound on the TVX time value on this path.
						The MAC must be configured with the minimum
						TVX time value >= TVXLB.
			T_MaxLB	= lower bound on the T_Max time value on this path.
						The MAC must be configured with the minimum
						T_Max time value >= T_MaxLB.
			MaxT_Req = upper bound on the T_Req time value on this path.
						The MAC must be configured with the maximum
						T_Req time value <= MaxT_Req.
Input:		None.
Output:		MAC is configured onto specied path the appropriate
			operational values.
Return:		None.
Notes:		TVXLB, T_MaxLB and MaxT_Req are presented in 2s COMPLEMENT
			format. Thus, the minimum TVX time value >= TVXLB will be
			some TVX_value such that TVX_value <= TVXLB.

			If the state == DISABLE, then the path is not valid and
			the operational values do not need to be set.

			The values for pathID, TVXLB, T_MaxLB and MaxT_Req have
			all been verified by RMT such that this MAC will support
			the requested configuration.

			When setting the new T_Req value, be sure that the value in
			the Claim frame matches the value used by the MAC.
Modification History:
930203.007	LJP
*********************************************************************/
{
	hwidbtype	*idb;
        leveltype	level;
	uInt16		mode;

        /* Initialize local variables */
        idb = STADATA(stnID, hwIDB[macID]);

        if (fddi_debug)
            buginf("\nIn ConfigureMACMulti");

        /* Disable interrupts */
        level = raise_interrupt_level(NETS_DISABLE);

        /* Select interface */
	select_if(idb);

        /* Select FORMAC Mode register */
	send_fip_fm_select_cmd(idb, FDDI_FM_RW_MODE_REG);

	/* Take the formac off-line */
    if (fddi_debug)
        buginf("\n\tTake FORMAC off-line");
	send_fip_fm_select_cmd(idb, FDDI_FM_RW_MODE_REG); /* Again, why? */
	send_fip_fm_write_cmd(idb, FM_MODE_OFF_LINE);

	/* Leave MAC off-line if new state is DISABLE */
	if (state == DISABLE) {
	    reset_interrupt_level(level);
	    return;
	}
 
	/* Set the configured interrupt mask */
	send_fip_fm_select_cmd(idb, FDDI_FM_RW_INT_MASK);

	mode = FDDIT_FORMAC_BASE_MASK;
	if (idb->fddi_options & FDDI_DUPLICATE_ADDRESS_CHK)
	    mode |= FDDIT_DUP_ADDR_MASK;
	send_fip_fm_write_cmd(idb, mode);
 
	/* Reset the FDDI hold register i.e. the correct transmit offset */
	hes_fddi_set_hold_reg(idb, get_fip_byte_rxoffset(idb));
 
	/* Put the formac on-line */
    if (fddi_debug)
        buginf("\n\tPut FORMAC on-line");
	send_fip_fm_select_cmd(idb, FDDI_FM_RW_MODE_REG);
	mode = FDDIT_MODE;
	mode |= FM_MODE_RA_BUS;
	send_fip_fm_write_cmd(idb, mode);

    /* Restore interrupts */
    reset_interrupt_level(level);
}

Flag
#ifdef __STDC__
ReadMACR_FlagMulti (uInt16 stnID, uInt16 macID)
#else
ReadMACR_FlagMulti (stnID, macID)
		uInt16                  stnID;
		uInt16                  macID;
#endif
/*********************************************************************
Function:	Read the current setting of the MAC's R_Flag.
Parameters:	stnID	= index of station to use.
			macID	= index of MAC to reset.
Input:		None.
Output:		None.
Return:		TRUE if the R_Flag is SET
			FALSE otherwise.
Modification History:
*********************************************************************/
{
        /*
        * Restricted-Token monitoring not supported.
        */
	return(FALSE);
}


/*********************************************************************
	LEM Interface Routines
*********************************************************************/

void
#ifdef __STDC__
SetLEMCounterMulti (uInt16 stnID, uInt16 portID, uInt16 threshold)
#else
SetLEMCounterMulti (stnID, portID, threshold)
		uInt16                  stnID;
		uInt16                  portID;
		uInt16                  threshold;
#endif
/*********************************************************************
Function:	Set the LEM hardware counter to cause an interrupt when
			it counts threshold number of errors.
Parameters:	stnID		= index of station to use.
			portID		= index of PORT whose link is to be monitored.
			threshold	= number of error to count before giving
							an interrupt.
Input:		None.
Output:		None.
Return:		None.
Modification History:
*********************************************************************/
{
	phytype         *phy;

        /* Initialize local variables */
	phy = (phytype *)STADATA(stnID, portData[portID]);

	/*
	 * High-End doesn't support LEM based interrupts.
	 * Therefore, LEM will rely exclusively on the LEM
	 * polling algorithm (every 4 seconds).
	 *
	 * The LEM 16 bits down counter will be simulated by
	 * saving the threshold value to indicate the LEM error
	 * events still left when using the ReadLEMCounterMulti ()
	 * function.
	 */
	phy->LEMThreshold = threshold;
	phy->HWLEMErrorCt = 0;

	return;
}

uInt16
#ifdef __STDC__
ReadLEMCounterMulti (uInt16 stnID, uInt16 portID)
#else
ReadLEMCounterMulti (stnID, portID)
		uInt16                  stnID;
		uInt16                  portID;
#endif
/*********************************************************************
Function:	Read the LEM hardware counter.
Parameters:	stnID	= index of station to use.
			portID	= index of PORT whose link is being monitored.
Input:		None.
Output:		None.
Return:		Number of errors left to count before threshold is reached.
Modification History:
*********************************************************************/
{
        hwidbtype       *idb;
        leveltype       level;
	phytype		*phy;

        /* Initialize local variables */
        idb = STADATA(stnID, hwIDB[0]);
	phy = (phytype *)STADATA(stnID, portData[portID]);

	/* Make sure that LEM is supported */
	if (!(idb->fddi_options & FDDI_LEM_SUPPORTED))
	    return(0);

        /* Disable interrupts */
        level = raise_interrupt_level(NETS_DISABLE);

        /* Select interface */
	select_if(idb);

	/* Update LEM Errors count */
	phy->HWLEMErrorCt +=
	    (ushort)send_errors_cmd(idb, portID ? FCI_ERR_FDDI_LEM_B :
				    FCI_ERR_FDDI_LEM_A);

	/* Restore interrupts */
	reset_interrupt_level(level);

	if (phy->LEMThreshold <= phy->HWLEMErrorCt)
	    return(0);
	else
	    return(phy->LEMThreshold - phy->HWLEMErrorCt);
}

void
#ifdef __STDC__
SetLEMStateMulti (uInt16 stnID, uInt16 portID, Flag state)
#else
SetLEMStateMulti (stnID, portID, state)
		uInt16                  stnID;
		uInt16                  portID;
		Flag                    state;
#endif
/*********************************************************************
Function:	Enable or disable the LEM hardware for a link.
Parameters:	stnID	= index of station to use.
			portID	= index of PORT whose link is being monitored.
			state	= ENABLE to start monitoring the link
					  DISABLE to stop monitoring the link
Input:		None.
Output:		None.
Return:		None.
Notes:		IMPORTANT: Be sure that the LEM interrupt has been cleared
			and that the threshold counter is properly set PRIOR
			to enabling the interrupt.
Modification History:
*********************************************************************/
{
        hwidbtype       *idb;
        leveltype       level;
	ushort		new_lem_state;

        /* Initialize local variables */
        idb = STADATA(stnID, hwIDB[0]);

	/* Make sure that LEM is supported */
	if (!(idb->fddi_options & FDDI_LEM_SUPPORTED))
	    return;

        /* Disable interrupts */
        level = raise_interrupt_level(NETS_DISABLE);

        /* Select interface */
	select_if(idb);

	/* Enable/Disable LEM */
	if (state)
	    new_lem_state = FDDI_LEM_ON;
	else
	    new_lem_state = FDDI_LEM_OFF;
	if (portID)
	    send_fip_phy_b_lem_cmd(idb, new_lem_state);
	else
	    send_fip_phy_a_lem_cmd(idb, new_lem_state);

	/* Restore interrupts */
	reset_interrupt_level(level);
}

void
#ifdef __STDC__
SetupMACLCTMulti (uInt16 stnID, uInt16 portID)
#else
SetupMACLCTMulti (stnID, portID)
		uInt16                  stnID;
		uInt16                  portID;
#endif
/*********************************************************************
Function:	Prepare MAC for use during link confidence test.
Parameters:	stnID	= index of station to use.
			portID	= index of PORT on link to be tested.
Input:		None.
Output:		None.
Return:		None.
*********************************************************************/
{
        /*
        * MAC use during LCT not supported.
        */
	return;
}

void
#ifdef __STDC__
FinishMACLCTMulti (uInt16 stnID, uInt16 portID)
#else
FinishMACLCTMulti (stnID, portID)
		uInt16                  stnID;
		uInt16                  portID;
#endif
/*********************************************************************
Function:	Restore MAC from use during link confidence test.
Parameters:	stnID	= index of station to use.
			portID	= index of PORT on link tested.
Input:		None
Output:		None
Return:		None.
*********************************************************************/
{
        /*
        * MAC use during LCT not supported.
        */
	return;
}

#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_csplocal.c if7_v2.2.1:cset.0009:3:
3 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS R
ESERVED";
#endif
/*********************************************************************
        Connection Services Process Module

        CSP Local Processing Module

        THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

        File:                   if7_csplocal.c
        Created:                12/01/89

        Version:                if7_v2.2.1              Mon Mar  1 16:49:14 199
3
        Last Modified:  cset.0009               10/23/92

        Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

        This module contains functions relevant to a particular
        implementation. The implementor may provide any additional
        support and functionality required for the product. Those
        areas in the CSP that have specific local action
        references call functions located in this file.

        Modification History:
        *** SMT 7.2 ***
                Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include        "smtdefs.h"
#include        "smttypes.h"
#include        "smterror.h"
#include        "smtmacro.h"
#include        "fddihdr.h"
#include        "smtmsg.h"
#include        "cspmacro.h"
#include        "csphdr.h"
#include        "cspglbl.h"
#include        "cspproto.h"
*/

/*********************************************************************
        Variables
*********************************************************************/
Flag		CSPTimerStarted = FALSE;


/*********************************************************************
        Local Processing Functions
*********************************************************************/

uInt32
#ifdef __STDC__
InitCSPLocalMulti (uInt16 stnID)
#else
InitCSPLocalMulti (stnID)
                uInt16                  stnID;
#endif
/*********************************************************************
Function:       Provide local initialization support. Such support can
                        include hardware initialization, local data initializat
ion,
                        system communications, etc.
Parameters:     None.
Input:          None.
Output:         None.
Return:         0 if initialization is successful, otherwise an error code
                is returned.
*********************************************************************/
{
uInt32                  result;

        /*
        * In systems where MSP is known to be initialized prior to CSP,
        * the CSP initialization process may be completed here by
        * calling InitCSPMIBDataMulti(). This routine reads the necessary
        * CSP data from the MIB.
        *
        * For systems that have MSP running as a separate process, CSP
        * must wait for MSP to be initialized before requesting this
        * information. One solution to this situation is to have MSP
        * send an impelementer-defined message to CSP saying that MSP
        * has been initialized. CSP will process this message in
        * LocalCSPMessages() (see below) by calling InitCSPMIBDataMulti().
        */

	/* Initialize CSP MIB */
        result = InitCSPMIBDataMulti(stnID);
        if (result == 0)
                /* set Path_Test value */
	    CSPDATA(stnID, stationData.Path_Test)
                        = StationPathTestMulti(stnID) ? PT_Passed : PT_Failed;

        /* Create CSP process-level timer if not done yet */
        if (!CSPTimerStarted) {
            CSPTimerStarted = TRUE;
	    mgd_timer_init_leaf(&CSPTimer, NULL, 0, NULL, TRUE);
	    cfork(fddi_cmt_timeout, 0, 0, "CMT timer", 0);
        }

	return(result);
}

#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_fbmfrmif.c if7_v2.2.1:cset.0009:2:2
 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RES
ERVED";
#endif
/*********************************************************************
        Frame Based Management Module

        Frame Interface Routines

        THIS MODULE IS TO BE PORTED BY THE IMPLEMENTOR.

        File:                   if7_fbmfrmif.c
        Created:                12/01/89

        Version:                if7_v2.2.1              Mon Mar  1 16:49:14 1993
        Last Modified:  cset.0009               10/23/92

        Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

        This module provides the interface routines to handle
        the sending and receiving for frames.

        Modification History:
        *** SMT 7.2 ***
                Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include        "smtdefs.h"
#include        "smttypes.h"
#include        "smtmacro.h"
#include        "fddihdr.h"
#include        "smterror.h"
#include        "fbmmacro.h"
#include        "fbmhdr.h"
#include        "fbmframe.h"
#include        "fbmglbl.h"
#include        "mib.h"
#include        "smtmsg.h"
#include        "fbmproto.h"
*/


/*********************************************************************
        Frame Interface Routines
*********************************************************************/

void
#ifdef __STDC__
SendSMTFrameMulti (uInt16 stnID, Void_p frameBuffer, uInt16 frameLength, uInt16 MACNum)
#else
SendSMTFrameMulti (stnID, frameBuffer, frameLength, MACNum)
                uInt16                  stnID;
                Void_p                  frameBuffer;
                uInt16                  frameLength;
                uInt16                  MACNum;
#endif
/*********************************************************************
Function:       Receive an SMT frame from the network.
Parameters:     stnID           = index of station to use.
                        frameBuffer     = address of buffer containing frame.
                        frameLength     = frame length.
                        MACNum          = output MAC index.
Input:          frameBuffer     = contains frame to send.
Output:         None.
Return:         None.
*********************************************************************/
{
        paktype *p;
	hwidbtype *idb;
	MACHdrType *machdr;

	if (fddi_smt_packet_debug)
		buginf("\nIn SendSMTFrameMulti");

    if (!FBMDATA(stnID, fbmMACData[MACNum].SM_MA_Avail))
        return;

	/* Initialize local variables */
	idb = STADATA(stnID, hwIDB[MACNum]);

    /* Copy frame information to local buffer and enqueue for tx */
    p = getbuffer(frameLength);
    if ( !p ) {
        return;
    }
    p->datagramsize = frameLength;
    p->datagramstart = p->network_start - FDDI_SNAP_ENCAPBYTES;
    bcopy(frameBuffer, p->datagramstart, frameLength + EXTRA_FC_BYTES);
    machdr = (MACHdrType *)(p->datagramstart);
    p->datagramstart += EXTRA_FC_BYTES;
	p->if_output = idb->firstsw;
    if (fddi_smt_packet_debug)
        fddi_smt_debug(p, TRUE);
	if (idb->type != IDBTYPE_FDDI) {
	    ieee_swap((uchar *)(machdr->DA), (uchar *)(machdr->DA));
	    ieee_swap((uchar *)(machdr->SA), (uchar *)(machdr->SA));
	}

    datagram_out(p);
}

uInt32
#ifdef __STDC__
InitFBMFrameMulti (uInt16 stnID)
#else
InitFBMFrameMulti (stnID)
                uInt16                  stnID;
#endif
/*********************************************************************
Function:       Initialize system for processing SMT frames.
Parameters:     stnID           = index of station to use.
Input:          None.
Output:         Sets system for frame processing.
Return:         0 if successful, error number if failed.
*********************************************************************/
{
        return(0);
}

#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_mibsysif.c if7_v2.2.1:cset.0009:3:3 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
	Management Information Base Module

	System Interface Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_mibsysif.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module is responsible for interfacing to the system
	environment for providing information used and supplied by
	the Management Information Base (MIB). This module provides
	routines to access hardware- and system-level data.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"fddihdr.h"
#include	"smtmacro.h"
#include	"mibdefs.h"
#include	"mibvend.h"
#include	"mibtypes.h"
#include	"mibglbl.h"
#include	"smtmsg.h"
#include	"mibproto.h"
*/


/*********************************************************************
	Defined Values
*********************************************************************/


/*********************************************************************
	External Variables
*********************************************************************/


/*********************************************************************
	MAC Interface Routines
*********************************************************************/

uInt32
#ifdef __STDC__
GetMACDataMulti (uInt16 stnID, uInt16 attr, MACDataPtr macPtr, uChar * buffer)
#else
GetMACDataMulti (stnID, attr, macPtr, buffer)
		uInt16                  stnID;
		uInt16                  attr;
		MACDataPtr              macPtr;
		uChar                  *buffer;
#endif
/*********************************************************************
Function:	Get a MAC attribute.
			Many of the MAC-related attributes are contained
			within the MAC or its associated hardware and software.
			To get the most current data, GetMIBMACAttr() calls
			GetMACDataMulti() to retrieve the data from the system itself.
Parameters:	stnID	= station index to use.
			attr	= MAC attribute to retrieve.
			macPtr	= this is the MAC entry in the MIB.
			buffer	= pointer to a buffer to hold the requested
					attribute value.
Input:		None.
Output:		Sets buffer to contain requested attribute.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or attribute ID not known
*********************************************************************/
{
uInt32                  result = RC_Success;	/* return value */
uInt16                  macID;                  /* MAC index */
hwidbtype	       *idb;
fddisb                 *fsb;

        /* Initialize local variables */
	macID = macPtr->baseData.xdi.xdiIndex;
        idb = STADATA(stnID, hwIDB[macID]);

        fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);

	/*
	*	Read the appropriate MAC attribute.
	*/
	switch (attr)
	{
	case fddiMACT_Req:
        /* Convert from useconds to 2s complement of 80ns units*/
		((TLV32BitType *) buffer)->data = -(fsb->fddi_trt*1000/80);
		break;

	case fddiMACT_Neg:
        /* Convert from useconds to 2s complement of 80ns units*/
		((TLV32BitType *) buffer)->data = -(fsb->fddi_trt*1000/80);
		break;

	case fddiMACT_Max:
		((TLV32BitType *) buffer)->data = -DEFAULT_T_MAX;
		break;

	case fddiMACTvxValue:
		((TLV32BitType *) buffer)->data = -(fsb->fddi_tvx*1000/80);
		break;

	case fddiMACFrame_Ct:
		get_frinc_count(idb);
		((TLV32BitType *) buffer)->data = fsb->fddi_frinc;
		break;

	case fddiMACCopied_Ct:
		((TLV32BitType *) buffer)->data = idb->counters.inputs;
		break;

	case fddiMACTransmit_Ct:
		((TLV32BitType *) buffer)->data = idb->counters.outputs;
		break;

	case fddiMACToken_Ct:
		result = RC_NoParameter;
		break;

	case fddiMACError_Ct:
		((TLV32BitType *) buffer)->data = idb->counters.input_crc;
		break;

	case fddiMACLost_Ct:
		((TLV32BitType *) buffer)->data = idb->counters.input_frame;
		break;

	case fddiMACTvxExpired_Ct:
		result = RC_NoParameter;
		break;

	case fddiMACNotCopied_Ct:
		result = RC_NoParameter;
		break;

	case fddiMACRingOp_Ct:
		((TLV32BitType *) buffer)->data = idb->fddi_rops;
		break;

	case fddiMACLate_Ct:
		result = RC_NoParameter;
		break;

	case fddiMACT_Pri0:
		result = RC_NoParameter;
		break;

	case fddiMACT_Pri1:
		result = RC_NoParameter;
		break;

	case fddiMACT_Pri2:
		result = RC_NoParameter;
		break;

	case fddiMACT_Pri3:
		result = RC_NoParameter;
		break;

	case fddiMACT_Pri4:
		result = RC_NoParameter;
		break;

	case fddiMACT_Pri5:
		result = RC_NoParameter;
		break;

	case fddiMACT_Pri6:
		result = RC_NoParameter;
		break;

	default:
		result = RC_NoParameter;
		break;
	}

        idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
	return(result);
}

uInt32
#ifdef __STDC__
GetMACAddressesMulti (uInt16 stnID, MACDataPtr macPtr, uInt16 bufSize,
	TLVParamType * paramData)
#else
GetMACAddressesMulti (stnID, macPtr, bufSize, paramData)
		uInt16                  stnID;
		MACDataPtr              macPtr;
		uInt16                  bufSize;
		TLVParamType           *paramData;
#endif
/*********************************************************************
Function:	MIB access routine to get the attribute
			fddiMACLongGrpAddrs or fddiMACShortGrpAddrs.
Parameter:	stnID		= index of station to use.
			macPtr		= MAC entry in the MIB.
			bufSize		= maximum length of buffer paramData
							measured in chars. This includes the
							space used for the parameter header.
			paramData	= pointer to buffer containing the
							attribute information to be removed
							and used to hold attribute value returned.
Input:		paramData	= contains the attribute type and Get
							request length represented in MIB TLV
							encoding.
						paramData->paramType contains the
							attribute type.
						paramData->MACINDEX contains the index
							of the MAC to use.
						&paramData->MACOTHER is the starting
							address for the attribute data.
Output:		paramData	= holds the attribute value after in the MIB
							TLV encoding specified in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported
Notes:		GetMACAddresses() may only be used with attributes
			fddiMACLongGrpAddrs and fddiMACShortGrpAddrs. If all
			addresses in the specified attribute do not fit in the
			buffer, then put as many addresses as will fit.
			REMEMBER TO PAD THE ATTRIBUTE AT THE END WITH ENOUGH X'00' SO
			THAT THE LENGTH OF THE ATTRIBUTE IS A MULTIPLE OF 4 BYTES.
*********************************************************************/
{
	/*
	* These attributes are optional
	*/
	return(RC_NoParameter);
}

uInt32
#ifdef __STDC__
SetMACDataMulti (uInt16 stnID, uInt16 attr, MACDataPtr macPtr, TLVParamType *p)
#else
SetMACDataMulti (stnID, attr, macPtr, p)
		uInt16                  stnID;
		uInt16                  attr;
		MACDataPtr              macPtr;
		TLVParamType			*p;
#endif
/*********************************************************************
Function:	Set a MAC attribute.
			Some of the MAC-related attributes are contained
			within the MAC or its associated hardware and software.
			To set the data values, SetMIBMACAttr() calls SetMACDataMulti()
			to place the data into the appropriate location.
Parameters:	stnID	= index of station to use.
			attr	= MAC attribute to set.
			macPtr	= MIB entry for the MAC to change.
			p		= pointer to a buffer holding the data value.
Input:		p		= attribute in TLV encoding.
Output:		Hardware or system software is set with new value.
			Additionally, the new value may be written to non-volatile
			memory. Some of the MAC attributes listed are managed by
			the MIB database and are passed to this routine only to be
			stored in non-volatile memory if desired.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
				RC_Success
					attribute changed successfully
				RC_NoParameter
					attribute not supported
				RC_OutOfRange
					value given is out of bounds
*********************************************************************/
{
uInt32                  result = RC_Success;

	switch (attr)
	{
	case fddiMACRequestedPaths:
	case fddiMACFrameErrorThreshold:
	case fddiMACMA_UnitdataEnable:
	case fddiMACNotCopiedThreshold:
		/*
		* These attributes are managed by the MIB database.
		* They are passed to this routine for storage in
		* non-volatile memory.
		*/
		result = RC_Success;
		break;

	case fddiMACVendorAttrib:
                /* check which vendor */
                if ((p->MACVENDORID & VENDOR_MASK) == fddiXDI)
                {
                        /* store XDI attribute values */
                        if (p->MACVENDORID == xdiMACRMTDupPolicy)
                        {
                        }

                        else if (p->MACVENDORID == xdiMACLocalPath)
                        {
                        }
                }

		/* handle other vendors with else-ifs */

		break;


	default:
		/*
		* Currently, there are no MAC parameters that need to
		* be written to the MAC hardware.
		*/
		result = RC_NoParameter;
		break;
	}

	return(result);
}


/*********************************************************************
	PATH Interface Routines
*********************************************************************/

uInt32
#ifdef __STDC__
GetPATHDataMulti (uInt16 stnID, uInt16 attr, PATHDataPtr pathPtr,
		uChar * buffer)
#else
GetPATHDataMulti (stnID, attr, pathPtr, buffer)
		uInt16                  stnID;
		uInt16                  attr;
		PATHDataPtr             pathPtr;
		uChar                  *buffer;
#endif
/*********************************************************************
Function:	Get a PATH attribute.
			Some of the PATH-related attributes are contained
			within the PATH or its associated hardware and software.
			To get the most current data, GetMIBPATHAttr() calls
			GetPATHDataMulti() to retrieve the data from the system itself.
Parameters:	stnID	= station index to use.
			attr	= PATH attribute to retrieve.
			pathPtr	= this is the PATH entry in the MIB.
			buffer	= pointer to a buffer to hold the requested
						attribute value.
Input:		None.
Output:		Sets buffer to contain requested attribute.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or attribute ID not known
*********************************************************************/
{
uInt32                  result = RC_Success;	/* return value */

	/*
	*	Read the appropriate PATH attribute.
	*/
	switch (attr)
	{
	case fddiPATHRingLatency:
		/*
		* Optional attribute that is generally not supported.
		*/
		result = RC_NoParameter;
		break;

	default:
		result = RC_NoParameter;
		break;
	}

	return(result);
}

uInt32
#ifdef __STDC__
SetPATHDataMulti (uInt16 stnID, uInt16 attr, PATHDataPtr pathPtr,
	TLVParamType *p)
#else
SetPATHDataMulti (stnID, attr, pathPtr, p)
		uInt16                  stnID;
		uInt16                  attr;
		PATHDataPtr             pathPtr;
		TLVParamType			*p;
#endif
/*********************************************************************
Function:	Set a PATH attribute.
			Many of the PATH-related attributes are contained
			within the PATH or its associated hardware and software.
			To set the data values, SetMIBPATHAttr() calls
			SetPATHDataMulti() to place the data into the
			appropriate location.
Parameters:	stnID	= index of station to use.
			attr	= PATH attribute to set.
			pathPtr	= entry in the MIB for the PATH to change.
			p		= pointer to a buffer holding the data value.
Input:		p		= attribute in TVL encoding.
Output:		Hardware or system software is set with new value.
			Additionally, the new value may be written to non-volatile
			memory. Some of the PATH attributes listed are managed by
			the MIB database and are passed to this routine only to be
			stored in non-volatile memory if desired.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
				RC_Success
					attribute changed successfully
				RC_NoParameter
					attribute not supported
				RC_OutOfRange
					value given is out of bounds
*********************************************************************/
{
uInt32                  result = RC_Success;

	switch (attr)
	{
	case fddiPATHT_Rmode:
	case fddiPATHTVXLowerBound:
	case fddiPATHT_MaxLowerBound:
	case fddiPATHMaxT_Req:
		/*
		* These attributes are managed by the MIB database.
		* They are passed to this routine for storage in
		* non-volatile memory.
		*/
		result = RC_Success;
		break;

	case fddiPATHSbaPayload:
		break;

	case fddiPATHSbaOverhead:
		break;

	case fddiPATHSbaAvailable:
		break;

	case fddiPATHVendorAttrib:
                /* check which vendor */
                if ((p->PATHVENDORID & VENDOR_MASK) == fddiXDI)
                {
                        /* store XDI attribute values */

                        /*
                         * Currently, no PATH attributes are replaceable.
                         */
                }

		/* handle other vendors with else-ifs */

		break;

	default:
		result = RC_NoParameter;
		break;
	}

	return(result);
}


/*********************************************************************
	PORT Interface Routines
*********************************************************************/

uInt32
#ifdef __STDC__
GetPORTDataMulti (uInt16 stnID, uInt16 attr, PORTDataPtr portPtr,
		uChar * buffer)
#else
GetPORTDataMulti (stnID, attr, portPtr, buffer)
		uInt16                  stnID;
		uInt16                  attr;
		PORTDataPtr             portPtr;
		uChar                  *buffer;
#endif
/*********************************************************************
Function:	Get a PORT attribute.
			Many of the PORT-related attributes are contained
			within the PORT or its associated hardware and software.
			To get the most current data, GetMIBPORTAttr() calls
			GetPORTDataMulti() to retrieve the data from the system itself.
Parameters:	stnID	= index of station to use.
			attr	= PORT attribute to retrieve.
			portPtr	= entry in the MIB for this PORT.
			buffer	= pointer to a buffer to hold the requested attribute.
Input:		None.
Output:		Sets buffer to contain requested attribute.
Return:		 The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or not known
*********************************************************************/
{
uInt32                  result = RC_Success;
uInt16			ls;

	/*
	*	Read the appropriate PORT attribute.
	*/
	switch (attr)
	{
        case fddiPORTPC_LS:
		/*
		* DO NOT use ReadLineState() here. Read the line state
		* directly from the PHY and convert to the appropriate
		* MIB value (see fddihdr.h, LS_qls through LS_nls).
		*/
		/*
		* We CAN use the ReadLineState() function in the High-End
		* case since it only returns the current line state.
		* Conversion is needed to the way the MIB understands
		* line states.
		*/
		ls = ReadLineStateMulti(stnID, portPtr->baseData.xdi.xdiIndex);
		switch (ls)
		{
		case MLS:
			((TLV16BitType *) buffer)->data = LS_mls;
			break;
		case SILS:
			((TLV16BitType *) buffer)->data = LS_ils;
			break;
		case HLS:
			((TLV16BitType *) buffer)->data = LS_hls;
			break;
		case QLS:
			((TLV16BitType *) buffer)->data = LS_qls;
			break;
		case ALS:
			((TLV16BitType *) buffer)->data = LS_pdr;
			break;
		default:
			((TLV16BitType *) buffer)->data = LS_lsu;
			break;
		}
		break;

	default:
		result = RC_NoParameter;
		break;
	}

	return(result);
}

uInt32
#ifdef __STDC__
SetPORTDataMulti (uInt16 stnID, uInt16 attr, PORTDataPtr portPtr,
	TLVParamType *p)
#else
SetPORTDataMulti (stnID, attr, portPtr, p)
		uInt16                  stnID;
		uInt16                  attr;
		PORTDataPtr             portPtr;
		TLVParamType			*p;
#endif
/*********************************************************************
Function:	Set an PORT attribute.
			Although ALL PORT attributes are managed through the MIB
			database, an interface routine is required for storing
			the PORT attribute values into a non-volatile memory
			location. This routine DOES NOT change the MIB itself.
			It only provides a place for storing PORT attributes
			permanently in the system.
Parameters:	stnID	= index of station to use.
			attr	= PORT attribute to set.
			portPtr	= PORT element that has changed.
			p		= pointer to a buffer holding the data value.
Input:		p		= attribute in TLV encoding.
Output:		Non-volatile memory is changed to store the new value. This
			action is optional and all changes to standard and XDI PORT
			attributes may be ignored by this routine.
Return:		This routine returns RC_Success.
*********************************************************************/
{

	switch (attr)
	{
	case fddiPORTConnectionPolicies:
	case fddiPORTRequestedPaths:
	case fddiPORTLer_Cutoff:
	case fddiPORTLer_Alarm:
	case fddiPORTMACLoop_Time:
	case fddiPORTMaint_LS:
		break;

	case fddiPORTVendorAttrib:
                /* check which vendor */
                if ((p->PORTVENDORID & VENDOR_MASK) == fddiXDI)
                {
                        /* store XDI attribute values */
                        if (p->PORTVENDORID == xdiPORTLocalPath)
                        {
                        }
                }

		/* handle other vendors with else-ifs */

		break;
	}

	return(RC_Success);
}
#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) if7_mibvenif.c if7_v3.1.1:cset.0167:7:7 Fri Jan 21 10:52:37 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		THIS FILE IS TO BE PORTED BY THE IMPLEMENTER.

		Vendor MIB Attribute Processing Module

		File:			if7_mibvenif.c
		Created:		04/30/92

		Version:		if7_v3.1.1		Fri Jan 21 10:52:37 1994
		Last Modified:	cset.0167		01/11/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module provides access to the FDDI MIB Vendor Attributes
		specified by the implementer.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_VSO_OTHER	include code for other vendor objects support.

		If the constant listed above is defined, then the
		associated portion of code is included in the compilation.

		Modification History:
		*** Created for SMT 7.3 ***
*********************************************************************/

/*
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"mibdefs.h"
#include	"mibvend.h"
#include	"mibtypes.h"
#include	"mibhdr.h"
#include	"mibglbl.h"
#include	"smtmsg.h"
#include	"mibproto.h"
*/


/*********************************************************************
	Get Vendor Attribute Routines
*********************************************************************/

uInt32
#ifdef __STDC__
GetVendorSMTAttrMulti (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	SMTBaseAttrType * smtPtr)
#else
GetVendorSMTAttrMulti (stnID, bufSize, p, smtPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		SMTBaseAttrType *smtPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get a Vendor fddiSMT attribute.
Parameter:	stnID	= index of station to use.
		bufSize	= maximum length of buffer bufData  measured in chars.
		p	= pointer to buffer containing the
			  requested attribute information and used to
			  hold attribute value returned.
		smtPtr	= address of SMT attribute data.
Input:		p	= contains the requested attribute ID and object
			  index (if required) using the MIB TLV encoding
			  for a Get Request (see SMT standard).
Output:		p	= holds the attribute value requested using the
			  MIB TLV encoding specified in the SMT standard.
Return:		The value returned corresponds to the results codes used
		by the Parameter Manager Protocol (see 8.4.2.2.1):
		RC_Success
			attribute retrieved successfully
		RC_NoParameter
			attribute not supported or unrecognized
		RC_IllegalOperation
			this operation is not valid for the requested attr
		RC_BUFFER_ERROR
			if the buffer is not large enough to hold the
			requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
		insure correct attribute values and a clean response buffer.
		If no vendor-specific attribute are supported, then this
		routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */

	/*
	 * Check if buffer is minimum size. Header + data Length is the
	 * same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (sizeof(TLVHdrType) + sizeof(TLV32BitType)
		       + sizeof(TLV8BitType)))
		return(RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size by header amount and vendor ID.
	 */
	bufSize -= (sizeof(TLVHdrType) + sizeof(TLV32BitType));

	/*
	 * Length is the same for 8, 16, and 32 bit data types
	 * which is the most common size.
	 */
	p->paramLen = sizeof (TLV8BitType);

	/*
	 * Get value from MIB.
	 * 
	 * if-else is used here because a switch() statement requires an int.
	 * The vendor IDs will be 32-bits which may not be ints on some
	 * machines.
	 */

	/*
	 * Last else statement is default for unknown parameters.
	 */
	result = RC_NoParameter;

	/*
	 * Adjust parameter length to include vendor attr ID.
	 */
	if (result == RC_Success)
		p->paramLen += sizeof(TLV32BitType);

	return(result);
}

uInt32
#ifdef __STDC__
GetVendorMACAttrMulti (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	MACDataPtr macPtr)
#else
GetVendorMACAttrMulti (stnID, bufSize, p, macPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		MACDataPtr       macPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get a vendor fddiMAC attribute.
Parameter:	stnID	= index of station to use.
			bufSize	= maximum length of buffer bufData measured in chars.
			p		= pointer to buffer containing the requested
						attribute information and used to hold attribute
						value returned.
			macPtr	= pointer to MAC's attribute structure in the MIB.
Input:		p		= contains the requested attribute ID and object
						index (if required) using the MIB TLV encoding
						for a Get Request (see SMT standard).
Output:		p		= holds the attribute value requested using the
						MIB TLV encoding specified in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or unrecognized
				RC_IllegalOperation
					this operation is not valid for the requested attr
				RC_BUFFER_ERROR
					if the buffer is not large enough to hold the
					requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
			If no vendor-specific attribute are supported, then this
			routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */

	/*
	 * Check if buffer is minimum size. Header + index + vendor ID + data
	 * Length is the same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (sizeof(TLVHdrType) + sizeof(TLV16BitType)
			+ sizeof(TLV32BitType) + sizeof(TLV8BitType)))
		return(RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size by header, index and vendor ID length.
	 */
	bufSize -= (sizeof(TLVHdrType) + sizeof(TLV16BitType)
		+ sizeof(TLV32BitType));

	/*
	 * Set length to most common size (index added later).
	 */
	p->paramLen = sizeof(TLV16BitType);

	/*
	 * Get value from MIB.
	 * 
	 * if-else is used here because a switch() statement requires an int. The
	 * vendor IDs will be 32-bits which may not be ints on some machines.
	 */

	/*
	 * Last else statement is default for unknown parameters.
	 */
	result = RC_NoParameter;

	/*
	 * Adjust parameter length to include entity index and vendor ID.
	 */
	if (result == RC_Success)
		p->paramLen += (sizeof(TLV16BitType) + sizeof(TLV32BitType));

	return(result);
}

uInt32
#ifdef __STDC__
GetVendorPATHAttrMulti (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	PATHDataPtr pathPtr)
#else
GetVendorPATHAttrMulti (stnID, bufSize, p, pathPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		PATHDataPtr      pathPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get a vendor fddiPATH attribute.
Parameter:	stnID	= index of station to use.
			bufSize	= maximum length of buffer bufData measured in chars.
			p		= pointer to buffer containing the requested
						attribute information and used to hold attribute
						value returned.
			pathPtr	= pointer to PATH's attribute structure in the MIB.
Input:		p		= contains the requested attribute ID and object
						index (if required) using the MIB TLV encoding
						for a Get Request (see SMT standard).
Output:		p		= holds the attribute value requested using the
						MIB TLV encoding specified in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or unrecognized
				RC_IllegalOperation
					this operation is not valid for the requested attr
				RC_BUFFER_ERROR
					if the buffer is not large enough to hold the
					requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
			If no vendor-specific attribute are supported, then this
			routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */

	/*
	 * Check if buffer is minimum size. Header + index + vendor ID + data
	 * Length is the same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (sizeof(TLVHdrType) + sizeof(TLV16BitType)
			+ sizeof(TLV32BitType) + sizeof(TLV8BitType)))
		return(RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size by header, index and vendor ID length.
	 */
	bufSize -= (sizeof(TLVHdrType) + sizeof(TLV16BitType)
		+ sizeof(TLV32BitType));

	/*
	 * Set length to most common size (index added later).
	 */
	p->paramLen = sizeof(TLV16BitType);

	/*
	 * Get value from MIB.
	 * 
	 * if-else is used here because a switch() statement requires an int. The
	 * vendor IDs will be 32-bits which may not be ints on some machines.
	 */

	/*
	 * Last else statement is default for unknown parameters.
	 */
	result = RC_NoParameter;

	/*
	 * Adjust parameter length to include entity index and vendor ID.
	 */
	if (result == RC_Success)
		p->paramLen += (sizeof(TLV16BitType) + sizeof(TLV32BitType));

	return(result);
}

uInt32
#ifdef __STDC__
GetVendorPORTAttrMulti (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	PORTDataPtr portPtr)
#else
GetVendorPORTAttrMulti (stnID, bufSize, p, portPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		PORTDataPtr      portPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get a vendor fddiPORT attribute.
Parameter:	stnID	= index of station to use.
			bufSize	= maximum length of buffer bufData measured in chars.
			p		= pointer to buffer containing the requested
						attribute information and used to hold attribute
						value returned.
			portPtr	= pointer to PORT's attribute structure in the MIB.
Input:		p		= contains the requested attribute ID and object
						index (if required) using the MIB TLV encoding
						for a Get Request (see SMT standard).
Output:		p		= holds the attribute value requested using the
						MIB TLV encoding specified in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or unrecognized
				RC_IllegalOperation
					this operation is not valid for the requested attr
				RC_BUFFER_ERROR
					if the buffer is not large enough to hold the
					requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
			If no vendor-specific attribute are supported, then this
			routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */

	/*
	 * Check if buffer is minimum size. Header + index + data Length is the
	 * same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (sizeof(TLVHdrType) + sizeof(TLV16BitType)
			+ sizeof(TLV32BitType) + sizeof(TLV8BitType)))
		return(RC_BUFFER_ERROR);

	/*
	 * Reduce size of buffer by header, vendor ID and index.
	 */
	bufSize -= (sizeof(TLVHdrType) + sizeof(TLV16BitType)
		+ sizeof(TLV32BitType));

	/*
	 * Set length to most common size.
	 */
	p->paramLen = sizeof(TLV16BitType);

	/*
	 * Get value from MIB.
	 * 
	 * if-else is used here because a switch() statement requires an int. The
	 * vendor IDs will be 32-bits which may not be ints on some machines.
	 */

	/*
	 * Last else statement is default for unknown parameters.
	 */
	result = RC_NoParameter;

	return(result);
}


/*********************************************************************
	Change Vendor Attribute Routines
*********************************************************************/

uInt32
#ifdef __STDC__
ChangeVendorSMTAttrMulti (uInt16 stnID, TLVParamType * p,
	SMTBaseAttrType * smtPtr)
#else
ChangeVendorSMTAttrMulti (stnID, p, smtPtr)
		uInt16           stnID;
		TLVParamType    *p;
		SMTBaseAttrType *smtPtr;
#endif
/*********************************************************************
Function:	MIB access routine to change a Vendor SMT attribute.
Parameter:	stnID	= index of station to use.
			p		= pointer to buffer containing the new attribute
						information.
			smtPtr	= address of SMT attribute data.
Input:		p		= contains the attribute ID using the MIB TLV
						encoding for a Change Request (see SMT standard).
Output:		p		= holds the attribute value set using MIB TLV
						encoding specified in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or unrecognized
				RC_OutOfRange
					value given is out of bounds
				RC_IllegalOperation
					this operation is not valid for the requested attr
				RC_BUFFER_ERROR
					if the buffer is not large enough to hold the
					requested attribute
Notes:		Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
			If no vendor-specific attribute are supported, then this
			routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_NoParameter;	/* return code */

	return(result);
}

uInt32
#ifdef __STDC__
ChangeVendorMACAttrMulti (uInt16 stnID, TLVParamType * p, MACDataPtr macPtr)
#else
ChangeVendorMACAttrMulti (stnID, p, macPtr)
		uInt16           stnID;
		TLVParamType    *p;
		MACDataPtr       macPtr;
#endif
/*********************************************************************
Function:	MIB access routine to change a Vendor MAC attribute.
Parameter:	stnID	= index of station to use.
			p		= pointer to buffer containing the new attribute
						information.
			macPtr	= address of MAC attribute data.
Input:		p		= contains the attribute ID using the MIB TLV
						encoding for a Change Request (see SMT standard).
Output:		p	= holds the attribute value set using MIB TLV
						encoding specified in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or unrecognized
				RC_OutOfRange
					value given is out of bounds
				RC_IllegalOperation
					this operation is not valid for the requested attr
				RC_BUFFER_ERROR
					if the buffer is not large enough to hold the
					requested attribute
Notes:		Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
			If no vendor-specific attribute are supported, then this
			routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_NoParameter;	/* return code */

	return(result);
}

uInt32
#ifdef __STDC__
ChangeVendorPATHAttrMulti (uInt16 stnID, TLVParamType * p, PATHDataPtr pathPtr)
#else
ChangeVendorPATHAttrMulti (stnID, p, pathPtr)
		uInt16           stnID;
		TLVParamType    *p;
		PATHDataPtr      pathPtr;
#endif
/*********************************************************************
Function:	MIB access routine to change a Vendor PATH attribute.
Parameter:	stnID	= index of station to use.
			p		= pointer to buffer containing the new attribute
						information.
			pathPtr = address of PATH attribute data.
Input:		p		= contains the attribute ID using the MIB TLV
						encoding for a Change Request (see SMT standard).
Output:	 	p		= holds the attribute value set using MIB TLV
						encoding specified in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or unrecognized
				RC_OutOfRange
					value given is out of bounds
				RC_IllegalOperation
					this operation is not valid for the requested attr
				RC_BUFFER_ERROR
					if the buffer is not large enough to hold the
					requested attribute
Notes:		Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
			If no vendor-specific attribute are supported, then this
			routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_NoParameter;	/* return code */

	return(result);
}

uInt32
#ifdef __STDC__
ChangeVendorPORTAttrMulti (uInt16 stnID, TLVParamType * p, PORTDataPtr portPtr)
#else
ChangeVendorPORTAttrMulti (stnID, p, portPtr)
		uInt16           stnID;
		TLVParamType    *p;
		PORTDataPtr      portPtr;
#endif
/*********************************************************************
Function:	MIB access routine to change a Vendor PORT attribute.
Parameter:	stnID	= index of station to use.
			p		= pointer to buffer containing the new attribute
						information.
			portPtr = address of PORT attribute data.
Input:		p		= contains the attribute ID using the MIB TLV
						encoding for a Change Request (see SMT standard).
Output:	 	p		= holds the attribute value set using MIB TLV
						encoding specified in the SMT standard.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
				RC_Success
					attribute retrieved successfully
				RC_NoParameter
					attribute not supported or unrecognized
				RC_OutOfRange
					value given is out of bounds
				RC_IllegalOperation
					this operation is not valid for the requested attr
				RC_BUFFER_ERROR
					if the buffer is not large enough to hold the
					requested attribute
Notes:		Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
			If no vendor-specific attribute are supported, then this
			routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_NoParameter;	/* return code */

	return(result);
}


/*********************************************************************
	Vendor Notification Routines
*********************************************************************/

void
#ifdef __STDC__
ProcessVendorMIBEvent (SMTMessage * msg, MIBNotificationType * notice)
#else
ProcessVendorMIBEvent (msg, notice)
		SMTMessage      *msg;
		MIBNotificationType *notice;
#endif
/*********************************************************************
Function:	Process event identifiers not handled by SendSMTEvent()
			in mibmsg.c. The SendSMTEvent() routine builds a
			notification report to be sent to FSP and the
			MAP. If SendSMTEvent() does not understand the event
			identifier, it calls this routine to complete the
			message information.
Parameters:	msg		= address of SMT message buffer.
			notice	= address of message notification buffer.
Input:		msg		= contains initial message information:
					msg->typeInfo	= event identifier passed to SendSMTEvent()
					msg->entity		= object identfier
					msg->p1.ptr		= points to notice
Output:		msg	= should remain untouched. The value of msg->len1
					will be calculated based on the length
					specified in param->paramLen.
			notice	= contains the notification event report:
					notice->type	= EC_TYPE_EVENT for event notices
									  EC_TYPE_CONDITION for conditions
									  0 for do not send message
					notice->state	= current state if this notificiation
										is for a condition
					notice->info	= any 4 octets of additional data
Return:		No value is returned through this function.
Notes:		Any type of processing may be done here. This routine may
			even handle sending the message itself. If this is
			done or no message should be sent, then set the value
			of notice->type to 0.
*********************************************************************/
{
	return;
}

void
#ifdef __STDC__
DetectVendorNotificationsMulti (uInt16 stnID, MIBType * mibPtr)
#else
DetectVendorNotificationsMulti (stnID, mibPtr)
		uInt16           stnID;
		MIBType         *mibPtr;
#endif
/*********************************************************************
Function:	Detect vendor-specific notifications on a periodic
			interval. This routine will usually look for conditions
			like the Frame Error Count conditions.
Parameters:	stnID	= index of station to use.
			mibPtr	= pointer to this station's MIB database.
Input:		mibPtr	= contains current MIB information.
Output:		mibPtr	= any condition flags or event information
						associated with the vendor-specific
						notifications may be updated.
			The routine SendSMTEvent() will be called to inform
			FSP and the MAP of the notification.
Return:		No value is returned through this function.
Notes:		Any type of processing may be done here.
*********************************************************************/
{
	return;
}


/*********************************************************************
*	Vendor-specific Object Operation Routines
*
*	The following routines are only included when an implementer-defined
*	vendor-specific object is supported by the MIB. These routines
*	provide the entry point for performing the Get and Set operations
*	on the vendor-specific object's attributes.
*********************************************************************/

#ifdef	XM_USE_VSO_OTHER

uInt32
#ifdef __STDC__
InitVSOMulti (uInt16 stnID)
#else
InitVSOMulti (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	This routine initializes the database for the implementer-
			defined vendor-specific objects by clearing the index lookup
			tables and inUse flags.
Parameters:	stnID	= index of station to initialize.
Input:		mib		= MIB database for this station.
Output:		mib		= all VSO data within MIB is set to starting state.
Return:		0		if successful.
Notes:		Refer to InitMASICObjectMulti() for an example implementation
			of this routine. Also, this routine is responsible for setting
			the vsoData pointer in the MIB.
Modification History:
*********************************************************************/
{
MIBType         *mibPtr;				/* MIB to use */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* clear VSO tables */

	/* set mibPtr->vsoData to the object data base */

	return (0);
}

uInt32
#ifdef __STDC__
GetMIBVSOAttrMulti (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	Void_p vsoPtr)
#else
GetMIBVSOAttrMulti (stnID, bufSize, p, vsoPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		Void_p           vsoPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get a VSO object attribute.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer p measured in chars.
			p			= pointer to buffer containing the
							requested attribute information
							and used to hold attribute value
							returned.
			vsoPtr		= pointer to VSO's attribute structure in
							the MIB.
Input:		p			= contains the requested attribute ID and
							object index using the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		p			= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document and the attribute contents
							defined in the XLNT Manager MIB appendix.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
Modification History:
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */

	/*
	 * Check if buffer is minimum size. For XLNT Designs' objects, this size
	 * = Header (4) + VSO index (4) + object header [attrib type + object
	 * index] (4) + value. Min value length is the same for 8, 16, and 32 bit
	 * data types.
	 */
	if (bufSize < (SIZE_TLVHdrType + (3 * SIZE_DEFAULT)))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size to get size of data portion.
	 */
	bufSize -= (SIZE_TLVHdrType + (2 * SIZE_DEFAULT));

	/*
	 * Set length to object ID + object header + most common data size.
	 */
	p->paramLen = 3 * SIZE_DEFAULT;

	/*
	 * Get value from MIB.
	 */
	switch (p->VSOTYPE)
	{
		/*
		 * Invalid Types
		 */
	case 0:							/**** PLACE HOLDER ****/
		result = RC_IllegalOperation;
		break;

		/*
		 * Base Attributes
		 */
	default:
		/*
		 * The following attributes are only available when the underlying
		 * hardware is available.
		 */
		if ( /* vsoPtr->baseData.HardwarePresent */ 0)
		{
			/* get info from local source */
		}

		else
		{
			/* hardware not present */
			result = RC_NoParameter;
		}
	}

	return (result);
}

uInt32
#ifdef __STDC__
ChangeMIBVSOAttrMulti (uInt16 stnID, Void_p bufData)
#else
ChangeMIBVSOAttrMulti (stnID, bufData)
		uInt16           stnID;
		Void_p           bufData;
#endif
/*********************************************************************
Function:	MIB access routine to change a VSO object attribute.
Parameters:	stnID		= index of station to use.
			bufData		= pointer to buffer containing the
							new attribute information and used to hold
							attribute value returned.
Input:		bufData		= contains the new attribute value using
							the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
			RC_Success
				attribute changed successfully
			RC_NoParameter
				attribute or object ID not supported
			RC_LengthError
				parameter length specified is not valid
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				parameter may not be changed
*********************************************************************/
{
TLVParamType    *p;						/* parameter structure */
uInt32           result = RC_Success;	/* return value */
MIBType         *mibPtr;				/* MIB database */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Get format for buffer.
	 */
	p = (TLVParamType *) bufData;

	/* check if resource ID is in the table */

	/* if it is */
	/* map resource index to array index */
	/* get pointer */
	/* change value */

	return (result);
}

uInt32
#ifdef __STDC__
GetVSOValueMulti (uInt16 stnID, uInt16 bufSize, Void_p bufData,
	uInt16 * bufUsed)
#else
GetVSOValueMulti (stnID, bufSize, bufData, bufUsed)
		uInt16           stnID;
		uInt16           bufSize;
		Void_p           bufData;
		uInt16          *bufUsed;
#endif
/*********************************************************************
Function:	Get the value for an attribute in an implementer-defined
			vendor-specific object.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information.
			bufUsed		= pointer to buffer to hold the length
							of all attributes returned.
Input:		None.
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
			bufUsed		= length, in bytes, of the TLV encoding of
							all attributes returned in bufData.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute. In this case, as much as
				has been processed is returned.
Notes:		Implementers may provide any functionality to implement this
			routines. XLNT Designs recommends basing this routine on
			GetMASICAttr() in mibmasic.c.
Modification History:
*********************************************************************/
{
uInt32           result;				/* return value */

	result = RC_NoParameter;
	return (result);
}

#endif							/* XM_USE_VSO_OTHER */
#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_csppcm.c xm7_v3.1.1:cset.0166:13:13 Fri Jan
21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
                Connection Services Process Module

                PCM State Machine

                File:                   xm7_csppcm.c
                Created:                12/01/89

                Version:                xm7_v3.1.1              Fri Jan 21 10:51:2
9 1994
                Last Modified:  cset.0166               01/10/94

                Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

                This module implements the PCM state machine listed in the
                ANSI X3T9.5 SMT document.

                With release 7.3, this file contains the following conditional
                compilation options:

                        XM_NO_M_PORT    do not include code for M port CFM.

                If the constant listed above is defined, then the
                associated portion of code is NOT included in the compilation.
                This "reverse option" allows the code to remain backward
                compatible with existing interface files and build procedures.

                Modification History:
        *** SMT 7.3 ***
*********************************************************************/

/*
#include        "smtdefs.h"
#include        "smttypes.h"
#include        "smterror.h"
#include        "smtmacro.h"
#include        "fddihdr.h"
#include        "smtmsg.h"
#include        "cspmacro.h"
#include        "csphdr.h"
#include        "cspglbl.h"
#include        "mibdefs.h"
#include        "cspproto.h"
#include        "multistn.h"
#include        "cspgrace.h"
*/


/*********************************************************************
                PCM State Machine
*********************************************************************/

void
#ifdef __STDC__
SignalPCM (uInt16 stnID, uInt16 sigType, uInt16 sigEntity, uInt32 sigData)
#else
SignalPCM (stnID, sigType, sigEntity, sigData)
                uInt16           stnID;
                uInt16           sigType;
                uInt16           sigEntity;
                uInt32           sigData;
#endif
/*********************************************************************
Function:       Process a PCM signal.
		The PCM State machine is all executed on the board's
			microcore state machine.
Parameters:     stnID           = index of station to use.
                        sigType         = the signal ID.
                        sigEntity       = entity (PHY) to process.
                        sigData         = any data associated with the signal.
Input:          Uses portData.
Output:         Changes portData.
Return:         No value returned.
Notes:
Modification History:
*********************************************************************/
{
PORTDataPtr		portPtr;        /* Port element to use */
hwidbtype		*idb;		/* HW IDB pointer */
uint			phy;		/* phy ID as understood by ucode */

        /* if Port is not present, then ignore the signal */
        if (!(CSPDATA (stnID, portData[sigEntity].hwPresent)))
                return;

	/* Initialize Local Variables */
        portPtr = &(MIBDATA (stnID, mib->portBase[sigEntity]));
	idb = STADATA (stnID, hwIDB[0]);
	phy = (sigEntity == 1) ? FCI_FDDI_PHY_B : 0;

        /*
         * For more efficient processing, some signals are preprocessed here to
         * avoid duplicate code throughout the state machine.
         */
        switch (sigType)
        {

        /*
         * Handle general transitions. PC_Start and PC_Stop signals are
         * processed from all states except PC9:MAINT.
         */
        case SIG_PC_Start:
		if ((portPtr->hwData.PCMState != PCM_pc9) &&
		    (((phytype *)(idb->phy[sigEntity]))->connect)) {
                    /* PC(01a) - PC(81a) */
		    /* Set programming mode */

		    if (fddi_debug)
			buginf("\n%s: In SIG_PC_Start", idb->hw_namestring);

                    cbus_daughter_cmd(idb, FCI_FDDI_SET_CMT_MODE,
				      CMT_MODE_CMT);
		    /* Program TL_Min */
                    cbus_daughter_cmd(idb, FCI_FDDI_SET_TL_MIN,
                        (STADATA (idb->fddi_stn_id, TLMin) ?
		        (STADATA (idb->fddi_stn_id, TLMin)/20) : 0));
		    /* Program TVal bits */
                    cbus_daughter_cmd(idb, FCI_FDDI_SET_TVAL,
		    (idb->phy[sigEntity])->tval | phy);
		    /* Start Phy */
                    cbus_daughter_cmd(idb, FCI_FDDI_KICK_PHY, FCI_FDDI_START_PHY | phy);
		    portPtr->hwData.PCMState = PCM_pc1;
		    }
                break;

        case SIG_PC_Stop:
	        if (fddi_debug)
	            buginf("\nIn SIG_PC_Stop");
                /* PC(00) - PC(80) */
                /* Stop Phy */
            cbus_daughter_cmd(idb, FCI_FDDI_KICK_PHY, FCI_FDDI_STOP_PHY | phy);
            SendSignalMulti(idb->fddi_stn_id, SIG_CF_Join, sigEntity, (uInt32) CLEAR);
			portPtr->hwData.PCMState = PCM_pc0;
            break;

        case SIG_PC_Enable:
	        if (fddi_debug)
	            buginf("\nIn SIG_PC_Enable");
                /* PC(90) */
			portPtr->hwData.PCMState = PCM_pc0;
            break;

        case SIG_PC_Disable:
	        if (fddi_debug)
	            buginf("\nIn SIG_PC_Disable");
                /* stop PCM first */
                /* Stop Phy */
            cbus_daughter_cmd(idb, FCI_FDDI_KICK_PHY, FCI_FDDI_STOP_PHY | phy);
            SendSignalMulti(idb->fddi_stn_id, SIG_CF_Join, sigEntity, (uInt32) CLEAR);
			portPtr->hwData.PCMState = PCM_pc9;
            break;

        case SIG_PC_Trace:
                if (fddi_debug)
                    buginf("\nIn SIG_PC_Trace");
                /* PC(82) */
                /* Start tracing - tracing is done in the microcode.	*/
				/* The only functionality used from the XDI code is	*/
				/* Trace timing.					*/
				break;

        case SIG_PC_LEM_Fail:

        /*Print out LEM fail message and start monitoring LEMs again*/

            if (fddi_debug) {
                buginf("\nIn SIG_PC_LEM_Fail");
            	if (sigEntity)
            	    buginf("\nLinkErrorRate on PHYB exceeded LinkErrorCutoff.");
            	else
            	    buginf("\nLinkErrorRate on PHYA exceeded LinkErrorCutoff.");
			}

        /*
         * Hardware revs below 2.4 on the FIP and the old appliques on
         * AGS+ are funky with respect to lem count.
         * They screw up the lem counting mechanism.
         * Hence, we keep the above Link error messages under debug control.
         */

            SendSignalMulti (stnID, SIG_LEM_Start, sigEntity , (uInt32) 0);
            break;

	/*
	 * Ignore all other signals (not used in the
	 * High-End microcode supported platforms).
	 */
        default:
		break;

        }

        return;
}
