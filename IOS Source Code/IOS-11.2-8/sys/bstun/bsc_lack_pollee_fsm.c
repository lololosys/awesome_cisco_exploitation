/* $Id: bsc_lack_pollee_fsm.c,v 3.2.58.3 1996/07/20 01:05:13 snyder Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_lack_pollee_fsm.c,v $
 *------------------------------------------------------------------
 * BSC local-ack pollee FSM  
 *
 * March 1995, Andrew R. Rothwell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_lack_pollee_fsm.c,v $
 * Revision 3.2.58.3  1996/07/20  01:05:13  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              missed a table banish another 632 from data to text
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/07/19  22:44:52  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/05/17  10:40:28  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.46.2  1996/04/22  08:57:42  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.46.1  1996/04/03  13:50:12  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  08:48:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/03  01:04:00  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.2  1995/06/16  06:24:33  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* ---- polleefsm state machine table file ---- */

#include "cape.h"
#include "bsc_cmd_set.h"
#include "bsc_lack_fsm.h"

/* Switch Return Value Table (indices into the Action Vector Table) */
static const int polleefsm_SwLblVect[] = {
   510,   513,          /* 'Sw_CrcValid' */
   498,   501,          /* 'Sw_CrcValid' */
   347,   350,          /* 'Sw_CrcValid' */
   335,   338,          /* 'Sw_CrcValid' */
   245,   248,          /* 'Sw_CrcValid' */
   233,   236,          /* 'Sw_CrcValid' */
   153,   156,          /* 'Sw_CrcValid' */
   144,   147,          /* 'Sw_CrcValid' */
   117,   120,          /* 'Sw_CrcValid' */
   108,   111,          /* 'Sw_CrcValid' */
    76,    85,          /* 'Sw_CrcValid' */
    61,    70,          /* 'Sw_CrcValid' */
   298,   300,          /* 'Sw_CuRetry' */
   222,   224,          /* 'Sw_CuRetry' */
   180,   182,          /* 'Sw_CuRetry' */
   531,   536,   537,          /* 'Sw_Cu_MoreData' */
   425,   430,   434,          /* 'Sw_Cu_MoreData' */
   414,   419,   423,          /* 'Sw_Cu_MoreData' */
   402,   407,   411,          /* 'Sw_Cu_MoreData' */
   378,   383,   384,          /* 'Sw_Cu_MoreData' */
   310,   315,   316,          /* 'Sw_Cu_MoreData' */
   266,   271,   272,          /* 'Sw_Cu_MoreData' */
   193,   198,   199,          /* 'Sw_Cu_MoreData' */
    37,    42,    45,          /* 'Sw_Cu_MoreData' */
    11,    16,    19,          /* 'Sw_Cu_MoreData' */
   445,   446,   448,   449,          /* 'Sw_DeviceState' */
    24,    25,    27,    28,          /* 'Sw_DeviceState' */
   532,   533,   534,          /* 'Sw_Frame' */
   426,   427,   428,          /* 'Sw_Frame' */
   415,   416,   417,          /* 'Sw_Frame' */
   403,   404,   405,          /* 'Sw_Frame' */
   391,   392,   393,          /* 'Sw_Frame' */
   379,   380,   381,          /* 'Sw_Frame' */
   322,   323,   324,          /* 'Sw_Frame' */
   311,   312,   313,          /* 'Sw_Frame' */
   267,   268,   269,          /* 'Sw_Frame' */
   194,   195,   196,          /* 'Sw_Frame' */
    38,    39,    40,          /* 'Sw_Frame' */
    12,    13,    14,          /* 'Sw_Frame' */
    33,    35,          /* 'Sw_MoreSpecData' */
    78,    80,          /* 'Sw_Printer' */
    63,    65,          /* 'Sw_Printer' */
   523,   525,          /* 'Sw_Retry' */
   515,   517,          /* 'Sw_Retry' */
   503,   505,          /* 'Sw_Retry' */
   473,   476,          /* 'Sw_Retry' */
   467,   470,          /* 'Sw_Retry' */
   431,   433,          /* 'Sw_Retry' */
   420,   422,          /* 'Sw_Retry' */
   408,   410,          /* 'Sw_Retry' */
   368,   370,          /* 'Sw_Retry' */
   360,   362,          /* 'Sw_Retry' */
   352,   354,          /* 'Sw_Retry' */
   340,   342,          /* 'Sw_Retry' */
   328,   330,          /* 'Sw_Retry' */
   319,   321,          /* 'Sw_Retry' */
   281,   283,          /* 'Sw_Retry' */
   276,   278,          /* 'Sw_Retry' */
   258,   260,          /* 'Sw_Retry' */
   250,   252,          /* 'Sw_Retry' */
   238,   240,          /* 'Sw_Retry' */
   207,   209,          /* 'Sw_Retry' */
   202,   204,          /* 'Sw_Retry' */
   185,   187,          /* 'Sw_Retry' */
   163,   165,          /* 'Sw_Retry' */
   158,   160,          /* 'Sw_Retry' */
   149,   151,          /* 'Sw_Retry' */
   127,   129,          /* 'Sw_Retry' */
   122,   124,          /* 'Sw_Retry' */
   113,   115,          /* 'Sw_Retry' */
    92,    94,          /* 'Sw_Retry' */
    87,    89,          /* 'Sw_Retry' */
    72,    74,          /* 'Sw_Retry' */
    82,    84,          /* 'Sw_Send_Wack' */
    67,    69,          /* 'Sw_Send_Wack' */
    32,    36,          /* 'Sw_SpecPollOnly' */
     8,    21,    29,    47,          /* 'Sw_bidType' */
    43,    44,          /* 'Sw_cState' */
    17,    18          /* 'Sw_cState' */
}; 

/* Indicies into the polleefsm_SwLblVect array, # of return values */
const static SWTBL polleefsm_SwTbl[] = {
    {0, 2},
    {2, 2},
    {4, 2},
    {6, 2},
    {8, 2},
    {10, 2},
    {12, 2},
    {14, 2},
    {16, 2},
    {18, 2},
    {20, 2},
    {22, 2},
    {24, 2},
    {26, 2},
    {28, 2},
    {30, 3},
    {33, 3},
    {36, 3},
    {39, 3},
    {42, 3},
    {45, 3},
    {48, 3},
    {51, 3},
    {54, 3},
    {57, 3},
    {60, 4},
    {64, 4},
    {68, 3},
    {71, 3},
    {74, 3},
    {77, 3},
    {80, 3},
    {83, 3},
    {86, 3},
    {89, 3},
    {92, 3},
    {95, 3},
    {98, 3},
    {101, 3},
    {104, 2},
    {106, 2},
    {108, 2},
    {110, 2},
    {112, 2},
    {114, 2},
    {116, 2},
    {118, 2},
    {120, 2},
    {122, 2},
    {124, 2},
    {126, 2},
    {128, 2},
    {130, 2},
    {132, 2},
    {134, 2},
    {136, 2},
    {138, 2},
    {140, 2},
    {142, 2},
    {144, 2},
    {146, 2},
    {148, 2},
    {150, 2},
    {152, 2},
    {154, 2},
    {156, 2},
    {158, 2},
    {160, 2},
    {162, 2},
    {164, 2},
    {166, 2},
    {168, 2},
    {170, 2},
    {172, 2},
    {174, 2},
    {176, 2},
    {178, 4},
    {182, 2},
    {184, 2},
};

/* ---- Action routines which pass arguments ---- */

static int A_CuFlushCtAbort(void* pECB)
{ return(A_CuFlush(CtAbort,pECB)); }

static int A_CuFlushCtPermErr(void* pECB)
{ return(A_CuFlush(CtPermErr,pECB)); }

static int A_CuFlushAllCtPermErr(void *pECB)
{ return(A_CuFlushAll(CtPermErr,pECB)); }

static int A_PeerAlertSEL_DATA_ABORT(void *pECB)
{ return(A_PeerAlert(SEL_DATA_ABORT,pECB)); }

static int A_RcvFrFreeCtTtdRspCtTtdRsp(void *pECB)
{ return(A_Rcv(FrFree,CtTtdRsp,CtTtdRsp,pECB)); }

static int A_RcvFrFreeCtRviCtRvi(void *pECB)
{ return(A_Rcv(FrFree,CtRvi,CtRvi,pECB)); }

static int A_RcvFrFreeCtNakCtNak(void *pECB)
{ return(A_Rcv(FrFree,CtNak,CtNak,pECB)); }

static int A_RcvFrFreeCtBadAckCtBadAck(void *pECB)
{ return(A_Rcv(FrFree,CtBadAck,CtBadAck,pECB)); }

static int A_RcvFrFreeCtAckCtAck(void *pECB)
{ return(A_Rcv(FrFree,CtAck,CtAck,pECB)); }

static int A_RcvFrFreeCtWackCtWack(void *pECB)
{ return(A_Rcv(FrFree,CtWack,CtWack,pECB)); }

static int A_RcvFrFreeCtAbortCtAbort(void *pECB)
{ return(A_Rcv(FrFree,CtAbort,CtAbort,pECB)); }

static int A_RcvFrFreeCtEnqCtEnq(void *pECB)
{ return(A_Rcv(FrFree,CtEnq,CtEnq,pECB)); }

static int A_RcvFrNotifyCtAbortCtAbort(void *pECB)
{ return(A_Rcv(FrNotify,CtAbort,CtAbort,pECB)); }

static int A_RcvFrFreeCtInvalCtInval(void *pECB)
{ return(A_Rcv(FrFree,CtInval,CtInval,pECB)); }

static int A_RcvFrFreeCtTtdCtTtd(void *pECB)
{ return(A_Rcv(FrFree,CtTtd,CtTtd,pECB)); }

static int A_RcvFrSelPassEtxCtDataCtData(void *pECB)
{ return(A_Rcv(FrSelPassEtx,CtData,CtData,pECB)); }

static int A_RcvFrFreeCtCrcErrCtCrcErr(void *pECB)
{ return(A_Rcv(FrFree,CtCrcErr,CtCrcErr,pECB)); }

static int A_RcvFrSelPassEtbCtDataCtData(void *pECB)
{ return(A_Rcv(FrSelPassEtb,CtData,CtData,pECB)); }

static int A_RcvFrFreeCtEotCtEot(void *pECB)
{ return(A_Rcv(FrFree,CtEot,CtEot,pECB)); }

static int A_RcvFrFreeCtEotCtNone(void *pECB)
{ return(A_Rcv(FrFree,CtEot,CtNone,pECB)); }

static int A_RcvFrFreeCtPollCtNone(void *pECB)
{ return(A_Rcv(FrFree,CtPoll,CtNone,pECB)); }

static int A_RcvFrFreeCtSPollCtSPoll(void *pECB)
{ return(A_Rcv(FrFree,CtSPoll,CtSPoll,pECB)); }

static int A_RcvFrFreeCtSelCtSel(void *pECB)
{ return(A_Rcv(FrFree,CtSel,CtSel,pECB)); }

static int A_RcvFrFreeCtPollCtPoll(void *pECB)
{ return(A_Rcv(FrFree,CtPoll,CtPoll,pECB)); }

static int A_RcvFrFreeCtNoneCtNone(void *pECB)
{ return(A_Rcv(FrFree,CtNone,CtNone,pECB)); }

static int A_ResetCUcDataRetry(void *pECB)
{ return(A_ResetCU(cDataRetry,pECB)); }

static int A_ResetLineTxAck0RxAck1(void *pECB)
{ return(A_ResetLine(TxAck0,RxAck1,pECB)); }

static int A_RxIncrCtPollToCtPollTo(void *pECB)
{ return(A_RxIncr(CtPollTo,CtPollTo,pECB)); }

static int A_SendFrStatusCtRetranCtRetran(void* pECB)
{ return(A_Send(FrStatus,CtRetran,CtRetran,pECB)); }

static int A_SendFrTtdCtTtdCtTtd(void *pECB)
{ return(A_Send(FrTtd,CtTtd,CtTtd,pECB)); }

static int A_SendFrCuDataCtRetranCtRetran(void *pECB)
{ return(A_Send(FrCuData,CtRetran,CtRetran,pECB)); }

static int A_SendFrEnqCtEnqCtEnq(void *pECB)
{ return(A_Send(FrEnq,CtEnq,CtEnq,pECB)); }

static int A_SendFrEotCtAbortCtAbort(void *pECB)
{ return(A_Send(FrEot,CtAbort,CtAbort,pECB)); }

static int A_SendFrControlCtAckRsCtAckRs(void *pECB)
{ return(A_Send(FrControl,CtAckRs,CtAckRs,pECB)); }

static int A_SendFrNakCtTtdRspCtTtdRsp(void *pECB)
{ return(A_Send(FrNak,CtTtdRsp,CtTtdRsp,pECB)); }

static int A_SendFrNakCtNakCtNak(void *pECB)
{ return(A_Send(FrNak,CtNak,CtNak,pECB)); }

static int A_SendFrEotCtEotCtEot(void *pECB)
{ return(A_Send(FrEot,CtEot,CtEot,pECB)); }

static int A_SendFrRviCtRviCtRvi(void *pECB)
{ return(A_Send(FrRvi,CtRvi,CtRvi,pECB)); }

static int A_SendFrAckCtAckCtAck(void *pECB)
{ return(A_Send(FrAck,CtAck,CtAck,pECB)); }

static int A_SendFrWackCtWackCtWack(void *pECB)
{ return(A_Send(FrWack,CtWack,CtWack,pECB)); }

static int A_SendFrStatusCtDataCtData(void* pECB)
{ return(A_Send(FrStatus,CtData,CtData,pECB)); }

static int A_SendFrEotCtNegPolpCtNegPolp(void *pECB)
{ return(A_Send(FrEot,CtNegPolp,CtNegPolp,pECB)); }

static int A_SendFrEotCtEotCtNegPolp(void *pECB)
{ return(A_Send(FrEot,CtEot,CtNegPolp,pECB)); }

static int A_SendFrCuDataCtDataCtData(void *pECB)
{ return(A_Send(FrCuData,CtData,CtData,pECB)); }

static int A_StartTimerToTtdDelay(void *pECB)
{ return(A_StartTimer(ToTtdDelay,pECB)); }

static int A_StartTimerToLineCheck(void *pECB)
{ return(A_StartTimer(ToLineCheck,pECB)); }

static int A_TxDoneToEnq(void *pECB)
{ return(A_TxDone(ToEnq,pECB)); }

static int A_TxDoneToAck(void *pECB)
{ return(A_TxDone(ToAck,pECB)); }

static int A_TxDoneToLineCheck(void *pECB)
{ return(A_TxDone(ToLineCheck,pECB)); }

static int A_TxIncrCtPermErrCtPermErr(void* pECB)
{ return(A_TxIncr(CtPermErr,CtPermErr,pECB)); }

static int Sw_CuRetrycDataRetry(void *pECB)
{ return(Sw_CuRetry(cDataRetry,pECB)); }

static int Sw_RetrylTtdRetry(void *pECB)
{ return(Sw_Retry(lTtdRetry,pECB)); }

static int Sw_RetrylNakRetry(void *pECB)
{ return(Sw_Retry(lNakRetry,pECB)); }

static int Sw_RetrylEnqRetry(void *pECB)
{ return(Sw_Retry(lEnqRetry,pECB)); }

static int Sw_RetrylWackRetry(void *pECB)
{ return(Sw_Retry(lWackRetry,pECB)); }

static int Sw_RetrylDataRetry(void *pECB)
{ return(Sw_Retry(lDataRetry,pECB)); }


/* ---- polleefsm Action Vector Jump Table [MaxStates][MaxEvents] ---- */
const static int polleefsm_CU_DevBusy[] = {
     453, 454, 455, 455, 455, 443, 441, 455, 455, 455, 455, 455, 455, 455, 455, 
     452, 450, 451
};
const static int polleefsm_CU_Down[] = {
     1, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
     3, 3, 4
};
const static int polleefsm_CU_EOFile[] = {
     171, 172, 161, 173, 173, 175, 141, 143, 152, 173, 173, 173, 173, 166, 173, 
     170, 169, 168
};
const static int polleefsm_CU_Idle[] = {
     53, 54, 55, 55, 55, 6, 50, 55, 55, 55, 55, 55, 55, 55, 55, 
     52, 48, 51
};
const static int polleefsm_CU_InFile[] = {
     135, 136, 125, 137, 137, 139, 104, 107, 116, 137, 137, 137, 137, 130, 137, 
     134, 133, 132
};
const static int polleefsm_CU_Selected[] = {
     100, 101, 90, 102, 102, 56, 58, 60, 75, 102, 102, 102, 102, 95, 102, 
     99, 98, 97
};
const static int polleefsm_CU_TtdDelay[] = {
     385, 386, 397, 397, 397, 395, 387, 397, 397, 397, 397, 397, 397, 397, 397, 
     390, 377, 397
};
const static int polleefsm_CU_TtdSent[] = {
     437, 438, 439, 439, 439, 412, 439, 439, 439, 439, 439, 400, 439, 439, 439, 
     436, 424, 435
};
const static int polleefsm_CU_TxEOFile[] = {
     293, 294, 253, 261, 274, 289, 219, 241, 229, 289, 289, 279, 284, 289, 225, 
     292, 288, 291
};
const static int polleefsm_CU_TxInFile[] = {
     217, 218, 213, 188, 200, 213, 177, 213, 213, 213, 213, 205, 210, 213, 183, 
     216, 212, 215
};
const static int polleefsm_CU_TxRetry[] = {
     375, 376, 355, 305, 317, 371, 295, 343, 331, 371, 371, 326, 363, 371, 301, 
     374, 367, 373
};
const static int polleefsm_CU_TxSpecPollData[] = {
     486, 487, 482, 462, 465, 482, 457, 482, 482, 482, 482, 471, 478, 482, 459, 
     485, 481, 484
};
const static int polleefsm_CU_TxSts[] = {
     552, 553, 518, 526, 539, 548, 488, 506, 494, 548, 548, 541, 543, 548, 490, 
     551, 547, 550
};

const static int *polleefsm_StateEvent[] = {
        polleefsm_CU_DevBusy,
        polleefsm_CU_Down,
        polleefsm_CU_EOFile,
        polleefsm_CU_Idle,
        polleefsm_CU_InFile,
        polleefsm_CU_Selected,
        polleefsm_CU_TtdDelay,
        polleefsm_CU_TtdSent,
        polleefsm_CU_TxEOFile,
        polleefsm_CU_TxInFile,
        polleefsm_CU_TxRetry,
        polleefsm_CU_TxSpecPollData,
        polleefsm_CU_TxSts
};

/* ---- Action Vector Table ---- */
const static AAEntry    polleefsm_ActArray[] = {
     /* Null Entry */     {0, 0, 0},    /* fcn, type, jmp or newstate */
     /* Action   1 */     {0, 2, 520},
     /* Action   2 */     {A_StartTimerToLineCheck, 5, 3},
     /* Action   3 */     {A_CuFlushAllCtPermErr, 5, 520},
     /* Action   4 */     {A_RcvFrFreeCtNoneCtNone, 5, 521},
     /* Action   5 */     {A_CheckCstate, 1, 520},
     /* Action   6 */     {A_ResetLineTxAck0RxAck1, 5, 521},
     /* Switch   7 */     {Sw_bidType, 3, 76},
     /* Action   8 */     {A_RcvFrFreeCtPollCtPoll, 5, 521},
     /* Action   9 */     {A_ClearCuNoPollFlag, 1, 521},
     /* Switch  10 */     {Sw_Cu_MoreData, 3, 24},
     /* Switch  11 */     {Sw_Frame, 3, 38},
     /* Action  12 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action  13 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action  14 */     {A_CuFreeData, 1, 521},
     /* Action  15 */     {A_SendFrEotCtEotCtNegPolp, 5, 520},
     /* Switch  16 */     {Sw_cState, 3, 78},
     /* Action  17 */     {A_SendFrEotCtNegPolpCtNegPolp, 5, 520},
     /* Action  18 */     {A_RxIncrCtPollToCtPollTo, 5, 520},
     /* Action  19 */     {A_MakeSts, 1, 521},
     /* Action  20 */     {A_SendFrStatusCtDataCtData, 5, 12},
     /* Action  21 */     {A_RcvFrFreeCtSelCtSel, 5, 521},
     /* Action  22 */     {A_ClearCuNoPollFlag, 1, 521},
     /* Switch  23 */     {Sw_DeviceState, 3, 26},
     /* Action  24 */     {A_SendFrWackCtWackCtWack, 5, 0},
     /* Action  25 */     {A_ClearStsData, 1, 521},
     /* Action  26 */     {A_SendFrAckCtAckCtAck, 5, 5},
     /* Action  27 */     {A_SendFrAckCtAckCtAck, 5, 5},
     /* Action  28 */     {A_SendFrRviCtRviCtRvi, 5, 0},
     /* Action  29 */     {A_RcvFrFreeCtSPollCtSPoll, 5, 521},
     /* Action  30 */     {A_ClearCuNoPollFlag, 1, 521},
     /* Switch  31 */     {Sw_SpecPollOnly, 3, 75},
     /* Switch  32 */     {Sw_MoreSpecData, 3, 39},
     /* Action  33 */     {A_MakeSts, 1, 521},
     /* Action  34 */     {A_SendFrStatusCtDataCtData, 5, 11},
     /* Action  35 */     {A_SendFrEotCtEotCtEot, 5, 520},
     /* Switch  36 */     {Sw_Cu_MoreData, 3, 23},
     /* Switch  37 */     {Sw_Frame, 3, 37},
     /* Action  38 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action  39 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action  40 */     {A_CuFreeData, 1, 521},
     /* Action  41 */     {A_SendFrEotCtEotCtNegPolp, 5, 520},
     /* Switch  42 */     {Sw_cState, 3, 77},
     /* Action  43 */     {A_SendFrEotCtNegPolpCtNegPolp, 5, 520},
     /* Action  44 */     {A_RxIncrCtPollToCtPollTo, 5, 520},
     /* Action  45 */     {A_MakeSts, 1, 521},
     /* Action  46 */     {A_SendFrStatusCtDataCtData, 5, 12},
     /* Action  47 */     {A_RcvFrFreeCtPollCtNone, 5, 520},
     /* Action  48 */     {A_CheckCuPollStop, 1, 521},
     /* Action  49 */     {A_StartTimerToLineCheck, 5, 520},
     /* Action  50 */     {A_RcvFrFreeCtEotCtNone, 5, 520},
     /* Action  51 */     {A_TxDoneToLineCheck, 5, 520},
     /* Action  52 */     {0, 2, 3},
     /* Action  53 */     {0, 2, 1},
     /* Action  54 */     {0, 2, 520},
     /* Action  55 */     {A_RcvFrFreeCtNoneCtNone, 5, 520},
     /* Action  56 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Action  57 */     {A_SendFrControlCtAckRsCtAckRs, 5, 520},
     /* Action  58 */     {A_RcvFrFreeCtEotCtEot, 5, 521},
     /* Action  59 */     {A_StartTimerToLineCheck, 5, 3},
     /* Switch  60 */     {Sw_CrcValid, 3, 11},
     /* Action  61 */     {A_SetTxAck, 1, 521},
     /* Switch  62 */     {Sw_Printer, 3, 41},
     /* Action  63 */     {A_RcvFrSelPassEtbCtDataCtData, 5, 521},
     /* Action  64 */     {A_SendFrAckCtAckCtAck, 5, 4},
     /* Action  65 */     {A_RcvFrSelPassEtbCtDataCtData, 5, 521},
     /* Switch  66 */     {Sw_Send_Wack, 3, 74},
     /* Action  67 */     {A_SendFrWackCtWackCtWack, 5, 521},
     /* Action  68 */     {A_SetDevBusy, 1, 0},
     /* Action  69 */     {A_SendFrAckCtAckCtAck, 5, 2},
     /* Action  70 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch  71 */     {Sw_RetrylDataRetry, 6, 72},
     /* Action  72 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action  73 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action  74 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Switch  75 */     {Sw_CrcValid, 3, 10},
     /* Action  76 */     {A_SetTxAck, 1, 521},
     /* Switch  77 */     {Sw_Printer, 3, 40},
     /* Action  78 */     {A_RcvFrSelPassEtxCtDataCtData, 5, 521},
     /* Action  79 */     {A_SendFrAckCtAckCtAck, 5, 2},
     /* Action  80 */     {A_RcvFrSelPassEtxCtDataCtData, 5, 521},
     /* Switch  81 */     {Sw_Send_Wack, 3, 73},
     /* Action  82 */     {A_SendFrWackCtWackCtWack, 5, 521},
     /* Action  83 */     {A_SetDevBusy, 1, 0},
     /* Action  84 */     {A_SendFrAckCtAckCtAck, 5, 2},
     /* Action  85 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch  86 */     {Sw_RetrylDataRetry, 6, 71},
     /* Action  87 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action  88 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action  89 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action  90 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Switch  91 */     {Sw_RetrylDataRetry, 6, 70},
     /* Action  92 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action  93 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action  94 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action  95 */     {A_RcvFrFreeCtTtdCtTtd, 5, 521},
     /* Action  96 */     {A_SendFrNakCtTtdRspCtTtdRsp, 5, 520},
     /* Action  97 */     {A_TxDoneToAck, 5, 520},
     /* Action  98 */     {A_StartTimerToLineCheck, 5, 520},
     /* Action  99 */     {0, 2, 520},
     /* Action 100 */     {0, 2, 1},
     /* Action 101 */     {0, 2, 520},
     /* Action 102 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 103 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 104 */     {A_RcvFrNotifyCtAbortCtAbort, 5, 521},
     /* Action 105 */     {A_PeerAlertSEL_DATA_ABORT, 5, 521},
     /* Action 106 */     {A_StartTimerToLineCheck, 5, 3},
     /* Switch 107 */     {Sw_CrcValid, 3, 9},
     /* Action 108 */     {A_SetTxAck, 1, 521},
     /* Action 109 */     {A_RcvFrSelPassEtbCtDataCtData, 5, 521},
     /* Action 110 */     {A_SendFrAckCtAckCtAck, 5, 4},
     /* Action 111 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 112 */     {Sw_RetrylDataRetry, 6, 69},
     /* Action 113 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 114 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 115 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Switch 116 */     {Sw_CrcValid, 3, 8},
     /* Action 117 */     {A_SetTxAck, 1, 521},
     /* Action 118 */     {A_RcvFrSelPassEtxCtDataCtData, 5, 521},
     /* Action 119 */     {A_SendFrAckCtAckCtAck, 5, 2},
     /* Action 120 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 121 */     {Sw_RetrylDataRetry, 6, 68},
     /* Action 122 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 123 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 124 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action 125 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Switch 126 */     {Sw_RetrylDataRetry, 6, 67},
     /* Action 127 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 128 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 129 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action 130 */     {A_RcvFrFreeCtTtdCtTtd, 5, 521},
     /* Action 131 */     {A_SendFrNakCtTtdRspCtTtdRsp, 5, 520},
     /* Action 132 */     {A_TxDoneToAck, 5, 520},
     /* Action 133 */     {A_StartTimerToLineCheck, 5, 520},
     /* Action 134 */     {0, 2, 520},
     /* Action 135 */     {0, 2, 1},
     /* Action 136 */     {0, 2, 520},
     /* Action 137 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 138 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 139 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Action 140 */     {A_SendFrControlCtAckRsCtAckRs, 5, 520},
     /* Action 141 */     {A_RcvFrFreeCtEotCtEot, 5, 521},
     /* Action 142 */     {A_StartTimerToLineCheck, 5, 3},
     /* Switch 143 */     {Sw_CrcValid, 3, 7},
     /* Action 144 */     {A_SetTxAck, 1, 521},
     /* Action 145 */     {A_RcvFrSelPassEtbCtDataCtData, 5, 521},
     /* Action 146 */     {A_SendFrAckCtAckCtAck, 5, 4},
     /* Action 147 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 148 */     {Sw_RetrylDataRetry, 6, 66},
     /* Action 149 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 150 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 151 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Switch 152 */     {Sw_CrcValid, 3, 6},
     /* Action 153 */     {A_SetTxAck, 1, 521},
     /* Action 154 */     {A_RcvFrSelPassEtxCtDataCtData, 5, 521},
     /* Action 155 */     {A_SendFrAckCtAckCtAck, 5, 2},
     /* Action 156 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 157 */     {Sw_RetrylDataRetry, 6, 65},
     /* Action 158 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 159 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 160 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action 161 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Switch 162 */     {Sw_RetrylDataRetry, 6, 64},
     /* Action 163 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 164 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 165 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 166 */     {A_RcvFrFreeCtTtdCtTtd, 5, 521},
     /* Action 167 */     {A_SendFrNakCtTtdRspCtTtdRsp, 5, 520},
     /* Action 168 */     {A_TxDoneToAck, 5, 520},
     /* Action 169 */     {A_StartTimerToLineCheck, 5, 520},
     /* Action 170 */     {0, 2, 520},
     /* Action 171 */     {0, 2, 1},
     /* Action 172 */     {0, 2, 520},
     /* Action 173 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 174 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 175 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Action 176 */     {A_SendFrControlCtAckRsCtAckRs, 5, 520},
     /* Action 177 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Action 178 */     {A_StartTimerToLineCheck, 5, 521},
     /* Switch 179 */     {Sw_CuRetrycDataRetry, 6, 14},
     /* Action 180 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 181 */     {A_ResetCUcDataRetry, 5, 3},
     /* Action 182 */     {0, 2, 3},
     /* Action 183 */     {A_RcvFrFreeCtWackCtWack, 5, 521},
     /* Switch 184 */     {Sw_RetrylWackRetry, 6, 63},
     /* Action 185 */     {A_CuFlushCtAbort, 5, 521},
     /* Action 186 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 187 */     {A_SendFrEnqCtEnqCtEnq, 5, 520},
     /* Action 188 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 189 */     {A_CuFreeData, 1, 521},
     /* Action 190 */     {A_SetRxAck, 1, 521},
     /* Action 191 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 192 */     {Sw_Cu_MoreData, 3, 22},
     /* Switch 193 */     {Sw_Frame, 3, 36},
     /* Action 194 */     {A_SendFrCuDataCtDataCtData, 5, 520},
     /* Action 195 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action 196 */     {A_CuFreeData, 1, 521},
     /* Action 197 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 198 */     {A_StartTimerToTtdDelay, 5, 6},
     /* Action 199 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 200 */     {A_RcvFrFreeCtBadAckCtBadAck, 5, 521},
     /* Switch 201 */     {Sw_RetrylEnqRetry, 6, 62},
     /* Action 202 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 203 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 204 */     {A_SendFrEnqCtEnqCtEnq, 5, 520},
     /* Action 205 */     {A_RcvFrFreeCtNakCtNak, 5, 521},
     /* Switch 206 */     {Sw_RetrylNakRetry, 6, 61},
     /* Action 207 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 208 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 209 */     {A_SendFrCuDataCtRetranCtRetran, 5, 520},
     /* Action 210 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 211 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 212 */     {A_SendFrEnqCtEnqCtEnq, 5, 10},
     /* Action 213 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 214 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 215 */     {A_TxDoneToEnq, 5, 520},
     /* Action 216 */     {0, 2, 520},
     /* Action 217 */     {0, 2, 1},
     /* Action 218 */     {0, 2, 520},
     /* Action 219 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Action 220 */     {A_StartTimerToLineCheck, 5, 521},
     /* Switch 221 */     {Sw_CuRetrycDataRetry, 6, 13},
     /* Action 222 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 223 */     {A_ResetCUcDataRetry, 5, 3},
     /* Action 224 */     {0, 2, 3},
     /* Action 225 */     {A_RcvFrFreeCtWackCtWack, 5, 521},
     /* Action 226 */     {A_CuFreeData, 1, 521},
     /* Action 227 */     {A_ResetCUcDataRetry, 5, 521},
     /* Action 228 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 229 */     {A_CuFreeData, 1, 521},
     /* Action 230 */     {A_SetRxAck, 1, 521},
     /* Action 231 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 232 */     {Sw_CrcValid, 3, 5},
     /* Action 233 */     {A_SetTxAck, 1, 521},
     /* Action 234 */     {A_RcvFrSelPassEtxCtDataCtData, 5, 521},
     /* Action 235 */     {A_SendFrAckCtAckCtAck, 5, 2},
     /* Action 236 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 237 */     {Sw_RetrylDataRetry, 6, 60},
     /* Action 238 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 239 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 240 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 241 */     {A_CuFreeData, 1, 521},
     /* Action 242 */     {A_SetRxAck, 1, 521},
     /* Action 243 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 244 */     {Sw_CrcValid, 3, 4},
     /* Action 245 */     {A_SetTxAck, 1, 521},
     /* Action 246 */     {A_RcvFrSelPassEtbCtDataCtData, 5, 521},
     /* Action 247 */     {A_SendFrAckCtAckCtAck, 5, 4},
     /* Action 248 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 249 */     {Sw_RetrylDataRetry, 6, 59},
     /* Action 250 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 251 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 252 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 253 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 254 */     {A_CuFreeData, 1, 521},
     /* Action 255 */     {A_SetRxAck, 1, 521},
     /* Action 256 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 257 */     {Sw_RetrylDataRetry, 6, 58},
     /* Action 258 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 259 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 260 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 261 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 262 */     {A_CuFreeData, 1, 521},
     /* Action 263 */     {A_SetRxAck, 1, 521},
     /* Action 264 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 265 */     {Sw_Cu_MoreData, 3, 21},
     /* Switch 266 */     {Sw_Frame, 3, 35},
     /* Action 267 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action 268 */     {A_SendFrCuDataCtDataCtData, 5, 520},
     /* Action 269 */     {A_CuFreeData, 1, 521},
     /* Action 270 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 271 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 272 */     {A_MakeSts, 1, 521},
     /* Action 273 */     {A_SendFrStatusCtDataCtData, 5, 12},
     /* Action 274 */     {A_RcvFrFreeCtBadAckCtBadAck, 5, 521},
     /* Switch 275 */     {Sw_RetrylEnqRetry, 6, 57},
     /* Action 276 */     {A_CuFlushCtAbort, 5, 521},
     /* Action 277 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 278 */     {A_SendFrEnqCtEnqCtEnq, 5, 520},
     /* Action 279 */     {A_RcvFrFreeCtNakCtNak, 5, 521},
     /* Switch 280 */     {Sw_RetrylEnqRetry, 6, 56},
     /* Action 281 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 282 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 283 */     {A_SendFrCuDataCtRetranCtRetran, 5, 520},
     /* Action 284 */     {A_RcvFrFreeCtRviCtRvi, 5, 521},
     /* Action 285 */     {A_CuFreeData, 1, 521},
     /* Action 286 */     {A_ResetCUcDataRetry, 5, 521},
     /* Action 287 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 288 */     {A_SendFrEnqCtEnqCtEnq, 5, 10},
     /* Action 289 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 290 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 291 */     {A_TxDoneToEnq, 5, 520},
     /* Action 292 */     {0, 2, 520},
     /* Action 293 */     {0, 2, 1},
     /* Action 294 */     {0, 2, 520},
     /* Action 295 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Action 296 */     {A_StartTimerToLineCheck, 5, 521},
     /* Switch 297 */     {Sw_CuRetrycDataRetry, 6, 12},
     /* Action 298 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 299 */     {A_ResetCUcDataRetry, 5, 3},
     /* Action 300 */     {0, 2, 3},
     /* Action 301 */     {A_RcvFrFreeCtWackCtWack, 5, 521},
     /* Action 302 */     {A_CuFreeData, 1, 521},
     /* Action 303 */     {A_ResetCUcDataRetry, 5, 521},
     /* Action 304 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 305 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 306 */     {A_CuFreeData, 1, 521},
     /* Action 307 */     {A_SetRxAck, 1, 521},
     /* Action 308 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 309 */     {Sw_Cu_MoreData, 3, 20},
     /* Switch 310 */     {Sw_Frame, 3, 34},
     /* Action 311 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action 312 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action 313 */     {A_CuFreeData, 1, 521},
     /* Action 314 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 315 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 316 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 317 */     {A_RcvFrFreeCtBadAckCtBadAck, 5, 521},
     /* Switch 318 */     {Sw_RetrylEnqRetry, 6, 55},
     /* Action 319 */     {A_CuFlushCtAbort, 5, 521},
     /* Action 320 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Switch 321 */     {Sw_Frame, 3, 33},
     /* Action 322 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action 323 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action 324 */     {A_CuFreeData, 1, 521},
     /* Action 325 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 326 */     {A_RcvFrFreeCtNakCtNak, 5, 521},
     /* Switch 327 */     {Sw_RetrylNakRetry, 6, 54},
     /* Action 328 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 329 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 330 */     {A_SendFrCuDataCtRetranCtRetran, 5, 520},
     /* Action 331 */     {A_CuFreeData, 1, 521},
     /* Action 332 */     {A_SetRxAck, 1, 521},
     /* Action 333 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 334 */     {Sw_CrcValid, 3, 3},
     /* Action 335 */     {A_SetTxAck, 1, 521},
     /* Action 336 */     {A_RcvFrSelPassEtxCtDataCtData, 5, 521},
     /* Action 337 */     {A_SendFrAckCtAckCtAck, 5, 2},
     /* Action 338 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 339 */     {Sw_RetrylDataRetry, 6, 53},
     /* Action 340 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 341 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 342 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 343 */     {A_CuFreeData, 1, 521},
     /* Action 344 */     {A_SetRxAck, 1, 521},
     /* Action 345 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 346 */     {Sw_CrcValid, 3, 2},
     /* Action 347 */     {A_SetTxAck, 1, 521},
     /* Action 348 */     {A_RcvFrSelPassEtbCtDataCtData, 5, 521},
     /* Action 349 */     {A_SendFrAckCtAckCtAck, 5, 4},
     /* Action 350 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 351 */     {Sw_RetrylDataRetry, 6, 52},
     /* Action 352 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 353 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 354 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 355 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 356 */     {A_CuFreeData, 1, 521},
     /* Action 357 */     {A_SetRxAck, 1, 521},
     /* Action 358 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 359 */     {Sw_RetrylDataRetry, 6, 51},
     /* Action 360 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 361 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 362 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 363 */     {A_RcvFrFreeCtRviCtRvi, 5, 521},
     /* Action 364 */     {A_CuFreeData, 1, 521},
     /* Action 365 */     {A_ResetCUcDataRetry, 5, 521},
     /* Action 366 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Switch 367 */     {Sw_RetrylEnqRetry, 6, 50},
     /* Action 368 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 369 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 370 */     {A_SendFrEnqCtEnqCtEnq, 5, 10},
     /* Action 371 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 372 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 373 */     {A_TxDoneToEnq, 5, 520},
     /* Action 374 */     {0, 2, 520},
     /* Action 375 */     {0, 2, 1},
     /* Action 376 */     {0, 2, 520},
     /* Switch 377 */     {Sw_Cu_MoreData, 3, 19},
     /* Switch 378 */     {Sw_Frame, 3, 32},
     /* Action 379 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action 380 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action 381 */     {A_CuFreeData, 1, 521},
     /* Action 382 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 383 */     {A_SendFrTtdCtTtdCtTtd, 5, 7},
     /* Action 384 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 385 */     {0, 2, 1},
     /* Action 386 */     {0, 2, 520},
     /* Action 387 */     {A_RcvFrFreeCtEotCtEot, 5, 521},
     /* Action 388 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 389 */     {A_StartTimerToLineCheck, 5, 3},
     /* Switch 390 */     {Sw_Frame, 3, 31},
     /* Action 391 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action 392 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action 393 */     {A_CuFreeData, 1, 521},
     /* Action 394 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 395 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Action 396 */     {A_SendFrTtdCtTtdCtTtd, 5, 7},
     /* Action 397 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 398 */     {A_CuFlushCtPermErr, 5, 521},
     /* Action 399 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 400 */     {A_RcvFrFreeCtTtdRspCtTtdRsp, 5, 521},
     /* Switch 401 */     {Sw_Cu_MoreData, 3, 18},
     /* Switch 402 */     {Sw_Frame, 3, 30},
     /* Action 403 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action 404 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action 405 */     {A_CuFreeData, 1, 521},
     /* Action 406 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Switch 407 */     {Sw_RetrylTtdRetry, 6, 49},
     /* Action 408 */     {A_CuFlushCtAbort, 5, 521},
     /* Action 409 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 410 */     {A_StartTimerToTtdDelay, 5, 6},
     /* Action 411 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 412 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Switch 413 */     {Sw_Cu_MoreData, 3, 17},
     /* Switch 414 */     {Sw_Frame, 3, 29},
     /* Action 415 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action 416 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action 417 */     {A_CuFreeData, 1, 521},
     /* Action 418 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Switch 419 */     {Sw_RetrylTtdRetry, 6, 48},
     /* Action 420 */     {A_CuFlushCtAbort, 5, 521},
     /* Action 421 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 422 */     {A_StartTimerToTtdDelay, 5, 6},
     /* Action 423 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Switch 424 */     {Sw_Cu_MoreData, 3, 16},
     /* Switch 425 */     {Sw_Frame, 3, 28},
     /* Action 426 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action 427 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action 428 */     {A_CuFreeData, 1, 521},
     /* Action 429 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Switch 430 */     {Sw_RetrylTtdRetry, 6, 47},
     /* Action 431 */     {A_CuFlushCtAbort, 5, 521},
     /* Action 432 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 433 */     {A_StartTimerToTtdDelay, 5, 6},
     /* Action 434 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 435 */     {A_TxDoneToEnq, 5, 520},
     /* Action 436 */     {0, 2, 520},
     /* Action 437 */     {0, 2, 1},
     /* Action 438 */     {0, 2, 520},
     /* Action 439 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 440 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 441 */     {A_RcvFrFreeCtEotCtEot, 5, 521},
     /* Action 442 */     {A_StartTimerToLineCheck, 5, 3},
     /* Action 443 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Switch 444 */     {Sw_DeviceState, 3, 25},
     /* Action 445 */     {A_SendFrWackCtWackCtWack, 5, 0},
     /* Action 446 */     {A_ClearStsData, 1, 521},
     /* Action 447 */     {A_SendFrAckCtAckCtAck, 5, 5},
     /* Action 448 */     {A_SendFrAckCtAckCtAck, 5, 5},
     /* Action 449 */     {A_SendFrRviCtRviCtRvi, 5, 0},
     /* Action 450 */     {A_StartTimerToLineCheck, 5, 520},
     /* Action 451 */     {A_TxDoneToAck, 5, 520},
     /* Action 452 */     {0, 2, 520},
     /* Action 453 */     {0, 2, 1},
     /* Action 454 */     {0, 2, 520},
     /* Action 455 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 456 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 457 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Action 458 */     {A_StartTimerToLineCheck, 5, 3},
     /* Action 459 */     {A_RcvFrFreeCtWackCtWack, 5, 521},
     /* Action 460 */     {A_ClearStsData, 1, 521},
     /* Action 461 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 462 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 463 */     {A_ClearStsData, 1, 521},
     /* Action 464 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 465 */     {A_RcvFrFreeCtBadAckCtBadAck, 5, 521},
     /* Switch 466 */     {Sw_RetrylEnqRetry, 6, 46},
     /* Action 467 */     {A_ClearStsData, 1, 521},
     /* Action 468 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 469 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 470 */     {A_SendFrEnqCtEnqCtEnq, 5, 520},
     /* Action 471 */     {A_RcvFrFreeCtNakCtNak, 5, 521},
     /* Switch 472 */     {Sw_RetrylNakRetry, 6, 45},
     /* Action 473 */     {A_ClearStsData, 1, 521},
     /* Action 474 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 475 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 476 */     {A_MakeSts, 1, 521},
     /* Action 477 */     {A_SendFrStatusCtRetranCtRetran, 5, 520},
     /* Action 478 */     {A_RcvFrFreeCtRviCtRvi, 5, 521},
     /* Action 479 */     {A_ClearStsData, 1, 521},
     /* Action 480 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 481 */     {A_SendFrEnqCtEnqCtEnq, 5, 520},
     /* Action 482 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 483 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 484 */     {A_TxDoneToEnq, 5, 520},
     /* Action 485 */     {0, 2, 520},
     /* Action 486 */     {0, 2, 1},
     /* Action 487 */     {0, 2, 520},
     /* Action 488 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Action 489 */     {A_StartTimerToLineCheck, 5, 3},
     /* Action 490 */     {A_RcvFrFreeCtWackCtWack, 5, 521},
     /* Action 491 */     {A_ClearStsData, 1, 521},
     /* Action 492 */     {A_ResetCUcDataRetry, 5, 521},
     /* Action 493 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 494 */     {A_ClearStsData, 1, 521},
     /* Action 495 */     {A_SetRxAck, 1, 521},
     /* Action 496 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 497 */     {Sw_CrcValid, 3, 1},
     /* Action 498 */     {A_SetTxAck, 1, 521},
     /* Action 499 */     {A_RcvFrSelPassEtxCtDataCtData, 5, 521},
     /* Action 500 */     {A_SendFrAckCtAckCtAck, 5, 2},
     /* Action 501 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 502 */     {Sw_RetrylDataRetry, 6, 44},
     /* Action 503 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 504 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 505 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 506 */     {A_ClearStsData, 1, 521},
     /* Action 507 */     {A_SetRxAck, 1, 521},
     /* Action 508 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 509 */     {Sw_CrcValid, 3, 0},
     /* Action 510 */     {A_SetTxAck, 1, 521},
     /* Action 511 */     {A_RcvFrSelPassEtbCtDataCtData, 5, 521},
     /* Action 512 */     {A_SendFrAckCtAckCtAck, 5, 4},
     /* Action 513 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 514 */     {Sw_RetrylDataRetry, 6, 43},
     /* Action 515 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 516 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 517 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 518 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 519 */     {A_ClearStsData, 1, 521},
     /* Action 520 */     {A_SetRxAck, 1, 521},
     /* Action 521 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 522 */     {Sw_RetrylDataRetry, 6, 42},
     /* Action 523 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 524 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 525 */     {A_SendFrNakCtNakCtNak, 5, 2},
     /* Action 526 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 527 */     {A_ClearStsData, 1, 521},
     /* Action 528 */     {A_SetRxAck, 1, 521},
     /* Action 529 */     {A_ResetCUcDataRetry, 5, 521},
     /* Switch 530 */     {Sw_Cu_MoreData, 3, 15},
     /* Switch 531 */     {Sw_Frame, 3, 27},
     /* Action 532 */     {A_SendFrCuDataCtDataCtData, 5, 9},
     /* Action 533 */     {A_SendFrCuDataCtDataCtData, 5, 8},
     /* Action 534 */     {A_ClearStsData, 1, 521},
     /* Action 535 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 536 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 537 */     {A_MakeSts, 1, 521},
     /* Action 538 */     {A_SendFrStatusCtDataCtData, 5, 12},
     /* Action 539 */     {A_RcvFrFreeCtBadAckCtBadAck, 5, 521},
     /* Action 540 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 541 */     {A_RcvFrFreeCtNakCtNak, 5, 521},
     /* Action 542 */     {A_SendFrEotCtAbortCtAbort, 5, 3},
     /* Action 543 */     {A_RcvFrFreeCtRviCtRvi, 5, 521},
     /* Action 544 */     {A_ClearStsData, 1, 521},
     /* Action 545 */     {A_ResetCUcDataRetry, 5, 521},
     /* Action 546 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 547 */     {A_SendFrEnqCtEnqCtEnq, 5, 10},
     /* Action 548 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 549 */     {A_SendFrEotCtEotCtEot, 5, 3},
     /* Action 550 */     {A_TxDoneToEnq, 5, 520},
     /* Action 551 */     {0, 2, 520},
     /* Action 552 */     {0, 2, 1},
     /* Action 553 */     {0, 2, 520},
};

/* ---- polleefsm state machine definitions ---- */
const SM polleefsm = {
    polleefsm_MaxStates,    /* max states */
    polleefsm_MaxEvents,    /* max events */
    polleefsm_ActArraySize, /* # of entries in the action array */
    polleefsm_SwLblVect,    /* Switch return value ActArray indicies */
    polleefsm_SwTbl,        /* SwLblVect index & # of rtn values */
    polleefsm_StateEvent,   /* state/event to action vector tbl */
    polleefsm_ActArray      /* action vector table */
};

