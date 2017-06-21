/* $Id: rtt_alert.h,v 3.1.4.3 1996/08/16 18:32:32 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/dspu/rtt_alert.h,v $
 *------------------------------------------------------------------
 * SNA Network Management resolutions
 *
 * Jan 96, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rtt_alert.h,v $
 * Revision 3.1.4.3  1996/08/16  18:32:32  lmetzger
 * CSCdi66255:  Add Modularity of RTR for DSPU/SNANM
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/06/07  22:24:31  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:49:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/22  18:50:52  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:42  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_ALERT_H_
#define _RTT_ALERT_H_

#include "registry.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"

extern void rtt_Resolution(Rtt_ResolveType resolveType,
			   long rttMonCtrlAdminIndex,
			   void *uniqueToken);

extern void rtt_Alert(Rtt_AlertType alertType, 
		      long rttMonCtrlAdminIndex,
		      void *uniqueToken);

#endif /* _RTT_ALERT_H_ */
