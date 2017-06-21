/* $Id: tei_ces.h,v 3.2 1995/11/17 17:42:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/tei_ces.h,v $
 *------------------------------------------------------------------
 * tei_ces.h  ISDN TEI/CES allocation table definitions 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: tei_ces.h,v $
 * Revision 3.2  1995/11/17  17:42:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:15:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:17:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       tei_ces.h                                               
DESCRIPTION:  ISDN TEI/CES allocation table definitions               
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


The purpose of this header file is to define and allow shared access
to the TEI/CES allocation table.  The reason this table is shared is
that Layer 2 must be capable of performing the translation between
TEI and CES, the Management Entity must be capable of performing TEI
assignment and Call Control must be able to perform CES assignment
(and implicitly TEI assignment) for calls originating in Call Control
(NETWORK side only).

The TEI/CES table is a rectangular two-dimensional array where the first
dimension is the data links on an interface and the second dimension is
the list of Digital Subscriber Loops (or USER/NETWORK interfaces)
served by the application.  In the NETWORK application, this table
has been filled in with initial values for Digital Subscriber Loop 0.
This initialization is the prototype for all the DSL's and the rest of
the table is replicated for each DSL from the prototype DSL 0 at the
run time initialization of Layer 2 NETWORK, which has ownership of the
memory consumed by this table.

For the USER side, the table is entirely initialized in L2_Init to
UNASSIGNED status pending the first data link establishment.


*************************************************************************
*************************************************************************
*/

#ifndef     TEI_CES_H                     /* prevent nested inclusions */
#define     TEI_CES_H                                   /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/* CUSTOMIZE HERE - Maximum number of Logical Links per Digital
*                   Subscriber Loop.
*/
#ifdef BRI
#define     MAX_LOGICAL_LINKS   11      /* Maximun logical links (BRI) */
#else
#define     MAX_LOGICAL_LINKS    2      /* Maximun logical links (PRI) */
#endif

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/* TEI to CES allocation table element structure for one data link on
*  one DSL..
*/
struct TeiCesTableStruct
    {
    BYTE  Tei;                         /* Terminal Endpoint Identifier */
    BYTE  Ces;                          /* Connection Endpoint Sufffix */
    INT16 DSL_Chan;      /* Associated Digital Subscriber Loop Channel */
    };

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/* Test for an automatic assignment TEI value.
*/
#define     AUTO_TEI(t)  ((((t)>=64) && ((t)<BROADCAST_TEI)) ? TRUE : FALSE)

/* Test for a non-automatic assignment TEI value.
*/
#define NON_AUTO_TEI(t)  (((t)>=0) && ((t)< 64))

/************************************************************************
*                   D A T A   A L L O C A T I O N                       *
*************************************************************************
*/
/* Allocate memory for the TeiCesTable.
*/
#ifdef L2_C
#ifdef USER
struct TeiCesTableStruct TeiCesTable [MAX_DSL][MAX_LOGICAL_LINKS];
#endif


#else                                                      /* Not L2_C */
extern struct TeiCesTableStruct TeiCesTable [MAX_DSL][MAX_LOGICAL_LINKS];
#endif                                                         /* L2_C */
#endif                                                    /* TEI_CES_H */

