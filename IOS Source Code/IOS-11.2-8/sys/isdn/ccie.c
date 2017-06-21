/* $Id: ccie.c,v 3.4.18.3 1996/09/05 00:03:24 snyder Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccie.c,v $
 *------------------------------------------------------------------
 * ccie.c  Utility Routines in Support of Q.931 Information Element Processing
 *         for Call Control
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccie.c,v $
 * Revision 3.4.18.3  1996/09/05  00:03:24  snyder
 * CSCdi68122:  declare a bunch of things const in isdn
 *              660 out of data, 40 out of image,
 *              ain't this fun.
 * Branch: California_branch
 *
 * Revision 3.4.18.2  1996/04/30  23:07:02  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/04/11  21:43:52  dclare
 * CSCdi52751:  Make DMS-100 ISDN PRI changes to conform to the
 * Specification
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/08  18:40:46  rbeach
 * CSCdi48239:  ISDN PRI dms switchtype doesnt correctly handle SL1
 * mode. Modifications were made to the primary-dms100 channel id
 * format, called party format and support for 1 or 2 octet call
 * reference values.
 *
 * Revision 3.3  1995/11/28  22:51:48  dclare
 * CSCdi44526:  ISDN PRI NET5 changes for Italian homologation. Changes to
 * handle corner test cases. Restart handling changed to handle interfaces,
 * a channel, and multiple channels.
 *
 * Revision 3.2  1995/11/17  17:39:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/19  16:52:28  dclare
 * CSCdi40646:  ISDN BRI changes for Italy NET3 homologation.
 *
 * Revision 2.2  1995/06/23  15:14:10  dclare
 * CSCdi36325:  Australian ISDN PRI implementation for the 7000. Also get
 * rid of some homologation defines that became too awkward to use. These
 * are the final changes found during homologation.
 *
 * Revision 2.1  1995/06/07  21:14:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: ccie.c
VERSION: 3.21
DESCRIPTION:  Utility Routines in Support of Q.931 Information
              Element Processing for Call Control

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

The functions contained herein are intended to be generic library
utilities available for use by Call Control applications as an aid
in management of CCITT Rec. Q.931 message information elements.

CCITT Rec. Q.931 Section 4.0 (General Message Format and Information
Element Coding) should be referenced for the definition of these
elements.

Element Search -

The largest and most complex functions implemented in this module are
the search and recovery of a specific information element imbedded
within a Q.931 message and an unspecified message type where the
accessing routine has specified the element to be sought by element I.D.,
codeset, and occurrance.  (A discussion of Q.931 codesets will be found
Q.931 Section 4.5.2).

One routine (CCIE_Get_IE()) is designed specifically to accept the
"Info" pointer and "InfoLen" for a Q.931 message as its starting
point.

The other routine (CCIE_GetAnyIE()) is designed to accept a pointer
and search length which may refer directly to any block of information
elements which have a general format consistent with the Q.931 elements,
but which need not agree with the message header format assumed by
CCIE_Get_IE().

The search routines allow for specification of the occurrence of a
specific element ID within a given codeset to be located.

The possibility of locking and non-locking codeset shifts is fully
supported.

The possibility of there being single-byte versus multi-byte elements
is also assumed and specific recognition of Type 1 versus Type 2
single byte elements is made (see Q.931 Section 4.5.1).  In the case
of Type 1 elements, the 4 least-significant bits of the provided
search ID are masked-off.

While the CCIE_Get_IE() information element search routine is complete
and general in its handling of the message header (protocol discriminator,
call reference length, call reference and message type) it is not
implemented as access routines to these components of the message,
although adding such functions would not be a difficult undertaking.

If the element to be located is found to be out-of-sequence in
accordance with CCITT Rec. Q.931 Sec. 5.8.5.1, the element search
will fail.

Other Functions -

The Functions CCIE_BuildXXXX_IE are included so that the application
doesn't need to be concerned with the packing or packing scheme for
the info element it wants to build.

The functions CCIE_ExpandXXXX_IE are included to unpack all of the
sub-fields within an info element and return a pointer to the buffer
where the expansion took place.  The calling routine must be "smart"
about the offset in the buffer where a particular field was placed.

Some relatively trivial sub-byte manipulation routines are included
as well for packing and unpacking sub-byte fields within any
target byte, the intent being that this sort of activity is
certain to exist in the construction an reduction of Q.931
information elements.  These functions are also implemented as
macros in gendef.h and it is up to the user of these functions in
which form his application will reference them.

*************************************************************************
*************************************************************************
*/

#define     CCIE_C		/* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

#include    "flags.h"
#include    "gendef.h"
#include    "lif.h"
#include    "ccie.h"
#include    "cc_chan.h"
/*
 * """ #include    <stdio.h> """
 */

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/*
 * A shared structure for keeping track of the current context while stepping
 * through the information elements within a Q.931 message within the
 * information element search functions of this module.
 */
typedef struct {
    BYTE Base_Codeset;		/* Background codeset in info element search */
    BYTE Active_Codeset;	/* Currently active codeset in the ie search */
    int Current_Offset;		/* Current offset into the msg (zero-based) */
    BYTE Previous_IE;		/* Previous variable length IE located */
    BOOLEAN Abort_Search;	/* IE search has failed */
    BYTE This_Occurrence;	/* Occurrence of the ie being sought */
}   SEARCH_CONTROL;


/*
 * To characterize the header of an individual information element.
 */
struct IE_Header {
    BYTE ident;			/* Element identifier */
    BYTE len;			/* Element size (in bytes) excluding the
				 * header */
};

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

/*
 * The following table or tables are CAUSE value bit arrays designed for
 * checking if a given received CAUSE value is defined for a SwitchType. They
 * are accessed through the function(s) CCIE_CauseValid.
 */

#ifdef BRI_5ESS
static const BYTE BRI_5ESS_Causes[] =
{
    0x06,			/* Cause(s) 1, 2 */
    0x00,			/* No causes */
    0x67,			/* Cause(s) 16, 17, 18, 21, 22 */
    0xF0,			/* Cause(s) 28, 29, 30, 31 */
    0x04,			/* Cause(s) 34 */

    0x0E,			/* Cause(s) 41, 42, 43 */
    0x54,			/* Cause(s) 50, 52, 54 */
    0x04,			/* Cause(s) 58 */
    0x26,			/* Cause(s) 65, 66, 69 */
    0x00,			/* No causes */

    0x26,			/* Cause(s) 81, 82, 85 */
    0x09,			/* Cause(s) 88, 91 */
    0x17,			/* Cause(s) 96, 97, 98, 100 */
    0x80,			/* Cause(s) 111 */
    0x00,			/* No causes */

    0x80			/* Cause(s) 127 */
};
#endif
#ifdef PRI_5ESS
static const BYTE PRI_5ESS_Causes[] =
{
    0x0E,			/* Cause(s) 1, 2, 3 */
    0x00,			/* No causes */
    0x6F,			/* Cause(s) 16, 17, 18, 19, 21, 22 */
    0xD8,			/* Cause(s) 27, 28, 30, 31 */
    0x04,			/* Cause(s) 34 */

    0x9E,			/* Cause(s) 41, 42, 43, 44, 47 */
    0x54,			/* Cause(s) 50, 52, 54 */
    0x06,			/* Cause(s) 57, 58 */
    0x26,			/* Cause(s) 65, 66, 69 */
    0x00,			/* No causes */

    0x06,			/* Cause(s) 81, 82 */
    0x01,			/* Cause(s) 88 */
    0x77,			/* Cause(s) 96, 97, 98, 100, 101, 102 */
    0x00,			/* No causes */
    0x00,			/* No causes */

    0x80			/* Cause(s) 127 */
};
#endif
#ifdef PRI_4ESS
static const BYTE PRI_4ESS_Causes[] =
{
    0x42,			/* Cause(s) 1, 6 */
    0x00,			/* No causes */
    0x6F,			/* Cause(s) 16, 17, 18, 19, 21, 22 */
    0xF0,			/* Cause(s) 28, 29, 30, 31 */
    0x04,			/* Cause(s) 34 */

    0x9E,			/* Cause(s) 41, 42, 43, 44, 47 */
    0x54,			/* Cause(s) 50, 52, 54 */
    0x84,			/* Cause(s) 58, 63 */
    0x26,			/* Cause(s) 65, 66, 69 */
    0x00,			/* No causes */

    0x06,			/* Cause(s) 81, 82 */
    0x01,			/* Cause(s) 88 */
    0x5F,			/* Cause(s) 96, 97, 98, 99, 100, 102 */
    0x00,			/* No causes */
    0x00,			/* No causes */

    0x80			/* Cause(s) 127 */
};
#endif
#ifdef PRI_DMS100
static const BYTE PRI_DMS100_Causes[] =
{
    0x4E,			/* Cause(s) 1, 2, 3, 6 */
    0x00,			/* No causes */
    0x6F,			/* Cause(s) 16, 17, 18, 19, 21, 22 */
    0xF8,			/* Cause(s) 27, 28, 29, 30, 31 */
    0x04,			/* Cause(s) 34 */

    0x9E,			/* Cause(s) 41, 42, 43, 44, 47 */
    0x44,			/* Cause(s) 50, 54 */
    0x86,			/* Cause(s) 57, 58, 63 */
    0x46,			/* Cause(s) 65, 66, 70 */
    0x80,			/* Cause(s) 79 */

    0x06,			/* Cause(s) 81, 82 */
    0x85,			/* Cause(s) 88, 90, 95 */
    0x7B,			/* Cause(s) 96, 97, 99, 100, 101, 102 */
    0x80,			/* Cause(s) 111 */
    0x00,			/* No causes */

    0x80			/* Cause(s) 127 */
};
#endif
#ifdef BRI_DMS100
static const BYTE BRI_DMS100_Causes[] =
{
    0x5E,			/* Cause(s) 1, 2, 3, 4, 6 */
    0x41,			/* Cause(s) 8, 14 */
    0x6F,			/* Cause(s) 16, 17, 18, 19, 21, 22 */
    0xF8,			/* Cause(s) 27, 28, 29, 30, 31 */
    0x04,			/* Cause(s) 34 */

    0x9E,			/* Cause(s) 41, 42, 43, 44, 47 */
    0x6C,			/* Cause(s) 50, 51, 53, 54 */
    0x86,			/* Cause(s) 57, 58, 63 */
    0x62,			/* Cause(s) 65, 69, 70 */
    0x80,			/* Cause(s) 79 */

    0x06,			/* Cause(s) 81, 82 */
    0x81,			/* Cause(s) 88, 95 */
    0x7B,			/* Cause(s) 96, 97, 99, 100, 101, 102 */
    0x80,			/* Cause(s) 111 */
    0x00,			/* No causes */

    0x80			/* Cause(s) 127 */
};

static const BYTE BRI_NI1_Causes[] =
{
    0xDE,                       /* Cause(s) 1, 2, 3, 4, 6, 7 */
    0x2F,                       /* Cause(s) 8, 9, 10, 11, 13 */
    0x6F,                       /* Cause(s) 16, 17, 18, 19, 21, 22 */
    0xFC,                       /* Cause(s) 26, 27, 28, 29, 30, 31 */
    0x04,                       /* Cause(s) 34 */

    0x9E,                       /* Cause(s) 41, 42, 43, 44, 47 */
    0x2C,                       /* Cause(s) 50, 51, 53 */
    0x86,                       /* Cause(s) 57, 58, 63 */
    0x22,                       /* Cause(s) 65, 69 */
    0x80,                       /* Cause(s) 79 */

    0x02,                       /* Cause(s) 81 */
    0x0D,                       /* Cause(s) 88, 90, 91 */
    0x7B,                       /* Cause(s) 96, 97, 99, 100, 101, 102 */
    0x80,                       /* Cause(s) 111 */
    0x00,                       /* No causes */
    0x80                        /* Cause(s) 127 */
};
#endif
#ifdef VN2
static const BYTE VN2_Causes[] =
{
    0x00,			/* No causes */
    0x00,			/* No causes */
    0x6F,			/* Cause(s) 16, 17, 18, 19, 21, 22 */
    0x99,			/* Cause(s) 24, 27, 28, 31 */
    0x04,			/* Cause(s) 34 */

    0x96,			/* Cause(s) 41, 42, 44, 47 */
    0x06,			/* Cause(s) 49, 50 */
    0xD8,			/* Cause(s) 59, 60, 62, 63 */
    0x26,			/* Cause(s) 65, 66, 69 */
    0x80,			/* Cause(s) 79 */

    0x1E,			/* Cause(s) 81, 82, 83, 84 */
    0x90,			/* Cause(s) 92, 95 */
    0x9F,			/* Cause(s) 96, 97, 98, 99, 100, 103 */
    0x80,			/* Cause(s) 111 */
    0x06,			/* Cause(s) 113, 114 */

    0xC0			/* Cause(s) 126, 127 */
};
#endif
#ifdef KDD_NTT			/* Note: These causes are identically the
				 * CCITT causes */
static const BYTE KDD_NTT_Causes[] =
{
    0xCE,			/* Causes 1, 2, 3, 6, 7 */
    0x00,			/* No causes */
    0x6F,			/* Cause(s) 16, 17, 18, 19, 21, 22 */
    0xFC,			/* Cause(s) 26, 27, 28, 29, 30, 31 */
    0x44,			/* Cause(s) 34, 38 */

    0x9E,			/* Cause(s) 41, 42, 43, 44, 47 */
    0x06,			/* Cause(s) 49, 50 */
    0x86,			/* Cause(s) 57, 58, 63 */
    0x66,			/* Cause(s) 65, 66, 69, 70 */
    0x80,			/* Cause(s) 79 */

    0x7E,			/* Cause(s) 81, 82, 83, 84, 85, 86 */
    0x89,			/* Cause(s) 88, 91, 95 */
    0x7F,			/* Cause(s) 96, 97, 98, 99, 100, 101, 102 */
    0x80,			/* Cause(s) 111 */
    0x00,			/* No causes */

    0x80			/* Cause(s) 127 */
};
#endif
#ifdef BRI_1TR6
static const BYTE BRI_1TR6_Causes[] =
{
    0x8A,			/* Cause(s) 1, 3, 7 */
    0x05,			/* Cause(s) 8, 10 */
    0x03,			/* Cause(s) 16, 17 */
    0x00,			/* no causes */
    0x27,			/* Cause(s) 32, 33, 34, 35, 37 */

    0x00,			/* no causes */
    0x20,			/* Cause(s)  53 */
    0x67,			/* Cause(s) 56, 57, 58, 59, 61, 62 */
    0x00,			/* no causes */
    0x00,			/* no causes */

    0x00,			/* no causes */
    0x06,			/* Cause(s) 89, 90 */
    0x00,			/* no causes */
    0x00,			/* no causes */
    0x0F,			/* Cause(s) 112, 113, 114, 115 */

    0x80			/* Cause(s) 127 */
};
#endif
#ifdef PRI_1TR6
static const BYTE PRI_1TR6_Causes[] =
{
    0x8A,			/* Cause(s) 1, 3, 7 */
    0x05,			/* Cause(s) 8, 10 */
    0x03,			/* Cause(s) 16, 17 */
    0x00,			/* no causes */
    0x27,			/* Cause(s) 32, 33, 34, 35, 37 */

    0x00,			/* no causes */
    0x20,			/* Cause(s)  53 */
    0x67,			/* Cause(s) 56, 57, 58, 59, 61, 62 */
    0x00,			/* no causes */
    0x00,			/* no causes */

    0x00,			/* no causes */
    0x06,			/* Cause(s) 89, 90 */
    0x00,			/* no causes */
    0x00,			/* no causes */
    0x0F,			/* Cause(s) 112, 113, 114, 115 */

    0x80			/* Cause(s) 127 */
};
#endif
#ifdef BRI_NET3
static const BYTE NET3_Causes[] =
{
    0xCE,			/* Cause(s) 1,2,3,6,7  === */
    0x05,			/* Cause(s) 8, 10 === */
    0x6F,			/* Cause(s) 16, 17, 18, 19, 21, 22 */
    0x99,			/* Cause(s) 24, 27, 28, 31 */
    0x04,			/* Cause(s) 34 */

    0x96,			/* Cause(s) 41, 42, 44, 47 */
    0x06,			/* Cause(s) 49, 50 */
    0xD8,			/* Cause(s) 59, 60, 62, 63 */
    0x26,			/* Cause(s) 65, 66, 69 */
    0x80,			/* Cause(s) 79 */

    0x1E,			/* Cause(s) 81, 82, 83, 84 */
    0x90,			/* Cause(s) 92, 95 */
    0x9F,			/* Cause(s) 96, 97, 98, 99, 100, 103 */
    0x80,			/* Cause(s) 111 */
    0x06,			/* Cause(s) 113, 114 */

    0xC0			/* Cause(s) 126, 127 */
};
#endif
#ifdef BRI_TS013
static const BYTE TS013_Causes[] =
{
    0x42,			/* Cause(s) 1,6  */
    0x00,			/* no Cause(s) */
    0x67,			/* Cause(s) 16, 17, 18, 21, 22 */
    0xDC,			/* Cause(s) 26, 27, 28, 30, 31 */
    0x44,			/* Cause(s) 34, 38 */
    0x9E,			/* Cause(s) 41, 42, 43, 44, 47 */
    0x00,			/* no Cause(s) */
    0x83,			/* Cause(s) 57, 58, 63 */
    0x43,			/* Cause(s) 65, 66, 70 */
    0x80,			/* Cause(s) 79 */
    0x03,			/* Cause(s) 81, 82 */
    0x81,			/* Cause(s) 88, 95 */
    0x7F,			/* Cause(s) 96, 97, 98, 99, 100, 101, 102 */
    0x80,			/* Cause(s) 111 */
    0x00,			/* no Cause(s) */
    0x80			/* Cause(s) 127 */
};
#endif
static const BYTE PRI_NET5_Causes[] =
{
    0x42,                       /* Cause(s) 1, 6 */
    0x00,                       /* No causes */
    0x6F,                       /* Cause(s) 16, 17, 18, 19, 21, 22 */
    0xF0,                       /* Cause(s) 28, 29, 30, 31 */
    0x04,                       /* Cause(s) 34 */

    0x9E,                       /* Cause(s) 41, 42, 43, 44, 47 */
    0x54,                       /* Cause(s) 50, 52, 54 */
    0x84,                       /* Cause(s) 58, 63 */
    0x26,                       /* Cause(s) 65, 66, 69 */
    0x00,                       /* No causes */

    0x06,                       /* Cause(s) 81, 82 */
    0x01,                       /* Cause(s) 88 */
    0x5F,                       /* Cause(s) 96, 97, 98, 99, 100, 102 */
    0x00,                       /* No causes */
    0x04,                       /* Cause 114 */

    0xC0                        /* Cause(s) 126, 127 */
};
/* TS-014, Australia PRI */
static const BYTE PRI_TS014_Causes[] =
{
      0x42,                                           /* Cause(s) 1,6  */
      0x00,                                             /* no Cause(s) */
      0x67,                             /* Cause(s) 16, 17, 18, 21, 22 */
      0xD8,                                 /* Cause(s) 27, 28, 30, 31 */
      0x44,                                         /* Cause(s) 34, 38 */
      0x96,                                 /* Cause(s) 41, 42, 44, 47 */
      0x00,                                             /* no Cause(s) */
      0x86,                                     /* Cause(s) 57, 58, 63 */
      0x46,                                     /* Cause(s) 65, 66, 70 */
      0x80,                                             /* Cause(s) 79 */
      0x06,                                         /* Cause(s) 81, 82 */
      0x81,                                         /* Cause(s) 88, 95 */
      0x7F,                  /* Cause(s) 96, 97, 98, 99, 100, 101, 102 */
      0x80,                                            /* Cause(s) 111 */
      0x00,                                             /* no Cause(s) */
      0x80                                             /* Cause(s) 127 */
};

PRIVATE BYTE Unpacked[MAX_NUMB_INFO_FIELDS];	/* IE unpacking array */

/*
 * The following elements are defined for discarding values passed to
 * functions which are not used.  This occurs because of the variant
 * information elements structures for various applications.  Values not used
 * and not referenced otherwise cause compiler warnings.
 */
PRIVATE BYTE *PtrTrash;			/* Trashcan for BYTE pointers */
PRIVATE int IntTrash;			/* Trashcan for integers */

/************************************************************************
*                            M A C R O S                                *
*************************************************************************
*/
/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/
PRIVATE BOOLEAN CCIE_Match_IE_ID(struct IE_Header *, BYTE, \
				 int, int, SEARCH_CONTROL *);

/************************************************************************
*                C C I E   B U I L D   F U N C T I O N S                *
*************************************************************************
*/

/*
 * These functions accept parameters to be implanted in the encoded Layer 3
 * information elements as input.  The purpose of the functions is to build
 * the encoded IE from these parameters and to return the length of the
 * element added.  Thereby, the calling function need not know anything about
 * IE packing schemes, only the parameters required to build it.
 */

/*************************
* CCIE_BuildBearerCap_IE *
**************************
*
* Build a Bearer Capability information element.  Return the length of the
* element added.  It is assumed that the calling function has already
* prepared the later bytes for the IE as a local array which is simply
* passed to this routine for non-judgemental inclusion.  This
* simplification is assumed here since the contents of the later bytes
* is highly specific to the application.
*
*/
PUBLIC int CCIE_BuildBearerCap_IE(info_buffer, bear_cap, bear_cap_len)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE *bear_cap;			/* Bearer capability data array */
BYTE bear_cap_len;		/* Length of capability array */
{
    BYTE i;


    *info_buffer++ = BEARER_CAPABILITY_IE;	/* Bearer capability IE */
    *info_buffer++ = bear_cap_len;	/* Length of IE */
    for (i = 1; i <= bear_cap_len; i++)
	*info_buffer++ = *bear_cap++;

    return (IE_HEAD_LENGTH + bear_cap_len);
}


/**************************
* CCIE_BuildCallAppear_IE *
***************************
*
* Build a Call Appearance information element. Return the length of the
* element added.
*
* NOTE: THIS IS CODESET 6 I.E. !!
*
*/
PUBLIC int CCIE_BuildCallAppear_IE(info_buffer, call_appearance)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE call_appearance;		/* call appearance */

{
    *info_buffer++ = ORIG_CALL_APPEAR_IE;	/* switchhook state */
    *info_buffer++ = 1;		/* len of IE = 1 */
    *info_buffer++ = call_appearance;	/* CA */

    return (IE_HEAD_LENGTH + 1);
}


#ifdef VN2
/****************************
* CCIE_BuildCallIdentity_IE *
*****************************
*
* Build a Call Identity information element.  Return the length of the
* element added.
*
*/
PUBLIC int CCIE_BuildCallIdentity_IE(info_buffer, id_digits, digit_count)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE *id_digits;		/* ID digit string */
int digit_count;		/* Number of ID digits */
{
    int i;			/* Counter */

    *info_buffer++ = CALL_IDENT_IE;
    *info_buffer++ = digit_count;
    for (i = 0; i < digit_count; i++) {
	*info_buffer++ = *id_digits++;
    }

    return (IE_HEAD_LENGTH + digit_count);
}


#endif
/*************************
* CCIE_BuildCallState_IE *
**************************
*
* Build a Call State information element.  Return the length of the
* element added.
*
*/
PUBLIC int CCIE_BuildCallState_IE(info_buffer, state)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE state;			/* Q.931 call state */

{
    *info_buffer++ = CALL_STATE_IE;	/* call state element ID */
    *info_buffer++ = CALL_STATE_LENGTH;	/* len of IE = 1 */
    *info_buffer++ = state;	/* call state */

    return (IE_HEAD_LENGTH + CALL_STATE_LENGTH);
}


/***************************
* CCIE_BuildCallingNumb_IE *
****************************
*
* Build a Calling Party Number information element.
*
* This function is provided as an alternate to CCIE_BuildPartyNumb_IE()
* for those applications requiring the inclusion of octet 3a.
*
* Return the length of the element added.
*
* NOTE: The digit buffer is assumed to be a byte array, one digit per byte,
*       where: digits 0-9 = 0x00-0x09, digit * = 0x0A and digit # = 0x0B
*       or the digits may be converted to ASCII already.
*
* NOTE: numb_type is coded as follows:
*       0x01 = International number
*       0x02 = National number
*       0x04 = Local (directory) number
*
* NOTE: numb plan is coded as follows:
*       0x01 = ISDN (E.164)
*       0x02 = Telephony (E.163)
*       0x09 = Private
*
*/
PUBLIC int CCIE_BuildCallingNumb_IE(info_buffer, numb_type,
				        numb_plan, pres_ind, scrn_ind,
				        digit_buf, digit_buf_len, ascii)
BYTE *info_buffer;		/* The info buffer to modify */
int numb_type;			/* International, national or local number */
int numb_plan;			/* ISDN, Telephony, etc, numbering plan */
int pres_ind;			/* Presentation indicator */
int scrn_ind;			/* Screening indicator */
BYTE *digit_buf;		/* The digits */
int digit_buf_len;		/* Number of digits */
BOOLEAN ascii;			/* Presented digits are already converted to
				 * ASCII */
{
    int i;


    *info_buffer++ = CALLER_NUMBER_IE;	/* Define IE identifier */
    *info_buffer++ = digit_buf_len + 2;	/* Len of IE (incl. octet 3, 3a) */
    *info_buffer++ = (BYTE) ((numb_type << 4) | (numb_plan));
    *info_buffer++ = (BYTE) ((pres_ind << 5) | (scrn_ind) | 0x80);

    if (ascii) {		/* Just copy the ASCII digits */
	for (i = 0; i < digit_buf_len; i++) {
	    *info_buffer++ = *digit_buf++;
	}
    } else {			/* Convert digits to ASCII, then copy */
	for (i = 0; i < digit_buf_len; i++) {
	    /*
	     * In this implementation it is assumed that digit 0 has been
	     * stored as 0x0, * as 0xA and # as 0xB.  In some schemes 0 will
	     * be as 0xA, * as 0xB and # as 0xC where 0 is reserved as a
	     * digit string terminator.
	     */
	    if (*digit_buf < 0x0A)
		*info_buffer++ = (*digit_buf | 0x30);	/* Digits 0 - 9 */
	    else if (*digit_buf == 0x0A)
		*info_buffer++ = 0x2A;	/* Digit * */
	    else
		*info_buffer++ = 0x23;	/* Digit # */
	    digit_buf++;
	}
    }

    return (IE_HEAD_LENGTH + digit_buf_len + 2);
}


/*********************
* CCIE_BuildCause_IE *
**********************
*
* Build a Cause information element.  Return the length of the element
* added.
*
*/
PUBLIC int CCIE_BuildCause_IE(info_buffer, cause, network, diag)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE cause;			/* Cause identifier */
BOOLEAN network;		/* Build Cause from Network vs User
				 * perspective */
BYTE *diag;			/* Data for ie diagnostic field */

{
    int calculated_len;		/* The ie length is variable */
    int diag_len;    		/* Length of diagnostic */
    int i;			/* A counter */

    diag_len = 0;
    *info_buffer++ = CAUSE_IE;	/* Cause IE ID */

    switch (SwitchType) {
      default:
	if (diag) {
	    while (diag_len < DIAG_LEN) {
		if (diag[diag_len])
		    diag_len++;
		else
		    break;
	    }
	}
	break;
    }

    switch (SwitchType) {
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	calculated_len = *info_buffer++ = CAUSE_LENGTH + diag_len;
	*info_buffer++ = 0x87;
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	/*
	 * A cause that's "leerelement", which signifies normal clearing and
	 * has a lenght of zero
	 */
	if ((!network) && (cause == LEER_ELEMENT)) {
	    calculated_len = *info_buffer++ = 0;
	} else
	    calculated_len = *info_buffer++ = 1 + diag_len;
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	/*
	 * A cause length of zero means a "leerelement", which signifies
	 * normal clearing the user may use a cause of "LEER_ELEMENT"
	 */
	if ((!network) && ((cause == LEER_ELEMENT) || (cause == 0x00))) {
	    calculated_len = *info_buffer++ = 0;
	} else
	    calculated_len = *info_buffer++ = 1 + diag_len;
	break;
#endif
      default:
	calculated_len = *info_buffer++ = CAUSE_LENGTH + diag_len;
	/*
	 * 0x82 -> ext = 1, standard = CCITT, location = network. 0x80 -> ext
	 * = 1, standard = CCITT, location = user.
	 */
	*info_buffer++ = (network ? 0x82 : 0x80);
	break;
    }

    switch (SwitchType) {
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	if ((!network) && ((cause == LEER_ELEMENT) || (cause == 0x00)))
	    break;
	else
	    *info_buffer++ = cause | 0x80;	/* add cause value and
						 * extension bit */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	if ((!network) && (cause == LEER_ELEMENT))
	    break;
	else
	    *info_buffer++ = cause | 0x80;	/* add cause value and
						 * extension bit */
	break;
#endif
      default:
	*info_buffer++ = cause | 0x80;	/* Add cause value and extension bit */
	break;
    }

    if (diag_len) {
	switch (SwitchType) {
#ifdef PRI_DMS100
	  case PRI_DMS100_STYPE:
	    *info_buffer++ = 1;	/* Diagnostic length */
	    break;
#endif
	  default:
	    break;
	}
	for (i = 1; i <= diag_len; i++)
	    *info_buffer++ = *diag++;	/* Add diagnostic */
    }
    return (IE_HEAD_LENGTH + calculated_len);
}


/****************************
* CCIE_BuildChangeStatus_IE *
*****************************
*
* Build a Change Status information element.  Return the length of the
* element added.
*
*/
PUBLIC int CCIE_BuildChangeStatus_IE(info_buffer, preference, new_status)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE preference;		/* Channel or interface */
BYTE new_status;		/* Channel maint. status */

{
    *info_buffer++ = CHANGE_STATUS_IE;	/* call state element ID */
    *info_buffer++ = CHANGE_STATUS_LENGTH;	/* len of IE = 1 */
    *info_buffer++ = (preference == CHANNEL_PREF) ? (new_status | 0xC0) :
	(new_status | 0x80);

    return (IE_HEAD_LENGTH + CHANGE_STATUS_LENGTH);
}


/**********************
* CCIE_BuildChanId_IE *
***********************
*
* Build a Channel Identification information element.  Return the length
* of the element added.
*
*/
PUBLIC int CCIE_BuildChanId_IE(info_buffer, chan_sel, channel, chan_type,
			      slot_map, exclusive, int_ident_len, int_ident)
BYTE *info_buffer;		/* The info buffer to modify */
int chan_sel;			/* Info channel selection */
int channel;			/* Channel number (For BRI, chan numb is
				 * chan_sel) */
int chan_type;			/* B-chan, H0 or h11 */
BOOLEAN slot_map;		/* Present channel(s) in a slot_map format */
BOOLEAN exclusive;		/* Exclusive vs preferred channel */
int int_ident_len;		/* Interface identifier length */
BYTE *int_ident;		/* Interface identifier */
{
    int total_ie_len;		/* Element length */
    int i;			/* A counter */
    int last_channel;		/* Last channel for multi-channel request */

    /*
     * The following will cause problems, so just use switchtype for the
     * moment. ===
     */

    total_ie_len = 0;		/* Element length */
    *info_buffer++ = CHANNEL_ID_IE;	/* IE_ID */
    if ((SwitchType == PRI_5ESS_STYPE) || (SwitchType == PRI_DMS100_STYPE)
	|| (SwitchType == PRI_4ESS_STYPE) || (SwitchType == PRI_NET5_STYPE)
      || (SwitchType == PRI_NTT_STYPE) || (SwitchType == PRI_TS014_STYPE)) {
	/*
	 * For PRI we will skip the length calculation for now since this
	 * will be easier to calculate when we are considering the channels.
	 */
	info_buffer++;
    } else {
	*info_buffer++ = CHANNEL_ID_LEN_BRI;	/* 1 */
	total_ie_len = CHANNEL_ID_LEN_BRI + IE_HEAD_LENGTH;	/* 3 */
    }

    switch (SwitchType) {
      case BRI_5ESS_STYPE:
	/*
	 * First prepare the single data byte building on the channel
	 * selected.
	 */
	*info_buffer++ = exclusive ? (0x88 | chan_sel) :
	    (0x80 | chan_sel);
	PtrTrash = int_ident;	/* Prevent compiler warning */
	IntTrash = int_ident_len;	/* Prevent compiler warning */
	IntTrash = channel;	/* Prevent compiler warning */
	break;

      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:	/* === */
	/*
	 * First prepare the single data byte building on the channel
	 * selected.
	 */
	*info_buffer++ = exclusive ? (0x88 | chan_sel) :
	    (0x80 | chan_sel);
	PtrTrash = int_ident;	/* Prevent compiler warning */
	IntTrash = int_ident_len;	/* Prevent compiler warning */
	IntTrash = channel;	/* Prevent compiler warning */
	IntTrash = (int) slot_map;	/* Prevent compiler warning */
	IntTrash = chan_type;	/* Prevent compiler warning */
	break;

      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_DMS100_STYPE:
      case PRI_NET5_STYPE:
      case PRI_NTT_STYPE:
      case PRI_TS014_STYPE:
	/*
	 * If the channel == 0 then the info channel selection field is
	 * written as "0" or "No Channel."  This is a special case used by
	 * Protocol Control to convey the final timeout of T303 to Call
	 * Control.
	 * Note, it is also set below as well. ===
	 */
	*info_buffer = exclusive ? (0xA8 | chan_sel) : (0xA0 | chan_sel);

	/*
	 * NET5  and TS014 don't want the interface identifier in
         * the outgoing channel ie.
	 */
	if (SwitchType != PRI_NET5_STYPE && SwitchType != PRI_TS014_STYPE) {
	    if (int_ident_len) {
		*info_buffer++ |= 0x40;	/* Tag the interface ID present */
		for (i = 0; i < (int_ident_len - 1); i++)
		    *info_buffer++ = *int_ident++;	/* Copy interface ident */
		*info_buffer++ = (*int_ident | 0x80);	/* Last octet */
	    } else {
		info_buffer++;	/* Perform the pointer increment only */
	    }
	} else {
            if (detail_debug)
	        buginf("\nCCIE_BuildChanId_IE, int_ident_len = %d", int_ident_len);
	    int_ident_len = 0;
            /*
             * This needs to be looked at a little more. All calls are being
             * set to exclusive. Italy raised this issue because we will
             * correctly negotiate on incoming calls, but it should not change
             * yet because it may break other PRI switches.
             * Note, it is set above as well, the following overrides. ===
             */
	    *info_buffer = (0xA8 | chan_sel);	/* set it exclusive */
	    info_buffer++;
	}

	if (chan_type == H11_CHANNEL_UNITS) {
	    /*
	     * This is a negative index back to the length octet in the IE.
	     */
	    total_ie_len = IE_HEAD_LENGTH + int_ident_len + 1;
	    info_buffer[IE_HEAD_LENGTH - total_ie_len - 1] =
		total_ie_len - IE_HEAD_LENGTH;
	} else {
	    total_ie_len = CHANNEL_ID_LEN_PRI + IE_HEAD_LENGTH + int_ident_len;
	    /*
	     * CCITT / (number or map) / B-chan octet
	     */
	    /*
	     * Note that the channel type field is always deliniated in
	     * B_CHANNEL_UNITS because this is really a description of the
	     * channel list or map rather than the channel itself.  The H0
	     * channel is conveyed as a three octet bit map of B-channels of
	     * which 6 are indicated.
	     */
	    *info_buffer++ = ((slot_map) ? 0x90 : 0x80) | B_CHANNEL_UNITS;
	    if (slot_map) {
		/*
		 * First zero out the slot map area.
		 */
		info_buffer[0] = info_buffer[1] = info_buffer[2] = 0;
		/*
		 * Now set the individual bit for each channel within the
		 * range.
		 */
		last_channel = channel + GET_CHAN_SPANP(chan_type);
		for (i = channel; i <= last_channel; i++)
		    /*
		     * Don't ask...it works.
		     */
		    info_buffer[2 - ((i - 1) >> 3)] |= 0x01 << ((i - 1) & 0x7);
		info_buffer += 2;
		total_ie_len += 2;
	    } else {		/* Channel list */
		last_channel = channel + GET_CHAN_SPANP(chan_type);

		if (detail_debug)
		    buginf("\n building channel, channel = %x, last_channel = %x", channel, last_channel);

		for (i = channel; i < last_channel; i++) {
		    *info_buffer++ = i;
		    total_ie_len++;
		}
		/*
		 * This was a logic error, it has to be last-1.
		 */

                /*
                 * May not want to set the extension bit, if
                 * don't then add the switchtype. This
                 * sets the extension bit for the last channel.
                 * The DMS switchtype has been added here to
                 * support the Meridian 1 in SL1 mode. CSCdi48239.
                 * This had no effect on the dms switch.
                 */

                if (SwitchType != PRI_TS014_STYPE && SwitchType != PRI_DMS100_STYPE)
		    info_buffer[-1] = (last_channel - 1) | 0x80;
	    }
	    /*
	     * This is a negative index back to the length octet.
	     */
	    info_buffer[IE_HEAD_LENGTH - total_ie_len - 1] =
		total_ie_len - IE_HEAD_LENGTH;
	}
	break;
#ifdef VN2
#ifdef BRI_NET3
      case BRI_NET3_STYPE:	/* === */
#endif
#ifdef BRI_TS013_STYPE
      case BRI_TS013_STYPE:
#endif
      case VN2_STYPE:
      case VN3_STYPE:
#ifdef BRI
	*info_buffer++ = exclusive ? (0x88 | chan_sel) :	/* Octet 3 */
	    (0x80 | chan_sel);
	IntTrash = channel;	/* Prevent compiler warning */
	IntTrash = (int) slot_map;	/* Prevent compiler warning */
	IntTrash = chan_type;	/* Prevent compiler warning */
#else				/* PRI */
	*info_buffer++ = exclusive ? (0xA8 | chan_sel) :	/* Octet 3 */
	    (0xA0 | chan_sel);
	*info_buffer++ = 0x83;	/* CCITT / number / B-chan */
	*info_buffer = channel | 0x80;	/* The channel */
	total_ie_len = CHANNEL_ID_LEN_BRI + IE_HEAD_LENGTH + int_ident_len;
	/*
	 * This is a negative index back to the length octet in the IE.
	 */
	info_buffer[IE_HEAD_LENGTH - total_ie_len] =
	    total_ie_len - IE_HEAD_LENGTH;
#endif
	PtrTrash = int_ident;	/* Prevent compiler warning */
	IntTrash = int_ident_len;	/* Prevent compiler warning */
	last_channel = i = 0;
	IntTrash = last_channel;/* Prevent compiler warning */
	IntTrash = i;		/* Prevent compiler warning */
	break;
#endif				/* VN2 */
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
#ifdef BRI
	*info_buffer++ = exclusive ? (0x88 | chan_sel) :	/* Octet 3 */
	    (0x80 | chan_sel);
	IntTrash = channel;	/* Prevent compiler warning */
	IntTrash = (int) slot_map;	/* Prevent compiler warning */
	IntTrash = chan_type;	/* Prevent compiler warning */
	PtrTrash = int_ident;	/* Prevent compiler warning */
	IntTrash = int_ident_len;	/* Prevent compiler warning */
#else				/* PRI */
	/*
	 * If the channel == 0 then the info channel selection field is
	 * written as "0" or "No Channel."  This is a special case used by
	 * Protocol Control to convey the final timeout of T303 to Call
	 * Control.
	 */
	*info_buffer = exclusive ? (0xA8 | chan_sel) : (0xA0 | chan_sel);
	if (int_ident_len) {
	    *info_buffer++ |= 0x40;	/* Tag the interface ID present */
	    for (i = 0; i < (int_ident_len - 1); i++)
		*info_buffer++ = *int_ident++;	/* Copy interface ident */
	    *info_buffer++ = (*int_ident | 0x80);	/* Last octet */
	} else {
	    info_buffer++;	/* Perform the pointer increment only */
	}

	/*
	 * CCITT / (number or map) / B-chan octet
	 */
	*info_buffer++ = ((slot_map) ? 0x90 : 0x80) | (BYTE) chan_type;
	total_ie_len = CHANNEL_ID_LEN_BRI + IE_HEAD_LENGTH + int_ident_len;
	if (slot_map) {
	    if (chan_type == B_CHANNEL_UNITS) {
		/*
		 * First zero out the slot map area.
		 */
		info_buffer[0] = info_buffer[1] = info_buffer[2] = 0;
		/*
		 * Now set the individual bit for each channel within the
		 * range.
		 */
		last_channel = channel + GET_CHAN_SPANP(chan_type);
		for (i = channel; i <= last_channel; i++)
		    /*
		     * Don't ask...it works.
		     */
		    info_buffer[2 - ((i - 1) >> 3)] |= 0x01 << ((i - 1) & 0x7);
		info_buffer += 2;
		total_ie_len += 2;
	    } else if (chan_type == H0_CHANNEL_UNITS) {
		/*
		 * Set the individual bit for the channel group in the map.
		 */
		if (channel == 1) {
		    info_buffer[0] = 0x01;
		} else if (channel == 7) {
		    info_buffer[0] = 0x02;
		} else if (channel == 13) {
		    info_buffer[0] = 0x04;
		} else {
#ifdef DISPLAY_LOCAL_ERR
		    LIF_Error("CCIE", "CCIE_BuildChanId_IE",
			      "Invalid channel type")
#endif
			;
		}
	    } else {
#ifdef DISPLAY_LOCAL_ERR
		LIF_Error("CCIE", "CCIE_BuildChanId_IE",
			  "Invalid channel type")
#endif
		    ;
	    }
	} else {		/* Channel list rather than slot map */
	    if (chan_type == B_CHANNEL_UNITS) {
		*info_buffer = channel;
	    } else if (chan_type == H0_CHANNEL_UNITS) {
		if (channel == 1)
		    info_buffer[0] = 0x01;	/* H0 channel 1 */
		else if (channel == 7)
		    info_buffer[0] = 0x02;	/* H0 channel 2 */
		else if (channel == 13)
		    info_buffer[0] = 0x03;	/* H0 channel 3 */
		else {
#ifdef DISPLAY_LOCAL_ERR
		    LIF_Error("CCIE", "CCIE_BuildChanId_IE",
			      "Invalid channel type")
#endif
			;
		}
	    } else {
#ifdef DISPLAY_LOCAL_ERR
		LIF_Error("CCIE", "CCIE_BuildChanId_IE", "Invalid channel type")
#endif
		    ;
	    }
	}

	/*
	 * This is a negative index back to the length octet in the IE.
	 */
	info_buffer[IE_HEAD_LENGTH - total_ie_len] =
	    total_ie_len - IE_HEAD_LENGTH;
#endif				/* end PRI */
	break;
#endif				/* KDD_NTT */
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	/*
	 * First prepare the single data byte building on the channel
	 * selected.
	 */
	*info_buffer++ = exclusive ? (0x88 | chan_sel) :
	    (0x80 | chan_sel);
	IntTrash = channel;	/* Prevent compiler warning */
	PtrTrash = int_ident;	/* Prevent compiler warning */
	IntTrash = int_ident_len;	/* Prevent compiler warning */
	IntTrash = (int) slot_map;	/* Prevent compiler warning */
	IntTrash = chan_type;	/* Prevent compiler warning */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	info_buffer[-1] = 3;	/* IE len */
	*info_buffer++ = exclusive ? (0xA8 | chan_sel) :
	    (0xA0 | chan_sel);	/* Octet 3 */
	*info_buffer++ = 0xC3;	/* Octet 4 */
	*info_buffer = channel;	/* Octet 5 */
	total_ie_len = 5;
	PtrTrash = int_ident;	/* Prevent compiler warning */
	IntTrash = int_ident_len;	/* Prevent compiler warning */
	IntTrash = (int) slot_map;	/* Prevent compiler warning */
	IntTrash = chan_type;	/* Prevent compiler warning */
	break;
#endif
      default:			/* Should not occur */
	PtrTrash = int_ident;	/* Prevent compiler warning */
	IntTrash = int_ident_len;	/* Prevent compiler warning */
	IntTrash = channel;	/* Prevent compiler warning */
	IntTrash = (int) slot_map;	/* Prevent compiler warning */
	IntTrash = chan_type;	/* Prevent compiler warning */
	break;
    }

    return (total_ie_len);
}


/****************************
* CCIE_BuildCodesetShift_IE *
*****************************
*
* Build a Codeset Shift information element. Return the length of the
* element added.
*
*/
PUBLIC int CCIE_BuildCodesetShift_IE(info_buffer, codeset, locking_shift)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE codeset;			/* new codeset to shift to */
BOOLEAN locking_shift;		/* Is this a locking shift? */

{
    *info_buffer++ = locking_shift ? CODESET_SHIFT_IE + codeset :
	CODESET_SHIFT_IE | 0x08 | codeset;
    return (1);
}


/*************************
* CCIE_BuildCongLevel_IE *
**************************
*
* Build a congestion level information element.
* Return the length of the element added.
*
*/
PUBLIC int CCIE_BuildCongLevel_IE(info_buffer, level)
BYTE *info_buffer;		/* Outgoing info buffer */
int level;			/* Congestion level */
{
    *info_buffer = CONGEST_LEVEL_IE | level;

    return (1);
}


/**************************
* CCIE_BuildEndpointId_IE *
***************************
*
* Build an endpoint identifier information element (5ESS codeset 6).
* Return the length of the element added.
*
*/
PUBLIC int CCIE_BuildEndpointId_IE(info_buffer, usid, tid, interpreter)
BYTE *info_buffer;		/* The info buffer to modify */
int usid;
int tid;
int interpreter;
{
    *info_buffer++ = ENDPOINT_ID_IE;
    *info_buffer++ = ENDPOINT_ID_LENGTH;
    *info_buffer++ = usid;
    *info_buffer++ = (tid & 0x7f) | ((interpreter) ? 0x80 : 0x00);

    return (IE_HEAD_LENGTH + ENDPOINT_ID_LENGTH);
}


/*************************
* CCIE_BuildHighLayer_IE *
**************************
*
* Build a High Layer Compatibility information element. Return the
* length of the element added.
*
* Note: Currently coded to FRENCH VN2 standard.
*/
PUBLIC int CCIE_BuildHighLayer_IE(info_buffer, standard, interpretation,
				      presentation, teleservice)
BYTE *info_buffer;		/* The info buffer to modify */
int standard;			/* Coding standard */
int interpretation;		/* Interpretation */
int presentation;		/* Presentation */
int teleservice;		/* Service capability */

{
    *info_buffer++ = HIGH_LAYER_COMPAT_IE;	/* High layer compatibility */
    *info_buffer++ = HIGH_LAYER_LENGTH;	/* len of IE = 2 */
    *info_buffer++ = (0x80) |
	(standard << 5) |
	(interpretation << 2) |
	(presentation);
    *info_buffer = (0x80 | teleservice);

    return (IE_HEAD_LENGTH + HIGH_LAYER_LENGTH);
}


/*************************
* CCIE_BuildHookState_IE *
**************************
*
* Build a Switchhook state information element. Return the length of the
* element added.
*
*/
PUBLIC int CCIE_BuildHookState_IE(info_buffer, hook_state)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE hook_state;		/* switchhook state */

{
    *info_buffer++ = SWITCHHOOK_IE;	/* switchhook state */
    *info_buffer++ = SWITCHHOOK_LENGTH;	/* len of IE = 1 */
    *info_buffer++ = hook_state;/* hook state */

    return (IE_HEAD_LENGTH + SWITCHHOOK_LENGTH);
}


/**********************
* CCIE_BuildKeypad_IE *
***********************
*
* Build a Keypad information element.  Return the length of the
* element added.
*
*/
PUBLIC int CCIE_BuildKeypad_IE(info_buffer, digit, len, ascii)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE *digit;			/* Dialed digit buffer pointer */
int len;			/* Number of digits */
BOOLEAN ascii;			/* TRUE if digits are already converted to
				 * ASCII */
{
    int i;			/* A counter */

    *info_buffer++ = KEYPAD_FACILITY_IE;	/* keypad element ID */
    *info_buffer++ = (BYTE) len;/* len of IE = digit count */
    /*
     * Store digit(s)
     */
    if (ascii) {
	for (i = 0; i < len; i++) {
	    *info_buffer++ = *digit++;
	}
    } else {
	for (i = 0; i < len; i++) {
	    /*
	     * In this implementation it is assumed that digit 0 has been
	     * stored as 0x0, * as 0xA and # as 0xB.  In some schemes 0 will
	     * be as 0xA, * as 0xB and # as 0xC where 0 is reserved as a
	     * digit string terminator.
	     */
	    if (*digit < 0x0A)
		*info_buffer++ = *digit | 0x30;	/* Digits 0 - 9 */
	    else if (*digit == 0x0A)
		*info_buffer++ = 0x2A;	/* Digit * */
	    else
		*info_buffer++ = 0x23;	/* Digit # */
	    digit++;
	}
    }

    return (IE_HEAD_LENGTH + len);
}

#ifdef PRI_1TR6
/***************************
* CCIE_BuildNetFac_IE_1TR6 *
****************************
*
* Build a Network Facility element.  For certain "Ack" messages
* such as FACILITY_ACK 1TR6 uses the Facility_Select IE instead of NSF.
* Facility_Select is little different in format
*/
PUBLIC int CCIE_BuildNetFac_IE_1TR6(info_buffer, facilities, ack, ie)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE facilities;		/* The Facility to send */
BOOLEAN ack;			/* Whether to use Facility_Select */
BYTE *ie;			/* pointer to the source ie */

{
    int k = 0;
    BYTE *j;
    int IE_index;

    *info_buffer++ = (ack ? FACILITY_SELECT_IE : NETWORK_FACILITY_IE);
    j = info_buffer++;		/* placeholder */
    if (!ack) {
	*info_buffer++ = 0x00;	/* zero */
	k += 1;
    }
    *info_buffer++ = facilities;/* Fac Code */
    *info_buffer++ = ie[3];	/* Service */
    if (ie[4]) {
	*info_buffer++ = ie[4];	/* Add   info */
	k += 3;
    } else {
	k += 2;
    }

    IE_index = 5;
    while (IE_index < (ie[1] + 2))	/* Pick up Parafelder */
	*info_buffer++ = ie[IE_index++];
    k += (IE_index - 5);
    *j = k;

    return (TRUE);
}
#endif

#ifdef VN2
/**********************
* CCIE_BuildNatFac_IE *
***********************
*
* Build a National Facility information element. Return the length of the
* element added.
*
* WARNING: This function is specific to French VN3 BRI/PRI and no other!
*
*/
PUBLIC int CCIE_BuildNatFac_IE(info_buffer, fac_spec, fac_len)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE *fac_spec;			/* Facility string */
int fac_len;			/* Length of facility string */

{
    int i;			/* Counter */


    *info_buffer++ = NATIONAL_FACILITY_IE;	/* Add IE ID */
    /*
     * Use the parameterized field to transport the Facility Specification.
     */
    *info_buffer++ = fac_len + 1;	/* Octet 2 */
    *info_buffer++ = 0;		/* Octet 3 - Network ID length */
    for (i = 0; i < fac_len; i++)
	*info_buffer++ = *fac_spec++;	/* Copy facility to octets 4+ */

    return (IE_HEAD_LENGTH + fac_len + 1);
}

#endif				/* VN2 */
#ifdef PRI
/**********************
* CCIE_BuildNetFac_IE *
***********************
*
* Build a Network Facilities information element. Return the length of the
* element added.
*
*/
PUBLIC int CCIE_BuildNetFac_IE(info_buffer, id_len, id_type, id_plan,
			           net_id, parameterized, expansion,
			           feat_serv, fac_cod_val, parm_field,
			           parm_field_len)
BYTE *info_buffer;		/* The info buffer to modify */
int id_len;			/* Length of network id */
int id_type;			/* Type of network id */
int id_plan;			/* Network id plan */
BYTE *net_id;			/* Network id array */
BOOLEAN parameterized;		/* TRUE for parameterized facility request */
BOOLEAN expansion;		/* TRUE for expansion of coding value */
BOOLEAN feat_serv;		/* TRUE if requested facility is a service */
int fac_cod_val;		/* Binary facility coding value */
BYTE *parm_field;		/* Parameterized field array */
int parm_field_len;		/* Length of parameterized field */

{
    int i;


    *info_buffer++ = NETWORK_FACILITY_IE;	/* Add IE ID */
    *info_buffer++ = id_len + parm_field_len + 2;	/* Add IE len */
    *info_buffer++ = id_len;	/* Network ID length */
    if (id_len) {		/* Add network ID */
	/*
	 * ID data.
	 */
	*info_buffer++ = 0x80 | (id_type << 4) | id_plan;
	for (i = 1; i < id_len; i++)
	    *info_buffer++ = *net_id++;
    }
    *info_buffer = (parameterized) ? 0 : 0x80;
    *info_buffer |= (expansion) ? 0x40 : 0;
    *info_buffer |= (feat_serv) ? 0x20 : 0;
    *info_buffer++ |= fac_cod_val;

    if (parameterized) {	/* Add the parm field */
	for (i = 1; i <= parm_field_len; i++)
	    *info_buffer++ = *parm_field++;
    }
    /*
     * The additional 2 bytes returned allows for network id length octet (3)
     * and the parameterized/exp/feat_serv/fac_code octet (4 or 6).
     */
    return (IE_HEAD_LENGTH + id_len + parm_field_len + 2);
}


#endif				/* PRI */
/*************************
* CCIE_BuildNotifyInd_IE *
**************************
*
* Build a Notify Indicator information element.
* Return the length of the element added.
*
*/
PUBLIC int CCIE_BuildNotifyInd_IE(info_buffer, desc, desc_len)
BYTE *info_buffer;		/* The info buffer to modify */
BYTE *desc;			/* The description */
int desc_len;			/* Length of description */

{
    int i;			/* Counter */


    *info_buffer++ = NOTIFY_IND_IE;	/* Define IE identifier */
    *info_buffer++ = desc_len;	/* IE length */
    i = desc_len;
    while (i--) {
	*info_buffer++ = *desc++;
    }
    return (IE_HEAD_LENGTH + desc_len);
}


/*************************
* CCIE_BuildPartyNumb_IE *
**************************
*
* Build a Called or Calling Party Number information element.
* Return the length of the element added.
*
* NOTE: The digit buffer is assumed to be a byte array, one digit per byte,
*       where: digits 0-9 = 0x00-0x09, digit * = 0x0A and digit # = 0x0B
*       or the digits may be converted to ASCII already.
*
* NOTE: numb_type is coded as follows:
*       0x01 = International number
*       0x02 = National number
*       0x04 = Local (directory) number
*
* NOTE: numb plan is coded as follows:
*       0x01 = ISDN (E.164)
*       0x02 = Telephony (E.163)
*       0x09 = Private
*
* NOTE: Octet 3a in the Calling party number IE is not used at this point.
*       (see CCIE_BuildCallingNumb_IE()).
*/
PUBLIC int CCIE_BuildPartyNumb_IE(info_buffer, ie_id, numb_type, numb_plan,
				      digit_buf, digit_buf_len, ascii)
BYTE *info_buffer;		/* The info buffer to modify */
int ie_id;			/* CALLER_NUMBER_IE, CALLER_SUB_ADDR_IE, */
/* CALLED_NUMBER_IE or CALLED_SUB_ADDR_IE */
int numb_type;			/* International, national or local number */
int numb_plan;			/* ISDN, Telephony, etc, numbering plan */
BYTE *digit_buf;		/* The digits */
int digit_buf_len;		/* Number of digits */
BOOLEAN ascii;			/* Presented digits are already converted to
				 * ASCII */
{
    int header_bytes;
    int i;


    *info_buffer++ = ie_id;	/* Define IE identifier */

    if ((ie_id == CALLED_SUB_ADDR_IE) || (ie_id == CALLER_SUB_ADDR_IE)) {
	/*
	 * This coding complies with VN2.  Other SwitchTypes should be
	 * reviewed on a case basis.
	 */
	*info_buffer++ = digit_buf_len + 2;	/* Len of IE (incl octet 3&4) */
        /*
         * This routine is not called for BRI at all it is only called
         * for PRI. The numb_type and numb_plan field should not be
         * or'ed for NET5. Other PRI switchtypes that support sub-addressing
         * will have to be reviewed case by case. They are actually
         * not number type and number plan for sub addressing. They
         * are the type of subaddress and the ascii "P" seperator.
         * The type of subaddress is NSAP 0x0 or'ed with 0x80.
         * The "P" is 0x50. CSCdi34492
         * CSCdi87936 was the same bug for the NTT switchtype.
         */ 
        if ((SwitchType != PRI_NET5_STYPE) && (SwitchType != PRI_NTT_STYPE))
	    *info_buffer++ = (BYTE) ((numb_type << 4) | (numb_plan) | 0x80);
        else
	    *info_buffer++ = (BYTE) numb_type;
	switch (SwitchType) {
#ifdef VN2
	  case VN2_STYPE:
	  case VN3_STYPE:
	    header_bytes = 1;
	    break;
#endif
	  default:
	    *info_buffer++ = IA_5_FORMAT;
	    header_bytes = 2;
	    break;
	}
    } else {
	*info_buffer++ = digit_buf_len + 1;	/* Len of IE (incl. octet 3) */
	*info_buffer++ = (BYTE) ((numb_type << 4) | (numb_plan) | 0x80);
	header_bytes = 1;
    }

    if (ascii) {		/* Just copy the ASCII digits */
	for (i = 0; i < digit_buf_len; i++) {
	    *info_buffer++ = *digit_buf++;
	}
    } else {			/* Convert digits to ASCII, then copy */
	for (i = 0; i < digit_buf_len; i++) {
	    /*
	     * In this implementation it is assumed that digit 0 has been
	     * stored as 0x0, * as 0xA and # as 0xB.  In some schemes 0 will
	     * be as 0xA, * as 0xB and # as 0xC where 0 is reserved as a
	     * digit string terminator.
	     */
	    if (*digit_buf < 0x0A)
		*info_buffer++ = (*digit_buf | 0x30);	/* Digits 0 - 9 */
	    else if (*digit_buf == 0x0A)
		*info_buffer++ = 0x2A;	/* Digit * */
	    else
		*info_buffer++ = 0x23;	/* Digit # */
	    digit_buf++;
	}
    }

    return (IE_HEAD_LENGTH + digit_buf_len + header_bytes);
}


/***********************
* CCIE_BuildProgInd_IE *
************************
*
* Build a Progress Indicator information element.  Return the length of
* the element added.
*
*/
PUBLIC int CCIE_BuildProgInd_IE(info_buffer, code_std, location, prog_desc)
BYTE *info_buffer;		/* The info buffer to modify */
int code_std;			/* Coding standard */
int location;			/* Location */
int prog_desc;			/* Progress description */

{
    *info_buffer++ = PROGRESS_IND_IE;	/* IE identifier */
    *info_buffer++ = PROG_IND_LENGTH;	/* Len of IE = 2 */
    *info_buffer++ = (0x80) | (code_std << 5) | (location);
    *info_buffer = (0x80) | (prog_desc);

    return (IE_HEAD_LENGTH + PROG_IND_LENGTH);
}


/**************************
* CCIE_BuildRestartInd_IE *
***************************
*
* Build a Restart Indication information element.  Return the length of the
* element added.
*
*/
PUBLIC int CCIE_BuildRestartInd_IE(info_buffer, restart_class)
BYTE *info_buffer;		/* The info buffer to modify */
int restart_class;		/* Type of restart */

{
    *info_buffer++ = RESTART_IND_IE;	/* Restart Ind element ID */
    *info_buffer++ = RESTART_IND_LENGTH;	/* Len of IE = 1 */
    *info_buffer = restart_class | 0x80;

    return (IE_HEAD_LENGTH + RESTART_IND_LENGTH);
}


/**********************
* CCIE_BuildSignal_IE *
***********************
*
* Build a Signal information element.  Return the length of
* the element added.
*
*/
PUBLIC int CCIE_BuildSignal_IE(info_buffer, sig_value)
BYTE *info_buffer;		/* The info buffer to modify */
int sig_value;			/* Signal value */

{
    *info_buffer++ = SIGNAL_IE;	/* IE identifier */
    *info_buffer++ = SIGNAL_IE_LENGTH;	/* Len of IE = 1 */
    *info_buffer = sig_value;

    return (IE_HEAD_LENGTH + SIGNAL_IE_LENGTH);
}


/***********************
* CCIE_BuildTermCap_IE *
************************
*
* Build a Terminal Capabilities information element.  Return the length of
* the element added.
*
*/
PUBLIC int CCIE_BuildTermCap_IE(info_buffer, standard, capability)
BYTE *info_buffer;		/* The info buffer to modify */
int standard;			/* Coding standard */
int capability;			/* Capability description */

{
    *info_buffer++ = TERMINAL_CAP_IE;	/* IE identifier */
    *info_buffer++ = TERMINAL_CAP_LENGTH;	/* Len of IE = 1 */
    *info_buffer = PACK_BYTE((BYTE) standard, *info_buffer, 6, 2);
    *info_buffer = PACK_BYTE((BYTE) capability, *info_buffer, 0, 6);

    return (IE_HEAD_LENGTH + TERMINAL_CAP_LENGTH);
}


/********************
* CCIE_BuildUser_IE *
*********************
*
* Build a User-user information element or User-user Facility information
* element.  Return the length of the element added.
*
*/
PUBLIC int CCIE_BuildUser_IE(info_buffer, ie_id, proto, info, info_len)
BYTE *info_buffer;		/* The info buffer to modify */
int ie_id;			/* USER_USER_IE or USER_USER_FAC_IE (VN2
				 * Codeset 6) */
int proto;			/* User protocol description */
BYTE *info;			/* User-user information */
int info_len;			/* Length of user-user info */

{
    int i;			/* A counter */


    *info_buffer++ = ie_id;	/* Element I.D. */
    *info_buffer++ = info_len + 1;	/* Element length incl. proto */
    *info_buffer++ = proto;	/* Info protocol */
    for (i = 0; i < info_len; i++)
	*info_buffer++ = *info++;

    return (IE_HEAD_LENGTH + info_len + 1);
}


/************************************************************************
*        C C I E   M I S C E L L A N E O U S   F U N C T I O N S        *
*************************************************************************
*/

/******************
* CCIE_CauseValid *
*******************
*
* Test if a cause value is defined for a SwitchType by testing a bit
* in a Causes bit array corresponding to the cause.  The shift
* right 3 near the top of this function is effectively an integer
* divide by 8 to locate the correct byte to examine in the array for
* the appropriate bit.  This test does not consider the Cause value
* within the context of the call state and action, only if the specs
* define it.
*
*/
PUBLIC BOOLEAN CCIE_CauseValid(cause)
int cause;			/* The cause value to test */

{
    int byte_numb;		/* Bit byte */
    int bit_numb;		/* The bit to test in the byte */
    BOOLEAN valid;		/* Result of this test */


    valid = (cause <= 0x7F);
    if (valid) {
	byte_numb = cause >> 3;	/* Locate the correct byte */
	bit_numb = cause & 0x07;/* Locate the correct bit in the byte */

	switch (SwitchType) {
#ifdef BRI_5ESS
	  case BRI_5ESS_STYPE:
	    valid = SUB_BYTE(BRI_5ESS_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
#ifdef PRI_5ESS
	  case PRI_5ESS_STYPE:
	    valid = SUB_BYTE(PRI_5ESS_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
#ifdef PRI_4ESS
	  case PRI_4ESS_STYPE:
	    valid = SUB_BYTE(PRI_4ESS_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
#ifdef PRI_DMS100
	  case PRI_DMS100_STYPE:
	    valid = SUB_BYTE(PRI_DMS100_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
#ifdef BRI_DMS100
	  case BRI_DMS100_STYPE:
	    valid = SUB_BYTE(BRI_DMS100_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
#ifdef VN2
	  case VN2_STYPE:
	  case VN3_STYPE:
	    valid = SUB_BYTE(VN2_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
#ifdef BRI_NET3			/* === */
	  case BRI_NET3_STYPE:
	    valid = SUB_BYTE(NET3_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
#ifdef BRI_TS013
	  case BRI_TS013_STYPE:
	    valid = SUB_BYTE(TS013_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
#ifdef KDD_NTT
	  case KDD_STYPE:
	  case NTT_STYPE:
	  case PRI_NTT_STYPE:
	    valid = SUB_BYTE(KDD_NTT_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    if (cause == LEER_ELEMENT) {
		valid = TRUE;
	    } else {
		valid = SUB_BYTE(PRI_1TR6_Causes[byte_numb], bit_numb, 1);
	    }
	    break;
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    valid = SUB_BYTE(BRI_1TR6_Causes[byte_numb], bit_numb, 1);
	    break;
#endif
          case PRI_NET5_STYPE:
            valid = SUB_BYTE(PRI_NET5_Causes[byte_numb], bit_numb, 1);
            break;

          case PRI_TS014_STYPE:
            valid = SUB_BYTE(PRI_TS014_Causes[byte_numb], bit_numb, 1);
            break;

	  case BRI_NI1_STYPE:
	    valid = SUB_BYTE(BRI_NI1_Causes[byte_numb], bit_numb, 1);
	    break;

	  default:
	    valid = TRUE;	/* Others not implemented here */
	    break;
	}
    }
    return (valid);
}


/***********************
* CCIE_Change_Msg_Type *
************************
*
* This function finds the message type field in the info string
* passed to it and changes it to the indicated msg type value.
* This may be useful in cases where the info string needs to be
* reused after changing the message type.
*
*/

PUBLIC void CCIE_Change_Msg_Type(info_string, message_type)
BYTE *info_string;
int message_type;

{
    info_string[(CCIE_LenOf_L3_Msg_Header(info_string)) - 1]
	= message_type;
    return;
}


/************************************************************************
*               C C I E   E X P A N D   F U N C T I O N S               *
*************************************************************************
*/

/*
 * These functions accept as input individual information elements.  The
 * purpose of the functions is to unpack the significant IE data fields into
 * a byte array, one field to a byte, placed in the least significant bits of
 * that byte.  Thereby, the calling function need not know anything about IE
 * packing schemes, only the byte numbers where the desired data has been
 * moved.
 * 
 * Additionally, if a structural error is detected in the IE, struc_err is set
 * to indicte this situation.
 */

#ifdef KDD_NTT
/*******************************
* CCIE_ExpandAdviceOfCharge_IE *
********************************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
*/
PUBLIC BYTE *CCIE_ExpandAdviceOfCharge_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */
{
    int i;			/* A counter */

    Unpacked[0] = ie[1];	/* Element length */
    Unpacked[1] = SUB_BYTE(ie[2], 4, 4);	/* Fixed Ext & resvd bits */
    Unpacked[2] = SUB_BYTE(ie[2], 0, 4);	/* Indicator type */
    for (i = 3; i < 17; i++) {	/* Move charging data. */
	if (i == Unpacked[0] + 2) {	/* End of data */
	    Unpacked[i] = 0;	/* Zero-terminate the list */
	    break;
	} else {
	    Unpacked[i] = SUB_BYTE(ie[i], 0, 7);	/* Charging data */
	}
    }

    /*
     * Test for element intergrity.
     */
    *struct_err = ((Unpacked[0] > 14) || (Unpacked[1] != 0x08));

    return (Unpacked);
}

#endif
/************************
* CCIE_ExpandBearCap_IE *
*************************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
*	Note: on the VN2 switch, elements 8 - 15 are always 0, as the
*	low layer compatibility information is not supported, and we are
*	not allowed to do only partial checking on it.  So sayeth CNET
*
*  Upon completion, the IE is unpacked as follows:
*
*  Unpacked[ 0 ]  -  Element length from octet 2
*  Unpacked[ 1 ]  -  Coding standard from octet 3
*  Unpacked[ 2 ]  -  Info transfer capability from octet 3
*  Unpacked[ 3 ]  -  Transfer mode from octet 4
*  Unpacked[ 4 ]  -  Info transfer rate from octet 4
*  Unpacked[ 5 ]  -  Bits 7-1 from octet 4a if present (else 0)
*  Unpacked[ 6 ]  -  Bits 7-1 from octet 4b if present (else 0)
*  Unpacked[ 7 ]  -  Number of octets (4, 4a and 4b) found
*  Unpacked[ 8 ]  -  User info layer 1 proto from octet 5 if present (else 0)
*  Unpacked[ 9 ]  -  Bits 7-1 from octet 5a if present (else 0)
*  Unpacked[ 10 ] -  Bits 7-1 from octet 5b if present (else 0)
*  Unpacked[ 11 ] -  Bits 7-1 from octet 5c if present (else 0)
*  Unpacked[ 12 ] -  Bits 7-1 from octet 5d if present (else 0)
*  Unpacked[ 13 ] -  Number of octets (5, 5a, 5b...etc) found
*  Unpacked[ 14 ] -  Bits 7-1 from octet 6 if present (else 0)
*  Unpacked[ 15 ] -  Bits 7-1 from octet 7 if present (else 0)
*/
PUBLIC BYTE *CCIE_ExpandBearCap_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */
{
    BYTE *ie_list;		/* Internal ie list pointer */
    BYTE *out_list;		/* Internal Unpacked pointer */
    int i;			/* A counter */
    int octet_no;		/* === */
    int ie_len;			/* Copy of IE length */


    for (i = 0; i < 16; i++)	/* Initialize the output buffer */
	Unpacked[i] = 0;

    Unpacked[0] = ie_len = ie[1];	/* Element length */
    octet_no = 2;
    Unpacked[1] = SUB_BYTE(ie[octet_no], 5, 2);	/* Coding standard */
    Unpacked[2] = SUB_BYTE(ie[octet_no], 0, 5);	/* Info xfer capability */
    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
	while (!(ie[octet_no] & 0x80)) {
	    octet_no++;
	    ie_len--;
	    Unpacked[0]--;
	}
    }
    ie_len--;			/* Octet 3 read */
    octet_no++;
    Unpacked[3] = SUB_BYTE(ie[octet_no], 5, 2);	/* Xfer mode */
    Unpacked[4] = SUB_BYTE(ie[octet_no], 0, 5);	/* Info xfer rate */
    ie_len--;			/* Octet 4 read */
    Unpacked[7]++;		/* Count octet 4 */

    /*
     * The unpacking of byte 4, 5 and their extensions are more tricky. It is
     * assumed that Call Control will be required to exercise some extra
     * smarts in looking at the result for possible extended octets.
     */

    /*
     * Consider Octet 4's extensions.
     */
    ie_list = &ie[octet_no];
    out_list = &Unpacked[5];
    for (;;) {
	if (ie_len && (*ie_list++ < 0x80)) {
	    *out_list++ = SUB_BYTE(*ie_list, 0, 7);	/* Read 4a or 4b */
	    ie_len--;		/* Octet 4 extension read */
	    Unpacked[7]++;	/* Count */
	} else {
	    break;
	}
    }

    /*
     * Consider Octet 5 and its extensions.
     */
    if ((ie_len) && (SUB_BYTE(*ie_list, 5, 2) == 0x01)) {
	Unpacked[8] = SUB_BYTE(*ie_list, 0, 5);	/* user info proto */
	Unpacked[13]++;		/* Count octet 5 */
	ie_len--;		/* Octet 5 read */
	out_list = &Unpacked[9];
	for (;;) {
	    if (ie_len && (*ie_list++ < 0x80)) {
		*out_list++ = SUB_BYTE(*ie_list, 0, 7);	/* Read 5a, 5b */
		ie_len--;	/* Octet 5 extension read */
		Unpacked[13]++;	/* Count 5a, 5b, etc. */
	    } else {
		break;
	    }
	}
    }
    /*
     * Consider octet 6
     */
    if ((ie_len) && (SUB_BYTE(*ie_list, 5, 2) == 0x02)) {
	Unpacked[14] = SUB_BYTE(*ie_list++, 0, 7);	/* User info proto */
	ie_len--;		/* Octet 6 read */
    }
    /*
     * Consider octet 7
     */
    if ((ie_len) && (SUB_BYTE(*ie_list, 5, 2) == 0x03)) {
	Unpacked[15] = SUB_BYTE(*ie_list, 0, 7);	/* User info proto */
	ie_len--;		/* Octet 7 read */
    }
    switch (SwitchType) {
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:	/* === */
	/*
	 * Test extension bits for intergrity and overall length and test
	 * that, if octet 5 is present, bits 7 & 6 convey Layer 1 ID.
	 */
	*struct_err = (((ie[2] & 0x80) != 0x80) ||
		       ((ie[3] & 0x80) != 0x80) ||
		       ((ie[1] > (1 + Unpacked[7])) &&	/* 5 present */
			(SUB_BYTE(ie[3 + Unpacked[7]], 5, 2) != 0x01)) ||
		       (ie_len != 0));

        /*
         * Octet 5a, the rate of the BC. If the value is zero, then it is a 64kb
         * call, else the 0x0f indicates 56kb; these are the only two supported.
         */
        if ((SwitchType == BRI_NI1_STYPE) || (SwitchType == BRI_DMS100_STYPE)) {
            if ((Unpacked[9] != 0) && (Unpacked[9] != 0x0f)) {
                *struct_err = TRUE;
            }
        }
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	/*
	 * Test extension bits and ie length for intergrity.
	 */
	*struct_err =
	    ((ie[2] < 0x80) ||
	     (ie[3] < 0x80) ||
	     (ie[1] < 2) ||
	     ((ie[1] == 3) && (ie[4] < 0x80)) ||
	     ((ie[1] == 4) && ((ie[4] >= 0x80) || (ie[5] < 0x80))) ||
	     (ie[1] > 4) ||
	     (ie_len != 0));
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	/*
	 * Assume octet 5 is optional for unrestricted digital capability.
	 */
	/*
	 * We must ignore octets 5 and onward if we do not support Low Layer
	 * Capability.  So sayeth CNET.  Previous code:
	 * 
	 *struct_err = (Unpacked[2] == BEAR_CAP_UNREST_DIG) ?
	 * ((Unpacked[0]!=3) && (Unpacked[0]!=2)) : (Unpacked[0] != 3);
	 */
	for (i = 8; i <= 15; i++)
	    Unpacked[i] = 0;
	*struct_err = (Unpacked[2] == BEAR_CAP_UNREST_DIG) ?
	    Unpacked[0] < 2 : (Unpacked[0] != 3);	/* Length error */
	break;
#endif
      default:
	/*
	 * This parameter is minimally supported for other SwitchTypes
	 */
	*struct_err = (ie_len != 0);

        /*
         * Make sure that transfer mode is circuit mode and not packet mode
         * found during Italy Net5 testing.
         */
        *struct_err = (Unpacked[3] != BEAR_RATE_XFER_CIRCUIT);
	break;
    }

    return (Unpacked);
}


/**********************
* CCIE_ExpandCause_IE *
***********************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
*/
PUBLIC BYTE *CCIE_ExpandCause_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */
{
    int i, j;			/* Counters */
    int vn2_octets;
    static const BYTE diagopt[] = {16, 0};
    static const BYTE diagreq[] = {1, 2, 3, 21, 29, 43, 49, 50,
	57, 58, 65, 66, 69, 82, 86, 88, 96, 97, 98, 99, 100,
    101, 102, 0};

    /*
     * Unpack the coding std, location and cause.
     */
    switch (SwitchType) {
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	if (ie[1] == 2) {
	    Unpacked[0] = SUB_BYTE(ie[3], 5, 2);	/* Coding std */
	    Unpacked[1] = SUB_BYTE(ie[3], 0, 4);	/* Location */
	}
	Unpacked[2] = (ie[2] & 0x7F);	/* Cause value */
	i = j = 3;		/* Set input and output indices */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	if (ie[1] == 2) {
	    Unpacked[0] = SUB_BYTE(ie[3], 5, 2);	/* Coding std */
	    Unpacked[1] = SUB_BYTE(ie[3], 0, 4);	/* Location */
	}
	if (ie[1] == 0) {
	    Unpacked[2] = LEER_ELEMENT;
	} else {
	    Unpacked[2] = (ie[2] & 0x7F);	/* Cause value */
	}
	i = j = 3;		/* Set input and output indices */
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Unpacked[0] = SUB_BYTE(ie[2], 5, 2);	/* Coding standard */
	Unpacked[1] = SUB_BYTE(ie[2], 0, 4);	/* Location */
	Unpacked[2] = (ie[3] & 0x7F);	        /* Cause value */
	i = 3;			/* Allow for diagnostic length */
	j = 5;
	break;
#endif
      case BRI_NET3_STYPE:
      case VN2_STYPE:		/* case added === */
      case VN3_STYPE:		/* case added === */
	j = 2;
	Unpacked[0] = SUB_BYTE(ie[j], 5, 2);	/* Coding standard */
	Unpacked[1] = SUB_BYTE(ie[j], 0, 4);	/* Location */
	while (!(ie[j++] & 0x80));
	Unpacked[2] = (ie[j] & 0x7F);	/* Cause value */
	while (!(ie[j++] & 0x80));
	i = 3;			/* Set input and output indices */
	break;

      case PRI_NET5_STYPE:
        /*
         * NET5 allows an optional octet in the Cause field, check for
         * the length, ie[1], before checking the fields, can be 2 or 3.
         */
        Unpacked[0] = SUB_BYTE(ie[2], 5, 2);    /* Coding standard */
        Unpacked[1] = SUB_BYTE(ie[2], 0, 4);    /* Location */
        if (ie[1] == 2) {
            /* this is what is normally seen, 2 octets */ 
            Unpacked[2] = (ie[3] & 0x7F);    /* Cause value */
            i = 3;			/* Set input and output indices */
            j = 4;
        } else {
            /* includes optional octet 3a */
            Unpacked[2] = (ie[4] & 0x7F);	/* Cause value */
            i = 3;
            j = 5;
        }
        break;

      default:
	Unpacked[0] = SUB_BYTE(ie[2], 5, 2);	/* Coding standard */
	Unpacked[1] = SUB_BYTE(ie[2], 0, 4);	/* Location */
	Unpacked[2] = (ie[3] & 0x7F);	/* Cause value */
	i = 3;			/* Set input and output indices */
	j = 4;
	break;
    }

    /*
     * Unpack the diagnostic
     */
    while ((i < MAX_NUMB_INFO_FIELDS) && (j <= (ie[1] + 1))) {
	Unpacked[i++] = ie[j++];
    }

    if (i == MAX_NUMB_INFO_FIELDS)
	Unpacked[i - 1] = 0;	/* Zero terminate at the last element */
    else
	Unpacked[i] = 0;	/* Zero terminate at the last element */

    switch (SwitchType) {
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	*struct_err = 0;	/* === */
	if ((ie[2] & 0x10) || (ie[2] & 0x60) == 0x20 ||	/* === */
	    (ie[2] & 0x60) == 0x40 ||	/* === */
	    (ie[2] & 0x60) == 0x60	/* === */
	    ) {
	    *struct_err = INVALID_ELEM_CONTENTS;	/* === */
	    break;		/* === */
	}			/* === */
	/*
	 * Check if the cause value may have a diagnostic
	 */
	/* substituted my test on length for theirs 		   === */
	for (vn2_octets = 0, i = 2; i < ie[1] + 2; i++)
	    if ((ie[i] & 0x80) || vn2_octets >= 2)	/* count non-extended or
							 * diagnostic fields */
		vn2_octets++;
	for (i = 0; diagopt[i]; i++)	/* === */
	    if (diagopt[i] >= Unpacked[2])	/* === */
		break;		/* === */
	if (Unpacked[2] == diagopt[i]) {	/* optional?	   === */
	    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
		if (vn2_octets < 2)
		    *struct_err = MANDATORY_IE_LEN_ERR;	/* === */
	    } else {
		if (ie[1] < 2 || ie[1] > 3)	/* === */
		    *struct_err = MANDATORY_IE_LEN_ERR;	/* === */
	    }
	} else {		/* === */
	    /* here diagnostic is either required or forbidden */
	    for (i = 0; diagreq[i]; i++)	/* === */
		if (diagreq[i] >= Unpacked[2])	/* === */
		    break;	/* === */
	    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
		if ((diagreq[i] == Unpacked[2] && vn2_octets < 3) ||	/* === */
		    (diagreq[i] != Unpacked[2] && vn2_octets != 2))	/* === */
		    *struct_err = MANDATORY_IE_LEN_ERR;	/* === */
	    } else {
		if ((diagreq[i] == Unpacked[2] && ie[1] != 3) ||	/* === */
		    (diagreq[i] != Unpacked[2] && ie[1] != 2))	/* === */
		    *struct_err = MANDATORY_IE_LEN_ERR;	/* === */
	    }
	}			/* === */
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:	/* === */
	/*
	 * Test the extension bits for intergrity.
	 */
	*struct_err = (((ie[2] & 0x80) != 0x80) ||
		       ((ie[3] & 0x80) != 0x80));
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	/*
	 * Test the extension bits for intergrity.
	 */
	*struct_err =
	    ((ie[1] < 2) ||	/* Elem too short */
	     (ie[2] < 0x80) ||	/* Missing ext bit */
             (( ie[3] >= 0x80 ) && ( ie[1] > 2 )) ||  /* Ext bit error */
	     ((ie[3] < 0x80) && ((ie[1] - 2) != ie[4])));	/* Len */
	break;
#endif
      case PRI_NET5_STYPE:
        /*
         * Test the extension bits for intergrity.
         * but first see if have a length of 2 or 3.
         */
        if (ie[1] == 3) {
            /*
             * Got a length of 3, includes optional octet 3a,
             * allow it without checking.
             */
            *struct_err = FALSE;
        } else {
            *struct_err =
                ((ie[1] < 2) ||     /* Elem too short */
                 (ie[2] < 0x80) ||  /* Missing ext bit */
                 ((ie[3] <= 0x80) && (ie[1] >= 2)) ||  
                 ((ie[3] < 0x80) && ((ie[1] - 2) != ie[4])));       /* Len */
        }
        break;
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	*struct_err = ((ie[1] != 2) & (ie[1] != 1));
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	*struct_err = ((ie[1] != 2) & (ie[1] != 1) & (ie[1] != 0));
	break;
#endif
      default:
	/*
	 * This parameter is not yet supported for default SwitchTypes
	 */
	*struct_err = FALSE;
	break;
    }

    return (Unpacked);
}


/***********************
* CCIE_ExpandChanId_IE *
************************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
#ifdef PRI
* NOTE: The Telementworks implementation currently supports only H0
*       channel assignments in contiguous groupings of B-channels as
*       1-6, 7-12 and 13-18, and does not support 19-24 since it does
*       not suport Non-facility associated signaling.  The designation
*       of H0 channels in Non-contiguous groupings is specifically NOT
*       implemented.
*
#endif
*  Upon completion, the IE is unpacked as follows:
*
*  Unpacked[ 0 ]    -  Interface ID present from octet 3
*  Unpacked[ 1 ]    -  Interface type from octet 3
*  Unpacked[ 2 ]    -  Preferred/exclusive from octet 3
*  Unpacked[ 3 ]    -  D-channel indicator from octet 3
*  Unpacked[ 4 ]    -  Info channel selection from octet 3
*  ************* End of unpacking for Basic Rate Interface *****************
*  Unpacked[ 5 ]    -  Coding standard from octet 3.2
*  Unpacked[ 6 ]    -  Number/map from octet 3.2
*  Unpacked[ 7 ]    -  Channel type from octet 3.2
*  Unpacked[ 8 ]    -  Offset in the Unpacked buffer for the interface ID--
*  Unpacked[ 9 ]    -  Channel number                                      |
*  .                -  Channel number                                      |
*  .                -  Channel number                                      |
*  Unpacked[ * ]    -  Channel number                                      |
*  Unpacked[ *+1 ]  -  0                                                   |
*  Unpacked[ *+2 ]  -  Interface ID if present (octets 3.1, bits 7-1) <----
*  .                -  Interface ID if present (octets 3.1, bits 7-1)
*  .                -  Interface ID if present (octets 3.1, bits 7-1)
*  .                -  Interface ID if present (octets 3.1, bits 7-1)
*  Unpacked[ ** ]   -  0
*
*/
PUBLIC BYTE *CCIE_ExpandChanId_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */

{
#ifdef PRI
    int in_index;		/* Index to IE. */
    int out_index;		/* Index to Unpacked buffer. */
    int interface_id_len = 0;	/* Interface ID length */
    int chan;			/* A b-channel number */
    int i, j;			/* Counters */
    int slots_octet;		/* Info elem octet index */
    int int_id_len = 0;		/* Interface ID length */
#endif

    Unpacked[0] = SUB_BYTE(ie[2], 6, 1);	/* Int ID field */
    Unpacked[1] = SUB_BYTE(ie[2], 5, 1);	/* Int type field */
    Unpacked[2] = SUB_BYTE(ie[2], 3, 1);	/* Pref/exc field */
    Unpacked[3] = SUB_BYTE(ie[2], 2, 1);	/* D-ch ind */
    Unpacked[4] = SUB_BYTE(ie[2], 0, 2);	/* Chan sel field */

#ifdef PRI			/* have to fix this */
    /*
     * The Layer 3 timers were getting clobbered due to having to check the Channel
     * IE in state 1 in the incoming Call Proceeding message. This will have to be
     * fixed correctly, but for now we can still be either BRI or PRI. Found on the
     * 4000 box during italy homologation.
     */
    if ((Unpacked[1]) && is_pri) {    /* Primary rate interface */
	in_index = 3;
	/*
	 * Count the interface ID octets 3.1, 3.2, etc.
	 * NET5 and TS014 do not support the interface ie (3.1)
	 */
	if ((Unpacked[0]) &&
            (SwitchType != PRI_NET5_STYPE) && (SwitchType != PRI_TS014_STYPE)) {
	    /* Is the interface explicitly identified? */
	    /*
	     * Skip over the interface ID using ext bit and accounting for
	     * the interface ID length.
	     */
	    while (ie[in_index++] < 0x80) {
	    };
	    interface_id_len = in_index - 3;	/* Save interface ID length */
	}
#ifdef PRI_DMS100
	int_id_len = interface_id_len;	/* Saved interface ID length */
#endif
#ifdef PRI_4ESS
	/*
	 * Test if octets beyond the interface ID are provided.  If not we
	 * will assume this is an H11 channel.
	 */
	if ((SwitchType == PRI_4ESS_STYPE) && ((ie[1] - interface_id_len - 1) == 0)) {
	    /*
	     * We will put a few "safe" values here for the presumed H11
	     * channel case since these values are expected and checked in
	     * CC.
	     */
	    Unpacked[5] = CCITT_STANDARD;	/* Coding std */
	    Unpacked[6] = CHANNEL_NUMBERS;	/* Number/map */
	    Unpacked[7] = H11_CHANNEL_UNITS;	/* type */
	    out_index = 9;	/* Put the interface ID starting here */
	} else {
#endif
	    switch (SwitchType) {
#ifdef PRI_1TR6
	      case PRI_1TR6_STYPE:
		Unpacked[5] = 0;/* not supported in 1TR6 */
		Unpacked[6] = 0;/* not supported in 1TR6 */
		Unpacked[7] = 0;/* not supported in 1TR6 */
		break;
#endif
	      default:
		Unpacked[5] = SUB_BYTE(ie[in_index], 5, 2);	/* Coding std */
		Unpacked[6] = SUB_BYTE(ie[in_index], 4, 1);	/* Number/map */
		Unpacked[7] = SUB_BYTE(ie[in_index], 0, 4);	/* Channel type */
		break;
	    }
	    in_index++;

	    /*
	     * Now unpack the channel numbers. Establish start index to out
	     * chan list.  Unpacked[ 8 ] is reserved for storing the index to
	     * the unpacked Interface ID octets (3a for AT&T or for DMS100)
	     * to be reconsidered later.
	     */
	    out_index = 9;    /* The channel list will begin at Unpacked[9*] */

	    switch (SwitchType) {	/* Unpack the channels */
#ifdef KDD_NTT
	      case KDD_STYPE:
	      case NTT_STYPE:
		if (Unpacked[6]) {	/* Channels are designated by slot
					 * map? */
		    /*
		     * In this case we will convert the bit map into an
		     * ascending order channel list which is NULL-terminated.
		     * Thus the expanded version of the IE will look the same
		     * as had the channels been listed in the first place.
		     */
		    if (Unpacked[7] == H0_CHANNEL_UNITS) {
			/*
			 * Channel slot map is a single octet H0 channel bit
			 * map.
			 */
			chan = ((ie[in_index] - 1) * 6) + 1;
			for (i = chan; i < chan + 6; i++)
			    Unpacked[out_index++] = i;
		    } else {	/* Channel(s) is/are slot-mapped B-channel(s) */
			chan = 1;
			for (i = in_index + 2; i >= in_index; i--) {
			    slots_octet = ie[i];
			    for (j = 1; j <= 8; j++) {
				if (slots_octet & 0x01)
				    Unpacked[out_index++] = chan;
				slots_octet = (slots_octet >> 1);
				chan++;	/* Next chan */
			    }
			}
		    }
		} else {	/* Chan numbers are listed (not mapped) */
		    do {
			Unpacked[out_index++] = SUB_BYTE(ie[in_index], 0, 7);
		    } while (ie[in_index++] < 0x80);	/* Last channel? */
		}
		if (out_index == 15) {
		    /*
		     * Six B-channels (an H0 channel) were specified
		     */
		    Unpacked[7] = H0_CHANNEL_UNITS;
		}
		break;
#endif				/* KDD_NTT */
#ifdef PRI_1TR6
	      case PRI_1TR6_STYPE:
		Unpacked[out_index++] = SUB_BYTE(ie[in_index++], 0, 7);
		break;
#endif				/* PRI_1TR6 */
	      case PRI_NET5_STYPE:
               /*
                * Note that for H11, the interface ID and the channel type
                * completely specify the channels involved. Added for Italy
                * homologation.
                */
                if (Unpacked[7] != H11_CHANNEL_UNITS) {
                    if (Unpacked[6]) {  /* Is channel ID by slot map? */
                        /*
                         * In this case we will convert the bit map into an
                         * ascending order channel list which is
                         * NULL-terminated. Thus the expanded version of the
                         * IE will look the same as had the channels been
                         * listed in the first place.
                         */
                        chan = 1;
                        for (i = in_index + 2; i >= in_index; i--) {
                            slots_octet = ie[i];
                            for (j = 1; j <= 8; j++) {
                                if (slots_octet & 0x01)
                                    Unpacked[out_index++] = chan;
                                slots_octet = (slots_octet >> 1);
                                chan++; /* Next chan */
                            }
                        }
                    } else {    /* Chan numbers are listed (not mapped) */
                        /*
                         * The channel octet may or may not have the extend bit
                         * set for the channel, so B-1 could be 0x81 or 0x01,
                         * so have to check the length and make sure don't go
                         * past the end of the Channel IE. For Italy homologation.
                         */ 
                        do {
                            /*
                             * Put in this safety check for the end of the ie
                             * otherwise won't stop checking.
                             */
                            if (in_index > ie[1] + 1) {
                                break;
                            }
                            Unpacked[out_index++] = SUB_BYTE(ie[in_index], 0, 7);
                        } while (in_index++);  /* but Last chan? */
                    }
                }
                break;

	      default:
		/*
		 * Note that for H11, the interface ID and the channel type
		 * completely specify the channels involved.
		 */
		if (Unpacked[7] != H11_CHANNEL_UNITS) {
		    if (Unpacked[6]) {	/* Is channel ID by slot map? */
			/*
			 * In this case we will convert the bit map into an
			 * ascending order channel list which is
			 * NULL-terminated. Thus the expanded version of the
			 * IE will look the same as had the channels been
			 * listed in the first place.
			 */
			chan = 1;
			for (i = in_index + 2; i >= in_index; i--) {
			    slots_octet = ie[i];
			    for (j = 1; j <= 8; j++) {
				if (slots_octet & 0x01)
				    Unpacked[out_index++] = chan;
				slots_octet = (slots_octet >> 1);
				chan++;	/* Next chan */
			    }
			}
		    } else {	/* Chan numbers are listed (not mapped) */
			do {
			    /*
			     * Put in this safety check for the end of the ie
			     * otherwise won't stop checking.
			     */
			    if (in_index > ie[1] + 1)
				break;
			    Unpacked[out_index++] = SUB_BYTE(ie[in_index], 0, 7);
			} while (ie[in_index++] < 0x80);	/* Last chan? */
		    }
		}
		break;
	    }			/* End switch on SwitchType */
	    Unpacked[out_index++] = 0;	/* Zero terminate the chan list */
#ifdef PRI_4ESS
	}
#endif
	/*
	 * Now consider the interface identifier again. First write the index
	 * to the start of the interface ID in the Unpacked buffer at
	 * Unpacked[ 8 ].
	 */
	if (interface_id_len) {
	    Unpacked[8] = out_index;
	    /*
	     * Now move the interface ID bytes into the Unpacked buffer.
	     */
	    in_index = 3;	/* Start index of interface ID */
	    while (interface_id_len--)
		Unpacked[out_index++] = SUB_BYTE(ie[in_index++], 0, 7);
	    Unpacked[out_index] = 0;	/* Zero terminate interface ID list */
	} else {
	    Unpacked[8] = 0;	/* No interface identifier */
	}
    }
#endif				/* PRI */

    switch (SwitchType) {	/* Test structural errors */
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:	/* === */
	/*
	 * Test the extension bit and ie length for intergrity.
	 */
	*struct_err = ((ie[1] != 1) || ((ie[2] & 0x80) != 0x80));
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	/*
	 * Test the extension bit and ie length for intergrity.
	 */
	if (int_id_len)
	    *struct_err = (((ie[1] - int_id_len) != 3) ||
			   (ie[2] < 0x80) ||
			   (ie[3] < 0x80) ||
			   (ie[4] < 0x80) ||
			   ((ie[5] & 0x7F) > 30));
	else
	    *struct_err = ((ie[1] != 3) ||
			   (ie[2] < 0x80) ||
			   (ie[3] < 0x80) ||
			   ((ie[4] & 0x7F) > 24));
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	*struct_err = ((ie[1] != 3) ||
		       ((ie[2] & 0xF4) != 0xA0) ||
		       (ie[3] != 0xC3));
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:	/* ===, may need to expand as below */
	*struct_err = ((ie[1] != 1) || ((ie[2] & 0x80) != 0x80));
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
#ifdef BRI			/* ===??? */
	/*
	 * ===   *struct_err = ( ie[ 1 ] != 1 );     * Length * error
	 */
	/* The previous code was not complete enough              === */
	if (ie[1] == 0)
	    *struct_err = MANDATORY_IE_LEN_ERR;	/* === */
	else if (ie[2] & 0x64)	/* === */
	    *struct_err = INVALID_ELEM_CONTENTS;	/* === */
	else {			/* === */
	    /* test changed *//* === */
	    int i;		/* === */
	    /* === */
	    for (i = 2; i < ie[1] && !(ie[i] & 0x80); i++)	/* === */
		;		/* === */
	    if (++i < ie[1])	/* === */
		*struct_err = MANDATORY_IE_LEN_ERR;	/* === */
	    else		/* === */
		*struct_err = 0;/* === */
	}
#else				/* PRI */
	*struct_err = (ie[1] != 3);	/* Length error */
#endif
	break;
#endif
      default:
	/*
	 * This parameter is not yet supported for default SwitchTypes
	 */
	*struct_err = FALSE;
	break;
    }

    return (Unpacked);
}


/*****************************
* CCIE_ExpandChangeStatus_IE *
******************************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
*/
PUBLIC BYTE *CCIE_ExpandChangeStatus_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */

{
    Unpacked[0] = SUB_BYTE(ie[2], 6, 1);	/* Channel vs Interface */
    Unpacked[1] = SUB_BYTE(ie[2], 0, 3);	/* New status */

    switch (SwitchType) {
      default:
	/*
	 * This parameter is not yet supported for default SwitchTypes
	 */
	*struct_err = FALSE;
	break;
    }

    return (Unpacked);
}


/***********************
* CCIE_ExpandNetFac_IE *
************************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
* It is regretable that CCITT has made this element so complex that
* even in its unpacked format, considerable effort will be required
* to fathom it.
*
*/
PUBLIC BYTE *CCIE_ExpandNetFac_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */

{
    int i;			/* Unpacked bytes counter */
    int j;			/* IE bytes counter */
#ifdef PRI_4ESS
    int id_octet;		/* A counter */
    BYTE parameterized;		/* Parameterized facility */
#endif

    switch (SwitchType) {
#ifdef PRI_4ESS
      case PRI_4ESS_STYPE:
	i = 0;			/* Unpacked bytes counter */
	j = 2;			/* IE bytes counter */
	if ((Unpacked[i++] = ie[j++]) != 0) {	/* ID length */
	    Unpacked[i++] = SUB_BYTE(ie[j], 4, 3);	/* ID type */
	    Unpacked[i++] = SUB_BYTE(ie[j++], 0, 4);	/* ID plan */

	    /*
	     * Unpack the Network Identification
	     */
	    for (id_octet = 1; id_octet < Unpacked[0]; id_octet++)
		Unpacked[i++] = ie[j++];
	}
	/*
	 * Now read the Parameterized/Binary bit, saving it for later study.
	 */
	parameterized = Unpacked[i++] = SUB_BYTE(ie[j], 7, 1);

	Unpacked[i++] = SUB_BYTE(ie[j], 6, 1);	/* Expansion bit */
	Unpacked[i++] = SUB_BYTE(ie[j], 5, 1);	/* Feat/Service */
	Unpacked[i++] = SUB_BYTE(ie[j++], 0, 5);	/* Fac coding */

	if (parameterized) {
	    while (j < (ie[1] + IE_HEAD_LENGTH))
		/*
		 * Unpack parameterized field.
		 */
		Unpacked[i++] = ie[j++];
	}
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Unpacked[0] = ie[1];	/* Preserve the ie length */
	i = 1;
	j = 3;
	while (j < (ie[1] + IE_HEAD_LENGTH))
	    Unpacked[i++] = ie[j++];	/* Copy the Facility Spec. */
	break;
#endif
      default:
	PtrTrash = ie;		/* Prevent a compiler warning */
	break;			/* No other scheme is currently supported */
    }

    switch (SwitchType) {
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
#ifdef PRI
	*struct_err = ie[1] > 22;	/* See REGISTER message */
#endif
	break;
#endif
      default:
	/*
	 * This parameter is not yet supported for default SwitchTypes
	 */
	*struct_err = FALSE;
	break;
    }

    return (Unpacked);
}

/**************************
* CCIE_ExpandNotifyInd_IE *
***************************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
*/
PUBLIC BYTE *CCIE_ExpandNotifyInd_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */

{
    Unpacked[0] = SUB_BYTE(ie[2], 0, 7);	/* Notify description */

    switch (SwitchType) {
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	/*
	 * Check the IE length and the extension bit on octet 3.
	 */
	*struct_err = ((ie[1] != 1) || (SUB_BYTE(ie[2], 7, 1) != 1));
	break;
#endif
      default:
	/*
	 * This parameter is not yet supported for default SwitchTypes
	 */
	*struct_err = FALSE;
	break;
    }

    return (Unpacked);
}

/**************************
* CCIE_ExpandPartyNumb_IE *
***************************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.  Zero terminate
* the digit string.  This function, in the standard Unpacked buffer,
* accomodated digit strings = MAX_NUMB_INFO_FIELDS - 4.
*
* This function may be called for the CALLED_NUMBER_IE, CALLED_SUB_ADDR_IE,
* CALLER_NUMBER_IE or CALLER_SUB_ADDR_IE.
*/

PUBLIC BYTE *CCIE_ExpandPartyNumb_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */

{
    BYTE *ie_list;		/* Internal ie pointer */
    BYTE *out_list;		/* Internal Unpacked buffer pointer */
    int i;			/* === */

    /*
     * check for a length error for Australia switchtype
     */
    if (ie[1] >= 127) {
	*struct_err = MANDATORY_IE_LEN_ERR;
	return (Unpacked);
    }
    *struct_err = FALSE;	/* Initially */
    Unpacked[0] = SUB_BYTE(ie[2], 4, 3);	/* Type of addr */
    Unpacked[1] = SUB_BYTE(ie[2], 0, 4);	/* Numbering plan */

    /*
     * Octet 3 extended only exists for Caller numbers.
     */

    if (((ie[2] & 0x80) == 0) &&
	(ie[0] != CALLED_NUMBER_IE)) {	/* Octet 3 extended? */
	/*
	 * octet 3a is included - unpack it:
	 */
	Unpacked[2] = SUB_BYTE(ie[3], 5, 2);	/* presentation ind. */
	Unpacked[3] = SUB_BYTE(ie[3], 0, 2);	/* scr indication */
	ie_list = &ie[4];
    } else {
	/*
	 * octet 3a not included - these values will indicate that octet 3a
	 * was not present.
	 */
	Unpacked[2] = 0xFF;
	Unpacked[3] = 0xFF;
	ie_list = &ie[3];
    }

    /*
     * Note that in SUB_ADDR IE's, octet 4 is not a sub-address digit, but
     * rather a digit format descriptor.
     */
    out_list = &Unpacked[4];
    while (ie_list < (ie + ie[1] + IE_HEAD_LENGTH)) {
	/*
	 * Check for ext bit coding errors as the digits are recovered.
	 */
	*struct_err = ((*struct_err) || ((*ie_list & 0x80) == 0x80));
	switch (SwitchType) {
#ifdef PRI_1TR6
	    /*
	     * Check to make sure these are valid digits
	     */
	  case PRI_1TR6_STYPE:
	    *struct_err = (((*struct_err) ||
		(((*ie_list >= '0') && (*ie_list <= '9')) ? FALSE : TRUE)));
	    *out_list++ = *ie_list++;	/* Telephone number digits  */
	    break;
#endif
	  default:
	    *out_list++ = *ie_list++;	/* Telephone number digits */
	    break;
	}
    }
    *out_list = 0;		/* Zero terminate at the last element */

    switch (SwitchType) {
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:	/* === */
	/*
	 * Test the extension bits for intergrity.
	 */
	if (Unpacked[2] == 0xFF)/* Octet 3a not included */
	    *struct_err = ((*struct_err) || ((ie[2] & 0x80) != 0x80));
	else {
	    if ((ie[3] & 0x80) != 0x80) {	/* if there is an error in
						 * octet 3a */
		/* just ignore it */
		Unpacked[2] = 0xFF;
		Unpacked[3] = 0xFF;
	    }
	}
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	/*
	 * Test the extension bits for intergrity.
	 */
	if (Unpacked[2] == 0xFF)/* Octet 3a not included */
	    *struct_err = ((*struct_err) || ((ie[2] & 0x80) != 0x80));
	else
	    *struct_err = ((*struct_err) || ((ie[3] & 0x80) != 0x80));
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	/*
	 * Test the extension bits for intergrity.
	 */
	*struct_err = ((*struct_err) || ((ie[2] & 0x80) != 0x80));
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	if ((ie[0] == CALLED_SUB_ADDR_IE) ||
	    (ie[0] == CALLED_SUB_ADDR_IE))
	    *struct_err = ((*struct_err) || (ie[1] > MAX_SUB_ADDR_LEN));
	break;
#endif
#ifdef VN2
      case VN2_STYPE:		/* Check length errors */
      case VN3_STYPE:		/* Check length errors */
	if (ie[0] == CALLER_NUMBER_IE) {
	    if ((ie[1] < 2) || (ie[1] > 20))
		*struct_err = MANDATORY_IE_LEN_ERR;	/* === */
	} else if (ie[0] == CALLED_NUMBER_IE) {
	    if ((ie[1] < 2) || (ie[1] > 19))
		*struct_err = MANDATORY_IE_LEN_ERR;	/* === */
	} else {		/* CALLER_SUB_ADDR_IE or CALLED_SUB_ADDR_IE */
	    if ((ie[1] < 2) || (ie[1] > 6))
		*struct_err = MANDATORY_IE_LEN_ERR;	/* === */
	}
	if (*struct_err)	/* === */
	    break;		/* === */
	if (ie[0] == CALLER_NUMBER_IE) {
	    if ((ie[2] & 0x70) > 2 ||	/* === */
		((ie[2] & 0x0f) > 2 && (ie[2] & 0x0f) != 9) ||	/* === */
		(!(ie[2] & 0x80) &&	/* === */
		 ((ie[3] & 0x7c) || (ie[3] & 0x3) == 2 ||	/* === */
		  (ie[3] & 0x3) == 3))) {	/* === */
		*struct_err = INVALID_ELEM_CONTENTS;	/* === */
		break;		/* === */
	    }			/* === */
	    i = (ie[2] & 0x80) ? 3 : 4;	/* === */
	} else if (ie[0] == CALLED_NUMBER_IE) {	/* === */
	    if ((ie[2] & 0x70) != 0 || (ie[2] & 0xf) != 0) {	/* === */
		*struct_err = INVALID_ELEM_CONTENTS;	/* === */
		break;		/* === */
	    }			/* === */
	    i = 3;		/* === */
	} else {		/* === */
	    /* A Sub-Address *//* === */
	    if (((ie[2] & 0x70) != 0 && (ie[2] & 0x70) != 0x20) ||	/* === */
		(ie[2] & 0xf) != 0) {	/* === */
		*struct_err = INVALID_ELEM_CONTENTS;	/* === */
		break;		/* === */
	    }			/* === */
	    i = 3;		/* === */
	}			/* === */
	for (; i < ie[1] + 2; i++) {	/* === */
	    if (ie[i] & 0x80) {	/* === */
		*struct_err = INVALID_ELEM_CONTENTS;	/* === */
		break;		/* === */
	    }			/* === */
	}			/* === */
	break;
#endif
      case BRI_TS013_STYPE:	/* Check length errors */
      case PRI_TS014_STYPE:
	/*
	 * ie[0] = octet 1 ie[1] = octet 2 ie[2] = octet 3 ie[3] = octet 3a
	 */
	if (ie[0] == CALLER_NUMBER_IE) {
	    if ((ie[1] < 2) || (ie[1] > 24))
		*struct_err = MANDATORY_IE_LEN_ERR;
	} else if (ie[0] == CALLED_NUMBER_IE) {
	    if ((ie[1] < 2) || (ie[1] > 23))
		*struct_err = MANDATORY_IE_LEN_ERR;
	} else {
	    /* CALLER_SUB_ADDR_IE or CALLED_SUB_ADDR_IE */
	    if ((ie[1] < 2) || (ie[1] > 23))
		*struct_err = MANDATORY_IE_LEN_ERR;
	}

	if (*struct_err)
	    break;

	if ((ie[0] != CALLER_NUMBER_IE) && (ie[0] != CALLED_NUMBER_IE)) {
	    /* A Sub-Address, check the type, octet 3 */
	    switch (ie[2] & 0x7f) {
	      case 0x00:	/* NSAP, even */
	      case 0x08:	/* NSAP, odd */
	      case 0x20:	/* user, even */
	      case 0x28:	/* user, odd */
		break;
	      default:
		*struct_err = INVALID_ELEM_CONTENTS;
		break;
	    }
	    i = 3;
	    for (; i < ie[1] + 2; i++) {
		if (ie[i] & 0x80) {
		    *struct_err = INVALID_ELEM_CONTENTS;
		    break;
		}
	    }
	}
	break;

      default:
	/*
	 * This parameter is not yet supported for default SwitchTypes
	 */
	*struct_err = FALSE;
	break;
    }

    return (Unpacked);
}


/****************************
* CCIE_ExpandProgressInd_IE *
*****************************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
*/
PUBLIC BYTE *CCIE_ExpandProgressInd_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */

{
    int i;			/* === */

    *struct_err = FALSE;

    Unpacked[0] = SUB_BYTE(ie[2], 5, 2);	/* Recover coding std */
    Unpacked[1] = SUB_BYTE(ie[2], 0, 4);	/* Recover location */
    Unpacked[2] = SUB_BYTE(ie[3], 0, 7);	/* Recover progress desc */

    switch (SwitchType) {
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:	/* === */
	/*
	 * Test the extension bits and ie length for intergrity.
	 */
	*struct_err = (((ie[2] & 0x80) != 0x80) ||
		       ((ie[3] & 0x80) != 0x80) ||
		       (ie[1] != 2));
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	/* all tests changed (ie., added) === */
	Unpacked[1] = 0xFF;	/* Location is not significant.  Rig it */
	if (ie[1] != 2) {	/* Length error */
	    *struct_err = MANDATORY_IE_LEN_ERR;
	    return Unpacked;
	}
	if ((ie[2] & 0x60) == 0x20 || (ie[2] & 0x60) == 0x40 ||
	    (ie[2] & 0x60) == 0x60 || (ie[2] & 0x10)) {
	    *struct_err = INVALID_ELEM_CONTENTS;
	    return Unpacked;
	}
	switch (ie[2] & 0xf) {
	    /*
	     * Tests ISEt-pi{5,8,9,11,12} disallow the values 1, 4, 5, 7 and
	     * 0xA respectively.  These seem like OK values to me.
	     */
	  case 0x0:
	    /* indicates User, which is wrong from the network */
	    goto vn2_bad_location;	/* ISEt-pi4 &&& */

	  case 0x1:
	  case 0x4:
	  case 0x5:
	  case 0x7:
	  case 0xa:
	    break;

	  default:
    vn2_bad_location:
	    *struct_err = INVALID_ELEM_CONTENTS;
	    return Unpacked;
	}
	for (i = 2; !(ie[i] & 0x80); i++);
	switch (ie[++i] & 0x7f) {
	  case 1:
	  case 3:
	    break;

	  case 2:
	  case 4:
	  case 8:
	    if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
		break;
	    /* fall through... */
	  default:
	    *struct_err = INVALID_ELEM_CONTENTS;
	    return Unpacked;
	}
	break;
#endif
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	/* all tests changed (ie., added) === */
	/* Unpacked[ 1 ] = 0xFF; === Location is not significant. */
	if (ie[1] != 2) {	/* Length error */
	    *struct_err = MANDATORY_IE_LEN_ERR;
	    return Unpacked;
	}
	/*
	 * coding standard, spare (bits 567 = 0)
	 */
	switch (ie[2] & 0x60) {
	    /* coding standard */
	  case 0x00:		/* CCITT standardized */
	    break;
	  default:
	    *struct_err = INVALID_ELEM_CONTENTS;
	    return Unpacked;
	}
	switch (ie[2] & 0x0f) {
	    /* this is the location */
	  case 0x00:		/* user */
	  case 0x01:		/* privite net serving local user */
	  case 0x02:		/* public net serving local user  */
	  case 0x04:		/* public net serving remote user */
	    break;
	  default:
	    *struct_err = INVALID_ELEM_CONTENTS;
	    return Unpacked;
	}
	for (i = 2; !(ie[i] & 0x80); i++);
	switch (ie[++i] & 0x7f) {
	  case 0x01:		/* call not end to end isdn */
	  case 0x02:		/* dest is not ISDN */
	  case 0x03:		/* orig is not ISDN */
	  case 0x08:		/* in band info available */
	    break;
	  default:
	    *struct_err = INVALID_ELEM_CONTENTS;
	    return Unpacked;
	}
	break;

      default:
	/*
	 * This parameter is not yet supported for default SwitchTypes
	 */
	*struct_err = FALSE;
	break;
    }

    return (Unpacked);
}


/***********************
* CCIE_ExpandSignal_IE *
************************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
*/
PUBLIC BYTE *CCIE_ExpandSignal_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */

{
    Unpacked[0] = ie[2];	/* Recover signal value */

    switch (SwitchType) {
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:	/* === */
	/*
	 * Test the ie length for intergrity.
	 */
	*struct_err = (ie[1] != 1);
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	*struct_err = (ie[1] != 1);	/* Length error */
	break;
#endif
      default:
	/*
	 * This parameter is not yet supported for default SwitchTypes
	 */
	*struct_err = FALSE;
	break;
    }

    return (Unpacked);
}


/*********************
* CCIE_ExpandUser_IE *
**********************
*
* Recover all the variable sub-byte fields in the information element
* and return them in a byte array, one byte per field.
*
* The value of MAX_UUI_LEN is defined in CCIE.H for the USER_USER_IE and
* may require adjustment depending upon the network.  The default value
* is 131.
*
*/
PUBLIC BYTE *CCIE_ExpandUser_IE(ie, struct_err)
BYTE *ie;			/* Pointer to IE to unpack */
BOOLEAN *struct_err;		/* Pointer to error BOOLEAN */

{
    int uui_to_move;
    BYTE *out;

    ie++;			/* Point ie at length octet (2) */
    uui_to_move = *ie - 1;	/* Establish UUI bytes to move */
    Unpacked[0] = *ie++;	/* Save Length in Unpacked[ 0 ] */
    Unpacked[1] = *ie++;	/* Protocol discriminator in Unpacked[ 1 ] */
    switch (SwitchType) {
#ifdef PRI_5ESS
      case PRI_5ESS_STYPE:
	*struct_err = (Unpacked[0] > 62);
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	if (ie[0] == USER_USER_IE)
	    /*
	     * struct_err = (( Unpacked[ 0 ] > 129 ) || ( Unpacked[ 0 ] < 0
	     * )); === gcc warning
	     */
	    *struct_err = (Unpacked[0] > 129);	/* === */
	else			/* USER_USER_FAC_IE */
	    /*
	     * struct_err = (( Unpacked[ 0 ] > 8 ) || ( Unpacked[ 0 ] < 0 ));
	     * === gcc warning
	     */
	    *struct_err = (Unpacked[0] > 8);	/* === */
	break;
#endif
      default:
	*struct_err = (Unpacked[0] > MAX_UUI_LEN);
	break;
    }

    if (!*struct_err) {
	out = &Unpacked[2];
	while (uui_to_move--)
	    *out++ = *ie++;	/* Move UUI */
    }
    return (Unpacked);
}


/************************************************************************
*         C C I E   M I S C E L L A N E O U S   F U N C T I O N S       *
*************************************************************************
*/
#ifdef BRI_5ESS
/****************
* CCIE_Get_OCRV *
*****************
*
* Unpacks the CODESET 6 Other Call Reference IE, returns Other Call
* Reference value as int.  Currently defined for 5ESS BRI only.
* Returns 0 if error.
*
*/
PUBLIC int CCIE_Get_OCRV(src)
BYTE *src;			/* Pointer to the beginning of OCRV IE */

{
    return (((src[0] == OTHER_CALL_REF_IE) && (src[1] == 0x01)) ? src[2] : 0);
}

#endif

/************************************************************************
*                C C I E   S E A R C H   F U N C T I O N S              *
*************************************************************************
*/

/**************
* CCIE_Get_IE *
***************
*
* Search a Q.931 message for a specific information element.
* Return a pointer to the element in the message which satisfies the
* search or return NULL.  The element must satisfy the ie_id, the occurrence
* number of the ie and the specified codeset.
*
* NOTE 1: The convention for bit numbering within a byte in this function
*         and its supporting functions is that the most significant
*         (leftmost) bit in a byte will be bit 7, the least significant,
*         bit 0.  This differs from the scheme found in CCITT Rec. Q.931
*         where the bits are numbered from 8 to 1 starting with the most
*         significant (leftmost) bit.
*
* NOTE 2: If a single-byte information element is to be located, this
*         routine assumes that the ie_id will be passed to this routine
*         in its packed form with bit 7 set and the element ID in the
*         bits 6-4.  Also, the contents of bits 3-0 are masked-off in
*         the search for all Type 1 single octet ie's.  For Type 2 ie's,
*         the entire byte is matched (see CCITT Rec. Q.931, Sec. 4.5.1).
*
* NOTE 3: The "occurrence" parameter is provided for two reasons:
*
*         1. To allow the calling function to detect multiple occurrences
*            of an element as an error (a message integrity check) by
*            searching for occurrence = 2.
*         2. To allow legitimate recovery of multiple occurrences of the
*            same element which may occur in the user-to-user context.
*
* NOTE 4: This routine returns search failure (NULL) on searches for
*         variable-length IEs when the IE is found out of ascending
*         numerical sequence within its codeset in accordance with the
*         criteria specified in CCITT Rec. Q.931, Section 5.8.5.1.
*
* Return a pointer to the matched element within the message or NULL if
* no match was possible.
*/
PUBLIC BYTE *CCIE_Get_IE(l3_msg, msg_len, ie_id, codeset, occurrence)
BYTE *l3_msg;			/* The Q.931 message to examine */
int msg_len;			/* The length of the message in bytes */
BYTE ie_id;			/* Identity of the l3 information element
				 * sought */
int codeset;			/* Codeset where ie_id is to be located */
int occurrence;			/* Occurence number for this IE (1 or
				 * greater) */

{
    SEARCH_CONTROL Context;	/* Search control structure */
    BYTE *matched_element;	/* Pointer to found element */
    BYTE single_byte_id;	/* Location for writing single-byte element
				 * ID */
    BYTE dont_care;		/* A waste basket */

    Context.Base_Codeset = CODESET_0;	/* Initialize */
    Context.Active_Codeset = CODESET_0;	/* Initialize */
    Context.Previous_IE = 0;	/* Initialize */
    Context.Abort_Search = FALSE;	/* Initialize */
    Context.This_Occurrence = 1;/* Initialize */
    single_byte_id = 0;		/* Initialize */
    matched_element = NULL;	/* Initialize */


    /*
     * Test for a locking/non-locking shift as search target.
     */
    CCIE_Single_Byte_IE(ie_id, &single_byte_id, &dont_care);
    if (single_byte_id != CODESET_SHIFT) {
	/*
	 * The search will proceed.  Start by skipping the message header.
	 */
	Context.Current_Offset = CCIE_LenOf_L3_Msg_Header(l3_msg);

	/*
	 * Check each information element until matched or end of message.
	 */
	while (Context.Current_Offset < msg_len) {	/* End of message yet? */
	    if (CCIE_Match_IE_ID((struct IE_Header *) & l3_msg[Context.Current_Offset],
				 ie_id,
				 codeset,
				 occurrence,
				 &Context)) {
		matched_element = &l3_msg[Context.Current_Offset];
		break;		/* Break loop */
	    } else if (Context.Abort_Search) {
		break;		/* Break loop and fail */
	    }
	}
    }
    return (matched_element);
}


/****************
* CCIE_GetAnyIE *
*****************
*
* Search a Telenetworks/Host interface message for a specific information
* element.
*
* All the header comments under CCIE_Get_IE() are effective here except
* that this function does not expect a Q.931-type message headed by
* Protocol Discriminator, Call Reference, and Message Type.
*
* At the interface between Telenetworks ISDN Call Controls and application
* processors above, propritary bi-directional multi-byte message sets
* have been defined which bear similarity to the Layer 3 Q.931 message
* structure, but differ from it and each other.  This function is the entry
* point to a parallel IE search machine to that provided for Q.931 IE search
* by CCIE_Get_IE, but for these private elements within a private message.
*
* In order the deal with variant message header structures and to increase
* generality, the start search location provided by the calling function
* MUST be the start of the first info element where search is to begin,
* rather than the start of the message.  The length provided is the length
* of the message from the provided start point to the end of the section of
* infomation elements to be searched (commonly to the end of the message).
*
* The elements to be searched must conform to Q.931 Type 1 or Type 2 single
* octet element model or the the standard multi-octet model to the extent
* of having an ID byte, a length byte and some number of followup data
* bytes equal in number to the length.  No content is enforced except that
* the multibyte IE's must be in ascending numerical ie_id order within their
* codeset.
*
* This function returns a pointer to the element in the success case, or
* a NULL pointer on failure.
*/
PUBLIC BYTE *CCIE_GetAnyIE(msg_seg, seg_len, ie_id, codeset, occurrence)
BYTE *msg_seg;			/* The message segment to examine */
int seg_len;			/* The length of the message segment in bytes */
BYTE ie_id;			/* Identity of the information element sought */
int codeset;			/* Codeset specific to the element sought */
int occurrence;			/* Occurence of the IE (1 or greater) in its
				 * codeset */

{
    SEARCH_CONTROL Context;	/* Search control structure */
    BYTE *matched_element;	/* Pointer to found element */

    Context.Base_Codeset = CODESET_0;	/* Initialize */
    Context.Active_Codeset = CODESET_0;	/* Initialize */
    Context.Previous_IE = 0;	/* Initialize */
    Context.Abort_Search = FALSE;	/* Initialize */
    Context.This_Occurrence = 1;/* Initialize */
    matched_element = NULL;	/* Initialize */
    Context.Current_Offset = 0;	/* Initialize */


    /*
     * Check each information element until matched or end of message.
     */
    while (Context.Current_Offset < seg_len) {	/* End of message yet? */
	if (CCIE_Match_IE_ID((struct IE_Header *) & msg_seg[Context.Current_Offset],
			     ie_id,
			     codeset,
			     occurrence,
			     &Context)) {
	    matched_element = &msg_seg[Context.Current_Offset];
	    break;		/* Break loop */
	} else if (Context.Abort_Search) {
	    break;		/* Break loop and fail */
	}
    }
    if (Context.Current_Offset < seg_len)
	return (matched_element);
    else
	return (NULL);
}


/***************************
* CCIE_LenOf_L3_Msg_Header *
****************************
*
* Calculate and return the number of bytes in a Q.931 message header.
* The fields are:
*
* Protocol discriminator - 1 byte
* Call reference length  - 1 byte
* Call reference         - variable
* Message type           - 1 byte.
*
* Return the computed byte count.
*
*/
PUBLIC BYTE CCIE_LenOf_L3_Msg_Header(l3_msg)
BYTE *l3_msg;			/* Message to consider */

{
    return (FIXED_L3_MSG_HEADER_LEN + l3_msg[CALL_REF_LEN_OFFSET]);
}


/*******************
* CCIE_Match_IE_ID *
********************
*
* Test if the current information element ID in a Q.931 message
* matches the ID of the sought element ID in the correct codeset
* context.
*
* NOTE: This function specifically avoids altering the value of
*       Context->Current_Offset if a match is achieved as this
*       offset into the message will be used by the calling function
*       to recover the pointer to the matching element in the message.
*
* Return TRUE if the match is achieved.
*
*/

PRIVATE BOOLEAN CCIE_Match_IE_ID(ie_head, ie_id, codeset, occurrence,
				     Context)
struct IE_Header *ie_head;	/* Header to the current info element */
BYTE ie_id;			/* Element id being sought */
int codeset;			/* Codeset where ie_id is to be sought */
int occurrence;			/* Which occurrence of the element to match */
SEARCH_CONTROL *Context;	/* Search control structure */

{
    BOOLEAN success;		/* Search success indicator */
    BYTE srch_id;		/* Unpacked ID of sought single-byte element */
    BYTE single_byte_id;	/* Location for putting a single-byte elem id */
    BYTE single_byte_data;	/* Location for putting single-byte elem data */


    /*
     * Success ceases if the sought codeset and the Active_Codeset differ.
     */
    success = (codeset == Context->Active_Codeset);

    /*
     * Whether or not we are even in the correct codeset, the appearance of
     * shift elements within the message cannot be ignored.
     */
    if (CCIE_Single_Byte_IE(ie_head->ident,
			    &single_byte_id,
			    &single_byte_data)) {
	if (single_byte_id == CODESET_SHIFT) {	/* A codeset shift element */
	    /*
	     * Recover the new codeset from the ie data field.
	     */
	    Context->Active_Codeset = SUB_BYTE(single_byte_data,
					       SINGLE_BYTE_DATA_LOC,
					       CODESET_LEN);

	    /*
	     * Test for a locking shift codeset change.
	     */
	    if (!(single_byte_data & NON_LOCKING_SHIFT_MASK))
		/*
		 * Update Base_Codeset from Active_Codeset for locking shift.
		 */
	    {
		Context->Base_Codeset = Context->Active_Codeset;
		Context->Previous_IE = 0;	/* Reset IE ordering flag */
		/*
		 * Test if we have now advanced permanently to a codeset
		 * greater than the one where the element is to be found.
		 */
		Context->Abort_Search = (Context->Base_Codeset > codeset);
	    }
	    Context->Current_Offset++;	/* Point to next info element */
	    success = FALSE;	/* We do not locate this type */
	} else {
	    /*
	     * Test for an element ID match for other single-byte elements.
	     * This comparison also involves unpacking the element ID from
	     * the input byte to be matched and it assumes the input byte is
	     * packed.  First a test is made for Type 2 single byte elements
	     * and a comparison of the entire byte is made in these cases
	     * (see Q.931 Sec. 4.5.1.).
	     */
	    if (single_byte_id == SINGLE_BYTE_TYPE_2) {
		success = ((success) && (ie_id == ie_head->ident));
	    } else {		/* A Type 1 single-byte element */
		srch_id = SUB_BYTE(ie_id, SINGLE_BYTE_ID_LOC, SINGLE_BYTE_ID_LEN);
		success = ((success) &&
			   (single_byte_id == srch_id) &&
			   (ie_id & 0x80));
	    }

	    if (success)
		success = (Context->This_Occurrence++ == occurrence);

	    if (!success) {
		/*
		 * Restore from possible non-locking shift context.
		 */
		Context->Active_Codeset = Context->Base_Codeset;
		Context->Current_Offset++;	/* Point to next info element */
	    }
	}
    } else {			/* A multi-byte information element */
	/*
	 * First test if we have an out-of-sequence ie.
	 */
	if ((Context->Base_Codeset == Context->Active_Codeset) &&
	    (Context->Base_Codeset == codeset) &&
	    (Context->Previous_IE > ie_id)) {
	    success = FALSE;	/* IE sequence error or element missing */
	    Context->Abort_Search = TRUE;
	} else {
	    /*
	     * Test for ID match for a multi-byte element
	     */
	    success = ((success) &&
		       (ie_id == ie_head->ident) &&
		       (Context->This_Occurrence++ == occurrence));

	    if (!success) {	/* No match */
		if (Context->Active_Codeset == Context->Base_Codeset)
		    /*
		     * Save mismatched ie_id for later IE sequence error
		     * checking.
		     */
		    Context->Previous_IE = ie_head->ident;
		else
		    /*
		     * Restore from possible non-locking shift context.
		     */
		    Context->Active_Codeset = Context->Base_Codeset;

		/*
		 * Point to next element using element length field & header
		 * size.
		 */
		Context->Current_Offset += (ie_head->len + sizeof(*ie_head));
	    }
	}
    }
    return (success);
}


/***************
* CCIE_Pack_IE *
****************
*
* Write a bit-wise field into target ie byte starting at bit location
* (pos), (len) bits long, leaving the remainder of target byte as found.
* This routine assumes that the bits in a byte is numbered from 7-0.
* This routine is provided in skeletal form and is anticipated to
* evolve to a higher state of functional complexity.
*
* Return the target byte with the pack modification completed.
*
*/

PUBLIC BYTE CCIE_Pack_IE(source, target, pos, len)
BYTE source;			/* Byte containing supplied field */
BYTE target;			/* Byte to be modified */
BYTE pos;			/* Lowest bit number of bit field to write */
BYTE len;			/* Size of the field to write in bits (0-8) */

{
    BYTE packed_byte;

    packed_byte = PACK_BYTE(source, target, pos, len);

    return (packed_byte);
}

/*****************
* CCIE_Remove_IE *
******************
*
* Locate and remove an IE from a Layer 3 message.
* Return the length of the IE removed.
*
*/
PUBLIC int CCIE_Remove_IE (l3_msg, msg_len, ie_id, codeset, occurrence)
BYTE *l3_msg;			/* The Q.931 message to examine */
int msg_len;			/* The length of the message in bytes */
BYTE ie_id;			/* Identity of the l3 information element
				 * sought */
int codeset;			/* Codeset where ie_id is to be located */
int occurrence;			/* Occurence number for this IE (1 or
				 * greater) */
{
    BYTE *ie;			/* Pointer to IE to be removed */
    int total_ie_len = 0;	/* Length of target IE */
    BYTE trash_byte;


    if ((ie = CCIE_Get_IE(l3_msg, msg_len, ie_id,
			  codeset, occurrence)) != NULL) {
	if (CCIE_Single_Byte_IE(ie_id, &trash_byte, &trash_byte))
	    total_ie_len = 1;
	else
	    total_ie_len = ie[1] + IE_HEAD_LENGTH;

	/*
	 * Pull the end of the message forward, overwriting the targrt IE.
	 */
	LIF_N_Cpy(ie, ie + total_ie_len, msg_len - (ie - l3_msg) - total_ie_len);
    }
    return (total_ie_len);
}


/**********************
* CCIE_Single_Byte_IE *
***********************
*
* Return TRUE if the element ID byte is a single-byte Q.931 information
* element.  If this is a single-byte element, also recover the three-bit
* ID field (bits 6-4) and the four-bit data field (bits 3-0) in
* accordance with the layout of Type 1 single-byte information elements
* as found in CCITT Rec. Q.931, Sec. 4.5.1.
*
*/

PUBLIC BOOLEAN CCIE_Single_Byte_IE(ie_id, unpacked_id, unpacked_data)
BYTE ie_id;			/* Identity of the l3 information element */
BYTE *unpacked_id;		/* Output byte for a single-byte element ID */
BYTE *unpacked_data;		/* Output byte for single-byte element data */

{
    BOOLEAN single_byte_element;/* TRUE if this is a single byte ie */

    single_byte_element = ((ie_id & EXTENSION_MASK) ? TRUE : FALSE);
    if (single_byte_element) {
	*unpacked_id
	    = SUB_BYTE(ie_id, SINGLE_BYTE_ID_LOC, SINGLE_BYTE_ID_LEN);
	*unpacked_data
	    = SUB_BYTE(ie_id, SINGLE_BYTE_DATA_LOC, SINGLE_BYTE_DATA_LEN);
    }
    return (single_byte_element);
}


/*****************
* CCIE_Unpack_IE *
******************
*
* Return a bit field from within a source information element byte.
*  This routine assumes that the bits in a byte is numbered from 7-0.
*  This routine is provided in skeletal form and is anticipated to
*  evolve to a higher state of functional complexity.
*
*/

PUBLIC BYTE CCIE_Unpack_IE(source, pos, len)
BYTE source;			/* Byte containing field to be unpacked */
BYTE pos;			/* Lowest bit number of bit field to unpack
				 * (zero-based) */
BYTE len;			/* Size of the field to unpack in bits (0-8) */

{
    BYTE unpacked_ie;

    unpacked_ie = SUB_BYTE(source, pos, len);

    return (unpacked_ie);
}
/*
 * End of module.
 */
