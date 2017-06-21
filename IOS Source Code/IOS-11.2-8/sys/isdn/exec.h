/* $Id: exec.h,v 3.2.60.2 1996/04/30 23:07:16 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/exec.h,v $
 *------------------------------------------------------------------
 * exec.h  Multitasking Executive Interface File
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec.h,v $
 * Revision 3.2.60.2  1996/04/30  23:07:16  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  20:30:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:51:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:31:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:40:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:44:03  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 21:15:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: exec.h
VERSION: 3.21
DESCRIPTION:  Multitasking Executive Interface File

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

EXEC.H defines the interface between external applications and the
the multitasking executive, EXEC.C.

*************************************************************************
*************************************************************************
*/

/* Define module name if not already defined
*/
#ifndef EXEC_H                            /* prevent nested inclusions */
#define EXEC_H                                          /* module name */



/************************************************************************
*                           C O N S T A N T S
*************************************************************************
*/

/* EXEC Error Codes
*/
#define MAILBOX_FULL          -1              /* mailbox limit reached */
#define NO_MAIL_DESCRIPTORS   -2                 /* out of descriptors */
#define NO_TIMERS             -3                      /* out of timers */
#define UNBALANCED_CS         -4                /* unbal enter/exit CS */
#define UNBALANCED_NPS        -5               /* unbal enter/exit NPS */
#define BAD_WAIT              -6              /* can't block during CS */
#define BAD_SWITCH            -7              /* unable to task switch */
#define NOT_DEAD              -8                     /* task won't die */

/* EXEC special priority levels
*/
#define SUSPEND_PRI           0                        /* suspend task */
#define MON_PRI               1                   /* EXEC monitor task */
#define TMR_PRI               251               /* timer task priority */
#define USER_MIN_PRI          (MON_PRI+1)                /* user tasks */
#define USER_MAX_PRI          (TMR_PRI-1)                /* user tasks */

/* Mail priority
*/
#define MUST_SEND_PRI         251                  /* prioritized mail */
#define MAX_PRI               255                  /* maximum priority */

/* EXEC mail directions
*  NOTE:  If 2 mail queues per mailbox are not needed for flow control
*  purposes, make DOWN=UP.  Then, only one mail queue will be used.
*/
#define UP                 0                      /* upward bound mail */
#define DOWN               1                    /* downward bound mail */
#define DEFAULT_DIR        UP                /* either (UP is default) */
#define SELF               UP                  /* self (UP is default) */

/* A Task's state can assume one of several values
*/
#define ISDN_RUNNING            0                        /* Task is running */
#undef READY	/* === */
#define READY              1                     /* Task in ready list */
#define BLOCKED            2                        /* Task is blocked */
#define INITIAL            3                            /* initialized */
#define RESIGNED           4                          /* task resigned */
#define UNKNOWN_TASK       5                     /* Unknown task state */ /* === */

/* define various machine dependent constants
*/
#ifdef HDW_MC68000
#define F_PUSH_NEG         1
#define SIZE_CONTEXT       1
#endif

#ifdef HDW_R4600
#define F_PUSH_NEG	   1
#define SIZE_CONTEXT	   1
#endif

#ifdef HDW_INTEL
#define F_PUSH_NEG         1
#define SIZE_CONTEXT       2
#endif


/************************************************************************
*              D A T A   T Y P E   D E F I N I T I O N S
*************************************************************************
*/

/* define machine word data type
*/
#ifdef HDW_MC68000
typedef unsigned long MWORD;                                /* 32 bits */
#endif

#ifdef HDW_R4600
typedef unsigned long MWORD;                                /* 32 bits */
#endif

#ifdef HDW_INTEL
typedef unsigned MWORD;                                     /* 16 bits */
#endif

/* Define integer type (16 or 32 bit) for clock/timer type.
*  Both a signed and unsigned integer type must be defined.
*
*  The range of timers is 0 to the max signed clock type.  Note
*  that with a 16 bit integer type, and 50 milliseconds between
*  calls to TickHandler(), this means that timers can be set for
*  as long as (32767 * 50) / 1000 / 60 = 27.3 minutes.
*/
typedef sys_timestamp U_CLOCK;                    /* unsigned timer type */
/* typedef int S_CLOCK;                              signed timer type */

/* timer sequence numbers typedef'd for backwards compatiblity
*/
typedef short int SEQNO;


/* Task control blocks are used in the management of switching and
*  queueing of tasks. The task control block (TCB) contains:
*
*  1. the task's status (READY, RUNNING, etc.)
*  2. a link field for linking the tcb into a wait queue
*  3. a pointer to the variable which contains the address of the
*     TCB at the head of the queue in which this TCB is queued
*     (it may be necessary to modify the variable when inserting and
*     removing items from the queue)
*  4. the task's relative priority
*  5. for the HPC, the task's page
*  6. an array of machine words to store the processor state for
*     context switches.
*/
typedef struct tcb
{
   int         Status;                                /* Task's status */
   struct tcb  *Next;                        /* link for task queueing */
   struct tcb  **Queue;                            /* queue task is in */
   int         Priority;                            /* task's priority */
   MWORD       Contxt[SIZE_CONTEXT];                  /* machine state */
   void        (*Task) (void);        /* task's starting address */
   char        *Name;		/* === Task's name */
   MWORD       *Stack;                      /* pointer to task's stack */
   int         StackSize;                      /* size of task's stack */
}
TCB;
typedef struct tcb *TASK_ID;

/* Semaphores are used to control access to critical resources. Each
*  semaphore has an integer value, and a pointer to the first
*  task (tcb) in the semaphore's wait queue.
*/
typedef struct semaphore
{
   int         Value;                               /* semaphore value */
   TCB         *Head;        /* first task in wait queue for semaphore */
}
SEM;
typedef struct semaphore *SEM_ID;

/* Extended memory addressing structure.  This is defined as a structure
*  rather than simply a pointer because on some systems, the absolute
*  address may be more complex; for instance, it may contain a segment or
*  a page.
*/
typedef struct ext_memory_addr
{
   BYTE        *Addr;                               /* address of item */
}
EXT_MEM;

/* Mail descriptors are used for the management of mail messages. Mail
*  descriptors include:
*
*  1. A link field for linking the descriptor into a mailbox.
*  2. The extended memory address of the actual mail message.
*  3. A serial number for timed mail used for when it is necessary to
*     remove the mail descriptor from a queue.
*  4. A priority field indicating the priority associated with the
*     message.
*/
typedef struct mail_descriptor
{
   struct mail_descriptor *Next;         /* next descriptor in mailbox */
   EXT_MEM                ExtMem;                  /* extended address */
   SEQNO                  Sn;        /* sequence number of timer block */
   int                    Priority;                /* priority of mail */
}
MAIL_DESCRIPTOR;

/* Mail queues are a list of mail that are attached to a mailbox.  The
*  structure has:
*
*  1. A limit field which indicates the maximum # of messages allowed
*     in the queue.
*  2. An avail field which indicates the available number of mail slots
*     in the queue.
*  3. A pointer to the head of the linked list of mail descriptors which
*     constitute the mail queue's contents.
*/
typedef struct mail_queue
{
   int               Limit;                 /* max mail slots in queue */
   int               Avail;           /* mail slots available in queue */
   MAIL_DESCRIPTOR   *Head;                    /* first entry in queue */
}
MAIL_Q;
typedef struct mail_queue *MAIL_Q_ID;

/* Mailbox structure. Each mailbox has two linked list of mail descriptors;
*  one for UP mail and one for DOWN mail.  Mail is sent to either of these
*  lists.  Since a mailbox only has one semaphore, it is not possible to
*  read from a particular list (UP or DOWN).  Each Mailbox has:
*
*  1. An associated semaphore for access control.
*  2. An UP mail queue.
*  3. A DOWN mail queue.
*/
typedef struct mailbox
{
   SEM               Sem;                      /* associated semaphore */
   MAIL_Q            Up;                              /* up mail queue */
   MAIL_Q            Down;                          /* down mail queue */
   MAIL_Q            *LastRead;        /* last queue read (up or down) */
}
MAILBOX;
typedef struct mailbox *MAILBOX_ID;

/* Timer blocks are used by the timer routines to sequence multiple
*  concurrent timer requests. Timer blocks contain:
*
*  1. A pointer to the next timer block in a linked list.
*  2. A pointer to the previous timer block in a linked list.
*  3. The ID of the mailbox to receive a message when the timer expires.
*  4. The HPC extended memory address of the message to be mailed on
*     timer expiry.
*  5. The timer's expiration time (relative to EXEC's master clock).
*  6. The timer's sequence number, used for early timer cancellations.
*  7. The priority of the mail message associated with the timer.
*/
typedef struct timer_block
{
   struct timer_block   *Next;            /* next block in timer chain */
   struct timer_block   *Prev;              /* Previous block in chain */
   MAILBOX              *MailboxID;   /* mailbox to receive expiry msg */
   EXT_MEM              ExtMem;                    /* extended address */
   U_CLOCK              ExpireTime;              /* timeout clock time */
   SEQNO                Sn;          /* sequence number of timer block */
   BYTE                 Avail;       /* is this timer block available? */
   int                  Priority;            /* priority of timed mail */
}
TIMER_BLOCK;
typedef struct timer_block *TIMER_ID;


/************************************************************************
*                              M A C R O S
*************************************************************************
*/




#ifdef HDW_MC68000
#ifdef OS_USG                            /* can't modify SR under UNIX */
#define Enable()
#define Disable()
#else
#define Enable()              _enable()
#define Disable()             _disable()
#endif
#endif

#ifdef HDW_R4600
#define Enable()
#define Disable()
#endif

#ifdef CMP_TURBOC
/* TURBO C specific pragmas and macros
*/
#define Enable()              enable()
#define Disable()             disable()
#endif






/* EXEC functions implemented as macros
*
*  We use macros for certain functions involved in task switching so that
*  debugging statements can be inserted BEFORE the function call in order
*  to identify the source of the error without the need for stack
*  traceback. User applications must use the macro names and should NEVER
*  under any circumstances call the function (_xxx()) directly.
*
*  InitTS is used to set the min and max priorities for time-slicing tasks.
*  SendMail is defined as a special case of SendTMail.
*  TestMail returns TRUE if mail is available.
*  TestMailAvail returns the number of available mail slots for a mail queue
*  TestMailLimit returns the total number of mail slots for a mail queue
*  EnterCS disables interrupts
*  EnterICS is used to enter an isr that uses MTEX services
*  EnterNPS is used to disable preemption
*/
#define InitTS(minp, maxp)    { TSMinPri = (minp); TSMaxPri = (maxp); }
#define SendMail(A,B,C,D)     SendTMail((A),(B),(C),(D),0)
#define TestMail(mb)          (Test(&(mb)->Sem) > 0)
#define TestMailAvail(mb,dir) ((dir==DOWN) ? mb->Down.Avail : mb->Up.Avail)
#define TestMailLimit(mb,dir) ((dir==DOWN) ? mb->Down.Limit : mb->Up.Limit)
#define EnterCS()             { Disable(); ++CSLevel; }
#define EnterICS()            (++CSLevel)
#define EnterNPS()            (++NPSLevel)
#ifdef DEBUG
#define ExitCS()              { DASSERT(CSLevel,UNBALANCED_CS); _ExitCS(); }
#define ExitICS()             { DASSERT(CSLevel,UNBALANCED_CS); _ExitICS(); }
#define ExitNPS()             { DASSERT(NPSLevel,UNBALANCED_NPS); _ExitNPS(); }
#define ExitTCS(arg)          { DASSERT(CSLevel,UNBALANCED_CS); _ExitTCS(arg); }
#else
#define ExitCS()              _ExitCS()
#define ExitICS()             _ExitICS()
#define ExitNPS()             _ExitNPS()
#define ExitTCS(arg)          _ExitTCS(arg)
#endif

/* Debugging utility macros. Do nothing if DEBUG not specified.
*  To correctly pass variable args, DPRINTF args should be passed in
*  parentheses. Ex.: DPRINTF(("The error is %d\n", err)). DASSERT
*  evaluates the specified conditional expression and performs an
*  error exit if it is FALSE.
*/
#ifdef DEBUG
#define DASSERT(expr,code) { \
   if (!(expr)) { \
   printf("ERROR %d in %s at line %d\n",(code),__FILE__,__LINE__); \
   ExecErr = (code); \
   ExecExit(); \
   }}
#else
#define DASSERT(expr,code)
#endif


/************************************************************************
*                     D A T A   A L L O C A T I O N
*************************************************************************
*/

/* ===
extern U_CLOCK MasterClock;                        system master clock */
#ifdef NEVER_REFERENCED
extern SEM MainSem;                          /* main program semaphore */
#endif /* NEVER_REFERENCED */
extern int CSLevel;                          /* Critical section level */
extern int ExecErr;                                 /* last EXEC error */
extern int NPSLevel;                      /* non-preempt section level */
extern int TSMinPri;                   /* min priority for timesharing */
extern int TSMaxPri;                   /* max priority for timesharing */

#ifdef MEM_STATS
extern int MS_UsedMailDesc;         /* # of mail descriptors allocated */
extern int MS_MaxMailDesc;        /* high water mark of MS_MaxMailDesc */
#ifdef NEVER_REFERENCED
extern int MS_UsedTimers;                     /* # of timers allocated */
#endif /* NEVER_REFERENCED */
extern int MS_MaxTimers;            /* high water mark of MS_MaxTimers */
#endif


/************************************************************************
*               F U N C T I O N    D E F I N I T I O N S
*************************************************************************
*/

PUBLIC  void ClearTimer       (TIMER_BLOCK *, SEQNO, MAILBOX *);
PUBLIC  void ExecExit         (void);
PUBLIC  void _ExitCS          (void);
PUBLIC  void _ExitICS         (void);
PUBLIC  void _ExitNPS         (void);
PUBLIC  void _ExitTCS         (BOOLEAN);
PUBLIC  void InitMail         (MAIL_DESCRIPTOR *, int);
PUBLIC  void InitTCB          (TCB *, MWORD *, int, void (*)(void));
PUBLIC  void Cisco_InitTCB    (TCB *, MWORD *, int, void (*)(void), char *);	/* === */
PUBLIC  void InitTimer        (TIMER_BLOCK *, int);
PUBLIC  void MailBoxInit      (MAILBOX *, int);
PUBLIC  void MemSet           (BYTE *, BYTE, unsigned);
PUBLIC  void ReadMail         (MAILBOX *, EXT_MEM *);
PUBLIC  void SemaphoreSignal  (SEM *);
PUBLIC  void SemaphoreWait    (SEM *);
PUBLIC  void SemInit          (SEM *, int);
PUBLIC  int SendTMail         (MAILBOX *, int, EXT_MEM *, int, SEQNO);
PUBLIC  void SetPriority      (TCB *, int);
PUBLIC  int SetTimer          (MAILBOX *, EXT_MEM *, int, TIMER_BLOCK **, SEQNO *, int);
#ifdef DEBUG
PUBLIC  void ShowQueue        (SEM *);
PUBLIC  void ShowReadyList    (void);
PUBLIC  void ShowTimers       (void);
#endif
PUBLIC  int Test              (SEM *);
PUBLIC  void TickHandler      (void);
#ifdef STACK_STATS
PUBLIC  int StackUsed         (TCB *);
#endif

/* Balance the ifdefs at the beginning of this module
*/
#endif                                                 /* ifdef EXEC_H */
