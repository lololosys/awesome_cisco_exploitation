/* $Id: sb1out.c,v 3.2 1995/11/17 17:35:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/sb1out.c,v $
 *------------------------------------------------------------------
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sb1out.c,v $
 * Revision 3.2  1995/11/17  17:35:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    sb_1_slot_out	(i) generate service class 1 output slots

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to generate the appropriate slot for a
    service class 1 session.

    CALL BY:	int sb_1_slot_out(ptr,sb,max)
		of_byte ptr;
		sb_ptr	sb;
		int	max;

    ON EXIT:	Returns length of generated slot, zero if none.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latob.h"
#include "latsb.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
#if !LE_OBJECTS
extern ob_node ob_dummy;		/* Dummy object structure	*/
#endif

#if (!LE_OBJECTS) && (!CME_SLAT)
extern	ob_node	ob_dummy;		/* Pointer to dummy object	*/
typedef data_b	OB_VIEW(dab_ptr);
#endif

int sb_1_slot_out(ptr,sb,max)

    of_byte	ptr;			/* Pointer to output frame	*/
    sb_ptr	sb;			/* Pointer to session block	*/
    int		max;			/* Remaining length of message	*/
{
#if !CME_SLAT
    ob_ptr	ob;			/* Pointer to object structure	*/
#endif
    slot_optr	slot;			/* Pointer to slots		*/
    int		len;			/* Returned data length		*/
    int		db_flags = 0;		/* DATA-B field  flags		*/
    byte	db_config = 0;		/* DATA-B config flags		*/
#if (LE_OBJECTS || CME_SLAT)
    dab_ptr	db;			/* Pointer to DATA-B struc	*/
#else
    dab_ptr	db;
#endif
    byte	cred;			/* Credits to extend		*/

/*-----------------------------------------------------------------------
    Setup pointer to slot header and move pointer to start of data.
    Set return length to no slot.
 -----------------------------------------------------------------------*/
    slot = (slot_optr)ptr;
    ptr += sizeof(slot_hdr);

    len  = 0;

/*-----------------------------------------------------------------------
    When host delay is active both SB_START and SB_STOP bits can be set
    in 'sb->sb_flags'.  This happens when a session open is immediately
    followed by a session close so both slots will be sent in the same
    circuit message.  In this case we should send a reject slot with a
    reason of 'user-requested-disconnect'.
 -----------------------------------------------------------------------*/
    if ((sb->sb_flags & (SB_START | SB_STOP)) == (SB_START | SB_STOP))
    {
	sb->sb_flags = SB_REJECT;
	sb->sb_stop_reason = SLOT_STOP_USER;
    }

/*-----------------------------------------------------------------------
    Check if session in run state, i.e. no block flags set. We can only
    send data-a, data-b, and attention in run state.
 -----------------------------------------------------------------------*/
    if (!(sb->sb_flags & SB_BLOCK))
    {

/*-----------------------------------------------------------------------
    Check if we should send a flush. If so, generate an attention slot
    with the flush mask set.
 -----------------------------------------------------------------------*/
	if (sb->sb_flags & SB_FLUSH)
	{
	    sb->sb_flags &= (~SB_FLUSH);

	    slot->slot_code = SLOT_ATTENTION;
	    *ptr++ = SLOT_FLUSH;

	    len = sizeof(slot_hdr) + 1;
	}

/*-----------------------------------------------------------------------
    Check if we should send a force XON.  If so, generate an attention
    slot with the force XON mask set.
 -----------------------------------------------------------------------*/
	else if (sb->sb_flags & SB_FORCE_XON)
	{
	    sb->sb_flags &= ~SB_FORCE_XON;

	    slot->slot_code = SLOT_ATTENTION;
	    *ptr++ = SLOT_FORCE_XON;

	    len = sizeof(slot_hdr) + 1;
	}

/*-----------------------------------------------------------------------
    Check if we should send characters. We must have credits from the
    remote system.
 -----------------------------------------------------------------------*/
	else if ((sb->sb_flags & SB_DATA_A) && sb->sb_rmt_credits)
	{
	    sb->sb_flags &= (~SB_DATA_A);

	    len = (int)sb->sb_max_data;
	    if (len > max - sizeof(slot_hdr))
		len = max - sizeof(slot_hdr);

	    if ((len = sx_get_char(sb,ptr,len)) != 0)	    {
		sb->sb_rmt_credits--;
		slot->slot_code = SLOT_DATA_A;

		len += sizeof(slot_hdr);
	    }
	}

/*-----------------------------------------------------------------------
    Check if we should send configuration. We must have credits from
    the remote system. The environment is responsible for setting up
    the configuration fields in the data-b block associated with the
    session or port.  If using CME, the local data-b structure is in
    the CM port structure; if using objects, it's in the object
    structure; if using neither, it's in a dummy object structure
    which is set up before or in the ox_session() call.
 -----------------------------------------------------------------------*/
	else if ((sb->sb_flags & SB_DATA_B) && sb->sb_rmt_credits  &&
		 (sb->sb_sc == SVC_CLASS_TERM))
	{
	    if (max > SLOT_DATAB_LEN)
	    {
		sb->sb_rmt_credits--;

		if (sb->sb_flags & SB_DELAY_A)
		{
		    sb->sb_flags |= SB_DATA_A;
		    sb->sb_flags &= ~SB_DELAY_A;
		}

#if LE_OBJECTS
		ob = sb->sb_ob;
		db = &ob->ob_lcl_data;
#elif CME_SLAT
		db = cx_data_b(sb->sb_cid,CX_DATAB_LCL);
#else
		ob = (ob_ptr)&ob_dummy;
		sb->sb_ob = ob;
		ox_session(sb,ob,OX_CONFIG_REP);
		db = &ob->ob_lcl_data;
#endif
		if (sb->sb_data_b_type & SB_DATA_B_REP)
		{
		    sb->sb_data_b_type &= ~SB_DATA_B_REP;
		    db_flags  = db->db_flags | DB_E_PORT;
   		    db_config = (byte)SLOT_INFO | (db->db_config & SLOT_FLOW);
		    db->db_config &= ~SLOT_BREAK;
		    db->db_flags  &= ~DB_E_STATUS;
		}
		else if (sb->sb_data_b_type & SB_DATA_B_SET)
		{
		    sb->sb_data_b_type &= ~SB_DATA_B_SET;
		    db_flags  = sb->sb_tmp_data.db_flags;
		    db_config = (byte)SLOT_SET | (sb->sb_tmp_data.db_config & SLOT_FLOW);
		}

		if ((db_flags & DB_E_MODE) && (sb->sb_data_b_lcl_mode == SB_PASSALL))
		{
		    db_config &= ~SLOT_FLOW;
		    db_config |= (SLOT_INP_OFF | SLOT_OUT_OFF);
		}

		if (sb->sb_data_b_type == SB_DATA_B_UNK)
		    sb->sb_flags &= ~SB_DATA_B;

		slot->slot_code = SLOT_DATA_B;
		*ptr++ = db_config;
		*ptr++ = DB_XOFF;
		*ptr++ = DB_XON;
		*ptr++ = DB_XOFF;
		*ptr++ = DB_XON;

		if (db_flags & DB_E_PARITY)
		{
		    *ptr++ = SLOT_P_CHAR;
		    *ptr++ = 1;

		    if (db_config & SLOT_INFO)
		    	*ptr++ = db->db_char_parity;
		    else if (db_config & SLOT_SET)
		    	*ptr++ = sb->sb_tmp_data.db_char_parity;
		}

		if (db_flags & DB_E_XMIT)
		{
		    *ptr++ = SLOT_P_XMIT;
		    *ptr++ = 2;

		    if (db_config & SLOT_INFO)
		    {
		    	*ptr++ = (byte)(LOBYTE(db->db_baud_xmit));
		    	*ptr++ = (byte)(HIBYTE(db->db_baud_xmit));
		    }
		    else if (db_config & SLOT_SET)
		    {
		    	*ptr++ = (byte)(LOBYTE(sb->sb_tmp_data.db_baud_xmit));
		    	*ptr++ = (byte)(HIBYTE(sb->sb_tmp_data.db_baud_xmit));
		    }
		}

		if (db_flags & DB_E_RECV)
		{
		    *ptr++ = SLOT_P_RECV;
		    *ptr++ = 2;

		    if (db_config & SLOT_INFO)
		    {
		    	*ptr++ = (byte)(LOBYTE(db->db_baud_recv));
		    	*ptr++ = (byte)(HIBYTE(db->db_baud_recv));
		    }
		    else if (db_config & SLOT_SET)
		    {
		    	*ptr++ = (byte)(LOBYTE(sb->sb_tmp_data.db_baud_recv));
		    	*ptr++ = (byte)(HIBYTE(sb->sb_tmp_data.db_baud_recv));
		    }

		}

		if (db_flags & DB_E_BELL)
		{
		    *ptr++ = SLOT_P_PREF;
		    *ptr++ = 1;

		    if (db_config & SLOT_INFO)
		    	*ptr++ = db->db_bell_mode;
		    else if (db_config & SLOT_SET)
		    	*ptr++ = sb->sb_tmp_data.db_bell_mode;
		}

		if (db_flags & DB_E_MODE)
		{
		    *ptr++ = SLOT_P_MODE;
		    *ptr++ = 1;
		    *ptr++ = sb->sb_data_b_lcl_mode;
		}

		if (db_flags & DB_E_STATUS)
		{
		    if ((db_config & SLOT_BREAK) && (sb->sb_lcl_data_b_status != 0))
		    {
		        *ptr++ = SLOT_P_STATUS;
			*ptr++ = 1;
			*ptr++ = sb->sb_lcl_data_b_status;
		    }
		    else
		    {
		        *ptr++ = SLOT_P_STATUS;
			*ptr++ = 2;
			*ptr++ = sb->sb_lcl_data_b_status;
			*ptr++ = sb->sb_lcl_data_b_status_char;
	 	    }
		}

		*ptr++ = 0;

	        len = (int)(ptr - ((of_byte)slot));
	    }
	}

/*-----------------------------------------------------------------------
    If no slot generated to this point, check if we should just send
    credits.
 -----------------------------------------------------------------------*/
	else if (sb->sb_flags & SB_CREDITS)
	{
	    slot->slot_code = SLOT_DATA_A;
	    len = sizeof(slot_hdr);
	}
    }

/*-----------------------------------------------------------------------
    If stalled, delay any processing as we are waiting a response from
    host to the start condition.
 -----------------------------------------------------------------------*/
    else if (sb->sb_flags & SB_STALL) {
	;
    }
/*-----------------------------------------------------------------------
    Check if we should send a START_SLOT. A START_SLOT from a server
    is converted into a stall condition while we wait on either the
    START or REJECT from the host.
 -----------------------------------------------------------------------*/
    else if (sb->sb_flags & SB_START)
    {
	if (max > SLOT_START_LEN)
	{

#if CME_SLAT
	    db = cx_data_b(sb->sb_cid,CX_DATAB_LCL);
#else
#if LE_OBJECTS
	    ob = sb->sb_ob;
#else
	    ob = (ob_ptr)&ob_dummy;
	    sb->sb_ob = ob;
	    ox_session(sb,ob,OX_START);
#endif
	    db = &ob->ob_lcl_data;
#endif
	    sb->sb_flags &= (~SB_START);

	    if (sb->sb_cb->cb_type == CB_SERVER)
		sb->sb_flags |= SB_STALL;

	    slot->slot_code = SLOT_START;
	    *ptr++ = sb->sb_sc;
	    *ptr++ = hs->hs_sb_max_attn;
	    *ptr++ = hs->hs_sb_max_data;
	    switch (sb->sb_sc) {
	      case SVC_CLASS_TERM:
		ptr    = (of_byte)sb_str_move_os(ptr,&sb->sb_name[0]);
		break;

	      case SVC_CLASS_XWIRE:
		ptr = str_move(ptr, (byte *)"\007X$LOGIN");
		break;

	      case SVC_CLASS_XFONT:
		ptr = str_move(ptr, (byte *)"\006X$FONT");
		break;

	      default:
		*ptr++ = 0;
	    }

	    *ptr++ = 0;

	    if (sb->sb_sc == SVC_CLASS_TERM) {
	    *ptr++ = SLOT_P_FLAGS;
	    *ptr++ = 2;

	    *ptr++ = db->db_type;

	    *ptr++ = 0;
	    }
#if LE_SUBJECT_HIC || LE_OBJECT_HIC
	    if (sb->sb_flags & SB_HIC)
	    {
		*ptr++ = SLOT_P_HIC;
		*ptr++ = 2;
		*ptr++ = (byte)(LOBYTE(sb->sb_req_no));
		*ptr++ = (byte)(HIBYTE(sb->sb_req_no));
	    }
#endif

#if LE_SERVER
	    if (sb->sb_cb->cb_type == CB_SERVER)
	    {
		if (sb->sb_port[0] != 0)
		{
		    *ptr++ = SLOT_P_DST_PORT;
		    ptr = (of_byte)sb_str_move_os(ptr,&sb->sb_port[0]);
		}
		*ptr++ = SLOT_P_SRC_PORT;
#if CME_SLAT
		ptr = (of_byte)cx_move(ptr,sb->sb_cid,CX_PORT_NAME);
#else
		ptr = (of_byte)ob_str_move_os(ptr,&ob->ob_name[0]);
#endif
	    }
#endif

#if LE_HOST
	    if (sb->sb_cb->cb_type == CB_HOST)
	    {
	        *ptr++ = SLOT_P_DST_PORT;
#if CME_SLAT
		ptr = (of_byte)cx_move(ptr,sb->sb_cid,CX_PORT_NAME);
#else
		ptr = (of_byte)ob_str_move_os(ptr,&ob->ob_name[0]);
#endif
		if (sb->sb_password[0] != 0)
		{
		    *ptr++ = SLOT_P_PASSWORD;
		    ptr = (of_byte)sb_str_move_os(ptr,&sb->sb_password[0]);
		}
	    }
#endif


#if LE_SERVER
	    if (sb->sb_cb->cb_type == CB_SERVER && sb->sb_sc == SVC_CLASS_TERM)
	    {
	        *ptr++ = SLOT_P_GROUPS;
#if CME_SLAT
		ptr = (of_byte)cx_move(ptr,sb->sb_cid,CX_LAT_GROUP);
#else
		ptr = (of_byte)ob_str_move_os(ptr,&ob->ob_idl[0]);
#endif

	    	if (sb->sb_password[0] != 0)
		{	    
		    *ptr++ = SLOT_P_PASSWORD;
		    ptr = (of_byte)sb_str_move_os(ptr,&sb->sb_password[0]);
		}
	    }
#endif
	    if (sb->sb_sc == SVC_CLASS_XWIRE) {
		*ptr++ = SLOT_P_XSERVER;
		ptr = ob_str_move_os(ptr,&ob->ob_name[0]);
	    }
	    *ptr++ = 0;
	    len = (int)(ptr - ((of_byte)slot));
	}
    }

/*-----------------------------------------------------------------------
    Check if we should send a STOP_SLOT. The session is actually
    deleted later in this module.
 -----------------------------------------------------------------------*/
    else if (sb->sb_flags & SB_STOP)
    {
	sb->sb_flags &= (~SB_STOP);
	sb->sb_flags |= SB_DEL_STOP;

	sb->sb_lcl_ses_id  = 0;
	slot->slot_code = (byte)(SLOT_STOP | sb->sb_stop_reason);
	len = sizeof(slot_hdr);
    }

/*-----------------------------------------------------------------------
    Check if we should send a REJECT_SLOT. The session is actually
    deleted later in this module.
 -----------------------------------------------------------------------*/
    else if (sb->sb_flags & SB_REJECT)
    {
	sb->sb_flags &= (~SB_REJECT);
	sb->sb_flags |= SB_DEL_NONE;

	sb->sb_lcl_ses_id  = 0;
	slot->slot_code = (byte)(SLOT_REJECT | sb->sb_stop_reason);
	len = sizeof(slot_hdr);
    }

/*-----------------------------------------------------------------------
    If a slot was generated, complete filling in the slot header and
    advance to next slot. Make sure total credits extended never will
    exceed 127  and credits extended in this slot will not exceed 15.
 -----------------------------------------------------------------------*/
    if (len)
    {
	slot->slot_dst   = sb->sb_rmt_ses_id;
	slot->slot_src   = sb->sb_lcl_ses_id;
	slot->slot_len   = (byte)(len - sizeof(slot_hdr));

        if (slot->slot_code == SLOT_DATA_A ||
	    slot->slot_code == SLOT_DATA_B ||
	    slot->slot_code == SLOT_START)
	{
	    if (sb->sb_lcl_credits + sb->sb_ext_credits > 127)
		sb->sb_lcl_credits = (byte)(LOBYTE(127 - sb->sb_ext_credits));

	    cred = (sb->sb_lcl_credits > 15) ? 15 : sb->sb_lcl_credits;

	    slot->slot_code    |= cred;
	    sb->sb_ext_credits += cred;
	    sb->sb_lcl_credits -= cred;
	    if (sb->sb_lcl_credits == 0)
	    	sb->sb_flags &= (~SB_CREDITS);
	}
    }
    return(len);
}
