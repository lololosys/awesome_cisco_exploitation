/* $Id: rsp_esa.c,v 3.1.2.3 1996/09/05 21:47:50 ptran Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_esa.c,v $
 *------------------------------------------------------------------
 * Encryption Service Adaptor support code
 *
 * August 1996, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../hes/msg_cbus.c"
#include "plural.h"
#include "subsys.h"
#include "interface_private.h"
#include "rsp_if.h"
#include "rsp_bufhdr.h"
#include "rsp_qasic.h"
#include "rsp_pcmap.h"
#include "sys_registry.h"
#include "packet.h"
#include "sys_registry.h"
#include "../hes/cbus_registry.h"
#include "media_registry.h"
#include "../hes/if_hes_common.h"
#include "rsp_common_if.h"
#include "rsp_vip.h"
#include "../if/ether.h"
#include "../hes/if_cmd.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "lovenote.h"
#include "../hes/if_hes_ether.h"
#include "../hes/if_feip.h"
#include "../hes/msg_cbus.h"
#include "ttysrv.h"




/*****************************************************************************
 * Name: rsp_esa_idb_init
 *
 * Description: platform specific encryption service adaptor init
 *
 * Input: hwidb
 *   
 * Return Value: None
 *
 *****************************************************************************/
void rsp_esa_idb_init (hwidbtype *hwidb, boolean vip_style)
{

    /* in case anybody tries to reset us before we're ready */
    hwidb->reset = (reset_t) return_nothing;
    hwidb->periodic = NULL;


    if (hwidb->status & IDB_NOHARDWARE) {
	hwidb->status &= ~IDB_NOHARDWARE;
	return;
    }

    hes_idb_init_before(hwidb);
    hwidb->status |= IDB_VIP;
    hwidb->status |= IDB_DELETED;
    hwidb->love_note = (void *)vip_rsp_love_letter;

    /*
     * Complete set up of common fields.
     */
    /* name need to be initialize so the show cont cx command
     * print out the interface name otherwise it would be blank
     */
    hwidb->name = "Encryption";
    hwidb->buffersize = 1;
    init_new_hwidb(hwidb);
    hes_idb_init_after(hwidb);
}

