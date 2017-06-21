/* $Id: if_cip_vlan.h,v 3.3.12.2 1996/05/17 11:18:12 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cip_vlan.h,v $
 *------------------------------------------------------------------
 * if_cip_vlan.h  Channel Interface Processor Virtual Lan support
 *
 * April 1995, Karen Mitchell
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_cip_vlan.h,v $
 * Revision 3.3.12.2  1996/05/17  11:18:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.2  1996/04/03  14:29:45  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.2.1  1996/03/21  06:08:05  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.3.12.1  1996/03/18  19:41:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/02/20  00:51:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/13  01:33:59  motto
 * Spontaneous cbus complex restarts running CIP CSNA and
 * also out stuck messages.
 * CSCdi47536:  CIP failure in 7513 - CY-Bus problems
 *
 * Revision 3.2.26.1  1996/01/24  21:46:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:17:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:40:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/11  10:23:52  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.4  1995/08/04  14:10:14  motto
 * Changed CIP LAN interface into SRB to only have one entry per LAN
 * instead of per adapter.
 * CSCdi37535:  CIP LAN SRB explorers multiplying by # of adapters defined
 * per LAN
 *
 * Revision 2.3  1995/06/22  15:34:33  motto
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.2  1995/06/21  20:28:14  motto
 * Fix 'show extended channel x/2 <llc2|csna|connection-map|
 * max-llc2-sessions> ....' commands for SNA support on the CIP card.
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.1  1995/06/07  20:41:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CIP_VLAN_H__
#define __CIP_VLAN_H__

extern char *pVlanPrompt;
extern char *pVadapPrompt;

/*
 * Prototypes
 */
/* Interface from general cip support code */
extern void cip_vlan_create(hwidbtype *, cip_idb_infotype *);
extern void cip_vlan_send_vlan_cfg(idbtype *);
extern void cip_vlan_cfg_acknowledge(hwidbtype *, ushort, ulong, uchar *,
				     ulong);
extern void cip_vlan_reset_cfg(hwidbtype *, cip_idb_infotype *);

/* show commands */
extern void show_ext_channel_vlan_command(parseinfo *);
extern void show_ext_channel_llc2_command(parseinfo *);
extern void show_ext_channel_max_llc2_sess_command(parseinfo *);
extern void show_ext_channel_csna_conn_map_command(parseinfo *);

/* Sub command support */
extern void csna_vlan_subcommand(parseinfo *, transition *, void *);
extern void *vlan_set_vars(parseinfo *);
extern void  vlan_reset_vars(parseinfo *, void *);

extern void csna_vadap_subcommand(parseinfo *, transition *, void *);
extern void *vadap_set_vars(parseinfo *);
extern void  vadap_reset_vars(parseinfo *, void *);

/* Configuration commands - General */
extern void csna_vlan_max_llc2_sess_command(parseinfo *);

/* Configuration commands - Virtual Lans */
extern void csna_vlan_srb_command(parseinfo *);
extern void csna_vlan_bridge_command(parseinfo *);

/* Configuration commands - Virtual Adapters */
extern void csna_vadap_name_command(parseinfo *);
extern void csna_llc2_command(parseinfo *);


/* SNMP APIs */
typedef enum lan_types_ {
    ETHER,
    TOKEN,
    FDDI,
    MAXTYPE
} lan_types_t;

typedef enum bridge_types_ {
    CSNA_VLAN_BRIDGE_TYPE_SRB,
    CSNA_VLAN_BRIDGE_TYPE_TB,
    CSNA_VLAN_BRIDGE_TYPE_HUH
} bridge_types_t;

typedef enum vlan_name_ {
    VLAN_NAME_FULL,
    VLAN_NAME_ABBREV
} enum_vlan_name;
/*
 * Retrieve virtual LAN on a CIP hwidb. Searching for
 * an exact match or the next best match.
 * returns a vlan handle.
 */
extern ushort cip_get_csna_vlan_maxllc2_sessions( hwidbtype *hwidb);
extern void *cip_get_vlan( hwidbtype *, boolean, ulong, ulong );
extern void *cip_get_vlan_from_vadapt( hwidbtype *, void *);

/*
 * These functions provide an interface to
 * the virtual LAN data without the user
 * having knowledge of the implementation.
 */
extern lan_types_t get_vlan_lan_type( void * );
extern ulong get_vlan_lan_id( void * );
extern bridge_types_t get_vlan_bridge_type( void * );
extern ulong get_vlan_srb_local_ring( void * );
extern ulong get_vlan_srb_bridge_num( void * );
extern ulong get_vlan_srb_target_ring( void * );
extern ulong get_vlan_tb_bridge_group( void * );
extern uchar *get_vlan_lan_type_name( void *, enum_vlan_name);

/*
 * Retrieve virtual adapter on a CIP virtual LAN. Searching for
 * an exact match or the next best match.
 * returns a vadapter handle.
 */
extern void *cip_get_vadapt_from_adapno( hwidbtype *, void *, boolean, ulong);
extern void *cip_get_vadapt_from_vc( hwidbtype *, boolean, ulong);

/*
 * These functions provide an interface to
 * the virtual LAN adapter data without the 
 * user having knowledge of the implementation.
 */
extern ulong get_vadapt_vc( void * );
extern ulong get_vadapt_adapt_no( void * );
extern uchar *get_vadapt_mac_address( void * );
extern uchar *get_vadapt_adapt_name( void * );

extern ulong get_vadapt_llc2_T1_ack( void * );    
extern ulong get_vadapt_llc2_T1_pf( void * );
extern ulong get_vadapt_llc2_T1_rej( void * );
extern ulong get_vadapt_llc2_T1_busy( void * );
extern ulong get_vadapt_llc2_Ti( void * );
extern ulong get_vadapt_llc2_Tw( void * );
extern ulong get_vadapt_llc2_Rw( void * );
extern ulong get_vadapt_llc2_N2( void * );
extern ulong get_vadapt_llc2_N1( void * );
extern ulong get_vadapt_llc2_T2( void * );
extern ulong get_vadapt_llc2_N3( void * );
extern ulong get_vadapt_llc2_Nw( void * );

hwidbtype *cip_ilan_hwidb_from_vc(hwidbtype *hwidb, ushort vc);
void cip_vlan_reset_idb(hwidbtype *hwidb);

#endif /* __CIP_VLAN_H__ */
