/* $Id: smrp_port.h,v 3.2.62.1 1996/05/02 08:44:50 dwong Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_port.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP ports.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_port.h,v $
 * Revision 3.2.62.1  1996/05/02  08:44:50  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2  1995/11/17  18:54:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:25:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/11  19:02:25  mleelani
 * CSCdi41471:  Add support for pruning duplicate paths
 * o Check if we are the right parent when we receive member
 * responses.
 * o Assign PortParentaddresses on other ports correctly, when
 * better routes are received.
 * o Negotiate portparents using PortParentUnreachable Vector,
 * when a new port comes up.
 *
 * Revision 2.4  1995/08/30  22:43:35  mleelani
 * CSCdi38436:  SMRP: Duplicate paths when the previous parent poisons the
 * route.
 * Wrong Port Parent addresses get assigned during race conditions; these
 * lead to multiple data paths.
 *
 * Revision 2.3  1995/08/01  00:22:58  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.2  1995/06/14  22:55:11  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_PORT_H__
#define __SMRP_PORT_H__

#include "../util/tree.h"


/*
 *------------------------------------------------------------------
 * SMRP Port data structure
 *
 * All group-related fields are managed by the group module.
 * All other fields are private to the port module.
 *------------------------------------------------------------------
 */

typedef boolean (*smrpfastswitch_type)(paktype*, SMRPCache*);
typedef boolean (*smrpmcifastswitch_type)(hwidbtype*, SMRPCache*, tinybool);

struct SMRPPort_ {
    struct	treeLink_	link;

    ulong	PortID;
    boolean	NetProtocolUp;

    uchar	State;	
    SMRPNetwork	RangeStart;
    SMRPNetwork	RangeEnd;
    boolean	ConfiguredRange;
    int		NetworkType;
    SMRPIdbsb  *NetworkReference;
    UnicastAddr	ThisNodeAddress;
    UnicastAddr	PrimaryNodeAddress;
    UnicastAddr SecondaryNodeAddress;

    int RouteCache;
    boolean FastOkay;
    smrpfastswitch_type Fastswitch;
    smrpmcifastswitch_type MCIFastswitch;
    char *Fastswitch_name;

    SMRPGroup* GroupTable;
    SMRPGroupID LastGroupID;
    
    const SMRPCallDowns* NetHandlers;
};


/*******************************************************************
 * Traffic statistics definitions
 *******************************************************************/
enum {
    SMRPPORT_NEW_PORTS,
    SMRPPORT_RECYCLED_PORTS,
    SMRPPORT_DELETED_PORTS,
    SMRPPORT_MAXTRAFFIC		/* must be last */
    };

extern ulong* SMRPPORT_stat;

extern rbTree* smrp_PortTable;


/*
 *------------------------------------------------------------------
 * SMRP Network Layer Entry Points defined by smrp_port.
 *------------------------------------------------------------------
 */
extern void smrp_DeletePortID(SMRPPortID);
extern SMRPPort *smrp_CreatePort(const SMRPNet*, const SMRPCallDowns*);
extern SMRPIdbsb* smrp_GetPortIDNetInterface(SMRPPortID);
extern void smrp_PortIDLinkState(SMRPPort *, boolean);

/*
 *------------------------------------------------------------------
 * SMRP Internal Entry Points defined by smrp_port.
 *------------------------------------------------------------------
 */
extern SMRPPort* smrp_FindPort(SMRPPortID);
extern SMRPPort *smrp_FindPortFromPak(paktype *);
extern boolean smrp_IsPortOperational(SMRPPort*);
extern boolean smrp_IsPortActive(SMRPPort*);
extern boolean smrp_IsOurNodePortPrimary(SMRPPort*);
extern boolean smrp_IsOurNodePortSecondary(SMRPPort*);
extern UnicastAddr smrp_GetPortSecondary(SMRPPort*);
extern void smrp_ForAllPortsDo(treeProc,void*);
extern void smrp_HandleAllPorts(void);
extern void smrp_DoPrimaryRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DoSecondaryRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_DoTunnelRequestPacket(SMRPPort *, SMRPResponse *);
extern void smrp_SetPortPrimary(SMRPPort*,UnicastAddr);
extern void smrp_SetPortSecondary(SMRPPort*,UnicastAddr);
extern void smrp_DoPortPrimaryNegotiation(SMRPPort *);
extern void smrp_DoPortSecondaryNegotiation(SMRPPort*);
extern void smrp_port_Initialize(void);
extern void smrp_CloseAllPorts(void);
extern void smrp_ResetIFNotify(idbtype *);
extern void smrp_route_adjust(idbtype *);
extern void smrp_IFFixupNotify(idbtype *, int);

/*
 *------------------------------------------------------------------
 * Inline Functions to access network layer functions.
 *------------------------------------------------------------------
 */
extern inline int
PORTCB_MaxPacketSize (SMRPPort *port)
{
    return (*port->NetHandlers->MaxPacketSize)(port->NetworkReference);
}

extern inline UnicastAddr
PORTCB_GetPacketSource (SMRPPort *port, paktype *pak)
{
    return (*port->NetHandlers->GetPacketSource)(pak);
}

extern inline boolean
PORTCB_CheckPacket (SMRPPort *port, paktype *pak)
{
    return (*port->NetHandlers->CheckPacket)(pak);
}

extern inline paktype*
PORTCB_CreatePacket(SMRPPort* port, int dsize)
{
    return (*port->NetHandlers->CreatePacket)(port->NetworkReference, dsize);
}

extern inline boolean
PORTCB_SendMCPacket(SMRPPort* port,paktype* pak, SMRPGroupID dmc)
{
    return (*port->NetHandlers->SendMCPacket)(port->NetworkReference,pak,dmc);
}

extern inline boolean
PORTCB_SendPacket(SMRPPort* port,paktype* pak,ulong dhost)
{
    return (*port->NetHandlers->SendPacket)(port->NetworkReference,pak,dhost);
}

extern inline void
PORTCB_ClosePort(SMRPPort* port)
{
    (*port->NetHandlers->ClosePort)(port->NetworkReference);
}

extern inline boolean
PORTCB_LinkState(SMRPPort* port)
{
    return (*port->NetHandlers->GetLinkState)(port->NetworkReference);
}

extern inline UnicastAddr
PORTCB_NetAddress(SMRPPort* port)
{
    return (*port->NetHandlers->GetNetAddress)(port->NetworkReference->phys_idb);
}

extern inline void
PORTCB_Subscribe(SMRPPort* port,SMRPGroupID mc)
{
    (*port->NetHandlers->Subscribe)(port->NetworkReference->phys_idb,mc);
}

extern inline void
PORTCB_Unsubscribe(SMRPPort* port,SMRPGroupID mc)
{
    (*port->NetHandlers->Unsubscribe)(port->NetworkReference->phys_idb,mc);
}

#if NOT_USED
extern inline void
PORTCB_PrintNodeAddress(SMRPPort* port, UnicastAddr na)
{
    (*port->NetHandlers->PrintNodeAddress)(port->PortID,na);
}
#endif

extern inline char *
PORTCB_PrintNetAddress(SMRPPort* port, char* buf, UnicastAddr na)
{
    return (*port->NetHandlers->PrintNetAddress)(port->NetworkReference->phys_idb,buf,na);
}

extern inline void
PORTCB_ShowPortProtocol(SMRPPort* port)
{
    (*port->NetHandlers->ShowPortProtocol)(port->NetworkReference->phys_idb);
}

extern inline void
PORTCB_DeletePort(SMRPPort *port)
{
    (*port->NetHandlers->DeletePort)(port->NetworkReference);
}

extern inline boolean
PORTCB_OpenPort (SMRPPort *port)
{
    return (*port->NetHandlers->OpenPort)(port->NetworkReference->phys_idb);
}

static inline boolean
PORTCB_DoTunnelRequest(SMRPPort *port, SMRPResponse *res)
{
    if (!port->NetHandlers->DoTunnelRequest)
	return FALSE;
    else
	return (*port->NetHandlers->DoTunnelRequest)(port, res);
}


#endif __SMRP_PORT_H__
