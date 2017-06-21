/* $Id: dspu_error.c,v 3.2.60.7 1996/09/10 06:08:39 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_error.c,v $
 *------------------------------------------------------------------
 * DSPU Error message printing.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Common procedures for printing all DSPU error messages.
 * Note well: If you make changes here, don't forget to make equivalent
 * changes in error.h.
 *------------------------------------------------------------------
 * $Log: dspu_error.c,v $
 * Revision 3.2.60.7  1996/09/10  06:08:39  lcox
 * CSCdi68591:  dspu pu matching algorithm behaviour is not clearly defined
 * Branch: California_branch
 *
 * Revision 3.2.60.6  1996/09/10  01:02:52  lcox
 * CSCdi61971:  dspu debug message should give readable string
 * Branch: California_branch
 *
 * Revision 3.2.60.5  1996/07/29  21:46:54  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/06/21  02:21:50  slogie
 * CSCdi57722:  DPSU dont notify SSCP that secondary LU is available
 * Branch: California_branch
 * Fixes to ensure that we do not lose any buffers in the activation
 * window.  Plus debug support for the activation window.
 *
 * Revision 3.2.60.3  1996/05/24  07:18:49  lcox
 * CSCdi47251:  Remove "bad" from return code msg cause it isn't always bad
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/17  10:47:29  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.3  1996/04/15  18:25:46  ppearce
 * CSCdi54277:  4500/7500 crashes when debug dspu is enabled
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.2  1996/04/09  18:37:07  ppearce
 * CSCdi54114:  DSPU crashes router during deactivation of downstream PUs
 * Branch: IbuMod_Calif_branch
 *   Check for null luluTimer before invoking StopItimer()
 *
 * Revision 3.2.48.1  1996/03/28  21:07:26  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *   Missed files during VDLC commit
 *
 * Revision 3.2.60.1  1996/05/09  14:12:39  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  06:34:51  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/08  01:45:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.66.1  1996/04/05  03:18:16  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2  1995/11/20  16:37:30  ppearce
 * CSCdi43278:  dspu start and sna start commands disappear from config
 *
 * Revision 3.1  1995/11/09  11:28:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:02:49  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/09/14 06:15:11  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include "dspu.h"
#include "dspu_pu.h"
#include "dspu_error.h"


const char* const errorMessage[] = {
    "",

    "ENABLE, same port defined twice, command ignored",
    "ACTIVATION-WINDOW configured twice, second command ignored", 
    "HOST, multiple hosts with the FOCALPOINT parameter",
    "POOL, host name %s invalid",
    "POOL command has conflicting host LU definitions",
    "POOL name %s invalid, deletion ignored",
    "POOL cannot be deleted because LU is active", 
    "LU, PU name %s is invalid",
    "PU name %s configured twice",
    "PU configured with same RMAC, RSAP and LSAP",
    "PU configured without RemoteAddress or XID parameters",
    "PU xid-rcv value is ignored when RemoteAddress is also configured",
    "PU interface specification conflicts with addressing definition",
    "PU cannot be deleted because dedicated lu is active",
    "PU name %s invalid",
    "POOL name %s invalid",
    "DSPU Host name %s invalid",
    "LU, pool name %s invalid",
    "LU, host name %s invalid",
    "LU command has conflicting host LU definitions",
    "LU command has conflicting downstream LU definitions",
    "LU command has conflicting pool LU definitions",
    "LU command does not immediately follow a DSPU PU command",
    "LU command invalid with both POOL and HOST parameters",
    "LU %d cannot be deleted because it is active",
    "PU_DEFAULT configured twice",
    "PU_DEFAULT not configured",
    "LU End address is before LU Start address",
    "Interface is not enabled",
    "Interface is not supported",
    "RSRB is not defined",
    "RSRB is already defined for %s",
    "RSRB configuration error - Local Ring %d is invalid",
    "RSRB configuration error - Target Ring %d is invalid",
    "RSRB configuration error - Virtual MAC address is invalid",
    "RSRB cannot be removed while PU is started via RSRB",
    "VDLC is not defined",
    "VDLC is already defined for %s",
    "VDLC configuration error - Ring number %d is invalid",
    "VDLC configuration error - Virtual MAC address is invalid",
    "VDLC cannot be removed while PU is started via VDLC",
    "ACTIVATION_WINDOW is not defined",
    "LU, no POOL or HOST parameter defined",
    "LU, no matching DSPU LU POOL definition, LUs not deleted",
    "HOST cannot be deleted because it is active",
    "No memory for configuration command",
    "No memory, command failed for LU %d",
    "No memory, command failed for downstream LU %d",
    "No memory to connect all LUs",
    "Insufficient POOL LUs to connect all LUs",
    "LU %d not configured",
    
    "ENABLE, same sap %d defined twice, command ignored",
    "RSRB ENABLE failed, RSRB Target Ring is not active",
    "RSRB ENABLE failed, cannot enable for %s because RSRB defined for %s",
    "RSRB START failed, no RSRB defined",
    "RSRB START failed, no SAPs enabled for RSRB",
    "RSRB START failed, RSRB Target Ring %d is not active",
    "VDLC ENABLE failed, cannot enable for %s because VDLC defined for %s",
    "VDLC START failed, no VDLC defined",
    "VDLC START failed, no SAPs enabled for VDLC",
    "START failed, no address defined",
    "ACTIVATE failed, connection not started",
    "START, Host/Pu name %s is not defined",
    "START, Host/Pu name %s may not be started on this interface",
    "START, Host/Pu name %s already started",
    "START failed, no SAPs enabled on this interface",
    "ACTIVATE, resource name %s invalid",
    "ACTIVATE, resource already active",
    "NO ACTIVATE failed, connection not active",
    "Invalid local QLLC subaddress, command ignored",
    "Invalid XID, too many digits",

    "Sap %d cannot be used for both upstream and downstream",
    "Sap %d is not enabled at this interface",
    "Sap %d cannot be removed while PU is started on this adaptor",
    
    "\n%s: PU %s input = %d->(%d,a%d)",
    "\n%s: PU %s: input=%s, %s -> %s",
    "\n%s: LU %s-%d in:%d s:%d->(%d,a%d)",
    "\n%s: LU %s-%d: input=%s, %s -> %s",
    "\n%s: LS %s input = %d ->(%d,a%d)",
    "\n%s: LS %s: input=%s, %s -> %s",
    "\n%s: %s: PU %s data length %d data:\n %s\n",    /* %s = Rx or Tx */
    "\n%s: PU %s activated",
    "\n%s: PU %s deactivated",
    "\n%s: LU %s-%d activated",
    "\n%s: LU %s-%d deactivated",
    "\n%s: LS %s connected",
    "\n%s: LS %s disconnected",
    "\n%s: LS %s activation %s Buffers %d, SAP Buffers %d",

    "\n%s: StartTimer failed",
    "\n%s: StopTimer failed",
    "\n%s: Return code (%s) from CLSI",

    "\n%s:\n",
    "  %s %s debugging is on for all PUs\n", 
    "  %s %s debugging is on for PU %s\n",
    "  %s %s debugging is off for all PUs\n", 
    "  %s %s debugging is off for PU %s\n",
    "  PU %s debugging:\n",
    "    %s %s debugging is on\n",
    "  %s debug reset for PU %s failed, Debug for all PUs active"
    
};

const char* const dspuActTraceMsg[] = {
        "",
        "Dactlu",
        "Actlu",
        "NotifyAvail",
        "NotifyNotAvail",
        "Unbind",
        "Dactpu",
        "Actpu",
        "XidNullReq",
        "XidNullRsp",
        "ConnOut",
        "ConnIn",
        "Xid3Req",
        "Xid3Rsp",
        "Free"
};
DspuActTraceEnum dspuActTraceType;











