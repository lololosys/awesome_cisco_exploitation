/* $Id: pools.c,v 3.3.26.2 1996/06/28 23:20:04 hampton Exp $
 * $Source: /release/112/cvs/Xsys/isdn/pools.c,v $
 *------------------------------------------------------------------
 * pools.c  Memory Pool Manager
 *
 * March 1992, Ronnie Kon 
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pools.c,v $
 * Revision 3.3.26.2  1996/06/28  23:20:04  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.26.1  1996/04/30  23:08:02  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/22  06:35:28  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  17:42:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/03  20:21:37  dclare
 * CSCdi38707:  ISDN exec exits. Make sure the system is ready before
 * getting memory for the task.
 *
 * Revision 2.2  1995/08/25  11:44:24  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 21:17:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: pools.c
VERSION: 3.21
DESCRIPTION: Memory Pool Manager

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

This module has two classes of functions: Initialization functions
and all the rest.

This module has two initialization functions InitMemMgr() and
MakePool().  Function InitMemMgr() should be called once at system
startup.  Function MakePool should be called once per memory pool.

*************************************************************************
*************************************************************************
*/


/************************************************************************
*                E X T E R N A L    R E F E R E N C E S                 
*************************************************************************
*/

/************************************************************************
*                              M A C R O S                              
*************************************************************************
*/


/************************************************************************
*                           C O N S T A N T S                           
*************************************************************************
*/

/*
 * NOTE:  This file intentionally left blank.
 * It is no longer used and should be removed, but the removal will be
 * delayed until some other file cleanup is done.
 */
