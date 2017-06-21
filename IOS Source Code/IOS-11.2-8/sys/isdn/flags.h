/* $Id: flags.h,v 3.2.58.2 1996/06/16 21:13:15 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/isdn/flags.h,v $
 *------------------------------------------------------------------
 * flags.h  ISDN application flags 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: flags.h,v $
 * Revision 3.2.58.2  1996/06/16  21:13:15  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.58.1  1996/03/21  22:51:45  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1996/02/16  03:42:12  mbeesley
 * CSCdi49179:  ISDN subsystem does not compile on Predator
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  17:40:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/23  15:14:12  dclare
 * CSCdi36325:  Australian ISDN PRI implementation for the 7000. Also get
 * rid of some homologation defines that became too awkward to use. These
 * are the final changes found during homologation.
 *
 * Revision 2.1  1995/06/07  21:15:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*                            F L A G S . H                              *
*************************************************************************
* MODULE:       flags.h                                                 *
* DESCRIPTION:  ISDN application flags                                  *
* REVISION:     V321                                                    *
*************************************************************************
*                                                                       *
*                          COPYRIGHT STATEMENT                          *
*                                                                       *
*                          Copyright (c) 1988                           *
*                           by Telenetworks                             *
*                         Petaluma, CA   94952                          *
*                            (707) 778-8737                             *
*                                                                       *
* All rights reserved. Copying, compilation, modification, distribution *
* or any other use whatsoever of this material is strictly prohibited   *
* except in accordance with a Software License Agreement with           *
* Telenetworks.                                                         *
*                                                                       *
*************************************************************************
*
* GENERAL NOTES
*
* Compile time flags allow the same source code to be compiled differently
* and run in a variety of application environments.  This include file
* contains all compile time flags; the application is defined here.  All
* source modules that contain any compile time flags must include this
* include file.
*
* Flags are grouped by category (compiler flags, hardware flags, module
* flags, etc).  Each flag is either defined or not; if it is not defined,
* it is surrounded by comments.  If the comments are removed by the user
* then the flag is defined.  Just because a flag is defined does not mean
* that the desired code will be included at compile time.  If the 'flavor'
* of software was not purchased (e.g. BRI_5ESS), the code does not exist
* and cannot be included.
*
* Flags are in one of three states:
*
* 1:  Defined - this 'flavor' of the software has been purchased and
*         the code to implement it is included.
* 2:  Undefined - this 'flavor' of the software has not been purchased
*         and the code to implement it is not included.
* 3:  Undefined (but may be defined) - the code is included but
*         surrounded by an '#ifdef' statement.  Although the flag is
*         undefined, it may be defined simply by removing the comments
*         that surround it's definition.  Then, the code will be
*         included.
*
* NOTE:  If a flag needs to be defined for one or some modules but not all
*        (e.g. DEBUG), the flag may be defined on the compile line rather 
*        than here.
*
*/

#ifndef     FLAGS_H                       /* prevent nested inclusions */
#define     FLAGS_H                                     /* module name */

#define APP_DUMB_TIO

/* 
 * PRI and RP1 defines should be set for the 4000, 7000, 7100, 
 * and AS5200 family.
 */

#if defined(RP1) || defined(RSP) || defined(XX) || defined(SIERRA) || defined(PREDATOR) || defined(BRASIL) || defined(C3600)
#define PRI
#define PRI_DMS100
#define PRI_4ESS
#define PRI_5ESS
#endif

#define BRI
#define BRI_1TR6
#define BRI_5ESS
#define BRI_5ESS_FI
#define BRI_DMS100
#define BRI_NET3
#define BRI_NI1
#define BRI_TS013
#define CCITT
#define CHANNEL_TABLE
#define CMP_MCC68K
#define COMMAND_INTERFACE
#define EURO_ACT
#define ISDN
#define KDD_NTT

#define MEM_STATS
#define FFS_CHECK_ERRORS

#define N_AMERICA_FIT
#define OS_USG
#define SUPPRESS_XID
#define TASK_CC_BRI
#define TASK_HDLC_D
#define TASK_L2
#define TASK_L3
#define TASK_ME
#define USER
#define VN2

#define DISPLAY_LOCAL_ERR
#define EXTENDED_HOST_MSG

#if defined(OBJECT_68) || defined(OBJECT_36)
/*
 * 68k based boxes
 */
#define HDW_MC68000
#endif /* OBJECT_68 */

#ifdef OBJECT_4K
/*
 * R4600 Orion based boxes
 */
#define HDW_R4600
#endif /* OBJECT_4K */

#ifndef NULL
#define NULL ((void *)0)
#endif
#endif
