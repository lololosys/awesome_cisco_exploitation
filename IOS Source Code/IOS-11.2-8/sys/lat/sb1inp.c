/* $Id: sb1inp.c,v 3.2 1995/11/17 17:34:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sb1inp.c,v $
 *------------------------------------------------------------------
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sb1inp.c,v $
 * Revision 3.2  1995/11/17  17:34:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*----------------------------------------------------------------------
    sb_1_slot_inp 	(i) process service class 1 input slots

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to process a service class 1 input slot.

    CALL BY:	void sb_1_slot_inp(ptr,sb,cb)
		if_byte ptr;
		sb_ptr	sb;
    		cb_ptr	cb;

    ON EXIT:	Returns when the slot processed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latob.h"
#include "latsb.h"
#include "latas.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
extern  byte	ob_temp[];		/* Temporary OB name		*/
extern	byte	id_temp[];		/* Temporary password		*/
extern	byte	as_temp[];		/* Temporary AS name		*/
#if LE_OBJECTS
extern	ob_ptr	ob_map[];		/* Object mapping table		*/
extern	int	ob_highest;		/* Largest number objects used	*/
#elif CME_SLAT
extern	cm_node cm_ports[];		/* CME ports			*/
extern  int	cm_port_maximum;	/* Maximum number CME ports	*/
#endif

#if LE_HOST
extern	byte	nd_temp[];		/* Temporary ND name		*/
extern	int	reject_index;		/* Number of pending rejects	*/
extern	cb_ptr	reject_cb[];		/* Rejected CB table		*/
extern	byte	reject_id[];		/* Rejected slot ID's		*/
#endif

#if (!LE_OBJECTS) && (!CME_SLAT)
extern	ob_node	ob_dummy;		/* Pointer to dummy object	*/
typedef data_b	OB_VIEW(dab_ptr);
#endif


void sb_1_slot_inp(ptr,sb,cb)

    if_byte	ptr;			/* Pointer to input  slot	*/
    sb_ptr	sb;			/* Pointer to session block	*/
    cb_ptr	cb;			/* Pointer to circuit block	*/
{
    nd_ptr	nd;			/* Pointer to remote node	*/
#if !CME_SLAT
    ob_ptr	ob;			/* Pointer to object structure	*/
#endif
    slot_iptr	slot;			/* Pointer to slot_header	*/
    starts_iptr	starts;			/* Pointer to start slot	*/
    int		type;			/* Slot type code		*/
    int		len;			/* Length field			*/
    byte	flag_word = 0;		/* Temp. type flag field	*/
#if (LE_OBJECTS || CME_SLAT)
    dab_ptr	db;			/* Pointer to data-b structure	*/
#else
    dab_ptr	db;
#endif
    byte	break_det;		/* Break detected indicator	*/
    byte	break_first;		/* Break reported first		*/

/*-----------------------------------------------------------------------
    Get pointer to slot, remote node.
 -----------------------------------------------------------------------*/
    nd = cb->cb_nd;

    slot = (slot_iptr)ptr;
    ptr += sizeof(slot_hdr);

    type = slot->slot_code & SLOT_TYPE;

/*-----------------------------------------------------------------------
    If we found a session, process the slot. Count any credits and get
    the type of slot.
 -----------------------------------------------------------------------*/
    if (sb != 0)
    {
	sb->sb_rmt_credits += (slot->slot_code & SLOT_CREDITS);

/*-----------------------------------------------------------------------
    Process those slots which are legal when session running.
 -----------------------------------------------------------------------*/
	if (!(sb->sb_flags & SB_BLOCK))
	{

/*-----------------------------------------------------------------------
    Process STOP slots. Delete the session. This test comes before the
    slot ID compare as a STOP. slot has a zero source slot ID.
 -----------------------------------------------------------------------*/
	    if (type == SLOT_STOP)
	    {
		sb->sb_stop_reason = slot->slot_code & SLOT_CREDITS;
		sb_delete(sb,SX_STOP);
	    }

/*-----------------------------------------------------------------------
    If the source slot ID does not match our remote ID, this is a bad slot.
    Ignore for host, send stop slot for server.
 -----------------------------------------------------------------------*/
	    else if (slot->slot_src != sb->sb_rmt_ses_id)
	    {
		count_1(co_slot_bad);
#if LE_HS_STATS
    		hs->hs_count.ch_err_masks[CIR_R_ERROR] |= 1<< ((ESLOT_SRC_ID & 0xff00) >> 8);
#endif
		if (cb->cb_type == CB_SERVER)
		{
		    sb->sb_stop_reason  = SLOT_STOP_BAD_SLOT;
		    sb->sb_flags = SB_STOP;
		}
	    }

/*-----------------------------------------------------------------------
    Process DATA-A slots. Pass the characters to the environment. If
    a slot with data arrives when we have not extended a credit, then
    declare a bad slot.
 -----------------------------------------------------------------------*/
	    else if (type == SLOT_DATA_A)
	    {
		if (slot->slot_len != 0)
		{
		    if (sb->sb_ext_credits != 0)
		    {
			sb->sb_ext_credits--;
			len = slot->slot_len;
			sx_put_char(sb,ptr,len);
		    }
		    else
		    {
			count_1(co_slot_bad);
#if LE_HS_STATS
			hs->hs_count.ch_err_masks[CIR_R_ERROR] |= 1<< ((ESLOT_CREDITS & 0xff00) >> 8);
#endif
			sb->sb_stop_reason = SLOT_STOP_NO_RESOURCE;
			sb->sb_flags = SB_STOP;
		    }
		}
	    }

/*-----------------------------------------------------------------------
    Process DATA-B slots. Count a credit consumed, and a return credit.
 -----------------------------------------------------------------------*/
	    else if (type == SLOT_DATA_B) 
	      {
		if ((len = slot->slot_len) != 0)
		{
		    if (sb->sb_ext_credits != 0)
		    {
		   	sb->sb_ext_credits--;
		 	sb->sb_lcl_credits++;
		        sb->sb_flags |= SB_CREDITS;
		    }	
		    else
		    {
		        count_1(co_slot_bad);
#if LE_HS_STATS
		        hs->hs_count.ch_err_masks[CIR_R_ERROR] |= 1<< ((ESLOT_CREDITS & 0xff00) >> 8);
#endif		
			sb->sb_stop_reason = SLOT_STOP_NO_RESOURCE;
		        sb->sb_flags = SB_STOP;
		    }
		}

/*-----------------------------------------------------------------------
    Notify session layer if there was port/session information.
 -----------------------------------------------------------------------*/
		if (sb->sb_sc == SVC_CLASS_TERM) {

		if (len >= 5 && (*ptr & SLOT_INFO))
		{
		    sb->sb_rmt_data.db_flags = 0;
		    sb->sb_rmt_data.db_config  = *ptr;
		    if (*ptr & SLOT_BREAK)
			break_det = SLOT_PS_LONG;
		    else
			break_det = 0;

		    ptr += 5;
		    len -= 5;
		    while (len > 0 && *ptr != 0)
		    {
			if (*ptr == SLOT_P_MODE)
			{
			    sb->sb_rmt_data.db_flags |= DB_E_MODE;
			    sb->sb_data_b_rmt_mode = *(ptr+2);
			}

			else if (*ptr == SLOT_P_CHAR)
			{
			    sb->sb_rmt_data.db_flags |= DB_E_PARITY;
			    sb->sb_rmt_data.db_char_parity = *(ptr+2);
			}


			else if (*ptr == SLOT_P_PREF)
			{
			    sb->sb_rmt_data.db_flags |= DB_E_BELL;
			    sb->sb_rmt_data.db_bell_mode = *(ptr+2);
			}

			else if (*ptr == SLOT_P_XMIT)
			{
			    sb->sb_rmt_data.db_flags |= DB_E_XMIT;
			    sb->sb_rmt_data.db_baud_xmit  = (word)(*(ptr+2));
			    sb->sb_rmt_data.db_baud_xmit |= (word)(*(ptr+3))<<8;
			}

			else if (*ptr == SLOT_P_RECV)
			{
			    sb->sb_rmt_data.db_flags |= DB_E_RECV;
			    sb->sb_rmt_data.db_baud_recv  = (word)(*(ptr+2));
			    sb->sb_rmt_data.db_baud_recv |= (word)(*(ptr+3))<<8;
			}

/*-----------------------------------------------------------------------
    Report break and bad status characters immediately when received in
the Data-B slot.
 -----------------------------------------------------------------------*/
			else if (*ptr == SLOT_P_STATUS)
			{
			    sb->sb_rmt_data.db_flags |= DB_E_STATUS;
			    sb->sb_rmt_data_b_status      = *(ptr+2);
			    if (*(ptr+2) == 1 || *(ptr+2) == 2)
				break_det = 0;
			    else
			    	sb->sb_rmt_data_b_status_char = *(ptr+3);
			    sx_signal(sb,SX_DATA_B_REP);
			    sb->sb_rmt_data.db_flags &= ~(DB_E_STATUS);
			}

			
			len -= (*(ptr+1) + 2);
			ptr += (*(ptr+1) + 2);
		    }

/*-----------------------------------------------------------------------
    Signal the DATA-B slot reception to the environment.  The environment
    should check if a break condition occured by testing
    sb->sb_rmt_data.db_config & SLOT_BREAK.  If DB_E_STATUS is set, then
    the remote system distinguished between a long and a short break.
    It is possible that a break condition did not occur, but the remote
    system is signaling a framing, data overrun, or parity error, in
    which case the error status code and bad character (if any) are in
    sb->sb_rmt_data_b_status and sb->sb_rmt_data_b_status_char respectively.
 -----------------------------------------------------------------------*/
		    if (sb->sb_rmt_data.db_flags || sb->sb_rmt_data.db_config)
		    	sx_signal(sb,SX_DATA_B_REP);

/*-----------------------------------------------------------------------
    Report break condition seen without a STATUS parm field.  (LAT V5.0
    compatibility).
 -----------------------------------------------------------------------*/
		    if (break_det)
		    {
			sb->sb_rmt_data_b_status = break_det;
			sx_signal(sb,SX_DATA_B_REP);
		    }
		}

/*-----------------------------------------------------------------------
    Notify session layer if there was local port/session information.
    In the case of a SET DATA-B we change a temporary configuration.
    The local configuration is updated if the environment changes it and
    notifies the engine via db_report_config.
 -----------------------------------------------------------------------*/
		else if (len >= 5 && ((*ptr & SLOT_SET) || (!(*ptr & (SLOT_SET | SLOT_INFO)))))
		{
#if LE_OBJECTS
		    ob = sb->sb_ob;
		    db = &ob->ob_tmp_data;
#elif CME_SLAT
		    db = cx_data_b(sb->sb_cid,CX_DATAB_TMP);
#else
		    ob = (ob_ptr)&ob_dummy;
		    sb->sb_ob = ob;
		    db = &ob->ob_tmp_data;
#endif
		    break_first = TRUE;
		    sb->sb_rmt_data.db_flags = 0;
		    db->db_config  = *ptr;
		    if (*ptr & SLOT_BREAK)
		    {
			db->db_config &= ~(SLOT_BREAK);
			break_det = SLOT_PS_LONG;
		    }
		    else
			break_det = 0;

		    ptr += 5;
		    len -= 5;
		    while (len > 0 && *ptr != 0)
		    {
			if (*ptr != SLOT_P_STATUS)
			    break_first = FALSE;

			if (*ptr == SLOT_P_MODE)
			{
			    sb->sb_rmt_data.db_flags |= DB_E_MODE;
			    sb->sb_data_b_rmt_mode = *(ptr+2);
			}

			else if (*ptr == SLOT_P_CHAR)
			{
			    db->db_flags |= DB_E_PARITY;
			    db->db_char_parity = *(ptr+2);
			}


			else if (*ptr == SLOT_P_PREF)
			{
			    db->db_flags |= DB_E_BELL;
			    db->db_bell_mode = *(ptr+2);
			}

			else if (*ptr == SLOT_P_XMIT)
			{
			    db->db_flags |= DB_E_XMIT;
			    db->db_baud_xmit  = (word)(*(ptr+2));
			    db->db_baud_xmit |= (word)(*(ptr+3))<<8;
			}

			else if (*ptr == SLOT_P_RECV)
			{
			    db->db_flags |= DB_E_RECV;
			    db->db_baud_recv  = (word)(*(ptr+2));
			    db->db_baud_recv |= (word)(*(ptr+3))<<8;
			}

/*-----------------------------------------------------------------------
    Report bad status characters immediately when received in the Data-B
    slot.  If this is a break and it's the first parm code in the slot,
    report it now; otherwise, report it after reporting configuration
    changes (LAT V5.2 rqmt with a "set" slot).
 -----------------------------------------------------------------------*/
			else if (*ptr & SLOT_P_STATUS)
			{
			    sb->sb_rmt_data_b_status = *(ptr+2);
			    if (*(ptr+2) == 1 || *(ptr+2) == 2)
			    {
				if (break_first)
				{
				    break_det = 0;
				    sb->sb_rmt_data.db_config |= SLOT_BREAK;
				    sx_signal(sb,SX_DATA_B_REP);
				}
				else
				    break_det = *(ptr+2);
			    }
			    else
			    {
				sb->sb_rmt_data_b_status_char = *(ptr+2);
				sx_signal(sb,SX_DATA_B_REP);
			    }
			    break_first = FALSE;
			}

			len -= (*(ptr+1) + 2);
			ptr += (*(ptr+1) + 2);
		    }

		    sx_signal(sb,SX_DATA_B_SET);
/*-----------------------------------------------------------------------
    Report break condition seen without a STATUS parm field (LAT V5.0
    compatibility) or one seen with delayed reporting status.
 -----------------------------------------------------------------------*/
		    if (break_det)
		    {
			sb->sb_rmt_data.db_config |= SLOT_BREAK;
			sb->sb_rmt_data_b_status = break_det;
			sx_signal(sb,SX_DATA_B_REP);
		    }
		}
		}
	    }
	    		
/*-----------------------------------------------------------------------
    Process ATTENTION slots. If the flush flag is set, notify the
    environment to flush its output. If the XON flag is set, notify
    the environment to force XON.
 -----------------------------------------------------------------------*/
	    else if (type == SLOT_ATTENTION 
		     && sb->sb_sc == SVC_CLASS_TERM
		     )
	    {
		if (*ptr & SLOT_FLUSH)
		    sx_signal(sb,SX_FLUSH);
		if (*ptr & SLOT_FORCE_XON)
		    sx_signal(sb,SX_FORCE_XON);
	    }
	}

#if LE_SERVER
/*-----------------------------------------------------------------------
    Process those slots which are legal when waiting for response to
    a start message.
 -----------------------------------------------------------------------*/
	else if (cb->cb_type == CB_SERVER && sb->sb_flags & SB_STALL)
	{

/*-----------------------------------------------------------------------
    Process START slots. Set the maximum data-a and attention size and
    remote port name.
 -----------------------------------------------------------------------*/
	    if (type == SLOT_START)
	    {
		sb->sb_flags &= ~(SB_STALL | SB_NODE);
		starts = (starts_iptr)ptr;

		sb->sb_rmt_ses_id = slot->slot_src;
		sb->sb_max_attn   = starts->max_attn;
		sb->sb_max_data   = starts->max_data;

		len  = slot->slot_len - 3;
		ptr += 3;
		
		len -= (*ptr + 1);
		ptr += (*ptr + 1);
		len -= (*ptr + 1);
		ptr += (*ptr + 1);

		id_temp[0] = 0;
		
		while (len > 0 && *ptr != 0)
		{
		    if (*ptr == SLOT_P_DST_PORT)
		    {
			str_input(&ob_temp[0],ptr+1,OB_STR,1);
			sb_str_move_sl(&sb->sb_port[0],&ob_temp[0]);
		    }
		    else if (*ptr == SLOT_P_SRC_PORT && sb->sb_port[0] == 0)
		    {
			str_input(&ob_temp[0],ptr+1,OB_STR,1);
			sb_str_move_sl(&sb->sb_port[0],&ob_temp[0]);
		    }
		    else if (*ptr == SLOT_P_PASSWORD)
		    {
		        str_input(&id_temp[0],ptr+1,SV_STR,1);
		        for (;id_temp[id_temp[0]] == ' '; id_temp[0]--);
		    }


		    len -= (*(ptr+1) + 2);
		    ptr += (*(ptr+1) + 2);
		}
		if (sb->sb_sid != SID_NULL)
		{
		
		    if (sb->sb_flags & SB_HIC)
/*-----------------------------------------------------------------------
    If this is an HI connect to a password protected service or a port
    associated with such a service, fail if  the correct password is not 
    supplied in the start slot.
 -----------------------------------------------------------------------*/
 		    {
#if LE_OBJECT_HIC
#if (CME_SLAT || LE_OBJECTS)
			int  i;
#endif
			byte stat = SUCCESS;
			as_ptr	as;
			sb_str_move_ls(&as_temp[0],&sb->sb_name[0]);
			if ((as = as_show(&as_temp[0])) != 0)
			{
			    if (as->as_password[0] != 0 && 
			    	as_str_diff_sl(&as->as_password[0],&id_temp[0]) != 0)
			    {
			    	sb->sb_stop_reason = SLOT_STOP_PASSWORD;
				stat = FAILURE;
			    }
			}
#if (CME_SLAT || LE_OBJECTS)
			else 
#if CME_SLAT
			{
			    i = sb->sb_cid;
			    for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
			    	if ((as->as_port_map[i/8] & (1 << (i%8))) &&
				     as->as_password [0] != 0 &&
				     as_str_diff_sl(&as->as_password[0],&id_temp[0]) != 0)
#elif LE_OBJECTS						
		 	{			
			    for (i = 0; i < ob_highest; i++)
			    	if (sb->sb_ob == ob_map[i])
				    break;
			    for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
			    	if ((as->as_ob_map[i/8] & (1 << (i%8))) &&
				     as->as_password[0] != 0 &&
				     as_str_diff_sl(&as->as_password[0],&id_temp[0]) != 0)
#endif
				{
				    sb->sb_stop_reason = SLOT_STOP_PASSWORD;
				    stat = FAILURE;
				}
			}
#endif
			if (stat == FAILURE)
			    sb_delete(sb,SX_REJECT);
			else               
			    sx_session(sb,SX_CONNECT_HIC);
#endif
		    }
		    else
		    	sx_session(sb,SX_CONNECT);
		}
		sb->sb_flags &= ~SB_HIC;
	    }

/*-----------------------------------------------------------------------
    Process REJECT and STOP slots the same by notifying the envionment
    and deleting the session.
 -----------------------------------------------------------------------*/
	    else if (type == SLOT_REJECT || type == SLOT_STOP)
	    {
		sb->sb_stop_reason = slot->slot_code & SLOT_CREDITS;
		sb_delete(sb,SX_REJECT);
	    }
	}
#endif
    }

#if LE_HOST
/*-----------------------------------------------------------------------
    If there is no session and this is a START, then process.
 -----------------------------------------------------------------------*/
    else if (type == SLOT_START)
    {
    	int	start;
    	word	req_no = 0;

/*-----------------------------------------------------------------------
    Check to see if we have a session already using the remote-ID. If
    we do, ignore this slot.
 -----------------------------------------------------------------------*/
	for (sb = cb->cb_slot[0]; sb != 0; sb = sb->sb_slot[0])
	{
	    if (sb->sb_rmt_ses_id == slot->slot_src)
		break;
	}
	if (sb == 0)
	{

/*-----------------------------------------------------------------------
    Save the service name for later use and skip these fields.
 -----------------------------------------------------------------------*/
	    starts = (starts_iptr)ptr;
	    len    = slot->slot_len - 3;
	    ptr   += 3;

/*-----------------------------------------------------------------------
    Save the service name for later use and skip these fields.
 -----------------------------------------------------------------------*/
	    str_input(&as_temp[0],ptr,SV_STR,1);

	    len -= (*ptr + 1);
	    ptr += (*ptr + 1);
	    len -= (*ptr + 1);
	    ptr += (*ptr + 1);

/*-----------------------------------------------------------------------
    Process the optional parameters. We are looking for request number
    and port name.
 -----------------------------------------------------------------------*/
	    start = FAILURE;
	    nd_temp[0] = 0;
	    ob_temp[0] = 0;
	    id_temp[0] = 0;

	    while (len > 0 && *ptr != 0)
	    {
		if (*ptr == SLOT_P_HIC)
		{
		    req_no  = (word)(*(ptr+2));
		    req_no |= (word)(*(ptr+3))<<8;
		    start = SUCCESS;
		}
		else if (*ptr == SLOT_P_DST_PORT)
		    str_input(&ob_temp[0],ptr+1,OB_STR,1);

		else if (*ptr == SLOT_P_SRC_PORT)
		    str_input(&nd_temp[0],ptr+1,ND_STR,1);

		else if (*ptr == SLOT_P_PASSWORD)
		{
		    str_input(&id_temp[0],ptr+1,SV_STR,1);
		    for (;id_temp[id_temp[0]] == ' '; id_temp[0]--);
		}

		else if (*ptr == SLOT_P_FLAGS)
		{
		    flag_word = *(ptr+2);
		}

		len -= (*(ptr+1) + 2);
		ptr += (*(ptr+1) + 2);
	    }

/*-----------------------------------------------------------------------
    Create a session and notfy the envionrment.
 -----------------------------------------------------------------------*/
	    if ((sb = sb_create(cb)) != 0)
	    {
		sb->sb_rmt_ses_id   = slot->slot_src;
		sb->sb_rmt_credits  = (slot->slot_code & SLOT_CREDITS);
		sb->sb_max_attn     = starts->max_attn;
		sb->sb_max_data     = starts->max_data;

		sb_str_move_sl(&sb->sb_name[0],&as_temp[0]);
		sb_str_move_sl(&sb->sb_port[0],&nd_temp[0]);
		sb->sb_rmt_data.db_type = (byte)(LOBYTE(flag_word));

		if (start)
		{
#if LE_SUBJECT_HIC || LE_OBJECT_HIC
		    hi_start(sb,req_no);
#else
		    sb->sb_stop_reason = SLOT_STOP_NO_RESOURCE;
#endif
		}
		else
		{
#if LE_HOST_AS
		    sb_start(sb,&as_temp[0],&ob_temp[0]);
#else
		    sb->sb_stop_reason = SLOT_STOP_NO_RESOURCE;
#endif
		}

		if (sb->sb_stop_reason != 0)
		    sb->sb_flags |= SB_REJECT;
	    }

/*-----------------------------------------------------------------------
    If no SB, fill in the reject slot table. If already full, we must
    just give up and let the otherside hang.
 -----------------------------------------------------------------------*/
	    else if (reject_index < SB_REJECTS)
	    {
		reject_cb[reject_index] = cb;
		reject_id[reject_index] = slot->slot_src;
		reject_index++;
	    }
	}
    }
#endif
}
