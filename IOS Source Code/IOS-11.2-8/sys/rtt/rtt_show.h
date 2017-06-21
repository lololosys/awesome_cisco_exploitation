/* $Id: rtt_show.h,v 1.1.4.2 1996/06/07 22:26:25 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_show.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor show CLI Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains CLI show parameters
 * RTT Monitor. 
 *------------------------------------------------------------------
 * $Log: rtt_show.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:25  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:42  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:41:55  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:49  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:36  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_SHOW_H_
#define _RTT_SHOW_H_

typedef enum {
    rtt_output_raw,
    rtt_output_brief,
    rtt_output_full,
} rttMonOutputFlag;


#endif /* _RTT_SHOW_H_ */
