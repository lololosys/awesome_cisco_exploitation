/* $Id: me.h,v 3.2.60.1 1996/04/30 23:07:58 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/me.h,v $
 *------------------------------------------------------------------
 * me.h  ISDN Management Entity include file 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: me.h,v $
 * Revision 3.2.60.1  1996/04/30  23:07:58  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:41:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:16:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       me.h                                                    
DESCRIPTION:  ISDN Management Entity include file                     
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


This include file contains definitions that are exported from the
management entity.


*************************************************************************
*************************************************************************
*/


#ifndef     ME_H                          /* prevent nested inclusions */
#define     ME_H                                        /* module name */

/************************************************************************
*                          C O N S T A N T S                            *
*************************************************************************
*/

#define     TIMER_MASK_ME       0x1700             /* time out message */
#ifdef EURO_ACT
#define ASSIGNMENT_NOT_STARTED      0
#define ASSIGNMENT_IN_PROGRESS      1
#define ASSIGNMENT_TIMEOUT          2
#define ASSIGNMENT_DONE             3
#endif EURO_ACT
/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
#ifdef EURO_ACT
#ifdef MEIF_C                                               /* storage */
BYTE assignment_state[MAX_DSL];
#else                                                     /* reference */
extern BYTE assignment_state[MAX_DSL];
#endif MEIF_C
#endif EURO_ACT

/************************************************************************
*                   D A T A   A L L O C A T I O N                       *
*************************************************************************
*/

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

extern int     CCBFIT_TerminalInit(int, int, int);
extern BOOLEAN ME_got_mnl_mim_ind(int, int, int, PKT_ID);

PUBLIC  void   ME_Go                (INT16, BYTE, BYTE, BYTE, PKT_ID);
PUBLIC  void   ME_Init              (void);
PUBLIC  void   ME_ShutDown          (int);
PUBLIC  void   LM_PersDeact         (int);
PUBLIC  boolean me_activate_ok      (int, int);
PUBLIC  void me_set_activate_flag   (int, int, boolean);
#endif
