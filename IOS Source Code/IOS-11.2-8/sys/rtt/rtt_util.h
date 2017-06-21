/* $Id: rtt_util.h,v 1.1.4.2 1996/06/07 22:26:32 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_util.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor dump interface Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the function definitions for
 * dump interface calls used by the components of the
 * RTT Monitor. 
 *------------------------------------------------------------------
 * $Log: rtt_util.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:32  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:54  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:42:00  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:56  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:40  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_UTIL_H_
#define _RTT_UTIL_H_

#include "rtt_lib.h"
#include "rtt_util_ext.h"

#define RTT_ERROR(x) ( (RTT_ERROR_FLAG & (1<<(x%MAXULONG_BITS))) == (1<<(x%MAXULONG_BITS)) )
#define RTT_TRACE(x) ( (RTT_TRACE_FLAG & (1<<(x%MAXULONG_BITS))) == (1<<(x%MAXULONG_BITS)) )

/* 
 * time to wait for trace output to get
 *    displayed before continuing in
 *    milliseconds (5000 = 5 seconds)
 */
#define OUTPUT_SLEEP_TIME 5000

#define RTT_BUGINF2(a,b) if (RTT_ERROR(a)) buginf(b)
#define RTT_BUGINF3(a,b,c) if (RTT_ERROR(a)) buginf(b,c)
#define RTT_BUGINF4(a,b,c,d) if (RTT_ERROR(a)) buginf(b,c,d)
#define RTT_BUGINF5(a,b,c,d,e) if (RTT_ERROR(a)) buginf(b,c,d,e)
#define RTT_BUGINF6(a,b,c,d,e,f) if (RTT_ERROR(a)) buginf(b,c,d,e,f)
#define RTT_BUGINF7(a,b,c,d,e,f,g) if (RTT_ERROR(a)) buginf(b,c,d,e,f,g)
#define RTT_BUGINF8(a,b,c,d,e,f,g,h) if (RTT_ERROR(a)) buginf(b,c,d,e,f,g,h)
#define RTT_BUGINF9(a,b,c,d,e,f,g,h,i) if (RTT_ERROR(a)) buginf(b,c,d,e,f,g,h,i)
#define RTT_BUGINF10(a,b,c,d,e,f,g,h,i,j) if (RTT_ERROR(a)) buginf(b,c,d,e,f,g,h,i,j)


extern void registerRtt SR_PROTOTYPE((void));

#endif /* _RTT_UTIL_H_ */
