/* $Id: atmsig_ie.h,v 3.2.56.5 1996/06/21 02:08:34 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_ie.h,v $
 *------------------------------------------------------------------
 *
 * A T M S I G _ I E . H
 *
 * Atm Signalling Information Element header file
 *
 * May 1993     Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_ie.h,v $
 * Revision 3.2.56.5  1996/06/21  02:08:34  jwjang
 * CSCdi60829:  unknow ie in connect messae
 *         also added txQ count to show sscop
 * Branch: California_branch
 *
 * Revision 3.2.56.4  1996/06/19  00:56:40  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.2.56.3  1996/05/09  14:11:08  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.56.2.2.3  1996/05/03  21:40:35  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * back out last change - confine fix to /lane.
 *
 * Revision 3.2.56.2.2.2  1996/05/03  18:27:20  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * 2 additional blli's for tr lane.  Bump max_blli by 2 and initialize
 * the extra 2.
 *
 * Revision 3.2.56.2.2.1  1996/04/27  06:33:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.56.2  1996/04/25  23:02:21  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.56.1  1996/04/23  04:47:42  grobelch
 * - When we initiate an SVC with no traffic parameters specified,
 *     use a default value of all-ones. Still recognize 1 as a
 *     default.
 *   - Use the correct id-value to specify the FWD/BWD max-burst
 *     for CLP0&1 in the UCR IE. Use B0/B1, the old/bad values
 *     were C0/C1.
 *   - Correct error in initiating an SVC using the forward SCR
 *     value for CLP0 in UCR
 *
 *   ** Also fix for CSCdi55344:
 *   - Signalling uses whole-cell rates in installing VCs/specifying
 *      map-class.
 * CSCdi55196:  ATMSIG: UserCellRate Information Element changes needed
 * Branch: California_branch
 *
 * Revision 3.2.76.1  1996/04/10  23:35:33  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2  1995/11/17  08:46:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/23  21:00:12  schrupp
 * CSCdi39217:  ATM Process UCR params in Setup message
 *
 * Revision 2.2  1995/08/01  18:41:33  percyk
 * CSCdi38028:  display cause byte as string.
 *
 * Revision 2.1  1995/06/07  20:11:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATMSIG_IE_H__
#define __ATMSIG_IE_H__ 


#ifndef V31
#define V31 1
#endif

/*
 * Information Element Header.
 * First part of every Information Element.
 */
typedef struct atmSig_ieHeader_ {
   uchar identifier;      /* opcode */
   uchar cii;             /* compatibility instruction indicator */
   uchar length[2];       /* IE contents length - excludes IE header */
} atmSig_ieHeader;

#define ATMSIG_IEHDRSIZE  4 

#define ATMSIG_IECODING_ITU  0       /* ITU-T standard */
#define ATMSIG_IECODING_NET  0x60    /* standard defined for the network */
#define ATMSIG_IECODING_MASK 0x60    /* IE coding standard bits mask */

/*
 * A T M   S i g n a l l i n g   I n f o r m a t i o n    E l e m e n t s
 */

/* Twenty-one Information Elements currently supported by the ATM Forum */
#define ATM_AAL_IE                              0x58
#define ATM_USERCELLRATE_IE                     0x59
#define ATM_BROADBAND_BEARERCAP_IE              0x5e
#define ATM_BROADBAND_HIGHLAYER_IE              0x5d
#define ATM_BROADBAND_LOWLAYER_IE               0x5f
#define ATM_CALLSTATE_IE                        0x14
#define ATM_CALLEDPARTY_NUMBER_IE               0x70
#define ATM_CALLEDPARTY_SUBADDR_IE              0x71
#define ATM_CALLINGPARTY_NUMBER_IE              0x6c
#define ATM_CALLINGPARTY_SUBADDR_IE             0x6d
#define ATM_CAUSE_IE                            0x08
#define ATM_CONNECTIONIDENTIFIER_IE             0x5a
#define ATM_QOS_IE                              0x5c
#define ATM_RESTARTINDICATOR_IE                 0x79
#define ATM_BROADBAND_SENDINGCMPLT_IE           0x62
#define ATM_TRANSIT_NTWRKSEL_IE                 0x78
#define ATM_ENDPOINT_REFERENCE_IE               0x54
#define ATM_ENDPOINT_STATE_IE                   0x55
#define ATM_BROADBAND_LCKNGSHFT_IE              0x60
#define ATM_BROADBAND_NON_LCKNGSHFT_IE          0x61
#define ATM_BBAND_REPEATINDICATOR_IE            0x63


#define ATM_RCV_AAL                             1 
#define ATM_RCV_USERCELLRATE                    2 
#define ATM_RCV_BROADBAND_BEARERCAP             3 
#define ATM_RCV_BROADBAND_HIGHLAYER             4 
#define ATM_RCV_BROADBAND_LOWLAYER              5 
#define ATM_RCV_CALLSTATE                       6 
#define ATM_RCV_CALLEDPARTY_NUMBER              7 
#define ATM_RCV_CALLEDPARTY_SUBADDR             8 
#define ATM_RCV_CALLINGPARTY_NUMBER             9 
#define ATM_RCV_CALLINGPARTY_SUBADDR           10 
#define ATM_RCV_CAUSE                          11 
#define ATM_RCV_CONNECTIONIDENTIFIER           12 
#define ATM_RCV_QOS                            13 
#define ATM_RCV_RESTARTINDICATOR               14 
#define ATM_RCV_BROADBAND_SENDINGCMPLT         15 
#define ATM_RCV_TRANSIT_NTWRKSEL               16 
#define ATM_RCV_ENDPOINT_REFERENCE             17 
#define ATM_RCV_ENDPOINT_STATE                 18 
#define ATM_RCV_REPEATINDICATOR_IE             19
#define ATM_RCV_INVALID_IE                     20


/*
 * Maximum sizes for Information Elements (includes IE header).
 */
#define ATM_CAUSE_MAXSIZE                      34
#define ATM_CALLSTATE_MAXSIZE                   5
#define ATM_ENDPTREF_MAXSIZE                    7
#define ATM_ENDPTSTATE_MAXSIZE                  5
#define ATM_AAL_MAXSIZE                        20
#define ATM_USERCELLRATE_MAXSIZE               30
#define ATM_CONNECTIONID_MAXSIZE                9
#define ATM_QOS_MAXSIZE                         6
#define ATM_BBAND_HILAYER_MAXSIZE              13
#define ATM_BBAND_BEARERCAP_MAXSIZE             7
#define ATM_BBAND_LOLAYER_MAXSIZE              17
#define ATM_BBAND_LOCKSHFT_MAXSIZE              5
#define ATM_BBAND_NONLOCKSHFT_MAXSIZE           5
#define ATM_SENDCMPLT_MAXSIZE                   5
#define ATM_BBAND_REPEATIND_MAXSIZE             5
#define ATM_CALLINGPARTY_MAXSIZE               26
#define ATM_CALLINGPARTY_SUBADDR_MAXSIZE       25
#define ATM_CALLEDPARTY_MAXSIZE                25
#define ATM_CALLEDPARTY_SUBADDR_MAXSIZE        25
#define ATM_TRANSIT_NTWRKSEL_MAXSIZE            8
#define ATM_RESTART_IND_MAXSIZE                 5
#define ATM_REPEAT_IND_MAXSIZE                  5


/*
 * AAL Information Element.
 * Part of a SETUP message.  Covers AAL Type 1, 3/4, 5 and X (User defined).
 */

/* AAL IE Type */
#define ATM_AALIE_TYPE1              0x01   /* Type 1 */
#define ATM_AALIE_TYPE34             0x03   /* Type 3/4 */
#define ATM_AALIE_TYPE5              0x05   /* Type 5 */
#define ATM_AALIE_TYPEX              0x10   /* Type X - User defined */
#define ATM_AALIE_TYPEINVLD          0xff   /* Invalid AAL Type */

/* Common Part Identifier */
#define ATM_AALIE_CPI                0x01

/* AAL IE Mode */
#define ATM_AALIE_MODE_MASK          0xf0
#define ATM_AALIE_MODE_MSG           0x01
#define ATM_AALIE_MODE_STREAM        0x02
#define ATM_AALIE_MODE_INVALID       0

/* AAL SDU Masks */
#define ATM_AALIE_SDU1_MASK          0x0f
#define ATM_AALIE_SDU2_MASK          0x7f
#define ATM_AALIE_SDU3_MASK          0x7c

/* AAL MID Masks */
#define ATM_AALIE_MID1_MASK          0x7f
#define ATM_AALIE_MID2_MASK          0x70

/* AAL Service Specific Part Identifier */
#define ATM_AALIE_SSPI               0x82

/* AAL Subtype */
#define ATM_AALIE_SUBTYPE_ID         0x83   
#define ATM_AALIE_SUBTYPE_NULL       0x00 
#define ATM_AALIE_SUBTYPE_VOICE      0x01    /* Voice-band based on 64kb/sec */
#define ATM_AALIE_SUBTYPE_SYNC       0x02    /* Circuit Emulation - Sync */
#define ATM_AALIE_SUBTYPE_ASYNC      0x03    /* Circuit Emulation - Async */
#define ATM_AALIE_SUBTYPE_AUDIO      0x04    /* High Quality Audio */
#define ATM_AALIE_SUBTYPE_VIDEO      0x05


/* AAL CBR Rate Identifier */
#define ATM_AALIE_CBR_IDENT          0x86
#define ATM_AALIE_CBR_64             0x01   /* 64 kbit/s */
#define ATM_AALIE_CBR_1544           0x04   /* 1544 kbit/s */
#define ATM_AALIE_CBR_6312           0x05   /* 6312 kbit/s */
#define ATM_AALIE_CBR_32064          0x06   /* 32064 kbit/s */
#define ATM_AALIE_CBR_44736          0x07   /* 44736 kbit/s */
#define ATM_AALIE_CBR_97728          0x08   /* 97728 kbit/s */
#define ATM_AALIE_CBR_2048           0x10   /* 2048 kbit/s (E1) */
#define ATM_AALIE_CBR_8448           0x11   /* 8448 kbit/s (E2) */
#define ATM_AALIE_CBR_34368          0x12   /* 34368 kbit/s (E3) */
#define ATM_AALIE_CBR_139264         0x13   /* 139264 kbit/s */
#define ATM_AALIE_CBR_N64            0x40   /* n x 64 kbit/s */
#define ATM_AALIE_CBR_N8             0x41   /* n x 8 kbit/s - UNI 3.1 only */

/* AAL Multiplier Identifier */
#define ATM_AALIE_MULTIPLIER_ID      0x87

/* AAL Clock Recovery Type */
#define ATM_AALIE_CRT_ID             0x84
#define ATM_AALIE_CRT_NULL           0x00   /* Null - derived from phy interface */
#define ATM_AALIE_CRT_SRTS           0x10   /* Sync Residual Time Stamp */
#define ATM_AALIE_CRT_ACR            0x40   /* Adaptive Clock Recovery */

/* AAL Error Correction Type */
#define ATM_AALIE_ECT_ID             0x85
#define ATM_AALIE_ECT_NULL           0x00
#define ATM_AALIE_ECT_FECI           0x01   /* Fwd Err Correction Interleaving */
#define ATM_AALIE_ECT_FECD           0x02   /* fwd err correction delay uni31 */

/* AAL Structured Data Transfer */
#define ATM_AALIE_SDT_ID             0x86
#define ATM_AALIE_SDT_NULL           0x00
#define ATM_AALIE_SDT                0x01   

/* Partially Filled Cells Identifier */
#define ATM_AALIE_PFC_ID             0x8B

/*
 * AAL Information Element - Type 1
 */
typedef struct atmSig_aalType1Ie_ {
   uchar aalType;                 /* AAL Type */
   uchar subtype_id;              /* Subtype Identifier */
   uchar subtype;                 /* Subtype value */
   uchar cbr_id;                  /* Constant Bit Rate Id */
   uchar cbr_rate;
   uchar multiplier_id;           /* Multiplier Id */
   ushort multiplier;
   uchar clkrcvry_id;             /* Clock Recovery */
   uchar clkrcvry_type;
   uchar error_corr_id;           /* Error Correction */
   uchar error_correction;
   uchar struct_data_id;          /* Structured Data Transfer */
   uchar struct_data_xfer;
   uchar partfilledcells_id;      /* Partially Filled Cells */
   uchar partially_filled_cells;  
} atmSig_aalType1Ie;


typedef struct atmSig_aalType1IeV31_ {
   uchar aalType;                 /* AAL Type */
   uchar subtype_id;              /* Subtype Identifier */
   uchar subtype;                 /* Subtype value */
   uchar cbr_id;                  /* Constant Bit Rate Id */
   uchar cbr_rate;
   uchar multiplier_id;           /* Multiplier Id */
   ushort multiplier;
   uchar clkrcvry_id;             /* Clock Recovery */
   uchar clkrcvry_type;
   uchar error_corr_id;           /* Error Correction */
   uchar error_correction;
   uchar struct_data_id;          /* Structured Data Transfer */
   ushort struct_data_xfer;
   uchar partfilledcells_id;      /* Partially Filled Cells */
   uchar partially_filled_cells;  
} atmSig_aalType1IeV31;


#define ATM_AALIE_FWD_SDUSZ_ID     0x8C
#define ATM_AALIE_BWD_SDUSZ_ID     0x81
#define ATM_AALIE_MID_SIZE_ID      0x12
#define ATM_AALIE_MODE_ID          0x83
#define ATM_AALIE_SSCSTYPE_ID      0x84

/* Mode */

/* SSCS Type */
#define ATM_AALIE_SSCS_NULL        0x00
#define ATM_AALIE_SSCS_ADATA       0x01    /* Assured data (SSCOP) */
#define ATM_AALIE_SSCS_UDATA       0x02    /* Unassured data (SSCOP) */
#define ATM_AALIE_SSCS_FRAMERELAY  0x04    /* Frame Relay SSCS */

/*
 * AAL Information Element - Type 3/4 
 */
typedef struct atmSig_aalType34Ie_ {
   uchar aalType;          /* AAL Type */
   uchar fwdSduSizeId;     /* fwd max cpcs-sdu id */
   uchar fwdSdu[2];
   uchar bwdSduSizeId;     /* bwd max cpcs-sdu id */
   uchar bwdSdu[2];
   uchar midSizeId_midRangeId;
   ushort midSize_lowestMid;
   uchar  modeId_highestMidHi;
   uchar mode_highestMidLo;
   uchar sscsTypeId;       /* SSCS Type */
   uchar sscsType;
} atmSig_aalType34Ie;


#define ATMSIG_AALTYPE34_IESIZE   14

/*
 * AAL Information Element - Type 5
 * odd length structure - need PUTSHORT and GETSHORT to extract maxSdu. 
 */
#define ATMSIG_AALTYPE5_IESIZE        11 
#define ATMSIG_AALTYPE5_IESIZE_V31    9 

typedef struct atmSig_aalType5Ie_ {
   uchar aalType;          /* AAL Type */
   uchar fwdSduSizeId;     /* fwd max cpcs-sdu id */
   uchar fwdSdu[2];
   uchar bwdSduSizeId;     /* bwd max cpcs-sdu id */
   uchar bwdSdu[2];
   uchar modeId_sscsId;    /* modeId-uni30, sscsId-uni31 */
   uchar mode_sscs;
   uchar sscsId;           /* sscs type */
   uchar sscs;
} atmSig_aalType5Ie;



/*
 * AAL Information Element - Type X
 * All the data present in this IE is user defined - upto 4 bytes. 
 */
typedef struct atmSig_AalTypeXIe_ {
   uchar aalType;          /* AAL Type */
   uchar user_info1;     
   uchar user_info2;
   uchar user_info3;
   uchar user_info4;
} atmSig_aalTypeXIe;
         
/*
 * Connection Identifier Information Element.
 * odd length structure - need PUTSHORT and GETSHORT to extract vpci, vci
 */
typedef struct atmSig_connectionIdIe_ {
   uchar assoc_sig;
   uchar vpci[2];
   uchar vci[2];
} atmSig_connectionIdIe; 

#define ATMSIG_CONNECTIONID_IESIZE      5     

/* VP Associated Signalling & Preferred/Exclusive */
#define ATM_VPASSOC_SIG   0x08

/*
 * BroadBand Bearer Capability IE Constants 
 */

typedef struct atmSig_bBearerCapability_t {
   uchar bearerClass;
   uchar trafficType;
   uchar clipping;
} atmSig_bBearerCapability;

#define ATMSIG_BBEARERCAP_IESIZE  3 

/* Bearer Class */
#define ATM_BCLASS_MASK       0x1F
#define ATM_BCLASS_BCOBA      0x01
#define ATM_BCLASS_BCOBC      0x03
#define ATM_BCLASS_BCOBX      0x10

/* Traffic Type */
#define ATM_TRAFFICTYPE_MASK       0x1C
#define ATM_TRAFFICTYPE_NULL       0x00
#define ATM_TRAFFICTYPE_CBR        0x04
#define ATM_TRAFFICTYPE_VBR        0x08

/* Timing Requirements */
#define ATM_TIMINGREQ_MASK         0x03
#define ATM_TIMINGREQ_NULL         0x00
#define ATM_TIMINGREQ_EETR         0x01
#define ATM_TIMINGREQ_EETNR        0x02
#define ATM_TIMINGREQ_RSRVD        0x03

/* Susceptibility to clipping */
#define ATM_CLIPPING_MASK          0x60
#define ATM_CLIPPING_NULL          0x00
#define ATM_CLIPPING_YES           0x02

/* User Plane Access Configuration */
#define ATM_UPAC_MASK              0x03
#define ATM_UPAC_NULL              0x00


/*
 * Call State IE 
 */

typedef struct atmSig_callStateIe_ {
   uchar value;
} atmSig_callStateIe;

#define ATMSIG_CALLSTATE_IESIZE     1

/* Call State Value */
#define ATM_CALLSTATE_NULL               0x00   /* Null */
#define ATM_CALLSTATE_CALLINIT           0x01   /* Call Initiated */
#define ATM_CALLSTATE_OUTCALLPROC        0x03   /* Outgoing Call Proceeding */
#define ATM_CALLSTATE_CALLPRESENT        0x06   /* Call Present */
#define ATM_CALLSTATE_CONNECTREQ         0x08   /* Connect Request */
#define ATM_CALLSTATE_INCALLPROC         0x09   /* Incoming Call Proceeding */
#define ATM_CALLSTATE_ACTIVE             0x0A   /* Active */
#define ATM_CALLSTATE_RELREQ             0x0B   /* Release Request */
#define ATM_CALLSTATE_RELIND             0x0C   /* Release Indication */
#define ATM_CALLSTATE_REST0             0x00   /* NULL */
#define ATM_CALLSTATE_REST1             0x3d   /* restart request */
#define ATM_CALLSTATE_REST2             0x3e   /* restart */

/* Endpoint Reference party state */
#define ATM_ENDPOINTSTATE_NULL		0x00   /* Null */
#define ATM_ENDPOINTSTATE_ADDPRTYINIT	0x01   /* Add Party Initiated */
#define ATM_ENDPOINTSTATE_PRTYALRTDLVD	0x04   /* Party Alerting delivered */
#define ATM_ENDPOINTSTATE_ADDPRTYRCVD	0x05   /* Add Party Received */
#define ATM_ENDPOINTSTATE_PRTYALRTRCVD	0x07   /* Party Alerting Received */
#define ATM_ENDPOINTSTATE_DRPPRTYINIT	0x0B   /* Drop Party Initiated */
#define ATM_ENDPOINTSTATE_DRPPRTYRCVD   0x0C   /* Drop Party Received */
#define ATM_ENDPOINTSTATE_ACTIVE	0x0A	/* Active */


/*
 * Quality of Service IE
 */

/* Constants - both fwd and backwd QOS Class have same constants */
#define ATM_QOSIE_ID                     0x5C
#define ATM_QOSIE_UNSPECD                0x00
#define ATM_QOSIE_CLASS1                 0x01
#define ATM_QOSIE_CLASS2                 0x02
#define ATM_QOSIE_CLASS3                 0x03
#define ATM_QOSIE_CLASS4                 0x04
#define ATM_QOSIE_RES                    0xff  /* reserved, uni 3.1 */

typedef struct atmSig_qosIe_ {
   uchar qosFwd;
   uchar qosBackwd;
} atmSig_qosIe;
#define ATMSIG_QOS_IESIZE     2
#define ATMSIG_QOS_CII        0xE0
#define ATMSIG_QOS_CII_ITU    0x80       /* coding standard is ITU-T */

/*
 * Broadband Sending Complete IE 
 */
#define ATM_BBAND_SENDCMPLTIE_ID        0x62
#define ATM_BBAND_SENDCMPLT_IND         0x21



/*
 * Cause IE
 */
#define ATM_CAUSEIE_ID          0x08

typedef struct atmSig_causeIe_ {
   uchar location;
   uchar causeValue;
} atmSig_causeIe;

#define ATMSIG_CAUSE_IESIZE     2 

/* Location */
#define ATM_CAUSELOC_USER               0x00
#define ATM_CAUSELOC_PRIV_NTWRK         0x01
#define ATM_CAUSELOC_PUBLIC_NTWRK       0x02
#define ATM_CAUSELOC_TRANSIT_NTWRK      0x03
#define ATM_CAUSELOC_PUBNTWRK_REMOTE    0x04
#define ATM_CAUSELOC_PRIVNTWRK_REMOTE   0x05
#define ATM_CAUSELOC_INTERNTL_NTWRK     0x07
#define ATM_CAUSELOC_NTWRK_BYND         0x06


/* Cause Value - in the IE */

/* unallocated (unassigned) number */
#define ATM_CAUSE_UNALLOC                  0x01

/* no route to specified transit network */
#define ATM_CAUSE_NOROUTE_TO_NTWRK         0x02

/* no route to destination */
#define ATM_CAUSE_NOROUTE_TO_DEST          0x03

/* VPCI/VCI unacceptable */
#define ATM_CAUSE_VPCI_UNACCEPT            0x0A

/* normal call clearing - UNI 3.1 */
#define ATM_CAUSE_CALL_CLEARING            0x10

/* User busy */
#define ATM_CAUSE_USER_BUSY                0x11

/* No user responding */
#define ATM_CAUSE_NOUSER_RSP               0x12

/* Call rejected */
#define ATM_CAUSE_CALL_REJ                 0x15

/* Number changed */
#define ATM_CAUSE_NUM_CHANGED              0x16

/* User rejects all screened numbers */
#define ATM_CAUSE_USER_REJ                 0x17

/* destination out of order */
#define ATM_CAUSE_DEST_INACTIVE            0x1B

/* invalid number format (address incomplete) */
#define ATM_CAUSE_INVALID_NUMBER_FMT       0x1C

/* response to Status Enquiry Message */
#define ATM_CAUSE_STATUSENQ_RSP            0x1E

/* normal, unspecified */
#define ATM_CAUSE_NORMAL_UNSPECIFIED       0x1F

/* requested VPCI/VCI not available */
#define ATM_CAUSE_VPCI_UNAVAIL             0x23

/* vpci/vci assignment failure, uni 3.1 */
#define ATM_CAUSE_VPCI_FAIL                0x24

/* user cell rate not available, uni 3.1, same as 0x33 */
#define ATM_CAUSE_CELLRATE_UNAVAIL         0x25

/* network out of order (Not used in this Implementation Agreement) */
#define ATM_CAUSE_NTWRK_OUT_OF_ORDER       0x26

/* temporary failure */
#define ATM_CAUSE_TMP_FAILURE              0x29

/* access information discarded */
#define ATM_CAUSE_ACCESS_INFO_DISCRD       0x2B

/* no VPCI/VCI available */
#define ATM_CAUSE_NO_VPCI                  0x2D

/* resource unavailable, unspecified */
#define ATM_CAUSE_RSRC_UNAVAIL             0x2F

/* Qulaity of Service unavailable */
#define ATM_CAUSE_QOS_UAVAIL               0x31

/* user cell rate not available */
#define ATM_CAUSE_USR_CELLRATE_UNAVAIL     0x33

/* bearer capability not authorized */
#define ATM_CAUSE_BEARERCAP_UNAUTH         0x39

/* bearer capability not presently available */
#define ATM_CAUSE_BEARERCAP_NOTAVAIL       0x3A

/* Service or option not available, unspecified */
#define ATM_CAUSE_SERVICE_UNAVAIL          0x3F

/* bearer capability not implemented */
#define ATM_CAUSE_BEARERCAP_UNIMP          0x41

/* Unsupported combination of traffic parameters */
#define ATM_CAUSE_UNSUPPORTED              0x49

/* aal parameter can not be support, uni 3.1, same as 0x5d */
#define ATM_CAUSE_AALPARAM_NOT_SUPPORT     0x4e

/* invalid call reference value */
#define ATM_CAUSE_INVALID_CALLREF          0x51

/* identified channel does not exist */
#define ATM_CAUSE_CHANNEL_NOTPRESENT       0x52

/* incompatible destination */
#define ATM_CAUSE_INCOMPAT_DEST            0x58

/* invalid end point reference */
#define ATM_CAUSE_INVALID_ENDPOINT_REF     0x59

/* invalid transit network selection */
#define ATM_CAUSE_INVALID_TRNTWRK_SEL      0x5B

/* too many pending add party requests */
#define ATM_CAUSE_TOOMANY_ADDPARTY_REQ     0x5C

/* AAL parameters cannot be supported */
#define ATM_CAUSE_INVALID_AAL_PARAM        0x5D

/* mandatory information element is missing */
#define ATM_CAUSE_MANDATORY_IE_MISSING     0x60

/* message type non-existent or not implemented */
#define ATM_CAUSE_MSG_NOT_EXISTING         0x61

/* information element non-existent or not implemented */
#define ATM_CAUSE_IE_NOT_EXISTING          0x63

/* invalid information element contents */
#define ATM_CAUSE_INVALID_IE_CONTENTS       0x64

/* message not compatible with call state */
#define ATM_CAUSE_INCOMPAT_MSG             0x65

/* recovery on timer expiry */
#define ATM_CAUSE_RCVRY_ON_TIMEREXPIRY     0x66

/* incorrect message length */
#define ATM_CAUSE_INCORRCT_MSG_LNGTH       0x68

/* protocol error, unspecified */
#define ATM_CAUSE_PROTOCOL_ERROR           0x6F


/*
 * Called Party Number IE
 */
#define ATM_CALLEDPARTYIE_ID               0x70

typedef struct atmSig_calledPartyNumber_ {
   uchar numberingPlan;
   uchar nsap[ATM_NSAP_ADDR_LNGTH];
} atmSig_calledPartyNumber;

#define ATMSIG_CALLEDPARTY_IESIZE        STATIONLEN_ATMNSAP + 1 

/* Type of Number */
#define ATM_CALLEDPARTY_NUM_MASK           0x70
#define ATM_CALLEDPARTY_NUM_UKNOWN         0x00
#define ATM_CALLEDPARTY_NUM_INTERNTL       0x10

/* Addressing/numbering plan identification */
#define ATM_CALLEDPARTY_NUMPLAN_MASK       0x0F
#define ATM_CALLEDPARTY_NUMPLAN_ISDN       0x01
#define ATM_CALLEDPARTY_NUMPLAN_NSAP       0x02

/* Presentation Indicator/Screening Indicator */
#define ATM_CALLEDPARTY_PRESCR_IND         0x00

/*
 * Calling Party Number IE
 */
#define ATM_CALLINGPARTYIE_ID                   0x6C

typedef struct atmSig_callingPartyNumber_ {
   uchar numberingPlan;
   uchar presentationIndicator;     /* Presentation & Screening Indicator */
   uchar nsap[ATM_NSAP_ADDR_LNGTH];
} atmSig_callingPartyNumber;

#define ATMSIG_CALLINGPARTY_IESIZE        STATIONLEN_ATMNSAP + 2 

/* Presentation Indicator */
#define ATM_CALLINGPARTYIE_PRESIND_MASK         0x60
#define ATM_CALLINGPARTYIE_PRESIND_ALLOWED      0x00
#define ATM_CALLINGPARTYIE_PRESIND_RESTRCTD     0x20
#define ATM_CALLINGPARTYIE_PRESIND_NOTAVAIL     0x40
#define ATM_CALLINGPARTYIE_PRESIND_RSRVD        0x60

/* Screening Indicator */
#define ATM_CALLINGPARTYIE_SCREENIND_MASK       0x03
#define ATM_CALLINGPARTYIE_SCREENIND_NONE       0x00
#define ATM_CALLINGPARTYIE_SCREENIND_PASSED     0x01
#define ATM_CALLINGPARTYIE_SCREENIND_FAILED     0x02
#define ATM_CALLINGPARTYIE_SCREENIND_NETWRK     0x03


/*
 * BroadBand High Layer IE
 */

/* Type */
#define ATM_BROADBAND_TYPE_ISO             0x00
#define ATM_BROADBAND_TYPE_USER            0x01
#define ATM_BROADBAND_TYPE_HIGHLYR         0x02
#define ATM_BROADBAND_TYPE_VENDOR          0x03


/*
 * Restart Indicator IE
 */

typedef struct atmSig_restartIndicatorIe_
{
   uchar class;

} atmSig_restartIndicatorIe;

#define ATMSIG_RESTARTINDICATOR_IESIZE    1 


/* Class */
#define ATM_RESTART_CLASS_MASK          0x07
#define ATM_RESTART_CLASS_ONE_VC        0x00
#define ATM_RESTART_CLASS_ALL_VC        0x02


/*
 * Transit Network Selection
 */

/* National Network Identification */
#define ATM_TRANSIT_NETID               0x02

/* Network Identification Plan */
#define ATM_TRANSIT_NETID_PLAN          0x01

/* Carrier Identification Code Length - we use a length of 20 for now */
#define ATM_TRANSIT_NETID_LENGTH          20


/*
 * User Cell Rate
 */
#define ATM_OP_FWD_PEAK_CLP0       0x82 
#define ATM_OP_BWD_PEAK_CLP0       0x83
#define ATM_OP_FWD_PEAK_CLP01      0x84
#define ATM_OP_BWD_PEAK_CLP01      0x85
#define ATM_OP_FWD_SUS_CLP0        0x88
#define ATM_OP_BWD_SUS_CLP0        0x89
#define ATM_OP_FWD_SUS_CLP01       0x90
#define ATM_OP_BWD_SUS_CLP01       0x91
#define ATM_OP_FWD_BURST_CLP0      0xA0
#define ATM_OP_BWD_BURST_CLP0      0xA1
#define ATM_OP_FWD_BURST_CLP01     0xB0
#define ATM_OP_BWD_BURST_CLP01     0xB1


#define ATM_UCR_FWD_PEAK_CLP0       0x82000000 
#define ATM_UCR_BWD_PEAK_CLP0       0x83000000
#define ATM_UCR_FWD_PEAK_CLP01      0x84000000
#define ATM_UCR_BWD_PEAK_CLP01      0x85000000
#define ATM_UCR_FWD_SUS_CLP0        0x88000000
#define ATM_UCR_BWD_SUS_CLP0        0x89000000
#define ATM_UCR_FWD_SUS_CLP01       0x90000000
#define ATM_UCR_BWD_SUS_CLP01       0x91000000
#define ATM_UCR_FWD_BURST_CLP0      0xA0000000
#define ATM_UCR_BWD_BURST_CLP0      0xA1000000
#define ATM_UCR_FWD_BURST_CLP01     0xB0000000
#define ATM_UCR_BWD_BURST_CLP01     0xB1000000
#define ATM_UCR_BEST_EFFORT_IND     0xBE
#define ATM_UCR_TRMGMT_OPTIONS_ID   0xBF

#define ATM_UCR_TAGGING_FWD         0x01
#define ATM_UCR_TAGGING_BWD         0x02
#define ATM_UCR_VALUE_SIZE          0x00FFFFFF

/* Best effort service reuires fwd and bwd peak cell rates */
#define ATM_UCR_BESTEFFORT_LNGTH    9

#define ATMSIG_UCRID_LENGTH         4
#define ATMSIG_UCRID_TRMGMT_LENGTH  2

typedef union atmSig_userCellRate_ {
   uchar id;
   uchar cellRate[4];
} atmSig_userCellRate;


typedef struct atmSig_lowLayerIe_ {
   uchar layer3id;
   uchar ipi[2];
   uchar snapid_oui[4];
   uchar pid[2];
} atmSig_lowLayerIe;

#define ATMSIG_LLAYER3_LENGTH   9 
#define ATMSIG_LLAYER3_ISO9577  0x6B
#define ATMSIG_LLAYER3_IPI      0x4080
#define ATMSIG_LLAYER3_SNAPID   0x80000000 

#define ATMSIG_LLAYER3_ID_OFFSET       0x0
#define ATMSIG_LLAYER3_IPI_OFFSET      0x1
#define ATMSIG_LLAYER3_SNAPID_OFFSET   0x3
#define ATMSIG_LLAYER3_PID_OFFSET      0x7

#define ATMSIG_LLAYER2_LENGTH   1
#define ATMSIG_LLAYER2_ID       0x40

#define ATMSIG_LLAYER_LENGTH (ATMSIG_LLAYER2_LENGTH + ATMSIG_LLAYER3_LENGTH)

/* Max number of BLL-IE's in a Setup message */
#define ATM_MAX_BLLI_NUM     3

/* Broadband Repeat Indicator IE */
#define ATMSIG_REPEAT_INDICATION        0x02

/* Multipoint here */

/* Endpoint Reference IE */

typedef struct atmSig_endpointRefIe_ {
   uchar endpointRefType;
   uchar endpointRefId[2];
} atmSig_endpointRefIe;

#define ATMSIG_ENDPOINT_REF_IESIZE 0x03 

#define ATMSIG_ENDPOINT_REF_TYPE   0x00 
#define ATMSIG_ENDPOINT_REF_FLAG   0x00 
#define ATMSIG_ENDPOINT_REF_MASK   0x7FFF

/* Endpoint State IE */

typedef struct atmSig_endpointStateIe_ {
   uchar endpointState;
} atmSig_endpointStateIe;

#define ATMSIG_ENDPOINT_STATE_IESIZE 0x01 
#define ATMSIG_ENDPOINT_STATE_IE_MASK 0x3F 

/* Endpoint States */

#define ATMSIG_ENDPOINT_STATE_NULL              0x00 
#define ATMSIG_ENDPOINT_STATE_ADD_PARTY_INIT    0x01 
#define ATMSIG_ENDPOINT_STATE_ADD_PARTY_RCVD    0x06 
#define ATMSIG_ENDPOINT_STATE_DROP_PARTY_INIT   0x0B 
#define ATMSIG_ENDPOINT_STATE_DROP_PARTY_RCVD   0x0C 
#define ATMSIG_ENDPOINT_STATE_ACTIVE            0x0A 



#endif __ATMSIG_IE_H__
