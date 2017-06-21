/* $Id: ifchk.c,v 3.2 1995/11/17 17:33:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/ifchk.c,v $
 *------------------------------------------------------------------
 * $Log: ifchk.c,v $
 * Revision 3.2  1995/11/17  17:33:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    if_check		(e) check input frame

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT Ethernet services when an input
    LAT Ethernet frame is received. The frame is checked for length.
    If the frames does not pass this routine, it should be discarded and
    not passed to if_message.

    CALL BY:	int if_check(frame,src,len)
		if_byte	frame;
		word	src[3];
    		int	len;

    ON EXIT:	Returns SUCCESS if valid frame, FAILURE otherwise.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latnd.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

int if_check(frame,src,len,idb)
    if_byte	frame;			/* Pointer to input frame	*/
    word	src[3];			/* Source address		*/
    int		len;			/* Received frame size		*/
    idbtype	*idb;			/* Input interface		*/
{
    if_byte	ptr;			/* Pointer to input frame	*/
    cir_iptr	cir;			/* Pointer to circuit		*/
    str_iptr	str;			/* Pointer to start		*/
    nd_ptr	nd;			/* Pointer to remote node	*/
    int		type;			/* Message type			*/
    int		mlen;			/* Message size			*/
    int		size;			/* Size of slot			*/
    int		reps;			/* Field count			*/
    int		err;			/* Error detected		*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Preset failure and dispatch on the message type.
 -----------------------------------------------------------------------*/
    count_h(ch_total_recv);

    err   = 0;
    mlen  = len;
    ptr   = frame;
    type  = *ptr;

/*-----------------------------------------------------------------------
    Process Circuit Run messages.
 -----------------------------------------------------------------------*/
    if ((type & LAT_TYPE) == LAT_RUN)
    {
	cir  = (cir_iptr)frame;
	len -= sizeof(cir_msg);
	ptr += sizeof(cir_msg);

	if (cir->cir_dst_id == 0 || cir->cir_src_id == 0)
	    err = CIR_R_ERROR | FIELD_CIR_ID;

	else
	{
	    for (reps = cir->cir_slots; reps != 0 && len > 0; reps--)
	    {
		type = *(ptr+3) & SLOT_TYPE;

		if (type == SLOT_DATA_A || type == SLOT_DATA_B || type == SLOT_ATTENTION)
		{
		    if (*(ptr+0) == 0)
		    {
			err = CIR_R_ERROR | ESLOT_DST_ID;
			break;
		    }

		    if (*(ptr+1) == 0)
		    {
			err = CIR_R_ERROR | ESLOT_SRC_ID;
			break;
		    }
		}

		else if (type == SLOT_START)
		{
		    if (*(ptr+1) == 0)
		    {
			err = CIR_R_ERROR | ESLOT_SRC_ID;
			break;
		    }
		}

		else if (type == SLOT_REJECT ||
			 type == SLOT_STOP)
		{
		    if (*(ptr+0) == 0)
		    {
			err = CIR_R_ERROR | ESLOT_DST_ID;
			break;
		    }

		    if (*(ptr+1) != 0)
		    {
			err = CIR_R_ERROR | ESLOT_SRC_ID;
			break;
		    }
		}

		else
		{
		    err = CIR_R_ERROR | ESLOT_TYPE;
		    break;
		}

	        size = *(ptr+2) + 4;
	        if ((size & 1) && (size != len))
	            size++;

	        len -= size;
	        ptr += size;
	    }
	    if (err == 0 && len < 0)
		err = CIR_R_ERROR | MSG_LENGTH;
	}
    }
    
/*-----------------------------------------------------------------------
    Process Directory Service messages.
 -----------------------------------------------------------------------*/
    else if (type == LAT_SERVICE)
    {
	len -= sizeof(svc_msg);
	ptr += sizeof(svc_msg);

	if ((len -= (*ptr) + 1) < 0)
	    err = SVC_A_ERROR | MSG_LENGTH;
	else if (*ptr >= GROUPS)
	    err = SVC_A_ERROR | FIELD_GROUP;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = SVC_A_ERROR | MSG_LENGTH;
	else if (*ptr == 0 || !str_check(ptr,ND_STR))
	    err = SVC_A_ERROR | FIELD_NODE;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = SVC_A_ERROR | MSG_LENGTH;

	else
	{
	    ptr += (*ptr) + 1;
	    for (reps = *ptr++, len--; reps != 0 && err == 0; reps--)
	    {
		ptr++;
		len--;

		if ((len -= (*ptr) + 1) < 0)
		    err = SVC_A_ERROR | MSG_LENGTH;
		else if (*ptr == 0 || !str_check(ptr,SV_STR))
		    err = SVC_A_ERROR | FIELD_SERVICE;

		else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
		    err = SVC_A_ERROR | MSG_LENGTH;
		else
		    ptr += (*ptr) + 1;
	    }
	}
    }

/*-----------------------------------------------------------------------
    Process Circuit Start messages.
 -----------------------------------------------------------------------*/
    else if (type == LAT_START || type == (LAT_START | LAT_MASTER))
    {
	cir  = (cir_iptr)frame;
	str  = (str_iptr)(frame + sizeof(cir_msg));
	len -= sizeof(cir_msg) + sizeof(str_msg);
	ptr += sizeof(cir_msg) + sizeof(str_msg);

/*	if (cir->cir_slots != 0)
	    err = CIR_S_ERROR | FIELD_SLOTS;

	else */

	if (cir->cir_src_id == 0)
	    err = CIR_S_ERROR | FIELD_CIR_ID;

	else if ( (type & LAT_MASTER) && cir->cir_dst_id != 0)
	    err = CIR_S_ERROR | FIELD_CIR_ID;

	else if (!(type & LAT_MASTER) && cir->cir_dst_id == 0)
	    err = CIR_S_ERROR | FIELD_CIR_ID;

	else if ((type & LAT_MASTER) && (str->str_vc_timer == 0))
	    err = CIR_S_ERROR | FIELD_VC_TIMER;

        else if (type & LAT_MASTER)
        {
	    if ((len -= (*ptr) + 1) < 0)
	        err = CIR_S_ERROR | MSG_LENGTH;
	    else if (*ptr == 0 || !str_check(ptr,ND_STR))
	        err = CIR_S_ERROR | FIELD_NODE;

	    else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	        err = CIR_S_ERROR | MSG_LENGTH;

	    else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	        err = CIR_S_ERROR | MSG_LENGTH;
	}
    }
    
/*-----------------------------------------------------------------------
    Process Circuit Halt messages.
 -----------------------------------------------------------------------*/
    else if (type == LAT_HALT || type == (LAT_HALT | LAT_MASTER))
    {
	cir = (cir_iptr)frame;
	len -= sizeof(cir_msg) + sizeof(halt_msg);
	ptr += sizeof(cir_msg) + sizeof(halt_msg);

	if (cir->cir_slots != 0)
	    err = CIR_H_ERROR | FIELD_SLOTS;

	else if (cir->cir_dst_id == 0 || cir->cir_src_id != 0)
	    err = CIR_H_ERROR | FIELD_CIR_ID;
    }
    
/*-----------------------------------------------------------------------
    Process HIC Command messages.
 -----------------------------------------------------------------------*/
    else if (type == LAT_COMMAND)
    {
	len -= sizeof(cmd_msg);
	ptr += sizeof(cmd_msg);

	if ((len -= (*ptr) + 1) < 0)
	    err = HIC_CMD_ERROR | MSG_LENGTH;
	else if (*ptr == 0 || !str_check(ptr,ND_STR))
	    err = HIC_CMD_ERROR | FIELD_NODE;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_CMD_ERROR | MSG_LENGTH;
	else if (*ptr >= GROUPS)
	    err = HIC_CMD_ERROR | FIELD_GROUP;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_CMD_ERROR | MSG_LENGTH;
	else if (*ptr == 0 || !str_check(ptr,ND_STR))
	    err = HIC_CMD_ERROR | FIELD_NODE;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_CMD_ERROR | MSG_LENGTH;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_CMD_ERROR | MSG_LENGTH;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_CMD_ERROR | MSG_LENGTH;
	else if (!str_check(ptr,SV_STR))
	    err = HIC_CMD_ERROR | FIELD_SERVICE;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_CMD_ERROR | MSG_LENGTH;
	else if (!str_check(ptr,OB_STR))
	    err = HIC_CMD_ERROR | FIELD_PORT;
    }

/*-----------------------------------------------------------------------
    Process HIC Status messages.
 -----------------------------------------------------------------------*/
    else if (type == LAT_STATUS)
    {
	len -= sizeof(sts_msg);
	ptr += sizeof(sts_msg);

	reps = *ptr++;
	len--;

	if ((len -= (*ptr) + 1) < 0)
	    err = HIC_STS_ERROR | MSG_LENGTH;

	else
	{
	    ptr += (*ptr) + 1;

	    if ((ptr - frame) & 1)
	    {
	        len--;
	        ptr++;
	    }

	    for (; reps != 0 && len > 0; reps--)
	    {
		if ((len -= sizeof(req_field)) < 0)
	    	    err = HIC_STS_ERROR | MSG_LENGTH;

		else if (ptr += sizeof(req_field), (len -= (*ptr) + 1) < 0)
	    	    err = HIC_STS_ERROR | MSG_LENGTH;

		else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    	    err = HIC_STS_ERROR | MSG_LENGTH;

		else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    	    err = HIC_STS_ERROR | MSG_LENGTH;
	        
		else
		{
		    ptr += (*ptr) + 1;

		    if ((ptr - frame) & 1)
		    {
			len--;
			ptr++;
		    }
		}
	    }
        }
     }

/*-----------------------------------------------------------------------
    Process HIC Solicit messages.
 -----------------------------------------------------------------------*/
    else if (type == LAT_SOLICIT)
    {
	len -= sizeof(si_msg);
	ptr += sizeof(si_msg);

	if ((len -= (*ptr) + 1) < 0)
	    err = HIC_SI_ERROR | MSG_LENGTH;
	else if (!str_check(ptr,ND_STR))
	    err = HIC_SI_ERROR | FIELD_NODE;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_SI_ERROR | MSG_LENGTH;
	else if (*ptr >= GROUPS)
	    err = HIC_SI_ERROR | FIELD_GROUP;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_SI_ERROR | MSG_LENGTH;
	else if (*ptr == 0 || !str_check(ptr,ND_STR))
	    err = HIC_SI_ERROR | FIELD_NODE;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_SI_ERROR | MSG_LENGTH;
	else if (!str_check(ptr,SV_STR))
	    err = HIC_SI_ERROR | FIELD_SERVICE;
    }    

/*-----------------------------------------------------------------------
    Process HIC Information messages.
 -----------------------------------------------------------------------*/
     else if (type == LAT_INFO)
     {
	len -= sizeof(ri_msg);
	ptr += sizeof(ri_msg);

	if ((len -= (*ptr) + 1) < 0)
	    err = HIC_RI_ERROR | MSG_LENGTH;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_RI_ERROR | MSG_LENGTH;

	else if (ptr += (*ptr) + 1, (len -= (*ptr) + 1) < 0)
	    err = HIC_RI_ERROR | MSG_LENGTH;
	else if (*ptr == 0 || !str_check(ptr,ND_STR))
	    err = HIC_RI_ERROR | FIELD_NODE;
    }

/*-----------------------------------------------------------------------
    Illegal message type.
 -----------------------------------------------------------------------*/
    else
	err = MSG_OTHER | FIELD_TYPE;

/*-----------------------------------------------------------------------
    Check if message is valid.
 -----------------------------------------------------------------------*/
    if (err != 0)
    {
	count_h(ch_total_illegal);

	for (i = 0; i < ND_HASHES; i++)
	{
	    for (nd = hs->hs_nd_hash[i][0]; nd != 0; nd = nd->nd_link[0])
	    {
		if (nd->nd_host_addr[0] == src[0] &&
		    nd->nd_host_addr[1] == src[1] &&
		    nd->nd_host_addr[2] == src[2] &&
		    nd->nd_idb == idb)
		{
		    count_1(co_msgs_bad);
		    break;
		}
		if (nd != 0)
		    break;
	    }
	}

#if LE_HS_STATS
	hs->hs_count.ch_err_masks[err & 0xff] |= 1 << ((err&0xff00) >> 8);
	hs->hs_count.ch_err_source[0] = src[0];
	hs->hs_count.ch_err_source[1] = src[1];
	hs->hs_count.ch_err_source[2] = src[2];
    	hs->hs_count.ch_idb = idb;
#endif

	ix_error(frame,src,mlen,err,idb);
	return(FAILURE);
    }
    else
	return(SUCCESS);
}
