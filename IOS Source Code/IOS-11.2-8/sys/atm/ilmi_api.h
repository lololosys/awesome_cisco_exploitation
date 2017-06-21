/* $Id: ilmi_api.h,v 3.2 1995/11/17 08:46:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm/ilmi_api.h,v $
 *------------------------------------------------------------------
 * Placeholder for ILMI support
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_api.h,v $
 * Revision 3.2  1995/11/17  08:46:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:12:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#ifndef _ILMI_API_H_ 
#define _ILMI_API_H_ 


#include "ilmi_public.h"


/* 
 *                               A P I 
 *
 * All the parameters passed to the ILMI are  read as data. The 
 * Client is responsible for managing the memory for these.
 *
 * All through the code it is asummed that the VarBind list has
 * only one VarBind in it.
 */

/* --------------------------------------------------------------------
 * ilmi_register_client(ilmiLabel)
 * RETURN TYPE: ilmiCient 
 * PARAMETERS : none
 * This function is called once by each client to register itself. From
 * that point on the Client uses the returned value (Client ID) as a 
 * parameter for the other API calls to the ILMI.
 */

extern ilmiClient ilmi_register_client(ilmiLabel) ;



/* -------------------------------------------------------------------
 * ilmi_register_service(ilmiClientReq *) ;
 * RETURN : ILMI_OK/ILMI_ERROR
 * This function is called by a Client to register a particular service
 * with the ILMI. The service could be one of the different registration
 * types defined as enumerated of type ilmiClientNotifyType.
 * A pointer to the ilmiClientReq structure is passed with all the required
 * fields filled in. These are:
 * 1.  client_id - The calling entity's ID value.
 * 2.  msgtype   - The type of notification.
 * 3.  notifyCB  - The callback that is required to be attached as per the
 *                 message type.
 */

extern int ilmi_register_service(ilmiClientReq  *) ;




/* -------------------------------------------------------------------
 * ilmi_client_request(ilmiClientReq *);
 * RETURN : ILMI_OK/ILMI_ERROR
 * This function is called by the Client to sppecify a ILMI request.
 * A pointer to the ilmiClientReq structure is passed with all the required
 * fields filled in. These are:
 * 1.  client_id      - The calling entity's ID value.
 * 2.  intf_id        - The UNI that client specifies.
 * 3.  ilmiOperType   - The type of notification.
 * 4.  transid        - The Client transaction ID
 * 5.  VarBind        - The OID-value pair which forms the varbind. Only one
 *                      pair should be specified as of now.
 */

extern int ilmi_client_request(ilmiClientReq *);



/* -------------------------------------------------------------------
 * ilmi_get_linkstatus(ilmiIntf)
 * RETURN : The Link Status for the given interface (boolean).
 *          TRUE/FALSE  - Up/Down
 * This is a direct call to get the current status of a interface.
 */

extern boolean ilmi_get_linkstatus(ilmiIntf);


/* -------------------------------------------------------------------------
 * This function allows a Client to traverse through the address table.
 * The Clients should not modify the returned memory address or the 
 * contents of the memory address. If the specified address is NULL
 * the first address (prefix/host-address) is returned.
 */
	     
extern OctetString *ilmi_addrtraverse(ilmiIntf, OctetString *);


/* -------------------------------------------------------------------------
 * Called by Client to unregister itself. 
 */
extern void ilmi_unregister_client (ilmiClient);

#endif
