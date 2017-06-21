/* $Id: bsc_lack_poller_fsm.c,v 3.2.60.2 1996/07/19 22:44:57 snyder Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_lack_poller_fsm.c,v $
 *------------------------------------------------------------------
 * BSC local-ack poller FSM
 *
 * March 1995, Andrew R. Rothwell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_lack_poller_fsm.c,v $
 * Revision 3.2.60.2  1996/07/19  22:44:57  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/17  10:40:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/22  08:57:44  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:50:15  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  08:48:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  06:24:35  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* ---- pollerfsm state machine table file ---- */

#include "cape.h"
#include "bsc_cmd_set.h"
#include "bsc_lack_fsm.h"
#include "bsc_lcb.h"

/* Switch Return Value Table (indices into the Action Vector Table) */
const static int pollerfsm_SwLblVect[] = {
   496,   499,          /* 'Sw_CrcValid' */
   482,   490,          /* 'Sw_CrcValid' */
   362,   370,          /* 'Sw_CrcValid' */
   352,   355,          /* 'Sw_CrcValid' */
   290,   298,          /* 'Sw_CrcValid' */
   280,   283,          /* 'Sw_CrcValid' */
   151,   159,          /* 'Sw_CrcValid' */
   142,   145,          /* 'Sw_CrcValid' */
   107,   110,          /* 'Sw_CrcValid' */
    98,   101,          /* 'Sw_CrcValid' */
    52,    60,          /* 'Sw_CrcValid' */
    42,    45,          /* 'Sw_CrcValid' */
   339,   341,          /* 'Sw_CuRetry' */
   317,   319,          /* 'Sw_CuRetry' */
   266,   268,          /* 'Sw_CuRetry' */
   251,   253,          /* 'Sw_CuRetry' */
   225,   227,          /* 'Sw_CuRetry' */
   214,   216,          /* 'Sw_CuRetry' */
   206,   208,          /* 'Sw_CuRetry' */
    78,    81,          /* 'Sw_CuRetry' */
   459,   460,   461,          /* 'Sw_Frame' */
   449,   450,   451,          /* 'Sw_Frame' */
   438,   439,   440,          /* 'Sw_Frame' */
   423,   424,   425,          /* 'Sw_Frame' */
   415,   416,   417,          /* 'Sw_Frame' */
   395,   396,   397,          /* 'Sw_Frame' */
   386,   387,   388,          /* 'Sw_Frame' */
   239,   240,   241,          /* 'Sw_Frame' */
   200,   201,   202,          /* 'Sw_Frame' */
   458,   463,          /* 'Sw_MoreData' */
   448,   453,          /* 'Sw_MoreData' */
   437,   442,          /* 'Sw_MoreData' */
   414,   419,          /* 'Sw_MoreData' */
   238,   243,          /* 'Sw_MoreData' */
   477,   478,   479,   480,          /* 'Sw_NextWork' */
   137,   138,   139,   140,          /* 'Sw_NextWork' */
    84,    85,    86,    87,          /* 'Sw_NextWork' */
    36,    37,    38,    39,          /* 'Sw_NextWork' */
    24,    25,    26,    27,          /* 'Sw_NextWork' */
    18,    19,    20,    21,          /* 'Sw_NextWork' */
    12,    13,    14,    15,          /* 'Sw_NextWork' */
     3,     4,     5,     6,          /* 'Sw_NextWork' */
   501,   503,          /* 'Sw_Retry' */
   492,   494,          /* 'Sw_Retry' */
   464,   466,          /* 'Sw_Retry' */
   454,   456,          /* 'Sw_Retry' */
   443,   445,          /* 'Sw_Retry' */
   404,   406,          /* 'Sw_Retry' */
   392,   394,          /* 'Sw_Retry' */
   383,   385,          /* 'Sw_Retry' */
   378,   380,          /* 'Sw_Retry' */
   372,   374,          /* 'Sw_Retry' */
   357,   359,          /* 'Sw_Retry' */
   327,   329,          /* 'Sw_Retry' */
   306,   308,          /* 'Sw_Retry' */
   300,   302,          /* 'Sw_Retry' */
   285,   287,          /* 'Sw_Retry' */
   246,   248,          /* 'Sw_Retry' */
   231,   233,          /* 'Sw_Retry' */
   171,   173,          /* 'Sw_Retry' */
   166,   168,          /* 'Sw_Retry' */
   161,   163,          /* 'Sw_Retry' */
   147,   149,          /* 'Sw_Retry' */
   122,   124,          /* 'Sw_Retry' */
   117,   119,          /* 'Sw_Retry' */
   112,   114,          /* 'Sw_Retry' */
   103,   105,          /* 'Sw_Retry' */
    74,    76,          /* 'Sw_Retry' */
    68,    70,          /* 'Sw_Retry' */
    62,    64,          /* 'Sw_Retry' */
    47,    49,          /* 'Sw_Retry' */
   483,   486,          /* 'Sw_RxFrame' */
   363,   366,          /* 'Sw_RxFrame' */
   291,   294,          /* 'Sw_RxFrame' */
   152,   155,          /* 'Sw_RxFrame' */
    53,    56          /* 'Sw_RxFrame' */
}; 

/* Indicies into the pollerfsm_SwLblVect array, # of return values */
static const SWTBL pollerfsm_SwTbl[] = {
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
    {30, 2},
    {32, 2},
    {34, 2},
    {36, 2},
    {38, 2},
    {40, 3},
    {43, 3},
    {46, 3},
    {49, 3},
    {52, 3},
    {55, 3},
    {58, 3},
    {61, 3},
    {64, 3},
    {67, 2},
    {69, 2},
    {71, 2},
    {73, 2},
    {75, 2},
    {77, 4},
    {81, 4},
    {85, 4},
    {89, 4},
    {93, 4},
    {97, 4},
    {101, 4},
    {105, 4},
    {109, 2},
    {111, 2},
    {113, 2},
    {115, 2},
    {117, 2},
    {119, 2},
    {121, 2},
    {123, 2},
    {125, 2},
    {127, 2},
    {129, 2},
    {131, 2},
    {133, 2},
    {135, 2},
    {137, 2},
    {139, 2},
    {141, 2},
    {143, 2},
    {145, 2},
    {147, 2},
    {149, 2},
    {151, 2},
    {153, 2},
    {155, 2},
    {157, 2},
    {159, 2},
    {161, 2},
    {163, 2},
    {165, 2},
    {167, 2},
    {169, 2},
    {171, 2},
    {173, 2},
    {175, 2},
};

/* ---- Action routines which pass arguments ---- */

static int A_FlushCtFail(void* pECB)
{ return(A_Flush(CtFail,pECB)); }

static int A_FlushCtPermErr(void* pECB)
{ return(A_Flush(CtPermErr,pECB)); }

static int A_PeerAlertPOLL_DATA_ABORT(void* pECB)
{ return(A_PeerAlert(POLL_DATA_ABORT, pECB)); }

int A_RcvFrFreeCtTtdRspCtTtdRsp(void* pECB)
{ return(A_Rcv(FrFree,CtTtdRsp,CtTtdRsp, pECB)); }

int A_RcvFrFreeCtNakCtNak(void* pECB)
{ return(A_Rcv(FrFree,CtNak,CtNak, pECB)); }

int A_RcvFrFreeCtBadAckCtBadAck(void* pECB)
{ return(A_Rcv(FrFree,CtBadAck,CtBadAck, pECB)); }

int A_RcvFrFreeCtAbortCtAbort(void* pECB)
{ return(A_Rcv(FrFree,CtAbort,CtAbort, pECB)); }

int A_RcvFrFreeCtAckCtAck(void* pECB)
{ return(A_Rcv(FrFree,CtAck,CtAck, pECB)); }

int A_RcvFrFreeCtRviCtRvi(void* pECB)
{ return(A_Rcv(FrFree,CtRvi,CtRvi, pECB)); }

int A_RcvFrFreeCtWackCtWack(void* pECB)
{ return(A_Rcv(FrFree,CtWack,CtWack, pECB)); }

int A_RcvFrFreeCtEotCtEot(void* pECB)
{ return(A_Rcv(FrFree,CtEot,CtEot, pECB)); }

int A_RcvFrFreeCtEnqCtEnq(void* pECB)
{ return(A_Rcv(FrFree,CtEnq,CtEnq, pECB)); }

int A_RcvFrNotifyCtAbortCtAbort(void* pECB)
{ return(A_Rcv(FrNotify,CtAbort,CtAbort, pECB)); }

int A_RcvFrFreeCtInvalCtInval(void* pECB)
{ return(A_Rcv(FrFree,CtInval,CtInval, pECB)); }

int A_RcvFrFreeCtTtdCtTtd(void* pECB)
{ return(A_Rcv(FrFree,CtTtd,CtTtd, pECB)); }

static int A_RcvFrPassEtxCtDataCtData(void* pECB)
{ return(A_Rcv(FrPassEtx,CtData,CtData, pECB)); }

static int A_RcvFrPassStsCtDataCtData(void* pECB)
{ return(A_Rcv(FrPassSts,CtData,CtData, pECB)); }

int A_RcvFrFreeCtCrcErrCtCrcErr(void* pECB)
{ return(A_Rcv(FrFree,CtCrcErr,CtCrcErr, pECB)); }

static int A_RcvFrPassEtbCtDataCtData(void* pECB)
{ return(A_Rcv(FrPassEtb,CtData,CtData, pECB)); }

static int A_RcvFrFreeCtNegPolpCtNegPolp(void* pECB)
{ return(A_Rcv(FrFree,CtNegPolp,CtNegPolp, pECB)); }

static int A_RcvFrFreeCtInvalCtNone(void* pECB)
{ return(A_Rcv(FrFree,CtInval,CtNone, pECB)); }

int A_ResetCUcDataRetry(void* pECB)
{ return(A_ResetCU(cDataRetry, pECB)); }

static int A_ResetCUcSelRetry(void* pECB)
{ return(A_ResetCU(cSelRetry, pECB)); }

static int A_ResetCUcPollRetry(void* pECB)
{ return(A_ResetCU(cPollRetry, pECB)); }

static int A_ResetLineTxAck0RxAck0(void* pECB)
{ return(A_ResetLine(TxAck0,RxAck0, pECB)); }

static int A_SendFrControlCtAckCtAck(void* pECB)
{ return(A_Send(FrControl,CtAck,CtAck, pECB)); }

static int A_SendFrEotCtPollToCtPollTo(void* pECB)
{ return(A_Send(FrEot,CtPollTo,CtPollTo, pECB)); }

int A_SendFrTtdCtTtdCtTtd(void* pECB)
{ return(A_Send(FrTtd,CtTtd,CtTtd, pECB)); }

static int A_SendFrDataCtRetranCtRetran(void* pECB)
{ return(A_Send(FrData,CtRetran,CtRetran, pECB)); }

int A_SendFrEnqCtEnqCtEnq(void* pECB)
{ return(A_Send(FrEnq,CtEnq,CtEnq, pECB)); }

int A_SendFrEotCtAbortCtAbort(void* pECB)
{ return(A_Send(FrEot,CtAbort,CtAbort, pECB)); }

static int A_SendFrDataCtDataCtData(void* pECB)
{ return(A_Send(FrData,CtData,CtData, pECB)); }

static int A_SendFrEotCtToCtTo(void* pECB)
{ return(A_Send(FrEot,CtTo,CtTo, pECB)); }

int A_SendFrControlCtAckRsCtAckRs(void *pECB)
{ return(A_Send(FrControl,CtAckRs,CtAckRs,pECB)); }

int A_SendFrEotCtEotCtEot(void* pECB)
{ return(A_Send(FrEot,CtEot,CtEot, pECB)); }

int A_SendFrNakCtTtdRspCtTtdRsp(void* pECB)
{ return(A_Send(FrNak,CtTtdRsp,CtTtdRsp, pECB)); }

int A_SendFrNakCtNakCtNak(void* pECB)
{ return(A_Send(FrNak,CtNak,CtNak, pECB)); }

int A_SendFrAckCtAckCtAck(void* pECB)
{ return(A_Send(FrAck,CtAck,CtAck, pECB)); }

static int A_SendFrEotCtEotCtNone(void* pECB)
{ return(A_Send(FrEot,CtEot,CtNone, pECB)); }

static int A_SendFrControlCtSPollCtSPoll(void* pECB)
{ return(A_Send(FrControl,CtSPoll,CtSPoll, pECB)); }

static int A_SendFrControlCtSelCtSel(void* pECB)
{ return(A_Send(FrControl,CtSel,CtSel, pECB)); }

static int A_SendFrControlCtPollCtPoll(void* pECB)
{ return(A_Send(FrControl,CtPoll,CtPoll, pECB)); }

int A_StartTimerToTtdDelay(void* pECB)
{ return(A_StartTimer(ToTtdDelay, pECB)); }

static int A_StartTimerToLineIdleStart(void* pECB)
{ return(A_StartTimer(ToLineIdleStart,pECB)); }

int A_TxDoneToEnq(void* pECB)
{ return(A_TxDone(ToEnq, pECB)); }

int A_TxDoneToAck(void* pECB)
{ return(A_TxDone(ToAck, pECB)); }

static int A_TxDoneToPoll(void* pECB)
{ return(A_TxDone(ToPoll, pECB)); }

static int A_TxDoneToNone(void* pECB)
{ return(A_TxDone(ToNone, pECB)); }

static int A_TxIncrCtSelToCtSelTo(void* pECB)
{ return(A_TxIncr(CtSelTo,CtSelTo, pECB)); }

int A_TxIncrCtPermErrCtPermErr(void* pECB)
{ return(A_TxIncr(CtPermErr,CtPermErr, pECB)); }

static int A_cStateCU_NONRESPONDING(void* pECB)
{ return(A_cState(CU_NONRESPONDING, pECB)); }

int Sw_CuRetrycDataRetry(void* pECB)
{ return(Sw_CuRetry(cDataRetry, pECB)); }

static int Sw_CuRetrycSelRetry(void* pECB)
{ return(Sw_CuRetry(cSelRetry, pECB)); }

static int Sw_CuRetrycPollRetry(void* pECB)
{ return(Sw_CuRetry(cPollRetry, pECB)); }

int Sw_RetrylTtdRetry(void* pECB)
{ return(Sw_Retry(lTtdRetry, pECB)); }

int Sw_RetrylNakRetry(void* pECB)
{ return(Sw_Retry(lNakRetry, pECB)); }

int Sw_RetrylEnqRetry(void* pECB)
{ return(Sw_Retry(lEnqRetry, pECB)); }

int Sw_RetrylWackRetry(void* pECB)
{ return(Sw_Retry(lWackRetry, pECB)); }

int Sw_RetrylDataRetry(void* pECB)
{ return(Sw_Retry(lDataRetry, pECB)); }


/* ---- pollerfsm Action Vector Jump Table [MaxStates][MaxEvents] ---- */
const static int pollerfsm_TCU_Down[] = {
     8, 1, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
     7, 8, 8
};
const static int pollerfsm_TCU_EOFile[] = {
     179, 180, 164, 181, 181, 174, 134, 141, 150, 181, 181, 181, 181, 169, 181, 
     178, 176, 177
};
const static int pollerfsm_TCU_Idle[] = {
     28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 
     22, 16, 9
};
const static int pollerfsm_TCU_InFile[] = {
     130, 131, 115, 132, 132, 125, 94, 97, 106, 132, 132, 132, 132, 120, 132, 
     129, 127, 128
};
const static int pollerfsm_TCU_Polled[] = {
     90, 91, 65, 92, 92, 92, 32, 40, 50, 92, 92, 92, 92, 71, 92, 
     89, 77, 88
};
const static int pollerfsm_TCU_Selected[] = {
     219, 220, 221, 196, 204, 221, 192, 221, 221, 221, 221, 210, 188, 221, 183, 
     218, 212, 217
};
const static int pollerfsm_TCU_SpecPolled[] = {
     511, 512, 504, 513, 513, 506, 474, 495, 481, 513, 513, 513, 513, 504, 513, 
     510, 508, 509
};
const static int pollerfsm_TCU_TtdDelay[] = {
     430, 431, 432, 432, 432, 420, 427, 432, 432, 432, 432, 432, 432, 432, 432, 
     422, 413, 432
};
const static int pollerfsm_TCU_TtdSent[] = {
     469, 470, 471, 471, 471, 446, 471, 471, 471, 471, 471, 435, 471, 471, 471, 
     468, 457, 467
};
const static int pollerfsm_TCU_TxEOFile[] = {
     333, 334, 303, 309, 312, 335, 264, 278, 288, 335, 335, 315, 321, 325, 270, 
     332, 330, 331
};
const static int pollerfsm_TCU_TxInFile[] = {
     260, 261, 262, 234, 244, 262, 223, 262, 262, 262, 262, 249, 255, 262, 229, 
     259, 257, 258
};
const static int pollerfsm_TCU_TxRetry[] = {
     409, 410, 375, 347, 381, 411, 337, 350, 360, 411, 411, 390, 399, 411, 343, 
     408, 403, 407
};

static const int *pollerfsm_StateEvent[] = {
        pollerfsm_TCU_Down,
        pollerfsm_TCU_EOFile,
        pollerfsm_TCU_Idle,
        pollerfsm_TCU_InFile,
        pollerfsm_TCU_Polled,
        pollerfsm_TCU_Selected,
        pollerfsm_TCU_SpecPolled,
        pollerfsm_TCU_TtdDelay,
        pollerfsm_TCU_TtdSent,
        pollerfsm_TCU_TxEOFile,
        pollerfsm_TCU_TxInFile,
        pollerfsm_TCU_TxRetry
};

/* ---- Action Vector Table ---- */
static const AAEntry    pollerfsm_ActArray[] = {
     /* Null Entry */     {0, 0, 0},    /* fcn, type, jmp or newstate */
     /* Action   1 */     {A_ResetLineTxAck0RxAck0, 5, 521},
     /* Switch   2 */     {Sw_NextWork, 3, 41},
     /* Action   3 */     {A_SendFrControlCtPollCtPoll, 5, 4},
     /* Action   4 */     {A_SendFrControlCtSelCtSel, 5, 5},
     /* Action   5 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action   6 */     {0, 2, 2},
     /* Action   7 */     {A_FlushCtPermErr, 5, 520},
     /* Action   8 */     {0, 2, 520},
     /* Action   9 */     {A_TxDoneToNone, 5, 521},
     /* Action  10 */     {A_ResetLineTxAck0RxAck0, 5, 521},
     /* Switch  11 */     {Sw_NextWork, 3, 40},
     /* Action  12 */     {A_SendFrControlCtPollCtPoll, 5, 4},
     /* Action  13 */     {A_SendFrControlCtSelCtSel, 5, 5},
     /* Action  14 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action  15 */     {0, 2, 520},
     /* Action  16 */     {A_ResetLineTxAck0RxAck0, 5, 521},
     /* Switch  17 */     {Sw_NextWork, 3, 39},
     /* Action  18 */     {A_SendFrControlCtPollCtPoll, 5, 4},
     /* Action  19 */     {A_SendFrControlCtSelCtSel, 5, 5},
     /* Action  20 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action  21 */     {0, 2, 520},
     /* Action  22 */     {A_CancelIOTimer, 1, 521},
     /* Switch  23 */     {Sw_NextWork, 3, 38},
     /* Action  24 */     {A_SendFrControlCtPollCtPoll, 5, 4},
     /* Action  25 */     {A_SendFrControlCtSelCtSel, 5, 5},
     /* Action  26 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action  27 */     {0, 2, 520},
     /* Action  28 */     {0, 2, 0},
     /* Action  29 */     {0, 2, 520},
     /* Action  30 */     {A_RcvFrFreeCtInvalCtNone, 5, 521},
     /* Action  31 */     {A_SendFrEotCtEotCtNone, 5, 520},
     /* Action  32 */     {A_RcvFrFreeCtNegPolpCtNegPolp, 5, 521},
     /* Action  33 */     {A_ActivateCU, 1, 521},
     /* Action  34 */     {A_ResetLineTxAck0RxAck0, 5, 521},
     /* Switch  35 */     {Sw_NextWork, 3, 37},
     /* Action  36 */     {A_SendFrControlCtPollCtPoll, 5, 4},
     /* Action  37 */     {A_SendFrControlCtSelCtSel, 5, 5},
     /* Action  38 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action  39 */     {0, 2, 2},
     /* Action  40 */     {A_ActivateCU, 1, 521},
     /* Switch  41 */     {Sw_CrcValid, 3, 11},
     /* Action  42 */     {A_RcvFrPassEtbCtDataCtData, 5, 521},
     /* Action  43 */     {A_SetTxAck, 1, 521},
     /* Action  44 */     {A_SendFrAckCtAckCtAck, 5, 3},
     /* Action  45 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch  46 */     {Sw_RetrylDataRetry, 6, 70},
     /* Action  47 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action  48 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action  49 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action  50 */     {A_ActivateCU, 1, 521},
     /* Switch  51 */     {Sw_CrcValid, 3, 10},
     /* Switch  52 */     {Sw_RxFrame, 3, 75},
     /* Action  53 */     {A_UpdateSts, 1, 521},
     /* Action  54 */     {A_RcvFrPassStsCtDataCtData, 5, 521},
     /* Break   55 */     {0, 0, 58},
     /* Action  56 */     {A_RcvFrPassEtxCtDataCtData, 5, 521},
     /* Break   57 */     {0, 0, 58},
     /* Action  58 */     {A_SetTxAck, 1, 521},
     /* Action  59 */     {A_SendFrAckCtAckCtAck, 5, 1},
     /* Action  60 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch  61 */     {Sw_RetrylDataRetry, 6, 69},
     /* Action  62 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action  63 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action  64 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action  65 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Action  66 */     {A_ActivateCU, 1, 521},
     /* Switch  67 */     {Sw_RetrylDataRetry, 6, 68},
     /* Action  68 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action  69 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action  70 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action  71 */     {A_RcvFrFreeCtTtdCtTtd, 5, 521},
     /* Action  72 */     {A_ActivateCU, 1, 521},
     /* Switch  73 */     {Sw_RetrylDataRetry, 6, 67},
     /* Action  74 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action  75 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action  76 */     {A_SendFrNakCtTtdRspCtTtdRsp, 5, 520},
     /* Switch  77 */     {Sw_CuRetrycPollRetry, 6, 19},
     /* Action  78 */     {A_cStateCU_NONRESPONDING, 5, 521},
     /* Action  79 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Break   80 */     {0, 0, 82},
     /* Break   81 */     {0, 0, 82},
     /* Action  82 */     {A_ResetLineTxAck0RxAck0, 5, 521},
     /* Switch  83 */     {Sw_NextWork, 3, 36},
     /* Action  84 */     {A_SendFrControlCtPollCtPoll, 5, 4},
     /* Action  85 */     {A_SendFrControlCtSelCtSel, 5, 5},
     /* Action  86 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action  87 */     {0, 2, 2},
     /* Action  88 */     {A_TxDoneToPoll, 5, 520},
     /* Action  89 */     {0, 2, 520},
     /* Action  90 */     {0, 2, 0},
     /* Action  91 */     {0, 2, 520},
     /* Action  92 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action  93 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action  94 */     {A_RcvFrNotifyCtAbortCtAbort, 5, 521},
     /* Action  95 */     {A_PeerAlertPOLL_DATA_ABORT, 5, 521},
     /* Action  96 */     {A_StartTimerToLineIdleStart, 5, 2},
     /* Switch  97 */     {Sw_CrcValid, 3, 9},
     /* Action  98 */     {A_RcvFrPassEtbCtDataCtData, 5, 521},
     /* Action  99 */     {A_SetTxAck, 1, 521},
     /* Action 100 */     {A_SendFrAckCtAckCtAck, 5, 3},
     /* Action 101 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 102 */     {Sw_RetrylDataRetry, 6, 66},
     /* Action 103 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 104 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 105 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Switch 106 */     {Sw_CrcValid, 3, 8},
     /* Action 107 */     {A_RcvFrPassEtxCtDataCtData, 5, 521},
     /* Action 108 */     {A_SetTxAck, 1, 521},
     /* Action 109 */     {A_SendFrAckCtAckCtAck, 5, 1},
     /* Action 110 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 111 */     {Sw_RetrylDataRetry, 6, 65},
     /* Action 112 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 113 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 114 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action 115 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Switch 116 */     {Sw_RetrylDataRetry, 6, 64},
     /* Action 117 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 118 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 119 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action 120 */     {A_RcvFrFreeCtTtdCtTtd, 5, 521},
     /* Switch 121 */     {Sw_RetrylDataRetry, 6, 63},
     /* Action 122 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 123 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 124 */     {A_SendFrNakCtTtdRspCtTtdRsp, 5, 520},
     /* Action 125 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Action 126 */     {A_SendFrControlCtAckRsCtAckRs, 5, 520},
     /* Action 127 */     {A_SendFrEotCtToCtTo, 5, 2},
     /* Action 128 */     {A_TxDoneToAck, 5, 520},
     /* Action 129 */     {0, 2, 520},
     /* Action 130 */     {0, 2, 0},
     /* Action 131 */     {0, 2, 520},
     /* Action 132 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 133 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 134 */     {A_RcvFrFreeCtEotCtEot, 5, 521},
     /* Action 135 */     {A_ResetLineTxAck0RxAck0, 5, 521},
     /* Switch 136 */     {Sw_NextWork, 3, 35},
     /* Action 137 */     {A_SendFrControlCtPollCtPoll, 5, 4},
     /* Action 138 */     {A_SendFrControlCtSelCtSel, 5, 5},
     /* Action 139 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action 140 */     {0, 2, 2},
     /* Switch 141 */     {Sw_CrcValid, 3, 7},
     /* Action 142 */     {A_RcvFrPassEtbCtDataCtData, 5, 521},
     /* Action 143 */     {A_SetTxAck, 1, 521},
     /* Action 144 */     {A_SendFrAckCtAckCtAck, 5, 3},
     /* Action 145 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 146 */     {Sw_RetrylDataRetry, 6, 62},
     /* Action 147 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 148 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 149 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Switch 150 */     {Sw_CrcValid, 3, 6},
     /* Switch 151 */     {Sw_RxFrame, 3, 74},
     /* Action 152 */     {A_UpdateSts, 1, 521},
     /* Action 153 */     {A_RcvFrPassStsCtDataCtData, 5, 521},
     /* Break  154 */     {0, 0, 157},
     /* Action 155 */     {A_RcvFrPassEtxCtDataCtData, 5, 521},
     /* Break  156 */     {0, 0, 157},
     /* Action 157 */     {A_SetTxAck, 1, 521},
     /* Action 158 */     {A_SendFrAckCtAckCtAck, 5, 1},
     /* Action 159 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 160 */     {Sw_RetrylDataRetry, 6, 61},
     /* Action 161 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 162 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 163 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action 164 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Switch 165 */     {Sw_RetrylDataRetry, 6, 60},
     /* Action 166 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 167 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 168 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action 169 */     {A_RcvFrFreeCtTtdCtTtd, 5, 521},
     /* Switch 170 */     {Sw_RetrylDataRetry, 6, 59},
     /* Action 171 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 172 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 173 */     {A_SendFrNakCtTtdRspCtTtdRsp, 5, 520},
     /* Action 174 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Action 175 */     {A_SendFrControlCtAckRsCtAckRs, 5, 520},
     /* Action 176 */     {A_SendFrEotCtToCtTo, 5, 2},
     /* Action 177 */     {A_TxDoneToAck, 5, 520},
     /* Action 178 */     {0, 2, 520},
     /* Action 179 */     {0, 2, 0},
     /* Action 180 */     {0, 2, 520},
     /* Action 181 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 182 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 183 */     {A_RcvDoWack, 1, 521},
     /* Action 184 */     {A_RcvFrFreeCtWackCtWack, 5, 521},
     /* Action 185 */     {A_ResetCUcSelRetry, 5, 521},
     /* Action 186 */     {A_BuildSpecPoll, 1, 521},
     /* Action 187 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action 188 */     {A_RcvFrFreeCtRviCtRvi, 5, 521},
     /* Action 189 */     {A_ResetCUcSelRetry, 5, 521},
     /* Action 190 */     {A_BuildSpecPoll, 1, 521},
     /* Action 191 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action 192 */     {A_RcvFrFreeCtEotCtEot, 5, 521},
     /* Action 193 */     {A_ResetCUcSelRetry, 5, 521},
     /* Action 194 */     {A_BuildSpecPoll, 1, 521},
     /* Action 195 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action 196 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 197 */     {A_ResetCUcSelRetry, 5, 521},
     /* Action 198 */     {A_SetRxAck, 1, 521},
     /* Switch 199 */     {Sw_Frame, 3, 28},
     /* Action 200 */     {A_SendFrDataCtDataCtData, 5, 10},
     /* Action 201 */     {A_SendFrDataCtDataCtData, 5, 9},
     /* Action 202 */     {A_FreeData, 1, 521},
     /* Action 203 */     {A_SendFrEotCtAbortCtAbort, 5, 2},
     /* Action 204 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Switch 205 */     {Sw_CuRetrycSelRetry, 6, 18},
     /* Action 206 */     {A_FlushCtPermErr, 5, 521},
     /* Break  207 */     {0, 0, 209},
     /* Break  208 */     {0, 0, 209},
     /* Action 209 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 210 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 211 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 212 */     {A_TxIncrCtSelToCtSelTo, 5, 521},
     /* Switch 213 */     {Sw_CuRetrycSelRetry, 6, 17},
     /* Action 214 */     {A_FlushCtPermErr, 5, 521},
     /* Action 215 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 216 */     {A_SendFrControlCtSelCtSel, 5, 520},
     /* Action 217 */     {A_TxDoneToPoll, 5, 520},
     /* Action 218 */     {0, 2, 520},
     /* Action 219 */     {0, 2, 0},
     /* Action 220 */     {0, 2, 520},
     /* Action 221 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 222 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 223 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Switch 224 */     {Sw_CuRetrycDataRetry, 6, 16},
     /* Action 225 */     {A_FlushCtPermErr, 5, 521},
     /* Break  226 */     {0, 0, 228},
     /* Break  227 */     {0, 0, 228},
     /* Action 228 */     {A_StartTimerToLineIdleStart, 5, 2},
     /* Action 229 */     {A_RcvFrFreeCtWackCtWack, 5, 521},
     /* Switch 230 */     {Sw_RetrylWackRetry, 6, 58},
     /* Action 231 */     {A_FlushCtFail, 5, 521},
     /* Action 232 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 233 */     {A_SendFrEnqCtEnqCtEnq, 5, 520},
     /* Action 234 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 235 */     {A_TxAccepted, 1, 521},
     /* Action 236 */     {A_SetRxAck, 1, 521},
     /* Switch 237 */     {Sw_MoreData, 3, 33},
     /* Switch 238 */     {Sw_Frame, 3, 27},
     /* Action 239 */     {A_SendFrDataCtDataCtData, 5, 520},
     /* Action 240 */     {A_SendFrDataCtDataCtData, 5, 9},
     /* Action 241 */     {A_FreeData, 1, 521},
     /* Action 242 */     {A_SendFrEotCtAbortCtAbort, 5, 2},
     /* Action 243 */     {A_StartTimerToTtdDelay, 5, 7},
     /* Action 244 */     {A_RcvFrFreeCtBadAckCtBadAck, 5, 521},
     /* Switch 245 */     {Sw_RetrylEnqRetry, 6, 57},
     /* Action 246 */     {A_FlushCtPermErr, 5, 521},
     /* Action 247 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 248 */     {A_SendFrEnqCtEnqCtEnq, 5, 520},
     /* Action 249 */     {A_RcvFrFreeCtNakCtNak, 5, 521},
     /* Switch 250 */     {Sw_CuRetrycDataRetry, 6, 15},
     /* Action 251 */     {A_FlushCtPermErr, 5, 521},
     /* Break  252 */     {0, 0, 254},
     /* Break  253 */     {0, 0, 254},
     /* Action 254 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 255 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 256 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 257 */     {A_SendFrEnqCtEnqCtEnq, 5, 11},
     /* Action 258 */     {A_TxDoneToEnq, 5, 520},
     /* Action 259 */     {0, 2, 520},
     /* Action 260 */     {0, 2, 0},
     /* Action 261 */     {0, 2, 520},
     /* Action 262 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 263 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 264 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Switch 265 */     {Sw_CuRetrycDataRetry, 6, 14},
     /* Action 266 */     {A_FlushCtPermErr, 5, 521},
     /* Action 267 */     {A_StartTimerToLineIdleStart, 5, 2},
     /* Action 268 */     {A_BuildSpecPoll, 1, 521},
     /* Action 269 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action 270 */     {A_RcvFrFreeCtWackCtWack, 5, 521},
     /* Action 271 */     {A_ResetCUcSelRetry, 5, 521},
     /* Action 272 */     {A_ResetCUcDataRetry, 5, 521},
     /* Action 273 */     {A_ResetCUcPollRetry, 5, 521},
     /* Action 274 */     {A_FreeData, 1, 521},
     /* Action 275 */     {A_RcvDoWack, 1, 521},
     /* Action 276 */     {A_BuildSpecPoll, 1, 521},
     /* Action 277 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action 278 */     {A_TxAccepted, 1, 521},
     /* Switch 279 */     {Sw_CrcValid, 3, 5},
     /* Action 280 */     {A_RcvFrPassEtbCtDataCtData, 5, 521},
     /* Action 281 */     {A_SetTxAck, 1, 521},
     /* Action 282 */     {A_SendFrAckCtAckCtAck, 5, 3},
     /* Action 283 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 284 */     {Sw_RetrylDataRetry, 6, 56},
     /* Action 285 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 286 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 287 */     {A_SendFrNakCtNakCtNak, 5, 1},
     /* Action 288 */     {A_TxAccepted, 1, 521},
     /* Switch 289 */     {Sw_CrcValid, 3, 4},
     /* Switch 290 */     {Sw_RxFrame, 3, 73},
     /* Action 291 */     {A_UpdateSts, 1, 521},
     /* Action 292 */     {A_RcvFrPassStsCtDataCtData, 5, 521},
     /* Break  293 */     {0, 0, 296},
     /* Action 294 */     {A_RcvFrPassEtxCtDataCtData, 5, 521},
     /* Break  295 */     {0, 0, 296},
     /* Action 296 */     {A_SetTxAck, 1, 521},
     /* Action 297 */     {A_SendFrAckCtAckCtAck, 5, 1},
     /* Action 298 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 299 */     {Sw_RetrylDataRetry, 6, 55},
     /* Action 300 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 301 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 302 */     {A_SendFrNakCtNakCtNak, 5, 1},
     /* Action 303 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Action 304 */     {A_TxAccepted, 1, 521},
     /* Switch 305 */     {Sw_RetrylDataRetry, 6, 54},
     /* Action 306 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 307 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 308 */     {A_SendFrNakCtNakCtNak, 5, 1},
     /* Action 309 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 310 */     {A_TxAccepted, 1, 521},
     /* Action 311 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 312 */     {A_RcvFrFreeCtBadAckCtBadAck, 5, 521},
     /* Action 313 */     {A_FlushCtPermErr, 5, 521},
     /* Action 314 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 315 */     {A_RcvFrFreeCtNakCtNak, 5, 521},
     /* Switch 316 */     {Sw_CuRetrycDataRetry, 6, 13},
     /* Action 317 */     {A_FlushCtPermErr, 5, 521},
     /* Break  318 */     {0, 0, 320},
     /* Break  319 */     {0, 0, 320},
     /* Action 320 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 321 */     {A_RcvFrFreeCtRviCtRvi, 5, 521},
     /* Action 322 */     {A_TxAccepted, 1, 521},
     /* Action 323 */     {A_BuildSpecPoll, 1, 521},
     /* Action 324 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action 325 */     {A_RcvFrFreeCtTtdCtTtd, 5, 521},
     /* Switch 326 */     {Sw_RetrylDataRetry, 6, 53},
     /* Action 327 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 328 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 329 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action 330 */     {A_SendFrEnqCtEnqCtEnq, 5, 11},
     /* Action 331 */     {A_TxDoneToEnq, 5, 520},
     /* Action 332 */     {0, 2, 520},
     /* Action 333 */     {0, 2, 0},
     /* Action 334 */     {0, 2, 520},
     /* Action 335 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 336 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 337 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Switch 338 */     {Sw_CuRetrycDataRetry, 6, 12},
     /* Action 339 */     {A_FlushCtPermErr, 5, 521},
     /* Break  340 */     {0, 0, 342},
     /* Break  341 */     {0, 0, 342},
     /* Action 342 */     {A_StartTimerToLineIdleStart, 5, 2},
     /* Action 343 */     {A_RcvFrFreeCtWackCtWack, 5, 521},
     /* Action 344 */     {A_TxAccepted, 1, 521},
     /* Action 345 */     {A_BuildSpecPoll, 1, 521},
     /* Action 346 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action 347 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 348 */     {A_TxAccepted, 1, 521},
     /* Action 349 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 350 */     {A_TxAccepted, 1, 521},
     /* Switch 351 */     {Sw_CrcValid, 3, 3},
     /* Action 352 */     {A_RcvFrPassEtbCtDataCtData, 5, 521},
     /* Action 353 */     {A_SetTxAck, 1, 521},
     /* Action 354 */     {A_SendFrAckCtAckCtAck, 5, 3},
     /* Action 355 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 356 */     {Sw_RetrylDataRetry, 6, 52},
     /* Action 357 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 358 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 359 */     {A_SendFrNakCtNakCtNak, 5, 1},
     /* Action 360 */     {A_TxAccepted, 1, 521},
     /* Switch 361 */     {Sw_CrcValid, 3, 2},
     /* Switch 362 */     {Sw_RxFrame, 3, 72},
     /* Action 363 */     {A_UpdateSts, 1, 521},
     /* Action 364 */     {A_RcvFrPassStsCtDataCtData, 5, 521},
     /* Break  365 */     {0, 0, 368},
     /* Action 366 */     {A_RcvFrPassEtxCtDataCtData, 5, 521},
     /* Break  367 */     {0, 0, 368},
     /* Action 368 */     {A_SetTxAck, 1, 521},
     /* Action 369 */     {A_SendFrAckCtAckCtAck, 5, 1},
     /* Action 370 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 371 */     {Sw_RetrylDataRetry, 6, 51},
     /* Action 372 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 373 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 374 */     {A_SendFrNakCtNakCtNak, 5, 1},
     /* Action 375 */     {A_RcvFrFreeCtAckCtAck, 5, 521},
     /* Action 376 */     {A_TxAccepted, 1, 521},
     /* Switch 377 */     {Sw_RetrylDataRetry, 6, 50},
     /* Action 378 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 379 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 380 */     {A_SendFrNakCtNakCtNak, 5, 1},
     /* Action 381 */     {A_RcvFrFreeCtBadAckCtBadAck, 5, 521},
     /* Switch 382 */     {Sw_RetrylEnqRetry, 6, 49},
     /* Action 383 */     {A_FlushCtPermErr, 5, 521},
     /* Action 384 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Switch 385 */     {Sw_Frame, 3, 26},
     /* Action 386 */     {A_SendFrDataCtRetranCtRetran, 5, 10},
     /* Action 387 */     {A_SendFrDataCtRetranCtRetran, 5, 9},
     /* Action 388 */     {A_FreeData, 1, 521},
     /* Action 389 */     {A_SendFrEotCtAbortCtAbort, 5, 2},
     /* Action 390 */     {A_RcvFrFreeCtNakCtNak, 5, 521},
     /* Switch 391 */     {Sw_RetrylNakRetry, 6, 48},
     /* Action 392 */     {A_FlushCtPermErr, 5, 521},
     /* Action 393 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Switch 394 */     {Sw_Frame, 3, 25},
     /* Action 395 */     {A_SendFrDataCtRetranCtRetran, 5, 10},
     /* Action 396 */     {A_SendFrDataCtRetranCtRetran, 5, 9},
     /* Action 397 */     {A_FreeData, 1, 521},
     /* Action 398 */     {A_SendFrEotCtAbortCtAbort, 5, 2},
     /* Action 399 */     {A_RcvFrFreeCtRviCtRvi, 5, 521},
     /* Action 400 */     {A_TxAccepted, 1, 521},
     /* Action 401 */     {A_BuildSpecPoll, 1, 521},
     /* Action 402 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Switch 403 */     {Sw_RetrylEnqRetry, 6, 47},
     /* Action 404 */     {A_FlushCtPermErr, 5, 521},
     /* Action 405 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 406 */     {A_SendFrEnqCtEnqCtEnq, 5, 520},
     /* Action 407 */     {A_TxDoneToEnq, 5, 520},
     /* Action 408 */     {0, 2, 520},
     /* Action 409 */     {0, 2, 0},
     /* Action 410 */     {0, 2, 520},
     /* Action 411 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 412 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Switch 413 */     {Sw_MoreData, 3, 32},
     /* Switch 414 */     {Sw_Frame, 3, 24},
     /* Action 415 */     {A_SendFrDataCtDataCtData, 5, 10},
     /* Action 416 */     {A_SendFrDataCtDataCtData, 5, 9},
     /* Action 417 */     {A_FreeData, 1, 521},
     /* Action 418 */     {A_SendFrEotCtAbortCtAbort, 5, 2},
     /* Action 419 */     {A_SendFrTtdCtTtdCtTtd, 5, 8},
     /* Action 420 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Action 421 */     {A_SendFrTtdCtTtdCtTtd, 5, 8},
     /* Switch 422 */     {Sw_Frame, 3, 23},
     /* Action 423 */     {A_SendFrDataCtDataCtData, 5, 10},
     /* Action 424 */     {A_SendFrDataCtDataCtData, 5, 9},
     /* Action 425 */     {A_FreeData, 1, 521},
     /* Action 426 */     {A_SendFrEotCtAbortCtAbort, 5, 2},
     /* Action 427 */     {A_RcvFrFreeCtAbortCtAbort, 5, 521},
     /* Action 428 */     {A_FlushCtPermErr, 5, 521},
     /* Action 429 */     {A_StartTimerToLineIdleStart, 5, 2},
     /* Action 430 */     {0, 2, 0},
     /* Action 431 */     {0, 2, 520},
     /* Action 432 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 433 */     {A_FlushCtPermErr, 5, 521},
     /* Action 434 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 435 */     {A_RcvFrFreeCtTtdRspCtTtdRsp, 5, 521},
     /* Switch 436 */     {Sw_MoreData, 3, 31},
     /* Switch 437 */     {Sw_Frame, 3, 22},
     /* Action 438 */     {A_SendFrDataCtDataCtData, 5, 10},
     /* Action 439 */     {A_SendFrDataCtDataCtData, 5, 9},
     /* Action 440 */     {A_FreeData, 1, 521},
     /* Action 441 */     {A_SendFrEotCtAbortCtAbort, 5, 2},
     /* Switch 442 */     {Sw_RetrylTtdRetry, 6, 46},
     /* Action 443 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 444 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 445 */     {A_StartTimerToTtdDelay, 5, 7},
     /* Action 446 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Switch 447 */     {Sw_MoreData, 3, 30},
     /* Switch 448 */     {Sw_Frame, 3, 21},
     /* Action 449 */     {A_SendFrDataCtDataCtData, 5, 10},
     /* Action 450 */     {A_SendFrDataCtDataCtData, 5, 9},
     /* Action 451 */     {A_FreeData, 1, 521},
     /* Action 452 */     {A_SendFrEotCtAbortCtAbort, 5, 2},
     /* Switch 453 */     {Sw_RetrylTtdRetry, 6, 45},
     /* Action 454 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 455 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 456 */     {A_StartTimerToTtdDelay, 5, 7},
     /* Switch 457 */     {Sw_MoreData, 3, 29},
     /* Switch 458 */     {Sw_Frame, 3, 20},
     /* Action 459 */     {A_SendFrDataCtDataCtData, 5, 10},
     /* Action 460 */     {A_SendFrDataCtDataCtData, 5, 9},
     /* Action 461 */     {A_FreeData, 1, 521},
     /* Action 462 */     {A_SendFrEotCtAbortCtAbort, 5, 2},
     /* Switch 463 */     {Sw_RetrylTtdRetry, 6, 44},
     /* Action 464 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 465 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 466 */     {A_StartTimerToTtdDelay, 5, 7},
     /* Action 467 */     {A_TxDoneToEnq, 5, 520},
     /* Action 468 */     {0, 2, 520},
     /* Action 469 */     {0, 2, 0},
     /* Action 470 */     {0, 2, 520},
     /* Action 471 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 472 */     {A_FlushCtPermErr, 5, 521},
     /* Action 473 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 474 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 475 */     {A_ResetLineTxAck0RxAck0, 5, 521},
     /* Switch 476 */     {Sw_NextWork, 3, 34},
     /* Action 477 */     {A_SendFrControlCtPollCtPoll, 5, 4},
     /* Action 478 */     {A_SendFrControlCtSelCtSel, 5, 5},
     /* Action 479 */     {A_SendFrControlCtSPollCtSPoll, 5, 6},
     /* Action 480 */     {0, 2, 2},
     /* Switch 481 */     {Sw_CrcValid, 3, 1},
     /* Switch 482 */     {Sw_RxFrame, 3, 71},
     /* Action 483 */     {A_UpdateSts, 1, 521},
     /* Action 484 */     {A_RcvFrPassStsCtDataCtData, 5, 521},
     /* Break  485 */     {0, 0, 488},
     /* Action 486 */     {A_RcvFrPassEtxCtDataCtData, 5, 521},
     /* Break  487 */     {0, 0, 488},
     /* Action 488 */     {A_SetTxAck, 1, 521},
     /* Action 489 */     {A_SendFrAckCtAckCtAck, 5, 1},
     /* Action 490 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 491 */     {Sw_RetrylDataRetry, 6, 43},
     /* Action 492 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 493 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 494 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Switch 495 */     {Sw_CrcValid, 3, 0},
     /* Action 496 */     {A_RcvFrPassEtbCtDataCtData, 5, 521},
     /* Action 497 */     {A_SetTxAck, 1, 521},
     /* Action 498 */     {A_SendFrAckCtAckCtAck, 5, 3},
     /* Action 499 */     {A_RcvFrFreeCtCrcErrCtCrcErr, 5, 521},
     /* Switch 500 */     {Sw_RetrylDataRetry, 6, 42},
     /* Action 501 */     {A_TxIncrCtPermErrCtPermErr, 5, 521},
     /* Action 502 */     {A_SendFrEotCtEotCtEot, 5, 2},
     /* Action 503 */     {A_SendFrNakCtNakCtNak, 5, 520},
     /* Action 504 */     {A_RcvFrFreeCtTtdCtTtd, 5, 521},
     /* Action 505 */     {A_SendFrNakCtTtdRspCtTtdRsp, 5, 520},
     /* Action 506 */     {A_RcvFrFreeCtEnqCtEnq, 5, 521},
     /* Action 507 */     {A_SendFrControlCtAckCtAck, 5, 520},
     /* Action 508 */     {A_SendFrEotCtPollToCtPollTo, 5, 2},
     /* Action 509 */     {A_TxDoneToPoll, 5, 520},
     /* Action 510 */     {0, 2, 520},
     /* Action 511 */     {0, 2, 0},
     /* Action 512 */     {0, 2, 520},
     /* Action 513 */     {A_RcvFrFreeCtInvalCtInval, 5, 521},
     /* Action 514 */     {A_SendFrEotCtEotCtEot, 5, 2},
};

/* ---- pollerfsm state machine definitions ---- */
const SM pollerfsm = {
    pollerfsm_MaxStates,    /* max states */
    pollerfsm_MaxEvents,    /* max events */
    pollerfsm_ActArraySize, /* # of entries in the action array */
    pollerfsm_SwLblVect,    /* Switch return value ActArray indicies */
    pollerfsm_SwTbl,        /* SwLblVect index & # of rtn values */
    pollerfsm_StateEvent,   /* state/event to action vector tbl */
    pollerfsm_ActArray      /* action vector table */
};
