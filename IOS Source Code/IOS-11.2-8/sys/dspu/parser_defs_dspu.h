/* $Id: parser_defs_dspu.h,v 3.1.40.4 1996/09/13 08:31:33 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/parser_defs_dspu.h,v $
 *------------------------------------------------------------------
 * Dummy placeholder for Downstream PU (DSPU)
 *
 * April 94, Vivian Tseng
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_dspu.h,v $
 * Revision 3.1.40.4  1996/09/13  08:31:33  lcox
 * CSCdi61967:  dspu - retries & retry-timeout dont work like doc says
 * Branch: California_branch
 *   retry-timeout range changed in code to agree with user
 *   documentation  (was 0..600, is now 1..600
 *
 * Revision 3.1.40.3  1996/08/09  01:53:53  lcox
 * CSCdi61966:  dspu - maxiframe range incorrect for default pu
 * Branch: California_branch
 *
 * Revision 3.1.40.2  1996/07/24  01:23:45  szdravko
 * CSCdi61963:  No action and no error message on 2nd dspu default-pu
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/05/17  10:49:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.26.2  1996/05/03  17:18:38  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.26.1  1996/03/28  08:20:45  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.1  1996/02/01  04:57:32  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:50:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:20:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/11  04:20:56  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.1  1995/06/07  22:29:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * DSPU/SNA configuration constants
 *
 */
#define DSPU_MIN_MAXIFRAME 64
#define DSPU_MAX_MAXIFRAME 18432
/* RSRB related constants */
#define DSPU_MAX_BRIDGE_NUM 15
#define DSPU_MAX_RING_NUM 4095

#define DSPU_MIN_RETRIES 1
#define DSPU_MAX_RETRIES 255
#define DSPU_MIN_RETRYTIMEOUT 1
#define DSPU_MAX_RETRYTIMEOUT 600

/*
 * DSPU/SNA Defaults
 */
#define DSPU_DEFAULT_WINDOW 7
#define DSPU_DEFAULT_MAXIFRAME 1472


/*
 * dspu defines
 */
enum {
    DSPU_ENABLE,
    DSPU_RSRB,
    DSPU_NCIA,
    DSPU_VDLC,
    DSPU_HOST,
    DSPU_ACTWINDOW,
    DSPU_POOL,
    DSPU_PU,
    DSPU_LU,
    DSPU_DEFAULT_PU,
    DSPU_START,
    DSPU_ACTIVATE,
    DSPU_ENABLE_HOST,
    DSPU_NOTIFICATION,
/*
 *  sna defines
 */
    SNA_ENDNODE,
    SNA_RSRB,
    SNA_VDLC,
    SNA_HOST,
    SNA_START,
    SNA_ENABLE_HOST,
    SNA_LOCATION,
    LAN_NAME,
} ;
/*
 * sna/dspu debug defines
 */
enum {
    DEBUG_DSPU_STATE =1,
    DEBUG_DSPU_TRACE,
    DEBUG_DSPU_PACKET,
    DEBUG_DSPU_ACTIVATION,
    DEBUG_SNA_STATE,
    DEBUG_SNA_TRACE,
    DEBUG_SNA_PACKET,
    DEBUG_SNA_ACTIVATION,
} ;

