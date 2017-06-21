/* $Id: st_counters.h,v 1.1.4.1 1996/05/09 15:01:52 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/st_counters.h,v $
 *------------------------------------------------------------------
 * st_counters.h -- Header file for saint statistics.
 * This header file provides the NMP with the SAINT statistics counters
 * interface to the LCP via SCP .
 * Note: All numbers are unsigned long and in BIG endian representation. 
 *
 * 11/07/94, Jason Kuo
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: jkuo
 *------------------------------------------------------------------
 * $Log: st_counters.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:52  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:47  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:57  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:41  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:55  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.5  1995/10/16  22:49:25  sgarcia
 * Added or completed file header.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef	_ST_COUNTERS_H_
#define	_ST_COUNTERS_H_


typedef	unsigned long	U32;
typedef	unsigned short	U16;
typedef	unsigned char	U8;

/*---------------------------------------------------------------------------
   The following data structures reflect how the SAINT counters are stored
   in the memory map.  This table provides a quick way for reading all the
   counters.
   NOTE: For a detailed description of the counter names, refer to the
   	 "SAINT Gate Array Hardware Functional Specification".

	 A1:	ALU domain 1
	 A2:	ALU domain 2
	 R:	Rx counters
	 T:	Tx counters
	 M:	MII counters
	 D:	DMA counters
	 1271:	1271 RMON MIB
	 1398:	1398 Ethernet MIB
	 1493:	1493 Bridging MIB
	 	un-numbered proposed 100BaseX MIB
 ---------------------------------------------------------------------------*/

typedef	struct SAINT_COUNTERS {
  /* ALU1 8-bit Domain */
  U32	etherStatsFragments;		/* A1_R_FRAG 1271 */
  U32	dot3StatsInternalMacRxErrs;     /* A1_R_OVRN 1398 */
  U32	etherStatsJabbers;		/* A1_R_JBBR 1271 */
  U32	baseX100StatsSymCodeViolates;	/* A1_R_CVIOL */
  U32	dot1BasePortDelayExcdAborts;	/* A1_T_DEFAB */
  U32	badTxCRC;			/* A1_T_CRC */
  U32	dot3StatsLateCollisions;	/* A1_T_COLTE 1398 */
  U32	dot3StatsExcessiveCollisions;	/* A1_T_COLEX 1398 */
  U32	dot3StatsSQETestErrors;		/* A1_M_SQE 1398 */
  U32	dot3StatsCarrierSenseErrors;	/* A1_M_CAR 1398 */
  U32	dmaOverflow;			/* A1_D_OFLOW */
  U32	linkFail;			/* A1_M_LINK */
  U32	txJabber;			/* A1_M_JBBR */
  U32	dmaTxLengthErrors;		/* A1_D_TXLEN */
  U32	dmaRxLengthErrors;		/* A1_D_RXLEN */
  U32	dmaXferLengthErrors;		/* A1_D_XT_LEN */  
  /* ALU1 16-bit Domain */
  U32	dmaMonitorPkts;			/* A1_D_MONPKT */
  U32	dot1dTpPortInFrames;		/* A1_R_RTOT 1493 */
  U32	dot3StatsFCSErrors;		/* A1_R_CRC 1398 */
  U32	dot1dTpPortOutFrames;		/* A1_T_TTOT 1493 */
  U32	dot3StatsSingleColFrames;     	/* A1_T_COL1 1398 */
  U32	dot3StatsMultiColFrames;     	/* A1_T_COLM 1398 */
  U32	dot3StatsInternalMacTxErrs;     /* A1_T_UNDRN 1398 */
  U32	dot3StatsDeferredTx;		/* A1_T_DEFER 1398 */
  U32	dot3StatsBadPreamble;		/* A1_R_BD_PRMBL 1398 (?) */
  U32	dot3StatsAbortLateCollision;	/* A1_T_AB_LCOL */
  U32	dot3StatsABortExcessiveLen;	/* A1_T_AB_LEN */
  U32	dot1dTpPortInDiscards;		/* A1_D_FLUSH 1493 */
  U32	rxRuntErrors;			/* A1_R_RUNT (RFCS|ALIGN)&SHORT */
  U32	dmaMacMismatch;			/* A1_D_MATCH in ver. 3 */
  					/* A2_D_MATCH in vers 1 & 2 */
  /* ALU2 16-bit Domain */
  U32	etherStatsRxPkts256to511;	/* A2_R_R511 1271 */
  U32	etherStatsRxPkts512to1023;	/* A2_R_R1023 1271 */
  U32	etherStatsRxPkts1024to1518;	/* A2_R_R1518 1271 */
  U32	etherStatsOversizePkts;		/* A2_R_GIANT 1271 */
  U32	dot3StatsAlignmentErrors;	/* A2_R_ALIGN 1398 */
  U32	etherStatsRxMulticastPkts;	/* A2_R_TOTRM 1271 */
  U32	etherStatsRxBroadcastPkts;	/* A2_R_TOTRB 1271 */
  U32	etherStatsTxPkts64;		/* A2_T_T64 1271 */
  U32	etherStatsTxPkts65to127;	/* A2_T_T127 1271 */
  U32	etherStatsTxPkts128to255;	/* A2_T_T255 1271 */
  U32	etherStatsTxPkts256to511;	/* A2_T_T511 1271 */
  U32	etherStatsTxPkts512to1023;	/* A2_T_T1023 1271 */
  U32	etherStatsTxPkts1024to1518;	/* A2_T_T1518 1271 */
  U32	dmaLastChance;			/* A2_D_LCHANCE */
  U32	etherStatsRxPkts65to127;	/* A2_R_R127 1271 */
  U32	etherStatsRxPkts128to255;	/* A2_R_R255 1271 */
  U32	etherStatsTxBroadcastPkts;	/* A2_T_BCAST 1271 */
  U32	etherStatsTxMulticastPkts;	/* A2_T_MCAST 1271 */
  /* ALU2 24-bit Domain */
  U32	etherStatsRxOctets;	       	/* A2_R_ROCT 1271 */
  U32	etherStatsTxOctets;	       	/* A2_T_TOCT 1271 */
  U32	dmaTxFull;			/* A2_D_TXFULL */
  U32	dmaRetry;			/* A2_D_RETRY */
  U32	dmaLevel2Request;		/* A2_D_REQ_P2 */
  U32	etherStatsUndersizePkts;	/* A2_R_SHORT 1271 */
  U32	dot3StatsTotalCollisions;	/* A2_T_COLLT 1398 ver. 3 */
  U32	etherStatsRxPkts64;		/* A2_R_R64 1271 */
} SAINT_COUNTERS, *P_SAINT_COUNTERS;

#define	TOTAL_COUNTS	(sizeof(SAINT_COUNTERS)/4)

extern	P_SAINT_COUNTERS  ReadCounters ( U8 portNum );

#endif
