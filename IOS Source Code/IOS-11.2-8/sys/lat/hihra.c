/*-----------------------------------------------------------------------
    hi_h_ri_action	(i) act on response messages

    Copyright 1991 Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is  used on subject systems to process a HIC response
    message when the solicit information message retry count expires.
    Setup to issue the command message or issue the start slot if the
    object only supports starts (and we are configured as a server).

    Response information messages are gathered by hi_h_ri_message and
    the one offering the service with the best rating is retained for
    processing by this routine.

    CALL BY:	void hi_h_ri_action(hi)
		hi_ptr  hi;

    ON EXIT:	The matching HIC entry processed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	nd_temp[];		/* Local node name storage	*/
extern	byte	as_temp[];		/* Local service name storage	*/

void hi_h_ri_action(hi)

    hi_ptr	hi;			/* Pointer to HI structure	*/
{

/*-----------------------------------------------------------------------
    Put the saved node name and node address in the HI structure.
 -----------------------------------------------------------------------*/
    	hi_str_move_ls(&nd_temp[0],&hi->hi_save_node[0]);
	hi_str_move_sl(&hi->hi_node[0],&nd_temp[0]);
		
	hi->hi_dest_addr[0]  = hi->hi_save_addr[0];
	hi->hi_dest_addr[1]  = hi->hi_save_addr[1];
	hi->hi_dest_addr[2]  = hi->hi_save_addr[2];
	hi->hi_idb = hi->hi_save_idb;

#if LE_SERVER
/*-----------------------------------------------------------------------
    If the remote system only supports start messages, issue the HI_OPEN.
    If the open fails, we issue a reject.
 -----------------------------------------------------------------------*/
    if ((hi->hi_save_status & RI_START) && !(hi->hi_save_status & RI_COMMAND))
	hx_session(hi,HX_ACCEPT);
    else
#endif

/*-----------------------------------------------------------------------
    If the remote system does not support command message, it is not
    possible to make a connection. Ignore message. Declare cannot start
    error if a specific node was named.
 -----------------------------------------------------------------------*/
    if (!(hi->hi_save_status & RI_COMMAND))
    {
	if (hi->hi_node[0] != 0)
	{
	    hi->hi_status = STS_ERROR;
	    hi->hi_error  = SLOT_STOP_CANNOT_START;
	    hi_delete(hi,HX_REJECTED);
	}
    }

/*-----------------------------------------------------------------------
    Setup to issue command to remote node. Generate a new request
    number so other response messages are ignored. If we specified a
    service name, check to see that the name is in the response
    message.
 -----------------------------------------------------------------------*/
    else
    {
	hi->hi_function = LAT_COMMAND;
	hi->hi_tics     = 1;
	hi->hi_retries  = hs->hs_hi_cmd_retries;

	while (hi_find(++hs->hs_hi_req_no,0,HI_SUBJECT,0) != 0 || hs->hs_hi_req_no == 0)
	    ;
	hi->hi_req_no = hs->hs_hi_req_no;
    }
}
