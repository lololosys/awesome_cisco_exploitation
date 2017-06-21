/* $Id: lif.h,v 3.2.60.1 1996/04/30 23:07:55 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/lif.h,v $
 *------------------------------------------------------------------
 * lif.h  ISDN layer interface include file
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: lif.h,v $
 * Revision 3.2.60.1  1996/04/30  23:07:55  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:41:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:16:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: lif.h
VERSION: 3.21
DESCRIPTION: ISDN layer interface include file

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

This include file contains definitions that are shared among all layers.

*************************************************************************
*************************************************************************
*/

#ifndef     LIF_H                         /* prevent nested inclusions */
#define     LIF_H                                       /* module name */


/************************************************************************
*                           C O N S T A N T S
*************************************************************************
*/

#define MINUS_ONE (-1)
#define PLUS_ONE 1
#define ZERO 0

/* Layer 2 Protocols
*/
#define     L2P_LAPD          0
#define     L2P_LAPB          1
#define     L2P_V120          2

/* LIF_Mail destination bit settings: these should be or'ed
*  together ass needed for the desired destinations
*/
#define     MAIL_TO_DEST      (unsigned) 1
#define     MAIL_TO_TRACER    (unsigned) 2

/* Task IDs (0x0<i><nn>: where i is the ISO level and nn is sequential)
*
*  NOTE: If more than one task exists at an ISO level, nn should reflect
*        the 'sub-level' of the task relative to others at that level.
*        For example, L2_ID (0x0205) is at a 'higher' level than
*        HDLC_D_ID (0x0200).  This system is used so that the directional
*        flow of mail (UP or DOWN) may be determined by the source and
*        destination IDs.  For example, mail flows from L2_ID (0x0205)
*        UP to L3_ID (0x0300) and DOWN to HDLC_D_ID (0x0200).
*/
#define     TRACER_PVT_ID   0x0702          /* tracing task private ID */
#define     TRACER_ID       0x0701                     /* tracing task */
#define     ME_ID           0x0700                /* management entity */
#define     TT_ID           0x0502        /* terminal adapter xmt task */
#define     RT_ID           0x0501        /* terminal adapter rcv task */
#ifdef TASK_V3
#define     HOST_ID         V3_ID
#else
#define     HOST_ID         0x0500      /* Application processor layer */
#endif
#define     CC_X25_ID       0x0401                 /* x25 call control */
#define     CC_ID           0x0400                     /* call control */
#define     DMI_ID          0x0303              /* DMI pad and L3 task */
#define     V3_ID           0x0302          /* V120 layer 3 controller */
#define     P3_ID           0x0301       /* X.25 packet level protocol */
#define     L3_ID           0x0300               /* layer 3 controller */
#define     L2_ID           0x020a               /* layer 2 controller */
#define     HDLC_V_ID       0x0206                        /* V120 HDLC */
#define     HDLC_B2_TX_ID   0x0205
#define     HDLC_B2_RX_ID   0x0204
#define     HDLC_B1_TX_ID   0x0203
#define     HDLC_B1_RX_ID   0x0202
#define     HDLC_D_TX_ID    0x0201
#define     HDLC_D_RX_ID    0x0200
#define     HDLC_B2_ID      HDLC_B2_TX_ID
#define     HDLC_B1_ID      HDLC_B1_TX_ID
#define     HDLC_D_XMT_ID   HDLC_D_TX_ID             /* LAPD HDLC Xmit */
#define     HDLC_D_ID       HDLC_D_RX_ID                  /* LAPD HDLC */
#define     HDLC_GENERIC_ID HDLC_D_ID                     /* LAP? HDLC */
#define     KP_ID           0x0105     /* demo board keypad (NSC only) */
#define     WD_ID           0x0103              /* watchdog (NSC only) */
#define     KB_ID           0x0102                         /* keyboard */
#define     UART_ID         0x0101                             /* UART */
#define     L1_ID           0x0100             /* layer 1 (may be SID) */

#ifdef TASK_DPDRV                                   /* DPDRV task id's */
#define     DPDRV_TX_ID     0x070b                    /* DPDRV Tx task */
#define     DPDRV_RX_ID     0x070c                    /* DPDRV Rx task */
#endif




#ifdef TASK_FDL
#define     LOG_ID          0x0710                 /* output formatter */
#define     FDL_ID          0x0705                         /* FDL task */
#define     HDLC_F2_ID      0x0212                        /* HDLC T1-2 */
#define     HDLC_F1_ID      0x0211                        /* HDLC T1-1 */
#define     SCAMP_ID        0x0110                       /* statistics */
#endif

/* Test Task support
*/
#ifdef TASK_1
#define     TASK1_ID        0x0721                      /* Test task 1 */
#endif
#ifdef TASK_2
#define     TASK2_ID        0x0722                      /* Test task 2 */
#endif
#ifdef TASK_3
#define     TASK3_ID        0x0723                      /* Test task 3 */
#endif

#define ALL_ISDN_INTERFACES 0xff   /* Useful while doing a shutdown */

/* Layer to Layer Primitives (see Table 6/Q.921 on page 53)
*
*   Primitive numbering scheme:
*
*   Nibble 1:  always 0
*
*   Nibble 2:  OSI layer (1,2,3), ME (5,6,7,8), Application Layers (f)
*
*   Nibble 3:  name (0-F)       0=EST   1=REL   2=ASS   3=REM
*                               4=DATA  5=UI    6=ACT   7=DEACT
*                               8=XID   9=ERR   A=STAT  B=IID
*                               C=IIC   D=EI1   E=EI2   F=RESET
*
*              alternatives:    2=MAINT 3=MIM 5=CHAN 8=CHAN_DISC
*
*   Nibble 4:  type (0-3)       0=REQ   1=IND   2=RESP  3=CONF
*/

/* Messages for application layers (AL)
*  These primitives use the SourceState field to store a message
*/
#define     AL_ERR_IND          0x0f91
#define     AL_STAT_REQ         0x0fA0
#define     AL_STAT_IND         0x0fA1
#define     MAL_ACT_IND         0x0f61
#define     MAL_ACT_REQ         0x0f60
#define     MAL_DEACT_IND       0x0f71
#define     MAL_RESET_REQ       0x0ff0

/* Messages relating to application layer X.31 call setup and teardown.
*  These primitives use the SourceState field to store message data
*/
#define     AL_CHAN_REQ         0x0F50
#define     AL_CHAN_IND         0x0F51
#define     AL_CHAN_CONF        0x0F53
#define     AL_CHAN_DISC_REQ    0x0F80
#define     AL_CHAN_DISC_IND    0x0F81


/* Management entity (M) to/from Call Control (CC)
*  NOTE:  This message an endanged definition
*/
#define     MCC_DATA_REQ        0x0840
#define     MCC_DATA_IND        0x0841

/* Management entity (M) to/from Network Layer (NL)
*/
#define     MNL_EST_IND         0x0701                          /* FFS */
#define     MNL_EST_CONF        0x0703                          /* FFS */
#define     MNL_MIM_REQ         0x0730                          /* FFS */
#define     MNL_MIM_IND         0x0731                          /* FFS */
#define     MNL_DATA_REQ        0x0740
#define     MNL_DATA_IND        0x0741
#define     MNL_ERR_IND         0x0791
#define     MNL_RESET_REQ       0x07F0
#define     MNL_ACT_IND         0x0761
#define     MNL_DEACT_IND       0x0771

/* Management entity (M) to/from Data Link Layer (DL)
*/
#define     MDL_ASS_REQ         0x0620
#define     MDL_ASS_IND         0x0621
#define     MDL_REM_REQ         0x0630
#define     MDL_DATA_REQ        0x0640                          /* FFS */
#define     MDL_DATA_IND        0x0641                          /* FFS */
#define     MDL_UI_DATA_REQ     0x0650
#define     MDL_UI_DATA_IND     0x0651
#define     MDL_ACT_REQ         0x0660
#define     MDL_XID_REQ         0x0680
#define     MDL_XID_IND         0x0681
#define     MDL_XID_RESP        0x0682
#define     MDL_XID_CONF        0x0683
#define     MDL_ERR_IND         0x0691
#define     MDL_ERR_RESP        0x0692
#define     MDL_RESET_REQ       0x06F0

/* Management entity (M) to/from physical layer (PH)
*/
#define     MPH_DATA_IND        0x0541
#define     MPH_ACT_IND         0x0561
#define     MPH_DEACT_REQ       0x0570
#define     MPH_DEACT_IND       0x0571
#define     MPH_ERR_IND         0x0591

/* NSC ONLY - Management entity (M) to/from physical layer (PH)
*/
#define     MPH_STAT_REQ        0x05A0
#define     MPH_STAT_IND        0x05A1
#define     MPH_IID_IND         0x05B1
#define     MPH_IIC_IND         0x05C1
#define     MPH_EI1_IND         0x05D1
#define     MPH_EI2_IND         0x05E1
#define     MPH_FI2_REQ         0x05F1

/* Network Layer (NL)
*/
#define     NL_EST_REQ          0x0300
#define     NL_EST_CONF         0x0303
#define     NL_REL_REQ          0x0310
#define     NL_REL_IND          0x0311
#define     NL_REL_CONF         0x0313
#define     NL_MAINT_REQ        0x0320
#define     NL_MAINT_IND        0x0321
#define     NL_DATA_REQ         0x0340
#define     NL_DATA_IND         0x0341
#define     NL_UI_DATA_REQ      0x0350
#define     NL_UI_DATA_IND      0x0351
#define     NL_PU_EST_IND       0x0355	/*===*/

/* Data link Layer (DL)
*/
#define     DL_EST_REQ          0x0200
#define     DL_EST_IND          0x0201
#define     DL_EST_RESP         0x0202       /* telenetworks extention */
#define     DL_EST_CONF         0x0203
#define     DL_REL_REQ          0x0210
#define     DL_REL_IND          0x0211
#define     DL_REL_CONF         0x0213
#define     DL_ASS_REQ          0x0220
#define     DL_DATA_REQ         0x0240
#define     DL_DATA_IND         0x0241
#define     DL_DATA_RESP        0x0242       /* telenetworks extention */
#define     DL_DATA_CONF        0x0243       /* telenetworks extention */
#define     DL_UI_DATA_REQ      0x0250
#define     DL_UI_DATA_IND      0x0251

/* Physical Layer (PH)
*/
#define     PH_DATA_REQ         0x0140
#define     PH_DATA_IND         0x0141
#define     PH_ACT_REQ          0x0160
#define     PH_ACT_IND          0x0161
#define     PH_DEACT_IND        0x0171

/* event not defined
*/
#define     UNEXPECTED          0x0ffe
#define     UNDEFINED           0x0fff

/* Timer masks
*/
#define TIMER_MASK_L3           0x1300
#define TIMER_MASK_CC           0x1400

/* LAPD Peer to Peer Commands and Responses (see Table 5/Q.921 on page 43)
*/
#define     I                   0x0000
#define     RR                  0x0001
#define     RNR                 0x0005
#ifdef L2_SREJ
#define     SREJ                0x000D
#else
#define     REJ                 0x0009
#endif
#define     SABME               0x006F
#define     SABM                0x002F
#define     DM                  0x000F
#define     UI                  0x0003
#define     DISC                0x0043
#define     UA                  0x0063
#define     FRMR                0x0087
#define     XID                 0x00AF


/* Layer 3 Peer to Peer Commands and Responses (see Table 44/Q.931)
*/

/* Call Establishment Messages
*/
#define     ALERTING            0x01                       /* Alerting */
#define     CALL_PROC           0x02                /* Call Proceeding */
#define     CONNECT             0x07                        /* Connect */
#define     CONNECT_ACK         0x0F            /* Connect Acknowledge */
#define     PROGRESS            0x03                       /* Progress */
#define     SETUP               0x05                          /* Setup */
#define     SETUP_ACK           0x0D              /* Setup Acknowledge */

/* Call Information Phase Messages
*/
#define     MODIFY              0x27                         /* Modify */
#define     MODIFY_COMP         0x2F                /* Modify Complete */
#define     MODIFY_REJ          0x23                  /* Modify Reject */
#define     RESUME              0x26                         /* Resume */
#define     RESUME_ACK          0x2E             /* Resume Acknowledge */
#define     RESUME_REJ          0x22                  /* Resume Reject */
#define     SUSPEND             0x25                        /* Suspend */
#define     SUSPEND_ACK         0x2D            /* Suspend Acknowledge */
#define     SUSPEND_REJ         0x21                 /* Suspend Reject */
#define     USER_INFO           0x20               /* User Information */

/* Call Clearing Messages
*/
#define     DETACH              0x40                         /* Detach */
#define     DISCONNECT          0x45                     /* Disconnect */
#define     RELEASE             0x4D                        /* Release */
#define     RELEASE_COMP        0x5A               /* Release Complete */
#define     RESTART             0x46                        /* Restart */
#define     RESTART_ACK         0x4E            /* Restart Acknowledge */

/* Miscellaneous Messages
*/
#define     FACILITY_1TR6       0x60                  /* 1TR6 Facility */
#define     FACILITY            0x62                       /* Facility */
#define     STATUS_1TR6         0x63                /* Status for 1TR6 */
#define     REGISTER            0x64                       /* Register */
#define     FACILITY_CANCEL     0x66                /* Facility cancel */
#define     CANCEL_REJ          0x67                /* 1TR6 Cancel Rej */
#define     FACILITY_ACK_1TR6   0x68              /* Facility Ack 1TR6 */
#define     FACILITY_ACK        0x6A           /* Facility acknowledge */
#define     REGISTER_ACK        0x6C                /* Register Accept */
#define     INFO_1TR6           0x6D                      /* 1TR6 Info */
#undef NOTIFY	/* === */
#define     NOTIFY              0x6E                         /* Notify */
#define     CANCEL_ACK          0x6E                /* 1TR6 Cancel Ack */
#define     REGISTER_REJ_1TR6   0x6F       /* Register Reject for 1TR6 */
#define     FACILITY_REJ        0x72                /* Facility reject */
#define     FACILITY_REJ_1TR6   0x65              /* Facility Rej 1TR6 */
#define     REGISTER_REJ        0x74                /* Register Reject */
#define     STATUS_ENQ          0x75                 /* Status Enquiry */
#define     CONG_CON            0x79             /* Congestion Control */
#define     CONG_CON_1TR6       0x69        /* Congestion Control 1TR6 */
#define     INFO                0x7B                    /* Information */
#define     STATUS              0x7D                         /* Status */


/* Commands From Call Control To Layer 3
*/
#define     CC_SETUP_REQ        0x80                  /* Setup Request */
#define     CC_SETUP_RESP       0x81                 /* Setup Response */
#define     CC_SETUP_REJ_REQ    0x82                   /* Setup Reject */
#define     CC_INFO_REQ         0x83                    /* Information */
#define     CC_DISCONNECT_REQ   0x84                     /* Disconnect */
#define     CC_RELEASE_REQ      0x85                        /* Release */
#define     CC_ALERTING_REQ     0x86                       /* Alerting */
#define     CC_BROADCAST_RESP   0x87             /* Broadcast Response */
#define     CC_CALL_PROC_REQ    0x88                /* Call Proceeding */
#define     CC_PROGRESS_REQ     0x89                       /* Progress */
#define     CC_NOTIFY_REQ       0x8A                         /* Notify */
#define     CC_RESUME_REQ       0x8B                         /* Resume */
#define     CC_RESUME_REJ       0x8C                  /* Resume Reject */
#define     CC_SUSPEND_REQ      0x8D                /* Suspend Request */
#define     CC_SUSPEND_REJ      0x8E                 /* Suspend Reject */
#define     CC_RESET_REQ        0x8F               /* L3 Reset Request */
#define     CC_CONNECT_ACK_REQ  0xA0         /* L3 Connect Ack Request */
#define     CC_FACILITY_REQ     0xA1               /* Facility request */
#define     CC_RESUME_REJ_REQ   0xA2          /* Resume reject request */
#define     CC_SUSPEND_REJ_REQ  0xA3         /* Suspend reject request */
#define     CC_RESUME_RESP      0xA4                 /* Resume confirm */
#define     CC_SUSPEND_RESP     0xA5                /* Suspend confirm */
#define     CC_RESTART_REQ      0xA6                /* Restart request */
#define     CC_TONE_DISC_REQ    0xA7   /* DiscReq w/tone, announcement */
#define     CC_MORE_INFO_REQ    0xA8      /* More info req (setup ack) */
#define     CC_STATUS_REQ       0xA9      /* Req for L3 to send STATUS */
#define     CC_CHNG_STATE_REQ   0xAA   /* Req for L3 NLCB state change */
#define     CC_REL_COMP_REQ     0xAB      /* Specific to VN2 error rec */
#define     CC_USER_INFO_REQ    0xAC       /* User information request */
#define     CC_CONG_CON_REQ     0xAD     /* Congestion Control request */
#define     CC_FAC_ACK_REQ      0xAE   /* Facility Acknowledge Request */
#define     CC_FAC_REJ_REQ      0xAF        /* Facility Reject Request */

/* Commands From Layer 3 To Call Control
*/
#define     CC_SETUP_IND        0x90                          /* Setup */
#define     CC_SETUP_CONF       0x91                  /* Setup Confirm */
#define     CC_SETUP_COMP_IND   0x92                 /* Setup Complete */
#define     CC_INFO_IND         0x93                /* Information Ind */
#define     CC_ALERTING_IND     0x94                       /* Alerting */
#define     CC_PROGRESS_IND     0x95                       /* Progress */
#define     CC_DISCONNECT_IND   0x96                     /* Disconnect */
#define     CC_RELEASE_IND      0x97                        /* Release */
#define     CC_RELEASE_IND_QUIET 0xC4 /* Release w/ no RelCom from CC === */
#define     CC_CALL_PROC_IND    0x98                /* Call Proceeding */
#define     CC_RELEASE_CONF     0x99                /* Release Confirm */
#define     CC_STATUS_IND       0x9A              /* Status Indication */
#define     CC_ERROR_IND        0x9B                     /* Error Ind. */
#define     CC_RESUME_CONF      0x9C                 /* Resume Confirm */
#define     CC_NOTIFY_IND       0x9D              /* Notify Indication */
#define     CC_SUSPEND_CONF     0x9E                /* Suspend Confirm */
#define     CC_RESET_CONF       0x9F               /* L3 Reset Confirm */
#define     CC_FACILITY_ACK     0xB0           /* Facility acknowledge */
#define     CC_FACILITY_REJ     0xB1                /* Facility reject */
#define     CC_TIMEOUT_IND      0xB2                 /* Timeout report */
#define     CC_USER_INFO_IND    0xB3    /* User information indication */
#define     CC_RESTART_CONF     0xB4           /* Restart confirmation */
#define     CC_RESTART_IND      0xB5             /* Restart indication */
#define     CC_FACILITY_IND     0xB6            /* Facility indication */
#define     CC_MORE_INFO_IND    0xB7           /* More info indication */
#define     CC_CONG_CON_IND     0xB8   /* Congestion Control idication */
#define     CC_REGISTER_ACK     0xB9           /* Register acknowledge */
#define     CC_REGISTER_REJ     0xBA                /* Register reject */
#define     CC_CANCEL_ACK       0xBB             /* Cancel acknowledge */
#define     CC_CANCEL_REJ       0xBC                  /* Cancel reject */

/* Commands From Call Control To Layer 3
*/
#define     CC_FAC_CANCEL_REQ   0xC0        /* Facility cancel request */
#define     CC_DETACH_REQ       0xC1                 /* Detach request */
#define     CC_REGISTER_REQ     0xC2      /* Facility register request */
#define     CC_RESTART_ACK_REQ  0xC3    /* Restart acknowledge request */
/* #define	CC_RELEASE_IND_QUIET 0xC4	(* defined above === */
#define     CC_KEY_HOLD_IND     0xD0            /*===*/


#ifdef BRI_5ESS
/* ATT message coding scheme
*/
#define     SS_MASK             0x0100

/* MEMIM messages between ME and L3
*/
#define     MAN_INFO            (SS_MASK|0xf7)
#define     MAN_REQ             (SS_MASK|0xe7)
#endif


/* Maintenance message types from and to Layer 2.
*/
#define     SERVICE             0x0F      /* B-Channel Service message */
#define     SERVICE_ACK         0x07  /* B-Channel Service Acknowledge */

/* Keystroke Commands From Key Pad Driver To Call Control Task
*/
#define     KP_DIGIT_1          '1'                         /* Digit 1 */
#define     KP_DIGIT_2          '2'                         /* Digit 2 */
#define     KP_DIGIT_3          '3'                         /* Digit 3 */
#define     KP_DIGIT_4          '4'                         /* Digit 4 */
#define     KP_DIGIT_5          '5'                         /* Digit 5 */
#define     KP_DIGIT_6          '6'                         /* Digit 6 */
#define     KP_DIGIT_7          '7'                         /* Digit 7 */
#define     KP_DIGIT_8          '8'                         /* Digit 8 */
#define     KP_DIGIT_9          '9'                         /* Digit 9 */
#define     KP_DIGIT_0          '0'                         /* Digit 0 */
#define     KP_DIGIT_STAR       '*'                         /* Digit * */
#define     KP_DIGIT_POUND      '#'                         /* Digit # */
#define     KP_HOOK_FLASH       0xFD                      /* hookflash */
#define     KP_ON_HOOK          0xFE                        /* On Hook */
#define     KP_OFF_HOOK         0xFF                       /* Off Hook */

/* Status Commands From Call Control Task To Tracer
*/
#define     TR_IDLE             0                     /* Idle, ON HOOK */
#define     TR_DIALTONE         1                         /* Dial Tone */
#define     TR_DIALING          2                           /* Dialing */
#define     TR_RINGING          3                           /* Ringing */
#define     TR_BUSY             4                              /* Busy */
#define     TR_CONVERSATION     5                      /* Conversation */
#define     TR_RINGBACK         6                          /* Ringback */
#define     TR_ERROR            7                             /* Error */
#define     TR_COLLECT          8                 /* Collecting digits */
#define     TR_LOCKOUT          9                           /* lockout */
#define     TR_DATA            10                     /* data transfer */
#define     TR_CALLWAIT        11                      /* call waiting */
#define     TR_ONHOLD          12                      /* call on hold */
#define     TR_RELEASING       13                    /* call releasing */
#define     TR_RECONNECT       14                         /* reconnect */
#define     TR_SUSPENDED       15                         /* suspended */
#define     TR_ROUTING         16                           /* routing */

/* Commands From Call Control To Management Entity
*/
#define     CC_ON_HOOK          KP_ON_HOOK                  /* On Hook */
#define     CC_OFF_HOOK         KP_OFF_HOOK                /* Off Hook */

/* Messages from Call Control (CC_ID) to Phone Driver (KB_ID):
*  first nibble: 0 = timeouts;       1 = speaker tones;  2 = handset tones;
*                3 = B1/B2 connect;  4 = DTMF receiver;  5 = HSW state.
*  second nibble: 0 = OFF;  1 ... = tone value.
*/
#ifdef SBC_INTERFACE
/* the new BRI Call Control requires the KP_ prefixes
*/
#define KP_STOP_RING          0x10
#define KP_START_RING         0x11
#define KP_START_CW           0x12
#define KP_START_DATARING     0x13
#define KP_STOP_DATARING      0x14
#define KP_TEST_CW_RING       0x1e
#define KP_TEST_RING          0x1f
#define KP_STOP_TONE          0x20
#define KP_START_SPECIALDT    0x21
#define KP_START_DIALTONE     0x22
#define KP_START_RINGBACK     0x23
#define KP_START_BUSYTONE     0x24
#define KP_DISCONNECT_CHAN    0x30
#define KP_CONNECT_B1         0x31
#define KP_CONNECT_B2         0x32
#define KP_DTMF_DETECT_OFF    0x40
#define KP_DTMF_DETECT_ON     0x41
#else
#define STOP_RING             0x10
#define START_RING            0x11
#define START_CW              0x12
#define START_DATARING        0x13
#define STOP_DATARING         0x14
#define TEST_CW_RING          0x1e
#define TEST_RING             0x1f
#define STOP_TONE             0x20
#define START_SPECIALDT       0x21
#define START_DIALTONE        0x22
#define START_RINGBACK        0x23
#define START_BUSYTONE        0x24
#define KP_DISCONNECT_B1      0x28
#define KP_DISCONNECT_B2      0x27
#define DISCONNECT_CHAN       0x30
#define CONNECT_B1            0x31
#define CONNECT_B2            0x32
#define DTMF_DETECT_OFF       0x40
#define DTMF_DETECT_ON        0x41
#define MOD_HANDSET_VOL       0xf0                   /* special case ! */
#endif

/* messages from HDLC driver to Phone Driver (KB_ID) relating the state
*  of the hook switch (this is done in the 79C30 chip).
*/
#define HSW_OFF            0x50
#define HSW_ON             0x51


#ifdef COMP_TEST
/* Special keys to condition CC to perform in special ways to satisfy
*  individual Switch vendor Compatibility tests.
*/
#define     DIGIT_COMP_TEST_0   0x10 /* Compatibility test special key */
#define     DIGIT_COMP_TEST_1   0x11 /* Compatibility test special key */
#define     DIGIT_COMP_TEST_2   0x12 /* Compatibility test special key */
#define     DIGIT_COMP_TEST_3   0x13 /* Compatibility test special key */
#define     DIGIT_COMP_TEST_4   0x14 /* Compatibility test special key */
#define     DIGIT_COMP_TEST_5   0x15 /* Compatibility test special key */
#define     DIGIT_COMP_TEST_6   0x16 /* Compatibility test special key */
#define     DIGIT_COMP_TEST_7   0x17 /* Compatibility test special key */
#define     DIGIT_COMP_TEST_8   0x18 /* Compatibility test special key */
#define     DIGIT_COMP_TEST_9   0x19 /* Compatibility test special key */

#endif                                                    /* COMP_TEST */
#ifdef BRI
#define HOST_MSG           0x01
#endif

/* Values used in CALL_CONGESTION message.
*/
#define CONGESTION_IE      0x31                      /* Channel status */

/* Values used in CHAN_STATUS message.
*/
#define CHAN_STATUS_IE     0x30                      /* Channel status */
/* "action" values
*/
#define CHAN_RESTARTED     0x01           /* The channel was restarted */
#define CHAN_MAINTENANCE   0x02    /* The channel received a maint msg */
#define CHAN_ENQUIRY       0x03        /* Host requests channel status */
#define CHAN_RESULT        0x04      /* CC responds to CHANNEL_ENQUIRY */
#define CHAN_RESTART_REQ   0x05               /* Local restart request */
#define CHAN_RESTART_ERR   0x06   /* Locally originated restart failed */
#define CHAN_MAINT_REQ     0x07      /* Host requests chan maintenance */

/* Values used in CALL_PROGRESS message.
*/
#define CALLED_ALERTING    0x81           /* For CALL_PROGRESS message */
#define CALLED_ANSWER      0x82           /* For CALL_PROGRESS message */
#define INTERNAL_ERROR     0x83       /* CC or HOST had internal error */
#define MESSAGE_ERROR      0x84  /* Msg error received from HOST or CC */
#define STATUS_REPORT      0x85       /* Report of peer STATUS message */
#define MORE_DIGITS        0x86        /* Caller routing digit request */
#define CALL_CONNECTED     0x87                /* Connect Ack Received */

/* Events - between HOST and Call Control
*/
#define     HOST_CALL_REQUEST       0x01
#define     HOST_INCOMING_CALL      0x02
#define     HOST_ALERTING           0x03
#define     HOST_CONNECT            0x04
#define     HOST_DISCONNECT         0x05
#define     HOST_DISCONNECT_ACK     0x06
#define     HOST_PROCEEDING         0x07
#define     HOST_MORE_INFO          0x08
#define     HOST_PROGRESS           0x09
#define     HOST_QUERY              0x0A
#define     HOST_QUERY_RESPONSE     0x0B
#define     HOST_INFORMATION        0x0C
#define     HOST_CALL_SENT          0x07
#define     HOST_FACILITY           0x0D
#define     HOST_FACILITY_ACK       0x0E
#define     HOST_FACILITY_REJ       0x0F
#define     HOST_FACILITY_REGISTER  0x10
#define     HOST_REGISTER_ACK       0x11
#define     HOST_REGISTER_REJ       0x12
#define     HOST_FACILITY_CANCEL    0x13
#define     HOST_CANCEL_ACK         0x14
#define     HOST_CANCEL_REJ         0x15
#define     HOST_STATUS             0x16
#define     HOST_TERM_REGISTER      0x17	/*===*/
#define     HOST_TERM_REGISTER_ACK  0x18	/*===*/
#define     HOST_TERM_REGISTER_NACK 0x19	/*===*/
#define     HOST_KEY_HOLD           0x1A	/*===*/

/* Message IDs for PRI multi-byte messages between a Telenetworks
 * call control and some higher (HOST) application.
 * Allows both BRI and PRI names to exist. ===
 */
#define CALL_ACCEPT        0x1A  /* Incoming or outgoing call accepted */
#define CALL_DISC          0x1B/* Incoming or outgoing call disconnect */
#define CALL_INCOMING      0x1C           /* Incoming call to the host */
#define CALL_OUTGOING      0x1D         /* Outgoing call from the host */
#define CALL_PROGRESS      0x1E/* Call status change (either direction */
#define CALL_REJECTION     0x1F  /* Incoming or outgoing call rejected */
#define CALL_UUI           0x20      /* UUI string in either direction */
#define CALL_FACILITY      0x21         /* Network facility indication */
#define CALL_FACILITY_ACK  0x22                        /* Facility ACK */
#define CALL_FACILITY_REJ  0x23                        /* Facility REJ */
#define CALL_CONGESTION    0x24        /* Call congestion report (UUI) */
#define CALL_NOTIFY        0x25            /* Call notify to/from HOST */
#define CHAN_STATUS        0x26         /* Channel status to/from HOST */
#define CALL_CLEARED       0x27          /* Call_ID is cleared to HOST */
#define CALL_INFO          0x28                 /* Information message */
#define CALL_FACILITY_REGISTER    0x29            /* Facility Register */
#define CALL_REGISTER_ACK         0x2A          /* Ack of Registration */
#define CALL_REGISTER_REJ         0x2B        /* Rej of Registration * */
#define CALL_FACILITY_CANCEL      0x2C              /* Facility Cancel */
#define CALL_CANCEL_ACK           0x2D                   /* Cancel Ack */
#define CALL_CANCEL_REJ           0x2E                /* Cancel Reject */
#define CALL_RELEASE       0x2F /* VN2 Release with UUI or Redirection */
#define CALL_CONNECT       0x30                     /* Connect message */

/* Broadcast constants
*/
#define     BROADCAST_TEI       127                       /* group TEI */
#define     BROADCAST_CES       0                         /* group CES */

/* SAPI values (see Table 2/Q.921)
*/
#define     Q931_SAPI           0        /* SAPI data on the D channel */
#define     X25_SAPI            16       /* SAPI data on the D channel */
#ifdef BRI_DMS100
#define     TERM_LOOP_SAPI      17      /* SAPI terminal loopback test */
#endif
#define     ME_SAPI             63       /* SAPI for management entity */
#define     LAPB_SAPI           64                   /* fake LAPB SAPI */

/* Mail priority
*/
#define     REGULAR_MAIL        10         /* priority of regular mail */
#define     UP_MAIL             10    /* priority of upward bound mail */
#define     DOWN_MAIL           10  /* priority of downward bound mail */
#define     EXPRESS_MAIL        255 /* express mail - highest priority */

/* Mail Delay
*/
#define     NOW                 (UINT32) 0     /* send mail right now! */

/* Miscellaneous constants
*/
#define     UNASSIGNED          0XFF    /* unassigned ces, tei or sapi */
#define     LAPD_OFFSET         2  /* offset to info part of LAPB data */
#define     UI_DATA_OFFSET      3    /* offset to info part of UI data */
#define     DATA_OFFSET         4     /* offset to info part of I data */
#define     Qdot931             0x08                /* layer 3 message */
#define     MaintProto          0x03  /* Maint msg Proto Discriminator */
#define     TERM_UNREGISTERED   0x01 /* For NI-1 Term Registration === */
#ifdef BRI_1TR6
#define     EDITOR_PD_1TR6      0x40  /* Editor protocol discriminator */
#define     TRANSACTION_PD_1TR6 0x41   /* Transaction protocol discrim */
#endif
#ifdef PRI_1TR6
#define     EDITOR_PD_1TR6      0x40  /* Editor protocol discriminator */
#define     TRANSACTION_PD_1TR6 0x41   /* Transaction protocol discrim */
#endif
#define     Q931_DSL_CHAN_ID    0              /* D-Channel identifier */
#define     DSS1_D_CHANNEL      Q931_DSL_CHAN_ID
#ifdef USER
#define     SIGNALING_CES       1
/* note: VOICE_DEFAULT_CES will be removed and replaced by SIGNALING_CES
*/
#define     VOICE_DEFAULT_CES   1
#define     DATA_DEFAULT_CES    1
#endif


/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S
*************************************************************************
*/

#include	"switch_def.h"		/* Used to be inline === */

/*
 * Memory Pool Handles
 */

typedef enum {
    PRIM_BTYPE,
    PKT_BTYPE,
    HEADER_BTYPE,
    SML_INFO_BTYPE,
    LRG_INFO_BTYPE,
    PKG_BTYPE
} BUFFER_TYPE;

typedef enum 
{
	IF_ACTIVATING,
	IF_ACTIVE,
	IF_ACTIVE_EI1,
	IF_DOWN
} INTF_STATES;

/* Packet structure (the 'payload' of a primitive)
*       - L2 uses primarily the header section of a packet, although it
*         may use the info section for communication with the management
*         entity.  L2 refers to the header and info sections combined
*         as a 'frame'.
*       - L3 uses only the info section of a packet, which it refers to
*         as the 'message unit'.
*/
struct Packet
{
   INT16 HeadLen;                                     /* header length */
   BYTE *Header;                                      /* packet header */
   INT16 InfoLen;                                       /* info length */
   BUFFER_TYPE InfoType;                                  /* info type */
   BYTE *Info;                                          /* packet info */
};
typedef struct Packet *PKT_ID, PKT;

/* Primitive Structure (used for layer-to-layer communication)
*/
struct Primitive
{
   BUFFER_TYPE BufType;                /* type of buffer (PRIM or SBC) */
   INT16       L2_Protocol;          /* L2P_LAPD, L2P_LAPB or L2P_V120 */
   INT16       Source_ID;                                 /* sender id */
   BYTE        SourceState;                            /* sender state */
   INT16       Dest_ID;                              /* destination id */
   INT16       Prim_ID;                             /* primitive event */
   BYTE        Sapi;                                           /* SAPI */
   BYTE        Ces;                                      /* CES or TEI */
   INT16       LLI;                          /* logical link id (V120) */
   UINT16      Call_ID;                   /* call ID or call reference */
   BYTE        Dsl_ID;                      /* Digital Subscriber Loop */
   INT16       DSL_Chan_ID;      /* Digital Subscriber Loop channel ID */
   BOOLEAN     FreePkt;          /* OK for destination to free packet? */
   PKT_ID      Pkt;                                         /* payload */
};
typedef struct Primitive PRIM, *PRIM_ID;

#ifndef NO_PKG
/* Package structure:
*  A "package" is not a "packet". A package is a general
*  purpose way to mail around any of the various buffer types.
*/
typedef struct
{
   BUFFER_TYPE     BufType;                    /* buffer type for info */
   INT16           SourceID;                              /* sender ID */
   INT16           DestID;               /* receiver ID (for TaskView) */
   INT16           InfoLen;                                  /* length */
   BUFFER_TYPE     InfoBufType;    /* type of buffer user for pkg info */
   BYTE            MsgType;        /* handy for decoding info contents */
   BYTE *          PkgInfo;                            /* package info */
}
PKG, * PKG_ID;
#endif                                                       /* NO_PKG */

/************************************************************************
*                              M A C R O S
*************************************************************************
*/

#define     MTEX_ClearTimer(A,B,C)   ClearTimer((A),(B),(C))

#define     MTEX_ReadMail(A,B)       ReadMail((A),(B))
#define     MTEX_ReturnBlock(A,B)    ReturnBlock((A),(B))
#define     MTEX_SendMail(A,B,C,D)   SendTMail((A),(B),(C),(D),0)

/*  CUSTOMIZE HERE - MTEX_SetTimer uses a priority of 0 (lowest).
*   This may not suit all applications!!
*/
#define     MTEX_SetTimer(A,B,C,D,E) SetTimer((A),(B),(C),(D),(E),0)

#define     LIF_AddTimer(A,B,C,D,E,F,G,H,I) \
            LIF_AddTimerP((A),(B),(C),(D),(E),(F),(G),(H),(I),NULL)

/* Offset to info (layer 3) portion of I frames is 4 for LAPD, 2 for LAPB
*/
#define     LIF_I_DATA_OFFSET(DSLCID)     ((DSLCID==0)?4:2)

/*
 * Offset to info (layer 3) portion of UI frames is 3 for LAPD, 2 for LAPB
 */
#define     LIF_UI_DATA_OFFSET(DSLCID)    ((DSLCID==0)?3:2)

/*
 * Memory block lengths must be as least as big as a pointer and
 * should be an even word size. The following macro can be used to
 * insure that the memory block sizes meet these requirements.
 *
 * !!! IMPORANT NOTE !!!
 * These macros affect memory pool block size. If running on more than one
 * processor (i.e. using DPDRV), make sure the block sizes will be compatible.
 */
#define ROUNDUP(s)   ((((s)+sizeof(int)-1)/sizeof(int))*sizeof(int))
#define BLKSIZE(s)   ((ROUNDUP(s)>sizeof(int *))?ROUNDUP(s):sizeof(int *))


/************************************************************************
*                     D A T A   A L L O C A T I O N
*************************************************************************
*/

#ifdef LIF_C       /* allocate storage for vars shared between modules */

BOOLEAN TimersOn = TRUE;             /* flag to en/disable task timers */
SWITCH_TYPE SwitchType = DEFAULT_ISDN_SWITCH;  /* === */   /* equipment type */
SWITCH_TYPE SubSwitchType = NO_ISDN_SWITCH;  /* === */   /* equipment type */
BOOLEAN FitTerm = FALSE;			/* === added for spid usage */
int     Trashcan;                   /* A general purpose disposal site */
extern void gdb_kernel_enable(void);

#ifdef MEM_STATS
int LIF_UsedTimers;                           /* # of timers allocated */
int LIF_MaxTimers;                /* high water mark of LIF_UsedTimers */
#endif                                                    /* MEM_STATS */

#else                         /* reference vars shared between modules */

extern BOOLEAN TimersOn;
extern SWITCH_TYPE SwitchType;
extern SWITCH_TYPE SubSwitchType;
extern int Trashcan;
extern BOOLEAN is_pri;

#ifdef MEM_STATS
extern int LIF_UsedTimers;
#ifdef NEVER_REFERENCED
extern int LIF_MaxTimers;
#endif /* NEVER_REFERENCED */
#endif                                                    /* MEM_STATS */

/*
 * Counters for the memory usage
 */
extern int Used_dlcb;
extern int Used_nlcb;
 
extern int Used_prim;
extern int Used_pkt;
extern int Used_header;
extern int Used_small;
extern int Used_lrg;
extern int Used_pkg;

#endif                                                        /* LIF_C */

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

PUBLIC  INT16       LIF_AddTimerP   (INT16, INT16, INT16, BYTE, BYTE, \
                                          BYTE, INT16, INT16, UINT32, PKT_ID);
PUBLIC  void        LIF_Block       (int);
PUBLIC  void        LIF_CpyPkt      (PKT_ID, PKT_ID);
PUBLIC  INT16       LIF_CpyPrim     (PRIM_ID, PRIM_ID);
PUBLIC  void        LIF_Dispose     (void *);
#ifdef DEBUG
#ifndef NO_PKG
PUBLIC  void        LIF_DispPkg     (PKG_ID);
#endif
PUBLIC  void        LIF_DispPkt     (PKT_ID);
PUBLIC  void        LIF_DispPrim    (PRIM_ID);
#endif
PUBLIC  void        LIF_EnterCS     (void);
PUBLIC  void        LIF_EnterICS    (void);
PUBLIC  void        LIF_EnterNPS    (void);
PUBLIC  void        LIF_Error       (char *, char *, char *);
PUBLIC  void        LIF_ExitCS      (void);
PUBLIC  void        LIF_ExitICS     (void);
PUBLIC  void        LIF_ExitNPS     (void);
PUBLIC  void        LIF_Fatal       (char *, char *, char *);
PUBLIC  void        LIF_FreeBuffer  (BUFFER_TYPE, BYTE *);
#ifndef NO_PKG
PUBLIC  void        LIF_FreePkg     (PKG_ID);
#endif
PUBLIC  void        LIF_FreePkt     (PKT_ID);
PUBLIC  void        LIF_FreePrim    (PRIM_ID);
PUBLIC  BYTE        *LIF_GetBuffer  (BUFFER_TYPE, BOOLEAN);
PUBLIC  INT16       LIF_GetBufSize  (BUFFER_TYPE);
#ifdef TASK_L2
PUBLIC  int         LIF_GetDriverID (int, int, int);
#endif
#ifndef NO_PKG
PUBLIC  PKG_ID      LIF_GetPkg      (void);
#endif
PUBLIC  PKT_ID      LIF_GetPkt      (BUFFER_TYPE, INT16, BOOLEAN);
PUBLIC  PRIM_ID     LIF_GetPrim     (BOOLEAN);
#ifdef EXEC_H
PUBLIC  MAILBOX_ID  LIF_ID_To_MB    (INT16);
#endif
PUBLIC  void        LIF_Init        (void);
#ifndef NO_PK
PUBLIC  INT16       LIF_MailPkg     (PKG_ID, UINT32, BYTE, unsigned );
#endif
PUBLIC  INT16       LIF_MailPrim    (PRIM_ID, INT16, INT16, UINT32, \
                                          BYTE, unsigned );
PUBLIC  INT16       LIF_MailSlotsAvail (INT16, INT16);
PUBLIC  INT16       LIF_MailSlotsLimit (INT16, INT16);
PUBLIC  void        LIF_N_Cpy       (BYTE *, BYTE *, int);
PUBLIC  BYTE        *LIF_ReadMail   ( INT16 );
PUBLIC  void        LIF_RemoveTimer (INT16);
#ifndef NO_PKG
PUBLIC  INT16       LIF_SendBuf     (INT16, INT16, INT16, BUFFER_TYPE, \
                                          BYTE *, BYTE, UINT32, BYTE);
#endif
PUBLIC  INT16       LIF_SendPkt     (INT16, INT16, BYTE, INT16, INT16, \
                                          BYTE, BYTE, BYTE, INT16, INT16, \
                                          INT16, BOOLEAN, PKT_ID, UINT32, \
                                          BYTE);
PUBLIC  INT16       LIF_Sleep       (INT16);
PUBLIC  void        LIF_StartTimer  (INT16);
PUBLIC  void        LIF_StopTimer   (INT16);
PUBLIC  BOOLEAN     LIF_TestMail    (INT16);
PUBLIC  BOOLEAN     LIF_TimerRunning (INT16);
#ifdef TRACING
PUBLIC  BOOLEAN     LIF_TraceOk     (int, int);
#ifndef NO_PKG
PUBLIC  int         LIF_TracePkg    (PKG_ID);
#endif
PUBLIC  int         LIF_TracePrim   (PRIM_ID);
#endif
PUBLIC  INT16       LIF_UpdateTimer (INT16, INT16, INT16, INT16, BYTE, \
                                          BYTE, BYTE, INT16, INT16, UINT32);
#define	MY_DEF_LLD	0xd3	/* a random single-byte number */
#endif
