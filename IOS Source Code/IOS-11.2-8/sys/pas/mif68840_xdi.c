/* $Id: mif68840_xdi.c,v 3.1.60.10 1996/08/28 13:07:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/mif68840_xdi.c,v $
 *------------------------------------------------------------------
 * mif68840_xdi.c - 1F MIF68840 based PCI port adaptor support
 *                  (Port Adapter platforms common FDDI SMT functions)
 *
 * August 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mif68840_xdi.c,v $
 * Revision 3.1.60.10  1996/08/28  13:07:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.60.9  1996/07/23  13:27:42  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.60.8  1996/06/07  06:59:14  fsunaval
 * CSCdi54755:  Fixes for SMT conformance tests.
 * Branch: California_branch
 * 1.Fix trace test and other minor MIB values.
 *
 * Revision 3.1.60.7  1996/05/27  06:02:49  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Branch: California_branch
 *
 * Revision 3.1.60.6  1996/05/07  00:38:57  mbeesley
 * CSCdi56232:  Enable IP Flow/Optimum on fddi
 * Branch: California_branch
 *
 * Revision 3.1.60.5  1996/05/06  19:27:40  fsunaval
 * CSCdi56768:  Update trt_neg
 * Branch: California_branch
 * 1.Pass one more parameter throught the MIB loveletter.
 *
 * Revision 3.1.60.4  1996/04/30  07:05:17  fsunaval
 * CSCdi54755:  Fixes for SMT conformance tests.
 * Branch: California_branch
 *
 * Revision 3.1.60.3  1996/04/13  04:39:04  fsunaval
 * CSCdi54379:  Optical bypass not working on new port adapter.
 * Branch: California_branch
 * 1.Pri. and Sec. relays do not have to be energized after configuration
 *  change.
 *
 * Revision 3.1.60.2  1996/04/11  02:03:10  izhu
 * CSCdi53807:  Parser support for new port adapter on predator
 * Also includes optical bypass change
 * Branch: California_branch
 *
 * Revision 3.1.60.1  1996/03/21  23:24:09  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.7  1996/02/26  04:39:24  fsunaval
 * CSCdi49791:  Incorporate code-review comments for Fddi port adapter
 * Branch: ELC_branch
 *
 * Revision 3.1.4.6  1996/02/08  08:48:22  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.4.5  1996/02/05  07:00:15  fsunaval
 * Branch: ELC_branch
 * 1.Particle based fast-switching Fddi support and code cleanup.
 *
 * Revision 3.1.4.4  1996/01/25  12:10:13  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1.4.3  1996/01/16  01:43:58  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.4.2  1995/12/26  19:56:50  fsunaval
 * CSCdi46111:  Add parser support for FDDI PA and other changes
 * Branch: ELC_branch
 * 1.Add missing diffs from previous commit for init_pas.c
 *   (NMI interrupts).
 * 2.Add FDDI PA parser support (sh cont, test pas fddi).
 * 3.Do a little code cleanup of hes_fddi.c and rsp_fip.c specifically
 *   in regards to the way trt and tvx are passed down.
 *
 * Revision 3.1.4.1  1995/12/23  04:41:30  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/11/09  12:58:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/24  19:46:30  fsunaval
 * Placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************************
                                Includes
*********************************************************************/
#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"   
#include "ttysrv.h" 
#include "logger.h"
#include "subsys.h"
#include "sched.h"
#include "parser.h"
#include "mgd_timers.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../if/network.h"

#include "../ip/ip.h"

#include "../xdi/xdihdrsif.h" /*
                               * ../src-vip/vip_xdi.h, mif68840_xdi.h 
                               * and ../if/if_xdi.h
                               */
#include "../xdi/fddihdr.h"
#include "../xdi/smtmsg.h"
#include "../xdi/csphdr.h"
#include "../xdi/cspglbl.h"
#include "../xdi/cspproto.h"
#include "../xdi/hw_csp.h"
#include "../xdi/hw_phy.h"
#include "../xdi/motomac.h"
#include "../xdi/mibdefs.h"
#include "../xdi/mibglbl.h"
#include "../xdi/mibproto.h"
#include "../xdi/fbmframe.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/fbmglbl.h"

#include "../if/if_fddi.h"
#include "../if/if_fddi_public.h"
#include "../if/if_xdi_public.h"

#include "if_pas.h"
#include "if_pas_mif68840.h"
#include "msg_mif68840.c"		/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "pas_id.h"

#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_cspintif.c if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
        Function Prototypes
*********************************************************************/
extern void datagram_out(paktype *p);
extern void fddi_smt_debug(paktype *, boolean);

/* FIXME */
extern void CSPInterruptHandler(void *ptr);

/* globals */
mgd_timer      mif68840_CSPTimer;


/*********************************************************************
	Interrupt Interface Routines
*********************************************************************/



void
#ifdef __STDC__
CSPInterruptHandler (void *ptr)
#else
CSPInterruptHandler (*ptr)
		void *ptr;
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
    uInt16		macID;	/* MAC index */
    uInt16		portID;	/* Port index */
    hwidbtype   *idb = (hwidbtype *)ptr;

    /* FIXME */
    uInt16      stnID = idb->fddi_stn_id;

    fddi_cbuginf("\nCSPInterrupt Handler:");

    MDisableCSPInterrupts(&SR);

    /* Handle PHY state changes & errors */
    for (portID=0; portID < PORTCFG (stnID, PortCt); portID++)
        CSPHandlePHYInterrupts (stnID, portID);

    /* Handle MAC state changes & errors */
    for (macID=0; macID < MAX_MAC_COUNT; macID++)
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
    hwidbtype *idb = (hwidbtype *)STADATA(stnID, hwIDB[0]); 

    fddi_cbuginf("\nInitCSPInterruptsMulti:");

    /* Install nmi interrupt handler */
    pas_install_pa_nmi_handler(idb->slot, CSPInterruptHandler, idb,
                               PA_NMI_PRIOR_HI);
    return (0);
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
Function:   Change the state of the port's interrupts. This function
            turns on/off all interrupts from the specified port.
Parameters:	stnID	= index of station to use.
            portID	= change interrupt state for PORT with index portID.
            state	= new state of interrupts.
Input:      state	= bit string indicating what interrupts are enabled:
            no bits set disables all interrupts.
            PORT_INT_QLS	enable QLS interrupt
            PORT_INT_HLS	enable HLS interrupt
            PORT_INT_MLS	enable MLS interrupt
            PORT_INT_SILS	enable SILS interrupt
            PORT_INT_NOISE	enable noise timer or NLS intr
            PORT_INT_EB	enable EB interrupt, if supported
Output:     PORT's interrupts are set as specified.
Return:	    No value returned.
Notes:      Enabling NOISE will enable either a hardware noise timer if
            it is available, otherwise the Noise line state interrupt
            is enabled.
            The EB interrupt is optional. If it is available it should
            be set appropriately.
            Setting the QLS, HLS and MLS interrupts will always occur
            together for backward compatibility.
Modification History:
*********************************************************************/
{
#define PORT_INT_BITS   ( INTR_TNE_EXPIRED | INTR_EBUF_ERR )
    leveltype	SR;
    volatile ulong *elm; 
    StnDCBType	*staData;
    ulong oldmask;

    fddi_cbuginf("\nSetPORTInterrupts:");

    /* Initialize local variables */
    staData = &STAINST (stnID);
    elm = staData->portRegs[portID];

    /* Disable CMT interrupts */
    MDisableCSPInterrupts(&SR);

    oldmask = elm[INTR_MASK_REG]; 
    oldmask &= ~PORT_INT_BITS;

    oldmask |= ( state & PORT_INT_NOISE) ? INTR_TNE_EXPIRED : 0;
    oldmask |= ( state & PORT_INT_EB) ? INTR_EBUF_ERR : 0;

    elm[INTR_MASK_REG] = oldmask;

    MRestoreCSPInterrupts(&SR);

    return;
}

void
#ifdef __STDC__
CSPHandlePHYInterrupts (uInt16 stnID, uInt16 portID)
#else
CSPHandlePHYInterrupts (stnID, portID)
	uInt16		stnID;		/* Station index */
	uInt16		portID;		/* Port index */
#endif
/*********************************************************************
Function:   Process interrupts for a PHY.
Parameters: portID   = PHY to process.
            phyICR  = current interrupt conditions.
Input:      Reads hardware status.
Output:     Hardware registers cleared.
            Posts signals for CMT.
Return:     None.
Notes:      Interrupts must be disabled before calling this function.

*********************************************************************/
{
    mif68840_instance *ds;
    volatile uInt32   *elm;	
    register uInt32	  event_reg;
    hwidbtype		  *idb;
    fddisb            *fsb;

    /* Initialize local variables */
    elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);
    ds  = (mif68840_instance *)STADATA (stnID, portData[portID]);
    idb = (hwidbtype *)STADATA(stnID, hwIDB[0]); 

    event_reg = elm[INTR_EVENT_REG];

    /* Mask off top 16 bits */
    event_reg &= 0xffff;
	
    fddi_cbuginf("\nPHYInterrupt: port=0x%02x, event_reg = 0x%04x", 
        portID, event_reg);

    /* Check if this elm generated any interrupt */
    if (!event_reg)
        return;

    if(event_reg & INTR_PCM_ENABLED)
        SendSignalMulti(stnID, SIG_PCM_ENABLED, portID, (uInt32) 0);
    if(event_reg & INTR_PCM_BREAK)
        SendSignalMulti(stnID, SIG_PCM_BREAK, portID, (uInt32) 0);
    if(event_reg & INTR_SELF_TEST)
        SendSignalMulti(stnID, SIG_PCM_SELF_TEST, portID, (uInt32) 0);
    if(event_reg & INTR_TRACE_PROP) {
        SendSignalMulti(stnID, SIG_PCM_TRACE_PROP, portID, (uInt32) 0);
        fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
		fsb->fddi_trace_count++;
        idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
	}
    if(event_reg & INTR_PCM_CODE)
        SendSignalMulti(stnID, SIG_PCM_CODE, portID, (uInt32) 0);

    if (event_reg & INTR_LE_CTR) {

        /* if count completed, set flag */
        if (LEMInterrupt(stnID, portID))
            SendSignalMulti(stnID, SIG_LEM_Threshhold, portID, (uInt32) 0);
    }

    /*
     *   This line checks the NP_ERR bit. The information is
     *   not used anywhere else. This line provides a place for
     *   a debugging breakpoint.
     */
    if (event_reg & INTR_NP_ERR)
        ds->np_err++;

    if (event_reg & INTR_EBUF_ERR)  {
        ds->ebuf_err++;
        MIBDATA(stnID, mib->portBase[portID].hwData.EBError_Ct) +=1;
    }
}

static void end_beacon (mif68840_instance *ds, volatile uInt32 *mac)
{
    uInt32		regval;

    fddi_cbuginf("\nend_beacon:");

    /* end directed beacon */
    /* Change FSI_BEACON to internally generated beacons */
    regval = mac[CONTROL_REG_B];
    regval &= ~FSI_BEACON;
    mac[CONTROL_REG_B] = regval;

    /* Disable beacon channel 0 and enable normal transmit channel 3 */
    ds->mif68840_regaddr->ifddi[FCR] =  WRITE_MACIF_TRANSMIT_EN_3;

    /* Set current beacon type to BCN_CLAIM_FAILED */
    ds->curBeaconType = BCN_CLAIM_FAILED;

    /* Reset ds->beaconing flag. */
    ds->beaconing = CLEAR;
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
Function:   Process MAC interrupt.
Parameters: None
Input:      Reads hardware status.
Output:     Hardware register cleared to current line state.
            Posts signal for RMT.
Return:     None.
Notes:      Interrupts must be disabled.

*********************************************************************/
{
    mif68840_instance *ds;
    volatile uInt32   *mac;	
    register uInt32	  intr_a, intr_b;
    uInt32			  newmask, ringOp, macResult, regval;
    hwidbtype		  *idb;
    fddisb            *fsb;

    /* Initialize local variables */
    idb = (hwidbtype *)STADATA(stnID, hwIDB[0]); 
    ds  = (mif68840_instance *) STADATA (stnID, macData[0]);
    mac = (volatile uInt32 *)STADATA (stnID, macRegs[macID]);

    macResult = 0;
    intr_a = mac[INTERRUPT_EVENT_REG_A] & 0xffff;
    intr_b = mac[INTERRUPT_EVENT_REG_B] & 0xffff;

    fddi_cbuginf("\nMACInterrupt:intr_a=0x%04x, intr_b=0x%04x",intr_a,intr_b);

    if (!(intr_a | intr_b))
        return;

    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);

    /* Check for claim. */
    newmask = 0;
    if (intr_a & CLAIM_RCVD) {
        fsb->fddi_claims++;

        if (intr_b & MY_CLAIM){
   		    macResult |= MAC_My_Claim;
       	    newmask |= MY_CLAIM;
        }

        /* Check for invalid claim received.  */
        if (intr_b & BAD_T_BID) {
            macResult |= MAC_TBid_TReq_Mismatch;
            newmask |= BAD_T_BID;
            ds->invalidClaim = SET;
        }
    }

    /* Check for beacon */
    if (intr_a & BEACON_RCVD) {
        fsb->fddi_beacons++;

    /* If we were sending directed beacons then stop. */
        if (ds->beaconing) 
		    end_beacon(ds, mac);

        if(intr_b & MY_BEACON) {
            macResult |= MAC_My_Beacon;
            newmask |= MY_BEACON;
        }
        
        if (intr_b & OTHER_BEACON){
            macResult |= MAC_Other_Beacon;
            newmask |= OTHER_BEACON;
        }

        /* clear mask of interrupts already seen */
        regval = mac[INTERRUPT_MASK_REG_B] &  ~newmask;
        mac[INTERRUPT_MASK_REG_B] = regval;  
    }


    /* Check for timer events.  */
    if (intr_a & RCVRY_FAIL)
        macResult |= MAC_TRT_In_T4T5;

    if (intr_a & TVX_EXPIR) {
        /* TVX expired, update mib counter.  */
	    MIBDATA(stnID, mib->macBase[macID].hwData.TvxExpired_Ct) +=1;
    }

    /* Check change in Ring_Op.  */
    if (intr_a & RING_OP_CHNG) {
        ringOp = mac[TRANSMIT_STATUS_REG] & RING_OP;
        if (ringOp) {
            ulong tneg_a, tneg_b;

            idb->counters.transitions++;

            /* Update database to reflect RingOP. */
            GET_TIMESTAMP(fsb->fddi_operational);

            tneg_a = mac[NEGOTIATED_TTRT_A_REG] & 0xffff;
            tneg_b = mac[NEGOTIATED_TTRT_B_REG] & 0xffff;
            tneg_b |= 0xffffff00; 

            /* This gives tneg in 80 ns. units */
            tneg_b = -((tneg_b << 16) | tneg_a);

            /* This gives  idb->fddi_trt_neg in microseconds */
            fsb->fddi_trt_neg = (tneg_b * 80) / 1000;

            /* Clear the invalid claim flag.  */
            ds->invalidClaim = CLEAR;

            /* Clear other MAC events since ring is operational.  */
            macResult = 0;

        } else {
            /* Update database to reflect RingNOP. */
            TIMER_STOP(fsb->fddi_operational);
            fsb->fddi_trt_neg = 0;

            idb->counters.transitions++;
        }
        SendSignalMulti(stnID, SIG_Ring_OP, macID, (uInt32)ringOp);
    }

    /*
     *   Check for frame counter overflows.  Note that this only
     *   works if the NOTEALLFRAMES is not set in MAC_CONTROL_A.
     */
    if (intr_a & FRAME_RCVD) {
        MIBDATA(stnID, mib->macBase[macID].hwData.Frame_Ct) +=65536;
    }

    /*
     * Check for lost and error (including CRC) counts.
     */
    if (intr_a & FRAME_ERR) {
        regval = mac[LOST_AND_ERROR_COUNT_REG] & 0xffff;

        /* Update lost count */
        MIBDATA(stnID, mib->macBase[macID].hwData.Lost_Ct) +=
            (regval & 0xff00) >> 8;

        /* Update error (including CRC) count */
        MIBDATA(stnID, mib->macBase[macID].hwData.Error_Ct) +=
            (regval & 0xff);
    }

    if(intr_b & NOT_COPIED) {
        idb->counters.input_resource++;
        /*
         * The ignore interrupt coming in at the NMI level will
         * slaughter us.  Set a flag and shut it off.
         * If we ever reach process level, the periodic function
         * will revive it. Sorry Curt, the ignore count goes kaput here.
         */
        ds->mif68840_regaddr->mac[INTERRUPT_MASK_REG_B] = 0;
        ds->rxovr_4 = TRUE;
    }

    if (intr_a & LATE_TKN) {
        MIBDATA(stnID, mib->macBase[macID].hwData.Late_Ct) +=1;
    }

    if (macResult)
    SendSignalMulti(stnID, SIG_MAC_Interrupt, macID, (uInt32) macResult);

    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
    return;
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

/********************************************************************
    Miscellaneous routines required by the hwpcm files
********************************************************************/
uInt16
#ifdef __STDC__
WritePHYRegMulti (uInt16 stnID, uInt16 portID, uInt16 reg, uInt16 val)
#else
WritePHYRegMulti (stnID, portID, reg, val)
		uInt16 stnID;
		uInt16 portID;
		uInt16 reg;
		uInt16 val;
#endif
/*********************************************************************
Function:   Write a 16 bit value to a register on the ELM chip
Parameters: phy = Phy number to write value to.
            reg = Register number to write value to.  This value is the
            byte address from the start of the phy. Please refer to
            motomac.h for definitions for this value.
            val = 16-bit value to write.
Input:      None.
Output:     Sets register on ELM.
Return:     None.
*********************************************************************/
{   
    volatile uInt32 *elm;

    fddi_cbuginf("\nWritePHYReg: port=0x%02x, reg=0x%04x, val=0x%04x", 
        portID, reg, val);

    /* Initialize local variables */
    elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);
    elm[reg] = val;
    return(val);
}

uInt16
#ifdef __STDC__
ReadPHYRegMulti (uInt16 stnID, uInt16 portID, uInt16 reg)
#else
ReadPHYRegMulti (stnID, portID, reg)
		uInt16 stnID;
		uInt16 portID;
		uInt16 reg;
#endif
/*********************************************************************
Function:   Read a 16 bit value from a register on the ELM chip
Parameters: phy = Phy number to read value from.
            reg = Register number to read value from.  This value is the
            byte address from the start of the phy. Please refer to
            motomac.h for definitions for this value.
Input:      None.
Output:     None.
Return:     Returns the value from the requested ELM register.
*********************************************************************/
{
    volatile uInt32 *elm;
    uInt16 regval;
	

    /* Initialize local variables */
    elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);

    regval = elm[reg] & 0xffff;

    fddi_cbuginf("\nReadPHYReg: port=0x%02x, reg=0x%04x, val=0x%04x",
        portID, reg, regval);

    return(regval);
}

/*********************************************************************
	Timer Interface Routines
*********************************************************************/

/* Process mif68840_CSPTimeout as a high priority process */
process mif68840_CSPTimer_process (void)
{
    leveltype SR;

    process_watch_mgd_timer(&mif68840_CSPTimer, ENABLE);
    while (TRUE) {
 
        /* Sleep until there is a CMT timer time-out */
        process_wait_for_event();
 
        mgd_timer_stop(&mif68840_CSPTimer);

        /* Check for CSP time-outs  */
        ServiceCSPTimer();

	    /* Process CSP signals (if any) */
        if (!EmptySignalQueue()) {
            MDisableCSPInterrupts(&SR);
            ProcessCSP(&SR);
            MRestoreCSPInterrupts(&SR);
        }
    }
}

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
    uInt32 value;

    /* Get CSP RT timer due time */
    value = 1000 * mgd_timer_left_sleeping(&mif68840_CSPTimer);

    return(value);
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
        mgd_timer_stop(&mif68840_CSPTimer);
    else
        mgd_timer_start(&mif68840_CSPTimer, 1 + interval/1000);
    return;
}

/***************** End of Timer routines ********************************/

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
    return(TRUE);
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
    mif68840_instance *ds;
    leveltype   level;
    ulong 		bypass;
 
    /* initialize local variables */
    ds = (mif68840_instance *) STADATA (stnID, macData[0]);

    MDisableCSPInterrupts(&level);

    /* Get previous state of csr */
    bypass = ds->mif68840_regaddr->csr & ~(EN_OPTICAL_BYPASS_SEC |
									EN_OPTICAL_BYPASS_PRI);
    if (bypassState == DEINSERTED) {
        fddi_cbuginf("\nIn BypassRequestMulti, bypass state is DEINSERT");
	    ds->mif68840_regaddr->csr = bypass;
    } else {
        fddi_cbuginf("\nIn BypassRequestMulti, bypass state is INSERT");
        ds->mif68840_regaddr->csr = bypass | (EN_OPTICAL_BYPASS_SEC | 
                                EN_OPTICAL_BYPASS_PRI );
    }
    MRestoreCSPInterrupts(&level);
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
Function:   Set the configuration hardware for the specified
            M port to the CFM configuration given.
Parameters:	stnID		= index of station to use.
            portPtr		= port element to use
            cfmState	= CFM state for hardware configuration as
                          defined in the M port CFM state machine.
Input:      None.
Output:     None.
Return:     No value returned.
Notes:      This routine may be stubbed out if the software is not
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
                IF (new configuration required) THEN
                    Read current xmit line state
                    IF (xmitLS == PDRLS) THEN
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
    volatile ulong *elma, *elmb; 
    ulong		phyAConfig, phyBConfig, ledA, ledB;
    StnDCBType	*staData;
    hwidbtype	*idb;
    mif68840_instance *ds;
    ulong       csr;

    fddi_cbuginf("\nConfigureAttachMulti:");

    /* Initialize local variables */
    staData = &STAINST (stnID);
    elma = staData->portRegs[0];
    elmb = staData->portRegs[1];
    idb = staData->hwIDB[0];
    ds = STADATA (stnID, macData[0]);
    phyAConfig = phyBConfig = ledA = ledB = 0;
    /* Get previous state of csr */
    csr = ds->mif68840_regaddr->csr & ~(LED_PHYA | LED_PHYB | LED_DUALHOME);

    /* Program Data Path according to new configuration */
    switch (CF_State)
    {
    case CF_isolated:
        phyAConfig = elma[CNTRL_A_REG];
        phyBConfig = elmb[CNTRL_A_REG];
        phyAConfig |= (SC_BYPASS_MASK | SC_REM_LOOP_MASK);
        phyBConfig |= (SC_BYPASS_MASK | SC_REM_LOOP_MASK);
        break;

    case CF_wrap_a:
    case CF_c_wrap_a:
    case CF_wrap_s:
    case CF_c_wrap_s:
        phyAConfig = elma[CNTRL_A_REG];
        phyBConfig = elmb[CNTRL_A_REG];
        phyAConfig &= ~(SC_BYPASS_MASK | SC_REM_LOOP_MASK);
        phyBConfig |= (SC_BYPASS_MASK | SC_REM_LOOP_MASK);
        csr |= LED_PHYA; 
	if (CSPDATA(stnID, portData[0].PC_Neighbor) == PT_m) {
	   csr |= LED_DUALHOME;
	}
        break;

    case CF_wrap_b:
    case CF_c_wrap_b:
        phyAConfig = elma[CNTRL_A_REG];
        phyBConfig = elmb[CNTRL_A_REG];
        phyAConfig |= (SC_BYPASS_MASK | SC_REM_LOOP_MASK);
        phyBConfig &= ~(SC_BYPASS_MASK | SC_REM_LOOP_MASK);
        csr |= LED_PHYB;
	if (CSPDATA(stnID, portData[1].PC_Neighbor) == PT_m) {
	   csr |= LED_DUALHOME;
        }

        break;

    case CF_thru:
        phyAConfig = elma[CNTRL_A_REG];
        phyBConfig = elmb[CNTRL_A_REG];
        phyAConfig &= ~(SC_BYPASS_MASK | SC_REM_LOOP_MASK);
        phyBConfig &= ~(SC_BYPASS_MASK | SC_REM_LOOP_MASK);
        csr |= (LED_PHYA | LED_PHYB);
        break;

    default:
        break;
    }

    /* Print CFM transition message */
    if (staData->prevCFState != CF_State) {
        errmsg(&msgsym (FDDISTAT, LINK), idb->hw_namestring,
            get_smt_string(&SMTMsg.cf_state, CF_State));

    /* Report Interface State Change as appropriate */
    if (idb->state != IDBS_ADMINDOWN) {
        if ((CF_State != CF_isolated) && (idb->state != IDBS_UP)) {
            net_cstate(idb, IDBS_UP);
            mif68840_lineproto_change(idb, TRUE);

            /* FIXME
            rif_update(hwidb, 0, hwidb->bit_swapped_hardware, NULL, 
                NULL, RIF_INTERFACE);
             */
        } else if ((CF_State == CF_isolated) && (idb->state == IDBS_UP)) {
            net_cstate(idb, IDBS_DOWN);
            mif68840_lineproto_change(idb, TRUE);
        }
    }
    /* Update CFM state history variable */
    staData->prevCFState = CF_State;
    }

    /* Write new configuration to Ports A and B */
    elma[CNTRL_A_REG] = phyAConfig;
    elmb[CNTRL_A_REG] = phyBConfig;

    /* Write LEDs */
    ds->mif68840_regaddr->csr = csr;
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
    volatile ulong *elm; 
    StnDCBType	*staData;
    ulong fot;

    /* Initialize local variables */
    staData = &STAINST (stnID);
    elm = staData->portRegs[portID];

    /* Disable CMT interrupts */
    MDisableCSPInterrupts(&SR);

    /* Set Fiber Optic Transmit to new state */
    fot = elm[CNTRL_A_REG];
    fot != (state ? 0 : FOT_OFF_MASK); 
    elm[CNTRL_A_REG] = fot;

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
    leveltype	SR;
    volatile ulong *elm; 
    StnDCBType	*staData;
    ulong ls;

    /* Initialize local variables */
    staData = &STAINST (stnID);
    elm = staData->portRegs[portID];

    /* Disable CMT interrupts */
    MDisableCSPInterrupts(&SR);

    /*
     * Read current line state.
     */
    ls = (elm[STATUS_A_REG]  >> 5) & 0x7;
    ls = 1 << ls;

    /* restore interrupts */
    MRestoreCSPInterrupts(&SR);

    /* return line state history read */
    return(ls);
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
    leveltype	SR;
    volatile ulong *elm; 
    StnDCBType	*staData;
    ulong new_ls;

    /* Initialize local variables */
    staData = &STAINST (stnID);
    elm = staData->portRegs[portID];

    MDisableCSPInterrupts(&SR);

    /* Set new line state */
    switch (ls)
    {
	case QLS: /* Transmit Quiet Line State */
	    new_ls =  MAINT_LS_QLS;
	    break;

	case HLS: /* Transmit Halt Line State */
	    new_ls = MAINT_LS_HLS;
	    break;

	case ILS: /* Transmit Idle Line State */
	    new_ls = MAINT_LS_ILS;
	    break;

	case MLS: /* Transmit Master Line State */
	    new_ls = MAINT_LS_MLS;
	    break;

	case PDRLS: /* Transmit PHY Request data */
	    new_ls = MAINT_LS_PDR;
	    break;

	default:
	    /* Do NOT change current Line State */
		MRestoreCSPInterrupts(&SR);
		return;
	}

	/* Write new line state */
	new_ls |=  elm[CNTRL_B_REG]  & ~MAINT_LS_MASK;

	elm[CNTRL_B_REG] = new_ls;

	MRestoreCSPInterrupts(&SR);
	
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
    leveltype	SR;
    volatile ulong *mac; 
    StnDCBType	*staData;
    ulong imaska, imaskb;

    /* Initialize local variables */
    staData = &STAINST (stnID);
    mac = staData->macRegs[macID];

MDisableCSPInterrupts(&SR);

    /* Read current settings */
    imaska = mac[INTERRUPT_MASK_REG_A];
    imaskb = mac[INTERRUPT_MASK_REG_B];

    /* Set new interrupt masks */
    MChangeBits (imaska, RING_OP_CHNG, RING_OP_CHNG);
 
    MChangeBits (imaskb, MY_CLAIM,
        state ? MY_CLAIM : 0); 
    MChangeBits (imaskb, MY_BEACON,
        state ? MY_BEACON : 0);
    MChangeBits (imaskb, OTHER_BEACON,
        state ? OTHER_BEACON : 0);

    MChangeBits (imaskb, BAD_T_BID,
        state ? BAD_T_BID : 0);

    /* Write new settings.  */
    mac[INTERRUPT_MASK_REG_A] = imaska;
    mac[INTERRUPT_MASK_REG_B] = imaskb;

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
    leveltype	SR;
    volatile ulong *mac; 
    StnDCBType	*staData;
    ulong regval;

    /* Initialize local variables */
    staData = &STAINST (stnID);
    mac = staData->macRegs[macID];

    MDisableCSPInterrupts(&SR);

    /* Read current settings */
    regval = mac[CONTROL_REG_B]; 
    regval &= ~0x0300;
    regval |= MAC_RESET;
    mac[CONTROL_REG_B] = regval;

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
    leveltype	SR;
    volatile ulong *mac; 
    StnDCBType	*staData;
    ulong regval;

    /* Initialize local variables */
    staData = &STAINST (stnID);
    mac = staData->macRegs[macID];

    MDisableCSPInterrupts(&SR);

    /* Read current settings */
    regval = mac[CONTROL_REG_B]; 
    regval &= ~0x0300;

    /* Force Claim */
    regval |= RESET_CLAIM;
    mac[CONTROL_REG_B] = regval;

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
    leveltype SR;
    mif68840_instance	*ds;
    volatile ulong 		*mac;
    fddi_hdr	*fddiPtr;

    fddi_cbuginf("\nSetBeaconMulti:");
    /* Initialize local variables */
    ds = (mif68840_instance *) STADATA(stnID, xferData[macID]);
    mac = (volatile ulong *) STADATA(stnID, xferRegs[macID]);

    PUTLONG(ds->bcn_ptr, PREMAC | 0x00010000);
    fddiPtr = (fddi_hdr *) (ds->bcn_ptr + 4);

    MDisableCSPInterrupts(&SR);

    if (beaconType == BCN_DIRECTED) {
        /*
         * Setup for directed beacons
         */

        ds->curBeaconType = BCN_DIRECTED;
        ds->beaconing = SET;

        /* Build FDDI FC,DA,SA fields */
        fddiPtr->fc = 0xc2;
        ieee_swap (DA, &fddiPtr->daddr[0]);
        ieee_copy (HWIDBDATA (stnID, macID, hardware),
            &fddiPtr->saddr[0]);

        /* Build first 4 info field bytes */
        fddiPtr->data[0] = BCN_DIRECTED;
        fddiPtr->data[1] = 0;
        fddiPtr->data[2] = 0;
        fddiPtr->data[3] = 0;

        /* Copy info (UNA address) in next bytes */
        bcopy (beaconInfo, &fddiPtr->data[4], infoLength);

        /* Update datagram size information */
        ds->bcn_ptr_size = 3+FDDI_FC+FDDI_DA+FDDI_SA+4+infoLength;
    }

    /* Determine if we need to stop the directed beaconing.  */
    if( (beaconType != BCN_DIRECTED) && (ds->beaconing))
        end_beacon(ds, mac);

    MRestoreCSPInterrupts(&SR);
    return;
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
    leveltype SR;
    mif68840_instance	*ds;
    volatile ulong 		*mac;
    ulong regval;

    /* Initialize local variables */
    ds = (mif68840_instance *) STADATA(stnID, xferData[macID]);
    mac = (volatile ulong *) STADATA(stnID, xferRegs[macID]);

    /* Disable CMT Interrupts */
    MDisableCSPInterrupts(&SR);

    /* If going to directed beacon then fill the FSI with beacons. */      

    if (ds->curBeaconType == BCN_DIRECTED) {

        fddi_cbuginf("\nBeaconRequestMulti: Directed Beacon");
 
        ds->bcn_rd_ptr->tx_buffer_addr=MIF68840_BUF_ADDR(ds->bcn_ptr + 1);
        ds->bcn_rd_ptr->tx_status_length = MIF68840_TX_FIRST_LAST_BUFFER |
            ds->bcn_ptr_size; 
    
        /* start directed beacon */
        /* Disable normal channel 3 and enable beacon channel 0 */
        FCR_IS_FREE;
        ds->mif68840_regaddr->ifddi[FCR] =  WRITE_MACIF_TRANSMIT_EN_0;

        /* Change FSI_BEACON to externally generated beacons */
        regval = mac[CONTROL_REG_B];
        regval |= FSI_BEACON;
        mac[CONTROL_REG_B] = regval;
  
        /* Do ring Ready to channel 0*/
        FCR_IS_FREE;
        ds->mif68840_regaddr->ifddi[FCR] = WRITE_RING_RDY_REG_0;
    }
	
    /* Force beacon.  */
   	regval = mac[CONTROL_REG_B];
   	regval &= ~0x0300;
    regval |= RESET_BEACON;
   	mac[CONTROL_REG_B]  = regval;

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
ChangeMACTBidMulti (uInt16 stnID, uInt16 macID, uInt32 newTBid)
#else
ChangeMACTBidMulti (stnID, macID, newTBid)
        uInt16                  stnID;
        uInt16                  macID;
        uInt32					newTBid;
#endif
/*********************************************************************
Function:   Change the MAC's bid T_Req value. This function should
        change the value in the claim frame AND in the MAC.
Parameters: macID   = index of MAC to start beaconing.
        newTBid = new bid value for T_Req.
Input:
Output:
Return:     No value returned.
Notes:
Modification History:
*********************************************************************/
{
    leveltype	SR;
    volatile ulong *mac; 
    StnDCBType	*staData;

    /* Initialize local variables */
    staData = &STAINST (stnID);
    mac = staData->macRegs[macID];

    MDisableCSPInterrupts(&SR);

    newTBid = (newTBid >> 8) & 0xffff;

    mac[TREQ_REG] = newTBid;

    MRestoreCSPInterrupts(&SR);
    return;
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
Function:   This routine implements the MAC CFM footnotes and
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
Input:      None.
Output:     MAC is configured onto specied path the appropriate
            operational values.
Return:	    None.
Notes:      TVXLB, T_MaxLB and MaxT_Req are presented in 2s COMPLEMENT
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
    volatile ulong *mac; 
    StnDCBType	*staData;
    hwidbtype   *idb;
    ulong macMode, tmp;
    fddisb      *fsb;


    /* Initialize local variables */
    staData = &STAINST (stnID);
    mac = staData->macRegs[macID];
    idb = staData->hwIDB[0];
    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);

    /* Disable CMT interrupts */
    MDisableCSPInterrupts(&SR);

    macMode = mac[CONTROL_REG_A];

    if (state == ENABLE)
    {
        macMode &= ~MAC_ON;
        mac[CONTROL_REG_A] = macMode;

        /* Set TVX and combine with T_Max into one register */
        tmp = (fsb->fddi_tvx * 100) >> 11; 
        tmp = 1 + ~tmp;
        tmp <<= 8;

        /* Set T_Max to 167.7721 ms. */
        tmp |= 0xe0;

        mac[TVX_VALUE_AND_TMAX_REG] = tmp;

        /* Set T_Req */
        tmp = (fsb->fddi_trt * 100) >> 11;
        tmp = 1 + ~tmp;

        MIBDATA (stnID, mib->macBase[macID].hwData.T_Req) = tmp;
        mac[TREQ_REG] = tmp;
        macMode = mac[CONTROL_REG_A];
        macMode |= MAC_ON;
    }
    else
    {
        macMode &= ~MAC_ON;
    }

    mac[CONTROL_REG_A] = macMode;

    /* Restore CMT Interrupts */
    MRestoreCSPInterrupts (&SR);

    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
    return;
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
    volatile ulong *mac; 
    StnDCBType	*staData;
    ulong rstatus;

    /* Initialize local variables */
    staData = &STAINST (stnID);
    mac = staData->macRegs[macID];

    rstatus = mac[RECEIVE_STATUS_REG];

    if (rstatus & R_FLAG)
        return(TRUE);
    return(FALSE);
}

/*********************************************************************
    LEM Interface Routines
*********************************************************************/

/*
*   Because the LEM counter only handles 8 bits and counts up,
*   these interface routines extend the registers to 16-bit values.
*   To support the XLNT Manager LEM operation, these routines must
*   be able to handle 16-bit threshold and counter values with a
*   count-down counter.
*/
   
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
Function:   Set the LEM hardware counter to cause an interrupt when
            it counts threshold number of errors.   
Parameters: stnID       = index of station to use.
        portID      = index of PORT whose link is to be monitored.
        threshold   = number of error to count before giving
                    an interrupt.
Input:      None.
Output:     None.
Return:     None.
Modification History:
*********************************************************************/
{  
    leveltype         SR;
    volatile uInt32   *elm;	
    uInt16 			  count, tmp;
    mif68840_instance *ds;

    /* Initialize local variables */
    elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);
    ds  = (mif68840_instance *)STADATA(stnID, portData[portID]);
    ds->remaining[portID] = threshold;

    /* Disable CMT interrupts */
    MDisableCSPInterrupts(&SR);

    /* counter max is 8 bits */
    count = (ds->remaining[portID] > 0xFF) ? 0xFF : ds->remaining[portID];
    ds->remaining[portID] -= count;

    /* Clear current LEM counter. */
    tmp = elm[LINK_ERR_CTR_REG];

    /* set register */
    elm[LE_THRESHOLD_REG] = count;

    MRestoreCSPInterrupts (&SR);

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
Function:   Read the LEM hardware counter.
Parameters: stnID   = index of station to use.
            portID  = index of PORT whose link is being monitored.
Input:      None.
Output:     None.
Return:     Number of errors left to count before threshold is reached.
Modification History:
*********************************************************************/
{
    leveltype 		 SR;
    uInt16  		 count,thresh,rem;
    volatile uInt32  *elm;
    mif68840_instance *ds;

    /* Initialize local variables */  
    elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);
    ds  = (mif68840_instance *)STADATA(stnID, portData[portID]);

    /* Disable CMT interrupts */
    MDisableCSPInterrupts(&SR);

    /* get current count, this clears count to zero */
    count = elm[LINK_ERR_CTR_REG];
    /* get current threshold */
    thresh = elm[LE_THRESHOLD_REG];
    /* calculate total remaining value */
    thresh = thresh - count;
    rem = ds->remaining[portID] + thresh;
    /*
        Reset threshold because the counter was reset to zero when we
        read it.
    */

    elm[LE_THRESHOLD_REG] = thresh;

    MRestoreCSPInterrupts (&SR);

    return (rem);
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
    uInt16           count;
    volatile uInt32  *elm;
    mif68840_instance *ds;
 
    /* Initialize local variables */
    elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);
    ds  = (mif68840_instance *)STADATA(stnID, portData[portID]);

    /* 
    *   If threshold still has more errors to count,
    *   then reset the threshold register.
    */
    if (ds->remaining[portID])
    {
        /* counter max is 8 bits */
        count = (ds->remaining[portID] > 0xFF) ? 0xFF : ds->remaining[portID];
        ds->remaining[portID] -= count;  

        /* set register if not at max */
        if (count != 0xFF)
            elm[LE_THRESHOLD_REG] = (count & 0xFF);
       	return (FALSE);
    }

    /* Otherwise, this link has failed.  */
    return (TRUE);
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
    volatile uInt32  *elm;
    ulong regval;
 
    /* Initialize local variables */
    elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);

    /* Disable CMT interrupts */
    MDisableCSPInterrupts(&SR);

    regval = elm[INTR_MASK_REG];
    /*
     *   If enabling interrupt, then clear current interrupt
     *   status and restart counter.
     */
    if (state)
        regval |= INTR_LE_CTR;
    else
        regval &= ~INTR_LE_CTR;

    elm[INTR_MASK_REG] = regval;

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
    leveltype	SR;
    volatile uInt32  *elm;
    ulong regval;
 
    /* Initialize local variables */
    elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);

    /* Disable CMT interrupts */
    MDisableCSPInterrupts(&SR);

    regval = elm[CNTRL_B_REG];

    /* set PC_LOOP */
    regval &= ~PC_LOOP_MASK;
    elm[CNTRL_B_REG] = (regval | PC_LOOP_PDR_LOOPBACK);

    /* Enable interrupts */
    MRestoreCSPInterrupts(&SR);

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
    leveltype	SR;
    volatile uInt32  *elm;
    ulong regval;
 
    /* Initialize local variables */
    elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);

    /* Disable CMT interrupts */
    MDisableCSPInterrupts(&SR);

    regval = elm[CNTRL_B_REG];

    /* clear PC_LOOP */
    regval &= ~PC_LOOP_MASK;
    elm[CNTRL_B_REG] = regval;

    /* Enable interrupts */
    MRestoreCSPInterrupts(&SR);

    return;
}


/* StartScrubbingMulti is take care of in ../if/if_xdi.c */


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
    uInt32		result;
    leveltype 	SR;
    uInt32 		imask, portID;
    hwidbtype 	*hwidb;
    mif68840_instance	*ds;
    volatile ulong *mac, *elm;


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

    result = InitCSPMIBDataMulti(stnID);

    if (result == 0)
        /* set Path_Test value */
        CSPDATA (stnID, stationData.Path_Test)
            = StationPathTestMulti (stnID) ? PT_Passed : PT_Failed;

    /* Initialize local variables */
    ds  = (mif68840_instance *)STADATA (stnID, portData[0]);
    hwidb = (hwidbtype *)STADATA(stnID, hwIDB[0]);
    mac = (volatile uInt32 *)STADATA (stnID, macRegs[0]);

   /*
    * Initialize CMT hardware for normal operation
    */

    /* Disable Interrupts */
    MDisableCSPInterrupts(&SR);

    /* Set interrupt mask a */
    imask = BEACON_RCVD | CLAIM_RCVD | RING_OP_CHNG | FRAME_ERR | 
                FRAME_RCVD | TVX_EXPIR | LATE_TKN | RCVRY_FAIL;
    mac[INTERRUPT_MASK_REG_A] = imask;

    /* Set interrupt mask b */
    imask = NOT_COPIED;
    mac[INTERRUPT_MASK_REG_B] = imask;

    /* clear invalid claim flag */
    ds->invalidClaim = CLEAR;

    for (portID=0; portID < PORTCFG (stnID, PortCt); portID++) {

        elm = (volatile uInt32 *)STADATA (stnID, portRegs[portID]);

        /* Set CONTROL_REG_A */
        elm[CNTRL_A_REG] = 0;

        /* Set CONTROL_REG_B */
        elm[CNTRL_B_REG] = CONFIG_CNTRL_MASK |  LONG_MASK;

        /* Set PCM Timing registers */
        WritePHYRegMulti(stnID, portID, C_MIN_REG, 
                    (-3200 * 100/2048) & 0xffff);
        WritePHYRegMulti(stnID, portID, TL_MIN_REG,
                    (-STADATA(stnID, TLMin) * 100/2048) & 0xffff);
        WritePHYRegMulti(stnID, portID, TB_MIN_REG, 
                    (-8000 * 100/2048) & 0xffff);
        WritePHYRegMulti(stnID, portID, T_OUT_REG, 
                    (-100000 * 100/2048) & 0xffff);
        WritePHYRegMulti(stnID, portID, LC_SHORT_REG, 0xf676);
        WritePHYRegMulti(stnID, portID, T_SCRUB_REG, 0xff55);
        WritePHYRegMulti(stnID, portID, NS_MAX_REG,	0xf022);

        /* Set ELM Interrupt Registers */
        WritePHYRegMulti(stnID, portID, INTR_MASK_REG,
            INTR_NP_ERR | INTR_EBUF_ERR | INTR_PCM_ENABLED |
            INTR_PCM_BREAK |  INTR_SELF_TEST| INTR_TRACE_PROP |
            INTR_PCM_CODE);
    }

    /* Restore Interrupts */
    MRestoreCSPInterrupts(&SR);	
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
    MACHdrType *machdr;

    if (fddi_smt_packet_debug)
        buginf("\nIn SendSMTFrameMulti");

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
    machdr = (MACHdrType *)(p->datagramstart);
    p->datagramstart += EXTRA_FC_BYTES;
    p->if_output = HWIDBDATA (stnID, MACNum, firstsw);
    ieee_swap((uchar *)(machdr->DA), (uchar *)(machdr->DA));
    ieee_swap((uchar *)(machdr->SA), (uchar *)(machdr->SA));
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
    uInt32 result = RC_Success;	    /* return value */
    uInt16 macID;
    hwidbtype *idb;
    fddisb *fsb;
    mif68840_instance *ds;

    /* Initialize local variables */
    macID = macPtr->baseData.xdi.xdiIndex;
    idb = STADATA(stnID, hwIDB[macID]);
    ds  = INSTANCE(idb);
    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
    
    /*
     *	Read the appropriate MAC attribute.
     */
	switch (attr) {
        case fddiMACT_Req:
		    ((TLV32BitType *) buffer)->data = -fsb->fddi_trt;
            break;

        case fddiMACT_Neg:
            ((TLV32BitType *) buffer)->data = -fsb->fddi_trt_neg;
            break;

        case fddiMACT_Max:
            ((TLV32BitType *) buffer)->data = -DEFAULT_T_MAX;
            break;

        case fddiMACTvxValue:
            ((TLV32BitType *) buffer)->data = -fsb->fddi_tvx;
            break;

        case fddiMACFrame_Ct:
            MIBDATA(stnID, mib->macBase[macID].hwData.Frame_Ct) += 
                ((ds->mif68840_regaddr->mac[FRAME_COUNT_REG]) & 0xffff);
            ((TLV32BitType *) buffer)->data = 
                MIBDATA(stnID, mib->macBase[macID].hwData.Frame_Ct);
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
            ((TLV32BitType *) buffer)->data = 
                MIBDATA(stnID, mib->macBase[macID].hwData.Error_Ct);
            break;

        case fddiMACLost_Ct:
            ((TLV32BitType *) buffer)->data =
                MIBDATA(stnID, mib->macBase[macID].hwData.Lost_Ct); 
            break;

        case fddiMACTvxExpired_Ct:
            result = RC_NoParameter;
            break;

        case fddiMACNotCopied_Ct:
            ((TLV32BitType *) buffer)->data = idb->counters.input_resource;
            break;

        case fddiMACRingOp_Ct:
            ((TLV32BitType *) buffer)->data = idb->counters.transitions;
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
Function:   MIB access routine to get the attribute
            fddiMACLongGrpAddrs or fddiMACShortGrpAddrs.
Parameter:  stnID       = index of station to use.
            macPtr      = MAC entry in the MIB.
            bufSize     = maximum length of buffer paramData
                            measured in chars. This includes the
                            space used for the parameter header.
            paramData   = pointer to buffer containing the
                            attribute information to be removed
                            and used to hold attribute value returned.
Input:      paramData   = contains the attribute type and Get
                            request length represented in MIB TLV
                            encoding.
                        paramData->paramType contains the
                            attribute type.
                        paramData->MACINDEX contains the index
                            of the MAC to use.
                        &paramData->MACOTHER is the starting
                            address for the attribute data.
Output:     paramData   = holds the attribute value after in the MIB
                            TLV encoding specified in the SMT standard.
Return:     The value returned corresponds to the results codes used
            by the Parameter Manager Protocol:
                RC_Success
                    attribute retrieved successfully
                RC_NoParameter
                    attribute not supported
Notes:      GetMACAddresses() may only be used with attributes
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
Function:   Get a PATH attribute.
            Some of the PATH-related attributes are contained
            within the PATH or its associated hardware and software.
            To get the most current data, GetMIBPATHAttr() calls
            GetPATHDataMulti() to retrieve the data from the system itself.
Parameters: stnID   = station index to use.
            attr    = PATH attribute to retrieve.
            pathPtr = this is the PATH entry in the MIB.
            buffer  = pointer to a buffer to hold the requested
                        attribute value.
Input:      None.
Output:     Sets buffer to contain requested attribute.
Return:     The value returned corresponds to the results codes used
            by the Parameter Manager Protocol:
                RC_Success
                    attribute retrieved successfully
                RC_NoParameter
                    attribute not supported or attribute ID not known
*********************************************************************/
{
uInt32                  result = RC_Success;    /* return value */

    /*
    *   Read the appropriate PATH attribute.
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
        TLVParamType            *p;
#endif
/*********************************************************************
Function:   Set a PATH attribute.
            Many of the PATH-related attributes are contained
            within the PATH or its associated hardware and software.
            To set the data values, SetMIBPATHAttr() calls
            SetPATHDataMulti() to place the data into the
            appropriate location.
Parameters: stnID   = index of station to use.
            attr    = PATH attribute to set.
            pathPtr = entry in the MIB for the PATH to change.
            p       = pointer to a buffer holding the data value.
Input:      p       = attribute in TVL encoding.
Output:     Hardware or system software is set with new value.
            Additionally, the new value may be written to non-volatile
            memory. Some of the PATH attributes listed are managed by
            the MIB database and are passed to this routine only to be
            stored in non-volatile memory if desired.
Return:     The value returned corresponds to the results codes used
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
Function:   Get a PORT attribute.
            Many of the PORT-related attributes are contained
            within the PORT or its associated hardware and software.
            To get the most current data, GetMIBPORTAttr() calls
            GetPORTDataMulti() to retrieve the data from the system itself.
Parameters: stnID   = index of station to use.
            attr    = PORT attribute to retrieve.
            portPtr = entry in the MIB for this PORT.
            buffer  = pointer to a buffer to hold the requested attribute.
Input:      None.
Output:     Sets buffer to contain requested attribute.
Return:      The value returned corresponds to the results codes used
            by the Parameter Manager Protocol:
                RC_Success
                    attribute retrieved successfully
                RC_NoParameter
                    attribute not supported or not known
*********************************************************************/
{
    /* FIXME */
    return(0);
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
Function:   Set an PORT attribute.
            Although ALL PORT attributes are managed through the MIB
            database, an interface routine is required for storing
            the PORT attribute values into a non-volatile memory
            location. This routine DOES NOT change the MIB itself.
            It only provides a place for storing PORT attributes
            permanently in the system.
Parameters: stnID   = index of station to use.
            attr    = PORT attribute to set.
            portPtr = PORT element that has changed.
            p       = pointer to a buffer holding the data value.
Input:      p       = attribute in TLV encoding.
Output:     Non-volatile memory is changed to store the new value. This
            action is optional and all changes to standard and XDI PORT
            attributes may be ignored by this routine.
Return:     This routine returns RC_Success.
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

#ifdef USE_WHAT_STRING
static char             xdi_id[] = "@(#) if7_mibvenif.c if7_v2.2.1:cset.0009:3:3 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
    Management Information Base Module

    THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

    Vendor MIB Attribute Processing Module

    File:           if7_mibvenif.c
    Created:        04/30/92

    Version:        if7_v2.2.1      Mon Mar  1 16:49:14 1993
    Last Modified:  cset.0009       10/23/92

    Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

    This module provides access to the FDDI MIB Vendor Attributes
    specified by the implementer.

    Modification History:
    *** Created for SMT 7.2 ***
*********************************************************************/

/*
#include    "smtdefs.h"
#include    "smttypes.h"
#include    "smtmacro.h"
#include    "fddihdr.h"
#include    "mibdefs.h"
#include    "mibvend.h"
#include    "mibtypes.h"
#include    "mibhdr.h"
#include    "mibglbl.h"
#include    "smtmsg.h"
#include    "mibproto.h"
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
                p       = pointer to buffer containing the
                          requested attribute information and used to
                          hold attribute value returned.
               smtPtr   = address of SMT attribute data.
Input:         p        = contains the requested attribute ID and object
                          index (if required) using the MIB TLV encoding
                          for a Get Request (see SMT standard).
Output:        p        = holds the attribute value requested using the
                          MIB TLV encoding specified in the SMT standard.
Return:        The value returned corresponds to the results codes used
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
Notes:         Entry to the MIB must be from GetMIBAttr() in mibproc.c to
               insure correct attribute values and a clean response buffer.
               If no vendor-specific attribute are supported, then this
               routine may simply return RC_NoParameter.
*********************************************************************/
{
uInt32  result = RC_Success;   /* return code */

        /*
         * Check if buffer is minimum size. Header + data Length is the
         * same for 8, 16, and 32 bit data types.
         */
        if (bufSize < (sizeof (TLVHdrType) + sizeof (TLV32BitType)
                        + sizeof (TLV8BitType)))
            return (RC_BUFFER_ERROR);

        /*
         * Reduce buffer size by header amount and vendor ID.
         */
        bufSize -= (sizeof (TLVHdrType) + sizeof (TLV32BitType));

        /*
         * Length is the same for 8, 16, and 32 bit data types which is
         * the most common size.
         */
        p->paramLen = sizeof (TLV8BitType);

        /*
         * Get value from MIB.
         *
         * if-else is used here because a switch() statement requires an
         * int. The vendor IDs will be 32-bits which may not be ints on
         * some machines.
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
                p       = pointer to buffer containing the requested
                          attribute information and used to hold attribute
                          value returned.
                macPtr  = pointer to MAC's attribute structure in the MIB.
Input:          p       = contains the requested attribute ID and object
                          index (if required) using the MIB TLV encoding
                          for a Get Request (see SMT standard).
Output:         p       = holds the attribute value requested using the
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
uInt32  result = RC_Success;   /* return code */

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
         * if-else is used here because a switch() statement requires an
         * int. The  vendor IDs will be 32-bits which may not be ints on
         * some machines.
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
                p       = pointer to buffer containing the requested
                          attribute information and used to hold attribute
                          value returned.
                pathPtr = pointer to PATH's attribute structure in the MIB.
Input:          p       = contains the requested attribute ID and object
                          index (if required) using the MIB TLV encoding
                          for a Get Request (see SMT standard).
Output:         p       = holds the attribute value requested using the
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
uInt32  result = RC_Success;   /* return code */

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
         * if-else is used here because a switch() statement requires an
         * int. The vendor IDs will be 32-bits which may not be ints on
         * some machines.
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
                p       = pointer to buffer containing the requested
                          attribute information and used to hold attribute
                                                value returned.
                portPtr = pointer to PORT's attribute structure in the MIB.
Input:          p       = contains the requested attribute ID and object
                          index (if required) using the MIB TLV encoding
                          for a Get Request (see SMT standard).
Output:         p       = holds the attribute value requested using the
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
uInt32  result = RC_Success;   /* return code */

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
         * if-else is used here because a switch() statement requires an
         * int. The vendor IDs will be 32-bits which may not be ints on
         * some machines.
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
                p       = pointer to buffer containing the new attribute
                          information.
                smtPtr  = address of SMT attribute data.
Input:          p       = contains the attribute ID using the MIB TLV
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
uInt32  result = RC_NoParameter;       /* return code */

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
                p       = pointer to buffer containing the new attribute
                          information.
                macPtr  = address of MAC attribute data.
Input:          p       = contains the attribute ID using the MIB TLV
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
uInt32  result = RC_NoParameter;       /* return code */

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
                p       = pointer to buffer containing the new attribute
                          information.
                pathPtr = address of PATH attribute data.
Input:          p       = contains the attribute ID using the MIB TLV
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
uInt32  result = RC_NoParameter;       /* return code */

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
                p       = pointer to buffer containing the new attribute
                          information.
                portPtr = address of PORT attribute data.
Input:          p       = contains the attribute ID using the MIB TLV
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
uInt32  result = RC_NoParameter;       /* return code */

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
Function:   Process event identifiers not handled by SendSMTEvent()
            in mibmsg.c. The SendSMTEvent() routine builds a
            notification report to be sent to FSP and the
            MAP. If SendSMTEvent() does not understand the event
            identifier, it calls this routine to complete the
            message information.
Parameters: msg     = address of SMT message buffer.
            notice  = address of message notification buffer.
Input:      msg     = contains initial message information:
                    msg->typeInfo   = event identifier passed to SendSMTEvent()
                    msg->entity     = object identfier
                    msg->p1.ptr     = points to notice
Output:     msg = should remain untouched. The value of msg->len1
                    will be calculated based on the length
                    specified in param->paramLen.
            notice  = contains the notification event report:
                    notice->type    = EC_TYPE_EVENT for event notices
                                      EC_TYPE_CONDITION for conditions
                                      0 for do not send message
                    notice->state   = current state if this notificiation
                                        is for a condition
                    notice->info    = any 4 octets of additional data
Return:     No value is returned through this function.
Notes:      Any type of processing may be done here. This routine may
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
Function:   Detect vendor-specific notifications on a periodic
            interval. This routine will usually look for conditions
            like the Frame Error Count conditions.
Parameters: stnID   = index of station to use.
            mibPtr  = pointer to this station's MIB database.
Input:      mibPtr  = contains current MIB information.
Output:     mibPtr  = any condition flags or event information
                        associated with the vendor-specific
                        notifications may be updated.
            The routine SendSMTEvent() will be called to inform
            FSP and the MAP of the notification.
Return:     No value is returned through this function.
Notes:      Any type of processing may be done here.
*********************************************************************/
{
    return;
}
