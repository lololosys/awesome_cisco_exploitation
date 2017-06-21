/* $Id: isdnexec.c,v 3.3.20.3 1996/06/19 16:20:43 rbeach Exp $
 * $Source: /release/112/cvs/Xsys/isdn/isdnexec.c,v $
 *------------------------------------------------------------------
 * exec.c  Multitasking Executive File
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: isdnexec.c,v $
 * Revision 3.3.20.3  1996/06/19  16:20:43  rbeach
 * CSCdi55115:  BRI connect fails layer 2 on reload. isdnmain
 * should wait until system init has completed before starting
 * the isdn initialization.
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/04/30  23:07:23  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/03/18  20:31:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:51:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  01:05:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/01/26  17:09:34  dclare
 * CSCdi45360:  Using ISDN TEI powerup flag will cause 4000 to crash.
 * Startup race condition causes message to be sent to MailBox 0.
 *
 * Revision 3.2  1995/11/17  17:40:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/29  19:20:22  hampton
 * Fix inverted timer test.  [CSCdi39258]
 *
 * Revision 2.3  1995/08/25 11:44:06  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.2  1995/06/30 17:29:42  rbeach
 * CSCdi34546:  After changing E1 PRI config, PRI level 2 will not come
 * up. This will also occur on a reboot. The problem was that the
 * layer 1 would synchronize and we would attempt layer 2 initialization
 * but there wasn't any isdn buffers allocated yet. The isdn process
 * was waiting for system configuration to complete.
 *
 * Revision 2.1  1995/06/07  21:15:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: exec.c
VERSION: 3.21
DESCRIPTION: Multitasking Executive File

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

The Multitasking Executive, hereinafter referred to as EXEC, is a
multi- tasking kernel designed to support real-time applications
running in a number of hardware/compiler environments. The basic
services provided by EXEC include:

1. A task scheduler which implements multi-tasking via a) a simple
non- preemptive, priority-based scheduling discipline, or b) a
preemptive, priority-based scheduling discipline (as indicated by the
compiler flag PREEMPT). Tasks request and receive critical resources
from EXEC through the use of semaphores and can shield themselves
from interrupts and/or preemption by entering "critical sections". In
case a), non-preemptive scheduling, a task can become blocked only
when requesting a shared resource semaphore. If the resource is not
available, or if there is a task waiting to run which has equal or
greater priority than the task requesting the resource, the running
task becomes blocked. In case b), preemptive scheduling, a task can
also become blocked on exits from critical and non-preempt sections
when a greater priority task is waiting to run. A form of
time-sharing is also implemented for case b) by allowing tasks whose
priorities are equal and within a specified range to share the
processor in a round-robin fashion.

2. A memory manager responsible for allocation and deallocation of
memory resources from fixed-size memory pools of user-defined memory
types.

3. A timer manager consisting of a) an interrupt service routine
(ISR) which intercepts interrupts from a hardware timer and b) a
callable timer function to provide timing support for the various
running tasks.

4. A mail manager which allows the running tasks to communicate with
each other through a well-defined and regulated mechanism.

EXEC services are designed primarily to support ISDN Layer 2 and
Layer 3 application tasks as well as system Monitoring tasks. In
addition, EXEC resources will be available to user application tasks
(terminal adapter, telephone, etc.) written to operate in the ISDN
environment.


CONVENTIONS

All defined constants, derived data types, and macros are generally
in upper case. Static variable names use mixed case with the first
letter of keywords capitalized, as in "ExpireTime".

Function names follow the same upper/lower case convention as static.
Automatic variables may be in mixed case, or entirely in lower case.


STACK MANIPULATION

EXEC performs its task (context) switching by maintaining a separate
execution stack and stack pointer for each task. Stack usage and
manipulation are obviously dependent on both the CPU and the
compiler.  Under EXEC, stack manipulation is confined to the
functions InitTCB() and _Switch(). All hardware dependent code is
conditionally compiled.


TIMER MANAGER

Timers in EXEC are essentially "delayed mail." That is, when a task
"sets" a timer, the task provides a mailbox identifier and a mail
message to be sent to the mailbox on expiration of the timer.

The active timer list is doubly-linked to provide for quick removal
of a timer block on cancellation without searching the list. The
available timer block list is singly-linked with the first element
always selected when a new block is required, and with released
blocks always placed at the front of the list.


CRITICAL SECTION STRATEGIES

1. Critical Sections

Interrupts must ALWAYS be disabled during execution of critical
sections.  Since levels of critical sections may be nested, it is
necessary to employ a level check when re-enabling interrupts at the
end of critical sections to avoid re-enabling interrupts when some
routine at a higher calling level is still in a critical section.
This is accomplished by use of the counter variable CSLevel. This
means that ALL EnterCS() calls must be balanced by ExitCS() calls.

2. Interrupt Critical Sections

Interrupt service routines (ISRs) must use the macros Enter/ExitICS()
so as not to re-enable interrupts prematurely.  The first executable
statement in an ISR MUST be EnterICS() (even though interrupts are in
fact disabled at this point). The ISR must balance this call by
calling ExitICS() at the end of the ISR. A unique case exists for the
timer interrupt routine, TimerInt. It calls ExitTCS() instead of
ExitICS() in order to perform time-sharing switches.

3. Non-Preempt Sections

Non-preempt sections (sections of code during which a task cannot be
switched out... DOS calls and printf() for example), must be shielded
with EnterNPS() and ExitNPS().

It is very important that a task which is in a critical or
non-preempt section request a resource via a SemaphoreWait() call
ONLY if the task has first determined (via a call to Test()) that the
resource is available (that is, the SemaphoreWait() call will
succeed). If this were not so, the task might be "put to sleep"
waiting on the resource while it was in a critical or non-preempt
section. SemaphoreWait() is coded to detect this condition and
generate an error if it occurs.


ERROR CONTROL STRATEGY

EXEC uses a global variable, ExecErr to hold an error code indicating
the type of the last EXEC error. EXEC routines which encounter errors
set this variable and return the constant ERR or NULL to the calling
routine.  The caller can then examine ExecErr to determine the reason
for the error return. Valid error codes for ExecErr are defined in
EXEC.H.


*************************************************************************
*************************************************************************
*/


/************************************************************************
*                E X T E R N A L    R E F E R E N C E S
*************************************************************************
*/

#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

/*
#include "../h/defs_pan.h"		for msclock define ===
*/
#include "flags.h"

/* """
#include <stdio.h>
""" */
#ifdef CMP_TURBOC
/* """
#include <stdlib.h>
#include <dos.h>
""" */
#endif

#ifdef APP_NO_TIMER
#define F_USE_FTIME 0                 /* enable or disable BSD support */
#if 0	/* === */
#if F_USE_FTIME
#include <sys/types.h>                   /* for ftime(), a 4.xBSD call */
#include <sys/timeb.h>
#else
#include <time.h>
#endif
#endif	/* === */
#endif

#include "gendef.h"
#include "exec.h"
#include "execuser.h"


#ifdef CMP_TURBOC
#ifdef HDW_IBMPC
/* On the IBM-PC and under turbo-c we can reduce the stack size
*  used by main() by doing the following trick:
*/
extern unsigned _stklen = 512;     /* stack size in words (default 2K) */
#endif
#endif


/* external low level routines
*/
extern void far _swstack (MWORD far *, MWORD far *, MWORD far *);


/************************************************************************
*                           C O N S T A N T S
*************************************************************************
*/

/* System task stack sizes
*/
#define MON_STACK_SIZE        SML_STACK_SIZE
#define TMR_STACK_SIZE        SML_STACK_SIZE


/************************************************************************
*              D A T A   T Y P E   D E F I N I T I O N S
*************************************************************************
*/

/************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S
*************************************************************************
*/

/* Pre-declare function headers for syntax checks.
*/
PRIVATE void        DeQueueTask     (TCB *);
PRIVATE void        Monitor         (void);
PRIVATE void        QueueTask       (TCB **, TCB *);
PRIVATE void        StartExec       (void);
PRIVATE void        StopExec        (void);
PRIVATE void        _Switch         (void);
PRIVATE void        TimerTask       (void);
#ifdef APP_NO_TIMER
PRIVATE void        TimerCheck      (void);
#endif


/************************************************************************
*                     D A T A   A L L O C A T I O N
*************************************************************************
*/

/* Public data
*/
/* ===
PUBLIC U_CLOCK MasterClock;                        system master clock */
PUBLIC int CSLevel;                          /* Critical section level */
PUBLIC int ExecErr;                                 /* last EXEC error */
PUBLIC int NPSLevel;                      /* non-preempt section level */
PUBLIC int TSMinPri;                   /* min priority for timesharing */
PUBLIC int TSMaxPri;                   /* max priority for timesharing */
#ifdef MEM_STATS
PUBLIC int MS_UsedMailDesc;         /* # of mail descriptors allocated */
PUBLIC int MS_MaxMailDesc;        /* high water mark of MS_MaxMailDesc */
PUBLIC int MS_UsedTimers;                     /* # of timers allocated */
PUBLIC int MS_MaxTimers;            /* high water mark of MS_MaxTimers */
#endif

/* Main program context
*/
PRIVATE TCB    MainTCB;                            /* Main program TCB */
PRIVATE SEM    MainSem;                      /* main program semaphore */

/* Monitor task things
*/
PUBLIC TCB     MonTCB;                        /* EXEC monitor task TCB */
PRIVATE unsigned MonStack[MON_STACK_SIZE];

/* Timer task things
*/
PUBLIC TCB       TmrTCB;                        /* EXEC timer task TCB */
PRIVATE SEM      TmrSem;                       /* Timer task semaphore */
PRIVATE unsigned TmrStack[TMR_STACK_SIZE];

/* Scheduler static data
*/
PRIVATE TCB    *ReadyList;                 /* First task in ready list */
PRIVATE TCB    *RunningTask;                 /* Task currently running */

/* Timer manager timer static data
*/
PRIVATE TIMER_BLOCK   *AvailTimers;        /* Head of timer avail list */
TIMER_BLOCK    *ActiveTimers;             /* Head of active timer list */
PRIVATE SEQNO  TimerSn;     /* sequence number counter of timer blocks */

/* Mail Manager static data
*/
PRIVATE MAIL_DESCRIPTOR *AvailMailDesc;          /* head of avail list */



/************************************************************************
*                              M A C R O S
*************************************************************************
*/

#ifdef DEBUG

/* We use macros for the functions involved in task switching so that
*  debugging statements can be inserted BEFORE the function call in order
*  to identify the source of the error without the need for stack
*  traceback.
*/

#define EndCS() { \
   DASSERT((CSLevel == 1), UNBALANCED_CS); \
   CSLevel = 0; \
   Enable(); \
   }

#define Switch() { \
   DASSERT((CSLevel == 1 && NPSLevel == 0), BAD_SWITCH); \
   DASSERT((ReadyList && ReadyList->Priority != SUSPEND_PRI), BAD_SWITCH); \
   _Switch(); \
   }

#else

#define EndCS() { \
   CSLevel = 0; \
   Enable(); \
   }

#define Switch() \
   _Switch()

#endif


/************************************************************************
*                     P U B L I C   F U N C T I O N S
*************************************************************************
*/


/************************************************************************
* main
*************************************************************************
*
*  Main program.  Initialize the executive data, initialize the user
*  data, 'hang up' the main program so tasks can run.
*
*/
PUBLIC void isdnmain (void)
{
   extern isdn_exit(int);

   process_wait_on_system_config();         /* DO NOT REMOVE THIS LINE */
   StartExec();                                /* Initialize executive */
   InitUsr();                                       /* Initialize user */
   SemaphoreWait(&MainSem);                /* wait for tasks to finish */
   FinUsr();                                    /* user shutdown stuff */
   StopExec();                                   /* Shutdown executive */

   isdn_exit(ExecErr);
}

/************************************************************************
* ClearTimer
*************************************************************************
*
* Clears a timer before expiry.
*
* ClearTimer has one small problem.  Assume task1 has set a timer and
* then waits for mail.  When the timer expires, the timed mail is placed
* in the mailbox, and task1 is placed in the ready queue.  Now assume
* that before task1 gets a chance to run, ClearTimer is called, which
* would cause the expired timer to be removed from the mailbox.  Now when
* task1 gets it's chance to run, it will be executing code in ReadMail.
* ReadMail assumes that when a task awakens, there is at least one piece
* of mail in the mailbox queue.  But in this case there isn't, because
* ClearTimer has removed it.
*
* This problem can be solved in one of two way:
*
*     1) Modify ReadMail so that it checks for an empty mail queue after
*     the task wakes up, and if empty, makes the task go to sleep again.
*
*     2) By placing a restriction on the use of ClearTimer such that
*     timers may not be cleared while a task is waiting for mail from
*     the mailbox to which the timer has been sent.
*
*/
PUBLIC void ClearTimer(Tid, Sn, MailBox)
TIMER_BLOCK *Tid;                         /* address of timer to clear */
SEQNO Sn;                                  /* sequence number of timer */
MAILBOX *MailBox;              /* mailbox to which mail was to be sent */

{
   MAIL_DESCRIPTOR *mail, *prev;
   MAIL_Q *tmq;

   EnterCS();

   /* Check if the timer is in use (not available) and has a matching
   *  sequence number.
   */
   if ( Tid ) {
     if (! Tid->Avail && Sn == Tid->Sn)
      {
      /* move timer block from active list to available list
      *  and mark the block as being available again.
      */
      if (Tid == ActiveTimers)              /* At head of active list? */
         {
         /* remove timer from head of active list
         */
         ActiveTimers = ActiveTimers->Next;
         }
      else                                      /* Not at head of list */
         {
         /* remove timer from within active list
         */
         Tid->Prev->Next = Tid->Next;
         if (Tid->Next != NULL)
            Tid->Next->Prev = Tid->Prev;
         }
      Tid->Next = AvailTimers;
      AvailTimers = Tid;
      Tid->Avail = TRUE;
#ifdef MEM_STATS
      --MS_UsedTimers;
#endif
      }
     }
   else
      {
      /* A matching timer was not in the active list, so it must have
      *  expired.  However, the timed mail associated with the timer
      *  may still be in the mail queue, awaiting delivery.  Search
      *  the mail queue for the timed mail, and if found, remove it.
      *
      *  NOTE: An assumption is made that timed mail is always queued
      *  to the DEFAULT_DIR mail queue.
      */
      tmq = (DEFAULT_DIR == DOWN) ? &MailBox->Down : &MailBox->Up;

      for (prev = NULL, mail = tmq->Head; mail != NULL;
         prev = mail, mail = mail->Next)
         {
         if (mail->Sn == Sn)
            {
            /* adjust semaphore value
            */
            SemaphoreWait(&MailBox->Sem);

            if (mail == tmq->Head)                 /* remove from head */
               tmq->Head = mail->Next;
            else
               prev->Next = mail->Next;            /* remove from body */

            /* Deallocate mail descriptor block
            */
            mail->Next = AvailMailDesc;   /* add to head of avail list */
            AvailMailDesc = mail;
            ++tmq->Avail;
#ifdef MEM_STATS
            --MS_UsedMailDesc;
#endif
            break;                                      /* stop search */
            }
         }
      }

   ExitCS();
}

/************************************************************************
* ExecExit
*************************************************************************
*
* Make a "clean" exit from MTEX.
*
*/
PUBLIC void ExecExit()

{
   SAVE_CALLER();

   buginf("\nExecExit called from 0x%x", caller());
   SemaphoreSignal(&MainSem);
   SemaphoreWait(&MainSem);
}

/************************************************************************
* _ExitCS
*************************************************************************
*
* Leave a Critical Section of code, re-enabling interrupts if
* appropriate. Since it is possible that a routine which is in a
* critical section may call another routine which also enters a critical
* section, we cannot simply re-enable interrupts because this would
* result in a return to the original calling routine, which is still
* in its CS, with interrupts enabled. To avoid this situation, a CS
* level (or depth) variable, CSLevel, is employed. On each call to
* EnterCS(), the variable is incremented by 1, while it is decremented on
* each call to ExitCS(). A positive value n of CSLevel indicates that n
* routines are in critical sections. Interrupts are re-enabled by ExitCS()
* only if CSLevel is 0.
*
* Note that it is implicit in this scheme that interrupts are DISABLED
* when ExitCS() is called. Otherwise, access to the variable CSLevel,
* itself a critical resource, would be in question.
*
*/
PUBLIC void _ExitCS()

{
   /* Task switch and re-enable only if leaving CS and no one is in NPS
   */
   if (CSLevel == 1)
      {
#ifdef PREEMPT
      if (NPSLevel == 0)
         {
         if (ReadyList && ReadyList->Priority != SUSPEND_PRI)
            {
            /* switch tasks if appropriate
            */
            if (RunningTask->Priority < ReadyList->Priority)
               {
               /* queue running task on ready list
               */
               RunningTask->Status = READY;
               QueueTask(&ReadyList, RunningTask);

               /* start task at head of ready list
               */
               Switch();
               }
            }
         }
#endif
      EndCS();
      }
   else
      {
      CSLevel--;                                   /* pop up one level */
      }
}

/************************************************************************
* _ExitICS
*************************************************************************
*
* Leave a Critical Section of code during interrupt processing.
*
*/
PUBLIC void _ExitICS()

{
   /* Task switch and re-enable only if leaving CS and no one is in NPS
   */
#ifdef PREEMPT
   if (CSLevel == 1)
      {
      if (NPSLevel == 0)
         {
         if (ReadyList && ReadyList->Priority != SUSPEND_PRI)
            {
            /* switch tasks if appropriate
            */
            if (RunningTask->Priority < ReadyList->Priority)
               {
               /* queue running task on ready list
               */
               RunningTask->Status = READY;
               QueueTask(&ReadyList, RunningTask);

               /* start task at head of ready list
               */
               Switch();
               }
            }
         }
      }
#endif

   CSLevel--;                                      /* pop up one level */
}

/************************************************************************
* _ExitNPS
*************************************************************************
*
* Leave non-preempt section.
*
*/
PUBLIC void _ExitNPS()

{
   NPSLevel--;                                     /* pop up one level */

   /* Task switch and re-enable only if not in CS and no one is in NPS
   */
#ifdef PREEMPT
   if (CSLevel == 0)
      {
      if (NPSLevel == 0)
         {
         EnterCS();                              /* switch tasks in CS */
         if (ReadyList && ReadyList->Priority != SUSPEND_PRI)
            {
            /* switch tasks if appropriate
            */
            if (RunningTask->Priority < ReadyList->Priority)
               {
               /* queue running task on ready list
               */
               RunningTask->Status = READY;
               QueueTask(&ReadyList, RunningTask);

               /* start task at head of ready list
               */
               Switch();
               }
            }
         EndCS();
         }
      }
#endif
}

/************************************************************************
* _ExitTCS
*************************************************************************
*
* Leave a Critical Section of code after a timer interrupt,
* attempting a time-sharing switch if appropriate (if input Share is
* TRUE).
*
*/
PUBLIC void _ExitTCS(Share)
BOOLEAN Share;

{
   /* Task switch and re-enable only if leaving CS and no one is in NPS
   */
#ifdef PREEMPT
   if (CSLevel == 1)
      {
      if (NPSLevel == 0)
         {
         if (ReadyList && ReadyList->Priority != SUSPEND_PRI)
            {
            /* switch tasks if appropriate
            *
            * A time sharing task switch can be performed if the
            * running task has a priority in the proper range and
            * if the task at the head of the ready list has a priority
            * equal to or greater than that of the running task.
            */
            if ((RunningTask->Priority < ReadyList->Priority)
                  || (Share &&
                  (RunningTask->Priority >= TSMinPri) &&
                  (RunningTask->Priority <= TSMaxPri) &&
                  (RunningTask->Priority <= ReadyList->Priority))
               )
               {
               /* queue running task on ready list
               */
               RunningTask->Status = READY;
               QueueTask(&ReadyList, RunningTask);

               /* start task at head of ready list
               */
               Switch();
               }
            }
         }
      }
#endif

   CSLevel--;                                      /* pop up one level */
}

/************************************************************************
* InitMail
*************************************************************************
*
* Initialize Mail data structures
*
*/
PUBLIC void InitMail(MailDescPool, MaxMailDescriptors)
MAIL_DESCRIPTOR *MailDescPool;               /* addr of mail desc pool */
int MaxMailDescriptors;                   /* max # of mail descriptors */

{
   int i;

   /* Create mail descriptor available list
   */
   AvailMailDesc = &MailDescPool[0];

   /* Chain the descriptors into the available list
   *  NOTE: MaxMailDescriptors MUST be >= 2
   */
   for (i = 0; i < (MaxMailDescriptors - 1) ; i++)
      {
      MailDescPool[i].Next = &MailDescPool[i+1];
      }

   MailDescPool[MaxMailDescriptors - 1].Next = NULL;
#ifdef MEM_STATS
   MS_MaxMailDesc = 0;
   MS_UsedMailDesc = 0;
#endif
}


#ifdef STACK_STATS
/************************************************************************
* StackUsed
*************************************************************************
*
* Determine how much of a stack was used after a run of the system.
*
* When InitTCB() was called, a pattern of was written into the stack.
* Unless the stack overflowed, the top of the stack will still contain
* this pattern.  This routine returns the size of the stack minus
* the length of the pattern, which should be the amount of the stack
* which was used.
*
*/
PUBLIC int StackUsed(tcb)
TCB *tcb;                             /* pointer to task control block */

{
   MWORD *stack;                            /* pointer to task's stack */
   int size;
   int i;

   size = tcb->StackSize;

#if F_PUSH_NEG
   /* stack push is negative; start search for end of pattern at top of
   *  stack (low memory) and work towards bottom of stack (high memory).
   */
   stack = tcb->Stack;
   for (i = 0; i < size; ++i, ++stack)
#else
   /* stack push is positive; start search for end of pattern at bottom
   *  of stack (high memory) and work towards top of stack (low memory).
   */
   stack = tcb->Stack + (size - 1);
   for (i = 0; i < size; ++i, --stack)
#endif
      {
      if (*stack != (unsigned) ~0)
         break;
      }

   return size - i;
}
#endif

/************************************************************************
* TaskBegin
*************************************************************************
*
*  Main body for all tasks
*
*/
static PUBLIC void far TaskBegin (void)
{
   /* Start the task
   */
   EndCS();
   (*RunningTask->Task)();

   /* The task has resigned - shut it down
   */
   EnterCS();
   RunningTask->Status = RESIGNED;
   Switch();

   /* Should never reach this point
   */
   ExecErr = NOT_DEAD;
   EndCS();
   ExecExit();
}

/************************************************************************
* InitTCB
*************************************************************************
*
* Initialize a task control block (TCB).
*
* We initialize the stack to simulate the state it would be in if it had
* been "put to sleep" (i.e. switched out) by the function _Switch(),
* which is responsible for switching from one task to another. _Switch()
* does this by first remembering the stack and stack pointer for the
* currently running task and then changing to a new task's stack and
* stack pointer. _Switch() then "returns" to the new task. This works
* fine when the new task was put to sleep by _Switch(). But the first
* time a task is started, this will not be true. Thus, we must initialize
* each new task (i.e. TCB) as though the task had, in fact, been put to
* sleep by _Switch(). This entails simulating any arguments placed on the
* stack on entry to _Switch() so that when _Switch() returns, the proper
* number of values will be removed from the stack. (There may or may not
* be any such dummy arguments... it varies for each memory model and each
* compiler. The only way to determine the correct number is to inspect the
* assembly code generated for _Switch()).
*
* On exit from InitTCB, a new task's stack is set up as follows:
*
*     dummy values to be popped off by swstack()      top
*     return address of TaskBegin()                   bot
*
*	This is somewhat rewritten for cisco.  I added the task name part to
*	aid debugging RBK ===
*
*/
PUBLIC void Cisco_InitTCB (tcb, stack, size, task, name) /* === */
TCB *tcb;                              /* ptr to TCB to be initialized */
MWORD *stack;                              /* ptr to TCB's stack space */
int size;                              /* size of TCB's stack in words */
void (*task)(void);                             /* task's starting address */
char *name;	/* === */

{
   /* Write known pattern into task stack, for post-mortem analysis.
   *  This must be done before initializing the stack.
   */
   MemSet((BYTE *) stack, ~0, sizeof(MWORD) * size);  /* write pattern */
   tcb->Stack = stack;                        /* record stack location */
   tcb->StackSize = size;                         /* record stack size */

   /* Store address of task's startup function in task control block
   */
   tcb->Task = task;
   tcb->Name = name;			/* === */

#ifdef HDW_INTEL
   /*
    * Munge the stack for an Intel based box
    */
   {
   MWORD *top;                            /* temp pointer to stack top */
   void (far *func)();             /* far pointer to starting function */

   /* Point to top of stack.
   */
   top = stack + size;

   /* Place address of TaskBegin() on stack top.
   *  A far pointer is used, because the call to _swstack is far.
   */
   func = TaskBegin;
   *--top = (MWORD) ((unsigned long) func >> 16);
   *--top = (MWORD) func;

   /* push dummy register variables & stack frame pointer
   */
   top -= 3;

   /* record task context
   */
   tcb->Contxt[0] = (MWORD) ((unsigned long) top >> 16);
   tcb->Contxt[1] = (MWORD) top;
   }
#endif

#ifdef HDW_MC68000
   /*
    * Munge the stack for a Motorola 68k based box
    */
   {
   MWORD *top;                            /* temp pointer to stack top */

   /* Point to top of stack.
   */
   top = stack + size;

   /* Place address of TaskBegin() on stack top
   */
   *--top = (MWORD) TaskBegin;

   /* push dummy register variables & stack frame pointer
   */
   top -= 15;

   /* record task context
   */
   tcb->Contxt[0] = (MWORD) top;
   }
#endif

#ifdef HDW_R4600
   /*
    * Munge the stack for R4600 Orion based box
    */
   {
   MWORD *top;                            /* temp pointer to stack top */

   /* Point to top of stack.
   */
   top = stack + size;

   /* decrement stack pointer to make space for registers that
    * will be popped off by _swstack(). The frame is 40 bytes.
    */
   top -= 10;

   /* Place address of TaskBegin() into the stack frame in the
    * position for the ra register
    */
   *top = (MWORD) TaskBegin;

   /* record task context
   */
   tcb->Contxt[0] = (MWORD) top;
   }
#endif

   /* Place task in ready list
   */
   EnterCS();
   tcb->Status = READY;
   QueueTask(&ReadyList, tcb);
   ExitCS();
}

/************************************************************************
* InitTimer
*************************************************************************
*
* Initialize timer manager data structures.
*
*/
PUBLIC void InitTimer(Timers, MaxTimers)
TIMER_BLOCK *Timers;                           /* addr of timer blocks */
int MaxTimers;                                      /* max # of timers */

{
   int i;

   ActiveTimers = NULL;                            /* No timers in use */

   AvailTimers = (TIMER_BLOCK *) Timers;

   /* Chain the timer blocks into the available list
   *  NOTE: MaxTimers MUST be >= 2
   */
   for (i = 0; i < (MaxTimers-1) ; i++)
      {
      Timers[i].Next = &Timers[i+1];
      }

   Timers[MaxTimers-1].Next = NULL;

/* ===
   MasterClock = 0;                                   Set master clock */
   TimerSn = 0;                  /* Set global sequence number counter */
#ifdef MEM_STATS
   MS_UsedTimers = 0;
   MS_MaxTimers = 0;
#endif
}

/************************************************************************
* MemSet
*************************************************************************
*
* Fill a block of memory with a specified BYTE value
*
*/
PUBLIC void MemSet(dest, value, size)
BYTE *dest;                              /* start addr of area to fill */
BYTE value;                      /* BYTE value to store in memory area */
unsigned size;                           /* # unsigned values to store */

{
   register unsigned i;

   for (i = size; i; --i)
      *dest++ = value;
}

/************************************************************************
* ReadMail
*************************************************************************
*
* Read a mail descriptor from a specified mailbox
*
* !!!NOTA BENE!!!
* If ReadMail is called from within a critical section, it MUST be
* preceded by a TestMail of the mailbox and then called only if the
* TestMail shows mail is present, i.e., only if the ReadMail will succeed.
* The task will then continue running, WITHOUT being subject to
* rescheduling (because the Wait checks for CSLevel). If a ReadMail within
* a critical section fails, i.e., the TestMail failed and the task will
* be queued, the executive will abort (exit) with an appropriate error code.
*
*/
PUBLIC void ReadMail(MailBox, ExtMem)
MAILBOX *MailBox;                        /* address of mailbox to read */
EXT_MEM *ExtMem;                               /* extended memory addr */

{
   MAIL_DESCRIPTOR *MailDesc;                          /* temp pointer */
   MAIL_Q *up, *down, *tmq;                           /* temp pointers */

   SemaphoreWait(&MailBox->Sem);            /* wait for mail to arrive */

   EnterCS();

   up = &MailBox->Up;
   down = &MailBox->Down;

   /* Decide from which mail queue to read
   */
   if (down->Head == NULL)
      tmq = up;                              /* down q empty, use up q */
   else if (up->Head == NULL)
      tmq = down;                            /* up q empty, use down q */
   else if (up->Head->Priority > down->Head->Priority)
      tmq = up;                            /* up q has higher priority */
   else if (up->Head->Priority < down->Head->Priority)
      tmq = down;                        /* down q has higher priority */
   else if (MailBox->LastRead == down)
      tmq = up;                              /* up q not read recently */
   else
      tmq = down;                          /* down q not read recently */

   MailBox->LastRead = tmq;   /* record which queue most recently read */

   MailDesc = tmq->Head;
   ExtMem->Addr = MailDesc->ExtMem.Addr;            /* set msg address */
   tmq->Head = tmq->Head->Next;                    /* unlink mail desc */
   ++tmq->Avail;                        /* mail slot becomes available */

   /* Deallocate mail descriptor block
   */
   MailDesc->Next = AvailMailDesc;        /* add to head of avail list */
   AvailMailDesc = MailDesc;
#ifdef MEM_STATS
   --MS_UsedMailDesc;
#endif

   ExitCS();
}

/************************************************************************
* SemInit
*************************************************************************
*
*/
PUBLIC void SemInit(sem, val)
SEM *sem;
int val;

{
   sem->Head = NULL;
   sem->Value = val;
}

/************************************************************************
* SemaphoreSignal
*************************************************************************
*
* Signal the release of a semaphore and schedule the first
* task waiting on the semaphore.
*
*/
PUBLIC void SemaphoreSignal(sem)
SEM *sem;                            /* address of semaphore to signal */

{
   TCB *Task;

   EnterCS();

   /* If a task is waiting for the semaphore, remove a task from
   *  semaphore's wait queue and place it on the ready list
   */
   if (++sem->Value <= 0)
      {
      if ((Task = sem->Head) != NULL)
         {
         /* unlink and schedule task
         */
         sem->Head = Task->Next;
         Task->Status = READY;
         QueueTask(&ReadyList, Task);
         }
      }

   ExitCS();
}

/************************************************************************
* SemaphoreWait
*************************************************************************
*
* Called when a task requires the exclusive use of a semaphore (resource)
* and must wait for its availability. If the resource is available and
* the calling task does not have priority over the first task in the
* ready list, the calling task is returned to the ready list (in priority
* order) and the task at the head of the ready list is started. If the
* resource is unavailable, the task is placed in the semaphore's waiting
* queue and the task at the head of the ready list, if any, is started.
* Note that if the resource is available and this task has high priority
* or the calling task has disabled interrupts, it remains the running task.
*
* !!!NOTA BENE!!!
* If SemaphoreWait() is called from within a critical or non-preempt
* section, it MUST be preceded by a Test() of the semaphore and then
* called only if the Test() shows a value greater than zero, i.e., only
* if the Wait will succeed. The task will then continue running unless the
* task at the head of the ready list has equal or greater priority than
* the running task. If a SemaphoreWait() within a critical section fails,
* i.e.,  the semaphore value was not positive and the task will be
* queued, the  executive will abort (exit) with an appropriate error
* code.
*
*/
PUBLIC void SemaphoreWait(sem)
SEM *sem;                           /* address of semaphore to wait on */

{
   int Schedule;               /* does this task need to be scheduled? */

#ifdef APP_NO_TIMER
   TimerCheck();
#endif

   Schedule = FALSE;

   EnterCS();

   /* If semaphore is not available, place running task on semaphore's
   *  wait queue, then schedule the next task in the ready list for
   *  execution. Otherwise, the calling task is re-queued on the
   *  ready list.  If this task would be at the head of the ready list,
   *  or the calling task has disabled interrupts, don't re-queue.
   */
   if (--sem->Value < 0)
      {
      /* Debug check for legal call
      */
      DASSERT((CSLevel <= 1 && NPSLevel == 0), BAD_WAIT);

      /* Place task in semaphore's wait queue
      */
      RunningTask->Status = BLOCKED;
      QueueTask(&sem->Head, RunningTask);
      Schedule = TRUE;
      }
   else if (ReadyList)
      {
      /* Queue task on ready list unless it would still be at the
      *  head of the list.
      */
      if (RunningTask->Priority <= ReadyList->Priority &&
         CSLevel <= 1 &&
         NPSLevel == 0)
         {
         /* queue task on ready list
         */
         RunningTask->Status = READY;
         QueueTask(&ReadyList, RunningTask);
         Schedule = TRUE;
         }
      }

   /* start a new task if indicated
   */
   if (Schedule)
      {
      Switch();                      /* start first task in ready list */
      EndCS();
      }
   else
      {
      ExitCS();
      }
}

/************************************************************************
* SendTMail
*************************************************************************
*
* Link a mail descriptor to the mail queue of the specified mailbox
*
* Returns the number of available mail slots, if successful; ERR if the
* mailbox is full or no mail descriptors are available.
*
* GUARANTEED MAIL: If the mailbox is full and the priority is
*                  MUST_SEND_PRI, the mail is added to the box anyway.
*
*/
PUBLIC int SendTMail(MailBox, Direction, ExtMem, Priority, Sn)
MAILBOX *MailBox;                    /* address of destination mailbox */
int Direction;                   /* mail goes to UP or DOWN mail queue */
EXT_MEM *ExtMem;     /* extended memory address containing msg to mail */
int Priority;                                      /* priority of mail */
SEQNO Sn;        /* sequence number of mail (valid only if timed mail) */

{
   MAIL_DESCRIPTOR *MailDesc, *TDesc;                   /* temporaries */
   MAIL_Q *tmq;                                                /* temp */
   int available;                    /* number of mail slots available */

   EnterCS();

   available = ERR;

   tmq = (Direction == DOWN) ? &MailBox->Down : &MailBox->Up;

   /* Check for full mail queue
   */
   if (tmq->Avail <= 0)
      {
      if (Priority != MUST_SEND_PRI)
         {
         ExecErr = MAILBOX_FULL;
         if(isdn_debug)
	        buginf("\nISDN MAILBOX FULL AGAIN SendTMail, Available = %d, Limit = %d", tmq->Avail, tmq->Limit);
         goto Out;
         }
      }

   /* Allocate a mail descriptor
   */
   MailDesc = AvailMailDesc;
   if (MailDesc == NULL)
      {
      ExecErr = NO_MAIL_DESCRIPTORS;
      goto Out;
      }
   else
      {
      AvailMailDesc = AvailMailDesc->Next;    /* unlink the descriptor */
#ifdef MEM_STATS
      if (++MS_UsedMailDesc > MS_MaxMailDesc)
         MS_MaxMailDesc = MS_UsedMailDesc;
#endif
      }

   /* Link Mail descriptor into specified mail queue based on the mails
   *  priority (larger priority values are given highest priority).
   */
   if (tmq->Head == NULL)                      /* mailbox queue empty? */
      {
      tmq->Head = MailDesc;                /* only descriptor in queue */
      MailDesc->Next = NULL;
      }
   else        /* add mail to mailbox's descriptor list using priority */
      {
      TDesc = tmq->Head;
      if (TDesc->Priority < Priority)             /* add to beginning? */
         {
         MailDesc->Next = tmq->Head;
         tmq->Head = MailDesc;
         }
      else                                           /* sort into list */
         {
         while (TDesc->Next != NULL)
            {
            if (TDesc->Next->Priority < Priority)
               break;
            TDesc = TDesc->Next;
            }
         MailDesc->Next = TDesc->Next;
         TDesc->Next = MailDesc;
         }
      }

   available = --tmq->Avail;

   /* Remember message address / priority
   */
   MailDesc->ExtMem.Addr = ExtMem->Addr;
   MailDesc->Priority = Priority;
   MailDesc->Sn = Sn;

Out:

   ExitCS();

   if (available != ERR)
      SemaphoreSignal(&MailBox->Sem);           /* signal mail arrival */

   return(available);
}

/************************************************************************
* SetPriority
*************************************************************************
*
* Set a task's priority
*
*/
PUBLIC void SetPriority(Task, Priority)
TCB *Task;                   /* Id of task whose priority is to be set */
int Priority;                           /* new priority value for task */

{
   TCB **Queue;                                    /* queue task is in */

   EnterCS();

   Task->Priority = Priority;                      /* set new priority */

   /* if setting priority of another task that is queued, make the
   *  change immediate by de-queueing and re-queueing the task
   */
   if (Task->Status == READY || Task->Status == BLOCKED)
      {
      Queue = Task->Queue;           /* queue on which task is waiting */
      DeQueueTask(Task);                          /* remove from queue */
      QueueTask(Queue, Task);            /* re-queue with new priority */
      }

   ExitCS();
}

/************************************************************************
* SetTimer
*************************************************************************
*
* Set a timer for task. On expiry, a message is mailed to a specified
* mailbox by the timer task. A timer address and timer sequence number are
* set for the caller.  These can be supplied to ClearTimer() to cancel
* the timer before expiry.
*
* A timer is "set" by filling in a free timing block and linking it to
* the timer manager's Active list. The timer's expire time is calculated
* by adding the requested timer count to the current value of the system
* master clock. This "absolute" time is then used to locate the timer's
* position in the Active chain (the chain is sorted numerically on the
* expire time fields).
*
* Returns ERR if no timer blocks available, NOERR otherwise.
*
*/
PUBLIC int SetTimer(Mailbox, ExtMem, Count, Tid, Sn, Priority)
MAILBOX *Mailbox;        /* address of mailbox to receive "wakeup" msg */
EXT_MEM *ExtMem;             /* extended memory address of msg to mail */
int Count;                                    /* timer countdown value */
TIMER_BLOCK **Tid;                           /* timer address (output) */
SEQNO *Sn;                           /* timer sequence number (output) */
int Priority;                                      /* priority of mail */

{
   TIMER_BLOCK *tblk;                       /* new timer block pointer */
   TIMER_BLOCK *t1;
   int result;

   /* Attempt to get a timer block from list of avail timers
   */
   EnterCS();
   if ((tblk = AvailTimers) != NULL)
      {
      AvailTimers = AvailTimers->Next;
#ifdef MEM_STATS
      if (++MS_UsedTimers > MS_MaxTimers)
         MS_MaxTimers = MS_UsedTimers;
#endif
      }
   ExitCS();

   if (tblk == NULL)
      {
      /* no timer blocks available
      */
      result = ERR;
      ExecErr = NO_TIMERS;
      }
   else
      {
      /* timer block available
      */
      result = NOERR;

      /* initialize timer block
      */
/*    tblk->ExpireTime = MasterClock + Count;		   === */
      TIMER_START(tblk->ExpireTime, Count);		/* === */
      tblk->MailboxID = Mailbox;
      tblk->ExtMem.Addr = ExtMem->Addr;
      tblk->Avail = FALSE;
      tblk->Sn = TimerSn++;
      tblk->Priority = Priority;

      /* Insert timer 'tblk' into active list
      */
      EnterCS();
      if (ActiveTimers == NULL)
         {
         /* add to empty list
         */
         ActiveTimers = tblk;
         tblk->Next = NULL;
         }
      else if (CLOCK_DIFF_SIGNED(tblk->ExpireTime, ActiveTimers->ExpireTime) > 0)
         {
         /* This new timer will expire before any others in the active
	 *  timer list; add it to the beginning of the list.
         */
         tblk->Next = ActiveTimers;
         ActiveTimers->Prev = tblk;
         ActiveTimers = tblk;
         }
      else
         {
         /* sort into list
         */
         for (t1 = ActiveTimers; t1->Next; t1 = t1->Next)
            {
            if (CLOCK_DIFF_SIGNED(tblk->ExpireTime, t1->Next->ExpireTime) > 0)
               break;
            }
         tblk->Next = t1->Next;
         tblk->Prev = t1;
         if (t1->Next != NULL)
            t1->Next->Prev = tblk;
         t1->Next = tblk;
         }
      ExitCS();

      /* Return Timer address and Sequence Number
      */
      *Tid = tblk;
      *Sn = tblk->Sn;
      }

   return result;
}

#ifdef DEBUG
/************************************************************************
* ShowQueue
*************************************************************************
*
* Display tasks in a (semaphore) queue
*
*/
PUBLIC void ShowQueue(sem)
SEM *sem;                                   /* semaphore queue to show */

{
   TCB *TempTask;

   EnterCS();

   printf("Semaphore %lx List:\n   ", (unsigned long) sem);
   TempTask = sem->Head;
   while (TempTask != NULL)
      {
      printf("%lx -> ", (unsigned long) TempTask);
      TempTask = TempTask->Next;
      }
   printf("NULL\n");

   ExitCS();
}

/************************************************************************
* ShowReadyList
*************************************************************************
*
* Display tasks in ready list
*
*/
PUBLIC void ShowReadyList()

{
   TCB *TempTask;

   EnterCS();

   printf("Ready List:\n   ");
   TempTask = ReadyList;
   while (TempTask != NULL)
      {
      printf("%lx -> ", (unsigned long) TempTask);
      TempTask = TempTask->Next;
      }
   printf("NULL\n");

   ExitCS();
}

/************************************************************************
* ShowTimers
*************************************************************************
*
* Display Timer Active and Avail lists
*
*/
PUBLIC void ShowTimers()

{
   TIMER_BLOCK *t1;

   EnterCS();

   printf("\nTimer available chain starts with %4x \n", AvailTimers);
   t1 = AvailTimers;
   while (t1 != NULL)
      {
      printf("  %4x:  %4x\n",t1, t1->Next);
      t1 = t1->Next;
      }

   printf("\nTimer active chain starts with %4x \n",ActiveTimers);
   t1 = ActiveTimers;
   while (t1 != NULL)
      {
      printf("  %4x:  %4x  %4x\n",t1, t1->Prev, t1->Next);
      t1 = t1->Next;
      }

   ExitCS();
}
#endif                                                        /* DEBUG */

/************************************************************************
* Test
*************************************************************************
*
*  Returns the value of a semaphore
*
*/
PUBLIC int Test(sem)
SEM *sem;

{
   return sem->Value;
}

/************************************************************************
* TickHandler
*************************************************************************
*
* Called by execuser's timer interrupt handler.
*
*/
PUBLIC void TickHandler()

{
/* ===
   ++MasterClock;                               Increment master clock */
   SemaphoreSignal(&TmrSem);
}

/************************************************************************
* MailBoxInit
*************************************************************************
*
*  Dynamically initialize a mailbox.
*
*/
PUBLIC void MailBoxInit(mailbox, limit)
MAILBOX *mailbox;
int limit;

{
   mailbox->Sem.Value = 0;
   mailbox->Sem.Head = NULL;
   mailbox->Up.Limit = limit;
   mailbox->Up.Avail = limit;
   mailbox->Up.Head = NULL;
   mailbox->Down.Limit = limit;
   mailbox->Down.Avail = limit;
   mailbox->Down.Head = NULL;
   mailbox->LastRead = NULL;
}


/************************************************************************
*                     P R I V A T E    F U N C T I O N S
*************************************************************************
*/

      
/************************************************************************
* DeQueueTask
*************************************************************************
*
* Remove a task from a queue (singly-linked list).
* It is assumed that this routine is called from a critical section.
*
*/
PRIVATE void DeQueueTask(task)
TCB *task;                           /* address of task to be dequeued */

{
   TCB **queue;
   TCB *temp;

   queue = task->Queue;                   /* point to queue task is in */

   if (*queue == task)
      {                              /* remove task from head of queue */
      *queue = task->Next;
      }
   else
      {                               /* find task in queue and remove */
      for (temp = *queue; temp->Next != task; temp = temp->Next)
         ;
      temp->Next = task->Next;
      }

   task->Queue = NULL;
   task->Next = NULL;
}

/************************************************************************
* Monitor
*************************************************************************
*
* EXEC Monitor task. This task is designed to run only when there are
* no runnable tasks in the ready list. This is accomplished by giving
* it the lowest runnable priority MON_PRI. It also provides the point
* where a check is made for the condition to shutdown the EXEC.
*
* !!!NOTA BENE!!!
* Under preemption, the monitor task must take no action (e.g. wait
* on a semaphore) which might result in the monitor task becoming BLOCKED.
* This would effectively leave no runnable tasks in the ready list and
* the next call to _Switch() would result in system failure. If preemption
* is disabled, the situation is reversed... the monitor MUST at some point
* perform an action which would result in relinquishing the processor if
* another task is waiting to run. This is accomplished by signaling and
* then waiting on a private semaphore. This process causes the monitor task,
* with its low priority to be re-queued behind any other tasks in the ready
* list.
*
*/
PRIVATE void Monitor ()
{
   extern void UserIdle (void);        /* external user function */
   SEM sem;                            /* monitor task semaphore */

   /* initialize private semaphore
   */
   SemInit(&sem, 0);
   
   for (;;)
      {
      /* wait for system to become idle
      */
      SemaphoreSignal(&sem);
      SemaphoreWait(&sem);

      /* execute user idle function
      */
      UserIdle();
      }
}

/************************************************************************
* QueueTask
*************************************************************************
*
* Add a task to a queue (singly-linked list) based on the task's priority.
* Higher priority tasks are queued in front of lower priority tasks.
* It is assumed that this routine is called from a critical section.
*
*/
PRIVATE void QueueTask(queue, task)
TCB **queue;                                  /* queue to receive task */
TCB *task;                             /* address of task to be queued */

{
   TCB *temp;

   temp = *queue;
   
   if (temp == NULL)
      {                                          /* add to empty queue */
      task->Next = NULL;
      *queue = task;
      }
   else if (temp->Priority < task->Priority)
      {                                        /* add to head of queue */
      task->Next = temp;
      *queue = task;
      }
   else
      {                                             /* sort into queue */
      while (temp->Next)
         {
         if (temp->Next->Priority < task->Priority)
            break;
         temp = temp->Next;
         }
      task->Next = temp->Next;
      temp->Next = task;
      }

   task->Queue = queue;
}

/************************************************************************
* StartExec
*************************************************************************
*
* Initialize EXEC data structures.
*
*/
PRIVATE void StartExec()

{

   /* Set various system variables
   */
   TSMinPri = 0;
   TSMaxPri = 0;
   CSLevel = 0;                     /* No routines in critical section */
   NPSLevel = 0;                         /* no routines in non-preempt */
   ExecErr = NOERR;                                   /* No errors yet */
   ReadyList = NULL;
   RunningTask = &MainTCB;
   
   /* Initialize main program as the running task
   */
   SemInit(&MainSem, 0);
   MainTCB.Next = NULL;
   MainTCB.Priority = MAX_PRI;
   MainTCB.Queue = NULL;
   MainTCB.Status = ISDN_RUNNING;

   /* Initialize monitor task
   */
   MonTCB.Next = NULL;
   MonTCB.Priority = MON_PRI;
   MonTCB.Queue = NULL;
   MonTCB.Status = BLOCKED;
   Cisco_InitTCB(&MonTCB, (MWORD *) MonStack, MON_STACK_SIZE, Monitor, "Monitor");	/* === */

   /* Initialize timer task
   */
   SemInit(&TmrSem, 0);
   TmrTCB.Next = NULL;
   TmrTCB.Priority = TMR_PRI;
   TmrTCB.Queue = NULL;
   TmrTCB.Status = BLOCKED;
   Cisco_InitTCB(&TmrTCB, (MWORD *) TmrStack, TMR_STACK_SIZE, TimerTask, "TimerTask");	/* === */
}

/************************************************************************
* StopExec
*************************************************************************
*
* Perform a clean shutdown of EXEC
*
*/
PRIVATE void StopExec()

{
#ifdef HDW_IBMPC
   fprintf(stderr, "\nExecErr=%d\n", ExecErr);
#endif
}

/************************************************************************
* _Switch
*************************************************************************
*
* Switch to the task at head of ready list. Save the state of the
* currently running task, and switch to the task at the head of the
* ready list.
*
* !!!NOTA BENE!!!
* During execution of _Switch(), it is implicitly assumed that:
*
*     1. CSLevel == 1
*     2. NPSLevel == 0
*
*/
PRIVATE void _Switch()

{
   MWORD far *old;
   MWORD far *new;

   /* If the task at the head of the ready list is not the same as the
   *  currently running task, then perform a stack switch.  Bad things
   *  can happen if _swstack is called with both parameters pointing
   *  to the same context storage space.
   */
   if (RunningTask != ReadyList)
      {
      /* point to old running task's context storage space
      */
      old = RunningTask->Contxt;

      /* point to new running task's context storage space
      */
      new = ReadyList->Contxt;

      /* unlink new running task from ready list (it is assumed that
      *  the old running task is already on some queue)
      */
      RunningTask = ReadyList;
      ReadyList = ReadyList->Next;
      
      /* update status of new running task
      */
      RunningTask->Status = ISDN_RUNNING;
      RunningTask->Queue = NULL;
      RunningTask->Next = NULL;


      /* call low level task switching routine
      */
      _swstack(old, new, RunningTask->Stack);
      }
}

#ifdef APP_NO_TIMER
/************************************************************************
* TimerCheck
*************************************************************************
*
*  If the application does not support a timer interrupt, then call
*  this this routine on a regular basis to check if SOFT_TICK_LEN
*  milliseconds have passed, then signal the TimerTask, and record
*  the current time.
*
*  NOTE - This scheme is intended ONLY for the purpose of running
*  demonstration software (e.g. Taskview, Tracker, etc) on systems
*  where the timer interrupt is not accessible (e.g. Unix).
*
*/
PRIVATE void TimerCheck()

{
#if F_USE_FTIME                                         /* 4.xBSD only */
   static struct timeb last_tick = { (time_t) 0, 0, 0, 0 };
   struct timeb this_tick;
   unsigned long ms;

   ftime(&this_tick);

   ms = ((this_tick.time - last_tick.time) * 1000L) +
      (this_tick.millitm - last_tick.millitm);

   if (ms >= SOFT_TICK_LEN)
      {
      last_tick.time = this_tick.time;
      last_tick.millitm = this_tick.millitm;
      TickHandler();
      }
#else
   static time_t last_tick = { (time_t) 0 };
   time_t this_tick;
   int i;

   time(&this_tick);

   if (this_tick != last_tick)
      {
      last_tick = this_tick;
      for (i = 0; i < 10; ++i)
         TickHandler();
      }
#endif
}
#endif                                                 /* APP_NO_TIMER */

/************************************************************************
* TimerTask
*************************************************************************
*
* This is the timer task.  It consits of a loop that 1) waits for a
* signal from the timer ISR, 2) processes expired timers, and 3) calls
* user function UserTick.
*
* This task waits for the signal from the timer ISR by performing
* a SemaphoreWait on semaphore TmrSem.  All the timer ISR must do
* is call TickHandler, which performs a SemaphoreSignal on the
* same semaphore.  In addition, TickHandler also increments the
* variable MasterClock.
*
* Active timers are in a list sorted by time.  Older timers are at
* the head of the list.  Therefore, processing expired timers consits
* of removing all expired timers from the head of the list, and
* sending off a corresponding mail message.
*
* The last step performed before blocking on semaphore TmrSem is to
* call the user supplied function UserTick.  This is intended to be
* a hook to allow for any regular processing that may be needed in
* an application.
*
* The data types S_CLOCK and U_CLOCK are the signed and unsigned
* integer types (16 or 32 bit) used for timer values.  Both types
* must have the same size. The master clock and all timers are of
* type U_CLOCK.  The S_CLOCK type is needed when we check a timer
* against the master clock, to see if the timer has expired.
* Assuming that "timer" is of type U_CLOCK, the test for an
* expired timer is:
*
*     expired = ((S_CLOCK) (MasterClock - timer) > 0);
*
* The range of timers is 0 to the max positive value that a S_CLOCK
* type can take on.  As an example, with a 16 bit integer type and
* 50 milliseconds between calls to TickHandler, timers can be set for
* intervals as long as (32767 * 50) / 1000 / 60 = 27.3 minutes.
*
*/
PRIVATE void TimerTask()

{
   extern void UserTick (void);        /* external user function */
   TIMER_BLOCK *timer;                     /* temp timer block pointer */

   for (;;)
      {
      /* wait for tick from timer interrupt
      */
      SemaphoreWait(&TmrSem);

      /* Check for expired timers
      */
      for (;;)
         {
         /* try to get an expired timer from head of timer list
         */
         EnterCS();
         if (ActiveTimers == NULL)
            {
            timer = NULL;                          /* no active timers */
            }
         else if (SLEEPING(ActiveTimers->ExpireTime)) /*===*/
            {
            timer = NULL;                         /* no expired timers */
            }
         else
            {
            timer = ActiveTimers;    /* remove expired timer from list */
            ActiveTimers = ActiveTimers->Next;
            }
         ExitCS();
         
         /* No timers? then quit loop
         */
         if (timer == NULL)
            break;

         /* Timer has expired... mail message.
          *
          * Make sure not trying to send to a MailBox of 0,
          * crashes the router.
          */
         if (timer->MailboxID != 0) {
             SendTMail(timer->MailboxID, DEFAULT_DIR, &timer->ExtMem,
                       timer->Priority, timer->Sn);
         } else {
             if (isdn_debug)
                buginf("\nISDN, attempting to send to MailBox 0");
         }

         /* Return expired timer to available list
         */
         EnterCS();
         timer->Next = AvailTimers;
         timer->Avail = TRUE;
         AvailTimers = timer;
#ifdef MEM_STATS
         --MS_UsedTimers;
#endif
         ExitCS();
         }

      /* Call external tick handler
      */
      UserTick();
      }
}

/*EOF*/
