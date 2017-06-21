/* $Id: ibmnm.h,v 3.3.44.3 1996/08/07 08:59:20 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibmnm.h,v $
 *------------------------------------------------------------------
 * ibmnm.h -- IBM Network Management Protocol Definitions
 *
 * January 1991, David R. Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm.h,v $
 * Revision 3.3.44.3  1996/08/07  08:59:20  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.44.2  1996/08/03  23:25:27  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.3.44.1  1996/03/18  20:11:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/14  01:03:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.14.2  1996/03/07  09:41:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  14:21:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/16  22:06:39  motto
 * CSCdi35920:  %ALIGN-3-CORRECT messages on 4500
 *
 * Revision 3.2  1995/11/17  09:22:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:01:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/13  20:32:54  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.2  1995/06/15  22:00:34  hampton
 * Fix 24.5 day bugs in Lan Manager support.  While there, eliminate
 * direct references to msclock.  [CSCdi35914]
 *
 * Revision 2.1  1995/06/07 20:46:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

#define IBMNM_DEBUG

typedef uchar cookie_t;

/**********************************************************************
 *
 *              IBM LAN MANAGEMENT PROTOCOL DEFINITIONS
 *
 **********************************************************************/
typedef struct _ibmnm_vector {
    ushort   length;
    ushort   command;
    charlong data;
} ibmnm_vector;

#define ibmnm_vector_headstart(ptr) ((ptr)->network_start)

#define IBMNM_SAP		 0xF4
#define IBMNM_MVID_LEN   4
#define IBMNM_SPECIFIC   0x8000
#define IBMNM_ATOMIC     0x4000

/**********************************************************************
 *
 *			    Major Vectors
 *
 **********************************************************************/
#define SERVER_MASK	    0xff00
#define REM_SERVER	    0x8100
#define RPS_SERVER	    0x8200
#define CRS_SERVER	    0x8300
#define CNV_SERVER	    0x8400
#define LBS_SERVER	    0x8500
#define LRM_SERVER	    0x8600
#define ALT_SERVER	    0x8700
#define COMMAND_MASK	    0x00ff

#define REM_REQ_STATUS	    0x8101	/* command  */
#define REM_REP_STATUS	    0x8102	/* response */
#define REM_SET_PARAM	    0x8103	/* command  */
#define REM_PARAM_SET	    0x8104	/* response */
#define REM_PARAM_CHANGED   0x8105	/* response */
#define REM_ERROR_DECAY	    0x8106	/* response */
#define REM_PREWEIGH_EXC    0x8107	/* response */
#define REM_WEIGHT_EXC	    0x8108	/* response */
#define REM_NONISO_THRESH   0x8109	/* response */
#define REM_FORWARD	    0x810A
#define REM_ERROR	    0x810C	/* response */
#define REM_RCVR_CNG	    0x810E	/* response */
#define REM_RCVR_CNG_END    0x810F	/* response */
#define REM_BEACONING	    0x8110	/* response */
#define REM_BEACONEND	    0x8111	/* response */
#define REM_LAST	    0x8112
  
#define RPS_REQ_STATUS	    0x8201	/* command  */
#define RPS_REP_STATUS	    0x8202	/* response */
#define RPS_ERROR	    0x8203	/* response */
#define RPS_REP_STATION	    0x8206	/* response */
#define RPS_LAST	    0x8207
  
#define CRS_REP_NEWMON	    0x8301	/* response */
#define CRS_REP_NAUN_CHG    0x8302	/* response */
#define CRS_REP_XMIT_FWD    0x8303	/* response */
#define CRS_REQ_STA_INFO    0x8304	/* command  */
#define CRS_REP_STA_INFO    0x8306	/* response */
#define CRS_SET_STATION	    0x8307	/* command  */
#define CRS_STATION_SET	    0x8308	/* response */
#define CRS_REMOVE_STA	    0x8309	/* command  */
#define CRS_STA_REMOVED	    0x830A	/* response */
#define CRS_ERROR	    0x830B	/* response */
#define CRS_LAST	    0x830C
  
#define CNV_PRESENT	    0x8402	/* response */
#define CNV_BEACONING	    0x8403	/* response */
#define CNV_LAST	    0x8404
  
#define LBS_REQ_STATUS	    0x8501	/* command  */
#define LBS_REP_STATUS	    0x8502	/* response */
#define LBS_SET_PARAM	    0x8503	/* command  */
#define LBS_PARAM_SET	    0x8504	/* response */
#define LBS_ERROR	    0x8505	/* response */
#define LBS_PARAM_CHANGED   0x8506	/* response */
#define LBS_PERF_THRESH	    0x8507	/* response */
#define LBS_TRACE_REP	    0x8508	/* response */
#define LBS_COUNTER_REP	    0x8509	/* response */
#define LBS_SPAN_STATUS	    0x850A	/* response */
#define LBS_LAST	    0x850B

#define LRM_PARSE_ERROR	    0x8600	/* response */
#define LRM_REQ_STATUS	    0x8601	/* command  */
#define LRM_REP_STATUS	    0x8602	/* response */
#define LRM_SET_PARAM	    0x8603	/* command  */
#define LRM_PARAM_SET	    0x8604	/* response */
#define LRM_ERROR	    0x8605	/* response */
#define LRM_PARAM_CHANGED   0x8606	/* response */
#define LRM_SET_REP_PNT	    0x8607	/* command  */
#define LRM_MGR_ACCEPT	    0x8608	/* response */
#define LRM_MGR_REJECT	    0x8609	/* response */
#define LRM_REP_MGR_REJECT  0x860A	/* response */
#define LRM_REP_NEW_LINK    0x860B	/* response */
#define LRM_REP_CNTL_SHIFT  0x860C	/* response */
#define LRM_REP_CNTL_BREACH 0x860D	/* response */
#define LRM_CLOSE_LINK	    0x860E	/* command  */
#define LRM_TERMINATING	    0x860F	/* response */
#define LRM_INVALID	    0x8610	/* response */
#define LRM_SET_REP_ERR	    0x8611	/* response */
#define LRM_CONGESTION	    0x8612	/* response */
#define LRM_SRVRS_PRESENT   0x8613	/* response */
#define LRM_LAST	    0x8614	/* response */

#define ALERT_TRANS	    0x8701
#define ALERT_TRANS_REC	    0x8702
#define ALERT_LAST	    0x8703

/**********************************************************************
 *
 *			  Global Sub-Vectors
 *
 **********************************************************************/
#define IBMNM_CORRELATOR    0x4001
#define IBMNM_ADDRESS	    0x4002
#define IBMNM_NAUN	    0x4003
#define IBMNM_LOCATION	    0x4004
#define IBMNM_RING	    0x4005
#define IBMNM_ERROR	    0x4007
#define IBMNM_ALL	    0x4008
#define IBMNM_ERRCODE	    0x4009
#define IBMNM_ERROFFSET	    0x400A
#define IBMNM_PRODUCTID	    0x400B
#define IBMNM_VERSION	    0x400C
#define   VERSIONLEN        14	  
#define   BRIDGEVERSIONLEN  10	  
#define IBMNM_PERMANENT	    0x4012
#define   PERMANENT         0


/**********************************************************************
 *
 *			REM Local Sub-Vectors
 * 
 * 			 REM Data Structures
 *
 **********************************************************************/
#define REM_SV_NOTIF_ENABLE	  0xC001
#define   ENA_NOT_WEIGHT	    0x8000
#define   ENA_NOT_PREWEIGHT	    0x4000
#define   ENA_NOT_NONISO_THRESH	    0x0800
#define   ENA_NOT_FORWARD	    0x0400
#define   ENA_INT_RING		    0x0040
#define   ENA_INT_AUTO		    0x0020
#define REM_SV_NONISO_NOTIF	  0x8003
#define REM_SV_INTEN_DATA	  0x8004
#define REM_SV_ISO_TABLE	  0xC006
#define REM_SV_ISOL_STATUS	  0x800A
#define REM_SV_FAULT_DOM	  0x800B
#define REM_SV_RESET		  0xC00E
#define REM_SV_SOFT_ERR_REPT	  0x8010
#define REM_SV_NNIF_REPT	  0x8011
#define REM_SV_MON_ERR_REPT	  0x8012
#define REM_SV_RING_STATUS	  0x8016
#define REM_SV_RING_STATUS_A	  0xC016
#define REM_SV_BEACON_DATA	  0x8018
#define REM_SV_LOST_FRAMES	  0xC01B
#define REM_SV_RCVR_CONGESTION	  0xC01C
#define REM_SV_FC_ERRORS	  0xC01D
#define REM_SV_FREQ_ERRORS	  0xC01E
#define REM_SV_TOKEN_ERRORS	  0xC01F
#define REM_SV_RESERVED_ERRORS	  0xC020
#define REM_SV_TABLE_FULL_ERRORS  0xC021
#define REM_SV_MIN_DEC_ERRORS	  0xC022
#define REM_SV_DEC_INTERVL	  0xC023
#define REM_SV_BEACON_TYPE	  0xC024
#define REM_SV_RING_STATE	  0xC026
#define   RS_NORMAL		    0x0000
#define   RS_TMP_BCN_PREMPT	    0x0001
#define   RS_TMP_BCN_UNIMP	    0x0002
#define   RS_TMP_BCN_BOTH_PRES	    0x0003
#define   RS_TMP_BCN_ONE_PRES	    0x0004
#define   RS_TMP_BCN_NONE_PRES	    0x0005
#define   RS_PRM_BCN		    0x0006
#define REM_SV_ENTRIES		  0xC028
#define REM_SV_RECEIVER		  0x8029
#define REM_SV_TRANSMITTER	  0x802A
#define REM_SV_RING_INTEN	  0xC02C
#define REM_SV_AUTO_INTEN	  0xC02D
#define   REM_MM_I_LINE	     0x8000
#define   REM_MM_I_INTERNAL  0x4000
#define   REM_MM_I_BURST     0x2000
#define   REM_MM_I_AC	     0x1000
#define   REM_MM_I_ABORTS    0x0800
#define   REM_MM_N_LOSTFRAME 0x0080
#define   REM_MM_N_RCVRCONG  0x0040
#define   REM_MM_N_FC        0x0020
#define   REM_MM_N_FREQ      0x0010
#define   REM_MM_N_TOKEN     0x0008
#define REM_SV_FLAGS		  0xC02F
#define REM_SV_WEIGHT		  0xC030
#define REM_SV_RCVR_CONG_TBL_FULL 0xC031
#define REM_SV_ISO_COUNTS	  0xC032
#define REM_SV_NONISO_COUNTS	  0xC033

/*
 * Single bit indicators for subvectors.  These are use to build required
 * and optional subvector indicatior bytes for major vectors.
 */
#define REM_B_CORRELATOR	0x00000001
#define REM_B_RING		0x00000002
#define REM_B_NOTIF_ENABLE	0x00000004
#define REM_B_INTEN_DATA	0x00000008
#define REM_B_ISOLATING_TABLE	0x00000010
#define REM_B_RING_STATUS_A	0x00000020
#define REM_B_RESET		0x00000040

#define REM_B_ID_RING		0x00000080
#define REM_B_ID_AUTO		0x00000100

#define REM_B_TOP_MASK		0x0000007f
#define REM_B_ID_MASK		0x00000180

/*
 * Required and Optional indicators.
 */
#define REM_R_REQ_STATUS	REM_B_CORRELATOR | REM_B_RING
#define REM_O_REQ_STATUS	REM_B_NOTIF_ENABLE | REM_B_INTEN_DATA | \
				REM_B_ID_RING | REM_B_ID_AUTO | \
				REM_B_ISOLATING_TABLE  | REM_B_RING_STATUS_A
#define REM_R_SET_PARAM		REM_B_CORRELATOR | REM_B_RING
#define REM_O_SET_PARAM		REM_B_NOTIF_ENABLE | REM_B_INTEN_DATA | \
				REM_B_ID_RING | REM_B_ID_AUTO | REM_B_RESET
/*
 * Data structures for parsing a REM frame.
 */
typedef struct rem_pb_ {
    ushort  *correlator;
    ushort  *ring;
    ushort  *notification;
    boolean intensive_mode;
    ushort  *id_ring;
    ushort  *id_auto;
    ushort  *iso_table;
    ushort  *ring_status;
    boolean reset;
} rem_pb;

/*
 * Data structures for a REM responses frame.
 */
typedef struct iso_tbl_header_ {
    uchar pad1;
    uchar num_err;
    uchar head_err;
    uchar head_free;
} iso_tbl_header;

typedef struct iso_tbl_entry_ {
    uchar next;
    uchar pad1;
    uchar threshold;
    uchar softerr_count;
    uchar address[IEEEBYTES];
    ulong PACKED(location);
    uchar naun_address[IEEEBYTES];
    uchar decay;
    uchar pad2;
    uchar pad3;
    uchar pad4;
} iso_tbl_entry;

typedef struct iso_tbl_vector_ {
    iso_tbl_header header;
    iso_tbl_entry entry[15];
} iso_tbl_vector;

#define ISO_T_WEIGHT    0x80
#define ISO_T_PREWEIGHT 0x40


/**********************************************************************
 *
 *			RPS Local Sub-Vectors
 *
 * 			 RPS Data Structures
 *
 **********************************************************************/
#define RPS_SV_TIMER		0xC001
#define RPS_SV_UCODE		0xC002
#define RPS_SV_STATUS		0xC004
#define   RPS_STAT_OK       0
#define   RPS_STAT_NACK     1
#define   RPS_STAT_NO_RESP  2

/*
 * Single bit indicators for subvectors.  These are use to build required
 * and optional subvector indicatior bytes for major vectors.
 */
#define RPS_B_NONE       	0x00000000
#define RPS_B_CORRELATOR	0x00000001
#define RPS_B_RING		0x00000002
#define RPS_B_ALL		0x00000004

/*
 * Required and Optional indicators.
 */
#define RPS_R_REQ_STATUS        RPS_B_CORRELATOR | RPS_B_RING | RPS_B_ALL
#define RPS_O_REQ_STATUS	RPS_B_NONE

/*
 * Data structures for parsing.
 */
typedef struct rps_pb_ {
    ushort *correlator;
    ushort *ring;
    ushort *all;
} rps_pb;

/**********************************************************************
 *
 *			CRS Local Sub-Vectors
 *
 * 			 CRS Data Structures
 *
 **********************************************************************/
#define CRS_SV_STATUS		0xC001
#define   CRS_STAT_REMOVED	    0
#define   CRS_STAT_NOT_REMOVED	    1
#define CRS_SV_ADDRESS_INFO	0x8002
#define CRS_SV_ADDRESS_INFO_A	0xC002
#define CRS_SV_STATE_INFO	0x8003
#define CRS_SV_STATE_INFO_A	0xC003
#define CRS_SV_ATTACH_INFO	0x8004
#define CRS_SV_ATTACH_INFO_A    0xC004
#define CRS_SV_STATION_ERR	0x8005
#define CRS_SV_GROUP		0xC00D
#define CRS_SV_FUNCTIONAL	0xC00F
#define CRS_SV_UCODE_LEVEL	0xC010
#define CRS_SV_STA_STATUS	0xC011
#define CRS_SV_UNIQUE_ID	0xC012
#define CRS_SV_ENABLED_FUNC	0xC013
#define CRS_SV_ALLOW_PRIO	0xC014
#define CRS_SV_REASON		0xC015
#define   CRS_REASON_NOT_RECOG	    0
#define   CRS_REASON_NOT_COPIED	    1
#define   CRS_REASON_TIMEOUT	    2
#define   CRS_REASON_NACK	    3
#define   CRS_REASON_BEACONING	    4
#define   CRS_REASON_NONE         255
#define CRS_SV_RESPONSE		0xC016
#define CRS_SV_SOFTERR		0xC017
#define CRS_SV_STATUSCODE	0xC019
#define CRS_SV_LOCALRING	0xC01A

/*
 * Single bit indicators for subvectors.  These are use to build required
 * and optional subvector indicatior bytes for major vectors.
 */
#define CRS_B_NONE       	0x00000000
#define CRS_B_CORRELATOR	0x00000001
#define CRS_B_ADDRESS		0x00000002
#define CRS_B_RING		0x00000004
#define CRS_B_ALL		0x00000008
#define CRS_B_LOCATION  	0x00000010

#define CRS_B_ADDRESS_INFO      0x00000020
#define CRS_B_STATE_INFO	0x00000040
#define CRS_B_ATTACH_INFO	0x00000080
#define CRS_B_SOFTERR	  	0x00000100
#define CRS_B_ENABLED_FUNC  	0x00000200
#define CRS_B_ALLOW_PRIO  	0x00000400
#define CRS_B_LOCALRING   	0x00000800

/*
 * Required and Optional indicators.
 */
#define CRS_R_REQ_STA_INFO      CRS_B_CORRELATOR | CRS_B_ADDRESS | CRS_B_RING
#define CRS_O_REQ_STA_INFO	CRS_B_ALL | CRS_B_ADDRESS_INFO | \
				CRS_B_STATE_INFO | CRS_B_ATTACH_INFO
#define CRS_R_SET_STATION	CRS_B_CORRELATOR | CRS_B_ADDRESS | CRS_B_RING
#define CRS_O_SET_STATION	CRS_B_LOCATION | CRS_B_LOCALRING | \
				CRS_B_SOFTERR | CRS_B_ENABLED_FUNC | \
				CRS_B_ALLOW_PRIO
#define CRS_R_REMOVE_STA	CRS_B_CORRELATOR | CRS_B_ADDRESS | CRS_B_RING
#define CRS_O_REMOVE_STA	CRS_B_NONE

/*
 * Data structures for parsing a CRS frame.
 */
typedef struct crs_pb_ {
    ushort *correlator;
    uchar  *address;
    ushort *ring;
    ushort *all;
    ulong  *location;
    uchar  *address_info;
    uchar  *state_info;
    uchar  *attach_info;
    ulong  *enabled_func;
    ushort *allow_prio;
    ushort *softerr;
    ushort *localring;
} crs_pb;

/*
 * Data that must be maintained from a CRS request frame until the MAC
 * response(s) arrives from the queried station..
 */
struct crs_data_ {
    bridge_info *bridge;
    int     link;
    ushort  correlator;
    boolean all;
    boolean address_info;
    boolean state_info;
    boolean attach_info;
    boolean address_done;
    boolean state_done;
    boolean attach_done;
};

/**********************************************************************
 *
 *			LBS Local Sub-Vectors
 *
 * 			 LBS Data Structures
 *
 **********************************************************************/
#define LBS_SV_TYPE		0xC001
#define   BRIDGE_TR_TR             0x0001
#define   BRIDGE_ETHER_ETHER	   0x0002
#define   BRIDGE_ETHER_TR          0x0003
#define   BRIDGE_LOCAL             0x0000
#define   BRIDGE_REMOTE            0x8000
#define LBS_SV_PORTS		0xC002
#define LBS_SV_PORT_INFO	0x8003
#define LBS_SV_PORT_TYPE	0xC004
#define   PORT_RING_STATION	   0x0001
#define LBS_SV_RING_STATUS	0xC005
#define   RING_OPERATIONAL         0x00
#define   RING_BEACONING           0x02
#define LBS_SV_ADAPTER_STATUS	0xC006
#define   ADAPTER_ATTACHED         0x0000
#define   ADAPTER_NOT_ATTACHED     0x0001
#define LBS_SV_RTE_ACTIVE	0xC007
#define LBS_SV_FRAME_DISC	0xC009
#define LBS_SV_LOSS_THRESH	0xC00A		/* in percent */
#define LBS_SV_B_FR_TRANS	0xC00B
#define LBS_SV_NOTIF_INTERVAL	0xC00C		/* in seconds */
#define LBS_SV_ROUTE_STATUS	0x800D
#define LBS_SV_ROUTE_IDENT	0x800E
#define LBS_SV_PORT_IDENT1	0xC00F
#define LBS_SV_PORT_IDENT2	0xC010
#define LBS_SV_BR_IDENT		0xC011
#define LBS_SV_SRTE_ENABLED	0xC012
#define LBS_SV_HOP_COUNT	0xC013
#define LBS_SV_LARGEST_FR	0xC015
#define LBS_SV_FORW_ADDR	0xC016
#define LBS_SV_FORW_LEN		0xC017
#define LBS_SV_FORW_DATA	0xC018
#define LBS_SV_PERC_FR_LOST	0xC020
#define LBS_SV_PART_BITS	0xC021
#define LBS_SV_FR_NOT_REC	0xC022
#define LBS_SV_T_FR_DISC	0xC023
#define LBS_SV_FR_NOT_FORW	0xC024
#define LBS_SV_T_B_FR_TRANS	0xC025
#define LBS_SV_PATH_TRACE	0xC026
#define LBS_SV_B_BY_TRANS	0xC027
#define LBS_SV_T_NB_FR_TRANS	0xC028
#define LBS_SV_NB_FR_TRANS	0xC029
#define LBS_SV_T_FR_NOT_REC	0xC02A
#define LBS_SV_NB_BY_TRANS	0xC02B
#define LBS_SV_T_FR_NOT_FORW	0xC02C
#define LBS_SV_FORW_STRIP	0xC02D
#define LBS_SV_INTERNAL_STATUS	0xC02E
#define LBS_SV_CALC_INT		0xC02F		/* in seconds */
#define LBS_SV_T_B_BY_TRANS	0xC030
#define LBS_SV_T_NB_BY_TRANS	0xC031
#define LBS_SV_FRAME_DISC_L	0xC032
#define LBS_SV_FR_NOT_REC_L	0xC033
#define LBS_SV_FR_NOT_FORW_L	0xC034
#define LBS_SV_DATA_RATE	0xC035
#define   DATA_RATE_2		  0x0002
#define   DATA_RATE_4		  0x0004
#define   DATA_RATE_16		  0x0010
#define LBS_SV_FR_DISC_INT	0xC036
#define LBS_SV_T_FR_DISC_INT	0xC037
#define LBS_SV_SRTE_MODE	0xC041
#define LBS_SV_BY_DISC_INT	0xC042
#define LBS_SV_ALL_LONG		0xC043
#define LBS_SV_FR_NOT_ROUTED	0xC044
#define LBS_SV_T_FR_NOT_ROUTED	0xC045

/*
 * Single bit indicators for subvectors.  These are use to build required
 * and optional subvector indicatior bytes for major vectors.
 */
#define LBS_B_CORRELATOR	0x00000001
#define LBS_B_ALL		0x00000002
#define LBS_B_TEMP		0x00000004
#define LBS_B_ALL_LONG		0x00000008
#define LBS_B_NOTIF_INTERVAL	0x00000010
#define LBS_B_LOSS_THRESH	0x00000020
#define LBS_B_PORT_INFO		0x00000040
#define LBS_B_ROUTE_STATUS	0x00000080
#define LBS_B_INTERNAL_STATUS	0x00000100

#define LBS_B_PORT_IDENT	0x00000200
#define LBS_B_SRTE_ENABLED	0x00000400
#define LBS_B_HOP_COUNT		0x00000800

#define LBS_B_ROUTE_IDENT	0x00001000
#define LBS_B_RTE_ACTIVE	0x00002000
#define LBS_B_BR_IDENT		0x00004000
#define LBS_B_SRTE_MODE		0x00008000
#define LBS_B_LARGEST_FR	0x00010000

#define LBS_B_PORT_IDENT1	0x00020000
#define LBS_B_PORT_IDENT2	0x00040000

#define LBS_B_TOP_MASK	        0x000001ff
#define LBS_B_PORT_MASK	        0x00000e00
#define LBS_B_ROUTE_MASK        0x0001f000
#define LBS_B_ROUTEID_MASK      0x00060000

#define LBS_B_SRTE_MUTEXC	0x00008400

/*
 * Required and Optional indicators.
 */
#define LBS_R_REQ_STATUS        LBS_B_CORRELATOR
#define LBS_O_REQ_STATUS	LBS_B_ALL | LBS_B_ALL_LONG

#define LBS_R_SET_PARAM         LBS_B_CORRELATOR
#define LBS_O_SET_PARAM		LBS_B_PORT_IDENT | LBS_B_SRTE_ENABLED | \
				LBS_B_HOP_COUNT | LBS_B_ROUTE_IDENT | \
				LBS_B_PORT_IDENT1 | LBS_B_PORT_IDENT2 | \
				LBS_B_SRTE_MODE | LBS_B_NOTIF_INTERVAL | \
				LBS_B_LOSS_THRESH | LBS_B_PORT_INFO | \
				LBS_B_ROUTE_STATUS | LBS_B_INTERNAL_STATUS | \
				LBS_B_TEMP | LBS_B_RTE_ACTIVE | LBS_B_BR_IDENT

/*
 * Output bits for various response frames.
 */
#define LBS_PI_RING	      0x0001
#define LBS_PI_TYPE	      0x0002
#define LBS_PI_STATUS	      0x0004
#define LBS_PI_HOP_COUNT      0x0008
#define LBS_PI_SET1	      0x0010
#define LBS_PI_SET2	      0x0020
#define LBS_PI_SRTE_ENA	      0x0040
#define LBS_PI_COUNTERS_LONG  0x0080
#define LBS_PI_COUNTERS_SHORT 0x0100


#define LBS_PI_REP_STATUS     LBS_PI_RING | LBS_PI_TYPE | LBS_PI_STATUS | \
			      LBS_PI_HOP_COUNT | LBS_PI_SET1 | LBS_PI_SRTE_ENA
#define LBS_PI_REP_STATUS_A   LBS_PI_RING | LBS_PI_TYPE | LBS_PI_STATUS | \
			      LBS_PI_HOP_COUNT | LBS_PI_SET1 | LBS_PI_SRTE_ENA | \
			      LBS_PI_COUNTERS_SHORT
#define LBS_PI_REP_STATUS_AL  LBS_PI_RING | LBS_PI_TYPE | LBS_PI_STATUS | \
			      LBS_PI_HOP_COUNT | LBS_PI_SET1 | LBS_PI_SRTE_ENA | \
			      LBS_PI_COUNTERS_LONG
#define LBS_PI_SET_PARAM      LBS_PI_HOP_COUNT | LBS_PI_SRTE_ENA 
#define LBS_PI_PARAM_CHANGED  LBS_PI_HOP_COUNT | LBS_PI_SRTE_ENA
#define LBS_PI_PERF_THRESH    LBS_PI_RING | LBS_PI_SET2
#define LBS_PI_TRACE_REP      LBS_PI_STATUS | LBS_PI_HOP_COUNT
#define LBS_PI_COUNTER_REP    LBS_PI_RING | LBS_PI_SET1 | \
			      LBS_PI_COUNTERS_SHORT | LBS_PI_COUNTERS_LONG
#define LBS_PI_SPAN_STATUS    LBS_PI_SRTE_ENA

#define LBS_RTE_REP_STATUS    LBS_B_RTE_ACTIVE | LBS_B_BR_IDENT | \
			      LBS_B_SRTE_MODE | LBS_B_LARGEST_FR
#define LBS_RTE_SET_PARAM     LBS_B_RTE_ACTIVE | LBS_B_BR_IDENT | LBS_B_SRTE_MODE
#define LBS_RTE_PARAM_CHANGED LBS_B_RTE_ACTIVE | LBS_B_BR_IDENT | LBS_B_SRTE_MODE
#define LBS_RTE_TRACE_REP     LBS_B_RTE_ACTIVE | LBS_B_BR_IDENT | LBS_B_LARGEST_FR

/*
 * Data structures for parsing a LBS frame.
 */
typedef struct lbs_pi_pb_ {
    uchar *port_id;		/* MAC address */
    uchar *hopcount;		/*     byte    */
    uchar *srte_enabled;	/*     byte    */
} lbs_pi_pb;

typedef struct lbs_rte_pb_ {
    uchar  *port_id1;		/* MAC address */
    uchar  *port_id2;		/* MAC address */
    ushort *bridge_id;
    uchar  *route_active;	/*     byte    */
    uchar  *srte_mode;	 	/*     byte    */   
} lbs_rte_pb;

typedef struct lbs_pb_ {
    ushort *correlator;
    uchar  *address;
    ushort *ring;
    ushort *all;
    ushort *all_long;
    ushort *interval;
    ushort *thresh;
    ushort *in_stats;
    uchar  *temp;
    int     num_ports;
    int     num_routes;
    lbs_pi_pb  pt[2];
    lbs_rte_pb rte[2];
} lbs_pb;

/**********************************************************************
 *
 *			LRM Local Sub-Vectors
 *
 * 			 LRM Data Structures
 *
 **********************************************************************/
#define LRM_SV_ERROR_MVID	0xC002
#define LRM_SV_NUM_ALT_LM	0xC004
#define LRM_SV_LINK_INFO	0x8005
#define LRM_SV_LINK_ID	 	0xC006
#define LRM_SV_PORT_INFO	0X8007
#define LRM_SV_REP_CLASS	0xC008
#define LRM_SV_AVAIL_CLASS	0xC009
#define LRM_SV_ACTIVE_CLASS	0xC00A
#define LRM_SV_REJECT_CODE	0xC00B
#define   LRM_REJ_BAD_KEY      1
#define   LRM_REJ_INUSE        2
#define   LRM_REJ_BAD_LINKNO   3
#define   LRM_REJ_CANT_ESTAB   4
#define   LRM_REJ_CROSSES_ME   5
#define LRM_SV_KEY		0xC00B
#define LRM_SV_NEW_KEY	 	0xC00C
#define LRM_SV_ROUTING	 	0xC00C
#define LRM_SV_STATUS_CODE	0xC00D
#define LRM_SV_CNTL_REASON	0xC00E
#define	  LRM_CNTL_CLOSE       0
#define	  LRM_CNTL_ABORT       1
#define	  LRM_CNTL_NEW         2
#define LRM_SV_PARSE_REASON	0xC00F
#define	  LRM_PARSE_MISSING    0
#define	  LRM_PARSE_UNKNOWN    1
#define	  LRM_PARSE_BAD_LEN    2
#define LRM_SV_SETREPPNT_REASON	0xC00F
#define	  LRM_SETREPPNT_ERROR  0
#define	  LRM_SETREPPNT_BADLEN 1
#define LRM_SV_TERM_REASON	0xC010
#define   LRM_TERM_NORMAL      0
#define   LRM_TERM_RING_ERR    1
#define   LRM_TERM_REMOVED     2
#define   LRM_TERM_INTERN_ERR  3
#define   LRM_TERM_LINE_FAIL   4
#define LRM_SV_ENAB_FUNC	0xC011
#define LRM_SV_INVALID_REASON	0xC012
#define   LRM_INVALID_NOT_AVAIL    0x0000
#define   LRM_INVALID_NOT_ACTIVE   0x0001
#define   LRM_INVALID_BAD_STATE    0x0002

/*
 * Single bit indicators for subvectors.  These are use to build required
 * and optional subvector indicatior bytes for major vectors.
 */
#define LRM_B_CORRELATOR	0x00000001
#define LRM_B_ALL		0x00000002
#define LRM_B_PORT		0x00000004
#define LRM_B_RING		0x00000008
#define	LRM_B_NUM_ALT		0x00000010
#define LRM_B_TEMP		0x00000020
#define LRM_B_ROUTING		0x00000040
#define LRM_B_RLI		0x00000080
#define LRM_B_RLI_ID		0x00000100
#define LRM_B_RLI_KEY		0x00000200
#define LRM_B_RLI_NEWKEY	0x00000400
#define LRM_B_RLI_PI		0x00000800
#define LRM_B_RLI_PI_ID		0x00001000
#define LRM_B_RLI_PI_CLASS	0x00002000
#define LRM_B_RLI_PI_ENA	0x00004000
     
#define LRM_B_TOP_MASK		0x000000ff
#define LRM_B_RLI_MASK		0x00000f00
#define LRM_B_RLI_PI_MASK	0x00007000

/*
 * Required and Optional indicators.
 */
#define LRM_R_REQ_STATUS	LRM_B_CORRELATOR
#define LRM_O_REQ_STATUS	LRM_B_ALL
#define LRM_R_SET_PARAM		LRM_B_CORRELATOR | LRM_B_RLI_ID | LRM_B_RLI_PI_ID
#define LRM_O_SET_PARAM		LRM_B_PORT | LRM_B_RING | LRM_B_NUM_ALT | \
				LRM_B_TEMP | LRM_B_RLI | LRM_B_RLI_NEWKEY | \
				LRM_B_RLI_PI | LRM_B_RLI_PI_CLASS | LRM_B_RLI_PI_ENA
#define LRM_R_SET_REP_PNT	LRM_B_CORRELATOR | LRM_B_ROUTING | LRM_B_RLI | \
				LRM_B_RLI_ID| LRM_B_RLI_KEY | LRM_B_RLI_PI | \
				LRM_B_RLI_PI_ID | LRM_B_RLI_PI_CLASS
#define LRM_O_SET_REP_PNT	LRM_B_NUM_ALT
#define LRM_R_CLOSE_LINK	NULL
#define LRM_R_CLOSE_LINK	NULL

typedef struct lrm_pi_pb_ {
    uchar *portid;
    ulong *repclass;
    ulong *enabled;
} lrm_pi_pb;

/*
 * Data structures for parsing a LRM frame.
 */
typedef struct lrm_pb_ {
    ushort *correlator;
    ushort *all;
    uchar  *port;
    ushort *ring;
    ushort *numalt;
    ushort *linkid;
    uchar  *routing;
    char   *key;
    char   *newkey;
    uchar  *temp;
    int	   numports;
    lrm_pi_pb ports[0];
} lrm_pb;

#define LRM_PI_RINGNO		0x0001
#define LRM_PI_REP_CLASS	0x0002
#define LRM_PI_FUNCTIONAL	0x0004
#define LRM_PI_REST		0x0008
#define LRM_PI_ALL		0x000F

#define LRM_RLI_NONE		0x0000
#define LRM_RLI_LINKNO		0x0010
#define LRM_RLI_ADDRESS		0x0020
#define LRM_RLI_KEY		0x0040
#define LRM_RLI_NEWKEY		0x0080

#define LRM_RLI_REP_STATUS	LRM_RLI_LINKNO | LRM_RLI_ADDRESS
#define LRM_RLI_PARAM_CHANGED	LRM_RLI_LINKNO | LRM_RLI_NEWKEY | \
				LRM_PI_REP_CLASS | LRM_PI_FUNCTIONAL
#define LRM_RLI_MGR_ACCEPT	LRM_RLI_LINKNO | LRM_PI_ALL
#define LRM_RLI_MGR_REJECT	LRM_RLI_LINKNO | LRM_PI_ALL
#define LRM_RLI_NEW_LINK	LRM_RLI_LINKNO | LRM_RLI_ADDRESS
#define LRM_RLI_CNTL_SHIFT	LRM_RLI_LINKNO | LRM_RLI_ADDRESS
#define LRM_RLI_CNTL_BREACH	LRM_RLI_LINKNO | LRM_RLI_ADDRESS
#define LRM_RLI_INVALID		LRM_RLI_LINKNO | LRM_PI_ALL
#define LRM_RLI_SRVRS_PRESENT	LRM_RLI_NONE | LRM_PI_RINGNO

/*
 * LRM announcement of presence values
 */
#define LM_DEF_ANNOUNCE_COUNT 4
#define LM_DEF_ANNOUNCE_TIME  ONEMIN


/**********************************************************************
 *
 *			  Global Error Codes
 *
 **********************************************************************/
#define ERR_BAD_LEN                 0x0002
#define ERR_BAD_SV_VALUE	    0x0006
#define ERR_REQ_SV_MISSING	    0x0008
#define ERR_LEN_TOO_SMALL	    0x0009
#define ERR_UNEXP_DUP		    0x000a
#define ERR_EXECUTION		    0x000c

#define REQ_ERR_BAD_ATOMIC_LEN	    0x0003
#define REQ_ERR_NO_SV_ALLOWED	    0x0004

#define SET_ERR_TOO_HIGH	    0x0004
#define SET_ERR_TOO_LOW		    0x0005
#define SET_ERR_BAD_ATOMIC_SV_LEN   0x0007
#define SET_ERR_UNAUTH_REQ	    0x000b


/**********************************************************************
 *
 *			    Global Defines
 *
 **********************************************************************/
#define IBMNM_MAXBUF       80
#define OFFSET(a, b)	   ((uchar *)a - (uchar *)b)
#define next_subvector(sv) ((ibmnm_vector *)((int)(sv) + GETSHORT(&sv->length)))
#define lnmebuginf	   if (lnm_e_debug) buginf
#define lnmllcbuginf	   if (lnm_llc_debug) buginf
#define lnmmacbuginf	   if (lnm_mac_debug) buginf
#define lnmmacebuginf	   if (lnm_e_debug | lnm_mac_debug) buginf

/**********************************************************************
 *
 * Generic sub-vector inclusion routines.  These are used to build
 * Network Management Major Vectors to send to a Lan Manager.  These are
 * declared here as 'static inline' in hopes that all references will be
 * compiled directly into the code.
 **********************************************************************/
static inline ibmnm_vector *insert_major (
    paktype *pak,
    int command)
{
    ibmnm_vector *vector;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    PUTSHORT(&vector->command, command);
    return((ibmnm_vector *)&vector->data);
}

static inline ibmnm_vector *insert_header (
    ibmnm_vector *sv,
    int command)
{
    PUTSHORT(&sv->command, command);
    return((ibmnm_vector *)&sv->data);
}

static inline ibmnm_vector *insert_atomic (
    ibmnm_vector *sv,
    int command)
{
    PUTSHORT(&sv->command, command);
    PUTSHORT(&sv->length, 4);
    return(next_subvector(sv));
}

static inline ibmnm_vector *insert_byte (
    ibmnm_vector *sv,
    int command,
    uchar number)
{
    PUTSHORT(&sv->length, (4 + sizeof(uchar)));
    PUTSHORT(&sv->command, command);
    sv->data.d.byte[0] = number;
    return(next_subvector(sv));
}

static inline ibmnm_vector *insert_short (
    ibmnm_vector *sv,
    int command,
    ushort number)
{
    PUTSHORT(&sv->length, (4 + sizeof(ushort)));
    PUTSHORT(&sv->command, command);
    PUTSHORT(&sv->data.d.sword[0], number);
    return(next_subvector(sv));
}

static inline ibmnm_vector *insert_long (
    ibmnm_vector *sv,
    int command,
    ulong number)
{
    PUTSHORT(&sv->length, 4 + sizeof(ulong));
    PUTSHORT(&sv->command, command);
    PUTLONG(&sv->data, number);
    return(next_subvector(sv));
}

static inline ibmnm_vector *insert_longer (
    ibmnm_vector *sv,
    int command,
    ulong number)
{
    PUTSHORT(&sv->length, 4 + sizeof(ushort) + sizeof(ulong));
    PUTSHORT(&sv->command, command);
    PUTLONG(&sv->data.d.sword[0], 0);
    PUTLONG(&sv->data.d.sword[1], number);
    return(next_subvector(sv));
}

static inline ibmnm_vector *insert_macaddr (
    ibmnm_vector *sv,
    int command,
    uchar *address)
{
    PUTSHORT(&sv->length, (4 + IEEEBYTES));
    PUTSHORT(&sv->command, command);
    ieee_copy(address, (uchar *)&sv->data);
    return(next_subvector(sv));
}

static inline ibmnm_vector *insert_bytes (
    ibmnm_vector *sv,
    int command,
    uchar *address,
    int length)
{
    PUTSHORT(&sv->length, 4 + length);
    PUTSHORT(&sv->command, command);
    bcopy(address, (uchar *)&sv->data, length);
    return(next_subvector(sv));
}

static inline ibmnm_vector *insert_correlator (
    ibmnm_vector *sv,
    ushort correlator)
{
    return(insert_short(sv, IBMNM_CORRELATOR, correlator));
}

static inline ibmnm_vector *insert_address (
    ibmnm_vector *sv,
    uchar *address)
{
    return(insert_macaddr(sv, IBMNM_ADDRESS, address));
}

static inline ibmnm_vector *insert_naun (
    ibmnm_vector *sv,
    uchar *address)
{
    return(insert_macaddr(sv, IBMNM_NAUN, address));
}

static inline ibmnm_vector *insert_location (
    ibmnm_vector *sv,
    ulong location)
{
    return(insert_long(sv, IBMNM_LOCATION, location));
}

static inline ibmnm_vector *insert_ring (
    ibmnm_vector *sv,
    ushort ring)
{
    return(insert_short(sv, IBMNM_RING, ring));
}

static inline ibmnm_vector *insert_error (
    ibmnm_vector *sv,
    ushort error)
{
    return(insert_short(sv, IBMNM_ERROR, error));
}

static inline ibmnm_vector *insert_errcode (
    ibmnm_vector *sv,
    ushort errcode)
{
    return(insert_short(sv, IBMNM_ERRCODE, errcode));
}

static inline ibmnm_vector *insert_erroffset (
    ibmnm_vector *sv,
    ushort erroffset)
{
    return(insert_short(sv, IBMNM_ERROFFSET, erroffset));
}

static inline ibmnm_vector *insert_productid (
    ibmnm_vector *sv,
    uchar *product)
{
    return(insert_bytes(sv, IBMNM_PRODUCTID, product, IDLEN));
}

static inline ibmnm_vector *insert_version (
    ibmnm_vector *sv,
    uchar *version)
{
    return(insert_bytes(sv, IBMNM_VERSION, version, VERSIONLEN));
}

static inline void finish_vector(
    paktype *pak,
    ibmnm_vector *sv)
{
    ibmnm_vector *vector;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    PUTSHORT(&vector->length, (ushort)((uchar *)sv - (uchar *)vector));
}

static inline ibmnm_vector *finish_subvector(
    ibmnm_vector *sv,
    ibmnm_vector *sv2)
{
    PUTSHORT(&sv->length, (ushort)((uchar *)sv2 - (uchar *)sv));
    return(sv2);
}

/*
 *  Codes used when the Ring Manager has requested we modify ring parameters.
 * These codes were apparently only implemented for the Netronix token ring
 * card, now obsolete.  These definitions are from tramon/sca.h
 */
#define TR_MODIFY_NONE	0
#define TR_MODIFY_MAXRD	1
#define TR_MODIFY_BNUM	2
#define TR_MODIFY_RING	3
#define TR_MODIFY_TRING	4
#define TR_MODIFY_SPAN	5

/**********************************************************************
 *
 *                          Prototypes
 *
 **********************************************************************/
extern boolean       ibmnm_no_changes;
extern boolean       lanmgr_running;
extern boolean       lnm_e_debug;
extern boolean       lnm_llc_debug;
extern boolean       lnm_mac_debug;
extern queuetype     ibmnmQ;
extern queuetype     lm_bridgeQ;
#ifdef IBMNM_DEBUG
extern llctype       *test_cookie;
extern ulong         test_correlator;
#endif
extern const char respvec[];
#ifdef NOBODY_CALLS_ME
extern const char unimpvec[];
#endif /* NOBODY_CALLS_ME */
extern const char unknvec[];
extern const char unxpctvec[];
extern const char missvec[];
extern const char toosmallvec[];
extern const char allvec[];
extern const char badveclen[];
extern const char nolink[];
extern const char illegalui[];
extern const char lnm_cb[];

extern void ibmnm_llc2_send(llctype *cookie, paktype *pak, int len);
extern hwidbtype *valid_port(bridge_info *, uchar *);
extern boolean cookie_to_link(llctype *, bridge_info **, int *);
extern boolean crs_parse_vectors(ibmnm_vector *, ulong, ulong, crs_pb *, int *, ibmnm_vector **);
extern boolean ibmnm_changes_allowed(bridge_info *, int);
extern boolean ibmnm_parse_correlator(ibmnm_vector *, ushort *);
extern boolean ibmnm_parse_request_status(ibmnm_vector *, ushort *, int *, ibmnm_vector **);
extern boolean ibmnm_parse_set_request(ibmnm_vector *, ushort *, int *, ibmnm_vector **);
extern boolean ibmnm_sum_lengths(ibmnm_vector *, int *, ibmnm_vector **);
extern boolean interested_in(bridge_info *, int, hwidbtype *, int);
extern boolean lrm_parse_pi(ibmnm_vector *, ulong, ulong, lrm_pb *, int *, ibmnm_vector **);
extern boolean lrm_parse_rli(ibmnm_vector *, ulong, ulong, lrm_pb *, int *, ibmnm_vector **);
extern boolean lrm_parse_vectors(ibmnm_vector *, ulong, ulong, lrm_pb *, int *, ibmnm_vector **);
extern boolean rem_beacon_postmortem(ring_info *);
extern boolean rem_parse_imd(ibmnm_vector *, ulong, ulong, rem_pb *, int *, ibmnm_vector **);
extern boolean rem_parse_vectors(ibmnm_vector *, ulong, ulong, rem_pb *, int *, ibmnm_vector **);
extern boolean rem_report_status(bridge_info *, int, rem_pb *);
extern boolean rps_parse_vectors(ibmnm_vector *, ulong, ulong, rps_pb *, int *, ibmnm_vector **);
extern bridge_info *address_to_bridge(uchar *);
extern bridge_info *idb_to_bridge(hwidbtype *);
extern bridge_info *ring_to_bridge(ring_info *);
extern char *crsbit_to_string(int);
extern char *lrmbit_to_string(int);
extern char *rembit_to_string(int);
extern char *rpsbit_to_string(int);
extern paktype *ibmnm_getbuffer(int);
extern ibmnm_vector *insert_lrm_pi(ibmnm_vector *, bridge_info *, int, int);
extern ibmnm_vector *insert_lrm_pi_one_ring(ibmnm_vector *, bridge_info *, hwidbtype *, int, int);
extern ibmnm_vector *insert_rem_iso_table(ibmnm_vector *, ring_info *);
extern ibmnm_vector *insert_rem_ringstat(ibmnm_vector *, ring_info *);
extern ibmnm_vector *insert_rep_link(ibmnm_vector *, bridge_info *, int, int);
extern int crsid_to_bitmask(ushort);
extern int lrmid_to_bitmask(ushort);
extern int remid_to_bitmask(ushort);
extern int rpsid_to_bitmask(ushort);
extern sys_timestamp ibmnm_background(void);
extern void create_link(bridge_info *, int, llctype *, uchar *, hwidbtype *);
extern void crs_error_frame(bridge_info *, int, int, int, int);
extern void crs_got_info(station_info *, uchar);
extern void crs_parameters_set(station_info *, uchar, ulong);
extern void crs_report_naun_change(station_info *);
extern void crs_report_new_monitor(station_info *);
extern void crs_report_station(station_info *, uchar, ulong);
extern void crs_report_transmit_fwd(station_info *);
extern void crs_station_removed(station_info *, uchar, uchar, ulong);
extern void crs_success_fail(station_info *, uchar, boolean, ulong);
extern void destroy_link(bridge_info *, int);
extern void ibmnm_add_bridge(hwidbtype *);
extern void ibmnm_bad_input(llctype *, ibmnm_vector *);
extern void ibmnm_crs_input(llctype *, ibmnm_vector *);
extern void ibmnm_display_config(void);
extern void ibmnm_display_bridge(hwidbtype *, char *);
extern void ibmnm_enqueue(paktype *);
extern void ibmnm_init(void);
extern void ibmnm_leg_input(void);
extern void ibmnm_input(llctype *cookie, paktype *pak, lnm_t *);
extern void ibmnm_lbs_input(llctype *, ibmnm_vector *);
extern void ibmnm_print_bridge_rcvd_msg(srb_triplet_t *, char *,
					bridge_info *, int); 
extern void ibmnm_link_state_change(llctype *, int);
extern void ibmnm_lrm_input(llctype *, ibmnm_vector *);
extern void ibmnm_lrm_ui_input(paktype *, lnm_t *);
extern void ibmnm_lrm_conn_estab(lnm_t *);
extern void ibmnm_lrm_conn_fail(lnm_t *);
extern void ibmnm_reload_message(void);
extern void ibmnm_rem_input(llctype *, ibmnm_vector *);
extern void ibmnm_rps_input(llctype *, ibmnm_vector *);
extern void lbs_counter_report(bridge_info *);
extern void lbs_srte_bcast(hwidbtype *);
#ifdef NOBODY_CALLS_ME
extern void lbs_thresh_exceeded(bridge_info *);
#endif /* NOBODY_CALLS_ME */
extern void lrm_accept(bridge_info *, int, int);
extern void lrm_control_breach(bridge_info *, int);
extern void lrm_error_frame(bridge_info *, int, int, int, int);
extern void lrm_find_controlling(bridge_info *, ushort);
extern void lrm_invalid_request(llctype *, ushort, int);
extern void lrm_mgmt_servers_present(bridge_info *);
extern void lrm_new_reporting_link(bridge_info *, int);
extern void lrm_params_changed(bridge_info *, int, lrm_pb *);
extern void lrm_params_set(bridge_info *, int);
extern void lrm_parsing_error(bridge_info *, int, ushort, int);
extern void lrm_reject(paktype *, bridge_info *, int, int, int, lnm_t *);
extern void lrm_report_reject(bridge_info *, uchar *, ushort);
extern void lrm_report_status(bridge_info *, int, int);
extern void lrm_send(paktype *, bridge_info *, int, char *);
extern void lrm_send_all_but(paktype *, bridge_info *, int, char *);
extern void lrm_send_link(paktype *, bridge_info *, int, char *);
extern void lrm_send_ring(paktype *, hwidbtype *, char *);
extern void lrm_set_rep_error(paktype *, ushort, int, int, int, lnm_t *);
extern void lrm_terminating(bridge_info *, int);
extern void lrm_find_ui_server_addr(lnm_t *, uchar *);
extern void lrm_find_ui_client_addr(lnm_t *, uchar *);
extern void rem_congestion_xxx(station_info *, int);
extern void rem_error_frame(bridge_info *, int, int, int, int);
extern void rem_forward_mac_frame(station_info *, int, uchar *, uchar *);
extern void rem_noniso_thresh(hwidbtype *);
extern void rem_parameters_changed(bridge_info *, int, rem_pb *);
extern void rem_parameters_set(bridge_info *, int, ushort);
extern void rem_success_fail(station_info *, boolean);
extern void rem_temp_beaconing(ring_info *, uchar *, uchar *);
extern void rem_xxx_notification(ushort, station_info *);
extern void rps_error_frame(bridge_info *, int, int, int, int);
extern void rps_report_station(station_info *);
extern void rps_report_status(bridge_info *, int, rps_pb *);
extern void send_trace_report(paktype *, hwidbtype *, hwidbtype *);
extern void set_ibmnm_interfaces(hwidbtype *, boolean);
/* SRB AST support */
extern boolean lnm_srb_span_command(hwidbtype *, hwidbtype *, boolean);
extern void ibmnm_init_hwidb(hwidbtype *);
#ifdef IBMNM_DEBUG
extern void ibmnm_test(char *);
extern void tibmnm_crs(jmp_buf *);
extern void tibmnm_lbs(jmp_buf *);
extern void tibmnm_lrm(jmp_buf *);
extern void tibmnm_rem(jmp_buf *);
extern void tibmnm_rps(jmp_buf *);
extern void get_macaddr(jmp_buf *, char  *, uchar *, char  *);
extern void tibmnm_set_reporting_point(jmp_buf *, uchar *, hwidbtype *);
extern ibmnm_vector *tibmnm_add_arbitrary(jmp_buf *, ibmnm_vector *);
#endif
