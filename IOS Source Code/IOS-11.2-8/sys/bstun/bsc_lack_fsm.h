/* $Id: bsc_lack_fsm.h,v 3.2.58.2 1996/07/22 18:22:16 snyder Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_lack_fsm.h,v $
 *------------------------------------------------------------------
 * BSC local ack (poller/pollee) FSM constants and prototypes.
 *
 * February 1995. Andrew R. Rothwell.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_lack_fsm.h,v $
 * Revision 3.2.58.2  1996/07/22  18:22:16  snyder
 * CSCdi63677:  fix a pointer problem, and move more text to data
 *              moves 2692 bytes to data, 16 out of image completely
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/05/17  10:40:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.46.2  1996/04/22  08:57:37  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.46.1  1996/04/03  13:50:09  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  08:48:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/03  01:03:57  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.2  1995/06/16  06:24:30  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __BSC_LACK_FSM_H
#define __BSC_LACK_FSM_H

/*  MIB - counter indices  */
#define CtSoftErr        0              /* transient link error that is recovered  */
#define CtHardErr        1              /* non-recovreable link error */
#define CtProtErr        2              /* protocol error  */
#define CtDataFrames     3              /* all data frames */
#define CtCtrlFrames     4              /* all control frames */
#define CtBytes          5              /* all frames */

/*---- Combined poller/pollee constants. */

#define CtAbort         CtSoftErr       /* ?type? from action: 'A_Rcv' */
#define CtAck           CtCtrlFrames    /* ?type? from action: 'A_Rcv' */
#define CtBadAck        CtProtErr       /* ?type? from action: 'A_Rcv' */
#define CtCrcErr        CtSoftErr       /* ?type? from action: 'A_Rcv' */
#define CtData          CtDataFrames    /* ?type? from action: 'A_Rcv' */
#define CtEnq           CtCtrlFrames    /* ?type? from action: 'A_Rcv' */
#define CtEot           CtCtrlFrames    /* ?type? from action: 'A_Rcv' */
#define CtInval         CtProtErr       /* ?type? from action: 'A_Rcv' */
#define CtNak           CtCtrlFrames    /* ?type? from action: 'A_Rcv' */
#define CtNegPolp       CtCtrlFrames    /* ?type? from action: 'A_Rcv' (poller) */
#define CtPoll          CtCtrlFrames    /* ?type? from action: 'A_Rcv' (pollee) */
#define CtRvi           CtCtrlFrames    /* ?type? from action: 'A_Rcv' */
#define CtSPoll         CtCtrlFrames    /* ?type? from action: 'A_Rcv' (pollee) */
#define CtSel           CtCtrlFrames    /* ?type? from action: 'A_Rcv' (pollee) */
#define CtTtd           CtCtrlFrames    /* ?type? from action: 'A_Rcv' */
#define CtTtdRsp        CtCtrlFrames    /* ?type? from action: 'A_Rcv' */
#define CtWack          CtCtrlFrames    /* ?type? from action: 'A_Rcv' */
#define CtAckRs         CtSoftErr       /* ?type? from action: 'A_Send' */
#define CtFail          CtHardErr       /* ?type? from action: 'A_Send' (poller) */
#define CtPermErr       CtHardErr       /* ?type? from action: 'A_Send' */
#define CtRetran        CtSoftErr       /* ?type? from action: 'A_Send' */
#define CtTo            CtSoftErr       /* ?type? from action: 'A_Send' (poller) */
#define CtPollTo        CtHardErr       /* ?type? from action: 'A_TxIncr' */
#define CtSelTo         CtSoftErr       /* ?type? from action: 'A_TxIncr' (poller) */

#define CtNone          255             /* ?type? from action: 'A_Rcv' */

#define FrNotify         0              /* ?type? from action: 'A_Rcv' */
#define FrSelPassEtb     1              /* ?type? from action: 'A_Rcv' (pollee) */
#define FrSelPassEtx     2              /* ?type? from action: 'A_Rcv' (pollee) */
#define FrPassEtx        3              /* ?type? from action: 'A_Rcv' (poller) */
#define FrPassEtb        4              /* ?type? from action: 'A_Rcv' (poller) */
#define FrPassSts        5              /* ?type? from action: 'A_Rcv' (poller) */
#define FrFree           6              /* ?type? from action: 'A_Rcv' */

#define FrAck            0              /* ?type? from action: 'A_Send' */
#define FrControl        1              /* ?type? from action: 'A_Send' */
#define FrCuData         2              /* ?type? from action: 'A_Send' (pollee) */
#define FrData           3              /* ?type? from action: 'A_Send' (poller) */
#define FrEnq            4              /* ?type? from action: 'A_Send' */
#define FrEot            5              /* ?type? from action: 'A_Send' */
#define FrNak            6              /* ?type? from action: 'A_Send' */
#define FrRvi            7              /* ?type? from action: 'A_Send' (pollee) */
#define FrTtd            8              /* ?type? from action: 'A_Send' */
#define FrWack           9              /* ?type? from action: 'A_Send' (pollee) */
#define FrStatus        10              /* ?type? from action: 'A_Send' (pollee) */

#define RxAck0           0              /* ?type? from action: 'A_ResetLine' (poller) */
#define TxAck0           0              /* ?type? from action: 'A_ResetLine' (poller) */
#define RxAck1           1              /* ?type? from action: 'A_ResetLine' (pollee) */
#define TxAck1           1              /* ?type? from action: 'A_ResetLine' (pollee)  */

#define ToAck            0              /* ?type? from action: 'A_TxDone' */
#define ToEnq            1              /* ?type? from action: 'A_TxDone' */
#define ToPoll           2              /* ?type? from action: 'A_TxDone' (poller) */
#define ToLineCheck      3              /* ?type? from action: 'A_StartTimer' */
#define ToTtdDelay       4              /* ?type? from action: 'A_StartTimer' */
#define ToLineIdleStart  5              /* ?type? from action: 'A_StartTimer' */
#define ToNone           255            /* ?type? from action: 'A_TxDone' (poller) */

#define cDataRetry       0              /* ?type? from action: 'A_ResetCU' */
#define cPollRetry       1              /* ?type? from action: 'A_ResetCU' (poller) */
#define cSelRetry        2              /* ?type? from action: 'A_ResetCU' (poller) */

#define lDataRetry       0              /* ?type? from action: 'Sw_Retry' */
#define lEnqRetry        1              /* ?type? from action: 'Sw_Retry' */
#define lNakRetry        2              /* ?type? from action: 'Sw_Retry' */
#define lTtdRetry        3              /* ?type? from action: 'Sw_Retry' */
#define lWackRetry       4              /* ?type? from action: 'Sw_Retry' */

/* ---- States ---- */ 
#define TCU_Down 0
#define TCU_EOFile 1
#define TCU_Idle 2
#define TCU_InFile 3
#define TCU_Polled 4
#define TCU_Selected 5
#define TCU_SpecPolled 6
#define TCU_TtdDelay 7
#define TCU_TtdSent 8
#define TCU_TxEOFile 9
#define TCU_TxInFile 10
#define TCU_TxRetry 11

#define pollerfsm_MaxStates 12
#define pollerfsm_InitialState TCU_Idle

/* ---- States ---- */ 
#define CU_DevBusy 0
#define CU_Down 1
#define CU_EOFile 2
#define CU_Idle 3
#define CU_InFile 4
#define CU_Selected 5
#define CU_TtdDelay 6
#define CU_TtdSent 7
#define CU_TxEOFile 8
#define CU_TxInFile 9
#define CU_TxRetry 10
#define CU_TxSpecPollData 11
#define CU_TxSts 12

#define polleefsm_MaxStates 13
#define polleefsm_InitialState CU_Idle

/* ---- Events ---- */
#define E_LineFail 0
#define E_LineUp 1
#define E_RxAbort 2
#define E_RxAck 3
#define E_RxBadAck 4
#define E_RxEnq 5
#define E_RxEot 6
#define E_RxEtb 7
#define E_RxEtx 8
#define E_RxGiant 9
#define E_RxInvalid 10
#define E_RxNak 11
#define E_RxRvi 12
#define E_RxTtd 13
#define E_RxWack 14
#define E_SendData 15
#define E_Timeout 16
#define E_TxComp 17

#define pollerfsm_MaxEvents 18
#define polleefsm_MaxEvents 18

/* ---- Switch Labels ---- */

/* 'Sw_CrcValid' return values */
#define swCrc_Good 0
#define swCrc_Bad 1

/* 'Sw_Cu_MoreData' return values */
#define SwMore 0
#define SwEmpty 1
#define SwStatus 2

/* 'Sw_CuRetry' return values */
#define SwCuRetry_Zero 0
#define SwCuRetry_NonZero 1

/* 'Sw_Frame' return values */
#define swFrEtb 0
#define swFrEtx 1
#define swFrAbort 2

/* 'Sw_MoreData' return values */
#define SwMore 0
#define SwEmpty 1

/* 'Sw_NextWork' return values */
#define swNW_Poll 0
#define swNW_Select 1
#define swNW_Spoll 2
#define swNW_Sleep 3

/* 'Sw_Printer' return values */
#define swPr_No 0
#define swPr_Yes 1

/* 'Sw_Retry' return values */
#define SwRetry_Zero 0
#define SwRetry_NonZero 1

/* 'Sw_Send_Wack' return values */
#define sw_SW_yes 0
#define sw_SW_no  1

/* 'Sw_cState' return values */
#define swCS_Active 0
#define swCS_Inactive 1

/* 'sw_DeviceState' return values */
#define swDS_DB 0
#define swDS_DE 1
#define swDS_OK 2
#define swDS_Other 3

/* 'Sw_SpecPollOnly' return values */
#define sw_NoSpecPollDev 0
#define sw_SpecPollDev 1

/* 'sw_MoreSpecData' return values */
#define swSMore 0
#define swSEmpty 1

/* 'sw_bidType' return values */
#define swBid_poll 0
#define swBid_sel 1
#define swBid_Spoll 2
#define swBid_Unknown 3

/* 'Sw_RxFrame' return values */
#define swRF_Sts 0
#define swRF_Data 1

#define pollerfsm_ActArraySize 515
#define polleefsm_ActArraySize 554


/* ---- Action Subroutine Declarations ---- */
    extern int A_ActivateCU(void* pECB);
    extern int A_BuildSpecPoll(void* pECB);
    extern int A_CancelIOTimer(void* pECB);
    extern int A_CheckCstate(void* pECB);
    extern int A_CheckCuPollStop(void* pECB);
    extern int A_ClearCuNoPollFlag(void* pECB);
    extern int A_ClearStsData(void* pECB);
    extern int A_CuFlush(int tx_frame_ct, void* pECB);
    extern int A_CuFlushAll(int tx_frame_ct, void *pECB);
    extern int A_CuFreeData (void* pECB);
    extern int A_Flush(int tx_frame_ct, void* pECB);
    extern int A_FreeData(void* pECB);
    extern int A_MakeSts(void* pECB);
    extern int A_PeerAlert(int alert_type, void* pECB);
    extern int A_Rcv(int frame_action, int lcb_frame_ct, int cucb_frame_ct,
                     void* pECB);
    extern int A_RcvDoWack(void* pECB);
    extern int A_RcvDontWack(void* pECB);
    extern int A_ResetCU( int retry_idx, void* pECB);
    extern int A_ResetLine(int tx_ack, int rx_ack, void* pECB);
    extern int A_RxIncr(int lcb_frame_ctidx, int cucb_frame_ctidx, void* pECB);
    extern int A_Send(int frame_type, int lcb_tx_frame_ct, int cucb_rx_frame_ct,
                      void* pECB);
    extern int A_SetDevBusy (void* pECB);
    extern int A_SetRxAck(void* pECB);
    extern int A_SetTxAck(void* pECB);
    extern int A_StartTimer(int timer_idx, void* pECB);
    extern int A_TxAccepted(void* pECB);
    extern int A_TxDone(int timer_idx, void* pECB);
    extern int A_TxIncr(int lcb_frame_ctidx, int cucb_frame_ctidx,
                        void* pECB);
    extern int A_UpdateSts(void* pECB);
    extern int A_cState(int cu_state, void* pECB);
    extern int Sw_bidType(void* pECB);
    extern int Sw_cState(void* pECB);
    extern int Sw_CrcValid(void* pECB);
    extern int Sw_Cu_MoreData(void* pECB);
    extern int Sw_CuRetry(int retry_idx, void* pECB);
    extern int Sw_DeviceState(void* pECB);
    extern int Sw_Frame(void* pECB);
    extern int Sw_MoreData(void* pECB);
    extern int Sw_MoreSpecData(void* pECB);
    extern int Sw_NextWork(void* pECB);
    extern int Sw_Printer(void* pECB);
    extern int Sw_Retry(int retry_idx, void* pECB);
    extern int Sw_RxFrame(void* pECB);
    extern int Sw_Send_Wack(void* pECB);
    extern int Sw_SpecPollOnly(void* pECB);


extern char *const polleefsm_StateNames[];
extern char *const pollerfsm_StateNames[];

#endif  /* __BSC_LACK_FSM_H. */
/*---- End include file: bsc_lack_fsm.h. */
