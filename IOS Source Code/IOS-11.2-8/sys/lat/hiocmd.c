/*-----------------------------------------------------------------------
    hi_o_cmd_message	(i) output command message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to output a command message.

    CALL BY:	void hi_o_cmd_message(of,hi)
		of_ptr	of;
		hi_ptr	hi;

    ON EXIT:	Appropriate message sent to remote node.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latob.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/

void hi_o_cmd_message(of,hi)

    of_ptr	of;			/* Output frame pointer		*/
    hi_ptr	hi;			/* HIC request pointer		*/
{
    cmd_optr	cmd;			/* Output command message	*/
    of_byte	next;			/* Output byte pointer		*/

/*-----------------------------------------------------------------------
    Fill in the fixed fields of a solicit host-initiated message.
 -----------------------------------------------------------------------*/
    cmd = (cmd_optr)of->of_data;
    next = of->of_data + sizeof(cmd_msg);

    cmd->cmd_type      = hi->hi_function;
    cmd->cmd_format    = 0;
    cmd->cmd_prot_hi   = hs->hs_prot_hi;
    cmd->cmd_prot_lo   = hs->hs_prot_lo;
    cmd->cmd_prot_ver  = hs->hs_prot_ver;
    cmd->cmd_prot_eco  = hs->hs_prot_eco;
    cmd->cmd_dll_max   = (hs->hs_inp_max != 0) ? swap(hs->hs_inp_max) : swap(hs->hs_dll_max);
    cmd->cmd_req_no    = swap(hi->hi_req_no);
    cmd->cmd_que_no    = swap(hi->hi_que_no);
    cmd->cmd_operation = hi->hi_command;
    cmd->cmd_modifier  = hi->hi_modifier;

/*-----------------------------------------------------------------------
    Complete the variable fields in a command message.
 -----------------------------------------------------------------------*/
    next = (of_byte)hi_str_move_os(next,&hi->hi_node[0]);

    next = (of_byte)hs_str_move_os(next,&hs->hs_acl_group[0]);

    next = (of_byte)hs_str_move_os(next,&hs->hs_node[0]);

#if (LE_OBJECTS || CME_SLAT)
#if LE_OBJECTS
    if (hi->hi_ob != 0)
        next = (of_byte)ob_str_move_os(next,&hi->hi_ob->ob_name[0]);
#elif CME_SLAT
    if (hi->hi_cid != CID_NULL)
	next = (of_byte)cx_move(next,hi->hi_cid,CX_PORT_NAME);
#endif
    else
        *next++ = 0;
#else
    *next++ = 0;
#endif

    *next++ = 0;
    next = (of_byte)hi_str_move_os(next,&hi->hi_service[0]);
    next = (of_byte)hi_str_move_os(next,&hi->hi_port[0]);
    if (hi->hi_pass[0] != 0)
    {
    	*next++ = CMD_PASSWD;
	next = (of_byte)hi_str_move_os(next,&hi->hi_pass[0]);
    }

/*-----------------------------------------------------------------------
    Set parameter list end and compute length of message.
 -----------------------------------------------------------------------*/
    *next++ = 0;
    of->of_size = (int)(next - of->of_data);
}
