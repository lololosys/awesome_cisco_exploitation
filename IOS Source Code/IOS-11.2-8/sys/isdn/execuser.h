/* $Id: execuser.h,v 3.2.60.3 1996/06/16 21:13:12 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/isdn/execuser.h,v $
 *------------------------------------------------------------------
 * execuser.h  Multitasking Executive user module interface file
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: execuser.h,v $
 * Revision 3.2.60.3  1996/06/16  21:13:12  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.60.2  1996/04/30  23:07:18  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  20:31:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/03/07  09:51:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  17:40:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:15:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: execuser.h                                              
VERSION: 3.21
DESCRIPTION:  Multitasking Executive user module interface file       

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

EXECUSER.H defines the interface between the external applications
and the objects (functions and data) within the multitasking
executive's user-definable module EXECUSER.C.

*************************************************************************
*************************************************************************
*/


#ifndef EXECUSER_H                        /* prevent nested inclusions */
#define EXECUSER_H                                      /* module name */


/************************************************************************
*                           C O N S T A N T S                           
*************************************************************************
*/

/* CUSTOMIZE HERE - Define all application task stack sizes, in MWORDS.
*
*  CAUTION - Be sure that the size is large enough, for there are
*  no checks for stack overflow.
*
*/
#ifdef HDW_INTEL  
#define SML_STACK_SIZE     512 
#define LRG_STACK_SIZE     1024
#define HUGE_STACK_SIZE    2048
#endif

#ifdef HDW_MC68000
#define SML_STACK_SIZE     1024
#define LRG_STACK_SIZE     2048
#define HUGE_STACK_SIZE    4096
#endif

#ifdef HDW_R4600
#define SML_STACK_SIZE     2048
#define LRG_STACK_SIZE     4096
#define HUGE_STACK_SIZE    8192
#endif

/* CUSTOMIZE HERE - Define all timer constants.
*
*  In order to implement timers, it's necessary to define the length of
*  the hardware timer tick interrupt in milliseconds (HARD_TICK_LEN)
*  and the number of hardware ticks that make up a call to the software
*  timer tick handler (HARD_PER_SOFT).  HARD_TICK_LEN * HARD_PER_SOFT gives
*  a software timer tick length in milliseconds (SOFT_TICK_LEN).  In this
*  example, a software timer tick length of 100 milliseconds (or .1 secs)
*  was desired.
*
*  Hardware  CLOCK_SPEED  HARD_TICK_LEN  HARD_PER_SOFT  SOFT_TICK_LEN
*            (mhz)        (msecs)                       (msecs)
*  --------  -----------  -------------  -------------  -------------
*  PC        N.A.         55 (roughly)   2              110
*  HPC       16           10             10             100
*  68302     16           100            1              100
*
*/

#ifdef APP_NO_TIMER
#define HARD_TICK_LEN     100      /* hardware timer interrupt length */
#define HARD_PER_SOFT     1      /* 1 hardware int per 1 software int */
#define SOFT_TICK_LEN     (HARD_TICK_LEN*HARD_PER_SOFT)  /* soft tick */
#endif

#ifdef HDW_IBMPC
#define HARD_TICK_LEN     50       /* hardware timer interrupt length */
#define HARD_PER_SOFT     2     /* 2 hardware ints per 1 software int */
#define SOFT_TICK_LEN     (HARD_TICK_LEN*HARD_PER_SOFT)  /* soft tick */
#define MSDOS_TIMER_INT   0x08     /* MSDOS timer interrupt vect IRQ0 */
#endif






/* This value must not be changed.
*/
#define MAX_HEADROOM       42


/************************************************************************
*              D A T A   T Y P E   D E F I N I T I O N S                
*************************************************************************
*/

/* interrupt vector pointer
*/
#ifdef HDW_MC68000
typedef void (*INTVEC)(void);
#endif

#ifdef HDW_R4600
typedef void (*INTVEC)(void);	/* Not really. But it is never used */
#endif

#ifdef HDW_INTEL
typedef void (interrupt *INTVEC)(void);
#endif


/************************************************************************
*                              M A C R O S                              
*************************************************************************
*/

#ifdef EXECUSER_C          /* Allocate storage if compiling EXECUSER.C */

/* Macros to define and initialize various EXEC objects
*/
#define SemaphoreDef(s,v) SEM s={v,NULL}
#define MailboxDef(m,l)   MAILBOX m={{0,NULL}, {l,l,NULL}, {l,l,NULL}, NULL}
#define StackDef(s,n)     PRIVATE MWORD FAR_OBJ s[n]

/* The TaskDef macro allocates a named task control block and initializes
*  the components of the TCB that are common to all processors. The
*  CONTEXT member (physically the last element) of the TCB structure
*  is not initialized since the values will be filled in when the TCB is
*  switched.
*/
#define TaskDef(t,p,pg)   TCB t={UNKNOWN_TASK,NULL,NULL,p}

#else     /* generate reference to objects if not compiling EXECUSER.C */

#define SemaphoreDef(s,v) extern SEM s
#define MailboxDef(m,l)   extern MAILBOX m
#define StackDef(s,n)     
#define TaskDef(t,p,pg)   extern TCB t

#endif                                                   /* EXECUSER_C */


/************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S
*************************************************************************
*/
/*
 * externals added for show isdn memory
 */
extern int AllocatedCCBs;

/* Declare EXECUSER public functions
*/

PUBLIC  void        FinUsr          (void);
PUBLIC  void        InitUsr         (void);
#ifdef MEM_STATS
PUBLIC  void        isdn_memstats   (void);
#endif
#ifdef STACK_STATS
PUBLIC  void        StkStats        (void);
#endif

PUBLIC  void        TimerOff        (void);
PUBLIC  void        TimerOn         (void);


PUBLIC  INTVEC      VectGet         (int);
PUBLIC  void        VectSet         (int, INTVEC);


/************************************************************************
*                    D A T A   D E C L A R A T I O N S
*************************************************************************
*/



/************************************************************************
*           A P P L I C A T I O N    C U S T O M I Z A T I O N          *
*************************************************************************
*
* CUSTOMIZE HERE - For each task, do the following:
*
* 1) Define all application tasks using macro TaskDef(). Parameters to
* TaskDef() are the Task Control Block name, the priority of the task
* and (if HPC) the page in which the task code will reside.
*
* 2) Define all application task stacks using macro StackDef(). Parameters
* to StackDef() are the stack name and its size.
*
* 3) Define all application semaphores; parameters to SemaphoreDef()
* are the semaphore name and its value.
*
* 4) Define all application task start functions.
*
* 5) Define all application mailboxes using macro MailboxDef(). Parameters
* to MailboxDef() are the mailbox name and the maximum number of mail
* messages it can contain.
*
* 6) Define all application constant mailbox IDs for easy use.
*
*/

#ifdef TASK_DPDRV                            /* DPDRV task definitions */

TaskDef(DPDRV_TX_TCB, USER_MIN_PRI+1, 0);
StackDef(Stack_TX, LRG_STACK_SIZE);
PUBLIC void DPDRV_Send (void);
MailboxDef(DPDRV_Write_Mb, 30);
#define DPDRV_Write_MB (&DPDRV_Write_Mb)

TaskDef(DPDRV_RX_TCB, USER_MIN_PRI+1, 0);
StackDef(Stack_RX, LRG_STACK_SIZE);
PUBLIC void DPDRV_RcvTask (void);

#endif

#ifdef TASK_1
TaskDef(Task1TCB, USER_MAX_PRI, 1);
StackDef(Stack1,LRG_STACK_SIZE);
PUBLIC void Task1 (void);
MailboxDef(Task1_Mb, 100);
#define TASK1_MB (&Task1_Mb)
SemaphoreDef(Sem1, 0);
#define SEM1 (&Sem1)
#endif                                                       /* TASK_1 */

#ifdef TASK_2
TaskDef(Task2TCB, USER_MIN_PRI, 1);
StackDef(Stack2,LRG_STACK_SIZE);
PUBLIC void Task2 (void);
MailboxDef(Task2_Mb, 100);
#define TASK2_MB (&Task2_Mb)
SemaphoreDef(Sem2, 0);
#define SEM2 (&Sem2)
#endif                                                       /* TASK_2 */

#ifdef TASK_3
TaskDef(Task3TCB, USER_MIN_PRI, 1);
StackDef(Stack3,LRG_STACK_SIZE);
PUBLIC void Task3 (void);
MailboxDef(Task3_Mb, 100);
#define TASK3_MB (&Task3_Mb)
SemaphoreDef(Sem3, 0);
#define SEM3 (&Sem3)
#endif                                                       /* TASK_3 */

#ifdef TASK_TRACER
TaskDef(TCB_TRACER, USER_MIN_PRI + 1, 2);
#ifdef OS_USG
/* Unix needs huge stack
*/
StackDef(STACK_TRACER, HUGE_STACK_SIZE);
#else
StackDef(STACK_TRACER, LRG_STACK_SIZE);
#endif
PUBLIC void Tracer (void);
MailboxDef(TRACER_Mb, 30);
MailboxDef(TRACER_PVT_Mb, 5);
#define TRACER_MB (&TRACER_Mb)
#define TRACER_PVT_MB (&TRACER_PVT_Mb)
#endif                                                  /* TASK_TRACER */

#ifdef TASK_ME
TaskDef(TCB_ME, USER_MIN_PRI + 1, 1);
StackDef(STACK_ME, SML_STACK_SIZE);
PUBLIC void MEIF_StartME (void);
MailboxDef(ME_Mb, me_mb_size);
#define ME_MB (&ME_Mb)
#endif                                                      /* TASK_ME */

#ifdef NEVER_REFERENCED
#ifdef TASK_MEMDRV_R
/* Shared memory driver read task.
 * Priority must be equal to or less than other user tasks.
 */
TaskDef(TCB_MEMDRV_R, USER_MIN_PRI, 1);
StackDef(STACK_MEMDRV_R, SML_STACK_SIZE);
PUBLIC void SIPBMDRV_R_Start (void);
MailboxDef(MEMDRV_R_Mb, 100);
#define MEMDRV_R_MB (&MEMDRV_R_Mb)
#endif                                                /* TASK_MEMDRV_R */

#ifdef TASK_MEMDRV_W
/* Shared memory driver write task.
 * Priority not as critical as shared memory read task.
 */
TaskDef(TCB_MEMDRV_W, USER_MIN_PRI + 1, 1);
StackDef(STACK_MEMDRV_W, SML_STACK_SIZE);
PUBLIC void SIPBMDRV_W_Start (void);
MailboxDef(MEMDRV_W_Mb, 100);
#define MEMDRV_W_MB (&MEMDRV_W_Mb)
#endif                                                /* TASK_MEMDRV_W */
#endif /* NEVER_REFERENCED */

#ifdef TASK_TT
TaskDef(TCB_TT, USER_MIN_PRI + 1, 1);
StackDef(STACK_TT, SML_STACK_SIZE);
PUBLIC void ttask (void);
MailboxDef(TT_Mb, 50);                         /* must accept all mail */
#define TT_MB (&TT_Mb)
#endif                                                      /* TASK_TT */

#ifdef TASK_RT
TaskDef(TCB_RT, USER_MIN_PRI + 1, 1);
StackDef(STACK_RT, SML_STACK_SIZE);
PUBLIC void rtask (void);
MailboxDef(RT_Mb, 50);                         /* must accept all mail */
#define RT_MB (&RT_Mb)
#endif                                                      /* TASK_RT */

#ifdef TASK_CC
TaskDef(TCB_CC, USER_MIN_PRI + 1, 1);
StackDef(STACK_CC, SML_STACK_SIZE);
PUBLIC void CC_Task (void);
MailboxDef(CC_Mb, cc_mb_size);
#define CC_MB (&CC_Mb)
MailboxDef(KP_Mb, cc_mb_size);                                  /* ??? */
#define KP_MB (&KP_Mb)                                          /* ??? */
#endif                                                      /* TASK_CC */

#ifdef TASK_CC_DMS_B
TaskDef(TCB_CC, USER_MIN_PRI + 1, 1);
StackDef(STACK_CC, SML_STACK_SIZE);
PUBLIC void CC_Task (void);
MailboxDef(CC_Mb, cc_mb_size);
#define CC_MB (&CC_Mb)
#endif                                                /* TASK_CC_DMS_B */

                                                      /* TASK_CC_BRI */
#ifdef TASK_CC_BRI
TaskDef(TCB_CC, USER_MIN_PRI + 1, 1);
StackDef(STACK_CC, SML_STACK_SIZE);
PUBLIC void CC_Task (void);
MailboxDef(CC_Mb, cc_mb_size);
#define CC_MB (&CC_Mb)
#endif                                                /* TASK_CC_BRI */

#ifdef TASK_CC_VN2
TaskDef(TCB_CC, USER_MIN_PRI + 1, 1);
StackDef(STACK_CC, SML_STACK_SIZE);
PUBLIC void CC_Task (void);
MailboxDef(CC_Mb, cc_mb_size);
#define CC_MB (&CC_Mb)
#endif                                                  /* TASK_CC_VN2 */

#ifdef TASK_CC_PRI
TaskDef(TCB_CC, USER_MIN_PRI + 1, 1);
StackDef(STACK_CC, SML_STACK_SIZE);
PUBLIC void CC_Task (void);
MailboxDef(CC_Mb, cc_mb_size);
#define CC_MB (&CC_Mb)
#endif                                                  /* TASK_CC_PRI */

#ifdef TASK_CC_PRI_S
TaskDef(TCB_CC, USER_MIN_PRI + 1, 1);
StackDef(STACK_CC, SML_STACK_SIZE);
PUBLIC void CC_Task (void);
MailboxDef(CC_Mb, cc_mb_size);
#define CC_MB (&CC_Mb)
#endif                                                /* TASK_CC_PRI_S */

#ifdef TASK_CC_5ESS
TaskDef(TCB_CC, USER_MIN_PRI + 1, 1);
StackDef(STACK_CC, SML_STACK_SIZE);
PUBLIC void CC_Task (void);
MailboxDef(CC_Mb, cc_mb_size);
#define CC_MB (&CC_Mb)
#endif                                                 /* TASK_CC_5ESS */

#ifdef TASK_CC_ASAI
TaskDef(TCB_CC, USER_MIN_PRI + 1, 1);
StackDef(STACK_CC, SML_STACK_SIZE);
PUBLIC void CC_Task (void);
MailboxDef(CC_Mb, cc_mb_size);
#define CC_MB (&CC_Mb)
#endif                                                 /* TASK_CC_ASAI */

#ifdef TASK_CC_X25
TaskDef(TCB_CC_X25, USER_MIN_PRI + 1, 1);
StackDef(STACK_CC_X25, SML_STACK_SIZE);
PUBLIC void CC_X25_Task (void);
MailboxDef(CC_X25_Mb, 100);
#define CC_X25_MB (&CC_X25_Mb)
#endif                                                  /* TASK_CC_X25 */

#ifdef TASK_P3
TaskDef(TCB_P3, USER_MIN_PRI + 1, 0);
StackDef(STACK_P3, SML_STACK_SIZE);
PUBLIC void P3IF_StartP3 (void);
MailboxDef(P3_Mb, 100);
#define P3_MB (&P3_Mb)
#endif                                                      /* TASK_P3 */

#ifdef TASK_V3
TaskDef(TCB_V3, USER_MIN_PRI + 1, 0);
StackDef(STACK_V3, SML_STACK_SIZE);
PUBLIC void V3_Task (void);
MailboxDef(V3_Mb, 10);
#define V3_MB (&V3_Mb)
#endif                                                      /* TASK_V3 */

#ifdef TASK_L3
TaskDef(TCB_L3, USER_MIN_PRI + 1, 0);
StackDef(STACK_L3, SML_STACK_SIZE);
PUBLIC void L3IF_StartL3 (void);
MailboxDef(L3_Mb, L3_mb_size);
#define L3_MB (&L3_Mb)
#endif                                                      /* TASK_L3 */

#ifdef TASK_L2
TaskDef(TCB_L2, USER_MIN_PRI + 1, 1);
StackDef(STACK_L2, SML_STACK_SIZE);
PUBLIC void L2IF_StartL2 (void);
MailboxDef(L2_Mb, L2_mb_size);
#define L2_MB (&L2_Mb)
#endif                                                      /* TASK_L2 */





#ifdef TASK_HDLC_B1
TaskDef(TCB_HDLC_B1, USER_MIN_PRI + 1, 0);
StackDef(STACK_HDLC_B1, LRG_STACK_SIZE);
PUBLIC void L2B1_Srq_Task (void);
MailboxDef(HDLC_B1_Mb, 100);
#define HDLC_B1_MB (&HDLC_B1_Mb)
MailboxDef(HDLC_B1_XMT_Mb, 100);                    /* internal mailbox */
#define HDLC_B1_XMT_MB (&HDLC_D_XMT_Mb)
#endif                                                 /* TASK_HDLC_B1 */

#ifdef TASK_HDLC_B2
TaskDef(TCB_HDLC_B2, USER_MIN_PRI + 1, 0);
StackDef(STACK_HDLC_B2, LRG_STACK_SIZE);
PUBLIC void L2B2_Srq_Task (void);
MailboxDef(HDLC_B2_Mb, 100);
#define HDLC_B2_MB (&HDLC_B2_Mb)
MailboxDef(HDLC_B2_XMT_Mb, 100);                    /* internal mailbox */
#define HDLC_B2_XMT_MB (&HDLC_D_XMT_Mb)
#endif                                                 /* TASK_HDLC_B2 */

#ifdef TASK_HDLC_D
TaskDef(TCB_HDLC_D, USER_MIN_PRI + 1, 2);
StackDef(STACK_HDLC_D, SML_STACK_SIZE);
PUBLIC void L2D_Srq_Task (void);
MailboxDef(HDLC_D_Mb, hdlc_mb_size);
#define HDLC_D_MB (&HDLC_D_Mb)
MailboxDef(HDLC_D_XMT_Mb, hdlc_mb_size);           /* internal mailbox */
#define HDLC_D_XMT_MB (&HDLC_D_XMT_Mb)
#endif                                                  /* TASK_HDLC_D */

#ifdef TASK_UART
TaskDef(TCB_UART, USER_MIN_PRI + 1, 0);
StackDef(STACK_UART, LRG_STACK_SIZE);
PUBLIC void  UDIF_StartUDRV   (void);
MailboxDef(UART_Mb, 100);
SemaphoreDef(UART_Sem, 0);
#define UART_MB (&UART_Mb)
#define UART_SEM (&UART_Sem)
#endif                                                    /* TASK_UART */

#ifdef TASK_L1
TaskDef(TCB_L1, USER_MIN_PRI + 1, 2);
StackDef(STACK_L1, SML_STACK_SIZE);
PUBLIC void L1_Srq_Task (void);
MailboxDef(L1_Mb, 100);
#define L1_MB (&L1_Mb)
#endif                                                      /* TASK_L1 */

#ifdef TASK_KB
TaskDef(TCB_KB, USER_MIN_PRI + 1, 0);
StackDef(STACK_KB, LRG_STACK_SIZE);
PUBLIC void KB_Drv (void);
MailboxDef(KB_Mb, 100);
#define KB_MB (&KB_Mb)
#endif                                                      /* TASK_KB */

#ifdef TASK_ARP
TaskDef(TCB_ARP, USER_MIN_PRI + 1, 1);
StackDef(STACK_ARP, SML_STACK_SIZE);
PUBLIC void ARP_Srq_Task (void);
MailboxDef(ARP_Mb, 100);
#define ARP_MB (&ARP_Mb)
#endif

/* FDL (facility data link) tasks:
*  - FDL is the core FDL task
*  - PRTR is the FDL report generation (printer) output task
*/
#ifdef TASK_FDL
TaskDef(TCB_FDL, USER_MIN_PRI+2, 0);
StackDef(STACK_FDL, LRG_STACK_SIZE);
PUBLIC void FDLIF_StartFDL (void);
MailboxDef(FDL_Mb, 100);
#define FDL_MB (&FDL_Mb)
#endif

#ifdef TASK_PRTR
TaskDef(TCB_PRTR, USER_MIN_PRI+1, 0);
StackDef(STACK_PRTR, SML_STACK_SIZE);
PUBLIC void PRTRIF_StartPRTR (void);
MailboxDef(PRTR_Mb, 100);
MailboxDef(PRTR_PVT_Mb, 100);
#define PRTR_MB (&PRTR_Mb)
#define PRTR_PVT_MB (&PRTR_PVT_Mb)
#endif

#define  HARD_TICK_LEN     1      /* === */
#define  HARD_PER_SOFT     1      /* === */
#define  SOFT_TICK_LEN     (HARD_TICK_LEN*HARD_PER_SOFT)	/* === */

TaskDef(TCB_HOST, USER_MIN_PRI + 1, 1);		/* === */
StackDef(STACK_HOST, LRG_STACK_SIZE);		/* === */
PUBLIC void Host_Start (void);		/* === */
MailboxDef(HOST_Mb, host_mb_size);
#define HOST_MB (&HOST_Mb)		/* === */

#endif                                             /* ifdef EXECUSER_H */
