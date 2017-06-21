/* $Id: t1rcv.c,v 1.14.6.1 1997/02/21 07:41:08 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/t1rcv.c,v $
 *------------------------------------------------------------------
 * t1rcv.c  NEAT framer receiver routines
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: t1rcv.c,v $
 * Revision 1.14.6.1  1997/02/21 07:41:08  jliu
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *
 * Revision 1.14  1996/09/10 02:53:10  jchlin
 * Add thw following new features:
 * o       busy dsx0 back to CO trunk when there's no free modem available
 *         in the system
 * o       Support E&M feature group D, FXS loop start and ground start,
 *         and SAS loop start and ground start.
 *
 * Revision 1.13  1996/07/13  05:41:44  towu
 * o Initialize rbs if sigstack is available.
 *
 * Revision 1.12  1996/07/10  06:44:24  jliu
 *     	o Created another timer(250ms) for framing process and one second
 *           DSX1 performance data statistics in order to pass ACCUNET test
 * 	o Code changed for both AT&T and ANSI fdl test
 *         o Removed frame_mode in T1InfoType by using FrameType in frm_info
 *
 * Revision 1.11  1996/06/07  21:40:34  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.10  1996/05/09  21:25:56  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.9  1996/05/06  06:01:07  jliu
 *     o Cleaned some code for LED on and off
 *     o Set DSX1_XMT_AIS in dsx1LineStatus after sending AIS
 *
 * Revision 1.8  1996/04/29  03:35:09  jliu
 *     o clean up some code
 *     o updated loopback bit of the line status when local loopback
 *       is being performed
 *
 * Revision 1.7  1996/04/14  20:45:24  jliu
 *     o need to initialize TxingAis since the memory on T1 card was not
 *       initialized during system reload
 *
 * Revision 1.6  1996/04/08  18:17:59  jliu
 *     o cleanup some code
 *     o modified code for digits collection
 *
 * Revision 1.5  1996/03/19  03:54:49  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.4  1996/03/02  03:51:24  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.3  1996/02/26  06:29:42  jliu
 *     o Mask off the upper byte of the error counters since the register read
 *       from the framer is only byte long
 *     o When local loopback is performed, the TRI_RLOOP bit need to be cleared
 *       and set after local loopback completed
 *
 * Revision 1.2  1996/02/22  22:27:51  jliu
 *     o update thge current T1 firmware for starting channelized T1 coding
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include	"t1_headers.h"

void bt8360_init ( T1InfoType *t1 )
{
    frm_info *frm;
    dsx1MIBDataType *mib;

    mib = t1->Mib;
    frm = &t1->Fr;
    
    t1_init_framer( t1 );	/* init the hardware */

    t1->fdl_std = DSX1_FDL_NONE;
    frm->HasAIS = FALSE;
    frm->HasFrame = FALSE;
    frm->TxingAis = FALSE;
    frm->SecsForFrame = 5 * FRAMER_TIMES_PER_SEC;
    CLEAR_COUNTER(frm->CountsUntilFrame);
    CLEAR_COUNTER(frm->CountsUntilLossClear);
    frm->FrameLossOccurred = FALSE;
    frm->FrameAcquired = FALSE;
    frm->UnavailableState = TRUE;
    frm->RxingRemAlarm = FALSE; 	/* will be set in Process below */
    frm->TxingRemAlarm = FALSE;
    frm->IntrptLOS = FALSE;
    frm->IntrptOOF = FALSE;
    frm->HasLOS = TRUE;
    /*
     * The below are for a 1 x 10-6 error rate for 1 minute
     */
    frm->BPVneededinMin = (1544000 * 60) / 1000000;
    frm->CRCneededinMin = (1544000 * 60) / 1000000;
        
    mib->Config.dsx1LineStatus = DSX1_LOS;

    SetMIBLineStatus( &mib->Config.dsx1LineStatus, DSX1_FRAME_LOSS );
    SendRemoteAlarm( t1, TRUE );
    if (!t1->unit) {
        NEAT_LED &= ~REM_ALARM_0;
        NEAT_LED |= LOCAL_ALARM_0;
    } else {
        NEAT_LED &= ~REM_ALARM_1;
        NEAT_LED |= LOCAL_ALARM_1;
   }
    
}

/*
 * bt8360_frame_process
 * This routine gets called every 250 ms and processes the T1
 * framer information
 *
 * This algorithm is really brain-dead, but it is called out by AT&T,
 * and if we want an ACCUNET certificate, we have to do it.
 *
 * We sample the state of frame-loss and LOS.  If we are showing an error,
 * we increment CountsTowardLoss. If it ever hits the equivalent of
 * 2.5 seconds, we will declare frame loss.
 *
 * If we detect good frame, and good signal, we do 2 things, based
 * on if we have declared frame.  If we don't have frame, we will
 * wait for 5 consecutive seconds of no errors.  If we have that, we
 * declare ourselves in frame.
 *
 * If we do have frame, we check out the remote alarm.  We also check
 * if we have counted CountsTowardLoss in the recent past.  If we
 * have, we start a CountsUntilLossClear counter.  If that reaches 5,
 * we will decrement the CountsTowardLoss counter.  This will provide
 * us with an intermittent timer for frame loss.  The CountsTowardLossClear
 * will get cleared if an error occurs.
 */
void bt8360_frame_process (T1InfoType *t1)
{
    ushort          almstat, temp;
    uchar           HaveReceiveError;
    dsx1MIBDataType *mib;
    ushort          sr;
    dsx1TableType   *errptr;

    mib = t1->Mib;
    errptr = &mib->CurrentData;
    HaveReceiveError = FALSE;

    sr = set_ipl(PRIO_FDL);
    almstat = bt8360_rd_freg(t1->Fr.framer, T1_ALSSTAT_REG);

    if (almstat & T1_ALSSTAT_SIGSTACK) {
        t1_rbs_int(t1);
    }

    if ((almstat & T1_ALSSTAT_LOS) || t1->Fr.IntrptLOS) {
	t1->Fr.IntrptLOS = FALSE;
	/* Detected LOS - if we didn't have it before, set it */
	if ( t1->Fr.HasLOS == FALSE ) {
	    t1->Fr.HasLOS = TRUE;
	    SetMIBLineStatus( &mib->Config.dsx1LineStatus, DSX1_LOS );
	}
	HaveReceiveError = TRUE;
    } else {
	/* no LOS - check framing */
	if (t1->Fr.HasLOS != FALSE) {
	    t1->Fr.HasLOS = FALSE;
	    ClearMIBLineStatus(&mib->Config.dsx1LineStatus, DSX1_LOS);
	    /* Re-enable interrupt we turned off in interrupt routine */
	    temp = bt8360_rd_freg(t1->Fr.framer, T1_ALRMINT_REG);
	    temp |= T1_ALRMINT_LOS;
	    bt8360_wr_freg(t1->Fr.framer, T1_ALRMINT_REG, temp);
	}

	if ((almstat & T1_ALSSTAT_OOF) || t1->Fr.IntrptOOF) {
	    /* Bad framing - process down below */
	    t1->Fr.IntrptOOF = FALSE;
	    HaveReceiveError = TRUE;
	} else {
	    /* Framing is okay */
	    if (t1->Fr.HasFrame == FALSE) {
		/*
		 * We are Out of frame - but we have good frame
		 * Increment timer until it expires
		 */
		/* Re-enable interrupt turned off in interrupt routine */
		temp = bt8360_rd_freg(t1->Fr.framer, T1_ALRMINT_REG);
		bt8360_wr_freg(t1->Fr.framer, T1_ALRMINT_REG,
				(temp | T1_ALRMINT_OOF));
		FORWARD_COUNTER(t1->Fr.CountsUntilFrame, FRAMER_TIME);
		if (COUNTER_EXPIRED(t1->Fr.CountsUntilFrame,t1->Fr.SecsForFrame)) {
		    /*
		     * We now have frame  - indicate this
		     * Alert the Core CPU that we can now transmit - only if we
		     * are still in the available state.
		     */
		    t1->Fr.HasFrame = TRUE;
		    t1->Fr.FrameAcquired = TRUE;
		    ClearMIBLineStatus(&mib->Config.dsx1LineStatus, 
				       DSX1_FRAME_LOSS );
		    
		    /* Turn off Yellow alarm if we are available */
		    if (!( t1->Fr.UnavailableState )) {
			SendRemoteAlarm( t1, FALSE );
                        if (!t1->unit) 
                            NEAT_LED &= ~LOCAL_ALARM_0;
                        else
                            NEAT_LED &= ~LOCAL_ALARM_1;
		    }
		    CheckAvailableAndSend( t1 );
		}
	    } else {
		/*
                 * This is the ugly part of the remote loopback algorithm
                 * If we had stopped sending the pattern, we will start
                 * sending it again.  We retain the time count.
                 */
		if (t1->RemoteLoopState == REM_LOOP_ON_SRCH_BUT_STOPPED) {
		    temp = t1->RemoteLoopCount;
		    bt8360_loopup_code(t1);
		    t1->RemoteLoopCount = temp;
		}
		/*
		 * We are in frame and received a good frame.
		 * We will check to see if we have any frame loss events
		 * in the last few seconds.  
		 * We keep track of the number of periods with frame
		 * loss (that's the CountsTowardLoss counter).  If we have any
		 * periods with frame loss, we will decrement that counter
		 * by 1 for every 5 periods we have without a frame loss.
		 * This allows us to handle intermittent frame loss.
		 */
		if (HAVE_COUNTER(t1->Fr.CountsTowardLoss)) {
		    FORWARD_COUNTER(t1->Fr.CountsUntilLossClear,FRAMER_TIME);
		    if (COUNTER_EXPIRED(t1->Fr.CountsUntilLossClear,
				     5 * FRAMER_TIME)) {
			BACKWARD_COUNTER(t1->Fr.CountsTowardLoss,
					 FRAMER_TIME);
			CLEAR_COUNTER(t1->Fr.CountsUntilLossClear);
		    }
		}
		/*
		 * We have frame, so see if we are getting in remote 
		 * alarm.  If we are receiving it, it will take 2 seconds
		 * for us to stop receiving it before we declare we are up.
		 * When we go up, we alert the core CPU.
		 */
		if (almstat & T1_ALSSTAT_YELLOW) {
		    /* Detected yellow alarm */
		    if (t1->Fr.RxingRemAlarm == FALSE) {
			if (t1->Available) {
			    t1->Available = FALSE;
                            carrier_state_to_MPU(t1, CARRIER_DOWN, t1->Carrier);
                            rbs_t1_framing_down(t1->unit);
			}
                        t1->Fr.RxingRemAlarm = TRUE;
                        SetMIBLineStatus(&t1->Mib->Config.dsx1LineStatus,
                                               DSX1_RCV_REM_ALM);
                        if (!t1->unit)
                            NEAT_LED |= REM_ALARM_0;
                        else
                            NEAT_LED |= REM_ALARM_1;
		    }
		    CLEAR_COUNTER(t1->Fr.CountsNoRemAlarm);
		} else {
		    /* No yellow alarm */
		    if ( t1->Fr.RxingRemAlarm ) {
			FORWARD_COUNTER(t1->Fr.CountsNoRemAlarm, FRAMER_TIME);
			if (COUNTER_EXPIRED(t1->Fr.CountsNoRemAlarm,
					   T1_REM_ALARM_TIME)) {
			    ClearRxingRemoteAlarm(t1);
			    CheckAvailableAndSend( t1 );
			}
		    }
		} /* end else remote alarm */
	    } /* end else we have frame */

	} /* end else we have frame from framer */
    }

    if (HaveReceiveError) {
	t1->Fr.FrameLossOccurred = TRUE;
	CLEAR_COUNTER(t1->Fr.CountsUntilFrame);
	CLEAR_COUNTER(t1->Fr.CountsUntilLossClear);
	/*
         * Ugly part of the remote loopback routine.  If we are
         * looking for the code to come back, and we get a frame
         * loss, we will stop sending the code, and change state
         */
	if (t1->RemoteLoopState == REM_LOOP_ON_SRCH_CODE) {
	    bt8360_stoploop_code(t1);
	    t1->RemoteLoopState = REM_LOOP_ON_SRCH_BUT_STOPPED;
	}
	/*
	 * Receiver error (LOF or LOS), but still available
	 * Must wait 3 seconds
	 */
	if (t1->Fr.HasFrame) {
	    FORWARD_COUNTER(t1->Fr.CountsTowardLoss, FRAMER_TIME);
	    if (COUNTER_EXPIRED(t1->Fr.CountsTowardLoss, T1_FRAME_LOSS_TIME)) {
		DeclareFrameLoss(t1);
                errptr->dsx1LOFCs++;
		CLEAR_COUNTER(t1->Fr.CountsTowardLoss);
	    }
	}
    }

    if ( almstat & T1_ALSSTAT_AIS ) {
	SetMIBLineStatus( &mib->Config.dsx1LineStatus, DSX1_RCV_AIS );
        t1->Fr.HasAIS = TRUE;
    } else {
        ClearMIBLineStatus( &t1->Mib->Config.dsx1LineStatus, DSX1_RCV_AIS );
        t1->Fr.HasAIS = FALSE;
    }
    reset_ipl(sr);
}

/*
 * framer_uas -
 *
 * Description:
 * Uas will handle the MIB interpretation of the unavailable state.
 * It will handle all the processing of the data into available and
 * unavailable seconds.  It will also disable the applique if we
 * go into unavailable state.
 * It will also handle processing the data into the correct 15 minute bucket.
 *
 */
void framer_uas (T1InfoType *t1,
                 dsx1MIBDataType *mib,
                 ushort tmpBPV,
                 ushort tmpCRC,
                 bool EsOccurred,
                 bool BESOccurred,
                 bool SESOccurred,
                 dsx1TableType *errptr)
{
    int           cnt, interval;
    dsx1TableType *old, *new;
    frm_info      *frm;

    frm = &t1->Fr;
    /*
     * Now we check for unavailable / available states
     */
    if ( frm->UnavailableState ) {
        /*
         * Unavailable - we must have 10 non-SES seconds to become
         * available again
         */
        errptr->dsx1UASs++;
        if ( SESOccurred == FALSE ) {
            frm->SEScount++;
            if ( frm->SEScount >= 10 ) {
                frm->SEScount = 0;
                frm->UnavailableState = FALSE;

                SendRemoteAlarm(t1, FALSE);

                if (!t1->unit)
                    NEAT_LED &= ~LOCAL_ALARM_0;
                else
                    NEAT_LED &= ~LOCAL_ALARM_1;

                CheckAvailableAndSend(t1);

                if (mib->Config.dsx1TimeElapsed >= 9) {
                    errptr->dsx1UASs -= 10;
                } else {
                    /*
                     * Less than 10 seconds in this time period -
                     * subtract/add from the previous time period
                     */
                    if (mib->CurrentIndex != 0)
                        interval = mib->CurrentIndex - 1;
                    else
                        interval = QUARTER_HOURS_IN_DAY - 1;
                    old = &mib->Intervals[interval];
                    old->dsx1UASs -= 9 - mib->Config.dsx1TimeElapsed;
                    old->dsx1ESs += frm->ESlastinterval;
                    old->dsx1BESs += frm->BESlastinterval;
                    errptr->dsx1UASs = 0;
                }
                errptr->dsx1ESs += frm->ESinfailed;
                errptr->dsx1BESs += frm->BESinfailed;

            } else {
                if ( EsOccurred ) frm->ESinfailed++;
                if ( BESOccurred ) frm->BESinfailed++;
            }
        } else {
            frm->SEScount = 0;
            frm->BESinfailed = 0;
            frm->ESinfailed = 0;
            frm->ESlastinterval = 0;
            frm->BESlastinterval = 0;
        }
    } else {
        /*
         * Available - 10 SES in a row, we become unavailable
         */
        if ( EsOccurred ) {
             errptr->dsx1ESs++;
        }
        /*
         * Now see if we have a degraded minute
         */
        frm->SecsForMinute++;
        ADD_TO_GAUGE( frm->BPVinMin, tmpBPV );
        ADD_TO_GAUGE( frm->CRCinMin, tmpCRC );
        if ( frm->SecsForMinute >= 60 ) {
             if (( frm->CRCinMin > frm->CRCneededinMin )
                || ( frm->BPVinMin > frm->BPVneededinMin )) {
             }
             frm->SecsForMinute = 0;
             frm->CRCinMin = 0;
             frm->BPVinMin = 0;
        }
        if ( SESOccurred == FALSE ) {
            frm->SEScount = 0;
            if ( BESOccurred ) {
                 errptr->dsx1BESs++;
            }
        } else {
            errptr->dsx1SESs++;
            frm->SEScount++;
            if ( frm->SEScount >= 10 ) {
                frm->SEScount = 0;
                if ( t1->Available ) {
                    /* We haven't lost frame - but have too many errors -
                     * disable the M32 channels
                     */
                    t1->Available = FALSE;
                    carrier_state_to_MPU(t1, CARRIER_DOWN, t1->Carrier);
                    rbs_t1_framing_down(t1->unit);
                    /* Turn on remote alarm */
                    SendRemoteAlarm(t1, TRUE );
                    errptr->dsx1LOFCs++;

                    if (!t1->unit)
                        NEAT_LED |= LOCAL_ALARM_0;
                    else
                        NEAT_LED |= LOCAL_ALARM_1;

                }
                if (mib->Config.dsx1TimeElapsed >= 9) {
                    errptr->dsx1UASs += 10;
                    errptr->dsx1SESs -= 10;
                } else {
                    /*
                     * Less than 10 seconds in this time period -
                     * subtract/add from the previous time period
                     */
                    if (mib->CurrentIndex != 0)
                        interval = mib->CurrentIndex - 1;
                    else
                        interval = QUARTER_HOURS_IN_DAY - 1;
                    old = &mib->Intervals[interval];
                    old->dsx1UASs += 9 - mib->Config.dsx1TimeElapsed;
                    old->dsx1SESs -= 9 - mib->Config.dsx1TimeElapsed;
                    errptr->dsx1UASs = mib->Config.dsx1TimeElapsed + 1;
                    errptr->dsx1SESs = 0;
                }
                frm->UnavailableState = TRUE;
                frm->ESinfailed = 0;
                frm->ESlastinterval = 0;
                frm->BESinfailed = 0;
                frm->BESlastinterval = 0;
            }
        }
    }

    mib->Config.dsx1TimeElapsed++;
    if ( mib->Config.dsx1TimeElapsed >= 900 ) {
        if (frm->UnavailableState) {
            frm->BESlastinterval = frm->BESinfailed;
            frm->BESinfailed = 0;
            frm->ESlastinterval = frm->ESinfailed;
            frm->ESinfailed = 0;
        }
        mib->Config.dsx1TimeElapsed = 0;
        mib->Config.dsx1ValidIntervals++;
        if (mib->Config.dsx1ValidIntervals > QUARTER_HOURS_IN_DAY)
            mib->Config.dsx1ValidIntervals = QUARTER_HOURS_IN_DAY;

        mib->CurrentData.dsx1Interval++;
        if (mib->CurrentData.dsx1Interval >= 96)
            mib->CurrentData.dsx1Interval = 0;

        interval = mib->CurrentIndex;
        old = &mib->Intervals[interval];
        new = &mib->CurrentData;
        /*
         * save the next index
         */
        mib->CurrentIndex = interval + 1; /* for next time */
        if (mib->CurrentIndex >= QUARTER_HOURS_IN_DAY)
            mib->CurrentIndex = 0;

        mib->TotalData.dsx1ESs -= old->dsx1ESs ;
        ADD_TO_GAUGE(mib->TotalData.dsx1ESs, new->dsx1ESs);
        old->dsx1ESs = new->dsx1ESs ;

        mib->TotalData.dsx1UASs -= old->dsx1UASs;
        ADD_TO_GAUGE(mib->TotalData.dsx1UASs, new->dsx1UASs);
        old->dsx1UASs = new->dsx1UASs ;

        mib->TotalData.dsx1BESs -= old->dsx1BESs;
        ADD_TO_GAUGE(mib->TotalData.dsx1BESs, new->dsx1BESs);
        old->dsx1BESs = new->dsx1BESs ;

        mib->TotalData.dsx1SESs -= old->dsx1SESs;
        ADD_TO_GAUGE(mib->TotalData.dsx1SESs, new->dsx1SESs);
        old->dsx1SESs = new->dsx1SESs ;

        mib->TotalData.dsx1CSSs -= old->dsx1CSSs;
        ADD_TO_SMGAUGE(mib->TotalData.dsx1CSSs, new->dsx1CSSs);
        old->dsx1CSSs = new->dsx1CSSs ;

        mib->TotalData.dsx1LOFCs -= old->dsx1LOFCs;
        ADD_TO_SMGAUGE(mib->TotalData.dsx1LOFCs, new->dsx1LOFCs);
        old->dsx1LOFCs = new->dsx1LOFCs ;

        Cleardsx1Table(new);
        /*
         * Go through and adjust the interval numbers to make the
         * latest set of data interval #1 (A pain, but it must be done)
         */
        for ( cnt = 1; cnt <= mib->Config.dsx1ValidIntervals; cnt++ ) {
            mib->Intervals[interval].dsx1Interval = cnt;
            if ( interval == 0 ) interval = QUARTER_HOURS_IN_DAY - 1;
            else interval--;
        }
    }
}

/*
 * UpdateT1RcvrSec
 * Handle everything about the T1, except the framing, here.
 */
void UpdateT1RcvrSec ( T1InfoType *t1 )
{
    volatile ushort *framer;
    dsx1MIBDataType *mib;
    ushort          tmpBPV, tmpFBE, tmpCRC, othererrs = 0;
    ushort          read;
    bool            EsOccurred, BESOccurred, SESOccurred;
    dsx1TableType   *errptr;
    ansi_fdl_info   *ansi;

    framer = t1->Fr.framer;
    mib = t1->Mib;
    ansi = &t1->Ansi;
    EsOccurred = FALSE;
    BESOccurred = FALSE;
    SESOccurred = FALSE;
    errptr = &mib->CurrentData;
    
    /* Read all the counters */	
    tmpBPV = (bt8360_rd_freg(framer,T1_BPV_LSB)) & 0xff;
    tmpBPV += ((bt8360_rd_freg(framer,T1_BPV_MSB)) << 8);
    tmpFBE = (bt8360_rd_freg(framer,T1_FBE_CTR)) & 0xff;
    tmpCRC = (bt8360_rd_freg(framer,T1_CRC_CTR)) & 0xff;
    read = bt8360_rd_freg(framer,T1_COFA_CTR);
    if (( read & T1_COFA_CRC ) != 0 )
	tmpCRC += 0x100;
    
    if (t1->fdl_std == DSX1_FDL_ANSI) {
        ansi->sapi = 0x38;    /* SAPI = 0xe, C/R = 0 & EA = 0 */
        ansi->tei = 0x01;     /* TEI = 0, EA = 1 */
        ansi->control = 0x03; /* Unacknowledged frame */
        /*
         * Copy the messages at T second to T-1 second before send the 
         * one second performance report message
         */
	ansi->message0_T3 = ansi->message0_T2;
	ansi->message1_T3 = ansi->message1_T2;
	ansi->message0_T2 = ansi->message0_T1;
	ansi->message1_T2 = ansi->message1_T1;
        ansi->message0_T1 = ansi->message0_T;
        ansi->message1_T1 = ansi->message1_T;
        ansi->message0_T = 0x0;    /* clear the latest message before update */
        ansi->message1_T &= 0x23;  /* only save Nm & Ni and LB value before */
                                   /* update */
        ansi->message1_T += 0x01;  /* increment Nm & Ni by one before update */
        ansi->message1_T &= 0x23;  /* clear bit 2 in case 3 + 1 */

        if (tmpBPV > 0)
            ansi->message0_T |= LV;
        if (tmpFBE == 1)
            ansi->message1_T |= FE;
        if (tmpFBE > 1)
            ansi->message1_T |= SE;
        if (tmpCRC == 1)
            ansi->message1_T |= CRC_1;
        if ((tmpCRC < 6) && (tmpCRC > 1))
            ansi->message1_T |= CRC_1to5;
        if ((tmpCRC < 11) && (tmpCRC > 5))
            ansi->message0_T |= CRC_5to10;
        if ((tmpCRC < 101) && (tmpCRC > 10))
            ansi->message0_T |= CRC_10to100;
        if ((tmpCRC < 320) && (tmpCRC > 100))
            ansi->message0_T |= CRC_100to319;
        if (tmpCRC > 319)
            ansi->message0_T |= CRC_320more;
    }

    if ( tmpBPV > 0 ) {
        othererrs |= MSG_LES_OCCURRED | MSG_ES_OCCURRED;
	EsOccurred = TRUE;
	if ( tmpBPV >= 1544 ) {
            othererrs |= MSG_SES_OCCURRED;
	    SESOccurred = TRUE;
        }
    t1->Fr.BPVErrs = tmpBPV;
    }
    
    if ( t1->Fr.HasFrame ) {
        t1->Fr.FBEErrs = 0x0;
	if ( tmpFBE > 0 ) {
	    EsOccurred = TRUE;
            othererrs |= MSG_ES_OCCURRED;
	    if ( t1->Fr.FrameType == DSX1_SF ) {
		if ( tmpFBE > 1 ) {
		    SESOccurred = TRUE;
                    othererrs |= MSG_SES_OCCURRED;
                }
                t1->Fr.FBEErrs = tmpFBE;
	    }
	}
	
	if ( t1->Fr.FrameType == DSX1_ESF ) {
            t1->Fr.CRCErrs = 0x0;
	    if ( tmpCRC > 0 ) {
                t1->crc_count += tmpCRC;
                if (t1->crc_count > MAX_GAUGE_COUNT)
                    t1->crc_count = 0xffff;
                othererrs |= MSG_ES_OCCURRED;
		EsOccurred = TRUE;
		if ( tmpCRC >= 320 ) {
                    othererrs |= MSG_SES_OCCURRED;
		    SESOccurred = TRUE;
		} else if ( tmpCRC > 1 ) {
                    othererrs |= MSG_BES_OCCURRED;
		    BESOccurred = TRUE;
                }
	    }
            t1->Fr.PCVErrs = tmpCRC;
	}
    } 
    
    if (t1->Fr.FrameLossOccurred) {
        othererrs |= MSG_FR_LOSS_OCCURRED | MSG_SES_OCCURRED;
	SESOccurred = TRUE;
	t1->Fr.FrameLossOccurred = FALSE;
    }
    
    /*
     * Check for severe errored frame, or controlled slips
     */
    read = bt8360_rd_freg(t1->Fr.framer,T1_ERRSTAT_REG);
    if ( read & T1_ERRSTAT_SEVERR )
	SESOccurred = TRUE;
    
    if ( read & T1_ERRSTAT_SLIPEV ) {
	/* Have a controlled slip event - add one, and set ES */
        othererrs |= MSG_CSS_OCCURRED | MSG_ES_OCCURRED;
	errptr->dsx1CSSs++;
        if (errptr->dsx1CSSs > MAX_SMGAUGE_COUNT)
            errptr->dsx1CSSs = 0xff;
	EsOccurred = TRUE;
        ansi->message0_T |= SL;
    }
    
    t1->Fr.OtherErrors = othererrs;

    /*
     * Now check for remote loopback for the T1
     */
    if (( t1->RemoteLoopState != REM_LOOP_NOT_SRCH ) 
	&& ( t1->RemoteLoopState != REM_LOOP_IN_LOOP )) {
	switch ( t1->RemoteLoopState ) {
	case REM_LOOP_NOT_SRCH:
	case REM_LOOP_IN_LOOP:
	    break;
	case REM_LOOP_OFF_WAIT_NO_CODE:
	    /*
	     * check if we have stopped getting the loop code yet.
	     * If so, indicate this and stop sending it.
	     */
	    read = bt8360_rd_freg(framer,T1_ALSSTAT_REG);
	    if (( read & T1_ALSSTAT_LOOPDEACT ) == 0 ) {
		bt8360_stoploop_code( t1 );
		t1->RemoteLoopState = REM_LOOP_NOT_SRCH;
		t1->InRemLoop = FALSE;
                t1->LoopbackAct |= REM_LP_PASS;
                t1->Mib->Config.dsx1LoopbackConfig = DSX1_NO_LOOP;
                t1->MsgToSendToMPU |= NEAT_MSG_LPBK;

		break;
	    }
	    /* else fall through */
	case REM_LOOP_ON_SRCH_CODE:
	case REM_LOOP_OFF_SRCH_CODE:
	case REM_LOOP_ON_SRCH_BUT_STOPPED:
	    /*
	     * Here we do a 10 second count to see if we
	     * have received the loop code response yet.  If we
	     * haven't, indicate this and stop sending it.
	     */
	    FORWARD_COUNTER(t1->RemoteLoopCount, 1);
	    if (COUNTER_EXPIRED(t1->RemoteLoopCount,10)) {
		/* no resp in 10 seconds.  Forget it */
		bt8360_stoploop_code( t1 );
		t1->RemoteLoopState = t1->InRemLoop == FALSE ?
		    REM_LOOP_NOT_SRCH : REM_LOOP_IN_LOOP;
                t1->LoopbackAct |= REM_LP_FAIL;
                t1->Mib->Config.dsx1LoopbackConfig = DSX1_NO_LOOP;
                t1->MsgToSendToMPU |= NEAT_MSG_LPBK;
	    }
	    break;
	} /* end switch */
    } /* end if looking for remote loop */

    /*
     * Now we check for unavailable / available states 
     * Note: may want to break out later - should be the same for E1
     */
    framer_uas(t1, mib, tmpBPV, tmpCRC, EsOccurred, BESOccurred, SESOccurred, errptr);
    
}

/*
 * Send ANSI performance report messages to far end equipment
 */
void t1_send_ansi(T1InfoType *t1)
{
        vushort          *bt8360;
	ansi_fdl_info    *ansi;
        uint             index, ix;
        uchar            *start;

        ansi = &t1->Ansi;
        start = &ansi->sapi;
        bt8360 = t1->Fr.framer;

        if (t1->Fr.TxingRemAlarm == TRUE) {
            bt8360_tx_remotealarm(t1, FALSE);
            for (ix=0; ix < 50000; ix++)
                      ;
            t1->fdl_resend_RAI = TRUE;
        }

        for (index = 0; index < ANSI_PRM_LENGTH; index++) {
            bt8360_wr_fram(bt8360, (T1_TX_DL_BUFFER + index), *start++);
        }

        bt8360_wr_freg(bt8360, T1_TXCTRL_REG, (((ANSI_PRM_LENGTH - 1) << 4) |
                         (T1_TXCTRL_MOP | T1_TXCTRL_FCS)));
}

/*
 * T1 framing process called through framer_algorithm vector,
 * run a 250 msec and an 1 second task
 */
void T1_Framing_Process(T1InfoType *t1, int unit)
{


        /*
         * Call the framer update routine.  If 1 second is done,
         * call the 1 second update routine
         */
        bt8360_frame_process(t1);
        FORWARD_COUNTER(OneSecTimer[unit], FRAMER_TIME);
        Have1Sec = COUNTER_EXPIRED(OneSecTimer[unit],FRAMER_TIMES_PER_SEC);
        if (Have1Sec) {
            CLEAR_COUNTER(OneSecTimer[unit]);
            UpdateT1RcvrSec(t1);
            t1->MsgToSendToMPU |= NEAT_MSG_FULL;
            if (t1->fdl_std == DSX1_FDL_ANSI)
                t1_send_ansi(t1);
        }
}

void interface_init ( T1InfoType *t1)
{
    frm_info        *frm;
    int   ix;

    frm = &t1->Fr;
    t1->Mib = &MIBData[t1->unit];
    t1->Available = FALSE;
    t1->att_lineloop = FALSE;
    t1->ansi_lineloop = FALSE;
    t1->fdl_resend_RAI = FALSE;
    t1->RAI_resend = FALSE;
    t1->Mib->Config.dsx1ValidIntervals = 0;
    t1->Mib->CurrentIndex = 0;
    t1->RemoteLoopState = REM_LOOP_NOT_SRCH;
    t1->InRemLoop = FALSE;
    t1->ct1_map = 0;		/* All the channels are PRI in default */
    t1->Carrier->carrier_state = CARRIER_DOWN;

    ClearMIBData( t1 );

    /*
     * initialize the mailbox used for communication between NEAT and 
     * core CPU.
     */
    t1->MsgToSendToMPU = NEAT_MSG_NONE;
    t1->MailToMPU->message_type = NEAT_MSG_NONE;
    t1->MsgReceivedFromMPU = NEAT_MSG_NONE;
    *t1->MailFromMPU = NEAT_MSG_NONE;

    frm->HasFrame = FALSE;
    CLEAR_COUNTER(frm->CountsTowardLoss);

    frm->SEScount = 0;

    frm->ESinfailed = 0;
    frm->ESlastinterval = 0;
    frm->BESinfailed = 0;
    frm->BESlastinterval = 0;

    frm->SecsForMinute = 0;
    frm->BPVinMin = 0;
    frm->CRCinMin = 0;

    /*
     * In oder to meet the minimum timing for LIU chip reset which is at least
     * one T1 cycle, we need to continue to hold one for RLOOP, LOOP & TAOS
     * until the least time elapsed.
     */
    for (ix=0; ix <= ONE_T1_CYCLE; ix++) {
         *t1->LIU_Control = (RLOOP | LLOOP | TAOS);
    }

    /*
     * Inactive RLOOP, LLOOP & TAOS to get LIU back to normal state and
     * floating RLOOP to enable Network Loopback Detection.
     */
    *t1->LIU_Control = TRI_RLOOP;

    bt8360_init( t1 );

}

