/* $Id: dbsetc.c,v 3.2 1995/11/17 17:32:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/dbsetc.c,v $
 *------------------------------------------------------------------
 * $Log: dbsetc.c,v $
 * Revision 3.2  1995/11/17  17:32:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:22:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    db_set_config	(e) change remote configuration

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function changes a remote configuration by causing the engine to
    issue a SET DATA-B slot. This will cause the remote
    side to change its (local) characteristics.  Bits in the flags field
    are set for those values you wish to change using the DB_E_xxx masks
    (in latsys.p).  The inp and out arguments are booleans which you should
    set to 1 to enable flow control and 0 to disable flow control.

    CALL BY:	void db_set_config(sb,flags,recv,xmit,parity,bell,inp,out)
    		sb_ptr	sb;
		word	flags;
    		word	recv;
    		word	xmit;
    		byte	parity;
    		int	bell;
    		int	inp;
    		int	out;


    ON EXIT:	Updates the sb_tmp_data fields in the sessions data-b
    		structure.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latob.h"
#include "latsb.h"
#include "latslt.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void db_set_config(

    sb_ptr	sb,			/* Pointer to session block	*/
    word	flags,			/* Set parameter flags		*/
    word	recv,			/* Port receive	 speed		*/
    word	xmit,			/* Port transmit speed		*/
    byte	parity,			/* Character size, parity	*/
    byte	bell,			/* Discard bell (1 = yes)	*/
    int		inp,			/* port->TTY flow (1 = on)	*/
    int		out)			/* TTY->port flow (1 = on)	*/

{
    int		change;			/* Port configution changed	*/

/*-----------------------------------------------------------------------
    Flag no changes seen.
------------------------------------------------------------------------*/
    change = FAILURE;

/*-----------------------------------------------------------------------
    Clear the field flags.  This way, only those fields which have
    changed are transmitted in the SET DATA-B slot; however, leave the
    mode flag alone, it may have been set by db_set_mode().
 -----------------------------------------------------------------------*/
    sb->sb_tmp_data.db_flags &= DB_E_MODE;

/*-----------------------------------------------------------------------
    Update the port receive/transmit speed, character size/parity, bell.
------------------------------------------------------------------------*/
    if (flags & DB_E_RECV)
    {
	sb->sb_tmp_data.db_baud_recv = recv;
	sb->sb_tmp_data.db_flags |= DB_E_RECV;
	change = SUCCESS;
    }

    if (flags & DB_E_XMIT)
    {
	sb->sb_tmp_data.db_baud_xmit = xmit;
	sb->sb_tmp_data.db_flags |= DB_E_XMIT;
	change = SUCCESS;
    }

    if (flags & DB_E_PARITY)
    {
	sb->sb_tmp_data.db_char_parity = parity;
	sb->sb_tmp_data.db_flags |= DB_E_PARITY;
	change = SUCCESS;
    }

    if (flags & DB_E_BELL)
    {
	sb->sb_tmp_data.db_bell_mode = bell;
	sb->sb_tmp_data.db_flags |= DB_E_BELL;
	change = SUCCESS;
    }

/*-----------------------------------------------------------------------
    Update the flow control flags.
------------------------------------------------------------------------*/
    if (inp)
    {
	if ((sb->sb_tmp_data.db_config & SLOT_INP_OFF) != 0)
	    change = SUCCESS;
	if ((sb->sb_tmp_data.db_config & SLOT_INP_ON ) == 0)
	    change = SUCCESS;

	sb->sb_tmp_data.db_config |= SLOT_INP_ON;
	sb->sb_tmp_data.db_config &= ~SLOT_INP_OFF;
    }
    else
    {
	if ((sb->sb_tmp_data.db_config & SLOT_INP_OFF) == 0)
	    change = SUCCESS;
	if ((sb->sb_tmp_data.db_config & SLOT_INP_ON ) != 0)
	    change = SUCCESS;

	sb->sb_tmp_data.db_config &= ~SLOT_INP_ON;
	sb->sb_tmp_data.db_config |= SLOT_INP_OFF;
    }

    if (out)
    {
	if ((sb->sb_tmp_data.db_config & SLOT_OUT_OFF) != 0)
	    change = SUCCESS;
	if ((sb->sb_tmp_data.db_config & SLOT_OUT_ON ) == 0)
	    change = SUCCESS;

	sb->sb_tmp_data.db_config |= SLOT_OUT_ON;
	sb->sb_tmp_data.db_config &= ~SLOT_OUT_OFF;
    }
    else
    {
	if ((sb->sb_tmp_data.db_config & SLOT_OUT_OFF) == 0)
	    change = SUCCESS;
	if ((sb->sb_tmp_data.db_config & SLOT_OUT_ON ) != 0)
	    change = SUCCESS;

	sb->sb_tmp_data.db_config &= ~SLOT_OUT_ON;
	sb->sb_tmp_data.db_config |= SLOT_OUT_OFF;
    }

/*-----------------------------------------------------------------------
    If any changes issue DATA-B.
 -----------------------------------------------------------------------*/
    if (change)
    {
    	sb->sb_data_b_type |= SB_DATA_B_SET;
        sb_signal(sb,SB_DATA_B,0);
    }	
}
