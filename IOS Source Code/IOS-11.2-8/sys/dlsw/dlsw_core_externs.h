/* $Id: dlsw_core_externs.h,v 3.5.6.4 1996/08/10 01:03:52 athippes Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_core_externs.h,v $
 *------------------------------------------------------------------
 * dlsw core/fsm specific extern definitions
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Includes all the function prototypes for 
 * dlsw core and fsm
 * Defines the debug variables also so the debug specific files 
 * could include them
 *------------------------------------------------------------------
 * $Log: dlsw_core_externs.h,v $
 * Revision 3.5.6.4  1996/08/10  01:03:52  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.5.6.3  1996/07/25  09:43:13  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.5.6.2  1996/07/23  22:09:32  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 *              Using Registry stub.
 * Branch: California_branch
 *
 * Revision 3.5.6.1  1996/05/17  10:44:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.32.3  1996/05/10  15:00:44  ioakley
 * CSCdi57325:  Remove dlsw reliance on port_number/slot_number
 *              and identify all dlsw ports by associated swidb.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.32.2  1996/04/24  06:30:15  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.4.32.1  1996/03/17  17:37:44  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5  1996/03/06  23:00:06  ravip
 * CSCdi49441:  Number of active circuits wrong on SNMP get of certain
 * objects
 *
 * Revision 3.4  1995/12/15  17:18:14  fbordona
 * CSCdi45661:  DLSw+ peers show incorrect circuit count.
 *              Added checking for enable and disable of ports.
 *              Consolidate fsm to disp *stn structures.
 *              Consolidate fsm_to_disp() and cstm_to_disp().
 *              Improve local switching debug.
 *
 * Revision 3.3  1995/12/01  23:39:14  ravip
 * CSCdi41808:  Dlsw Mib does not respond to get-next correctly
 *
 * Revision 3.2  1995/11/17  09:02:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:22:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:54:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DLSW_CORE_EXTERNS_H
#define __DLSW_CORE_EXTERNS_H


/*
 * Constants
 */
#define NDLSW_LINES            10 /* for automore */
#define NDLSW_HASH_SIZE 256      /* number of entries in the hash table */
#define NDLSW_HASH_MASK (NDLSW_HASH_SIZE - 1)
#define ADAPTIVE_RECV_WINDOW   2    /* defined in the MIB */

#define DLSW_RING_ACTIVE   99		/* For the DLSW 'SAP' */

/*
 * Exported Variables
 */
extern queuetype  dlsw_ctlQ;
extern queuetype dlsw_bgroupQ;
extern queuetype  dlsw_delay_eventQ;
 
typedef void (* restart_fsm_t)(void *, int, int, int);
STRUCTURE_REF(dlsw_delay_t);
  
struct dlsw_delay_t_ {
    dlsw_delay_t	*next;
    restart_fsm_t	restart_fsm;	/* wrapper */
    void		*context;
    int			old_state;
    int			major;
    int			minor;
};


/*
 *  P r o t o t y p e   D e c l a r a t i o n s 
 */


extern boolean dlsw_delayed_event(restart_fsm_t fsm, void *context,
				  int state, int major, int minor);
extern int llc_mtu(hwidbtype *);
extern char *dlsw_sspop2str(uint);

/* extern variables for counting dlsw circuits */
extern int    dlsw_circuit_creates;
extern int    dlsw_active_circuits;


/* 
 * Init
 */
extern void dlsw_init (subsystype *subsys); 
extern void dlsw_work(paktype *newpak, hwidbtype *idb, int riflen);
extern void csm_destroy_peer(peerhandle_t peer_handle);
extern void csm_delete_cache(peerhandle_t peer_handle);
extern TCLSIMsg *dlsw_default_get_ring_req (idbtype *swidb);
extern dlsw_ring_t *dlsw_get_ring(CLSPortID *port_id, ushort span_index);
extern dlsw_ring_t *dlsw_get_ring_frpeer(idbtype *swidb);
extern dlsw_bgroup_t *dlsw_get_bgroup(ushort span_index);
extern void dlsw_set_bgroup_params (hwidbtype *idb, 
                                            boolean sense, ushort span_index);
extern void dlsw_assign_bgroup_llc2_params (idbtype *, dlsw_bgroup_t *);
extern dlsw_sap_t *dlsw_get_sap(dlsw_ring_t *dlsw_ring_tmp,uchar sap);
extern dlsw_ring_t *usapid2dgra(uint u_sap_id);
extern void dlsw_ports_disable(void);

/*
 * debug
 */
extern boolean dlsw_is_init_clsi_op(ushort primitive);
extern boolean dlsw_is_cstm_clsi_msg(TCLSIMsg *clsi_msg);
extern int dlsw_get_circuit_creates (void);
extern int dlsw_get_active_circuits (void);

#endif __DLSW_CORE_EXTERNS_H
