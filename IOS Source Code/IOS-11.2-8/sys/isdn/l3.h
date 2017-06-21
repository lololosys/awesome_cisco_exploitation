/* $Id: l3.h,v 3.3.4.2 1996/04/30 23:07:41 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/l3.h,v $
 *------------------------------------------------------------------
 * l3.h  ISDN Layer 3 (Network Layer) include file
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: l3.h,v $
 * Revision 3.3.4.2  1996/04/30  23:07:41  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3.4.1  1996/04/11  21:43:58  dclare
 * CSCdi52751:  Make DMS-100 ISDN PRI changes to conform to the
 * Specification
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/08  23:44:54  smackie
 * Fix various signed/unsigned comparison conflicts that cause the
 * 95q4 toolchain to blow it's cookies. (CSCdi49069)
 *
 * Revision 3.2  1995/11/17  17:41:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:16:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       l3.h                                                    
DESCRIPTION:  ISDN Layer 3 (Network Layer) include file               
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

This include file contains definitions that are shared between both the
L3 core module (l3.c) and the L3 SDL module (l3_sdl_u or l3_sdl_n).

*************************************************************************
*************************************************************************
*/

#ifndef     L3_H                          /* prevent nested inclusions */
#define     L3_H                                        /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

#ifdef USER
typedef enum
   {
   /*
   *  Q.931 User States:
   */
   NULL_STATE,                                        /* Q.931 state 0 */
   CALL_INIT,                                         /* Q.931 state 1 */
   OVERLAP_SENDING,                                   /* Q.931 state 2 */
   OUT_CALL_PROC,                                     /* Q.931 state 3 */
   CALL_DELIVERED,                                    /* Q.931 state 4 */
   UNDEFINED_STATE_5,                               /* Undefined state */
   CALL_PRESENT,                                      /* Q.931 state 6 */
   CALL_RECEIVED,                                     /* Q.931 state 7 */
   CONNECT_REQUEST,                                   /* Q.931 state 8 */
   IN_CALL_PROC,                                      /* Q.931 state 9 */
   CALL_ACTIVE,                                      /* Q.931 state 10 */
   DISCONNECT_REQUEST,                               /* Q.931 state 11 */
   DISCONNECT_IND,                                   /* Q.931 state 12 */
   UNDEFINED_STATE_13,                              /* Undefined state */
   CALL_PASSIVE,                                 /* 1TR6 P-MP state 14 */
   SUSPEND_REQUEST,                                  /* Q.931 state 15 */
   UNDEFINED_STATE_16,                              /* Undefined state */
   RESUME_REQUEST,                                   /* Q.931 state 17 */
   UNDEFINED_STATE_18,                              /* Undefined state */
   RELEASE_REQUEST,                                  /* Q.931 state 19 */
   REGISTER_REQUEST,                                  /* 1TR6 state 20 */
   CANCEL_REQUEST,                                    /* 1TR6 state 21 */
   UNDEFINED_STATE_22,                              /* Undefined state */
   UNDEFINED_STATE_23,                              /* Undefined state */
   UNDEFINED_STATE_24,                              /* Undefined state */
   OVERLAP_RECEIVE,                                  /* Q.931 state 25 */
   /* Data Link Establishment States
   */
   DL_IDLE_STATE,                         /* Idle Data Link - state 98 */
   ESTABLISH_WAIT,                           /* Waiting Est - state 99 */
   /* ASAI PROTOCOL additional states
   */
   ASAI_TR1_STATE,               /* additional state for ASAI protocol */
   /* 5ESS BRI supplementry voice services states
   */
   U10_HOLD_REQUEST,                                    /* state 30-10 */
   U10_TRANSFER_REQUEST,                                /* state 31-10 */
   U10_CONFERENCE_REQUEST,                              /* state 32-10 */
   U10_RECONNECT_REQUEST,                               /* state 33-10 */
   U10_AWAITING_DISC,                                      /* state 34 */
   U10_CALL_ON_HOLD,                                    /* state 36-10 */
   /* Broadcast State
   */
   BROADCAST_STATE                                  /* Broadcast State */
   }
L3_STATE;
#else                                                       /* Network */
typedef enum
   {
   /*
   *  Q.931 User States:
   */
   NULL_STATE,                                        /* Q.931 state 0 */
   CALL_INIT,                                         /* Q.931 state 1 */
   OVERLAP_SENDING,                                   /* Q.931 state 2 */
   OUT_CALL_PROC,                                     /* Q.931 state 3 */
   CALL_DELIVERED,                                    /* Q.931 state 4 */
   UNDEFINED_STATE_5,                               /* Undefined state */
   CALL_PRESENT,                                      /* Q.931 state 6 */
   CALL_RECEIVED,                                     /* Q.931 state 7 */
   CONNECT_REQUEST,                                   /* Q.931 state 8 */
   IN_CALL_PROC,                                      /* Q.931 state 9 */
   CALL_ACTIVE,                                      /* Q.931 state 10 */
   DISCONNECT_REQUEST,                               /* Q.931 state 11 */
   DISCONNECT_IND,                                   /* Q.931 state 12 */
   UNDEFINED_STATE_13,                              /* Undefined state */
   UNDEFINED_STATE_14,                              /* Undefined state */
   SUSPEND_REQUEST,                                  /* Q.931 state 15 */
   UNDEFINED_STATE_16,                              /* Undefined state */
   RESUME_REQUEST,                                   /* Q.931 state 17 */
   UNDEFINED_STATE_18,                              /* Undefined state */
   RELEASE_REQUEST,                                  /* Q.931 state 19 */
   UNDEFINED_STATE_20,                              /* Undefined state */
   UNDEFINED_STATE_21,                              /* Undefined state */
   CALL_ABORT,                                      /* DMS100 State 22 */
   UNDEFINED_STATE_23,                              /* Undefined state */
   UNDEFINED_STATE_24,                              /* Undefined state */
   OVERLAP_RECEIVE,                                  /* Q.931 state 25 */
   /* Data Link Establishment States
   */
   DL_IDLE_STATE,                         /* Idle Data Link - state 98 */
   ESTABLISH_WAIT,                           /* Waiting Est - state 99 */
   /* Broadcast State
   */
   BROADCAST_STATE
   }
L3_STATE;
#endif

#define MAX_L3_STATE       BROADCAST_STATE

/* Internal Events -- 1st nibble is 1; 2nd nibble is OSI layer (3)
*/
#define  T302_EXPIRY       0x1310              /* Send Setup Ack Timer */
#define  T303_EXPIRY       0x1320                       /* Setup Timer */
#define  T304_EXPIRY       0x1330             /* Rcv Suspend Ack Timer */
#define  T305_EXPIRY       0x1340                  /* Disconnect Timer */
#define  T306_EXPIRY       0x1350           /* Disconnect Timer w/tone */
#define  T308_EXPIRY       0x1360                     /* Release Timer */
#define  T309_EXPIRY       0x1370              /* Data Link Disc Timer */
#define  T310_EXPIRY       0x1380             /* Call Proceeding Timer */
#define  T312_EXPIRY       0x1390             /* Broadcast Setup Timer */
#define  T313_EXPIRY       0x13A0                 /* Connect Ack Timer */
#define  T318_EXPIRY       0x13C0                  /* Resume Ack Timer */
#define  T319_EXPIRY       0x13D0                 /* Suspend Ack Timer */
#define  T322_EXPIRY       0x13E0              /* Status Enquiry Timer */
#ifdef VN2
#define  T399_EXPIRY       0x13F0                /* VN2 Register Timer */
#else
#define  T3M1_EXPIRY       0x13F0            /* AT&T Service Ack Timer */
#endif

#define TWAIT_EXPIRY  0x13B0      /* Timer for NI interface activation */

/* Timer Default Values
*/
#ifdef USER
#ifndef L3_TEST                      /* These are the NON-TEST values. */
#ifdef BRI_5ESS                         /* STANDARD VALUES - USER SIDE */
#define   BRI_5ESS_T303      (UINT32)   2500            /* 2.5 seconds */
#define   BRI_5ESS_T305      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T308      (UINT32)   4000              /* 4 seconds */
#endif                                                     /* BRI_5ESS */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_4ESS                         /* STANDARD VALUES - USER SIDE */
#define   PRI_4ESS_T303      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T305      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   PRI_4ESS_T310      (UINT32)  30000             /* 30 seconds */
#define   PRI_4ESS_T313      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T316      (UINT32) 120000L             /* 2 minutes */
#define   PRI_4ESS_T3M1      (UINT32) 120000L             /* 2 minutes */
#endif                                                     /* PRI_4ESS */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_5ESS                         /* STANDARD VALUES - USER SIDE */
#define   PRI_5ESS_T303      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T305      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   PRI_5ESS_T310      (UINT32)  10000             /* 10 seconds */
#define   PRI_5ESS_T313      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T316      (UINT32) 120000L             /* 2 minutes */
#define   PRI_5ESS_T3M1      (UINT32) 120000L             /* 2 minutes */
#endif                                                     /* PRI_5ESS */

#ifdef BRI_NI1 /*===*/                  /* STANDARD VALUES - USER SIDE * */
#define   BRI_NI1_T302    (UINT32)  10000             /* 10 seconds */
#define   BRI_NI1_T303    (UINT32)   4000              /* 4 seconds */
#define   BRI_NI1_T304    (UINT32)  15000             /* 15 seconds */
#define   BRI_NI1_T305    (UINT32)  30000             /* 30 seconds */
#define   BRI_NI1_T306    (UINT32)  30000             /* 30 seconds */
#define   BRI_NI1_T308    (UINT32)   4000              /* 4 seconds */
#define   BRI_NI1_T309    (UINT32)  30000L            /* 30 seconds */
#define   BRI_NI1_T310    (UINT32)   4000              /* 4 seconds */
#define   BRI_NI1_T312    (UINT32)   4500            /* 4.5 seconds */
#define   BRI_NI1_T313    (UINT32)   4000              /* 4 seconds */
#define   BRI_NI1_T316    (UINT32) 120000L             /* 2 minutes */
#define   BRI_NI1_T318    (UINT32)   4000              /* 4 seconds */
#define   BRI_NI1_T319    (UINT32)   4000              /* 4 seconds */
#define   BRI_NI1_T322    (UINT32)   4000              /* 4 seconds */
#endif

#ifdef BRI_DMS100                     /* STANDARD VALUES - USER SIDE * */
#define   BRI_DMS100_T302    (UINT32)  10000             /* 10 seconds */
#define   BRI_DMS100_T303    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T304    (UINT32)  15000             /* 15 seconds */
#define   BRI_DMS100_T305    (UINT32)  30000             /* 30 seconds */
#define   BRI_DMS100_T306    (UINT32)  30000             /* 30 seconds */
#define   BRI_DMS100_T308    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T309    (UINT32)  90000L            /* 90 seconds */
#define   BRI_DMS100_T310    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T312    (UINT32)   4500            /* 4.5 seconds */
#define   BRI_DMS100_T313    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T316    (UINT32) 120000L             /* 2 minutes */
#define   BRI_DMS100_T318    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T319    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T322    (UINT32)   4000              /* 4 seconds */
#endif                                                   /* BRI_DMS100 */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_DMS100                       /* STANDARD VALUES - USER SIDE */
#define   PRI_DMS100_T303    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T305    (UINT32)  30000             /* 30 seconds */
#define   PRI_DMS100_T308    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T310    (UINT32)  10000             /* 10 seconds */
#define   PRI_DMS100_T313    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T315    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T316    (UINT32) 120000L             /* 2 minutes */
#endif                                                   /* PRI_DMS100 */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
 * values for T316 and T3M1 since these timers are implemented there.
 *
 * STANDARD VALUES - USER SIDE
 */
#define   PRI_NET5_T303      (UINT32)   4000              /* 4 seconds */
#define   PRI_NET5_T305      (UINT32)  30000             /* 30 seconds */
#define   PRI_NET5_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_NET5_T309      (UINT32)  90000L            /* 90 seconds */
#define   PRI_NET5_T310      (UINT32)  40000             /* 40 seconds */
#define   PRI_NET5_T313      (UINT32)   4000              /* 4 seconds */
#define   PRI_NET5_T316      (UINT32) 120000L             /* 2 minutes */
#define   PRI_NET5_T3M1      (UINT32) 120000L             /* 2 minutes */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
 * values for T316 and T3M1 since these timers are implemented there.
 *
 * STANDARD VALUES - USER SIDE
 */
#define   PRI_NTT_T303       (UINT32)   4000              /* 4 seconds */
#define   PRI_NTT_T305       (UINT32)   4000              /* 4 seconds */
#define   PRI_NTT_T308       (UINT32)   4000              /* 4 seconds */
#define   PRI_NTT_T309       (UINT32)  90000L            /* 90 seconds */
#define   PRI_NTT_T310       (UINT32)  30000             /* 30 seconds */
#define   PRI_NTT_T313       (UINT32)   4000              /* 4 seconds */
#define   PRI_NTT_T316       (UINT32) 120000L             /* 2 minutes */
#define   PRI_NTT_T3M1       (UINT32) 120000L             /* 2 minutes */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
 * values for T316 and T3M1 since these timers are implemented there.
 *
 * STANDARD VALUES - USER SIDE
 */
#define   PRI_TS014_T303    (UINT32)  40000             /* 40 seconds */
#define   PRI_TS014_T305    (UINT32)  30000             /* 30 seconds */
#define   PRI_TS014_T308    (UINT32)   4000             /*  4 seconds */
#define   PRI_TS014_T309    (UINT32)  90000L            /* 90 seconds */
#define   PRI_TS014_T310    (UINT32)  30000             /* 30 seconds */
#define   PRI_TS014_T313    (UINT32)   4000             /*  4 seconds */
#define   PRI_TS014_T316    (UINT32)  30000             /* 30 seconds */
#define   PRI_TS014_T3M1    (UINT32) 120000L            /*  2 minutes */

#ifdef VN2                              /* STANDARD VALUES - USER SIDE */
#define   VN2_T303           (UINT32)  10000             /* 10 seconds */
#define   VN2_T304           (UINT32)  60000             /* 60 seconds */
#define   VN2_T305           (UINT32)   4000              /* 4 seconds */
#define   VN2_T308           (UINT32)   4000              /* 4 seconds */
#define   VN2_T309           (UINT32)  90000L            /* 90 seconds */
#define   VN2_T310           (UINT32)  60000L        /* === 60 seconds */
#define   VN2_T313           (UINT32)   4000              /* 4 seconds */
#define   VN2_T318           (UINT32)   4000         /* ===  4 seconds */
#define   VN2_T319           (UINT32)   4000              /* 4 seconds */
#define   VN2_T399           (UINT32)  10000             /* 10 seconds */
#endif                                                          /* VN2 */

#ifdef BRI_NET3                   /* STANDARD VALUES - USER SIDE === */
#define   NET3_T303           (UINT32)  4000              /* 4 seconds */
#define   NET3_T305           (UINT32)  30000            /* 30 seconds */
#define   NET3_T308           (UINT32)   4000             /* 4 seconds */
#define   NET3_T309           (UINT32)  90000L           /* 90 seconds */
#define   NET3_T310           (UINT32)  40000L        /* === 40 seconds */
#define   NET3_T313           (UINT32)   4000             /* 4 seconds */
#define   NET3_T318           (UINT32)   4000        /* ===  4 seconds */
#define   NET3_T319           (UINT32)   4000             /* 4 seconds */
#define   NET3_T322           (UINT32)   4000             /* 4 seconds */
#endif                                                     /* BRI_NET3 */

#ifdef BRI_TS013                /* STANDARD VALUES - USER SIDE */
#define   TS013_T303        (UINT32)  14000            /* 14 seconds */
#define   TS013_T305        (UINT32)  30000            /* 30 seconds */
#define   TS013_T308        (UINT32)   4000             /* 4 seconds */
#define   TS013_T309        (UINT32)  90000L           /* 90 seconds */
#define   TS013_T310        (UINT32)  10000            /* 10 seconds */
#define   TS013_T313        (UINT32)   4000             /* 4 seconds */
#define   TS013_T318        (UINT32)   4000             /* 4 seconds */
#define   TS013_T319        (UINT32)   4000             /* 4 seconds */
#define   TS013_T322        (UINT32)   4000             /* 4 seconds */
#endif                                                  /* BRI_TS013 */

#ifdef BRI_1TR6                         /* STANDARD VALUES - USER SIDE */
#define   BRI_1TR6_T303      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T305      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T308      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T313      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T318      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T319      (UINT32)   4000              /* 4 seconds */
#endif                                           /* BRI_1TR6 (P-to-MP) */

#ifdef PRI_1TR6                         /* STANDARD VALUES - USER SIDE */
#define   PRI_1TR6_T303      (UINT32)   4000              /* 4 seconds */
#define   PRI_1TR6_T305      (UINT32)  30000             /* 30 seconds */
#define   PRI_1TR6_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_1TR6_T313      (UINT32)   4000              /* 4 seconds */
#endif                                            /* PRI_1TR6 (P-to-P) */

                        /* DEFAULT STANDARD VALUES - USER SIDE - CCITT */
#define   DEFAULT_T302       (UINT32)  10000             /* 10 seconds */
#define   DEFAULT_T303       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T304       (UINT32)  15000             /* 15 seconds */
#define   DEFAULT_T305       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T306       (UINT32)  30000             /* 30 seconds */
#define   DEFAULT_T308       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T309       (UINT32)  90000L            /* 90 seconds */
#define   DEFAULT_T310       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T312       (UINT32)   4500            /* 4.5 seconds */
#define   DEFAULT_T313       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T316       (UINT32) 120000L             /* 2 minutes */
#define   DEFAULT_T318       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T319       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T322       (UINT32)   4000              /* 4 seconds */

#else                                                       /* L3_TEST */
#ifdef BRI_5ESS                             /* TEST VALUES - USER SIDE */
#define   BRI_5ESS_T303      (UINT32)   8000              /* 8 seconds */
#define   BRI_5ESS_T305      (UINT32)  10000             /* 10 seconds */
#define   BRI_5ESS_T308      (UINT32)  30000              /* 4 seconds */
#endif                                                     /* BRI_5ESS */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_4ESS                             /* TEST VALUES - USER SIDE */
#define   PRI_4ESS_T303      (UINT32)   8000              /* 8 seconds */
#define   PRI_4ESS_T305      (UINT32)   8000              /* 8 seconds */
#define   PRI_4ESS_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   PRI_4ESS_T310      (UINT32)  10000             /* 10 seconds */
#define   PRI_4ESS_T313      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T316      (UINT32)   5000              /* 5 seconds */
#define   PRI_4ESS_T3M1      (UINT32)   5000              /* 5 seconds */
#endif                                                     /* PRI_4ESS */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_5ESS                             /* TEST VALUES - USER SIDE */
#define   PRI_5ESS_T303      (UINT32)   8000              /* 8 seconds */
#define   PRI_5ESS_T305      (UINT32)   8000              /* 8 seconds */
#define   PRI_5ESS_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   PRI_5ESS_T310      (UINT32)  10000             /* 10 seconds */
#define   PRI_5ESS_T313      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T316      (UINT32)   5000              /* 5 seconds */
#define   PRI_5ESS_T3M1      (UINT32)   5000              /* 5 seconds */
#endif                                                     /* PRI_5ESS */

#ifdef BRI_DMS100                           /* TEST VALUES - USER SIDE */
#define   BRI_DMS100_T302    (UINT32)  18000             /* 18 seconds */
#define   BRI_DMS100_T303    (UINT32)   5000              /* 5 seconds */
#define   BRI_DMS100_T304    (UINT32)  20000             /* 20 seconds */
#define   BRI_DMS100_T305    (UINT32)  10000             /* 10 seconds */
#define   BRI_DMS100_T306    (UINT32)  10000             /* 10 seconds */
#define   BRI_DMS100_T308    (UINT32)   5000              /* 5 seconds */
#define   BRI_DMS100_T309    (UINT32)  90000L            /* 90 seconds */
#define   BRI_DMS100_T310    (UINT32)  10000             /* 10 seconds */
#define   BRI_DMS100_T312    (UINT32)  30000             /* 30 seconds */
#define   BRI_DMS100_T313    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T316    (UINT32) 120000L            /* 20 seconds */
#define   BRI_DMS100_T318    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T319    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T322    (UINT32)   4000              /* 4 seconds */
#endif                                                   /* BRI_DMS100 */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_DMS100                           /* TEST VALUES - USER SIDE */
#define   PRI_DMS100_T303    (UINT32)   5000              /* 5 seconds */
#define   PRI_DMS100_T305    (UINT32)  10000             /* 10 seconds */
#define   PRI_DMS100_T308    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T310    (UINT32)  10000             /* 10 seconds */
#define   PRI_DMS100_T313    (UINT32)  90000L            /* 90 seconds */
#define   PRI_DMS100_T315    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T316    (UINT32)  20000             /* 20 seconds */
#endif                                                   /* PRI_DMS100 */

#ifdef VN2                                  /* TEST VALUES - USER SIDE */
#define   VN2_T303           (UINT32)   8000              /* 8 seconds */
#define   VN2_T304           (UINT32)  20000             /* 20 seconds */
#define   VN2_T305           (UINT32)  10000             /* 10 seconds */
#define   VN2_T308           (UINT32)  30000              /* 4 seconds */
#define   VN2_T309           (UINT32)  90000L            /* 90 seconds */
#define   VN2_T310           (UINT32)  10000             /* 10 seconds */
#define   VN2_T313           (UINT32)  30000              /* 4 seconds */
#define   VN2_T318           (UINT32)  10000             /* 10 seconds */
#define   VN2_T319           (UINT32)   4000              /* 4 seconds */
#define   VN2_T399           (UINT32)  20000             /* 20 seconds */
#endif

#ifdef BRI_1TR6                             /* TEST VALUES - USER SIDE */
#define   BRI_1TR6_T303      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T305      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T308      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T313      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T318      (UINT32)   4000              /* 4 seconds */
#define   BRI_1TR6_T319      (UINT32)   4000              /* 4 seconds */
#endif                                           /* BRI_1TR6 (P-to-MP) */

#ifdef PRI_1TR6                             /* TEST VALUES - USER SIDE */
#define   PRI_1TR6_T303      (UINT32)   4000              /* 4 seconds */
#define   PRI_1TR6_T305      (UINT32)  10000             /* 10 seconds */
#define   PRI_1TR6_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_1TR6_T313      (UINT32)   4000              /* 4 seconds */
#endif                                            /* PRI_1TR6 (P-to-P) */

                                    /* DEFAULT TEST VALUES - USER SIDE */
#define   DEFAULT_T302       (UINT32)  18000             /* 18 seconds */
#define   DEFAULT_T303       (UINT32)   8000              /* 8 seconds */
#define   DEFAULT_T304       (UINT32)  20000             /* 20 seconds */
#define   DEFAULT_T305       (UINT32)  10000             /* 10 seconds */
#define   DEFAULT_T306       (UINT32)  10000             /* 10 seconds */
#define   DEFAULT_T308       (UINT32)   4000             /* 4 seconds */
#define   DEFAULT_T309       (UINT32)  90000L            /* 90 seconds */
#define   DEFAULT_T310       (UINT32)  10000             /* 10 seconds */
#define   DEFAULT_T312       (UINT32)  30000             /* 30 seconds */
#define   DEFAULT_T313       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T316       (UINT32)  20000L            /* 20 seconds */
#define   DEFAULT_T318       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T319       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T322       (UINT32)   4000              /* 4 seconds */
#endif                                                      /* L3_TEST */

#else                                                       /* NETWORK */
#ifndef L3_TEST             /* These are the standard NON-TEST values. */
#ifdef BRI_5ESS                      /* STANDARD VALUES - NETWORK SIDE */
#define   BRI_5ESS_T302      (UINT32)  10000             /* 10 seconds */
#define   BRI_5ESS_T303      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T304      (UINT32)  15000             /* 15 seconds */
#define   BRI_5ESS_T305      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T306      (UINT32)  30000             /* 30 seconds */
#define   BRI_5ESS_T308      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   BRI_5ESS_T310      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T312      (UINT32)   4500            /* 4.5 seconds */
#define   BRI_5ESS_T313      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T316      (UINT32) 120000L             /* 2 minutes */
#define   BRI_5ESS_T318      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T319      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T322      (UINT32)   4000              /* 4 seconds */
#endif                                                     /* BRI_5ESS */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_4ESS                      /* STANDARD VALUES - NETWORK SIDE */
#define   PRI_4ESS_T303      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T305      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T306      (UINT32)  15000             /* 15 seconds */
#define   PRI_4ESS_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   PRI_4ESS_T310      (UINT32)  10000             /* 10 seconds */
#define   PRI_4ESS_T316      (UINT32) 120000L             /* 2 minutes */
/* T3M1 is implemented in Call Control
*/
#endif                                                     /* PRI_4ESS */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_5ESS                      /* STANDARD VALUES - NETWORK SIDE */
#define   PRI_5ESS_T303      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T305      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T306      (UINT32)  15000             /* 15 seconds */
#define   PRI_5ESS_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   PRI_5ESS_T310      (UINT32)  10000             /* 10 seconds */
#define   PRI_5ESS_T316      (UINT32) 120000L             /* 2 minutes */
/* T3M1 is implemented in Call Control
*/
#endif                                                     /* PRI_5ESS */

#ifdef BRI_DMS100                    /* STANDARD VALUES - NETWORK SIDE */
#define   BRI_DMS100_T302    (UINT32)  10000             /* 10 seconds */
#define   BRI_DMS100_T303    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T304    (UINT32)  15000             /* 15 seconds */
#define   BRI_DMS100_T305    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T306    (UINT32)  30000             /* 30 seconds */
#define   BRI_DMS100_T308    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T309    (UINT32)  90000L            /* 90 seconds */
#define   BRI_DMS100_T310    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T312    (UINT32)   4500            /* 4.5 seconds */
#define   BRI_DMS100_T313    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T316    (UINT32) 120000L             /* 2 minutes */
#define   BRI_DMS100_T318    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T319    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T322    (UINT32)   4000              /* 4 seconds */
#endif                                                   /* BRI_DMS100 */

#ifdef PRI_DMS100                    /* STANDARD VALUES - NETWORK SIDE */
#define   PRI_DMS100_T302    (UINT32)  10000             /* 10 seconds */
#define   PRI_DMS100_T303    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T304    (UINT32)  15000             /* 15 seconds */
#define   PRI_DMS100_T305    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T306    (UINT32)  30000             /* 30 seconds */
#define   PRI_DMS100_T308    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T309    (UINT32)   5000              /* 5 seconds */
#define   PRI_DMS100_T310    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T312    (UINT32)   4500            /* 4.5 seconds */
#define   PRI_DMS100_T313    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T316    (UINT32) 120000L             /* 2 minutes */
#define   PRI_DMS100_T318    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T319    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T322    (UINT32)   4000              /* 4 seconds */
#endif                                                   /* PRI_DMS100 */

#ifdef VN2                           /* STANDARD VALUES - NETWORK SIDE */
#define   VN2_T302           (UINT32)  18000             /* 18 seconds */
#define   VN2_T303           (UINT32)  10000             /* 10 seconds */
#define   VN2_T304           (UINT32)  60000             /* 60 seconds */
#define   VN2_T305           (UINT32)   4000              /* 4 seconds */
#define   VN2_T306           (UINT32)  10000             /* 10 seconds */
#define   VN2_T308           (UINT32)   4000              /* 4 seconds */
#define   VN2_T309           (UINT32)  90000L            /* 90 seconds */
#define   VN2_T310           (UINT32)  60000             /* 60 seconds */
#define   VN2_T312           (UINT32)  30000             /* 30 seconds */
#define   VN2_T313           (UINT32)   4000              /* 4 seconds */
#define   VN2_T316           (UINT32) 120000L           /* 120 seconds */
#define   VN2_T318           (UINT32)   4000              /* 4 seconds */
#define   VN2_T319           (UINT32)  10000             /* 10 seconds */
#define   VN2_T322           (UINT32)   8000              /* 8 seconds */
#define   VN2_T399           (UINT32)  10000             /* 10 seconds */
#endif                                                          /* VN2 */

                     /* DEFAULT STANDARD VALUES - NETWORK SIDE - CCITT */
#define   DEFAULT_T302       (UINT32)  10000             /* 10 seconds */
#define   DEFAULT_T303       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T304       (UINT32)  15000             /* 15 seconds */
#define   DEFAULT_T305       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T306       (UINT32)  30000             /* 30 seconds */
#define   DEFAULT_T308       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T309       (UINT32)   5000              /* 5 seconds */
#define   DEFAULT_T310       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T312       (UINT32)   4500            /* 4.5 seconds */
#define   DEFAULT_T313       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T316       (UINT32) 120000L             /* 2 minutes */
#define   DEFAULT_T318       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T319       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T322       (UINT32)   4000              /* 4 seconds */

#else                                                       /* L3_TEST */
#ifdef BRI_5ESS                          /* TEST VALUES - NETWORK SIDE */
#define   BRI_5ESS_T302      (UINT32)  18000             /* 18 seconds */
#define   BRI_5ESS_T303      (UINT32)  30000             /* 30 seconds */
#define   BRI_5ESS_T304      (UINT32)  20000             /* 20 seconds */
#define   BRI_5ESS_T305      (UINT32)  10000             /* 10 seconds */
#define   BRI_5ESS_T306      (UINT32)  10000             /* 10 seconds */
#define   BRI_5ESS_T308      (UINT32)  30000             /* 30 seconds */
#define   BRI_5ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   BRI_5ESS_T310      (UINT32)  10000             /* 10 seconds */
#define   BRI_5ESS_T312      (UINT32)  30000             /* 30 seconds */
#define   BRI_5ESS_T313      (UINT32)  30000             /* 30 seconds */
#define   BRI_5ESS_T316      (UINT32) 120000L           /* 120 seconds */
#define   BRI_5ESS_T318      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T319      (UINT32)   4000              /* 4 seconds */
#define   BRI_5ESS_T322      (UINT32)   4000              /* 4 seconds */
#endif                                                     /* BRI_5ESS */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_4ESS                          /* TEST VALUES - NETWORK SIDE */
#define   PRI_4ESS_T303      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T305      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T306      (UINT32)   5000              /* 5 seconds */
#define   PRI_4ESS_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_4ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   PRI_4ESS_T310      (UINT32)  10000             /* 10 seconds */
#define   PRI_4ESS_T316      (UINT32) 120000L             /* 2 minutes */
/* T3M1 is implemented in Call Control
*/
#endif                                                     /* PRI_4ESS */

/* Note that the Telenetworks PRI Call Control (CCPRI) defines its own
*  values for T316 and T3M1 since these timers are implemented there.
*/
#ifdef PRI_5ESS                          /* TEST VALUES - NETWORK SIDE */
#define   PRI_5ESS_T303      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T305      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T306      (UINT32)   5000              /* 5 seconds */
#define   PRI_5ESS_T308      (UINT32)   4000              /* 4 seconds */
#define   PRI_5ESS_T309      (UINT32)  90000L            /* 90 seconds */
#define   PRI_5ESS_T310      (UINT32)  10000             /* 10 seconds */
#define   PRI_5ESS_T316      (UINT32) 120000L             /* 2 minutes */
/* T3M1 is implemented in Call Control
*/
#endif                                                     /* PRI_5ESS */

#ifdef BRI_DMS100                        /* TEST VALUES - NETWORK SIDE */
#define   BRI_DMS100_T302    (UINT32)   5000              /* 5 seconds */
#define   BRI_DMS100_T303    (UINT32)   5000              /* 5 seconds */
#define   BRI_DMS100_T304    (UINT32)  20000             /* 20 seconds */
#define   BRI_DMS100_T305    (UINT32)   5000              /* 5 seconds */
#define   BRI_DMS100_T306    (UINT32)  10000             /* 10 seconds */
#define   BRI_DMS100_T308    (UINT32)   5000              /* 5 seconds */
#define   BRI_DMS100_T309    (UINT32)   8000L             /* 8 seconds */
#define   BRI_DMS100_T310    (UINT32)  10000             /* 10 seconds */
#define   BRI_DMS100_T312    (UINT32)  10000             /* 10 seconds */
#define   BRI_DMS100_T313    (UINT32)  30000              /* 4 seconds */
#define   BRI_DMS100_T316    (UINT32) 120000L           /* 120 seconds */
#define   BRI_DMS100_T318    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T319    (UINT32)   4000              /* 4 seconds */
#define   BRI_DMS100_T322    (UINT32)   8000              /* 8 seconds */
#endif                                                   /* BRI_DMS100 */

#ifdef PRI_DMS100                        /* TEST VALUES - NETWORK SIDE */
#define   PRI_DMS100_T302    (UINT32)  18000             /* 18 seconds */
#define   PRI_DMS100_T303    (UINT32)  30000             /* 30 seconds */
#define   PRI_DMS100_T304    (UINT32)  20000             /* 20 seconds */
#define   PRI_DMS100_T305    (UINT32)  10000             /* 10 seconds */
#define   PRI_DMS100_T306    (UINT32)  10000             /* 10 seconds */
#define   PRI_DMS100_T308    (UINT32)  30000             /* 30 seconds */
#define   PRI_DMS100_T309    (UINT32)  90000L            /* 90 seconds */
#define   PRI_DMS100_T310    (UINT32)  10000             /* 10 seconds */
#define   PRI_DMS100_T312    (UINT32)  30000             /* 30 seconds */
#define   PRI_DMS100_T313    (UINT32)  30000             /* 30 seconds */
#define   PRI_DMS100_T316    (UINT32) 120000L           /* 120 seconds */
#define   PRI_DMS100_T318    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T319    (UINT32)   4000              /* 4 seconds */
#define   PRI_DMS100_T322    (UINT32)   8000              /* 8 seconds */
#endif                                                   /* PRI_DMS100 */

#ifdef VN2                               /* TEST VALUES - NETWORK SIDE */
#define   VN2_T302           (UINT32)  18000             /* 18 seconds */
#define   VN2_T303           (UINT32)  30000             /* 30 seconds */
#define   VN2_T304           (UINT32)  20000             /* 20 seconds */
#define   VN2_T305           (UINT32)  10000             /* 10 seconds */
#define   VN2_T306           (UINT32)  10000             /* 10 seconds */
#define   VN2_T308           (UINT32)  30000             /* 30 seconds */
#define   VN2_T309           (UINT32)  90000L            /* 90 seconds */
#define   VN2_T310           (UINT32)  10000             /* 10 seconds */
#define   VN2_T312           (UINT32)  30000             /* 30 seconds */
#define   VN2_T313           (UINT32)  30000             /* 30 seconds */
#define   VN2_T316           (UINT32) 120000L           /* 120 seconds */
#define   VN2_T318           (UINT32)   4000              /* 4 seconds */
#define   VN2_T319           (UINT32)   4000              /* 4 seconds */
#define   VN2_T322           (UINT32)   8000              /* 8 seconds */
#define   VN2_T399           (UINT32)  20000             /* 20 seconds */
#endif

                                 /* DEFAULT TEST VALUES - NETWORK SIDE */
#define   DEFAULT_T302       (UINT32)  18000             /* 18 seconds */
#define   DEFAULT_T303       (UINT32)  30000             /* 30 seconds */
#define   DEFAULT_T304       (UINT32)  20000             /* 20 seconds */
#define   DEFAULT_T305       (UINT32)  10000             /* 10 seconds */
#define   DEFAULT_T306       (UINT32)  10000             /* 10 seconds */
#define   DEFAULT_T308       (UINT32)  30000             /* 30 seconds */
#define   DEFAULT_T309       (UINT32)  90000L            /* 90 seconds */
#define   DEFAULT_T310       (UINT32)  10000             /* 10 seconds */
#define   DEFAULT_T312       (UINT32)  30000             /* 30 seconds */
#define   DEFAULT_T313       (UINT32)  30000             /* 30 seconds */
#define   DEFAULT_T316       (UINT32) 120000L           /* 120 seconds */
#define   DEFAULT_T318       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T319       (UINT32)   4000              /* 4 seconds */
#define   DEFAULT_T322       (UINT32)   8000              /* 8 seconds */
#endif                                                      /* L3_TEST */
#endif                         /* End of USER and NETWORK Timer Values */

/* Information Field Array Definition
*/
#define         PROTO_DISC_NDX          0
#define         CALL_REF0_NDX           1
#define         CALL_REF1_NDX           2
#define         MSG_NDX                 3

/* Information Field Elements Lengths and Identifiers
*/
#define         INFO_LEN_BRI            4
#define         INFO_LEN_PRI            5

/* Miscellaneous constants
*/
#define         MAX_SHORT_CALL_REF   0x7F    /* max call ref out value */
#define         MAX_LONG_CALL_REF  0x7FFF    /* max call ref out value */
#ifdef PRI_1TR6 
#define         PRI_1TR6_STD_CALL_REF_LEN 1 /* Default call ref length */
#endif 
#define         PRI_STD_CALL_REF_LEN        2   /* Default call ref length */
#define         BRI_STD_CALL_REF_LEN        1   /* Default call ref length */

#define         COMPLETED              -1  /* completion case in L3_Go */
#define         NULL_CREF              -2     /* special case in L3_Go */
#define         GLOBAL_STATUS          -3     /* special case in L3_Go */
#define         GLOBAL_REL_COMP        -4     /* special case in L3_Go */
#define         BAD_PD                 -5     /* special case in L3_Go */
#define         RELEASECOMP_SENT        3     /* Release Complete sent */

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/

/* Layer 3 Queue
*
*  Packets may arrive at Layer 3 prior to being in a state in which the
*  event may be processed.  The packets are saved until the necessary
*  state is reached at which time the packets are processed.  The packets
*  are saved in a packet queue of which there are two:
*
*  MsgQ:  save CC_SETUP_REQ, CC_BROADCAST_RESP and/or CC_ALERTING_REQ
*         to be processed when state 1 is reached.
*
*  InfoQ: save CC_INFO_REQ to be processed when state 2 is reached.
*
*/
typedef struct
{
    PKT_ID Pkt[MAX_L3_QUEUE_LEN];                     /* saved packets */
    int Count;                           /* number of packets on queue */
} L3_Q;

/* Layer 3 Network Link Control Block
*
*  This structure holds all of the state information for a network link.
*
*  While processing an event, a function may assume that 'NLCB'
*  points to the current (and correct) Network Link Control Block.
*
*  Two NLCB's are defined; one for USER side implementations, one for
*  NETWORK side implementations.  This is necessary to support divergent
*  timer sets.
*/
struct NetworkLinkControlBlock
#ifdef USER                                               /* User NLCB */
    {
    BYTE         Ces;                       /* connect endpoint suffix */
    int          CallId;                        /* call identification */
#ifdef BRI
    int          ChanId;                     /* channel identification */
#endif
    L3_STATE     State;                          /* network link state */
    BYTE         CallRefLen;                  /* call reference length */
    int          CallRef;            /* call reference; 0,1 or 2 bytes */
#ifdef BRI
#ifdef VN2
    BOOLEAN      AssCref;                  /* Call reference assigned? */
#endif
#endif
    int          Event;                         /* current event value */
    int          Timer_T303;                        /* Setup Ack Timer */
    int          Timer_T305;                       /* Disconnect Timer */
    int          Timer_T308;                 /* Release Complete Timer */
#ifdef VN2
    int          Timer_T310;                 /* Call Progression Timer */
#endif
    int          Timer_T313;                      /* Connect Ack Timer */
    int          Timer_T318;                       /* Resume Ack Timer */
    int          Timer_T319;                      /* Suspend Ack Timer */
    int          Timer_twait;     /* NI delay for interface activation */
    BOOLEAN      T303_First;                         /* first timeout? */
    BOOLEAN      T308_First;                         /* first timeout? */
    BOOLEAN      T313_First;                         /* first timeout? */
    BOOLEAN      UserOriginated;      /* did USER originate this call? */
    BOOLEAN      Internal;       /* internal event(s) await processing */
    BYTE         Cause;                   /* cause information element */
    BYTE         PreviousCause;           /* cause information element */
    L3_Q         MsgQ;                                /* message queue */
    L3_Q         InfoQ;                                  /* info queue */
    PKT_ID       Setup;           /* save SETUP msg for retransmission */
    PKT_ID       Connect;       /* save CONNECT msg for retransmission */
    PKT_ID       Release;       /* save RELEASE msg for retransmission */
    struct NetworkLinkControlBlock *Next;                 /* next link */
    };
#endif
typedef struct NetworkLinkControlBlock *NLCB_ID;

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

#ifdef L3_C
/* first, last, current NLCB
*/
NLCB_ID NLCB_Head[MAX_DSL];
NLCB_ID NLCB_Tail[MAX_DSL];
NLCB_ID NLCB;

/*
* Packet rules:
*   If a function processes ONLY incoming packets, it may assume
*   that 'L3_PktIn' points to that packet structure.
*   If a function processes ONLY outgoing packets, it may assume
*   that 'L3_PktOut' points to that packet structure.
*
*   L3_PktIn is preserved while its processing is incomplete.
*   L3_PktOut is allocated every time outgoing pkt's are mailed.
*/
PKT_ID L3_PktIn;                                    /* packet received */
PKT_ID L3_PktOut;                                 /* packet to be sent */

/* Misc. values
*/
int     L3_CallRef;                          /* call reference counter */
BYTE    L3_DSL_Id;               /* digital subscriber loop identifier */
int     L3_Call_Id;
int     L3_MsgSource;                       /* Incoming message source */
BOOLEAN L3_AvailPktIn;     /* Packet received is available to be freed */
BYTE    Diagnostic[ DIAG_LEN ];    /* Diagnostic value(s) for Cause IE */

#else                                                      /* NOT L3_C */
/* EXECUSER.C includes L3.H and must also include TEI_CES.H (to resolve
*  MAX_DSL) unless this exclusion is made.
*/
#ifndef EXECUSER_C
extern NLCB_ID NLCB_Head[MAX_DSL];
#ifdef NEVER_REFERENCED
extern NLCB_ID NLCB_Tail[MAX_DSL];
#endif /* NEVER_REFERENCED */
extern NLCB_ID NLCB;

extern PKT_ID L3_PktIn;
extern PKT_ID L3_PktOut;

extern int     L3_CallRef;
extern BYTE    L3_DSL_Id;
extern int     L3_Call_Id;
extern int     L3_MsgSource;
extern BOOLEAN L3_AvailPktIn;
extern BYTE    Diagnostic[ DIAG_LEN ];
#endif
#endif

#ifdef L3_SDL_U_C
extern NLCB_ID NLCB_Head[MAX_DSL];
#ifdef NEVER_REFERENCED
extern NLCB_ID NLCB_Tail[MAX_DSL];
#endif /* NEVER_REFERENCED */
#endif
#ifdef NEVER_REFERENCED
extern int L3_NlcbPool;
#endif /* NEVER_REFERENCED (Actually, already extern'd in execuser.h) */

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/* For most switch types, the NLCB state and the Q.931 are identical.
*  For BRI_5ESS, in the ASAI world or the Supp Services world, non- 
*  standard NLCB states ate used.  To "match" the Q.931 state with the
*  NLCB state in these cases, a special L3 function is used.
*/
#ifdef BRI_5ESS
#define L3_EXT_STATE(s) L3_XlateNLCB_State(s)
#else
#define L3_EXT_STATE(s) (s)
#endif

/* Call reference comparison macros.  These macros are intended to
*  compare two call references of equal magnitude but differing lengths.
*  Their use is necessitated by the habit of some switch vendor 
*  compatibility test plans to change call reference lengths in mid-call,
*  thus demanding comparison of these different representations.
*/

/* Return call reference magnitude, removing flag bit.
*  c=call reference, l=call reference length.
*/
#define CR_VAL(c,l) (( (l)==2 ) ? ((c) & 0x7FFF) : ((c) & 0x7F))

/* not even used === */
#ifdef BRI
/* Return 0x80 if call reference flag bit is set, zero otherwise.
*  c=call reference, l=call reference length.
*/
#define CR_FLG(c,l) (((l)==2) ? (((c) & 0x8000 ) >> 8 ) : ((c) & 0x80))
#else
/* Return 0x8000 if call reference flag bit is set, zero otherwise.
*  c=call reference, l=call reference length.
*/
#define CR_FLG(c,l) (((l)==2) ? ((c) & 0x8000) : ((((c) & 0x80 ) << 8 )))
#endif

/* Return TRUE if two call references match in magnitude and flag bit sense.
*  c=call reference, l=call reference length.
*/
#define CR_MATCH(c1,l1,c2,l2) ((CR_VAL((c1),(l1))==CR_VAL((c2),(l2))) && \
                               (CR_FLG((c1),(l1))==CR_FLG((c2),(l2))))

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

PUBLIC  void      show_isdn_L3timers    (void);
PUBLIC  void      show_nlcbs            (int);
PUBLIC  BOOLEAN   L3_CallRefSelect      (BOOLEAN);
PUBLIC  void      L3_CallRelease        (void);
PUBLIC  void      L3_EnQueue            (L3_Q *);
PUBLIC  void      L3_Free_NLCB          (NLCB_ID);
PUBLIC  NLCB_ID   L3_GetBroadcastNLCB   (int);
PUBLIC  int       L3_GetCause           (PKT_ID);
PUBLIC  L3_STATE  L3_GetState           (BYTE *, int, BYTE *);
PUBLIC  void      L3_Go                 (int, int, BYTE, BYTE, int, \
                                              PKT_ID);
PUBLIC  void      L3_Init               (void);
PUBLIC  void      L3_Mail               (int, int, BOOLEAN, BOOLEAN);
PUBLIC  void      L3_ProcessEvent       (void);
PUBLIC  void      L3_PutInfo            (int, PKT_ID, BOOLEAN, BOOLEAN);
PUBLIC  void      L3_Restart_nlcb       (int);
PUBLIC  void      L3_SavePkt            (PKT_ID *, PKT_ID);
PUBLIC  NLCB_ID   L3_Search_NLCB_Ref    (int, int);
PUBLIC  void      L3_ShutDown           (BYTE);
PUBLIC  void      L3_StopAllTimers      (void);
#ifdef BRI
PUBLIC  void      L3_UpdateChannel      (PKT_ID);
#endif
PUBLIC  void      L3_UpdateTimers       ( NLCB_ID, int, BYTE );
PUBLIC  BOOLEAN   L3_ValidCallRef       (BYTE, int);
PUBLIC  int       L3_ValidState         (unsigned int);
#ifdef BRI_5ESS
PUBLIC  L3_STATE  L3_XlateNLCB_State    (L3_STATE);
#endif
PUBLIC  BOOLEAN   Ux_UnknownCallRef     (void);
PUBLIC  int       L3_start_twait_timer  (int);
#endif
