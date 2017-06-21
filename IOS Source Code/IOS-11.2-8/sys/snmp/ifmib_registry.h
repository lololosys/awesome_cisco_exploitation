/* $Id: ifmib_registry.h,v 3.3.56.1 1996/04/30 23:17:29 sdurham Exp $
 * $Source: /release/112/cvs/Xsys/snmp/ifmib_registry.h,v $
 *------------------------------------------------------------------
 * ifmib_registry.h Service points for ifmib support
 *
 * June 1995, Sandra Durham
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ifmib_registry.h,v $
 * Revision 3.3.56.1  1996/04/30  23:17:29  sdurham
 * CSCdi46300:  Interface missing from MIB
 * Branch: California_branch
 *          add atm and aal5 layers to the ifmib
 *
 * Revision 3.3  1995/11/20  22:27:29  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.2  1995/11/17  18:58:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:48:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IFMIB_REGISTRY_H__
#define __IFMIB_REGISTRY_H__


#include "../h/registry.h"
#include "../h/encapstype.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"
#include "../snmp/ifmib_registry.regh"

#endif  /* __IFMIB_REGISTRY_H__ */


