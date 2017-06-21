/* $Id: if_tn3270s.h,v 3.1.4.9 1996/09/10 06:36:15 bchan Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_tn3270s.h,v $
 *------------------------------------------------------------------
 * TN3270 server network management
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_tn3270s.h,v $
 * Revision 3.1.4.9  1996/09/10  06:36:15  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.8  1996/09/02  08:19:25  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.7  1996/08/07  07:35:40  bchan
 * CSCdi64701:  crash when doing getmany tn3270sGlobalTable
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/08/05  07:24:23  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/07/17  06:52:09  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/07/15  07:28:21  bchan
 * CSCdi62509:  Change tn3270 server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/06/27  04:52:09  mkirk
 * CSCdi59893:  TN3270 Server DLUR SHOW commands incomplete/incorrect
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/05/28  07:44:28  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  11:18:39  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/05/12  08:10:07  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.2.1  1996/03/21  03:21:40  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:00:38  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef if_tn3270s_h
#define if_tn3270s_h

#include "sysmgt_tn3270s.h"
#include "sysmgt_cipappn.h"
#include "sysmgt_dlurmib.h"

typedef struct LuControlCommon {
    ulong   puIndex;
    ulong   luIndex;
} LuControlCommon ;


boolean get_tn3270s(boolean search, hwidbtype *hwidb, sysmgt_snatn3270s_table_t type, void* entry);

boolean get_appn(boolean search, hwidbtype* hwidb, sysmgt_appn_table_t type, void* data);

boolean get_dlur(boolean search, hwidbtype* hwidb, sysmgt_dlur_table_t type, void* data);


hwidbtype* GetLuControlBlock(int searchType, sysmgt_snatn3270s_table_t type, 
                               LuControlCommon* luEntryData);

Tn3270sLogEntry_t*
get_tn3270_log (boolean search, hwidbtype* hwidb, long newest_index);

Tn3270sLogDefEntry_t*
get_tn3270_log_def (boolean search, hwidbtype* hwidb, long logdef_index);

struct _ContextInfo;  /* forward declaration */
hwidbtype* GetCipIdbFromCommunity (struct _ContextInfo *contextInfo);

hwidbtype* GetCipIdbFromSlot(int searchType, uint slot);
hwidbtype *cip_get_virtaul_hwidb (int searchType, long ifIndex);

/*
 * --------------------------------------------------------------- 
 * Show command interface
 * ---------------------------------------------------------------
 */
/*
 * --------------------------------------------------------------- 
 * Defines
 * ---------------------------------------------------------------
 */
#define MAX_DISPLAYMOD_SIZE 8
#define MaxEvents 16
#define MAX_NAME_SIZE  8
#define MAX_FQDN_SIZE 64
#define MAX_FQNN_SIZE 17       /* Fully Qualified Network Name 8.8 */

#define TN3270S_STATUS_NONE     0x0000
#define TN3270S_STATUS_NOTQRYD  0xfffe
#define TN3270S_STATUS_NOTKNOWN 0xffff

/*
 * PU State
 * Reference: SNA-NAU-MIB
 */
#define TN3270S_STATUS_PU_INACTIVE     0x0001
#define TN3270S_STATUS_PU_ACTIVE       0x0002
#define TN3270S_STATUS_PU_WAITING      0x0003
#define TN3270S_STATUS_PU_STOPPING     0x0004
/*
 * DLUR PU State
 * Reference: APPN-DLUR-MIB
 */
#define TN3270S_STATUS_DLUR_PU_RESET           0x0001
#define TN3270S_STATUS_DLUR_PU_PND_REQ_ACT_RSP 0x0002
#define TN3270S_STATUS_DLUR_PU_PND_ACTPU       0x0003
#define TN3270S_STATUS_DLUR_PU_PND_ACTPU_RSP   0x0004
#define TN3270S_STATUS_DLUR_PU_ACTIVE          0x0005
#define TN3270S_STATUS_DLUR_PU_PND_LINKACT     0x0006
#define TN3270S_STATUS_DLUR_PU_PND_DACTPU_RSP  0x0007
#define TN3270S_STATUS_DLUR_PU_PND_INOP        0x0008
#define TN3270S_STATUS_DLUR_PU_PND_INOP_ACTPU  0x0009
/*
 * LU State
 * Reference: SNA-NAU-MIB
 */
#define TN3270S_STATUS_LU_INACTIVE     0x0001
#define TN3270S_STATUS_LU_ACTIVE       0x0002
/*
 * Dlur-Dlus State
 * Reference: APPN-DLUR-MIB
 */
#define TN3270S_STATUS_DLUR_RESET      0x0001
#define TN3270S_STATUS_DLUR_PENDACTIVE 0x0002
#define TN3270S_STATUS_DLUR_ACTIVE     0x0003
#define TN3270S_STATUS_DLUR_PENDINACT  0x0004
/*
 * CP-CP State
 * Reference: None.
 *            These are yet to be defined
 *            The defined values are what the Spec indicates
 */
#define TN3270S_STATUS_CPCP_ACTIVE     0x0001
#define TN3270S_STATUS_CPCP_CONLOSER   0x0002
#define TN3270S_STATUS_CPCP_CONWINNER  0x0003
#define TN3270S_STATUS_CPCP_INACTIVE   0x0004
/*
 * SAP State
 * Reference: None.
 *            These are yet to be defined
 *            The defined values are what the Spec indicates
 */
#define TN3270S_STATUS_SAP_INACTIVE    0x0001
#define TN3270S_STATUS_SAP_ACTIVE      0x0002
#define TN3270S_STATUS_SAP_SAP_CLSD    0x0003
/*
 * Link State
 * Reference: IBM-6611-APPN-MIB
 */
#define TN3270S_STATUS_LINK_INACTIVE    0x0001
#define TN3270S_STATUS_LINK_PENDACTIVE  0x0002
#define TN3270S_STATUS_LINK_ACTIVE      0x0003
#define TN3270S_STATUS_LINK_PENDINACT   0x0004

#define TN3270S_STATUS_STR_NONE            "WAIT"
#define TN3270S_STATUS_STR_NOTQRYD         "SHUT"
#define TN3270S_STATUS_STR_UNKNOWN         "OTHER"
#define TN3270S_STATUS_STR_NOTKNOWN        "NOTKNOWN"

#define TN3270S_STATUS_STR_ACTIVE          "ACTIVE"
#define TN3270S_STATUS_STR_INACTIVE        "INACTIVE"
#define TN3270S_STATUS_STR_STOPPING        "STOPPING"
#define TN3270S_STATUS_STR_WAITING         "WAITING"
#define TN3270S_STATUS_STR_PENDACTIVE      "PND-ACTV"
#define TN3270S_STATUS_STR_PENDINACT       "PND-INAC"
#define TN3270S_STATUS_STR_RESET           "RESET"   
#define TN3270S_STATUS_STR_PND_REQ_ACT_RSP "P-REQA-R"
#define TN3270S_STATUS_STR_PND_ACTPU       "P-ACTPU"
#define TN3270S_STATUS_STR_PND_ACTPU_RSP   "P-ACTP-R"
#define TN3270S_STATUS_STR_PND_LINKACT     "LINKACT"
#define TN3270S_STATUS_STR_PND_DACTPU_RSP  "P-DACT-R"
#define TN3270S_STATUS_STR_PND_INOP        "P-INOP"
#define TN3270S_STATUS_STR_PND_INOP_ACTPU  "P-INOP-A"


/*
 * --------------------------------------------------------------- 
 * Structures for show commands
 * ---------------------------------------------------------------
 */
typedef struct sift_val_ {
    ushort      val;
} sift_val_t;

typedef struct show_siftdown_ {
    sift_val_t tcp_port;
    sift_val_t idle_time; 
    sift_val_t keepalive; 
    sift_val_t unbind_act;
    sift_val_t generic_pool;
} show_siftdown_t;

typedef struct tn3270s_show_svr_ {
    ulong           index;
    ushort          max_lu;
    ushort          max_lu_used;
    ushort          timingMarkSupported;
    char            lu_dns_fqdn[MAX_FQDN_SIZE+1];
    char            lu_rev_dns_fqdn[MAX_FQDN_SIZE+1];
    show_siftdown_t siftdown;
} tn3270s_show_svr_t;


typedef struct tn3270s_show_stat_ {
    ulong           ipAddr;
    ushort          tcpPort;
    ushort          maxSess;
    ushort          spareSess;
    ulong          connectsIn;
    ulong          disconnects;
    ulong          connectsFailed;
    ulong          iTransactions;
    ulong          oTransactions;
    ulong          siTransactions;
    ulong          siTransactionsTime;
    ulong          soTransactions;
    ulong          soTransactionsTime;
} tn3270s_show_stat_t;


typedef struct tn3270s_show_lu_ {
    ulong              pu_index;
    ulong              lu_index;
    uchar              lu_state;
    char               lu_name[MAX_FQNN_SIZE+1];
    char               pu_name[MAX_NAME_SIZE+1];
    ulong              client_port;
    ipaddrtype         client_ipaddr;
    char               telnetType;     /* tn3270(1), tn3270e(2), neverConnected(3) */
    char               appnLinkName[9];
    ushort             lfsid;
    uchar              curInbPacing;
    uchar              curInbQsize;
    uchar              curOutQsize;
    uchar              dynamicType;    /* dynamic(1), static(2) */
    uchar              luType;         /* type 1,2,3 */
    ushort             idleTime;
    char               display_mod_name[MAX_DISPLAYMOD_SIZE+1];
    char               luEvents[MaxEvents];
    ulong              maxSndRuSize;
    ulong              maxRcvRuSize;
    ulong              sndPacingSize;
    ulong              rcvPacingSize;
    char               assoc_pu_name[MAX_NAME_SIZE+1];
    ushort             assoc_locaddr;
    ulong              sentBytes;
    ulong              receivedBytes;
    ulong              sentRus;
    ulong              receivedRus;
    ulong              sentNegativeResps;
    ulong              receivedNegativeResps;
    uchar              trace[NumberOfTraceBuffer][TraceBufferLength]; /* trace data  */
} tn3270s_show_lu_t;

typedef union {
       ulong  w[2];
       ulonglong  l;
    } long_long;        /* for 64 bits arithmetics */

typedef struct tn3270s_show_pu_ {
    ulong              index;
    ushort             status;
    ushort             dlurStatus;
    ulong              xid;
    ipaddrtype         ipaddr;
    uchar              lsap;
    uchar              rmac[IEEEBYTES];
    uchar              rsap;
    show_siftdown_t    siftdown;
    char               lu_seed[MAX_NAME_SIZE+1];
    char               name_at_host[MAX_FQNN_SIZE+1];
    ushort             tn3270sPuType;
    long_long          sentBytes;
    long_long          receivedBytes;
    ulong              sentPius;
    ulong              receivedPius;
    ulong              sentNegativeResps;
    ulong              receivedNegativeResps;
    ulong              actLus;
    ulong              dactLus;
    ulong              bindLus;
    
} tn3270s_show_pu_t;

/* Note: DLUR links can be established without being
 * explicitly configured (pShowLink->dynamic)
 */
typedef struct tn3270s_show_link_ {
    ulong              index;
    ushort             status;
    char               name[MAX_NAME_SIZE+1];
    boolean            dynamic;
    uchar              lmac[IEEEBYTES];
    uchar              lsap;
    uchar              rmac[IEEEBYTES];
    uchar              rsap;
    char               partner_name[MAX_FQNN_SIZE+1];
    ushort             tgn;
    ushort             sessions;
    ushort             maxdata;    
    ushort             maxin;
    ushort             maxout;
} tn3270s_show_link_t;

typedef struct tn3270s_show_dlur_sap_ {
    ulong              index;
    ushort             status;
    char               name[MAX_NAME_SIZE+1];
} tn3270s_show_dlur_sap_t;

typedef struct tn3270s_show_dlur_ {
    ulong               index;
    char                dlur_fq_cpname[MAX_FQNN_SIZE+1];
    char                dlur_fq_dlusname[MAX_FQNN_SIZE+1];
    char                dlur_dlus_backup_name[MAX_FQNN_SIZE+1];
    char                dlur_nn_svr[MAX_FQNN_SIZE+1];
    ushort              dlur_dlus_status;
    ushort              cp_cp_status;
    char                current_dlusname[MAX_FQNN_SIZE+1];
    char                current_nn_svr[MAX_FQNN_SIZE+1];
} tn3270s_show_dlur_t;

/*
 * --------------------------------------------------------------- 
 * Prototypes
 * ---------------------------------------------------------------
 */

enum tn3270s_cip_query_t_ {
    TN3270S_CIP_QUERY_SVR,
    TN3270S_CIP_QUERY_PU,
    TN3270S_CIP_QUERY_LU,
    TN3270S_CIP_QUERY_CLIENT_LU,
    TN3270S_CIP_QUERY_DLUR,
    TN3270S_CIP_QUERY_DLUR_PU,
    TN3270S_CIP_QUERY_DLUR_SAP,
    TN3270S_CIP_QUERY_DLUR_LINK,
    TN3270S_CIP_QUERY_MAX
};
typedef enum tn3270s_cip_query_t_ tn3270s_cip_query_t;

typedef struct tn3270s_direct_pu_t_ tn3270s_direct_pu_t;
typedef struct tn3270s_dlur_pu_  tn3270s_dlur_pu_t;
typedef struct tn3270s_pu_  tn3270s_pu_t;

#define TN3270S_FIND_MATCH TRUE
#define TN3270S_FIND_NEXT  FALSE
#define TN3270S_QUERY_DETAIL TRUE
#define TN3270S_QUERY_STATUS FALSE

boolean tn3270s_cip_query(hwidbtype *, boolean, boolean,
			  tn3270s_cip_query_t, void *);

boolean tn3270s_cip_query_stats (hwidbtype          *hwidb, 
		      tn3270s_show_stat_t*  pShowStat);

hwidbtype *tn3270s_hwidb_from_pu_index(ulong, boolean, tn3270s_pu_t**);

hwidbtype* cip_get_virtual_hwidb(int searchType, long ifIndex);

tn3270s_pu_t* tn3270s_pu_from_index(hwidbtype* hwidb, boolean match, ulong* index);

boolean IsPuShut(tn3270s_pu_t* pPU, ulong* index);
boolean UpdateLocalPuConfig(tn3270s_pu_t* pPU, sysmgt_snatn3270s_table_t type,
    void* entry);

hwidbtype*
UpdatePuConfig (int exact, sysmgt_snatn3270s_table_t type, 
                  LuControlCommon* puEntry);

boolean  UpdatePuCardConfig(int exact, hwidbtype* idb,LuControlCommon* puEntry);

void add_tn3270s_entity(hwidbtype* hwidb);

ulong GetTn3270sUpTime (hwidbtype *hwidb);

#endif
