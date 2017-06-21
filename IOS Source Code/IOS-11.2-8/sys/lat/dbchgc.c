/* $Id: dbchgc.c,v 3.2 1995/11/17 17:32:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/dbchgc.c,v $
 *------------------------------------------------------------------
 * $Log: dbchgc.c,v $
 * Revision 3.2  1995/11/17  17:32:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    db_change_config	(e) change local configuration

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function updates an object's local port configuration.
    The function searches for any sessions using the object
    and marks them to send a REPORT DATA-B slot.

    If your environment is using SuperLAT objects, pass an object pointer
    in the 'port' parameter; if it is using CME, pass the CME port's cid.
    In either case, the SuperLAT engine willl put the configuration
    information in the object or CME port data-b structure and then send
    the report data-b slot(s).

    CALL BY:	void db_change_config(port,recv,xmit,parity,bell,inp,out)
    		lat_port  ob;
    		word	  recv;
    		word	  xmit;
    		byte	  parity;
    		int	  bell;
    		int	  inp;
    		int	  out;

    ON EXIT:	Updates the local data-b fields in the appropriate
    		port structure.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latob.h"
#include "latsb.h"
#include "latslt.h"
#include "latfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
#if !(LE_OBJECTS || CME_SLAT)
extern	ob_node	ob_dummy;		/* Dummy object structure	*/
#endif

void db_change_config(

    lat_port	port,			/* Port ID;  'ob' pointer or
					     CME id			*/
    word	recv,			/* Port receive	 speed		*/
    word	xmit,			/* Port transmit speed		*/
    byte	parity,			/* Character size, parity	*/
    byte	bell,			/* Discard bell (1 = yes)	*/
    int		inp,			/* port->TTY flow (1 = on)	*/
    int		out)			/* TTY->port flow (1 = on)	*/
{
    cb_ptr	cb;			/* Pointer to circuit block	*/
    sb_ptr	sb;			/* Pointer to session block	*/
    sb_ptr	sx;			/* Pointer to session block	*/
#if CME_SLAT
    dab_ptr	db;			/* Pointer to data-b structure	*/
#else
    data_b	OB_VIEW(db);
#endif

    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Get the pointer to the data-b structure.
 -----------------------------------------------------------------------*/
#if CME_SLAT
    db = cx_data_b(port,CX_DATAB_LCL);
#elif LE_OBJECTS
    db = &port->ob_lcl_data;
#else
    db = &ob_dummy.ob_lcl_data;
#endif

/*-----------------------------------------------------------------------
    Set the field flags.  Because this function generates a REPORT DATA-B
    slot, we send all of the object fields, whether they have changed
    or not.
 -----------------------------------------------------------------------*/
    db->db_flags = DB_E_PORT;

/*-----------------------------------------------------------------------
    Update the port receive/transmit speed, character size/parity, bell.
------------------------------------------------------------------------*/
    db->db_baud_recv   = recv;
    db->db_baud_xmit   = xmit;
    db->db_char_parity = parity;
    db->db_bell_mode   = bell;

/*-----------------------------------------------------------------------
    Update the flow control flags.
------------------------------------------------------------------------*/
    if (inp)
    {
	db->db_config |= SLOT_INP_ON;
	db->db_config &= ~SLOT_INP_OFF;
    }
    else
    {
	db->db_config &= ~SLOT_INP_ON;
	db->db_config |= SLOT_INP_OFF;
    }

    if (out)
    {
	db->db_config |= SLOT_OUT_ON;
	db->db_config &= ~SLOT_OUT_OFF;
    }
    else
    {
        db->db_config &= ~SLOT_OUT_ON;
	db->db_config |= SLOT_OUT_OFF;
    }

/*-----------------------------------------------------------------------
    Issue a REPORT DATA-B on all sessions using the port.
 -----------------------------------------------------------------------*/
    for (i = 1; i < hs->hs_cb_index; i++)
    {
	if ((cb = hs->hs_cb_table[i]) != 0)
	{
	    for (sb = cb->cb_slot[1]; sb != 0; sb = sx)
	    {
		sx = sb->sb_slot[1];
#if CME_SLAT
		if (sb->sb_cid == port)
#else
		if (sb->sb_ob == port)
#endif
		{
		    sb->sb_data_b_type |= SB_DATA_B_REP;
		    sb_signal(sb,SB_DATA_B,0);
		}
	    }
	}
    }
}
