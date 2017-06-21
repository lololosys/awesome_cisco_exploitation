/* $Id: l3.c,v 3.6.4.3 1996/07/18 21:23:35 snyder Exp $
 * $Source: /release/112/cvs/Xsys/isdn/l3.c,v $
 *------------------------------------------------------------------
 * l3.c  ISDN Layer 3 (Network Layer) Protocol Control Task
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: l3.c,v $
 * Revision 3.6.4.3  1996/07/18  21:23:35  snyder
 * CSCdi63402:  get isdn table out of data space and into text space
 * Branch: California_branch
 *              saves 9788 in RAM on run from flash devices
 *
 * Revision 3.6.4.2  1996/05/01  14:51:27  hrobison
 * CSCdi46413:  MIP when used as PRI must be in slot 0-4
 * Branch: California_branch
 * Change PRI initialization: for the 7xxx platform the
 * dsl id is now assigned independently of the MIP card
 * slot/subunit location.  The number of PRI interfaces
 * per router is still limited to 10.  The dsl assignment
 * for the 4xxx platform is based on the slot location
 * and corrected to allow only 1 dsl id per slot.
 *
 * Read bug release notes for details on how the ISDN
 * debug output and ISDN show commands have changed.
 * The debug output now uses the hw_short_namestring
 * rather than PRI<dsl #> or BRI<dsl #>.  The show commands
 * now take the interface name or the dsl id as input.
 *
 * Revision 3.6.4.1  1996/04/30  23:07:39  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.6  1996/03/08  23:44:51  smackie
 * Fix various signed/unsigned comparison conflicts that cause the
 * 95q4 toolchain to blow it's cookies. (CSCdi49069)
 *
 * Revision 3.5  1996/03/06  16:11:00  dclare
 * CSCdi47302:  segV exception in ClearTimer/LIF_StopTimer. Increase the
 * number of blocks and reflect change in show isdn memory.
 *
 * Revision 3.4  1996/02/08  18:40:56  rbeach
 * CSCdi48239:  ISDN PRI dms switchtype doesnt correctly handle SL1
 * mode. Modifications were made to the primary-dms100 channel id
 * format, called party format and support for 1 or 2 octet call
 * reference values.
 *
 * Revision 3.3  1995/11/22  17:39:24  dclare
 * CSCdi44348:  ISDN BRI on 5ESS switches can leak memory. Ignore MAN_INFO
 * messages received as a Broadcast.
 *
 * Revision 3.2  1995/11/17  17:41:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/19  16:52:31  dclare
 * CSCdi40646:  ISDN BRI changes for Italy NET3 homologation.
 *
 * Revision 2.1  1995/06/07  21:16:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       l3.c
DESCRIPTION:  ISDN Layer 3 (Network Layer) Protocol Control Task
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

The OSI Network Layer 3 Controller Task implements the Layer 3
Call Processing Procedures per the Q.931 CCITT Specification.  The
L3 Controller Task communicates with LAPD and Call Control.

This module contains non-SDL layer 3 functions.  Together with the
SDL layer 3 module (l3_sdl_u.c or l3_sdl_n.c), it implements layer 3.


THE NETWORK LINK CONTROL BLOCK (NLCB) DATA STRUCTURE

The NLCB is the basic call state block data structure with which a call
executes the SDLs of Layer 3 Protocol Control.  It is used to preserve
dynamic information about a call across time gaps in call events and to
manage the timers specified in the Layer 3 SDLs.  Much of this code
module concerns itself with the management of this state block
(allocation, initialization, location, timer maintenance, release and
removal).

NLCBs exist either in the memory pool of free NLCBs managed by the
Executive or they are allocated from the pool by this module being attached
to one of possibly several NULL-terminated singly linked lists where
there is one list for each Digital Subscriber Loop (DSL) configured.
Each list is headed by a headcell element (NLCB_Head).  There is a singly-
dimensioned array of these headcells, one per DSL up to MAX_DSL.  On
each NLCB list built from each headcell will be found one or more utility
BROADCAST NLCBs which are used primarily in the early stages of call
setup, when the absolute identity of the call is still ambiguous.  Also
various NLCBs dedicated to serve a particular CES value (Layer 2 data link) may exist.  Multiple data links may be
may exist and multiple dedicated CES values may exist concurrently on one
DSL.  Of the NLCBs dedicated to a particular CES, some on a list will, in
general, be in use for current calls and others will be idle.  One call
occupies one NLCB.  For example, a Layer 2 data link may exist for CES = 1
and CES = 2 on some DSL.  On the NLCB list for that DSL one might expect
to find a BROADCAST NLCB (BROADCAST CES) and perhaps one idle NLCB
dedicated to CES = 1 and another busy NLCB dedicated to CES = 1 and a
another idle NLCB dedicated to CES = 2.

When a new call is to begin for a particular CES, an idle "washed" NLCB is
located by searching down the list for the DSL in question.  If no idle
NLCB with the proper CES is located, a new NLCB is allocated from the
free pool for the new call and it is added to the list.  At hangup, an
NLCB is "washed", but its CES value is preserved and it retains its
linkage to the list.

The presence of at least one NLCB bearing a particular CES value on the
linked list for one DSL is the indication, at Layer 3, that Layer 2
maintains that CES on that DSL in the Layer 2 Multiframe Established state.
When Call Control initiates a call setup for a particular dedicated CES
and Layer 3 finds no NLCB of this identity on the list, it preserves the
setup message and must request and be confirmed of data link establishment
(DL_EST_REQ and DL_EST_CONF) for that CES by Layer 2 before the call setup
protocol may progress.  When Layer 3 receives the DL_REL_IND primitive
from Layer 2 it flushes out and frees all the NLCBs with matching CES
on the DSL in question.


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

*************************************************************************
*************************************************************************
*/

#define     L3_C                                        /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

/* """
#include    <stdio.h>
""" */

#include    "master.h"
#include    "../h/types.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "ccie.h"
#include    "l3.h"
#include    "cc_chan.h"
#include    "gp.h"
#include "../util/random.h"

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

int Used_nlcb;    /* counter for memory usage */

/* Run-time timer default settitngs.
*/
/*
* Timers T303, T305, T308 and T316 are common.
* Timers T313, T318 & T319 are USER side only.
#ifdef VN2
* Timer T310 is also implemented for user side VN3.
#endif
* Timers T302, T304, T306, T309, T310, T312, & T322 are NETWORK side only.
*/
PRIVATE UINT32 T303_TimeOut;            /* Timer T303 timeout interval */
PRIVATE UINT32 T305_TimeOut;            /* Timer T305 timeout interval */
PRIVATE UINT32 T308_TimeOut;            /* Timer T308 timeout interval */
PRIVATE UINT32 T316_TimeOut;            /* Timer T316 timeout interval */
#ifdef USER
#ifdef VN2
PRIVATE UINT32 T310_TimeOut;            /* Timer T310 timeout interval */
#endif
PRIVATE UINT32 T313_TimeOut;            /* Timer T313 timeout interval */
PRIVATE UINT32 T318_TimeOut;            /* Timer T318 timeout interval */
PRIVATE UINT32 T319_TimeOut;            /* Timer T319 timeout interval */
#endif

/* State Validation Tables.
*
* The following tables are used to determine if the NLCB state is valid 
* for the the current SwitchType before and event is processed.
* The comment line of state numbers is aligned with the array of values,
* (1 or 0), used to indicate which are the legal states for a SwitchType.
* These tables are reference through L3_ValidState().
*/

#ifdef BRI_NI1	/*===*/
static const BYTE BRI_NI1_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */

#endif

#ifdef PRI 
static const BYTE PRI_4ESS_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */

static const BYTE PRI_5ESS_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif

static const BYTE PRI_NET5_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */

static const BYTE PRI_NTT_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */

static const BYTE PRI_TS014_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */

#ifdef BRI_5ESS
static const BYTE BRI_5ESS_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif

#ifdef BRI_DMS100
#ifdef USER
static const BYTE BRI_DMS100_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif
#endif

#ifdef PRI_DMS100
static const BYTE PRI_DMS100_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif

#ifdef VN2
static const BYTE VN2_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif

#ifdef BRI_NET3		/*===*/
static const BYTE NET3_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif

#ifdef BRI_TS013
static const BYTE TS013_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif

#ifdef KDD_NTT
static const BYTE KDD_NTT_ValidStates[] =
  {1,1,0,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif

#ifdef BRI_1TR6                                 /* Point-to-multipoint */
static const BYTE BRI_1TR6_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif

#ifdef PRI_1TR6                                      /* Point-to-point */
static const BYTE PRI_1TR6_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif


#ifdef CCITT
static const BYTE CCITT_ValidStates[] =
  {1,1,1,1,1,0,1,1,1,1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1};
/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,98,99 */
#endif

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

/* Non-SDL functions
*/
PRIVATE NLCB_ID  near L3_Alloc_NLCB       (void);
PRIVATE BOOLEAN  near L3_BadPD            (BYTE, int);
#ifdef USER
PRIVATE BOOLEAN  near L3_BadPeerMsg       (int, BYTE, int, BYTE, int);
#endif
PRIVATE PKT_ID   near L3_DeQueue          (L3_Q *);
PRIVATE void     near L3_DiscardQueue     (L3_Q *);
PRIVATE NLCB_ID  near L3_DuplicateCref    (int, int);
PRIVATE void     near L3_GetInfo          (int *, BYTE *, INT16 *, \
                                                 BYTE *, int *, int);
PRIVATE NLCB_ID  near L3_GetNew_NLCB      (BYTE, int, int);
#ifdef USER
PRIVATE NLCB_ID  near L3_GetUser_NLCB     (BYTE, int, int, BYTE, \
                                                int, int, BOOLEAN *);
#endif
#ifdef PRI
PRIVATE void     near L3_GlobalEvent      (int, BYTE);
#endif
PRIVATE BOOLEAN  near L3_InfoLenErr       (int, int);
PRIVATE int      near L3_Init_NLCB        (NLCB_ID, int, BYTE, int);
#ifdef PRI
PRIVATE void     near L3_MaintMsg         (int, BYTE);
#endif
PRIVATE void     near L3_NullCref         (int, BYTE);
PRIVATE void     near L3_ProcessInternal  (void);
PRIVATE void     near L3_Restart          (int, BYTE, int, BYTE, PKT_ID);
#ifdef BRI
PRIVATE BOOLEAN  near L3_RestartAll       (int, int, BYTE *, BYTE *, \
                                                 BOOLEAN *);
PRIVATE void     near L3_RestartChannel   (int, int, BOOLEAN *);
#endif
#ifdef USER
PRIVATE void     near L3_RestartCrefErr   (BYTE, int, BYTE, int);
#endif
#ifdef BRI
PRIVATE BOOLEAN  near L3_RestartGrp       (int, int, BYTE *, BOOLEAN);
#endif
#ifdef 0
PRIVATE void     near L3_RestartIllegal   (int, BYTE, BYTE, int);
#endif
#ifdef PRI
PRIVATE void     near L3_RestartAckReq    (BYTE, PKT_ID);
PRIVATE void     near L3_RestartCalls     (BYTE, PKT_ID);
#endif
PRIVATE void     near L3_RestartReq       (BYTE, int, PKT_ID);

/************************************************************************
*/

/****************
* L3_Alloc_NLCB *
*****************
*
* Allocate an NLCB memory block.
*
* Return:  Address of the NLCB if possible, NULL otherwise.
*
* NOTE:  This function is factored out in order to enable easy change to
*        static allocation of NLCB blocks.
*
*/
PRIVATE NLCB_ID near L3_Alloc_NLCB ()

{
   return(malloc_named(sizeof(struct NetworkLinkControlBlock), "isdn_nlcb"));
}

/***********
* L3_BadPD *
************
*
* Test the message protocol discriminator to determine if it is not
* that expected from the far end for call action processing.
*
* return: TRUE if it is bad.
*
*/
PRIVATE BOOLEAN near L3_BadPD( pd, source )

BYTE pd;                                  /* L3 Protocol Discriminator */
int  source;                   /* Source task for the incoming message */

{
   BOOLEAN bad_pd;

   switch ( SwitchType )
      {
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
         bad_pd = (( pd != TRANSACTION_PD_1TR6 ) && ( source == L2_ID ));
         break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
         bad_pd = (( pd != TRANSACTION_PD_1TR6 ) && ( source == L2_ID ));
         break;
#endif
      default:
         bad_pd = (( pd != Qdot931 ) && ( source == L2_ID ));
         break;
      }
   return ( bad_pd );
}

#ifdef USER
/****************
* L3_BadPeerMsg *
*****************
*
* Default handling for bad messages from the peer when a state
* block (nlcb) could not otherwise be located.
*/
PRIVATE BOOLEAN near L3_BadPeerMsg( cr, cr_len, call_id, ces, event )

int  cr;                                             /* Call Reference */
BYTE cr_len;                                  /* Call Reference Length */
int  call_id;                                               /* Call ID */
BYTE ces;                                /* Connection Endpoint Suffix */
int  event;                                                /* L3 Event */

{
   BOOLEAN ignore_msg;
   NLCB_ID nlcb;

   /* Get an idle nlcb of correct ces to field the event.
   */
   nlcb = NLCB_Head[ L3_DSL_Id ];
   while (( nlcb ) &&
          (( nlcb->Ces != ces ) || ( nlcb->State != NULL_STATE )))
      nlcb = nlcb->Next;

   if ( nlcb == NULL )
      {
      /* All nlcbs for this ces must be in use.  Get a new one.
      */
      nlcb = L3_GetNew_NLCB( ces, cr, call_id );
      }

   if ( nlcb )           /* Save relevant call parameters in the nlcb. */
      {
      nlcb->CallRef = cr;
      nlcb->CallRefLen = cr_len;
#ifdef VN2
#ifdef BRI
      if ( SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
         {
         if ( cr_len > 0 )
            nlcb->AssCref = TRUE;
         else
            nlcb->AssCref = FALSE;
         }
#endif
#endif
      nlcb->Event = event;
      nlcb->State = NULL_STATE;
      NLCB = nlcb;
      L3_UpdateTimers( NLCB, cr, NLCB->Ces );
      /* In general, Ux_UnknownCallRef() will completely handle
      *  the event.  In a few cases, follow-on events will be
      *  expected, in which case, follow-on processing will be
      *  required through normal SDL dispatch.
      */
      ignore_msg = Ux_UnknownCallRef();
      }
   else
      {
      ignore_msg = TRUE;
      }

   return ( ignore_msg );
}

#endif                                                         /* USER */

/****************
* L3_ShowTimers *
*****************
*
* display the values of the L3 timers
*/
PUBLIC void show_isdn_L3timers ()
{
    printf("\n\tISDN Layer 3 values:");
    printf("\n\t T303 = %d   seconds", T303_TimeOut / 1000);
    printf("\n\t T305 = %d   seconds", T305_TimeOut / 1000);
    printf("\n\t T308 = %d   seconds", T308_TimeOut / 1000);
    printf("\n\t T310 = %d  seconds", T310_TimeOut / 1000);
    printf("\n\t T313 = %d   seconds", T313_TimeOut / 1000);
    printf("\n\t T316 = %d seconds", T316_TimeOut / 1000);
    printf("\n\t T318 = %d   seconds", T318_TimeOut / 1000);
    printf("\n\t T319 = %d   seconds", T319_TimeOut / 1000);
}

/**************
 * show_nlcbs *
 **************
 *
 * show the values of the active NLCBs 
 */
PUBLIC void show_nlcbs (int dsl)
{
    NLCB_ID nlcb;
    int number;  /* number of active calls */
    boolean twait_running;

    number = 0;
    twait_running = FALSE;
    /*
     * Look thru all the nlcbs and see if there are any that are
     * in use (meaning Call Active), increment count for each active call.
     */
    nlcb = NLCB_Head[dsl];
    while (nlcb) {
        if (nlcb->State == CALL_ACTIVE)
            number++;
        /*
         * Check to see if a twait timer is running. This means that
         * Layer 2 will not attempt to activate until this timer expires.
         */
        if (LIF_TimerRunning(nlcb->Timer_twait))
            twait_running = TRUE;
        nlcb = nlcb->Next;
    }
    printf("\n    Layer 3 Status:");
    if (twait_running)
        printf("\n\tTWAIT timer active");
    printf("\n\t%d Active Layer 3 Call(s)", number);
}

/*******************
* L3_CallRefSelect *
********************
*
* Select a new call reference.  Store value in current NLCB.
*
*/
PUBLIC BOOLEAN L3_CallRefSelect (update_timers)

BOOLEAN update_timers;          /* TRUE unless NETWORK BROADCAST setup */

{
   int     saved_cref;
   BOOLEAN done = FALSE;
   BOOLEAN wrapped = FALSE;

   saved_cref = L3_CallRef;
   while ( !done )
      {
      if ( L3_Search_NLCB_Ref( L3_CallRef, NLCB->CallRefLen ) == NULL )
         {
         NLCB->CallRef = L3_CallRef;
         done = TRUE;
         }

      /* Increment the call reference value or wrap it around.
      */
      if ( NLCB->CallRefLen == 1 )
         L3_CallRef = ( (L3_CallRef < MAX_SHORT_CALL_REF) ? L3_CallRef + 1 : 1 );
      else                                       /* 2 byte call reference */
         L3_CallRef = ( (L3_CallRef < MAX_LONG_CALL_REF) ? L3_CallRef + 1 : 1 );

      if (( !done ) && ( saved_cref == L3_CallRef ))
         {
         LIF_Error ("l3", "L3_CallRefSelect", "Call Reference wrapped");
         wrapped = TRUE;       /* This may be grounds to kill the call */
         done = TRUE;
         }
      }

   /* Update the timers associated with this NLCB to agree with the
   *  parameters for this call.
   */
   if (update_timers)
      L3_UpdateTimers(NLCB, NLCB->CallRef, NLCB->Ces);

   return ( wrapped );
}

/*****************
* L3_CallRelease *
******************
*
*  Release a Call:
*     - Clear NLCB variables; goto NULL state
*     - Stop timers
*     - Release saved packets
*
*/
PUBLIC void L3_CallRelease ()

{
   /* Clear the NLCB variables.  CES is NOT modified.
   */
#ifdef USER
   NLCB->State = (NLCB->Ces == BROADCAST_CES) ? BROADCAST_STATE : NULL_STATE;
#else
   NLCB->State = NULL_STATE;
#endif
#ifdef BRI
#ifdef VN2
   NLCB->AssCref    = FALSE;                          /* Re-initialize */
#endif
#endif
   NLCB->CallRefLen = 0;
   NLCB->CallRef    = 0;
   NLCB->Event      = 0;
   NLCB->CallId     = 0;
#ifdef BRI
   NLCB->ChanId     = 0;
#endif
   NLCB->Cause      = 0;

   /* Stop Timers
   */
   L3_StopAllTimers();

   /* Discard packet queues
   */
   L3_DiscardQueue (&NLCB->MsgQ);
   L3_DiscardQueue (&NLCB->InfoQ);
   NLCB->Internal = FALSE;

   /* Discard any saved packets
   */
   if (NLCB->Setup)
      {
      LIF_FreePkt (NLCB->Setup);
      NLCB->Setup = NULL;
      }
#ifdef USER
   if (NLCB->Connect)
      {
      LIF_FreePkt (NLCB->Connect);
      NLCB->Connect = NULL;
      }
#endif

   if (NLCB->Release)
      {
      LIF_FreePkt (NLCB->Release);
      NLCB->Release = NULL;
      }
   return;
}

/*************
* L3_DeQueue *
**************
*
* DeQueue packet from queue to incoming packet.
*
*/
PRIVATE PKT_ID near L3_DeQueue (q)

L3_Q *q;                                /* queue from which to dequeue */
{
   PKT_ID return_pkt;
   int    i;

   /* Remove the first packet from the head of the queue.  Then scoot all
   *  the remaining packets up one in the queue, setting the last element
   *  to NULL.
   *
   *  Finally just decrement the number of packets in the queue, and
   *  return the one pulled off of the front.
   */
   return_pkt = q->Pkt[ 0 ];
   for ( i = 0; i < MAX_L3_QUEUE_LEN - 1; i++ )
      {
      q->Pkt[ i ] = q->Pkt[ i + 1 ];
      }
   q->Pkt[ MAX_L3_QUEUE_LEN - 1 ] = NULL;
   q->Count--;

   return ( return_pkt );
}

/******************
* L3_DiscardQueue *
*******************
*
* Discard packet queue.
*
*/
PRIVATE void near L3_DiscardQueue (q)

L3_Q *q;                                           /* queue to discard */

{
   while ( q->Count )
      LIF_FreePkt( L3_DeQueue ( q ) );
   NLCB->Internal = FALSE;
}


/*******************
* L3_DuplicateCref *
********************
*
* Test if a call reference value is already in use.
* In the case of call SETUP, a call being processed at Call Control
* may receive a second SETUP attempt from the peer if the Call Control
* is slow in responding.  In this case the second attempt is detected.
* Return nlcb pointer to the first nlcb on the headcell matching this
* cref if the call reference is a duplicate.
*
*/
PRIVATE NLCB_ID near L3_DuplicateCref( cr, cr_len )

int cr;                               /* Call reference value to test. */
int cr_len;                            /* Call reference value length. */

{
   NLCB_ID nlcb = NULL;                                /* NLCB pointer */


   switch ( SwitchType )
      {
#ifdef BRI
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:	  
         nlcb = NLCB_Head[ L3_DSL_Id ];       /* Point to head of list */
         while ( nlcb )
            {
            /* Allow search for global cref
            */
            if (( nlcb->CallRef == cr ) && ( nlcb->AssCref == TRUE ))
               break;
            nlcb = nlcb->Next;
            }
         break;
#endif
#endif
      default:
         if ( !(GLOBAL_CALL_REF( cr, cr_len )))
            {
            nlcb = L3_Search_NLCB_Ref( cr, cr_len );
            }
         break;
      }

   return ( nlcb );
}


/*************
* L3_EnQueue *
**************
*
* Queue incoming packet on a queue.
*
*/
PUBLIC void L3_EnQueue (q)

L3_Q *q;                                    /* queue on which to queue */
{
   if (q->Count == MAX_L3_QUEUE_LEN)
      {
      if(detail_debug)
         buginf("ISDN LIF Error, L3_EnQueue - Packet Queue Full");
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("l3", "L3_EnQueue", "Packet Queue Full.")
#endif
      ;
      }
   else
      {
      q->Pkt[q->Count] = L3_PktIn;
      q->Count++;
      NLCB->Internal = TRUE;               /* an internal event exists */
      L3_AvailPktIn = FALSE;      /* this packet not available for use */
      }
}

/***************
* L3_Free_NLCB *
****************
*
* Remove network link control block from list of NLCBs and deallocate
* its storage.
*
*/
PUBLIC void L3_Free_NLCB (nlcb)

NLCB_ID nlcb;                                          /* NLCB to free */
{
   NLCB_ID temp;

   /*
    * Make sure that this is not being called with a NULL, if it is,
    * just return.
    */
   if (nlcb == NULL) {
       return;
   }

   NLCB = nlcb;                              /* DiscardQueue uses NLCB */

   /* Search and Remove NLCB from Linked List
   */
   if (nlcb == NLCB_Head[L3_DSL_Id])             /* first link in list */
      {
      temp = NULL;
      NLCB_Head[L3_DSL_Id] = nlcb->Next;
      }
   else                           /* search for link; logically delete */
      {
      temp = NLCB_Head[L3_DSL_Id];
      while (temp->Next != nlcb)
         {
         temp = temp->Next;
         if (temp == NULL)                               /* not found */
            goto bailout;
         }
      temp->Next = nlcb->Next;
      }
   if (nlcb == NLCB_Tail[L3_DSL_Id])              /* last link in list */
      NLCB_Tail[L3_DSL_Id] = temp;

   /* Stop / Remove Timers
   */
#ifdef USER
   LIF_StopTimer (nlcb->Timer_T303);
   LIF_StopTimer (nlcb->Timer_T305);
   LIF_StopTimer (nlcb->Timer_T308);
#ifdef VN2
   LIF_StopTimer (nlcb->Timer_T310);
#endif
   LIF_StopTimer (nlcb->Timer_T313);
   LIF_StopTimer (nlcb->Timer_T318);
   LIF_StopTimer (nlcb->Timer_T319);
   LIF_RemoveTimer (nlcb->Timer_T303);
   LIF_RemoveTimer (nlcb->Timer_T305);
   LIF_RemoveTimer (nlcb->Timer_T308);
#ifdef VN2
   LIF_RemoveTimer (nlcb->Timer_T310);
#endif
   LIF_RemoveTimer (nlcb->Timer_T313);
   LIF_RemoveTimer (nlcb->Timer_T318);
   LIF_RemoveTimer (nlcb->Timer_T319);
   LIF_RemoveTimer (nlcb->Timer_twait);
#endif

   nlcb->Cause  = 0;
#ifdef BRI
   nlcb->ChanId = 0;
#endif

   /* Discard packet queues
   */
   L3_DiscardQueue (&nlcb->MsgQ);
   L3_DiscardQueue (&nlcb->InfoQ);

   /* Discard any saved packets
   */
   if (NLCB->Setup)
      {
      LIF_FreePkt (NLCB->Setup);
      NLCB->Setup = NULL;
      }
#ifdef USER
   if (NLCB->Connect)
      {
      LIF_FreePkt (NLCB->Connect);
      NLCB->Connect = NULL;
      }
#endif
   if (NLCB->Release)
      {
      LIF_FreePkt (NLCB->Release);
      NLCB->Release = NULL;
      }

   /*
    * Release Network Link Control Block
    */
   Used_nlcb--;
   free(nlcb);

bailout:

   return;
}

/**************
* L3_GetCause *
***************
*
* Recover the cause value from the Cause IE in the current pkt or
* return Cause = 0 if the IE cannot be found.
*
*/
PUBLIC int L3_GetCause( pkt )

PKT_ID pkt;

{
   int  cause = 0;
   BYTE *current_ie;
   BYTE *unpacked_ie;


   current_ie = CCIE_Get_IE( pkt->Info, pkt->InfoLen,
                             CAUSE_IE, CODESET_0, 1 );
   if ( current_ie )
      {
      unpacked_ie = CCIE_ExpandCause_IE( current_ie, &Trashcan );
      cause = unpacked_ie[ 2 ];
      Diagnostic[ 0 ] = ( current_ie[ 1 ] > 2 ) ? unpacked_ie[ 3 ] : 0;
      Diagnostic[ 1 ] = ( current_ie[ 1 ] > 3 ) ? unpacked_ie[ 4 ] : 0;
      Diagnostic[ 2 ] = ( current_ie[ 1 ] > 4 ) ? unpacked_ie[ 5 ] : 0;
      }

   return( cause );
}


/*************
* L3_GetInfo *
**************
*
* Get the goodies out of the layer 3 message (call reference length,
* call reference and the message type).
*
* This function also performs the call reference flag bit inversion
* operation on the incoming message.
*
* This function also performs special decoding of PRI Maintenance Service
* messages.
*
* Return: Message type as the event and NOERR, if successful; otherwise
*         (unrecognized Protocol Discrimator or Call Reference), ERR.
*
*/
PRIVATE void near L3_GetInfo( event, info, info_len, cr_len, cr, source)

int   *event;                     /* the event which has just occurred */
BYTE  *info;                                          /* incoming info */
INT16 *info_len;                               /* incoming info length */
BYTE  *cr_len;                                            /* cr length */
int   *cr;                                           /* call reference */
int    source;                                      /* event source ID */
{
   int msg_ndx;                               /* index to message type */
#ifdef PRI
   BYTE *new_info;          /* Info buff pointer for info manipulation */
   BYTE *old_info;          /* Info buff pointer for info manipulation */
   int  i;                                                  /* Counter */
#endif

   if (detail_debug)
       buginf("\nEntering L3_GetInfo"); 

   if ( L3_InfoLenErr( *info_len, info[ 1 ] )) /* Chk for min info len */
      {
      *event = COMPLETED;  /* The (too short) message is just ignored. */
      goto bailout;
      }

#ifdef PRI
   /* Check for a Maintnance Protocol Discriminator (=0x03)
   */
   if (info[PROTO_DISC_NDX] == MaintProto)
      {
      if (( SwitchType == PRI_4ESS_STYPE ) ||
          ( SwitchType == PRI_5ESS_STYPE ) ||
          ( SwitchType == PRI_DMS100_STYPE ))
         {
         if ( source == CC_ID )
            {
            /* This event will be passed directly to L2.
            */
            *event = NL_MAINT_REQ;
            }
         else                                          /* From Layer 2 */
            {
            /* This event will be passed directly to Call Control.
            */
            *event = NL_MAINT_IND;
#ifndef CREF_TEST
            if ( info[ CALL_REF0_NDX ] > 0 )    /* Test for cr_len > 0 */
               /* Flip call ref flag bit.
               */
               info[CALL_REF1_NDX] = info[CALL_REF1_NDX] ^ 0x80;
#endif
            }
         }
      else                                    /* All other SwitchTypes */
         {
         *event = COMPLETED;    /* Msg with undefined P.D. is ignored. */
         }
      }
#endif                                                    /* endif PRI */
   else                                             /* All other cases */
      {
      /* Check for error in Layer 3 Protocol Discriminator
      */
      if ( L3_BadPD( info[ PROTO_DISC_NDX ], source ))
         {
		 if(SwitchType != BRI_1TR6_STYPE) { /* === */
         	*event = COMPLETED;                 /* No further decoding */
			goto bailout;
		 }
		 else {
			*event = BAD_PD;  /* 1tr6, but don't do any more for now */
			goto bailout;     /* if N0 gets implemented, may have work to do */
		 }
      }

      *cr_len = info[CALL_REF0_NDX];

#ifndef CREF_TEST
      /* This is the normal path.  The call reference flag bit is inverted.
      */
      if (( source == L2_ID ) && ( *cr_len > 0 ))
         /* Flip call ref flag bit.
         */
         info[CALL_REF1_NDX] = info[CALL_REF1_NDX] ^ 0x80;
#endif

      switch ( *cr_len )
         {
         case 0:                        /* Null (dummy) call reference */
            *cr = 0;
#ifdef BRI_DMS100
            if ((( SwitchType == BRI_DMS100_STYPE ) || 
                        ( SwitchType == BRI_NI1_STYPE )) && /*===*/
               /* ( info[2] != INFO ))	=== */
               ( info[2] != INFO ) && ( info[2] != CC_INFO_REQ ))
               {
               /* Per NIS S208-4 Issue 1 Sec. 5.1.5.3.2 (f). Only INFO
               *  messages are permitted in this context.
               */
               *event = COMPLETED;                   /* Ignore message */
               goto bailout;
               }
#endif
            *event = NULL_CREF;
            goto bailout;

         case 1:                                            /* 1 octet */
            *cr = (int)info[ CALL_REF1_NDX ];
            if ( *info_len < 4 )     /* Msg too short to have msg_type */
               {
               *event = COMPLETED;              /* No further decoding */
               goto bailout;           /* The message is just ignored. */
               }
#ifdef PRI
            if (( source == L2_ID ) &&
                (( SwitchType == PRI_4ESS_STYPE ) ||
                 ( SwitchType == PRI_5ESS_STYPE ) ||
                 ( SwitchType == PRI_DMS100_STYPE ) ||
                 ( SwitchType == PRI_NET5_STYPE ))) {
               /* Here we will force the message to a two octet call ref
               *  if it is delivered with one octet.  AT&T PRI test plan
               *  deliberately changes cref length in mid-call just in
               *  case things were going too smoothly for you.  The
               *  message will be processed with two octet cref internally.
               */
               new_info = info + *info_len;          /* Extend message */
               old_info = info + *info_len - 1;
               for ( i = 0; i < ( *info_len - 3 ); i++ )
                  *new_info-- = *old_info--;
               *cr_len = 2;
               info[ CALL_REF0_NDX ] = *cr_len;    /* Fix message cr_len */
               if ( *cr & 0x80 )                       /* Fix flag bit */
                  {
                  *cr = ( *cr & 0x7F ) | 0x8000;
                  info[ CALL_REF1_NDX ] = 0x80;
                  }
               else
                  {
                  *cr = ( *cr & 0x7F );
                  info[ CALL_REF1_NDX ] = 0x00;
                  }
               info[ CALL_REF1_NDX + 1 ] = ( *cr & 0x7F );
               ( *info_len )++;
               }
#endif
            break;

         case 2:                                           /* 2 octets */
            *cr = ( info[ CALL_REF1_NDX ] << 8 ) + info[ CALL_REF1_NDX + 1 ];
            if ( *info_len < 5 )     /* Msg too short to have msg_type */
               {
               *event = COMPLETED;              /* No further decoding */
               goto bailout;           /* The message is just ignored. */
               }
            break;

         default:                       /* Unsupported (long) call ref */
#ifdef DISPLAY_LOCAL_ERR
            LIF_Error( "l3", "L3_GetInfo", "Bad Message Encoding." );
#endif
            *event = COMPLETED;                 /* No further decoding */
            goto bailout;
         }

      msg_ndx = CALL_REF1_NDX + info[ CALL_REF0_NDX ];

#ifdef BRI_5ESS
      /* NOTE: in order to process 5ESS supplementary voice services one
      *  must enable the flag ATT_TYPE_A also.  Otherwise only MEMIM
      *  messages will be processed from the pool of alternate coding
      *  messages.
      */
      if (( SwitchType == BRI_5ESS_STYPE ) && ( info[ msg_ndx ] == 0 ))
         {
         /* Escape to alternate coding scheme.
         *
         *  Zero indicates an escape to national-specific or network-specific
         *  message coding schemes, so next byte carries the message.  Check
         *  that the high order bit is set, which indicates a network-specific
         *  coding scheme.
         */
         ++msg_ndx;
         if (!( info[ msg_ndx ] & 0x80 ))
            goto bailout;
         *event = SS_MASK | info[ msg_ndx ];
         if ( *event == MAN_INFO )
            {
            *cr = 0;                      /* cr is always zero for MIM */
            info[CALL_REF1_NDX] = (BYTE) 0;       /* GSD - 2/27/90 FFS */
            if ( source == ME_ID )
               *event = MAN_REQ;
            }
         }
      else
#endif
/* === added the following code */
#ifdef BRI_DMS100
      /* For  DMS-100 the first byte of the message type for supplementary
      *  service is zero.
      */
      if (( SwitchType == BRI_DMS100_STYPE ) && ( info[ msg_ndx ] == 0 ))
         {
         ++msg_ndx;
         if (!( info[ msg_ndx ] & 0x80 ))
            goto bailout;
         *event = (int)info[ msg_ndx ];
         }
      else
#endif                                                   /* BRI_DMS100 */
#ifdef BRI_NI1
      /* For NI-1 the first byte of the message type for supplementary
      *  service is zero.
      */
      if ( ( SwitchType == BRI_NI1_STYPE ) && ( info[ msg_ndx ] == 0 ) )
         {
         ++msg_ndx;
         if (!( info[ msg_ndx ] & 0x80 ))
            goto bailout;
         *event = (int)info[ msg_ndx ];
         }
      else
#endif                                                      /* BRI_NI1 */
/* === end of added code */

		if ( *event == DL_UI_DATA_IND ) {
      		if ( info[ msg_ndx ] == SETUP )
				/* This is a special case of the DL_UI_DATA_IND which
				 *  will be handled by Layer 3 in its normal engine.
				 *  Otherwise the DL_UI_DATA_IND bypasses the normal state
				 *  machine on its way to Call Control.
				 */
				*event = SETUP;
		} else {
			*event = (int) info[msg_ndx];
		}
      }

   /* A few special cases
   */
#ifdef BRI_DMS100
   if ((( SwitchType == BRI_DMS100_STYPE ) || 
            ( SwitchType == BRI_NI1_STYPE )) &&         /*===*/
       ( GLOBAL_CALL_REF( *cr, *cr_len )) &&
       ( source == L2_ID ))
      {
      if (( *event == RESTART ) || ( *event == RESTART_ACK ))
         {
         /* Restore flag bit inversion.
         */
         if ( *cr_len )
            {
            info[ CALL_REF1_NDX ] ^= 0x80;
            if ( *cr_len == 1 )
               *cr ^= 0x80;
            else if ( *cr_len == 2 )
              *cr ^= 0x8000;
            }
         }
      else
         {
         /* Per NIS S208-5 Issue 1, Sec. 6.1.5.3.2 (e).
         */
         *event = COMPLETED;                         /* Ignore message */
         }
      }
#endif
#ifdef PRI_DMS100
   if (( SwitchType == PRI_DMS100_STYPE ) &&
       ( GLOBAL_CALL_REF( *cr, *cr_len )) &&
       ( source == L2_ID )                &&
       ( *event != RESTART )              &&
       ( *event != RESTART_ACK )          &&
       ( *event != NL_MAINT_IND)) /* added to make SERVICE work for DMS */
      /* Per NIS A211-1, Ver. 2, Sec. B, 5.1.5.3.
      */
      *event = COMPLETED;                            /* Ignore message */

#endif

   /* Ignore messages with global call reference
    * for Austalia BRI.
    */
   if ((SwitchType == BRI_TS013_STYPE) &&
       ( GLOBAL_CALL_REF( *cr, *cr_len )) && ( source == L2_ID ))
      *event = COMPLETED;                            /* Ignore message */
   /*
    * for Austalia PRI accept RESTARTS.
    */
   if ((SwitchType == PRI_TS014_STYPE) &&
       (GLOBAL_CALL_REF(*cr, *cr_len)) &&
       (source == L2_ID)               &&
       (*event != RESTART) && (*event != RESTART_ACK))
      *event = COMPLETED;                            /* Ignore message */

   if (( SwitchType == PRI_4ESS_STYPE ) &&
       ( GLOBAL_CALL_REF( *cr, *cr_len )) &&
       ( source == L2_ID )                &&
       ( *event == STATUS)) {
      *event = COMPLETED;                            /* Ignore message */
   }
#ifdef PRI
   if ( !L3_Call_Id )
      {
      /* These are error reports for globall processes (RESTART and SERVICE)
      *  messages operating within call control for the global call
      *  reference.
      */
      if ( *event == CC_STATUS_REQ )
         *event = GLOBAL_STATUS;
      else if ( *event == CC_REL_COMP_REQ )
         *event = GLOBAL_REL_COMP;
      }
#endif
bailout:
   return;
}


/*****************
* L3_GetNew_NLCB *
******************
*
* Request allocation and initialization of a new NLCB.  Report
* failures in the process.
*
*/
PRIVATE NLCB_ID near L3_GetNew_NLCB (ces, cr, call_id)
BYTE ces;                                /* Connection endpoint suffix */
int  cr;                                             /* Call Reference */
int  call_id;                                        /* Call ID number */
{
    NLCB_ID nlcb;   /* Network Link Control Block */

    if ((nlcb = L3_Alloc_NLCB()) == NULL) {    /* Get a new NLCB buffer */
        /*
         * Out of memory blocks
         */
        if (detail_debug)
            buginf("\nL3_GetNew_NLCB, out of NLCBs");

        LIF_Error("l3", "L3_GetNew_NLCB", "L3_Alloc_NLCB failed.");
    } else {
        /*
         * Initialize new NLCB
         */
        Used_nlcb++;
        if ((L3_Init_NLCB(nlcb, cr, ces, call_id)) == ERR) {
            /*
             * Out of memory blocks to prepare NLCB
             */
            LIF_Error("l3", "L3_GetNew_NLCB", "L3_Init_NLCB failed.");
            L3_Free_NLCB(nlcb);
            nlcb = NULL;
        } else {
            /*
             * Add new NLCB to the list of NLCB's
             */
            if (NLCB_Head[L3_DSL_Id])
                NLCB_Tail[L3_DSL_Id]->Next = nlcb;
            else
                NLCB_Head[L3_DSL_Id] = nlcb;
            NLCB_Tail[L3_DSL_Id] = nlcb;
        }
    }
    return (nlcb);
}

#ifdef USER
/******************
* L3_GetUser_NLCB *
*******************
*
* Search for Network Link Control Block.  If not found, allocate a new one.
* Add to end of list of NLCBs.
*
* See THE NETWORK LINK CONTROL BLOCK (NLCB) DATA STRUCTURE at the top of
* this module for a general discussion of the NLCB.
*
*     NOTE: An NLCB once allocated for a particular CES will not be
*           deallocated on call disconnect but rather will be re-used
*           when a new call is established for the same CES.
*
* Return:   Address of NLCB if successful; NULL, otherwise.
*
* Note that the USER side, like the NETWORK side is capable of tracking
* calls on multiple Subscriber Interfaces (DSL's).  The current DSL is
* stored in the L3_DSL_Id and the associated NLCB will be found linked
* onto NLCB_Head[ L3_DSL_Id ].
*
*/
PRIVATE NLCB_ID near L3_GetUser_NLCB (ces, call_id, cr, cr_len, event, source,
   ignore_msg)

BYTE     ces;                            /* Connection endpoint suffix */
int      call_id;                                    /* Call ID number */
int      cr;                                         /* Call reference */
BYTE     cr_len;                      /* Byte length of call reference */
int      event;                          /* The event that has occured */
int      source;                              /* Source of the message */
BOOLEAN *ignore_msg;                  /* Ignore or process the message */

{
   NLCB_ID nlcb = NULL;                  /* Network Link Control Block */
   NLCB_ID tmp_nlcb;                 /* Temporary nlcb address storage */
   NLCB_ID saveNLCB;		/* === */
   BOOLEAN in_loop;                               /* Loop control flag */

   *ignore_msg = FALSE;         /* Assume that we have a valid message */

   if ( source == L2_ID ) /* Look for an NLCB for message from Layer 2 */
      {
      /* An NLCB to be used with a message from L2 will be found
      *  based on the call reference of this particular message
      *  after the connection has been established.  However,
      *  before and during the establishment the following
      *  exceptions will be considred:
      *      -  DL_EST_CONF
      *      -  DL_EST_IND
      *      -  DL_DATA_IND with SETUP message
      *  Also the following messages will be treated as exceptions
      *  due to the fact that they will require special treatment:
      *      -  DL_REL_CONF
      *      -  DL_REL_IND
      *      -  STATUS_ENQ
      */

      /*
       *  If a call reference is longer than 1, we must ignore it in VN2
       *  === added the NET3 test
       */
      if (((SwitchType == VN2_STYPE) || (SwitchType == VN3_STYPE) ||
           (SwitchType == BRI_NET3_STYPE) || (SwitchType == BRI_TS013_STYPE))
            && cr_len > 1) {
                *ignore_msg = TRUE;
                return NULL;
       }

      switch ( event )
         {
         case SETUP:
            /* L2 setup message received from L2 - this message
            *  does not have a call ID. It must be sent up to call
            *  control and call control will take care of setting
            *  up the link.  We do not need an NLCB for this link
            *  yet.  However, we need an NLCB in order to send a
            *  message up to call control.  We will use Broadcast
            *  NLCB here replacing the BROADCAST_CES with the proper
            *  CES for this message.  The BROADCAST_CES will be
            *  restored after the message to CC is mailed (see
            *  function U0_Setup() ).  First test for a second
            *  SETUP using the same cref.
            */
            if (( SwitchType == BRI_5ESS_STYPE ) && ( cr_len != 1 ))
               /* AT&T 5E4.2 Compat. Test Plan (Illeg. Msgs, Test 2.4) */
               *ignore_msg = TRUE;
            else
            if (( nlcb = L3_DuplicateCref( cr, cr_len )) != NULL )
               {
               switch ( SwitchType )
                  {
                  case BRI_DMS100_STYPE:
                  case BRI_NI1_STYPE:               /*===*/
                     /* Per NIS S208-5 Issue 1 Sec. 6.1.5.3.2 (d).
                     */
                     *ignore_msg = TRUE;
                     break;
#ifdef VN2
                  case VN2_STYPE:
                  case VN3_STYPE:
                     *ignore_msg = TRUE;   /* Secondary SETUPs ignored */

                     /*
                      * This is causing problems.  Let's see if we can
                      * avoid having to figure out what's going on by
                      * assuming that any call in state 0 can be re-used.
                      * This makes sense, and should fix the failures.
                      */
                      *ignore_msg = nlcb->State != NULL_STATE;
                      if (*ignore_msg && isdn_debug) {
                         printf("\nBRI: Ignoring incoming call because we have a call in state %d", nlcb->State);
                         flush();
                      }
                      if (*ignore_msg && nlcb->State == 11) {
                          /*
                           * We need to release this puppy for test
                           * VAL/U11/SETUP.
                           */
                          saveNLCB = NLCB;
                          NLCB = nlcb;
                          NLCB->Cause = NORMAL_CLEARING;
                          /*
                           * Don't send a diagnostic--this is not a STATUS msg
                           */
                          Diagnostic[ 0 ] = 0;
                          L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
                          NLCB = saveNLCB;
                      }
                      break;
#endif
                  case PRI_4ESS_STYPE:
                  case PRI_5ESS_STYPE:
                  case PRI_NET5_STYPE:
                  case PRI_NTT_STYPE:
                     *ignore_msg = TRUE;   /* Secondary SETUPs ignored */
                     break;
#ifdef KDD_NTT
                  case KDD_STYPE:
                  case NTT_STYPE:
                     *ignore_msg = TRUE;
                     break;
#endif
#ifdef BRI_NET3		/*===*/
                  case BRI_NET3_STYPE:
                     *ignore_msg = TRUE;
                     break;
#endif
#ifdef BRI_TS013
                  case BRI_TS013_STYPE:
                  case PRI_TS014_STYPE:
                     *ignore_msg = TRUE;
                     break;
#endif
                  default:
                     /* Ignore second SETUP for the same call in State 6.
                     */
                     *ignore_msg = ( nlcb->State == CALL_PRESENT );
                     break;
                  }
               }
            else if ( GLOBAL_CALL_REF( cr, cr_len ))
               {
               switch ( SwitchType )
                  {
#ifdef BRI
#ifdef VN2
                  case VN2_STYPE:
                  case VN3_STYPE:
                     nlcb = NLCB_Head[ L3_DSL_Id ];
                     while (( nlcb ) && ( nlcb->Ces != BROADCAST_CES ))
                        nlcb = nlcb->Next;
                     if ( nlcb != NULL )
                        {
                        nlcb->Ces = ces;
                        nlcb->State = NULL_STATE;
                        }
                     break;         /* Setup permitted for global cref */
#endif
#endif
#ifdef PRI_4ESS
                  case PRI_4ESS_STYPE:
                  case PRI_NET5_STYPE:
                     *ignore_msg = L3_BadPeerMsg( cr, cr_len, call_id,
                                                  ces, event );
                     break;
#endif
#ifdef PRI_5ESS
                  case PRI_5ESS_STYPE:
                     *ignore_msg = L3_BadPeerMsg( cr, cr_len, call_id,
                                                  ces, event );
                     break;
#endif
#ifdef BRI_1TR6
                  case BRI_1TR6_STYPE:	/* === */
                     nlcb = NLCB_Head[ L3_DSL_Id ];
                     while (( nlcb ) && ( nlcb->Ces != BROADCAST_CES ))
                        nlcb = nlcb->Next;
                     if ( nlcb != NULL )
                        {
                        nlcb->Ces = ces;
                        nlcb->State = NULL_STATE;
                        }
                     break;
#endif
                  /*
                   * net3 added here for italy homologation, TC10021
                   */
                  case BRI_NET3_STYPE:
                     *ignore_msg = L3_BadPeerMsg( cr, cr_len, call_id,
                                                  ces, event );
                     break;

                  default:
                     /* Ignore SETUP with GLOBAL CALL REFERENCE.
                     */
                     *ignore_msg = TRUE;
                     break;
                  }
               }
            else
               {
               nlcb = NLCB_Head[ L3_DSL_Id ];
               while (( nlcb ) && ( nlcb->Ces != BROADCAST_CES ))
                  nlcb = nlcb->Next;
               if ( nlcb != NULL )
                  {
                  nlcb->Ces = ces;
                  nlcb->State = NULL_STATE;
                  }
               }
            break;


         case DL_EST_CONF:
            /* Search for NLCB based on CES only. A valid NLCB for
            *  this case must be in state ESTABLISH_WAIT only.
            */
            nlcb = NLCB_Head[ L3_DSL_Id ];
            while (( nlcb ) &&
                  (( nlcb->Ces != ces ) || ( nlcb->State != ESTABLISH_WAIT )))
               nlcb = nlcb->Next;
            /* Ignore this whole message if NLCB is not found.
            *  This may mean that:
            *    -  it is an out of sequence message
            *    -  or the link is already established
            */
            if ( nlcb == NULL )
               *ignore_msg = TRUE;
            break;

         case DL_EST_IND:
            /* Search for NLCB based on CES only.  If it is found it
            *  means that the link has been established and we may
            *  proceed.  If the matching NLCB does not exist, we must
            *  establish the new link.  In order to do this we will
            *  get a new NLCB and initialize it to ESTABLISH_WAIT
            *  state.  In the SDL leg for this event in this state,
            *  all nlcb's awaiting establishment will progress to
            *  State 0.
            */
            nlcb = NLCB_Head[ L3_DSL_Id ];
            while (( nlcb ) && ( nlcb->Ces != ces ))
               nlcb = nlcb->Next;
            if ( nlcb == NULL )          /* Not found - get a new NLCB */
               {
               nlcb = L3_GetNew_NLCB( ces, cr, 0 );
               if ( nlcb != NULL )
                  nlcb->State = ESTABLISH_WAIT;            /* State 99 */
               }
            /* Unless we are waiting for establishment, ignore.
            */
            else if ( nlcb->State != ESTABLISH_WAIT )
               {
               *ignore_msg = TRUE;
               }
            break;

         case DL_REL_CONF:
         case DL_REL_IND:
            /* Search for NLCB based on CES.
            */
            nlcb = NLCB_Head[ L3_DSL_Id ];
            while (( nlcb ) && ( nlcb->Ces != ces ))
               nlcb = nlcb->Next;
            if ( nlcb == NULL )
               /* Ignore the message if no NLCB's with proper
               *  CES were located.
               */
               *ignore_msg = TRUE;
            break;

         case MAN_INFO:
            /*
             * This is to handle 5ESS UIs which may come when Layer 2
             * is not active. It will cause the NLCBs to get consumed
             * and calls will fail.
             */
            if (SwitchType == BRI_5ESS_STYPE) {
               if (ces == BROADCAST_CES) {
                  *ignore_msg = TRUE;
               } else {
                  if (GLOBAL_CALL_REF(cr, cr_len)) {
                     nlcb = NLCB_Head[L3_DSL_Id];
                     while (nlcb && (nlcb->Ces != ces))
                            nlcb = nlcb->Next;
                     if (nlcb == NULL)
                        *ignore_msg = TRUE;
                  } else {
                     nlcb = L3_Search_NLCB_Ref(cr, cr_len);
                     if (nlcb == NULL)
                        *ignore_msg = TRUE;
                  }
              }
            }
            break;

         default:
            /* All other messages.  Search on call reference only.
            */
            if (( nlcb = L3_Search_NLCB_Ref( cr, cr_len )) == NULL )
               {
               /* Could not find the nlcb. This failure could either mean
               *  that the call reference is bad or that the call reference
               *  is not bad, but the call is in State 0 and this is some
               *  message besides one of the special cases addressed above.
               *  Use default bad peer message recovery.
               */
               if (detail_debug)
                   buginf ("\n calling badpeermsg from get dlcb and null");
               *ignore_msg = L3_BadPeerMsg( cr, cr_len, call_id, ces, event );
               }
            break;                                 /* End default case */
         }                                        /* End case on event */
      }
   else if ( source == CC_ID )
      {
      if ( call_id == 0 ) /* Reserved NLCB Call_ID indicating NO CALL! */
         {
         if (detail_debug)
             buginf("\nIllegal Call_ID = 0 from Call Control");
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error( "l3", "L3_GetUser_NLCB",
                    "Illegal Call_ID = 0 from Call Control" );
#endif
         *ignore_msg = TRUE;
         }
      else
         {
         /* The message was sent from Call Control
         *  The program will attempt to find a correcsponding NLCB based
         *  on Call_ID number alone. If no match is found it means that
         *  the link is not established at all or that this is a new call
         *  setup or both.  If the link is established, at least one NLCB
         *  bearing the corresponding CES should exist, but it may not be
         *  available for use.
         */
         nlcb = NLCB_Head[ L3_DSL_Id ];
         while (( nlcb ) && ( nlcb->CallId != call_id ))
            nlcb = nlcb->Next;

         /* If no match found - search for NLCB based on CES only and
         *  consider the following cases:
         *          -  match found in state ESTABLISH_WAIT
         *          -  match found in state DL_IDLE_STATE
         *          -  match found in any other state
         *  The above cases will be described in more detail below.
         */
         if ( nlcb == NULL )
            {
            nlcb = NLCB_Head[ L3_DSL_Id ];
            tmp_nlcb = NULL;
            in_loop = TRUE;
            while (( nlcb ) && ( in_loop ))
               {
               if ( nlcb->Ces == ces )
                  {
                  /* Match found - consider the various cases
                  * Try to find an available NLCB for this ces in State 0.
                  */
                  if (( nlcb->CallId == 0 ) && ( nlcb->State == NULL_STATE ))
                     /* Use this (idle) NLCB for the (new) call.
                     */
                     {
                     nlcb->CallId = call_id;           /* Save call_id */
                     in_loop = FALSE;
                     }
                  else if ( event == NL_REL_REQ )
                     {
                     in_loop = FALSE;/* Any nlcb with this CES will do */
                     }
                  else if ( nlcb->State == ESTABLISH_WAIT )
                     {
                     /* The link is still being established by another call.
                     *  Get a new NLCB and leave it in ESTABLISH_WAIT also.
                     */
                     if (( nlcb = L3_GetNew_NLCB( ces, cr, call_id )) != NULL )
                        nlcb->State = ESTABLISH_WAIT;
                     in_loop = FALSE;
                     }
                  else if ( nlcb->State == DL_IDLE_STATE )
                     /* State = DL_IDLE_STATE, exit the loop and use this
                     *  NLCB only if responding to a setup message or if
                     *  this is a "post hangup" report of far end
                     *  disconnect (CC_INFO_REQ).  Otherwise, continue in
                     *  the loop.
                     */
                     {
                     if (( event == CC_SETUP_RESP ) ||
                         ( event == CC_BROADCAST_RESP ))
                        in_loop = FALSE;
                     else if ( event == CC_INFO_REQ )
                        {
                        nlcb->CallRef = cr;
                        nlcb->CallRefLen = 1;
#ifdef BRI
#ifdef VN2
                        if ( SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
                           nlcb->AssCref = TRUE;
#endif
#endif
                        L3_UpdateTimers( nlcb, cr, ces );
                        in_loop = FALSE;
                        }
                     }
                  else
                     /* Other state.  We cannot use the same NLCB so we
                     *  will stay in the loop.  But the fact that there
                     *  is an NLCB with the same CES means that the link
                     *  has been established for this CES.  This will
                     *  determine the state of the new NLCB for this CES
                     *  (if it needs to be assigned).  The value of
                     *  tmp_nlcb (initially set to NULL) will be examined
                     *  when the new NLCB is initialized.
                     */
                     tmp_nlcb = nlcb;
                  }
               /* Look at the mext NLCB if still in loop.
               */
               if ( in_loop )
                  nlcb = nlcb->Next;
               }
            /* No match found - need to get a new NLCB.
            */
            if ( nlcb == NULL )
               {
		if (event == CC_DISCONNECT_REQ) {
		/* we are ignoring CC_DISCONNECT_REQ if no NLCB associated with 
		 * given call_id, CSCdj26436 and case H61335.
		*/
			*ignore_msg = TRUE;
			return nlcb;
		}

               /* Get a new NLCB
               */
               if (( nlcb = L3_GetNew_NLCB( ces, cr, call_id )) != NULL )
                  {
                  /* Special case - INFO request received after
                  *  RELEASE.  Will send it to layer 2 with the
                  *  proper CES and call reference.
                  */
                  if ( event == CC_INFO_REQ )
                     {
                     nlcb->CallRef = cr;
                     nlcb->CallRefLen = 1;
#ifdef BRI
#ifdef VN2
                     if ( SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
                        nlcb->AssCref = TRUE;
#endif
#endif
                     }
                  /* Initialize to a proper state based on whether
                  *  an NLCB with the same CES was found earlier in
                  *  an initialized state.
                  */
                  if ( ces != BROADCAST_CES )
                     {
                     if ( tmp_nlcb == NULL )
                        nlcb->State = DL_IDLE_STATE;
                     else
                        nlcb->State = NULL_STATE;
                     }
                  }
               }

            }
         }
      }
   else if ( source == L3_ID )
      {
      /* The message was sent by L3 to itself (Timer Messages).
      *  Conduct the search based upon Call Reference alone.
      *  NOTE that the call reference is actually returned from
      *  the timer routine as the call_id as this is the way the
      *  timer was updated!!
      */
      
         switch ( SwitchType )
            {
#ifdef PRI_1TR6

            case PRI_1TR6_STYPE:
               nlcb = L3_Search_NLCB_Ref( call_id, PRI_1TR6_STD_CALL_REF_LEN );
               break;
#endif
            case PRI_4ESS_STYPE:
            case PRI_5ESS_STYPE:
            case PRI_DMS100_STYPE:
            case PRI_NET5_STYPE:
            case PRI_NTT_STYPE:
            case PRI_TS014_STYPE:
               nlcb = L3_Search_NLCB_Ref( call_id, PRI_STD_CALL_REF_LEN );
               break;
            default:
               /*
                * All of the PRI switch types should be taken care
                * of above. This is for BRI only.
                */	
               nlcb = L3_Search_NLCB_Ref( call_id, BRI_STD_CALL_REF_LEN );
               break;
            }
      }
#ifdef BRI_5ESS
   else if (( SwitchType == BRI_5ESS_STYPE ) && ( source == ME_ID ))
      {
      nlcb = NLCB_Head[ L3_DSL_Id ];
      while (( nlcb ) && ( nlcb->Ces != ces ))
         nlcb = nlcb->Next;
      if (nlcb == NULL)
         *ignore_msg = TRUE;
      }
#endif

   return ( nlcb );
}
#endif

/**************
* L3_GetState *
***************
*
* Get Call State from the Info element. This function uses CCIE
* functions in order to find Call State I.E. from the info string
* (passed to it as a parameter).  It returns call state.  If no
* call state info element is found, this function will return call
* state 0.  The function also provides the appropriate CAUSE value
* under the assuption that this is a mandatory information element.
*
*/
PUBLIC L3_STATE L3_GetState (info_string, info_len, cause)

BYTE *info_string;            /* info string (part of the data packet) */
int   info_len;                               /* Length of info_string */
BYTE *cause;                  /* Cause return value for problems found */

{
   L3_STATE  call_state = 0;                    /* call state variable */
   BYTE     *call_state_ie;                       /* call state string */
#ifdef VN2
   BYTE      call_state_att;
#endif

   *cause = 0;                                           /* Initialize */
   call_state_ie = CCIE_Get_IE( info_string, info_len, CALL_STATE_IE,
                                CODESET_0, 1 );
   if ( call_state_ie == NULL )
      {
      *cause = MANDATORY_IE_MISSING;                       /* Cause 96 */
      call_state = NULL_STATE;
      }
#ifdef BRI_DMS100
   else if ((( SwitchType == BRI_DMS100_STYPE ) || 
                ( SwitchType == BRI_NI1_STYPE )) &&         /*===*/
            ( call_state_ie[1] != CALL_STATE_LENGTH ))
      {
      *cause = INVALID_ELEM_CONTENTS;                     /* Cause 100 */
      }
#endif                                                   /* BRI_DMS100 */
   else
      {
#ifdef VN2
      if ( SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
         {
         if ( call_state_ie[ 1 ] != 1 )  /* Check for IE length error. */
            {
            *cause = MANDATORY_IE_LEN_ERR;                /* Cause 103 */
            }
         else
            {
            call_state_att = call_state_ie[ 2 ];
            if ( call_state_att == 254 )
               call_state_att = 25;
            call_state = (L3_STATE) ( call_state_att );
            }
         }
      else
         {
#endif                                                          /* VN2 */
         call_state = (L3_STATE) ( call_state_ie[2] & CALL_STATE_MASK );
#ifdef VN2
         }
#endif
      /* Test for some undefined state.
      */
      if (( *cause == 0 ) && ( !L3_ValidState( (int)call_state ) ))
         {
         switch ( SwitchType )
            {
#ifdef VN2
            case VN2_STYPE:
            case VN3_STYPE:
               if ( call_state != NULL_STATE )
                  *cause = INVALID_ELEM_CONTENTS;         /* Cause 100 */
               break;
#endif
            default:
               *cause = INVALID_ELEM_CONTENTS;            /* Cause 100 */
               break;
            }
         }
      }

   return ( call_state );
}

#ifdef PRI
/*****************
* L3_GlobalEvent *
******************
*
* Handle the special case of CC_STATUS_REQ for the global call reference
* from Call Control.
*
* Handle the special case of CC_REL_COMP_REQ for the global call reference
* from Call Control.
*
* These cases arises in error recovery for erroneous messages to the global
* call reference detected in call control, such as bad CC_RESTART_IND.
* Re-label and relay the message down.  This function does an end around
* to the SDLs since the global call reference does not have an NLCB.
*
*/
PRIVATE void near L3_GlobalEvent( event, ces )

int  event;                     /* Event for the global call reference */
BYTE ces;                                /* Connection Endpoint Suffix */
{
   if(detail_debug)
      buginf("\nentering L3_GlobalEvent");
   switch ( event )
      {
      case GLOBAL_STATUS:
         CCIE_Change_Msg_Type( L3_PktIn->Info, STATUS );
         break;
      case GLOBAL_REL_COMP:
         CCIE_Change_Msg_Type( L3_PktIn->Info, RELEASE_COMP );
         break;
      default:
         break;                                    /* Should not occur */
      }

   LIF_SendPkt( L2P_LAPD, L3_ID, 0, L2_ID, DL_DATA_REQ, L3_DSL_Id,
                Q931_SAPI, ces, 0, 0, Q931_DSL_CHAN_ID, TRUE, L3_PktIn,
                NOW, REGULAR_MAIL );
   L3_AvailPktIn = FALSE;            /* The packet has been mailed. */
}

#endif
/********
* L3_Go *
*********
*
* This is the entry point to this module for message processing.
*
* Process a Layer 3 message.  Locate the Network Link Control Block
* for this CES.  Determine the event associated with this message
* (may have to look into the INFO elements).  Based on the event
* and the NLCBs state, jump to the appropriate 'state machine' SDL
* function.  If internal events await processing, process them.
*
*/
PUBLIC void L3_Go( source, event, dsl_id, ces, call_id, pkt )

int    source;                                 /* source of this event */
int    event;                     /* the event which has just occurred */
BYTE   dsl_id;                              /* digital subscriber loop */
BYTE   ces;                             /* connections endpoint suffix */
int    call_id;                                      /* call ID number */
PKT_ID pkt;                                         /* incoming packet */
{
   BYTE    cr_len;                                        /* cr length */
   int     cr;                                       /* call reference */
   int	   i, j;
   BOOLEAN ignore_msg;             /* ignore or process this message ? */
   NLCB_ID next_nlcb;
#ifdef USER
   BOOLEAN save_cr;                /* Save call reference in the NLCB? */
#endif

#ifdef DEBUG
   printf("\n%x\n", pkt);
   if ( pkt )
      LIF_DispPkt( pkt );
#endif

   if(detail_debug)
      buginf("\nentering L3_Go");

   cr_len = 0;
   cr = 0;
   ignore_msg = FALSE;

   /* Save certain incoming parameters.
   */
   L3_AvailPktIn = (( pkt && pkt->Info ) ? TRUE : FALSE);
   L3_PktIn      = pkt;
   L3_DSL_Id     = dsl_id;
   L3_MsgSource  = source;
   L3_Call_Id    = call_id;

   if ( L3_AvailPktIn )                      /* Unpack the info fields */
		L3_GetInfo(&event, pkt->Info, &pkt->InfoLen, &cr_len, &cr, source);

   switch ( event )                      /* Weed out the special cases */
      {
      case DL_UI_DATA_IND:
         if(detail_debug)
            buginf("\nL3_Go DL_UI_DATA_IND");
         if (pkt->InfoLen == 0) {
             if (L3_PktIn) {
                 /* got an empty packet, free it */
                 LIF_FreePkt(L3_PktIn);
             }
             break;
         }
         /*
          * VN2 has an odd concept of what constitutes a SETUP. ===
          */
         if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
            if (pkt->Header[0] == 2 && pkt->Header[1] == 0xff) {
                /*
                 * This is a badly formed SETUP message (believe
                 * it or not).  I don't really understand how T'works
                 * allocates and de-allocates Packages/Info, so I'll
                 * just recycle the one that I got.
                 *
                 * Note how I avoid all the mess of figuring out
                 * what NLCB to use and such by just formatting everything
                 * by myself.
                 */
                i = 2 + pkt->Info[1];
                pkt->Info[i++] = RELEASE_COMP;
                pkt->Info[i++] = CAUSE_IE;
                pkt->Info[i++] = 2;
                pkt->Info[i++] = 0x80;
                pkt->Info[i++] = 0x80 | INVALID_CALL_REF;
                pkt->InfoLen = i;
				
                /* Make room for a 4 byte header */
                for(j = i - 1 ; j >= 0 ; j--)
                pkt->Info[j + 1] = pkt->Info[j];
                pkt->HeadLen = 4;

                LIF_SendPkt(L2P_LAPD,		/* protocol */
                    L3_ID,			/* source layer id */
                    (BYTE)0,			/* sender's state */
                    L2_ID,			/* destination layer id */
                    DL_DATA_REQ,		/* event to mail */
                    dsl_id,			/* digital subscriber loop*/
                    Q931_SAPI,			/* SAPI */
                    SIGNALING_CES,		/* CES */
                    0,				/* logical link id */
                    call_id,			/* call id */
                    Q931_DSL_CHAN_ID,		/* channel ID */
                    TRUE,			/* free packet? */
                    pkt,			/* address of packet */
                    NOW,			/* delay */
                    REGULAR_MAIL);		/* mail priority */
                 return;
            }
         }
         /*
          * Pass these messages directly to Call Control.
          */
         L3_Mail( NL_UI_DATA_IND, CC_ID, FALSE, FALSE );
         break;
      case RESTART:
      case RESTART_ACK:
         if(detail_debug)
            buginf("\nL3_Go RESTART source is %x ", source);
         L3_Restart( cr, cr_len, event, ces, pkt );
         break;
      case CC_RESTART_REQ:
         if(detail_debug)
            buginf("\nL3_Go CC_RESTART_REQ");
         L3_RestartReq( ces, call_id, pkt );
         break;
#ifdef PRI
      case NL_MAINT_REQ:
      case NL_MAINT_IND:
         if(detail_debug)
            buginf("\nL3_Go NL_MAINT");
         L3_MaintMsg( event, ces );
         break;
      case CC_RESTART_ACK_REQ:
         if(detail_debug)
            buginf("\nL3_Go CC_RESTART_ACK_REQ");
         L3_RestartAckReq( ces, pkt );
         break;
      case GLOBAL_STATUS:
      case GLOBAL_REL_COMP:
         if(detail_debug)
            buginf("\nL3_Go GLOBAL_");
         L3_GlobalEvent( event, ces );
         break;
#endif
      case NULL_CREF:
         if(detail_debug)
            buginf("\nL3_Go NULL_CREF");
         L3_NullCref( source, ces );
         break;
      case COMPLETED:
         if(detail_debug)
            buginf("\nL3_Go COMPLETED");
         break;/* Dummy event from L3_GetInfo for event to be ignored. */
      case BAD_PD:	/* === */
         if(detail_debug)
            buginf("\nL3_Go BAD_PD");
         break;

      case DL_EST_IND :
         if ((SwitchType == PRI_5ESS_STYPE) ||
             (SwitchType == PRI_DMS100_STYPE)) {
             NLCB = NLCB_Head[ L3_DSL_Id ];
             while (NLCB) {
                next_nlcb = NLCB->Next;
                if (NLCB->State == CALL_CONNECTED)
                   L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);
                NLCB = next_nlcb;
             }
         }
      default:
         if(detail_debug)
            buginf("\nL3_Go default, the event = %x", event);
         /*
          * Need to ignore setups with CRef flag set.  By the time we
          * get here, Telenetworks' L3_GetInfo routine has toggled the
          * flag, so we test the inverse of what we want
          */
          if ((SwitchType == VN2_STYPE || SwitchType == VN3_STYPE ||
               SwitchType == BRI_NI1_STYPE || 
               SwitchType == BRI_TS013_STYPE ||
               SwitchType == PRI_TS014_STYPE) && event == SETUP) {
                if (pkt->Info[1] && !(pkt->Info[2] & 0x80)) {
                    LIF_FreePkt(pkt);
                    return;
                }
          }
			
         /* Get NLCB associated with this Call Reference or Call_ID.
         */
         NLCB = L3_GetUser_NLCB( ces, call_id, cr, cr_len, event, source,
            &ignore_msg );
            if (detail_debug)
                buginf("\nL3_Go NCLB is %x ignore_msg is %x", NLCB, ignore_msg);
         if (( NLCB == NULL ) && ( ignore_msg == FALSE ))
            {
            if (detail_debug)
                buginf("\nL3_GetUser_NLCB failed");
            LIF_Error( "l3", "L3_Go", "L3_GetUser_NLCB failed." );
            }
         else if ( ignore_msg == FALSE )
            {
            NLCB->Event = event;       /* Put the event into the NLCB. */
            if ( pkt )
               {
               if ( NLCB->CallRefLen == 0 )           /* Save cref len */
                  NLCB->CallRefLen = cr_len;
#ifdef USER
               switch ( SwitchType )
                  {
#ifdef BRI
#ifdef VN2
                  case VN2_STYPE:
                  case VN3_STYPE:
                     if (( save_cr = (( NLCB->AssCref == FALSE ) &&
                                (( source == L2_ID ) ||
                                 ( event == CC_BROADCAST_RESP )))) == TRUE )
                        NLCB->AssCref = TRUE;
                     break;
#endif
#endif
                  default:
                     save_cr = (( NLCB->CallRef == 0 ) &&
                                (( source == L2_ID ) ||
                                 ( event == CC_BROADCAST_RESP )));
                     break;
                  }

               if ( save_cr )
                  {
                  NLCB->CallRef = cr;    /* Put cref value into the NLCB. */
                  if ( event != SETUP )
                     /* In the case of SETUP, the BROADCAST_NLCB is in use and
                     *  timer updating is a wasted effort since the use of this
                     *  NLCB is transitory with the sending of the CC_SETUP_IND.
                     */
                     L3_UpdateTimers( NLCB, cr, NLCB->Ces );
                  }
#endif                                                         /* USER */
               }
#ifdef BRI
            /* Now attempt to fill in the B-channel ID if it has not already
            *  been filled in.  The msg source is significant as we assume
            *  the Network Call Control ultimately selects the channel.
            */
          if((SwitchType != PRI_5ESS_STYPE) && (SwitchType != PRI_DMS100_STYPE)
              && (SwitchType != PRI_4ESS_STYPE) && (SwitchType != PRI_NTT_STYPE)
              && (SwitchType != PRI_NET5_STYPE) && (SwitchType != PRI_TS014_STYPE)) {
            if (( NLCB->ChanId == 0 ) && ( source == L2_ID ) && ( pkt ))
               L3_UpdateChannel( pkt );
          }
#endif                                                          /* BRI */
            L3_ProcessEvent();   /* Process the main event in the SDLs */
            if (( NLCB ) && ( NLCB->Internal ))
               L3_ProcessInternal();      /* Process internal event(s) */
            }
         break;                                 /* End of default case */
      }                                      /* End of switch on event */

   if ( L3_AvailPktIn )              /* Free the packet if appropriate */
      {
#ifdef DEBUG
      printf( "FREE Incoming Packet\n" );
#endif
      LIF_FreePkt( L3_PktIn );
      }
}

/****************
* L3_InfoLenErr *
*****************
*
* Test if the info length in the incoming message is below the minimum
* allowable length on a SwitchType basis.
*
*/
PRIVATE BOOLEAN near L3_InfoLenErr( info_len, cr_len )

int info_len;                               /* Length of info to check */
int cr_len;                                /* Length of call reference */

{
   BOOLEAN len_err = TRUE;              /* Function result initialized */


   switch ( SwitchType )                /* Evaluate the message length */
      {
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
         /* 5ESS switches routinely send INFO with NULL call reference which
         *  are three octets in length.
         */
         len_err = ( info_len < 3 );
         break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
         len_err = ( info_len < 4 );
         break;
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:           /*===*/
         if ( cr_len == 1 )
            len_err = ( info_len < 4 );
         else
            len_err = ( info_len < 5 );
         break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
         if ( cr_len == 1 )
            len_err = ( info_len < 4 );
         else
            len_err = ( info_len < 5 );
         break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
         len_err = (( cr_len != 1 ) || ( info_len < 4 ));
         break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
         len_err = (( cr_len != 1 ) || ( info_len < 4 ));
         break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
#endif
      default:            /* Msg too short to contain a message type ? */
         if ( cr_len == 0 )                            /* 0 octet CREF */
            len_err = ( info_len < 3 );
         else if ( cr_len == 1 )                     /* One octet CREF */
            len_err = ( info_len < 4 );
         else if ( cr_len == 2 )                     /* Two octet CREF */
            len_err = ( info_len < 5 );
         break;
      }

   /* Prevent possible compiler warning for non-referenced parameter.
   */
   Trashcan = cr_len;

   return( len_err );
}


/**********
* L3_Init *
***********
*
* Initialize Layer 3 Variables
*
*/
PUBLIC void L3_Init ()
{
   NLCB_ID nlcb;

   NLCB = NULL;
   L3_CallRef = 1;                      /* Initial cref to be assigned */
   Diagnostic[ 0 ] = Diagnostic[ 1 ] = Diagnostic[ 2 ] = 0;

   /*
    * Establish one BROADCAST nlcb per DSL.
    */
   for ( L3_DSL_Id = 0; L3_DSL_Id < MAX_DSL; L3_DSL_Id++ )
      {
      NLCB_Head[ L3_DSL_Id ] = NLCB_Tail [L3_DSL_Id ] = NULL;
      if (( nlcb = L3_Alloc_NLCB() ) == NULL )
         {
         /* Out of memory blocks.
         */
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error( "l3", "L3_Init", "L3_Alloc_NLCB failed." )
#endif
         ;
         }
      else
         {
         /* Initialize new NLCB.
         */
         if (( L3_Init_NLCB( nlcb, 0, BROADCAST_CES, 0 )) == ERR )
            {
            /* Out of memory blocks to prepare NLCB.
            */
#ifdef DISPLAY_LOCAL_ERR
            LIF_Error( "l3", "L3_Init", "L3_Init_NLCB failed." );
#endif
            L3_Free_NLCB( nlcb );
            nlcb = NULL;
            }
         else
            {
            /* Add new NLCB to the list of NLCB's.
            */
            NLCB_Head[ L3_DSL_Id ] = nlcb;
            NLCB_Tail[ L3_DSL_Id ] = nlcb;
            }
         }
      }

   /* Now fill in the correct timer values on a SwitchType basis.
   *  This activity must be performed at run-time because a single
   *  application may support multiple switches as a configurable
   *  item.
   */
   T310_TimeOut = VN2_T310;	/* default to a known value ===*/
   switch ( SwitchType )
      {
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
         T303_TimeOut = BRI_5ESS_T303;
         T305_TimeOut = BRI_5ESS_T305;
         T308_TimeOut = BRI_5ESS_T308;
         T316_TimeOut = DEFAULT_T316;
#ifdef USER
         T313_TimeOut = DEFAULT_T313;
         T318_TimeOut = DEFAULT_T318;
         T319_TimeOut = DEFAULT_T319;
#endif
         break;
#endif                                                     /* BRI_5ESS */
#ifdef PRI_4ESS
      case PRI_4ESS_STYPE:
         T303_TimeOut = PRI_4ESS_T303;
         T305_TimeOut = PRI_4ESS_T305;
         T308_TimeOut = PRI_4ESS_T308;
         T310_TimeOut = PRI_4ESS_T310; /*===*/
         T316_TimeOut = PRI_4ESS_T316;
#ifdef USER
         T313_TimeOut = PRI_4ESS_T313;
         T318_TimeOut = DEFAULT_T318;
         T319_TimeOut = DEFAULT_T319;
#endif
         break;
#endif                                                     /* PRI_4ESS */
#ifdef PRI_5ESS
      case PRI_5ESS_STYPE:
         T303_TimeOut = PRI_5ESS_T303;
         T305_TimeOut = PRI_5ESS_T305;
         T308_TimeOut = PRI_5ESS_T308;
         T316_TimeOut = PRI_5ESS_T316;
#ifdef USER
         T313_TimeOut = PRI_5ESS_T313;
         T318_TimeOut = DEFAULT_T318;
         T319_TimeOut = DEFAULT_T319;
#endif
         break;
#endif                                                     /* PRI_5ESS */

      case PRI_NET5_STYPE:
         T303_TimeOut = PRI_NET5_T303;
         T305_TimeOut = PRI_NET5_T305;
         T308_TimeOut = PRI_NET5_T308;
         T310_TimeOut = PRI_NET5_T310;
         T316_TimeOut = PRI_NET5_T316;
         T313_TimeOut = PRI_NET5_T313;
         T318_TimeOut = DEFAULT_T318;
         T319_TimeOut = DEFAULT_T319;
         break;

      case PRI_NTT_STYPE:
         T303_TimeOut = PRI_NTT_T303;
         T305_TimeOut = PRI_NTT_T305;
         T308_TimeOut = PRI_NTT_T308;
         T310_TimeOut = PRI_NTT_T310;
         T316_TimeOut = PRI_NTT_T316;
         T313_TimeOut = PRI_NTT_T313;
         T318_TimeOut = DEFAULT_T318;
         T319_TimeOut = DEFAULT_T319;
         break;

      case PRI_TS014_STYPE:
         T303_TimeOut = PRI_TS014_T303;
         T305_TimeOut = PRI_TS014_T305;
         T308_TimeOut = PRI_TS014_T308;
         T310_TimeOut = PRI_TS014_T310;
         T316_TimeOut = PRI_TS014_T316;
         T313_TimeOut = PRI_TS014_T313;
         T318_TimeOut = DEFAULT_T318;
         T319_TimeOut = DEFAULT_T319;
         break;

#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
         T303_TimeOut = BRI_DMS100_T303;
         T305_TimeOut = BRI_DMS100_T305;
         T308_TimeOut = BRI_DMS100_T308;
         T316_TimeOut = BRI_DMS100_T316;
#ifdef USER
         T313_TimeOut = BRI_DMS100_T313;
         T318_TimeOut = BRI_DMS100_T318;
         T319_TimeOut = BRI_DMS100_T319;
#endif
         break;
#endif                                                   /* BRI_DMS100 */
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
         T303_TimeOut = PRI_DMS100_T303;
         T305_TimeOut = PRI_DMS100_T305;
         T308_TimeOut = PRI_DMS100_T308;
         T316_TimeOut = PRI_DMS100_T316;
#ifdef USER
         T313_TimeOut = PRI_DMS100_T313;
         T318_TimeOut = DEFAULT_T318;
         T319_TimeOut = DEFAULT_T319;
#endif
         break;
#endif                                                   /* PRI_DMS100 */
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
         T303_TimeOut = VN2_T303;
         T305_TimeOut = VN2_T305;
         T308_TimeOut = VN2_T308;
         T316_TimeOut = DEFAULT_T316;
#ifdef USER
         T310_TimeOut = VN2_T310;
         T313_TimeOut = VN2_T313;
         T318_TimeOut = VN2_T318;
         T319_TimeOut = VN2_T319;
#endif
         break;
#endif                                                          /* VN2 */
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
         T303_TimeOut = BRI_1TR6_T303;
         T305_TimeOut = BRI_1TR6_T305;
         T308_TimeOut = BRI_1TR6_T308;
         T316_TimeOut = DEFAULT_T316;
         T313_TimeOut = BRI_1TR6_T313;
         T318_TimeOut = BRI_1TR6_T318;
         T319_TimeOut = BRI_1TR6_T319;
         break;
#endif                                                     /* BRI_1TR6 */
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
         T303_TimeOut = PRI_1TR6_T303;
         T305_TimeOut = PRI_1TR6_T305;
         T308_TimeOut = PRI_1TR6_T308;
         T316_TimeOut = DEFAULT_T316;
         T313_TimeOut = PRI_1TR6_T313;
         T318_TimeOut = DEFAULT_T318;
         T319_TimeOut = DEFAULT_T319;
         break;
#endif                                                     /* PRI_1TR6 */
#ifdef BRI_NET3		/*===*/
      case BRI_NET3_STYPE:
         T303_TimeOut = NET3_T303;
         T305_TimeOut = NET3_T305;
         T308_TimeOut = NET3_T308;
         T310_TimeOut = NET3_T310;
         T313_TimeOut = NET3_T313;
         T318_TimeOut = NET3_T318;
         T319_TimeOut = NET3_T319;
         break;
#endif                                                     /* BRI_NET3 */
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
         T303_TimeOut = TS013_T303;
         T305_TimeOut = TS013_T305;
         T308_TimeOut = TS013_T308;
         T310_TimeOut = TS013_T310;
         T313_TimeOut = TS013_T313;
         T318_TimeOut = TS013_T318;
         T319_TimeOut = TS013_T319;
         break;
#endif                                                  /* BRI_TS013 */
#ifdef BRI_NI1
      case BRI_NI1_STYPE:               /*===*/
         T303_TimeOut = BRI_NI1_T303;
         T305_TimeOut = BRI_NI1_T305;
         T308_TimeOut = BRI_NI1_T308;
         T316_TimeOut = BRI_NI1_T316;
         T313_TimeOut = BRI_NI1_T313;
         T318_TimeOut = BRI_NI1_T318;
         T319_TimeOut = BRI_NI1_T319;
#endif                                                     /* BRI_NI1 */
         break;
      default:                                                /* CCITT */
         T303_TimeOut = DEFAULT_T303;
         T305_TimeOut = DEFAULT_T305;
         T308_TimeOut = DEFAULT_T308;
         T316_TimeOut = DEFAULT_T316;
#ifdef USER
         T313_TimeOut = DEFAULT_T313;
         T318_TimeOut = DEFAULT_T318;
         T319_TimeOut = DEFAULT_T319;
#endif
         break;
      }
}

/***************
* L3_Init_NLCB *
****************
*
* Initialize Network Link Control Block
*
*  Return:  NOERR, if successful; ERR, otherwise.
*/
PRIVATE int near L3_Init_NLCB (nlcb, cr, ces, call_id)

NLCB_ID nlcb;                                    /* NLCB to initialize */
int     cr;                                          /* Call Reference */
BYTE    ces;                             /* connection endpoint suffix */
int     call_id;                                            /* call ID */
{
   int ret_code = ERR;             /* Start off anticipating the worst */


   /* Initialize NLCB fields
   */
   nlcb->Ces        = ces;
#ifdef USER
   nlcb->State      = (ces == BROADCAST_CES) ? BROADCAST_STATE : DL_IDLE_STATE;
#endif
#ifdef BRI
#ifdef VN2
   nlcb->AssCref    = FALSE;
#endif
#endif
   nlcb->CallRefLen = 0;
   nlcb->CallRef    = 0;
   nlcb->Event      = 0;
   nlcb->CallId     = call_id;
#ifdef BRI
   nlcb->ChanId     = 0;
#endif

   nlcb->Cause = 0;
#ifdef BRI_DMS100
#ifdef USER
   nlcb->PreviousCause = 0;
#endif
#endif

   /* Add Timers:
   *  Timers T302, T304, T306, T309, T310, T312 & T322 are NETWORK side only.
#ifdef VN2
   *  Timer T310 is supported for User VN3.
#endif
   *  Timers T313, T318 & T319 are USER side only.
   *  Timers T303, T305 & T308 are common.
   */


   if ((nlcb->Timer_T303 = LIF_AddTimer( L3_ID, L3_ID, T303_EXPIRY,
                                         L3_DSL_Id, Q931_SAPI, ces, cr,
                                         0, T303_TimeOut)) == ERR )
      goto bailout;

   if ((nlcb->Timer_T305 = LIF_AddTimer( L3_ID, L3_ID, T305_EXPIRY,
                                         L3_DSL_Id, Q931_SAPI, ces, cr,
                                         0, T305_TimeOut)) == ERR )
      goto bailout;

   if ((nlcb->Timer_T308 = LIF_AddTimer( L3_ID, L3_ID, T308_EXPIRY,
                                         L3_DSL_Id, Q931_SAPI, ces, cr,
                                         0, T308_TimeOut)) == ERR )
      goto bailout;
#ifdef USER
#ifdef VN2
   if ((nlcb->Timer_T310 = LIF_AddTimer( L3_ID, L3_ID, T310_EXPIRY,
                                         L3_DSL_Id, Q931_SAPI, ces, cr,
                                         0, T310_TimeOut)) == ERR )
      goto bailout;
#endif
#endif
#ifdef USER
   if ((nlcb->Timer_T313 = LIF_AddTimer( L3_ID, L3_ID, T313_EXPIRY,
                                         L3_DSL_Id, Q931_SAPI, ces, cr,
                                         0, T313_TimeOut)) == ERR )
      goto bailout;

   if ((nlcb->Timer_T318 = LIF_AddTimer( L3_ID, L3_ID, T318_EXPIRY,
                                         L3_DSL_Id, Q931_SAPI, ces, cr,
                                         0, T318_TimeOut)) == ERR )
      goto bailout;

   if ((nlcb->Timer_T319 = LIF_AddTimer( L3_ID, L3_ID, T319_EXPIRY,
                                         L3_DSL_Id, Q931_SAPI, ces, cr,
                                         0, T319_TimeOut)) == ERR )
      goto bailout;

   nlcb->Timer_twait = LIF_AddTimer(L3_ID, L3_ID, TWAIT_EXPIRY, L3_DSL_Id,
                                    Q931_SAPI, ces, cr, 0, T319_TimeOut);
   if (nlcb->Timer_twait == ERR)
       goto bailout;
#endif

   /* No error
   */
   ret_code = NOERR;
   nlcb->T303_First = TRUE;
   nlcb->T308_First = TRUE;
#ifdef USER
   nlcb->T313_First = TRUE;
#endif

   nlcb->Internal = FALSE;

   /* Initialize packet queues
   */
   nlcb->MsgQ.Count  = 0;
   nlcb->InfoQ.Count = 0;

   nlcb->Next = NULL;

bailout:

   return( ret_code );
}

/**********
* L3_Mail *
***********
*
*  Mail an event to a destination.
*
*/
PUBLIC void L3_Mail (event, dest_id, new_msg, network)

int     event;                                        /* event to mail */
int     dest_id;                               /* destination layer ID */
BOOLEAN new_msg;            /* The message is entirely constucted here */
BOOLEAN network;                 /* Appear as the Network side (Cause) */
{

   /* Output packet management algorithm:
   *
   * if event is a NL_EST_REQ this message source must be CC.
   * elsif mailing DL_EST_REQ or NL_EST_CONF no packet is needed.
   * elsif mailing NL_UI_DATA_IND to CC use incoming packet.
   * elsif not mailing DL_EST_REQ or NL_EST_CONF (need message packet)
   *     if an incoming pkt exists with info and this is not a "new_msg"
   *         L3_PktOut is allocated and L3_PktIn copied to it.
   *         The msg_type may be changed.
   *         The incoming msg_type is copied to the protocol discriminator
   *         if the destination is CC.
   *     else
   *         L3_PktOut is allocated and the message is created locally.
   *     PutInfo (write call reference and message type to out packet)
   */

   switch ( event )
      {
      case NL_EST_REQ:
         /* We are sending a naked primitive and do not require Pkt or Info.
         *  This is an internally generated event from State 99 DLEst_Conf.
         *  It must be made to appear to have a Call Control source.
         */
   if(detail_debug)
      buginf("\nsending NL_EST_REQ in L3_Mail");
         LIF_SendPkt( L2P_LAPD, CC_ID, (BYTE)NLCB->State, L3_ID,
                      NL_EST_REQ, L3_DSL_Id, Q931_SAPI, NLCB->Ces, 0,
                      NLCB->CallId, Q931_DSL_CHAN_ID, TRUE, NULL, NOW,
                      REGULAR_MAIL );
         break;

      case DL_EST_REQ:
      case DL_REL_REQ:
      case NL_EST_CONF:
      case NL_REL_CONF:
      case NL_REL_IND:
#ifdef BRI_5ESS
      case MNL_EST_CONF:
#endif
         /* We are sending a naked primitive and do not require Pkt or Info.
         */
         LIF_SendPkt( L2P_LAPD, L3_ID, (BYTE)NLCB->State, dest_id,
                      event, L3_DSL_Id, Q931_SAPI, NLCB->Ces, 0,
                      NLCB->CallId, Q931_DSL_CHAN_ID, FALSE, NULL, NOW,
                      REGULAR_MAIL );
         break;

      case NL_UI_DATA_IND:
         LIF_SendPkt( L2P_LAPD, L3_ID, 0, CC_ID, NL_UI_DATA_IND, L3_DSL_Id,
                      Q931_SAPI, 0, 0, 0, Q931_DSL_CHAN_ID, TRUE, L3_PktIn,
                      NOW, REGULAR_MAIL );
         L3_AvailPktIn = FALSE;      /* The L3_PktIn has been remailed */
         L3_PktIn = NULL;
         break;

      default:                               /* Need a Packet and Info */
         if ( L3_PktIn && L3_PktIn->Info && !new_msg )
            {
            /* In this case the PktOut will simply be the PktIn recopied.
            */
            L3_PktOut = LIF_GetPkt( L3_PktIn->InfoType,
                                    L3_PktIn->HeadLen, TRUE );
#ifdef FFS_CHECK_ERRORS
            if (L3_PktOut == NULL)
               LIF_Fatal( "l3", "L3_Mail", "LIF_GetPkt failed" );
#endif
            LIF_CpyPkt (L3_PktOut, L3_PktIn);
            }
         else
            {
            /* In this case the out msg is a new creation because it is
            *  very different than tbe incoming msg and an incoming pkt
            *  may not even exist.
            */
            L3_PktOut = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
            if( L3_PktOut == NULL )
               LIF_Fatal( "l3", "L3_Mail", "LIF_GetPkt failed" );
#endif
            L3_PktOut->HeadLen  = DATA_OFFSET;                    /* 4 */
            /* To be adjusted from the default as the msg is constructed.
            */
            L3_PktOut->InfoLen = 4;
            }
         L3_PutInfo( event, L3_PktOut, new_msg, network );

         switch ( dest_id )
            {
            case ME_ID:
               break;
            case L2_ID:                                     /* To LAPD */
               event = DL_DATA_REQ;
               break;
            case CC_ID:                             /* To call control */
               /* For msg to CC, the incoming event is replicated in
               *  the protocol discriminator field.
               */
               L3_PktOut->Info[0] = (BYTE) NLCB->Event;
               event = NL_DATA_IND;
               break;
            default:                             /* Should never occur */
#ifdef DISPLAY_LOCAL_ERR
               LIF_Error( "l3", "L3_Mail", "Invalid Destination ID." );
#endif
               break;
            }
         /* Mail the packet.
         */
         LIF_SendPkt (L2P_LAPD, L3_ID, (BYTE)NLCB->State, dest_id,
                      event, L3_DSL_Id, Q931_SAPI, NLCB->Ces, 0,
                      NLCB->CallId, Q931_DSL_CHAN_ID, TRUE, L3_PktOut, NOW,
                      REGULAR_MAIL);
         break;
      }                                               /* End of switch */
}


#ifdef PRI
/**************
* L3_MaintMsg *
***************
*
* Relay SERVICE/SERVICE_ACK messages up on down the ISDN stack without
* further study.  The state machine for PD = 3 is implemented in CC_SERV.C.
*
* These channel maintenance messages are sent directly to the Call Control
* or L2.  It is assumed that Call Control has filled in the Protocol
* Discriminator and Call Reference in these messages when it is the message
* source.
*
*/
PRIVATE void near L3_MaintMsg( event, ces )

int  event;                       /* The event which has just occurred */
BYTE ces;                                /* Connection endpoint suffix */

{
   int dest_id;                                /* Destination layer ID */

   if ( event == NL_MAINT_REQ )                     /* Message from CC */
      {
      dest_id = L2_ID;
      event = DL_DATA_REQ;        /* Change incoming to outgoing event */
      }
   else                                                     /* From L2 */
      {
      dest_id = CC_ID;
      event = NL_MAINT_IND;       /* Change incoming to outgoing event */
      }

   LIF_SendPkt( L2P_LAPD, L3_ID, 0, dest_id, event, L3_DSL_Id, Q931_SAPI,
                ces, 0, 0, Q931_DSL_CHAN_ID, TRUE, L3_PktIn, NOW,
                REGULAR_MAIL );
   L3_AvailPktIn = FALSE;
}

#endif

/**************
* L3_NullCref *
***************
*
* Handle messages with the NULL (zero length or dummy) call reference.
* These messages are not analyzed in L3, but rather, are simply
* forwarded UP or DOWN.
*
*/
PRIVATE void near L3_NullCref( source, ces )

int  source;                            /* Source task for the message */
BYTE ces;                                /* Connection endpoint suffix */
{
   int event;
   int dest_id;

   switch ( source )
      {
      case L2_ID:
         event = NL_DATA_IND;
         dest_id = CC_ID;
         if (( L3_PktIn ) && ( L3_PktIn->Info ))
            {
            /* Perform message type translation.
            *  Only the INFO message is a type currently expected in
            *  this context.
            */
            switch ( L3_PktIn->Info[ 2 ] )
               {
               case INFO:
                  /* Overwrite PD with L2 msg_type as always.
                  */
                  L3_PktIn->Info[ 0 ] = INFO;
                  /* Change msg_type.
                  */
                  L3_PktIn->Info[ 2 ] = CC_INFO_IND;
                  break;
               default:
                  /* Don't know anything else to be expected.
                  */
                  break;
               }
            }
         break;
      case CC_ID:
         event = DL_DATA_REQ;
         dest_id = L2_ID;
		/* === added the following code */
         switch ( SwitchType )
            {
            case BRI_NI1_STYPE:
            case BRI_DMS100_STYPE:
               switch ( L3_PktIn->Info[ 2 ] )
                  {
                  case CC_INFO_REQ:
                     /* Change msg_type.
                     */
                     L3_PktIn->Info[ 2 ] = INFO;
                     break;
                  default:
                     /* Don't know anything else to be expected.
                     */
                     break;
                  }
               break;
            default:
               break;
            }
		/* === end of added code */
         break;
      default:
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error ("l3", "L3_NullCref", "Unknown message source.");
#endif
         goto bailout;
      }

   LIF_SendPkt( L2P_LAPD, L3_ID, 0, dest_id, event, L3_DSL_Id, Q931_SAPI,
                ces, 0, 0, Q931_DSL_CHAN_ID, TRUE, L3_PktIn, NOW,
                REGULAR_MAIL );
   L3_AvailPktIn = FALSE;

bailout:

   return;
}


/*********************
* L3_ProcessInternal *
**********************
*
* Process internal event(s).  MsgQ is ready to process in state 0;
* InfoQ is ready to process beyond state 1.
*
*/
PRIVATE void near L3_ProcessInternal ()

{
   int cr;

   if ( NLCB->State == NULL_STATE )
      {
      while ( NLCB->MsgQ.Count )
         {
         if ( L3_AvailPktIn )
            {
#ifdef DEBUG
            printf( "FREE Incoming Packet\n" );
#endif
            LIF_FreePkt( L3_PktIn );
            }
         L3_PktIn = L3_DeQueue( &NLCB->MsgQ );
         L3_AvailPktIn = TRUE;

         /* Unpack info field
         */
         L3_GetInfo( &NLCB->Event, L3_PktIn->Info, &L3_PktIn->InfoLen,
                     &NLCB->CallRefLen, &cr, CC_ID);
         if ( NLCB->Event == COMPLETED )
            goto bailout;
         L3_ProcessEvent();
         }
      }

   if (( NLCB->State > CALL_INIT ) && ( NLCB->State < DL_IDLE_STATE ))
      {
      while ( NLCB->InfoQ.Count )
         {
         if ( L3_AvailPktIn )
            {
#ifdef DEBUG
            printf( "FREE Incoming Packet\n" );
#endif
            LIF_FreePkt( L3_PktIn );
            }
         L3_PktIn = L3_DeQueue( &NLCB->InfoQ );
         L3_AvailPktIn = TRUE;

         /* Unpack info field
         */
         L3_GetInfo( &NLCB->Event, L3_PktIn->Info, &L3_PktIn->InfoLen,
                     &NLCB->CallRefLen, &cr, CC_ID);
         if ( NLCB->Event == COMPLETED )
            goto bailout;
         L3_ProcessEvent();
         }
      }

bailout:

   if (( NLCB->MsgQ.Count == 0 ) && ( NLCB->InfoQ.Count == 0 ))
      NLCB->Internal = FALSE;
}

/*************
* L3_PutInfo *
**************
*
* Put the goodies into a layer 3 message.
*   (call reference length, call reference and the message type)
*
*/
PUBLIC void L3_PutInfo (event, pkt, new_msg, network)

int     event;                                        /* event to pack */
PKT_ID  pkt;                                        /* outgoing packet */
BOOLEAN new_msg;            /* The message is entirely constucted here */
BOOLEAN network;                 /* Appear as the Network side (Cause) */

{
   BYTE *msg;                        /* layer 3 message part of packet */

   msg = pkt->Info;                               /* Save info pointer */

   switch ( SwitchType )
      {
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
         *msg++ = TRANSACTION_PD_1TR6;   /* put protocol discriminator */
         break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
         *msg++ = TRANSACTION_PD_1TR6;   /* put protocol discriminator */
         break;
#endif
      default:
         *msg++ = Qdot931;               /* put protocol discriminator */
         break;
      }

   *msg++ = NLCB->CallRefLen;                   /* put call ref length */

   switch (NLCB->CallRefLen)
      {
      case 0:                                                   /* FFS */
         /* Message header consists of Proto disc, cr_len, msg_type
         *  Move entire info buffer "back" by one byte
         */
         LIF_N_Cpy (msg, msg+1, pkt->InfoLen - 3);
         pkt->InfoLen--;
         break;

      case 1:
         /* Msg header consists of Proto disc, cr_len, cr, msg_type
         */
         *msg++ = (BYTE) NLCB->CallRef;
         break;

      case 2:
         /* Msg head consists of Proto dis, cr_len, cr(2), msg_type
         *  2 octets of call reference, most significant byte first
         */
         *msg++ = (BYTE) (NLCB->CallRef >> 8);
         *msg++ = (BYTE) NLCB->CallRef;
         break;

      default:                         /* Proto disc, cr_len, msg_type */
         break;
      }

   /* Escape to specific message coding?
   */
   if (( SwitchType == BRI_5ESS_STYPE ) && ( (event & 0xff00) == SS_MASK ))
      *msg++ = 0;

   *msg++ = event;                                 /* put message type */

   /* A few locally generated info elements may be appended to the msg.
   */

   if ( NLCB->Cause != 0 )
      {
      if (detail_debug)
        buginf("\n  L3_PutInfo the cause is %x", NLCB->Cause);
      msg += CCIE_BuildCause_IE( msg, NLCB->Cause, network, Diagnostic );
      NLCB->Cause = 0;
      }
   Diagnostic[ 0 ] = Diagnostic[ 1 ] = Diagnostic[ 2 ] = 0;

   if (( event == STATUS ) && ( new_msg ))
      msg += CCIE_BuildCallState_IE( msg, L3_EXT_STATE(NLCB->State) );
	
   /* The above values may have been inserted at the beginning of a much
   *  longer message in which the info elements following any added here
   *  are assumed to be valid and thus the info length may not require
   *  correction.
   */
   if ( pkt->Info + pkt->InfoLen < msg )
      pkt->InfoLen = msg - pkt->Info;

	/*
	 *	Release Completes and Connects need to carry TermCap.  We
	 *	appear to be overwriting some IEs in some cases if we do this
	 *	above the length test just above us, and the comment indicates
	 *	that the s/w may rely on this.  So we will tack onto the end. 
	 */
	if((SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) &&
			(event == RELEASE_COMP || event == CONNECT || event == STATUS)) {
		pkt->InfoLen += CCIE_BuildCodesetShift_IE( &pkt->Info[pkt->InfoLen],
				CODESET_6, FALSE );
		pkt->InfoLen += CCIE_BuildTermCap_IE( &pkt->Info[pkt->InfoLen],
				CEPT_NATIONAL_STANDARD,
				NATIONAL_FUNCTION );
	}

}

/*************
* L3_Restart *
**************
*
#ifdef PRI
* For PRI, simply relay the RESTART and RESTART_ACK messages to Call
* Control as CC_RSTART_IND and CC_RESTART_CONF.
#else
* Process the RESTART and the RESTART_ACK messages from the peer.
* The RESTART message is an unsolicited RESTART order from the peer.
* The RESTART_ACK is a solicited acknowledgment of a restart performed
* by the peer at the request of this side of the interface.  In this case,
* the local restart action is deferred until the peer has acknowledged
* completion of the restart.
*
* Dispatch the restart request first to consider the RESTART_ALL case
* and then to the individual channel restart case.
*
* If restart was performed in one of these contexts, relay the event to
* Call Control (CC_RESTART_IND) and to the peer (RESTART_ACK) if driven
* by the RESTART message.
*
* If restart was performed in one of these contexts, relay the event to
* Call Control only (CC_RESTART_CONF) if driven by the RESTART_ACK message.
*
* RESTART or RESTART_ACK will cause a reset of one or more channels at L3
* (the NLCB associated with the restarted channel is returned to the
* NULL_STATE) and any active call(s) using the restarted channel(s) is
* thus terminated.  Call Control is expected to perform a parallel action.
#endif
*
*/
PRIVATE void near L3_Restart( cr, cr_len, event, ces, packet )

int    cr;                                           /* Call reference */
BYTE   cr_len;                                            /* CR length */
int    event;                         /* Either RESTART or RESTART_ACK */
BYTE   ces;                              /* Connection endpoint suffix */
PKT_ID packet;                                          /* Data packet */
{
   if (SwitchType == PRI_4ESS_STYPE || SwitchType == PRI_5ESS_STYPE ||
       SwitchType == PRI_NET5_STYPE || SwitchType == PRI_NTT_STYPE ||
       SwitchType == PRI_DMS100_STYPE || SwitchType == PRI_TS014_STYPE) {

       if (GLOBAL_CALL_REF(cr, cr_len)) {
          /* Prepare and send message to CC.
           */
          CCIE_Change_Msg_Type(packet->Info,
                            (event == RESTART) ? CC_RESTART_IND :
                                                 CC_RESTART_CONF);
          LIF_SendPkt(L2P_LAPD, L3_ID, 0, CC_ID, NL_DATA_IND, L3_DSL_Id,
                   Q931_SAPI, ces, 0, 0, Q931_DSL_CHAN_ID, TRUE, packet,
                   NOW, REGULAR_MAIL);
          L3_AvailPktIn = FALSE;         /* The packet has been mailed. */
       }
   } else {

   PKT_ID   extra_pkt;
   BYTE    *chan_id_ie;
   BYTE    *restart_ind_ie;
   BOOLEAN  error;

   if (GLOBAL_CALL_REF(cr, cr_len )) {
      /* Look for the CHANNEL_ID_IE
      */
      chan_id_ie = CCIE_Get_IE( packet->Info, packet->InfoLen,
                                CHANNEL_ID_IE, CODESET_0, 1 );

      /* Look for the RESTART_IND_IE
      */
      restart_ind_ie = CCIE_Get_IE( packet->Info, packet->InfoLen,
                                    RESTART_IND_IE, CODESET_0, 1 );

      /* Note that the following construct relies upon the TRUE evaluation
      *  of the first expression (the call to L3_RestartAll) to prevent
      *  a call to L3_RestartGrp.  If L3_RestartAll is successfully
      *  performed, L3_RestartGrp should not be entered.
      */
      if (( L3_RestartAll(event, ces, restart_ind_ie, chan_id_ie, &error)) ||
          ( L3_RestartGrp(event, ces, chan_id_ie, error)))
         {
         /* Inform layer 2 or CC - note that we must use LIF mail call
         *  because we do not have an NLCB (L3_Mail call requires an NLCB
         *  available).  Also we must have restarted something at this point
         *  or we would have bailed out.
         */
         if ( event == RESTART )             /* Request from the peer. */
            {
            /* Get a new packet to send the indication to CC.
            */
            extra_pkt = LIF_GetPkt( packet->InfoType, packet->HeadLen, FALSE );
#ifdef FFS_CHECK_ERRORS
            if (extra_pkt == NULL)
               LIF_Fatal("l3", "L3_Restart", "LIF_GetPkt failed");
#endif
            /* Prepare and send message to CC.
            */
            LIF_CpyPkt( extra_pkt, packet );
            CCIE_Change_Msg_Type( extra_pkt->Info, CC_RESTART_IND );
            LIF_SendPkt( L2P_LAPD, L3_ID, 0, CC_ID, NL_DATA_IND, L3_DSL_Id,
                         Q931_SAPI, ces, 0, 0, Q931_DSL_CHAN_ID, TRUE,
                         extra_pkt, NOW, REGULAR_MAIL );

            /* Use the incoming packet to respond to the peer
            */
            CCIE_Change_Msg_Type( packet->Info, RESTART_ACK );
#ifdef BRI_DMS100
            if ((( SwitchType == BRI_DMS100_STYPE ) ||
                    ( SwitchType == BRI_NI1_STYPE )) &&     /*===*/
                ( (restart_ind_ie[ 2 ] & 0x07) == RESTART_ALL ))
               /* If we restart all and if a CHANNEL_ID_IE was present
               *  in the RESTART message, we will remove it.
               */
               packet->InfoLen -= CCIE_Remove_IE( packet->Info,
                                                  packet->InfoLen,
                                                  CHANNEL_ID_IE,
                                                  CODESET_0,
                                                  1 );
#endif
            LIF_SendPkt( L2P_LAPD, L3_ID, 0, L2_ID, DL_DATA_REQ, L3_DSL_Id,
                         Q931_SAPI, ces, 0, 0, Q931_DSL_CHAN_ID, TRUE,
                         packet, NOW, REGULAR_MAIL );
            }

         else if ( event == RESTART_ACK )
            {
            /* Confirm restart to CC.
            */
            CCIE_Change_Msg_Type( packet->Info, CC_RESTART_CONF );
            LIF_SendPkt( L2P_LAPD, L3_ID, 0, CC_ID, NL_DATA_IND, L3_DSL_Id,
                         Q931_SAPI, ces, 0, 0, Q931_DSL_CHAN_ID, TRUE,
                         packet, NOW, REGULAR_MAIL );
            }
         L3_AvailPktIn = FALSE;         /* The packet has been mailed. */
         }
      } else {
#ifdef USER
          L3_RestartCrefErr(ces, cr, cr_len, event);
#endif
      }
   }
   return;
}

#ifdef BRI
/****************
* L3_RestartAll *
*****************
*
* Restart all B-Channels if appropriate.
*
* Test the RESTART_IND_IE and the CHANNEL_ID_IE on a SwitchType basis to
* determine if RESTART_ALL is appropriate and perform this action if so.
* If RESTART_ALL is performed, restart_performed is returned TRUE, if not,
* do nothing.  If message errors are found, return an error result as
* an implicit return value.
*
*/
PRIVATE BOOLEAN near L3_RestartAll( event, ces, restart_ind_ie, chan_id_ie,
                                    error )

int      event;                       /* Either RESTART or RESTART_ACK */
int      ces;                            /* Connection endpoint suffix */
BYTE    *restart_ind_ie;                     /* Restart indicator I.E. */
BYTE    *chan_id_ie;                        /* Channel ID info element */
BOOLEAN *error;                                    /* To be determined */

{
   int     channel_selection = 0;
   BYTE    restart_class;
   NLCB_ID next_nlcb;
   BOOLEAN restart_performed = FALSE;


   *error = TRUE;                           /* Skeptically initialize. */

   if ( restart_ind_ie == NULL )
      goto bailout;    /* Missing a mandatory element is an error case */
   restart_class = restart_ind_ie[ 2 ] & 0x07;   /* Read restart class */

   /* Check validity of the restart class, in the context of the channel id,
   *  on a SwitchType basis, to determine the class of restart.
   */
   switch ( SwitchType )
      {
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
         if ( restart_class == RESTART_ALL )
            channel_selection = ALL_CHANNELS;

         else if ( restart_class != RESTART_SINGLE )
            goto bailout;          /* This is an element coding error. */

         else if ( chan_id_ie == NULL )
            /* restart_class = RESTART_SINGLE.  The channel is to be
            *  indicated through the CHANNEL_ID_IE, which is missing.
            *  Its absence is a fatal error.
            */
            goto bailout;
         break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:                       /*===*/
         if ( restart_class == RESTART_ALL )
            channel_selection = ALL_CHANNELS;

         else if ( restart_class != RESTART_SINGLE )
            goto bailout; /* Coding error.  There are but two choices. */

         else if ( chan_id_ie == NULL )
            /* restart_class = RESTART_SINGLE.  The channel is to be
            *  indicated through the CHANNEL_ID_IE, which is missing.
            *  Its absence is a fatal error.
            */
            goto bailout;
         break;
#endif
      default:
         if (( restart_class == RESTART_ALL ) ||
            ( restart_class == RESTART_SINGLE_INTERFACE ))
            channel_selection = ALL_CHANNELS;

         else if ( restart_class != RESTART_SINGLE )
            goto bailout;          /* This is an element coding error. */

         else if ( chan_id_ie == NULL )
            /* restart_class = RESTART_SINGLE.  The channel is to be
            *  indicated through the CHANNEL_ID_IE, which is missing.
            *  Its absence is a fatal error.
            */
            goto bailout;
         break;
      }

   if ( channel_selection == ALL_CHANNELS )
      {
#ifdef CHANNEL_TABLE
      /* Test the channel state for correctness.  In some Call Control
      *  implementations, the channel state in the Channel Management Table
      *  may be used to indicate a locally-initiated RESTART request.  This
      *  state is checked here, conditionally, for sanity, when RESTART_ACK
      *  has been received from the peer, but before the channel is actually
      *  restarted locally (Note that Channel 0's state is consulted as the
      *  "global channel" for restart all.
      */
      if (( event == RESTART )      ||
         (( event == RESTART_ACK ) &&
         ( CC_CHAN_GetStatebri( 0, B_CHANNEL_UNITS, L3_DSL_Id )
              == CHANNEL_RESTART_PEND )))
#else
      /* If the CC_CHAN.C routine is not available then another channel
      *  state access method is required to test if all channel restart
      *  was initiated by this side of the interface.
      */
      if (( event == RESTART ) || ( event == RESTART_ACK ))
#endif
         {
         /* Restart ALL !!
         *  Re-initialize the call ref selector.  This proves useful in
         *  testing as a "Restart All" guarantees restoration of call ref
         *  to a known starting point.
         */
         L3_CallRef = 1;
         /* Release all the calls associated with the interface.
         */
         NLCB = NLCB_Head[ L3_DSL_Id ];
         while ( NLCB )
            {
            next_nlcb = NLCB->Next;
            if ( NLCB->Ces == ces )
               L3_CallRelease();
            NLCB = next_nlcb;
            }
         *error = FALSE;
         restart_performed = TRUE;
         }
      }
   else
      {
      *error = FALSE;    /* Probably a valid case, but not RESTART_ALL */
      }

bailout:

   return ( restart_performed );
}


/********************
* L3_RestartChannel *
*********************
*
* Verify the validity of the channel number passed to this function.
* If valid, locate and free any NLCB bearing a B-Channel identifier matching
* the channel number passed to this function.
*
*/
PRIVATE void near L3_RestartChannel( channel, ces, restart_performed )

int      channel;                              /* B-Channel to restart */
int      ces;                            /* Connection endpoint suffix */
BOOLEAN *restart_performed;

{
   NLCB_ID nlcb;
   NLCB_ID tmp_nlcb;

   if ( VALID_CHANNELP( channel ))
      {
      *restart_performed = TRUE;
      nlcb = NLCB_Head[L3_DSL_Id];
      while ( nlcb )
         {
         tmp_nlcb = nlcb->Next;
         if (( nlcb->ChanId == channel ) && ( nlcb->Ces == ces ))
            {
            NLCB = nlcb;
            L3_CallRelease();
            }
         nlcb = tmp_nlcb;
         }
      }
}

#endif                                                          /* BRI */
#ifdef USER
/********************
* L3_RestartCrefErr *
*********************
*
* This function is provided to handle special error cases of RESTART
* and RESTART_ACK messages received with other than GLOBAL_CALL_REF.
*
*/
PRIVATE void near L3_RestartCrefErr( ces, cr, cr_len, event )

BYTE ces;                                /* Connection endpoint suffix */
int  cr;                                             /* Call reference */
BYTE cr_len;                          /* Byte length of call reference */
int  event;                              /* The event that has occured */

{
   BOOLEAN ignore_msg;                /* Ignore or process the message */


   switch ( SwitchType )
      {
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:                   /*===*/
         NLCB = L3_GetUser_NLCB(ces, 0, cr, cr_len, event, L2_ID, &ignore_msg);
         if ( NLCB && ( !ignore_msg ))
            {
            switch( NLCB->State )
               {
               case CALL_INIT:                              /* State 1 */
               case CALL_PRESENT:                           /* State 6 */
                  NLCB->Cause = NONEXISTENT_MSG;           /* Cause 97 */
                  Diagnostic[ 0 ] = event;
                  Diagnostic[ 1 ] = 0;
                  L3_Mail (RELEASE_COMP, L2_ID, TRUE, FALSE);
                  L3_CallRelease ();                        /* State 0 */
                  break;
 
               case OUT_CALL_PROC:                          /* State 3 */
               case CALL_DELIVERED:                         /* State 4 */
               case CALL_RECEIVED:                          /* State 7 */
               case CONNECT_REQUEST:                        /* State 8 */
               case IN_CALL_PROC:                           /* State 9 */
               case CALL_ACTIVE:                           /* State 10 */
                  NLCB->Cause = NONEXISTENT_MSG;           /* Cause 97 */
                  Diagnostic[ 0 ] = event;
                  Diagnostic[ 1 ] = 0;
                  L3_Mail( STATUS, L2_ID, TRUE, FALSE );
                  break;
 
               case OVERLAP_SENDING:                        /* State 2 */
                  /* If we support overlap_sending, then we need to send
                  *  DISC here through call control.
                  */
                  L3_Mail( CC_ERROR_IND, CC_ID, TRUE, FALSE );
                  break;
 
               default:                    /* Any other states, ignore */
                  break;
               }
            }
         break;
#endif                                               /* End BRI_DMS100 */
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
         /* For DMS100 we will run through the get NLCB machinery.  If this
         *  is a bad CREF, the function Ux_UnknownCallRef will be invoked.
         *  If this is the CREF of some current call, we will begin tearing
         *  it down here, as specified.
         */
         NLCB = L3_GetUser_NLCB(ces, 0, cr, cr_len, event, L2_ID, &ignore_msg);
         if ( NLCB && ( !ignore_msg ) && ( NLCB->State != RELEASE_REQUEST ))
            {
            /* This is functionally the same as Ux_BadMsg().
            */
            L3_StopAllTimers();
            NLCB->Cause = WRONG_MESSAGE;                   /* Cause 98 */
            L3_Mail( RELEASE, L2_ID, TRUE, FALSE );
            L3_SavePkt( &NLCB->Release, L3_PktOut );   /* Save RELEASE */
            LIF_StartTimer( NLCB->Timer_T308 );
            NLCB->State = RELEASE_REQUEST;                 /* State 19 */
            }
         break;                                                 /* NOP */
#endif                                               /* End PRI_DMS100 */
#ifdef PRI_4ESS
      case PRI_4ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_NTT_STYPE:
      case PRI_TS014_STYPE:
         /* For PRI_4ESS we will run through the get NLCB machinery.
         *  If this is a bad CREF, the function Ux_UnknownCallRef will be
         *  invoked.  If this is the CREF of some current call, we will
         *  handle the RESTART or RESTART_ACK as a bad msg for the state.
         */
         NLCB = L3_GetUser_NLCB(ces, 0, cr, cr_len, event, L2_ID, &ignore_msg);
         if ( NLCB && ( !ignore_msg ))
            {
            NLCB->Event = event;
            L3_ProcessEvent();
            }
         break;
#endif
#ifdef PRI_5ESS
      case PRI_5ESS_STYPE:
         /* For PRI_5ESS we will run through the get NLCB machinery.
         *  If this is a bad CREF, the function Ux_UnknownCallRef will be
         *  invoked.  If this is the CREF of some current call, we will
         *  handle the RESTART or RESTART_ACK as a bad msg for the state.
         */
         NLCB = L3_GetUser_NLCB(ces, 0, cr, cr_len, event, L2_ID, &ignore_msg);
         if ( NLCB && ( !ignore_msg ))
            {
            NLCB->Event = event;
            L3_ProcessEvent();
            }
         break;

#endif
      default:
         Trashcan = ces;
         Trashcan = cr;
         Trashcan = cr_len;
         Trashcan = event;
         break;                                                 /* NOP */
      }
}

#endif                                                         /* USER */
#ifdef BRI
/****************
* L3_RestartGrp *
*****************
*
* Test for the restart of a group of B-channels (quite possibly a
* one-channel group).  Cause restart to be performed if so and return
* restart_performed = TRUE.
*
*/
PRIVATE BOOLEAN near L3_RestartGrp( event, ces, chan_id_ie, previous_error )

int      event;                       /* Either RESTART or RESTART_ACK */
int      ces;                            /* Connection endpoint suffix */
BYTE    *chan_id_ie;                        /* Channel ID info element */
BOOLEAN  previous_error;                             /* Already failed */

{
   int      channel_selection;
   BYTE    *expanded_chan;
   BOOLEAN  restart_performed = FALSE;


   if (( previous_error ) || ( !chan_id_ie ))
      goto bailout;                                  /* Don't progress */

   expanded_chan = CCIE_ExpandChanId_IE( chan_id_ie, &Trashcan );
   channel_selection = expanded_chan[ 4 ];
   /* Check validity of Channel ID info element received.
   */
   if (( expanded_chan[ 1 ] != BASIC_RATE_INTERFACE ) ||
       ( expanded_chan[ 0 ] != 0 ) ||         /* Test interface ID bit */
       ( expanded_chan[ 3 ] != 0 ))        /* Test D-channel indicator */
      goto bailout;                       /* Element incorrectly coded */

#ifdef CHANNEL_TABLE
   /* Test the channel state for correctness.  In some Call Control
   *  implementations, the channel state in the Channel Management Table
   *  may be used to indicate a locally-initiated RESTART request.  This
   *  state is checked here, conditionally, for sanity, when RESTART_ACK
   *  has been received from the peer, but before the channel is actually
   *  restarted locally.
   */
   if (( event == RESTART )      ||
       (( event == RESTART_ACK )    &&
        ( CC_CHAN_GetStatebri( channel_selection, B_CHANNEL_UNITS, L3_DSL_Id )
             == CHANNEL_RESTART_PEND )))
#else
   /* If the CC_CHAN.C routine is not available then another channel
   *  state access method is desirable to test if individual channel
   *  restart was initiated by this side of the interface.
   */
   if (( event == RESTART ) || ( event == RESTART_ACK ))
#endif
      L3_RestartChannel( channel_selection, ces, &restart_performed );

bailout:
   return ( restart_performed );
}

#endif                                                          /* BRI */

#ifdef 0  /* not currently used */
/********************
* L3_RestartIllegal *
*********************
*
* Handle response to illegal RESTART and RESTART_ACK messages.
*
*/
PRIVATE void near L3_RestartIllegal( cr, cr_len, ces, event )

int  cr;                                             /* Call reference */
BYTE cr_len;                                  /* Call reference length */
BYTE ces;                                /* Connection endpoint suffix */
int  event;                           /* Either RESTART or RESTART_ACK */
{
   /* We need an NLCB to use L3_Mail services.
   */
   NLCB =  L3_GetNew_NLCB( ces, cr, 0 );
   if ( NLCB )
      {
      NLCB->CallRef    = cr;
      NLCB->CallRefLen = cr_len;
      NLCB->State      = NULL_STATE;
      switch ( SwitchType )
         {
#ifdef KDD_NTT
         case KDD_STYPE:
            NLCB->Cause      = NONEXISTENT_MSG;            /* Cause 97 */
            break;
#endif
         default:
            break;                             /* should never happen */
         }
         Diagnostic[ 0 ]  = event;
         Diagnostic[ 1 ]  = 0;		/* === */
         L3_Mail( STATUS, L2_ID, TRUE, FALSE );
         L3_Free_NLCB( NLCB );
         NLCB = NULL;
      }
}
#endif

#ifdef PRI
/*******************
* L3_RestartAckReq *
********************
*
* Call control restart acknowledgement request.
*
* Have any associated calls cleared.
* Relay the message to the ISDN peer as RESTART_ACK.
*
*/
PRIVATE void near L3_RestartAckReq( ces, packet )

BYTE   ces;                              /* Connection endpoint suffix */
PKT_ID packet;                                          /* Data packet */
{
   L3_RestartCalls( ces, packet );
   /* Prepare and send message to L2.
   */
   CCIE_Change_Msg_Type( packet->Info, RESTART_ACK );
   LIF_SendPkt( L2P_LAPD, L3_ID, 0, L2_ID, DL_DATA_REQ, L3_DSL_Id, Q931_SAPI,
                ces, 0, 0, Q931_DSL_CHAN_ID, TRUE, packet, NOW, REGULAR_MAIL );
   L3_AvailPktIn = FALSE;
}

/******************
* L3_RestartCalls *
*******************
*
* Remove NLCB context of restarted calls.
*
* Peel off the call_id's of the affected calls from the message from CC.
* Release any call with a matching call_id.
* "Fix" the message info length.
*
*/
PRIVATE void near L3_RestartCalls( ces, pkt )

BYTE   ces;                              /* Connection endpoint suffix */
PKT_ID pkt;                                             /* Data packet */
{
   BYTE    *access_ptr;
   int      calls, call_id;
   BOOLEAN  do_all;
   NLCB_ID  tmp_nlcb;


   if (( access_ptr = CCIE_Get_IE( pkt->Info, pkt->InfoLen,
                                   RESTART_IND_IE, CODESET_0, 1 )) != NULL )
      {
      access_ptr++;                              /* Point to IE length */
      access_ptr += *access_ptr + 1;/* Point one passed RESTART_IND_IE */
      do_all = *access_ptr++;                          /* Restart all? */
      calls  = *access_ptr++;              /* Number of affected calls */
      /* Decrease the info len by 2 (do_all octet and calls octet) and
      *  (two octets per call since each call_id occuppies two octets,
      *  so that call_id data is not passed across the interface.
      */
      pkt->InfoLen -= (( calls << 1 ) + 2 );
      if ( do_all )
         {
         /* Restart ALL !!
         *  Re-initialize the call ref selector.  This proves useful in
         *  testing as a "Restart All" guarantees restoration of call ref
         *  to a known starting point.
         */
         L3_CallRef = 1;
         /* Release all the calls associated with the interface.
         */
         NLCB = NLCB_Head[ L3_DSL_Id ];
         while ( NLCB )
            {
            tmp_nlcb = NLCB->Next;
            if ( NLCB->Ces == ces )
               L3_CallRelease();
            NLCB = tmp_nlcb;
            }
         }
      else
         {
         while ( calls-- )
            {
            call_id = ( *access_ptr++ << 8 );
            call_id += *access_ptr++;
            NLCB = NLCB_Head[ L3_DSL_Id ];
            while ( NLCB && ( NLCB->CallId != call_id ))
               NLCB = NLCB->Next;
            if ( NLCB )                                 /* Match found */
               {
               L3_CallRelease();
               }
            }
         }
      }
}

#endif                                                          /* PRI */

/****************
* L3_RestartReq *
*****************
*
* Call control restart request.
*
#ifdef PRI
* Have any associated calls cleared.
#endif
* Relay the message to the ISDN peer as RESTART.
*
*/
PRIVATE void near L3_RestartReq( ces, call_id, packet )

BYTE   ces;                              /* Connection endpoint suffix */
int    call_id;                                             /* Call ID */
PKT_ID packet;                                          /* Data packet */
{

#ifdef PRI
   L3_RestartCalls( ces, packet );
#endif
   /* Prepare and send message to L2.
   */
   CCIE_Change_Msg_Type( packet->Info, RESTART );
   LIF_SendPkt( L2P_LAPD, L3_ID, 0, L2_ID, DL_DATA_REQ, L3_DSL_Id, Q931_SAPI,
                ces, 0, call_id, Q931_DSL_CHAN_ID, TRUE, packet, NOW,
                REGULAR_MAIL );
   L3_AvailPktIn = FALSE;
}

/******************
* L3_Restart_nlcb *
*******************
*
* Restart the NLCBs for an interface.
*/
PUBLIC void L3_Restart_nlcb (dsl)
int  dsl;
{
   NLCB_ID tmp_nlcb;

   /* Restart ALL !!
    *  Re-initialize the call ref selector.  This proves useful in
    *  testing as a "Restart All" guarantees restoration of call ref
    *  to a known starting point.
    */
    L3_CallRef = 1;
    /*
     * Release all the calls associated with the interface.
     */
     NLCB = NLCB_Head[dsl];
     while (NLCB) {
         tmp_nlcb = NLCB->Next;
         L3_CallRelease();
         NLCB = tmp_nlcb;
     }
}

/*************
* L3_SavePkt *
**************
*
* Save a packet in the designated packet.
*
*/
PUBLIC void L3_SavePkt (d, s)

PKT_ID *d;                                 /* save in this destination */
PKT_ID s;                                          /* save this source */
{
   if ( *d )
      LIF_FreePkt( *d );                             /* get rid of old */
   *d = LIF_GetPkt( s->InfoType, s->HeadLen, TRUE );        /* get new */
#ifdef FFS_CHECK_ERRORS
   if ( *d == NULL )
      LIF_Fatal( "l3", "L3_SavePkt", "LIF_GetPkt failed" );
#endif
   LIF_CpyPkt ( *d, s );                               /* copy to save */
}

/*********************
* L3_Search_NLCB_Ref *
**********************
*
* Search NLCB List for match of call reference
*
* Return: Address of desired NLCB, if successful; NULL, otherwise.
*
*/
PUBLIC NLCB_ID L3_Search_NLCB_Ref (cref, cref_len)

int cref;                                     /* call reference number */
int cref_len;                                 /* call reference length */
{
   NLCB_ID nlcb;

   nlcb = NLCB_Head[ L3_DSL_Id ];

   while ( nlcb &&
         (( !CR_MATCH(nlcb->CallRef, nlcb->CallRefLen, cref, cref_len )) ||
          ( nlcb->Ces == BROADCAST_CES )))
      nlcb = nlcb->Next;

   return( nlcb );
}

/***************
* L3_Shut_Down *
****************
*
* Shutdown the Network Link Layer
*
*/
PUBLIC void L3_ShutDown (dsl)
uchar dsl;
{
	NLCB_ID nlcb;
	NLCB_ID tmp_nlcb;

	if (isdn_debug)
	    buginf("\nISDN %s: Shutting down ISDN Layer 3", DSL_TO_NAME(dsl));
	if (dsl == ALL_ISDN_INTERFACES) {
            /* 
             * Free ALL Network Links
             */
            for (L3_DSL_Id=0; L3_DSL_Id < MAX_DSL; L3_DSL_Id++) {
                while (NLCB_Head[L3_DSL_Id])
                    L3_Free_NLCB(NLCB_Head[L3_DSL_Id]);
            }
            /* 
             * Inform Call Control.
             */
            LIF_SendPkt(L2P_LAPD, L3_ID, 0, CC_ID, MNL_RESET_REQ, 0, Q931_SAPI, 
                        0, 0, 0, Q931_DSL_CHAN_ID, TRUE, NULL, NOW, REGULAR_MAIL);
	} else {
            L3_DSL_Id = dsl;
            nlcb = NLCB_Head[dsl];
            while (nlcb) {
                /* 
                 * We don't free the NLCB for broadcast ces. It will be used
                 * once the interface is up again.
                 */
                if (nlcb->Ces == BROADCAST_CES)
                    nlcb = nlcb->Next;
                else {
                    tmp_nlcb = nlcb->Next;
                    L3_Free_NLCB(nlcb);
                    nlcb = tmp_nlcb;
                }
            }
            LIF_SendPkt(L2P_LAPD, L3_ID, 0, CC_ID, MNL_RESET_REQ, L3_DSL_Id, 
                        Q931_SAPI, 0, 0, 0, Q931_DSL_CHAN_ID, TRUE, NULL,
                        NOW, REGULAR_MAIL);
        }
}

/*******************
* L3_StopAllTimers *
********************
*
* Stop all timers potentially running for a call.
*
*/
PUBLIC void L3_StopAllTimers ()

{
   /*
   * Timers T313, T318 & T319 are USER side only.
#ifdef VN2
   * Timer T310 is supported for user VN3
#endif
   * Timers T302, T304, T306, T309, T310, T312 & T322 are NETWORK side only.
   * Timers T303, T305 & T308 are common.
   */

#ifdef USER
   LIF_StopTimer (NLCB->Timer_T303);
   LIF_StopTimer (NLCB->Timer_T305);
   LIF_StopTimer (NLCB->Timer_T308);
#ifdef VN2
   LIF_StopTimer (NLCB->Timer_T310);
#endif
   LIF_StopTimer (NLCB->Timer_T313);
   LIF_StopTimer (NLCB->Timer_T318);
   LIF_StopTimer (NLCB->Timer_T319);

   NLCB->T303_First = TRUE;
   NLCB->T308_First = TRUE;
   NLCB->T313_First = TRUE;
#endif
}

#ifdef BRI
/*******************
* L3_UpdateChannel *
********************
*
* Check if a CHANNEL_ID information element is present in the incoming
* message.  If so, extract the B-channel number and write it to the
* NLCB, testing the validity of the info element in the process.
*
* This function is critical for the correct operation of RESTART since
* the current B-Channel in use is updated in the NLCB through its actions.
* Failure to maintain the correct B-Channel in the NLCB could cause the
* NLCB to be stranded after an individual channel RESTART is performed.
*
#ifdef BRI_DMS100
* For BRI_DMS100, because B-Channel selection is strongly controlled by the 
* Network, the CHANNEL_ID information element is only optional in the first 
* USER response to a SETUP (e.g. ALERTING).  However, the correct operation
* of Restart on the USER side at Layer 3 demands that the channel specified
* by the Network be returned in this information element in State 6
* messages by Call Controls using the Telenetworks L3.C.  Note that this
* requirement is relaxed for applications on BROADCAST data links where
* Restart is not used.
#endif
*/
PUBLIC void L3_UpdateChannel( pkt )

PKT_ID pkt;                         /* Holder for the message to check */

{
   BYTE *chan_ie;
   BYTE *expanded_chan;              /* Pointer to IE expansion buffer */
   int  channel;            /* Channel ID to which the message applies */


 if((SwitchType != PRI_5ESS_STYPE) && (SwitchType != PRI_DMS100_STYPE)
     && (SwitchType != PRI_4ESS_STYPE) &&  (SwitchType != PRI_NET5_STYPE)
     && (SwitchType != PRI_NTT_STYPE) && (SwitchType != PRI_TS014_STYPE)) {
   if (( pkt ) && ( pkt->Info ))
      {
      /* Go see if the CHANNEL_ID_IE is present in the message.
      */
      chan_ie = CCIE_Get_IE( pkt->Info, pkt->InfoLen, CHANNEL_ID_IE,
                             CODESET_0, 1 );
      if ( chan_ie )              /* Located a Channel ID info element */
         {
         expanded_chan = CCIE_ExpandChanId_IE( chan_ie, &Trashcan );
         channel = expanded_chan[ 4 ];
         /* Check validity of info elements received.
         */
         if (( expanded_chan[ 0 ] == 0 ) &&   /* Test interface ID bit */
            ( expanded_chan[ 1 ] == BASIC_RATE_INTERFACE ) &&/* Int type */
            ( expanded_chan[ 3 ] == 0 ) && /* Test D-channel indicator */
            ( VALID_CHANNELP( channel )))
            NLCB->ChanId = (int) channel;              /* Save channel */
         }
      }
  }
}


#endif                                                          /* BRI */
/******************
* L3_UpdateTimers *
*******************
*
* On re-allocation of an NLCB, change the timer timeout reporting
* parameters to agree with the identity of the new user and call.
*
* Note: The Call Reference (cr) is substituted here for the call_ID
*       as a parameter to LIF_UpdateTimer.  This value will be inserted
*       into a primitive maintained by LIF with which to mail the timeout
*       report back to L3.  Thus the cr will be substituted into the
*       primitive in place of the CallID.  This will cause the subsequent
*       NLCB recovery by L3 to perform the NLCB search as though the
*       timeout were an event coming "from below" as an alternate response
*       to whatever response is being awaited (typically) "from below."
*
*/
PUBLIC void L3_UpdateTimers( nlcb, cr, ces )

NLCB_ID nlcb;                           /* NLCB to have timers updated */
int     cr;                  /* Call Reference for the associated call */
BYTE    ces;                  /* Connect Endpoint Suffix for this NLCB */

{
   /*
   * Timers T313, T318 & T319 are USER side only.
#ifdef VN2
   * Timer T310 is for USER side VN2.
#endif
   * Timers T302, T304, T306, T309, T310, T312, & T322  are NETWORK side only.
   * Timers T303, T305 & T308 are common.
   */

#ifdef USER
   /* USER side only
   */
   LIF_UpdateTimer( nlcb->Timer_T313, L3_ID, L3_ID, T313_EXPIRY,
                    L3_DSL_Id, Q931_SAPI, ces, cr, 0, T313_TimeOut );
   LIF_UpdateTimer( nlcb->Timer_T318, L3_ID, L3_ID, T318_EXPIRY,
                    L3_DSL_Id, Q931_SAPI, ces, cr, 0, T318_TimeOut );
   LIF_UpdateTimer( nlcb->Timer_T319, L3_ID, L3_ID, T319_EXPIRY,
                    L3_DSL_Id, Q931_SAPI, ces, cr, 0, T319_TimeOut );
#ifdef VN2
   LIF_UpdateTimer( nlcb->Timer_T310, L3_ID, L3_ID, T310_EXPIRY,
                    L3_DSL_Id, Q931_SAPI, ces, cr, 0, T310_TimeOut );
#endif
#endif
   /* Timers common to USER and NETWORK
   */
   LIF_UpdateTimer( nlcb->Timer_T303, L3_ID, L3_ID, T303_EXPIRY,
                    L3_DSL_Id, Q931_SAPI, ces, cr, 0, T303_TimeOut );
   LIF_UpdateTimer( nlcb->Timer_T305, L3_ID, L3_ID, T305_EXPIRY,
                    L3_DSL_Id, Q931_SAPI, ces, cr, 0, T305_TimeOut );
   LIF_UpdateTimer( nlcb->Timer_T308, L3_ID, L3_ID, T308_EXPIRY,
                    L3_DSL_Id, Q931_SAPI, ces, cr, 0, T308_TimeOut );
}

/******************
* L3_ValidCallRef *
*******************
*
* Test the call reference provided for validity by verifying that
* the ISDN peer has delivered it with the MSB (flag bit) cleared.
* Note that as this function executes, the inversion of the flag bit
* has already occurred, so that the flag bit should be set here.
*
* This function assumes that the CREF_TEST call reference bit inversion
* compile flag has been properly defined.
*
* Caution: This function will not neccessarily return the correct result
*          unless it is known that the Call Reference being tested was
*          assigned by the Peer (e.g. we are in State 6 for USER side or
*          State 1 for the NETWORK side).
*          It is also assumed that the call reference length is 1 or 2.
*/
PUBLIC BOOLEAN L3_ValidCallRef( cr_len, cr )

BYTE cr_len;                         /* Call Reference length (1 or 2) */
int  cr;                                             /* Call Reference */
{

#ifndef CREF_TEST
   /* Network originated Call Reference flag bit value better be 1 or
   *  it is not valid.  Mask all but the flag bit.
   */
   cr &= ( cr_len == 1 ) ? 0x80 : 0x8000;
#else
   /* Prevent a compiler warning for unreferenced parameters and
   *  guarantee passage of the test.
   */
   cr = cr_len;
#endif

   return ( cr != 0 );
}

/****************
* L3_ValidState *
*****************
*
* Verify that the state parameter to this function is valid for the
* SwitchType.
*
*/
PUBLIC BOOLEAN L3_ValidState( state )

unsigned int state;
{
   BOOLEAN valid_state = FALSE;             /* Initialize the result */

   switch ( SwitchType )
      {
#ifdef BRI_NI1	/*===*/
      case BRI_NI1_STYPE:
         if ( state < sizeof( BRI_NI1_ValidStates ))
            valid_state = (BOOLEAN)BRI_NI1_ValidStates[ state ];
         break;
#endif
#ifdef PRI_4ESS
      case PRI_4ESS_STYPE:
         if ( state < sizeof( PRI_4ESS_ValidStates ))
            valid_state = PRI_4ESS_ValidStates[ state ];
         break;
#endif
#ifdef PRI_5ESS
      case PRI_5ESS_STYPE:
         if ( state < sizeof( PRI_5ESS_ValidStates ))
            valid_state = PRI_5ESS_ValidStates[ state ];
         break;
#endif

      case PRI_NET5_STYPE:
         if ( state < sizeof( PRI_NET5_ValidStates ))
            valid_state = PRI_NET5_ValidStates[ state ];
         break;

      case PRI_NTT_STYPE:
         if ( state < sizeof( PRI_NTT_ValidStates ))
            valid_state = PRI_NTT_ValidStates[ state ];
         break;

      case PRI_TS014_STYPE:
         if ( state < sizeof( PRI_TS014_ValidStates ))
            valid_state = PRI_TS014_ValidStates[ state ];
         break;

#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
         if ( state < sizeof( BRI_5ESS_ValidStates ))
            valid_state = BRI_5ESS_ValidStates[ state ];
         break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
         if ( state < sizeof( BRI_DMS100_ValidStates ))
            valid_state = BRI_DMS100_ValidStates[ state ];
         break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
         if ( state < sizeof( PRI_DMS100_ValidStates ))
            valid_state = PRI_DMS100_ValidStates[ state ];
         break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
         if ( state < sizeof( VN2_ValidStates ))
            valid_state = VN2_ValidStates[ state ];
         break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
         if ( state < sizeof( KDD_NTT_ValidStates ))
            valid_state = KDD_NTT_ValidStates[ state ];
         break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
         if ( state < sizeof( BRI_1TR6_ValidStates ))
            valid_state = BRI_1TR6_ValidStates[ state ];
         break;
#endif
#ifdef BRI_NET3		/*===*/
      case BRI_NET3_STYPE:
         if ( state < sizeof( NET3_ValidStates ))
            valid_state = NET3_ValidStates[ state ];
         break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
         if ( state < sizeof( TS013_ValidStates ))
            valid_state = TS013_ValidStates[ state ];
         break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
         if ( state < sizeof( PRI_1TR6_ValidStates ))
            valid_state = PRI_1TR6_ValidStates[ state ];
         break;
#endif
#ifdef CCITT
      case CCITT_STYPE:
         if ( state < sizeof( CCITT_ValidStates ))
            valid_state = CCITT_ValidStates[ state ];
         break;
#endif
      default:
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error( "l3", "L3_ValidState", "Bad switch type" );
#endif
         break;
      }

   if ( !valid_state )
      {
#ifdef DISPLAY_LOCAL_ERR
#ifdef FFS_CHECK_ERRORS
      LIF_Error( "l3", "L3_ValidState", "Bad state" );
#endif
#endif
      }
   return ( valid_state );
}

#ifdef BRI_5ESS
/*********************
* L3_XlateNLCB_State *
**********************
*
* The public state for ASAI and AT&T 5ESS BRI Supp Services does not 
* align with the NLCB representation.  This function is intended to
* make the translation from nlcb state to external state for a few
* special case states.
*
*/
PUBLIC L3_STATE L3_XlateNLCB_State( nlcb_state )

L3_STATE nlcb_state;
{
   L3_STATE ext_state;

   ext_state = nlcb_state;                          /* Probable result */

   if ( SwitchType == BRI_5ESS_STYPE )
      {
      }

   return ( ext_state );
}
#endif                                                     /* BRI_5ESS */

/*
 * Routine to determine if National ISDN switches have to start a twait timer,
 * cannot actively activate interfaces within a range of 1 second to 5 minutes
 * of a reload. It may be configured in the router to NOT do the twait, this
 * is allowed on a per interface basis, but the default must be twait enabled.
 */
PUBLIC int L3_start_twait_timer (int dsl)
{
    sys_timestamp now;
    ulonglong timeup;
    long random;

    random = 0;
    if (!isdn_sys_constants[dsl].twait_disabled) {
        GET_TIMESTAMP(now);
        timeup = TS_VALUE(now);
        if (timeup <= 1*ONEMIN) {
            /*
             * Returns a random number in the range of 1 to 10000, times 30
             * gives a range of 0-300 seconds
             */
            random = random_gen() * 30;
        }
    }
    return (random);
}
/* End of module. */
