/* $Id: if_fddi.h,v 3.3.56.3 1996/06/19 04:43:44 smackie Exp $
 * $Source: /release/112/cvs/Xsys/if/if_fddi.h,v $
 *------------------------------------------------------------------
 * global FDDI definitions used by the Cisco router software.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_fddi.h,v $
 * Revision 3.3.56.3  1996/06/19  04:43:44  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.3.56.2  1996/05/18  08:34:06  fsunaval
 * CSCdi57554:  FDDI PA needs OIR support
 * Branch: California_branch
 * 1.1F OIR support and patch for CSCdi57554.
 *
 * Revision 3.3.56.1  1996/03/21  22:48:49  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.2.1  1995/12/23  04:40:25  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.3  1995/11/17  09:27:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:52:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_FDDI_H__
#define __IF_FDDI_H__


/*********************************************************************
	Real Time (RT) Timer Definitions
*********************************************************************/
#define MAX_RTTIMER_COUNT	2


/*********************************************************************
	SMT ASCII Strings Definitions
*********************************************************************/
#define get_pcm_state_string(v)	get_smt_string (&SMTMsg.pc_state, v)
#define get_ecm_state_string(v)	get_smt_string (&SMTMsg.ec_state, v)
#define get_ecm_sig_string(v)	get_smt_string (&SMTMsg.ec_signal, v)
#define get_cfm_state_string(v)	get_smt_string (&SMTMsg.cf_state, v)
#define get_rmt_state_string(v)	get_smt_string (&SMTMsg.rm_state, v)
#define get_phy_type_string(v)	get_smt_string (&SMTMsg.phy_type, v)
#define get_mac_type_string(v)	get_smt_string (&SMTMsg.mac_type, v)
#define get_mac_intr_string(v)	get_smt_string (&SMTMsg.mac_intr, v)

/*********************************************************************
	Debugging definitions.
*********************************************************************/
extern int log_buffer;
#define	fddi_cbuginf	if (fddi_cmt_events_debug && \
			    (log_buffer == LOG_DEBUG)) buginf
extern	boolean		fddi_cmt_events_debug;
extern boolean      fddi_smt_packet_debug;
extern boolean      fddi_debug;
#define	bprintf		fddi_cbuginf

#define DBG_SYS_TMR	"SysTmr=%d         "
#define DBG_PHY_IND	"I%d*%1s:PHind=%4s "
#define DBG_PHY_REQ	"I%d*%1s:PHreq=%4s "
#define DBG_PCM_STATE	"I%d*%1s:PCsta=%4s "
#define DBG_PCM_TOUT	"I%d*%1s:PCtout     "
#define DBG_PCM_TVAL	"I%d*%1s:PCtval%d=%d  "
#define DBG_PCM_RVAL	"I%d*%1s:PCrval%d=%d  "
#define DBG_PCM_SIG	"I%d*%1s:PCsig=%4s "
#define DBG_PCM_NOISE	"I%d*%1s:PCnoise    "
#define DBG_PCM_OTHER	"I%d*%1s:PC%2x       "
#define DBG_LEM_FAIL	"I%d*%1s:LEfail     "
#define DBG_CFM_STATE	"I%d*CFsta=%6s "
#define DBG_CEM_OTHER	"I%d*%1s:CE%2x       "
#define DBG_CEM_SIG	"I%d*%1s:CEsig=%4s "
#define DBG_RMT_STATE	"I%d*%1s:RMsta=%4s "
#define DBG_RMT_TOUT	"I%d*%1s:RMtout     "
#define DBG_RMT_OTHER	"I%d*%1s:RM%2x       "
#define DBG_MAC_INTR	"I%d*%1s:MAintr=%4x "
#define DBG_ECM_STATE	"I%d*:ECsta=%5s "
#define DBG_ECM_SIG	"I%d*:ECsig=%5s "
#define DBG_ECM_OTHER	"I%d*:CF%2x        "

#define fddi_cbug_sys_tmr(time)	if (!(time & 0xF))		\
	fddi_cbuginf   (DBG_SYS_TMR,				\
			time)
#define fddi_cbug_phy_ind(stn, inst, val)			\
	fddi_cbuginf   (DBG_PHY_IND,				\
			stn,					\
			get_phy_type_string (inst),		\
			get_ls_string (val))
#define fddi_cbug_phy_req(stn, inst, val)			\
	fddi_cbuginf   (DBG_PHY_REQ,				\
			stn,					\
			get_phy_type_string (inst),		\
			get_ls_string (val))
#define fddi_cbug_pcm_state(stn, inst, val)			\
	fddi_cbuginf   (DBG_PCM_STATE,				\
			stn,					\
			get_phy_type_string (inst),		\
			get_pcm_state_string (val))
#define fddi_cbug_pcm_tout(stn, inst)				\
	fddi_cbuginf   (DBG_PCM_TOUT,				\
			stn,					\
			get_phy_type_string (inst))
#define fddi_cbug_pcm_noise(stn, inst)				\
	fddi_cbuginf   (DBG_PCM_NOISE,				\
			stn,					\
			get_phy_type_string (inst))
#define fddi_cbug_pcm_tval(stn, inst, index, val)		\
	fddi_cbuginf   (DBG_PCM_TVAL,				\
			stn,					\
			get_phy_type_string (inst),		\
			index,					\
			val)
#define fddi_cbug_pcm_rval(stn, inst, index, val)		\
	fddi_cbuginf   (DBG_PCM_RVAL,				\
			stn,					\
			get_phy_type_string (inst),		\
			index,					\
			val)
#define fddi_cbug_pcm_signal(stn, inst, val)			\
	fddi_cbuginf   (DBG_PCM_SIG,				\
			stn,					\
			get_phy_type_string (inst),		\
			get_pcm_signal_string (val))
#define fddi_cbug_pcm_other(stn, inst, index)			\
	fddi_cbuginf   (DBG_PCM_OTHER,				\
			stn,					\
			get_phy_type_string (inst),		\
			index & 0xFF)
#define fddi_cbug_lem_fail(stn, inst)				\
	fddi_cbuginf   (DBG_LEM_FAIL,				\
			stn,					\
			get_phy_type_string (inst))
#define fddi_cbug_ecm_state(stn, inst)				\
	fddi_cbuginf   (DBG_ECM_STATE,				\
			stn,					\
			get_ecm_state_string (inst))
#define fddi_cbug_ecm_signal(stn, val)				\
	fddi_cbuginf   (DBG_ECM_SIG,				\
			stn,					\
			get_ecm_sig_string (val & 0xFF))
#define fddi_cbug_ecm_other(stn, index)				\
	fddi_cbuginf   (DBG_ECM_OTHER,				\
			stn,					\
			index & 0xFF)
#define fddi_cbug_cfm_state(stn, val)				\
	fddi_cbuginf   (DBG_CFM_STATE,				\
			stn,					\
			get_cfm_state_string (val))
#define fddi_cbug_cem_other(stn, inst, index)			\
	fddi_cbuginf   (DBG_CEM_OTHER,				\
			stn,					\
			get_phy_type_string (inst),		\
			index & 0xFF)
#define fddi_cbug_rmt_state(stn, inst, val)			\
	fddi_cbuginf   (DBG_RMT_STATE,				\
			stn,					\
			get_mac_type_string (inst),		\
			get_rmt_state_string (val))
#define fddi_cbug_mac_intr(stn, inst, val)			\
	fddi_cbuginf   (DBG_MAC_INTR,				\
			stn,					\
			get_mac_type_string (inst),		\
			val)
#define fddi_cbug_rmt_tout(stn, inst)				\
	fddi_cbuginf   (DBG_RMT_TOUT,				\
			stn,					\
			get_mac_type_string (inst))
#define fddi_cbug_rmt_other(stn, inst, index)			\
	fddi_cbuginf   (DBG_RMT_OTHER,				\
			stn,					\
			get_mac_type_string (inst),		\
			index & 0xFF)


/*********************************************************************
	Cisco Platform-Independent SMT definitions
*********************************************************************/

/*
* Resource Index numbers.
*/
#define A_INDEX         ((uInt16) 1)
#define B_INDEX         ((uInt16) 2)
#define S_INDEX         ((uInt16) 1)
#define PRIM_MAC_INDEX  ((uInt16) 1)

/*
* SMT Configuration Structures
*/
typedef struct {
        uChar			manufacturerOUI[3];		/* 0x00-0x02 */
	uChar			pad1;				/* 0x03 */
        uChar			*pmanufacturerData;		/* 0x04-0x07 */
	uChar			*pUserData;			/* 0x08-0x0B */
	uInt16			ConfigCapabilities;		/* 0x0C-0x0D */
	uInt16			ConfigPolicy;			/* 0x0E-0x0F */
	uInt16			ConnectionPolicy;		/* 0x10-0x11 */
	uInt16			T_Notify;			/* 0x12-0x13 */
	FDDITime		Trace_MaxExpiration;		/* 0x14-0x17 */
	FDDIFlag		StatRptPolicy;			/* 0x18 */
	FDDIFlag		BypassPresent;			/* 0x19 */
	FDDIAvailablePaths	AvailablePaths;			/* 0x1A */
	uChar			pad2;				/* 0x1B */
	struct {
	    uChar		RMTDupCapabilities;		/* 0x1C */
	    uChar		pad[3];				/* 0x1D-0x1F */
	    uInt32		packages;			/* 0x20-0x23 */
	} xdi;
} SMTCfgType, *SMTCfgPtr;

/*
* MAC Configuration Structures
*/
typedef struct {
        uInt16			    MacCt;			/* 0x00-0x01 */
        uChar			    pad[2];			/* 0x02-0x03 */
	struct {
	    uInt16		    MacIndex;			/* 0x04-0x05 */
            FDDIFlag                MA_UnitdataEnable;		/* 0x06 */
            FDDIAvailablePaths	    AvailablePaths;		/* 0x07 */
            uInt16                  FrameErrorThreshold;	/* 0x08-0x09 */
            uInt16                  NotCopiedThreshold;		/* 0x0A-0x0B */
	    uInt16		    FrameStatusFunctions;	/* 0x0C-0x0D */
            uInt16		    BridgeFunctions;		/* 0x0E-0x0F */
            MACRequestedPathsType   RequestedPaths;		/* 0x10-0x13 */
            FDDITime		    T_MaxCapability;		/* 0x14-0x17 */
            FDDITime		    TVXCapability;		/* 0x18-0x1B */
	    struct {
	        uInt32		    packages;			/* 0x1C-0x1F */
	    } xdi;
	} mac[MAX_MAC_COUNT];
} MACCfgType, *MACCfgPtr;

/*
* PATH Resource Configuration Structures
*/
typedef struct {
        uInt16			    ResCt;			/* 0x00-0x01 */
	uChar			    pad[2];			/* 0x02-0x03 */
	struct {
	    uInt16		    ResIndex;			/* 0x00-0x01 */
	    uInt16		    ResType;			/* 0x02-0x03 */
	    uInt16		    OutResIndex;		/* 0x04-0x05 */
	    uInt16		    OutResType;			/* 0x06-0x07 */
	    uInt16		    InResIndex;			/* 0x08-0x09 */
	    uInt16		    InResType;			/* 0x0A-0x0B */
	} res[MAX_MAC_COUNT + MAX_PORT_COUNT];
} PATHRESCfgType, *PATHRESCfgPtr;

/*
* PATH Configuration Structures
*/
typedef struct {
        uInt16			    PathCt;			/* 0x00-0x01 */
	uChar			    pad[2];			/* 0x02-0x03 */
	struct {
	    uInt16		    PathIndex;			/* 0x00-0x01 */
	    uChar		    pad[2];			/* 0x02-0x03 */
	    FDDITime		    TVXLowerBound;		/* 0x04-0x07 */
	    FDDITime		    T_MaxLowerBound;		/* 0x08-0x0B */
	    FDDITime		    MaxT_Req;			/* 0x0C-0x0F */
	    PATHRESCfgPtr	    resCfg;			/* 0x10-0x13 */
	    struct {
	        uInt32		    packages;			/* 0x14-0x17 */
	    } xdi;
	} path[MAX_PATH_COUNT];
} PATHCfgType, *PATHCfgPtr;

/*
* PORT Configuration Structures
*/
typedef struct {
        uInt16			    PortCt;			/* 0x00-0x01 */
	uChar			    pad[2];			/* 0x02-0x03 */
	struct {
	    uInt16		    PortIndex;			/* 0x04-0x05 */
	    FDDIPortType	    My_Type;			/* 0x06-0x07 */
	    uChar		    ConnectionPolicies;		/* 0x08 */
	    uChar		    ConnectionCapabilities;	/* 0x09 */
	    uChar		    Ler_Cutoff;			/* 0x0A */
	    uChar		    Ler_Alarm;			/* 0x0B */
	    uInt16		    Maint_LS;			/* 0x0C-0x0D */
	    uInt16		    PMDClass;			/* 0x0E-0x0F */
	    PORTRequestedPathsType  RequestedPaths;		/* 0x10-0x13 */
	    FDDITime		    MACLoop_Time;		/* 0x14-0x17 */
	    FDDIAvailablePaths	    AvailablePaths;		/* 0x18 */
	    uChar		    pad[3];			/* 0x19-0x1B */
	    struct {
	        uInt32		    packages;			/* 0x1C-0x1F */
	    } xdi;
	} port[MAX_PORT_COUNT];
} PORTCfgType, *PORTCfgPtr;

/*
* Maximum SMT variables IDs
*/
/*
#define EC_STATE_MAXID			EC_DEINSERT
#define PC_STATE_MAXID			PC_MAINT
#define CF_STATE_MAXID			CF_thru
#define RM_STATE_MAXID			RM_TRACE
#define PC_SIGNAL_MAXID			PORTAction_stopport
#define EC_SIGNAL_MAXID			4
#define PHY_TYPE_MAXID			PC_Type_M
#define MAC_TYPE_MAXID			1
*/
#define EC_STATE_MAXID			7
#define PC_STATE_MAXID			9
#define CF_STATE_MAXID			12
#define RM_STATE_MAXID			7
#define PC_SIGNAL_MAXID			4
#define EC_SIGNAL_MAXID			4
#define PHY_TYPE_MAXID			3
#define MAC_TYPE_MAXID			1

/*
* SMT Messages Structure
*/
struct SmtMsg {
	uInt16 max;
	char *str[2];
};
typedef struct {
        struct {uInt16 max; char *str[EC_STATE_MAXID+1]; } ec_state;
        struct {uInt16 max; char *str[EC_STATE_MAXID+1]; } ec_signal;
        struct {uInt16 max; char *str[PC_STATE_MAXID+1]; } pc_state;
        struct {uInt16 max; char *str[CF_STATE_MAXID+1]; } cf_state;
        struct {uInt16 max; char *str[RM_STATE_MAXID+1]; } rm_state;
        struct {uInt16 max; char *str[PHY_TYPE_MAXID+1]; } phy_type;
        struct {uInt16 max; char *str[MAC_TYPE_MAXID+1]; } mac_type;
} SMTMsgType;

/*
* Station Descriptor
*/
typedef struct {
	SMTCfgType		*smtCfg;
	MACCfgType		*macCfg;
	PATHCfgType		*pathCfg;
	PORTCfgType		*portCfg;
	void			*xferRegs[MAX_MAC_COUNT];
	void			*macRegs[MAX_MAC_COUNT];
	void			*xferData[MAX_MAC_COUNT];
	void			*macData[MAX_MAC_COUNT];
	hwidbtype		*hwIDB[MAX_MAC_COUNT];
	void			*portRegs[MAX_PORT_COUNT];
	void			*portData[MAX_PORT_COUNT];
	mgd_timer		fbmTimer;
	ushort			prevCFState;
	boolean			hwPresent;
	boolean			smtUp;
	ushort			TLMin;
	ushort			CMin;
	ushort			TBMin;
	ushort			TOut;
	ushort			TraceCt;
	sys_timestamp		StartupTime;
} StnDCBType, *StnDCBPtr; 
#define STADATA(stnID,var)		(cscStnDCB[stnID].var)
#define STAINST(stnID)			(cscStnDCB[stnID])
#define SMTCFG(stnID,var)		(cscStnDCB[stnID].smtCfg->var)
#define MACCFG(stnID,var)		(cscStnDCB[stnID].macCfg->var)
#define PATHCFG(stnID,var)		(cscStnDCB[stnID].pathCfg->var)
#define PATHRESCFG(stnID,pathID,var)	(cscStnDCB[stnID].pathCfg->path[pathID].resCfg->var)
#define PORTCFG(stnID,var)		(cscStnDCB[stnID].portCfg->var)
#define HWIDBDATA(stnID,macID,var)	(cscStnDCB[stnID].hwIDB[macID]->var)


/*********************************************************************
	Data & Function Prototypes
*********************************************************************/

extern	StnDCBType			cscStnDCB[MAX_STATION_COUNT];
extern	SMTCfgType			SASSMTCfg;
extern	SMTCfgType			DASSMTCfg;
extern	MACCfgType			SingleBMACMACCfg;
extern	MACCfgType			SingleFORMACMACCfg;
extern	PATHCfgType			SASPATHCfg;
extern	PATHCfgType			DASPATHCfg;
extern	PORTCfgType			MSASPORTCfg;
extern	PORTCfgType			MMDASPORTCfg;
extern	PORTCfgType			SSDASPORTCfg;
extern  SMTMsgType			SMTMsg;

extern char *get_smt_string(void *ptr, ushort index);
extern char *get_pcm_signal_string(ushort pc_signal);
extern char *get_ls_string(ushort ls);

extern int fddi_smtqsize;

pid_t ServiceFBMTimer_process_id;
mgd_timer fbmTimer_parent;

    /* idb->fddi_stn_id needs to be unique
     * because the station id is used for allocating memory for its MIB data;
     * and this memory needs to be freed when this station is (oir) removed.
     *
     * Find unique station id.
     * In eoir cases, use the previous idb->fddi_stn_id if possible.
     *
     * First, try to use the current idb->fddi_stn_id value;
     * if this value is already taken by a FDDI PA in the box,
     * go thru the loop to find one that's not used.
     */

#define	FIND_FDDI_STATION_ID(idb)	\
    if ( STADATA(idb->fddi_stn_id, hwPresent) == TRUE ) { \
	int i; \
        for (i = 0; i < MAX_STATION_COUNT; i++) { \
            if (STADATA (i, hwPresent) == FALSE) { \
                idb->fddi_stn_id = i; \
                break; \
            } \
        } \
        if (i == MAX_STATION_COUNT) { \
            ttyprintf(CONTTY, "\nmif_init_interface: no fddi_stn_id avail"); \
            return(FALSE); \
        } \
    }

#endif __IF_FDDI_H__
