/* $Id: x25.h,v 3.2 1995/11/17 17:42:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/x25.h,v $
 *------------------------------------------------------------------
 * x25.h  X.25 packet utilities include file  
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: x25.h,v $
 * Revision 3.2  1995/11/17  17:42:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:15:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:18:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       x25.h                                                   
DESCRIPTION:  X.25 packet utilities include file                      
VERSION:      3.21                                                    

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************

GENERAL NOTES

*************************************************************************
*************************************************************************
*/


/************************************************************************
*                           C O N S T A N T S
*************************************************************************
*/

/* local compile time flags
*/
#define CF_PKT_DTE_REJ                    1
#define CF_FAC_PKT_SIZE                   1
#define CF_FAC_WIN_SIZE                   1
#define CF_FAC_THRU_PUT                   0
#define CF_FAC_CUG                        0
#define CF_FAC_RCFS                       1
#define CF_FAC_NUI_SEL                    0
#define CF_FAC_CHRG_INFO_REQ              0
#define CF_FAC_CHRG_INFO_SEG              0
#define CF_FAC_CHRG_INFO_DUR              0
#define CF_FAC_RPOA                       0
#define CF_FAC_CALL_DEF                   0
#define CF_FAC_CALL_RED_DEF_NTFY          0
#define CF_FAC_ADD_MOD_NTFY               0
#define CF_FAC_TRNST_DLY_IND              0
#define CF_FAC_MARKER                     0
#define CF_FAC_STATE_P1                   0
#define CF_FAC_ANY_TIME                   0
#define CF_FAC_LCN_RNG                    0

/* Function error return codes
*/
#define ERR_PKT_UNIDENT                   -1
#define ERR_PKT_BAD_GFI                   -2

/* Packet types - from DTE to DCE (table 17/X.25)
*/
#define CALL_REQUEST                      0        /* setup & clearing */
#define INCOMING_CALL                     0
#define CALL_ACCEPTED                     1
#define CALL_CONNECTED                    1
#define CLEAR_REQUEST                     2
#define CLEAR_IND                         2
#define DTE_CLEAR_CONF                    3
#define DCE_CLEAR_CONF                    3
#define DTE_DATA                          4      /* data and interrupt */
#define DCE_DATA                          4
#define DTE_INT                           5
#define DCE_INT                           5
#define DTE_INT_CONF                      6
#define DCE_INT_CONF                      6
#define DTE_RR                            7            /* flow control */
#define DCE_RR                            7
#define DTE_RNR                           8
#define DCE_RNR                           8
#define DTE_REJ                           9           /* not supported */
#define RESET_REQUEST                     10                  /* reset */
#define RESET_IND                         10
#define DTE_RESET_CONF                    11
#define DCE_RESET_CONF                    11
#define RESTART_REQUEST                   12                /* restart */
#define RESTART_IND                       12
#define DTE_RESTART_CONF                  13
#define DCE_RESTART_CONF                  13
#define DIAGNOSTIC                        14            /* diagnostics */
#define REG_REQUEST                       15           /* registration */
#define REG_CONF                          16

/* Clearing causes (table 20/X.25)
*/
#define CLR_CAUSE_DTE_ORIGINATED          0x00
#define CLR_CAUSE_NUMBER_BUSY             0x01
#define CLR_CAUSE_OUT_OF_ORDER            0x09
#define CLR_CAUSE_REMOTE_PROC_ERROR       0x11
#define CLR_CAUSE_REV_CHRG_ACCPT_NOT_SUB  0x19
#define CLR_CAUSE_INCOMPATIBLE_DEST       0x21
#define CLR_CAUSE_FAST_SEL_ACCPT_NOT_SUB  0x29
#define CLR_CAUSE_SHIP_ABSENT             0x39
#define CLR_CAUSE_INVALID_FAC_REQ         0x03
#define CLR_CAUSE_ACCESS_BARRED           0x0B
#define CLR_CAUSE_LOCAL_PROC_ERROR        0x13
#define CLR_CAUSE_NETWORK_CONGEST         0x05
#define CLR_CAUSE_NOT_OBTAINABLE          0x0D
#define CLR_CAUSE_RPOA_OUT_OF_ORDER       0x15

/* Resetting causes (table 21/X.25)
*/
#define RST_CAUSE_DTE_ORIGINATED          0x00
#define RST_CAUSE_OUT_OF_ORDER            0x01
#define RST_CAUSE_REMOTE_PROC_ERROR       0x03
#define RST_CAUSE_LOCAL_PROC_ERROR        0x05
#define RST_CAUSE_NETWORK_CONGEST         0x07
#define RST_CAUSE_REMOTE_DTE_OPER         0x09
#define RST_CAUSE_NETWORK_OPER            0x0f
#define RST_CAUSE_INCOMPATIBLE_DEST       0x11
#define RST_CAUSE_NEWORK_OUT_OF_ORDER     0x1d

/* Restart causes (table 22/X.25)
*/
#define RSTRT_CAUSE_LOCAL_PROC_ERROR      0x00
#define RSTRT_CAUSE_NETWORK_CONGEST       0x03
#define RSTRT_CAUSE_NETWORK_OPER          0x07
#define RSTRT_CAUSE_REG_CANC_CONF         0x7f

/* Registration cause field values (table 23/X.25)
*/
#define REG_CAUSE_CANCEL_CONF             0x7f
#define REG_CAUSE_INVALID_FAC_REQ         0x03
#define REG_CAUSE_LOCAL_PROC_ERROR        0x13
#define REG_CAUSE_NETWORK_CONGEST         0x05

/* Diagnostic codes (table E1/X.25)
*/
#define DIAG_G1_NO_ADD_INFO               0
#define DIAG_G1_INVALID_PS                1
#define DIAG_G1_INVALID_PR                2
#define DIAG_G2_PKT_INVALID               16
#define DIAG_G2_STATE_R1                  17
#define DIAG_G2_STATE_R2                  18
#define DIAG_G2_STATE_R3                  19
#define DIAG_G2_STATE_P1                  20
#define DIAG_G2_STATE_P2                  21
#define DIAG_G2_STATE_P3                  22
#define DIAG_G2_STATE_P4                  23
#define DIAG_G2_STATE_P5                  24
#define DIAG_G2_STATE_P6                  25
#define DIAG_G2_STATE_P7                  26
#define DIAG_G2_STATE_D1                  27
#define DIAG_G2_STATE_D2                  28
#define DIAG_G2_STATE_D3                  29
#define DIAG_G3_PKT_NOT_ALLWD             32
#define DIAG_G3_UNIDENT_PKT               33
#define DIAG_G3_CALL_ON_ONE_WAY_CHAN      34
#define DIAG_G3_INVALID_PKT_ON_PVC        35
#define DIAG_G3_PKT_ON_UNASSGND_CHAN      36
#define DIAG_G3_REJ_NOT_SUB               37
#define DIAG_G3_PKT_TOO_SHORT             38
#define DIAG_G3_PKT_TOO_LONG              39
#define DIAG_G3_INVAILD_GFI               40
#define DIAG_G3_BAD_NON_ZERO_BITS         41
#define DIAG_G3_PKT_NOT_COMPAT            42
#define DIAG_G3_UNAUTH_INT_CONF           43
#define DIAG_G3_UNAUTH_INT                44
#define DIAG_G3_UNAUTH_REJ                45
#define DIAG_G4_TIME_EXP                  48
#define DIAG_G4_TIME_EXP_INC_CALL         49
#define DIAG_G4_TIME_EXP_CLR_IND          50
#define DIAG_G4_TIME_EXP_RST_IND          51
#define DIAG_G4_TIME_EXP_RSRT_IND         52
#define DIAG_G4_TIME_EXP_CALL_DEF         53
#define DIAG_G5_SETUP_CLR_REG_PROB        64
#define DIAG_G5_FAC_REG_CODE_NOT_ALLWD    65
#define DIAG_G5_FAC_PARAM_NOT_ALLWD       66
#define DIAG_G5_INVALID_CALLED_DTE        67
#define DIAG_G5_INVALID_CALLING_DTE       68
#define DIAG_G5_INVALID_FAC_REG_LEN       69
#define DIAG_G5_INC_CALL_BARRED           70
#define DIAG_G5_NO_CHAN_AVAIL             71
#define DIAG_G5_CALL_COLL                 72
#define DIAG_G5_DUP_FAC_REQ               73
#define DIAG_G5_NON_ZERO_ADD_LEN          74
#define DIAG_G5_NON_ZERO_FAC_LEN          75
#define DIAG_G5_FAC_NOT_PROVIDED          76
#define DIAG_G5_INVALID_DTE_FAC           77
#define DIAG_G5_MAX_RED_DEF_EXCEED        78
#define DIAG_G6_MISC                      80
#define DIAG_G6_IMPROPER_CAUSE            81
#define DIAG_G6_NOT_ALIGNED_OCTET         82
#define DIAG_G6_INCONSISTENT_Q_BIT        83
#define DIAG_G6_NUI_PROB                  84
#define DIAG_G7_NOT_ASSIGNED              96
#define DIAG_G8_INTL_PROB                 112
#define DIAG_G8_REM_NETWORK_PROB          113
#define DIAG_G8_INTL_PROT_PROB            114
#define DIAG_G8_INTL_LINK_OUT_OF_ORDER    115
#define DIAG_G8_INTL_LINK_BUSY            116
#define DIAG_G8_TRAN_NETWORK_FAC_PROB     117
#define DIAG_G8_REM_NETWORK_FAC_PROB      118
#define DIAG_G8_INTL_ROUTE_PROB           119
#define DIAG_G8_TMP_ROUTE_PROB            120
#define DIAG_G8_UNKN_CALLED_DNIC          121
#define DIAG_G8_MAINT_ACTION              122
#define DIAG_G0_RESEVED                   128

/* High order two bits define the facility/registration element class
*  (Par. 7.1)
*/
#define CLASS_A                           0x00
#define CLASS_B                           0x40
#define CLASS_C                           0x80
#define CLASS_D                           0xC0

/* Facility codes
*/
#define FAC_PKT_SIZE                      (CLASS_B|0x02)
#define FAC_WIN_SIZE                      (CLASS_B|0x03)
#define FAC_THRU_PUT                      (CLASS_A|0x02)
#define FAC_CUG                           (CLASS_A|0x03)
#define FAC_EXT_CUG                       (CLASS_B|0x07)
#define FAC_CUGWOA                        (CLASS_A|0x09)
#define FAC_EXT_CUGWOA                    (CLASS_B|0x08)
#define FAC_BCUG                          (CLASS_B|0x01)
#define FAC_REV_CHRG                      (CLASS_A|0x01)
#define FAC_NUI_SEL                       (CLASS_D|0x06)
#define FAC_CHRG_INFO_REQ                 (CLASS_A|0x04)
#define FAC_CHRG_INFO_SEG                 (CLASS_D|0x02)
#define FAC_CHRG_INFO_DUR                 (CLASS_D|0x01)
#define FAC_RPOA                          (CLASS_B|0x04)
#define FAC_EXT_RPOA                      (CLASS_D|0x04)
#define FAC_CALL_DEFLECT                  (CLASS_D|0x11)
#define FAC_CALL_RED_DEF_NTFY             (CLASS_D|0x03)
#define FAC_ADD_MOD_NTFY                  (CLASS_A|0x08)
#define FAC_TRNST_DLY_IND                 (CLASS_B|0x09)
#define FAC_MARKER                        0

/* Registration codes
*/
#define REG_STATE_P1                      (CLASS_A|0x05)
#define REG_ANY_TIME                      (CLASS_B|0x05)
#define REG_AVAILABILITY                  (CLASS_B|0x06)
#define REG_NON_NEG                       (CLASS_A|0x06)
#define REG_THRU_PUT                      (CLASS_A|0x02)
#define REG_PKT_SIZE                      (CLASS_B|0x02)
#define REG_WIN_SIZE                      (CLASS_B|0x03)
#define REG_LCN_RNG                       (CLASS_D|0x08)

/* closed user group types
*/
#define CUG_NONE                          0
#define CUG_BASIC                         1
#define CUG_WOA                           2
#define CUG_BILAT                         3


/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S
*************************************************************************
*/


/************************************************************************
*                              M A C R O S
*************************************************************************
*/

/* X.25 Facility field unpacking macros.
*  Refer to CCITT Rec. X.25 Section 7.
*
*  In general, the 'i' argument to these macros is a byte pointer
*  to the first byte of the facility/registration element of the packet.
*  If the element has multiple fields a second parameter, 'n', specifies
*  which of those fields to unpack.  An 's' parameter is a character
*  pointer to a destination string of suitable length.
*
*/
#define x25upfPktSizeOut(i)               x25uDecodePktSize((i)[1])
#define x25upfPktSizeInc(i)               x25uDecodePktSize((i)[2])
#define x25upfWinSizeOut(i)               ((i)[1])
#define x25upfWinSizeInc(i)               ((i)[2])
#define x25upfThruPutOut(i)               x25uDecodeThrpt((i)[1]>>4)
#define x25upfThruPutInc(i)               x25uDecodeThrpt((i)[1]&0x0f)
#define x25upfCUG(i)                      x25uBcdToLong((i)+1,0,2)
#define x25upfExtCUG(i)                   x25uBcdToLong((i)+1,0,4)
#define x25upfCUGWOA(i)                   x25uBcdToLong((i)+1,0,2)
#define x25upfExtCUGWOA(i)                x25uBcdToLong((i)+1,0,4)
#define x25upfExtBCUG(i)                  x25uBcdToLong((i)+1,0,4)
#define x25upfRCFSRevChr(i)               (((i)[1]&0x01)!=0)
#define x25upfRCFSRestrict(i)             (((i)[1]&0x40)!=0)
#define x25upfRCFSFastSel(i)              (((i)[1]&0x80)!=0)
#define x25upfNUISelLen(i)                ((i)[1])
#define x25upfNUISelPtr(i)                ((i)+2)
#define x25upfChrgInfoReq(i)              (((i)[1]&0x01)!=0)
#define x25upfChrgInfoSegPeriods(i)       ((i)[1])
#define x25upfChrgInfoSegSent(i,n)        x25uBcdToLong((i)+2+((n)*4),0,8)
#define x25upfChrgInfoSegRcvd(i,n)        x25uBcdToLong((i)+6+((n)*4),0,8)
#define x25upfChrgInfoDurPeriods(i)       ((i)[1])
#define x25upfChrgInfoDurDays(i,n)        x25uBcdToLong((i)+2+((n)*4),0,2)
#define x25upfChrgInfoDurHours(i,n)       x25uBcdToLong((i)+3+((n)*4),0,2)
#define x25upfChrgInfoDurMins(i,n)        x25uBcdToLong((i)+4+((n)*4),0,2)
#define x25upfChrgInfoDurSecs(i,n)        x25uBcdToLong((i)+5+((n)*4),0,2)
#define x25upfRPOA(i)                     x25uBcdToLong((i)+1,0,2)
#define x25upfExtRPOANetworks(i)          ((i)[1])
#define x25upfExtRPOAid(i,n)              x25uBcdToLong((i)+2+((n)*2),0,4)
#define x25upfCallDefReason(i)            ((i)[2])
#define x25upfCallDefAddStr(i,s)          x25uBcdToAsc((i)+4,0,(i)[3],(s))
#define x25upfCallRedDefNtfyReason(i)     ((i)[2])
#define x25upfCallRedDefNtfyAddStr(i,s)   x25uBcdToAsc((i)+4,0,(i)[3],(s))
#define x25upfAddModNtfy(i)               ((i)[1])
#define x25upfTrnstDlyInd(i)              (((i)[1]<<8)|(i)[2])

/* Registration field unpacking macros.
*  Refer to CCITT Rec. X.25 Section 7.
*
*  In general, the 'i' argument to these macros is a byte pointer
*  to the first byte of the facility/registration element of the packet.
*/
#define x25uprStateP1SeqNum(i)            (((i)[1]&0x01)!=0)
#define x25uprStateP1ReXmt(i)             (((i)[1]&0x02)!=0)
#define x25uprStateP1DbitMod(i)           (((i)[1]&0x04)!=0)
#define x25uprAnyTimeChginfo(i)           (((i)[1]&0x40)!=0)
#define x25uprAnyTimeThruputneg(i)        (((i)[1]&0x20)!=0)
#define x25uprAnyTimeFlowneg(i)           (((i)[1]&0x10)!=0)
#define x25uprAnyTimeRevchgacp(i)         (((i)[1]&0x08)!=0)
#define x25uprAnyTimeFastselacp(i)        (((i)[1]&0x04)!=0)
#define x25uprAnyTimeNooutcall(i)         (((i)[1]&0x02)!=0)
#define x25uprAnyTimeNoinccall(i)         (((i)[1]&0x01)!=0)
#define x25uprAvailRevChrg(i)             (((i)[1]&0x80)!=0)
#define x25uprAvailRevChrgAcpt(i)         (((i)[1]&0x40)!=0)
#define x25uprAvailChrgInfoPerCall(i)     (((i)[1]&0x20)!=0)
#define x25uprAvailChrgInfoPerInt(i)      (((i)[1]&0x10)!=0)
#define x25uprAvailCldLineAddrModNtfy(i)  (((i)[1]&0x08)!=0)
#define x25uprAvailDBitMod(i)             (((i)[1]&0x04)!=0)
#define x25uprAvailPktReXmt(i)            (((i)[1]&0x02)!=0)
#define x25uprAvailExtPktSeqNum(i)        (((i)[1]&0x01)!=0)
#define x25uprAvailRPOASel(i)             (((i)[2]&0x10)!=0)
#define x25uprAvailLCNTypeRanges(i)       (((i)[2]&0x08)!=0)
#define x25uprAvailNonStdDefPktSiz(i)     (((i)[2]&0x04)!=0)
#define x25uprAvailNonStdDefWindSiz(i)    (((i)[2]&0x02)!=0)
#define x25uprAvailDefThruputClassAsgn(i) (((i)[2]&0x01)!=0)
#define x25uprNonNegLocChrgPrev(i)        (((i)[1]&0x01)!=0)
#define x25uprThruPutOut(i)               x25uDecodeThrpt((i)[1]>>4)
#define x25uprThruPutInc(i)               x25uDecodeThrpt((i)[1]&0x0f)
#define x25uprPktSizeOut(i)               x25uDecodePktSize((i)[1])
#define x25uprPktSizeInc(i)               x25uDecodePktSize((i)[2])
#define x25uprWinSizeOut(i)               ((i)[1])
#define x25uprWinSizeInc(i)               ((i)[2])
#define x25uprLcnRngLic(i)                (((i)[2]<<8)|(i)[3])
#define x25uprLcnRngHic(i)                (((i)[4]<<8)|(i)[5])
#define x25uprLcnRngLtc(i)                (((i)[6]<<8)|(i)[7])
#define x25uprLcnRngHtc(i)                (((i)[8]<<8)|(i)[9])
#define x25uprLcnRngLoc(i)                (((i)[10]<<8)|(i)[11])
#define x25uprLcnRngHoc(i)                (((i)[12]<<8)|(i)[13])
#define x25uprLcnRngNum(i)                (((i)[14]<<8)|(i)[15])


/************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S
*************************************************************************
*/

/* utility routines
*/
PUBLIC int x25uEncodePktSize ARGS((int));
PUBLIC int x25uDecodePktSize ARGS((int));
PUBLIC int x25uEncodeThrpt ARGS((unsigned));
PUBLIC unsigned x25uDecodeThrpt ARGS((int));
PUBLIC void x25uPackNybble ARGS((int, BYTE *, int));
PUBLIC void x25uLongToBcd ARGS((long, int, BYTE *, int));
PUBLIC long x25uBcdToLong ARGS((BYTE *, int, int));
PUBLIC int x25uAscToBcd ARGS((char *, BYTE *, int));
PUBLIC char * x25uBcdToAsc ARGS((BYTE *, int, int, char *));

/* X.25 packet header building routines
*/
PUBLIC void x25pktCallReq ARGS((PKT *, int, int, int));
PUBLIC void x25pktCallAcc ARGS((PKT *, int, int, int));
PUBLIC void x25pktClrReq ARGS((PKT *, int, int, int, int));
PUBLIC void x25pktDteClrConf ARGS((PKT *, int, int));
PUBLIC void x25pktDteData ARGS((PKT *, int, int, int, int, int, int, int));
PUBLIC void x25pktDteInt ARGS((PKT *, int, int));
PUBLIC void x25pktDteIntConf ARGS((PKT *, int, int));
PUBLIC void x25pktDteRR ARGS((PKT *, int, int, int));
PUBLIC void x25pktDteRNR ARGS((PKT *, int, int, int));
PUBLIC void x25pktResetReq ARGS((PKT *, int, int, int, int));
PUBLIC void x25pktDteResetConf ARGS((PKT *, int, int));
PUBLIC void x25pktRestartReq ARGS((PKT *, int, int, int));
PUBLIC void x25pktDteRestartConf ARGS((PKT *, int));
#if CF_PKT_DTE_REJ
PUBLIC void x25pktDteRej ARGS((PKT *, int, int, int));
#endif
PUBLIC void x25pktRegReq ARGS((PKT *, int));

/* Other X.25 packet field routines
*/
PUBLIC void x25setPs ARGS((PKT *, int));
PUBLIC void x25setPr ARGS((PKT *, int));
PUBLIC void x25addBlk ARGS((PKT *, char *, char *));

/* X.25 facility element building routines
*/
PUBLIC void x25facLenZero ARGS((PKT *));
#if CF_FAC_PKT_SIZE
PUBLIC void x25facPktSize ARGS((PKT *, int, int));
#endif
#if CF_FAC_WIN_SIZE
PUBLIC void x25facWinSize ARGS((PKT *, int, int));
#endif
#if CF_FAC_THRU_PUT
PUBLIC void x25facThruPut ARGS((PKT *, unsigned, unsigned));
#endif
#if CF_FAC_CUG
PUBLIC void x25facCUG ARGS((PKT *, int, int));
#endif
#if CF_FAC_RCFS
PUBLIC void x25facRCFS ARGS((PKT *, int, int, int));
#endif
#if CF_FAC_NUI_SEL
PUBLIC void x25facNUISel ARGS((PKT *, BYTE *, int));
#endif
#if CF_FAC_CHRG_INFO_REQ
PUBLIC void x25facChrgInfoReq ARGS((PKT *, int));
#endif
#if CF_FAC_CHRG_INFO_SEG
PUBLIC void x25facChrgInfoSeg ARGS((PKT *, int, long *, long *));
#endif
#if CF_FAC_CHRG_INFO_DUR
PUBLIC void x25facChrgInfoDur ARGS((PKT *, int, int *, int *, int *, int *));
#endif
#if CF_FAC_RPOA
PUBLIC void x25facRPOA ARGS((PKT *, int *));
#endif
#if CF_FAC_CALL_DEF
PUBLIC void x25facCallDef ARGS((PKT *, int, char *));
#endif
#if CF_FAC_CALL_RED_DEF_NTFY
PUBLIC void x25facCallRedDefNtfy ARGS((PKT *, int, char *));
#endif
#if CF_FAC_ADD_MOD_NTFY
PUBLIC void x25facAddModNtfy ARGS((PKT *, int));
#endif
#if CF_FAC_TRNST_DLY_IND
PUBLIC void x25facTrnstDlyInd ARGS((PKT *, unsigned));
#endif
#if CF_FAC_MARKER
PUBLIC void x25facMarker ARGS((PKT *, int));
#endif

/* X.25 registration element building routines
*/
#if CF_FAC_STATE_P1
PUBLIC void x25facStateP1 ARGS((PKT *, int, int, int));
#endif
#if CF_FAC_ANY_TIME
PUBLIC void x25facAnyTime ARGS((PKT *, int, int, int, int, int, int, int));
#endif
#if CF_FAC_LCN_RNG
PUBLIC void x25facLcnRng ARGS((PKT *, int, int, int, int, int, int));
#endif

/* X.25 packet header decoding routines
*/
PUBLIC int x25getType ARGS((PKT *));
PUBLIC int x25getMod ARGS((PKT *));
PUBLIC int x25getLcn ARGS((PKT *));
PUBLIC int x25getPr ARGS((PKT *));
PUBLIC int x25getPs ARGS((PKT *));
PUBLIC int x25getQbit ARGS((PKT *));
PUBLIC int x25getMbit ARGS((PKT *));
PUBLIC int x25getDbit ARGS((PKT *));
PUBLIC int x25getCause ARGS((PKT *));
PUBLIC int x25getDiag ARGS((PKT *));
PUBLIC int x25getDataLoc ARGS((PKT *));
PUBLIC int x25getFacLenLoc ARGS((PKT *));
PUBLIC int x25getNextFacLoc ARGS((PKT *, int));
PUBLIC int x25getFac ARGS((PKT *, int));

