/* $Id: rtt_debug_ext.h,v 1.1.4.2 1996/06/07 22:26:01 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_debug_ext.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor debug external functions.
 *
 * Apr 1996, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rtt_debug_ext.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:01  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:05  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.1  1996/04/19  02:41:25  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/04/18  23:59:32  lmetzger
 * Initial Version of Response Time Reporter Configuration CLI
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_DEBUG_EXT_H_
#define _RTT_DEBUG_EXT_H_

extern void rttmon_debug_command(parseinfo * csb);

extern void rtt_init_debug(void);

#endif /* _RTT_DEBUG_EXT_H_ */
