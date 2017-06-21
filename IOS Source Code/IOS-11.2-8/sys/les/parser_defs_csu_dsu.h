/* $Id: parser_defs_csu_dsu.h,v 3.2 1996/03/29 03:50:11 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/parser_defs_csu_dsu.h,v $
 *------------------------------------------------------------------
 *
 *	P A R S E R _ D E F S _ C S U _ D S U . H              
 *
 * March 1996, Johnny Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_csu_dsu.h,v $
 * Revision 3.2  1996/03/29  03:50:11  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:31  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */

#define CSU_DSU_MAX_TIMESLOTS	 24
#define CSU_DSU_MAX_USR_PAT_BITS 24
#define CSU_DSU_INVALID_USER_PAT 0xFFFFFFFF
#define CSU_DSU_TS_ALL_BMASK 0x00FFFFFF
#define CSU_DSU_BIT_SET_CHK(field, pos)	(field & (1 << (pos-1)))

#define CSU_DSU_NO_MODULE        0x1000
#define CSU_DSU_SERIAL_5IN1      0x2000
#define CSU_DSU_FT1              0x4000
#define CSU_DSU_SW56             0x8000
#define CSU_DSU_SW56_2_WIRE      0x0001
#define CSU_DSU_SW56_4_WIRE      0x0002
#define CSU_DSU_UNRECOGNISED     0x1010

#define CHK_ANY_CSU_DSU           	1
#define CHK_5IN1_CSU_DSU       	  	2
#define CHK_FT1_CSU_DSU           	3
#define CHK_SW56_CSU_DSU          	4
#define CHK_SW56_2W_CSU_DSU  		5
#define CHK_SW56_4W_CSU_DSU  		6
#define CHK_FT1_OR_SW56_4W_CSU_DSU 	7

/*
 * 56K/T1 flag for parser chain
 */
#define CSU_DSU_FT1_OPT			1
#define CSU_DSU_SW56_OPT		2

/*
 * service-module commands
 */
#define CSU_DSU_CLK_SRC 		0 
#define CSU_DSU_DCODE			(CSU_DSU_CLK_SRC + 1)
#define CSU_DSU_LOOPBACK_DTE		(LOOPBACK_DTE)     /* Statically mapped to value of 2 */
#define CSU_DSU_LOOPBACK_LINE		(LOOPBACK_LINE)    /* Statically mapped to value of 3 */
#define CSU_DSU_LOOPBACK_REMOTE		(LOOPBACK_REMOTE)  /* Statically mapped to value of 4 */
#define CSU_DSU_NWKTYPE			(CSU_DSU_LOOPBACK_REMOTE + 1)
#define CSU_DSU_SW_CARRIER		(CSU_DSU_NWKTYPE + 1)
#define CSU_DSU_RMT_LPBACK_SW56		(CSU_DSU_SW_CARRIER + 1)
#define CSU_DSU_RMT_LPBACK_T1_FULL	(CSU_DSU_RMT_LPBACK_SW56 + 1)
#define CSU_DSU_RMT_LPBACK_T1_FR	(CSU_DSU_RMT_LPBACK_T1_FULL + 1)
#define CSU_DSU_FRAMING			(CSU_DSU_RMT_LPBACK_T1_FR + 1)
#define CSU_DSU_LINECODE		(CSU_DSU_FRAMING + 1)
#define CSU_DSU_LBO			(CSU_DSU_LINECODE + 1)
#define CSU_DSU_TIMESLOTS		(CSU_DSU_LBO + 1)
#define CSU_DSU_REMOTE_ALRM_ENA		(CSU_DSU_TIMESLOTS + 1)
#define CSU_DSU_CLK_RATE		(CSU_DSU_REMOTE_ALRM_ENA + 1)
#define CSU_DSU_TEST			(CSU_DSU_CLK_RATE + 1)
#define CSU_DSU_SHOW			(CSU_DSU_TEST + 1)
#define CSU_DSU_CLEAR_COUNTERS		(CSU_DSU_SHOW + 1)
#define CSU_DSU_CLEAR          		(CSU_DSU_CLEAR_COUNTERS + 1)

#define CSU_DSU_CLK_SRC_NOSEL		0
#define CSU_DSU_CLK_SRC_LINE		1
#define CSU_DSU_CLK_SRC_INT  		2
#define CSU_DSU_CLK_SRC_DEFAULT		CSU_DSU_CLK_SRC_LINE

#define CSU_DSU_DCODE_NOSEL		0
#define CSU_DSU_DCODE_NORMAL		1
#define CSU_DSU_DCODE_INVERTED		2
#define CSU_DSU_DCODE_SCRAMBLED		3
#define CSU_DSU_DCODE_TIMESLOT_56KBPS	4
#define CSU_DSU_DCODE_DEFAULT		CSU_DSU_DCODE_NORMAL

#define CSU_DSU_NWKTYPE_NOSEL  		0
#define CSU_DSU_NWKTYPE_DDS		1
#define CSU_DSU_NWKTYPE_SWITCHED	2
#define CSU_DSU_SW56_2W_NWKTYPE_DEFAULT	CSU_DSU_NWKTYPE_SWITCHED
#define CSU_DSU_SW56_4W_NWKTYPE_DEFAULT CSU_DSU_NWKTYPE_DDS

#define CSU_DSU_SW_CARRIER_NOSEL	0
#define CSU_DSU_SW_CARRIER_ATT		1
#define CSU_DSU_SW_CARRIER_SPRINT	2
#define CSU_DSU_SW_CARRIER_OTHER	3
#define CSU_DSU_SW56_2W_CARRIER_DEFAULT CSU_DSU_SW_CARRIER_SPRINT
#define CSU_DSU_SW56_4W_CARRIER_DEFAULT	CSU_DSU_SW_CARRIER_ATT

#define CSU_DSU_RMT_LPBACK_MODE_NOSEL 	0
#define CSU_DSU_RMT_LPBACK_MODE_DEF 	1
#define CSU_DSU_RMT_LPBACK_MODE_FULL 	2
#define CSU_DSU_RMT_LPBACK_MODE_FRACT 	3

#define CSU_DSU_RMT_LPBACK_TYPE_NOSEL 	0
#define CSU_DSU_RMT_LPBACK_TYPE_DEF 	1
#define CSU_DSU_RMT_LPBACK_TYPE_ALT 	2
#define CSU_DSU_RMT_LPBACK_TYPE_V54 	3
#define CSU_DSU_RMT_LPBACK_TYPE_DISABLE	4
#define CSU_DSU_RMT_LPBACK_TYPE_DEFAULT	CSU_DSU_RMT_LPBACK_TYPE_DEF

#define CSU_DSU_FRAMING_NOSEL 		0
#define CSU_DSU_FRAMING_ESF 		1
#define CSU_DSU_FRAMING_SF 		2
#define CSU_DSU_FRAMING_DEFAULT		CSU_DSU_FRAMING_ESF

#define CSU_DSU_LINECODE_NOSEL		0
#define CSU_DSU_LINECODE_B8ZS 		1
#define CSU_DSU_LINECODE_AMI 		2
#define CSU_DSU_LINECODE_DEFAULT	CSU_DSU_LINECODE_B8ZS

#define CSU_DSU_LBO_NOSEL		0
#define CSU_DSU_LBO_NONE 		1
#define CSU_DSU_LBO_7_5_DB 		2
#define CSU_DSU_LBO_15DB 		3
#define CSU_DSU_LBO_DEFAULT		CSU_DSU_LBO_NONE


#define CSU_DSU_YELLOW_ALARM_MODE_DEF FALSE
#define CSU_DSU_RDL_MODE_DEFAULT      TRUE

#define CSU_DSU_CLK_RATE_NOSEL		0
#define CSU_DSU_CLK_RATE_AUTO 		1
#define CSU_DSU_CLK_RATE_2_4 		2
#define CSU_DSU_CLK_RATE_4_8 		3
#define CSU_DSU_CLK_RATE_9_6 		4
#define CSU_DSU_CLK_RATE_19_2 		5
#define CSU_DSU_CLK_RATE_38_4 		6
#define CSU_DSU_CLK_RATE_56 		7
#define CSU_DSU_CLK_RATE_64 		8
#define CSU_DSU_CLK_RATE_DEFAULT	CSU_DSU_CLK_RATE_AUTO

#define CSU_DSU_TS_TYPE_NOSEL 		0
#define CSU_DSU_TS_TYPE_ALL 		1
#define CSU_DSU_TS_TYPE_RANGE 		2

#define CSU_DSU_TS_SPEED_NOSEL 		0
#define CSU_DSU_TS_SPEED_56 		1
#define CSU_DSU_TS_SPEED_64 		2
#define CSU_DSU_TS_SPEED_DEFAULT	CSU_DSU_TS_SPEED_64

#define CSU_DSU_SHOW_DEF		1
#define CSU_DSU_SHOW_PERF_STAT		2

#define CSU_DSU_LPBACK_NONE		0
#define CSU_DSU_SELFTEST	        (CSU_DSU_LOOPBACK_REMOTE + 1)
		
#define CSU_DSU_LPBACK_TYPE_NOSEL 	0
#define CSU_DSU_LPBACK_TYPE_DEF		1
#define CSU_DSU_LPBACK_TYPE_FR		2
#define CSU_DSU_LPBACK_TYPE_SJ		3

#define CSU_DSU_LPBACK_PAT_NOSEL	0
#define CSU_DSU_LPBACK_PAT_QRW		1
#define CSU_DSU_LPBACK_PAT_1IN8		2
#define CSU_DSU_LPBACK_PAT_3in24	3
#define CSU_DSU_LPBACK_PAT_1IN2		4
#define CSU_DSU_LPBACK_PAT_1IN1		5
#define CSU_DSU_LPBACK_PAT_0IN1		6
#define CSU_DSU_LPBACK_PAT_1IN3		7
#define CSU_DSU_LPBACK_PAT_1IN5		8
#define CSU_DSU_LPBACK_PAT_UP		9
#define CSU_DSU_LPBACK_PAT_2047		10
#define CSU_DSU_LPBACK_PAT_511		11
#define CSU_DSU_LPBACK_PAT_STR_PAT	12

