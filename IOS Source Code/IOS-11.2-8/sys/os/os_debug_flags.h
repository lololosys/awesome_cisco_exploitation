/* $Id: os_debug_flags.h,v 3.2.62.3 1996/08/04 06:27:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/os_debug_flags.h,v $
 *------------------------------------------------------------------
 * os_debug_flags.h -- Debugging flag declarations for OS
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: os_debug_flags.h,v $
 * Revision 3.2.62.3  1996/08/04  06:27:19  thille
 * CSCdi64520:  %SYS-2-GETBUFFFAIL: Memory allocation of 67 bytes failed
 * Branch: California_branch
 * Add "debug buffer failures" command and default to office
 *
 * -Nick
 *
 * Revision 3.2.62.2  1996/06/04  05:16:06  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.2.62.1  1996/04/19  15:24:43  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.3  1996/04/02  10:31:49  carrel
 * cvs fixes
 *
 * Revision 2.1.6.2  1996/02/28  06:14:47  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.1  1995/11/17  17:22:58  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.2  1995/11/17  18:50:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:19:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/15  03:42:21  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.1  1995/06/07  21:57:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from OS code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(os_debug_arr)

DEBUG_FLAG(sanity_debug,DEBUG_SANITY,"Buffer pool sanity")
DEBUG_FLAG(buffer_debug,DEBUG_BUFFER,"Additional buffer checking")
DEBUG_FLAG(subsys_debug,DEBUG_SUBSYS,"Subsystem")
DEBUG_FLAG(nvram_debug,DEBUG_NVRAM,"NVRAM behaviour")
DEBUG_FLAG(modem_debug,DEBUG_MODEM,"Modem control/process activation")
DEBUG_FLAG(tacacs_debug,DEBUG_TACACS,"TACACS access control")
DEBUG_FLAG(tacacs_events_debug,DEBUG_TACACS_EVENTS,"TACACS+ events")
DEBUG_FLAG(aaa_authen_debug,DEBUG_AAA_AUTHEN, "AAA Authentication")
DEBUG_FLAG(aaa_author_debug,DEBUG_AAA_AUTHOR, "AAA Authorization")
DEBUG_FLAG(aaa_acct_debug,DEBUG_AAA_ACCT, "AAA Accounting")
DEBUG_FLAG(kerberos_debug,DEBUG_KERBEROS,"Kerberos")
DEBUG_FLAG(buffer_fail_debug,DEBUG_BUFFER_FAIL,"Buffer Allocation failures")

DEBUG_ARRDONE
