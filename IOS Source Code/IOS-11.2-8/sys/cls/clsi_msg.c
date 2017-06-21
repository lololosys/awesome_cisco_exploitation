/* $Id: clsi_msg.c,v 3.3.26.5 1996/09/11 04:45:17 kchiu Exp $
 * $Source: /release/112/cvs/Xsys/cls/clsi_msg.c,v $
 *------------------------------------------------------------------
 * TCLSIMsg objects contain messages between CLS subsystem and DLUs
 *
 * March 1994, Steve Berl
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains the functions for manipulating CLSIMsg objects.
 * There are functions to create, and destroy the messages.
 * There are functions for accessing various field of the messages.
 *------------------------------------------------------------------
 * $Log: clsi_msg.c,v $
 * Revision 3.3.26.5  1996/09/11  04:45:17  kchiu
 * CSCdi68756:  DLSw+/fddi is not working
 * Branch: California_branch
 *
 * Revision 3.3.26.4  1996/06/27  17:04:42  ioakley
 * CSCdi61354:  Back out CSCdi59238 pak_grow_realign update - in those
 *              instances where new pak is obtained the loss of cls
 *              parameter block context causes CLS_ID_STN corruption.
 * Branch: California_branch
 *
 * Revision 3.3.26.3  1996/06/20  23:57:17  ioakley
 * CSCdi59238:  Invoke pak_grow_realign from CLSIMsgCreateFromPak
 *              to address reservedSize related CLSASSERT errors.
 * Branch: California_branch
 *
 * Revision 3.3.26.2  1996/05/17  10:42:02  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.14.3  1996/04/05  06:05:16  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.3.14.2  1996/04/03  13:54:21  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.14.1  1996/03/28  08:19:20  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.3.26.1  1996/03/18  19:10:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/13  01:14:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3  1996/01/18  20:37:37  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2.4.1  1995/12/17  09:49:58  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.2  1995/11/17  08:55:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:53:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/07/31  18:58:15  ppearce
 * CSCdi37939:  Eliminate use of CLS_P_TYPE_NUMBERS from DLSw+
 *
 * Revision 2.5  1995/07/21  17:08:01  sberl
 * CSCdi37437:  Add new ModifyStn primitive to CLSI
 * Just set the right bits in fModifyFlags and put the new values in the
 * parameter structure.
 *
 * Revision 2.4  1995/07/19  20:58:18  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.3  1995/07/02  05:15:20  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.2  1995/06/18  19:39:59  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:19:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#include "cls_private.h"
#include "../if/if_vidb.h"  /* Prototype to grab current vidb_unit number */
#include "../os/pool.h"
#include "../os/buffers.h"
#include "packet.h"
#include "dlc_registry.h"
#include "../llc2/llc2.h"

typedef struct 
{
    void*          fArg1;
    void (*fDestructor)(void* arg1);
} CLSIPrivate_t;


TCLSIMsg* CLSIMsgCreateInPak (size_t size)
{
    paktype* pak;
    TCLSIMsg* theMsg;
    CLSIPrivate_t* privateArea;
    
    pak = getbuffer(size);
    if (pak != NULL)
    {
	theMsg = (TCLSIMsg*) pak->datagramstart;
        memset(theMsg, 0, size);
	theMsg->fLinkPtr = NULL;
	privateArea = (CLSIPrivate_t*) &theMsg->fCLSIPrivate[0];
	privateArea->fArg1 = pak;
	privateArea->fDestructor = (void (*)(void*)) datagram_done;
	return (theMsg);
    }
    else
    {
	return (NULL);
    }
}

void CLSIMsgDestroy (TCLSIMsg* theMsg)
{
    CLSIPrivate_t* privateArea;

    privateArea = (CLSIPrivate_t*) &theMsg->fCLSIPrivate[0];
    (*privateArea->fDestructor)(privateArea->fArg1);
}

/* Return the usable space available for CLSI message in this buffer. */
 /* The size returned is from the start of the CLSIMsg header to the */
 /* end of the buffer.  */

size_t CLSIMsgSpaceAvail (TCLSIMsg* theMsg)
{
    CLSIPrivate_t* privateArea;
    paktype* pak;
    size_t size;
    
    privateArea = (CLSIPrivate_t*) &theMsg->fCLSIPrivate[0];
    pak = (paktype*) privateArea->fArg1;   /* need to change to
						more generalize  later */
    size = pak->pool->size;

    /* The size we get here is from the end of the paktype to the end */
    /* of the buffer. It does not include the ENCAPBYTES or the rest */
    /* of the paktype structure. If CLSIMsg does not start immediately */
    /* after the encapsulation area, we need to adjust the size a bit. */
    /* total available space in a packet is (size + ENCAPBYTES) */
    /* space already used is (theMsg - data_area) */

    size = (size + ENCAPBYTES) - (((byte*) theMsg) - pak->data_area);
    
    return (size);
}

#ifdef notdef
/* This function returns the space available in the buffer before the */
/* CLSI header. This is useful when the buffer is going to be used */
/* for something else. The program can use all the space between the */
/* beginning of the buffer and the start of the actual data, */
/* overwriting the CLSI header. */

size_t CLSIMsgPrefixAvail (TCLSIMsg* theMsg)
{
    CLSIPrivate_t* privateArea;
    paktype* pak;
    size_t size;
    
    privateArea = (CLSIPrivate_t*) &theMsg->fCLSIPrivate[0];
    pak = (paktype*) privateArea->fPakPtr;
    size = (byte*) theMsg - pak->data_area;
    return (size);
}
#endif

/* Create the  CLSI message for the specified primitive type. */
/* Allocates the buffer, and fills in the following  header fields. */
/* headerLength, primitiveCode, dataSize, selector, identifierType, */
/* and identifier. The rest of the header fields and the data area */
/* are the callers responsibility. It is guarenteed that there are at */
/* least dataSize bytes of space available for the user data. */

TCLSIMsg* CLSIMsgCreateHeader (hword primitiveCode,                                   
			       hword dataSize,
			       byte selector,
			       byte identifierType,
			       word identifier)
{
    TCLSIMsg* this;
    size_t size;

    size = CLSIGetHdrSize(primitiveCode);
    if (size == -1)
	return(NULL);
    
    this = CLSIMsgCreate(size + dataSize);
    if (this != NULL)
    {
	this->fHeaderLength = size;
	this->fPrimitiveCode = primitiveCode;
	this->fDataSize = dataSize;
	this->fSelector = selector;
	this->fIdentifierType = identifierType;
	this->fIdentifier = identifier;
    }
    return (this);
}

/* Return the size of the header for this primitive. */
/* Should redo this as a table. */

size_t CLSIGetHdrSize (hword primitiveCode)
{
    switch (primitiveCode) {
    case CLS_ACTIVATE_SAP|CLS_REQUEST:
	return CLS_ACT_SAP_REQ_HDR_SZ;

    case CLS_ACTIVATE_SAP|CLS_CONFIRM:
	return CLS_ACT_SAP_CNF_HDR_SZ;

    case CLS_DEACTIVATE_SAP|CLS_REQUEST:
	return CLS_DEACT_SAP_REQ_HDR_SZ;

    case CLS_DEACTIVATE_SAP|CLS_CONFIRM:
	return CLS_DEACT_SAP_CNF_HDR_SZ;

    case CLS_ENABLE|CLS_REQUEST:
	return CLS_ENABLE_REQ_HDR_SZ;

    case CLS_ENABLE|CLS_CONFIRM:
	return CLS_ENABLE_CNF_HDR_SZ;

    case CLS_FLOW|CLS_REQUEST:
	return CLS_FLOW_REQ_HDR_SZ;

    case CLS_FLOW|CLS_INDICATION:
	return CLS_FLOW_IND_HDR_SZ;

    case CLS_OPEN_STN|CLS_REQUEST:
	return CLS_OPEN_STN_REQ_HDR_SZ;

    case CLS_OPEN_STN|CLS_CONFIRM:
	return CLS_OPEN_STN_CNF_HDR_SZ;

    case CLS_REQ_OPNSTN|CLS_REQUEST:
	return CLS_REQ_OPNSTN_REQ_HDR_SZ;

    case CLS_REQ_OPNSTN|CLS_CONFIRM:
	return CLS_REQ_OPNSTN_CNF_HDR_SZ;

    case CLS_CLOSE_STN|CLS_REQUEST:
	return CLS_CLOSE_STN_REQ_HDR_SZ;

    case CLS_CLOSE_STN|CLS_CONFIRM:
	return CLS_CLOSE_STN_CNF_HDR_SZ;

    case CLS_CONNECT|CLS_REQUEST:
	return CLS_CONNECT_REQ_HDR_SZ;

    case CLS_CONNECT|CLS_CONFIRM:
	return CLS_CONNECT_CNF_HDR_SZ;

    case CLS_CONNECT|CLS_INDICATION:
	return CLS_CONNECT_IND_HDR_SZ;

    case CLS_CONNECT|CLS_RESPONSE:
	return CLS_CONNECT_RSP_HDR_SZ;

    case CLS_SIGNAL_STN|CLS_REQUEST:
	return CLS_SIGNAL_STN_REQ_HDR_SZ;

    case CLS_CONNECTED|CLS_INDICATION:
	return CLS_CONNECTED_IND_HDR_SZ;

    case CLS_DISCONNECT|CLS_REQUEST:
	return CLS_DISCONNECT_REQ_HDR_SZ;

    case CLS_DISCONNECT|CLS_CONFIRM:
	return CLS_DISCONNECT_CNF_HDR_SZ;

    case CLS_DISCONNECT|CLS_INDICATION:
	return CLS_DISCONNECT_IND_HDR_SZ;

    case CLS_DISCONNECT|CLS_RESPONSE:
	return CLS_DISCONNECT_RSP_HDR_SZ;
	
    case CLS_ID|CLS_REQUEST:
	return CLS_ID_REQ_HDR_SZ;

    case CLS_ID|CLS_CONFIRM:
	return CLS_ID_CNF_HDR_SZ;

    case CLS_ID|CLS_INDICATION:
	return CLS_ID_IND_HDR_SZ;

    case CLS_ID|CLS_RESPONSE:
	return CLS_ID_RSP_HDR_SZ;
	
    case CLS_DATA|CLS_REQUEST:
	return CLS_DATA_REQ_HDR_SZ;

    case CLS_DATA|CLS_CONFIRM:
	return CLS_DATA_CNF_HDR_SZ;

    case CLS_DATA|CLS_INDICATION:
	return CLS_DATA_IND_HDR_SZ;

    case CLS_DISABLE|CLS_REQUEST:
	return CLS_DISABLE_REQ_HDR_SZ;

    case CLS_DISABLE|CLS_CONFIRM:
	return CLS_DISABLE_CNF_HDR_SZ;

    case CLS_ACTIVATE_RING|CLS_REQUEST:
	return CLS_ACT_RING_REQ_HDR_SZ;

    case CLS_ACTIVATE_RING|CLS_CONFIRM:
	return CLS_ACT_RING_CNF_HDR_SZ;

    case CLS_DEACTIVATE_RING|CLS_REQUEST:
	return CLS_DEACT_RING_REQ_HDR_SZ;

    case CLS_DEACTIVATE_RING|CLS_CONFIRM:
	return CLS_DEACT_RING_CNF_HDR_SZ;

    case CLS_TEST|CLS_REQUEST:
	return CLS_TEST_REQ_HDR_SZ;

    case CLS_TEST|CLS_CONFIRM:
	return CLS_TEST_CNF_HDR_SZ;

    case CLS_TEST|CLS_INDICATION:
	return CLS_TEST_IND_HDR_SZ;

    case CLS_TEST|CLS_RESPONSE:
	return CLS_TEST_RSP_HDR_SZ;

    case CLS_UDATA|CLS_REQUEST:
	return CLS_UDATA_REQ_HDR_SZ;

    case CLS_UDATA|CLS_CONFIRM:
	return CLS_UDATA_CNF_HDR_SZ;

    case CLS_UDATA|CLS_INDICATION:
	return CLS_UDATA_IND_HDR_SZ;
 
    case CLS_TEST_STN|CLS_REQUEST:
	return CLS_TEST_STN_REQ_HDR_SZ;

    case CLS_TEST_STN|CLS_CONFIRM:
	return CLS_TEST_STN_CNF_HDR_SZ;

    case CLS_TEST_STN|CLS_INDICATION:
	return CLS_TEST_STN_IND_HDR_SZ;

    case CLS_TEST_STN|CLS_RESPONSE:
	return CLS_TEST_STN_RSP_HDR_SZ;

    case CLS_ID_STN|CLS_REQUEST:
	return CLS_ID_STN_REQ_HDR_SZ;

    case CLS_ID_STN|CLS_CONFIRM:
	return CLS_ID_STN_CNF_HDR_SZ;

    case CLS_ID_STN|CLS_INDICATION:
	return CLS_ID_STN_IND_HDR_SZ;

    case CLS_ID_STN|CLS_RESPONSE:
	return CLS_ID_STN_RSP_HDR_SZ;

    case CLS_UDATA_STN|CLS_REQUEST:
	return CLS_UDATA_STN_REQ_HDR_SZ;

    case CLS_UDATA_STN|CLS_CONFIRM:
	return CLS_UDATA_STN_CNF_HDR_SZ;

    case CLS_UDATA_STN|CLS_INDICATION:
	return CLS_UDATA_STN_IND_HDR_SZ;

    case CLS_CONNECT_STN|CLS_INDICATION:
	return CLS_CONNECT_STN_IND_HDR_SZ;

    case CLS_PONG|CLS_REQUEST:
	return CLS_PONG_REQ_HDR_SZ;

    case CLS_PONG|CLS_CONFIRM:
	return CLS_PONG_CNF_HDR_SZ;

    case CLS_MODIFY_STN|CLS_REQUEST:
	return CLS_MODIFY_STN_REQ_HDR_SZ;

    case CLS_MODIFY_STN|CLS_CONFIRM:
	return CLS_MODIFY_STN_CNF_HDR_SZ;

    default:
	CLSASSERT(FALSE);		/* Error, primitive not in table */
	return(-1);
    }
}

TCLSIMsg* CLSIMsgCreateFromPak(paktype* pak,
			       hword primitiveCode,
			       byte selector,
			       byte identifierType,
			       word identifier,
			       size_t varSize,
			       hword* reservedSize)
{
    TCLSIMsg* msgPtr;
    CLSIPrivate_t* privateArea;
    size_t headerSize;

    /* Make sure that we got something legal */
    if (pak == NULL)
	return(NULL);
    
    /* Calculate size of header to add */

    switch (primitiveCode)
    {
    case CLS_ID | CLS_CONFIRM:
    case CLS_ID | CLS_INDICATION:
	headerSize = offsetof(IdInd_t, fXID);
	break;
    case CLS_DATA | CLS_INDICATION:
	headerSize = offsetof(DataInd_t, fData);
	break;
    case CLS_DATA | CLS_REQUEST:
	headerSize = offsetof(DataReq_t, fData);
	break;
    case CLS_ID_STN | CLS_INDICATION:
	headerSize = offsetof(IdStnInd_t, fXIDData);
	break;
    case CLS_CONNECTED | CLS_INDICATION:
	headerSize = offsetof(ConnectedInd_t, fCorrelator);
	break;

    case CLS_UDATA | CLS_INDICATION:
        headerSize = offsetof(UDataInd_t, fUData);
        break;
    case CLS_UDATA_STN | CLS_INDICATION:
        headerSize = offsetof(UDataStnInd_t, fUData);
        break;
    case CLS_TEST | CLS_INDICATION:
        headerSize =  offsetof(TestInd_t, fTESTLength);
        break;
    case CLS_TEST | CLS_CONFIRM:
        headerSize =  offsetof(TestCnf_t, fTESTLength);
        break;
    case CLS_TEST_STN | CLS_INDICATION:
        headerSize =  offsetof(TestStnInd_t, fTESTLength);
        break;
    case CLS_CONNECT_STN | CLS_INDICATION:
        headerSize =  offsetof(ConnectStnInd_t, fCONNECTLength);
        break;

    default:
	return(NULL);
	break;
    }

    
    /* Adjust the packet structure */
    pak_delta_adjust_start(pak, -headerSize);

    /*
     * Make sure header is long word aligned.
     *
     * The way this works is we back up the start of the clsi header
     * until it starts on a 4 byte boundary. This will leave a space
     * between the end of the clsi header and the start of the data that
     * was originally pointed at by datagramstart. The "reservedSize"
     * value is the size of this space and will be in the range 0-3.
     * The caller must save this value into the clsi header of the
     * primitive it is building, and the DLU must use it when attempting
     * to find the start of the data field. 
     */
    *reservedSize =  ((ulong) (pak->datagramstart)) & 0x03;
    pak_delta_adjust_start(pak, -(*reservedSize));

    /* Fill in TCLSIMsg fields */

    msgPtr = (TCLSIMsg*) pak->datagramstart;
    msgPtr->fLinkPtr = NULL;
    msgPtr->fHeaderLength = CLSIGetHdrSize(primitiveCode);
    msgPtr->fPrimitiveCode = primitiveCode;
    msgPtr->fDataSize = pak->datagramsize - msgPtr->fHeaderLength;
    msgPtr->fSelector = selector;
    msgPtr->fIdentifierType = identifierType;
    msgPtr->fIdentifier = identifier;

    privateArea = (CLSIPrivate_t*) &msgPtr->fCLSIPrivate[0];
    privateArea->fArg1 = pak;
    privateArea->fDestructor = (void (*)(void*)) datagram_done;
    return(msgPtr);
}

paktype* CLSIMsgGetPak(TCLSIMsg* theMsg)
{
    CLSIPrivate_t* privateArea;
    
    privateArea = (CLSIPrivate_t*) &theMsg->fCLSIPrivate[0];
    return(privateArea->fArg1);
}

llctype* CLSIMsgGetLlc(TCLSIMsg* theMsg)
{
    TCLSCep* theCep;
    
    theCep = (TCLSCep*) theMsg->fIdentifier;
    return((llctype *)theCep->fDLCPCepId);
}

void CLSIMsgSetSize (TCLSIMsg* theMsg, size_t size)
{
    CLSIPrivate_t* privateArea;
    paktype* pak;

    privateArea = (CLSIPrivate_t*) &theMsg->fCLSIPrivate[0];
    pak = (paktype*) privateArea->fArg1;
    pak->datagramsize = size;
}

size_t CLSIMsgGetSize (TCLSIMsg* theMsg)
{
    CLSIPrivate_t* privateArea;
    paktype* pak;
    
    privateArea = (CLSIPrivate_t*) &theMsg->fCLSIPrivate[0];
    pak = (paktype*) privateArea->fArg1;
    return (pak->datagramsize);
}

ActRngCnf_t* BuildActRngCnf(ActRngReq_t* theReq, hword retCode)
{
    ActRngCnf_t* theCnf;
 
    theCnf = (ActRngCnf_t*) CLSIMsgCreateHeader(CLS_ACTIVATE_RING | CLS_CONFIRM,
                                                sizeof(ActRngCnf_t)
                                                - sizeof(ActRngCnfh_t),
                                                CLS_LLC_LAYER,
                                                CLS_U_SAP_ID,
                                                theReq->fCLSIHdr.fUSapId);
    if  (theCnf != NULL)
    {
        CLSIMsgSetReturnCode((TCLSIMsg*) theCnf, retCode);
        theCnf->fVersion = CLSI_VERSION;
        theCnf->fCorrelator = theReq->fCorrelator;
        theCnf->fPortId = theReq->fPortId;
        theCnf->fDLCType = theReq->fDLCType;
    }
 
    return(theCnf);
}

/*
 * Returns a pointer to the Remote Mac address. 
 * If parameter remoteMac is not NULL, then also
 * builds a hwaddrtype structure containing the remote
 * mac address.
 */
byte* CLSIIdStnGetRemoteMac(const IdStnInd_t* idStn, hwaddrtype* remoteMac)
{
    byte* macPtr = ((byte*) idStn) + idStn->fRemoteMacOffset;
    
    if (remoteMac != NULL)
    {
	snpa_init(remoteMac, idStn->fMacType, TARGET_UNICAST, macPtr);
	if (snpa_get_type(remoteMac) == STATION_IEEE48)
	{
	    remoteMac->addr[0] &= ~TR_RII;	/* Turn off RII bit */
	}
    }
    return macPtr;
}

boolean CLSIPortIdCopy (CLSPortID* srcPort, CLSPortID* dstPort)
{

     if ((dstPort == NULL) || (srcPort == NULL))
      return(FALSE);
     dstPort->type = srcPort->type;
     switch (srcPort->type)
     {
       case CLS_P_TYPE_SWIDB:
          dstPort->fTypeInfo.swidb = srcPort->fTypeInfo.swidb;
          break;
       default:
          return(FALSE);
          break;
     }
     return(TRUE);
}

boolean CLSIPortIdEqual (CLSPortID* portA, CLSPortID* portB)
{

     switch (portB->type)
     {
       case CLS_P_TYPE_SWIDB:
        if (portA->fTypeInfo.swidb != portB->fTypeInfo.swidb)
              return(FALSE);
          break;
       default:
        return(FALSE);
        break;
     }
     return(TRUE);
}

/*
 * Accessor functions for getting information from CLSPortID.
 * 
 * idbtype *CLSIPortIdGetSwIdb(CLSPortID *clsPortID);
 * byte CLSIPortIdGetPortNumber(CLSPortID *clsPortID);
 * byte CLSIPortIdGetSlotNumber(CLSPortID *clsPortID);
 * byte CLSIPortIdGetSubNumber(CLSPortID *clsPortID);
 * CLSDLCType_e CLSIPortIdGetPortType(CLSPortID *clsPortID);
 *
 */
idbtype *CLSIPortIdGetSwIdb(CLSPortID *clsPortID)
{
    switch (clsPortID->type)
    {
    case CLS_P_TYPE_SWIDB:
        {
	idbtype *swidb = clsPortID->fTypeInfo.swidb;
	return(swidb);
	break;
        }
    case CLS_P_TYPE_ILLEGAL:
    default:
	CLSASSERT(FALSE);	/* bitch about bad port id type! */
	return(NULL);
	break;
    }
}

byte CLSIPortIdGetPortNumber(CLSPortID *clsPortID)
{
    return(CLSIPortIdGetUnitNumber(clsPortID));
}

byte CLSIPortIdGetUnitNumber(CLSPortID *clsPortID)
{
    switch (clsPortID->type)
    {
    case CLS_P_TYPE_SWIDB:
    {
	hwidbtype *idb = hwidb_or_null(clsPortID->fTypeInfo.swidb);
	/*
	 * TBRIDGE idbs will pass in a NULL during port enable,
	 * because the idb is created within the Enable and we don't
	 * have a priori knowledge of where the idb will be.  DLSw
	 * likes to know about port types, unit and slot numbers
	 * (p/u/s) for now.  It likes to store these values before
	 * doing a port Enable.  The interim solution is to return the
	 * vidb_unit number, which we _DO_ know apriori and let DLSw
	 * chew on it.  It will pass it down in the port enable and
	 * DLSw and CLS/TBRIDGE will be in agreement as to how to
	 * identify this port.  The final solution is to have DLSw
	 * implement *swidbs instead of p/u/s... [01/31/95,
	 * ccartee]
	 */
	if (clsPortID->fTypeInfo.swidb == NULL)
	    return(get_current_vidb_unit());

	if (idb != NULL)
	{
	    /*
	     * Vidbs store their 'ring numbers' in idb->unit, so
	     * rather than present DLSw with a big unit number, use a
	     * smaller port number which is stashed away in
	     * idb->hw_if_index. Look at comment in dlc_new_vidb() for
	     * more gory details and verbose prose on this topic...
	     */
	    if (idb->status == (IDB_VIRTUAL | IDB_TR))
                if (reg_invoke_cls_is_tbridge_type(idb))
                    return(idb->hw_if_index); /* TBRIDGE's unit number... */
                else
                    return(idb->unit); /* RSRB's unit number... */
	    else
		return(idb->unit); /* Everyone else's unit number... */
	} else
	{
	    /* Current error condition.  This is a bad value as it is
	     * also a legal value.  Until we agree on what
	     * legal/illegal values are, leave the comment and hope
	     * someone sees the ASSERT! (Same goes for
	     * CLS_P_TYPE_ILLEGAL case below)
	     */
	    CLSASSERT(FALSE);
	    return(0);	
	}
    } /* endCase CLS_P_TYPE_SWIDB */
	break;

    case CLS_P_TYPE_ILLEGAL:
    default:
	CLSASSERT(FALSE);	/* bitch about bad port id type! */
	return(0);
	break;
    }
}


byte CLSIPortIdGetSlotNumber(CLSPortID *clsPortID)
{
    switch (clsPortID->type)
    {
    case CLS_P_TYPE_SWIDB:
    {
	hwidbtype *idb = hwidb_or_null(clsPortID->fTypeInfo.swidb);
	/* See comment in byte CLSIPortIdGetUnitNumber(CLSPortID
	 * *clsPortID) regarding illegal return values...
	 */
	return((idb == NULL) ? 0 : idb->slot);
    }
	break;

    case CLS_P_TYPE_ILLEGAL:
    default:
	CLSASSERT(FALSE);	/* bitch about bad port id type! */
	return(0);
	break;
    }
}

byte CLSIPortIdGetSubNumber(CLSPortID *clsPortID)
{
    switch (clsPortID->type)
    {
    case CLS_P_TYPE_SWIDB:
    {
	idbtype *swidb = clsPortID->fTypeInfo.swidb;
	return((swidb == NULL) ? 0 : swidb->sub_number);
    }
	break;

    case CLS_P_TYPE_ILLEGAL:
    default:
	CLSASSERT(FALSE);	/* bitch about bad port id type! */
	return(0);
	break;
    }
}

/*
 * get_cls_type
 *
 * Given an idb, what's its cls type
 */

inline CLSDLCType_e get_cls_type (hwidbtype *idb)
{
    CLSDLCType_e port_type;

    if (idb == NULL)
        return(CLS_ILLEGAL_DLC);
    /*
     * Logic taken verbatim from old dlsw_set_srb_params(), from
     * revision 1.31...
     */
    if (idb->enctype == ET_SDLC)
        port_type = CLS_SDLC;
    else if (idb->enctype == ET_X25)
        port_type = CLS_QLLC;
    else if (idb->status & IDB_ETHER)
        port_type = CLS_LLC;
    else if (idb->status == (IDB_VIRTUAL | IDB_TR)) {
        if (reg_invoke_cls_is_tbridge_type(idb))
            port_type = CLS_TBRIDGE;
        else
            port_type = CLS_RSRB;
    } else if (idb->enctype == ET_NDLC)
        port_type = CLS_NCIA;
    else if (idb->status == IDB_VIRTUAL)
        port_type = CLS_VDLC;
    else
        port_type = CLS_LLC;

    return(port_type);
}

/*
 *  Function is _NOT_ generic across clsPortID->type's!  It is not
 *  possible to derive hwidb->status from port/slot/unit, as these do
 *  not define a unique interface in the box.  For example, a box may
 *  have Ethernet 0/1 and a Token-Ring 0/1 as well.  If we are asked
 *  to derive status from p/s/u, bitch!
 *
 *  Function only works with *swidbs, as this uniquely identifies an
 *  interface in the box!
 */
CLSDLCType_e CLSIPortIdGetPortType(CLSPortID *clsPortID)
{
    hwidbtype *idb;

    /*
     * grab the idb however is appropriate.
     */
    switch (clsPortID->type)
    {
    case CLS_P_TYPE_SWIDB:
	idb = hwidb_or_null(clsPortID->fTypeInfo.swidb);
	if (idb == NULL) 
	    return(CLS_TBRIDGE);
	break;

    case CLS_P_TYPE_ILLEGAL:
    default:			/* no idb to be found! */
	idb = NULL;
	break;
    }

    if (idb == NULL)
	return(CLS_ILLEGAL_DLC);

    return (get_cls_type(idb));
}

/*
 * Returns a pointer to the Remote Mac address. 
 * If parameter remoteMac is not NULL, then also
 * builds a hwaddrtype structure containing the remote
 * mac address.
 */
byte* CLSIConnectStnGetRemoteMac(const ConnectStnInd_t* connectStn, hwaddrtype* remoteMac)
{
    byte* macPtr = ((byte*) connectStn) + connectStn->fRemoteMacOffset;
    
    if (remoteMac != NULL)
    {
	snpa_init(remoteMac, connectStn->fMacType, TARGET_UNICAST, macPtr);
	if (snpa_get_type(remoteMac) == STATION_IEEE48)
	{
	    remoteMac->addr[0] &= ~TR_RII;	/* Turn off RII bit */
	}
    }
    return macPtr;
}

