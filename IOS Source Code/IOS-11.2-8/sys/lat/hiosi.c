/* $Id: hiosi.c,v 3.2 1995/11/17 17:32:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/hiosi.c,v $
 *------------------------------------------------------------------
 * $Log: hiosi.c,v $
 * Revision 3.2  1995/11/17  17:32:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:23:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*-----------------------------------------------------------------------
    hi_o_si_message	(i) output solicit message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called to output a solicit message.

    CALL BY:	void hi_o_si_message(of,hi)
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

void hi_o_si_message(of,hi)

    of_ptr	of;			/* Output frame pointer		*/
    hi_ptr	hi;			/* HIC request pointer		*/
{
    si_optr	si;			/* Output solicit message	*/
    of_byte	next;			/* Output byte pointer		*/

/*-----------------------------------------------------------------------
    Fill in the fixed fields of a solicit host-initiated message.
 -----------------------------------------------------------------------*/
    si = (si_optr)of->of_data;
    next = of->of_data + sizeof(si_msg);

    si->si_type     = hi->hi_function;
    si->si_format   = 0;
    si->si_prot_hi  = hs->hs_prot_hi;
    si->si_prot_lo  = hs->hs_prot_lo;
    si->si_prot_ver = hs->hs_prot_ver;
    si->si_prot_eco = hs->hs_prot_eco;
    si->si_dll_max  = (hs->hs_inp_max != 0) ? swap(hs->hs_inp_max) : swap(hs->hs_dll_max);
    si->si_req_no   = swap(hi->hi_req_no);
    si->si_timer    = swap(hs->hs_hi_cmd_timer);

/*-----------------------------------------------------------------------
    Complete the variable fields in a solcit message.
 -----------------------------------------------------------------------*/
    next = (of_byte)hi_str_move_os(next,&hi->hi_node[0]);

#if (LE_OBJECTS || CME_SLAT)
#if LE_OBJECTS
    if (hi->hi_ob != 0)
        next = (of_byte)ob_str_move_os(next,&hi->hi_ob->ob_idl[0]);
#elif CME_SLAT
    if (hi->hi_cid != CID_NULL)
	next = (of_byte)cx_move(next,hi->hi_cid,CX_LAT_GROUP);
#endif
    else
        *next++ = 0;
#else
    next = (of_byte)hs_str_move_os(next,&hs->hs_idl_group[0]);
#endif

    next = (of_byte)hs_str_move_os(next,&hs->hs_node[0]);
    next = (of_byte)hi_str_move_os(next,&hi->hi_service[0]);

/*-----------------------------------------------------------------------
 * Set the required service class
 -----------------------------------------------------------------------*/

    if (hi->hi_sc) {
	*next++ = SI_P_SC;	/* Specify service class parameter */
	*next++ = 1;		/* Length = 1 */
	*next++ = hi->hi_sc;	/* Specify the service class */
    }
/*-----------------------------------------------------------------------
    Set end of parameters and compute length of message.
 -----------------------------------------------------------------------*/
    *next++ = 0;
    of->of_size = (int)(next - of->of_data);
}
