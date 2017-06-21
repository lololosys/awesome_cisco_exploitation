/* $Id: ilmi_public.h,v 3.4.18.1 1996/05/09 14:11:31 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_public.h,v $
 *------------------------------------------------------------------
 * Placeholder for ILMI support
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_public.h,v $
 * Revision 3.4.18.1  1996/05/09  14:11:31  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.44.1  1996/04/27  06:33:54  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.34.2  1996/04/10  23:35:52  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.34.1  1996/04/08  01:44:35  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.24.1  1996/03/25  02:18:28  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.2.1  1996/03/21  21:50:52  cakyol
 * Kartik's fix for "ILMI hung" problem picked up manually.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4  1996/02/09  18:36:22  kartik
 * CSCdi48139:   Fixed ILMI request ID counter wrap-around which occurs
 * after the ILMI is up for an extended time.
 *
 * Revision 3.3  1995/12/05  22:30:01  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.2  1995/11/17  08:47:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  20:31:47  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:12:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "../snmp/snmp_api.h"

#ifndef _ILMI_PUBLIC_H_ 
#define _ILMI_PUBLIC_H_ 

/*
 * General Definitions
 */

#define ILMI_ERROR   			-1
#define ILMI_OK      			0

/*
 * ILMI addressing related
 */

#define ILMI_NETPREFIX_LEN      13
#define ILMI_ATMADDRESS_LEN     20
#define CLIENTNAMESIZE          40


typedef void (* ilmiNotifyCB)(void *);
typedef int ilmiClient;
typedef int ilmiErrcode;
typedef uint ilmiIntf;
typedef ushort ilmiOper;
typedef ushort ilmiReqID;
typedef short ilmiEvContext;
typedef char * ilmiLabel;


typedef enum ilmiClientNotifyType_ {
	ilmiAddressAdd,
	ilmiAddressDel,
	ilmiAddressValidate,
	ilmiTrapNotify,
	ilmiResponseNotify,
	ilmiLinkDownNotify,
	ilmiUniVersionNotify
}ilmiClientNotifyType;

typedef enum ilmiFunctionType_ {
	ilmiPoll,
	ilmiAddressTableCheck,
	ilmiUniVersion,
	ilmiNeighborsIp,
	ilmiNeighborsIfName,
	ilmiReqOther
}ilmiFunctionType;

typedef enum ilmiOperType_ {
	ilmiGet = GET_REQUEST_TYPE,
	ilmiGetNext = GET_NEXT_REQUEST_TYPE,
	ilmiGetResp = GET_RESPONSE_TYPE,
	ilmiSet = SET_REQUEST_TYPE,
	ilmiTrap = TRAP_TYPE,
}ilmiOperType;

typedef enum ilmiTrapType_ {
	ilmiColdStart,
	ilmiVCCChange
}ilmiTrapType;

typedef enum ilmiVCCChangeTrapType_ {
    ilmiOther,
    ilmiUnknown,
	ilmiEnd2endUp,
	ilmiEnd2endDown,
	ilmiLocalUpEnd2endUnknown,
	ilmiLocalDown
}ilmiVCCChangeTrapType;

typedef enum ilmiAddrState_ {
	ilmiAddressValid = 1,
	ilmiAddressInValid = 2
}ilmiAddrState;

/*
 * The ILMI Address types.
 */

 typedef enum ilmiAddrType_{
 	ilmiPrefixAddress,
 	ilmiHostddress,
 }ilmiAddrType;

typedef enum ilmiAddressTestType_ {
    ilmiAddressNoMatch,
    ilmiAddressMatchWithSEL,
    ilmiAddressMatchWithoutSEL
}ilmiAddressTestType;

/*
 * The ILMI Error Types
 */

typedef enum ilmiErrorType_ {
	ilmiIllegalReq,
	ilmiReqEncodeError,
	ilmiSendPduError,
	ilmiBadClientReq,
	ilmiBadIntfBlock,
	ilmiBadNotifyType,
	ilmiIllegalTrapType,
	ilmiAddrDupError,
	ilmiEncapError,
	ilmiNoError,
	ilmiNoResponse,
	ilmiLocalAddFailure,
	ilmiBadObject,
	ilmiResponseReceived
} ilmiErrorType;

/* 
 * 1-1 matching with SNMP Error is assumed here.
 */
typedef enum ilmiResponseType_ {
	ilmiRespNoErr,
	ilmiRespTooBigErr,
	ilmiRespNoSuchNameErr,
	ilmiRespBadValueErr,
	ilmiRespReadOnlyErr,
	ilmiRespGenErr
}ilmiResponseType;

/*
 * The various stages in the ILMI address registration process.
 */
typedef enum ilmiIntfState_ {
 	ilmiIntfDown,
 	ilmiIntfRestarting,
 	ilmiIntfAwaitRestartAck,
 	ilmiIntfUpAndNormal
 }ilmiIntfState;

 typedef enum ilmiEventType_ {
 ilmiPeriodicPoll,
 ilmiPeriodicAddressReg,
 ilmiPeriodicReqRetry,
 ilmiPeriodicPeerAddressCheck
 }ilmiEventType;



/*
 * The address of this structure is passed in the API calls for notifier
 * registration and request (ilmi_register_service(), ilmi_client_request()).
 * The ILMI then fills up the necessary fields before passing the address
 * of this structure in the callbacks.
 */



typedef struct ilmiClientReq_ {
 	ilmiClient              client_id;       /* Client Identification */
 	ilmiIntf                intf_id;         /* Interface Identification */
 	ilmiClientNotifyType    notify_type;     /* API notification type. */
 	unsigned long           transid;         /* The Client transaction ID */
 	ilmiOperType            oper_type;       /* The Operation Type.
 	                                            GET/GETNEXT/SET */
 	ilmiErrorType           error;           /* Error Code */
	ilmiResponseType        response;        /* What kind of response 
						    received. Maps to SNMP V1 
                                                    error code. Set if and 
                                                    when a response is received 
						  */
 	ilmiTrapType 		trap_type;       /* What trap was received */
 	VarBind                 *vblist;         /* List of Varbinds. Currently
                                                    should have only one 
                                                    element.
                                                 */
	OctetString		*address;        /* Used to report address 
						    ref.*/
 	ilmiNotifyCB            notifyCB;        /* Notifier Callback function. */
	int                     myUniVersion;
}ilmiClientReq;


/* --------------------------------------------------------------------
 * ILMI Initialization
 * ilmi_init(void)
 * RETURN :  void
 * PARAMETERS :  none
 * This function is called only once to initialize all ILMI related data
 * structures including queues. It creates ILMI control blocks for all
 * the physical ATM interfaces if any. Most important of all it defines 
 * and registers the ILMI process.
 */
extern void      ilmi_init(void) ;

extern boolean   ilmi_create_event(int, int, int, int);
extern void      ilmi_destroy_event(int, int, int);
extern boolean   ilmi_send_trap(uint,ushort,int,int,int );
extern void      ilmi_show_command(hwidbtype *);



#endif
      
