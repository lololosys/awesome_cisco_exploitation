/*-----------------------------------------------------------------------
    as_message		(i) generate directory service message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function generates a directory service message. If any changes
    have been flagged since the last message, a new directory sequence
    number is gnerated.

    CALL BY:	int as_message(of)
		of_ptr	of;

    ON EXIT:	Advertised service message generated into output frame.
		If no need to send message, function returns FAILURE.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

int as_message(of)

    of_ptr	of;			/* Pointer to output frame	*/
{
    as_ptr	as;			/* Pointer to AS structure	*/
    svc_optr	svc;			/* Pointer to output frame	*/
    of_byte	next;			/* Byte pointer in output frame */
    of_byte	num;			/* Number of as services in msg	*/
    byte	rate;			/* Rating of client service	*/
    int		sts;			/* Status return value		*/

/*-----------------------------------------------------------------------
    Preset no services generated and make sure there are some advertised
    services present or service changes before going to any trouble.
    Also, check that service announcements are enabled.
 -----------------------------------------------------------------------*/
    sts = FAILURE;

    if ((hs->hs_status & SVC_ANNC_EN) && (hs->hs_as_head[0] != 0 || hs->hs_as_next_changes != 0))
    {

/*-----------------------------------------------------------------------
    Get pointer to data portion of frame and set constant fields.
 -----------------------------------------------------------------------*/
	svc = (svc_optr)of->of_data;
	svc->svc_type = LAT_SERVICE;
	svc->svc_vc_timer    = hs->hs_cb_vc_timer / 10;
	svc->svc_prot_hi     = hs->hs_prot_hi;
	svc->svc_prot_lo     = hs->hs_prot_lo;
	svc->svc_prot_ver    = hs->hs_prot_ver;
	svc->svc_prot_eco    = hs->hs_prot_eco;
	svc->svc_dll_max     = (hs->hs_inp_max != 0) ? swap(hs->hs_inp_max) : swap(hs->hs_dll_max);
	svc->svc_mc_timer    = hs->hs_as_timer;
	svc->svc_node_status = (byte)(LOBYTE(hs->hs_status)) & SVC_NODE_OFF;

/*-----------------------------------------------------------------------
    Scan services and update the dynamic service ratings.
 -----------------------------------------------------------------------*/
 	for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
	{
	    if (as->as_flags & AS_DYNAMIC)
	    {
	        if (as->as_rating != hs->hs_as_next_rating)
		{
		    as->as_rating  = hs->hs_as_next_rating;
	    	    hs->hs_as_next_changes |= SVC_RATING;
		}
	    }
	    else if (as->as_flags & AS_CLIENT)
	    {
	        if (as->as_rating != (rate = ax_set_rate(as)))
		{
		    as->as_rating  = rate;
	    	    hs->hs_as_next_changes |= SVC_RATING;
		}
	    }
	}

/*-----------------------------------------------------------------------
    If something has changed, set the flags and increment the directory
    sequence number. Otherwise, use the last sequence number.
 -----------------------------------------------------------------------*/
	if (hs->hs_as_next_changes != 0)
	{
	    hs->hs_as_last_changes ^= hs->hs_as_next_changes;
	    hs->hs_as_sequence++;
	}
	svc->svc_sequence = hs->hs_as_sequence;
	svc->svc_changes  = hs->hs_as_last_changes;

/*-----------------------------------------------------------------------
    Get a byte pointer to output frame and store the host group codes.
 -----------------------------------------------------------------------*/
	next = (of_byte)svc + sizeof(svc_msg);
	next = hs_str_move_os(next,&hs->hs_acl_group[0]);

/*-----------------------------------------------------------------------
    Store the host name and identification.
 -----------------------------------------------------------------------*/
	next = hs_str_move_os(next,&hs->hs_node[0]);
	next = hs_str_move_os(next,&hs->hs_ident[0]);

/*-----------------------------------------------------------------------
    Save the current address of next to fill in the services count.
 -----------------------------------------------------------------------*/
	num  = next++;
	*num = 0;

/*-----------------------------------------------------------------------
    Loop through all the advertised services.  If the service is being
    offered, add it to the output frame.
 -----------------------------------------------------------------------*/
	for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
	{
	    if (as->as_flags & AS_OFFERED)
	    {
		*next++ = as->as_rating;

		next = as_str_move_os(next,&as->as_service[0]);
		next = as_str_move_os(next,&as->as_ident[0]);
		*num += 1;
	    }
	}
	next = hs_str_move_os(next,&hs->hs_class[0]);

/*-----------------------------------------------------------------------
    Check again if there is any reason to send this message. We must
    store at least one service name or had some changes flaged. The
    second case will cause us to send one message when all service names
    are deleted.
 -----------------------------------------------------------------------*/
	if (*num != 0 || hs->hs_as_next_changes != 0)
	{
	    count_h(ch_svc_xmit);

	    of->of_size   = (int)(next - of->of_data);
	    of->of_size  |= OF_DELETE;

	    of->of_dst[0] = hs->hs_multicast[0];
	    of->of_dst[1] = hs->hs_multicast[1];
	    of->of_dst[2] = hs->hs_multicast[2];
	    if (*num == 0)
		svc->svc_node_status |= SVC_NODE_OFF;
	    sts = SUCCESS;
        }
	hs->hs_as_next_changes = 0;
    }
    return(sts);
}
