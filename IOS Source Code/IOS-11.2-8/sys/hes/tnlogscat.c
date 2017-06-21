/* $Id: tnlogscat.c,v 3.1.4.2 1996/06/28 23:06:28 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/tnlogscat.c,v $
 *------------------------------------------------------------------
 * tn3270 server log
 *
 * Jan 96, Michael Boe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tnlogscat.c,v $
 * Revision 3.1.4.2  1996/06/28  23:06:28  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  11:19:33  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/21  03:21:50  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/21  01:13:19  bchan
 * placeholder for tn3270 server.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** ---NOTICE---: this file automatically generated from CIP repository.
** generated from tnlogdef.input on Thu Oct  3 23:21:31 US/Eastern 1996
**
*/
#include "tnlogscat.h"
#ifdef CIP
#include "util.h"
#include "tnutil.h"
#include "free.h"
#else
#include "master.h"
#include "string.h"
#endif
#ifndef min
#define min(a,b) (((a)<(b))? (a) : (b))
#endif
typedef struct _tnuint1_ { unsigned char num; } _tnuint1;
typedef struct _tnuint2_ { unsigned short PACKED(num); } _tnuint2;
typedef struct _tnbiguint1_ { unsigned char num; } _tnbiguint1;
typedef struct _tnbiguint2_ { unsigned short PACKED(num); } _tnbiguint2;
typedef struct _tnuint4_ { unsigned long PACKED(num); } _tnuint4;
typedef struct _tnbiguint4_ { unsigned long PACKED(num); } _tnbiguint4;
typedef struct _tnptr_ { unsigned short PACKED(pktLength); unsigned short PACKED(len); unsigned char ptr[1]; } _tnptr;
typedef struct _tnint1_ { char num; } _tnint1;
typedef struct _tnint2_ { short PACKED(num); } _tnint2;
typedef struct _tnint4_ { long PACKED(num); } _tnint4;

static void tn_scatter_adapter (
     unsigned char* wptr,
     void* rptr)
{
  memcpy(wptr, rptr, 10);
}

static void tn_scatter_cpNameLcl (
     unsigned char* wptr,
     void* rptr)
{
  memcpy(wptr, rptr, 19);
}

static void tn_scatter_cpNameRem (
     unsigned char* wptr,
     void* rptr)
{
  memcpy(wptr, rptr, 19);
}

static void tn_scatter_idBlkNum (
     unsigned long* wptr,
     void* rptr)
{
  _tnbiguint4* _t = rptr; *wptr = _t->num;
}

static void tn_scatter_ipAddrLcl (
     unsigned long* wptr,
     void* rptr)
{
  *wptr = *(unsigned long*)rptr;
}

static void tn_scatter_snaSense (
     unsigned long* wptr,
     void* rptr)
{
  _tnuint4* _t = rptr; *wptr = _t->num;
}

static void tn_scatter_systemStateTransition (
     unsigned char* wptr,
     void* rptr)
{
  *wptr = *(unsigned char*)rptr;
}

static void tn_scatter_tcpPortLcl (
     unsigned short* wptr,
     void* rptr)
{
  _tnbiguint2* _t = rptr; *wptr = _t->num;
}

static void tn_scatter_tgNumber (
     unsigned char* wptr,
     void* rptr)
{
  memcpy(wptr, rptr, 5);
}

/*
** STRUCT DEFINITIONS, LOCAL VARS, ETC
*/
#define tn_max_entrynum 256
struct copy_fld_def {
    short PACKED(defIdnum);
    short PACKED(defFldLength);
};

typedef struct TnScatBind {
  short    fldNum;
  short    fldOffset;
} TnScatBind;

typedef struct TnScatBindEntry {
 short                entrySize;
 TnScatBind         * scatbindArr;
} TnScatBindEntry;

static TnScatBindEntry tn_scatbind_arr[tn_max_entrynum+1] = {{0,0}};

typedef struct TnFixedFields {
  long          PACKED(timeticks);
  unsigned long PACKED(subticks);
  short         PACKED(entryNum);
} TnFixedFields;

enum TnLogFldNames {
  TLFID_entryName          =   1 /* entry idx                           */,
  TLFID_timeTicks          =   2 /* time ticks                          */,
  TLFID_subTicks           =   3 /* time subticks                       */,
  TLFID_puNameLclAdmin     =  10 /* local pu name                       */,
  TLFID_luNameSna          =  11 /* name of LU (local or SNA)           */,
  TLFID_tn3270eDeviceType  =  12 /* tn3270e device-type                 */,
  TLFID_tn3270eDeviceName  =  13 /* tn3270e device-name                 */,
  TLFID_tn3270eSubErr      =  14 /* connection-error explanation        */,
  TLFID_ipAddrLcl          =  15 /* local ip address                    */,
  TLFID_ipAddrClient       =  16 /* remote ip address                   */,
  TLFID_tcpPortLcl         =  17 /* local tcp port                      */,
  TLFID_tcpPortClient      =  18 /* remote tcp port                     */,
  TLFID_msgLength          =  19 /* length of traced message            */,
  TLFID_msgFragment        =  20 /* message fragment                    */,
  TLFID_puIndex            =  21 /* index of pu entity                  */,
  TLFID_luLocaddr          =  22 /* locaddr identifying LU              */,
  TLFID_logRingNbr         =  23 /* log-ring number                     */,
  TLFID_logEntrySize       =  24 /* log-ring entry size                 */,
  TLFID_logNumEntries      =  25 /* log-ring number of entries          */,
  TLFID_lucOldState        =  26 /* lu fsm old state                    */,
  TLFID_lucEvent           =  27 /* lu fsm event                        */,
  TLFID_lucNewState        =  28 /* lu fsm new state                    */,
  TLFID_connectReasonCode  =  29 /* RFC1647 reject code                 */,
  TLFID_ruDump             =  30 /* dump of RU                          */,
  TLFID_connectSpecificP   =  31 /* specific/dynamic                    */,
  TLFID_ipPoolNumActivePus =  32 /* nbr of active pus now               */,
  TLFID_systemStateTransition =  33 /* true iff up                         */,
  TLFID_tnetIndex          =  34 /* telnet cb index                     */,
  TLFID_puSnmpState        =  35 /* sna nau mib pu state                */,
  TLFID_listenPointStateExtern =  36 /* telnet listen-point state           */,
  TLFID_luType             =  37 /* type 1,2,3                          */,
  TLFID_snaMaxRuIn         =  38 /* max inbound ru sz                   */,
  TLFID_snaMaxRuOut        =  39 /* max outbound ru sz                  */,
  TLFID_snaPaceIn          =  40 /* inbound paced window                */,
  TLFID_snaPaceOut         =  41 /* inbound paced window                */,
  TLFID_snaChainIn         =  42 /* <chn> <no-chn>                      */,
  TLFID_snaRspMode         =  43 /* <def-rsp> <ex-rsp>                  */,
  TLFID_puOldState         =  44 /* pu old state                        */,
  TLFID_puNewState         =  45 /* pu new state                        */,
  TLFID_puEvent            =  46 /* pu event                            */,
  TLFID_snaSense           =  47 /* SNA sense code                      */,
  TLFID_cpNameLcl          =  48 /* local CP name                       */,
  TLFID_cpNameRem          =  49 /* Partner CP name                     */,
  TLFID_adapter            =  50 /* Adapter name                        */,
  TLFID_tgNumber           =  51 /* TG number                           */,
  TLFID_idBlkNum           =  52 /* ID BLK/NUM                          */,
  TLFID_luSnaState         =  53 /* lu sna state                        */,
  TLFID_luStateFlags       =  54 /* lu state flags                      */,
  TLFID_linkName           =  55 /* link name                           */,
  TLFID_oldState           =  56 /* from state                          */,
  TLFID_event              =  57 /* link event                          */,
  TLFID_linkState          =  58 /* new state of link                   */,
  TLFID_maxData            =  59 /* Accept                              */,
  TLFID_sense              =  60 /* sense                               */,
  TLFID_offset             =  61 /* offset                              */,
  TLFID_count              =  62 /* count                               */,
  TLFID_lSap               =  63 /* lSap                                */,
  TLFID_adapType           =  64 /* adaptor type                        */,
  TLFID_adapNo             =  65 /* adaptor number                      */,
  TLFID_length             =  66 /* length                              */,
  TLFID_unbindType         =  67 /* unbind type                         */,
  TLFID_luName             =  68 /* lu name                             */,
  TLFID_netName            =  69 /* network name                        */,
  TLFID_modeName           =  70 /* mode name                           */,
  TLFID_peerName           =  71 /* peerNameNet                         */,
  TLFID_tgNum              =  72 /* tgNum                               */,
  TLFID_luDiscReason       =  73 /* lu disconnect reason                */
};

enum TnLogEntryNames {
  TLEID_LogGrowth          =   1 /* log ring growth                     */,
  TLEID_SystemUp           =   2 /* system up                           */,
  TLEID_PuFsm              =   3 /* pu fsm trans                        */,
  TLEID_PuExternalState    =   4 /* pu external state trans             */,
  TLEID_PuSnaBadMsg        =   5 /* bad sna msg frag                    */,
  TLEID_LuPacingAddCredits =   6 /* add pacing credits                  */,
  TLEID_LuPacingFlush      =   7 /* flush pacing window                 */,
  TLEID_LuSendPsid         =   8 /* send PSID                           */,
  TLEID_LuFsm              =   9 /* lu fsm transition                   */,
  TLEID_BindParm           =  10 /* lu bind                             */,
  TLEID_LuTnetRcvInvalidData =  11 /* invalid tnet data                   */,
  TLEID_BadBind            =  12 /* bad bind                            */,
  TLEID_BadTnetConnect     =  13 /* bad tnet connect                    */,
  TLEID_NoLuSessions       =  14 /* no free locaddrs in generic pool    */,
  TLEID_Connected          =  15 /* tnet session connected              */,
  TLEID_LuEnterSpec        =  16 /* LU entered as specific              */,
  TLEID_LuBadRequest       =  17 /* bad request data or type            */,
  TLEID_LuBadResponse      =  18 /* bad response from host              */,
  TLEID_PacketLuIn         =  19 /* lu pkt trace (inbound)              */,
  TLEID_PacketLuOut        =  20 /* lu pkt trace (outbound)             */,
  TLEID_PacketPuIn         =  21 /* pu pkt trace (inbound)              */,
  TLEID_PacketPuOut        =  22 /* pu pkt trace (outbound)             */,
  TLEID_PacketTnetIn       =  23 /* tnet pkt trace (inbound)            */,
  TLEID_PacketTnetOut      =  24 /* tnet pkt trace (outbound)           */,
  TLEID_TnetState          =  25 /* tnet ext state transition           */,
  TLEID_NoLuMemory         =  26 /* no memory for lu alloc              */,
  TLEID_SystemDown         =  27 /* system down                         */,
  TLEID_APPNProtErr        =  28 /* DLUS protocol error                 */,
  TLEID_DLUSIsnt           =  29 /* Partner not a DLUS                  */,
  TLEID_DLUSXSubNet        =  30 /* DLUS across subnets                 */,
  TLEID_APPNBindFail       =  31 /* Rejecting rcvd Bind                 */,
  TLEID_XIDRej             =  32 /* XID rejected                        */,
  TLEID_BadPIU             =  33 /* Rcvd bad frame                      */,
  TLEID_NoEventBufs        =  34 /* no IPC event bufs                   */,
  TLEID_XIDRejID           =  35 /* XID with IDBLK/NUM rejected         */,
  TLEID_NoPsidRspRcvd      =  36 /* no nmvt psid response from host     */,
  TLEID_NoNtfyAvRspRcvd    =  37 /* no notify_av response from host     */,
  TLEID_NoBindReqRcvd      =  38 /* no bind request from host           */,
  TLEID_NoSdtReqRcvd       =  39 /* no sdt request from host            */,
  TLEID_NoSdtTmarkRcvd     =  40 /* no sdt timing mark response from client */,
  TLEID_NoUnbindTmarkRcvd  =  41 /* no unbind timing mark response from client */,
  TLEID_NoNtfyUaRcvd       =  42 /* no notify_ua response from host     */,
  TLEID_NoDynActluReqRcvd  =  43 /* no dynamic-lu actlu request from host */,
  TLEID_NoUnbindRspRcvd    =  44 /* no unbind response from host        */,
  TLEID_LuStateFlags       =  45 /* lu state flag values at receipt time */,
  TLEID_LinkStateChange    =  46 /* change of link state                */,
  TLEID_LinkAccept         =  47 /* link active                         */,
  TLEID_LinkRspDisc        =  48 /* link disconnected by remote         */,
  TLEID_LinkXIDErrCV       =  49 /* Link received XID CV22              */,
  TLEID_LinkRetries        =  50 /* Link consecutive activation failure count */,
  TLEID_DroppedFrame       =  51 /* No buffers - dropped frame to Host  */,
  TLEID_SapRetries         =  52 /* SAP consecutive activation failure count */,
  TLEID_AdaptRetries       =  53 /* adapter consecutive activation failure count */,
  TLEID_LinkXIDErrLen      =  54 /* Link received XID                   */,
  TLEID_LU62FrameRej       =  55 /* Received LU 6.2 frame rejected      */,
  TLEID_BadFrame           =  56 /* Bad frame                           */,
  TLEID_RejDLURBind        =  58 /* Rejecting DLUR Bind - already bound by a DLUS */,
  TLEID_AccCPBind          =  59 /* Accepting CP Bind from NN Server    */,
  TLEID_RejCPBind          =  60 /* Rejecting CP Bind - not from current NN Server */,
  TLEID_CPBindAcc          =  61 /* Bind accepted by NN Server - sending CP Caps */,
  TLEID_CPBindRej          =  62 /* CP Bind rejected by NN Server       */,
  TLEID_CPConWinUnbnd      =  63 /* CP Conwinner unbound by NN Server   */,
  TLEID_CPConLosUnbnd      =  64 /* CP Conloser unbound by NN Server    */,
  TLEID_CPSendResReg       =  65 /* CP Sending Resource registration to NN Server */,
  TLEID_CPReply2Caps       =  66 /* CP replying to Capabilities from NN Server */,
  TLEID_CPReplyRecd        =  67 /* CP Capabilities reply received from NN Server */,
  TLEID_AnswerLocate       =  68 /* Answering Locate for DLUR from NN Server */,
  TLEID_RejRecdLocate      =  69 /* Rejecting received Locate           */,
  TLEID_RecdRoute2DLUS     =  70 /* Received Route to DLUS from NN Server */,
  TLEID_NoRoute2DLUS       =  71 /* No Route to DLUS from NN Server - trying NN server link */,
  TLEID_CPSendBind         =  72 /* CP Sending Bind to NN Server        */,
  TLEID_Link2SrvrLost      =  73 /* Link to NN Server lost              */,
  TLEID_SendTGTDU2DLUS     =  74 /* Sending TG TDU to DLUS              */,
  TLEID_LinkUp             =  75 /* Link up                             */,
  TLEID_LinkDown           =  76 /* Link lost                           */,
  TLEID_Link2DLUSLost      =  77 /* Link to DLUS lost                   */,
  TLEID_SndVRNTDU2DLUS     =  78 /* Sending VRN TDU to DLUS             */,
  TLEID_SndREQACTPU        =  79 /* Sending REQACTPU to DLUS            */,
  TLEID_SndTDUs2DLUS       =  80 /* Sending TDUs to DLUS                */,
  TLEID_BindAccdByDLUS     =  81 /* Bind accepted by DLUS               */,
  TLEID_BindRejByDLUS      =  82 /* DLUR Bind rejected by DLUS          */,
  TLEID_ConWinUnbDLUS      =  83 /* Conwinner unbound by DLUS           */,
  TLEID_ConLosUnbDLUS      =  84 /* Conloser unbound by DLUS            */,
  TLEID_SndLocDLUS         =  85 /* Sending Locate for DLUS             */,
  TLEID_SndLocBUDLUS       =  86 /* Sending Locate for Backup DLUS      */,
  TLEID_SndBind2DLUS       =  87 /* Sending Bind to DLUS                */,
  TLEID_GotREQACTPU        =  88 /* RSP(REQACTPU) from DLUS             */,
  TLEID_GotREQDACTPU       =  89 /* RSP(REQDACTPU) from DLUS            */,
  TLEID_UnknownPuFr        =  90 /* Frame from DLUS for unknown PU      */,
  TLEID_DLUSProtErr        =  91 /* DLUS protocol Error                 */,
  TLEID_DLURRejBind        =  92 /* Rejecting Bind from DLUS            */,
  TLEID_AccBindFrDLUS      =  93 /* Accepting Bind from DLUS            */,
  TLEID_PoolExhausted      =  94 /* Link station pool exhausted         */,
  TLEID_SndREQDACTPU       =  95 /* Sending REQDACTPU to DLUS           */,
  TLEID_LuDisconnected     =  96 /* LU disconnected from Telnet session */,
  TLEID_LuContention       =  97 /* LU contention                       */,
  TLEID_LuUnexpectedRsp    =  98 /* LU unexpected response              */,
  TLEID_LuConnected        =  99 /* LU-LU session started               */,
  TLEID_LuUnbind           = 100 /* LU unbind                           */,
  TLEID_UnusedEntry        = 101 /* unused entry                        */
};

int InitScatBindingsTn (
     long          given_event_index,
     const char*   fld_sem_string,
     unsigned int  fld_sem_string_size)
{
  TnScatBind* scatbind_arr;
  const struct copy_fld_def* fld_def = (struct copy_fld_def*)fld_sem_string;
  int num_flds = fld_sem_string_size / sizeof(*fld_def);
  int bytes_to_malloc =  (num_flds+1) * sizeof(*scatbind_arr);
  int fldpos;
  int total_length = 0;
  if (given_event_index >= tn_max_entrynum)
    return -1;
  if ((scatbind_arr = (TnScatBind*)malloc(bytes_to_malloc)) == NULL)
    return -1;
  for (fldpos = 0; fldpos < num_flds; ++fldpos) {
    scatbind_arr[fldpos].fldNum = fld_def[fldpos].defIdnum;
    scatbind_arr[fldpos].fldOffset = total_length;
    total_length += fld_def[fldpos].defFldLength;
  }
  scatbind_arr[fldpos].fldNum = -1;
  scatbind_arr[fldpos].fldOffset = -1;
  tn_scatbind_arr[given_event_index].entrySize = total_length;
  tn_scatbind_arr[given_event_index].scatbindArr = scatbind_arr;
  return 0;
}

unsigned long APPNBindFailScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     unsigned long* snaSense,
     const void* vec_ptr,
     short vec_len)
{
  const TnFixedFields* fixed_flds;
  int           entrynum;
  TnScatBind* scatbind_ptr;
  int           fldnum, fld_pos;
  if (vec_len < sizeof(*fixed_flds))
    return -1;
  /*
  ** run thru each field in the vector, and translate it as a
  ** `switch'. Dumb, but simple.
  ** */
  fixed_flds = vec_ptr;
  entrynum = fixed_flds->entryNum;
  if ((entrynum > tn_max_entrynum) || (vec_len != tn_scatbind_arr[entrynum].entrySize))
    return -1;
  scatbind_ptr = tn_scatbind_arr[entrynum].scatbindArr;
  for (fld_pos = 0; (fldnum = scatbind_ptr[fld_pos].fldNum) != -1; ++fld_pos) {
    int fld_offset = scatbind_ptr[fld_pos].fldOffset;
    switch(fldnum) {
    case TLFID_snaSense:
        tn_scatter_snaSense(snaSense, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameLcl:
        tn_scatter_cpNameLcl(cpNameLcl, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameRem:
        tn_scatter_cpNameRem(cpNameRem, (unsigned char*)vec_ptr + fld_offset);  break;
    default:
      continue;
    }
  }
  return 0;
}

unsigned long APPNProtErrScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     unsigned long* snaSense,
     const void* vec_ptr,
     short vec_len)
{
  const TnFixedFields* fixed_flds;
  int           entrynum;
  TnScatBind* scatbind_ptr;
  int           fldnum, fld_pos;
  if (vec_len < sizeof(*fixed_flds))
    return -1;
  /*
  ** run thru each field in the vector, and translate it as a
  ** `switch'. Dumb, but simple.
  ** */
  fixed_flds = vec_ptr;
  entrynum = fixed_flds->entryNum;
  if ((entrynum > tn_max_entrynum) || (vec_len != tn_scatbind_arr[entrynum].entrySize))
    return -1;
  scatbind_ptr = tn_scatbind_arr[entrynum].scatbindArr;
  for (fld_pos = 0; (fldnum = scatbind_ptr[fld_pos].fldNum) != -1; ++fld_pos) {
    int fld_offset = scatbind_ptr[fld_pos].fldOffset;
    switch(fldnum) {
    case TLFID_snaSense:
        tn_scatter_snaSense(snaSense, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameLcl:
        tn_scatter_cpNameLcl(cpNameLcl, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameRem:
        tn_scatter_cpNameRem(cpNameRem, (unsigned char*)vec_ptr + fld_offset);  break;
    default:
      continue;
    }
  }
  return 0;
}

unsigned long BadPIUScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     unsigned long* snaSense,
     unsigned char* tgNumber,
     const void* vec_ptr,
     short vec_len)
{
  const TnFixedFields* fixed_flds;
  int           entrynum;
  TnScatBind* scatbind_ptr;
  int           fldnum, fld_pos;
  if (vec_len < sizeof(*fixed_flds))
    return -1;
  /*
  ** run thru each field in the vector, and translate it as a
  ** `switch'. Dumb, but simple.
  ** */
  fixed_flds = vec_ptr;
  entrynum = fixed_flds->entryNum;
  if ((entrynum > tn_max_entrynum) || (vec_len != tn_scatbind_arr[entrynum].entrySize))
    return -1;
  scatbind_ptr = tn_scatbind_arr[entrynum].scatbindArr;
  for (fld_pos = 0; (fldnum = scatbind_ptr[fld_pos].fldNum) != -1; ++fld_pos) {
    int fld_offset = scatbind_ptr[fld_pos].fldOffset;
    switch(fldnum) {
    case TLFID_snaSense:
        tn_scatter_snaSense(snaSense, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameLcl:
        tn_scatter_cpNameLcl(cpNameLcl, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameRem:
        tn_scatter_cpNameRem(cpNameRem, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_tgNumber:
        tn_scatter_tgNumber(tgNumber, (unsigned char*)vec_ptr + fld_offset);  break;
    default:
      continue;
    }
  }
  return 0;
}

unsigned long DLUSIsntScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     const void* vec_ptr,
     short vec_len)
{
  const TnFixedFields* fixed_flds;
  int           entrynum;
  TnScatBind* scatbind_ptr;
  int           fldnum, fld_pos;
  if (vec_len < sizeof(*fixed_flds))
    return -1;
  /*
  ** run thru each field in the vector, and translate it as a
  ** `switch'. Dumb, but simple.
  ** */
  fixed_flds = vec_ptr;
  entrynum = fixed_flds->entryNum;
  if ((entrynum > tn_max_entrynum) || (vec_len != tn_scatbind_arr[entrynum].entrySize))
    return -1;
  scatbind_ptr = tn_scatbind_arr[entrynum].scatbindArr;
  for (fld_pos = 0; (fldnum = scatbind_ptr[fld_pos].fldNum) != -1; ++fld_pos) {
    int fld_offset = scatbind_ptr[fld_pos].fldOffset;
    switch(fldnum) {
    case TLFID_cpNameLcl:
        tn_scatter_cpNameLcl(cpNameLcl, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameRem:
        tn_scatter_cpNameRem(cpNameRem, (unsigned char*)vec_ptr + fld_offset);  break;
    default:
      continue;
    }
  }
  return 0;
}

unsigned long DLUSXSubNetScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     const void* vec_ptr,
     short vec_len)
{
  const TnFixedFields* fixed_flds;
  int           entrynum;
  TnScatBind* scatbind_ptr;
  int           fldnum, fld_pos;
  if (vec_len < sizeof(*fixed_flds))
    return -1;
  /*
  ** run thru each field in the vector, and translate it as a
  ** `switch'. Dumb, but simple.
  ** */
  fixed_flds = vec_ptr;
  entrynum = fixed_flds->entryNum;
  if ((entrynum > tn_max_entrynum) || (vec_len != tn_scatbind_arr[entrynum].entrySize))
    return -1;
  scatbind_ptr = tn_scatbind_arr[entrynum].scatbindArr;
  for (fld_pos = 0; (fldnum = scatbind_ptr[fld_pos].fldNum) != -1; ++fld_pos) {
    int fld_offset = scatbind_ptr[fld_pos].fldOffset;
    switch(fldnum) {
    case TLFID_cpNameLcl:
        tn_scatter_cpNameLcl(cpNameLcl, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameRem:
        tn_scatter_cpNameRem(cpNameRem, (unsigned char*)vec_ptr + fld_offset);  break;
    default:
      continue;
    }
  }
  return 0;
}

unsigned long NoLuSessionsScatter (
     unsigned long* ipAddrLcl,
     unsigned short* tcpPortLcl,
     const void* vec_ptr,
     short vec_len)
{
  const TnFixedFields* fixed_flds;
  int           entrynum;
  TnScatBind* scatbind_ptr;
  int           fldnum, fld_pos;
  if (vec_len < sizeof(*fixed_flds))
    return -1;
  /*
  ** run thru each field in the vector, and translate it as a
  ** `switch'. Dumb, but simple.
  ** */
  fixed_flds = vec_ptr;
  entrynum = fixed_flds->entryNum;
  if ((entrynum > tn_max_entrynum) || (vec_len != tn_scatbind_arr[entrynum].entrySize))
    return -1;
  scatbind_ptr = tn_scatbind_arr[entrynum].scatbindArr;
  for (fld_pos = 0; (fldnum = scatbind_ptr[fld_pos].fldNum) != -1; ++fld_pos) {
    int fld_offset = scatbind_ptr[fld_pos].fldOffset;
    switch(fldnum) {
    case TLFID_ipAddrLcl:
        tn_scatter_ipAddrLcl(ipAddrLcl, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_tcpPortLcl:
        tn_scatter_tcpPortLcl(tcpPortLcl, (unsigned char*)vec_ptr + fld_offset);  break;
    default:
      continue;
    }
  }
  return 0;
}

unsigned long SystemScatter (
     unsigned char* systemStateTransition,
     const void* vec_ptr,
     short vec_len)
{
  const TnFixedFields* fixed_flds;
  int           entrynum;
  TnScatBind* scatbind_ptr;
  int           fldnum, fld_pos;
  if (vec_len < sizeof(*fixed_flds))
    return -1;
  /*
  ** run thru each field in the vector, and translate it as a
  ** `switch'. Dumb, but simple.
  ** */
  fixed_flds = vec_ptr;
  entrynum = fixed_flds->entryNum;
  if ((entrynum > tn_max_entrynum) || (vec_len != tn_scatbind_arr[entrynum].entrySize))
    return -1;
  scatbind_ptr = tn_scatbind_arr[entrynum].scatbindArr;
  for (fld_pos = 0; (fldnum = scatbind_ptr[fld_pos].fldNum) != -1; ++fld_pos) {
    int fld_offset = scatbind_ptr[fld_pos].fldOffset;
    switch(fldnum) {
    case TLFID_systemStateTransition:
        tn_scatter_systemStateTransition(systemStateTransition, (unsigned char*)vec_ptr + fld_offset);  break;
    default:
      continue;
    }
  }
  return 0;
}

unsigned long XIDRejScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     unsigned long* snaSense,
     unsigned char* adapter,
     const void* vec_ptr,
     short vec_len)
{
  const TnFixedFields* fixed_flds;
  int           entrynum;
  TnScatBind* scatbind_ptr;
  int           fldnum, fld_pos;
  if (vec_len < sizeof(*fixed_flds))
    return -1;
  /*
  ** run thru each field in the vector, and translate it as a
  ** `switch'. Dumb, but simple.
  ** */
  fixed_flds = vec_ptr;
  entrynum = fixed_flds->entryNum;
  if ((entrynum > tn_max_entrynum) || (vec_len != tn_scatbind_arr[entrynum].entrySize))
    return -1;
  scatbind_ptr = tn_scatbind_arr[entrynum].scatbindArr;
  for (fld_pos = 0; (fldnum = scatbind_ptr[fld_pos].fldNum) != -1; ++fld_pos) {
    int fld_offset = scatbind_ptr[fld_pos].fldOffset;
    switch(fldnum) {
    case TLFID_snaSense:
        tn_scatter_snaSense(snaSense, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameLcl:
        tn_scatter_cpNameLcl(cpNameLcl, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_cpNameRem:
        tn_scatter_cpNameRem(cpNameRem, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_adapter:
        tn_scatter_adapter(adapter, (unsigned char*)vec_ptr + fld_offset);  break;
    default:
      continue;
    }
  }
  return 0;
}

unsigned long XIDRejIDScatter (
     unsigned long* idBlkNum,
     unsigned long* snaSense,
     unsigned char* adapter,
     const void* vec_ptr,
     short vec_len)
{
  const TnFixedFields* fixed_flds;
  int           entrynum;
  TnScatBind* scatbind_ptr;
  int           fldnum, fld_pos;
  if (vec_len < sizeof(*fixed_flds))
    return -1;
  /*
  ** run thru each field in the vector, and translate it as a
  ** `switch'. Dumb, but simple.
  ** */
  fixed_flds = vec_ptr;
  entrynum = fixed_flds->entryNum;
  if ((entrynum > tn_max_entrynum) || (vec_len != tn_scatbind_arr[entrynum].entrySize))
    return -1;
  scatbind_ptr = tn_scatbind_arr[entrynum].scatbindArr;
  for (fld_pos = 0; (fldnum = scatbind_ptr[fld_pos].fldNum) != -1; ++fld_pos) {
    int fld_offset = scatbind_ptr[fld_pos].fldOffset;
    switch(fldnum) {
    case TLFID_snaSense:
        tn_scatter_snaSense(snaSense, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_adapter:
        tn_scatter_adapter(adapter, (unsigned char*)vec_ptr + fld_offset);  break;
    case TLFID_idBlkNum:
        tn_scatter_idBlkNum(idBlkNum, (unsigned char*)vec_ptr + fld_offset);  break;
    default:
      continue;
    }
  }
  return 0;
}

