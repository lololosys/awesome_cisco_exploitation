/* $Id: Syn_ipc.c,v 3.1.6.2 1996/06/19 19:30:16 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/Syn_ipc.c,v $
 *-----------------------------------------------------------------
 * Syn_ipc.c -- IPC between ACP and NMP
 *
 * March 1995, sakumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: Syn_ipc.c,v $
 * Revision 3.1.6.2  1996/06/19  19:30:16  snyder
 * CSCdi60807:  spelllink errors
 * Branch: California_branch
 *
 *         unexpeced -> unexpected
 *
 * Revision 3.1.6.1  1996/05/09  14:40:54  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.52.1  1996/04/27  07:25:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.40.1  1996/04/08  02:08:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.24.3  1996/03/27  21:55:21  sakumar
 * Got rid of some compiler warnings.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.2  1996/03/22  23:40:04  rlowe
 * Give relative paths to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.1  1996/03/22  09:43:26  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.2.2  1996/03/20  03:11:59  sakumar
 * Apply ATM line card 2.1 fixes to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.2.1  1996/03/13  03:18:33  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1  1996/03/11  08:15:21  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.7  1995/10/04  01:30:46  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.6  1995/06/23  17:01:57  sakumar
 * Enabled call to biga_idb_rx() (previously commented out) to allow
 * low-level IPC to send packets to IP.
 *
 * Revision 1.5  1995/06/15  21:02:54  rahuja
 * 	Added get_vcd_from_vpc, get_vcd_from_vpc calls in atmdriver.c
 * 	Started ipctest from atmtest.c
 *
 * Revision 1.4  1995/05/08  01:08:41  jjchen
 * Hooked upper layer to process packets.
 *
 * Revision 1.3  1995/05/02  04:28:17  rlowe
 * Fix upcall registration and use, so that ATM driver will work even if
 * NO upcall is current (fcn pointer is NULL).  Add upcall registration
 * to Syn_ipc.c test file.
 *
 * Revision 1.2  1995/04/08  00:41:18  rahuja
 * Fixed IPC data corruption bug (sakumar)
 *
 * Revision 1.1  1995/03/30  17:59:21  sakumar
 * Added Syn_ipc.c (currently just does and echo)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "packet.h"

#include "../../wbu/cat5k/alc_cominc/basic_types.h"
#include "../../wbu/cat5k/alc_cominc/shram.h"
#include "../../wbu/cat5k/alc_cominc/spram.h"
#include "../../wbu/cat5k/alc_cominc/atmdrv_if.h"


/*
 * Globals
 */
watched_queue *Syn_ipcInputQ;

int Ipc_rx (paktype *);
int is_ipc_pkt (paktype *);
int biga_idb_rx (paktype *);


/* 
 * The following function is called on the Interrupt stack by the ATM 
 * driver when it gets an IPC packet. 
 * Get a paktype buffer, bcopy the contents and queue it to Syn_ipcInputQ
 */

void
Syn_ipc_InPacket_enqueue( void *pData, UINT32 length)
{
	paktype *pak;

	if ( (pak = getbuffer(length)) ) {
        /* Copy data to pak buffer (in DRAM) from Frame Buffer memory */
        bcopy (pData, (pak->datagramstart), length);
        pak->datagramsize = length;
    } else {
        /* Couldn't get packet */
        return;
    }
	
	process_enqueue_pak(Syn_ipcInputQ, pak);
}


forktype Syn_ipcProcess(void)
{
    ulong major, minor;
	paktype *pak;	

	/* wait for system to initialize */
    process_wait_on_system_init();

	/* ATM driver now exists, register our upcall routine. */
	ADVR_RegInPktUpCall (Syn_ipc_InPacket_enqueue) ;

    process_watch_queue(Syn_ipcInputQ, ENABLE, RECURRING);
	while (1) {
        process_wait_for_event();
        while (process_get_wakeup(&major, &minor)) {
            switch(major) {
              case QUEUE_EVENT:
		        while ( (pak = process_dequeue(Syn_ipcInputQ) ) != NULL) {
		        	if (is_ipc_pkt(pak))
		        	    Ipc_rx(pak);
		        	else
		        	    biga_idb_rx(pak);
		        }
                break;
              default:
                buginf("\n\rSyn_ipcProcess: Syn_ipcProcess() unexpected event!");
                break;
            }
        }
	}
}


void Syn_ipc_init(void)
{
    Syn_ipcInputQ = create_watched_queue("C5K ATM IPC Input", 0, 0);
 
    (void) cfork(Syn_ipcProcess, 0L, 0, "C5K ATM IPC", 0);
}
