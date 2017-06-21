/* $Id: execuser.c,v 3.3.6.1 1996/04/30 23:07:17 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/execuser.c,v $
 *------------------------------------------------------------------
 * execuser.c  Multitasking Executive user module implementation file
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: execuser.c,v $
 * Revision 3.3.6.1  1996/04/30  23:07:17  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/06  16:10:51  dclare
 * CSCdi47302:  segV exception in ClearTimer/LIF_StopTimer. Increase the
 * number of blocks and reflect change in show isdn memory.
 *
 * Revision 3.2  1995/11/17  17:40:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:44:05  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 21:15:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef 0
		* needed because of the  * /  appearing in the macro below *
*
 *		The magic macro to change the InitTCB calls into Cisco_InitTCB
 *	calls is:
 *
:map q /\<InitTCBiCisco_f(%ik$bi"add"aP"apI"A"kA,kJJJA	/* """ */
 *
 *	Get this line into a named buffer (other than a) and execute it. 
 *	Then hit 'q' until all are converted.		===
 *
#endif

/************************************************************************
*************************************************************************

MODULE: execuser.c
VERSION: 3.21
DESCRIPTION: Multitasking Executive user module implementation file

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

EXECUSER.C is the implementation of the user-defined portion of the
MTEX real time Executive.

CONVENTIONS

All defined constants, derived data types, and macros are generally
in upper case. Static variable names use mixed case with the first
letter of keywords capitalized, as in "ExpireTime".

Function names follow the same upper/lower case convention as static.
Automatic variables may be in mixed case, or entirely in lower case.

ERROR CONTROL STRATEGY

EXEC uses a global variable, ExecErr to hold an error code indicating
the type of the last EXEC error. EXEC routines which encounter errors
set this variable and return the constant ERR or NULL to the calling
routine.  The caller can then examine ExecErr to determine the reason
for the error return. Valid error codes for ExecErr are defined in EXEC.H.

*************************************************************************
*************************************************************************
*/

#define     EXECUSER_C                            /* Module Identifier */

/************************************************************************
*                E X T E R N A L    R E F E R E N C E S                 *
*************************************************************************
*/

#include "master.h"
#undef NULL

#include "flags.h"

#include "tune.h"
#include "gendef.h"
#include "exec.h"
#include "execuser.h"

/*
 * CUSTOMIZE HERE - Include application header files here so that execuser
 * has access to application structures that will be used as a template
 * over memory blocks.
 */
#include "lif.h"
#include "me.h"

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/

#ifdef HDW_IBMPC
#define BOMB() { \
   EnterNPS(); \
   fprintf(stderr,"\nFATAL: %d %s\n",__LINE__,__FILE__); \
   ExecExit(); \
   }
#endif

#ifndef BOMB
/* No other definition of BOMB given, so define as ExecExit
*/
#define BOMB() ExecExit();
#endif


/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/* CUSTOMIZE HERE - Define the lengths of all memory block types.
*  CAUTION - Use macro BLKSIZE() to define the block lengths.
*/
#define PRIM_LEN  BLKSIZE(sizeof(struct Primitive))
#define PKT_LEN   BLKSIZE(sizeof(struct Packet))
#ifndef NO_PKG
#define PKG_LEN   BLKSIZE(sizeof(PKG))
#endif


/************************************************************************
*              D A T A   T Y P E   D E F I N I T I O N S                *
*************************************************************************
*/

/************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S
*************************************************************************
*/

#ifdef ISDN
extern  void              LIF_Init            (void);
#endif

/* timer interrupt function
*/
#ifdef HDW_INTEL
PRIVATE void interrupt    TimerInt            (void);
#endif



#ifdef CMP_MCC68K
#if 0                                                   /* MCC68K v3.3 */
PRIVATE void              TimerInt            (void);
#else                                                   /* MCC68K v4.1 */
/* ===
PRIVATE interrupt void    TimerInt            (void);
=== */

/*  PRIVATE void    TimerInt            (void); */
#endif
#endif


/************************************************************************
*                     D A T A   A L L O C A T I O N                     
*************************************************************************
*/

/* Mail Manager static data
*/
MAIL_DESCRIPTOR   MailDescPool[MAX_MAIL_DESCRIPTORS];

/* Timer manager timer static data
*/
TIMER_BLOCK       Timers[MAX_TIMERS];         /* array of timer blocks */
static INT16             Hard_Per_Soft;           /* hard timer ints per soft */
static BOOLEAN           TimersEnabled = FALSE;     /* Timers not yet enabled */

/* Storage for previous value of timer interrupt vector
*/
#ifdef HDW_INTEL
PRIVATE INTVEC            OldTimerInt;
#endif
#ifdef HDW_MC68000
#ifdef NEVER_REFERENCED
PRIVATE INTVEC            OldTimerInt;
#endif /* NEVER_REFERENCED */
#endif



#ifdef TASK_FDL
#ifdef FILESYSTEM
FILE *fp;
#endif
#endif


/************************************************************************
*               F U N C T I O N    D E F I N I T I O N S
*************************************************************************
*/


/************************************************************************
* FinUsr
*************************************************************************
*
*  Complete User dependent operations prior to MAIN shutdown.
*  EXEC calls this function at completion.
*
*/
PUBLIC void FinUsr ()
{
   /* CUSTOMIZE HERE - Enter any code that should execute after
   *  all tasks complete.
   */
   if (TimersEnabled)
      TimerOff();
#ifdef MEM_STATS
   isdn_memstats();
#endif
#ifdef STACK_STATS
   StkStats();
#endif
}

#ifdef MEM_STATS
/************************************************************************
* isdn_memstats
*************************************************************************
*
*  Function to dump memory usage statistics.  This function should
*  not be called before the system has been initialized.
*
*  Statistics for memory pool usage is aquired by the memory
*  pool management module, in a uniform manner, by calling
*  functions TestMaxUsed() and TestBlkLimit().
*
*  Statistics for "mail descriptors", "exec timer blocks" and
*  "LIF timer blocks" are aquired by ad hoc code in modules
*  exec.c and lif.c.  This is a result of those memory blocks
*  being managed by ad hoc methods.
*   
*/
PUBLIC void isdn_memstats()
{
   printf("\n\tMEMORY POOL STATISTICS (BlockType: in-use max-allowed)");

   /* Special cases for memory blocks not managed by the
    * memory pool management functions.
    */
   printf("\n\t%s: %d %d", "mail descriptors", MS_MaxMailDesc, MAX_MAIL_DESCRIPTORS);
   printf("\n\t%s: %d %d", "exec timer blocks", MS_MaxTimers, MAX_TIMERS);
   printf("\n\t%s: %d %d", "LIF timer blocks", LIF_UsedTimers, LIF_MAX_TIMERS);

   /* Statistics for memory pools managed by the
    * Memory pool management functions.
    *
    * CUSTOMIZE HERE - Add a printout of usage for each memory pool
    */
   printf("\n\t%s: %d", "PRIM_BTYPE", Used_prim);
   printf("\n\t%s: %d", "PKT_BTYPE", Used_pkt);
   printf("\n\t%s: %d", "HEADER_BTYPE", Used_header);
   printf("\n\t%s: %d", "SML_INFO_BTYPE", Used_small);
   printf("\n\t%s: %d", "LRG_INFO_BTYPE", Used_lrg);
   printf("\n\t%s: %d", "PKG_BTYPE", Used_pkg);

   printf("\n\t%s: %d", "CCBs", AllocatedCCBs);
   printf("\n\t%s: %d", "DLCBs", Used_dlcb);
   printf("\n\t%s: %d", "NLCBs", Used_nlcb);
}
#endif

#ifdef STACK_STATS
/************************************************************************
* StkStats
*************************************************************************
*
*  Function to dump stack usage statistics.  This function should
*  not be called before a TCB has been initialized.  When a TCB
*  is initialized, it's stack size is recorded, and a pattern is
*  written into the stack.  Function StackUsed, determines how
*  much of the stack was used by counting the number of stack
*  entries that still have the pattern in it (and thus have not
*  been overwritten by stack usage).
*
*/
PUBLIC void StkStats()
{
   extern TCB MonTCB;
   extern TCB TmrTCB;

   printf("\tSTACK STATISTICS\n");

   /* print stack usage for mtex system tasks
   */   
   printf("%s: %d %d\n", "mon", StackUsed(&MonTCB), MonTCB.StackSize);
   printf("%s: %d %d\n", "tmr", StackUsed(&TmrTCB), TmrTCB.StackSize);

   /* CUSTOMIZE HERE - Add a printout of usage for the
   *  stack of each task you are interested in.
   */
#ifdef TASK_1
   printf("%s: %d %d\n", "task1", StackUsed(&Task1TCB), Task1TCB.StackSize);
#endif
#ifdef TASK_2
   printf("%s: %d %d\n", "task2", StackUsed(&Task2TCB), Task2TCB.StackSize);
#endif
#ifdef TASK_3
   printf("%s: %d %d\n", "task3", StackUsed(&Task3TCB), Task3TCB.StackSize);
#endif
#ifdef TASK_TRACER
   printf("%s: %d %d\n", "Tracer", StackUsed(&TCB_TRACER), TCB_TRACER.StackSize);
#endif
}
#endif

/************************************************************************
* InitUsr
*************************************************************************
*
* Initialize user dependent Exec data.  EXEC calls this function on startup.
*
*/
PUBLIC void InitUsr()
{
   /* CUSTOMIZE HERE - Enter any code that should execute before tasks begin.
   */

#ifdef HDW_IBMPC
   atexit(TimerOff);                             /* setup exit handler */
   signal(SIGINT, ExecExit);                /* setup control-c handler */
#endif
#ifdef OS_USG
   /* === 
   signal(SIGINT, ExecExit);                 setup control-c handler */
#endif

   /* Init mail manager
   */
   InitMail(MailDescPool, MAX_MAIL_DESCRIPTORS);

#ifdef DEMO_L1
   L1_Init();
#endif

#ifdef TASK_TT
   LIF_EnterCS();
   PAD_Init();
   LIF_ExitCS();
#endif





   /* Init timer manager
   */
   InitTimer(Timers, MAX_TIMERS);

   /* Start hardware timer
   */
   TimerOn();

   /* CUSTOMIZE HERE - Initialize all application tasks.
   *
   *  CAUTION - Be sure that each TCB is initialized with the
   *  correct stack, stack size and starting function!
   */
Cisco_InitTCB(&TCB_HOST, STACK_HOST, DIM(STACK_HOST), Host_Start, "Host Task");	/* === */

#ifdef TASK_DPDRV
   /* Dual-processor I/O driver, both TX and RX tasks
   */
   DPDRV_Init();
   Cisco_InitTCB(&DPDRV_TX_TCB, Stack_TX, DIM(Stack_TX), DPDRV_Send, "DPDRV_Send");	/* """ */
   Cisco_InitTCB(&DPDRV_RX_TCB, Stack_RX, DIM(Stack_RX), DPDRV_Receive, "DPDRV_Receive");	/* """ */
#endif

#ifdef TASK_1
   Cisco_InitTCB(&Task1TCB, Stack1, DIM(Stack1), Task1, "Task1");	/* """ */
#endif

#ifdef TASK_2
   Cisco_InitTCB(&Task2TCB, Stack2, DIM(Stack2), Task2, "Task2");	/* """ */
#endif

#ifdef TASK_3
   Cisco_InitTCB(&Task3TCB, Stack3, DIM(Stack3), Task3, "Task3");	/* """ */
#endif

#ifdef TASK_M302_HDLC_1
   /* MC68302 driver test tasks
   */
#ifndef TASK_L2
   /* must be running driver test tasks, no L2
   */
   Cisco_InitTCB(&TxTaskTCB, TxStack, DIM(TxStack), TxTask, "TxTask");	/* """ */
   Cisco_InitTCB(&RxTaskTCB, RxStack, DIM(RxStack), RxTask, "RxTask");	/* """ */
#endif
   Cisco_InitTCB(&TCB_HDLC_XRQ_1, STACK_HDLC_XRQ_1, DIM(STACK_HDLC_XRQ_1),
           M302DRV_HDLC_XRQ_1_Task, "M302DRV_HDLC_XRQ_1_Task");	/* """ */
   Cisco_InitTCB(&TCB_HDLC_RCV_1, STACK_HDLC_RCV_1, DIM(STACK_HDLC_RCV_1),
           M302DRV_HDLC_RCV_1_Task, "M302DRV_HDLC_RCV_1_Task");	/* """ */
#endif
#ifdef TASK_M302_HDLC_2
    Cisco_InitTCB(&TCB_HDLC_XRQ_2, STACK_HDLC_XRQ_2, DIM(STACK_HDLC_XRQ_2),
            M302DRV_HDLC_XRQ_2_Task, "M302DRV_HDLC_XRQ_2_Task");	/* """ */
    Cisco_InitTCB(&TCB_HDLC_RCV_2, STACK_HDLC_RCV_2, DIM(STACK_HDLC_RCV_2),
            M302DRV_HDLC_RCV_2_Task, "M302DRV_HDLC_RCV_2_Task");	/* """ */
#endif
#ifdef TASK_M302_HDLC_3
    Cisco_InitTCB(&TCB_HDLC_XRQ_3, STACK_HDLC_XRQ_3, DIM(STACK_HDLC_XRQ_3),
            M302DRV_HDLC_XRQ_3_Task, "M302DRV_HDLC_XRQ_3_Task");	/* """ */
    Cisco_InitTCB(&TCB_HDLC_RCV_3, STACK_HDLC_RCV_3, DIM(STACK_HDLC_RCV_3),
            M302DRV_HDLC_RCV_3_Task, "M302DRV_HDLC_RCV_3_Task");	/* """ */
#endif




#ifdef TASK_TRACER
   Cisco_InitTCB(&TCB_TRACER, STACK_TRACER, DIM(STACK_TRACER), Tracer, "Tracer");	/* """ */
#endif
#ifdef TASK_ME
   Cisco_InitTCB(&TCB_ME, STACK_ME, DIM(STACK_ME), MEIF_StartME, "MEIF_StartME");	/* """ */
#endif
#ifdef TASK_TT
   Cisco_InitTCB(&TCB_TT, STACK_TT, DIM(STACK_TT), ttask, "ttask");	/* """ */
#endif
#ifdef TASK_RT
   Cisco_InitTCB(&TCB_RT, STACK_RT, DIM(STACK_RT), rtask, "rtask");	/* """ */
#endif
#ifdef TASK_CC
   Cisco_InitTCB(&TCB_CC, STACK_CC, DIM(STACK_CC), CC_Task, "CC_Task");	/* """ */
#endif
#ifdef TASK_CC_BRI
   Cisco_InitTCB(&TCB_CC, STACK_CC, DIM(STACK_CC), CC_Task, "CC_Task");	/* """ */
#endif
#ifdef TASK_CC_DMS_B
   Cisco_InitTCB(&TCB_CC, STACK_CC, DIM(STACK_CC), CC_Task, "CC_Task");	/* """ */
#endif
#ifdef TASK_CC_VN2
   Cisco_InitTCB(&TCB_CC, STACK_CC, DIM(STACK_CC), CC_Task, "CC_Task");	/* """ */
#endif
#ifdef TASK_CC_PRI
   Cisco_InitTCB(&TCB_CC, STACK_CC, DIM(STACK_CC), CC_Task, "CC_Task");	/* """ */
#endif
#ifdef TASK_CC_PRI_S
   Cisco_InitTCB(&TCB_CC, STACK_CC, DIM(STACK_CC), CC_Task, "CC_Task");	/* """ */
#endif
#ifdef TASK_CC_ASAI
   Cisco_InitTCB(&TCB_CC, STACK_CC, DIM(STACK_CC), CC_Task, "CC_Task");	/* """ */
#endif
#ifdef TASK_CC_5ESS
   Cisco_InitTCB(&TCB_CC, STACK_CC, DIM(STACK_CC), CC_Task, "CC_Task");	/* """ */
#endif
#ifdef TASK_CC_X25
   Cisco_InitTCB(&TCB_CC_X25, STACK_CC_X25, DIM(STACK_CC_X25), CC_X25_Task, "CC_X25_Task");	/* """ */
#endif
#ifdef TASK_P3
   Cisco_InitTCB(&TCB_P3, STACK_P3, DIM(STACK_P3), P3IF_StartP3, "P3IF_StartP3");	/* """ */
#endif
#ifdef TASK_L3
   Cisco_InitTCB(&TCB_L3, STACK_L3, DIM(STACK_L3), L3IF_StartL3, "L3IF_StartL3");	/* """ */
#endif
#ifdef TASK_V3
   Cisco_InitTCB(&TCB_V3, STACK_V3, DIM(STACK_V3), V3_Task, "V3_Task");	/* """ */
#endif
#ifdef TASK_L2
   Cisco_InitTCB(&TCB_L2, STACK_L2, DIM(STACK_L2), L2IF_StartL2, "L2IF_StartL2");	/* """ */
#endif
#ifdef TASK_HDLC_B1
   Cisco_InitTCB(&TCB_HDLC_B1, STACK_HDLC_B1, DIM(STACK_HDLC_B1), L2B1_Srq_Task, "L2B1_Srq_Task");	/* """ */
#endif
#ifdef TASK_HDLC_B2
   Cisco_InitTCB(&TCB_HDLC_B2, STACK_HDLC_B2, DIM(STACK_HDLC_B2), L2B2_Srq_Task, "L2B2_Srq_Task");	/* """ */
#endif
#ifdef TASK_HDLC_D
   Cisco_InitTCB(&TCB_HDLC_D, STACK_HDLC_D, DIM(STACK_HDLC_D), L2D_Srq_Task, "L2D_Srq_Task");	/* """ */
#endif
#ifdef TASK_KB
   Cisco_InitTCB(&TCB_KB, STACK_KB, DIM(STACK_KB), KB_Drv, "KB_Drv");	/* """ */
#endif
#ifdef TASK_UART
   Cisco_InitTCB(&TCB_UART, STACK_UART, DIM(STACK_UART), UDIF_StartUDRV, "UDIF_StartUDRV");	/* """ */
#endif
#ifdef TASK_L1
   Cisco_InitTCB(&TCB_L1, STACK_L1, DIM(STACK_L1), L1_Srq_Task, "L1_Srq_Task");	/* """ */
#endif
#ifdef TASK_ARP
   Cisco_InitTCB(&TCB_ARP, STACK_ARP, DIM(STACK_ARP), ARP_Srq_Task, "ARP_Srq_Task");	/* """ */
#endif

#ifdef TASK_FDL
   Cisco_InitTCB(&TCB_FDL, STACK_FDL, DIM(STACK_FDL), FDLIF_StartFDL, "FDLIF_StartFDL");	/* """ */
#endif

#ifdef TASK_PRTR
   Cisco_InitTCB(&TCB_PRTR, STACK_PRTR, DIM(STACK_PRTR), PRTRIF_StartPRTR, "PRTRIF_StartPRTR");	/* """ */
#endif


#ifdef ISDN
   LIF_Init();                   /* Initialize LIF global data objects */
#endif

#ifdef TASK_FDL
#ifdef FILESYSTEM
   if ( ( fp=fopen( "fdl_diag.log", "w+t" ) ) == NULL )
      {
      LIF_Error( "EXECUSER", "InitUsr", "Can't open FDL diagnostics log." );
      }
#endif
#endif
}


#ifdef 0 /* currently not used */
#ifndef APP_NO_TIMER
/************************************************************************
* TimerInt
*************************************************************************
*
* Handle interrupt from hardware timer.  With every HARD_PER_SOFT
* hardware interrupts, call TickHandler() in EXEC to signal the
* timer task.
*
*/

#ifdef HDW_INTEL
PRIVATE void interrupt TimerInt()
#endif



#ifdef CMP_MCC68K
#if 0                                                   /* MCC68K v3.3 */
#define $INTERRUPT        /* enable interrupt-type function definition */
PRIVATE void TimerInt()
#else                                                   /* MCC68K v4.1 */
/* ===
PRIVATE interrupt void TimerInt()
=== */
PRIVATE void TimerInt (void)

#endif
#endif
{
   BOOLEAN Share;

#ifdef HDW_IBMPC
   (*OldTimerInt)();
#endif                                                    /* HDW_IBMPC */

   EnterICS();                               /* Begin critical section */


   Share = FALSE;                        /* no time-sharing is default */


   /* TickHandler() is called only every HARD_PER_SOFT hardware timer tick
   */
   if (Hard_Per_Soft)                            /* decrement counter? */
      {
      --Hard_Per_Soft;
      }
   else                                            /* call tickhandler */
      {
      Hard_Per_Soft = HARD_PER_SOFT-1;           /* reset clock update */
      TickHandler();                    /* perform EXEC timer services */
      Share = TRUE;                  /* time-share after TickHandler() */
      }




   ExitTCS(Share);                       /* End timer critical section */
}

#ifdef CMP_MCC68K
#if 0                                                   /* MCC68K v3.3 */
#undef $INTERRUPT        /* Disable interrupt-type function definition */
#endif
#endif
#endif                                                /* !APP_NO_TIMER */

#endif

/************************************************************************
* TimerOff
*************************************************************************
*
* Stop the hardware timer
*
*/
PUBLIC void TimerOff()
{


#ifdef HDW_IBMPC
   /* Only set the timer interrupt vector back to the value stored
   *  in OldTimerInt if the vector is pointing to execuser's TimerInt
   */
   if (TimerInt == VectGet(MSDOS_TIMER_INT))
      VectSet(MSDOS_TIMER_INT, OldTimerInt);
#endif

}

/************************************************************************
* TimerOn
*************************************************************************
*
* Start the hardware timer
*
*/

PUBLIC void TimerOn()
{
   EnterCS();

   Hard_Per_Soft = HARD_PER_SOFT;


#ifdef HDW_IBMPC
   OldTimerInt = VectGet(MSDOS_TIMER_INT);
   VectSet(MSDOS_TIMER_INT, TimerInt);
#endif



   TimersEnabled = TRUE;                         /* indicate timers on */

   ExitCS();
}

/************************************************************************
* UserIdle
*************************************************************************
*
* Called by exec's monitor task when system is idle.
*
*/
#if 0	/* === */
PUBLIC void UserIdle()
{
#ifdef EXEC_EXIT
   /* When testing, it's nice to be able to exit gracefully when hung
   *  on a Read or SemaphoreWait.  Hit any key and the EXEC exits.
   */
   EnterNPS();
   if (kbhit())
      {
      getch();
      ExecExit();
      }
   ExitNPS();
#endif
}
#endif	/* === */

/************************************************************************
* UserTick
*************************************************************************
*
* Called by exec's timer task on every software tick.
*
* Periodic operations such as feeding hardware watchdog logic
* should be performed here rather than within the timer ISR.
* The intent is to shorten interrupt service time for the timer.
*
*/
PUBLIC void UserTick (void)
{
#ifdef TASK_CC_X25
   extern unsigned pad_TmrClock;
   
   ++pad_TmrClock;
#endif   
}

/************************************************************************
* VectGet
*************************************************************************
*
* Gets interrupt vector setting.
*
*/
PUBLIC INTVEC VectGet(vec_num)
int vec_num;

{
   INTVEC cur_isr;
   
#ifdef HDW_INTEL
#ifdef HDW_IBMPC
#ifdef CMP_TURBOC
   cur_isr = getvect(vec_num);
#endif
#else
   INTVEC far *vec_table = (INTVEC far *) 0;
   cur_isr = vec_table[vec_num];
#endif
#endif

#ifdef HDW_MC68000
   INTVEC *vec_table = (INTVEC *) 0;
   cur_isr = vec_table[vec_num];
#endif

#ifdef HDW_R4600
   cur_isr = (INTVEC)0;
#endif

   return cur_isr;
}

/************************************************************************
* VectSet
*************************************************************************
*
* Sets interrupt vector.
*
*/
PUBLIC void VectSet(vec_num, new_isr)
int vec_num;
INTVEC new_isr;

{
#ifdef HDW_INTEL
#ifdef HDW_IBMPC
#ifdef CMP_TURBOC
   setvect(vec_num, new_isr);
#endif
#else
   INTVEC far *vec_table = (INTVEC far *) 0;
   vec_table[vec_num] = new_isr;
#endif
#endif

#ifdef HDW_MC68000
   INTVEC *vec_table = (INTVEC *) 0;
   vec_table[vec_num] = new_isr;
#endif
}

