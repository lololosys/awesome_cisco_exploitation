/*-----------------------------------------------------------------------
    ob_delete		(e) delete object and any references to it

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes the ob_node structure and all references to it:
    remove all references from advertised services, delete any host-
    initiated pending requests, and stop all sessions using the object.

    CALL BY:	void ob_delete(ob)
    		ob_ptr     ob;

    ON EXIT:	The object structure is deleted along with all references.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "latcb.h"
#include "lathi.h"
#include "latob.h"
#include "latsb.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	ob_ptr	ob_map[];		/* Object mapping table		*/
extern	ob_ptr	ob_head[2];		/* OB structure list header	*/
extern	int	ob_current;		/* Current number objects	*/
extern	int	ob_highest;		/* Largest number objects	*/

void ob_delete(ob)

    ob_ptr	ob;                     /* Pointer to OB structure	*/
{
    cb_ptr	cb;			/* Pointer to CB structure	*/
    sb_ptr	sb;			/* Pointer to SB structure	*/
    sb_ptr	sx;			/* Pointer to SB structure	*/
    int		i;			/* Do index			*/

/*------------------------------------------------------------------------
    Check delete pending flag; if set, do nothing.  Needed to prevent
    recursive calls to routine.
 ------------------------------------------------------------------------*/
    if (ob->ob_flags & OB_DEL_PEND)
	;
    else
    {
#if LE_HOST_AS
/*------------------------------------------------------------------------
    Scan the advertised services and remove this object; but, only if
    it is not cloned.
 ------------------------------------------------------------------------*/
        if (!(ob->ob_flags & OB_CLONED))
        {
            as_ptr as;

	    for (as = hs->hs_as_head[0]; as != 0; as = as->as_link[0])
	        as_clr_object(as,ob->ob_oid);
        }
#endif

#if LE_SUBJECT_HIC || LE_OBJECT_HIC
/*------------------------------------------------------------------------
    Scan the HIC requests and delete any HIC requests associated
    with this object.
 ------------------------------------------------------------------------*/
        {
	    hi_ptr  hi;
	    hi_ptr  hx;

	    for (hi = hs->hs_hi_head[0]; hi != 0; hi = hx)
	    {
	        hx = hi->hi_link[0];

	        if (hi->hi_ob == ob)
	        {
		
		    if (hi->hi_flags & HI_SUBJECT)
		    {
		        hi->hi_status = STS_ERROR;
		        hi->hi_error  = SLOT_STOP_SYSTEM;
		        hi_delete(hi,HX_TIMEOUT);
		    }
		    else
		        hi_delete(hi,0);
	        }
	    }
        }
#endif

/*------------------------------------------------------------------------
    Clear the object's groups.
 ------------------------------------------------------------------------*/
        idl_clr_group(ob,-1);
    
/*------------------------------------------------------------------------
    Scan the current sessions and stop any sessions using this object.
 ------------------------------------------------------------------------*/
        for (i = 1; i < hs->hs_cb_index; i++)
        {
	    if ((cb = hs->hs_cb_table[i]) != 0)
	    {
	        for (sb = cb->cb_slot[0]; sb != 0; sb = sx)
	        {
		    sx = sb->sb_slot[0];
		    if (sb->sb_ob == ob)
		    {
		        if (sb->sb_sid != SID_NULL)
			    sx_session(sb,SX_STOP);

		        sb->sb_ob = 0;
		        sb->sb_sid = SID_NULL;
		        sb->sb_stop_reason = SLOT_STOP_SYSTEM;
		        sb_signal(sb,(int)SB_STOP,0);
		    }
	        }
	    }
        }

/*------------------------------------------------------------------------
    Remove object pointer from the object mapping table.
 ------------------------------------------------------------------------*/
        for (i = 0; i < ob_highest; i++)
        {
	    if (ob_map[i] == ob)
	    {
	        ob_map[i] = 0;
	        break;
	    }
        }

/*------------------------------------------------------------------------
    Remove the object from the list and deallocate.
 ------------------------------------------------------------------------*/
        if (ob->ob_link[0] != 0)
	    ob->ob_link[0]->ob_link[1] = ob->ob_link[1];
        else
	    ob_head[1] = ob->ob_link[1];

        if (ob->ob_link[1] != 0)
	    ob->ob_link[1]->ob_link[0] = ob->ob_link[0];
        else
	    ob_head[0] = ob->ob_link[0];

        ob_current--;
        ob_free(ob);

#if LE_OBJECTS && LE_OBJECT_HIC
        {
	    extern int qr_changes;
	    qr_changes = SUCCESS;
        }
#endif
    }
}
