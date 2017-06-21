/* $Id: chassismib_registry.h,v 3.1.2.1 1996/05/21 06:38:35 mordock Exp $
 * $Source: /release/112/cvs/Xsys/snmp/chassismib_registry.h,v $
 *------------------------------------------------------------------
 * chassismib_registry.h Service points for chassis mib support
 *
 * May 1996, Scott R. Mordock
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: chassismib_registry.h,v $
 * Revision 3.1.2.1  1996/05/21  06:38:35  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/05/20  17:02:11  mordock
 * placeholders for california
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CHASSISMIB_REGISTRY_H__
#define __CHASSISMIB_REGISTRY_H__


#include "../h/registry.h"
#include "../snmp/chassismib_api.h"
#include "../snmp/chassismib_registry.regh"
#include "../snmp/sr_old_lchassismibdefs.h"   /* D_cardType_* defines */

#endif  /* __CHASSISMIB_REGISTRY_H__ */


