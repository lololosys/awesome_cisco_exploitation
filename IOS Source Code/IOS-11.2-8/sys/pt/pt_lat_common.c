/* $Id: pt_lat_common.c,v 3.2 1995/11/17 18:59:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/pt_lat_common.c,v $
 *------------------------------------------------------------------
 * pt_lat_common.c  Protocol translation common subfunctions shared by
 * lat to auto/pad/slip_ppp files.
 *
 * Dec. 1994, Eman Schaffer
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pt_lat_common.c,v $
 * Revision 3.2  1995/11/17  18:59:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "ttysrv.h"
#include "connect.h"
#include "translate.h"
#include "../os/async.h"
#include "../pt/pt_lat_common.h"

extern vectortype LATvector;

/*
 * pt_setup_lat_session
 *
 */
void pt_setup_lat_session (translatetype *trans, tt_soc *vty)
{
    conntype *latconn = vty->conntop;

    vty->ovector = &LATvector;
    vty->ivector = &LATvector;

    if (latconn->proto.lat.flags & LAT_CONN_HIC) {
    if (translate_debug)
	buginf("\n%s%s%t: HIC connection is not supported",
	       reg_invoke_pt_translate_string(INPROT(trans->type),
					      INPROT(trans->type)),
	       reg_invoke_pt_translate_string(OUTPROT(trans->type), 
					      OUTPROT(trans->type)),
	       vty->ttynum);
            trans->failures++;
            death(vty);
    }

    trans->active++;
    if (trans->active > trans->maxusers)
        trans->maxusers = trans->active;

    pt_do_login(trans, vty);
    psioff(vty);
    vty->statbits |= CONNECTED;         /* Mark as connected */
}
