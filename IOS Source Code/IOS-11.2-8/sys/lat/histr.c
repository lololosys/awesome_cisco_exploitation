/*-----------------------------------------------------------------------
    hi_start		(i) start by request number

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    A start slot with request number has arrived. See if we can start and
    if so notify the environment.

    CALL BY:	void hi_start(sb,req_no,service,port)
		sb_ptr	sb;
    		word	req_no;
    		byte	*service;
    		byte	*port;

    ON EXIT:	Set sb_stop_reason if no pending HIC request.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "lathi.h"
#include "latsb.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"
#if CME_SLAT
#include "cmefnc.h"
#endif

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	byte	id_temp[];		/* Temporary password storage	*/

void hi_start(

    sb_ptr	sb,			/* Pointer to session block	*/
    word	req_no)			/* Request number		*/
{
    hi_ptr	hi;			/* Pointer to HIC block		*/

/*-----------------------------------------------------------------------
    See if request is defined at this node and start session if it is.
 -----------------------------------------------------------------------*/
#if LE_HOST
    if ((hi = hi_find(req_no,0,HI_SUBJECT,0)) != 0  && hi->hi_sid != SID_NULL)
    {
    	if (hi->hi_pass[0] != 0)
	{
	    hi_str_move_ls(&id_temp[0],&hi->hi_pass[0]);
	    sb_str_move_sl(&sb->sb_password[0],&id_temp[0]);
	}	
#else
    if ((hi = hi_find(0,req_no,HI_OBJECT,0))  != 0  && hi->hi_sid != SID_NULL)
    {
#endif
	sb->sb_flags = SB_STALL;
	sb->sb_sid   = hi->hi_sid;
#if CME_SLAT
	sb->sb_cid   = hi->hi_cid;
        hi->hi_cid   = CID_NULL;
#elif LE_OBJECTS	
	sb->sb_ob    = hi->hi_ob;
	hi->hi_ob    = 0;
#endif

	sx_session(sb,SX_HI_ACCESS);
	hi_delete(hi,0);
    }
    else
    {
	sb->sb_stop_reason = SLOT_STOP_BAD_SOLCIT;
	if (hi != 0)
	    hi_delete(hi,HX_REJECTED);
    }
}
