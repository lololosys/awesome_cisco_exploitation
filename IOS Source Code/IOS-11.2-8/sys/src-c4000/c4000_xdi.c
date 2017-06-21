/* $Id: c4000_xdi.c,v 3.7.28.4 1996/08/28 13:12:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_xdi.c,v $
 *------------------------------------------------------------------
 * Cisco Low-End platforms XDI interface Module.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module includes all or part of the XDI interface files
 * which contain platform specific code:
 *
 *              cspintif.c,
 *              cspsysif.c,
 *              csplocal.c,
 *              fbmfrmif.c,
 *              mibsysif.c
 *
 *------------------------------------------------------------------
 * $Log: c4000_xdi.c,v $
 * Revision 3.7.28.4  1996/08/28  13:12:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.28.3  1996/07/23  13:28:32  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.7.28.2  1996/05/27  05:52:45  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Branch: California_branch
 *
 * Revision 3.7.28.1  1996/03/18  22:00:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/13  02:00:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/09  05:12:44  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.7.2.1  1996/01/24  23:02:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/01/22  07:15:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/05  10:18:18  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.5  1996/01/05  04:57:00  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.4  1995/12/19  00:41:02  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.3  1995/11/17  18:45:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/07  05:42:19  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.2  1995/07/18  06:48:48  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 22:56:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************************
                                Includes
*********************************************************************/
#include	"master.h"
#include	"logger.h"
#include        "../if/msg_datalink.c"	/* Not a typo, see logger.h */
#include	"../les/msg_fddi.c"	/* Not a typo, see logger.h */
#include	"../xdi/cschdrsif.h"
#include	"../../boot/cpu.h"
#include	"../if/fddi.h"
#include	"../os/old_timer_callbacks.h"

#include	"../xdi/xdihdrsif.h"
#include	"../xdi/fddihdr.h"
#include	"../xdi/smtmsg.h"
#include	"../xdi/csphdr.h"
#include	"../xdi/cspglbl.h"
#include	"../xdi/cspproto.h"
#include	"../xdi/mibglbl.h"
#include	"../xdi/fbmframe.h"
#include	"../xdi/fbmhdr.h"
#include	"../xdi/fbmglbl.h"

#include	"../if/if_fddi.h"
#include	"../if/if_fddi_public.h"
#include	"../if/rif.h"
#include        "../if/rif_inline.h"
#include        "c4000_fddi_public.h"
#include	"c4000_xdi_public.h"
#include        "../if/network.h"

/*********************************************************************
	Variables
*********************************************************************/
static timer_t			*CSPTimerPtr;	      /* CSP Timer struc ptr */
static Flag		RTTimerStarted = FALSE;	/* RT Timers started flag */
ulong			CSPInterruptsDisabled = 0;/* CSP Intr Dis for NMI */

extern ulong		tvx_table[];


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

void
#ifdef __STDC__
CSPInterruptHandler (uInt16 stnID)
#else
CSPInterruptHandler (stnID)
		uInt16		stnID;
#endif
/*********************************************************************
Function:	This routine may do any number of things depending
			upon the implementation. Generally, this routine
			invokes CSP by calling ProcessCSP(). This routine
			may be called as an ISR or as the entry point from
			an executive scheduler.
Parameters:	Implementor defined.
Input:		Implementor defined.
Output:		Implementor defined.
Return:		None.
*********************************************************************/
{
	leveltype	SR;		/* processor intr status */
	uInt16		macID;		/* MAC index */
	uInt16		portID;		/* Port index */

	/*
	* Reenable interrupts above CMT level
	* (needed to get current state for ProcessCSP() )
	*/
	MDisableCSPInterrupts(&SR);

	/* Handle PHY state changes & errors */
	for (portID=0; portID<PORTCFG (stnID, PortCt); portID++)
	    CSPHandlePHYInterrupts (stnID, portID);

	/* Handle MAC state changes & errors */
	for (macID=0; macID<MACCFG (stnID, MacCt); macID++)
	    CSPHandleMACInterrupts (stnID, macID);

	/* Process CSP signals (if any) */
	if (!EmptySignalQueue())
	    ProcessCSP (&SR);

	/* Restore interrupts to initial state */
	MRestoreCSPInterrupts(&SR);

	return;
}

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
	/* Start RT Timers if not done yet */
	if (!RTTimerStarted) {
	    /* Initialize RT Timers */
	    rt_timer_init ();
	    RTTimerStarted = TRUE;

	    /* Create FDDI CSP RT Timer */
	    CSPTimerPtr = create_rt_timer (csp_rt_timer_int_hand, 0, 0, 0,
					   "CSP RT Timer", TMT_ONCE);
	}

	return (0);
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
/*
* Conversion of CRSR LS index to the Line State index
* as understood by the XDI code (which is set to the
* exact mask as provided in RCRA & RCRB registers in
* order to avoid unnecessary convertions.
*/
uInt16		CRSRLSIndex2XDILSMask[] = { /* indexed by CRSR LS bits */
		    ALS,	/* PLAYER_CRSR_ALS */
		    SILS,	/* PLAYER_CRSR_ILS */
		    QLS,	/* PLAYER_CRSR_NSD */
		    0,		/* Reserved */
		    MLS,	/* PLAYER_CRSR_MLS */
		    HLS,	/* PLAYER_CRSR_HLS */
		    QLS,	/* PLAYER_CRSR_QLS */
		    0		/* PLAYER_CRSR_NLS (Not Used) */
};
/*
* Conversion of CRSR LS index into RCRA & RCRB LS masks.
* This conversion is used to disable interrupts from the
* current line state (right after it is detected), such
* that the same line state will not generate multiple
* interrupts (which may happen due to line noise).
*/
struct {
		uChar rcmra;
		uChar rcmrb;
} CRSRLSIndex2RCMRLSMask[] = { /* indexed by CRSR LS bits */
		{'\0',		  PLAYER_RCRB_ALS },	/* ALS */
		{'\0',		  PLAYER_RCRB_SILS},	/* ILS */
		{PLAYER_RCRA_QLS | PLAYER_RCRA_NSD,
				  '\0'		  },	/* NSD */
		{PLAYER_RCRA_MLS, '\0'		  },	/* MLS */
		{'\0',		  '\0'		  },	/* Reserved */
		{PLAYER_RCRA_HLS, '\0'		  },	/* HLS */
		{PLAYER_RCRA_QLS | PLAYER_RCRA_NSD,
				  '\0'		  },	/* QLS */
		{'\0',		  '\0'		  }	/* NLS (Not Used) */
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
        return (1000 * get_due_time (CSPTimerPtr));
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
        /* Set CSP RT timer */
	if (interval == 0)
	    stop_rt_timer (CSPTimerPtr);
	else
	    set_rt_timer (CSPTimerPtr, 1 + interval/1000);

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
	return (TRUE);
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
player_t	*playerRegs;
uChar		udr;

	/* Check requested state */
	udr = (bypassState == DEINSERTED) ? PLAYER_UDR_BYPASS : 0;

	/* Set bypass as requested */
	playerRegs = STADATA (stnID, portRegs[0]);
	playerRegs->udr = (playerRegs->udr & ~PLAYER_UDR_BYPASS) | udr;
	if ((playerRegs = STADATA (stnID, portRegs[1])))
	    playerRegs->udr = (playerRegs->udr & ~PLAYER_UDR_BYPASS) | udr;

	return;
}

void
#ifdef __STDC__
ConfigureMasterMulti (uInt16 stnID, CSPPORTInfoType * portPtr, uInt16 cfmState)
#else
ConfigureMasterMulti (stnID, portPtr, cfmState)
		uInt16                  stnID;
		CSPPORTInfoType        *portPtr;
		uInt16                  cfmState;
#endif
/*********************************************************************
Function:	Set the configuration hardware for the specified
			M port to the CFM configuration given.
Parameters:	stnID		= index of station to use.
			portPtr		= port element to use
			cfmState	= CFM state for hardware configuration as
							defined in the M port CFM state machine.
Input:		None.
Output:		None.
Return:		No value returned.
Notes:		This routine may be stubbed out if the software is not
			implementing a concentrator.

			Non-concatenation of Frames
			=======================
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
	return;
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
	player_t	*playerARegs, *playerBRegs;
	uChar		phyAConfig, phyBConfig, ledA, ledB;
	StnDCBType	*staData;
	hwidbtype	*hwidb;

	/* Initialize local variables */
	staData = &STAINST (stnID);
	playerARegs = staData->portRegs[0];
	playerBRegs = staData->portRegs[1];
	hwidb = staData->hwIDB[0];
	phyAConfig = phyBConfig = ledA = ledB = 0;

	/* Program Data Path according to new configuration */
	switch (CF_State)
	{
	case CF_isolated:
	    phyAConfig = (PORTCFG (stnID, PortCt) == 1) ?
	        PLAYER_S_CR_ISOLATED : PLAYER_A_CR_ISOLATED;
	    phyBConfig = PLAYER_B_CR_ISOLATED;
	    break;

	case CF_wrap_a:
	case CF_c_wrap_a:
	case CF_wrap_s:
	case CF_c_wrap_s:
	    phyAConfig = PLAYER_A_CR_INSERT_P;
	    phyBConfig = PLAYER_B_CR_ISOLATED;
	    ledA = PLAYER_UDR_LED;
	    break;

	case CF_wrap_b:
	case CF_c_wrap_b:
	    phyAConfig = PLAYER_A_CR_ISOLATED;
	    phyBConfig = PLAYER_B_CR_INSERT_P;
	    ledB = PLAYER_UDR_LED;
	    break;

	case CF_thru:
	    phyAConfig = PLAYER_A_CR_INSERT_X;
	    phyBConfig = PLAYER_B_CR_INSERT_X;
	    ledA = ledB = PLAYER_UDR_LED;
	    break;

	default:
	    break;
	}

	/* Print CFM transition message */
	if (staData->prevCFState != CF_State) {
	    errmsg(&msgsym (FDDISTAT, LINK), 
		    hwidb->hw_namestring,
		    get_smt_string(&SMTMsg.cf_state, CF_State));

	    /* Report Interface State Change is appropriate */
	    if (hwidb->state != IDBS_ADMINDOWN) {
		if ((CF_State != CF_isolated) && (hwidb->state !=
						  IDBS_UP)) {
		    net_cstate(hwidb, IDBS_UP);
		    rif_update(hwidb, 0, hwidb->bit_swapped_hardware, NULL, 
			       NULL, RIF_INTERFACE);
		} else if ((CF_State == CF_isolated) && (hwidb->state == IDBS_UP))
		    net_cstate(hwidb, IDBS_DOWN);
	    }
	    /* Update CFM state history variable */
	    staData->prevCFState = CF_State;
	}

	/* Write new configuration to Ports A and B */
	playerARegs->cr = phyAConfig;
	playerARegs->udr = (playerARegs->udr & ~PLAYER_UDR_LED) | ledA;
	if (PORTCFG (stnID, PortCt) > 1) {
	    playerBRegs->cr = phyBConfig;
	    playerBRegs->udr = (playerBRegs->udr & ~PLAYER_UDR_LED) | ledB;
	}

#ifdef XX
	/* Update Synoptics 3809 LEDs.  */
	if (cookie->vendor == VENDOR_SYNOPTICS)
	    set_synoptics_leds (CF_State);
#endif

	return;
}

/*********************************************************************
	PORT Interface Routines
*********************************************************************/

void
#ifdef __STDC__
TransmitPMDMulti (uInt16 stnID, uInt16 portID, uInt16 state)
#else
TransmitPMDMulti (stnID, portID, state)
		uInt16                  stnID;
		uInt16                  portID;
		uInt16                  state;
#endif
/*********************************************************************
Function:	Implement the SM_PM_CONTROL.request primitive. The
			implementation of this primitive is not required unless the
			station supports control over the PMD.
Parameters:	stnID	= index of station to use.
			portID	= change PMD for PORT with index portID.
			state	= new state of PMD, either ENABLE or DISABLE.
Input:		None.
Output:		None.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/
{
	leveltype	SR;
	player_t	*playerRegs;

	/* Initialize local variables */
	playerRegs = (player_t *)STADATA (stnID, portRegs[portID]);

	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/* Set Transmit Enable bit to new state */
	MChangeBits (playerRegs->mr, PLAYER_MR_TE, state ? PLAYER_MR_TE : 0);

	/* Restore interrupts */
	MRestoreCSPInterrupts(&SR);

	return;
}

void
#ifdef __STDC__
SetPORTInterruptsMulti (uInt16 stnID, uInt16 portID, uInt16 state)
#else
SetPORTInterruptsMulti (stnID, portID, state)
		uInt16                  stnID;
		uInt16                  portID;
		uInt16                  state;
#endif
/*********************************************************************
Function:	Change the state of the port's interrupts. This function
			turns on/off all interrupts from the specified port.
Parameters:	stnID	= index of station to use.
			portID	= change interrupt state for PORT with index portID.
			state	= new state of interrupts.
Input:		state	= bit string indicating what interrupts are enabled:
			no bits set disables all interrupts.
			PORT_INT_QLS	enable QLS interrupt
			PORT_INT_HLS	enable HLS interrupt
			PORT_INT_MLS	enable MLS interrupt
			PORT_INT_SILS	enable SILS interrupt
			PORT_INT_NOISE	enable noise timer or NLS intr
			PORT_INT_EB	enable EB interrupt, if supported
Output:		PORT's interrupts are set as specified.
Return:		No value returned.
Notes:		Enabling NOISE will enable either a hardware noise timer if
			it is available, otherwise the Noise line state interrupt
			is enabled.
			The EB interrupt is optional. If it is available it should
			be set appropriately.
			Setting the QLS, HLS and MLS interrupts will always occur
			together for backward compatibility.
Modification History:
*********************************************************************/
{
	/*
	* In order to avoid problems with multiple interrupts from
	* a single line state, the LAST detected line state will
	* always be excluded from the interrupt enable mask.
	*/
	leveltype		SR;
	player_t		*playerRegs;
	uChar			rcmra, rcmrb;

	/* Initialize local variables */
	playerRegs = (player_t *)STADATA (stnID, portRegs[portID]);

	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/* Assemble Player interrupt masks */
	rcmra = rcmrb = 0;
	if (state & (PORT_INT_QLS | PORT_INT_HLS | PORT_INT_MLS))
	    rcmra |= (PLAYER_RCRA_NSD | PLAYER_RCRA_QLS |
		      PLAYER_RCRA_HLS | PLAYER_RCRA_MLS);
	if (state & PORT_INT_SILS)
	    rcmrb |= PLAYER_RCRB_SILS;
	if (state & PORT_INT_NOISE)
	    rcmra |= PLAYER_RCRA_NT;
	if (state & PORT_INT_EB)
	    rcmrb |= PLAYER_RCRB_EBOU;

	/* Save copy for later use when disabling repeating line states */
	PLAYERDATA (stnID, portID, cur_rcmra_mask) = rcmra;
	PLAYERDATA (stnID, portID, cur_rcmrb_mask) = rcmrb;

	/* Exclude LAST detected line state to avoid repeating intr */
	rcmra &= ~PLAYERDATA (stnID, portID, last_ls_rcmra_mask);
	rcmrb &= ~PLAYERDATA (stnID, portID, last_ls_rcmrb_mask);

	/* Write new masks */
	playerRegs->rcmra = rcmra;
	playerRegs->rcmrb = rcmrb;

	/* Restore interrupts */
	MRestoreCSPInterrupts(&SR);

	return;
}

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
	* Remember: The XDI line state encoding is set to the line
	* 	    state mask as read in RCRA and RCRB, expect
	*	    for ALS which overlaps with HLS.
	*/
	leveltype		SR;
	player_t		*playerRegs;
	uInt16			ls;
	uInt16			rcra, rcrb, crsr;

	/* Initialize local variables */
	playerRegs = (player_t *)STADATA (stnID, portRegs[portID]);

	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/* Read and Clear hardware LS history registers */
	rcra = playerRegs->rcra;
	playerRegs->rcra = 0;
	rcrb = playerRegs->rcrb;
	playerRegs->rcrb = 0;

        /* Convert NSD to QLS */
        if (rcra & PLAYER_RCRA_NSD)
            rcra |= PLAYER_RCRA_QLS;

	/* Correct ALS encoding */
	if (rcrb & PLAYER_RCRB_ALS) {
	    rcrb &= ~PLAYER_RCRB_ALS;
	    rcrb |= ALS;
	}

        /*
	* Keep linestate bits. Include previously known line state.
	* This is necessary because only new line states are recorded
	* in RCRA and RCRB.
	*
	* Noise is reported through the Noise Timer and not NLS.
	*/
        rcra &= (PLAYER_RCRA_QLS | PLAYER_RCRA_HLS |
		 PLAYER_RCRA_MLS | PLAYER_RCRA_NT);
	if (rcra & PLAYER_RCRA_NT) {
	    /* ## We should reload the Noise threshold register to
		be able to clear this interrupt bit. ## */
	    rcra &= ~PLAYER_RCRA_NT;
	    if (PLAYERDATA (stnID, portID, cur_rcmra_mask) & PLAYER_RCRA_NT)
		SendSignalMulti (stnID, SIG_Noise_Event, portID, (uInt32) 0);
	}
        rcrb &= PLAYER_RCRB_SILS;
        ls = rcra | rcrb | PLAYERDATA (stnID, portID, previousLS);

	/* Read current line state & save it for later use */
	crsr = playerRegs->crsr & PLAYER_CRSR_LS_MSK;
	if (crsr == PLAYER_CRSR_LS_ILS) {
	    WASTETIME (0); /* wait for SILS */
	    crsr = playerRegs->crsr & PLAYER_CRSR_LS_MSK;
	}
        PLAYERDATA (stnID, portID, previousLS) = CRSRLSIndex2XDILSMask[crsr];

	MRestoreCSPInterrupts(&SR);

	/* return line state history read */
	return (ls);
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
Function:	Implement the SM_PH_LINE-STATE.request primitive to cause
			the PORT to transmit a specified symbol stream.
Parameters:	stnID	= index of station to use.
			portID	= index of PORT to change.
			ls		= defined value indicating symbol stream to transmit.
Input:		None.
Output:		Sets port to transmit appropriate line state.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	* There is no need to disable CMT interrupts because this is
	* the only routine handling the Player CTSR register and
	* the PCM process is protected against multiple entrances
	*/
	player_t	*playerRegs;
	uChar		new_ls;

	/* Load local variables */
	playerRegs = (player_t *)STADATA (stnID, portRegs[portID]);

	/* Set new line state */
	switch (ls)
	{
	case QLS: /* Transmit Quiet Line State */
	    new_ls = PLAYER_CTSR_TM_QTM;
	    break;

	case HLS: /* Transmit Halt Line State */
	    new_ls = PLAYER_CTSR_TM_HTM;
	    break;

	case ILS: /* Transmit Idle Line State */
	    new_ls = PLAYER_CTSR_TM_ITM;
	    break;

	case MLS: /* Transmit Master Line State */
	    new_ls = PLAYER_CTSR_TM_MTM;
	    break;

	case PDRLS: /* Transmit PHY Request data */
	    new_ls = PLAYER_CTSR_TM_ATM;
	    break;

	default:
	    return; /* Do NOT change current Line State */
	}
	fddi_cbug_phy_req (stnID, portID, ls);
	MChangeBits (playerRegs->ctsr, PLAYER_CTSR_TM_MSK, new_ls);
	
	return;
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
	* It is assumed that the MAC is already enabled at this
	* point with interrupts for changes in RingOp enabled.
	*/
	leveltype	SR;		/* Processor intr state */
	bmac_t		*bmacRegs;	/* BMAC registers pointer */
	uChar		remr1;		/* Ring Event mask reg */
	uChar		temr0;		/* Token/Timer event mask reg */

	/* Initialize local variables */
	bmacRegs = (bmac_t *)STADATA (stnID, macRegs[macID]);

	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/* Read current settings */
	remr1 = bmacRegs->remr1;
	temr0 = bmacRegs->temr0;

	/* Set new interrupt masks */
	MChangeBits (remr1,
	    BMAC_RELR1_MYCLM | BMAC_RELR1_MYBCN | BMAC_RELR1_OTRBCN,
	    state ? BMAC_RELR1_MYCLM | BMAC_RELR1_MYBCN | BMAC_RELR1_OTRBCN : 0);
	MChangeBits (temr0, BMAC_TELR0_CBERR, state ? BMAC_TELR0_CBERR : 0);

	/* Write new settings */
	bmacRegs->remr1 = remr1;
	bmacRegs->temr0 = temr0;

	/* Restore interrupts */
	MRestoreCSPInterrupts(&SR);

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
	leveltype	SR;		/* Processor intr status */
	bmac_t		*bmacRegs;	/* BMAC registers ptr */
	hwidbtype	*idb;		/* BMAC IDB ptr */
	FDDITimer2sComp	T_Req;		/* T_Req time value */
	FDDITimer2sComp	T_Max;		/* T_Max time value */
	ulong		tvx;
	fddisb          *fsb;

	/* Initialize local variables */
	bmacRegs = (bmac_t *)STADATA (stnID, macRegs[macID]);
	idb = STADATA (stnID, hwIDB[macID]);
	fsb = idb_get_hwsb_inline(idb, HWIDB_SB_FDDI);

	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/* Stop BMAC to configure timer registers */
	bmacRegs->mr &= ~BMAC_MR_RUN;

	/* Set T_Req */
	T_Req = ((fsb->fddi_trt) * 125) / 10;
	T_Req = 1 + ~T_Req;
	MIBDATA (stnID, mib->macBase[macID].hwData.T_Req) = T_Req;
	/* BMAC TREQ3 register always set to 0x00 */
	bmacRegs->treq2 = (T_Req >> 8) & 0xFF;
	bmacRegs->treq1 = (T_Req >> 16) & 0xFF;
	/* BMAC TREQ0 register always set to 0x00 */

	/* Set T_Neg to T_Max */
	T_Max = MACCFG (stnID, mac[macID].T_MaxCapability);
	bmacRegs->tneg3 = T_Max & 0xFF;
	bmacRegs->tneg2 = (T_Max >> 8) & 0xFF;
	bmacRegs->tneg1 = (T_Max >> 16) & 0xFF;
	/* BMAC TNEG0 register always set to 0xFF */

	/* Set TVX */
	tvx = 0;
	for (tvx = 0;
	     (tvx < 16) && (tvx_table[tvx]/XX_TVXSCALE < fsb->fddi_tvx);
	     tvx++);
	bmacRegs->tvx = tvx;

	/* Reenable BMAC */
	bmacRegs->mr |= BMAC_MR_RUN;
	
	/* Reset BMAC  & wait for completion */
	bmacRegs->function = BMAC_FUNCTION_MCRST;
	while (bmacRegs->function & BMAC_FUNCTION_MCRST);

	/* Restore CMT interrupts */
	MRestoreCSPInterrupts(&SR);

	return;
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
	leveltype	SR;		/* Processor intr status */
	bmac_t		*bmacRegs;	/* BMAC registers ptr */

	/* Initialize local variables */
	bmacRegs = (bmac_t *)STADATA (stnID, macRegs[macID]);

	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/*
	* Reset MAC first to guarantee that Claim will be
	* entered (which is not true if current MAC tx state
	* is Beacon). Otherwise, this routine may hang for
	* quite some time if beaconing is not successful.
	*/
	/* Reset BMAC  & wait for completion */
	bmacRegs->function = BMAC_FUNCTION_MCRST;
	while (bmacRegs->function & BMAC_FUNCTION_MCRST);

	/* For Claim & wait for completion */
	bmacRegs->function = BMAC_FUNCTION_CLM;
	while (bmacRegs->function & BMAC_FUNCTION_CLM);

	/* Restore CMT interrupts */
	MRestoreCSPInterrupts(&SR);

	return;
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
	* JAM Beacon actions will not be supported.
	*/
	bsi_instance_t	*bsiData;
	bsi_t		*bsiRegs;
	bmac_t		*bmacRegs;
	paktype		*bcnPtr;
	fddi_hdr	*fddiPtr;

	/* Initialize local variables */
	bsiData = (bsi_instance_t *)STADATA (stnID, xferData[macID]);
	bsiRegs = (bsi_t *)STADATA (stnID, xferRegs[macID]);
	bmacRegs = (bmac_t *)STADATA (stnID, macRegs[macID]);
        bcnPtr = BMACDATA (stnID, macID, bcn_ptr);
        fddiPtr = (fddi_hdr *)bcnPtr->datagramstart;

	if ((BMACDATA (stnID, macID, curBeaconType) != BCN_CLAIM_FAILED) &&
	    (beaconType == BCN_CLAIM_FAILED)) {
	    /* Stop directed beaconing */
	    bsiRegs->mr &= ~BSI_SAR_ABR0; /* Abort pending SMT frames */
	    bmacRegs->option &= ~BMAC_OPTION_IRR;
	    bmacRegs->remr0 &= ~BMAC_REMR0_BCNR;
	    bsi_init_req_channel(bsiData, &bsiData->BEAC_CHAN);
	    BMACDATA (stnID, macID, curBeaconType) = BCN_CLAIM_FAILED;
	}

	if (beaconType == BCN_DIRECTED) {
	    /*
	    * Setup for directed beacons
	    */
	    /* Build FDDI FC,DA,SA fields */
            fddiPtr->fc = 0xc2;
            ieee_copy (DA, &fddiPtr->daddr[0]);
            ieee_copy (HWIDBDATA (stnID, macID, bit_swapped_hardware),
		       &fddiPtr->saddr[0]);

	    /* Build first 4 info field bytes */
	    fddiPtr->data[0] = BCN_DIRECTED;
            fddiPtr->data[1] = '\0';
            fddiPtr->data[2] = '\0';
            fddiPtr->data[3] = '\0';

	    /* Copy info (UNA address) in next bytes */
	    bcopy (beaconInfo, &fddiPtr->data[4], infoLength);

	    /* Update datagram size information */
            bcnPtr->datagramsize = FDDI_FC+FDDI_DA+FDDI_SA+4+infoLength;
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
	leveltype	SR;
	bmac_t		*bmacRegs;
	paktype		*bcnPtr;

	/* Initialize local variables */
	bmacRegs = (bmac_t *)STADATA (stnID, macRegs[macID]);
	bcnPtr = BMACDATA (stnID, macID, bcn_ptr);

	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/*
	 * Set Beacon Packet reference count to 2 in order to ensure
	 * that Beacon packet buffer will not be freed after Beaconing
	 * process is completed.
	 */
	bcnPtr->refcount = 2;
	  
	/* Request Beacon transmission */
	if (BMACDATA (stnID, macID, beacontype) == BCN_DIRECTED) {
	    bsi_repeat (STADATA (stnID, hwIDB[macID]), bcnPtr);
	    bmacRegs->remr0 |= BMAC_REMR0_BCNR;
	} else {
	    bmacRegs->function = BMAC_FUNCTION_BCN;
	    while (bmacRegs->function & BMAC_FUNCTION_BCN);
	}

	/* Restore CMT interrupts */
	MRestoreCSPInterrupts(&SR);

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
	* Leave ring policy in use.
	*/
	return (RMT_DUP_LEAVE);
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
	leveltype	SR;
	bmac_t		*bmacRegs;

	/* Initialize local variables */
	bmacRegs = (bmac_t *)STADATA (stnID, macRegs[macID]);

	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/* Enable/disable BMAC as requested */
	MChangeBits (bmacRegs->mr, BMAC_MR_RUN, state ? BMAC_MR_RUN : 0);

	/* Restore CMT interrupts */
	MRestoreCSPInterrupts(&SR);
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
	return (FALSE);
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
	leveltype	SR;

	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/* Update counter (AuxCtr, letr) */
	if (threshold > (uInt16) 0xFF) {
	    PLAYERREG (stnID, portID, letr) = 0xFF;
            PLAYERDATA (stnID, portID, LEMremaining) = threshold - (uInt16) 0xFF;
	} else {
	    PLAYERREG (stnID, portID, letr) = threshold & 0xFF;
            PLAYERDATA (stnID, portID, LEMremaining) = 0;
	}

	/* Restore interrupts */
	MRestoreCSPInterrupts(&SR);

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
	uInt16		count;

	/* Get current count left from hardware */
	count = (uInt16)PLAYERREG (stnID, portID, clecr);

	/* Calculate total count remaining by adding software counter */
	count += PLAYERDATA (stnID, portID, LEMremaining);

	return (count);
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
	leveltype	SR;
	player_t	*playerRegs;

	/* Initialize local variables */
	playerRegs = (player_t *)STADATA (stnID, portRegs[portID]);
	
	/* Disable CMT interrupts */
	MDisableCSPInterrupts(&SR);

	/* If enabling interrupts, clear interrupt status and restart counter */
	if (state == ENABLE) {
	    playerRegs->letr = playerRegs->letr;
	    playerRegs->icr &= ~PLAYER_ICR_LEMT;
	}

	/* Enable/disable LEM interrupts as requested */
	MChangeBits (playerRegs->icmr, PLAYER_ICMR_LEMT, state ? PLAYER_ICMR_LEMT : 0);

	/* Restore interrupts */
	MRestoreCSPInterrupts(&SR);

	return;
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
static char             xdi_id[] = "@(#) if7_csplocal.c if7_v2.2.1:cset.0009:3:3 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
	Connection Services Process Module
	
	CSP Local Processing Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.
	
	File:			if7_csplocal.c
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
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
#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smterror.h"
#include	"smtmacro.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"cspmacro.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"cspproto.h"
*/


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
Function:	Provide local initialization support. Such support can
			include hardware initialization, local data initialization,
			system communications, etc.
Parameters:	None.
Input:		None.
Output:		None.
Return:		0 if initialization is successful, otherwise an error code
		is returned.
*********************************************************************/
{
uInt32                  result;
uInt16			temp;
uInt16			i;
leveltype		SR;
bmac_t			*bmacRegs;
player_t		*playerRegs;

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
	result = InitCSPMIBDataMulti (stnID);

	if (result == 0)
		/* set Path_Test value */
		CSPDATA (stnID, stationData.Path_Test)
			= StationPathTestMulti (stnID) ? PT_Passed : PT_Failed;

	/*
	* Initialize CMT hardware for normal operation
	*/

	/* Disable interrupts */
	MDisableCSPInterrupts(&SR);

	/*
	* Initialize MAC.
	* Notes:
	*    1. mask regs get written via RMT.
	*/
	bmacRegs = (bmac_t *)STADATA (stnID, macRegs[0]);
     
	/* set interrupt condition mask registers */
	bmacRegs->imr = BMAC_IMR_RNG | BMAC_IMR_TTE | BMAC_IMR_CIE | BMAC_IMR_COE;
    
	/* set event registers */
	bmacRegs->remr0 = BMAC_REMR0_ROP | BMAC_REMR0_RNOP;
	bmacRegs->remr1 = 0;
	bmacRegs->temr0 = 0;
	bmacRegs->cimr = 0;
	bmacRegs->comr = BMAC_COMR_FRRCV | BMAC_COMR_FREI | BMAC_COMR_FRLST |
			 BMAC_COMR_FRCOP | BMAC_COMR_FRNCOP | BMAC_COMR_FRTRX;

	/* Set mode and option registers */
	bmacRegs->mr = 0;
	bmacRegs->option = BMAC_OPTION_ELA;

	/* Zero counters */
	bmacRegs->frct1 = bmacRegs->frct2 = bmacRegs->frct3 = 0;
	bmacRegs->eict1 = bmacRegs->eict2 = bmacRegs->eict3 = 0;
	bmacRegs->lfct1 = bmacRegs->lfct2 = bmacRegs->lfct3 = 0;
	bmacRegs->fcct1 = bmacRegs->fcct2 = bmacRegs->fcct3 = 0;
	bmacRegs->fnct1 = bmacRegs->fnct2 = bmacRegs->fnct3 = 0;
	bmacRegs->ftct1 = bmacRegs->ftct2 = bmacRegs->ftct3 = 0;
	bmacRegs->tkct1 = bmacRegs->tkct2 = bmacRegs->tkct3 = 0;

	/*
	* Initialize PHY(s).
	* Notes:
	*   1.	Set Noise Threshold registers. Using the NTR and
	*	NPTR together gives a 16-bit countdown counter
	*	at a rate of 80ns. The value of NS_Max is converted
	*	into 80ns intervals and the low 8 bits are placed into
	*	NPTR and the high 8 bits are put in NTR. When both
	*	registers go to 0, the NT bit in RCRA gets set and
	*	causes an interrupt.
	*/
	for ( i = 0; i < PORTCFG (stnID, PortCt); i++ ) {
	    playerRegs = (player_t *)STADATA (stnID, portRegs[i]);
	    temp = (NS_Max*1000) / 80;	/* convert to 80ns intervals */
	    temp /= 256;		/* use 256 count for NPTR */
	    playerRegs->nptr = 0xFF;	/* set to 256 counts */
	    playerRegs->ntr = temp & 0xFF; /* # of 256x80ns counts */
	
	    /*	Clear line state history */
	    ReadLineStateMulti (stnID, i);

	    /* Clear Receive Condition registers */
	    playerRegs->rcmra = 0;
	    playerRegs->rcmrb = 0;
	
	    /* Set interrupt condition mask reg */
	    playerRegs->icmr = PLAYER_ICMR_RCA | PLAYER_ICMR_RCB;
    
	    /*
	    * Enable control bus write command reject interrupt
	    * for player 0 on station 0 only since we use this to
	    * get the 4ms tick.
	    */
	    if (i == 0)
		playerRegs->icmr |= PLAYER_ICMR_CCR;
	
	    /* Clear condition registers */
	    playerRegs->rcra = 0;
	    playerRegs->rcrb = 0;
	    playerRegs->icr = 0;

	    /* Put it in Isolate state */
	    if (i)
		playerRegs->cr = PLAYER_B_CR_ISOLATED;
	    else
		playerRegs->cr = (PORTCFG (stnID, PortCt) == 1) ?
		    PLAYER_S_CR_ISOLATED : PLAYER_A_CR_ISOLATED;

	    /* Initialize hardware associated variables */
	    PLAYERDATA (stnID, i, previousLS) = 0;
	    PLAYERDATA (stnID, i, last_ls_rcmra_mask) = 0;
	    PLAYERDATA (stnID, i, cur_rcmra_mask) = 0;
	
	    /* Set run mode */
	    playerRegs->mr |= PLAYER_MR_RUN;
	}

	/* Deinsert optical bypass */
	BypassRequestMulti (stnID, DEINSERTED);

	/* Restore interrupts */
	MRestoreCSPInterrupts(&SR);

	return (result);
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

        /* Ignore request if interface down */
        if (!FBMDATA (stnID, fbmMACData[MACNum].SM_MA_Avail))
            return;

        /* Copy frame information to local buffer and enqueue for tx */
        p = getbuffer (frameLength);
        if (!p) {
            return;
        }
        p->datagramsize = frameLength;
        bcopy (frameBuffer, p->datagramstart, frameLength + EXTRA_FC_BYTES);
        p->datagramstart += EXTRA_FC_BYTES;
        p->if_output = HWIDBDATA (stnID, MACNum, firstsw);
        if (fddi_smt_packet_debug)
            fddi_smt_debug(p,TRUE);

        datagram_out(p);

        return;
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
        return (0);
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
register union Splice	data;			/* data value */
register uInt32		temp;			/* temporary variable */
bmac_t			*bmacRegs;		/* BMAC hardware regs ptr */
uInt16			macID;			/* MAC index */

	/* Initialize local variables */
	macID = macPtr->baseData.xdi.xdiIndex;
	bmacRegs = (bmac_t *)STADATA(stnID, macRegs[macID]);
	data.dword = 0;

	/*
	*	Read the appropriate MAC attribute.
	*/
	switch (attr)
	{
	case fddiMACT_Req:
		/* Can only be read while in Stop mode */
		((TLV32BitType *) buffer)->data = macPtr->hwData.T_Req;
		break;

	case fddiMACT_Neg:
		data.b.byte0 = bmacRegs->tneg3;
		data.b.byte1 = bmacRegs->tneg2;
		data.b.byte2 = bmacRegs->tneg1;
		data.b.byte3 = 0xFF;
		((TLV32BitType *) buffer)->data = data.dword;
		break;

	case fddiMACT_Max:
		data.dword = 0xFFFFFC00;
		temp = (uInt32) bmacRegs->tmax;
		while (temp--)
		    data.dword <<= 1;
		((TLV32BitType *) buffer)->data = data.dword;
		break;

	case fddiMACTvxValue:
		data.dword = 0xFFFFFC00;
		temp = (uInt32) bmacRegs->tvx;
		while (temp--)
		    data.dword <<= 1;
		((TLV32BitType *) buffer)->data = data.dword;
		break;

	case fddiMACFrame_Ct:
		data.b.byte0 = bmacRegs->frct3;
		data.b.byte1 = bmacRegs->frct2;
		data.b.byte2 = bmacRegs->frct1;
		((TLV32BitType *) buffer)->data = data.dword + macPtr->hwData.Frame_Ct;
		break;

	case fddiMACCopied_Ct:
		data.b.byte0 = bmacRegs->fcct3;
		data.b.byte1 = bmacRegs->fcct2;
		data.b.byte2 = bmacRegs->fcct1;
		((TLV32BitType *) buffer)->data = data.dword + macPtr->hwData.Copied_Ct;
		break;

	case fddiMACTransmit_Ct:
		data.b.byte0 = bmacRegs->ftct3;
		data.b.byte1 = bmacRegs->ftct2;
		data.b.byte2 = bmacRegs->ftct1;
		((TLV32BitType *) buffer)->data = data.dword + macPtr->hwData.Transmit_Ct;
		break;

	case fddiMACToken_Ct:
		data.b.byte0 = bmacRegs->tkct3;
		data.b.byte1 = bmacRegs->tkct2;
		data.b.byte2 = bmacRegs->tkct1;
		((TLV32BitType *) buffer)->data = data.dword + macPtr->hwData.Token_Ct;
		break;

	case fddiMACError_Ct:
		data.b.byte0 = bmacRegs->eict3;
		data.b.byte1 = bmacRegs->eict2;
		data.b.byte2 = bmacRegs->eict1;
		((TLV32BitType *) buffer)->data = data.dword + macPtr->hwData.Error_Ct;
		break;

	case fddiMACLost_Ct:
		data.b.byte0 = bmacRegs->lfct3;
		data.b.byte1 = bmacRegs->lfct2;
		data.b.byte2 = bmacRegs->lfct1;
		((TLV32BitType *) buffer)->data = data.dword + macPtr->hwData.Lost_Ct;
		break;

	case fddiMACNotCopied_Ct:
		data.b.byte0 = bmacRegs->fnct3;
		data.b.byte1 = bmacRegs->fnct2;
		data.b.byte2 = bmacRegs->fnct1;
		((TLV32BitType *) buffer)->data = data.dword + macPtr->hwData.NotCopied_Ct;
		break;

	case fddiMACRingOp_Ct:
		((TLV32BitType *) buffer)->data = HWIDBDATA (stnID, macID, fddi_rops);
		break;

	default:
		result = RC_NoParameter;
		break;
	}

	return (result);
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
	return (RC_NoParameter);
}

uInt32
#ifdef __STDC__
SetMACDataMulti (uInt16 stnID, uInt16 attr, MACDataPtr macPtr, TLVParamType * p)
#else
SetMACDataMulti (stnID, attr, macPtr, p)
                uInt16           stnID;
                uInt16           attr;
                MACDataPtr       macPtr;
                TLVParamType    *p;
#endif
/*********************************************************************
Function:       Set a MAC attribute.
                        Some of the MAC-related attributes are contained
                        within the MAC or its associated hardware and software.
                        To set the data values, SetMIBMACAttr() calls SetMACDataMulti()
                        to place the data into the appropriate location.
Parameters:     stnID   = index of station to use.
                        attr    = MAC attribute to set.
                        macPtr  = MIB entry for the MAC to change.
                        p               = pointer to a buffer holding the data value.
Input:          p               = attribute in TLV encoding.
Output:         Hardware or system software is set with new value.
                        Additionally, the new value may be written to non-volatile
                        memory. Some of the MAC attributes listed are managed by
                        the MIB database and are passed to this routine only to be
                        stored in non-volatile memory if desired.
Return:         The value returned corresponds to the results codes used
                        by the Parameter Manager Protocol:
                                RC_Success
                                        attribute changed successfully
                                RC_NoParameter
                                        attribute not supported
                                RC_OutOfRange
                                        value given is out of bounds
*********************************************************************/
{
uInt32           result = RC_Success;

        switch (attr)
        {
        case fddiMACRequestedPaths:
        case fddiMACFrameErrorThreshold:
        case fddiMACNotCopiedThreshold:
        case fddiMACMA_UnitdataEnable:
                /*
                 * These attributes are managed by the MIB database. They are passed
                 * to this routine for storage in non-volatile memory.
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
                 * Currently, there are no MAC parameters that need to be written to
                 * the MAC hardware.
                 */
                result = RC_NoParameter;
                break;
        }

        return (result);
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

	return (result);
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
uInt32           result = RC_Success;

        switch (attr)
        {
        case fddiPATHT_Rmode:
        case fddiPATHTVXLowerBound:
        case fddiPATHT_MaxLowerBound:
        case fddiPATHMaxT_Req:
                /*
                 * These attributes are managed by the MIB database. They are passed
                 * to this routine for storage in non-volatile memory.
                 */
                result = RC_Success;
                break;

        case fddiPATHRingLatency:
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

        return (result);
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

	/*
	*	Read the appropriate PORT attribute.
	*/
	switch (attr)
	{
	case fddiPORTPC_LS:
		switch (PLAYERREG (stnID, portPtr->baseData.xdi.xdiIndex, crsr) & PLAYER_CRSR_LS_MSK)
		{
		case PLAYER_CRSR_LS_ALS:
		    ((TLV16BitType *) buffer)->data = LS_pdr;
		    break;

		case PLAYER_CRSR_LS_ILS:
		    ((TLV16BitType *) buffer)->data = LS_ils;
		    break;

		case PLAYER_CRSR_LS_NSD:
		case PLAYER_CRSR_LS_QLS:
		    ((TLV16BitType *) buffer)->data = LS_qls;
		    break;

		case PLAYER_CRSR_LS_MLS:
		    ((TLV16BitType *) buffer)->data = LS_mls;
		    break;

		case PLAYER_CRSR_LS_HLS:
		    ((TLV16BitType *) buffer)->data = LS_hls;
		    break;

		case PLAYER_CRSR_LS_NLS:
		    ((TLV16BitType *) buffer)->data = LS_nls;
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

	return (result);
}

uInt32
#ifdef __STDC__
SetPORTDataMulti (uInt16 stnID, uInt16 attr, PORTDataPtr portPtr,
        TLVParamType * p)
#else
SetPORTDataMulti (stnID, attr, portPtr, p)
                uInt16           stnID;
                uInt16           attr;
                PORTDataPtr      portPtr;
                TLVParamType    *p;
#endif
/*********************************************************************
Function:       Set an PORT attribute.
                        Although ALL PORT attributes are managed through the MIB
                        database, an interface routine is required for storing
                        the PORT attribute values into a non-volatile memory
                        location. This routine DOES NOT change the MIB itself.
                        It only provides a place for storing PORT attributes
                        permanently in the system.
Parameters:     stnID   = index of station to use.
                        attr    = PORT attribute to set.
                        portPtr = PORT element that has changed.
                        p               = pointer to a buffer holding the data value.
Input:          p               = attribute in TLV encoding.
Output:         Non-volatile memory is changed to store the new value. This
                        action is optional and all changes to standard and XDI PORT
                        attributes may be ignored by this routine.
Return:         This routine returns RC_Success.
*********************************************************************/
{
        switch (attr)
        {
        case fddiPORTConnectionPolicies:
        case fddiPORTRequestedPaths:
        case fddiPORTMACLoop_Time:
        case fddiPORTMaint_LS:
        case fddiPORTLer_Cutoff:
        case fddiPORTLer_Alarm:
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

        return (RC_Success);
}


/*********************************************************************
		Cisco specific routines
*********************************************************************/

void
#ifdef __STDC__
CSPHandlePHYInterrupts (uInt16 stnID, uInt16 portID)
#else
CSPHandlePHYInterrupts (stnID, portID)
	uInt16		stnID;		/* Station index */
	uInt16		portID;		/* Port index */
#endif
/*********************************************************************
Function:       Handle the Player chip interrupts.
Parameters:     playerRegs: pointer to the Player registers.
Input:          
Output:         
Return:         None.
*********************************************************************/
{
	player_t		*playerRegs;
	uChar			icr, crsr;
	uInt16			ls;

	/* Initialize local variables */
	playerRegs = (player_t *)STADATA (stnID, portRegs[portID]);

	/* Acknowledge CMT activations caused by the NMI CSP timer */
	if ((portID == 0) && (playerRegs->icr & PLAYER_ICR_CCR))
	    playerRegs->icr &= ~PLAYER_ICR_CCR;

	/* Check if this Player generated any interrupt */
	if (!(icr = (playerRegs->icr & playerRegs->icmr)))
	    return;

	/* Check for line state changes  */
	if (icr & (PLAYER_ICR_RCA | PLAYER_ICR_RCB)) {
            /*
	    * Read current line states shown in hardware.
            * Disable further interrupts for the line state being
	    * currenlty received to avoid repeating interrupts from
	    * same line state due to link noise problems.
            */
            crsr = playerRegs->crsr & PLAYER_CRSR_LS_MSK;
	    PLAYERDATA (stnID, portID, last_ls_rcmra_mask) =
		CRSRLSIndex2RCMRLSMask[crsr].rcmra;
	    PLAYERDATA (stnID, portID, last_ls_rcmrb_mask) =
		CRSRLSIndex2RCMRLSMask[crsr].rcmrb;
            playerRegs->rcmra = PLAYERDATA (stnID, portID, cur_rcmra_mask) &
				~CRSRLSIndex2RCMRLSMask[crsr].rcmra;
            playerRegs->rcmrb = PLAYERDATA (stnID, portID, cur_rcmrb_mask) &
				~CRSRLSIndex2RCMRLSMask[crsr].rcmrb;

            /* Post line state change signal */
            if ((ls = ReadLineStateMulti (stnID, portID))) {
                SendSignalMulti (stnID, SIG_Line_State, portID, (uInt32) ls);
		fddi_cbug_phy_ind (stnID, portID, ls);
	    }
        }

	/* Check if LEM Threshold reached */
	if (icr & PLAYER_ICR_LEMT)  {
	    /* if count completed, set flag  */
	    if (LEMInterrupt (stnID, portID))
		SendSignalMulti (stnID, SIG_LEM_Threshhold, portID, (uInt32) 0);
	}

	/*
	*  RCRA and RCRB are cleared by the ReadLineState() routine.
	*  Clear ICR to value 0x80 since MSB is always set.
	*/
	playerRegs->icr = 0x80;

	return;
}

void
#ifdef __STDC__
CSPHandleMACInterrupts (uInt16 stnID, uInt16 macID)
#else
CSPHandleMACInterrupts (stnID, macID)
	uInt16		stnID;		/* Station index */
	uInt16		macID;		/* MAC index */
#endif
/*********************************************************************
Function:       Handle the BMAC chip interrupts.
Parameters:     playerRegs: pointer to the Player registers.
Input:          
Output:         
Return:         None.
*********************************************************************/
{
	bmac_t		*bmacRegs;
	bmac_instance_t	*bmacData;
	bsi_instance_t	*bsiData;
	hwidbtype	*bmacIDB;
	uInt32		tneg;
	uInt32		macSignal;
	uInt32		ringOp;
	uChar		icr, relr0, relr1, colr; 
	fddisb          *fsb;

	/* Initialize local variables */
	bmacRegs = (bmac_t *)STADATA (stnID, macRegs[macID]);
	bmacIDB = (hwidbtype *)STADATA (stnID, hwIDB[macID]);
	bmacData = (bmac_instance_t *)STADATA (stnID, macData[macID]);
	bsiData = (bsi_instance_t *)STADATA (stnID, xferData[macID]);
	macSignal = 0;

	/* Check if this BMAC generated any interrupt */
	if (!(icr = (bmacRegs->icr & bmacRegs->imr)))
	    return;

	/*
	* Check for change in Ring_Op.
	*/
	if ((relr0 = bmacRegs->relr0) & (BMAC_RELR0_ROP | BMAC_RELR0_RNOP)) {

	    /* clear events after they were saved */
	    bmacRegs->relr0 = 0;

	    /* Send RingOp signal */
	    ringOp = bmacRegs->cts0 & BMAC_CTS0_ROP;
	    SendSignalMulti (stnID, SIG_Ring_OP, macID, ringOp);

	    fsb = idb_use_hwsb_inline(bmacIDB, HWIDB_SB_FDDI);

	    if (ringOp) {
		/*
		* Update database to reflect RingOP.
		*/
		GET_TIMESTAMP(fsb->fddi_operational);
		bmacIDB->counters.transitions++;
		bmacIDB->fddi_rops++;
		tneg = ~((bmacRegs->tneg1 << 16) + (bmacRegs->tneg2 << 8) +
		       (bmacRegs->tneg3) + 0xFF000000) + 1;
		fsb->fddi_trt_neg = (tneg * 1000) / 12500;
	    } else {
		/*
		* Update database to reflect RingNOP.
		*/
		TIMER_STOP(fsb->fddi_operational);
		bmacIDB->counters.transitions++;
	    }
	    
	    idb_release_hwsb_inline(bmacIDB, HWIDB_SB_FDDI);
	}

	/*
	* Check for received MAC frames.
	*/
	if ((relr1 = bmacRegs->relr1) & (BMAC_RELR1_MYCLM |
	    BMAC_RELR1_MYBCN | BMAC_RELR1_OTRBCN)) {
	    uChar remr1Mask = 0;

	    /* Check for My_Claim */
	    if (relr1 & BMAC_RELR1_MYCLM) {
		macSignal |= MAC_My_Claim;
		remr1Mask |= BMAC_RELR1_MYCLM;
	    }

	    /* Check for My_Beacon */
	    if (relr1 & BMAC_RELR1_MYCLM) {
		macSignal |= MAC_My_Beacon;
		remr1Mask |= BMAC_RELR1_MYBCN;
	    }

	    /* Check for Other_Beacon */
	    if (relr1 & BMAC_RELR1_OTRBCN) {
		macSignal |= MAC_Other_Beacon;
		remr1Mask |= BMAC_RELR1_OTRBCN;
	    }

	    /* Clear status & mask of interrupts already seen */
	    bmacRegs->remr1 &= ~remr1Mask;
	    bmacRegs->relr1 = 0;
	}

	/*
	* Check for timer events.
	*/
	if (icr & BMAC_ICR_TTE) {

	    /* Check if TRT expired in T4 or T5 */
	    if (bmacRegs->telr0 & BMAC_TELR0_CBERR) {

		/* Mark interrupt  & clear interrupt */
		macSignal |= MAC_TRT_In_T4T5;
		bmacRegs->temr0 &= ~BMAC_TELR0_CBERR;
		bmacRegs->relr0 = 0;
	    }
	}

	/*
	* Check for counter overflow events.
	*/
	if (icr & BMAC_ICR_COE) {

	    /* Read & save counters status */
	    colr = bmacRegs->colr;
	    bmacRegs->colr = 0;

	    /* Check for each counter overflow */
	    if (colr & BMAC_COLR_FRRCV)
		MIBDATA (stnID, mib->macBase[macID].hwData.Frame_Ct) += 0x100000;
	    if (colr & BMAC_COLR_FREI)
		MIBDATA (stnID, mib->macBase[macID].hwData.Error_Ct) += 0x100000;
	    if (colr & BMAC_COLR_FRLST)
		MIBDATA (stnID, mib->macBase[macID].hwData.Lost_Ct) += 0x100000;
	    if (colr & BMAC_COLR_FRCOP)
		MIBDATA (stnID, mib->macBase[macID].hwData.Copied_Ct) += 0x100000;
	    if (colr & BMAC_COLR_FRNCOP)
		MIBDATA (stnID, mib->macBase[macID].hwData.NotCopied_Ct) += 0x100000;
	    if (colr & BMAC_COLR_FRTRX)
		MIBDATA (stnID, mib->macBase[macID].hwData.Transmit_Ct) += 0x100000;
	    if (colr & BMAC_COLR_TKRCVD)
		MIBDATA (stnID, mib->macBase[macID].hwData.Token_Ct) += 0x100000;
	}

	/* Send MAC signals (if any) */
	if (macSignal)
	    SendSignalMulti (stnID, SIG_MAC_Interrupt, macID, macSignal);

	return;
}

Flag
#ifdef __STDC__
LEMInterrupt (uInt16 stnID, uInt16 portID)
#else
LEMInterrupt (stnID, portID)
	uInt16		stnID;		/* Station index */
	uInt16		portID;		/* Port index */
#endif
/*********************************************************************
Function:       Check if interrupt is end of threshold count.
Parameters:     stnID  = index of PHY whose link is being monitored.
Input:          Uses remaining[].
Output:         Resets counter if not end of threshold
Return:         TRUE if end of threshold, otherwise FALSE.
Notes:          MUST BE CALLED WITH INTERRUPTS DISABLED.
Modification History:
*********************************************************************/
{
	uInt16		*pCtr;

	/* Initialize local variables */
	pCtr = &PLAYERDATA (stnID, portID, LEMremaining);

	/*
	*  If threshold still has more errors to count,
	*  then reset the threshold register.
	*/
        if (*pCtr) {
	    if (*pCtr & 0xFF00) {
	        *pCtr -= 0xFF;
	        PLAYERREG (stnID, portID, letr) = 0xFF;
	    } else {
	        PLAYERREG (stnID, portID, letr) = (uChar)*pCtr;
	        *pCtr = 0;
	    }
	    return (FALSE);
	}

	/* Otherwise, this link has failed */
	return (TRUE);
}
#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_mibvenif.c if7_v2.2.1:cset.0009:3:3 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
	Management Information Base Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	Vendor MIB Attribute Processing Module

	File:			if7_mibvenif.c
	Created:		04/30/92

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92

	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This module provides access to the FDDI MIB Vendor Attributes
	specified by the implementer.

	Modification History:
	*** Created for SMT 7.2 ***
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
Function:       MIB access routine to get a Vendor fddiSMT attribute.
Parameter:      stnID   = index of station to use.
                        bufSize = maximum length of buffer bufData
                                                        measured in chars.
                        p               = pointer to buffer containing the
                                                requested attribute information and used to
                                                hold attribute value returned.
                        smtPtr  = address of SMT attribute data.
Input:          p               = contains the requested attribute ID and object
                                                index (if required) using the MIB TLV encoding
                                                for a Get Request (see SMT standard).
Output:         p               = holds the attribute value requested using the
                                                MIB TLV encoding specified in the SMT standard.
Return:         The value returned corresponds to the results codes used
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
Notes:          Entry to the MIB must be from GetMIBAttr() in mibproc.c to
                        insure correct attribute values and a clean response buffer.
                        If no vendor-specific attribute are supported, then this
                        routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_Success;   /* return code */

        /*
         * Check if buffer is minimum size. Header + data Length is the same for
         * 8, 16, and 32 bit data types.
         */
        if (bufSize < (sizeof (TLVHdrType) + sizeof (TLV32BitType)
                        + sizeof (TLV8BitType)))
                return (RC_BUFFER_ERROR);

        /*
         * Reduce buffer size by header amount and vendor ID.
         */
        bufSize -= (sizeof (TLVHdrType) + sizeof (TLV32BitType));

        /*
         * Length is the same for 8, 16, and 32 bit data types which is the most
         * common size.
         */
        p->paramLen = sizeof (TLV8BitType);

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
         * Adjust parameter length to include vendor attr ID.
         */
        if (result == RC_Success)
                p->paramLen += sizeof (TLV32BitType);

        return (result);
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
Function:       MIB access routine to get a vendor fddiMAC attribute.
Parameter:      stnID   = index of station to use.
                        bufSize = maximum length of buffer bufData measured in chars.
                        p               = pointer to buffer containing the requested
                                                attribute information and used to hold attribute
                                                value returned.
                        macPtr  = pointer to MAC's attribute structure in the MIB.
Input:          p               = contains the requested attribute ID and object
                                                index (if required) using the MIB TLV encoding
                                                for a Get Request (see SMT standard).
Output:         p               = holds the attribute value requested using the
                                                MIB TLV encoding specified in the SMT standard.
Return:         The value returned corresponds to the results codes used
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
Notes:          Entry to the MIB must be from GetMIBAttr() in mibproc.c to
                        insure correct attribute values and a clean response buffer.
                        If no vendor-specific attribute are supported, then this
                        routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_Success;   /* return code */

        /*
         * Check if buffer is minimum size. Header + index + vendor ID + data
         * Length is the same for 8, 16, and 32 bit data types.
         */
        if (bufSize < (sizeof (TLVHdrType) + sizeof (TLV16BitType)
                        + sizeof (TLV32BitType) + sizeof (TLV8BitType)))
                return (RC_BUFFER_ERROR);

        /*
         * Reduce buffer size by header, index and vendor ID length.
         */
        bufSize -= (sizeof (TLVHdrType) + sizeof (TLV16BitType)
                + sizeof (TLV32BitType));

        /*
         * Set length to most common size (index added later).
         */
        p->paramLen = sizeof (TLV16BitType);

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
                p->paramLen += (sizeof (TLV16BitType) + sizeof (TLV32BitType));

        return (result);
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
Function:       MIB access routine to get a vendor fddiPATH attribute.
Parameter:      stnID   = index of station to use.
                        bufSize = maximum length of buffer bufData measured in chars.
                        p               = pointer to buffer containing the requested
                                                attribute information and used to hold attribute
                                                value returned.
                        pathPtr = pointer to PATH's attribute structure in the MIB.
Input:          p               = contains the requested attribute ID and object
                                                index (if required) using the MIB TLV encoding
                                                for a Get Request (see SMT standard).
Output:         p               = holds the attribute value requested using the
                                                MIB TLV encoding specified in the SMT standard.
Return:         The value returned corresponds to the results codes used
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
Notes:          Entry to the MIB must be from GetMIBAttr() in mibproc.c to
                        insure correct attribute values and a clean response buffer.
                        If no vendor-specific attribute are supported, then this
                        routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_Success;   /* return code */

        /*
         * Check if buffer is minimum size. Header + index + vendor ID + data
         * Length is the same for 8, 16, and 32 bit data types.
         */
        if (bufSize < (sizeof (TLVHdrType) + sizeof (TLV16BitType)
                        + sizeof (TLV32BitType) + sizeof (TLV8BitType)))
                return (RC_BUFFER_ERROR);

        /*
         * Reduce buffer size by header, index and vendor ID length.
         */
        bufSize -= (sizeof (TLVHdrType) + sizeof (TLV16BitType)
                + sizeof (TLV32BitType));

        /*
         * Set length to most common size (index added later).
         */
        p->paramLen = sizeof (TLV16BitType);

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
                p->paramLen += (sizeof (TLV16BitType) + sizeof (TLV32BitType));

        return (result);
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
Function:       MIB access routine to get a vendor fddiPORT attribute.
Parameter:      stnID   = index of station to use.
                        bufSize = maximum length of buffer bufData measured in chars.
                        p               = pointer to buffer containing the requested
                                                attribute information and used to hold attribute
                                                value returned.
                        portPtr = pointer to PORT's attribute structure in the MIB.
Input:          p               = contains the requested attribute ID and object
                                                index (if required) using the MIB TLV encoding
                                                for a Get Request (see SMT standard).
Output:         p               = holds the attribute value requested using the
                                                MIB TLV encoding specified in the SMT standard.
Return:         The value returned corresponds to the results codes used
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
Notes:          Entry to the MIB must be from GetMIBAttr() in mibproc.c to
                        insure correct attribute values and a clean response buffer.
                        If no vendor-specific attribute are supported, then this
                        routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_Success;   /* return code */

        /*
         * Check if buffer is minimum size. Header + index + data Length is the
         * same for 8, 16, and 32 bit data types.
         */
        if (bufSize < (sizeof (TLVHdrType) + sizeof (TLV16BitType)
                        + sizeof (TLV32BitType) + sizeof (TLV8BitType)))
                return (RC_BUFFER_ERROR);

        /*
         * Reduce size of buffer by header, vendor ID and index.
         */
        bufSize -= (sizeof (TLVHdrType) + sizeof (TLV16BitType)
                + sizeof (TLV32BitType));

        /*
         * Set length to most common size.
         */
        p->paramLen = sizeof (TLV16BitType);

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

        return (result);
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
Function:       MIB access routine to change a Vendor SMT attribute.
Parameter:      stnID   = index of station to use.
                        p               = pointer to buffer containing the new attribute
                                                information.
                        smtPtr  = address of SMT attribute data.
Input:          p               = contains the attribute ID using the MIB TLV
                                                encoding for a Change Request (see SMT standard).
Output:         p               = holds the attribute value set using MIB TLV
                                                encoding specified in the SMT standard.
Return:         The value returned corresponds to the results codes used
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
Notes:          Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
                        insure correct attribute values and a clean response buffer.
                        If no vendor-specific attribute are supported, then this
                        routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_NoParameter;       /* return code */

        return (result);
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
Function:       MIB access routine to change a Vendor MAC attribute.
Parameter:      stnID   = index of station to use.
                        p               = pointer to buffer containing the new attribute
                                                information.
                        macPtr  = address of MAC attribute data.
Input:          p               = contains the attribute ID using the MIB TLV
                                                encoding for a Change Request (see SMT standard).
Output:         p       = holds the attribute value set using MIB TLV
                                                encoding specified in the SMT standard.
Return:         The value returned corresponds to the results codes used
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
Notes:          Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
                        insure correct attribute values and a clean response buffer.
                        If no vendor-specific attribute are supported, then this
                        routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_NoParameter;       /* return code */

        return (result);
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
Function:       MIB access routine to change a Vendor PATH attribute.
Parameter:      stnID   = index of station to use.
                        p               = pointer to buffer containing the new attribute
                                                information.
                        pathPtr = address of PATH attribute data.
Input:          p               = contains the attribute ID using the MIB TLV
                                                encoding for a Change Request (see SMT standard).
Output:         p               = holds the attribute value set using MIB TLV
                                                encoding specified in the SMT standard.
Return:         The value returned corresponds to the results codes used
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
Notes:          Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
                        insure correct attribute values and a clean response buffer.
                        If no vendor-specific attribute are supported, then this
                        routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_NoParameter;       /* return code */

        return (result);
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
Function:       MIB access routine to change a Vendor PORT attribute.
Parameter:      stnID   = index of station to use.
                        p               = pointer to buffer containing the new attribute
                                                information.
                        portPtr = address of PORT attribute data.
Input:          p               = contains the attribute ID using the MIB TLV
                                                encoding for a Change Request (see SMT standard).
Output:         p               = holds the attribute value set using MIB TLV
                                                encoding specified in the SMT standard.
Return:         The value returned corresponds to the results codes used
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
Notes:          Entry to the MIB must be from ChangeMIBAttr() in mibproc.c to
                        insure correct attribute values and a clean response buffer.
                        If no vendor-specific attribute are supported, then this
                        routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32           result = RC_NoParameter;       /* return code */

        return (result);
}


/*********************************************************************
	Vendor Notification Routines
*********************************************************************/

void
#ifdef __STDC__
ProcessVendorMIBEvent (SMTMessage * msg, MIBNotificationType * notice)
#else
ProcessVendorMIBEvent (msg, notice)
		SMTMessage             *msg;
		MIBNotificationType    *notice;
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
			even take care of sending the message itself. If this is
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
		uInt16                  stnID;
		MIBType                *mibPtr;
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

