/* $Id: sr_novell_nlsp.h,v 3.2 1995/11/17 19:25:01 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/sr_novell_nlsp.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * October 1994, John Wright (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_novell_nlsp.h,v $
 * Revision 3.2  1995/11/17  19:25:01  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:34:09  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:30:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

#ifndef _novell_nlsp_h_
#define _novell_nlsp_h_

#include "../snmp/snmp_api.h"
#include "sr_novell_nlspdefs.h"
#include "sr_novell_nlspsupp.h"

/* The Graph and LSP groups are optional and unsupported */
#undef I_nlspGraph
#undef I_nlspNodeTable
#undef I_nlspNodeEntry
#undef I_nlspNodeSysInstance
#undef I_nlspNodeID
#undef I_nlspNodeNetNum
#undef I_nlspNodeType
#undef I_nlspNodeEstDelay
#undef I_nlspNodeEstThroughput
#undef I_nlspNodeMaxPacketSize
#undef I_nlspNodeCost
#undef I_nlspNodeOverload
#undef I_nlspNodeReachable
#undef I_nlspLinkTable
#undef I_nlspLinkEntry
#undef I_nlspLinkSysInstance
#undef I_nlspLinkNLSPID
#undef I_nlspLinkIndex
#undef I_nlspLinkNeighNLSPID
#undef I_nlspLinkFromNeighCost
#undef I_nlspLinkMaxPacketSize
#undef I_nlspLinkThroughput
#undef I_nlspLinkDelay
#undef I_nlspLinkMediaType
#undef I_nlspLinkToNeighCost
#undef I_nlspPathTable
#undef I_nlspPathEntry
#undef I_nlspPathSysInstance
#undef I_nlspPathDestNLSPID
#undef I_nlspPathLinkIndex
#undef I_nlspGraphXRouteTable
#undef I_nlspGraphXRouteEntry
#undef I_nlspGraphXRouteSysInstance
#undef I_nlspGraphXRouteNLSPID
#undef I_nlspGraphXRouteNetNum
#undef I_nlspGraphXRouteCost
#undef I_nlspGraphXRouteHopCount
#undef I_nlspGraphServTable
#undef I_nlspGraphServEntry
#undef I_nlspGraphServSysInstance
#undef I_nlspGraphServNLSPID
#undef I_nlspGraphServName
#undef I_nlspGraphServTypeValue
#undef I_nlspGraphServType
#undef I_nlspGraphServNetNum
#undef I_nlspGraphServNode
#undef I_nlspGraphServSocket
#undef I_nlspLSP
#undef I_nlspLSPTable
#undef I_nlspLSPEntry
#undef I_nlspLSPSysInstance
#undef I_nlspLSPID
#undef I_nlspLSPLifetime
#undef I_nlspLSPSeqNum
#undef I_nlspLSPChecksum
#undef I_nlspLSPRouterType
#undef I_nlspLSPOverload
#undef I_nlspLSPHeader
#undef I_nlspLSPOptTable
#undef I_nlspLSPOptEntry
#undef I_nlspLSPOptSysInstance
#undef I_nlspLSPOptLSPID
#undef I_nlspLSPOptIndex
#undef I_nlspLSPOptCode
#undef I_nlspLSPOptLength
#undef I_nlspLSPOptValue

/* MAP groups unfinished remove them too for now */
#undef I_nlspIDMapTable
#undef I_nlspIDMapEntry
#undef I_nlspIDMapSysInstance
#undef I_nlspIDMapID
#undef I_nlspIDMapServerName
#undef I_nlspIDMapNetNum
#undef I_nlspNetMapTable
#undef I_nlspNetMapEntry
#undef I_nlspNetMapSysInstance
#undef I_nlspNetMapNetNum
#undef I_nlspNetMapServerName
#undef I_nlspNetMapID
#undef I_nlspNameMapTable
#undef I_nlspNameMapEntry
#undef I_nlspNameMapSysInstance
#undef I_nlspNameMapServerName
#undef I_nlspNameMapNetNum
#undef I_nlspNameMapID


#include "sr_novell_nlsppart.h"
#include "sr_novell_nlsptype.h"

extern const ObjectInfo novell_nlsp_OidList[];
extern const uint       novell_nlsp_OidListNum;

extern void init_novell_nlsp(subsystype *);

#endif  /* _novell_nlsp_h_ */
      
