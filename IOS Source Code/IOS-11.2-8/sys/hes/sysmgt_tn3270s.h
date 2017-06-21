/* $Id: sysmgt_tn3270s.h,v 3.1.4.4 1996/09/10 06:36:19 bchan Exp $
 * $Source: /release/112/cvs/Xsys/hes/sysmgt_tn3270s.h,v $
 *------------------------------------------------------------------
 * TN3270 mib rp-cip communication structure
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysmgt_tn3270s.h,v $
 * Revision 3.1.4.4  1996/09/10  06:36:19  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/09/02  08:19:36  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/08/07  07:35:42  bchan
 * CSCdi64701:  crash when doing getmany tn3270sGlobalTable
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  11:19:28  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/05/12  08:10:12  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.2.1  1996/03/21  03:21:49  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:01:27  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _SYSMGT_TN3270SMIBTYPE_H_
#define _SYSMGT_TN3270SMIBTYPE_H_



/*
 * The tn3270sGlobalEntry family. 
 */


typedef
struct _tn3270sGblEntry {
        long            tn3270sChannelSlot;
        long            tn3270sMaxLus;
        unsigned long   tn3270sLusInUse;
        unsigned long   tn3270sRunningTime;     /* name changed from tn3270sStartupTime */
        char            tn3270sLuDomainStem[64];
        long            tn3270sGblTcpPort;
        long            tn3270sGblIdleTimeout;
        long            tn3270sGblKeepAlive;
        long            tn3270sGblUnbindAction;
        long            tn3270sGblGenericPool;
        long            tn3270sTimingMarkSupported;
  unsigned long   tn3270sGblLastEvent;/* no longer used */
} Tn3270sGblEntry_t;

typedef
struct _tn3270sStatsEntry {
        unsigned long   tn3270sStatsServerAddr;
        long            tn3270sStatsServerTcpPort;
        unsigned long   tn3270sStatsMaxSess;
        unsigned long   tn3270sStatsSpareSess;
        unsigned long   tn3270sStatsConnectsIn;
        unsigned long   tn3270sStatsDisconnects;
        unsigned long   tn3270sStatsTN3270ConnectsFailed;
        unsigned long   tn3270sStatsInboundTransactions;
        unsigned long   tn3270sStatsOutboundTransactions;
        unsigned long   tn3270sStatsSampledInboundTransactions;
        unsigned long   tn3270sStatsNetSampledInboundTransactionRespTime;
        unsigned long   tn3270sStatsSampledOutboundTransactions;
        unsigned long   tn3270sStatsNetSampledOutboundTransactionRespTime;
} Tn3270sStatsEntry_t;

/*
 * The tn3270sPuEntry family. 
 */


typedef
struct _tn3270sPuEntry {
        long            tn3270sPuIndex;
        unsigned long   tn3270sPuIpAddr;
        long            tn3270sPuTcpPort;
        long            tn3270sPuIdleTimeout;
        long            tn3270sPuKeepAlive;
        long            tn3270sPuUnbindAction;
        long            tn3270sPuGenericPool;
        long            tn3270sPuState;
        char            tn3270sPuLuSeed[8];
        long            tn3270sPuType;
        long            tn3270sLocalSapAddress;
        long            tn3270sRemoteSapAddress;
        char            tn3270sRemoteMacAddress[6];
        char            align[2];
  unsigned long   tn3270sPuLastEvent;/* no longer used */
        unsigned long   xid;
} Tn3270sPuEntry_t;

/*
 * The tn3270sIpEntry family. 
 */


typedef
struct _tn3270sIpEntry {
        unsigned long   tn3270sIpClientAddr;
        long            tn3270sIpClientTcpPort;
        long            tn3270sIpPuIndex;
        long            tn3270sIpLuIndex;
} Tn3270sIpEntry_t;



/*
 * The tn3270sLuEntry family. 
 */

#define NumberOfTraceBuffer 8
#define TraceBufferLength   12
#define InboundData         0x80
#define TelnetTrace         0x40


typedef
struct _tn3270sLuEntry {
        unsigned long   tn3270sLuPuIndex;
        unsigned long   tn3270sLuIndex;
        unsigned long   tn3270sLuClientAddr;
        long            tn3270sLuClientTcpPort;
        long            tn3270sLuTelnetType;
        char            tn3270sLuTermModel[8];
        long            tn3270sLuState;
        long            tn3270sLuCurInbPacing;
        long            tn3270sLuCurInbQsize;
        long            tn3270sLuCurOutQsize;
        long            tn3270sLuIdleTime;
        long            tn3270sLuType;
        long            tn3270sLuLfsid;
        char            tn3270sLuAppnLinkName[8];
        char            tn3270sLuEvents[16];
        char            luName[8];              /* lu name */
        long            lu_type;                /* lu type 1,2,3 */
        unsigned char   trace[NumberOfTraceBuffer][TraceBufferLength]; /* trace data  */

} Tn3270sLuEntry_t;



#define Tn3270sLogDataSize      480
typedef struct _tn3270sLogEntry {
        long            tn3270sLogIndexNewest;
        long            tn3270sLogChannelSlot;
        long            tn3270sLogNumReads;
        long            tn3270sLogTimeOldest;
        long            tn3270sLogTimeNewest;
        char            tn3270sLogData[Tn3270sLogDataSize];
} Tn3270sLogEntry_t;


/*
** sizeof tn3270sLogDefSemString() must be >= 120 see <tnloggath.c>
** */

typedef struct _tn3270sLogDefEntry {
        long            tn3270sLogDefEntryChannelSlot;
        long            tn3270sLogDefEntryEventIndex;
        char            tn3270sLogDefEntryIdString[36];
        char            tn3270sLogDefEntryFldString[256];

  /* this is extra to the MIB, but necessary.  don't mess with it */
        long            tn3270sLogDefEntryFldSize;
} Tn3270sLogDefEntry_t;

typedef struct _tn3270sExtra {
        long            tn3270sExtraIndex0;
        long            tn3270sExtraIndex1;
        unsigned long   version;         /* passed from CIP to indicate what data */
        unsigned long   puSentBytesHi;
        unsigned long   puReceivedBytesHi;
        char            spare[44];       /* to be added */
} Tn3270sExtra_t;

typedef enum sysmgt_snatn3270s_table_ {
    SYSMGT_TN3270S_GLOBAL,
    SYSMGT_TN3270S_STATS,
    SYSMGT_TN3270S_PU,
    SYSMGT_TN3270S_IP,
    SYSMGT_TN3270S_LU,
    SYSMGT_TN3270S_LOG,
    SYSMGT_TN3270S_NODE_ADMIN,
    SYSMGT_TN3270S_NODE_OPER,
    SYSMGT_TN3270S_PU_STAT,
    SYSMGT_TN3270S_LINK_ADMIN,
    SYSMGT_TN3270S_LINK_OPER,
    SYSMGT_TN3270S_LU_ADMIN,
    SYSMGT_TN3270S_LU_OPER,
    SYSMGT_TN3270S_LU_SESSN,
    SYSMGT_TN3270S_LU_SESSN_STAT,
    SYSMGT_TN3270S_LOG_DEF,
    SYSMGT_TN3270S_EXTRA,
    SYSMGT_TN3270S_MAXIMUM      /* always last */
} sysmgt_snatn3270s_table_t;


/*
** - TN3270S EVENTS AND EVENT CODES: Notes: the data passed in the
** events are passed in blocks which are 32-bit aligned (see
** ipc_messages.c).  Do _not_ attempt to add data objects to these
** structures which require larger alignment (doubles & ulonglongs).
** */
typedef struct sysmgt_tn3270s_gen_event {
  char           eventFields[256];        /* max 256 bytes of event-data */
  unsigned short eventLength;             /* length of event data        */
  unsigned short reserved;                /* reserved for alignment      */
  long           cipUpTime;               /* current CIP time since boot */
} sysmgt_tn3270s_gen_event;


typedef enum sysmgt_tn3270s_event_code {
  EVTTN3270S_GENERIC_POOL_DEPLETED,     /* generic pool ran out of LUs    */
  EVTTN3270S_NO_MEMORY_LU,              /* LU not allocated; no memory    */
  EVTTN3270S_SYSTEM,                    /* TN3270 SYSTEM UP/DOWN          */

  EVTTN3270S_DLUR_PROT_ERR,             /* DLUS Protocol error   */
  EVTTN3270S_DLUR_NOT_DLUS,             /* Partner not a DLUS  */
  EVTTN3270S_DLUR_MULTISUBNET,          /* Partner is cross-subnet */
  EVTTN3270S_APPN_BIND_FAIL,            /* Session set-up failure */
  EVTTN3270S_APPN_XID_ERR_CP,           /* XID negot'n err, CP name */
  EVTTN3270S_APPN_XID_ERR_ID,           /* XID negot'n err, IDBLKNUM */
  EVTTN3270S_APPN_BAD_PIU,              /* Bad PIU   */

  EVTTN3270S_MAX_EVENTS         /* always last                            */
} sysmgt_tn3270s_event_code;
   

#endif /* _TN3270SMIBTYPE_H_ */
