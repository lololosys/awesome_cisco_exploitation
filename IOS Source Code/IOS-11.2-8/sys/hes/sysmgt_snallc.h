/* $Id: sysmgt_snallc.h,v 3.2 1995/11/17 09:21:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sysmgt_snallc.h,v $
 *------------------------------------------------------------------
 * sysmgt_snallc.h - System management SNA LLC definitions.
 *
 * May 1995, Mike Otto
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysmgt_snallc.h,v $
 * Revision 3.2  1995/11/17  09:21:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/20  13:34:05  motto
 * CSCdi37360:  CIP SNA LLC CC state display incorrect
 *
 * Revision 2.2  1995/07/18  21:46:36  motto
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.1  1995/06/07  20:45:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _SYSMGT_SNADLCLLC_H_
#define _SYSMGT_SNADLCLLC_H_

/*
 * Special Note:  Keep this file in sync between the the CIP microcode
 *                and RP/RSP system code. The file will have the same
 *                name in both repositories. You must edit the changes
 *                so the cvs header does not get screwed up.   /motto
 */
 
 
#define MAC_ADDR_LEN 6
#define MAC_ADDR_ALIGNED_LEN 8
#define PORT_NAME_LEN 8
#define FRMR_INFO_LEN 5
#define FRMR_INFO_ALIGNED_LEN 8

typedef enum sysmgt_snadlcllc_table_ {
    SYSMGT_LLC_SESSIONS,
    SYSMGT_LLC_PORT_ADMIN,
    SYSMGT_LLC_PORT_OPER,
    SYSMGT_LLC_PORT_STATS,
    SYSMGT_LLC_SAP_ADMIN,
    SYSMGT_LLC_SAP_OPER,
    SYSMGT_LLC_SAP_STATS,
    SYSMGT_LLC_CC_ADMIN,
    SYSMGT_LLC_CC_OPER,
    SYSMGT_LLC_CC_STATS
} sysmgt_snadlcllc_table_t;

typedef enum sysmgt_snadlcllc_states_ {
    SYSMGT_UNKNOWN,
    SYSMGT_ADM,
    SYSMGT_SETUP,
    SYSMGT_CONN,
    SYSMGT_NORMAL,
    SYSMGT_BUSY,
    SYSMGT_REJECT,
    SYSMGT_AWAIT,
    SYSMGT_AWAIT_BUSY,
    SYSMGT_AWAIT_REJECT,
    SYSMGT_D_CONN,
    SYSMGT_RESET,
    SYSMGT_ERROR,
    SYSMGT_PEND_DISC_RSP
} sysmgt_snadlcllc_states_t;

typedef enum sysmgt_snadlcllc_ww_causes_ {
    SYSMGT_NEVER_INVOKED = 1,
    SYSMGT_LOST_DATA,
    SYSMGT_MAC_LAYER_CONGESTION,
    SYSMGT_OTHER
} sysmgt_snadlcllc_ww_causes_t;

typedef enum sysmgt_snadlcllc_fail_causes_ {
    SYSMGT_UNDEFINED = 1,
    SYSMGT_RX_FRMR,
    SYSMGT_TX_FRMR,
    SYSMGT_DISC_RECEIVED,
    SYSMGT_DISC_SENT,
    SYSMGT_RETRIES_EXPIRED,
    SYSMGT_FORCED_SHUTDOWN
} sysmgt_snadlcllc_fail_causes_t;

typedef struct llc2_sessions_ {
    long            cipCardOperMaxLlc2Sessions;
    unsigned long   cipCardStatsHiWaterLlc2Sessions;
    unsigned long   cipCardStatsLlc2SessionAllocationErrs;
} llc2_sessions_t;

/*
 * The llcPortAdminEntry family. 
 */
typedef struct llc_port_index_ {
        long            vcn;
} llc_port_index_t;

typedef struct llc_port_admin_ {
        llc_port_index_t index;
        char            llcPortAdminName[PORT_NAME_LEN];
        unsigned long   llcPortAdminMaxSaps;
        unsigned long   llcPortAdminMaxCcs;
        long            llcPortAdminMaxPDUOctets;
        long            llcPortAdminMaxUnackedIPDUsSend;
        long            llcPortAdminMaxUnackedIPDUsRcv;
        long            llcPortAdminMaxRetransmits;
        unsigned long   llcPortAdminAckTimer;
        unsigned long   llcPortAdminPbitTimer;
        unsigned long   llcPortAdminRejTimer;
        unsigned long   llcPortAdminBusyTimer;
        unsigned long   llcPortAdminInactTimer;
        long            llcPortAdminDelayAckCount;
        unsigned long   llcPortAdminDelayAckTimer;
        long            llcPortAdminNw;
} llc_port_admin_t;

/*
 * The llcPortOperEntry family. 
 */
typedef struct llc_port_oper_ {
        llc_port_index_t index;
        uchar           llcPortOperMacAddress[MAC_ADDR_ALIGNED_LEN];
        unsigned long   llcPortOperNumSaps;
        unsigned long   llcPortOperHiWaterNumSaps;
        unsigned long   llcPortOperLastModifyTime;
} llc_port_oper_t;

/*
 * The llcPortStatsEntry family. 
 */
typedef struct llc_port_stats_ {
        llc_port_index_t index;
        unsigned long   llcPortStatsPDUsIn;
        unsigned long   llcPortStatsPDUsOut;
        unsigned long   llcPortStatsOctetsIn;
        unsigned long   llcPortStatsOctetsOut;
        unsigned long   llcPortStatsTESTCommandsIn;
        unsigned long   llcPortStatsTESTResponsesOut;
        unsigned long   llcPortStatsLocalBusies;
        unsigned long   llcPortStatsUnknownSaps;
} llc_port_stats_t;

/*
 * The llcSapAdminEntry family. 
 */
typedef struct llc_sap_index_ {
        long            vcn;
        long            local_sap;
} llc_sap_index_t;

typedef struct llc_sap_admin_ {
        llc_sap_index_t index;
        long            llcSapAdminMaxPDUOctets;
        long            llcSapAdminMaxUnackedIPDUsSend;
        long            llcSapAdminMaxUnackedIPDUsRcv;
        long            llcSapAdminMaxRetransmits;
        unsigned long   llcSapAdminAckTimer;
        unsigned long   llcSapAdminPbitTimer;
        unsigned long   llcSapAdminRejTimer;
        unsigned long   llcSapAdminBusyTimer;
        unsigned long   llcSapAdminInactTimer;
        long            llcSapAdminDelayAckCount;
        unsigned long   llcSapAdminDelayAckTimer;
        long            llcSapAdminNw;
} llc_sap_admin_t;

/*
 * The llcSapOperEntry family. 
 */
typedef struct _llc_sap_oper_ {
        llc_sap_index_t index;
        long            llcSapOperStatus;
        long            llcSapOperNumCcs;
        unsigned long   llcSapOperHiWaterNumCcs;
        unsigned long   csnaHandle;
        unsigned long   vtamHandle;
} llc_sap_oper_t;

/*
 * The llcSapStatsEntry family. 
 */
typedef struct _llc_sap_stats_ {
        llc_sap_index_t index;
        unsigned long   llcSapStatsTESTCommandsOut;
        unsigned long   llcSapStatsTESTResponsesIn;
        unsigned long   llcSapStatsXIDCommandsIn;
        unsigned long   llcSapStatsXIDCommandsOut;
        unsigned long   llcSapStatsXIDResponsesIn;
        unsigned long   llcSapStatsXIDResponsesOut;
        unsigned long   llcSapStatsUIFramesIn;
        unsigned long   llcSapStatsUIFramesOut;
        unsigned long   llcSapStatsUIOctetsIn;
        unsigned long   llcSapStatsUIOctetsOut;
        unsigned long   llcSapStatsConnectOk;
        unsigned long   llcSapStatsConnectFail;
        unsigned long   llcSapStatsDisconnect;
        unsigned long   llcSapStatsDisconnectFRMRSend;
        unsigned long   llcSapStatsDisconnectFRMRRcv;
        unsigned long   llcSapStatsDisconnectTimer;
        unsigned long   llcSapStatsDMsInABM;
        unsigned long   llcSapStatsSABMEsInABM;
} llc_sap_stats_t;

/*
 * The llcCcAdminEntry family. 
 */
typedef struct llc_cc_index_ {
        long            vcn;
        long            local_sap;
        uchar           rem_mac[MAC_ADDR_ALIGNED_LEN];
        long            rem_sap;
} llc_cc_index_t;

typedef struct llc_cc_admin_ {
        llc_cc_index_t  index;
        long            llcCcAdminBounce;
        long            llcCcAdminMaxPDUOctets;
        long            llcCcAdminMaxUnackedIPDUsSend;
        long            llcCcAdminMaxUnackedIPDUsRcv;
        long            llcCcAdminMaxRetransmits;
        unsigned long   llcCcAdminAckTimer;
        unsigned long   llcCcAdminPbitTimer;
        unsigned long   llcCcAdminRejTimer;
        unsigned long   llcCcAdminBusyTimer;
        unsigned long   llcCcAdminInactTimer;
        long            llcCcAdminDelayAckCount;
        unsigned long   llcCcAdminDelayAckTimer;
        long            llcCcAdminNw;
} llc_cc_admin_t;

/*
 * The llcCcOperEntry family. 
 */
typedef struct llc_cc_oper_ {
        llc_cc_index_t  index;
        long            llcCcOperState;
        long            llcCcOperMaxIPDUOctetsSend;
        long            llcCcOperMaxIPDUOctetsRcv;
        long            llcCcOperMaxUnackedIPDUsSend;
        long            llcCcOperMaxUnackedIPDUsRcv;
        long            llcCcOperMaxRetransmits;
        unsigned long   llcCcOperAckTimer;
        unsigned long   llcCcOperPbitTimer;
        unsigned long   llcCcOperRejTimer;
        unsigned long   llcCcOperBusyTimer;
        unsigned long   llcCcOperInactTimer;
        long            llcCcOperDelayAckCount;
        unsigned long   llcCcOperDelayAckTimer;
        long            llcCcOperNw;
        long            llcCcOperWw;
        unsigned long   llcCcOperCreateTime;
        unsigned long   llcCcOperLastModifyTime;
        unsigned long   llcCcOperLastFailTime;
        long            llcCcOperLastFailCause;
        uchar           llcCcOperLastFailFRMRInfo[FRMR_INFO_ALIGNED_LEN];
        long            llcCcOperLastWwCause;
} llc_cc_oper_t;

/*
 * The llcCcStatsEntry family. 
 */
typedef struct llc_cc_stats_ {
        llc_cc_index_t  index;
        unsigned long   llcCcStatsLocalBusies;
        unsigned long   llcCcStatsRemoteBusies;
        unsigned long   llcCcStatsIFramesIn;
        unsigned long   llcCcStatsIFramesOut;
        unsigned long   llcCcStatsIOctetsIn;
        unsigned long   llcCcStatsIOctetsOut;
        unsigned long   llcCcStatsSFramesIn;
        unsigned long   llcCcStatsSFramesOut;
        unsigned long   llcCcStatsRetransmitsOut;
        unsigned long   llcCcStatsREJsIn;
        unsigned long   llcCcStatsREJsOut;
        unsigned long   llcCcStatsWwCount;
} llc_cc_stats_t;


/*
 * The 802 Stack Events
 */
typedef enum llc_802_event_code_ {
    UNKNOWN_SAP_EVENT,               /* Received connection for unknown SAP */
    DUP_SAP_EVENT,                   /* Received Duplicate OPEN SAP */
    CCS_EXCEEDED_EVENT,              /* Connection attempt aborted due */
    LINK_LOST_EVENT,                 /* poll retry exhausted */
    DM_RXD_EVENT,                    /* DM received */
    SABME_RXD_IN_ABME_EVENT,         /* SABME received while in ABME */
    FRMR_RXD_EVENT,                  /* Received FRMR */
    FRMR_TXD_EVENT,                  /* Transmitted FRMR */
    MAX_SNALLC_EVENTS 
} llc_802_event_code;


/*
 * The llcCcEvents 
 */
#define CC_NOT_BUSY    0x00
#define REM_CC_BUSY    0x40
#define LOCAL_CC_BUSY  0x80


typedef struct llc_sap_event_ {
        llc_sap_index_t index;
        unsigned long csna_open_handle;    /* CSNA handle for open SAP */
        unsigned long vtam_open_handle;    /* VTAM handle for open SAP */
        unsigned long csna_fail_handle;    /* CSNA handle for duplicate SAP */
        unsigned long vtam_fail_handle;    /* VTAM handle for duplicate SAP */
} llc_sap_event_t;

typedef struct llc_cc_event_ {
        llc_cc_index_t  index;
        unsigned long csna_handle;         /* CSNA handle for SAP */
        unsigned long vtam_handle;         /* VTAM handle for SAP */
        unsigned long flow_correlator;     /* Flow corr for VTAM */
        unsigned long data_correlator;     /* Data corr for VTAM */
        unsigned long no_ack_cnt;          /* iframes rxd but not ack'd */
        uchar         v_r;                 /* Nr count */
        uchar         v_s;                 /* Ns count */
        uchar         last_n_r_rxd;        /* last Nr rxd */
        uchar         last_ctrl_rxd_msb;   /* last control rxd hi-order */
        uchar         last_ctrl_rxd_lsb;   /* last control rxd lo-order */
        uchar         last_ctrl_txd_msb;   /* last control txd hi-order */
        uchar         last_ctrl_txd_lsb;   /* last control txd lo-order */
        uchar         lstn_role;           /* lstn role (local busy/rem busy) */
        uchar         frmr[FRMR_INFO_LEN]; /* frmr bytes */
} llc_cc_event_t;


#define LLC2_SESSIONS_SIZE      sizeof(llc2_sessions_t)
#define LLC_SAP_EVENT_SIZE      sizeof(llc_sap_event_t)
#define LLC_CC_EVENT_SIZE       sizeof(llc_cc_event_t)
#define LLC_PORT_INDEX_SIZE     sizeof(llc_port_index_t)
#define LLC_SAP_INDEX_SIZE      sizeof(llc_sap_index_t)
#define LLC_CC_INDEX_SIZE       sizeof(llc_cc_index_t)
#define LLC_PORT_ADMIN_SIZE     sizeof(llc_port_admin_t)
#define LLC_PORT_OPER_SIZE      sizeof(llc_port_oper_t)
#define LLC_PORT_STATS_SIZE     sizeof(llc_port_stats_t)
#define LLC_SAP_ADMIN_SIZE      sizeof(llc_sap_admin_t)
#define LLC_SAP_OPER_SIZE       sizeof(llc_sap_oper_t)
#define LLC_SAP_STATS_SIZE      sizeof(llc_sap_stats_t)
#define LLC_CC_ADMIN_SIZE       sizeof(llc_cc_admin_t)
#define LLC_CC_OPER_SIZE        sizeof(llc_cc_oper_t)
#define LLC_CC_STATS_SIZE       sizeof(llc_cc_stats_t)



typedef enum sysmgt_snadlcllc_type_ {
    SYSMGT_LLC_PORT,
    SYSMGT_LLC_SAP,
    SYSMGT_LLC_CC
} sysmgt_snadlcllc_type_t;

typedef struct sna_dlc_llc_ {
    sysmgt_snadlcllc_type_t   info_type;
    ushort                    size;
    sysmgt_snadlcllc_table_t  table_id;
    long                      llcPortVirtualIndex;
    long                      llcSapNumber;
    uchar                     llcCcRMac[MAC_ADDR_LEN];
    long                      llcCcRSap;
    void                      *llc_info;
} sna_dlc_llc_t;

#endif /* _SYSMGT_SNADLCLLC_H_ */
