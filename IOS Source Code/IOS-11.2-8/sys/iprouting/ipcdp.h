/* $Id: ipcdp.h,v 3.1.2.1 1996/04/15 21:17:58 bcole Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ipcdp.h,v $
 *------------------------------------------------------------------
 * Automatic stub routing for IP via CDP
 *
 * January 1996, Bruce Cole
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipcdp.h,v $
 * Revision 3.1.2.1  1996/04/15  21:17:58  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:24  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CDP_IPPREFIX_TLV_LEN 5
#define CDP_ACQUIRETIME (CDP_DEF_XMIT_INTERVAL * ONESEC)

#define CDP_AGETIME (CDP_DEF_XMIT_INTERVAL * ONESEC)
#define CDP_INVALIDTIME (3 * CDP_DEF_XMIT_INTERVAL * ONESEC)
#define CDP_FLUSHTIME (4 * CDP_DEF_XMIT_INTERVAL * ONESEC)

#define CDP_METRIC 1
