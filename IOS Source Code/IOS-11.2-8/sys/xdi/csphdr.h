/* $Id: csphdr.h,v 3.2 1995/11/17 18:08:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/csphdr.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: csphdr.h,v $
 * Revision 3.2  1995/11/17  18:08:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_csphdr.h xm7_v3.1.1:cset.0166:14:14 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Connection Services Process Header File
		
		CSP Header File
		
		File:			xm7_csphdr.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This is the header file used by all CSP modules. This file contains
		definitions for data structures, defined values of states, signals,
		and variables.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

/*********************************************************************
	Data Structures
*********************************************************************/

/*
*	Set the number of PORTs to one greater than MAX_PORT_COUNT to allow
*	for placeholder attachment port.
*/
#define	CSP_PORT_COUNT	(MAX_PORT_COUNT + 1)


/*
*	The following data structures define the global data used
*	throughout CSP. The data is divided to correspond to the station as
*	an object and each state machine as an object. The organization
*	for the data is based on the facilities description of the FDDI SMT
*	Connection Management.
*/

/*
*	This data structure defines the set of data associated with the
*	station.
*/
typedef struct CSPStationInfo
{
	/* MIB SMT values */
	Flag             Bypass;			/* bypass switch present */
	uInt32           Trace_Max;			/* Trace_Max expiration value */
	uInt16           configPolicy;		/* station configuration policies */
	uInt16           connectionPolicy;	/* station connection policies */
	uChar            attachCount;		/* # of attach PORTs */
	uChar            masterCount;		/* # of master PORTs */
	uChar            macCount;			/* total # of MACs */
	uChar            availablePaths;	/* paths available in the station */

	/* Locally determined values */
	Flag             CSPInitialized;	/* SET after CSP initialized from MIB */
	Flag             CF_MAC;			/* SET if MAC avail for LCT/loopback */
	uChar            A_Index;			/* index for port A */
	uChar            B_Index;			/* index for port B */
	uChar            S_Index;			/* index for port S */

	/* ECM Information */
	Flag             SB_Flag;			/* stuck bypass */
	uInt16           TEC;				/* ECM timer ID */
	uChar            Path_Test;			/* path test status */
	uInt16           ecState;			/* current ECM state */
	uInt16           cfState;			/* current configuration */

	/* Configuration Information */
	Flag             PrimaryUseless;	/* usability of primary path */
	Flag             SecondaryUseless;	/* usability of secondary path */
	Flag             ThruEnabled;		/* ThruEnabled flag */
	Flag             PeerWrapFlag;		/* PeerWrap condition status */
	uInt16           Pri_MPort_Active;	/* # M ports inserted */
	uInt16           Sec_MPort_Active;	/* # M ports inserted */
	uInt16           Pri_MAC_Avail;		/* # MACs on primary path */
	uInt16           Sec_MAC_Avail;		/* # MACs on secondary path */

	/* The following four fields are for PCM port management */
	uChar            maxMasterCanConnect;	/* max M ports connecting */
	uChar            shadowmaxMasterCanConnect;	/* saved max M ports */
	uChar            numPortsConnecting;/* # M ports connecting */
	uChar            gracefulMax;		/* max M port doing GI */

	/* Graceful insertion mode of the station */
	uInt16           gracefulMode;

	uInt16           giMACID;			/* index of MAC used for GI */
} CSPStationInfoType;

/*
*	This data structure defines the set of data associated with the
*	PATHs.
*/

typedef struct CSPPATHInfo
{
	/* MIB PATH values */
	uInt32           T_Rmode;			/* restricted token timer */
	uInt32           MaxT_Req;			/* max value for T_Req on this path */
	uInt32           T_MaxLB;			/* T_Max bound on this path */
	uInt32           TVXLB;				/* TVX bound on this path */

	/* XDI values */
	uInt16           ID;				/* paths MIB index */

	/* State information */
	uInt16           traceStatus;		/* trace status */
	Flag             scrubRequired;		/* set if path needs to be scrubbed */
} CSPPATHInfoType;

/*
*	This data structure defines the set of data associated with the
*	PORTs.
*/

/*
*	This value determines the number of sample periods to use for the
*	link error rate estimate. The estimate is performed over several
*	intervals to average out error bursts.
*/
#define LEM_WindowSize		5

/*
*	This structure is used by LEM to associate error counts with
*	time values. The count member indicates the number of
*	errors counted by the LEM hardware. The time member is the
*	amount of time in seconds.
*/
struct TimeCount
{
	uInt32           count;				/* number of errors counted */
	uInt32           time;				/* current total internal in seconds */
};

typedef struct CSPPORTInfo
{
	/* Manageable MIB PORT values */
	uInt32           MACLoop_Time;		/* time for loopback (not used) */
	uInt16           Maint_LS;			/* line state in maint state */
	uInt16           PC_Type;			/* PORT type */
	uInt16           index;				/* resource ID */
	uInt16           localPathIndex;	/* local path index */
	uChar            connectionPolicies;/* PORT's connection policies */
	uInt16           hwPresent;			/* set if PORT is present */
	uChar            lerCutoff;			/* LEM cutoff value */
	uChar            lerAlarm;			/* LEM alarm value */
	uChar            lerFlag;			/* LEM condition state */
	uChar            requestedPaths[3];	/* requested paths for this PORT */

	/* MIB attributes if HW present */
	uChar            availablePaths;	/* paths available to this PORT */
	uChar            conCapabilities;	/* PORT's connection capabilities */

	/* XDI PORT attributes */
	PathConfigType   configList[MAX_PATH_COUNT];	/* prev/next pointers for
													 * all PATHs */
	uChar            ID;				/* array element number */

	/* PCM information */
	uTime            LC_Test;			/* time for LCT (in usecs) */
	uChar            PC_Neighbor;		/* PORT's neighbor's type */
	uChar            PC_Mode;			/* physical connection mode */
	uChar            PC_Withhold;		/* reason for not connecting */
	uInt16           Connect_State;		/* connection status */
	uInt16           PC_LS;				/* current line state */
	uInt32           PC_LCT_Fail;		/* count of LCT failures */
	uChar            n;					/* next signal value */
	Flag             LS_Flag;			/* line state detected */
	Flag             RC_Flag;			/* receive pcode started */
	Flag             TC_Flag;			/* transmit pcode started */
	Flag             TD_Flag;			/* delay transmit pcode */
	Flag             CF_Loop;			/* requesting MAC for PORT */
	Flag             CF_Join;			/* requesting PORT to join ring */
	Flag             TR_Flag;			/* trace received */
	Flag             WC_Flag;			/* withhold connection */
	Flag             PC_MAC_LCT;		/* MAC is used for LCT */
	Flag             PC_MAC_Loop;		/* MAC is used for loopback */
	Flag             BS_Flag;			/* stuck in break */
	Flag             LEM_Fail;			/* SMT flag set when link fails */
	Flag             LCT_Passed;		/* set if LCT passes */
	uInt16           T_Val;				/* bits sent to neighbor PORT */
	uInt16           R_Val;				/* bits from neighbor PORT */
	uInt16           TPC;				/* PCM state timer ID */
	uInt16           TNE;				/* PCM noise timer */
	uInt16           TScrub;			/* PCM scrubbing timer */
	uInt16           pcState;			/* current PCM state */
	uInt16           intrState;			/* current port interrupts enabled */
	uInt16           TGI;				/* sched/graceful insertion timer */

	/* The following two fields are for PCM management */
	Flag             isConnecting;		/* port has a connect pending */
	Flag             isWaiting;			/* port has a connect waiting */

	/* CM Information */
	Flag             CF_Thru;			/* connect port in thru (A/B only) */
	Flag             CF_Wrap;			/* connect in wrap (A/B/S only) */
	Flag             CF_C_Wrap;			/* concatenated wrap (A/B/S only) */
	Flag             CF_Insert_P;		/* connect to primary (M only) */
	Flag             CF_Insert_S;		/* connect to secondary (M only) */
	Flag             ConcatEnabled;		/* configuration allows concatenation */
	Flag             PrimaryEnabled;	/* Object can be config on primary */
	Flag             SecondaryEnabled;	/* Object can be config on sec */
	Flag             LocalEnabled;		/* local path available for use */
	Flag             ConcatBackup;		/* configuration allows concatenation */
	Flag             PrimaryBackup;		/* Object can be config on primary */
	Flag             SecondaryBackup;	/* Object can be config on sec */
	uInt16           currentPath;		/* path port is on */
	uInt16           cpIndex;			/* PORT's current path index */
	uInt16           cmState;			/* current CM state */
	Flag             GI_Pending;		/* GI pending flag */

	/* LEM Information */
	uInt32           LEM_Reject_Ct;		/* # times link was rejected */
	uInt32           lemCount;			/* aggregate count */
	uInt16           TLE;				/* LEM timer ID */
	uInt16           threshhold;		/* current threshhold error count */
	uChar            estimate;			/* last LER estimate */
	uInt16           leState;			/* current LEM state */
	uChar            slotNumber;		/* current window slot */
	struct TimeCount					/* LEM sliding window */
	                 lemWindow[LEM_WindowSize];
} CSPPORTInfoType;

/*
*	XLNT Entry support
*
*   This structure is used to make a linked list of
*   ports to insert.  A linked list is used to implement
*   a quick method of retrieving the info structure of the
*   port to be inserted.
*/
typedef struct GIPortNode
{
	CSPPORTInfoType *portToInsert;
	struct GIPortNode *nextNode;
} GIPortNode;

/*
*	This data structure defines the set of data associated with the
*	MACs.
*/
typedef struct CSPMACInfo
{
	/* Manageable MIB attributes */
	uInt16           hwPresent;			/* set if MAC is present */
	Flag             unitdataEnable;	/* MAC available for xmit */
	uInt16           index;				/* resource ID */
	uInt16           localPathIndex;	/* local path index */
	uChar            requestedPaths;	/* request path for MAC */

	/* MIB attributes if HW present */
	uTime            T_MaxCap;			/* T_Max capability */
	uTime            TVXCap;			/* TVX capability */
	FDDILongAddressType SMTAddress;		/* MAC's address */
	FDDILongAddressType upstreamNbr;	/* MAC's upstream neighbor */
	uChar            availablePaths;	/* paths available to this MAC */

	/* MAC XDI attributes */
	uInt16           rmtDupPolicy;		/* RMT DA policy */
	uChar            ID;				/* array element number */
	PathConfigType   configList[MAX_PATH_COUNT];	/* next/prev pointers for
													 * all PATHs */
	uInt16           configStatus;		/* MAC config status */

	/* State information */
	Flag             Loop_Avail;		/* indicating MAC is ready for loop */
	Flag             MAC_Avail;			/* MAC available for PDUs */

	/* RMT Info */
	Flag             RM_Available;		/* XDI flag to control RMT operation */
	Flag             RM_Loop;			/* indicating MAC set to PORT */
	Flag             RM_Join;			/* indicating PORT is in token path */
	Flag             JM_Flag;			/* jam flag */
	Flag             DA_Flag;			/* duplicate address detected */
	Flag             BN_Flag;			/* beacon state reached */
	Flag             ringOP;			/* current ring operational state */
	Flag             claimTimed;		/* time since claiming > 2*D_Max */
	Flag             beaconTimed;		/* time since beaconing > 2*D_Max */
	Flag             Rmode;				/* MAC is in restricted mode */
	Flag             SM_MA_Avail;		/* SM_MA_Avail flag */
	uInt16           TRM;				/* RMT timer ID */
	uInt16           TRC;				/* RMT claim timer */
	uInt16           TRB;				/* RMT beacon timer */
	uInt16           rmState;			/* RMT state */
	uInt32           T_Max;				/* T_Max value to use */
	uInt32           TVX;				/* TVX value to use */
	uInt32           T_Req;				/* T_Req value to use */

	/* CM Information */
	Flag             Attach_P;			/* attach to primary */
	Flag             Attach_S;			/* attach to secondary */
	Flag             Attach_L;			/* attach to local */
	Flag             PrimaryEnabled;	/* Object can be config on primary */
	Flag             SecondaryEnabled;	/* Object can be config on sec */
	Flag             LocalEnabled;		/* local path available for use */
	Flag             PrimaryBackup;		/* Object can be config on primary */
	Flag             SecondaryBackup;	/* Object can be config on sec */
	uInt16           currentPath;		/* MAC's current path */
	uInt16           cpIndex;			/* MAC's current path index */
	uInt16           cmState;			/* CM state */

#if USE_FULL_GRACE
	/* Local MAC information */
	Flag             Run_Diagnostics;	/* run diag flag */
	Flag             Loop_Req;			/* local loop requested */
	Flag             Claim_Started;		/* wait for claim to complete */
	Flag             GI_Flag;			/* graceful insertion allowed */
	uInt16           lmState;			/* LMM state */
	uInt16           TGI;				/* GI timeout timer */

	/* list of ports to gracefully insert */
	GIPortNode       portListGI[MAX_PORT_GI];
	GIPortNode      *portListHead;		/* head of portListGI[] */
	GIPortNode      *portListTail;		/* tail of portListGI[] */
	GIPortNode      *freeListHead;		/* head of the free list */
#endif							/* USE_FULL_GRACE */

} CSPMACInfoType;


#ifdef MULTIPLE_STATION

struct CSPAllInfo
{
	struct CSPStationInfo stationData;
	struct CSPMACInfo macData[MAX_MAC_COUNT];
	struct CSPPATHInfo pathData[MAX_PATH_COUNT];
	struct CSPPORTInfo portData[CSP_PORT_COUNT];
};

#endif							/* MULTIPLE_STATION */


/*********************************************************************
	Signal Queue Data Structure
*********************************************************************/

struct SigQueueEntry
{
	uInt16           type;				/* signal type */
	uInt16           entity;			/* MAC/PORT array index */
	uInt32           data;				/* data if any */
	uInt16           stnID;				/* station index */
};


/*********************************************************************
	Timer List Data Structures
*********************************************************************/

struct TimerListEntry
{
	Flag             busy;				/* SET if entry is in use */
	uInt32           time;				/* timer interval in microseconds */
	uInt16           machine;			/* state machine signal type */
	uInt16           entity;			/* state machine entity (if needed) */
	uInt16           ID;				/* time event ID */
	uInt16           stnID;				/* station ID */
};


/*********************************************************************
	Signals
*********************************************************************/

/*
*	As defined in the SMT document, signals are used to initiate state
*	transitions. Signals may be sent from a management entity or another
*	state machine, or as a result of a change in hardware status. The
*	signals are divided into groups based upon the state machine used
*	to process the signal. Note that the high byte determines the state
*	machine and the low byte identifies the signal.
*/

#define	TIMER_SIGNAL	0xFF

#define	ECM_SIGNALS		0x100
#define	SIG_Connect		(ECM_SIGNALS | 0x01)
#define	SIG_Disconnect	(ECM_SIGNALS | 0x02)
#define	SIG_Trace_Prop	(ECM_SIGNALS | 0x03)
#define	SIG_Path_Test	(ECM_SIGNALS | 0x04)
#define	SIG_EC_PCM_RETRY	(ECM_SIGNALS | 0x07)	/* PCM retries */
#define	SIG_EC_Timer	(ECM_SIGNALS | TIMER_SIGNAL)

#define	PCM_SIGNALS		0x200
#define	SIG_PC_Start	(PCM_SIGNALS | 0x01)
#define	SIG_PC_Maint	(PCM_SIGNALS | 0x02)
#define	SIG_PC_Trace	(PCM_SIGNALS | 0x03)
#define	SIG_PC_Stop		(PCM_SIGNALS | 0x04)
#define	SIG_PC_Signal	(PCM_SIGNALS | 0x05)
#define	SIG_PC_PDR		(PCM_SIGNALS | 0x06)
#define	SIG_PC_Join		(PCM_SIGNALS | 0x07)
#define	SIG_Line_State	(PCM_SIGNALS | 0x09)
#define	SIG_Noise_Event	(PCM_SIGNALS | 0x0A)
#define	SIG_Noise_Timer	(PCM_SIGNALS | 0x0B)
#define	SIG_PC_Scrub_Complete\
						(PCM_SIGNALS | 0x0D)
#define	SIG_PC_Enable	(PCM_SIGNALS | 0x0F)
#define	SIG_PC_Disable	(PCM_SIGNALS | 0x10)
#define	SIG_PC_LEM_Fail	(PCM_SIGNALS | 0x11)
#define	SIG_PC_WC_Flag	(PCM_SIGNALS | 0x13)	/* change to WC_Flag in CFM */
#define	SIG_PC_RETRY	(PCM_SIGNALS | 0x14)	/* retry PCM */
#define	SIG_PC_Timer	(PCM_SIGNALS | TIMER_SIGNAL)

#define	CFM_SIGNALS		0x300
#define	SIG_CF_Loop		(CFM_SIGNALS | 0x01)
#define	SIG_CF_Join		(CFM_SIGNALS | 0x02)
#define SIG_CF_Change	(CFM_SIGNALS | 0x03)	/* XLNT Entry */
#define SIG_CF_Timer	(CFM_SIGNALS | TIMER_SIGNAL)

#define	RMT_SIGNALS		0x400
#define	SIG_RM_Loop		(RMT_SIGNALS | 0x01)
#define	SIG_RM_Join		(RMT_SIGNALS | 0x02)
#define	SIG_Ring_OP		(RMT_SIGNALS | 0x03)	/* change in Ring_OP */
#define	SIG_MAC_Interrupt\
						(RMT_SIGNALS | 0x04)	/* change in MAC state */
#define	SIG_Dup_Addr	(RMT_SIGNALS | 0x06)	/* dupl addr found in FBM */
#define	SIG_RM_DA_Flag	(RMT_SIGNALS | 0x0B)	/* change to DA_Flag */
#define	SIG_RM_MA_Unitdata\
						(RMT_SIGNALS | 0x0D)	/* change to MA_Unitdata */
#define	SIG_RM_Available\
						(RMT_SIGNALS | 0x0E)	/* XDI flag to control RMT */
#define	SIG_RM_Timer	(RMT_SIGNALS | TIMER_SIGNAL)

#define	LEM_SIGNALS		0x500
#define	SIG_LEM_Start	(LEM_SIGNALS | 0x01)	/* start LEM monitoring */
#define	SIG_LEM_Stop	(LEM_SIGNALS | 0x02)	/* stop LEM monitoring */
#define	SIG_LCT_Start	(LEM_SIGNALS | 0x03)	/* start LCT */
#define	SIG_LCT_Stop	(LEM_SIGNALS | 0x04)	/* stop LCT */
#define	SIG_LEM_Threshhold\
						(LEM_SIGNALS | 0x05)	/* H/W count overflow */
#define	SIG_LE_Timer	(LEM_SIGNALS | TIMER_SIGNAL)

/* XLNT Entry LMM State Machine */
#define LMM_SIGNALS		0x600
#define SIG_Abort_GI	(LMM_SIGNALS | 0x01)	/* abort graceful insertion */
#define SIG_Join_Primary	(LMM_SIGNALS | 0x02)	/* GI on primary path */
#define SIG_Join_Secondary	(LMM_SIGNALS | 0x03)	/* GI on secondary path */
#define SIG_Leave_Primary	(LMM_SIGNALS | 0x04)	/* graceful removal */
#define SIG_Leave_Secondary	(LMM_SIGNALS | 0x05)	/* graceful removal */
#define SIG_Change_Complete	(LMM_SIGNALS | 0x06)	/* GI is complete */
#define SIG_Run_Diagnostics	(LMM_SIGNALS | 0x07)	/* run local mac diags */
#define SIG_Loop_Req		(LMM_SIGNALS | 0x08)	/* put in loop */
#define SIG_Claim_Complete	(LMM_SIGNALS | 0x09)	/* local MAC claim done */
#define SIG_LM_Timer        (LMM_SIGNALS | TIMER_SIGNAL)


/*********************************************************************
	CSP Defined Values
*********************************************************************/

/*
*	PORT Interrupt Settings for SetPORTInterrupts()
*/
#define	PORT_INT_NONE		0x0			/* no interrupts enabled */
#define	PORT_INT_QLS		0x0001		/* enable QLS interrupt */
#define	PORT_INT_HLS		0x0002		/* enable HLS interrupt */
#define	PORT_INT_MLS		0x0004		/* enable MLS interrupt */
#define	PORT_INT_SILS		0x0008		/* enable SILS interrupt */
#define	PORT_INT_NOISE		0x0010		/* enable noise timer or NLS intr */
#define	PORT_INT_EB			0x0020		/* enable EB interrupt */

/*
*	MAC Interrupt Types
*/
#define	MAC_My_Claim		0x0001		/* MyClaim received */
#define	MAC_My_Beacon		0x0002		/* MyBeacon received */
#define	MAC_Other_Beacon	0x0004		/* OtherBeacon received */
#define	MAC_TRT_In_T4T5		0x0008		/* TRT expired in T4 or T5 */
#define	MAC_Rmode			0x0010		/* MAC entered restricted mode */
#define	MAC_TBid_TReq_Mismatch	0x0020	/* MyClaim rec'd and T_Bid != T_Req */


/*********************************************************************
	CSP Defined Macros
*********************************************************************/

#define	MCF_WRAP_AB(stnID)\
	(CSPDATA(stnID,stationData.configPolicy) & Config_CF_Wrap_AB)

/*
* The placeholder port element is at the end of the array.
*/
#define	EMPTY_PORT	MAX_PORT_COUNT

/*
*	Known Addresses
*/
#ifdef	SMT_Broadcast_DA
#undef	SMT_Broadcast_DA
#endif							/* SMT_Broadcast_DA */
#ifdef	NULL_ADDRESS
#undef	NULL_ADDRESS
#endif							/* NULL_ADDRESS */
#ifdef	SMT_Unknown_Address
#undef	SMT_Unknown_Address
#endif							/* SMT_Unknown_Address */
#ifdef	SMT_Directed_Beacon_DA
#undef	SMT_Directed_Beacon_DA
#endif							/* SMT_Directed_Beacon_DA */
#ifdef	SMT_SRF_DA
#undef	SMT_SRF_DA
#endif							/* SMT_SRF_DA */

#define	SMT_Broadcast_DA		cspBroadcastAddr
#define	NULL_ADDRESS			cspNullAddr
#define	SMT_Unknown_Address		cspUnknownAddr
#define	SMT_Directed_Beacon_DA	cspDirBcnDA
#define	SMT_SRF_DA				cspSrfDA

#ifdef	USE_62_DEFS

#ifdef	BROADCAST_ADDRESS
#undef	BROADCAST_ADDRESS
#endif							/* BROADCAST_ADDRESS */
#ifdef	UNKNOWN_ADDRESS
#undef	UNKNOWN_ADDRESS
#endif							/* UNKNOWN_ADDRESS */
#ifdef	DIR_BCN_ADDRESS
#undef	DIR_BCN_ADDRESS
#endif							/* DIR_BCN_ADDRESS */
#ifdef	SRF_MULTICAST_ADDRESS
#undef	SRF_MULTICAST_ADDRESS
#endif							/* SRF_MULTICAST_ADDRESS */

#define	BROADCAST_ADDRESS		SMT_Broadcast_DA
#define	UNKNOWN_ADDRESS			SMT_Unknown_Address
#define	DIR_BCN_ADDRESS			SMT_Directed_Beacon_DA
#define	SRF_MULTICAST_ADDRESS	SMT_SRF_DA

#endif							/* USE_62_DEFS */
