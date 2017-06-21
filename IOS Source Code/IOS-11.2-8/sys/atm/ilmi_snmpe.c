/* $Id: ilmi_snmpe.c,v 3.5.26.4 1996/08/09 02:13:33 yanke Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_snmpe.c,v $
 *------------------------------------------------------------------
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_snmpe.c,v $
 * Revision 3.5.26.4  1996/08/09  02:13:33  yanke
 * send queries for uni version after receives traps.
 *
 * CSCdi54317:  ilmi uni-version negotiation doesnt work properly sometimes
 * Branch: California_branch
 *
 * Revision 3.5.26.3  1996/08/07  08:56:47  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.26.2  1996/05/09  14:11:33  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.26.1.18.1  1996/04/27  06:33:55  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.26.1.8.2  1996/04/10  23:35:54  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.26.1.8.1  1996/04/08  01:44:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.26.1  1996/03/18  19:00:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.30.1  1996/03/25  02:18:30  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.12.1  1996/03/21  21:50:54  cakyol
 * Kartik's fix for "ILMI hung" problem picked up manually.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/01/21  22:30:31  bbenson
 * CSCdi47237:  Spurious memory access in ilmi_snmp_callback
 * Call to ilmi_state_transit should be off of intfblk instead of msg.
 *
 * Revision 3.4  1996/01/02  18:27:38  bbenson
 * CSCdi46281:  ILMI should consider trap as indication of empty
 * atm-address table.
 * ILMI on the router should reset to initial state whenever it receives a
 * trap from the switch
 *
 * Revision 3.3  1995/12/05  22:30:03  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.2  1995/11/17  08:47:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/08  05:33:32  schrupp
 * CSCdi38341:  Dont cancel ATM NSAP if never registered
 *
 * Revision 2.1  1995/06/07  20:12:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "address.h"
#include "../if/atm.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "ilmi_public.h"
#include "ilmi_private.h"
#include "ilmi_api.h"

char *const ilmi_snmp_errors[] = {"No Error",	"Too Big",
                                  "No Such Name", "Bad Value",
				  "Read Only", "General Error"};



/* 
 * Note that you can use pdu->type to demux on trap vs response, and can
 * use pdu->u.normpdu.request_id to sync responses with requests.
 */


/* ---------------------------------------------------------------------
 * This function is registered with the SNMP Engine and is called by the
 * Engine whenever it receives a reponse to a request or a trap.
 * Note: We currently handle only one object per request/response. 
 * Response : Return False for responses if the request Id does not match.
 * Traps    :
 * 
 */


boolean
ilmi_snmp_callback (Pdu *pdu)
{
    ilmi_msgblock *msg=NULL;
    addrtype *intf;
    ilmi_intfblock *intfblk;

    /* Interface on which the trap came in. */
    intf = reg_invoke_snmp_packet_source();
    if((!intf) || 
        ( (intfblk = ilmi_getintfblk (intf->addr.ilmi_address)) == NULL)) {
        ilmi_log_error(ILMI_ID,ilmiBadIntfBlock,"<ilmi_send_pkt>");
        return (TRUE);
    }

    if(pdu->type == TRAP_TYPE) {
        switch(pdu->u.trappdu.generic_trap) {
        case COLD_START_TRAP :
        case LINK_UP_TRAP    :
        case WARM_START_TRAP :
            if(atm_ilmi_debug)
		buginf("\nILMI: Trap Received (%s)",
		       ilmi_namestring_forintf(intfblk));

	    ilmi_clear_addrTab(intfblk);
	    ilmi_delete_localdb(intfblk);
	    /* Get the latest values for Uni Version, Neighbor's IP
	     * and IfName
	     */
	    ilmi_initial_sets(intfblk);
	    ilmi_create_event(intfblk->intf_id, ilmiPeriodicAddressReg,
		ILMI_PREFIX_WAITTIME,0);
            break;
        case ENTERPRISE_TRAP :
            break;
        default:
            break;
        }
        return (TRUE);
    }
    else if (pdu->type == GET_RESPONSE_TYPE) {

        /*
         * It is a response to one of our earlier requests.
         * Remove the corresponding msg from the outstanding
         * request hash and put it on the  response queue.
         * If none found drop the response and return quietly.
         */

        msg = ilmi_reqmsg_hashretreive(pdu->u.normpdu.request_id);
        if(!msg)
            return (TRUE);

	/* If removed from the hash remove from the event queue also */
	ilmi_destroy_event(msg->intf_id, ilmiPeriodicReqRetry, msg->req_id);
	

        /*
         * Dequeue the message from the request awaiting queue. update it
         * and put it in the response queue.
         */

        if((atm_ilmi_debug) && (msg->ilmi_func == ilmiPoll))
            buginf("\nILMI: Poll Response received. (%s)",
		   ilmi_namestring_forintf(intfblk));
        enqueue(&ilmi_responseQ,msg);

        msg->errcode = ilmiResponseReceived;
        if(pdu->u.normpdu.error_status == NO_ERROR) {
            msg->response = ilmiRespNoErr;
            FreeVarBind(msg->vblist);
            msg->vblist = CloneVarBind(pdu->var_bind_list);

	    if (atm_ilmi_debug) {
		buginf("\nILMI: VALID_RESP_RCVD ");
		ilmi_dump_msgblock(msg);
	    }

        }
        else {
            msg->response = pdu->u.normpdu.error_status;
            if(atm_ilmi_debug) {
		buginf("\nILMI: ERROR_RESP_RCVD (%s) ",ilmi_snmp_errors[msg->response]);
		ilmi_dump_msgblock(msg);
	    }
        }
        return (TRUE);
    }
    /* 
     * Not for us 
     */
    return (FALSE);
}




      
