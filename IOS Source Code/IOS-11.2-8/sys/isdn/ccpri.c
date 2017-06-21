/* $Id: ccpri.c,v 3.7.4.8 1996/09/13 04:54:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccpri.c,v $
 *------------------------------------------------------------------
 * ccpri.c ISDN Primary Rate Interface Call Control Task
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccpri.c,v $
 * Revision 3.7.4.8  1996/09/13  04:54:43  thille
 * CSCdi68927:  Clean up some things that should be const in ISDN
 * Branch: California_branch
 * Make some more stuff const to save ram.  Also make some more stuff
 * static.
 *
 * Revision 3.7.4.7  1996/09/10  22:14:12  rbeach
 * CSCdi68630:  PRI NET5 homologation failure accepting 384 kbit/s BC.
 * Reject the bearer cap with Incompatible Destination.
 * Branch: California_branch
 *
 * Revision 3.7.4.6  1996/06/22  17:31:20  hrobison
 * CSCdi51925:  Spurious Accesses; RSP7000 + MIP(PRI)
 * Branch: California_branch
 * When a call is rejected, as for example in the case
 * when no channel is available, the CCB will be null.
 * Handle outgoing call appropriately for this case
 *
 * Revision 3.7.4.5  1996/06/10  20:49:22  dclare
 * CSCdi59816:  ISDN PRI 5ESS switch may deliver invalid IE in incoming
 * call. Allow an unknown numbering type and unknown numbering plan
 * for the Called Party IE.
 * Branch: California_branch
 *
 * Revision 3.7.4.4  1996/05/01  14:51:15  hrobison
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
 * Revision 3.7.4.3  1996/04/30  23:07:12  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.7.4.2  1996/04/10  17:58:18  hrobison
 * CSCdi52503:  LIF_Fatal displays garbage with ISDN process exits
 * Branch: California_branch
 *
 * Revision 3.7.4.1  1996/03/21  22:51:38  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.4  1996/03/18  23:32:21  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/02/16  03:42:05  mbeesley
 * CSCdi49179:  ISDN subsystem does not compile on Predator
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/08  08:07:20  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/11  18:54:29  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.7  1996/03/08  23:44:46  smackie
 * Fix various signed/unsigned comparison conflicts that cause the
 * 95q4 toolchain to blow it's cookies. (CSCdi49069)
 *
 * Revision 3.6  1996/03/07  19:28:44  dclare
 * CSCdi50967:  ISDN PRI not properly displaying number of CCBs used
 * on each interface with the show isdn status command.
 *
 * Revision 3.5  1996/02/08  18:40:52  rbeach
 * CSCdi48239:  ISDN PRI dms switchtype doesnt correctly handle SL1
 * mode. Modifications were made to the primary-dms100 channel id
 * format, called party format and support for 1 or 2 octet call
 * reference values.
 *
 * Revision 3.4  1996/01/10  22:32:57  dclare
 * CSCdi46675:  ISDN PRI using 5ESS switchtype not accepting incoming
 * calls.  Add some additional IEs to interoperate with the Teleos
 * boxes.
 *
 * Revision 3.3  1995/11/28  22:51:56  dclare
 * CSCdi44526:  ISDN PRI NET5 changes for Italian homologation. Changes to
 * handle corner test cases. Restart handling changed to handle interfaces,
 * a channel, and multiple channels.
 *
 * Revision 3.2  1995/11/17  17:40:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/28  17:39:08  rbeach
 * CSCdi35949:  Spurious accesses out of isdn code when running PRI.
 * This is caused at bringup at the RESTART and RESTART_ACK exchange.
 * We don't have a valid CCB at this time.
 *
 * Revision 2.1  1995/06/07  21:14:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       ccpri.c
DESCRIPTION:  ISDN Primary Rate Interface Call Control Task
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

This Call Control module and its supporting call state modules,

   CCPATT.C  - AT&T compatible, User side
   CCPATTN.C - AT&T compatible, Network side
   CCPNTI.C  - NTI (DMS100) compatible, User side
   CCP1TR6.C - German 1TR6 compatible, User side
   CCPVN.C   - French VN(X) compatible, User side

have been designed to operate in accordance with the AT&T ISDN
Primary Rate Interface Specification Document number 5D5-900-322
of December, 1988 and AT&T Technical Reference number 41449 of
July 1989 for 4ESS and 5ESS switch compatibility and NTI document
number NIS A211-1, Version 2, 1989 for DMS-100.  Annex 2 to the
Telenetworks ISDN Programmer's Manual gives a more complete description
of these modules than is included here.
These Call Control modules are intended as examples for use by the
Telenetworks customer in developing a useful Call Control module to
satisfy the individual application.  Every effort has been made to be
thorough within this scope, however.

These modules act as a rather elaborate interworking units between
some high level "HOST" application, and the lower layers of the
ISDN stack communicating with the ISDN peer.  They involve themselves
with various forms of state-dependent integrity checking and resource
management (channel management).  They is designed to be driven, in both
directions, by a multi-byte message interfaces.  They is capable of
tracking at least 23 independent and concurrent calls.  A detailed
specification of the message interface between the HOST and this Call
Control is available in Annex 2 of the Telenetworks Programmer's Manual.

A typical application for these modules would be at the top of the
Telenetworks ISDN stack, resident on a PRI board, installed in, for
example, a PBX or a mainframe computer.

It is seen as the responsibility of these modules to remove from the HOST
any requirement for it to understand the ISDN (or Telenetworks) interface
or protocol.  Rather, the HOST directs them, and is directed by them,
through a synthesized (simple and limited) message set wherein only the
essential information is passed.  It may well be that this message set is
in some manner inadequate for a specific application.  It is anticipated
that the message set may require tailoring to meet the needs of a each
application.  In this regard, a capability has also been included for
the HOST processor to specify some or all of the required Q.931
information elements in a standard Q.931 format to cover cases where
the proprietary information is in some manner inappropriate and the
proprietary message set has not or cannot be extended.  See the
Annex 2 document in this regard.

These modules communicate with Layer 3 Protocol Control through the
primitive/packet mail data structures and with the higher HOST entity
through the package/info data structures.

No assumption has been made about the physical location of these
modules with respect to the other software components of the system.
Thus these modules (and the lower ISDN layers) may be located on a
"PRI card", remote from the HOST, or all may share the same processor.
When mail is sent to the "HOST_ID" it should be understood that this
task ID may be an alias designation for some intermediate communications
driver.  Thus if this is a board-resident Call Control, it is assumed that
it communicates to the HOST across some (here undefined) backplane or other
hardware device driver pair, such as across a serial link.  From the
perspective of this Call Control, it is the HOST even though a device
driver may alias in that role.  The same condition could also exist in the
opposite direction where Call Control is resident with the HOST and
Layer 3 and the layers below it are remote.

B-channel connection and tone control functions are assumed to be
a HOST activity, handled independently of this module.

Much of the message compatability checking function, such as the
checking of bearer capability compatability, is unavoidably deferred
to the HOST since this module has no way of knowing what the HOST is,
what it can terminate, or what its other capabilities are.  What is
checked at this level is the legality of the incoming mandatory
information elements and their parameters, but not their local
acceptability in any given situation.

The capability has been added for recovery of calling party number
and its transmission to the HOST from the incoming SETUP message.

Message-Associated User-to-User Information (MA-UUI) has been
implemented for UUI recovery in the SETUP, ALERTING, CONNECT, DISCONNECT,
RELEAASE (first clearing message) and RELEASE_COMP (first clearing message)
per AT&T 41449, Section 7.1.3.  Call Associated Temporary Signaling
Connections (CA-TSCs) are also supported for UUI passage with the CA-TSC
initiated through the SETUP message or by a subsequent FACILITY message.
The CONGESTION_CONTROL function is also supported.

Call status updates are provided to the HOST via CALL_PROGERSS messages
to report events such as far end answer.

A capability has been provided for keeping the HOST updated as to channel
status.  The CHAN_STATUS message is used in the HOST->CC direction to
request the call and maintenance state of any B-channel.  In the CC->HOST
direction, the message is used to respond to the HOST enquiry and to give
the HOST unsolicited reports of channel SERVICE activities and channel
RESTART.

The CHAN_STATUS message also allows the HOST to request individual channel
restart and maintenance service actions (SERVICE messages).

This module is intended to use B-channels and H0 channels (6 B-channel
combinations providing a 384kbps rate)  and H11 channels in arbitrary
combinations (as per 41449).

The HOST<->CC message structure:

 entry          octet number    description
 -----------------------------------------------------------------------
 msg_type       0               Message ID
 call_id        1               call_id (MSB)
 call_id        2               call_id (LSB)
 <dsl_id        3>              present only if MAX_DSL in TUNE.H > 1
 <priv_info_len 3 or 4>         present only if EXTENDED_HOST_MSG defined
 private info   4 or 5 to N     "private" info elements
 Q.931 info     N to M          "public" info elements.  May be present
                                 only if EXTENDED_HOST_MSG is defined

The L3 message structure:

Messages from "below" are constructed as Q.931 messages.

ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

*************************************************************************
*************************************************************************
*/
/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"
#include    "address.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "ccie.h"
#include    "gp.h"
#include    "ccbri.h"
#include    "ccpri.h"
#include    "cc_chan.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/*
 * Error reporting definitions.
 */
#define CCPRI "CCPRI"		/* The module name. */

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/************************************************************************
*                            M A C R O S                                *
*************************************************************************
*/
/*
 * Conditionally translate CCB->State to a numeric index for display.
 */
#ifdef TRACING
#define CALL_STATE(s) CCPRI_NumericState( (s) )
#else
#define CALL_STATE    0
#endif

/*
 * Enter CCB State by calling the state function by value, not reference. The
 * event (e) is passed into the state.
 */
#define CCPRI_ENTER_STATE(e)  ( *CCB->PriState )( (int)(e) )

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S               *
*************************************************************************
*/

PRIVATE int	near 	CCPRI_GetL3Event 	(int, int *, int *);
PRIVATE int	near 	CCPRI_GetNewCall_Id 	(void);
PRIVATE BOOLEAN near 	CCPRI_Match 		(const BYTE *, int, BYTE);
#ifdef TRACING
PRIVATE BYTE	near 	CCPRI_NumericState 	(CC_STATE);
#endif
PRIVATE void	near 	CCPRI_ReleaseFailure 	(void);
PRIVATE void	near 	CCPRI_ResizeInfo 	(BOOLEAN);
PRIVATE void	near 	CCPRI_ServMsg 		(void);
PRIVATE void	near 	CCPRI_Shutdown 		(BOOLEAN);

/************************************************************************
*                    D A T A   A L L O C A T I O N                      *
*************************************************************************
*/
/*
 * Previously public stuff not used outside of ccpri.c
 */
PRIVATE int 		AddrLen;		/* Host msg address length */
PRIVATE int 		CC_SwitchType;		/* Record of initialzed switchtype */
PRIVATE CCB_ID     	CCB_Tail[MAX_DSL];
PRIVATE struct     	CallControlBlock Dummy_Ccb; /* The dummy CCB */
PRIVATE BYTE      	*Host_IEs;		/* Pointer to HOST IEs */
PRIVATE int 		LrgInfoSize;		/* Size of large info buffer */
PRIVATE BOOLEAN      	MultiPRI;		/* Multiple DSLs? */
PRIVATE BUFFER_TYPE 	OutInfoBufType;		/* Info buffer type */
PRIVATE int 		SmlInfoSize;		/* Size of small info buffer */

/*
 * Public stuff
 */
PUBLIC int 		AllocatedCCBs;		/* Counter of CCBs allocated */
PUBLIC int          	Call_Id_Source;		/* "Well" for new Call_ID's */
PUBLIC CCB_ID     	CCB_Head[MAX_DSL];
PUBLIC CCB_ID     	CCB;
PUBLIC BYTE 		Diag[2];		/* Cause diagnostic buffer */
PUBLIC BYTE 		Dsl_Id;			/* Interface identifier */
PUBLIC int 		Host_IEs_Len;		/* Length of HOST IEs */
PUBLIC BYTE     	*IE;			/* Pointer to CCIE info element */
PUBLIC BYTE       	*IE_Fields;		/* Ptr to CCIE buffer for IE unpack */
PUBLIC PKT_ID     	InPkt, OutPkt;		/* Internal packet pointers */
PUBLIC BOOLEAN    	MultiInterface;		/* Multiple interfaces? */
PUBLIC BYTE      	*OutPkgInfo;		/* Ptr to the package info */
PUBLIC int 		OutPkgInfoLen;		/* Package info length */
PUBLIC int 		SourceState;		/* State of L3 */
PUBLIC UINT32     	T316_TimeOut;		/* Restart timer timeout */

#ifdef EXTENDED_HOST_MSG
PRIVATE int 		PrivInfoLenOffset;	/* Host private IE len offset */
PRIVATE int 		Q931_IEs_Len;		/* Len of Q931 elements in HOST msg */

#endif
/************************************************************************
*                                                                       *
*            THE INFORMATION ELEMENT VALIDATION DATABASE                *
*                                                                       *
* Following is a set of tables organized by information element type.   *
* These tables are referenced to validate the contents of the Q.931     *
* information elements received from the far end.                       *
*                                                                       *
* A table exists for each element field analyzed, for each switch type  *
* supported.  The values in each table are derived from the switch      *
* vendor specifications and represent all the legal values defined,     *
* rather than some subset anticipated for a particular application.     *
*                                                                       *
* For each table, a table pointer variable and a size variable are      *
* declared, which are initialized, on a SwitchType-basis (in            *
* CCPRI_Init()) once, at the start of task execution, and are used      *
* subsequently during active call processing, to access this database.  *
*                                                                       *
************************************************************************/

/***********************************************************************
*    Data used in the analysis of the Bearer Capability Info Element.  *
***********************************************************************/

PRIVATE const BYTE *Xfer_Cap;	/* Transfer capability list pointer */
PRIVATE const BYTE *Xfer_Rate;	/* Transfer rate list pointer */
PRIVATE const BYTE *Xfer_Mode;	/* Transfer mode list pointer */
PRIVATE int Xfer_Cap_Size;	/* Size of transfer capability list */
PRIVATE int Xfer_Rate_Size;	/* Size of trasfer rate list */
PRIVATE int Xfer_Mode_Size;	/* Size of transfer mode list */

/*
 * Legal Bearer Capability IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Xfer_Cap[] = {
    0x00,			/* Speech */
    0x08,			/* Unrest digital */
    0x09,			/* Restricted digital */
    0x10			/* 3.1K audio */
};
static const BYTE Att4_Xfer_Mode[] = {
    0x00,			/* Circuit */
    0x02			/* Packet */
};
static const BYTE Att4_Xfer_Rate[] = {
    0x00,			/* Packet mode */
    0x10,			/* 64kbps */
    0x11,			/* 2X64kbps */
    0x13,			/* 384kbps */
    0x15			/* 1536kbps */
};
#endif
#ifdef PRI_5ESS
static const BYTE Att5_Xfer_Cap[] = {
    0x00,			/* Speech */
    0x08,			/* Unrest digital */
    0x09,			/* Restricted digital */
    0x10			/* 3.1K audio */
};
static const BYTE Att5_Xfer_Mode[] = {
    0x00			/* Circuit */
};
static const BYTE Att5_Xfer_Rate[] = {
    0x10			/* 64kbps */
};
#endif
#ifdef PRI_DMS100
static const BYTE Dms_Xfer_Cap[] = {
    0x00,			/* Speech */
    0x08,			/* Unrest digital */
    0x09,			/* Restricted digital */
    0x10			/* 3.1K audio */
};
static const BYTE Dms_Xfer_Mode[] = {
    0x00			/* Circuit */
};
static const BYTE Dms_Xfer_Rate[] = {
    0x10			/* 64kbps */
};
#endif
#ifdef VN2
static const BYTE Vn2_Xfer_Cap[] = {
    0x00,			/* Speech */
    0x08,			/* Unrest digital */
    0x10			/* 3.1K audio */
};
static const BYTE Vn2_Xfer_Mode[] = {
    0x00			/* Circuit */
};
static const BYTE Vn2_Xfer_Rate[] = {
    0x10			/* 64kbps */
};
#endif
/* NTT */
static const BYTE Nttp_Xfer_Cap[] = {
    0x00,			/* Speech */
    0x08,			/* Unrest digital */
    0x09,			/* Restricted digital */
    0x10			/* 3.1K audio */
};
static const BYTE Nttp_Xfer_Mode[] = {
    0x00,			/* Circuit */
    0x02			/* Packet */
};
static const BYTE Nttp_Xfer_Rate[] = {
    0x00,			/* Packet mode */
    0x10,			/* 64kbps */
    0x11,			/* 2X64kbps */
    0x13,			/* 384kbps */
    0x15			/* 1536kbps */
};
/* NET5 */
static const BYTE Net5_Xfer_Cap[] = {
    0x00,			/* Speech */
    0x08,			/* Unrest digital */
    0x09,			/* Restricted digital */
    0x10,			/* 3.1K audio */
    0x11			/* 7 Khz audio */
};
static const BYTE Net5_Xfer_Mode[] = {
    0x00,			/* Circuit */
    0x02			/* Packet */
};
static const BYTE Net5_Xfer_Rate[] = {
    0x00,			/* Packet mode */
    0x10			/* 64kbps */
};
/* TS014 */
static const BYTE Ts014_Xfer_Cap[] = {
    0x00,			/* Speech */
    0x08,			/* Unrest digital */
    0x09,			/* Restricted digital */
    0x10			/* 3.1K audio */
};
static const BYTE Ts014_Xfer_Mode[] = {
    0x00 			/* Circuit */
};
static const BYTE Ts014_Xfer_Rate[] = {
    0x10 			/* 64kbps */
};

/***********************************************************************
*   Data used in the analysis of the Called Party Number Info Element. *
************************************************************************/

PRIVATE const BYTE *Cd_Numb_Type;	/* Number type list pointer */
PRIVATE const BYTE *Cd_Numb_Plan;	/* Number plan list pointer */
PRIVATE int Cd_Numb_Type_Size;	/* Size of number type list */
PRIVATE int Cd_Numb_Plan_Size;	/* Size of number plan list */

/*
 * Legal Called Party Number IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Cd_Numb_Type[] = {
    0x01,			/* International */
    0x02,			/* National */
    0x04,			/* Subscriber */
    0x06			/* Abbreviated */
};
static const BYTE Att4_Cd_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x02,			/* Telephony */
    0x09			/* Private */
};
#endif
#ifdef PRI_5ESS
static const BYTE Att5_Cd_Numb_Type[] = {
    0x00,			/* Unknown, added for teleos interop */
    0x01,			/* International */
    0x02,			/* National */
    0x04,			/* Subscriber */
    0x06			/* Abbreviated */
};
static const BYTE Att5_Cd_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x02,			/* Telephony */
    0x09			/* Private */
};
#endif
#ifdef PRI_DMS100
static const BYTE Dms_Cd_Numb_Type[] = {
    0x00,			/* Unknown */
    0x01,			/* International */
    0x02,			/* National */
    0x04,			/* Subscriber */
    0x06			/* Abbreviated */
};
static const BYTE Dms_Cd_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x09			/* Private */
};
#endif
#ifdef VN2
static const BYTE Vn2_Cd_Numb_Type[] = {
    0x00,			/* En-bloc */
    0x05			/* Overlap sending */
};
static const BYTE Vn2_Cd_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x09			/* Private (internal) */
};
#endif
/* NTT */
static const BYTE Nttp_Cd_Numb_Type[] = {
    0x00,			/* Unknown */
    0x01,			/* International */
    0x02,			/* National */
    0x04			/* Subscriber */
};
static const BYTE Nttp_Cd_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x02,			/* Telephony */
    0x09			/* Private */
};
/* NET5 */
static const BYTE Net5_Cd_Numb_Type[] = {
    0x00,			/* Unknown */
    0x01,			/* International */
    0x02,			/* National */
    0x03,			/* Network specific */
    0x04,			/* Subscriber */
    0x06,			/* Abbreviated number*/
    0x07 			/* Reserved */
};
static const BYTE Net5_Cd_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x03,			/* Data numbering plan, X.121 */
    0x04,			/* Telex numbering plan, F.69 */
    0x08,			/* National Standard numbering plan */
    0x09			/* Private */
};
/* TS014 */
static const BYTE Ts014_Cd_Numb_Type[] = {
    0x00,			/* Unknown */
    0x01,			/* International */
    0x02,			/* National */
    0x04			/* Subscriber */
};
static const BYTE Ts014_Cd_Numb_Plan[] = {
    0x01 			/* ISDN */
};

/***********************************************************************
* Data used in the analysis of the Called Party Sub Addr Info Element. *
************************************************************************/

PRIVATE const BYTE *Cd_Sub_Type;	/* Number type list pointer */
PRIVATE int Cd_Sub_Type_Size;	/* Size of number type list */

/*
 * Legal Calling Party Number IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Cd_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};
#endif
#ifdef PRI_5ESS
static const BYTE Att5_Cd_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};
#endif
#ifdef PRI_DMS100
/*
 * This element is not supported for PRI_DMS100.
 */
static const BYTE Dms_Cd_Sub_Type[] = {
    0xFF
};
#endif
#ifdef VN2
static const BYTE Vn2_Cd_Sub_Type[] = {
    0x00,			/* OSI sub-address */
    0x02			/* User-specific */
};
#endif
/* NTT */
static const BYTE Nttp_Cd_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};
/* NET5 */
static const BYTE Net5_Cd_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};
/* TS014 */
static const BYTE Ts014_Cd_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};

/***********************************************************************
*  Data used in the analysis of the Calling Party Number Info Element. *
************************************************************************/

PRIVATE const BYTE *Cr_Numb_Type; /* Numper type list pointer */
PRIVATE const BYTE *Cr_Numb_Plan; /* Number plan list pointer */
PRIVATE const BYTE *Cr_Scrn_Ind;  /* Screening indicator list pointer */
PRIVATE const BYTE *Cr_Prest_Ind; /* Presentation indicator list pointer */
PRIVATE int Cr_Numb_Type_Size;	/* Size of number type list */
PRIVATE int Cr_Numb_Plan_Size;	/* Size of number plan list */
PRIVATE int Cr_Scrn_Ind_Size;	/* Size of screening indicator list */
PRIVATE int Cr_Prest_Ind_Size;	/* Size of presentation indicator list */

/*
 * Legal Calling Party Number IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Cr_Numb_Type[] = {
    0x01,			/* International */
    0x02,			/* National */
    0x04			/* Subscriber */
};
static const BYTE Att4_Cr_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x02,			/* Telephony */
    0x09			/* Private */
};
static const BYTE Att4_Cr_Prest_Ind[] = {
    0xFF,			/* No presentation ind (CCIE flag) */
    0x00,			/* Presentation allowed */
    0x01,			/* Presentation restricted */
    0x02			/* No number available */
};
static const BYTE Att4_Cr_Scrn_Ind[] = {
    0xFF,			/* No screening ind (CCIE flag) */
    0x00,			/* User provided, not screened */
    0x01,			/* User prov, verified/passed */
    0x02,			/* User prov, verified/failed */
    0x03			/* Network provided */
};
#endif
#ifdef PRI_5ESS
static const BYTE Att5_Cr_Numb_Type[] = {
    0x01,			/* International */
    0x02,			/* National */
    0x04			/* Subscriber */
};
static const BYTE Att5_Cr_Numb_Plan[] = {
    0x01,			/* ISDN */
    0x02,			/* Telephony */
    0x09			/* Private */
};
static const BYTE Att5_Cr_Prest_Ind[] = {
    0xFF,			/* No presentation ind (CCIE flag) */
    0x00,			/* Presentation allowed */
    0x01,			/* Presentation restricted */
    0x02			/* No number available */
};
static const BYTE Att5_Cr_Scrn_Ind[] = {
    0xFF,			/* No screening ind (CCIE flag) */
    0x00,			/* User provided, not screened */
    0x01,			/* User prov, verified/passed */
    0x02,			/* User prov, verified/failed */
    0x03			/* Network provided */
};
#endif
#ifdef PRI_DMS100
static const BYTE Dms_Cr_Numb_Type[] = {
    0x00,			/* Unknown */
    0x01,			/* International */
    0x02,			/* National */
    0x04			/* Subscriber */
};
static const BYTE Dms_Cr_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x09			/* Private */
};
static const BYTE Dms_Cr_Prest_Ind[] = {
    0xFF,			/* No presentation ind (CCIE flag) */
    0x00,			/* Presentation allowed */
    0x01,			/* Presentation restricted */
    0x02			/* No number available */
};
static const BYTE Dms_Cr_Scrn_Ind[] = {
    0xFF,			/* No screening ind (CCIE flag) */
    0x00,			/* User provided, not screened */
    0x03			/* Network provided */
};
#endif
#ifdef VN2
static const BYTE Vn2_Cr_Numb_Type[] = {
    0x00,			/* Unknown */
    0x01,			/* International */
    0x02,			/* National */
    0x04			/* Subscriber */
};
static const BYTE Vn2_Cr_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x09			/* Private */
};
static const BYTE Vn2_Cr_Prest_Ind[] = {
    0x00,			/* Presentation allowed */
    0x01,			/* Presentation restricted */
    0x02			/* No number available */
};
static const BYTE Vn2_Cr_Scrn_Ind[] = {
    0x00,			/* User provided, not screened */
    0x01,			/* User prov, verified/passed */
    0x03			/* Network provided */
};
#endif
/* NTT */
static const BYTE Nttp_Cr_Numb_Type[] = {
    0x01,			/* International */
    0x02,			/* National */
    0x04			/* Subscriber */
};
static const BYTE Nttp_Cr_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x02,			/* Telephony */
    0x09			/* Private */
};
static const BYTE Nttp_Cr_Prest_Ind[] = {
    0xFF,			/* No presentation ind (CCIE flag) */
    0x00,			/* Presentation allowed */
    0x01,			/* Presentation restricted */
    0x02			/* No number available */
};
static const BYTE Nttp_Cr_Scrn_Ind[] = {
    0xFF,			/* No screening ind (CCIE flag) */
    0x00,			/* User provided, not screened */
    0x01,			/* User prov, verified/passed */
    0x02,			/* User prov, verified/failed */
    0x03			/* Network provided */
};
/* NET5 */
static const BYTE Net5_Cr_Numb_Type[] = {
    0x01,			/* International */
    0x02,			/* National */
    0x04			/* Subscriber */
};
static const BYTE Net5_Cr_Numb_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* ISDN */
    0x02,			/* Telephony */
    0x09			/* Private */
};
static const BYTE Net5_Cr_Prest_Ind[] = {
    0xFF,			/* No presentation ind (CCIE flag) */
    0x00,			/* Presentation allowed */
    0x01,			/* Presentation restricted */
    0x02			/* No number available */
};
static const BYTE Net5_Cr_Scrn_Ind[] = {
    0xFF,			/* No screening ind (CCIE flag) */
    0x00,			/* User provided, not screened */
    0x01,			/* User prov, verified/passed */
    0x02,			/* User prov, verified/failed */
    0x03			/* Network provided */
};
/* TS014 */
static const BYTE Ts014_Cr_Numb_Type[] = {
    0x00,			/* Unknown */
    0x01,			/* International */
    0x02,			/* National */
    0x04			/* Subscriber */
};
static const BYTE Ts014_Cr_Numb_Plan[] = {
    0x01 			/* ISDN */
};
static const BYTE Ts014_Cr_Prest_Ind[] = {
    0xFF,			/* No presentation ind (CCIE flag) */
    0x00,			/* Presentation allowed */
    0x01,			/* Presentation restricted */
    0x02			/* No number available */
};
static const BYTE Ts014_Cr_Scrn_Ind[] = {
    0xFF,			/* No screening ind (CCIE flag) */
    0x01,			/* User prov, verified/passed */
    0x03			/* Network provided */
};

/***********************************************************************
* Data used in the analysis of the Calling Party Sub Addr Info Element *
************************************************************************/

PRIVATE const BYTE *Cr_Sub_Type; /* Numper type list pointer */
PRIVATE int Cr_Sub_Type_Size;	/* Size of number type list */

/*
 * Legal Calling Party Number IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Cr_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};
#endif
#ifdef PRI_5ESS
static const BYTE Att5_Cr_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};
#endif
#ifdef PRI_DMS100
/*
 * This element is not supported for PRI_DMS100.
 */
static const BYTE Dms_Cr_Sub_Type[] = {
    0xFF
};
#endif
#ifdef VN2
static const BYTE Vn2_Cr_Sub_Type[] = {
    0x00,			/* OSI sub-address */
    0x02			/* User-specific */
};
#endif
/* NTT */
static const BYTE Nttp_Cr_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};
/* NET5 */
static const BYTE Net5_Cr_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};
/* TS014 */
static const BYTE Ts014_Cr_Sub_Type[] = {
    0x00,			/* NSAP */
    0x02			/* User specified */
};


/***********************************************************************
*         Data used in the analysis of the Cause Info Element.         *
************************************************************************/

PRIVATE const BYTE *Cause_Location; /* Cause location list pointer */
PRIVATE int Cause_Location_Size; /* Size of cause location list */

/*
 * Legal Cause IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Cause_Location[] = {
    0x00,			/* User */
    0x01,			/* Local private network */
    0x02,			/* Local network */
    0x03,			/* Transit network */
    0x04,			/* Remote local network */
    0x05,			/* Remote private network */
    0x07,			/* International network */
    0x0a			/* Network beyond internetworking pt */
};
#endif
#ifdef PRI_5ESS
static const BYTE Att5_Cause_Location[] = {
    0x00,			/* User */
    0x01,			/* Local private network */
    0x02,			/* Local network */
    0x03,			/* Transit network */
    0x04,			/* Remote local network */
    0x05			/* Remote private network */
};
#endif
#ifdef PRI_DMS100
static const BYTE Dms_Cause_Location[] = {
    0x00,			/* User */
    0x01,			/* Local priv network */
    0x02,			/* Local network */
    0x03,			/* Transit network */
    0x04,			/* Remote loc network */
    0x05,			/* Remote priv network */
    0x07			/* International network */
};
#endif
#ifdef PRI_1TR6
static const BYTE P1tr6_Cause_Location[] = {
    0x04,			/* public net */
    0x05,			/* private network */
    0x0F			/* keine Angabe uber Entsthungsort */
};
#endif
#ifdef VN2
static const BYTE Vn2_Cause_Location[] = {
    0x07			/* Not signif. (implicit by cause) */
};
#endif
/* NTT */
static const BYTE Nttp_Cause_Location[] = {
    0x00,			/* User */
    0x01,			/* Local private network */
    0x02,			/* Local network */
    0x03,			/* Transit network */
    0x04,			/* Remote local network */
    0x05,			/* Remote private network */
    0x07,			/* International network */
    0x0a			/* Network beyond internetworking pt */
};
/* NET5 */
static const BYTE Net5_Cause_Location[] = {
    0x00,			/* User */
    0x01,			/* Local private network */
    0x02,			/* Local network */
    0x03,			/* Transit network */
    0x04,			/* Remote local network */
    0x05,			/* Remote private network */
    0x07,			/* International network */
    0x0a			/* Network beyond internetworking pt */
};
/* TS014 */
static const BYTE Ts014_Cause_Location[] = {
    0x00,			/* User */
    0x01,			/* Local private network */
    0x02,			/* Local network */
    0x04 			/* Remote local network */
};


/***********************************************************************
*       Data used in the analysis of the Channel ID Info Element.      *
************************************************************************/

PRIVATE const BYTE *Chan_Type;	/* Channel type list pointer */
PRIVATE int Chan_Type_Size;	/* Size of channel type list */

/*
 * Legal Channel ID IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Chan_Type[] = {
    0x03,			/* B_Chan units */
    0x06,			/* H0 Chan units */
    0x08			/* H11 Chan units */
};
#endif
#ifdef PRI_1TR6
static const BYTE P1tr6_Chan_Type[] = {
    0x00			/* Not supported */
};
#endif
#ifdef PRI_5ESS
static const BYTE Att5_Chan_Type[] = {
    0x03			/* B_Chan units */
};
#endif
#ifdef PRI_DMS100
static const BYTE Dms_Chan_Type[] = {
    0x03			/* B_Chan units */
};
#endif
#ifdef VN2
static const BYTE Vn2_Chan_Type[] = {
    0x03			/* B_Chan units */
};
#endif
/* NTT */
static const BYTE Nttp_Chan_Type[] = {
    0x03,			/* B_Chan units */
    0x06,			/* H0 Chan units */
    0x08			/* H11 Chan units */
};
/* NET5 */
static const BYTE Net5_Chan_Type[] = {
    0x03,			/* B_Chan units */
    0x06,			/* H0 Chan units */
    0x08			/* H11 Chan units */
};
/* TS014 */
static const BYTE Ts014_Chan_Type[] = {
    0x03 			/* B_Chan units */
};


/***********************************************************************
*    Data used in the analysis of the Network Facility Info Element.   *
************************************************************************/

PRIVATE const BYTE *Net_Id;	/* Network ID list pointer */
PRIVATE const BYTE *Id_Plan;	/* ID plan list pointer */
PRIVATE const BYTE *Feats;	/* Features list pointer */
PRIVATE const BYTE *Servs;	/* Services list pointer */
PRIVATE int Net_Id_Size;	/* Size of network ID list */
PRIVATE int Id_Plan_Size;	/* Size of ID plan list */
PRIVATE int Feats_Size;		/* Size of features list */
PRIVATE int Servs_Size;		/* Size of services list */

/*
 * Legal Network Facility IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Net_Id[] = {
    0x00,			/* User */
    0x02			/* National network */
};
static const BYTE Att4_Id_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* Carrier ID code */
    0x03			/* Data network ID code */
};
static const BYTE Att4_Feats[] = {
    0x01,			/* CPN (SID) preferred */
    0x02,			/* BN (ANI) preferred */
    0x03,			/* CPN (SID) only */
    0x04,			/* BN (ANI) only */
    0x09,			/* Call Assoc. TSC */
    0x0A			/* Call Assoc, TSC clearing/unavail */
};
static const BYTE Att4_Servs[] = {
    0x01,			/* SDN */
    0x02,			/* Megacom 800 */
    0x03,			/* Megacom */
    0x06,			/* Accunet */
    0x08,			/* International 800 */
    0x10			/* AT&T Multiquest */
};
#endif
#ifdef PRI_5ESS
static const BYTE Att5_Net_Id[] = {
    0x02			/* National network */
};
static const BYTE Att5_Id_Plan[] = {
    0x01			/* Carrier ID code */
};
static const BYTE Att5_Feats[] = {
    0x05,			/* Operator */
    0x06			/* Default Common Carrier Operator */
};
static const BYTE Att5_Servs[] = {
    0x01,			/* Access to Virtual Private Network */
    0x02,			/* Megacom 800 telecommuications */
    0x03,			/* Megacom telecommuications */
    0x04,			/* INWATS */
    0x05,			/* WATS maximal subscribed band */
    0x06,			/* Accunet Switched Digital */
    0x07,			/* International Long Distance */
    0x08,			/* International 800 */
    0x0B,			/* Electronic Tandem Network */
    0x0D,			/* Private Virtual Network */
    0x10			/* DIAL-IT NOVA */
};
#endif
#ifdef PRI_DMS100
static const BYTE Dms_Net_Id[] = {
    0x00,			/* User */
    0x02			/* National network */
};
static const BYTE Dms_Id_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* Carrier ID code */
    0x03			/* Data network ID code */
};
static const BYTE Dms_Feats[] = {
    0x01,			/* Private */
    0x02,			/* In WATS */
    0x03,			/* Out WATS */
    0x04,			/* FX */
    0x05,			/* Tie Trunk */
    0x08			/* TRO call */
};
static const BYTE Dms_Servs[] = {
    0x01,			/* Private */
    0x02,			/* In WATS */
    0x03,			/* Out WATS */
    0x04,			/* FX */
    0x05,			/* Tie Trunk */
    0x08			/* TRO call */
};
#endif
#ifdef VN2
/*
 * This element is not supported for VN with CCITT standard coding.
 */
static const BYTE Vn2_Net_Id[] = {
    0xFF
};
static const BYTE Vn2_Id_Plan[] = {
    0xFF
};
static const BYTE Vn2_Feats[] = {
    0xFF
};
static const BYTE Vn2_Servs[] = {
    0xFF
};
#endif				/* VN */
/* NTT */
static const BYTE Nttp_Net_Id[] = {
    0x00,			/* User */
    0x02			/* National network */
};
static const BYTE Nttp_Id_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* Carrier ID code */
    0x03			/* Data network ID code */
};
static const BYTE Nttp_Feats[] = {
    0x01,			/* CPN (SID) preferred */
    0x02,			/* BN (ANI) preferred */
    0x03,			/* CPN (SID) only */
    0x04,			/* BN (ANI) only */
    0x09,			/* Call Assoc. TSC */
    0x0A			/* Call Assoc, TSC clearing/unavail */
};
static const BYTE Nttp_Servs[] = {
    0x01,			/* SDN */
    0x02,			/* Megacom 800 */
    0x03,			/* Megacom */
    0x06,			/* Accunet */
    0x08,			/* International 800 */
    0x10			/* AT&T Multiquest */
};
/* NET5 */
static const BYTE Net5_Net_Id[] = {
    0x00,			/* User */
    0x02			/* National network */
};
static const BYTE Net5_Id_Plan[] = {
    0x00,			/* Unknown */
    0x01,			/* Carrier ID code */
    0x03			/* Data network ID code */
};
static const BYTE Net5_Feats[] = {
    0x01,			/* CPN (SID) preferred */
    0x02,			/* BN (ANI) preferred */
    0x03,			/* CPN (SID) only */
    0x04,			/* BN (ANI) only */
    0x09,			/* Call Assoc. TSC */
    0x0A			/* Call Assoc, TSC clearing/unavail */
};
static const BYTE Net5_Servs[] = {
    0x01,			/* SDN */
    0x02,			/* Megacom 800 */
    0x03,			/* Megacom */
    0x06,			/* Accunet */
    0x08,			/* International 800 */
    0x10			/* AT&T Multiquest */
};
/* TS014 */
static const BYTE Ts014_Net_Id[] = {
    0xFF
};
static const BYTE Ts014_Id_Plan[] = {
    0xFF
};
static const BYTE Ts014_Feats[] = {
    0xFF
};
static const BYTE Ts014_Servs[] = {
    0xFF
};


/***********************************************************************
*    Data used in the analysis of the Notify Indicator Info Element.   *
************************************************************************/

PRIVATE const BYTE *Notify_Ind;	/* Notify indicator list pointer */
PRIVATE int Notify_Ind_Size;	/* Size of notify indicator list */

/*
 * Legal Notify Indicator IE field values.
 */
#ifdef PRI_4ESS
/*
 * This element is not currently considered for AT&T - future.
 */
static const BYTE Att4_Notify_Ind[] = {
    0xFF
};
#endif
#ifdef PRI_5ESS
/*
 * This element is not currently considered for AT&T - future.
 */
static const BYTE Att5_Notify_Ind[] = {
    0xFF
};
#endif
#ifdef PRI_DMS100
static const BYTE Dms_Notify_Ind[] = {
    0x00,			/* Suspended */
    0x01,			/* Resumed */
    0xE1			/* BNR Call info/event */
};
#endif
#ifdef VN2
/*
 * This element is not supported for VN.
 */
static const BYTE Vn2_Notify_Ind[] = {
    0xFF
};
#endif
/*
 * This element is not currently considered for NTT
 */
static const BYTE Nttp_Notify_Ind[] = {
    0xFF
};
/*
 * This element is not currently considered for NET5
 */
static const BYTE Net5_Notify_Ind[] = {
    0xFF
};
/*
 * This element is not currently considered for TS014
 */
static const BYTE Ts014_Notify_Ind[] = {
    0xFF
};


/***********************************************************************
*   Data used in the analysis of the Progress Indicator Info Element.  *
************************************************************************/

PRIVATE const BYTE *Prog_Location;	/* Progress location list pointer */
PRIVATE const BYTE *Prog_Desc;	/* Progress description list pointer */
PRIVATE int Prog_Location_Size;	/* Size of progress locationlist */
PRIVATE int Prog_Desc_Size;	/* Size of progress description list */

/*
 * Legal Progress Indicator IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Prog_Location[] = {
    0x00,			/* User */
    0x01,			/* Local priv network */
    0x02,			/* Local network */
    0x03,			/* Transit network */
    0x04,			/* Remote loc network */
    0x05			/* Remote priv network */
};
static const BYTE Att4_Prog_Desc[] = {
    0x01,			/* Not end-to-end ISDN */
    0x02,			/* Dest addr non-ISDN */
    0x03,			/* Orig addr non-ISDN */
    0x04,			/* Call has returned to the ISDN */
    0x08			/* In-band info now avail */
};
#endif
#ifdef PRI_5ESS
static const BYTE Att5_Prog_Location[] = {
    0x00,			/* User */
    0x01,			/* Local priv network, added for teleos interop */
    0x02,			/* Local network */
    0x03			/* Transit network */
};
static const BYTE Att5_Prog_Desc[] = {
    0x01,			/* Not end-to-end ISDN */
    0x02,			/* Dest addr non-ISDN */
    0x03,			/* Orig addr non-ISDN */
    0x04,			/* Call has returned to the ISDN */
    0x08,			/* In-band info now avail */
    0x0A			/* Delay at terminating switch */
};
#endif
#ifdef PRI_DMS100
static const BYTE Dms_Prog_Location[] = {
    0x00,			/* User */
    0x01,			/* Local priv network */
    0x02,			/* Local network */
    0x03,			/* Transit network */
    0x04,			/* Remote loc network */
    0x05,			/* Remote priv network */
    0x07			/* International network */
};
static const BYTE Dms_Prog_Desc[] = {
    0x01,			/* Not end-to-end ISDN */
    0x08			/* In-band info now avail */
};
#endif
#ifdef VN2
static const BYTE Vn2_Prog_Location[] = {
    0xFF			/* Any progress location */
};
static const BYTE Vn2_Prog_Desc[] = {
    0x01,			/* Not end-to-end ISDN */
    0x03			/* Orig addr non-ISDN */
};
#endif
/* NTT */
static const BYTE Nttp_Prog_Location[] = {
    0x00,			/* User */
    0x01,			/* Local priv network */
    0x02,			/* Local network */
    0x03,			/* Transit network */
    0x04,			/* Remote loc network */
    0x05			/* Remote priv network */
};
static const BYTE Nttp_Prog_Desc[] = {
    0x01,			/* Not end-to-end ISDN */
    0x02,			/* Dest addr non-ISDN */
    0x03,			/* Orig addr non-ISDN */
    0x04,			/* Call has returned to the ISDN */
    0x08			/* In-band info now avail */
};
/* NET5 */
static const BYTE Net5_Prog_Location[] = {
    0x00,			/* User */
    0x01,			/* Local priv network */
    0x02,			/* Local network */
    0x03,			/* Transit network */
    0x04,			/* Remote loc network */
    0x05			/* Remote priv network */
};
static const BYTE Net5_Prog_Desc[] = {
    0x01,			/* Not end-to-end ISDN */
    0x02,			/* Dest addr non-ISDN */
    0x03,			/* Orig addr non-ISDN */
    0x04,			/* Call has returned to the ISDN */
    0x08			/* In-band info now avail */
};
/* TS014 */
static const BYTE Ts014_Prog_Location[] = {
    0x00,			/* User */
    0x01,			/* Local priv network */
    0x02,			/* Local network */
    0x04 			/* Remote loc network */
};
static const BYTE Ts014_Prog_Desc[] = {
    0x01,			/* Not end-to-end ISDN */
    0x02,			/* Dest addr non-ISDN */
    0x03,			/* Orig addr non-ISDN */
    0x08			/* In-band info now avail */
};


/***********************************************************************
*       Data used in the analysis of the User-User Info Element.       *
************************************************************************/

PRIVATE const BYTE *Proto_Des;	/* Protocol descriminator list pointer */
PRIVATE int Proto_Des_Size;	/* Size of protocol descriminator list */

/*
 * Legal User-User IE field values.
 */
#ifdef PRI_4ESS
static const BYTE Att4_Proto_Des[] = {
    0x00,			/* User-specific */
    0x01,			/* OSI high layer */
    0x02,			/* X.244 */
    0x03,			/* System management */
    0x04,			/* IA5 characters */
    0x07,			/* V.120 rate adaption */
    0x08			/* Q.931 call control */
};				/* Others to be added as required */
#endif
#ifdef PRI_5ESS
/*
 * While this element is used by this switch type, the 5E7 specification does
 * not define values for the protocol descriminator (user-defined). It should
 * not be tested for the present, if ever.
 */
static const BYTE Att5_Proto_Des[] = {
    0xFF			/* No protocol descriminator */
};
#endif
#ifdef PRI_DMS100
static const BYTE Dms_Proto_Des[] = {
    0xFF			/* No protocol descriminator */
};
#endif
#ifdef VN2
static const BYTE Vn2_Proto_Des[] = {
    0x04,			/* IA5 characters */
    0x40			/* NT2 - NT2 facilities */
};
#endif
/* NTT */
static const BYTE Nttp_Proto_Des[] = {
    0x00,			/* User-specific */
    0x01,			/* OSI high layer */
    0x02,			/* X.244 */
    0x03,			/* System management */
    0x04,			/* IA5 characters */
    0x07,			/* V.120 rate adaption */
    0x08			/* Q.931 call control */
};				/* Others to be added as required */
/* NET5 */
static const BYTE Net5_Proto_Des[] = {
    0x00,			/* User-specific */
    0x01,			/* OSI high layer */
    0x02,			/* X.244 */
    0x03,			/* System management */
    0x04,			/* IA5 characters */
    0x07,			/* V.120 rate adaption */
    0x08			/* Q.931 call control */
};				/* Others to be added as required */
/* TS014 */
static const BYTE Ts014_Proto_Des[] = {
    0xFF
};				/* Others to be added as required */

/***********************************************************************
*                  End of Standard IE Validation Tables                *
************************************************************************/

#ifdef PRI_1TR6
/***********************************************************************
*                        Non-standard 1TR6 IEs                         *
************************************************************************/

/*
 * The equivalent of Bearer Capability is Service_Ind which is in Codeset 6.
 * These sub-arrays are coded so that the first value is the total number of
 * value occurences for that Service Octet.  This table should be customized
 * to suit the particular application.
 */
#ifdef VARIABLES_NEVER_USED
BYTE *Service_Ind;		/* pointer to Services list */
int Service_Ind_Size;		/* Size of Services list */
BYTE *Service_SubTab;		/* For accessing sub-arrays */
#endif

static const int Max_Service_Octets = 16;	/* Size of Table */
static const BYTE Fernsprechen[] = {3, 0x01, 0x02, 0x03};
static const BYTE Abdienste[] = {4, 0x01, 0x02, 0x03, 0x04};
static const BYTE X21dienste[] = {4, 0x04, 0x05, 0x06, 0x0c};
static const BYTE Telefax_Gruppe_4[] = {1, 0x00};
static const BYTE Btx[] = {1, 0x00};
static const BYTE Datenubertragung[] = {1, 0x00};
static const BYTE X25[] = {6, 0x02, 0x03, 0x04, 0x05, 0x06};
static const BYTE Teletex_64[] = {1, 0x00};
static const BYTE Mixed_Mode[] = {1, 0x00};
static const BYTE Fernwirken[] = {1, 0x00};
static const BYTE Grafiktelefondienste[] = {1, 0x00};
static const BYTE Bildschirmtext[] = {1, 0x00};
static const BYTE Bildtelefon[] = {3, 0x01, 0x02, 0x03};

static const BYTE *const P1tr6_Service_Ind[] = {(BYTE *) 0,
    Fernsprechen,
    Abdienste,
    X21dienste,
    Telefax_Gruppe_4,
    Btx,
    (BYTE *) 0,
    Datenubertragung,
    X25,
    Teletex_64,
    Mixed_Mode,
    (BYTE *) 0,
    (BYTE *) 0,
    Fernwirken,
    Grafiktelefondienste,
    Bildschirmtext,
    Bildtelefon
};

static const BYTE P1tr6_Conn_Addr_Type[] = {
    0x00,			/* Unknown */
    0x01,			/* International */
    0x02			/* National */
};

static const BYTE P1tr6_Conn_Addr_Plan[] = {
    0x00,			/* Unknown */
    0x01			/* ISDN numbering plan */
};

PRIVATE BYTE *Facility_Code;	/* pointer to Fac Code list */
PRIVATE int Facility_Code_Size;	/* Size of Fac Code list */

static const BYTE P1tr6_Facility_Code[] = {
    0x01,			/* Sperre */
    0x02,			/* Anrufweiterschaltung I */
    0x03,			/* Anrufweiterschaltung II */
    0x0E,			/* Konferenz */
    0x0F,			/* B-Kanal ubernehmen */
    0x10,			/* Aktivschalten ein gehalt Verbind */
    0x11,			/* Dreierkonferenz */
    0x12,			/* Einseitiger Dienst-bzw Endgerat */
    0x13,			/* Zweiseitiger Dienstwechsel */
    0x14,			/* Rufnummer-Id */
    0x15,			/* Geschlossene Benutzergrupp */
    0x17,			/* Anzeige ubergebener Ruf */
    0x1A,			/* Anzeige umgeleiteter/weiter */
    0x1B,			/* Unterdrucken A-Rufnummer */
    0x1E,			/* Nutzung der Verb deaktive */
    0x1D,			/* Nutzung der Verbindung ackt */
    0x1F,			/* Semipermanet Verbindung SPV */
    0x23,			/* Ruckwechsel bei zweseitigem */
    0x24			/* Anrufumleitung im privaten */
};
PRIVATE BYTE *Status;		/* pointer to Status list */
PRIVATE int Status_Size;	/* Size of Status list */

static const BYTE P1tr6_Status[] = {
    0x01,			/* keine Angabe */
    0x02			/* Tln wird gerufen */
};
BYTE *Transmit_Attr;		/* pointer to Transmit attr list */
int Transmit_Attr_Size;		/* Size of transmit attr list */

static const BYTE P1tr6_Transmit_Attr[] = {
    0x00,			/* keine Satellitenverb * */
    0x01,			/* eine Satellitenverb enthalten */
    0x02,			/* zwei Satellitenverb enhalten */
    0x03			/* drei Satellitenver enthalten */
};

#endif				/* PRI_1TR6 */
/************************************************************************
*     I N F O   E L E M E N T   A N A L Y S I S   F U N C T I O N S     *
*************************************************************************
*/

/*
 * These functions determine if a given information element is present in the
 * message from the ISDN peer and use the above information element
 * validation tables to check its coding integrity to determine whether or
 * not to "ACCEPT" it.
 */
/************************
* CCPRI_AcceptBearerCap *
*************************
*
* Find and validate the contents of the BEARER CAPABILITY information
* element from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptBearerCap(rate)
int *rate;			/* Recovered bearer rate */
{
    int cause;			/* Cause for cause info element */
    BOOLEAN struct_err = FALSE;	/* Element corrupted? */
    int i;			/* A counter */
    int len_offset;		/* HOST IE length byte offset */


    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     BEARER_CAPABILITY_IE, CODESET_0, 1);
    if (IE) {
	IE_Fields = CCIE_ExpandBearCap_IE(IE, &struct_err);
	/*
	 * Check IE for legal (though perhaps unbearable) contents.
	 */
	if ((IE_Fields[1] == 0) &&	/* CCITT coding standard */
	    (!struct_err) &&	/* No structural error */
	    (CCPRI_Match(Xfer_Cap, Xfer_Cap_Size, IE_Fields[2])) &&
	    (CCPRI_Match(Xfer_Mode, Xfer_Mode_Size, IE_Fields[3])) &&
	    (CCPRI_Match(Xfer_Rate, Xfer_Rate_Size, IE_Fields[4]))) {
	    cause = ACCEPTED;	/* A non-cause - element accepted */

	    /*
	     * Recover and return the bearer rate.
	     */
	    *rate = IE_Fields[4];

	    /* Save relevant parameters in the CALL_INCOMING for the HOST */
	    OutPkgInfo[OutPkgInfoLen++] = BEARER_CAPABILITY_IE;
	    len_offset = OutPkgInfoLen;	/* Save offset of IE length byte */
	    OutPkgInfo[OutPkgInfoLen++] = 3;	/* Priliminary length */
	    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[2];	/* Capability */
	    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[3];	/* Mode */
	    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[4];	/* Rate */
	    if (IE_Fields[13]) {/* Is Layer 1 protocol present? */
		i = 8;		/* Start offset of unpacked Layer 1 protocol */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[i++];	/* L1 Proto */
		OutPkgInfo[len_offset]++;	/* Increment HOST IE length */
		IE_Fields[13]--;/* Decrement octet 5s count */
		while (IE_Fields[13]) {	/* Look for end of user rate data */
		    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[i++];
		    OutPkgInfo[len_offset]++;	/* Increment HOST IE length */
		    IE_Fields[13]--;	/* Decrement octet 5s count */
		}
	    }
	}
#ifdef VN2
	else if ((struct_err) && (SwitchType == VN2_STYPE)) {
	    cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	}
#endif
        else {
           /*
            * This is for the net5 video incoming setup fix. 
            * The incoming SETUP is as follows:
            *    BEARER CAP 98 C0 C2
            *    CH ID      A9 83 81
            *    CALLED     81          12345
            * I  think the problem is that we  don't process the
            * third byte of the BC correctly. The above message
            * will cause CCPRI_AcceptChanId to return INVAL ELEM CONT.
            * I made the code change below and I will also change
            * the Net5_Xfer_Mode to not process video.
            */
           if ((IE_Fields[2] = 0x18) && (SwitchType == PRI_NET5_STYPE))
               cause = INCOMPATIBLE_DEST;       /* Cause 88 */
           else
               cause = INVALID_ELEM_CONTENTS;   /* Cause 100 */
           /*
            * Test the transfer mode, only accept circuit mode calls,
            * packet mode will be rejected with cause "incompatible dest",
            * for italy homologation.
            */
           if (IE_Fields[3] == 0x02)
               cause = INCOMPATIBLE_DEST;       /* Cause 88 */
        }
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }

    return (cause);
}

/********************
* CCPRI_AcceptCause *
*********************
*
* Find and validate the contents of the CAUSE information element
* from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptCause(recovered_cause)
int *recovered_cause;		/* Cause from message */
{
    int cause;			/* Cause for cause info element rejection */
    BOOLEAN struct_err = FALSE;	/* Element corrupted? */


    *recovered_cause = 0;	/* Initialize this output */

    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     CAUSE_IE, CODESET_0, 1);
#ifdef PRI_1TR6
    if ((SwitchType == PRI_1TR6_STYPE) && (IE) && (IE[1] == 0)) {
	*recovered_cause = LEER_ELEMENT;
	cause = ACCEPTED;
    } else
#endif
    if (IE) {
	IE_Fields = CCIE_ExpandCause_IE(IE, &struct_err);

	/*
	 * Test every field of the info element for 4/5ESS-PRI compliance.
	 */
	if ((!struct_err) &&
#ifdef PRI_1TR6
            ((SwitchType == PRI_1TR6_STYPE) &&
            (IE[1] == 1)         ||           /* May send 1 byte only */
            ((IE[1] == 2) && (IE_Fields[0] == 2))))
#else
	    (IE_Fields[0] == 0) &&	/* CCITT coding standard */
	    (CCPRI_Match(Cause_Location, Cause_Location_Size, IE_Fields[1])))
#endif
	{
	    if (CCIE_CauseValid((int) IE_Fields[2])) {
		/*
		 * The significance of the actual cause value must be
		 * interpreted in the context of the state.
		 */
		*recovered_cause = (int) IE_Fields[2];
		cause = ACCEPTED;
	    } else {
		cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	    }
	}
#ifdef VN2
	else if ((struct_err) && (SwitchType == VN2_STYPE)) {
	    cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	}
#endif
	else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }

    return (cause);
}

/*********************
* CCPRI_AcceptCdNumb *
**********************
*
* Find and validate the contents of the CALLED NUMBER information
* element from the incoming L3 message.  We will not compare the called
* number with a local number since it is a function for the HOST to see
* that the called number is acceptable.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptCdNumb()
{
    int cause = ACCEPTED;	/* Cause for cause info element */
    int i;			/* Local counter */
    BOOLEAN struct_err = FALSE;	/* CCIE found the IE to be coded wrong */


    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     CALLED_NUMBER_IE, CODESET_0, 1);
    if (IE) {
	IE_Fields = CCIE_ExpandPartyNumb_IE(IE, &struct_err);
	if ((!struct_err) &&
	    (CCPRI_Match(Cd_Numb_Type, Cd_Numb_Type_Size, IE_Fields[0])) &&
	    (CCPRI_Match(Cd_Numb_Plan, Cd_Numb_Plan_Size, IE_Fields[1]))) {
	    cause = ACCEPTED;	/* IE appears OK */
	    /*
	     * Put result into outgoing HOST message.
	     */
	    OutPkgInfo[OutPkgInfoLen++] = CALLED_NUMBER_IE;
	    OutPkgInfoLen++;	/* Length is skipped for now */
	    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[0];	/* Type */
	    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[1];	/* Plan */
	    i = 4;		/* Skip to the digits */
	    while (IE_Fields[i])/* Copy the individuaal digits */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[i++];
	    OutPkgInfo[OutPkgInfoLen - i + 1] = i - 2;	/* Write length */
	}
#ifdef VN2
	else if ((struct_err) && (SwitchType == VN2_STYPE)) {
	    cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	}
#endif
	else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }

    return (cause);
}

/************************
* CCPRI_AcceptCdSubAddr *
*************************
*
* Find and validate the contents of the CALLED SUB ADDRESS information
* element from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptCdSubAddr()
{
    int cause = ACCEPTED;	/* Cause for cause info element */
    int i;			/* Local counter */
    BOOLEAN struct_err = FALSE;	/* CCIE found the IE to be coded wrong */


    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     CALLED_SUB_ADDR_IE, CODESET_0, 1);
    if (IE) {
	IE_Fields = CCIE_ExpandPartyNumb_IE(IE, &struct_err);
	if ((!struct_err) &&
	    (CCPRI_Match(Cd_Sub_Type, Cd_Sub_Type_Size, IE_Fields[0]))) {
	    cause = ACCEPTED;	/* IE appears OK */
	    /*
	     * Put result into outgoing HOST message.
	     */
	    OutPkgInfo[OutPkgInfoLen++] = CALLED_SUB_ADDR_IE;
	    OutPkgInfoLen++;	/* Length is skipped for now */
	    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[0];	/* Type */
	    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[1];	/* Plan */
	    i = 4;		/* Skip to the digits */
	    while (IE_Fields[i])/* Copy the individuaal digits */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[i++];
	    OutPkgInfo[OutPkgInfoLen - i + 1] = i - 2;	/* Write length */
	}
#ifdef VN2
	else if ((struct_err) && (SwitchType == VN2_STYPE)) {
	    cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	}
#endif
	else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }

    return (cause);
}

/*********************
* CCPRI_AcceptChanId *
**********************
*
* Find and validate the contents of the CHANNEL IDENTIFICATION information
* element from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case, the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptChanId(event, relay_result)
int event;			/* Event in which the ie arrived */
BOOLEAN relay_result;		/* Send CHANNEL_ID to HOST */
{
    int cause;			/* Cause for cause info element */
    int type;			/* Channel type */
    int lo_chan, hi_chan;	/* Channel limits */
    int int_id;			/* Interface ID */
    int int_id_len;		/* Interface ID length */
    BOOLEAN struct_err;		/* IE structural error */
    int i;			/* A counter */
    int state, serv, ix, index;

    cause = 0;
    ix = 0;
    index = 0;
    struct_err = FALSE;
    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     CHANNEL_ID_IE, CODESET_0, 1);
    if (IE) {
	IE_Fields = CCIE_ExpandChanId_IE(IE, &struct_err);

	if (IE_Fields[0]) {	/* Interface ID present? */
	    /*
	     * A single octet interface ID is assumed here as per AT&T.
	     */
	    int_id = CCPRI_MapIntId(IE_Fields[IE_Fields[8]], FALSE);
	    int_id_len = (IE_Fields[IE_Fields[8] + 1]) ? ERR : 1;
	    if ((!VALID_INT_IDP(int_id)) || (int_id_len == ERR)) {	/* Legal interface? */
		cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
		goto bailout;
	    }
	} else {
	    int_id = 0;
	    int_id_len = 1;
	}
	type = CCB->ChanType;
	if (type == H11_CHANNEL_UNITS) {
	    lo_chan = 1;	/* Lowest channel within the interface */
	    hi_chan = 1 + GET_CHAN_SPANP(H11_CHANNEL_UNITS);
	} else {
	    lo_chan = IE_Fields[9];
	    i = 10;		/* Potential end of channel list */
	    while (IE_Fields[i++]);	/* Search for NULL terminator */
	    hi_chan = IE_Fields[i - 2];
	}

        /*
         * If I am here and I have received a SETUP and they are
         * requesting any channel and preferred and we are NET5
         * then let's set lo_chan and hi_chan.
         * This is for NET5 homologation.
         * 
         * If IE_Fields[2] == 0 Preferred
         * && IE_Fields[4] == 3 Any Channel
         * && NET5
         */
        if ((IE_Fields[2] == 0) && (IE_Fields[4] == 3) &&
                        (SwitchType == PRI_NET5_STYPE)) {
            lo_chan = 1;
            hi_chan = 31;
            goto net5_anychan;
        }

        /*
         * So far so good. This is for Preferred, No Channel.
         * If we are NET5 return cause 34.
         */
        if ((IE_Fields[2] == 0) && (IE_Fields[4] == 0) &&
                        (SwitchType == PRI_NET5_STYPE)) {
            cause = NO_CIRCUIT_AVAILABLE;       /* Cause 34 */
            goto bailout;
        }

	/*
	 * Test every field of the info element for coding correctness. The
	 * exclusive/preferred bit is ignored for now.
	 */
	if ((IE_Fields[1] == 1) &&	/* Test interface type */
	    (IE_Fields[3] == 0) &&	/* Test D-Chan indicator */
	    (IE_Fields[4] == 1) &&	/* Test info chan selection */
	    ((SwitchType == PRI_1TR6_STYPE) ? TRUE :
	     (IE_Fields[5] == 0)) &&	/* Test coding standard */
	    (!struct_err) &&	/* No structural error in IE? */
	    (CCPRI_Match(Chan_Type, Chan_Type_Size, IE_Fields[7])) &&
	/*
	 * Check that the range of channels received is consistent with the
	 * channel type. Note the -1 is needed to fix the logic.
	 */
	    ((lo_chan + GET_CHAN_SPANP(type) - 1) == hi_chan)) {
#ifdef CHANNEL_TABLE
	    /*
	     * Now check if the boundry channels are within the range of
	     * configured channels.
	     */
	    if ((VALID_CHANNELP(lo_chan)) && (VALID_CHANNELP(hi_chan))) {
		/*
		 * We accept the coding correctness of the information
		 * element. Now free any other channel which might be
		 * associated with this CCB.
		 *
		 * Check for different channel numb or same channel in a
		 * different interface.
		 */
		if ((CCB->B_Chan != lo_chan) || (CCB->Int_Id[0] != int_id)) {
#ifdef PRI_1TR6
		    if ((SwitchType == PRI_1TR6_STYPE) && (CCB->B_Chan) &&
			(CC_CHAN_GetStatepri(int_id, CCB->B_Chan, type, Dsl_Id) == CHANNEL_BUSY)) {
			cause = CALL_REJECTED_1TR6;
			goto bailout;
		    } else if ((CCB->B_Chan))
			CCPRI_ReleaseChan();

#else
		    if (CCB->B_Chan)
			CCPRI_ReleaseChan();
#endif
		    /*
		     * We will not blindly accept the new B-channel as
		     * available. Here we explicitly override user
		     * CHANNEL_PROPOSED cases. This is a recognition of
		     * B-Channel SETUP glare and a knowledge that NETWORK
		     * will be giving preference on the channel selection to
		     * this call over one pending SETUP by the USER side.
		     */

net5_anychan:
		    CCB->Exclusive = IE_Fields[2];
		    /*
		     * Useful for checking Glare situations.
		     */
		    state = CC_CHAN_GetStatepri(int_id, lo_chan, type, Dsl_Id);
		    serv = CC_CHAN_GetServState(int_id, lo_chan, type, Dsl_Id);
		    if (detail_debug)
			buginf("\nCCPRI, state = %d, serv = %d, int_id = %d, lo_chan = %d, type = %d, Dsl_Id = %d, callid = %x", state, serv, int_id, lo_chan, type, Dsl_Id, CCB->Call_ID);

		    if (((state == CHANNEL_IDLE) || (state == CHANNEL_PROPOSED)) &&
			(serv == IN_SERVICE))
		    {
			if (detail_debug)
			    buginf("\nCCPRI in a Glare situation");
			CCB->B_Chan = lo_chan;
			CCB->Int_Id[0] = int_id;
			CCB->Int_Id_Len = 0;
			CC_CHAN_SetStatepri(int_id, lo_chan, type, CHANNEL_BUSY, Dsl_Id);
			CC_CHAN_SetProtocolpri(int_id, lo_chan, type, L2P_LAPD, Dsl_Id);
			CC_CHAN_SetUserpri(int_id, lo_chan, type, CCB->Call_ID, Dsl_Id);
			cause = ACCEPTED;
		    } else if (event == CC_SETUP_IND) {	/* Consider negotiation */
			if (detail_debug)
			    buginf("\nCCPRI, Glare situation, checking negotiation");
                        /*
                         * Only attempt to do B-channel negotiation if it is not an
                         * exclusive channel request and not a slot map. The slot map cod
                         * doesn't work well because have to change the message around.
                         */
			if ((!CCB->Exclusive) && (IE_Fields[6] != CHANNEL_SLOT_MAP)) {
			    /*
			     * Go for an alternate channel.
			     */
			    /*
			     * Select another channel for proposal to the
			     * ISDN peer. Reset the int_id to the lowest one
			     * possible for the chan_type.  It is assumed
			     * that the D-channel is in the MIN_INT_IDP and
			     * thus this interface is unavailable for H11
			     * allocation.
			     */
                            if (detail_debug)
                                buginf ("\nCCPRI, Glare situation check alt ch")
;

		            int_id = 0;
			    switch (SwitchType) {
#ifdef PRI_DMS100
			      case PRI_DMS100_STYPE:
				while ((!CCB->B_Chan) && (int_id <= MAX_INT_IDP)) {
				    /*
				     * Only 23 chans in int_id 0. This will
				     * change a single D-channel supporting
				     * multiple int ===
				     */
				    hi_chan =
					(MultiInterface && (int_id == MIN_INT_IDP)) ?
					MAX_CHANNELP - 1 : MAX_CHANNELP;
				    /*
				     * User side (default) gets channels high
				     * to low (23 to 1); Networks side gets
				     * channels low to high (1 to 23).
				     */
				    if (Network_side) {
					if ((CCB->B_Chan = CC_CHAN_GetIdleChanpri(int_id,
										  MIN_CHANNEL, hi_chan, type, Dsl_Id)) == 0)
					    int_id++;
				    } else {
					if ((CCB->B_Chan = CC_CHAN_GetIdleDMSChan(int_id,
					MIN_CHANNEL, hi_chan, Dsl_Id)) == 0)
					    int_id++;
				    }
				}
				break;
#endif				/* PRI_DMS100 */
			      default:
				while ((!CCB->B_Chan) && (int_id <= MAX_INT_IDP)) {

				    /*
				     * User side (default) uses channels high
				     * to low (23 to 1); Networks side uses
				     * channels low to high (1 to 23).
				     * CC_CHAN_GeetIdleDMSChan does 23->1
				     * CC_CHAN_GeetIdleChanpri does 1->23
				     */
                                    if (Network_side) {
                                        if ((CCB->B_Chan = CC_CHAN_GetIdleDMSChan(int_id, MIN_CHANNEL, MAX_CHANNELP, Dsl_Id)) == 0)
                                            int_id++;
                                    } else {
                                        if ((CCB->B_Chan = CC_CHAN_GetIdleChanpri(int_id, MIN_CHANNEL, MAX_CHANNELP, CCB->ChanType, Dsl_Id)) == 0)
                                            int_id++;
                                    }
                                }
                                break;
			    }
			    if (CCB->B_Chan) {
                                CC_CHAN_SetUserpri(CCB->Int_Id[0], CCB->B_Chan,                                        CCB->ChanType, CCB->Call_ID, Dsl_Id);
                                CC_CHAN_SetProtocolpri(CCB->Int_Id[0],
                                  CCB->B_Chan, CCB->ChanType, L2P_LAPD, Dsl_Id);
                                CC_CHAN_SetStatepri(CCB->Int_Id[0], CCB->B_Chan,
                                         CCB->ChanType, CHANNEL_BUSY, Dsl_Id);
				CCB->Exclusive = TRUE;
				cause = ACCEPTED;
                                /*
                                 * Added this code for NET5 homologation
                                 * at KTL in the UK. The problem is that
                                 * the original Channel_IE is included in
                                 * the message and isdnintrfc.c decodes the
                                 * public not the private portion. Will find
                                 * the channel and zap it before it goes
                                 * to isdnintrfc.c
                                 * It will look something like:
                                 * 5 2 80 19 90 4 2 88 90 18 3 A1 83 81
                                 *              ^         ^^ ^       ^^
                                 *              BC      Chan Len    Chan B1
                                 */
                                for (ix=5; ix<InPkt->InfoLen; ) {
                                     if (InPkt->Info[ix] == CHANNEL_ID_IE) {
                                        index = ix + 1 + InPkt->Info[ix + 1];
                                        InPkt->Info[index] = 0x80 | CCB->B_Chan;
                                        break;
                                     } else {
                                        ix += 2 + InPkt->Info[ix + 1];
                                     }
                                }
			    } else {
			        if (detail_debug)
                                    buginf("\n CCPRI B-chan not found.");
				CCPRI_Error(CCPRI_ACCEPT_CHAN_ID, P_OUT_OF_CHANNELS,
					    type, FALSE);
				cause = NO_CIRCUIT_AVAILABLE;	/* Cause 34 */
			    }
			} else {
			    if (detail_debug)
				buginf("\nCCPRI, cannot negotiate channel, exclusive");
			    cause = REQ_CHANNEL_NOT_AVAIL;	/* Cause 44 */
			}
		    } else {	/* Channel is busy and we cannot negotiate
				 * further */
			cause = REQ_CHANNEL_NOT_AVAIL;	/* Cause 44 */
		    }
		} else {
		    /*
		     * Advance channel state (which may have been PROPOSED).
		     */
		    CC_CHAN_SetStatepri(CCB->Int_Id[0], CCB->B_Chan,
					CCB->ChanType, CHANNEL_BUSY, Dsl_Id);
		    cause = ACCEPTED;	/* No change of channel */
		}
		if ((cause == ACCEPTED) && (relay_result)) {
		    /*
		     * Pack result into the outgoing message for the HOST.
		     */
		    OutPkgInfo[OutPkgInfoLen++] = CHANNEL_ID_IE;
		    OutPkgInfo[OutPkgInfoLen++] = (MultiInterface) ? 4 : 3;
		    /*
		     * make sure that 0x20 is included, specifies a PRI
		     * interface
		     */
		    OutPkgInfo[OutPkgInfoLen++] = CCB->Exclusive | 0x20;
		    OutPkgInfo[OutPkgInfoLen++] = CCB->ChanType;	/* Typ */
		    OutPkgInfo[OutPkgInfoLen++] = CCB->B_Chan;	/* Chan */
		    if (MultiInterface)
			OutPkgInfo[OutPkgInfoLen++] = CCB->Int_Id[0];
		}
	    } else {		/* Channel out of range */
		switch (SwitchType) {
#ifdef PRI_4ESS
		  case PRI_4ESS_STYPE:
		  case PRI_NET5_STYPE:
		  case PRI_NTT_STYPE:
		  case PRI_TS014_STYPE:
		    cause = CHAN_DOES_NOT_EXIST;	/* Cause 82 */
		    break;
#endif
#ifdef PRI_5ESS
		  case PRI_5ESS_STYPE:
		    cause = CHAN_DOES_NOT_EXIST;	/* Cause 82 */
		    break;
#endif
#ifdef PRI_DMS100
		  case PRI_DMS100_STYPE:
		    cause = REQ_CHANNEL_NOT_AVAIL;	/* Cause 44 */
		    break;
#endif
#ifdef VN2
		  case VN2_STYPE:
		    cause = REQ_CHANNEL_NOT_AVAIL;	/* Cause 44 */
		    break;
#endif
#ifdef PRI_1TR6
		  case PRI_1TR6_STYPE:
		    cause = CALL_REJECTED_1TR6;	/* Cause 62 */
		    break;
#endif
		  default:
		    CCPRI_Error(CCPRI_ACCEPT_CHAN_ID, P_BAD_SWITCHTYPE,
				SwitchType, TRUE);
		    break;
		}
	    }
#else				/* Not CHANNEL_TABLE */
	    Trashcan = event;	/* Prevent compiler warning for unused parm */
	    CCB->B_Chan = lo_chan;	/* lo_chan & type imply hi_chan */
	    CCB->Exclusive = IE_Fields[2];
	    CCB->Int_Id[0] = int_id;
	    CCB->Int_Id_Len = int_id_len;
	    cause = ACCEPTED;
	    if (relay_result) {
		/*
		 * Pack result into the outgoing message for the HOST.
		 */
		OutPkgInfo[OutPkgInfoLen++] = CHANNEL_ID_IE;
		OutPkgInfo[OutPkgInfoLen++] = (MultiInterface) ? 4 : 3;
		OutPkgInfo[OutPkgInfoLen++] = CCB->Exclusive;	/* Excl */
		OutPkgInfo[OutPkgInfoLen++] = CCB->ChanType;	/* Typ */
		OutPkgInfo[OutPkgInfoLen++] = CCB->B_Chan;	/* Chan */
		if (MultiInterface)
		    OutPkgInfo[OutPkgInfoLen++] = CCB->Int_Id[0];
	    }
#endif				/* CHANNEL_TABLE */
	}
#ifdef VN2
	else if ((struct_err) && (SwitchType == VN2_STYPE)) {
	    cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	}
#endif
	else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }
bailout:
    return (cause);
}

/*********************
* CCPRI_AcceptCrNumb *
**********************
*
* Find and validate the contents of the CALLING NUMBER information
* element from the incoming L3 message.  We will not compare the caller
* number with a local number since it is a function for the HOST to see that
* the caller number is acceptable.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptCrNumb()
{
    int cause = ACCEPTED;	/* Cause for cause info element */
    int i;			/* Local counter */
    BOOLEAN struct_err = FALSE;	/* Element corrupted? */


    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     CALLER_NUMBER_IE, CODESET_0, 1);
    if (IE) {
#ifdef PRI_1TR6
	/* Do we have to add NET5 and NTT here, what about 5ESS ? */
	IE_Fields = CCIE_ExpandPartyNumb_IE(IE, &struct_err);
	if ((SwitchType == PRI_4ESS_STYPE) && (!struct_err) &&
	    (CCPRI_Match(Cr_Numb_Type, Cr_Numb_Type_Size, IE_Fields[0])) &&
	    (CCPRI_Match(Cr_Numb_Plan, Cr_Numb_Plan_Size, IE_Fields[1])))
#else
	IE_Fields = CCIE_ExpandPartyNumb_IE(IE, &struct_err);
	if ((!struct_err) &&
	    (CCPRI_Match(Cr_Numb_Type, Cr_Numb_Type_Size, IE_Fields[0])) &&
	    (CCPRI_Match(Cr_Numb_Plan, Cr_Numb_Plan_Size, IE_Fields[1])) &&
	    (CCPRI_Match(Cr_Prest_Ind, Cr_Prest_Ind_Size, IE_Fields[2])) &&
	    (CCPRI_Match(Cr_Scrn_Ind, Cr_Scrn_Ind_Size, IE_Fields[3])))
#endif
	{
	    cause = ACCEPTED;	/* IE appears OK */
	    if (OutPkgInfo[0] == CALL_INCOMING) {
		/*
		 * Put result into outgoing HOST message.
		 */
		OutPkgInfo[OutPkgInfoLen++] = CALLER_NUMBER_IE;
		OutPkgInfoLen++;/* Length is skipped for now */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[0];	/* Type */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[1];	/* Plan */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[2];	/* Pres ind */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[3];	/* Scrn ind */
		i = 4;
		while (IE_Fields[i])	/* Copy the individuaal digits */
		    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[i++];
		OutPkgInfo[OutPkgInfoLen - i - 1] = i;	/* Write length */
	    }
	}
#ifdef VN2
	else if ((struct_err) && (SwitchType == VN2_STYPE)) {
	    cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	}
#endif
	else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }

    return (cause);
}

/************************
* CCPRI_AcceptCrSubAddr *
*************************
*
* Find and validate the contents of the CALLING SUB-ADDR information
* element from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptCrSubAddr()
{
    int cause = ACCEPTED;	/* Cause for cause info element */
    int i;			/* Local counter */
    BOOLEAN struct_err = FALSE;	/* Element corrupted? */


    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     CALLER_SUB_ADDR_IE, CODESET_0, 1);
    if (IE) {
	IE_Fields = CCIE_ExpandPartyNumb_IE(IE, &struct_err);
	if ((!struct_err) &&
	    (CCPRI_Match(Cr_Sub_Type, Cr_Sub_Type_Size, IE_Fields[0]))) {
	    cause = ACCEPTED;	/* IE appears OK */
	    if (OutPkgInfo[0] == CALL_INCOMING) {
		/*
		 * Put result into outgoing HOST message.
		 */
		OutPkgInfo[OutPkgInfoLen++] = CALLER_SUB_ADDR_IE;
		OutPkgInfoLen++;/* Length is skipped for now */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[0];	/* Type */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[1];	/* Plan */
		i = 4;
		while (IE_Fields[i])	/* Copy the individuaal digits */
		    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[i++];
		OutPkgInfo[OutPkgInfoLen - i + 1] = i - 2;	/* Write length */
	    }
	}
#ifdef VN2
	else if ((struct_err) && (SwitchType == VN2_STYPE)) {
	    cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	}
#endif
	else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }

    return (cause);
}

/**********************
* CCPRI_AcceptDisplay *
***********************
*
* Find and validate the DISPLAY info element in the incoming
* message, recovering the characters. Needed for TS014 homologation.
*/
PUBLIC int CCPRI_AcceptDisplay ()
{
   int  ix;
   int  cause = NOERR;
 
   /*
    * The Display is mandatory in the Info IE.
    */
   IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen, DISPLAY_IE, CODESET_0, 1);
   if (IE) {
      if ((IE[1] > 0) && (IE[1] <= DISPLAY_IE_LEN)) {
         for (ix = 2; ix < IE[1]; ix++) {
            OutPkgInfo[OutPkgInfoLen++] = IE[ix];
            if (IE[ix] & 0x80)
                cause = INVALID_ELEM_CONTENTS;              /* Cause 100 */
         }
          OutPkgInfo[OutPkgInfoLen++] = 0;  /* string is zero terminated */
      } else {
         cause = INVALID_ELEM_CONTENTS;                     /* Cause 100 */
      }
   } else {
      /*
       * no Display IE, mandatory for Australia only.
       */
      if (SwitchType == PRI_TS014_STYPE) 
          cause = MANDATORY_IE_MISSING;
   }
   return(cause);
}

/*********************
* CCPRI_AcceptNetFac *
**********************
*
* Find and validate the contents of the NETWORK FACILITY information
* element from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case, the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptNetFac()
{
    BOOLEAN ok_so_far = TRUE;	/* Progress tag */
    int index = 0;		/* Array index */
    int cause;			/* Why this IE was not accepted */
    int id_len;			/* Network ID length */
    int i;			/* A counter */
    BYTE *ie_len;		/* Pointer to private IE length octet */
    BOOLEAN struct_err = FALSE;	/* IE structural error */


    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     NETWORK_FACILITY_IE, CODESET_0, 1);
    if (IE) {
	IE_Fields = CCIE_ExpandNetFac_IE(IE, &struct_err);
	id_len = IE_Fields[0];
#ifdef VN2
	if (SwitchType == VN2_STYPE) {
	    /*
	     * This is the NATIONAL_FACILITY_IE, which bears little
	     * similarity to the NETWORK_FACILITY_IE.  We have to handle VN3
	     * as a special case.
	     */
	    if (struct_err) {
		cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	    } else if (IE[2]) {	/* Check length of network ID */
		cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	    } else {
		cause = ACCEPTED;	/* No cause */
		/*
		 * Put result into outgoing HOST message.
		 */
		OutPkgInfo[OutPkgInfoLen++] = NATIONAL_FACILITY_IE;
		OutPkgInfo[OutPkgInfoLen++] = --id_len;	/* IE len */
		for (i = 1; i <= id_len; i++)
		    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[i];
	    }
	    goto bailout;
	} else
#endif
	if (IE_Fields[index++]) {	/* Network Identification is present */
	    if ((CCPRI_Match(Net_Id, Net_Id_Size, IE_Fields[index++])) &&
		(CCPRI_Match(Id_Plan, Id_Plan_Size, IE_Fields[index++])))
		index += (id_len - 1);	/* Skip network ID */
	    else
		ok_so_far = FALSE;
	} else {
	    index = 3;
	}

	if (IE_Fields[index])	/* Is the facility a service ? */
	    ok_so_far =		/* Test the list of defined services */
		((ok_so_far) &&
		 (CCPRI_Match(Servs, Servs_Size, IE_Fields[index + 1])));
	else			/* A feature */
	    ok_so_far =		/* Test the list of defined features */
		((ok_so_far) &&
		 (CCPRI_Match(Feats, Feats_Size, IE_Fields[index + 1])));

	if ((ok_so_far) && (!struct_err)) {
	    cause = ACCEPTED;	/* No cause */
	    /*
	     * Put result into outgoing HOST message.
	     */
	    OutPkgInfo[OutPkgInfoLen++] = NETWORK_FACILITY_IE;
	    ie_len = &OutPkgInfo[OutPkgInfoLen++];	/* Save len location */
	    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[index++];	/* feat/serv */
	    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[index];	/* fac code */
	    if (id_len) {	/* Network ID provided? */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[1];	/* ID type */
		OutPkgInfo[OutPkgInfoLen++] = IE_Fields[2];	/* ID plan */
		OutPkgInfo[OutPkgInfoLen++] = id_len;
		for (i = 1; i <= id_len; i++)
		    OutPkgInfo[OutPkgInfoLen++] = IE_Fields[2 + i];
	    }
	    *ie_len = (id_len) ? id_len + 5 : 2;
	} else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }

bailout:
    return (cause);
}

/*********************
* CCPRI_AcceptNotify *
**********************
*
* Find and validate the contents of the NOTIFY information element
* from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptNotify(notify_desc)
int *notify_desc;		/* Return place for notify description */
{
    BOOLEAN struct_err = FALSE;	/* Problem in IE coding */
    int cause;			/* Why this IE was not accepted */


    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen, NOTIFY_IND_IE, CODESET_0, 1);
    if (IE) {
	IE_Fields = CCIE_ExpandNotifyInd_IE(IE, &struct_err);
	if ((!struct_err) &&
	    (CCPRI_Match(Notify_Ind, sizeof(Notify_Ind), IE_Fields[0]))) {
	    cause = ACCEPTED;	/* No cause */
	    *notify_desc = IE_Fields[0];
	} else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }

    return (cause);
}

#ifdef PRI_1TR6
/*************************
* CCPRI_AcceptFacilities *
**************************
*
*/
PUBLIC int CCPRI_AcceptFacilities(facilities, user, nsf)
BYTE *facilities;		/* facility code is returned */
BOOLEAN user;			/* message from user side */
BOOLEAN nsf;			/* network specific facilities or service ind */

{

    int cause;
    BOOLEAN found = FALSE;
    BYTE *ptr;
    BYTE *code_ptr;
    BYTE *subtab;
    BYTE *service_ptr;
    cause = NOERR;

    if (!user) {
	IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
			 ((nsf) ? NETWORK_FACILITY_IE : FACILITY_SELECT_IE),
			 ((nsf) ? CODESET_0 : CODESET_6), 1);
	if ((IE) && (nsf)) {
	    code_ptr = &IE[3];
	    service_ptr = &IE[4];
	} else if ((IE) && (!nsf)) {
	    code_ptr = &IE[2];
	    service_ptr = &IE[3];
	}
    } else {
	IE = CCPRI_GetPrivate(((nsf) ? NETWORK_FACILITY_IE : FACILITY_SELECT_IE), CODESET_0);
	if (IE) {
	    code_ptr = &IE[2];
	    service_ptr = &IE[3];
	}
    }

    if (IE) {
	for (ptr = P1tr6_Facility_Code; (!found) && (ptr != P1tr6_Facility_Code + Facility_Code_Size); ptr++) {
	    if (*code_ptr == *ptr) {
		/*
		 * Check to see if service is within range
		 */
		if ((*service_ptr >= 0) & (*service_ptr < Max_Service_Octets))
		    subtab = P1tr6_Service_Ind[(int) *service_ptr];

		/*
		 * Its a hit if not null - However zero may be used for
		 * filler
		 */
		if ((subtab != NULL) || (*service_ptr == 0))
		    found = TRUE;
		*facilities = *ptr;
		cause = NOERR;
	    }
	}

	if (!found)
	    cause = INVALID_ELEM_CONTENTS;
    }
    return (cause);
}

#endif				/* PRI_1TR6 */
/**************************
* CCPRI_AcceptProgressInd *
***************************
*
* Find and validate the contents of the PROGRESS INDICATOR information
* element from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptProgressInd(prog_ind)
int *prog_ind;			/* Return place for progress indicator */

{
    int cause;			/* Cause for cause info element */
    BOOLEAN struct_err = FALSE;	/* IE structural error */


    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     PROGRESS_IND_IE, CODESET_0, 1);
    if (IE) {
	IE_Fields = CCIE_ExpandProgressInd_IE(IE, &struct_err);
	if ((IE_Fields[0] == 0) &&	/* CCITT coding standard */
	    (!struct_err) &&	/* No structural error in IE */
	    (CCPRI_Match(Prog_Location, Prog_Location_Size, IE_Fields[1])) &&
	    (CCPRI_Match(Prog_Desc, Prog_Desc_Size, IE_Fields[2]))) {
	    cause = ACCEPTED;	/* ProgressInd ie OK */
	    *prog_ind = IE_Fields[2];
	}
#ifdef VN2
	else if ((struct_err) && (SwitchType == VN2_STYPE)) {
	    cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	}
#endif
	else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }

    return (cause);
}

#ifdef PRI_1TR6
/*************************
* CCPRI_AcceptServiceInd *
**************************
*
*/
PUBLIC int CCPRI_AcceptServiceInd(service_description, addition_inf, user)
BYTE *service_description;
BYTE *addition_inf;
BOOLEAN user;			/* message from the user */

{
    int cause;
    BOOLEAN found = FALSE;
    BYTE Service_Octet;
    BYTE Additional_Inf_Octet;
    BYTE *ptr;
    BYTE *subtab;
    int j;

    cause = NOERR;

    if (!user)
	IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
			 SERVICE_INDICATOR_IE, CODESET_6, 1);
    else
	IE = CCPRI_GetPrivate(SERVICE_INDICATOR_IE, CODESET_0);

    if (IE) {
	Service_Octet = IE[2];
	Additional_Inf_Octet = IE[3];

	j = 0;
	subtab = ptr = P1tr6_Service_Ind[(int) Service_Octet];

	if ((subtab != NULL) &&
	  ((Service_Octet >= 0) && (Service_Octet <= Max_Service_Octets))) {
	    subtab++;
	    for (j = 0; (j < *ptr) && (!found); subtab++)
		if (*subtab == Additional_Inf_Octet) {
		    found = TRUE;
		    *service_description = Service_Octet;
		    *addition_inf = Additional_Inf_Octet;
		} else
		    j++;
	}
	if (!found)
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
    }
     /* End of IE present processing */ 
    else
	cause = CALL_REJECTED_1TR6;

    return (cause);
}

#endif				/* PRI_1TR6 */
/*********************
* CCPRI_AcceptSignal *
**********************
*
* Find and validate the contents of the SIGNAL INDICATOR information
* element from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* This function is currently useful only for French VN3 and returns
* cause = ACCEPTED in all other cases.
*
*/
PUBLIC int CCPRI_AcceptSignal()
{
    int cause;			/* Cause for bad info element */

    cause = 0;
#ifdef VN2
    if (SwitchType == VN2_STYPE) {
	IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
			 SIGNAL_IE, CODESET_0, 1);
	if (IE) {
	    if (IE[1] != 1) {
		cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	    } else {
		LIF_N_Cpy(&OutPkgInfo[OutPkgInfoLen], IE, 3);
		OutPkgInfoLen += 3;
	    }
	} else {
	    cause = MANDATORY_IE_MISSING;	/* Cause 96 */
	}
    } else {
	cause = ACCEPTED;
    }
#else
    cause = ACCEPTED;
#endif

    return (cause);
}

/***********************
* CCPRI_AcceptUserUser *
************************
*
* Find and validate the contents of the USER-USER information
* element from the incoming L3 message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
* Return the appropriate Q.931 cause value if the IE was found unacceptable.
* Return ACCEPTED if the IE was found acceptable.
*
*/
PUBLIC int CCPRI_AcceptUserUser ()
{
    int cause;			/* Cause for cause info element */
    /* int i, uui_to_move; */		/* Counters */
    BOOLEAN struct_err = FALSE;	/* Problem with this IE? */

    if (detail_debug)
	buginf("\nentering CCPRI_AcceptUserUser routine");

    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen, USER_USER_IE, CODESET_0, 1);
    if (IE) {
	IE_Fields = CCIE_ExpandUser_IE(IE, &struct_err);
#ifdef PRI_4ESS
	if (SwitchType == PRI_4ESS_STYPE)
	    struct_err = ((struct_err) ||
                     (!CCPRI_Match(Proto_Des, Proto_Des_Size, IE_Fields[1])));
#endif
	if (!struct_err) {
	    cause = ACCEPTED;	/* IE OK */
	    /*
	     * Pack result into outgoing HOST message.
	     * But this needs an application to fully test. Until
	     * we really need the UUI, do not send it up to the
	     * host application. Don't need it so don't copy it.
	     * The code that was here was clobbering memory.        
	     */
	}
#ifdef VN2
	else if (SwitchType == VN2_STYPE) {
	    cause = MANDATORY_IE_LEN_ERR;	/* Cause 103 */
	}
#endif
	else {
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	}
    } else {
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
    }
    return (cause);
}

#ifdef EXTENDED_HOST_MSG
/*******************
* CCPRI_CopyPublic *
********************
*
* Locate and copy the indicated CCITT (Q.931) compatible (public)
* information element from the HOST-created InPkgInfo to the OutPktInfo
* destined for the peer.  The HOST should have constructed the public
* elements in ascending numerical order within a codeset as per
* CCITT Rec. Q.931 with differences indicated for the specific switch type.
*
*/
PUBLIC void CCPRI_CopyPublic(info_element_id, codeset)
int info_element_id;		/* IE ID to locate */
int codeset;			/* codeset for IE */

{
    int len;			/* Length of public IE */
    BYTE dont_care;		/* Dumping ground */


    if ((IE = CCPRI_GetPublic(info_element_id, codeset)) != NULL) {
	/*
	 * The length of multi-octet IEs is the formal IE length (in octet 1)
	 * plus the identifier octet (0) plus the length octet itself (1).
	 */
	len = (CCIE_Single_Byte_IE(info_element_id,
				   &dont_care,
				   &dont_care)) ? 1 : IE[1] + 2;
	LIF_N_Cpy(&OutPkt->Info[OutPkt->InfoLen], IE, len);
	OutPkt->InfoLen += len;
    }
}

/**********************
* CCPRI_CopyPublicAll *
***********************
*
* Copy all CCITT (Q.931) compatible (public) information element from
* the HOST-created InPkgInfo to the OutPktInfo destined for the peer.
*
*/
PUBLIC void CCPRI_CopyPublicAll()
{
    if (Q931_IEs_Len > 0) {
	LIF_N_Cpy(&OutPkt->Info[OutPkt->InfoLen],
		  &Host_IEs[Host_IEs_Len],
		  Q931_IEs_Len);
	OutPkt->InfoLen += Q931_IEs_Len;
    }
}

/**************************
* CCPRI_CopyPublicShifted *
***************************
*
* Install the specified codeset shift IE in the outgoing message
* before copying a "public" element from the HOST message to the
* outgoing message.  Then have the shifted IE inserted.
*
*/
PUBLIC void CCPRI_CopyPublicShifted(info_element_id, codeset, lock)
int info_element_id;		/* IE ID to locate */
int codeset;			/* codeset for IE */
BOOLEAN lock;			/* Add as locking shift */
{
    if ((IE = CCPRI_GetPublic(info_element_id, codeset)) != NULL) {
	OutPkt->InfoLen +=
	    CCIE_BuildCodesetShift_IE(&OutPkt->Info[OutPkt->InfoLen],
				      codeset, lock);
	CCPRI_CopyPublic(info_element_id, codeset);
    }
}

#endif				/* EXTENDED_HOST_MSG */
/**************
* CCPRI_Error *
***************
*
* Perform error reporting and recovery if possible.
*
* Note that if "fatal" is TRUE, this will kill the entire ISDN machine.
* "fatal" is currently passed as TRUE for what are seen as hopeless
* failures, such as inability to get a memory resoure or inability to
* send mail.  In an actual product, these criteria should be reconsidered.
* The intention is that in the test and integration phase, it is probably
* desirable to stop everything immediately in such cases and locate and
* fix the problem, rather than letting the system move on and have the
* context lost.
*
*/
PUBLIC void CCPRI_Error(func, err, err_data, fatal)
int func;			/* Function where the error was detected */
int err;			/* Error detected */
int err_data;			/* Error data */
BOOLEAN fatal;			/* Fatal error? */

{
    extern void itoa(int, char*, int);

    static char err_text[512];	/* Err descrip text buffer (min) */
    static char err_func[128];	/* Err reporting func name buffer (min) */
    static char ascii_data[128];/* Numeric data buffer */

#ifdef DISPLAY_LOCAL_ERR
    /*
     * Define error function string.
     */
    STRCPY(err_func, "Unknown function reporting error");	/* Default */ 
    switch (func) {
      case CCPRI_ACCEPT_CHAN_ID:
	STRCPY(err_func, "CCPRI_AcceptChanId");
	break;
      case CCPMSG_CHAN_STATUS:
	STRCPY(err_func, "CCMSG_ChanStatus");
	break;
      case CCPRI_FAC_ACK_OUT:
	STRCPY(err_func, "CCPRI_FacAckOut");
	break;
      case CCPRI_GET_L3_EVENT:
	STRCPY(err_func, "CCPRI_GetL3Event");
	break;
      case CCPRI_GET_NEW_CCB:
	STRCPY(err_func, "CCPRI_GetNewCCB");
	break;
      case CCPRI_GET_OUT_INFO:
	STRCPY(err_func, "CCPMSG_GetOutInfo");
	break;
      case CCPRI_GET_OUT_PKT:
	STRCPY(err_func, "CCPRI_GetOutPkt");
	break;
      case CCPRI_GO:
	STRCPY(err_func, "CCPRI_Go");
	break;
      case CCPRI_INIT:
	STRCPY(err_func, "CCPRI_Init");
	break;
      case CCPRI_MAIL:
	STRCPY(err_func, "CCPRI_Mail");
	break;
      case CCPMSG_OUT_CALL:
	STRCPY(err_func, "CCPMSG_OutCall");
	break;
      case CCPRI_RELEASE_CALL:
	STRCPY(err_func, "CCPRI_ReleaseCall");
	break;
      case CCPMSG_RESTART:
	STRCPY(err_func, "CCPMSG_Restart");
	break;
      case CCPMSG_RESTART_REQ:
	STRCPY(err_func, "CCPMSG_RestartReq");
	break;
      case CCPRI_RESIZE_INFO:
	STRCPY(err_func, "CCPRI_ResizeInfo");
	break;
      case CCPCC_CALL_CONNECTED:
	STRCPY(err_func, "CCPCC_CallConnected");
	break;
      case CCPCC_CALL_IDLE:
	STRCPY(err_func, "CCPCC_CallIdle");
	break;
      case CCPCC_CALL_OFFERED:
	STRCPY(err_func, "CCPCC_CallOffered");
	break;
      case CCPCC_CALL_ORIGINATION:
	STRCPY(err_func, "CCPCC_CallOrigination");
	break;
      case CCPCC_CALL_RELEASING:
	STRCPY(err_func, "CCPCC_CallReleasing");
	break;
      case CCPCC_CALL_RINGING:
	STRCPY(err_func, "CCPCC_CallRinging");
	break;
      case CCPCC_CALL_IN_ROUTING:
	STRCPY(err_func, "CCPCC_CallRoutingIn");
	break;
      case CCPCC_CALL_OUT_ROUTING:
	STRCPY(err_func, "CCPCC_CallRoutingOut");
	break;
      default:
	break;
    }

    /*
     * Define error text string.
     */
    switch (err) {
      case P_SEND_PKT_FAILED:
	STRCPY(err_text, "LIF_SendPkt returned failure: ");
	break;
      case P_SEND_BUF_FAILED:
	STRCPY(err_text, "Couldn't send a package to the HOST: ");
	break;
      case P_UNKNOWN_EVENT:
	STRCPY(err_text, "Unknown event received in message from L3: ");
	break;
      case P_NO_CCB:
	STRCPY(err_text, "Out of CCB memory blocks: ");
	break;
      case P_UNKNOWN_CALL:
	STRCPY(err_text, "Event received for an unrecognized call: ");
	break;
      case P_UNKNOWN_MSG_SOURCE:
	STRCPY(err_text, "Event received from an unrecognized task ID : ");
	break;
      case P_NULL_CCB:
	STRCPY(err_text, "Function found CCB = NULL unexpectedly: ");
	break;
      case P_OUT_OF_INFO_BUFFERS:
	STRCPY(err_text, "Unable to get a package info buffer: ");
	break;
      case P_OUT_OF_PKTS:
	STRCPY(err_text, "Unable to get a packet for mail: ");
	break;
      case CCPRI_INITIALIZATION_FAILED:
	STRCPY(err_text, "Module initialization failed: ");
	break;
      case P_CCB_NOT_FOUND:
	STRCPY(err_text, "CCB to be removed was not found on list: ");
	break;
      case P_OUT_OF_CHANNELS:
	STRCPY(err_text, "Cannot get an idle channel from chan mgt: ");
	break;
      case P_BAD_DATA_FROM_HOST:
	STRCPY(err_text, "Bad data in HOST message to CC: ");
	break;
      case P_DUMMY_CCB_FOUND:
	STRCPY(err_text, "Dummy CCB found in improper code section: ");
	break;
      case P_BAD_SWITCHTYPE:
	STRCPY(err_text, "Unexpected switchtype assigned to SwitchType: ");
	break;
      case P_RESTART_ERR:
	STRCPY(err_text, "Internal error in processing a restart: ");
	break;
      case P_ADD_TIMER_FAILED:
	STRCPY(err_text, "LIF_AddTimer returned an error: ");
	break;
      case P_UPDATE_TIMER_FAILED:
	STRCPY(err_text, "LIF_UpdateTimer returned an error: ");
	break;
      case P_INTERFACE_ID_OVERFLOW:
	STRCPY(err_text, "Chan ID Interface ID len exceeds CCB storage: ");
	break;
      case P_DSL_OUT_OF_RANGE:
	STRCPY(err_text, "Bad Digital Subscriber Loop number: ");
	break;
      default:
	STRCPY(err_text, "Unrecognized error code - CCPRI_Error: ");
	break;
    }

    /* Catenate error number on to end of description */
    itoa(err_data, ascii_data, 16);
    strcat(err_text, ascii_data);

    if (fatal) {
        if (detail_debug)
            buginf("\n calling LIF_fatal");
	LIF_Fatal("CCPRI", err_func, err_text);
    } else {
        if (detail_debug)
            buginf("\n calling lif_error");
	LIF_Error("CCPRI", err_func, err_text);
    }

#else				/* NOT DISPLAY_LOCAL_ERR */

    if (fatal) {
        if (detail_debug)
            buginf("\n calling lif fatal");
        /* Catenate error number on to end description */
	itoa(err_data, ascii_data, 16);
	strcat(err_text, ascii_data);
	LIF_Fatal("CCPRI", err_func, err_text);
    } else {
        if (detail_debug)
            buginf("\n calling lif_error routine");
       LIF_Error("CCPRI", err_func, err_text);
    }
       
#endif
}

/********************
* CCPRI_FreePkgInfo *
*********************
*
* Release package info for a message to the HOST because the message
* is not to be sent after all.
*
*/
PUBLIC void CCPRI_FreePkgInfo()
{
    LIF_FreeBuffer(LRG_INFO_BTYPE, OutPkgInfo);
    OutPkgInfo = NULL;
    OutPkgInfoLen = 0;
}
#ifdef PRI_1TR6
/*******************
CCPRI_GetCharges   *
********************
*
* Extract Charges. If a charge IE is invalid (not ASCII digit), then zero
* length is returned to caller.
*
*/
PUBLIC int CCPRI_GetCharges()
{
    int cause = NOERR;
    int j = 0;
    int IE_index;
    int save_len;
    BOOLEAN gut = TRUE;

    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     CHARGING_INFORMATION_IE, CODESET_6, 1);

    save_len = OutPkgInfoLen;
    if (IE) {
	IE_index = 3;
	OutPkgInfo[OutPkgInfoLen++] = CHARGING_INFORMATION_IE;
	j = OutPkgInfoLen++;
	while ((IE_index < (IE[1] + 2)) && gut) {
	    if ((IE[IE_index] >= '0') & (IE[IE_index] <= '9'))
		OutPkgInfo[OutPkgInfoLen++] = IE[IE_index++];
	    else
		gut = FALSE;
	}
	OutPkgInfo[j] = IE[1] - 1;	/* IE length */
    }
    if (!gut) {
	cause = INVALID_ELEM_CONTENTS;
	OutPkgInfoLen = save_len;
#ifdef DISPLAY_LOCAL_ERR
	LIF_Error("CCPRI", "CCPRI_GetCharges", "Invalid Charges Character");
#endif
    }
    return cause;
}
/*******************
CCPRI_GetDate    *
********************
*
* Extract Date. If a date octet is invalid, zero length is returned to
* caller. This routine is not extremely rigorous, but it does screen
* bad characters, which is the degree of validation  1 TR20 seems to call
* for.
*/
PUBLIC int CCPRI_GetDate()
{

    int cause = NOERR;
    BOOLEAN gut = TRUE;
    int IE_index;
    BYTE *ie_ptr;
    int j;

    IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
		     DATE_IE, CODESET_6, 1);

    if ((IE) && (IE[1] > 0)) {
	for (ie_ptr = &IE[2]; (ie_ptr < (&IE[2] + DATE_IE_LEN)) & (gut); ie_ptr++) {
	    switch (ie_ptr - IE) {
	      case 2:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '3'))	/* Tag */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 3:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '9'))	/* Tag */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 4:
		if ((*ie_ptr == '.'))
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 5:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '1'))	/* Monat */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 6:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '9'))	/* Monat */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 7:
		if ((*ie_ptr == '.'))
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 8:
		if ((*ie_ptr = '9') & (*ie_ptr = '0'))	/* Year 9n-0n */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 9:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '9'))	/* Year */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 10:
		if ((*ie_ptr == '-'))
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 11:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '2'))	/* Std */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 12:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '9'))	/* Std */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 13:
		if ((*ie_ptr == ':'))
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 14:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '5'))	/* Min */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 15:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '9'))	/* Min */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 16:
		if ((*ie_ptr == ':'))
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 17:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '5'))	/* Sek */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      case 18:
		if ((*ie_ptr >= '0') & (*ie_ptr <= '9'))	/* Sek */
		    break;
		else {
		    gut = FALSE;
		}
		break;
	      default:
		break;
	    }			/* End of Case Statement */
	}			/* End of For Loop */

	if (gut) {
	    IE_index = 2;
	    OutPkgInfo[OutPkgInfoLen++] = DATE_IE;
	    j = OutPkgInfoLen++;
	    while (IE_index < IE[1] + 2)
		OutPkgInfo[OutPkgInfoLen++] = IE[IE_index++];
	    OutPkgInfo[j] = IE[1];
	} else {
	    cause = INVALID_ELEM_CONTENTS;
#ifdef DISPLAY_LOCAL_ERR
	    LIF_Error("CCPRI", "CCP1TR6_GetDate", "Invalid Date Character");
#endif
	}
    } else {
	cause = MANDATORY_IE_MISSING;
    }

    return cause;
}
/************************
CCPRI_GetFacilities     *
*************************
*
* Extract Network Specific Facilities or Facilties Select. When you
* find it, attach it to the Host message normalized back to NSF format.
*
*/
PUBLIC int CCPRI_GetFacilities(nsf)
BOOLEAN nsf;			/* Either Network Spec Fac or Select */
{
    int cause;
    int j;
    int IE_index;
    BYTE facilities;

    if (((cause = CCPRI_AcceptFacilities(&facilities, FALSE,
			       ((nsf) ? TRUE : FALSE))) == NOERR) && (IE)) {
	OutPkgInfo[OutPkgInfoLen++] = NETWORK_FACILITY_IE;
	j = OutPkgInfoLen++;
	if (nsf) {
	    OutPkgInfo[OutPkgInfoLen++] = IE[3];	/* Fac Code */
	    OutPkgInfo[OutPkgInfoLen++] = IE[4];	/* Service */
	    OutPkgInfo[OutPkgInfoLen++] = IE[5];	/* additional info */
	    IE_index = 6;
	    while (IE_index < (IE[1] + 2))	/* Pick up Parafelder */
		OutPkgInfo[OutPkgInfoLen++] = IE[IE_index++];
	    OutPkgInfo[j] = IE[1] - 1;	/* length of Private IE */
	} else {
	    OutPkgInfo[OutPkgInfoLen++] = IE[2];	/* Fac Code */
	    OutPkgInfo[OutPkgInfoLen++] = IE[3];	/* Service */
	    OutPkgInfo[j] = 2;	/* length of Private IE */
	}
    }
    return cause;

}

#endif

/********************
* CCPRI_GetDummyCcb *
*********************
*
* Initialize and return a pointer to the DummyCcb used only to reject
* setups, for example, when we are out of real CCBs.
*
*/
PUBLIC CCB_ID CCPRI_GetDummyCcb(call_id)
int call_id;			/* Call ID for the Dummy */

{
    Dummy_Ccb.Call_ID = (call_id) ? call_id : CCPRI_GetNewCall_Id();

    return (&Dummy_Ccb);
}

/*******************
* CCPRI_GetL3Event *
********************
*
* Recover the message type from the incoming message from Layer 3
* Protocol Control.  Event = 0 is returned for unrecognized messages.
* Also recover the call reference and its length (*cr and *cr_len).
*
*/
PRIVATE int near CCPRI_GetL3Event(prim_id, cr, cr_len)
int prim_id;			/* Primitive ID of the incoming message */
int *cr;			/* Call reference */
int *cr_len;			/* Its length */

{
    int event;			/* The result */

    event = 0;
    /*
     * Check for notifications tied to a primitive.
     */
    if ((prim_id == NL_MAINT_IND) ||
	(prim_id == NL_EST_CONF) ||
	(prim_id == NL_REL_IND) ||
	(prim_id == MNL_RESET_REQ) ||
	(prim_id == NL_UI_DATA_IND)) {
	event = prim_id;	/* The event is the prim_id at this level */
    }
    /*
     * Check if we have a complete packet and info.
     */
    else if ((InPkt) && (InPkt->Info)) {
	*cr_len = InPkt->Info[1];
	if (*cr_len == 1) {	/* Short call reference */
	    *cr = InPkt->Info[2];
	    event = (int) InPkt->Info[3];
	} else {		/* Long call reference */
	    *cr = (InPkt->Info[2] << 8) + InPkt->Info[3];
	    event = (int) InPkt->Info[4];
	}

	if (GLOBAL_CALL_REF(*cr, *cr_len)) {
	    if ((event != CC_RESTART_IND) &&
		(event != CC_RESTART_CONF) &&
		(event != CC_ERROR_IND))
		event = UNKNOWN_EVENT;
	}
    } else {
	CCPRI_Error(CCPRI_GET_L3_EVENT, P_UNKNOWN_EVENT, prim_id, TRUE);
    }

    return (event);
}

/**********************
* CCPRI_GetNewCall_Id *
***********************
*
* Each Call_ID in use on a DSL must be unique.
* Return a new (unique) call_id for the new call.
*
*/
PRIVATE int near CCPRI_GetNewCall_Id()
{
    CCB_ID ccb;
    BOOLEAN sure = FALSE;	/* Not sure the Call_ID is unique on the DSL */
    int iy = SEARCH_CNT_INIT, jy = SEARCH_CNT_INIT;

    /*
     * It will be prudent to verify that this new call ID is not in use at
     * present (by looking at call_id in all CCB's in use on this DSL).
     */

    Call_Id_Source++;

    while (!sure) {
	/*
	 * Prepare new call_id value by allowing source of all call_id's to
	 * increment or wrap at 1.  call_id = 0 is reserved by L3 as the
	 * "non-call" call_id.  The upper range of Call_IDs belongs to the
	 * HOST for its Call_ID assignments (0x8000-0xFFFF).
	 */
	if ((Call_Id_Source == PRI_CALL_ID_MIN) || (Call_Id_Source >= PRI_CALL_ID_MAX))
	    Call_Id_Source = PRI_CALL_ID_MIN+1;

	ccb = CCB_Head[Dsl_Id];	  /* Start checking call_id at CCB head */
	jy = SEARCH_CNT_INIT;

	while (ccb) {
	    if (ccb->Call_ID == Call_Id_Source) {   /* Found a conflict */

		if (isdn_debug || isdn_debug_ok(q931_debug, Dsl_Id))
		   buginf("\nISDN %s: Event: call id already in use: 0x%x",
		          DSL_TO_NAME(Dsl_Id), Call_Id_Source);

		Call_Id_Source++;               /* Try the next call_id */
		iy++;
		if (iy > SEARCH_CNT_MAX) {
		   buginf("\nISDN %s: Error: call id run away: 0x%x",
		          DSL_TO_NAME(Dsl_Id), Call_Id_Source);
		   ccb = NULL;
		   Call_Id_Source = PRI_CALL_ID_MIN;
		}
		break;
	    } else {
	        ccb = ccb->Next;            /* Try next CCB in the list */
	        jy++;
	        if (jy > SEARCH_CNT_MAX) {
		   buginf("\nISDN %s: Error: CCB run away: 0x%x",
		          DSL_TO_NAME(Dsl_Id), ccb);
		   ccb = NULL;
		   Call_Id_Source = PRI_CALL_ID_MIN;
	        }
	    }
	}
	if (ccb == NULL)
	   sure = TRUE;	                  /* End of CCB list w/no match */
    }

    if (isdn_debug || isdn_debug_ok(q931_debug, Dsl_Id))
       buginf("\nISDN %s: Incoming call id = 0x%x",
              DSL_TO_NAME(Dsl_Id), Call_Id_Source);

    return (Call_Id_Source);
}

/******************
* CCPRI_GetNewCCB *
*******************
*
* Get a new Call Control Block and initialize it, adding it to the
* linked list of CCB's.   Return a NULL pointer if the CCB could not
* be recovered.
*
*/
PUBLIC CCB_ID CCPRI_GetNewCCB(call_id)
int call_id;			/* =0 if this is a call where we assign Call
				 * ID */
{
    CCB_ID ccb;			/* Temporary ccb pointer */

    ccb = malloc_named(sizeof(struct CallControlBlock), "isdn-ccb");
    if (ccb) {
	/*
	 * Get the correct initial state for the CCB from the client call
	 * state module.
	 * 
	 * Init the CCB_State to the single call control module ===
	 */
	ccb->PriState = CCPCC_InitialState();	/* CCB state */


	/*
	 * Finish initializing the CCB.
	 */
	ccb->Call_ID = (call_id) ? call_id : CCPRI_GetNewCall_Id();
	ccb->Next = NULL;	/* Next CCB */
	ccb->B_Chan = 0;	/* No b_chan initially assigned */
	ccb->ChanType = 0;	/* No b_chan initially assigned */
	ccb->Exclusive = FALSE;	/* Exclusive channel */
	ccb->CallRef = 0;	/* Call reference */
	switch (SwitchType) {
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    ccb->CallRefLen = PRI_1TR6_STD_CALL_REF_LEN;	/* 1 byte */
	    break;
#endif
	  case PRI_NET5_STYPE:
          case PRI_NTT_STYPE:
            /*
             * Do not use the interface identifier in the channle ie.
             */
	    ccb->Int_Id_Len = 0;/* Interface ID len */
	    break;
	  default:
	    ccb->Int_Id_Len = 1;/* Interface ID len */
	    break;
	}
	ccb->CallRefLen = PRI_STD_CALL_REF_LEN;	/* 2 bytes */
	ccb->Int_Id[0] = 0;	/* Channel ID Interface ID */
	ccb->Congested = FALSE;	/* No UUI congestion condition */
	ccb->ClearHost = FALSE;	/* Too early to say */
#ifdef PRI_1TR6
	if (SwitchType == PRI_1TR6_STYPE) {
	    ccb->Ics = 0;	/* channel select */
	    ccb->DM_Substate = FALSE;	/* FAC outstanding */
	    ccb->ChanType = B_CHANNEL_UNITS;	/* Only single B chans used */
	}
#endif
#ifdef VN2
	ccb->Responded = FALSE;	/* CC_BROADCAST_RESP yet to be sent */
#endif

	/*
	 * Add new CCB to the list of active CCB's.
	 */
	if (CCB_Head[Dsl_Id])	/* One or more others already present. */
	    CCB_Tail[Dsl_Id]->Next = ccb;
	else
	    CCB_Head[Dsl_Id] = ccb;
	CCB_Tail[Dsl_Id] = ccb;
	if (detail_debug)
	    buginf("\nCCPRI_GetNewCCB - New CCB recovered");
	ActiveCCBs[ Dsl_Id ]++;  /* Increment number used per interface */
	AllocatedCCBs++;
    } else {
	if (detail_debug)
	    buginf("\nCCPRI, CCPRI ERROR, couldn't allocate CCB");
	CCPRI_Error(CCPRI_GET_NEW_CCB, P_NO_CCB, AllocatedCCBs, FALSE);
    }

    return (ccb);
}

/*******************
* CCPRI_GetPrivate *
********************
*
* Find an indicated private (non-Q.931 compatible) information element
* from the HOST-created InPkgInfo.
*
* Return a pointer to the element if found.  If it is not found or the
* search is not attempted, return a NULL pointer.
*
*/
PUBLIC BYTE *CCPRI_GetPrivate(info_element_id, codeset)
int info_element_id;		/* IE ID to locate */
int codeset;			/* Codeset for IE */

{
    return ((Host_IEs_Len > 0) ? CCIE_GetAnyIE(Host_IEs,
					       Host_IEs_Len,
					       info_element_id,
					       codeset,
					       1) : NULL);
}

#ifdef EXTENDED_HOST_MSG
/******************
* CCPRI_GetPublic *
*******************
*
* Find an indicated CCITT (Q.931) compatible (public) information element
* from the HOST-created InPkgInfo.
*
* Return a pointer to the element if found.  If it is not found or the
* search is not attempted, return a NULL pointer.
*
*/
PUBLIC BYTE *CCPRI_GetPublic(info_element_id, codeset)
int info_element_id;		/* IE ID to locate */
int codeset;			/* codeset for IE */

{
    return ((Q931_IEs_Len > 0) ? CCIE_GetAnyIE(&Host_IEs[Host_IEs_Len],
					       Q931_IEs_Len,
					       info_element_id,
					       codeset,
					       1) : NULL);
}

#endif				/* EXTENDED_HOST_MSG */
/*******************
* CCPRI_GetOutInfo *
********************
*
* Recover and initialize the outgoing package info buffer for outgoing
* mail (package) to the HOST. Make the PRI messages match the BRI
* messages.
*
*/
PUBLIC BOOLEAN CCPRI_GetOutInfo(msg_type)
int msg_type;			/* Message identifier for the message to be
				 * built */
{

    if (OutPkgInfo)
	CCPRI_FreePkgInfo();

    if ((OutPkgInfo = LIF_GetBuffer(LRG_INFO_BTYPE, TRUE)) != NULL) {
	OutInfoBufType = LRG_INFO_BTYPE;
	OutPkgInfoLen = 0;	/* Length so far */
	OutPkgInfo[OutPkgInfoLen++] = msg_type;	/* Message type */
	OutPkgInfo[OutPkgInfoLen++] = Dsl_Id;	/* DSL ID */
	OutPkgInfo[OutPkgInfoLen++] = 1;	/* CES === */
	if (CCB) {
	   OutPkgInfo[OutPkgInfoLen++] = (BYTE) ((CCB->Call_ID >> 8) & 0xFF);	/* Call_ID */
	   OutPkgInfo[OutPkgInfoLen++] = (BYTE) (CCB->Call_ID & 0xFF);	/* Call_ID LSB */
	} else {
	   OutPkgInfo[OutPkgInfoLen++] = 0;	/* Call_ID */
	   OutPkgInfo[OutPkgInfoLen++] = 0;	/* Call_ID LSB */
	}
	/*
	 * The multipri logic will be revisited when the D-channel controls
	 * multiple interfaces. ===
	 */
#ifdef 0
	if (MultiPRI)
	    OutPkgInfo[OutPkgInfoLen++] = Dsl_Id;	/* Add Dsl_Id */
#endif
#ifdef EXTENDED_HOST_MSG
	OutPkgInfoLen++;	/* Leave space for private elements length */
#endif
    } else {
	CCPRI_Error(CCPRI_GET_OUT_INFO, P_OUT_OF_INFO_BUFFERS, 0, TRUE);
    }

    return (OutPkgInfo != NULL);
}

/******************
* CCPRI_GetOutPkt *
*******************
*
* Recover and initialize the outgoing packet for outgoing mail.
*
* Note: The value written here for call reference only has validity
*       for this Call Control's initial responses to the CC_SETUP_IND
*       message.  For subsequent messages the value is not guaranteed.
*       Once the call setup cycle is underway with Layer 3 Protocol Control,
*       the call is identified at the interface by the Call_ID which is
*       passed in the message primitive through calls to the function
*       LIF_SendPkt in CCPRI_Mail.  Thus after the initial response the
*       CC_SETUP_IND, the call reference is irrelevant at this interface.
*
* Return TRUE is the packet was fetched sucessfully.
*
*/
PUBLIC BOOLEAN CCPRI_GetOutPkt(msg_type)
BYTE msg_type;			/* Message type to apply to the new buffer */

{
    if (OutPkt)
	LIF_FreePkt(OutPkt);

    if ((OutPkt = LIF_GetPkt(LRG_INFO_BTYPE, DATA_OFFSET, TRUE)) != NULL) {
	OutPkt->InfoType = LRG_INFO_BTYPE;
	OutPkt->HeadLen = DATA_OFFSET;	/* 4 */
	OutPkt->Info[0] = Qdot931;	/* Protocol discriminator 0x08 */
	if (CCB && CCB->CallRefLen == 2) {
	    OutPkt->Info[1] = 2;/* CR length = 2 */
	    OutPkt->Info[2] = (BYTE) ((CCB->CallRef >> 8) & 0xFF);	/* Cref */
	    OutPkt->Info[3] = (BYTE) (CCB->CallRef & 0xFF);	/* Cref */
	    OutPkt->Info[4] = msg_type;	/* Message ID */
	    OutPkt->InfoLen = 5;/* Leave space for L3 header */
	} else {
	    OutPkt->Info[1] = 1;/* CR length = 1 */
            if (CCB)
	        OutPkt->Info[2] = (BYTE) CCB->CallRef;	/* Call reference */
            else
	        OutPkt->Info[2] = (BYTE) NULL;	/* Call reference */
	    OutPkt->Info[3] = msg_type;	/* Message ID */
	    OutPkt->InfoLen = 4;/* Leave space for L3 header */
	}
    } else {
	CCPMSG_SendHostProgress(INTERNAL_ERROR, FALSE, FALSE);
	CCPRI_Error(CCPRI_GET_OUT_PKT, P_OUT_OF_PKTS, 0, TRUE);
    }

    return (OutPkt != NULL);
}

/***********
* CCPRI_Go *
************
*
* Entry point to the module.  The outer layer of message processing.
* Decode the source of the message and recover the CCB for the call.
* Dispatch the incoming message to the state of the referenced call.
*
*/
PUBLIC void CCPRI_Go(source_id, event, pkt, call_id, source_state,
		         dsl_id, timer_int_id)
int source_id;			/* Message source task */
int event;			/* Driving event */
PKT_ID pkt;			/* Message packet */
int call_id;			/* Call identifier */
int source_state;		/* State of call in sending task */
int dsl_id;			/* Interface identifier */
BYTE timer_int_id;		/* Interface ID (timeout only) */

{
    PKG_ID in_pkg;		/* Incoming package pointer */
    int cr;			/* Call reference from L3 */
    int cr_len;			/* Call reference length */


    /*
     * The incoming messages will be treated differently based on the message
     * source.
     */
    if (detail_debug)
	buginf("\nISDN %s: CCPRI, entering CCPRI_Go, the call-id = %x, event = %x", DSL_TO_NAME(dsl_id), call_id, event);

    switch (source_id) {
      case HOST_ID:		/* The HOST application function */
	in_pkg = (PKG_ID) pkt;	/* All the mail is sent in packages */
	if (in_pkg) {
	    if (in_pkg->PkgInfo) {
		event = in_pkg->PkgInfo[0];	/* Save msg_type */
		Dsl_ID = in_pkg->PkgInfo[1];	/* Save dsl_id */
		call_id = ((in_pkg->PkgInfo[3]) << 8) + (in_pkg->PkgInfo[4]);

		Host_IEs = in_pkg->PkgInfo + AddrLen;
#ifdef EXTENDED_HOST_MSG
		Host_IEs_Len = in_pkg->PkgInfo[PrivInfoLenOffset];
		Q931_IEs_Len = in_pkg->InfoLen - Host_IEs_Len - AddrLen;
#else
		Host_IEs_Len = in_pkg->InfoLen - AddrLen;
#endif
		Dsl_Id = in_pkg->PkgInfo[1];	/* === */
		if (event == CHAN_STATUS) {
		    /*
		     * This message is a special case, not requiring a
		     * call-associated CCB to be located.
		     */
		    CCPMSG_ChanStatus();
		    goto bailout;	/* All done */
		} else if ((event == CALL_OUTGOING) ||	/* New call */
		    /*
		     * Special European facility states
		     */
			   (((event == CALL_FACILITY_REGISTER) ||
			     (event == CALL_FACILITY_CANCEL)) &&
			    (SwitchType == PRI_1TR6_STYPE))) {
		    if ((CCB = CCPRI_GetNewCCB(call_id)) == NULL) {
			/*
			 * We have to tell the HOST to back off since we are
			 * not able to deal with the call without a new CCB.
			 */
			CCB = CCPRI_GetDummyCcb(call_id);
			CCPMSG_RejectOutCall(TEMPORARY_FAILURE, FALSE);
			if (CCPRI_GetOutInfo(CALL_CLEARED))
			    CCPRI_Mail(HOST_ID, FALSE);
			goto bailout;
		    }
		} else {
		    /*
		     * Search the linked list of CCBs for matching call_id.
		     */
		    CCB = CCB_Head[Dsl_Id];
		    while (CCB && (CCB->Call_ID != call_id)) {
			CCB = CCB->Next;
		    }
		}
		if (CCB) {
		    CCPRI_ENTER_STATE((int) event);	/* Dispatch the event */
		} else {
		    CCPRI_Error(CCPRI_GO, P_UNKNOWN_CALL, event, FALSE);
		}
	    }
	}
	break;

      case L3_ID:		/* ISDN Layer 3 Protocol Control */
	Dsl_Id = dsl_id;	/* === Save dsl_id */
	InPkt = pkt;		/* Save incoming packet */
	SourceState = source_state;	/* Save L3 state - error recovery */
	event = CCPRI_GetL3Event(event, &cr, &cr_len);
	switch (event) {
	  case CC_SETUP_IND:
	    /*
	     * The call reference is saved from the CC_SETUP_IND. It must be
	     * preserved on a call-basis in the CCB because there is an
	     * indeterminate dislocation in the processing of the call setup
	     * when it is offered to the HOST and before the HOST responds.
	     * The CC_SETUP_IND cref is recovered in CCPRI_GetL3Event().
	     */

	    if ((CCB = CCPRI_GetNewCCB(call_id)) != NULL) {
		if (detail_debug)
		    buginf("\nCCPRI, incoming Setup, after CCB check call_id = %x and CCB->Call_ID = %x, AllocatedCCBs = %x", call_id, CCB->Call_ID, AllocatedCCBs);
		CCB->CallRef = cr;
		CCB->CallRefLen = cr_len;
		CCPRI_ENTER_STATE(CC_SETUP_IND);
	    } else {		/* CCB allocation or initialization failure. */
		/*
		 * We have to tell the NETWORK to back off since we are not
		 * able to deal with the call.
		 */
		CCB = CCPRI_GetDummyCcb(0);
		CCPMSG_RejectInCall(CALL_REJECTED, 0);
		CCPMSG_SendHostProgress(INTERNAL_ERROR, FALSE, FALSE);
	    }
	    break;

	  case NL_MAINT_IND:	/* Check for chan maint service msg. */
	    CCPRI_ServMsg();
	    break;

	  case CC_RESTART_CONF:
	  case CC_RESTART_IND:
	    CCPMSG_Restart(event, dsl_id);
	    break;

	  case CC_TIMEOUT_IND:
	    while (CCB && (CCB->Call_ID != call_id))
		CCB = CCB->Next;
#ifdef PRI_1TR6
	    if (SwitchType == PRI_1TR6_STYPE) {
		if (CCB) {
		    CCB->ClearHost = TRUE;
		    CCPRI_ReleaseCall();
		}
	    } else {		/* Not 1TR6 SsitchType */
		if (CCB)
		    CCPRI_ReleaseFailure();
	    }
#else				/* Not 1TR6 */
	    if (CCB)
		CCPRI_ReleaseFailure();
#endif
	    break;

	  case MNL_RESET_REQ:
	    if (isdn_debug_ok(q921_debug || q931_debug, dsl_id) || isdn_debug)
		buginf("\nISDN %s: Event: received MNL_RESET_REQ", DSL_TO_NAME(dsl_id));
	    CCPRI_Shutdown(FALSE);
	    break;

	  case NL_REL_IND:
	    if (isdn_debug_ok(q921_debug || q931_debug, dsl_id) || isdn_debug)
		buginf("\nISDN %s: Event: received NL_REL_IND", DSL_TO_NAME(dsl_id));
	    CCPRI_Shutdown(FALSE);
	    break;

	  case NL_EST_CONF:
	  case CC_ERROR_IND:
	  case NL_UI_DATA_IND:
	  case UNKNOWN_EVENT:
	    /*
	     * Messages not currently supported or for which no further
	     * action is currently defined - NOP.
	     */
	    break;

	  default:
	    /*
	     * Msg for an existing call.  Search the linked list of CCB's
	     * based upon Call_ID.
	     */
	    CCB = CCB_Head[Dsl_Id];
	    while (CCB && (CCB->Call_ID != call_id))
		CCB = CCB->Next;
	    if (CCB)
		CCPRI_ENTER_STATE(event);	/* Dispatch the message */
	    else
		CCPRI_Error(CCPRI_GO, P_UNKNOWN_CALL, event, FALSE);
	    break;
	}			/* End of switch statement */
	break;

      case CC_ID:
	Dsl_Id = dsl_id;	/* Save dsl_id */
	if (event == T316_EXPIRY) {	/* Restart timer? */
	    /*
	     * Retry. The channel to restart has been stored in the timer
	     * primitive as the call_id, the interface ID as the ces (Ces
	     * passed into this function is referred to as int_id).
	     */
	    CCPMSG_RestartReq(timer_int_id, call_id, TRUE);
	}
	else if (((SwitchType == PRI_4ESS_STYPE) ||
                 (SwitchType == PRI_5ESS_STYPE)) &&
		 (event == T3M1_EXPIRY)) {	/* AT&T service timer */
	    /*
	     * Retry. The channel to restart has been stored in the timer
	     * primitive as the call_id, the interface ID as the ces (Ces
	     * passed into this function is referred to as int_id).
	     */
	    CC_SERV_ServTimeout(timer_int_id,
				call_id,
				Dsl_Id,
				T3M1_EXPIRY,
				CCPRI_4ESS_T3M1);
	    /*
	     * Send the HOST and update each time the SERVICE message is
	     * resent.
	     */
	    CCPMSG_SendHostChanStat(timer_int_id, call_id, CHAN_MAINTENANCE);
	} else {
	    CCPRI_Error(CCPRI_GO, P_UNKNOWN_EVENT, CC_ID, TRUE);
	}
	break;

      default:
	CCPRI_Error(CCPRI_GO, P_UNKNOWN_MSG_SOURCE, source_id, TRUE);
	break;
    }				/* End switch on source_id */

bailout:
    if (OutPkgInfo)		/* Be sure these have been released */
	CCPRI_FreePkgInfo();
    if (InPkt) {		/* Note that CCIF.C disposes of incoming
				 * packages. */
	LIF_FreePkt(InPkt);	/* Release the incoming packet. */
	InPkt = NULL;
    }
}

/*************
* CCPRI_Init *
**************
*
* Initialize the module and its data structures.
*
*/
PUBLIC BOOLEAN CCPRI_Init ()
{
    BOOLEAN no_error;		/* Initialization result */

    /*
     * Initialize
     */
    CC_SwitchType = SwitchType;	/* Save current switchtype */
    AddrLen = 0;
    Call_Id_Source = PRI_CALL_ID_MIN;
    AllocatedCCBs = 0;
    CCB = NULL;
    Diag[0] = Diag[1] = 0;
    Host_IEs = NULL;
    Host_IEs_Len = 0;
    IE = IE_Fields = NULL;
    InPkt = OutPkt = NULL;
    LrgInfoSize = 0;
    MultiInterface = FALSE;
    MultiPRI = FALSE;
    OutInfoBufType = 0;
    OutPkgInfo = NULL;
    OutPkgInfoLen = 0;
    SmlInfoSize = 0;
    SourceState = 0;

    no_error = TRUE;

    /*
     * Initialization of the Channel Management Table (CC_CHAN_InitTablepri)
     * was moved to isdnintrfc.c, due to a race condition initializing the
     * table. ===
     */

    if (no_error) {
	AllocatedCCBs = 0;	/* CCBs currently allocated from the pool */
	for (Dsl_Id = 0; Dsl_Id < MAX_DSL; Dsl_Id++) {
	    ActiveCCBs[Dsl_Id] = 0;	/* no active ccbs === */
	    CCB_Head[Dsl_Id] = NULL;	/* Init CCB linked list headcells */
	    CCB_Tail[Dsl_Id] = NULL;	/* Init CCB linked list tailcells */
	}
	Call_Id_Source = PRI_CALL_ID_MIN;	/* The "well" for new Call_ID's */

	/*
	 * The multipri logic needs to be checked when a single D-channel can
	 * control multiple interfaces. ===
	 */
#ifdef 0
	MultiPRI = (MAX_DSL > 1) ? TRUE : FALSE;
	MultiInterface = (MAX_INT_IDP > 0) ? TRUE : FALSE;
#endif
	/*
	 * Don't use these flags, make sure initialized to a known value.===
	 */
	MultiPRI = FALSE;
	MultiInterface = FALSE;

	/*
	 * Old PRI Message structure msg_type       0 call_id 1      1
	 * call_id 2      2 <dsl_id        3>       present only if MAX_DSL
	 * in TUNE.H > 1 <priv_info_len 3 or 4>  present only if
	 * EXTENDED_HOST_MSG is defined info elements *
	 * 
	 * new PRI message structure (now lines up with BRI) msg_type      0 dsl
	 * 1 ces           2   - not used but same as bri call_id 1     3
	 * call_id 2     4 priv_info_len 5 info elements
	 */
	if (MultiPRI) {
#ifdef EXTENDED_HOST_MSG
	    AddrLen = 5;	/* Host msg address length */
	    PrivInfoLenOffset = 5;	/* Host private IE len offset */
#else
	    AddrLen = 4;	/* Host msg address length */
#endif
	} else {
#ifdef EXTENDED_HOST_MSG
	    AddrLen = 6;	/* Host msg address length */
	    PrivInfoLenOffset = 5;	/* Host private IE len offset */
#else
	    AddrLen = 3;	/* Host msg address length */
#endif
	}

	SmlInfoSize = LIF_GetBufSize(SML_INFO_BTYPE);
	LrgInfoSize = LIF_GetBufSize(LRG_INFO_BTYPE);

	/*
	 * Now establish the values of all switch-specific parameters needed
	 * at run-time.
	 */
	switch (SwitchType) {
	  case PRI_4ESS_STYPE:
	    T316_TimeOut = CCPRI_4ESS_T316;
	    Xfer_Cap = Att4_Xfer_Cap;
	    Xfer_Cap_Size = sizeof(Att4_Xfer_Cap);
	    Xfer_Rate = Att4_Xfer_Rate;
	    Xfer_Rate_Size = sizeof(Att4_Xfer_Rate);
	    Xfer_Mode = Att4_Xfer_Mode;
	    Xfer_Mode_Size = sizeof(Att4_Xfer_Mode);
	    Cd_Numb_Type = Att4_Cd_Numb_Type;
	    Cd_Numb_Type_Size = sizeof(Att4_Cd_Numb_Type);
	    Cd_Numb_Plan = Att4_Cd_Numb_Plan;
	    Cd_Numb_Plan_Size = sizeof(Att4_Cd_Numb_Plan);
	    Cd_Sub_Type = Att4_Cd_Sub_Type;
	    Cd_Sub_Type_Size = sizeof(Att4_Cd_Sub_Type);
	    Cr_Numb_Type = Att4_Cr_Numb_Type;
	    Cr_Numb_Type_Size = sizeof(Att4_Cr_Numb_Type);
	    Cr_Numb_Plan = Att4_Cr_Numb_Plan;
	    Cr_Numb_Plan_Size = sizeof(Att4_Cr_Numb_Plan);
	    Cr_Scrn_Ind = Att4_Cr_Scrn_Ind;
	    Cr_Scrn_Ind_Size = sizeof(Att4_Cr_Scrn_Ind);
	    Cr_Prest_Ind = Att4_Cr_Prest_Ind;
	    Cr_Prest_Ind_Size = sizeof(Att4_Cr_Prest_Ind);
	    Cr_Sub_Type = Att4_Cr_Sub_Type;
	    Cr_Sub_Type_Size = sizeof(Att4_Cr_Sub_Type);
	    Cause_Location = Att4_Cause_Location;
	    Cause_Location_Size = sizeof(Att4_Cause_Location);
	    Chan_Type = Att4_Chan_Type;
	    Chan_Type_Size = sizeof(Att4_Chan_Type);
	    Net_Id = Att4_Net_Id;
	    Net_Id_Size = sizeof(Att4_Net_Id);
	    Id_Plan = Att4_Id_Plan;
	    Id_Plan_Size = sizeof(Att4_Id_Plan);
	    Feats = Att4_Feats;
	    Feats_Size = sizeof(Att4_Feats);
	    Servs = Att4_Servs;
	    Servs_Size = sizeof(Att4_Servs);
	    Notify_Ind = Att4_Notify_Ind;
	    Notify_Ind_Size = sizeof(Att4_Notify_Ind);
	    Prog_Location = Att4_Prog_Location;
	    Prog_Location_Size = sizeof(Att4_Prog_Location);
	    Prog_Desc = Att4_Prog_Desc;
	    Prog_Desc_Size = sizeof(Att4_Prog_Desc);
	    Proto_Des = Att4_Proto_Des;
	    Proto_Des_Size = sizeof(Att4_Proto_Des);
	    break;

	  case PRI_5ESS_STYPE:
	    T316_TimeOut = CCPRI_5ESS_T316;
	    Xfer_Cap = Att5_Xfer_Cap;
	    Xfer_Cap_Size = sizeof(Att5_Xfer_Cap);
	    Xfer_Rate = Att5_Xfer_Rate;
	    Xfer_Rate_Size = sizeof(Att5_Xfer_Rate);
	    Xfer_Mode = Att5_Xfer_Mode;
	    Xfer_Mode_Size = sizeof(Att5_Xfer_Mode);
	    Cd_Numb_Type = Att5_Cd_Numb_Type;
	    Cd_Numb_Type_Size = sizeof(Att5_Cd_Numb_Type);
	    Cd_Numb_Plan = Att5_Cd_Numb_Plan;
	    Cd_Numb_Plan_Size = sizeof(Att5_Cd_Numb_Plan);
	    Cd_Sub_Type = Att5_Cd_Sub_Type;
	    Cd_Sub_Type_Size = sizeof(Att5_Cd_Sub_Type);
	    Cr_Numb_Type = Att5_Cr_Numb_Type;
	    Cr_Numb_Type_Size = sizeof(Att5_Cr_Numb_Type);
	    Cr_Numb_Plan = Att5_Cr_Numb_Plan;
	    Cr_Numb_Plan_Size = sizeof(Att5_Cr_Numb_Plan);
	    Cr_Scrn_Ind = Att5_Cr_Scrn_Ind;
	    Cr_Scrn_Ind_Size = sizeof(Att5_Cr_Scrn_Ind);
	    Cr_Prest_Ind = Att5_Cr_Prest_Ind;
	    Cr_Prest_Ind_Size = sizeof(Att5_Cr_Prest_Ind);
	    Cr_Sub_Type = Att5_Cr_Sub_Type;
	    Cr_Sub_Type_Size = sizeof(Att5_Cr_Sub_Type);
	    Cause_Location = Att5_Cause_Location;
	    Cause_Location_Size = sizeof(Att5_Cause_Location);
	    Chan_Type = Att5_Chan_Type;
	    Chan_Type_Size = sizeof(Att5_Chan_Type);
	    Net_Id = Att5_Net_Id;
	    Net_Id_Size = sizeof(Att5_Net_Id);
	    Id_Plan = Att5_Id_Plan;
	    Id_Plan_Size = sizeof(Att5_Id_Plan);
	    Feats = Att5_Feats;
	    Feats_Size = sizeof(Att5_Feats);
	    Servs = Att5_Servs;
	    Servs_Size = sizeof(Att5_Servs);
	    Notify_Ind = Att5_Notify_Ind;
	    Notify_Ind_Size = sizeof(Att5_Notify_Ind);
	    Prog_Location = Att5_Prog_Location;
	    Prog_Location_Size = sizeof(Att5_Prog_Location);
	    Prog_Desc = Att5_Prog_Desc;
	    Prog_Desc_Size = sizeof(Att5_Prog_Desc);
	    Proto_Des = Att5_Proto_Des;
	    Proto_Des_Size = sizeof(Att5_Proto_Des);
	    break;

	  case PRI_DMS100_STYPE:
	    T316_TimeOut = CCPRI_DMS100_T316;
	    Xfer_Cap = Dms_Xfer_Cap;
	    Xfer_Cap_Size = sizeof(Dms_Xfer_Cap);
	    Xfer_Rate = Dms_Xfer_Rate;
	    Xfer_Rate_Size = sizeof(Dms_Xfer_Rate);
	    Xfer_Mode = Dms_Xfer_Mode;
	    Xfer_Mode_Size = sizeof(Dms_Xfer_Mode);
	    Cd_Numb_Type = Dms_Cd_Numb_Type;
	    Cd_Numb_Type_Size = sizeof(Dms_Cd_Numb_Type);
	    Cd_Numb_Plan = Dms_Cd_Numb_Plan;
	    Cd_Numb_Plan_Size = sizeof(Dms_Cd_Numb_Plan);
	    Cd_Sub_Type = Dms_Cd_Sub_Type;
	    Cd_Sub_Type_Size = sizeof(Dms_Cd_Sub_Type);
	    Cr_Numb_Type = Dms_Cr_Numb_Type;
	    Cr_Numb_Type_Size = sizeof(Dms_Cr_Numb_Type);
	    Cr_Numb_Plan = Dms_Cr_Numb_Plan;
	    Cr_Numb_Plan_Size = sizeof(Dms_Cr_Numb_Plan);
	    Cr_Scrn_Ind = Dms_Cr_Scrn_Ind;
	    Cr_Scrn_Ind_Size = sizeof(Dms_Cr_Scrn_Ind);
	    Cr_Prest_Ind = Dms_Cr_Prest_Ind;
	    Cr_Prest_Ind_Size = sizeof(Dms_Cr_Prest_Ind);
	    Cr_Sub_Type = Dms_Cr_Sub_Type;
	    Cr_Sub_Type_Size = sizeof(Dms_Cr_Sub_Type);
	    Cause_Location = Dms_Cause_Location;
	    Cause_Location_Size = sizeof(Dms_Cause_Location);
	    Chan_Type = Dms_Chan_Type;
	    Chan_Type_Size = sizeof(Dms_Chan_Type);
	    Net_Id = Dms_Net_Id;
	    Net_Id_Size = sizeof(Dms_Net_Id);
	    Id_Plan = Dms_Id_Plan;
	    Id_Plan_Size = sizeof(Dms_Id_Plan);
	    Feats = Dms_Feats;
	    Feats_Size = sizeof(Dms_Feats);
	    Servs = Dms_Servs;
	    Servs_Size = sizeof(Dms_Servs);
	    Notify_Ind = Dms_Notify_Ind;
	    Notify_Ind_Size = sizeof(Dms_Notify_Ind);
	    Prog_Location = Dms_Prog_Location;
	    Prog_Location_Size = sizeof(Dms_Prog_Location);
	    Prog_Desc = Dms_Prog_Desc;
	    Prog_Desc_Size = sizeof(Dms_Prog_Desc);
	    Proto_Des = Dms_Proto_Des;
	    Proto_Des_Size = sizeof(Dms_Proto_Des);
	    break;
#ifdef VN2
	  case VN2_STYPE:
	    T316_TimeOut = CCPRI_VN2_T316;
	    Xfer_Cap = Vn2_Xfer_Cap;
	    Xfer_Cap_Size = sizeof(Vn2_Xfer_Cap);
	    Xfer_Rate = Vn2_Xfer_Rate;
	    Xfer_Rate_Size = sizeof(Vn2_Xfer_Rate);
	    Xfer_Mode = Vn2_Xfer_Mode;
	    Xfer_Mode_Size = sizeof(Vn2_Xfer_Mode);
	    Cd_Numb_Type = Vn2_Cd_Numb_Type;
	    Cd_Numb_Type_Size = sizeof(Vn2_Cd_Numb_Type);
	    Cd_Numb_Plan = Vn2_Cd_Numb_Plan;
	    Cd_Numb_Plan_Size = sizeof(Vn2_Cd_Numb_Plan);
	    Cr_Numb_Type = Vn2_Cr_Numb_Type;
	    Cd_Sub_Type = Vn2_Cd_Sub_Type;
	    Cd_Sub_Type_Size = sizeof(Vn2_Cd_Sub_Type);
	    Cr_Numb_Type_Size = sizeof(Vn2_Cr_Numb_Type);
	    Cr_Numb_Plan = Vn2_Cr_Numb_Plan;
	    Cr_Numb_Plan_Size = sizeof(Vn2_Cr_Numb_Plan);
	    Cr_Scrn_Ind = Vn2_Cr_Scrn_Ind;
	    Cr_Scrn_Ind_Size = sizeof(Vn2_Cr_Scrn_Ind);
	    Cr_Prest_Ind = Vn2_Cr_Prest_Ind;
	    Cr_Prest_Ind_Size = sizeof(Vn2_Cr_Prest_Ind);
	    Cr_Sub_Type = Vn2_Cr_Sub_Type;
	    Cr_Sub_Type_Size = sizeof(Vn2_Cr_Sub_Type);
	    Cause_Location = Vn2_Cause_Location;
	    Cause_Location_Size = sizeof(Vn2_Cause_Location);
	    Chan_Type = Vn2_Chan_Type;
	    Chan_Type_Size = sizeof(Vn2_Chan_Type);
	    Net_Id = Vn2_Net_Id;
	    Net_Id_Size = sizeof(Vn2_Net_Id);
	    Id_Plan = Vn2_Id_Plan;
	    Id_Plan_Size = sizeof(Vn2_Id_Plan);
	    Feats = Vn2_Feats;
	    Feats_Size = sizeof(Vn2_Feats);
	    Servs = Vn2_Servs;
	    Servs_Size = sizeof(Vn2_Servs);
	    Notify_Ind = Vn2_Notify_Ind;
	    Notify_Ind_Size = sizeof(Vn2_Notify_Ind);
	    Prog_Location = Vn2_Prog_Location;
	    Prog_Location_Size = sizeof(Vn2_Prog_Location);
	    Prog_Desc = Vn2_Prog_Desc;
	    Prog_Desc_Size = sizeof(Vn2_Prog_Desc);
	    Proto_Des = Vn2_Proto_Des;
	    Proto_Des_Size = sizeof(Vn2_Proto_Des);
	    break;
#endif
	  case PRI_NTT_STYPE:
	    T316_TimeOut = CCPRI_NTT_T316;
	    Xfer_Cap = Nttp_Xfer_Cap;
	    Xfer_Cap_Size = sizeof(Nttp_Xfer_Cap);
	    Xfer_Rate = Nttp_Xfer_Rate;
	    Xfer_Rate_Size = sizeof(Nttp_Xfer_Rate);
	    Xfer_Mode = Nttp_Xfer_Mode;
	    Xfer_Mode_Size = sizeof(Nttp_Xfer_Mode);
	    Cd_Numb_Type = Nttp_Cd_Numb_Type;
	    Cd_Numb_Type_Size = sizeof(Nttp_Cd_Numb_Type);
	    Cd_Numb_Plan = Nttp_Cd_Numb_Plan;
	    Cd_Numb_Plan_Size = sizeof(Nttp_Cd_Numb_Plan);
	    Cd_Sub_Type = Nttp_Cd_Sub_Type;
	    Cd_Sub_Type_Size = sizeof(Nttp_Cd_Sub_Type);
	    Cr_Numb_Type = Nttp_Cr_Numb_Type;
	    Cr_Numb_Type_Size = sizeof(Nttp_Cr_Numb_Type);
	    Cr_Numb_Plan = Nttp_Cr_Numb_Plan;
	    Cr_Numb_Plan_Size = sizeof(Nttp_Cr_Numb_Plan);
	    Cr_Scrn_Ind = Nttp_Cr_Scrn_Ind;
	    Cr_Scrn_Ind_Size = sizeof(Nttp_Cr_Scrn_Ind);
	    Cr_Prest_Ind = Nttp_Cr_Prest_Ind;
	    Cr_Prest_Ind_Size = sizeof(Nttp_Cr_Prest_Ind);
	    Cr_Sub_Type = Nttp_Cr_Sub_Type;
	    Cr_Sub_Type_Size = sizeof(Nttp_Cr_Sub_Type);
	    Cause_Location = Nttp_Cause_Location;
	    Cause_Location_Size = sizeof(Nttp_Cause_Location);
	    Chan_Type = Nttp_Chan_Type;
	    Chan_Type_Size = sizeof(Nttp_Chan_Type);
	    Net_Id = Nttp_Net_Id;
	    Net_Id_Size = sizeof(Nttp_Net_Id);
	    Id_Plan = Nttp_Id_Plan;
	    Id_Plan_Size = sizeof(Nttp_Id_Plan);
	    Feats = Nttp_Feats;
	    Feats_Size = sizeof(Nttp_Feats);
	    Servs = Nttp_Servs;
	    Servs_Size = sizeof(Nttp_Servs);
	    Notify_Ind = Nttp_Notify_Ind;
	    Notify_Ind_Size = sizeof(Nttp_Notify_Ind);
	    Prog_Location = Nttp_Prog_Location;
	    Prog_Location_Size = sizeof(Nttp_Prog_Location);
	    Prog_Desc = Nttp_Prog_Desc;
	    Prog_Desc_Size = sizeof(Nttp_Prog_Desc);
	    Proto_Des = Nttp_Proto_Des;
	    Proto_Des_Size = sizeof(Nttp_Proto_Des);
	    break;

	  case PRI_NET5_STYPE:
	    T316_TimeOut = CCPRI_NET5_T316;
	    Xfer_Cap = Net5_Xfer_Cap;
	    Xfer_Cap_Size = sizeof(Net5_Xfer_Cap);
	    Xfer_Rate = Net5_Xfer_Rate;
	    Xfer_Rate_Size = sizeof(Net5_Xfer_Rate);
	    Xfer_Mode = Net5_Xfer_Mode;
	    Xfer_Mode_Size = sizeof(Net5_Xfer_Mode);
	    Cd_Numb_Type = Net5_Cd_Numb_Type;
	    Cd_Numb_Type_Size = sizeof(Net5_Cd_Numb_Type);
	    Cd_Numb_Plan = Net5_Cd_Numb_Plan;
	    Cd_Numb_Plan_Size = sizeof(Net5_Cd_Numb_Plan);
	    Cd_Sub_Type = Net5_Cd_Sub_Type;
	    Cd_Sub_Type_Size = sizeof(Net5_Cd_Sub_Type);
	    Cr_Numb_Type = Net5_Cr_Numb_Type;
	    Cr_Numb_Type_Size = sizeof(Net5_Cr_Numb_Type);
	    Cr_Numb_Plan = Net5_Cr_Numb_Plan;
	    Cr_Numb_Plan_Size = sizeof(Net5_Cr_Numb_Plan);
	    Cr_Scrn_Ind = Net5_Cr_Scrn_Ind;
	    Cr_Scrn_Ind_Size = sizeof(Net5_Cr_Scrn_Ind);
	    Cr_Prest_Ind = Net5_Cr_Prest_Ind;
	    Cr_Prest_Ind_Size = sizeof(Net5_Cr_Prest_Ind);
	    Cr_Sub_Type = Net5_Cr_Sub_Type;
	    Cr_Sub_Type_Size = sizeof(Net5_Cr_Sub_Type);
	    Cause_Location = Net5_Cause_Location;
	    Cause_Location_Size = sizeof(Net5_Cause_Location);
	    Chan_Type = Net5_Chan_Type;
	    Chan_Type_Size = sizeof(Net5_Chan_Type);
	    Net_Id = Net5_Net_Id;
	    Net_Id_Size = sizeof(Net5_Net_Id);
	    Id_Plan = Net5_Id_Plan;
	    Id_Plan_Size = sizeof(Net5_Id_Plan);
	    Feats = Net5_Feats;
	    Feats_Size = sizeof(Net5_Feats);
	    Servs = Net5_Servs;
	    Servs_Size = sizeof(Net5_Servs);
	    Notify_Ind = Net5_Notify_Ind;
	    Notify_Ind_Size = sizeof(Net5_Notify_Ind);
	    Prog_Location = Net5_Prog_Location;
	    Prog_Location_Size = sizeof(Net5_Prog_Location);
	    Prog_Desc = Net5_Prog_Desc;
	    Prog_Desc_Size = sizeof(Net5_Prog_Desc);
	    Proto_Des = Net5_Proto_Des;
	    Proto_Des_Size = sizeof(Net5_Proto_Des);
	    break;

	  case PRI_TS014_STYPE:
	    T316_TimeOut = CCPRI_TS014_T316;
	    Xfer_Cap = Ts014_Xfer_Cap;
	    Xfer_Cap_Size = sizeof(Ts014_Xfer_Cap);
	    Xfer_Rate = Ts014_Xfer_Rate;
	    Xfer_Rate_Size = sizeof(Ts014_Xfer_Rate);
	    Xfer_Mode = Ts014_Xfer_Mode;
	    Xfer_Mode_Size = sizeof(Ts014_Xfer_Mode);
	    Cd_Numb_Type = Ts014_Cd_Numb_Type;
	    Cd_Numb_Type_Size = sizeof(Ts014_Cd_Numb_Type);
	    Cd_Numb_Plan = Ts014_Cd_Numb_Plan;
	    Cd_Numb_Plan_Size = sizeof(Ts014_Cd_Numb_Plan);
	    Cd_Sub_Type = Ts014_Cd_Sub_Type;
	    Cd_Sub_Type_Size = sizeof(Ts014_Cd_Sub_Type);
	    Cr_Numb_Type = Ts014_Cr_Numb_Type;
	    Cr_Numb_Type_Size = sizeof(Ts014_Cr_Numb_Type);
	    Cr_Numb_Plan = Ts014_Cr_Numb_Plan;
	    Cr_Numb_Plan_Size = sizeof(Ts014_Cr_Numb_Plan);
	    Cr_Scrn_Ind = Ts014_Cr_Scrn_Ind;
	    Cr_Scrn_Ind_Size = sizeof(Ts014_Cr_Scrn_Ind);
	    Cr_Prest_Ind = Ts014_Cr_Prest_Ind;
	    Cr_Prest_Ind_Size = sizeof(Ts014_Cr_Prest_Ind);
	    Cr_Sub_Type = Ts014_Cr_Sub_Type;
	    Cr_Sub_Type_Size = sizeof(Ts014_Cr_Sub_Type);
	    Cause_Location = Ts014_Cause_Location;
	    Cause_Location_Size = sizeof(Ts014_Cause_Location);
	    Chan_Type = Ts014_Chan_Type;
	    Chan_Type_Size = sizeof(Ts014_Chan_Type);
	    Net_Id = Ts014_Net_Id;
	    Net_Id_Size = sizeof(Ts014_Net_Id);
	    Id_Plan = Ts014_Id_Plan;
	    Id_Plan_Size = sizeof(Ts014_Id_Plan);
	    Feats = Ts014_Feats;
	    Feats_Size = sizeof(Ts014_Feats);
	    Servs = Ts014_Servs;
	    Servs_Size = sizeof(Ts014_Servs);
	    Notify_Ind = Ts014_Notify_Ind;
	    Notify_Ind_Size = sizeof(Ts014_Notify_Ind);
	    Prog_Location = Ts014_Prog_Location;
	    Prog_Location_Size = sizeof(Ts014_Prog_Location);
	    Prog_Desc = Ts014_Prog_Desc;
	    Prog_Desc_Size = sizeof(Ts014_Prog_Desc);
	    Proto_Des = Ts014_Proto_Des;
	    Proto_Des_Size = sizeof(Ts014_Proto_Des);
	    break;

	  default:
	    CCPRI_Error(CCPRI_INIT, P_BAD_SWITCHTYPE, SwitchType, TRUE);
	    break;
	}
    } else {
	CCPRI_Error(CCPRI_INIT, CCPRI_INITIALIZATION_FAILED,
		    MAX_CHANNELP, TRUE);
    }
    Dsl_Id = 0;
    return (no_error);
}
/*************
* CCPRI_Mail *
**************
*
* Mail a packet to L3 or a package to the HOST as output of this module.
*
*/
PUBLIC void CCPRI_Mail(dest, copy_q931)
int dest;			/* addressee */
BOOLEAN copy_q931;		/* Copy all Q.931 elements to the end of the
				 * msg? */
{
    int call_id;
    CC_STATE pristate;

#ifdef EXTENDED_HOST_MSG
    int copy_len;		/* Q.931 elements length */
#endif

    /*
     * If we are processing a RESTART there is no CCB
     * yet so the following code was added to avoid
     * spurious accesses. CSCdi35949.
     */
    if (CCB == NULL) {
        call_id = 0;
        pristate = 0;
    } else {
        call_id = CCB->Call_ID;
        pristate = CCB->PriState;
    }
    if (dest == HOST_ID) {	/* Mailing a package */
#ifdef EXTENDED_HOST_MSG
	/*
	 * Set length of "private" IEs
	 */
	OutPkgInfo[PrivInfoLenOffset] = OutPkgInfoLen - AddrLen;
	if ((copy_q931) && (InPkt)) {	/* Pass Q.931 elements? */
	    /*
	     * Length of q.931 IEs to copy.
	     */
	    copy_len = InPkt->InfoLen - (InPkt->Info[1] + 3);
            /*
             * Homologating with AT&T, they send a huge packet and the
             * buffer doesn't have enough room for private and public.
             * Use the max size as a limiting factor, at worst will lose
             * some UUI, but not used anyway. B-channel info getting dropped.
             */
            if (copy_len > (LrgInfoSize - OutPkgInfoLen))
                copy_len = (LrgInfoSize - OutPkgInfoLen);
            LIF_N_Cpy(&OutPkgInfo[OutPkgInfoLen],
                       InPkt->Info + InPkt->Info[1] + 3, copy_len);
            OutPkgInfoLen += copy_len;
	}
#else
	Trashcan = (int) copy_q931;	/* Stop compiler warning (unused
					 * parm) */
#endif

	if (OutPkgInfo && (OutPkgInfoLen <= SmlInfoSize))
	    CCPRI_ResizeInfo(TRUE);	/* The buffer is excessively large */

	if (LIF_SendBuf(CC_ID, HOST_ID, OutPkgInfoLen, OutInfoBufType,
			OutPkgInfo, OutPkgInfo[0], NOW, REGULAR_MAIL) == ERR)
	    CCPRI_Error(CCPRI_MAIL, P_SEND_BUF_FAILED, OutPkgInfoLen, TRUE);

	OutPkgInfo = NULL;
	OutPkgInfoLen = 0;
    } else {			/* Mailing a packet (to L3) */
	if (OutPkt && ((OutPkt->InfoLen + OutPkt->HeadLen) <= SmlInfoSize))
	    CCPRI_ResizeInfo(FALSE);	/* The buffer is excessively large */

	if (LIF_SendPkt(L2P_LAPD, CC_ID, (int) pristate, L3_ID,
			NL_DATA_REQ, Dsl_Id, Q931_SAPI, 1, 0, call_id,
			DSS1_D_CHANNEL, TRUE, OutPkt, NOW, REGULAR_MAIL)
	    == ERR) {
	    CCPRI_Error(CCPRI_MAIL, P_SEND_PKT_FAILED, OutPkt->InfoLen, TRUE);
	    CCPMSG_SendHostProgress(INTERNAL_ERROR, FALSE, FALSE);
	}
	OutPkt = NULL;
    }
}

/*****************
* CCPRI_MapIntId *
******************
*
* Map the CHANNEL_ID_IE interface ID to and from an internal
* representation to a subscribed version.
*
* Explanation:  This function is provided because the subscribed interface
*               IDs supplied by the network carrier may not be numbered
*               contiguously in acsending order from zero.  The internal
*               representration of the ID is assumed to be normalized to
*               the low values (0, 1, 2...).  The user of this code
*               may insert a database reference mechanism here to make
*               this conversion as required.  As provided, this function
*               maps the input directly to the output (NOP).
*
*/
PUBLIC int CCPRI_MapIntId(int_id, intern_to_extern)
int int_id;			/* Interface ID */
BOOLEAN intern_to_extern;	/* Map internal to external representation */
{
    int result_id;		/* Mapped-to ID */

    if (intern_to_extern)
	result_id = int_id;	/* CUSTOMIZE HERE */
    else
	result_id = int_id;	/* CUSTOMIZE HERE */

    return (result_id);
}

/**************
* CCPRI_Match *
***************
*
* Attempt to match a specific value with any one of the entries in a
* list.  Return TRUE if a match is found.
*
*/
PRIVATE BOOLEAN near CCPRI_Match(list, list_size, value)
const BYTE *list;		/* List to search */
int list_size;			/* Size of list */
BYTE value;			/* Value to match */
{
    int i;			/* Index */
    BOOLEAN match = FALSE;	/* Result flag */


    for (i = 0; i < list_size; i++) {
	if (list[i] == value) {
	    match = TRUE;
	    break;
	}
    }
    return (match);
}

#ifdef TRACING			/* Taskview or Tracker environment */
/*********************
* CCPRI_NumericState *
**********************
*
* Have the state of a call translated to a numeric index for display.
*
* Return the index.
*
*/
PRIVATE BYTE near CCPRI_NumericState(ccb_state)
CC_STATE ccb_state;		/* State of a call */
{
    BYTE state_index;		/* Return value */

    switch (SwitchType) {
#ifdef PRI_4ESS
      case PRI_4ESS_STYPE:
#ifdef USER
	state_index = CCPATT_NumericState(ccb_state);
#else
	state_index = CCPATTN_NumericState(ccb_state);
#endif
	break;
#endif
#ifdef PRI_5ESS
      case PRI_5ESS_STYPE:
#ifdef USER
	state_index = CCPATT_NumericState(ccb_state);
#else
	state_index = CCPATTN_NumericState(ccb_state);
#endif
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	state_index = CCPNTI_NumericState(ccb_state);
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
	state_index = CCPVN_NumericState(ccb_state);
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	state_index = CCP1TR6_NumericState(ccb_state);
	break;
#endif
      default:
	state_index = 0;
	break;			/* Should never occur */
    }
    return (state_index);
}

#endif				/* TRACING */
/*******************
* CCPRI_RateToType *
********************
*
* Convert a bearer channel rate to a bearer channel type.  Return the type.
*/
PUBLIC int CCPRI_RateToType(bear_rate)
int bear_rate;			/* Bearer channel rate */
{
    int chan_type;		/* Bearer channel type */


    switch (bear_rate) {
      case BEAR_RATE_64KBPS:
	chan_type = B_CHANNEL_UNITS;
	break;
      case BEAR_RATE_384KBPS:
	chan_type = H0_CHANNEL_UNITS;
	break;
      case BEAR_RATE_1536KBPS:
	chan_type = H11_CHANNEL_UNITS;
	break;
      case BEAR_RATE_1920KBPS:
	chan_type = H12_CHANNEL_UNITS;
	break;
      default:			/* Unsupported data rate */
	chan_type = NO_CHANNEL_UNITS;
	break;
    }

    return (chan_type);
}

/********************
* CCPRI_ReleaseCall *
*********************
*
* Terminate a call and put the CCB to bed.
*
*/
PUBLIC void CCPRI_ReleaseCall()
{
    CCB_ID temp;		/* Temporary CCB pointer */

    if (CCB) {
	/*
	 * Always send the HOST a CALL_CLEARED message as the last act before
	 * this call is dropped if the HOST shares a context for this call
	 * with CCPRI.
	 */
	if ((CCB->ClearHost) && (CCPRI_GetOutInfo(CALL_CLEARED)))
	    CCPRI_Mail(HOST_ID, FALSE);

	if (CCB->B_Chan)
	    CCPRI_ReleaseChan();/* Release B-Chan */

	if (CCB != &Dummy_Ccb) {
	    /*
	     * Remove CCB from Linked List
	     */
	    if (CCB == CCB_Head[Dsl_Id]) {	/* First link in list */
		temp = NULL;
		CCB_Head[Dsl_Id] = CCB->Next;
	    } else {		/* Search for link; logically delete */
		temp = CCB_Head[Dsl_Id];
		while (temp->Next != CCB) {
		    temp = temp->Next;
		    if (temp == NULL) {	/* Not found */
			CCPRI_Error(CCPRI_RELEASE_CALL, P_CCB_NOT_FOUND,
				    CCB->Call_ID, TRUE);
			break;
		    }
		}
		temp->Next = CCB->Next;
	    }
	    if (CCB == CCB_Tail[Dsl_Id])	/* Last link in list */
		CCB_Tail[Dsl_Id] = temp;

            free(CCB);
	    ActiveCCBs[ Dsl_Id ]--;         /* Decrement CCBs in use */
	    AllocatedCCBs--;
	    if (detail_debug)
		buginf("\nAllocated CCBs = %d", AllocatedCCBs);
	} else {
	    CCPRI_Error(CCPRI_RELEASE_CALL, P_DUMMY_CCB_FOUND, 0, TRUE);
	}
	CCB = NULL;
    } else {
	CCPRI_Error(CCPRI_RELEASE_CALL, P_NULL_CCB, 0, FALSE);
    }
    if (isdn_debug)
	buginf("\nleaving CCPRI_ReleaseCall, Allocated CCBs = %d", AllocatedCCBs);
}

/********************
* CCPRI_ReleaseChan *
*********************
*
* Remove a Channel from use.
*
*/
PUBLIC void CCPRI_ReleaseChan()
{
    if (CCB->B_Chan) {
	if (detail_debug)
	    buginf("\nCCPRI_ReleaseChan, releasing Bchan, %x, type = %x, callid = %x",
		   CCB->B_Chan, CCB->ChanType, CCB->Call_ID);
	if (CC_CHAN_GetUserpri(CCB->Int_Id[0],
			       CCB->B_Chan,
			       CCB->ChanType,
			       Dsl_Id) == CCB->Call_ID) {
	    CC_CHAN_ReleaseChanpri(CCB->Int_Id[0], CCB->B_Chan,
				   CCB->ChanType, Dsl_Id);
	}
	/*
	 * Remove CCB associations
	 */
	CCB->B_Chan = CCB->Int_Id[0] = 0;
	CCB->Int_Id_Len = 1;
    }
}

/***********************
* CCPRI_ReleaseFailure *
************************
*
* Second timeout of T308 in Protocol Control.  Attempt here to sweep
* up the remains by initiating restart for each B-channel owned by the
* input CCB and retiring the "parent" CCB once these "child" processes
* are initiated.
*
* The overall complexity of this operation exists because of the potential
* for hyperchannels channels and knowledge that the network only conducts
* restart on a B-channel basis, thus the restart of an H0 channel, for
* example, evolves into the independent restarts of its 6 component
* B-channels which wiil operate subsequently as independent processes.
*
*/
PRIVATE void near CCPRI_ReleaseFailure()
{
    CCB_ID ccb;			/* Intermediate shuffling variable */
    int channel;		/* Owned B-channel */

    if (CCB->B_Chan) {
	ccb = CCB;		/* Save parent CCB identifier */
	/*
	 * Consider each B-channel it "owns."
	 */
	for (channel = ccb->B_Chan;
	     channel <= ccb->B_Chan + GET_CHAN_SPANP(ccb->ChanType);
	     channel++) {
	    CCPMSG_RestartReq(ccb->Int_Id[0], channel, FALSE);
	}
	CCB = ccb;		/* Restore the "parent" in order to release
				 * it. */
	CCB->B_Chan = 0;	/* Don't interfere with restart processes. */
	CCPRI_ReleaseCall();	/* Release the parent CCB. */
    } else {
	CCPRI_ReleaseCall();	/* Release the parent CCB */
    }
}

/*******************
* CCPRI_ResizeInfo *
********************
*
* Downsize the OutPkt to SML_INFO_BTYPE.
* Copy information into a smaller block to efficiently use memory.
*
*/
PRIVATE void near CCPRI_ResizeInfo(is_pkginfo)
BOOLEAN is_pkginfo;		/* Buffer is a HOST package info buffer */

{
    PKT_ID old_pkt;		/* Packet being discarded */
    BYTE *old_info;		/* Info being discarded */


    if (is_pkginfo) {		/* HOST message */
	old_info = OutPkgInfo;
	if ((OutPkgInfo = LIF_GetBuffer(SML_INFO_BTYPE, FALSE)) != NULL) {
	    LIF_N_Cpy(OutPkgInfo, old_info, OutPkgInfoLen);
	    LIF_FreeBuffer(LRG_INFO_BTYPE, old_info);
	    OutInfoBufType = SML_INFO_BTYPE;
	} else {
	    CCPRI_Error(CCPRI_RESIZE_INFO, P_OUT_OF_INFO_BUFFERS, 0, TRUE);
	    OutPkgInfo = old_info;
	}
    } else {			/* L3 message */
	old_pkt = OutPkt;	/* Save old pkt */
	if ((OutPkt = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE)) != NULL) {
	    LIF_CpyPkt(OutPkt, old_pkt);
	    OutPkt->InfoType = SML_INFO_BTYPE;
	    LIF_FreePkt(old_pkt);
	} else {
	    CCPRI_Error(CCPRI_RESIZE_INFO, P_OUT_OF_PKTS, 0, TRUE);
	    OutPkt = old_pkt;	/* Continue using the oversized packet */
	}
    }
}

/****************
* CCPRI_ServMsg *
*****************
*
* Differential handling of SERVICE messages for different SwitchTypes.
*
*/
PRIVATE void near CCPRI_ServMsg()
{
    BYTE int_id = 0;		/* Interface ID receiving maintenance */
    int chan = 0;		/* Channel receiving maintenance */

    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_DMS100_STYPE:
      case PRI_NET5_STYPE:
      case PRI_NTT_STYPE:
      case PRI_TS014_STYPE:
	if ((chan = CC_SERV_ServiceMsg(InPkt, Dsl_Id, &int_id)) != 0)
	    CCPMSG_SendHostChanStat(int_id, chan, CHAN_MAINTENANCE);
	break;

      default:			/* Messages not supported for other switch
				 * types */
	Trashcan = int_id;	/* Prevent compiler warning for the unused */
	Trashcan = chan;
	break;
    }
}

/*****************
* CCPRI_Shutdown *
******************
*
* Terminate all calls on one or all DSLs.
* This function is invoked for NL_REL_IND or for MNL_RESET_REQ
*/
PRIVATE void near CCPRI_Shutdown(all_dsls)
BOOLEAN all_dsls;		/* Shutdown is for all the active D-channels? */
{
    int lo_dsl, hi_dsl;		/* Dsl range to shut down */
    CCB_ID ccb;			/* Temp ccb pointer */
    int chan;			/* Channel counter */
    int int_id;			/* Interface ID counter */
    int timer_id = NOT_TIMING;	/* Timer T3M1 handle */

    if (detail_debug)
	buginf("\nentering CCPRI_Shutdown");
    lo_dsl = (all_dsls) ? 0 : Dsl_Id;
    hi_dsl = (all_dsls) ? MAX_DSL : Dsl_Id + 1;
    for (Dsl_Id = lo_dsl; Dsl_Id < hi_dsl; Dsl_Id++) {
	CCB = CCB_Head[Dsl_Id];
	while (CCB) {
	    ccb = CCB->Next;
	    if (CCB->Call_ID)
		CCPMSG_SendHostDisc(TEMPORARY_FAILURE, FALSE);	/* Cause 41 */
	    CCPRI_ReleaseCall();
	    CCB = ccb;
	}

	for (int_id = MIN_INT_IDP; int_id <= MAX_INT_IDP; int_id++) {
	    for (chan = MIN_CHANNEL; chan <= MAX_CHANNELP; chan++) {
		switch (SwitchType) {
		    /* What about NET5 and NTT ? */
		  case PRI_4ESS_STYPE:
		  case PRI_5ESS_STYPE:
		  case PRI_DMS100_STYPE:
		    if ((timer_id =
			 CC_CHAN_GetTimerID(int_id, chan,
					    Dsl_Id)) != NOT_TIMING) {
			/*
			 * Remove timer T3M1.
			 */
			LIF_StopTimer(timer_id);
			LIF_RemoveTimer(timer_id);
			CC_CHAN_SetTimerID(int_id, chan, NOT_TIMING, Dsl_Id);
		    }
		    break;
		  default:
		    Trashcan = (int) timer_id;	/* Prevent compiler warning */
		    break;
		}
		CC_CHAN_ReleaseChanpri(int_id, chan, B_CHANNEL_UNITS, Dsl_Id);
	    }
	}
    }
}
/*
 * This is a stub routine for the BRI code
 * This routine is already included in the 4000 and the 4500.
 * If we have PRI && BRI in all platforms, this can probably be removed.
 */
#if !defined(XX) && !defined(SIERRA)

void isdn_bri_enable_layer1_Q (void)
{
    if (detail_debug)
	buginf("\nin isdn_bri_enable_layer1_Q. ");
}

#endif

/*
 * End of module.
 */
