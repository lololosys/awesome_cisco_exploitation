/*-----------------------------------------------------------------------
    sx_signal		(x) signal session condition

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT engine whenever a session
    out-of-band condition occurs.

    CALL BY:	void sx_signal(sb,reason)
    		sb_ptr	sb;
    		int	reason;

    ON EXIT:	The condition is handled appropriately for the environment.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsb.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

void sx_signal(sb,reason)

    sb_ptr	sb;			/* Pointer to session block	*/
    int		reason;			/* Signal reason		*/
{

/*-----------------------------------------------------------------------
    Using the SID, Get the environment session data.
 -----------------------------------------------------------------------*/
    <get environment session data from sb->sb_sid>

/*-----------------------------------------------------------------------
    Check for possible settings of reason.
 -----------------------------------------------------------------------*/
    switch (reason)
    {

/*-----------------------------------------------------------------------
    If flush, flush all pending output if possible.  Doing nothing is
    okay.
 -----------------------------------------------------------------------*/
        case SX_FLUSH:
	    break;

/*-----------------------------------------------------------------------
    If SX_FORCE_XON, resume output stopped from an XOFF as if an XON
    were received from the terminal device.
 -----------------------------------------------------------------------*/
	case SX_FORCE_XON:
	    break;

/*-----------------------------------------------------------------------
    If SX_DATA_B_SET, the remote session wants to change the local object's
    configuration.  Transparency mode is in sb->sb_data_b_rmt_mode.
    If you are not using CME, other local information is in
    sb->sb_ob->ob_tmp_data.db_XXX.
    If you are using CME, other local information is in the CM structure
    whose 'cid' is the value of sb->sb_cid, and it's in the fields:
	cm->cm_lat_datab_tmp.db_XXX
    where cm is a pointer to the CM structure corresponding to 'cid'.

    If you change the local port configuration, you should copy the
    relevant fields from ->ob_tmp_data.db_XXX to ->ob_lcl_data.db_XXX
    (or for CME, from ->cm_lat_datab_tmp.db_XXX to
    ->cm_lat_datab_lcl.db_XXX.  Whether or not you change the local
    port configuration, you should call db_report_config() to notify
    the remote session of the the current port configuration. 
  -----------------------------------------------------------------------*/
	case SX_DATA_B_SET:
	    break;

/*-----------------------------------------------------------------------
    If SX_DATA_B_REP, the remote object associated with the session
    is reporting its transparency mode (sb->sb_data_b_rmt_mode) and 
    configuration (sb->sb_rmt_data.db_XXX).

    If your environment handles status reasons (ie short break, long
    break, framing error, data overrun, or parity error), the status code
    and bad character can be found in sb->sb_rmt_data_b_status and 
    sb->sb_rmt_data_b_status_char.  You may check for the break condition
    using the statement:  sb->sb_rmt_data.db_config & SLOT_BREAK; and 
    for a status reason using the statement: sb->sb_rmt_data.db_flags & 
    DB_E_STATUS.  

    After processing this information, sb->sb_rmt_data.db_config field 
    should be cleared of the break condition.

-----------------------------------------------------------------------*/
	case SX_DATA_B_REP:
	    sb->sb_rmt_data.db_config &= ~SLOT_BREAK;
	    break;

    }
}
