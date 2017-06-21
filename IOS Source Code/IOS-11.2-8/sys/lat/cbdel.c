/*-----------------------------------------------------------------------
    cb_delete		(i) delete circuit structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes a circuit structure. The function also deletes
    any sessions, free any output frames, and unlocks the node structure
    usage.

    CALL BY:	void cb_delete(cb)
    		cb_ptr	cb;

    ON EXIT:	The ciruit structure is deleted, all session are deleted,
		any output frames are returned, and the node usage is
		unlocked.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latcb.h"
#include "latnd.h"
#include "latsb.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern  hs_ptr	hs;			/* Local node definitions	*/
extern	cb_ptr	reject_cb[];		/* Reject CB table		*/
extern	byte	reject_id[];		/* Reject slot ID's		*/
extern	int	reject_index;		/* Current pending rejects	*/

void cb_delete(cb)

    cb_ptr	cb;			/* Pointer to CB structure	*/
 {
    of_ptr	of;			/* Output frame pointer		*/
    int		cir;			/* Circuit identification	*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Remove any entries for this circuit from reject table.
 -----------------------------------------------------------------------*/
    for (i = 0; i < reject_index; i++)
    {
	if (reject_cb[i] == cb)
	{
	    --reject_index;
	    reject_cb[i] = reject_cb[reject_index];
	    reject_id[i] = reject_id[reject_index];
	    i--;
	}
    }

/*-----------------------------------------------------------------------
    Mark the slot in the LAT table as free and if need, reset the upper
    limit of the table. This prevents possible loop if environment tries
    to use the this circuit in new sb_open.
 -----------------------------------------------------------------------*/
    cir = LOBYTE(cb->cb_lcl_id);
    hs->hs_cb_table[cir] = 0;

    if (cir == hs->hs_cb_index-1)
    {
	for (hs->hs_cb_index--; hs->hs_cb_index != 1; hs->hs_cb_index--)
	{
	    if (hs->hs_cb_table[hs->hs_cb_index-1] != 0)
		break;
	}
    }

/*-----------------------------------------------------------------------
    If circuit is going down for an abnormal reason, mark the remote
    node as unreachable. This is done now so sessions may failover to
    an alternate service.
 -----------------------------------------------------------------------*/
    if (cb->cb_halt_reason != HALTC_DONE && cb->cb_halt_reason != HALTC_NONE)
	nd_fail(cb->cb_nd,ND_UNREACH);

/*-----------------------------------------------------------------------
    Delete any session still on this circuit.
 -----------------------------------------------------------------------*/
    while (cb->cb_slot[0] != 0)
	sb_delete(cb->cb_slot[0],SX_CIRCUIT);

/*-----------------------------------------------------------------------
    Count no longer using the node and delete node if no other usage.
 -----------------------------------------------------------------------*/
    cb->cb_nd->nd_usage--;
    nd_fail(cb->cb_nd,0);

/*-----------------------------------------------------------------------
    Free any output frame buffers the circuit may have allocated.
 -----------------------------------------------------------------------*/
    while ((of = cb->cb_xmit[0]) != 0)
    {
	if ((cb->cb_xmit[0] = of->of_link) == 0)
	     cb->cb_xmit[1] = (of_ptr)&cb->cb_xmit[0];

	if (of->of_size & OF_OUTPUT)
	    of->of_size |= OF_DELETE;
	else
	    of_free(of);
    }

/*-----------------------------------------------------------------------
    Deallocate the circuit structure.
 -----------------------------------------------------------------------*/
    hs->hs_cb_current--;
    cb_free(cb);
}
