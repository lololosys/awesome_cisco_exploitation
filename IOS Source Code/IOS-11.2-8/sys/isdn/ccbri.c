/* $Id: ccbri.c,v 3.6.4.5 1996/09/05 00:03:15 snyder Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccbri.c,v $
 *------------------------------------------------------------------
 * ccbri.c  Basic Rate Interface (BRI) Call Control Task 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccbri.c,v $
 * Revision 3.6.4.5  1996/09/05  00:03:15  snyder
 * CSCdi68122:  declare a bunch of things const in isdn
 *              660 out of data, 40 out of image,
 *              ain't this fun.
 * Branch: California_branch
 *
 * Revision 3.6.4.4  1996/06/17  08:33:02  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.6.4.3  1996/05/07  01:53:38  bdas
 * CSCdi56554:  ISDN BRI Release message has incorrect Cause value
 * Branch: California_branch
 *
 * Revision 3.6.4.2  1996/05/01  14:51:09  hrobison
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
 * Revision 3.6.4.1  1996/04/30  23:06:55  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.6  1996/03/08  23:44:40  smackie
 * Fix various signed/unsigned comparison conflicts that cause the
 * 95q4 toolchain to blow it's cookies. (CSCdi49069)
 *
 * Revision 3.5  1996/02/15  17:27:20  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.4  1995/11/21  04:52:03  dclare
 * CSCdi42123:  isdn layer 2 gets hung in ESTABLISH_AWAITING_TEI. The PBX
 * can give the router a B-channel already in use. Make sure it is not in
 * use, if it is in use, clear the call.
 *
 * Revision 3.3  1995/11/20  18:50:05  rbeach
 * CSCdi42830:  Crash while initiating pings on BRI interface with
 * no cable attached. This will only occur for NTT and 1TR6 switch
 * types. InPkt was NULL in CCBRI_DisconnectHostCall and the NULL
 * check was added.
 *
 * Revision 3.2  1995/11/17  17:39:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/19  16:52:25  dclare
 * CSCdi40646:  ISDN BRI changes for Italy NET3 homologation.
 *
 * Revision 2.2  1995/07/28  15:24:27  dclare
 * CSCdi37836:  ISDN BRI switches DMS-100 and NI1 may need only one TEI.
 * One spid configured, use one TEI. Two spids configured, use two TEIs.
 *
 * Revision 2.1  1995/06/07  21:13:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*************************************************************************

MODULE:      ccbri.c
VERSION:     3.21
DESCRIPTION: Basic Rate Interface (BRI) Call Control Task

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

This module may be linked with the following specific call control
modules:
             - CCB5ESS.C
             - CCB5ESSN.C
             - CCBDMSB.C
             - CCBKDNT.C
             - CCB1TR6.C
             - CCBVN.C
             - CCBNET3.c
             - CCBTS013.c

The following flags control the operation of the call control:

    - SBC_INTERFACE (use single byte commands to communicate with
                     a simple voice telephone hardware)

    - TASK_KB       (the simple voice telephone task - use SBC
                     commands to communicate with this task)

    - COMMAND_INTERFACE ( use multibyte commands - packages - to
                     communicate with a HOST processor or other
                     intelligent task)

    - BRI_5ESS      (use state machine and message format compatible
                     with 5ESS 5E5 switch)

    - BRI_DMS100    (use state machine and message format compatible
                     with NT DMS100 switch)

    - BRI_1TR6      (use state machine and message format compatible
                     with German 1TR6 switch)

    - KDD_NTT       (use state machine and message format compatible
                     with Japanese KDD or NTT switch)

    - BRI_NET3      (use state machine and message format compatible
                     with British Telecom NET3)

    - BRI_TS013     (use state machine and message format compatible
                     with Telecom Australia TS013)

    - VN2           (use state machine and message format compatible
                     with France Telecom VN2 (and later) switch

    - COMP_TEST     (used only with a set of AT&T compatibility tests)

    - ICLID         (send messages about new incoming calls and call
                     terminations to an Incoming Calling Number ID
                     processor)

    - DISPLAY_LOCAL_ERR (use only while testing the Call Control
                     in order to display the error messages in human
                     readible format)

    - ATT_TYPE_A    (include the state machine and message set for AT&T
                     supplementary services - TYPE A terminal)

    - DEBUG         (display some additional debugging information)

    - PU_ESTABLISH  (perform the link establishment on power up rather
                     than on demand)


NOTE: depending on the configuration ordered, not all of the flags
listed above will be present in your version of the Call Control.

ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

The COMP_TEST conditional compilation flag is found within this module
wherever modification to the basic Call Control has been made in order
that it may perform some specific test or tests within the AT&T 5E4.2
BRI Test Plan (Jan. 1988).  These modifications generally are necessary
to cause Call Control to assume optional modes of operation rather than
to correct some inherent operational flaws.


*************************************************************************
*************************************************************************
*/

#define     CCBRI_C                                     /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

/* """
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
""" */
#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"
#include    "address.h"

#undef      NULL		/* Defined in both master.h and flags.h */

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "ccie.h"
#include    "gp.h"
#include    "ccbri.h"
#include    "cc_chan.h"
#include    "tei_ces.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/
/* Hardware Control Block
*/
PUBLIC HCB Hcb[ MAX_DSL ];
PUBLIC HOST_CB Host_CB [MAX_DSL] [ MAX_HOSTS ];	/*===*/

/* assorted public variables
*/
/* CCB heads, tails and the CCB
*/
PUBLIC CCB_ID   CCB_Head[ MAX_DSL ];
PRIVATE CCB_ID  CCB_Tail[ MAX_DSL ];
PUBLIC CCB_ID   CCB;
PUBLIC BYTE     Dsl_ID;             /* Current digital subscriber loop */
PUBLIC int      Call_Id_Source;      /* Master "new" call_id generator */
PUBLIC int      Source;                       /* Source of current msg */
PUBLIC int      L3_State;                               /* State of L3 */
PRIVATE int     SmlBufSize;    /* For re-size comparison in CCBRI_Mail */
PUBLIC int      ActiveCCBs[ MAX_DSL ];          /* CCBs in use counter */
PUBLIC int      AllocatedCCBs;            /* total CCBs in use counter */
PRIVATE struct  CallControlBlock Dummy_Ccb;           /* The dummy CCB */
PUBLIC BYTE *   IE;                    /* Pointer to CCIE info element */
PUBLIC BYTE *   IE_Fields;     /* Pointer to CCIE buffer for IE unpack */
PUBLIC int      TheTrash;            /* use to avoid compiler warnings */
PUBLIC PKT_ID   InPkt;                     /* Internal packet pointers */
PUBLIC PKT_ID   OutPkt;                    /* Internal packet pointers */
PUBLIC BYTE *   InPkgInfo;                  /* Ptr to the package info */
PUBLIC int      InPkgInfoLen;                   /* Package info length */
PUBLIC BYTE *   OutPkgInfo;             /* Pointer to the package info */
PUBLIC int      OutPkgInfoLen;                  /* Package info length */
PRIVATE BUFFER_TYPE OutPkgInfoType;               /* Package info type */

/*
 * This will be used to determine the priority of the message to be
 * sent out. Make sure that the spid goes out before the Setup message
 * if Layer 2 is not up and an outgoing call is make.
 */
PUBLIC BYTE Mail_priority;

#ifdef COMP_TEST
/* The following special flags are used to control unusual operation
*  by CC to enable it to perform AT&T compatibility test suites wherein
*  responses to specific messages are variable and optional.  Through
*  these flags, which are externally toggled by keystrokes (see
*  CCB5E_SpecialFunction), Call Control may assume a number of optional
*  behaviors.
*/
BOOLEAN Exclusive;          /* Exclusively accept B-Chan 1 for Test 25 */
BOOLEAN SuspendDiscon;          /* Condition CC to perform AT&T Test 8 */
BOOLEAN EnBlocSetup;           /* Condition CC to perform AT&T Test 16 */
BOOLEAN QuickConn;             /* Condition CC to perform AT&T Test 42 */
BOOLEAN QuickProceed;          /* Condition CC to perform AT&T Test 45 */
BOOLEAN ReleaseActive;         /* Condition CC to perform AT&T Test 60 */

/* A dialed number available when Call Control must provide pre-dialed
*  digits for en-bloc transmission in the SETUP message.
*/
BYTE EnBlocDigits[] = { 0x35, 0x35, 0x35, 0x31, 0x32, 0x31, 0x32 };
#endif

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

/***********************************************************************
*                THE INFORMATION ELEMENT DATABASE                      *
*                                                                      *
* Following is a set of tables organized by the set of information     *
* element types analyzed in this module for coding integrity.          *
*                                                                      *
* A table exists for each element field analyzed, for each switch type *
* supported.  The values in each table are derived from the switch     *
* vendor specifications and represent all the legal values defined,    *
* rather than some subset anticipated for a particular application.    *
*                                                                      *
* For each table, a table pointer variable and a size variable are     *
* declared, which are initialized, on a SwitchType-basis in CCBRI_Init *
* and are used, during call processing, to access this database.       *
*                                                                      *
************************************************************************/

/***********************************************************************
*    Data used in the analysis of the Bearer Capability Info Element.  *
************************************************************************/

PRIVATE const BYTE *Xfer_Cap;      /* Transfer capability list pointer */
PRIVATE const BYTE *Xfer_Rate;           /* Transfer rate list pointer */
PRIVATE const BYTE *Xfer_Mode;           /* Transfer mode list pointer */
PRIVATE const BYTE *Xfer_Prot;     /* Transfer protocol identification */
PRIVATE int   Xfer_Cap_Size;       /* Size of transfer capability list */
PRIVATE int   Xfer_Rate_Size;             /* Size of trasfer rate list */
PRIVATE int   Xfer_Mode_Size;            /* Size of transfer mode list */
PRIVATE int   Xfer_Prot_Size;        /* size of transfer protocol list */

#ifdef BRI_NI1	/*===*/
#ifdef NEVER_REFERENCED
const BYTE *L2_Prot;                     /* L2 Protocol identification */
const BYTE *L3_Prot;                     /* L3 Protocol identification */
int   L2_Prot_Size;                            /* size of L2 prot list */
int   L3_Prot_Size;                            /* size of L3 prot list */
#endif /* NEVER_REFERENCED */
#endif                

/* Legal Bearer Capability IE field values.
*/
#ifdef BRI_5ESS
static const BYTE Att5_Xfer_Cap[] = {
                             0x00,                           /* Speech */
                             0x08,                   /* Unrest digital */
                             0x09,               /* Restricted digital */
                             0x10                        /* 3.1K audio */
                             };
static const BYTE Att5_Xfer_Mode[] = {
                             0x00,                          /* Circuit */
                             0x02                            /* Packet */
                             };
static const BYTE Att5_Xfer_Rate[] = {
                             0x00,                 /* Packet Xfer rate */
                             0x10,                 /* 64kbps Xfer rate */
                             };
static const BYTE Att5_Xfer_Prot[] = {
                             0x00,                        /* Undefined */
                             0x01,                    /* rate adaption */
                             0x02,                            /* u-law */
                             0x06                  /* layer 2.3 = X.25 */
                             };
#endif
#ifdef BRI_DMS100
static const BYTE Dmsb_Xfer_Cap[] = {
                             0x00,                           /* Speech */
                             0x08,                   /* Unrest digital */
                             0x09,               /* Restricted digital */
                             0x10,                       /* 3.1K audio */
                             0x11                          /* 7K audio */
                             };
static const BYTE Dmsb_Xfer_Mode[] = {
                             0x00,                          /* Circuit */
                             0x02                            /* Packet */
                             };
static const BYTE Dmsb_Xfer_Rate[] = {
                             0x10,                           /* 64kbps */
                             };
static const BYTE Dmsb_Xfer_Prot[] = {
                             0x00,                        /* Undefined */
                             0x01,                    /* rate adaption */
                             0x02,                            /* u-law */
                             0x05                        /* 7kHz audio */
                             };
#endif
#ifdef KDD_NTT
static const BYTE Ntkdb_Xfer_Cap[] = {
                             0x00,                           /* Speech */
                             0x08,                   /* Unrest digital */
                             0x10                        /* 3.1K audio */
                             };
static const BYTE Ntkdb_Xfer_Mode[] = {
                             0x00,                          /* Circuit */
                             0x02                            /* Packet */
                             };
static const BYTE Ntkdb_Xfer_Rate[] = {
                             0x00,
                             0x10                            /* 64kbps */
                             };
#endif
#ifdef VN2
static const BYTE Vn2_Xfer_Cap[] = {
                             0x00,                           /* Speech */
                             0x08,                   /* Unrest digital */
                             0x10                        /* 3.1K audio */
                             };
static const BYTE Vn2_Xfer_Mode[] = {
                             0x00                           /* Circuit */
                             };
static const BYTE Vn2_Xfer_Rate[] = {
                             0x10                            /* 64kbps */
                             };
static const BYTE Vn2_Xfer_Prot[] = {
			     0x00,		     /* ===  Undefined */
                             0x03        /* L1 proto - Rec G.711 A-law */
                             };
#endif
#ifdef BRI_NET3 	/*===*/
static const BYTE Net3_Xfer_Cap[] = {
                             0x00,                           /* Speech */
                             0x08,                   /* Unrest digital */
                             0x10,                       /* 3.1K audio */
                             0x11,                      /* 7 kHz audio */
                             0x18                             /* video */
                             };
static const BYTE Net3_Xfer_Mode[] = {
                             0x00,                          /* Circuit */
                             0x10                            /* Packet */
                             };
static const BYTE Net3_Xfer_Rate[] = {
                             0x00,                           /* Packet */
                             0x10                            /* 64kbps */
                             };
static const BYTE Net3_Xfer_Prot[] = {
                             0x00,		                  /* Undefined */
                             0x01,		        /* CCITT RA V.110/X.30 */
                             0x02,		                /* G.711 u-law */
                             0x03,       /* L1 proto - Rec G.711 A-law */
                             0x04,		        /* G.721 32kbits ADPCM */
                             0x05,		      /* G.722 and G.725 7 kHz */
                             0x06,		       /* G.7xx 384 kbps video */
                             0x07,		               /* Non-CCITT RA */
                             0x08        /* CCITT std RA X.31 HDLC flag*/
                             };
#endif
#ifdef BRI_TS013
static const BYTE Ts013_Xfer_Cap[] = {
                             0x00,                           /* Speech */
                             0x08,                   /* Unrest digital */
                             0x10                        /* 3.1K audio */
                             };
static const BYTE Ts013_Xfer_Mode[] = {
                             0x00                           /* Circuit */
                             };
static const BYTE Ts013_Xfer_Rate[] = {
                             0x10                            /* 64kbps */
                             };
static const BYTE Ts013_Xfer_Prot[] = {
                             0x01,                    /* rate adaption */
                             0x03        /* L1 proto - Rec G.711 A-law */
                             };
#endif
#ifdef BRI_NI1	/*===*/
static const BYTE NI1_Xfer_Cap[] = {
                             0x00,                           /* Speech */
                             0x08,                   /* Unrest digital */
                             0x10                        /* 3.1K audio */
                             };
static const BYTE NI1_Xfer_Mode[] = {
                             0x00,                          /* Circuit */
                             0x80                            /* Packet */
                             };
static const BYTE NI1_Xfer_Rate[] = {
                             0x10                  /* 64kbps Xfer rate */
                             };
static const BYTE NI1_Xfer_Prot[] = {
                             0x00,                        /* Undefined */
                             0x01,                    /* Rate Adaption */
                             0x02,                            /* u-law */
                             0x05                       /* 7 kHz audio */
                             };
static const BYTE NI1_L2_Prot[] = {
                             0x82,                             /* LAPD */
                             0x86                              /* LAPB */
                             };
static const BYTE NI1_L3_Prot[] = {
                             0xB5                              /* LAPB */
                             };
#endif

/***********************************************************************
*   Data used in the analysis of the Called Party Number Info Element. *
************************************************************************/

PRIVATE const BYTE *Cd_Numb_Type;          /* Number type list pointer */
PRIVATE const BYTE *Cd_Numb_Plan;          /* Number plan list pointer */
PRIVATE int   Cd_Numb_Type_Size;           /* Size of number type list */
PRIVATE int   Cd_Numb_Plan_Size;           /* Size of number plan list */

/* Legal Called Party Number IE field values.
*/
#ifdef BRI_5ESS
static const BYTE Att5_Cd_Numb_Type[] = {
                             0x01,                    /* International */
                             0x02,                         /* National */
                             0x04                             /* Local */
                             };
static const BYTE Att5_Cd_Numb_Plan[] = {
                             0x01,                             /* ISDN */
                             0x09                           /* Private */
                             };
#endif
#ifdef BRI_DMS100
static const BYTE Dmsb_Cd_Numb_Type[] = {
                             0x00,                          /* Unknown */
                             0x01,                    /* International */
                             0x02,                         /* National */
                             0x03,                 /* Network specific */
                             0x04,                            /* Local */
                             0x06                       /* Abbreviated */
                             };
static const BYTE Dmsb_Cd_Numb_Plan[] = {
                             0x00,                          /* Unknown */
                             0x01,                             /* ISDN */
                             0x03,                             /* Data */
                             0x09                           /* Private */
                             };
#endif
#ifdef KDD_NTT

static const BYTE Ntkdb_Cd_Numb_Plan[] = {
                             0x00                           /* unknown */
                              };
static const BYTE Ntkdb_Cd_Numb_Type[] = {
                             0x00                           /* unknown */
                              };
#endif
#ifdef VN2

static const BYTE Vn2_Cd_Numb_Plan[] = {
                             0x00,                          /* unknown */
                             0x09                           /* private */
                              };
static const BYTE Vn2_Cd_Numb_Type[] = {
                             0x00,                          /* en-bloc */
                             0x05                           /* overlap */
                              };
#endif
#ifdef BRI_NET3 	/*===*/

static const BYTE Net3_Cd_Numb_Plan[] = {
                             0x00,                          /* unknown */
                             0x01,                          /* ISDN, E.164 */
                             0x03,                          /* data, X.121 */
                             0x04,                          /* telex, F.69 */
                             0x08,                          /* national std */
                             0x09,                          /* private */
                             0x0f                           /* reserved */
                              };
static const BYTE Net3_Cd_Numb_Type[] = {
                             0x00,                          /* en-bloc */
                             0x01,                          /* international */
                             0x02,                          /* national */
                             0x03,                          /* net specific */
                             0x04,                          /* subscriber */
                             0x06,                          /* abbreviated */
                             0x07                           /* reserved */
                              };
#endif
#ifdef BRI_TS013
static const BYTE Ts013_Cd_Numb_Plan[] = {
                             0x01                              /* ISDN */
                              };
static const BYTE Ts013_Cd_Numb_Type[] = {
                             0x00,                          /* Unknown */
                             0x01,                   /* Internaltional */
                             0x02,                         /* National */
                             0x04                             /* Local */
                              };
#endif
#ifdef BRI_NI1	/*===*/
static const BYTE NI1_Cd_Numb_Type[] = {
                             0x00,                          /* Unknown */
                             0x01,                   /* Internaltional */
                             0x02,                         /* National */
                             0x03,                          /* Network */
                             0x04,                            /* Local */
                             0x06                           /* Private */
                             };
static const BYTE NI1_Cd_Numb_Plan[] = {
                             0x00,                          /* Unknown */
                             0x01                              /* ISDN */
                             };
#endif

/***********************************************************************
*  Data used in the analysis of the Calling Party Number Info Element. *
************************************************************************/

PRIVATE const BYTE *Cr_Numb_Type;          /* Numper type list pointer */
PRIVATE const BYTE *Cr_Numb_Plan;          /* Number plan list pointer */
PRIVATE const BYTE *Cr_Scrn_Ind;   /* Screening indicator list pointer */
PRIVATE const BYTE *Cr_Prest_Ind; /* PresentationIndicator ListPointer */
PRIVATE int   Cr_Numb_Type_Size;           /* Size of number type list */
PRIVATE int   Cr_Numb_Plan_Size;           /* Size of number plan list */
PRIVATE int   Cr_Scrn_Ind_Size;    /* Size of screening indicator list */
PRIVATE int   Cr_Prest_Ind_Size; /* Sizeof presentation indicator list */

/* Legal Calling Party Number IE field values.
*/
#ifdef BRI_5ESS
static const BYTE Att5_Cr_Numb_Type[] = {
                             0x00,                          /* Unknown */
                             0x01,                    /* International */
                             0x02,                         /* National */
                             0x04                        /* Subscriber */
                             };
static const BYTE Att5_Cr_Numb_Plan[] = {
                             0x00,                          /* Unknown */
                             0x01,                             /* ISDN */
                             0x09                           /* Private */
                             };
static const BYTE Att5_Cr_Prest_Ind[] = {
                             0xFF,  /* No presentation ind (CCIE flag) */
                             0x00,             /* Presentation allowed */
                             0x01,          /* Presentation restricted */
                             0x02               /* No number available */
                             };
static const BYTE Att5_Cr_Scrn_Ind[] = {
                             0xFF,     /* No screening ind (CCIE flag) */
                             0x00,      /* User provided, not screened */
                             0x01,       /* User prov, verified/passed */
                             0x02,       /* User prov, verified/failed */
                             0x03                  /* Network provided */
                             };
#endif
#ifdef BRI_DMS100
static const BYTE Dmsb_Cr_Numb_Type[] = {
                             0x00,                          /* Unknown */
                             0x01,                    /* International */
                             0x02,                         /* National */
                             0x03,                 /* Network specific */
                             0x04,                       /* Subscriber */
                             0x06                       /* Abbreviated */
                             };
static const BYTE Dmsb_Cr_Numb_Plan[] = {
                             0x00,                          /* Unknown */
                             0x01,                             /* ISDN */
                             0x03,                             /* Data */
                             0x09                           /* Private */
                             };
static const BYTE Dmsb_Cr_Prest_Ind[] = {
                             0xFF,  /* No presentation ind (CCIE flag) */
                             0x00,             /* Presentation allowed */
                             0x01,          /* Presentation restricted */
                             0x02               /* No number available */
                             };
static const BYTE Dmsb_Cr_Scrn_Ind[] = {
                             0xFF,     /* No screening ind (CCIE flag) */
                             0x00,      /* User provided, not screened */
                             0x01,       /* User prov, verified/passed */
                             0x02,       /* User prov, verified/failed */
                             0x03                  /* Network provided */
                             };
#endif
#ifdef KDD_NTT
static const BYTE Ntkdb_Cr_Numb_Type[] = {
                             0x00                           /* Unknown */
                             };
static const BYTE Ntkdb_Cr_Numb_Plan[] = {
                             0x00                           /* Unknown */
                             };
static const BYTE Ntkdb_Cr_Prest_Ind[] = {
                             0x00,     /* presentation ind (CCIE flag) */
                             0x01,          /* Presentation restricted */
                             0x02              /* Number not available */
                             };
static const BYTE Ntkdb_Cr_Scrn_Ind[] = {
                             0x00,      /* User provided, not screened */
                             0x01,       /* User prov, verified/passed */
                             0x02,       /* User prov, verified/failed */
                             0x03                  /* Network provided */
                             };
#endif
#ifdef VN2
static const BYTE Vn2_Cr_Numb_Type[] = {
                             0x00,                          /* Unknown */
                             0x02,                         /* National */
                             0x01,                    /* International */
                             0x04                        /* Subscriber */
                             };
static const BYTE Vn2_Cr_Numb_Plan[] = {
                             0x00,                          /* Unknown */
                             0x01,                        /* ISDN Plan */
                             0x09                           /* Private */
                             };
static const BYTE Vn2_Cr_Prest_Ind[] = {
                             0x00,          /* Authorized presentation */
                             0x01,      /* Non-authorized presentation */
                             0x02   /* Number not avail - interworking */
                             };
static const BYTE Vn2_Cr_Scrn_Ind[] = {
                             0x00,      /* User provided, not screened */
                             0x01,       /* User prov, verified/passed */
                             0x03                  /* Network provided */
                             };
#endif
#ifdef BRI_NET3		/*===*/
static const BYTE Net3_Cr_Numb_Type[] = {
                             0x00,                          /* Unknown */
                             0x01,                    /* International */
                             0x02,                         /* National */
                             0x03,                          /* network */
                             0x04,                       /* Subscriber */
                             0x06,                      /* abbreviated */
                             0x07                          /* reserved */
                             };
static const BYTE Net3_Cr_Numb_Plan[] = {
                             0x00,                          /* Unknown */
                             0x01,                        /* ISDN Plan */
                             0x03,                             /* data */
                             0x04,                            /* telex */
                             0x08,                         /* national */
                             0x09,                          /* private */
                             0x0f                          /* reserved */
                             };
static const BYTE Net3_Cr_Prest_Ind[] = {
                             0x00,          /* Authorized presentation */
                             0x01,      /* Non-authorized presentation */
                             0x02,  /* Number not avail - interworking */
                             0x03                          /* reserved */
                             };
static const BYTE Net3_Cr_Scrn_Ind[] = {
                             0x00,      /* User provided, not screened */
                             0x01,       /* User prov, verified/passed */
                             0x03                  /* Network provided */
                             };
#endif
#ifdef BRI_TS013
static const BYTE Ts013_Cr_Numb_Type[] = {
                             0x00,                          /* Unknown */
                             0x01,                    /* International */
                             0x02,                         /* National */
                             0x04                        /* Subscriber */
                             };
static const BYTE Ts013_Cr_Numb_Plan[] = {
                             0x00,                          /* Unknown */
                             0x01,                        /* ISDN Plan */
                             0x03,                    /* data -  X.121 */
                             0x09                           /* Private */
                             };
static const BYTE Ts013_Cr_Prest_Ind[] = {
                             0x00,          /* Authorized presentation */
                             0x01,      /* Non-authorized presentation */
                             0x02,  /* Number not avail - interworking */
                             0x03                          /* Reserved */
                             };
static const BYTE Ts013_Cr_Scrn_Ind[] = {
                             0x01,       /* User prov, verified/passed */
                             0x03                  /* Network provided */
                             };
#endif
#ifdef BRI_NI1	/*===*/

static const BYTE NI1_Cr_Numb_Type[] = {
                             0x00,                          /* Unknown */
                             0x01,                   /* Internaltional */
                             0x02,                         /* National */
                             0x03,                          /* Network */
                             0x04,                            /* Local */
                             0x06                           /* Private */
                             };
static const BYTE NI1_Cr_Numb_Plan[] = {
                             0x00,                          /* Unknown */
                             0x01                              /* ISDN */
                             };
static const BYTE NI1_Cr_Prest_Ind[] = {
                             0x00,             /* Presentation allowed */
                             0x01,          /* Presentation restricted */
                             0x02               /* No number available */
                             };
static const BYTE NI1_Cr_Scrn_Ind[] = {
                             0x00,      /* User provided, not screened */
                             0x01,       /* User prov, verified/passed */
                             0x02,       /* User prov, verified/failed */
                             0x03                  /* Network provided */
                             };
#endif

/***********************************************************************
*         Data used in the analysis of the Cause Info Element.         *
************************************************************************/

PRIVATE const BYTE *Cause_Location;     /* Cause location list pointer */
PRIVATE int   Cause_Location_Size;      /* Size of cause location list */

/* Legal Cause IE field values.
*/
#ifdef BRI_5ESS
static const BYTE Att5_Cause_Location[] = {
                             0x00,                             /* User */
                             0x01,               /* Local priv network */
                             0x02,                    /* Local network */
                             0x03,                  /* Transit network */
                             0x04,               /* Remote loc network */
                             0x05,              /* Remote priv network */
                             0x07,            /* International network */
                             0x0A /* Network beyond interworking point */
                             };
#endif
#ifdef BRI_DMS100
static const BYTE Dmsb_Cause_Location[] = {
                             0x00,                             /* User */
                             0x01,               /* Local priv network */
                             0x02,                    /* Local network */
                             0x03,                  /* Transit network */
                             0x04,               /* Remote loc network */
                             0x05,              /* Remote priv network */
                             0x07,            /* International network */
                             0x0A /* Network beyond interworking point */
                             };
#endif
#ifdef KDD_NTT
static const BYTE Ntkdb_Cause_Location[] = {
                             0x00,                             /* user */
                             0x03, /* private network serving loc user */
                             0x04, /* public ntwrk serving remote user */
                             0x05,/* private ntwrk serving remote user */
                             0x07,            /* international network */
                             0x0A   /* ntwrk beyond interworking point */
                             };
#endif
#ifdef BRI_1TR6
static const BYTE B1tr6_Cause_Location[] = {
                             0x04,                       /* public net */
                             0x05,                  /* private network */
                             0x0F   /* keine Angabe uber Entsthungsort */
                             };

#endif
#ifdef VN2
static const BYTE Vn2_Cause_Location[] = {
							 0, 1, 2, 3, 4, 5,
                             0x07,          /* Location not significant */
							 0xa
                             };
#endif
#ifdef BRI_NET3 	/*===*/
static const BYTE Net3_Cause_Location[] = {
							 0, 1, 2, 3, 4, 5,
                             0x07,          /* Location not significant */
							 0xa
                             };
#endif
#ifdef BRI_TS013
static const BYTE Ts013_Cause_Location[] = {
                             0x00,                             /* User */
                             0x01,       /* private network local user */
                             0x02,                       /* public net */
                             0x04        /* public network remote user */
                             };
#endif
#ifdef BRI_NI1	/*===*/
static const BYTE NI1_Cause_Location[] = {
                             0x00,                             /* User */
                             0x01,       /* private network local user */
                             0x02,                       /* public net */
                             0x03,                  /* transit network */
                             0x04,       /* public network remote user */
                             0x05,      /* private network remote user */
                             0x07,            /* international network */
                             0x0A          /* network beyond interwork */
                             };

#endif

/***********************************************************************
*         Data used in the analysis of the Signal Info Element.        *
************************************************************************/

PRIVATE const BYTE *Sig_Val;              /* Signal value list pointer */
PRIVATE int   Sig_Val_Size;               /* Size of signal value list */

/* Legal Signal IE field values.
*/
#ifdef BRI_5ESS
static const BYTE Att5_SigVal[] = {
                             0x00,                        /* Dial tone */
                             0x01,                    /* Ringback tone */
                             0x03,          /* Network congestion tone */
                             0x04,                        /* Busy tone */
                             0x05,                     /* Confirm tone */
                             0x07,                /* Call waiting tone */
                             0x10,                       /* 3.1K audio */
                             0x3F,                        /* Tones off */
                             0x40,          /* Alerting on - pattern 0 */
                             0x41,          /* Alerting on - pattern 1 */
                             0x42,          /* Alerting on - pattern 2 */
                             0x43,          /* Alerting on - pattern 3 */
                             0x44,          /* Alerting on - pattern 4 */
                             0x46,          /* Alerting on - pattern 6 */
                             0x4F                      /* Alerting off */
                             };
#endif
#ifdef BRI_DMS100
static const BYTE Dms_SigVal[] = {
                             0x00,                        /* Dial tone */
                             0x01,                    /* Ringback tone */
                             0x03,          /* Network congestion tone */
                             0x04,                        /* Busy tone */
                             0x05,                     /* Confirm tone */
                             0x07,                /* Call waiting tone */
                             0x10,                       /* 3.1K audio */
                             0x3F,                        /* Tones off */
                             0x40,          /* Alerting on - pattern 0 */
                             0x41,          /* Alerting on - pattern 1 */
                             0x42,          /* Alerting on - pattern 2 */
                             0x43,          /* Alerting on - pattern 3 */
                             0x44,          /* Alerting on - pattern 4 */
                             0x46,          /* Alerting on - pattern 6 */
                             0x4F                      /* Alerting off */
                             };
#endif
#ifdef KDD_NTT
static const BYTE Ntkdb_SigVal[] = {
                             0x00,                        /* Dial tone */
                             0x01,                    /* Ringback tone */
                             0x03,          /* Network congestion tone */
                             0x04,                        /* Busy tone */
                             0x05,                     /* Confirm tone */
                             0x07,                /* Call waiting tone */
                             0x10,                       /* 3.1K audio */
                             0x3F,                        /* Tones off */
                             0x40,          /* Alerting on - pattern 0 */
                             0x41,          /* Alerting on - pattern 1 */
                             0x42,          /* Alerting on - pattern 2 */
                             0x43,          /* Alerting on - pattern 3 */
                             0x44,          /* Alerting on - pattern 4 */
                             0x46,          /* Alerting on - pattern 6 */
                             0x4F                      /* Alerting off */
                             };
#endif
#ifdef BRI_1TR6
static const BYTE B1tr6_SigVal[] = {
                             0x00,                        /* Dial tone */
                             0x01,                    /* Ringback tone */
                             0x03,          /* Network congestion tone */
                             0x04,                        /* Busy tone */
                             0x05,                     /* Confirm tone */
                             0x07,                /* Call waiting tone */
                             0x10,                       /* 3.1K audio */
                             0x3F,                        /* Tones off */
                             0x40,          /* Alerting on - pattern 0 */
                             0x41,          /* Alerting on - pattern 1 */
                             0x42,          /* Alerting on - pattern 2 */
                             0x43,          /* Alerting on - pattern 3 */
                             0x44,          /* Alerting on - pattern 4 */
                             0x46,          /* Alerting on - pattern 6 */
                             0x4F                      /* Alerting off */
                             };
#endif
#ifdef VN2
static const BYTE Vn2_SigVal[] = {
                             0x00,                        /* Dial tone */
                             0x01,                    /* Ringback tone */
                             0x02,                   /* Intercept tone */
                             0x03,          /* Network congestion tone */
                             0x04,                        /* Busy tone */
                             0x05,                     /* Confirm tone */
                             0x06,                    /* Response tone */
                             0x07,                /* Call waiting tone */
                             0x08,              /* Alerting validation */
                             0x3F,                        /* Tones off */
                             0x40,          /* Alerting on - pattern 0 */
                             0x41,          /* Alerting on - pattern 1 */
                             0x42,          /* Alerting on - pattern 2 */
                             0x43,          /* Alerting on - pattern 3 */
                             0x44,          /* Alerting on - pattern 4 */
                             0x46,          /* Alerting on - pattern 6 */
                             0x47,          /* Alerting on - pattern 7 */
                             0x4F                      /* Alerting off */
                             };

#endif
#ifdef BRI_NET3 
static const BYTE Net3_SigVal[] = {
                             0x00,                        /* Dial tone */
                             0x01,                    /* Ringback tone */
                             0x02,                   /* Intercept tone */
                             0x03,          /* Network congestion tone */
                             0x04,                        /* Busy tone */
                             0x05,                     /* Confirm tone */
                             0x06,                    /* Response tone */
                             0x07,                /* Call waiting tone */
                             0x08,              /* Alerting validation */
                             0x3F,                        /* Tones off */
                             0x40,          /* Alerting on - pattern 0 */
                             0x41,          /* Alerting on - pattern 1 */
                             0x42,          /* Alerting on - pattern 2 */
                             0x43,          /* Alerting on - pattern 3 */
                             0x44,          /* Alerting on - pattern 4 */
                             0x46,          /* Alerting on - pattern 6 */
                             0x47,          /* Alerting on - pattern 7 */
                             0x4F                      /* Alerting off */
                             };

#endif
#ifdef BRI_TS013 
static const BYTE Ts013_SigVal[] = {
                             0x00,                        /* Dial tone */
                             0x01,                    /* Ringback tone */
                             0x02,                   /* Intercept tone */
                             0x03,          /* Network congestion tone */
                             0x04,                        /* Busy tone */
                             0x05,                     /* Confirm tone */
                             0x06,                    /* Response tone */
                             0x07,                /* Call waiting tone */
                             0x08,              /* Alerting validation */
                             0x3F,                        /* Tones off */
                             0x40,          /* Alerting on - pattern 0 */
                             0x41,          /* Alerting on - pattern 1 */
                             0x42,          /* Alerting on - pattern 2 */
                             0x43,          /* Alerting on - pattern 3 */
                             0x44,          /* Alerting on - pattern 4 */
                             0x46,          /* Alerting on - pattern 6 */
                             0x47,          /* Alerting on - pattern 7 */
                             0x4F                      /* Alerting off */
                             };

#endif
#ifdef BRI_NI1	/*===*/
static const BYTE NI1_SigVal[] = {
                             0x00,                        /* Dial tone */
                             0x01,                    /* Ringback tone */
                             0x03,          /* Network congestion tone */
                             0x04,                        /* Busy tone */
                             0x05,                     /* Confirm tone */
                             0x07,                /* Call waiting tone */
                             0x67,                        /* Tones off */
                             0x40,          /* Alerting on - pattern 0 */
                             0x41,          /* Alerting on - pattern 1 */
                             0x42,          /* Alerting on - pattern 2 */
                             0x43,          /* Alerting on - pattern 3 */
                             0x44,          /* Alerting on - pattern 4 */
                             0x46,          /* Alerting on - pattern 6 */
                             0x4F,                     /* Alerting off */
                             0xF2,     /* incoming additonal call tone */
                             0xF3,    /* priority additional call tone */
                             0xFD      /* expensive route warning tone */
                             };
#endif

#ifdef BRI_1TR6
/***********************************************************************
*        Non-standard 1TR6 IEs                                         *
************************************************************************/

/* The equivalent of Bearer Capability is Service_Ind which is in Codeset 6.
These sub-arrays are coded so that the first value is the total number of
value occurences for that Service Octet.  This table should be customized
to suit the particular application.  The customization may be done 
by inserting (BYTE * ) 0 in all but the services you wish to support in
the B1tr6_Service_Ind array. NOTE: === the service ie is also checked in
isdnintrfc.c file in the parse_service routine. ===
*/
#ifdef VARIABLES_NEVER_USED
BYTE *Service_Ind;                         /* pointer to Services list */
int   Service_Ind_Size;                       /* Size of Services list */
BYTE *Service_SubTab;                      /* For accessing sub-arrays */
#endif

static const int  Max_Service_Octets = 16;         						/* Size of Table */
static const BYTE Fernsprechen[]         = { 3, 0x01, 0x02, 0x03 };
static const BYTE Abdienste[]            = { 4, 0x01, 0x02, 0x03, 0x04 };
static const BYTE X21dienste[]           = { 4, 0x04, 0x05, 0x06, 0x0c };
static const BYTE Telefax_Gruppe_4[]     = { 1, 0x00 };
static const BYTE Btx[]                  = { 1, 0x00 };
/*static const BYTE Datenubertragung[]     = { 1, 0x00 }; === */
static const BYTE Datenubertragung[]     = { 2, 0x00, 0xa9 }; /* === for 1tr6 certif */
/*static const BYTE X25[]               = { 6, 0x02, 0x03, 0x04, 0x05, 0x06 }; ===*/
static const BYTE X25[]                  = { 6, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
static const BYTE Teletex_64[]           = { 1, 0x00 };
static const BYTE Mixed_Mode[]           = { 1, 0x00 };
static const BYTE Fernwirken[]           = { 1, 0x00 };
static const BYTE Grafiktelefondienste[] = { 1, 0x00 };
static const BYTE Bildschirmtext[]       = { 1, 0x00 };
static const BYTE Bildtelefon[]          = { 3, 0x01, 0x02, 0x03 };

static const BYTE *B1tr6_Service_Ind[]  = {
                   (BYTE *) 0,
                    Fernsprechen,
                    Abdienste,
                    X21dienste,
                    Telefax_Gruppe_4,
                    Btx,
                   (BYTE *) 0,
                    Datenubertragung,
                    X25, /*(BYTE *) 0, if want to ignore x25, === */
                    Teletex_64,
                    Mixed_Mode,
                   (BYTE *) 0,
                   (BYTE *) 0,
                    Fernwirken,
                    Grafiktelefondienste,
                    Bildschirmtext,
                    Bildtelefon
                    };

static const BYTE B1tr6_Conn_Addr_Type[] = {
                             0x00,                          /* Unknown */
                             0x01,                    /* International */
                             0x02                          /* National */
                             };

static const BYTE B1tr6_Conn_Addr_Plan[] = {
                             0x00,                          /* Unknown */
                             0x01               /* ISDN numbering plan */
                             };

PRIVATE const BYTE *Facility_Code;         /* pointer to Fac Code list */
PRIVATE int   Facility_Code_Size;             /* Size of Fac Code list */

static const BYTE B1tr6_Facility_Code[] = {
                             0x01,                           /* Sperre */
                             0x02,           /* Anrufweiterschaltung I */
                             0x03,          /* Anrufweiterschaltung II */
                             0x0E,                        /* Konferenz */
                             0x0F,               /* B-Kanal ubernehmen */
                             0x10, /* Aktivschalten ein gehalt Verbind */
                             0x11,                  /* Dreierkonferenz */
                             0x12,  /* Einseitiger Dienst-bzw Endgerat */
                             0x13,       /* Zweiseitiger Dienstwechsel */
                             0x14,                     /* Rufnummer-Id */
                             0x15,       /* Geschlossene Benutzergrupp */
                             0x17,          /* Anzeige ubergebener Ruf */
                             0x1A,      /* Anzeige umgeleiteter/weiter */
                             0x1B,         /* Unterdrucken A-Rufnummer */
                             0x1E,        /* Nutzung der Verb deaktive */
                             0x1D,      /* Nutzung der Verbindung ackt */
                             0x1F,      /* Semipermanet Verbindung SPV */
                             0x23,      /* Ruckwechsel bei zweseitigem */
                             0x24        /* Anrufumleitung im privaten */
                             };
PRIVATE const BYTE *Status;                  /* pointer to Status list */
PRIVATE int   Status_Size;                      /* Size of Status list */

static const BYTE B1tr6_Status[] = {
                             0x01,                     /* keine Angabe */
                             0x02                  /* Tln wird gerufen */
                             };
PRIVATE const BYTE *Transmit_Attr;    /* pointer to Transmit attr list */
PRIVATE int   Transmit_Attr_Size;        /* Size of transmit attr list */

static const BYTE B1tr6_Transmit_Attr[] = {
                             0x00,           /* keine Satellitenverb * */
                             0x01,    /* eine Satellitenverb enthalten */
                             0x02,     /* zwei Satellitenverb enhalten */
                             0x03      /* drei Satellitenver enthalten */
                             };

#endif                                                     /* BRI_1TR6 */
/************************************************************************
*     I N F O   E L E M E N T   A N A L Y S I S   F U N C T I O N S     *
*************************************************************************
*/
/************************
* CCBRI_AcceptBearerCap *
*************************
*
* Find and validate the BEARER CAPABILITY info element in the incoming
* message.
*
*/
PUBLIC int CCBRI_AcceptBearerCap()

{
   int     cause;
   BOOLEAN struc_err;
   H_ID  hostcb;	/*===*/
   int i;			/*===*/


   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     BEARER_CAPABILITY_IE, CODESET_0, 1 );
   if ( IE )
      {
      IE_Fields = CCIE_ExpandBearCap_IE( IE, &struc_err );
      /* Check IE for legal (though perhaps unbearable) contents.
       * NOTE: See CCIE_ExpandBearCap_IE() for the IE_Fields organization.
       * Added some checks for NI1.
       */
      if (( IE_Fields[ 1 ] == 0 ) &&          /* CCITT coding standard */
          ( !struc_err )          &&            /* No structural error */
          ( CCBRI_Match( Xfer_Cap,  Xfer_Cap_Size,  IE_Fields[ 2 ] )) &&
          ( CCBRI_Match( Xfer_Mode, Xfer_Mode_Size, IE_Fields[ 3 ] )) &&
          ( CCBRI_Match( Xfer_Rate, Xfer_Rate_Size, IE_Fields[ 4 ] )) &&
          /* (( Xfer_Prot_Size ) ? CCBRI_Match( Xfer_Prot,	=== */
          (( Xfer_Prot_Size && IE_Fields[ 13 ] ) ? CCBRI_Match( Xfer_Prot,
                                             Xfer_Prot_Size,
                                             IE_Fields[ 8 ] ) :
                                TRUE ))
         /* Note that additional checks may be added (and required) here
         *  for some implementations to test, for example, octets 4a,b, 5
         *  of the IE and beyond.
         */
         {
#ifdef COMMAND_INTERFACE
         /* write private info element if needed:
         */
         if ( ( CCB->MakePrivateMsg ) &&
              ( OutPkgInfo ) )
            {
            OutPkgInfo[ OutPkgInfoLen++ ] = BEARER_CAPABILITY_IE;
            OutPkgInfo[ OutPkgInfoLen++ ] = 3;   /* Priliminary length */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ 2 ];/* Capability */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ 3 ];    /* Mode */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ 4 ];    /* Rate */
            }
#endif
         if ( IE_Fields[ 2 ] == 0 )           /* Voice xfer capability */
            {
            cause = NOERR;
            CCB->CallType = VOICE_CALL;
            }
         else
            {
            if (IE_Fields[ 3 ] == 0)
               {
               cause = NOERR;
               CCB->CallType = CIRCUIT_DATA_CALL;
               }
            else
               {
               /* assume packet data - may need to check other fields in
               *  this IE in order to be sure.
               */
               cause = NOERR;
               CCB->CallType = PACKET_DATA_CALL;
               }
            }

         for ( i = 0; i < MAX_HOSTS; i++ )	/*===*/
            {
            hostcb =  &Host_CB [Dsl_ID] [ i ];
            if ((hostcb->Bearer_Routing) && ( hostcb->Bearer_Type == CCB->CallType ))
               {
               CCB->Host_Specifier = i; 
               }
            }  
         }
      else
         {
         switch ( SwitchType )
            {
#ifdef VN2
            case VN2_STYPE:
            case VN3_STYPE:
               cause = ( struc_err ) ? MANDATORY_IE_LEN_ERR :  /* #103 */
                                       INVALID_ELEM_CONTENTS;  /* #100 */
               break;
#endif
#ifdef BRI_NET3 	/*===*/
            case BRI_NET3_STYPE:
               cause = ( struc_err ) ? MANDATORY_IE_LEN_ERR :  /* #103 */
                                       INVALID_ELEM_CONTENTS;  /* #100 */
               break;
#endif
#ifdef BRI_TS013
            case BRI_TS013_STYPE:
               cause = INVALID_ELEM_CONTENTS;             /* #100 */
               break;
#endif
#ifdef BRI_NI1	/*===*/
            case BRI_NI1_STYPE:
               cause = INVALID_ELEM_CONTENTS;             /* Cause 100 */                     
               break;
#endif
            default:
               cause = INVALID_ELEM_CONTENTS;             /* Cause 100 */
               break;
            }
         }
      } else {
          switch (SwitchType) {
            case BRI_5ESS_STYPE:
               cause = INVALID_ELEM_CONTENTS;   /* Cause 100 */
               break;
            default: 
               cause = MANDATORY_IE_MISSING;    /* Cause 96 */
               break;
          }
      }

   return ( cause );
}

/*************************
* CCBRI_AcceptCalledNumb *
**************************
*
* Find and validate the Called Party Number in an incoming message.
*
*/
PUBLIC int CCBRI_AcceptCalledNumb( length, dsl_id )

int  *length;
BYTE dsl_id;

{
   int      cause;                     /* Cause for cause info element */
   BOOLEAN  struc_err;
   int      i;
   BOOLEAN  matched_dn;      /* used for switches trying to match Called number */
#ifdef  SBC_INTERFACE
   char    *ldn;
   ldn = Hcb[ dsl_id ].Ldn;
#endif
   BYTE ces;
   int specifier;
   BOOLEAN verify_dn;

#ifdef  COMMAND_INTERFACE
   Trashcan = dsl_id;                      /* Prevent compiler warning */
#endif
   *length = 0;
   matched_dn = TRUE;
   verify_dn = FALSE;

   specifier = CCB->Host_Specifier;
   ces = CCB->Ces;

   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     CALLED_NUMBER_IE, CODESET_0, 1 );
   if ( IE )
      {
      IE_Fields = CCIE_ExpandPartyNumb_IE( IE, &struc_err );
      if (( !struc_err )                                                    &&
          ( CCBRI_Match( Cd_Numb_Type, Cd_Numb_Type_Size, IE_Fields[ 0 ] )) &&
          ( CCBRI_Match( Cd_Numb_Plan, Cd_Numb_Plan_Size, IE_Fields[ 1 ] )))
         {
         cause = NOERR;                               /* IE appears OK */

#ifdef COMMAND_INTERFACE
         /* copy the directory number into OutPkgInfo if it is present (note:
          *  the CCITT compatible info elements will be copied "en_block"
          *  into OutPkgInfo later
          *  This is the private info element !!!
          */
         TheTrash = dsl_id;                  /* avoid compiler warning */
         if (CCB->MakePrivateMsg) {
            OutPkgInfo[ OutPkgInfoLen++ ] = CALLED_NUMBER_IE;
            OutPkgInfoLen++;              /* Length is skipped for now */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ 0 ];    /* Type */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ 1 ];    /* Plan */
            *length = 4;                         /* Skip to the digits */
            while ( IE_Fields[ *length ] )          /* Copy the digits */
               OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ (*length)++ ];
            OutPkgInfo[ OutPkgInfoLen - *length + 1 ] = *length - 2;

            /*
             * Here we support DN routing, that is match the CALLED_NUMBER
             * with that in the Host_CB.
             */
            if ((SwitchType == BRI_DMS100_STYPE) ||
                (SwitchType == BRI_NI1_STYPE) || (SwitchType == BRI_5ESS_STYPE)) {
                for (i = 0; i < MAX_HOSTS; i++) {
                   if (np_fit[dsl_id][i].DN_Routing) {
                       if (strcmp((char *) np_fit[dsl_id][i].ldn,
                               ((char *) &IE_Fields[4])) == 0 ) {
                            specifier = i;
                            ces = i + 1;
                            matched_dn = TRUE;
                            verify_dn = TRUE;
                            break;	
                       } else {
                           /*
                            * Configured, but no match found.
                            */
                           matched_dn = FALSE;
                       }
                   }
                }
                /*
                 * If :
                 *    - configured and match, allow
                 *    - NOT configured and no match, allow
                 *    - configured and no match, do not allow
                 */ 
                if (!matched_dn) {
                    cause = INCOMPATIBLE_DEST; 
                }

                /*
                 * If an EID already matched, make sure using the same
                 * CES, else set the cause.
                 */
                if (verify_dn) {            
                    if ((CCB->Eid_matched) && (ces != CCB->Ces)) {
                        cause = INCOMPATIBLE_DEST;
                    } else {
                        /*
                         * EID did not match, okay to just use the DN
                         */
                        CCB->Host_Specifier = specifier;
                        CCB->Ces = ces;
                    }
                }
            }
         }
#endif
#ifdef SBC_INTERFACE
         /* now try to match the called party number with our own
         *  directory number - note that the result of this match may
         *  be ignored by Call Control if it is up to the Host to
         *  accept or reject the call.
         */
         for ( i = 4; i < PHONE_NUMB_LEN + 4; i++ )
            {
            if ( *ldn++ != IE_Fields[ i ] )
               {
               cause = CALL_REJECTED;                      /* Cause 21 */
               break;
               }
            }
#endif
         }
      else
         {
#ifdef VN2
         if (( SwitchType == VN2_STYPE || SwitchType == VN3_STYPE ) && ( struc_err ))
            cause = struc_err;                 /* === */
         else
#endif
         cause = INVALID_ELEM_CONTENTS;                   /* Cause 100 */
         }
      } else {
          if (SwitchType == BRI_DMS100_STYPE)
              cause = MANDATORY_IE_MISSING;
          else
              cause = NOERR;
      }

   return ( cause );
}

/**************************
* CCBRI_AcceptCallingNumb *
***************************
*
* Find and validate the Calling Party Number in an incoming message.
*
*/
PUBLIC int   CCBRI_AcceptCallingNumb( calling_number, length )

BYTE *calling_number;
int  *length;

{
   int     cause;                      /* Cause for cause info element */
   BOOLEAN struc_err;

   *length = 0;
   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     CALLER_NUMBER_IE, CODESET_0, 1 );
   if ( IE )
      {
      IE_Fields = CCIE_ExpandPartyNumb_IE( IE, &struc_err );

#ifdef BRI_1TR6
      if ((( !struc_err )                                                   &&
          ( SwitchType == BRI_1TR6_STYPE )                                  &&
          ( CCBRI_Match( Cr_Numb_Type, Cr_Numb_Type_Size, IE_Fields[ 0 ] )) &&
          ( CCBRI_Match( Cr_Numb_Plan, Cr_Numb_Plan_Size, IE_Fields[ 1 ] ))) ||
      	  (( !struc_err )                                                    &&
          ( CCBRI_Match( Cr_Numb_Type, Cr_Numb_Type_Size, IE_Fields[ 0 ] )) &&
          ( CCBRI_Match( Cr_Numb_Plan, Cr_Numb_Plan_Size, IE_Fields[ 1 ] )) &&
          ( CCBRI_Match( Cr_Prest_Ind, Cr_Prest_Ind_Size, IE_Fields[ 2 ] )) &&
          ( SwitchType != BRI_1TR6_STYPE) &&
          ( CCBRI_Match( Cr_Scrn_Ind,  Cr_Scrn_Ind_Size,  IE_Fields[ 3 ] ))))
#else
      if (( !struc_err )                                                    &&
          ( CCBRI_Match( Cr_Numb_Type, Cr_Numb_Type_Size, IE_Fields[ 0 ] )) &&
          ( CCBRI_Match( Cr_Numb_Plan, Cr_Numb_Plan_Size, IE_Fields[ 1 ] )) &&
          ( CCBRI_Match( Cr_Prest_Ind, Cr_Prest_Ind_Size, IE_Fields[ 2 ] )) &&
          ( CCBRI_Match( Cr_Scrn_Ind,  Cr_Scrn_Ind_Size,  IE_Fields[ 3 ] )))
#endif
         {
         cause = NOERR;                               /* IE appears OK */

         /* copy the number into OutPkgInfo if it is present (note: the
         *  CCITT copmatible info elements will be copied "en_block"
         *  into OutPkgInfo later
         *  This is the private info element !!!
         */
         if (CCB->MakePrivateMsg)
            {
            OutPkgInfo[ OutPkgInfoLen++ ] = CALLER_NUMBER_IE;
            OutPkgInfoLen++;              /* Length is skipped for now */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ 0 ];    /* Type */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ 1 ];    /* Plan */
            *length = 4;                         /* Skip to the digits */
            while ( IE_Fields[ *length ] )          /* Copy the digits */
               OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ (*length)++ ];
            OutPkgInfo[ OutPkgInfoLen - *length + 1 ] = *length - 2;
            }
#ifdef ICLID
         STRCPY( calling_number, Unpacked_IE + 4 );
#endif
         }
      else
         {
#ifdef VN2
         if (( SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) && ( struc_err ))
            cause = struc_err;                 /* === */
         else
#endif
         cause = INVALID_ELEM_CONTENTS;                   /* Cause 100 */
#ifdef ICLID
         calling_number[0] = NO_ICLID;
         calling_number[1] = '\0'
#endif
         }
      }
   else
      {
      cause = MANDATORY_IE_MISSING;                        /* Cause 96 */
#ifdef ICLID
      calling_number[0] = NO_ICLID;
      calling_number[1] = '\0'
#endif
      }
#ifndef ICLID
   calling_number[0] = '\0';
#endif

   return ( cause );
}

/********************
* CCBRI_AcceptCause *
*********************
*
* Find and validate the CAUSE info element in the incoming message.
*
* It is assumed by this routine that the info element is mandatory by
* the cause value returned.  If this is not the case the calling routine
* must act accordingly.
*
*/
PUBLIC int   CCBRI_AcceptCause( recovered_cause )

BYTE *recovered_cause;                           /* Cause from message */
{

   int     cause;            /* Cause for cause info element rejection */
   BOOLEAN struc_err;

   *recovered_cause = 0x00;                  /* Initialize this output */

   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     CAUSE_IE, CODESET_0, 1 );
   if ( IE )
      {
      IE_Fields = CCIE_ExpandCause_IE( IE, &struc_err );
      if ((!struc_err)   &&              /* No structural error */
          ((SwitchType == BRI_1TR6_STYPE && 
           (IE[1] == 1 ||                       /* May send 1 byte only  */
            IE[1] == 0 ||                       /* May send 0 byte only === */
            IE_Fields[0] == 2))     ||          /* 1TR6 is national standard */
          (SwitchType != BRI_1TR6_STYPE && 
           IE_Fields[0] == 0 &&          /* CCITT coding standard */
           (CCBRI_Match(Cause_Location, Cause_Location_Size, IE_Fields[1])))))
         {
#ifdef BRI_1TR6
            /* 1TR6 normal clearing has a cause length of 0x00
            */
         if ( (SwitchType == BRI_1TR6_STYPE) && ( IE[1] == 0x00 ) ) 
            {
            *recovered_cause = LEER_ELEMENT; 
            cause = NOERR; 
            }
         else
#endif 
         if ( CCIE_CauseValid( (int) IE_Fields[ 2 ] ))
            {
            /* The significance of the actual cause value must be
            *  interpreted in the context of the state.
            */
            *recovered_cause = IE_Fields[ 2 ];
            cause = NOERR;
            return (cause);
            }
         else
            {
            cause = INVALID_ELEM_CONTENTS;                /* Cause 100 */
            }
         }
      else
         {
#ifdef VN2
         if (( SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) && ( struc_err ))
            cause = struc_err;			/* === */
         else
#endif
         cause = INVALID_ELEM_CONTENTS;                   /* Cause 100 */
         }
      }
   else
      {
      cause = MANDATORY_IE_MISSING;                        /* Cause 96 */
      }
   /*
    * have to put this in for Italy homologation
    */
   *recovered_cause = cause;
   return ( cause );
}

/*********************
* CCBRI_AcceptChanId *
**********************
*
* Find and validate the contents of the CHANNEL IDENTIFICATION information
* element from the incoming message.  Note that this function, on accepting
* the channel, will allocate it in the local channel management table.
*
*/
PUBLIC int   CCBRI_AcceptChanId( channel, exclusive)

BYTE  *channel;
BYTE  *exclusive;

{
   int     cause;
   BOOLEAN exclusive_passes;
   BOOLEAN struc_err;

   cause = NOERR;

   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     CHANNEL_ID_IE, CODESET_0, 1 );
   if ( IE )
      {
      IE_Fields = CCIE_ExpandChanId_IE( IE, &struc_err );

      /* Special switch-specific evaluation of the returned
      *  channel exclusive condition.
      */
      switch ( SwitchType )
         {
#ifdef BRI_DMS100
         case BRI_DMS100_STYPE:
         case BRI_NI1_STYPE:                            /* === */
            exclusive_passes = ( IE_Fields[ 2 ] == 1 );
            break;
#endif
         default:
            exclusive_passes = ( IE_Fields[ 2 ] < 2  );
            break;
         }
      /* Verify the correctness of element coding.
      */
      if (( !struc_err )           &&           /* No structural error */
          ( IE_Fields[ 0 ] == 0 )  &&         /* Implicit interface ID */
          ( IE_Fields[ 1 ] == 0 )  &&          /* Interface type = BRI */
#ifdef USER
          ( IE_Fields[ 4 ] < 3  )  &&     /* No chan, chan 1 or chan 2 */
#else
          ( IE_Fields[ 4 ] < 4  )  &&    /* No chan, chan 1, 2, or ANY */
#endif
          ( exclusive_passes )     &&   /* Exceptable exclusive clause */
          ( IE_Fields[ 3 ] == 0 ))                         /* D-Ch ind */
         {
         *channel   = IE_Fields[ 4 ];               /* Receive channel */
         *exclusive = IE_Fields[ 2 ];                     /* exclusive */
         }
      else
         {
         switch ( SwitchType )
            {
#ifdef BRI_1TR6
            case BRI_1TR6_STYPE:
               cause = CALL_REJECTED_1TR6;                 /* Cause 62 */
               break;              	
#endif
#ifdef VN2
            case VN2_STYPE:
            case VN3_STYPE:
#if 0	/* === */
               cause = ( struc_err ) ? MANDATORY_IE_LEN_ERR :  /* #103 */
                                       INVALID_ELEM_CONTENTS;  /* #100 */
#endif /* === */
               cause = struc_err ? struc_err : INVALID_ELEM_CONTENTS;	/* ===*/
               break;
#endif
            default:
               cause = INVALID_ELEM_CONTENTS;             /* Cause 100 */
               break;
            }
         }
      } else {
         switch (SwitchType) {
            case BRI_5ESS_STYPE:
               cause = INVALID_ELEM_CONTENTS;    /* cause 100 */
               break;
            default:
               cause = MANDATORY_IE_MISSING;     /* cause 96 */
               break;
         }
      }

   return ( cause );
}

#ifdef BRI_1TR6
/****************************
* CCBRI_AcceptConnectedAddr *
*****************************
*
* Find and validate the Connected Addr in an incoming message.
*
*/
PUBLIC int   CCBRI_AcceptConnectedAddr( length, dsl_id )

int  *length;
BYTE dsl_id;

{
   int      cause;                     /* Cause for cause info element */
   BOOLEAN  struc_err;
#ifdef  SBC_INTERFACE
   int      i;
   char    *ldn;
   ldn = Hcb[ dsl_id ].Ldn;
#endif

   *length = 0;

   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     CONNECTED_ADDRESS_IE, CODESET_0, 1 );
   if ( IE )
      {
      IE_Fields = CCIE_ExpandPartyNumb_IE( IE, &struc_err );
      if (( !struc_err )                                                    &&
          ( CCBRI_Match( Cd_Numb_Type, Cd_Numb_Type_Size, IE_Fields[ 0 ] )) &&
          ( CCBRI_Match( Cd_Numb_Plan, Cd_Numb_Plan_Size, IE_Fields[ 1 ] )))
         {
         cause = NOERR;                               /* IE appears OK */

#ifdef COMMAND_INTERFACE
         /* copy the directory number into OutPkgInfo if it is present (note:
         *  the CCITT copmatible info elements will be copied "en_block"
         *  into OutPkgInfo later
         *  This is the private info element !!!
         */
         TheTrash = dsl_id;                  /* avoid compiler warning */
         if (CCB->MakePrivateMsg)
            {
            OutPkgInfo[ OutPkgInfoLen++ ] = CONNECTED_ADDRESS_IE;
            OutPkgInfoLen++;              /* Length is skipped for now */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ 0 ];    /* Type */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ 1 ];    /* Plan */
            *length = 4;                         /* Skip to the digits */
            while ( IE_Fields[ *length ] )          /* Copy the digits */
               OutPkgInfo[ OutPkgInfoLen++ ] = IE_Fields[ (*length)++ ];
            OutPkgInfo[ OutPkgInfoLen - *length + 1 ] = *length - 2;
            }
#endif
#ifdef SBC_INTERFACE
         /* now try to match the called party number with our own
         *  directory number - note that the result of this match may
         *  be ignored by Call Control if it is up to the Host to
         *  accept or reject the call.
         */
         for ( i = 4; i < PHONE_NUMB_LEN + 4; i++ )
            {
            if ( *ldn++ != IE_Fields[ i ] )
               {
               cause = CALL_REJECTED;                      /* Cause 21 */
               break;
               }
            }
#endif
         }
      else
         {
         cause = INVALID_ELEM_CONTENTS;                   /* Cause 100 */
         }
      }
   else
      {
      cause = MANDATORY_IE_MISSING;                        /* Cause 96 */
      }
   return ( cause );

}

#endif                                                     /* BRI_1TR6 */

/**********************
* CCBRI_AcceptDisplay *
***********************
*
* Find and validate the DISPLAY info element in the incoming
* message, recovering the characters.
* Added for Australia switchtype, BRI_TS013_STYPE. 
*/
PUBLIC int CCBRI_AcceptDisplay ( display_chars )

char *display_chars;                           /* Location of chars */

{
   int  i;
   int  cause = NOERR;


   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     DISPLAY_IE, CODESET_0, 1 );
   if ( IE ) {
      if ((IE[1] > 0) && (IE[1] <= 34 )) {
         for ( i = 0; i < IE[1]; i++ ) {
            /*
             * just validate, no reason to keep the Display chars
             * *display_chars++ = IE[i+2];
             */
            if (IE[i+2] & 0x80)
                cause = INVALID_ELEM_CONTENTS;            /* Cause 100 */ 
         }
         *display_chars = 0;  /* Be sure the string is zero terminated */
      } else {
         cause = INVALID_ELEM_CONTENTS;                   /* Cause 100 */
      }
   }
   return ( cause );
}

/*************************
* CCBRI_AcceptEndpointId *
**************************
*
* Find and validate the ENDPOINT ID info element in the incoming message.
*
*/
PUBLIC int   CCBRI_AcceptEndpointId( usid, tid, interpreter )

int *usid;
int *tid;
int *interpreter;

{
   int cause = NOERR;


   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
      ENDPOINT_ID_IE, CODESET_6, 1 );

   /* there is no error checking for this info element.  It may be
   *  added later if there is a need for it.
   */
   if ( IE )
      {
      *usid = IE[2];
      *tid  = IE[3] & 0x7F;
      *interpreter = ( ( IE[3] & 0x80 ) == 0x80 ) ? 1 : 0;
      }
   else
      {
      cause = MANDATORY_IE_MISSING;                        /* Cause 96 */
      }

   return ( cause );
}

#ifdef BRI_1TR6
/*************************
* CCBRI_AcceptFacilities *
**************************
*
*/
PUBLIC int   CCBRI_AcceptFacilities ( facilities, user, nsf )

BYTE *facilities;                         /* facility code is returned */
BOOLEAN user;                                /* message from user side */
BOOLEAN nsf;             /* network specific facilities or service ind */

{

   int     cause ;
   BOOLEAN found = FALSE;
   const BYTE *ptr;
   BYTE *code_ptr;
   const BYTE *subtab;
   BYTE *service_ptr;

   cause = NOERR;
   
   code_ptr = NULL;			/* === used unitialized */
   service_ptr = NULL;		/* === used unitialized */

   subtab = NULL;  
   if (!user)
      {
      IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
       ((nsf) ? NETWORK_FACILITY_IE : FACILITY_SELECT_IE) ,
         ((nsf) ? CODESET_0 : CODESET_6 ),  1 );
      if ( (IE) && (nsf) )
         {
         code_ptr = &IE[3];
         service_ptr = &IE[4];
         }
      else if ( (IE) && (!nsf) )
         {
         code_ptr = &IE[2];
         service_ptr = &IE[3];
         }
      }
   else
      {
      IE = CCBRI_GetHost_IE ( InPkgInfo, ((nsf) ? NETWORK_FACILITY_IE : FACILITY_SELECT_IE) );
      if (IE)
         {
         code_ptr =    &IE[2];
         service_ptr = &IE[3];
         }
     }

   if ( IE )
      {
      for ( ptr = B1tr6_Facility_Code ;(!found) && (ptr != B1tr6_Facility_Code + Facility_Code_Size) ; ptr++)
         {
         if (*code_ptr ==  *ptr)
            {
     /* Check to see if service is within range
     */
    /*if ( (*service_ptr >= 0) & ( *service_ptr < Max_Service_Octets) )=== */
            if ( *service_ptr < Max_Service_Octets) 
               subtab  = B1tr6_Service_Ind [(int) *service_ptr];

     /* Its a hit if not null - however zero may be used for filler
     */
            /* if ( (subtab != NULL ) || ( *subtab == 0 ) ) === */
            if ( (subtab != NULL ) || ( *subtab != 0 ) ) 	/* === */
               found = TRUE;
            *facilities = *ptr;
            cause = NOERR;
            }
         }

      if (!found)
         cause = INVALID_ELEM_CONTENTS;
      }

   return ( cause );
}

#endif                                                     /* BRI_1TR6 */
/*********************
* CCBRI_AcceptKeypad *
**********************
*
* Find and validate the KEYPAD FACILITY info element in the incoming
* message, recovering the digit(s).
*
*/
PUBLIC int CCBRI_AcceptKeypad( digits )

char *digits;                                    /* Location of digits */

{
   int  i;
   int  cause = NOERR;


   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     KEYPAD_FACILITY_IE, CODESET_0, 1 );
   if ( IE )
      {
      if ( ( IE[ 1 ] > 0 ) && ( IE[ 1 ] <= MAX_ADDRESS_DIGITS ) )
         {
         for ( i = 0; i < IE[ 1 ]; i++ )
            *digits++ = IE[ i + 2 ];
         *digits = 0;         /* Be sure the string is zero terminated */
         }
      else
         {
         cause = INVALID_ELEM_CONTENTS;                   /* Cause 100 */
         }
      }
   else
      {
      cause = MANDATORY_IE_MISSING;                        /* Cause 96 */
      }

   return ( cause );
}

/***********************
* CCBRI_AcceptProgress *
************************
*
*/
PUBLIC int CCBRI_AcceptProgress( progress_description )

BYTE *progress_description;
{
   int     cause = NOERR;
   BOOLEAN struc_err;


   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     PROGRESS_IND_IE, CODESET_0, 1 );

   if ( IE )
      {
      IE_Fields = CCIE_ExpandProgressInd_IE( IE, &struc_err );
      if ( !struc_err )
         {
         *progress_description = IE_Fields[ 2 ];
         }
      else
         {
#ifdef VN2
         if (( SwitchType == VN2_STYPE ||SwitchType == VN3_STYPE) && ( struc_err ))
            cause = struc_err;                 /* === */
         else
#endif
         cause = INVALID_ELEM_CONTENTS;                   /* Cause 100 */
         }
      }
   else
      {
      cause = MANDATORY_IE_MISSING;                        /* Cause 96 */
      }

   return ( cause );
}
#ifdef BRI_1TR6
/*************************
* CCBRI_AcceptServiceInd *
**************************
*
*/
PUBLIC int   CCBRI_AcceptServiceInd ( service_description, addition_inf, user )
BYTE *service_description;
BYTE *addition_inf;
BOOLEAN user;                                 /* message from the user */

{
   int     cause;
   BOOLEAN found = FALSE;
   BYTE Service_Octet;
   BYTE Additional_Inf_Octet;
   const BYTE *ptr;
   const BYTE *subtab;
   int  j;

   cause = NOERR;

   if (!user)
     IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
        SERVICE_INDICATOR_IE, CODESET_6,  1 );
   else
     IE = CCBRI_GetHost_IE ( InPkgInfo, SERVICE_INDICATOR_IE );

   if ( IE )
      {
      Service_Octet = IE[2];
      Additional_Inf_Octet =  IE[3];

      j = 0;
      subtab = ptr = B1tr6_Service_Ind [(int) Service_Octet];

      if ( (subtab != NULL ) &&
      /* (( Service_Octet >= 0) && (Service_Octet <= Max_Service_Octets )))  ===*/ 
          (Service_Octet <= Max_Service_Octets ))   
         {
         subtab++;
         for ( j = 0; (j < *ptr) && (!found) ; subtab++ )
           if (*subtab == Additional_Inf_Octet)
              {
              found = TRUE;
              *service_description = Service_Octet;
              *addition_inf = Additional_Inf_Octet;
              }
           else
              j++;
         }

      if (!found)
         cause = INVALID_ELEM_CONTENTS;                   /* Cause 100 */
      }			 		 	               /* End of IE present processing */
   else
      cause = CALL_REJECTED_1TR6;
	
   return ( cause );
}

#endif                                                     /* BRI_1TR6 */
/*********************
* CCBRI_AcceptSignal *
**********************
*
* Find and validate the contents of the SIGNAL information element
* from the incoming message.
*
*/
PUBLIC int   CCBRI_AcceptSignal( signal )

BYTE  *signal;
{
   int     cause;
   BOOLEAN struc_err;

   cause = NOERR;

   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     SIGNAL_IE, CODESET_0, 1 );
   if ( IE )
      {
      IE_Fields = CCIE_ExpandSignal_IE( IE, &struc_err );
      *signal = IE_Fields[ 0 ];
      if (( !struc_err ) &&                     /* No structural error */
          ( CCBRI_Match( Sig_Val, Sig_Val_Size, *signal )))
         {
         cause = NOERR;                          /* No cause for worry */
         }
      else
         {
#ifdef VN2
         if (( SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) && ( struc_err ))
            cause = MANDATORY_IE_LEN_ERR;                 /* Cause 103 */
         else
#endif
         cause = INVALID_ELEM_CONTENTS;                   /* Cause 100 */
         }
      }
   else
      {
      cause = MANDATORY_IE_MISSING;                        /* Cause 96 */
      }

   return ( cause );
}

/************************************************************************
*                     O T H E R   F U N C T I O N S                     *
*************************************************************************
*/

/***************
* isdn_powerup *
****************
*
* Send the startup primative to get isdn going.
*/
PUBLIC void isdn_powerup (dsl)
int dsl;
{
   int ix;
   extern hwidbtype *isdn_idblist[MAX_DSL];

   /* The code below was added to handle the FIT terminals, dms, ni1 and
    * 5ess. The new data structure is not fully used by us, but was in the
    * T'networks code. It expects to use ces values 1 and 2.
    */
   switch ( SwitchType ) {
      case BRI_NI1_STYPE:
      case BRI_5ESS_STYPE:
      case BRI_DMS100_STYPE:
             if (isdn_idblist[dsl]) {
                 for (ix=0; ix < MAX_HOSTS; ix++) {
                     if (!Host_CB[dsl][ix].epid.valid ||
                         !np_fit[dsl][ix].spid_sent) {
                         Host_CB[ dsl ][ ix ].SPID[0] = '\0';
                         Host_CB[ dsl ][ ix ].DN[0] = '\0';
                         Host_CB[ dsl ][ ix ].DN_Routing = FALSE;
                         Host_CB[ dsl ][ ix ].Bearer_Type = '\0';
                         Host_CB[ dsl ][ ix ].Bearer_Routing = FALSE;
                         Host_CB[ dsl ][ ix ].Host_ID = HOST_ID;
                         Host_CB[ dsl ][ ix ].Ces = ix+1;
                         Host_CB[ dsl ][ ix ].Initializing_Terminal = TRUE;
                         Host_CB [ dsl ][ ix ].Terminal_State = TERM_DOWN;
                         /*
                          * Clean up the End Point ID fields 
                          */
                         Host_CB[dsl][ix].epid.valid = FALSE;
                         Host_CB[dsl][ix].epid.epsf = FALSE;
                         Host_CB[dsl][ix].epid.usid = EID_NOTASSIGNED;
                         Host_CB[dsl][ix].epid.tid = EID_NOTASSIGNED;
                     }
                 }
             }

            /* 
             * For FIT Terminals we will assume the Host Control block has 
             * been initialized
             * Start up 1 ces/tei values right away for these switchtypes,
             * then after the first one comes up, start the second one.
             */ 
            CCBFIT_TerminalInit(NL_PU_EST_IND, SIGNALING_CES, dsl);  
            break;

    	default:
    		/* send NL_EST_REQ to L3 (non-zero call_id).  We do not have
    		 * control blocks yet.  Do not use CCBRI_MailtoL3, but rather
    		 * send the message directly via call to LIF:
    		 */
    		LIF_SendPkt(L2P_LAPD, CC_ID, 0, L3_ID, NL_EST_REQ, dsl,
			Q931_SAPI, SIGNALING_CES, 0, 1, 0, FALSE, NULL,
			NOW, REGULAR_MAIL);
    		break;
    }
}

/**********************
* CCBRI_CopyPublic_IE *
***********************
*
* Find and copy an indicated CCITT compatible (public) information element
* from InPkgInfo to OutPkt->Info.
* The assumptions:
*        - InPkgInfo is not NULL,
*        - OutPkt is not NULL,
*        - OutPkt->InfoLen points to the place in OutPkt->Info where the
*          next info element should go,
*        - Only CODESET 0 info elements are being searched.
*
*/
PUBLIC int CCBRI_CopyPublic_IE( info_element_ID )

   BYTE info_element_ID;

{

   int  result_code ;
   int  k;
   int  public_IE_ptr;


   public_IE_ptr = InPkgInfo[ PRIVATE_IE_INDEX ] + PRIVATE_IE_INDEX + 1;

   if ( ( InPkgInfo ) &&
        ( OutPkt )    &&
        ( InPkgInfoLen > public_IE_ptr ) )
      {
      if ( ( IE = CCIE_GetAnyIE( &InPkgInfo[ public_IE_ptr ],
                                      InPkgInfoLen - public_IE_ptr,
                                      info_element_ID,
                                      CODESET_0,
                                      1 ) ) != NULL )
         {
         result_code = NOERR;
         /* copy the info element verbatim.
         */
         OutPkt->Info[ OutPkt->InfoLen++ ] = IE[ 0 ];         /* IE ID */
         OutPkt->Info[ OutPkt->InfoLen++ ] = IE[ 1 ];     /* IE length */

         for ( k = 0; k < IE[ 1 ]; k++ )
            {
            OutPkt->Info[ OutPkt->InfoLen++] = IE[ k + 2 ];
            }
         }
      else
         {
         result_code = ERR;
         }
      }
   else
      {
      result_code = ERR;
      }

   return( result_code );
}

/************************
* CCBRI_DecodeCallState *
*************************
*
* Call a proper function in one of the switch specific modules
*
*/
PUBLIC int CCBRI_DecodeCallState( event )

int *event;
{
   int result;

   switch ( SwitchType )
      {
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
#ifdef USER
         result = CCB5E_DecodeCallState( event );
#else
         result = CCB5EN_DecodeCallState( event );
#endif
         break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
         result = CCBDMS_DecodeCallState( event );
         break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
         result = CCBNTKD_DecodeCallState( event );
         break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
         result = CCB1TR6_DecodeCallState( event );
         break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
         result = CCBVN_DecodeCallState( event );
         break;
#endif
#ifdef BRI_NET3		/*===*/
      case BRI_NET3_STYPE:
         result = CCBNET3_DecodeCallState( event );
         break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
         result = CCBTS013_DecodeCallState( event );
         break;
#endif
#ifdef BRI_NI1
	  case BRI_NI1_STYPE:
         result = CCBNI1_DecodeCallState( event );
		 break;
#endif
      default:
         result = INVALID_SWITCH_TYPE;
         CCBRI_Error( CCBRI_DECODE_CALL_STATE, result, SwitchType, 0, TRUE );
         break;
      }

   return( result );
}

/***************************
* CCBRI_DisconnectHostCall *
****************************
*
* Build and send the HOST_DISCONNECT/DISCONNECT_ACK message to the HOST.
*
*/
PUBLIC void   CCBRI_DisconnectHostCall (event, cause)

BYTE *event;
int   cause;                                   /* Reason for rejection */
{
   int     k;
   int     j;
   int     err;
   int     IE_index;
   BYTE    facilities;

    if (CCBRI_GetOutInfo(*event)) {
        if (cause && (SwitchType != BRI_1TR6_STYPE)) {   
            OutPkgInfo[ OutPkgInfoLen++ ] = 4; /* total length of private IE */
            OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;      /* Build CAUSE_IE */
            OutPkgInfo[ OutPkgInfoLen++ ] = 2;
            OutPkgInfo[ OutPkgInfoLen++ ] = 0x80;
            OutPkgInfo[ OutPkgInfoLen++ ] = cause | 0x80;
        }
    }

    /* 
     * Switch specific Q.931 info elements
     */
    switch (SwitchType) {
        case KDD_STYPE:
        case NTT_STYPE:
            /*
             * This check was added to fix a crash where a
             * ping was attempted with no cable attached.    
             * NTT and 1TR6 only. CSCdi42830.  
             */
            if (InPkt == NULL) {
                CCBRI_MailtoHost(FALSE);    /* mail it */
                break;
            }
            IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
                           ADVICE_OF_CHARGE_IE, CODESET_6, 1);
            CCBRI_MailtoHost (
                ((!IE) || (IE[ 2 ] != 0x82) || ( IE[ 1 ] > MAX_CHARGES_LEN)) ?
                FALSE : TRUE);       /* If the Message includes Charges */
            break;                  /* Copy to the Host */
        case BRI_1TR6_STYPE:
            /*
             * This check was added to fix a crash where a
             * ping was attempted with no cable attached.    
             * NTT and 1TR6 only. CSCdi42830.  
             */
            if (InPkt == NULL) {
                CCBRI_MailtoHost(FALSE);   /* mail it */
                break;
            }
            OutPkgInfo[ OutPkgInfoLen++ ] = 3;/* total length of private IE */
            OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;/* Build CAUSE_IE */
            if (cause == LEER_ELEMENT) {
                OutPkgInfo[ OutPkgInfoLen++ ] = 0;
                k = 2;
            } else {
                OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                OutPkgInfo[ OutPkgInfoLen++ ] = cause;
                k = 3;                             /* total priv IE len */
            }

            if ((err = CCBRI_AcceptFacilities(&facilities, FALSE, TRUE)) == INVALID_ELEM_CONTENTS) {
             /* 
              * The element was present, but was coded incorrectly.
              */
               ;
            } else {
                if (IE) {
                    OutPkgInfo[ OutPkgInfoLen++ ] = NETWORK_FACILITY_IE;
                    j =  OutPkgInfoLen++;
                    OutPkgInfo[ OutPkgInfoLen++ ] = IE[3];   /* Fac Code */
                    OutPkgInfo[ OutPkgInfoLen++ ] = IE[4];   /* Service */
                    OutPkgInfo[ OutPkgInfoLen++ ] = IE[5];   /* addit'nl info */
                    IE_index = 6;
                    while (IE_index < (IE[1] +2 ))   /* Pick up Parafelder */
                        OutPkgInfo[ OutPkgInfoLen++] = IE[ IE_index++ ];
                    k+= IE[1] +1;
                    OutPkgInfo[j] = IE[1] -1;  /* length of Private IE */
                }
            }

            IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                    CHARGING_INFORMATION_IE, CODESET_6, 1);
            if (IE) {
                IE_index = 3;
                OutPkgInfo[ OutPkgInfoLen++ ] = CHARGING_INFORMATION_IE;
                j =  OutPkgInfoLen++ ;
                while (IE_index < (IE[1] +2) )
                    OutPkgInfo[ OutPkgInfoLen++] = IE[IE_index++ ];
                OutPkgInfo[j] = IE[1] -1;
                k += IE[1] +1;
            }
            /* 
             * Date is Mandatory, but on a Disconnect what can you do
             * if its not present -we stickin LEER_ELEMENT
             * which is dead wrong.  I took it out.  RBK  ===
             */
            IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
              DATE_IE, CODESET_6, 1);
            if (IE) {
                IE_index = 2;
                OutPkgInfo[ OutPkgInfoLen++ ] = DATE_IE;
                j =  OutPkgInfoLen++;
                while (IE_index < IE[1] +2)
                    OutPkgInfo[ OutPkgInfoLen++] = IE[IE_index++ ];
                OutPkgInfo[j] = IE[1];
                k += IE[1] +2;
            }
            OutPkgInfo[ OutPkgInfoLen - k -1 ] = k;
            CCBRI_MailtoHost( FALSE );
            break;
        default:
            CCBRI_MailtoHost( FALSE );    /* mail it */
            break;
    }
    return;
}

/**************
* CCBRI_Error *
***************
*
* Perform error reporting and recovery if possible.
*
*/
PUBLIC void   CCBRI_Error( func, err, data1, data2, fatal )

int     func;                 /* Function where the error was detected */
int     err;                                         /* Error detected */
int     data1;                                           /* Error data */
int     data2;                                           /* Error data */
BOOLEAN fatal;                                         /* Fatal error? */

{
   extern void itoa(int, char*, int);

   /* Made these big enough	   === */
   static char err_text[ 512 ];	/* === */
   static char err_func[ 128 ];	/* === */
   static char ascii_data[ 128 ];	/* === */

*err_text = *err_func = *ascii_data = '\0';		/* === */
#ifdef DISPLAY_LOCAL_ERR 
   /* Define error function string.
   */
   switch ( func )
      {
      case CCBRI_CALL_ALERTING:
         STRCPY( err_func, "_CallAlerting" );
         break;
      case CCBRI_CALL_CONNECTED:
         STRCPY( err_func, "_CallConversation" );
         break;
      case CCBRI_CALL_DIALING:
         STRCPY( err_func, "_CallDialing" );
         break;
      case CCBRI_CALL_IDLE:
         STRCPY( err_func, "_CallIdle" );
         break;
      case CCBRI_CALL_LOCKOUT:
         STRCPY( err_func, "_CallLockout" );
         break;
      case CCBRI_CALL_OFFERED:
         STRCPY( err_func, "_CallOffered" );	/* === */
         break;
      case CCBRI_CALL_ORIGINATE:
         STRCPY( err_func, "_CallOrigination" );
         break;
      case CCBRI_CALL_RELEASING:
         STRCPY( err_func, "_CallRelease" );
         break;
      case CCBRI_CALL_OFFHOOK_RELEASING:
         STRCPY( err_func, "_CallOffHookRelease" );	/* === */
         break;
      case CCBRI_CALL_ONHOOK_RELEASING:
         STRCPY( err_func, "_CallOnHookRelease" );
         break;
      case CCBRI_CALL_ROUTING:
         STRCPY( err_func, "_CallBusy" );
         break;
      case CCBRI_RESIZE_INFO:
         STRCPY( err_func, "_ResizeInfo" );
         break;
      case CCBRI_DECODE_CALL_STATE:
         STRCPY( err_func, "_DecodeCallState" );
         break;
      case CCBRI_GET_OUT_PKT:
         STRCPY( err_func, "_GetOutPkt" );
         break;
      case CCBRI_GET_OUT_INFO:
         STRCPY( err_func, "_GetOutInfo" );
         break;
      case CCBRI_GO:
         STRCPY( err_func, "_Go" );
         break;
      case CCBRI_SEND_TRACE:
         STRCPY( err_func, "_SendTrace" );
         break;
      case CCBRI_GET_HCB_INDEX:
         STRCPY( err_func, "_GetHcbIndex" );
         break;
      case CCBRI_GET_L3_EVENT:
         STRCPY( err_func, "_GetL3Event" );
         break;
      case CCBRI_GET_NEW_CCB:
         STRCPY( err_func, "_GetNewCCB" );
         break;
      case CCBRI_VOICE_CONNECTED:
         STRCPY( err_func, "_VoiceConnected");
         break;
      case CCBRI_VOICE_CONFERENCING:
         STRCPY( err_func, "_VoiceConferencing");
         break;
      case CCBRI_VALID_HW_EVENT:
         STRCPY( err_func, "_ValidHwEvent" );
         break;
      case CCBRI_INIT:
         STRCPY( err_func, "_Init" );
         break;
      case CCBRI_MAIL_TO_L3:
         STRCPY( err_func, "_MailToL3" );
         break;
      case CCBRI_MAIL_TO_HOST:
         STRCPY( err_func, "_MailToHost" );
         break;
      case CCBRI_REMOVE_CCB:
         STRCPY( err_func, "_RemoveCCB" );
         break;
      case CCBRI_RELEASE_CHAN:
         STRCPY( err_func, "_ReleaseChan" );
         break;
      case CCBRI_RELEASE_CALL:
         STRCPY( err_func, "_ReleaseCall" );
         break;
      case CCBRI_SET_NEW_CHAN:
         STRCPY( err_func, "_SetNewChan" );
         break;
      default:
         STRCPY( err_func, "Unknown error function" );
         break;
      }

   /* Define error text string.
   */
   switch ( err )
      {
      case SEND_SBC_FAILED:
         STRCPY( err_text, "LIF_SendSbc returned failure." );
         break;
      case SEND_PKT_FAILED:
         STRCPY( err_text, "LIF_SendPkt returned failure." );
         break;
      case SEND_BUF_FAILED:
         STRCPY( err_text, "LIF_SendBuf returned failure." );
         break;
      case UNKNOWN_EVENT:
         STRCPY( err_text, "Unknown or unexpected event received." );
         break;
      case NO_CCB:
         STRCPY( err_text, "Out of CCB memory blocks." );
         break;
      case OUT_OF_INFO_BUFFERS:
         STRCPY( err_text, "Out of info memory blocks." );
         break;
      case INVALID_HW_EVENT:
         STRCPY( err_text, "Invalid hardware event received." );
         break;
      case UNKNOWN_CALL:
         STRCPY( err_text, "Event received for an unrecognized call." );
         break;
      case UNKNOWN_MSG_SOURCE:
         STRCPY( err_text, "Event received for an unrecognized task ID." );
         break;
      case NO_CALL_SLOT:
         STRCPY( err_text, "No more space in HCB - could not process call." );
         break;
      case INVALID_STATE:
         STRCPY( err_text, "Unknown CCB State." );
         break;
      case INVALID_SWITCH_TYPE:
         STRCPY( err_text, "Unknown Switch Type." );
         break;
      case INVALID_PAS_CCB:
         STRCPY( err_text, "Unexpected passive CCB." );
         break;
      case OUT_OF_PKTS:
         STRCPY( err_text, "Unable to get a packet for mail." );
         break;
      case INITIALIZATION_FAILED:
         STRCPY( err_text, "Module initialization failed." );
         break;
      case CCB_CORRUPTED:
         STRCPY( err_text, "CCB data corrupted." );
         break;
      case OUT_OF_CHANNELS:
         STRCPY( err_text, "Cannot get an idle channel from chan mgt." );
         break;
      case CCB_OR_CHAN_TABLE_CORRUPTED:
         STRCPY( err_text, "CCB or channel table corrupted." );
         break;
      case CCB_LIST_CORRUPTED:
         STRCPY( err_text, "Linked list of CCBs corrupted - stranded CCB." );
         break;
      case OUT_OF_CCBS:
         STRCPY( err_text, "Cannot get a CCB from memory mgt." );
         break;
      case HCB_CORRUPTED:
         STRCPY( err_text, "HCB data corrupted." );
         break;
      default:
         STRCPY( err_text, "Unrecognized error code in _Error." );
         break;
      }

   /* Add error data converted to hex strings for display or logging.
   */
   STRCAT( err_text, "  Data-");
   itoa( data1, ascii_data, 16 );
   STRCAT( err_text, ascii_data );
   STRCAT( err_text, ", ");
   itoa( data2, ascii_data, 16 );
   STRCAT( err_text, ascii_data );

   if ( fatal )
      LIF_Fatal( "CCBRI", err_func, err_text );
   else
      LIF_Error( "CCBRI", err_func, err_text );

#else                                         /* NOT DISPLAY_LOCAL_ERR*/

   if ( fatal )
      {
      itoa( func, err_func, 16 );/* Issue the "number" of the err_func */
      itoa( err, err_text, 16 ); /* Issue the "number" of the err_text */
      STRCAT( err_text, " ");
      itoa( data1, ascii_data, 16 );
      STRCAT( err_text, ascii_data );
      STRCAT( err_text, " ");
      itoa( data2, ascii_data, 16 );
      STRCAT( err_text, ascii_data );
      LIF_Fatal( "CCBRI", err_func, err_text );
      }
#endif                                          /* */
   return;
}
/* === following routine added */
/***************
* CCBRI_GetLdn *
****************
*
*/
PUBLIC BYTE * CCBRI_GetLdn()

{
   BYTE *ldn;
   H_ID  hostcb;

   hostcb =  &Host_CB [Dsl_ID] [ CCB->Host_Specifier ];

   if (hostcb->DN[0] == '0' && hostcb->DN[1] == '\0')
      ldn = NULL;
   else
      ldn = (BYTE *) hostcb->DN;

   return ldn;
}
/*********************
* CCBRI_FreeCallBufs *
**********************
*
* Frees up Buffers that may have been allocated when a SETUP IND fails
* message syntax checks.
*/
PUBLIC void CCBRI_FreeCallBufs()
{
   /* Release the incoming packet if it has not already been freed.
   */
   if ( InPkt )
      {
      LIF_FreePkt( InPkt );            /* Release the incoming packet. */
      InPkt = NULL;
      }

   InPkgInfo     = NULL;
   InPkgInfoLen  = 0;

   /* now, release OutPkgInfo if it is not null (the MailtoHost
   *  procedure has made it null if it had executed).
   */
   if ( OutPkgInfo )
      {
      LIF_FreeBuffer( LRG_INFO_BTYPE, OutPkgInfo );
	   OutPkgInfo    = NULL;
      OutPkgInfoLen = 0;
      }

   return;
}

/********************
* CCBRI_GetDummyCcb *
*********************
*
* Initialize and return a pointer to the DummyCcb used only to
* reject setups, for example, when we are out of real CCBs.
*
*/
PUBLIC CCB_ID   CCBRI_GetDummyCcb( call_id, ces, sapi )

   int  call_id;
   BYTE ces;
   BYTE sapi;

{
   Dummy_Ccb.Call_ID   = ( call_id ) ? call_id : CCBRI_GetNewCall_Id();
   Dummy_Ccb.Ces       = ces;
   Dummy_Ccb.Sapi      = sapi;
   Dummy_Ccb.CallRefLen = DEFAULT_CR_LEN;
   Dummy_Ccb.CallRef    = 0;
   Dummy_Ccb.State      = IDLE_STATE;

   return ( &Dummy_Ccb );
}

/********************
* CCBRI_GetHcbIndex *
*********************
*
* Return the next available Hcb index.  If all indexes are in use
* - return 0 (note that 0 is an invalid Hcb index for voice calls).
* NOTE: this function has the meaning only for single byte interface;
*       it always returns 0 if SBC_INTERFACE flag is not on.
*
*/
PUBLIC int   CCBRI_GetHcbIndex( dsl_id )

BYTE  dsl_id;

{
   int index;

#ifdef SBC_INTERFACE
   int i;

   index = ERR;
   for ( i=0; i < MAX_VOICE_CALLS; i++ )
      {
      if ( Hcb[ dsl_id ].Ccb_In_Use[ i ] == NULL )
         {
         index = i;
         break;
         }
      }
#else
   index = 0;
   TheTrash = dsl_id;                        /* avoid compiler warning */
#endif

   return( index );
}

/*******************
* CCBRI_GetHost_IE *
********************
*
* Serach the info string for an occurence of a private info element.
*
*/
PUBLIC BYTE *CCBRI_GetHost_IE ( messagex, ie_id )

   BYTE *messagex;
   BYTE  ie_id;

{
   BYTE *matched_element;
   int   current_byte;
   int   ie_length;

   /* initialize the variables first.
   */
   current_byte = PRIVATE_IE_INDEX;
   ie_length    = messagex[ current_byte++ ];

   if ( ie_length )
      matched_element = CCIE_GetAnyIE( &messagex[ current_byte ], ie_length,
                                       ie_id, CODESET_0, 1 );
   else
      matched_element = NULL;

   return( matched_element );
}
/* === added below routine */
/*******************
* CCBRI_GetL3CCB   *
********************
*
* Setup the CCB for the incoming event from L3 
*
*/

PUBLIC CCB_ID  CCBRI_GetL3CCB( event, call_id, sapi, ces, dsl )

int event;                                                 /* L3 event */
int call_id;                                        /* Call Control ID */
BYTE sapi;                                                    /* sapi  */
int ces;                                         /* Endpoint specifier */
BYTE dsl;                                   /* digital subscriber loop */

{
   int     index;               /* CCB index in hardware control block */
   int     callref;                   /* Working storage callreference */
   int     callreflen = DEFAULT_CR_LEN; /* Length of the call ref numb */

   CCB = CCB_Head[ dsl ];                        /* Start at the head. */
   while ( CCB        &&
         ( ( CCB->Call_ID != call_id ) ||
           ( CCB->Sapi    != sapi    ) ||
           ( CCB->Ces     != ces      )) )
             CCB = CCB->Next;
   if (( CCB == NULL ) && ( event == CC_SETUP_IND ))
      {
      if (( callreflen = InPkt->Info[1] ) == 1 )
         callref = InPkt->Info[2];
      else
         callref = ( InPkt->Info[2] << 8 ) + InPkt->Info[3];

      /* No CCB was found for this message and this ia a
      *  call SETUP from the far end (a CC_SETUP_IND here).
      *  A new CCB will be allocated for this call.
      */
      if (( index = CCBRI_GetHcbIndex( dsl )) != ERR )
         {
         if ( ( CCB = CCBRI_GetNewCCB( IDLE_STATE,
                                       NO_CHANNEL,
                                       sapi,
                                       ces,
                                       index,
                                       CCBRI_GetNewCall_Id() ) ) != NULL )
            {
            CCB->CallRefLen = callreflen;
            CCB->CallRef    = callref;
#ifdef SBC_INTERFACE
            Hcb[ Dsl_ID ].Ccb_In_Use[ index ] = CCB;
            if ( Hcb[ Dsl_ID ].Active_Call == NO_ACTIVE_CALLS )
                Hcb[ Dsl_ID ].Active_Call = index;
#endif
            }
         else
            {
            ;
            }
         }
      else
         {
         /* With no way to handle yet another call we reject
         *  this one automatically.
         */
         if ( ( CCB = CCBRI_GetNewCCB( IDLE_STATE,
                                       NO_CHANNEL,
                                       sapi,
                                       ces,
                                       ERR,                /* HcbIndex */
                                       CCBRI_GetNewCall_Id() ) ) != NULL )
            {
            CCB->CallRefLen = callreflen;
            CCB->CallRef    = callref;
            CCBRI_RejectSetup( USER_BUSY, 0 );
            CCBRI_RemoveCCB( CCB ); 
            }
         else
            {
            ;
            }
         CCB = NULL;
         }
      }
   return ( CCB );
}
/* === added above routine */
/*******************
* CCBRI_GetL3Event *
********************
*
* Unpack Layer 3 event (message type) from the L3 (Q.931) Info string.
*
*/
PUBLIC void   CCBRI_GetL3Event( event )

int *event;

{
   /* Check for notifications tied to a primitive.
   */
   if (( *event == NL_REL_IND )    ||
       ( *event == NL_REL_CONF )   ||
#if 0
       ( *event == NL_EST_IND )    ||
#endif
       ( *event == NL_EST_CONF )   ||
       ( *event == MNL_RESET_REQ ) ||
       ( *event == MNL_MIM_IND ) ||		/*===*/
       ( *event == NL_UI_DATA_IND ))
      {
      /* The event is the prim_id at this level - do nothing further.
      */
      ;
      }

   else if (( InPkt ) && ( InPkt->Info ))
      {
      /* Check if we have a complete packet and info.
      */
      if (( *event = InPkt->Info[ InPkt->Info[ 1 ] + 2 ]) == 0)
         {
         *event = InPkt->Info[ MESSAGE_TYPE_OFFSET + 1];
         }

#ifdef BRI_5ESS
      if ((SwitchType == BRI_5ESS_STYPE) &&
         (GLOBAL_CALL_REF( InPkt->Info[ 2 ], InPkt->Info[ 1 ] )))
         {
         if (( *event != CC_RESTART_IND ) &&
             ( *event != CC_RESTART_CONF ))
            *event = 0;                               /* Unknown event */
         }
#endif                                                     /* BRI_5ESS */
      }
   else
      {
      CCBRI_Error( CCBRI_GET_L3_EVENT, UNKNOWN_EVENT, *event, Source, FALSE );
      *event = 0;                                     /* Unknown event */
      }

   return;
}

/**********************
* CCBRI_GetNewCall_Id *
***********************
*
* Each Call_ID in use on a DSL must be unique.
* Return a new (unambiguous) call_id for the new call.
*
*/
PUBLIC int CCBRI_GetNewCall_Id()

{
   CCB_ID  ccb;
   BOOLEAN sure = FALSE;  /* Not sure the Call_ID is unique on the DSL */

   /* It will be prudent to verify that this new call ID is not in use
   *  at present (by looking at call_id in all CCB's in use on this DSL).
   */
   while ( !sure )
      {
      /* Prepare new call_id value by allowing source of all call_id's to
      *  increment or wrap at 1.  call_id = 0 is reserved by L3 as the
      *  "non-call" call_id.
      */
      if ( ( Call_Id_Source == 0 ) ||
           ( Call_Id_Source == 0x8000 ) )
         Call_Id_Source = 1;

      ccb = CCB_Head[ Dsl_ID ];  /* Start checking call_id at CCB head */
      while ( ccb )
         {
         if ( ccb->Call_ID == Call_Id_Source )     /* Found a conflict */
            {
            Call_Id_Source++;                  /* Try the next call_id */
            break;
            }
         else
            {
            ccb = ccb->Next;               /* Try next CCB in the list */
            }
         }
      sure = ( ccb == NULL );            /* End of CCB list w/no match */
      }

   return ( Call_Id_Source++ );
}

/******************
* CCBRI_GetNewCCB *
*******************
*
* Get a new Call Control Block and initialize it.
*
*/
PUBLIC CCB_ID CCBRI_GetNewCCB (state, b_chan_id, sapi, ces, hcb_index,
                               call_id)
BYTE state;
int  b_chan_id;
BYTE sapi;
BYTE ces;
int  hcb_index;
int  call_id;
{
   /*
    * get a new CCB from the MTEX and ..
    */
   CCB = malloc_named(sizeof(struct CallControlBlock), "isdn-ccb");

   if ( CCB != NULL )
      {
      /* ... initialize it.
      */
      CCB->State          = state;                        /* CCB state */
      CCB->CallType       = VOICE_CALL;           /* Assume voice call */
      CCB->Call_ID        = call_id;
      CCB->B_Chan         = b_chan_id;                    /* B-Channel */
      CCB->FirstDigit     = TRUE;    /* Dial tone requires termination */
#ifdef COMMAND_INTERFACE
      CCB->MakePrivateMsg = TRUE;        /* Create private IE messages */
#else
      CCB->MakePrivateMsg = FALSE;       /* Create private IE messages */
#endif
      CCB->CopyPublicMsg  = TRUE;
      CCB->HcbIndex       = hcb_index;    /* Index in hcb for this ccb */
      CCB->Next           = NULL;                          /* Next CCB */
      CCB->CallRef        = 0;                       /* call reference */
      CCB->CallRefLen     = DEFAULT_CR_LEN;
      CCB->CallAppearance = 1;      /* NOTE: this will be configurable */
      /* CCB->Ces = ( ( ces == BROADCAST_CES ) ? SIGNALING_CES : ces ); === */
      CCB->Ces  = ( ces ) ? ces : SIGNALING_CES;	/* ===??? */
      CCB->Sapi = sapi;
      CCB->Eid_matched = FALSE;       /* Keeps from changing ces again */
      CCB->Host_Specifier = 0;    /* set it to a known default */

      ActiveCCBs[ Dsl_ID ]++;               /* Increment number in use */
      AllocatedCCBs++;

      switch ( SwitchType )
         {
#ifdef KDD_NTT
         case KDD_STYPE:
         case NTT_STYPE:
            CCB->Cause     = 0;
            CCB->NtkdTimer = LIF_AddTimer (CC_ID, CC_ID, DIGIT_COLLECT_TIMEOUT,
                                 Dsl_ID, 0, 0, 0, 0, DIGIT_COLLECT_DEFAULT);

            break;
#endif
#ifdef VN2
         case VN2_STYPE:
         case VN3_STYPE:
            CCB->OtherCallIndex = NO_CALL;      /* Init OtherCallIndex */
            CCB->CollectAddress = TRUE;      /* Collect called address */
            CCB->Service = NO_SERVICE_PENDING;      /* No srv act pend */
            CCB->CCVNTimer = LIF_AddTimer (CC_ID, CC_ID, DISC_HOST_EVENT,
                                Dsl_ID, 0, 0, 0, 0, DISC_HOST_TIMEOUT);
            break;
#endif
#ifdef BRI_NET3		/*===*/
         case BRI_NET3_STYPE:
			CCB->Cause = 0;
            break;
#endif
#ifdef BRI_TS013
         case BRI_TS013_STYPE:
			CCB->Cause = 0;
            break;
#endif
#ifdef BRI_5ESS
         case BRI_5ESS_STYPE:
            CCB->Cause       = 0;
            CCB->CC5essTimer = LIF_AddTimer (CC_ID, CC_ID, DISC_HOST_EVENT,
                                 Dsl_ID, 0, 0, 0, 0, DISC_HOST_TIMEOUT);
            break;
#endif
#ifdef BRI_DMS100
         case BRI_DMS100_STYPE:
            CCB->Cause       = 0;
            CCB->CCDMSTimer = LIF_AddTimer (CC_ID, CC_ID, DISC_HOST_EVENT,
                                 Dsl_ID, 0, 0, 0, 0, DISC_HOST_TIMEOUT);
            break;
#endif
#ifdef BRI_1TR6
         case BRI_1TR6_STYPE:
            CCB->Cause          = 0;
/*            CCB->CopyPublicMsg  = FALSE;	=== */
            CCB->DM_Substate = DM_IDLE;
            CCB->CC1TR6Timer = LIF_AddTimer (CC_ID, CC_ID, DISC_HOST_EVENT,
                                 Dsl_ID, 0, 0, 0, 0, DISC_HOST_TIMEOUT);
            break;
#endif
#ifdef BRI_NI1	/*===*/
		 case BRI_NI1_STYPE:
			CCB->Cause          = 0;
			CCB->CopyPublicMsg  = TRUE;
			/* CCB->Host_ID        = 0;	=== */
			break;
#endif
         default:
            CCB = NULL;
            break;
         }

      /* Add new CCB to the list of CCB's.
      */
      if ( CCB_Head[ Dsl_ID ] ) /* One or more others already present. */
         CCB_Tail[ Dsl_ID ]->Next = CCB;
      else
         CCB_Head[ Dsl_ID ] = CCB;
      CCB_Tail[ Dsl_ID ] = CCB;
      }
   else
      {
      /*
       * Could not get a new CCB; error.
       * But don't make it a fatal error.
       */
#ifdef DISPLAY_LOCAL_ERR 
      CCBRI_Error(CCBRI_GET_NEW_CCB, NO_CCB, AllocatedCCBs,
                  ActiveCCBs[ Dsl_ID ], FALSE);
#endif
      }
   return( CCB );
}

/*******************
* CCBRI_GetOutInfo *
********************
*
* Recover and initialize the outgoing package info buffer for outgoing
* mail to the HOST.
*
*/
PUBLIC BOOLEAN   CCBRI_GetOutInfo( msg_type )

BYTE msg_type;

{

   if (( OutPkgInfo = LIF_GetBuffer( LRG_INFO_BTYPE, TRUE )) != NULL )
      {
      OutPkgInfoType                = LRG_INFO_BTYPE;
      OutPkgInfoLen                 = 0;            /* initialize to 0 */
      OutPkgInfo[ OutPkgInfoLen++ ] = msg_type;          /* message ID */
      OutPkgInfo[ OutPkgInfoLen++ ] = Dsl_ID;                /* DSL ID */
      OutPkgInfo[ OutPkgInfoLen++ ] = CCB->Ces;                 /* CES */

      /* Some compilers have a problem with the cast so we AND with 0xFF too.
      */
      OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE)(( CCB->Call_ID >> 8 ) & 0xFF );
      OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) ( CCB->Call_ID & 0xFF );
      OutPkgInfo[ OutPkgInfoLen ] = 0;	/*=== default length to zero */
      }
   else
      {
      CCBRI_Error( CCBRI_GET_OUT_INFO, OUT_OF_INFO_BUFFERS, 0, 0, TRUE );
      }

   return ( OutPkgInfo != NULL );
}

/******************
* CCBRI_GetOutPkt *
*******************
*
* Recover and initialize the outgoing packet for outgoing mail.
*
* NOTE: OutPkt is an external variable for this function.
*
* NOTE: This function handles both 1 and 2 byte call reference values and
*       1 and 2 byte message types (i.e.: AT&T extended message set).
*/
PUBLIC BOOLEAN CCBRI_GetOutPkt( msg_type, two_bytes )

BYTE    msg_type;                            /* message type to insert */
BOOLEAN two_bytes;                         /* is it a two-byte message */

{
   int callref;
   int callreflen = DEFAULT_CR_LEN;

   /* Get a new packet and ...
   */
   OutPkt = LIF_GetPkt( LRG_INFO_BTYPE, DATA_OFFSET,  FALSE );
   /* ... initialize it using default values that are applicable for
   *  a voice call.
   */
   if ( OutPkt )
      {
      OutPkt->HeadLen  = DATA_OFFSET;                             /* 4 */
      OutPkt->InfoType = LRG_INFO_BTYPE;                    /* default */
#ifdef BRI_1TR6
      if ( SwitchType == BRI_1TR6_STYPE )
         OutPkt->Info[ 0 ] = TRANSACTION_PD_1TR6;          /* protocol */
      else
         OutPkt->Info[ 0 ] = Qdot931;                      /* protocol */
#else
      OutPkt->Info[ 0 ] = Qdot931;                         /* protocol */
#endif
      callref       = CCB->CallRef;
      callreflen    = CCB->CallRefLen;
      if (( OutPkt->Info[ 1 ] = callreflen ) == 1 )     /* Cref length */
         {
         OutPkt->Info[ 2 ] = (BYTE) callref;
         }
      else                                          /* Cref length = 2 */
         {
         OutPkt->Info[ 2 ] = (BYTE)((callref>>8)&0xFF);
         OutPkt->Info[ 3 ] = (BYTE) ( callref & 0xFF );        /* Cref */
         }

      if (two_bytes)
         {
         OutPkt->InfoLen = callreflen + 4;
         OutPkt->Info[ callreflen + 2 ] = 0x00;    /* extended msg set */
         OutPkt->Info[ callreflen + 3 ] = msg_type;      /* message ID */
         }
      else
         {
         OutPkt->InfoLen = callreflen + 3;   /* leave space for L3 hdr */
         OutPkt->Info[ callreflen + 2 ] = msg_type;      /* message ID */
         }
      }
   else
      {
      CCBRI_Error( CCBRI_GET_OUT_PKT, msg_type, CCB->State, OUT_OF_PKTS, TRUE );
      }

   return ( OutPkt != NULL );
}

/********************
* CCBRI_GetPublicIE *
*********************
*
* Find an indicated CCITT (Q.931) compatible (public) information element
* from the HOST-created InPkgInfo.
*
* The assumptions:
*
*    - InPkgInfo is not NULL,
*    - Only CODESET 0 info elements are being searched.
*
*/
PUBLIC BYTE* CCBRI_GetPublicIE( info_element_id, codeset )

int info_element_id;                                /* IE ID to locate */
int codeset;                                         /* codeset for IE */

{
   BYTE *ie = NULL;
   int   public_ie_offset;     /* Public elem start offset in HOST msg */


   public_ie_offset
      = PRIVATE_IE_INDEX +                           /* Address length */
        InPkgInfo[ PRIVATE_IE_INDEX ] +           /* Private IE length */
        1;                                 /* Private IE length length */

   if ( InPkgInfoLen > public_ie_offset )
      {
      ie = CCIE_GetAnyIE( InPkgInfo + public_ie_offset,
                          InPkgInfoLen - public_ie_offset,
                          info_element_id,
                          codeset,
                          1 );
      }

   return( ie );
}

/***********
* CCBRI_Go *
************
*
* Entry point to the module.  The outer layer of message processing.
* Decode the source of the message and recover the CCB for the call.
*
* NOTE: Enter here from a procedure in CCIF.C (or other comparable
*       interface module) after a mail message is received.
*
*/
PUBLIC void CCBRI_Go( source_id, state, event, pkt, call_id, dsl_id,
                      sapi, ces )

int    source_id;                               /* Message source task */
int    state;                                 /* State of sending task */
int    event;                                         /* Driving event */
PKT_ID pkt;                                          /* Message packet */
int    call_id;                                     /* Call identifier */
BYTE   dsl_id;                   /* Digital subscriber loop identifier */
BYTE   sapi;                        /* Service assess point identifier */
BYTE   ces;                              /* Connection endpoint suffix */

{
   int     result_code;                       /* function result codes */
   /* int     index;                CCB index in hardware control block */
   /* int     callref;                    Working storage callreference */
   int     callreflen = DEFAULT_CR_LEN; /* Length of the call ref numb */
   BOOLEAN do_l3_default = FALSE;        /* For L3_ID processing below */
#ifdef COMMAND_INTERFACE
   PKG_ID  in_pkg;                         /* Incoming package pointer */
   /* int call_event_type;   Whether it's call associated or not ===    */
   /* int max_hosts = MAX_HOSTS; */
   int host_specifier;
   BYTE specifier;
#endif
   extern BOOLEAN FitTerm;

   result_code = NOERR;
   CCB = NULL;                                   /* Initialize the CCB */
   InPkt = NULL;                               /* Initialize the InPkt */
   Dsl_ID = dsl_id;                 /* Save digital subscriber loop ID */
   Source = source_id;       /* Save message source for error recovery */
   specifier = ces;			/*===*/

   /* start with quick sanity check
   */
#ifdef USER
   if ((Dsl_ID < MAX_DSL)       &&              /* check for valid DSL */
       (sapi == SIGNALING_SAPI) &&     /* only SAPI = 0 supported here */
       1)
       /* ( ces <= MAX_CES))                       check for valid CES */
#endif
      {
      /* check if this is an establish confirm message.
      */
      if ( ( event == NL_EST_CONF )
#if 0
         || ( event == NL_EST_IND )
#endif
            )
         {
         /* note: the SAPI has to be 0 - see test above.
         */
         Hcb[ Dsl_ID ].Established = TRUE;	/* really needed  ===??? */
         }
      else
         {
         /* the incoming messages will be treated differently based on
         *  the message source.
         */
         switch ( source_id )             /* Source task for the event */
            {
#ifdef SBC_INTERFACE
            case CC_ID:                             /* Dialing Timeout */
            case KB_ID:                    /* Hardware keyboard driver */
               /* The keystrokes from the driver are initially treated as
               *  events for the active voice call.
               */
               Dsl_ID = DEFAULT_DSL;
#ifdef COMP_TEST
               /* Test if the "keystroke" is actually a special function
               *   activation key.
               */
               if ( CCBRI_SpecialFunction( event ) )
                  {
                  ;
                  }
               else
               {
#endif
               ces = SIGNALING_CES;
               if ( Hcb[ DEFAULT_DSL ].Active_Call == NO_ACTIVE_CALLS )
                  {
                  if ((index = CCBRI_GetHcbIndex( DEFAULT_DSL )) != ERR)
                     {
                     if ( CCBRI_GetNewCCB( IDLE_STATE,
                                           NO_CHANNEL,
                                           SIGNALING_SAPI,
                                           SIGNALING_CES,
                                           index,
                                           CCBRI_GetNewCall_Id()))
                        {
                        Hcb[ DEFAULT_DSL ].Ccb_In_Use[ index ] = CCB;
                        Hcb[ DEFAULT_DSL ].Active_Call = index;
                        }
                     else
                        {
                        result_code = OUT_OF_CCBS;
                        }
                     }
                  else
                     {
                     result_code = NO_CALL_SLOT;
                     CCB = NULL;
                     }
                  }
               else
                  {
                  /* the message will apply to the present active call
                  */
                  CCB = Hcb[ DEFAULT_DSL ].Ccb_In_Use[ Hcb[ DEFAULT_DSL ].Active_Call ];
                  }

               if ( CCB && CCBRI_ValidateHwEvent( event, Hcb[ DEFAULT_DSL ].Hw_State ))
                  {
                  result_code = EVENT_PENDING;
                  while ( result_code == EVENT_PENDING )
                     result_code = CCBRI_DecodeCallState( &event );
                  }
               else
                  {
                  ;
                  }
#ifdef COMP_TEST
               }
#endif
               break;
#endif                                                /* SBC_INTERFACE */
#ifdef COMMAND_INTERFACE
            case CC_ID:
               CCB = CCB_Head[ Dsl_ID ];
               while ( CCB &&
#ifdef USER
                     ( ( CCB->Call_ID != call_id ) ||
                       ( CCB->Sapi    != sapi    ) ||
                       ( CCB->Ces     != ces     )) )
                  CCB = CCB->Next;
#endif
               if ( ( CCB ) && ( event == FIT_TIMEOUT ) ) {	/*===*/
                   CCBFIT_TerminalInit ( event, ces, Dsl_ID );
                  }
				else

               if ( CCB )
                  {
                  result_code = EVENT_PENDING;
                  while ( result_code == EVENT_PENDING )
                     result_code = CCBRI_DecodeCallState( &event );
                  }
               else
                  {
                  CCBRI_Error( CCBRI_GO, UNKNOWN_EVENT, event, Source, FALSE );
                  }
               break;
            case HOST_ID:
#ifdef X31_DEV
            case P3_ID:
#endif
               /* the communication between Host and the Call Control will
               *  be always in the form of PACKAGES.
               */
               in_pkg = (PKG_ID) pkt;      /* mail is sent in packages */
               if ( in_pkg )
                  {
                  InPkgInfo = in_pkg->PkgInfo;            /* Save info */
                  InPkgInfoLen = in_pkg->InfoLen;  /* Save info length */
                  /* NOTE: the actual dsl_id may be passed from the Host
                  *        in the package by using the MessageType field.
                  */
                  }

               if ( InPkgInfo )
                  {
                  event   = InPkgInfo[ 0 ];           /* Save msg_type */
                  Dsl_ID  = InPkgInfo[ 1 ];             /* Save dsl_id */

				/* === modified below */
				/* the specifier is 1 less than the ces to index into the
				 *  Host_CB table correctly. It has entries 0,1 and the ces
				 *  range will be 1,2  ===
				 * 1 "Host" is the default case, and we assume that
				 *  InPkgInfo[ 2 ] is the Ces value.  There is only one
				 *  occurence in the Host_CB table, the zeroth.
				 *
				 * If a FIT terminal, we assume that InPkgInfo [ 2 ] is
				 *  the index to the Host_CB table.  It is up to the ISDN
				 *  developer to have initialized the ces to a meaningful
				 *  value.
				 */
				if((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) {
					ces  = InPkgInfo[ 2 ];                /* Save ces */
					specifier = InPkgInfo[ 2 ] - 1;   /* Save specifier === */
					host_specifier = specifier;
				} else {
					ces  = SIGNALING_CES;	/* assume default ces for other switches */
					host_specifier = 0;
				}
				/* === modified above */

                  call_id = ( (InPkgInfo[ 3 ]) << 8 ) + (InPkgInfo[ 4 ] );

                  if ((event == HOST_TERM_REGISTER) && FitTerm) {
#ifdef 0
                  	CCBFIT_TerminalParms ( Dsl_ID, host_specifier, InPkgInfo );
#endif
                    CCBFIT_TerminalInit(HOST_TERM_REGISTER, ces, Dsl_ID);
                  	goto bailout;
                  }

                  if ((event == HOST_CALL_REQUEST) ||
                      (((event == HOST_FACILITY_REGISTER) ||
                       (event == HOST_FACILITY_CANCEL)) && 
                       (SwitchType == BRI_1TR6_STYPE))) {
                     /*
                      * New call with no CCB
                      */
                     if ((CCB = CCBRI_GetNewCCB(IDLE_STATE, NO_CHANNEL,
                                                SIGNALING_SAPI, ces, 0,
                                                call_id)) == NULL) {
                        /*
                         * We have to tell the HOST to back off since we are
                         * not able to deal with the call without a new CCB.
                         */
                        buginf("\nISDN %s: Error: No CCBs available ", DSL_TO_NAME(Dsl_ID));
                        CCB = CCBRI_GetDummyCcb( call_id, ces, sapi );
                        CCBRI_RejectHostCall( TEMPORARY_FAILURE );
                        goto bailout;
                     }
                     CCB->Host_Specifier = host_specifier;
                     Host_CB[dsl_id][specifier].Ces = 
                             (Host_CB[dsl_id][specifier].Ces) ?
                              Host_CB[dsl_id][specifier].Ces : SIGNALING_CES;
                  } else {
                     /*
                      * Search the linked list of CCBs for matching call_id.
                      */
                     CCB = CCB_Head[ Dsl_ID ];
                     while (CCB &&
#ifdef USER
                          ((CCB->Call_ID != call_id) ||
                           (CCB->Sapi    != sapi) ||
                           ((CCB->Ces    != ces) && (CCB->Ces != BROADCAST_CES))))
                               CCB = CCB->Next;
#endif
                     if (CCB != NULL)
		        if (CCB->Ces == BROADCAST_CES)
                     	   CCB->Ces = ces;
                  }

                  /* special case - a HOST_QUERY for channel status.
                  */
                  if ( ( CCB == NULL ) && ( event == HOST_QUERY ) )

#ifdef USER
                     {
                     /* CCB = CCBRI_GetDummyCcb( call_id, ces, sapi ); === */
                     CCB = CCBRI_GetDummyCcb( call_id, specifier, sapi );
                     }
#else
                     {
                     if ( ( CCB = CCBRI_GetNewCCB( IDLE_STATE,
                                                   NO_CHANNEL,
                                                   SIGNALING_SAPI,
                                                   ces, /*=== */
                                                   /*specifier,	=== */
                                                   0,
                                                   call_id ) ) == NULL )
                        {
                        /* We have to tell the HOST to back off since we are
                        *  not able to deal with the call without a new CCB.
                        */
                        CCB = CCBRI_GetDummyCcb( call_id, ces, sapi );
                        CCBRI_RejectHostCall( TEMPORARY_FAILURE );
                        goto bailout;
                        }
                     CCB->CallRefLen = 1;
                     CCB->CallRef    = 0;
                     }
#endif                                                 /* USER/NETWORK */
                  if ( CCB )
                     {
                     result_code = EVENT_PENDING;
                     while ( result_code == EVENT_PENDING )
                        result_code = CCBRI_DecodeCallState( &event );
                     }
                  else if ( event != HOST_DISCONNECT_ACK )
                     {
                     /* send a message to the HOST informing it that the
                     *  call with the specified address is in NULL
                     *  state.
                     */
                     CCB = CCBRI_GetDummyCcb( call_id, ces, sapi );
                     if ( CCBRI_GetOutInfo( HOST_QUERY_RESPONSE ) )
                        {
                        OutPkgInfo[ OutPkgInfoLen++ ] = 3;   /* length */
                        OutPkgInfo[ OutPkgInfoLen++ ] = CALL_STATE_IE;
                        OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;
                        OutPkgInfo[ OutPkgInfoLen++ ] = 0;    /* state */
                        CCBRI_MailtoHost( FALSE );
                        }
#ifdef DEBUG
                     CCBRI_Error( CCBRI_GO, UNKNOWN_CALL, event, 0, FALSE );
#endif
                     }
                  }
               break;
#endif
            case L3_ID:               /* ISDN Layer 3 Protocol Control */
               InPkt = pkt;                    /* Save incoming packet */
               L3_State = state;   /* Save L3 state for error recovery */
               CCBRI_GetL3Event( &event );          /* Decode L3 event */
               switch ( event )
                  {
                  case NL_REL_IND:
                  case NL_REL_CONF:
                     CCBRI_ReleasedLink( ces, Dsl_ID );

                     /*
                      * This is to handle the case where the DMS will remove
                      * all the TEIs after a clear int b 0 command.
                      * For US switchtypes keep L2 up, but sometimes ME will
                      * activate as well.
                      */
                     if (((SwitchType == BRI_DMS100_STYPE) ||
                         (SwitchType == BRI_NI1_STYPE) ||
                         (SwitchType == BRI_5ESS_STYPE)) && isdn_pu_establish) {
                         /*
                          * me_activate_ok
                          *       True  - ME Task to activate
                          *       False - ccbri to activate (right here)
                          */
                         if (!me_activate_ok(dsl_id, ces)) {
                             CCBFIT_TerminalInit(NL_PU_EST_IND, ces, dsl_id);
                         }
                     }
                     break;

                  case CC_RESTART_IND:
                  case CC_RESTART_CONF:
                     CCBRI_Restart( event, Dsl_ID );
                     break;

                  case MNL_RESET_REQ:
                     CCBRI_ShutDown(dsl_id);
                     break;

                  case MNL_MIM_IND:		/*===*/
                    CCBFIT_TerminalInit( event, ces,  dsl_id );
                    break;

#ifdef BRI_DMS100
                  case NL_EST_CONF:
                     if ( SwitchType != BRI_DMS100_STYPE )
                        /* For all Switches other then DMS100 NL_EST_CONF
                        *  will go thru the CC SDLs 
                        */
                        do_l3_default = TRUE;
                     break;
#endif
                  case CC_INFO_IND:
                     /*
                      *   Null call reference
                      */
                     if ((callreflen = InPkt->Info[1]) == 0) {
                        /*
                         * May be NI1 requesting router to send its spid
                         */
                        switch (SwitchType) {
                           case BRI_NI1_STYPE:
                           case BRI_DMS100_STYPE:
                              CCBFIT_TerminalInit(event, ces, dsl_id);
                              break;
                           default:
                              CCB = CCBRI_GetDummyCcb(call_id, ces, sapi);
                              CCB->Call_ID = 0x00;
                              CCB->Ces = ces;
                              if (CCBRI_GetOutInfo(HOST_INFORMATION))
                                  CCBRI_MailtoHost(FALSE);
                              break;
                        }
                        goto bailout;
                     } else {
                        do_l3_default = TRUE;
                     }
                     break;
                  case MNL_EST_IND:		/*===*/
                  case NL_UI_DATA_IND:
                     /* Msgs not supported or requiring no action.
                     */
                     break;

                  case 0:
                     /* A previously detected and logged error.
                     */
#ifdef VN2
                     if ( SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
                        result_code = UNEXPECTED_MSG;
#endif
                     break;

                  default:
                     do_l3_default = TRUE;
                     break;
                  }
               break;                             /* End case of L3_ID */
#ifdef ICLID
#ifdef TASK_ICLID_DRV
            case ICLID_ID:
               /* message from ICLID id always "release a call"
               *  message.
               */
               if (event == RING_CALL_REJECT)
                  {
                  /* any other message from the ARP driver will apply to
                  *  the active call (and will be ignored if there is no
                  *  active call.
                  */
                  if ( ( CCB = Hcb[ DEFAULT_DSL ].Ccb_In_Use[ Hcb[ DEFAULT_DSL ].Active_Call ] ) != NULL )
                     result_code = CCBRI_DecodeVoiceState( &event );
                  }
               break;
#endif
#endif
            case ME_ID:   /* Management Entity - Actions to be defined */
               if ( event == MAL_DEACT_IND )  	/*===*/
               		CCBRI_ReleasedLink ( 0, Dsl_ID );
               else
               		CCBRI_Error(CCBRI_GO, UNKNOWN_EVENT, event, Source, FALSE);
               break;
            default:                                 /* Unknown source */
               CCBRI_Error( CCBRI_GO, UNKNOWN_MSG_SOURCE, event, Source, FALSE );
               break;
            }                      /* End of switch on event source_id */

         /* This BOOLEAN should ONLY be TRUE for messages received 
         *  from L3, and only conditionally so for them.
         */
         if (( do_l3_default ) && ( source_id == L3_ID ))  
            {
            /* We search the list of CCB's for a match of call_id.
            */

            CCB = CCBRI_GetL3CCB( event, call_id, sapi, ces, Dsl_ID );/*===*/

            if ( CCB )
               /* Here we enter the state of the CCB.
               */
               {
               result_code = EVENT_PENDING;
               while ( result_code == EVENT_PENDING )
                  result_code = CCBRI_DecodeCallState( &event );
               }
            else
               {
	      /* assume that we already forgot about this one === */
               if(event != CC_RELEASE_CONF)	            /* === */
                   CCBRI_Error( CCBRI_GO, UNKNOWN_CALL, event, 0, FALSE );
               }
            }                    /* End of default L3 event processing */
         }
      }

bailout:
   /* Release the incoming packet if it has not already been freed.
   */
   if ( InPkt )
      {
      LIF_FreePkt( InPkt );            /* Release the incoming packet. */
      InPkt = NULL;
      }

   InPkgInfo    = NULL;
   InPkgInfoLen = 0;

   /* now, release OutPkgInfo if it is not null (the MailtoHost
   *  procedure has made it null if it had executed).
   */
   if ( OutPkgInfo )
      {
      LIF_FreeBuffer( LRG_INFO_BTYPE, OutPkgInfo );
      OutPkgInfoLen = 0;
      OutPkgInfo = NULL;
      }
   return;
}

/**********************
CCBRI_HostCallRequest *
***********************
*
* Process a request for new call from HOST.
*
*/
PUBLIC int   CCBRI_HostCallRequest()

{
   int     k;
   int     chan;
   BOOLEAN exclusive;
   int     result_code = NOERR;


   if  ( CCBRI_GetOutPkt( CC_SETUP_REQ, FALSE ) )
      {
      if (InPkgInfo[ PRIVATE_IE_INDEX ] == 0x00)
         {
         /* simply copy all CCITT compatible info elements from
         *  InPkgInfo to info string for L3. Assume that the HOST
         *  did error checking already.
         */
         k = PRIVATE_IE_INDEX + 1;
         while ( k < (InPkgInfoLen ) )
            {
            OutPkt->Info[ OutPkt->InfoLen++ ] = InPkgInfo[ k++ ];
            }
         }
      else
         {
         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, BEARER_CAPABILITY_IE ) ) != NULL)
            {
            /* build the CCITT compatible info element.
            */
            OutPkt->Info[ OutPkt->InfoLen++] = BEARER_CAPABILITY_IE;
            OutPkt->Info[ OutPkt->InfoLen++] = 0x03;         /* length */
            /* transfer capability.
            */
            OutPkt->Info[ OutPkt->InfoLen++] = IE [2 ] | 0x80;
            /* transfer mode and transfer rate.
            */
            OutPkt->Info[ OutPkt->InfoLen++ ] = ( IE[3] << 5 ) | IE[4] | 0x80;
            /* now defaults: layer 1 ID, layer 1 protocol.
            */
            /* now defaults: layer 1 ID, layer 1 protocol.
            */
            switch ( SwitchType )
               {
#ifdef VN2
               case VN2_STYPE:
               case VN3_STYPE:
                  OutPkt->Info[ OutPkt->InfoLen++ ] = 0xA3;
                  break;
#endif
               default:
                  OutPkt->Info[ OutPkt->InfoLen++ ] = 0xA2;
                  break;
               }
#ifdef X31_DEV
            if ( IE[ 2 ] == 0 )
               {
               CCB->CallType = VOICE_CALL;
               }
            else if ( IE[ 3 ] == 0 )
               {
               CCB->CallType = CIRCUIT_DATA_CALL;
               }
            else
               {
               CCB->CallType = PACKET_DATA_CALL;
               }
#endif
            }
         else if ( CCBRI_CopyPublic_IE( BEARER_CAPABILITY_IE ) != NOERR )
            {
            /* error - mandatory info element missing.
             * send a proper message to the HOST.
             * exit to bailout !!!
            */
            }

         /* now for non-mandatory info elements.
         */
         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CHANNEL_ID_IE ) ) != NULL )
            {
            exclusive = IE[ 2 ];
            chan      = IE[ 3 ];
            }
         else if (( IE = CCBRI_GetPublicIE( CHANNEL_ID_IE, CODESET_0 ) ) != NULL)
            {
            IE = CCIE_ExpandChanId_IE( IE, &Trashcan );
            exclusive = IE[ 2 ];
            chan = IE[ 4 ];
            }
         else                                 /* Channel not specified */
            {
            exclusive = FALSE;
            chan = ANY_CHANNEL;
            }

         if ( chan == ANY_CHANNEL )
            {
#ifdef USER
            result_code =
               CCBRI_SetNewChan( ANY_CHANNEL, CHANNEL_PROPOSED, FALSE );
#else
            result_code =
               CCBRI_SetNewChan( ANY_CHANNEL, CHANNEL_BUSY, FALSE );
#endif
            }
         else if ( CC_CHAN_GetStatebri( chan,
                                     B_CHANNEL_UNITS,
                                     Dsl_ID ) == CHANNEL_IDLE )
            {
#ifdef USER
            result_code =
               CCBRI_SetNewChan( chan, CHANNEL_PROPOSED, FALSE );
#else
            result_code =
               CCBRI_SetNewChan( chan, CHANNEL_BUSY, FALSE );
#endif
            }
         else if ( !exclusive )
            {
#ifdef USER
            result_code =                         /* Pick another one. */
               CCBRI_SetNewChan( ANY_CHANNEL, CHANNEL_PROPOSED, FALSE );
#else
            result_code =                         /* Pick another one. */
               CCBRI_SetNewChan( ANY_CHANNEL, CHANNEL_BUSY, FALSE );
#endif
            }
         else
            {
            result_code = NO_CIRCUIT_AVAILABLE;            /* Cause 34 */
            }

         if ( result_code == NOERR )
            {
            /* Build the CCITT compatible info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    CCB->B_Chan,       /* channel id * */
                                    /* CCB->B_Chan,     ditto ===??? */
                                    0,		/*===*/
                                    B_CHANNEL_UNITS,
                                    FALSE,           /* not a slot map */
                                    exclusive,            /* exclusive */
                                    0,      /* no interface identifier */
                                    NULL );

            }
         else
            {
            LIF_FreePkt( OutPkt );     /* Release the outgoing packet. */
            OutPkt = NULL;
            goto bailout;
            }

         switch ( SwitchType )
            {
#ifdef VN2
            case VN2_STYPE:
            case VN3_STYPE:
               /* Build Terminal Cap IE preceeded by non-locking codeset shift.
               */
               OutPkt->InfoLen +=
                  CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                             CODESET_6, FALSE );
               if (( IE = CCBRI_GetPublicIE( TERMINAL_CAP_IE,
                                             CODESET_6 )) == NULL )
                  {
                  OutPkt->InfoLen +=
                     CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                           CEPT_NATIONAL_STANDARD,
                                           NATIONAL_FUNCTION );
                  }
               else                              /* Use public element */
                  {
                  OutPkt->Info[ OutPkt->InfoLen++ ] = *IE++;
                  OutPkt->Info[ OutPkt->InfoLen++ ] = *IE++;
                  OutPkt->Info[ OutPkt->InfoLen++ ] = *IE;
                  }
               break;
#endif
            default:
               break;
            }

#ifdef USER
         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, KEYPAD_FACILITY_IE ) ) != NULL )
            {
            /* build the CCITT compatible info element - KEYPAD
            */
            OutPkt->InfoLen +=
               CCIE_BuildKeypad_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    &IE[ 2 ], IE[ 1 ], TRUE );
            }
#endif
#ifdef USER
         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, SWITCHHOOK_IE ) ) != NULL )
            {
            /* build the CCITT compatible info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildHookState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       IE[ 2 ] );        /* hook state */
            }
         else
            {
            CCBRI_CopyPublic_IE( SWITCHHOOK_IE );
            }
#endif                                                         /* USER */
         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CALLER_NUMBER_IE ) ) != NULL )
            {
            /* build the CCITT compatible info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildCallingNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                         IE[ 2 ],       /* number type */
                                         IE[ 3 ],       /* number plan */
                                         0,  /* presentation indicator */
                                         0,     /* screening indicator */
                                         &IE[ 4 ],           /* digits */
                                         IE[ 1 ] - 2, /* number length */
                                         TRUE );       /* ASCII digits */
            }
         else
            {
            CCBRI_CopyPublic_IE( CALLER_NUMBER_IE );
            }
         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo,
                                        CALLED_NUMBER_IE ) ) != NULL )
            {
            /* build the CCITT compatible info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CALLED_NUMBER_IE,
                                       IE[ 2 ],         /* number type */
                                       IE[ 3 ],         /* number plan */
                                       &IE[ 4 ],             /* digits */
                                       IE[ 1 ] - 2,   /* number length */
                                       TRUE );         /* ASCII digits */
            }
         else
            {
            CCBRI_CopyPublic_IE( CALLED_NUMBER_IE );
            }
         CCBRI_CopyPublic_IE( LOW_LAYER_COMPAT_IE );

         /* ******************** CUSTOMIZE HERE ***********************
         *
         * NOTE: The above routine assumes that the bearer capability is
         *       the only mandatory info element in SETUP message. The
         *       routine also scans for a number of PRIVATE and CCCITT
         *       compatibe (PUBLIC) info elements (but will not get upset
         *       if any are missing). This is the place to customize this
         *       search for a particular application (e.g. to take out
         *       a search for elements that will never be used or to
         *       add search for some other info elements).  Notice
         *       that some switches require that the info elements be
         *       in the ascending order of IE ID's (the above routine
         *       complies with this requirement).
         *
         **************************************************************
         */
         /* On a Switchtype-basis, add HIGH_LAYER_COMPAT_IE if required.
         */
         switch ( SwitchType )
            {
#ifdef VN2
            case VN2_STYPE:
            case VN3_STYPE:
               if ( CCBRI_CopyPublic_IE( HIGH_LAYER_COMPAT_IE ) == ERR )
                  {
                  /* Build High Layer Compatibility.
                  */
                  OutPkt->InfoLen +=
                     CCIE_BuildHighLayer_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                             CCITT_STANDARD,
                                             CCITT_CEPT_INTERPRETATION,
                                             PROFILE_PRESENTATION,
                                             TELEPHONY_SERVICE );
                  }
               break;
#endif
            default:
               break;
            }
         }
      /* send the mail now:
      */
      CCBRI_MailtoL3();
      /* NOTE: this function will NOT change the state of the interface.
      *        It is left up to the calling function !
      */
      }
   else
      {
      /* error - no more memory.
      */
      ;
      }

bailout:
   return( result_code );
}

/********************
* CCBRI_HostConnect *
*********************
*
* Process a request from HOST to connect an incoming call.
*
*/
PUBLIC void CCBRI_HostConnect()

{
   int k = 0;
#ifdef BRI_1TR6
   int j;
   int IE_index;
   int cause;
   BYTE service_description;
   BYTE addition_info;
   BYTE facilities;
#endif
   if ( CCBRI_GetOutPkt( CC_SETUP_RESP, FALSE ) )
      {
      if ( InPkgInfo[ PRIVATE_IE_INDEX ] == 0x00 )
         {
         /* simply copy all CCITT compatible info elements from
         *  InPkgInfo to info string for L3. Assume that the HOST
         *  did error checking already.
         */
         k = PRIVATE_IE_INDEX + 1;
         while ( k < (InPkgInfoLen) )
            {
            OutPkt->Info[ OutPkt->InfoLen++ ] = InPkgInfo[ k++ ];
            }
         }
      else
         {
         switch ( SwitchType )
            {
#ifdef BRI_1TR6
            case BRI_1TR6_STYPE:
              if ( CCBRI_AcceptServiceInd
                      ( &service_description, &addition_info, TRUE )
                         != INVALID_ELEM_CONTENTS)
                 {
                 /* build the CCITT compatible info element.
                 */
                 if ( IE )
                    {
                    OutPkt->Info[ OutPkt->InfoLen++] =   SERVICE_INDICATOR_IE;
                    OutPkt->Info[ OutPkt->InfoLen++] =   0x02;/* length */
                    OutPkt->Info[ OutPkt->InfoLen++] =   service_description;
                    OutPkt->Info[ OutPkt->InfoLen++] =   addition_info;
                    }
                 }
              else
                 {
                 CCBRI_RejectHostCall ( INVALID_ELEM_CONTENTS );
                 }

              if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CONNECTED_ADDRESS_IE ) ) != NULL )
                 {
                 /* build the CCITT compatible info element.
                 */
                 OutPkt->InfoLen +=
                    CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CONNECTED_ADDRESS_IE,
                                       IE[ 2 ],         /* number type */
                                       IE[ 3 ],         /* number plan */
                                       &IE[ 4 ],             /* digits */
                                       IE[ 1 ] - 2,   /* number length */
                                       TRUE );         /* ASCII digits */
                 }

              if (( cause = CCBRI_AcceptFacilities( &facilities, TRUE, TRUE )) == NOERR )
                 {
                 if (IE)
                    {
                    OutPkt->Info[ OutPkt->InfoLen++] = NETWORK_FACILITY_IE;
                    j =  OutPkt->InfoLen++;             /* placeholder */
                    OutPkt->Info[ OutPkt->InfoLen++] = 0x00;   /* zero */
                    OutPkt->Info[ OutPkt->InfoLen++] = facilities;/* Fac Code */
                    OutPkt->Info[ OutPkt->InfoLen++] = IE[3];/* Service */
                    OutPkt->Info[ OutPkt->InfoLen++] = IE[4];/* Addition info */
                    k += 4;
                    IE_index = 5;
                    while (IE_index < (IE[1] +2 )  )/* Pick up Parafelder */
                       OutPkt->Info[ OutPkt->InfoLen++]  = IE[ IE_index++ ];
                    k += (IE_index - 5);
                    OutPkt->Info[ j ] = k;
                    }
                 }
               else
                 {
                 /* Reject call - Facility Code was invalid
                 */
                 CCBRI_RejectHostCall (INVALID_ELEM_CONTENTS);
                 LIF_FreePkt( OutPkt );/* Release the outgoing packet. */
                 OutPkt = NULL;
                 goto bailout;
                 }

               break;
#endif
            default:
               /* note: no mandatory info elements in this message.
               */
               if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CHANNEL_ID_IE ) ) != NULL )
                  {
                  /* build the CCITT compatible info element.
			         */
                  OutPkt->InfoLen +=
                     CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          IE[ 3 ],     /* channel id * */
                                          IE[ 3 ],     /* ditto ===??? */
                                          /* 0,		=== */
                                          B_CHANNEL_UNITS,
                                          FALSE,     /* not a slot map */
                                          IE [2 ],        /* exclusive */
                                          0,/* no interface identifier */
                                          NULL );             /* ditto */
                  }
               else
                  {
                  CCBRI_CopyPublic_IE( CHANNEL_ID_IE );
                  }
               if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, PROGRESS_IND_IE ) ) != NULL )
                  {
                  /* build the CCITT compatible info element.
                  */
                  OutPkt->Info[ OutPkt->InfoLen++ ] = PROGRESS_IND_IE;
                  OutPkt->Info[ OutPkt->InfoLen++ ] = 0x02;/* length of IE */
                  /* the following defaults are used here:
                  *       - coding standard = 00 (CCITT)
                  *       - location =      0000 (user)
                  */
                  OutPkt->Info[ OutPkt->InfoLen++ ] = 0x80;
                  /* progress description.
                  */
                  OutPkt->Info[ OutPkt->InfoLen++ ] = 0x80 | IE[ 2 ];
                  }
               else
                  {
                  CCBRI_CopyPublic_IE( PROGRESS_IND_IE );
                  }

               CCBRI_CopyPublic_IE( TERMINAL_CAP_IE );

               /* ****************** CUSTOMIZE HERE ***********************
               *
               * NOTE: The above routine assumes that there are no mandatory
               *       info elements in the CONNECT message.  The routine
               *       scans for a number of PRIVATE and CCCITT compatible
               *       (PUBLIC) info elements (but will not get upset if
               *       any are missing). This is the place to customize this
               *       search for a particular application (e.g. to take out
               *       a search for elements that will never be used or to
               *       add search for some other info elements).  Notice
               *       that some switches require that the info elements be
               *       in the ascending order of IE ID's (the above routine
               *       complies with this requirement).
               *
               **************************************************************
               */
               break;		   				  /* end of general, default */
            }										  /* end of switch statement */
		   }
      /* send the mail now:
      */
      CCBRI_MailtoL3();

      /* NOTE: this function will NOT change the state of the interface.
      *        It is left up to the calling function !
      */
      }

bailout:
   return;
}

/***********************
* CCBRI_HostDisconnect *
************************
*
* Process DISCONNECT and DISCONNECT_ACK messages from the HOST
*
*/
PUBLIC void   CCBRI_HostDisconnect( event )

   int event;

{
   int k;
   BYTE diag[ 2 ];
   BYTE cause;

   switch ( event )
      {
      case HOST_DISCONNECT:
         if  ( CCBRI_GetOutPkt( CC_DISCONNECT_REQ, FALSE ) == FALSE )
            {
            /* error - no more memory.
            */
            ;
            }
         break;

      case HOST_DISCONNECT_ACK:
         if  ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE )  == FALSE )
            {
            /* error - no more memory.
            */
            ;
            }
         break;

      default:
         /* wrong message
         */
         break;
      }

   if ( OutPkt )
      {
      if (InPkgInfo[ PRIVATE_IE_INDEX ] == 0x00)
         {
         /* simply copy all CCITT compatible info elements from
         *  InPkgInfo to info string for L3. Assume that the HOST
         *  did error checking already.
         */
         k = PRIVATE_IE_INDEX + 1;
         while ( k < (InPkgInfoLen) )		/*===*/
            {
            OutPkt->Info[ OutPkt->InfoLen++ ] = InPkgInfo[ k++ ];
            }
         }
      else
         {
         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CAUSE_IE ) ) != NULL)
            {
            /* build the CCITT compatible info element.
            */
#ifdef BRI_1TR6
            if ( SwitchType == BRI_1TR6_STYPE )
               {
               if ( IE[1] == 0 ) 
                  cause = LEER_ELEMENT; 
               else
                  cause = IE[2];  
               diag[ 0 ] = diag[ 1 ] = 0;
               }
            else
#endif 
            cause = IE[2];
            diag[ 0 ] = diag[ 1 ] = 0;
            OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   cause,                     /* cause */
#ifdef USER
                                   FALSE,          /* not network side */
#else
                                   TRUE,               /* network side */
#endif
                                   diag);            /* no diagnostics */
            }

         else if ( CCBRI_CopyPublic_IE( CAUSE_IE ) != NOERR )
            {
            if ( event == HOST_DISCONNECT )
               {
               /* error - mandatory info element missing.
               */
               }
            }
         /* now for non-mandatory info elements.
         */

         /* ******************** CUSTOMIZE HERE ***********************
         *
         * NOTE: The above routine assumes that the cause is the only
         *       mandatory info element in DISCONNECT message (there are
         *       no mandatory info elements in DISCONNECT_ACK message).
         *       The routine also scans for a number of PRIVATE and CCCITT
         *       compatibe (PUBLIC) info elements (but will not get upset
         *       if any are missing). This is the place to customize this
         *       search for a particular application (e.g. to take out
         *       a search for elements that will never be used or to
         *       add search for some other info elements).  Notice
         *       that some switches require that the info elements be
         *       in the ascending order of IE ID's (the above routine
         *       complies with this requirement).
         *
         **************************************************************
         */
         }
      /* send the mail now:
      */
      CCBRI_MailtoL3();
      /* NOTE: this function will NOT change the state of the interface.
      *        It is left up to the calling function !
      */
      }

   return;
}

/*****************
* CCBRI_HostInfo *
******************
*
* Process the INFORMATION message from HOST.
*
*/
PUBLIC void   CCBRI_HostInfo()

{
   int k;


   if  ( CCBRI_GetOutPkt( CC_INFO_REQ, FALSE ) )
      {
      if (InPkgInfo[ PRIVATE_IE_INDEX ] == 0x00)
         {
         /* simply copy all CCITT compatible info elements from
         *  InPkgInfo to info string for L3. Assume that the HOST
         *  did error checking already.
         */
         k = PRIVATE_IE_INDEX + 1;
         while ( k < (InPkgInfoLen ) )
            {
            OutPkt->Info[ OutPkt->InfoLen++ ] = InPkgInfo[ k++ ];
            }
         }
      else
         {
         /* ******************** CUSTOMIZE HERE ***********************
         *
         * NOTE: At present, the Call Control module does not accept
         *       any private info elements in HOST_INFORMATION
         *       message.  This may be modified in order to suit
         *       specific applications.
         *
         **************************************************************
         */
         }
      CCBRI_MailtoL3();
      }
   else
      {
      /* error - no more memory
      */
      }

   return;
}

/******************
* CCBRI_HostQuery *
*******************
*
* Process the QUERY message from HOST.
*
*/
PUBLIC void   CCBRI_HostQuery()

{
   int private_IE_ptr = 0;
   CCB_ID ccb;

   /* unpack the query message;
   */
   if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CHANNEL_STATUS_IE ) ) != NULL)
      {
      /* process channel status request - check for correct message format.
      */
      if ( IE[ 3 ] == CHANNEL_STATUS_REQUEST )
         {
         if ( CCBRI_GetOutInfo( HOST_QUERY_RESPONSE ) )
            {
            /* search all CCB's for the channel ID passed here.
            */
            private_IE_ptr = OutPkgInfoLen++;
            ccb = CCB_Head[ Dsl_ID ];
            while ( ( ccb  )                       &&
                    ( ccb->B_Chan != IE[ 2 ] ) )
               ccb = ccb->Next;

            OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_STATUS_IE;
            OutPkgInfo[ OutPkgInfoLen++ ] = 0x02;            /* length */
            OutPkgInfo[ OutPkgInfoLen++ ] = IE[ 2 ];     /* channel ID */
            if ( ccb == NULL )
               OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_IDLE;
            else if ( ccb->CallType == VOICE_CALL )
               OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_VOICE;
            else
               OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_DATA;
            }
         else
            {
            /* error - no more memory
            */
            goto bailout;
            }
         }
      }

   /* now see if the HOST is requesting call state
   */
   if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CALL_STATE_IE ) ) != NULL)
      {
      /* process call state request
      */
      if ( OutPkgInfo == NULL )
         {
         if ( CCBRI_GetOutInfo( HOST_QUERY_RESPONSE ) )
            {
            private_IE_ptr = OutPkgInfoLen++;
            }
         else
            {
            /* error - no more memory
            */
            goto bailout;
            }
         }
      OutPkgInfo[ OutPkgInfoLen++ ] = CALL_STATE_IE;
      OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;                  /* length */
      OutPkgInfo[ OutPkgInfoLen++ ] = CCB->State;        /* call state */
      }

   if ( OutPkgInfo )
      {
      /* write the total length of private info elements here.
      */
      OutPkgInfo[ private_IE_ptr ] = OutPkgInfoLen - private_IE_ptr - 1;
      /* mail it to HOST.
      */
      CCBRI_MailtoHost( FALSE );
      }
bailout:

   return;
}

/*************
* CCBRI_Init *
**************
*
* Initialize the module and its datastructures.
*
* This procedure is called from CCIF.C on initialization.
*
*/
PUBLIC void CCBRI_Init ()
{
   int   i;
   int   k;
   UINT32 msecs;        /* length of FIT timer in msecs */
   extern hwidbtype *isdn_idblist[MAX_DSL];

   Mail_priority = REGULAR_MAIL;  /* for spid startup */

   /*
    * Initialize some variables:
    */
   Call_Id_Source = 1;                              /* 1st call_id = 1 */
   CCB = NULL;

   for (i=0; i < MAX_DSL; i++)
      {
      ActiveCCBs[ i ] = 0;                              /* CCBs in use */
      CCB_Head[ i ]  = NULL;      /* Initialize the CCB list headcells */
      CCB_Tail[ i ]  = NULL;
      }
   AllocatedCCBs = 0;

/* === the code below was added to handle the FIT terminals, dms, ni1 and
 * 5ess. The new data structure is not fully used by us, but was in the 
 * T'networks code. It expects to use ces values 1 and 2.
 */
   switch ( SwitchType )
      { 
      case BRI_NI1_STYPE:  
      case BRI_5ESS_STYPE:  
      case BRI_DMS100_STYPE:  
          for (k=0; k < MAX_DSL; k++)
             {
             if (isdn_idblist[k]) {
             for (i=0; i < MAX_HOSTS; i++)
                {
                Host_CB[ k ][ i ].SPID[0] = '\0';
                Host_CB[ k ][ i ].DN[0] = '\0';
                /*
                 * keep the spid values in np_fit otherwise these values
                 * locally will not be updated.
                 */
                Host_CB[ k ][ i ].DN_Routing = FALSE;
                Host_CB[ k ][ i ].Bearer_Type = '\0';
                Host_CB[ k ][ i ].Bearer_Routing = FALSE;
                Host_CB[ k ][ i ].Host_ID = HOST_ID; 
                Host_CB[ k ][ i ].Ces = i+1; 
                Host_CB[ k ][ i ].Initializing_Terminal = TRUE;

                /*
                 * Some endpoint id init 
                 */
                Host_CB[k][i].epid.valid = FALSE;
                Host_CB[k][i].epid.epsf = FALSE;
                Host_CB[k][i].epid.usid = EID_NOTASSIGNED;
                Host_CB[k][i].epid.tid = EID_NOTASSIGNED;

                /* now load some parameters for our host interface(s):
                *  This should be CUSTOMIZED or deleted to suit the
                *  specific application
                */
                gpBuildHostCb((char *)Host_CB[ k ][ i ].SPID, 
                           (char *)Host_CB[ k ][ i ].DN, 
                           &(Host_CB[ k ][ i ].DN_Routing),
                           &(Host_CB[ k ][ i ].Bearer_Type),
                           &(Host_CB[ k ][ i ].Bearer_Routing),
                           &(Host_CB[ k ][ i ].Host_ID),
                           &(Host_CB[ k ][ i ].Ces),
                           &(Host_CB[ k ][ i ].Initializing_Terminal),
                           k,
                           i );

                /*
                 * FIT timer for dms100 is different than that of National ISDN.
                 */ 
                if (SwitchType == BRI_DMS100_STYPE)
                    msecs = FIT_TIMEOUT_DEFAULT_DMS;
                else
                    msecs = FIT_TIMEOUT_DEFAULT_NI1;
                Host_CB[k][i].Timer_T415 = LIF_AddTimer(CC_ID, CC_ID, FIT_TIMEOUT,
                                                        Dsl_ID, 0, 0, 0, 0, msecs);
                Host_CB[k][i].Terminal_State = TERM_DOWN; 
                /*
                 * However, some implementations may not take use the multiple host 
                 * feature.  Therefor we minimally need a valid CES - the default 
                 * SIGNALING_CES  
                 */
                Host_CB[k][i].Ces = Host_CB[k][i].Ces ? Host_CB[k][i].Ces : SIGNALING_CES;
                }
             }
             }
          break;    
      default:
         break; 
      }

   SmlBufSize = (LIF_GetBufSize( SML_INFO_BTYPE  ));/* for future uses */

   /* initialize dummy CCB.
   */
   Dummy_Ccb.State      = DUMMY_STATE;
   Dummy_Ccb.B_Chan     = NO_CHANNEL;
   Dummy_Ccb.Call_ID    = 0;
   Dummy_Ccb.Ces        = SIGNALING_CES;
   Dummy_Ccb.Sapi       = SIGNALING_SAPI;
   Dummy_Ccb.CallRef    = 0;
   Dummy_Ccb.CallRefLen = DEFAULT_CR_LEN;

   /* initialize the B channel table:
   */
   if ( !CC_CHAN_InitTablebri( MIN_CHANNEL,      /* Initialize Channel  1 */
                            MAX_CHANNELB,         /* thru Channel 2 */
                            CHANNEL_IDLE,        /* Initial chan state */
                            0,                 /* Protocol not defined */
                            0 ))                    /* Initial Call_ID */
      CCBRI_Error( CCBRI_INIT, INITIALIZATION_FAILED,
                   MIN_CHANNEL, MAX_CHANNELB, TRUE );

   /* Now establish the values of all switch-specific parameters needed
   *  at run-time.
   */
   switch ( SwitchType )
      {
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
         Xfer_Cap            = Att5_Xfer_Cap;
         Xfer_Cap_Size       = sizeof( Att5_Xfer_Cap );
         Xfer_Rate           = Att5_Xfer_Rate;
         Xfer_Rate_Size      = sizeof( Att5_Xfer_Rate );
         Xfer_Mode           = Att5_Xfer_Mode;
         Xfer_Mode_Size      = sizeof( Att5_Xfer_Mode );
         Xfer_Prot           = Att5_Xfer_Prot;
         Xfer_Prot_Size      = sizeof( Att5_Xfer_Prot );
         Cd_Numb_Type        = Att5_Cd_Numb_Type;
         Cd_Numb_Type_Size   = sizeof( Att5_Cd_Numb_Type );
         Cd_Numb_Plan        = Att5_Cd_Numb_Plan;
         Cd_Numb_Plan_Size   = sizeof( Att5_Cd_Numb_Plan );
         Cr_Numb_Type        = Att5_Cr_Numb_Type;
         Cr_Numb_Type_Size   = sizeof( Att5_Cr_Numb_Type );
         Cr_Numb_Plan        = Att5_Cr_Numb_Plan;
         Cr_Numb_Plan_Size   = sizeof( Att5_Cr_Numb_Plan );
         Cr_Scrn_Ind         = Att5_Cr_Scrn_Ind;
         Cr_Scrn_Ind_Size    = sizeof( Att5_Cr_Scrn_Ind );
         Cr_Prest_Ind        = Att5_Cr_Prest_Ind;
         Cr_Prest_Ind_Size   = sizeof( Att5_Cr_Prest_Ind );
         Cause_Location      = Att5_Cause_Location;
         Cause_Location_Size = sizeof( Att5_Cause_Location );
         Sig_Val             = Att5_SigVal;
         Sig_Val_Size        = sizeof( Att5_SigVal );
         break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
         Xfer_Cap            = Dmsb_Xfer_Cap;
         Xfer_Cap_Size       = sizeof( Dmsb_Xfer_Cap );
         Xfer_Rate           = Dmsb_Xfer_Rate;
         Xfer_Rate_Size      = sizeof( Dmsb_Xfer_Rate );
         Xfer_Mode           = Dmsb_Xfer_Mode;
         Xfer_Mode_Size      = sizeof( Dmsb_Xfer_Mode );
         Xfer_Prot           = Dmsb_Xfer_Prot;
         Xfer_Prot_Size      = sizeof( Dmsb_Xfer_Prot );
         Cd_Numb_Type        = Dmsb_Cd_Numb_Type;
         Cd_Numb_Type_Size   = sizeof( Dmsb_Cd_Numb_Type );
         Cd_Numb_Plan        = Dmsb_Cd_Numb_Plan;
         Cd_Numb_Plan_Size   = sizeof( Dmsb_Cd_Numb_Plan );
         Cr_Numb_Type        = Dmsb_Cr_Numb_Type;
         Cr_Numb_Type_Size   = sizeof( Dmsb_Cr_Numb_Type );
         Cr_Numb_Plan        = Dmsb_Cr_Numb_Plan;
         Cr_Numb_Plan_Size   = sizeof( Dmsb_Cr_Numb_Plan );
         Cr_Scrn_Ind         = Dmsb_Cr_Scrn_Ind;
         Cr_Scrn_Ind_Size    = sizeof( Dmsb_Cr_Scrn_Ind );
         Cr_Prest_Ind        = Dmsb_Cr_Prest_Ind;
         Cr_Prest_Ind_Size   = sizeof( Dmsb_Cr_Prest_Ind );
         Cause_Location      = Dmsb_Cause_Location;
         Cause_Location_Size = sizeof( Dmsb_Cause_Location );
         Sig_Val             = Dms_SigVal;
         Sig_Val_Size        = sizeof( Dms_SigVal );
         break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
         Xfer_Cap            = Ntkdb_Xfer_Cap;
         Xfer_Cap_Size       = sizeof( Ntkdb_Xfer_Cap );
         Xfer_Rate           = Ntkdb_Xfer_Rate;
         Xfer_Rate_Size      = sizeof( Ntkdb_Xfer_Rate );
         Xfer_Mode           = Ntkdb_Xfer_Mode;
         Xfer_Mode_Size      = sizeof( Ntkdb_Xfer_Mode );
         Cd_Numb_Type        = Ntkdb_Cd_Numb_Type;
         Cd_Numb_Type_Size   = sizeof( Ntkdb_Cd_Numb_Type );
         Cd_Numb_Plan        = Ntkdb_Cd_Numb_Plan;
         Cd_Numb_Plan_Size   = sizeof( Ntkdb_Cd_Numb_Plan );
         Cr_Numb_Type        = Ntkdb_Cr_Numb_Type;
         Cr_Numb_Type_Size   = sizeof( Ntkdb_Cr_Numb_Type );
         Cr_Numb_Plan        = Ntkdb_Cr_Numb_Plan;
         Cr_Numb_Plan_Size   = sizeof( Ntkdb_Cr_Numb_Plan );
         Cr_Scrn_Ind         = Ntkdb_Cr_Scrn_Ind;
         Cr_Scrn_Ind_Size    = sizeof( Ntkdb_Cr_Scrn_Ind );
         Cr_Prest_Ind        = Ntkdb_Cr_Prest_Ind;
         Cr_Prest_Ind_Size   = sizeof( Ntkdb_Cr_Prest_Ind );
         Cause_Location      = Ntkdb_Cause_Location;
         Cause_Location_Size = sizeof( Ntkdb_Cause_Location );
         Sig_Val             = Ntkdb_SigVal;
         Sig_Val_Size        = sizeof( Ntkdb_SigVal );
         break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
         Cd_Numb_Type        = B1tr6_Conn_Addr_Type;
         Cd_Numb_Type_Size   = sizeof( B1tr6_Conn_Addr_Type );
         Cd_Numb_Plan        = B1tr6_Conn_Addr_Plan;
         Cd_Numb_Plan_Size   = sizeof( B1tr6_Conn_Addr_Plan );
         Cr_Numb_Type        = B1tr6_Conn_Addr_Type;
         Cr_Numb_Type_Size   = sizeof( B1tr6_Conn_Addr_Type );
         Cr_Numb_Plan        = B1tr6_Conn_Addr_Plan;
         Cr_Numb_Plan_Size   = sizeof( B1tr6_Conn_Addr_Plan );
         Facility_Code       = B1tr6_Facility_Code;
         Facility_Code_Size  = sizeof( B1tr6_Facility_Code );
         Status              = B1tr6_Status;
         Status_Size         = sizeof( B1tr6_Status );
         Transmit_Attr       = B1tr6_Transmit_Attr;
         Transmit_Attr_Size  = sizeof ( B1tr6_Transmit_Attr );
         Cause_Location      = B1tr6_Cause_Location;
         Cause_Location_Size = sizeof( B1tr6_Cause_Location );
         Sig_Val             = B1tr6_SigVal;
         Sig_Val_Size        = sizeof( B1tr6_SigVal );
         break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
         Xfer_Cap            = Vn2_Xfer_Cap;
         Xfer_Cap_Size       = sizeof( Vn2_Xfer_Cap );
         Xfer_Rate           = Vn2_Xfer_Rate;
         Xfer_Rate_Size      = sizeof( Vn2_Xfer_Rate );
         Xfer_Mode           = Vn2_Xfer_Mode;
         Xfer_Mode_Size      = sizeof( Vn2_Xfer_Mode );
         Xfer_Prot           = Vn2_Xfer_Prot;
         Xfer_Prot_Size      = sizeof( Vn2_Xfer_Prot );
         Cd_Numb_Type        = Vn2_Cd_Numb_Type;
         Cd_Numb_Type_Size   = sizeof( Vn2_Cd_Numb_Type );
         Cd_Numb_Plan        = Vn2_Cd_Numb_Plan;
         Cd_Numb_Plan_Size   = sizeof( Vn2_Cd_Numb_Plan );
         Cr_Numb_Type        = Vn2_Cr_Numb_Type;
         Cr_Numb_Type_Size   = sizeof( Vn2_Cr_Numb_Type );
         Cr_Numb_Plan        = Vn2_Cr_Numb_Plan;
         Cr_Numb_Plan_Size   = sizeof( Vn2_Cr_Numb_Plan );
         Cr_Scrn_Ind         = Vn2_Cr_Scrn_Ind;
         Cr_Scrn_Ind_Size    = sizeof( Vn2_Cr_Scrn_Ind );
         Cr_Prest_Ind        = Vn2_Cr_Prest_Ind;
         Cr_Prest_Ind_Size   = sizeof( Vn2_Cr_Prest_Ind );
         Cause_Location      = Vn2_Cause_Location;
         Cause_Location_Size = sizeof( Vn2_Cause_Location );
         Sig_Val             = Vn2_SigVal;
         Sig_Val_Size        = sizeof( Vn2_SigVal );
         break;
#endif
#ifdef BRI_NET3		/*===*/ 
      case BRI_NET3_STYPE:
         Xfer_Cap            = Net3_Xfer_Cap;
         Xfer_Cap_Size       = sizeof( Net3_Xfer_Cap );
         Xfer_Rate           = Net3_Xfer_Rate;
         Xfer_Rate_Size      = sizeof( Net3_Xfer_Rate );
         Xfer_Mode           = Net3_Xfer_Mode;
         Xfer_Mode_Size      = sizeof( Net3_Xfer_Mode );
         Xfer_Prot           = Net3_Xfer_Prot;
         Xfer_Prot_Size      = sizeof( Net3_Xfer_Prot );
         Cd_Numb_Type        = Net3_Cd_Numb_Type;
         Cd_Numb_Type_Size   = sizeof( Net3_Cd_Numb_Type );
         Cd_Numb_Plan        = Net3_Cd_Numb_Plan;
         Cd_Numb_Plan_Size   = sizeof( Net3_Cd_Numb_Plan );
         Cr_Numb_Type        = Net3_Cr_Numb_Type;
         Cr_Numb_Type_Size   = sizeof( Net3_Cr_Numb_Type );
         Cr_Numb_Plan        = Net3_Cr_Numb_Plan;
         Cr_Numb_Plan_Size   = sizeof( Net3_Cr_Numb_Plan );
         Cr_Scrn_Ind         = Net3_Cr_Scrn_Ind;
         Cr_Scrn_Ind_Size    = sizeof( Net3_Cr_Scrn_Ind );
         Cr_Prest_Ind        = Net3_Cr_Prest_Ind;
         Cr_Prest_Ind_Size   = sizeof( Net3_Cr_Prest_Ind );
         Cause_Location      = Net3_Cause_Location;
         Cause_Location_Size = sizeof( Net3_Cause_Location );
         Sig_Val             = Net3_SigVal;
         Sig_Val_Size        = sizeof( Net3_SigVal );
         break;
#endif
#ifdef BRI_TS013
	  case BRI_TS013_STYPE:
         Xfer_Cap            = Ts013_Xfer_Cap;
         Xfer_Cap_Size       = sizeof( Ts013_Xfer_Cap );
         Xfer_Rate           = Ts013_Xfer_Rate;
         Xfer_Rate_Size      = sizeof( Ts013_Xfer_Rate );
         Xfer_Mode           = Ts013_Xfer_Mode;
         Xfer_Mode_Size      = sizeof( Ts013_Xfer_Mode );
         Xfer_Prot           = Ts013_Xfer_Prot;
         Xfer_Prot_Size      = sizeof( Ts013_Xfer_Prot );
         Cd_Numb_Type        = Ts013_Cd_Numb_Type;
         Cd_Numb_Type_Size   = sizeof( Ts013_Cd_Numb_Type );
         Cd_Numb_Plan        = Ts013_Cd_Numb_Plan;
         Cd_Numb_Plan_Size   = sizeof( Ts013_Cd_Numb_Plan );
         Cr_Numb_Type        = Ts013_Cr_Numb_Type;
         Cr_Numb_Type_Size   = sizeof( Ts013_Cr_Numb_Type );
         Cr_Numb_Plan        = Ts013_Cr_Numb_Plan;
         Cr_Numb_Plan_Size   = sizeof( Ts013_Cr_Numb_Plan );
         Cr_Scrn_Ind         = Ts013_Cr_Scrn_Ind;
         Cr_Scrn_Ind_Size    = sizeof( Ts013_Cr_Scrn_Ind );
         Cr_Prest_Ind        = Ts013_Cr_Prest_Ind;
         Cr_Prest_Ind_Size   = sizeof( Ts013_Cr_Prest_Ind );
         Cause_Location      = Ts013_Cause_Location;
         Cause_Location_Size = sizeof( Ts013_Cause_Location );
         Sig_Val             = Ts013_SigVal;
         Sig_Val_Size        = sizeof( Ts013_SigVal );
         break;
#endif
#ifdef BRI_NI1	/*===*/
      case BRI_NI1_STYPE:
         Xfer_Cap            = NI1_Xfer_Cap;
         Xfer_Cap_Size       = sizeof( NI1_Xfer_Cap );
         Xfer_Rate           = NI1_Xfer_Rate;
         Xfer_Rate_Size      = sizeof( NI1_Xfer_Rate );
         Xfer_Mode           = NI1_Xfer_Mode;
         Xfer_Mode_Size      = sizeof( NI1_Xfer_Mode );
         Xfer_Prot           = NI1_Xfer_Prot;
         Xfer_Prot_Size      = sizeof( NI1_Xfer_Prot );
#ifdef NEVER_REFERENCED
         L2_Prot             = NI1_L2_Prot;
         L2_Prot_Size        = sizeof(NI1_L2_Prot);     
         L3_Prot             = NI1_L3_Prot; 
         L3_Prot_Size        = sizeof(NI1_L3_Prot);     
#endif /* NEVER_REFERENCED */
         Cd_Numb_Type        = NI1_Cd_Numb_Type;
         Cd_Numb_Type_Size   = sizeof( NI1_Cd_Numb_Type );
         Cd_Numb_Plan        = NI1_Cd_Numb_Plan;
         Cd_Numb_Plan_Size   = sizeof( NI1_Cd_Numb_Plan );
         Cr_Numb_Type        = NI1_Cr_Numb_Type;
         Cr_Numb_Type_Size   = sizeof( NI1_Cr_Numb_Type );
         Cr_Numb_Plan        = NI1_Cr_Numb_Plan;
         Cr_Numb_Plan_Size   = sizeof( NI1_Cr_Numb_Plan );
         Cr_Scrn_Ind         = NI1_Cr_Scrn_Ind;
         Cr_Scrn_Ind_Size    = sizeof( NI1_Cr_Scrn_Ind );
         Cr_Prest_Ind        = NI1_Cr_Prest_Ind;
         Cr_Prest_Ind_Size   = sizeof( NI1_Cr_Prest_Ind );
         Cause_Location      = NI1_Cause_Location;
         Cause_Location_Size = sizeof( NI1_Cause_Location );
         Sig_Val             = NI1_SigVal;
         Sig_Val_Size        = sizeof( NI1_SigVal );
         break;
#endif 
      default:
         CCBRI_Error( CCBRI_INIT, INVALID_SWITCH_TYPE, SwitchType, 0, TRUE );
         break;
      }

   /* let's do the power up link establishment here if applicable:
   */
   for (i=0; i < MAX_DSL; i++) {
       if(isdn_idblist[i])
            Hcb[ i ].Established = TRUE;
   }

#ifdef COMP_TEST
   /* initialize the "special behavior" flags (to OFF).
   */
   Exclusive     = FALSE;              /* Exclusive B-Chan = 1 Test 25 */
   SuspendDiscon = FALSE;   /* Condition CC not to perform AT&T Test 8 */
   EnBlocSetup   = FALSE;  /* Condition CC not to perform AT&T Test 16 */
   QuickConn     = FALSE;  /* Condition CC not to perform AT&T Test 42 */
   QuickProceed  = FALSE;  /* Condition CC not to perform AT&T Test 45 */
   ReleaseActive = FALSE;  /* Condition CC not to perform AT&T Test 60 */
#endif

#ifdef TASK_KB
   /* finally, let's initialize the phone (hardware)
   */
   CCBRI_StopConnect( 1 );
   CCBRI_StopConnect( 2 );
   CCBRI_StopDigitDetect();
   CCBRI_StopRing( VOICE_ALERTING );
   CCBRI_StartTone( SILENT_TONE );
#endif

   return;
}

/*******************
* CCBRI_MailtoHost *
********************
*
* Mail a packet to a task as output of this module.
*
*/
PUBLIC void CCBRI_MailtoHost( copy_public_IE )

BOOLEAN copy_public_IE;
{
   extern isdn_debug;
#ifdef COMMAND_INTERFACE
   int IE_index;                  /* an index to the IE string in Info */
#endif

#ifdef COMMAND_INTERFACE
   /* Do we need to copy the CCITT compatible info elements from
   *  InPkt to OutPkgInfo ?
   */
   if ( copy_public_IE && InPkt )
      {
      /* we assume that OutPkgInfoLen is the total length of the info
      *  so far (i.e. including all private info elements).
      */
      /* find the start of info elements in the InPkt->Info
      */
      if (InPkt->Info[1] == 1)                /* call reference length */
         {
         if (InPkt->Info[3] == 0x00)           /* extended message set */
            IE_index = 5;
         else
            IE_index = 4;
         }
      else
         {
         if (InPkt->Info[4] == 0x00)           /* extended message set */
            IE_index = 6;
         else
            IE_index = 5;
         }

      if ( !( CCB->MakePrivateMsg ) )
         OutPkgInfoLen = 5;

      while (IE_index < InPkt->InfoLen)
         OutPkgInfo[ OutPkgInfoLen++] = InPkt->Info[IE_index++ ];
      }

   /* Now see if we need to re-size the info buffer.
   */
   if ( OutPkgInfo && ( OutPkgInfoLen <= SmlBufSize ))
      CCBRI_ResizeInfo( TRUE );     /* The buffer is excessively large */

   if ( LIF_SendBuf( CC_ID,
#ifdef X31_DEV
                     (CCB->CallType == VOICE_CALL) ? HOST_ID : P3_ID,
#else
                     HOST_ID,
#endif
                     OutPkgInfoLen,
                     OutPkgInfoType,
                     OutPkgInfo,
                     OutPkgInfo[0],
                     NOW,
                     REGULAR_MAIL ) == ERR )
      CCBRI_Error( CCBRI_MAIL_TO_HOST, SEND_BUF_FAILED, OutPkgInfoLen, 0, TRUE );

   OutPkgInfo    = NULL;
   OutPkgInfoLen = 0;
#else
   TheTrash = copy_public_IE;
#endif                                            /* COMMAND_INTERFACE */

   return;
}

/*****************
* CCBRI_MailtoL3 *
******************
*
* Mail a packet to a task as output of this module.
*
*/
PUBLIC void CCBRI_MailtoL3()

{
   /* First see if we need to re-size the info buffer.
   */
   if ( ( OutPkt) &&
      ( OutPkt->InfoLen + OutPkt->HeadLen ) <= SmlBufSize )
      CCBRI_ResizeInfo( FALSE );    /* The buffer is excessively large */
	
   if ( LIF_SendPkt( L2P_LAPD,
                     CC_ID,
                     CCB->State,
                     L3_ID,
                     NL_DATA_REQ,
                     Dsl_ID,
                     Q931_SAPI,
                     CCB->Ces,
                     0,                                         /* lli */
                     CCB->Call_ID,
                     Q931_DSL_CHAN_ID,
                     TRUE,
                     OutPkt,
                     NOW,
                     Mail_priority) == ERR )
      {
      CCBRI_Error( CCBRI_MAIL_TO_L3,                   /* Mail failed! */
                   SEND_PKT_FAILED,
                   LIF_MailSlotsAvail(CC_ID, L3_ID),
                   LIF_MailSlotsLimit(CC_ID, L3_ID),
                   TRUE );
      }
   OutPkt = NULL;
   Mail_priority = REGULAR_MAIL; /* set it back to Regular as default */

   return;
}

/**************
* CCBRI_Match *
***************
*
* From a list of byte values attempt to match a specific value with any one
* of its entries.  Return TRUE if a match is found.
*
*/
PUBLIC BOOLEAN CCBRI_Match( list, list_size, value )

const BYTE *list;                                     /* List to search */
int         list_size;                                /* Size of list */
BYTE        value;                                    /* Value to match */
{
   int i;
   BOOLEAN match = FALSE;

   for ( i = 0; i < list_size; i++ )
      {
      if ( list[ i ] == value )
         {
         match = TRUE;
         break;
         }
      }

   return( match );
}

/*******************
* CCBRI_RejectCall *
********************
*
* Reject a call due to flaws or incompatibilities in the associated message
* received from the peer.  The function CCBRI_RejectSetup is only used for
* rejecting the CC_SETUP_IND and this function is used otherwise.
#ifdef BRI_1TR6
*  1TR6 rejects SETUP with RELEASE if User is Busy.
#endif
*
*/
PUBLIC void   CCBRI_RejectCall( cause, diagnostic )

int cause;                                     /* Reason for rejection */
int diagnostic;                                /* Diagnostic for cause */

{
   BYTE diag[ 2 ];

	/*
	 *		This causes the Call Reference to appear in the Release
	 *	Complete message.  I haven't a clue how or why this works,
	 *	and I don't think I want to know.	===
	 */
	if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {		/* === */
		if(!CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))			/* === */
			return;													/* === */
		CCBRI_MailtoL3();			/* Send CC_BROADCAST_RESP */	/* === */
	}																/* === */

   /* We request L3 to send RELEASE_COMP directly.
	*
	*		Am I missing something real basic here, or are they requesting
	*	a RELEASE instead of RELEASE COMPLETE?  I only changed it for VN2,
	*	which may be correct, given the comment to CC_REL_COMP_REQ in lif.h.
	*				=== RBK
    */
   if ( CCBRI_GetOutPkt(
			(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) ?
			CC_REL_COMP_REQ : CC_RELEASE_REQ, FALSE ))	/* === */
      {
      /* Build the CC_RELEASE_REQ message.
      */
      diag[ 0 ] = diagnostic & 0x7F;
      diag[ 1 ] = 0;
      OutPkt->InfoLen +=
         CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ], cause,
#ifdef USER
                             FALSE,                /* not network side */
#else
                             TRUE,                     /* network side */
#endif
                             diag);
      CCBRI_MailtoL3();                            /* Send the message */
      }

   return;
}

/***********************
* CCBRI_RejectHostCall *
************************
*
* Build and send the CALL_REJECTION message to the HOST.
*
*/
PUBLIC void   CCBRI_RejectHostCall( cause )

int     cause;                                 /* Reason for rejection */

{
   if ( !cause )
      {
      cause = NORMAL_CLEARING;
      }

   if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
      {
      OutPkgInfo[ OutPkgInfoLen++ ] = 3; /* total length of private IE */
      OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;      /* Build CAUSE_IE */
      OutPkgInfo[ OutPkgInfoLen++ ] = 1;
      OutPkgInfo[ OutPkgInfoLen++ ] = cause;
      CCBRI_MailtoHost( FALSE );                            /* mail it */
      }

   return;
}

/**********************
* CCBRI_RejectHostMsg *
***********************
*
* Build and send the HOST_QUERY_RESPONSE to the host if a wrong message
* was received for the state.  The state is passed to the HOST in this
* message.
*
*/
PUBLIC void   CCBRI_RejectHostMsg( state )

   BYTE state;
{
   if ( CCBRI_GetOutInfo( HOST_QUERY_RESPONSE ) )
      {
      OutPkgInfo[ OutPkgInfoLen++ ] = 3;                     /* length */
      OutPkgInfo[ OutPkgInfoLen++ ] = CALL_STATE_IE;
      OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;
      OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) state;
      CCBRI_MailtoHost( FALSE );
      }

   return;
}

/******************
* CCBRI_RejectMsg *
*******************
*
* Reject a message because of its flawed nature.  The particular message
* may have caused Layer 3 to change states.  Since Layer 3 did not analyze
* the message in detail, it is unaware of the flaw and, thus, must be
* instructed to fix itself since the flawed message is to be ignored.
*
*/
PUBLIC void   CCBRI_RejectMsg( cause, diagnostic )

int cause;
int diagnostic;

{
   BYTE diag[ 2 ];


   if ( CCBRI_GetOutPkt( CC_CHNG_STATE_REQ, FALSE ))
      {
      OutPkt->Info[ OutPkt->InfoLen++ ] = L3_State;
      CCBRI_MailtoL3();                  /* Mail the change state msg. */
      }

   /* the further actions are switch dependent:
   */
   switch ( SwitchType )
      {
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
         if ( CCBRI_GetOutPkt( CC_STATUS_REQ, FALSE ))
            {
            diag[ 0 ] = diagnostic & 0x7F;
            diag[ 1 ] = 0;
            OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   cause,
#ifdef USER
                                   FALSE,          /* not network side */
#else
                                   TRUE,               /* network side */
#endif
                                   diag);
            OutPkt->InfoLen +=
               CCIE_BuildCallState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       L3_State );
            }
         break;
#endif                                                          /* VN2 */
      default:
         if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ))
            {
            diag[ 0 ] = diagnostic & 0x7F;
            diag[ 1 ] = 0;
            OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   cause,
#ifdef USER
                                   FALSE,          /* not network side */
#else
                                   TRUE,               /* network side */
#endif
                                   diag);
            }
         break;
      }

   if (OutPkt)
      CCBRI_MailtoL3();                     /* Mail the CC_RELEASE_REQ */

   return;
}

/********************
* CCBRI_RejectSetup *
*********************
*
* This function is called as the first response to the CC_SETUP_IND
* when the call is rejected.
*
*/
PUBLIC void CCBRI_RejectSetup( cause, diagnostic )

int cause;
int diagnostic;

{
   BYTE diag[ 2 ];

#ifdef USER
   if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
      {
      CCBRI_MailtoL3();                         /* Send BROADCAST_RESP */
      }
#endif
   if ( CCBRI_GetOutPkt( CC_SETUP_REJ_REQ, FALSE ))
      {
      /* Build the message.
      */
      diag[ 0 ] = diagnostic & 0x7F;
      diag[ 1 ] = 0;
      OutPkt->InfoLen +=
         CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ], cause,
#ifdef USER
                             FALSE,                /* not network side */
#else
                             TRUE,                     /* network side */
#endif
                             diag );
#ifdef VN2
      /* special info element for VN2 specification:
      */
      if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
         {
         OutPkt->InfoLen +=
            CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CODESET_6, FALSE );
         OutPkt->InfoLen +=
            CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                  CEPT_NATIONAL_STANDARD,
                                  NATIONAL_FUNCTION );
         }
#endif
      CCBRI_MailtoL3();                       /* Send CC_SETUP_REJ_REQ */
      }

   return;
}

/********************
* CCBRI_ReleaseCall *
*********************
*
* Dispatch for an appropriate switch specific call release function
*
*/
PUBLIC void CCBRI_ReleaseCall()

{
   switch ( SwitchType )
      {
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
#ifdef USER
         CCB5E_ReleaseCall();
#else
         CCB5EN_ReleaseCall();
#endif
         break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
         CCBDMS_ReleaseCall();
         break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
         CCB1TR6_ReleaseCall();
         break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
         CCBNTKD_ReleaseCall();
         break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
         CCBVN_ReleaseCall();
         break;
#endif
#ifdef BRI_NET3		/*===*/
      case BRI_NET3_STYPE:
         CCBNET3_ReleaseCall();
         break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
         CCBTS013_ReleaseCall();
         break;
#endif
#ifdef BRI_NI1	/*===*/
	  case BRI_NI1_STYPE:
		CCBNI1_ReleaseCall();
		break;
#endif
      default:
         CCBRI_Error( CCBRI_RELEASE_CALL, INVALID_SWITCH_TYPE, SwitchType, 0, TRUE );
         break;
      }

   return;
}

/********************
* CCBRI_ReleaseChan *
*********************
*
* Remove a B-Channel from allocation by a CCB.
*
*/
PUBLIC void   CCBRI_ReleaseChan()

{
   if ( CCB->B_Chan )
      {
      if ( CC_CHAN_GetUserbri( CCB->B_Chan,
                            B_CHANNEL_UNITS,
                            Dsl_ID ) == CCB->Call_ID )
         {
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
#endif
         CC_CHAN_ReleaseChanbri( CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID );
         }
      CCB->B_Chan = 0;
      }

   return;
}

/*********************
* CCBRI_ReleasedLink *
**********************
*
* The data link is down at Layer 2. Call a proper switch dependent
* function.
*
*/
PUBLIC void  CCBRI_ReleasedLink (ces, dsl_id)

    BYTE ces;
    BYTE dsl_id;

{
    switch (SwitchType) {
        case BRI_5ESS_STYPE:
            CCB5E_ReleasedLink(ces, dsl_id);
            break;
        case BRI_DMS100_STYPE:
            CCBDMS_ReleasedLink(ces, dsl_id);
            break;
        case BRI_NI1_STYPE:
            CCBNI1_ReleasedLink(ces, dsl_id);
            break;
        case BRI_1TR6_STYPE:
            CCB1TR6_ReleasedLink(ces, dsl_id);
            break;
        case KDD_STYPE:
        case NTT_STYPE:
            CCBNTKD_ReleasedLink(ces, dsl_id);
            break;
        case VN2_STYPE:
        case VN3_STYPE:
            CCBVN_ReleasedLink(ces, dsl_id);
            break;
        case BRI_NET3_STYPE:
            CCBNET3_ReleasedLink(ces, dsl_id);
            break;
        case BRI_TS013_STYPE:
            CCBTS013_ReleasedLink(ces, dsl_id);
            break;
        default:
            break;
    }
    return;
}

/******************
* CCBRI_RemoveCCB *
*******************
*
* Remove a CCB from the Hcb and the linked list of all CCB's
*
*/
PUBLIC void CCBRI_RemoveCCB (ccb)
   CCB_ID  ccb;
{
   CCB_ID  temp;
   extern isdn_debug;	/* === */

   SAVE_CALLER();

	/* This is sometimes called with no CCBs allocated.  === */
	if(CCB_Head[Dsl_ID] == NULL) {
            if(isdn_debug)
               buginf("\nBRI Error: CCBRI_RemoveCCB called from 0x%x "
                      "with no CCBs allocated.", caller());
            return;
        }

	/* They sometimes dereference CCB instead of ccb === */
	if(CCB != ccb && isdn_debug)
            buginf("\nBRI%d Warning: CCBRI_RemoveCCB(ccb = 0x%x), CCB = 0x%x",
                   Dsl_ID, ccb, CCB);
	CCB = ccb;

   /* Remove any channel association.
   */
   CCBRI_ReleaseChan();

   /* Find the ccb and remove it from linked list of CCBs  */
   /* Rewritten.  What was here was broken === */
   if(ccb == CCB_Head[Dsl_ID]) {
      if((CCB_Head[Dsl_ID] = ccb->Next) == NULL)
         CCB_Tail[Dsl_ID] = NULL;
   } else { 
      for(temp = CCB_Head[Dsl_ID]
         ; temp->Next != NULL && temp->Next != ccb
         ; temp = temp->Next)
         ;
      if(temp->Next == NULL) {
         /* it was not on the list */
         if(isdn_debug)
            buginf("\nBRI%d Error: CCBRI_RemoveCCB called from "
                   "0x%x nonexistent CCB 0x%x.", Dsl_ID, caller(), ccb);
         return;
      }
      if(ccb == CCB_Tail[Dsl_ID]) {
         CCB_Tail[Dsl_ID] = temp;
         temp->Next = NULL;
      } else
         temp->Next = ccb->Next;
   }

   switch (SwitchType) {
      case BRI_5ESS_STYPE:
         LIF_RemoveTimer (CCB->CC5essTimer);
         break;
      case NTT_STYPE:
         LIF_RemoveTimer (CCB->NtkdTimer);
         break;
      case BRI_1TR6_STYPE:
         LIF_RemoveTimer (CCB->CC1TR6Timer);
         break;
      case BRI_DMS100_STYPE:
         LIF_RemoveTimer (CCB->CCDMSTimer);
         break;
      case VN2_STYPE:
      case VN3_STYPE:
         LIF_RemoveTimer (CCB->CCVNTimer);
         break;
      default:
         break;
      }
   free(ccb);
   ccb = NULL;

   ActiveCCBs[ Dsl_ID ]--;                    /* Decrement CCBs in use */
   AllocatedCCBs--;

   if ( ActiveCCBs[ Dsl_ID ] == 0 )      /* Perform a simple CCB audit */
      {
      if (( CCB_Head[Dsl_ID] ) || ( CCB_Tail[Dsl_ID] ))/* CCB stranded? */
         CCBRI_Error( CCBRI_REMOVE_CCB, CCB_LIST_CORRUPTED, 0, 0, TRUE );
      }
}

/*******************
* CCBRI_ResizeInfo *
********************
*
* Downsize the OutPkt to SML_INFO_BTYPE.
* Copy information into a smaller block to efficiently use memory.
*
*/
PUBLIC void   CCBRI_ResizeInfo( is_pkginfo )

BOOLEAN is_pkginfo;

{
   PKT_ID  old_pkt;                          /* Packet being discarded */
   BYTE   *old_info;                           /* Info being discarded */


   if ( is_pkginfo )                                   /* HOST message */
      {
      old_info = OutPkgInfo;
      if (( OutPkgInfo = LIF_GetBuffer( SML_INFO_BTYPE, FALSE )) != NULL )
         {
         LIF_N_Cpy( OutPkgInfo, old_info, OutPkgInfoLen );
         LIF_FreeBuffer( LRG_INFO_BTYPE, old_info );
         OutPkgInfoType = SML_INFO_BTYPE;
         }
      else
         {
         CCBRI_Error( CCBRI_RESIZE_INFO, OUT_OF_INFO_BUFFERS, 0, 0, TRUE );
         OutPkgInfo = old_info;
         }
      }
   else                                                  /* L3 message */
      {
      old_pkt = OutPkt;                                /* Save old pkt */
      if (( OutPkt = LIF_GetPkt(SML_INFO_BTYPE, old_pkt->HeadLen, TRUE )) != NULL)
         {
         LIF_CpyPkt( OutPkt, old_pkt );
         OutPkt->InfoType = SML_INFO_BTYPE;
         LIF_FreePkt( old_pkt );
         }
      else
         {
         CCBRI_Error( CCBRI_RESIZE_INFO, OUT_OF_PKTS, 0, 0, TRUE );
         OutPkt = old_pkt;      /* Continue using the oversized packet */
         }
      }

   return;
}

/****************
* CCBRI_Restart *
*****************
*
*/
PUBLIC void CCBRI_Restart( event, dsl_id )

int  event;
BYTE dsl_id;

{
   switch ( SwitchType )
      {
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
#ifdef USER
         CCB5E_Restart( event, dsl_id );
#else
         CCB5EN_Restart( event, dsl_id );
#endif
         break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
         CCBDMS_Restart( dsl_id );
         TheTrash = event;                   /* avoid compiler warning */
         break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
         CCBNTKD_Restart( dsl_id );
         TheTrash = event;                   /* avoid compiler warning */
         break;
#endif
#ifdef BRI_NI1	/*===*/
      case BRI_NI1_STYPE:
      	CCBNI1_Restart ( dsl_id );
      	TheTrash = event;                  /* avoid compiler warning */
      	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
         TheTrash = event;                   /* avoid compiler warning */
         TheTrash = dsl_id;                  /* avoid compiler warning */
         break;
#endif
#ifdef BRI_NET3		/*===*/
      case BRI_NET3_STYPE:
      	TheTrash = event;                  /* avoid compiler warning */
        TheTrash = dsl_id;                  /* avoid compiler warning */
      	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      	TheTrash = event;                  /* avoid compiler warning */
        TheTrash = dsl_id;                  /* avoid compiler warning */
      	break;
#endif
      default:
      	TheTrash = event;                   /* avoid compiler warning */
      	TheTrash = dsl_id;                  /* avoid compiler warning */
      	break;
      }

   return;
}

#ifdef SEND_TR_MSG
/******************
* CCBRI_SendTrace *
*******************
*
* Send a state update message to Tracker.
*
*/
PUBLIC void   CCBRI_SendTrace( state )

int state;                                               /* "TR" state */
{
   if ( LIF_SendSbc( CC_ID, TRACER_ID, state, REGULAR_MAIL) == ERR )
      CCBRI_Error( CCBRI_SEND_TRACE, SEND_SBC_FAILED, 0, 0, TRUE );

   return;
}

#endif
/*******************
* CCBRI_SetNewChan *
********************
*
* Establish a new B-Channel in the CCB and the Channel Management Table.
*
*/
PUBLIC int CCBRI_SetNewChan( b_chan, new_state, overwrite )

int     b_chan;                                    /* B-Channel to set */
int     new_state;    /* State (e.g. CAHNNEL_BUSY or CHANNEL_PROPOSED) */
BOOLEAN overwrite;   /* Change table for chan found in PROPOSED state? */

{
   int cause;
   int old_state;
   int old_chan;

   old_chan = CCB->B_Chan; 

   /* Release any other B-Channel assignment.
   *  CCB->B_Chan is set to 0 through this call.
   */
   if ( CCB->B_Chan && ( CCB->B_Chan != b_chan ))
      CCBRI_ReleaseChan();


   switch ( b_chan )
      {
      case B1_CHANNEL:
      case B2_CHANNEL:
         /* Here we implicitly "trust" the network not to assign a
         *  busy channel.  If we think we are 1B+D and we are offered
         *  the other B, VALID_CHANNEL will catch it.
         */
         if VALID_CHANNELB( b_chan )
            {
            CCB->B_Chan = b_chan;
            }
         cause = ( CCB->B_Chan ) ? NOERR : NO_CIRCUIT_AVAILABLE; /* 34 */
         break;

      case ANY_CHANNEL:
         /* Select any available channel.
         */
         CCB->B_Chan = CC_CHAN_GetIdleChanbri( B1_CHANNEL, B2_CHANNEL,
                                            B_CHANNEL_UNITS, Dsl_ID );
         cause = ( CCB->B_Chan ) ? NOERR : NO_CIRCUIT_AVAILABLE; /* 34 */
         break;

      case NO_CHANNEL:
         /* This situation may arise in the case of additional call
         *  offering where all B-channels have been assigned to other
         *  existing calls.  This function is not explicitly implemented,
         *  but the case is handled for completeness.
         */
          switch ( SwitchType )
              {
#ifdef BRI_1TR6
              /* This situation is determined by subscription to 
              *   "Anklopfen", a Dienstmerkmal described in 3.3.1.2.3
              *  We can accept a call with "no channel" if we subscribe to
              *  Anklopfen 
              */  		/*===*/
              case BRI_1TR6_STYPE:
                 cause = CALL_REJECTED_1TR6;
                 break;
#endif 
              default:
                 cause = NOERR;  
                 break;
              }
          break;
      default:                                   /* Should never occur */
         cause = NO_CIRCUIT_AVAILABLE;                     /* Cause 34 */
         break;
      }

   if ( CCB->B_Chan )
      {
      /* Get pre-existing state of new channel.
      */
      old_state = CC_CHAN_GetStatebri( CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID );
#ifdef BRI_1TR6
      if ( SwitchType == BRI_1TR6_STYPE )
         {  
         overwrite = ( overwrite &&
                     (  ( old_state == CHANNEL_PROPOSED ) || (old_state == CHANNEL_PROPOSED_1TR6 ) ));
         }
#else
      overwrite = ( overwrite && ( old_state == CHANNEL_PROPOSED ));
#endif

      if (( old_state == CHANNEL_IDLE ) || ( overwrite )|| (old_chan == CCB->B_Chan ))
         {
         /* Allocate the new channel in the table.
         */
         CC_CHAN_SetProtocolbri( CCB->B_Chan, B_CHANNEL_UNITS, L2P_LAPD, Dsl_ID );
         CC_CHAN_SetStatebri( CCB->B_Chan, B_CHANNEL_UNITS, new_state, Dsl_ID );
         CC_CHAN_SetUserbri( CCB->B_Chan, B_CHANNEL_UNITS, CCB->Call_ID, Dsl_ID );
         }
      else
         {
         /* This is either the case of the HOST or the ISDN peer
         *  attempting to re-assign a busy channel.
         */
         CCBRI_Error( CCBRI_SET_NEW_CHAN, OUT_OF_CHANNELS,
                      CCB->B_Chan, old_state, FALSE );
         CCB->B_Chan = 0;
#ifdef BRI_1TR6
         if ( SwitchType == BRI_1TR6_STYPE )
            {
            cause = USER_BUSY_1TR6;                        /* Cause 51 */
            }
		else	/* ===??? */
/* #else	===??? */
#endif	/* === ??? */
         cause = NO_CIRCUIT_AVAILABLE;                     /* Cause 34 */
/* #endif	===??? */
         }
      }

   return( cause );
}

/*****************
* CCBRI_ShutDown *
******************
*
* Shut down the Call Control.
*
*/
PUBLIC void   CCBRI_ShutDown (dsl)
BYTE dsl;
{
   switch ( SwitchType )
      {
      case VN2_STYPE:
      case VN3_STYPE:
         CCBVN_Shutdown(dsl);
         break;

      case BRI_NET3_STYPE:
         CCBNET3_Shutdown(dsl);
         break;

      case BRI_TS013_STYPE:
         CCBTS013_Shutdown(dsl);
         break;

      case BRI_DMS100_STYPE:
         CCBDMS_Shutdown(dsl);
         break;

      case BRI_1TR6_STYPE:
         CCB1TR6_Shutdown(dsl);
         break;

      case KDD_STYPE:
      case NTT_STYPE:
         CCBNTKD_Shutdown(dsl);
         break;

      case BRI_NI1_STYPE:
      	CCBNI1_Shutdown(dsl);
      	break;

      case BRI_5ESS_STYPE:
      	CCB5E_Shutdown(dsl);
      	break;

      default:
         /* Action is implementation dependent.
         */
         break;
      }

   return;
}

#ifdef COMP_TEST
/************************
* CCBRI_SpecialFunction *
*************************
*
* Invoke or terminate special functions for AT&T compatibility testing by
* toggling a specific "behavior" flag under control of a keystroke.
*
*/
PUBLIC BOOLEAN   CCBRI_SpecialFunction( key )

int key;                                                /* Key pressed */

{
   BOOLEAN special_key = TRUE;


   switch ( key )
      {
      case DIGIT_COMP_TEST_0:
         SuspendDiscon = SuspendDiscon ? FALSE : TRUE;       /* Test 8 */
#ifdef DEBUG
         if ( SuspendDiscon )
            printf( "Suspend disconnect enabled.\n" );
         else
            printf( "Suspend disconnect disabled.\n" );
#endif                                                        /* DEBUG */
         break;

      case DIGIT_COMP_TEST_1:
         EnBlocSetup = EnBlocSetup ? FALSE : TRUE;          /* Test 16 */
#ifdef DEBUG
         if ( EnBlocSetup )
            printf( "En Bloc Setup enabled.\n" );
         else
            printf( "En Bloc Setup disabled.\n" );
#endif                                                        /* DEBUG */
         break;

      case DIGIT_COMP_TEST_2:
         Exclusive = Exclusive ? FALSE : TRUE;              /* Test 25 */
#ifdef DEBUG
         if ( Exclusive )
            printf( "Exclusive acceptance of B-Chan = 1 enabled.\n" );
         else
            printf( "Exclusive acceptance of B-Chan = 1 disabled.\n" );
#endif                                                        /* DEBUG */
         break;

      case DIGIT_COMP_TEST_3:
         QuickConn = QuickConn ? FALSE : TRUE;              /* Test 42 */
#ifdef DEBUG
         if ( QuickConn )
            printf( "Quick Connect enabled.\n" );
         else
            printf( "Quick Connect disabled.\n" );
#endif                                                        /* DEBUG */
         break;

      case DIGIT_COMP_TEST_4:
         QuickProceed = QuickProceed ? FALSE : TRUE;        /* Test 45 */
#ifdef DEBUG
         if ( QuickProceed )
            printf( "Direct CALL PROCEEDING enabled.\n" );
         else
            printf( "Direct CALL PROCEEDING disabled.\n" );
#endif                                                        /* DEBUG */
         break;

      case DIGIT_COMP_TEST_5:
      case DIGIT_COMP_TEST_6:
         /* These pseudo-key is fielded in the active call state.
         */
         special_key = FALSE;
         break;

      case DIGIT_COMP_TEST_7:
         ReleaseActive = ReleaseActive ? FALSE : TRUE;      /* Test 60 */
#ifdef DEBUG
         if ( ReleaseActive )
            printf( "RELEASE substituted for DISCONNECT.\n" );
         else
            printf( "RELEASE not substituted for DISCONNECT.\n" );
#endif                                                        /* DEBUG */
         break;

      default:
         special_key = FALSE;
         break;
      }

   return ( special_key );
}

#endif                                                    /* COMP_TEST */
/************************************************************************
*          H A R D W A R E   C O N T R O L   F U N C T I O N S          *
*************************************************************************
*/
#ifdef VN2
#endif
#ifdef TASK_KB
/********************
* CCBRI_StartConnect *
*********************
*
* Connect a PCM or data source to a B-Channel.
*
*/
PUBLIC void   CCBRI_StartConnect( b_chan )

int   b_chan;        /* B-Channel to which "source" is to be connected */

{
   /* The exact actions required here are highly specific to
   *  the application.
   */
   switch (b_chan)
      {
      case 1:
         LIF_SendSbc ( CC_ID, KB_ID, KP_CONNECT_B1, REGULAR_MAIL);
         break;
      case 2:
         LIF_SendSbc ( CC_ID, KB_ID, KP_CONNECT_B2, REGULAR_MAIL);
         break;
      default:
         break;
      }

   return;
}

#endif
#ifdef TASK_KB
/************************
* CCBRI_StartDigitDetect *
*************************
*
* Start the detection and reporting of dialed digits from the hardware.
*
* NOTE: this function is not used in this implementation of Call Control.
*       The DTMF receiver is turned on by the KB module whenever an off
*       hook condition is detected.
*/
PUBLIC void   CCBRI_StartDigitDetect()

{
   /* The exact actions required here are highly specific to
   *  the application.
   */
   return;
}

#endif
#ifdef TASK_KB
/******************
* CCBRI_StartRing *
*******************
*
* Cause a source to begin power (bell) ringing.
*
*/
PUBLIC void   CCBRI_StartRing( ring_type )

BYTE ring_type;                   /* Type of distinctive ring to apply */

{
   /* The exact actions required here are highly specific to
   *  the application.
   */
   switch (ring_type)
      {
      case VOICE_ALERTING:
         LIF_SendSbc ( CC_ID, KB_ID, KP_START_RING, REGULAR_MAIL);
         break;
      case CW_ALERTING:
         LIF_SendSbc ( CC_ID, KB_ID, KP_START_CW, REGULAR_MAIL);
         break;
      default:
         break;
      }

   return;
}

#endif
#ifdef TASK_KB
/******************
* CCBRI_StartTone *
*******************
*
* Cause a source to be connected to a specific tone.
*
*/
PUBLIC void   CCBRI_StartTone( tone_type )

BYTE tone_type;               /* Tone to apply (including SILENT_TONE) */

{
   /* The exact actions required here are highly specific to
   *  the application.
   */
   switch (tone_type)
      {
      case SILENT_TONE:
         LIF_SendSbc ( CC_ID, KB_ID, KP_STOP_TONE, REGULAR_MAIL);
         break;
      case DIAL_TONE:
         LIF_SendSbc ( CC_ID, KB_ID, KP_START_DIALTONE, REGULAR_MAIL);
         break;
      case SPECIAL_DIAL_TONE:
         LIF_SendSbc ( CC_ID, KB_ID, KP_START_SPECIALDT, REGULAR_MAIL);
         break;
      case RINGBACK_TONE:
         LIF_SendSbc ( CC_ID, KB_ID, KP_START_RINGBACK, REGULAR_MAIL);
         break;
      case BUSY_TONE:
         LIF_SendSbc ( CC_ID, KB_ID, KP_START_BUSYTONE, REGULAR_MAIL);
         break;
      default:
         break;
      }

   return;
}

#endif
/******************
* CCBRI_StatusInd *
*******************
*
* CC_STATUS_IND received from L3.  Handle this message.
*
*/
PUBLIC void CCBRI_StatusInd()

{
   if ( L3_State == 0 )
      {
      /* The network has reported state 0.  The call is over.
      */
#ifdef COMMAND_INTERFACE
      if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ))
         {
         /* Total length of IEs = 0.
         */
         OutPkgInfo[ OutPkgInfoLen++ ] = 0;
         CCBRI_MailtoHost( CCB->CopyPublicMsg );
         }
#else                                                 /* SBC_INTERFACE */
      Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
#ifdef TASK_KB
      CCBRI_StopConnect( CCB->B_Chan );
#endif
#endif
      CCBRI_ReleaseCall();
      }
   /* Other actions to be determined.
   */
   return;
}

#ifdef TASK_KB
/********************
* CCBRI_StopConnect *
*********************
*
* Remove the connection of PCM from a B-Channel.
*
* NOTE: this implementation of Call Control supports voice on only one
*       B channel at a time.  Thus, disconnect channel will always
*       clear the connection from both B channels.
*
*/
PUBLIC void   CCBRI_StopConnect( b_chan )

int   b_chan;    /* B-Channel to which "source" is initially connected */

{
   /* The exact actions required here are highly specific to
   *  the application.
   */
   if ((b_chan == B1_CHANNEL) || (b_chan == B2_CHANNEL))
      LIF_SendSbc ( CC_ID, KB_ID, KP_DISCONNECT_CHAN, REGULAR_MAIL);

   return;
}

#endif
#ifdef TASK_KB
/***********************
* CCBRI_StopDigitDetect *
************************
*
* Stop the detection and reporting of dialed digits from the hardware.
*
*/
PUBLIC void   CCBRI_StopDigitDetect()

{
   /* The exact actions required here are highly specific to
   *  the application.
   */
   LIF_SendSbc ( CC_ID, KB_ID, KP_DTMF_DETECT_OFF, REGULAR_MAIL);

   return;
}

#endif
#ifdef TASK_KB
/*****************
* CCBRI_StopRing *
******************
*
* Cause a source to stop power (bell) ringing.
*
*/
PUBLIC void CCBRI_StopRing( ring_type )

BYTE ring_type;                          /* Type of ring to be removed */

{
   /* The exact actions required here are highly specific to
   *  the application.
   */
   if (ring_type == VOICE_ALERTING)
      LIF_SendSbc ( CC_ID, KB_ID, KP_STOP_RING, REGULAR_MAIL);

   return;
}

#endif                                                      /* TASK_KB */
/************************
* CCBRI_ValidateHwEvent *
*************************
*
* Test if a keystroke input for a voice phone is consistent with
* the hardware state of the call. Return TRUE if the state/event
* combination is valid, FALSE if it is not.
*
* NOTE: only voice phone hardware events are validated.  Also note that
*       only the original hardware events coming from KB_ID should
*       be validated.  Internal hardware events (generated by the
*       software) do not have to be consistent with this validation
*       procedure and should not be validated here !!
*
* Allowed events in various hardware states are listed below:
*
*  ONHOOK_IDLE        BELL (ringing)       OFFHOOK
*  -----------------------------------------------------
*   off hook            off hook            on hook
*                                           hook flash
*                                           digits
*
*/
PUBLIC BOOLEAN   CCBRI_ValidateHwEvent( event, state )

int   event;
BYTE  state;

{
   BOOLEAN valid;


   switch ( SwitchType )
      {
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
         switch ( state )
            {
            case OFFHOOK:
               valid = ( event != KP_OFF_HOOK );
               break;
            case ONHOOK_IDLE:
            case BELL:
               valid = FALSE;                            /* Initialize */
               valid = (( valid ) ||
                        ( event == KP_OFF_HOOK ) ||
                        ( event == DIAL_KEY ));
               break;
            case DIAL_ONHOOK:
               valid = ( event != KP_ON_HOOK );
               break;
            default:
               valid = FALSE;          /* Invalid state - do not allow */
               break;
            }
         break;
#endif                                                          /* VN2 */
      default:
         switch ( state )
            {
            case OFFHOOK:
               if (event == KP_OFF_HOOK)
                  valid = FALSE;              /* do not allow OFF HOOK */
               else
                  valid = TRUE;               /* allow any other event */
               break;

            case ONHOOK_IDLE:
            case BELL:
               if (event == KP_OFF_HOOK)
                  valid = TRUE;                 /* allow only OFF HOOK */
#ifdef COMP_TEST
               else if ((event == DIGIT_COMP_TEST_5) ||
                        (event == DIGIT_COMP_TEST_6))
                  /* Slip in an exception under compatibility testing to
                  *  allow these special keys to be processed in certain
                  *  ON_HOOK states.
                  */
                  valid = TRUE;
#endif
               else
                  valid = FALSE;      /* do not allow any other events */
               break;
            default:
               valid = FALSE;          /* invalid state - do not allow */
               break;
            }
         break;
      }
   return ( valid );
}

#ifdef NOBODY_CALLS_ME
/* return the cause for the field being bad, 0 if it's good === */
int
CCBRI_AcceptLowLayerCompat (void)
{
	int i;

	if(SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
		return 0;

	IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
			LOW_LAYER_COMPAT_IE, CODESET_0, 1);
	if(IE == NULL)
		return 0;		/* not MANDATORY_IE_MISSING */
	if(IE[1] < 2 || IE[1] > 15)
		return MANDATORY_IE_LEN_ERR;

	/* test the Norme de codage */
	if(IE[2] & 0x60)
		return INVALID_ELEM_CONTENTS;
	
	/* test the Possibilite de transfert d'information */
	if((IE[2] & 0x1f) != 0x00)		/* why can we not accept 8, 16, and 17? */
		return INVALID_ELEM_CONTENTS;
	
	/* test the Mode de transfert */
	if(IE[3] & 0x60)
		return INVALID_ELEM_CONTENTS;

	/* test the Debit du transfert d'information */
	if((IE[3] & 0x1e) != 0x10)		/* note that bit 0 is not tested */
		return INVALID_ELEM_CONTENTS;
	
	i = 3;
	/* look at octet 4a? */
	if(!(IE[i++] & 0x80)) {
		/* test the Structure */
		if(IE[i] & 0x70)
			return INVALID_ELEM_CONTENTS;
		
		/* test the Configuration */
		if(IE[i] & 0x0c)
			return INVALID_ELEM_CONTENTS;
		
		/* test the Etablissement */
		if(IE[i] & 0x03)
			return INVALID_ELEM_CONTENTS;

		/* look at octet 4b? */
		if(!(IE[i++] & 0x80)) {
			/* check the Symetrie */
			if(IE[i] & 0x60)
				return INVALID_ELEM_CONTENTS;

			/* check the Debit du transfert d'information */
			if((IE[i] & 0x1f) != 0x10)
				return INVALID_ELEM_CONTENTS;
			
			i++;
		}
	}

	/* now examine octet 5 */
	if((IE[i] & 0x60) != 0x20)
		return INVALID_ELEM_CONTENTS;
	
	/* look at the Layer 1 Protocol information */
	switch(IE[i] & 0x1f) {
		case 0x03:
		case 0x07:
		case 0x08:
		case 0x09:
			break;
		
		default:
			return INVALID_ELEM_CONTENTS;
	}
	
	return 0;
}
#endif

#ifdef NOBODY_CALLS_ME
/* return the cause for the field being bad, 0 if it's good === */
int
CCBRI_AcceptUserUserInfo (void)
{
	if(SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
		return 0;

	IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
			USER_USER_IE, CODESET_0, 1);
	if(IE == NULL)
		return 0;		/* not MANDATORY_IE_MISSING */
	
	/*
	 *		There is a temporary limitation of 67 octets of length.
	 *	This is best observed only on the transmit side.
	 */
	if(IE[1] < 2 || IE[1] > 131)
		return MANDATORY_IE_LEN_ERR;
	
	/* check protocol discriminator */
	if(IE[2] != IA5_CHAR_PROTO && IE[2] != NT2_NT2_FAC_PROTO)
		return INVALID_ELEM_CONTENTS;
	
	return 0;
}
#endif

/*
 * Given a CES, return a TEI value associated with the CES.
 */ 
int ces_to_tei (int dsl, int ces)
{
    int ix;
    int tei;

    tei = 0;
    for (ix = 0; ix < MAX_LOGICAL_LINKS; ix++) {
        if (TeiCesTable[dsl][ix].Ces == ces) {
            tei = TeiCesTable[dsl][ix].Tei;
            break;
        }
    }
    return(tei);
}
 
/*
 * Display the information held in the Host Control Block. Includes spid and
 * EID information. This is only useful for US BRI switches, custom 5ess, custom
 * dms100 and ni1. Shown with the show isdn status, but only if a spid is
 * configured. 
 */
PUBLIC void show_host_cb (int dsl)
{
    int hcb;    /* host control block */
    int dx;
    int ces;
    int tei;
    tinybool first_time;

    hcb = 0;
    dx = 0;
    ces = 0;
    tei = 0;
    first_time = TRUE;
    if (SwitchType == BRI_5ESS_STYPE ||
        SwitchType == BRI_DMS100_STYPE || SwitchType == BRI_NI1_STYPE) {
        /*
         * The Host control block array starts at zero, but the CES value starts
         * at one.
         */ 
        for (hcb = 0; hcb < MAX_HOSTS; hcb++) {
            if (np_fit[dsl][hcb].spid[0] != '\0') {
                ces = hcb + 1;
                tei = ces_to_tei(dsl, ces);

                /*
                 * Can have a case where the TEI has not been assigned and the
                 * spid has not been sent out. Indicate that it is unassigned.
                 */
                if (first_time) {
                    printf("\n    Spid Status:");
                    first_time = FALSE;
                }
                if (tei == 0)
                    printf("\n\tTEI Not Assigned, ces = %d, ", ces);
                else
                    printf("\n\tTEI %d, ces = %d, ", tei, ces);

                printf("state = %x", Host_CB[dsl][hcb].Terminal_State);
                switch (Host_CB[dsl][hcb].Terminal_State) {
                    case 0x01:
                        printf("(terminal down)");
                        break;
                    case 0x03:
                        printf("(await establishment)");
                        break;
                    case 0x04:
                        printf("(await init)");
                        break;
                    case 0x05:
                        printf("(init)");
                        break;
                    case 0x06:
                        printf("(not initialized)");
                        break;
                    case 0x07:
                        printf("(init not supported)");
                        break;
                    case 0x08:
                        printf("(established)");
                        break;
                    case 0x99:
                        printf("(term initialized)");
                        break;
                    default:
                        printf("(unknown state)");
                        break;
                }

                printf("\n\t    spid%d configured, ", ces);
                if (np_fit[dsl][hcb].ldn[0] == '\0')
                    printf("no LDN, "); 

                printf("spid%d ", ces);
                if (!np_fit[dsl][hcb].spid_sent)
                    printf("NOT ");
                printf("sent, ");

                printf("spid%d ", ces);
                if (!Host_CB[dsl][hcb].epid.valid)
                    printf("NOT ");
                printf("valid");

                if (Host_CB[dsl][hcb].epid.valid) {
                    printf("\n\t    Endpoint ID Info: epsf = %x, usid = %x, tid = %x",
                           Host_CB[dsl][hcb].epid.epsf, Host_CB[dsl][hcb].epid.usid,
                           Host_CB[dsl][hcb].epid.tid);
                }
           }
        }
    }
}

/* End of module
*/
