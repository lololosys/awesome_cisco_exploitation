/* $Id: smrp_net.h,v 3.2 1995/11/17 18:53:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_net.h,v $
 *------------------------------------------------------------------
 * Network-level interface to SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_net.h,v $
 * Revision 3.2  1995/11/17  18:53:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/14  22:55:05  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_NET_H__
#define __SMRP_NET_H__
/*
 * SMRP Network Layer Definitions
 *
 * When SMRP is configured for a network layer, a port definition block
 * is created and initialized.  The port definition block joins the 
 * network layer and its underlying physical interface to SMRP.
 * 
 * At SMRP port creation, three data structures are passed to SMRP
 * to describe the network layer specifications.  These data structures
 * are defined by this module.  SMRP returns an SMRPPortID to be
 * used during all communications between SMRP and the network layer.
 *
 * SMRP uses the information contained in the SMRPNet structure to
 * set the SMRP configuration parameters for the port.  Network layer
 * information such as the interface are also provided.
 *
 * The network protocol supports a list of multicast addresses
 * available for used by SMRP.  This list is used when sending MC
 * packets.  The group to MC list entry mapping is protocol specific
 * but must always support two special IDs: SMRPAllNodesMC (1) and
 * SMRPAllEndpointsMC (2). NOTICE: The code assumes that these special
 * IDs are not valid network layer unicast addresses nor are they 
 * valid SMRP group addresses.
 *
 * SMRP invokes network layer operations by calling routines that
 * are defined in the SMRPCallDowns structure.  This table is network
 * protocol specific and usually shared by all ports of that protocol.
 *
 * The network layer invokes SMRP operations by calling routines defined
 * later in this module.  The network layer must notify SMRP of events
 * such as line protocol changes.
 */

/*
 *------------------------------------------------------------------
 * SMRP Network Layer Data Type Definitions
 *------------------------------------------------------------------
 */

#define SMRP_UNICASTADDR_SIZE 4
typedef ulong UnicastAddr;	/* Network layer addresses */
typedef ulong SMRPGroupID;

enum {
    AllNodesAddr = 1,
    AllEndpointsAddr = 2,
    AllEntitiesAddr = 3,
};

enum {
    AllNodesGroup = 1,
    AllEndpointsGroup = 2,
    AllEntitiesGroup = 3
};

/*
 *------------------------------------------------------------------
 * SMRP Network Layer Configuration Descriptor
 *------------------------------------------------------------------
 *
 * RangeStart, RangeEnd - The SMRP network range for this port.  If
 * a single network number is desired, it is used as both the start and
 * the end.  Zero is not allowed for either network number.  Even if
 * they are not explicitly configured, they should contain a mapped
 * range based upon the underlying network number.  In this case, the
 * range is used if no other nodes are adjacent to this port.  Otherwise
 * the current range defined by the primary node is used.
 *
 * NetType - A unique network layer protocol identifier used by SMRP to 
 * create unique port identifiers.  The current implementation uses
 * the linktype definitions.  ASSUMPTION: The link type must be unique
 * and limited to a single byte.
 *
 * NetInterface - The underlying network interface for the port.  Used
 * only to create port identifiers and to allow the network layer to
 * identify the interface associated with an SMRP port identifier.
 *
 * ConfiguredRange - TRUE is the range was explicitly configured and
 * therefore the range must agree with the adjacent nodes before this
 * port will become operational.  #### Also used when writing to non-vol.
 */
typedef struct SMRPNet_ {
    ulong	RangeStart;
    ulong	RangeEnd;
    int		NetType;		/* LINK_APPLETALK, LINK_NOVELL.... */
    idbtype*	NetInterface;		/* IDB for this port */
    boolean	ConfiguredRange;	/* TRUE if range explicitly specified */
} SMRPNet;

/*
 * SMRP Network Layer Call Downs
 *
 * The network layer defines a list of handlers that SMRP uses
 * to communicate with.  These functions allow SMRP to handle
 * each port without concern for underlying protocol specifics.
 *
 * int Subscribe(SMRPPortID,MCTableIndex)
 *	Enable reception of packets for a multicast address that
 *	was allocated by AllocateMCastAddrs().  
 * 
 * int Unsubscribe(SMRPPortID,MCTableIndex)
 *	Disable reception of packets for a multicast address that
 *	was allocated by AllocateMCastAddrs().  It is not manditory
 *	that the network layer stop delivery packets for a specific
 *	multicast address.  This is only an advisory to the network
 *	layer.
 * 
 * paktype* CreatePacket(SMRPPortID,int datasize)
 *	Returns a packet that has been properly initialized for the
 *	network protocol.  pak->transport_start should point to the
 *	first of datasize bytes for use by SMRP.  pak->transport_size
 * 	should be set to datasize. pak->smrp_portid should be set
 *	to the port id. Datasize specified will be less than or equal
 *	to the value return by MaxPacketSize.
 *
 * UnicastAddr GetPacketSource(SMRPPortID,paktype*)
 *	Returns the network layer source address for the specified 
 *	packet.  
 *
 * boolean CheckPacket(SMRPPortID,paktype*)
 *	Returns TRUE if the packet is valid for all network layer
 *	checks and the SMRP data size is at least 8 bytes.  The 
 *	pak->transport_start field should be initialized to point
 *	to the first SMRP data byte.  This call down is issued for
 *	each packet that is processed by our node.  It is not called
 *	for transit packets (nor by the input process).  CheckPacket
 *	should perform any network layer checksum checks, etc.
 *
 * int SendMCPacket(SMRPPortID,paktype*,SMRPGroupID)
 *	Sends the specified packet to the multicast address for
 *	the group MC.  SMRPGroupID may be a group address or one
 *	of SRMPAllNodesMC, SMRPAllEndpointsMC.  The transport_start,
 *	transport_size, and smrp_portid will be correct.  NOTE: This
 *	routine assumes all responsibility for the packet.
 *
 * int SendPacket(SMRPPortID,paktype*,UnicastAddr)
 *	Sends the specified packet to the unicast address. The 
 *	transport_start, transport_size, and smrp_portid will 
 *	be correct.  NOTE: This routine assumes all responsibility
 *	for the packet.
 *
 * int MaxPacketSize(SMRPPortID)
 *	Returns the maximum SMRP packet size for this network layer.
 *	ASSUMPTION: Does not change after port creation. If not valid,
 *	must use link state transitions.
 *
 * char *PrintNetAddress(SMRPPortID,char*,UnicastAddr)
 *	This routine should sprintf the formatted network address.
 * 	#### Buffsize must be defined here.
 *
 * boolean GetLinkState(SMRPPortID)
 * 	This routine should return the link state of the network 
 *      layer.  Must not return TRUE prior to the network layer being
 *	fully initialized.
 *
 * UnicastAddr GetNetAddress(SMRPPortID)
 * 	This routine should return the active network address.  This 
 * 	call down is used during port startup and after link state
 *	transitions.  ASSUMPTION: Link state will always transition if
 *	the network address changes.  We use the network fixup registry
 *	to detect config changes.
 *
 * void ShowPortProtocol(SMRPPortID, ulong, ulong)
 *	This routine is called when a SHOW PROTOCOL command has been
 *	issued for an interface.  For each port associated with that
 *	interface, this routine is invoked.  The message should be
 *	printf("\n  SMRP network %d-%d running on AppleTalk port %d");
 *
 * 
 */

typedef struct SMRPCallDowns_ {
    void        (*Subscribe)      (idbtype *, SMRPGroupID);
    void        (*Unsubscribe)    (idbtype *, SMRPGroupID);
    paktype*    (*CreatePacket)   (SMRPIdbsb *, int);
    UnicastAddr (*GetPacketSource)(paktype *);
    boolean     (*CheckPacket)    (paktype *);
    boolean     (*SendMCPacket)   (SMRPIdbsb *, paktype *, SMRPGroupID);
    boolean     (*SendPacket)     (SMRPIdbsb *, paktype *, UnicastAddr);
    int         (*MaxPacketSize)  (SMRPIdbsb *);
    char *      (*PrintNetAddress)(idbtype *, char*, UnicastAddr);
    boolean     (*GetLinkState)   (SMRPIdbsb *);
    UnicastAddr (*GetNetAddress)  (idbtype *);
    void        (*ShowPortProtocol)(idbtype *);
    boolean     (*OpenPort)       (idbtype *);
    void        (*ClosePort)      (SMRPIdbsb *);
    void        (*DeletePort)     (SMRPIdbsb *);
    boolean     (*DoTunnelRequest)(SMRPPort *, SMRPResponse *);
} SMRPCallDowns;

typedef boolean (*GroupToMCfn)(hwidbtype *, SMRPGroupID, uchar *);
    
/*
 *------------------------------------------------------------------
 * SMRP Network Layer Entry Points defined by smrp_port.
 *------------------------------------------------------------------
 */

void smrp_ClosePort(struct SMRPPort_ *);
boolean smrp_StartPort(struct SMRPPort_ *);

/*
 *------------------------------------------------------------------
 * SMRP Network Layer Entry Points defined by smrp_input.
 *------------------------------------------------------------------
 */

#if NOT_USED
void smrp_EnqueuePacket(SMRPPortID, paktype*, uchar*, int);
#endif

#endif __SMRP_NET_H__
