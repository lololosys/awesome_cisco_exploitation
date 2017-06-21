/* $Id: decnet_externs.h,v 3.5.12.8 1996/08/03 23:21:47 fox Exp $
 * $Source: /release/112/cvs/Xsys/decnet/decnet_externs.h,v $
 *------------------------------------------------------------------
 * decnet_externs.h - prototype support for decnet
 *	
 * October 1991, William Miskovetz
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: decnet_externs.h,v $
 * Revision 3.5.12.8  1996/08/03  23:21:47  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.5.12.7  1996/07/30  14:41:15  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.5.12.6  1996/07/22  23:07:01  asastry
 * CSCdi62165:  DECnet fast-switching should be able to use DECnet default
 * route
 * Branch: California_branch
 *
 * Revision 3.5.12.5  1996/07/16  09:36:14  hampton
 * Centralize knowledge of Decnet/Token Ring interaction.  [CSCdi63066]
 * Branch: California_branch
 *
 * Revision 3.5.12.4  1996/05/30  23:42:52  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.5.12.3  1996/05/29  19:08:03  asastry
 * CSCdi46634:  Misc DECnet code cleanup - get rid of unwanted registry
 * call.
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/05/22  20:54:32  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.5.12.1  1996/03/18  19:11:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/14  00:53:17  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.26.1  1996/02/20  00:43:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  19:44:16  hampton
 * Reorder routines to remove platform specific extern statements in
 * fastswitching code.  [CSCdi48844]
 *
 * Revision 3.4  1996/01/22  23:20:14  asastry
 * CSCdi46327:  'show decnet static' shows bad SNPA for non-ethernet
 * static routes
 *
 * Revision 3.3  1995/11/17  08:58:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:15:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:22:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../decnet/decnet.h"

#ifndef __DECNET_EXTERNS_H__
#define __DECNET_EXTERNS_H__

extern dnnet *dn_nets[];	/* pointers to each network structure */
extern boolean dn_running;

/* 
 * Static routes stored here.
 */
extern queuetype decnet_staticQ;

/* trigger an update */
extern sys_timestamp dn_special_L1_routing;
extern sys_timestamp dn_special_L2_routing;
extern sys_timestamp dn_special_hellos;

/* 
 * This pointer will point to a DECnet default route, if one is configured.
 */
extern dnroutetype *decnet_default_route;

/*
 * decnet.c
 */

extern boolean decnet_accesscheck(dnnspconpkt *, int, int, int, int, int,
				  idbtype *);
extern boolean decnet_data_accesscheck(paktype *, int);
extern char *cnttostr(uchar *, int);
extern boolean decnet_fastcheck(int, int, int);
extern boolean decnet_on_idb(idbtype *);
extern int dn_add_adjacency(uchar *, ushort, int, int, dnnet *, int, int,
				int, int, int, paktype *, dnhellotype *,
				int, idbtype *);
extern boolean dn_return(unsigned int, paktype *, dndatapacket *);
extern void dn_change_mtu_size(hwidbtype *, idbtype *, int);
extern boolean dn_routeBLOCK(void);
extern boolean dn_show_route(dnnet *, dnroutetype **, int);
extern boolean show_dnroutes(dnnet *, int, int);
extern dnmetrictype *dn_make_mvec(int);
extern dnnet *dn_initnet(int);
extern void display_dn(idbtype *);
extern void dn_addmaproute(int, int, dnnet *, dnmap *);
extern void dn_clear_rt(dnroutetype *);
extern void dn_control(paktype *, unsigned char *, int, int);
extern void dn_disable_if(idbtype *);
extern void dn_dispose(int, paktype *);
extern void dn_enable_if(idbtype *);
extern void dn_flushroutes(dnnet *);
extern void dn_forward(paktype *, dndatapacket *, int);
extern void dn_packet_forward(paktype *, dndatapacket *, int);
extern void dn_free_rt(dnroutetype **);
extern void dn_hello_input(paktype *, int, dnhellotype *);
extern void dn_init_idb(idbtype *, boolean);
extern void dn_init(subsystype *);
extern void dn_maproutes(dnnet *);
extern void dn_purgemap(void);
extern void dn_recomp(dnroutetype **, dnnet *);
extern void dn_remove_via(dnnet *, dnroutetype *, dnroutetype **, dnroutetype **);
extern void dn_route(void);
extern void dn_rt_input(paktype *, uchar *, int, boolean);
extern void dn_sendhello_if(int, idbtype *, int);
extern void dn_sendhellos(void);
extern void dn_sendroutes(void);
extern void dn_sendroutes_if(idbtype *, boolean, boolean, int);
extern void dn_setaddr(int, int, dnnet *, boolean);
extern void dn_traffic(void);
extern void dn_unsplit(dnnet *, dnroutetype **, dnroutetype *);
extern void dn_unsplit_all(dnnet *);
extern void dn_var_init(dnnet *);
extern void dnobjfree(dnaccitemtype *);
extern void nv_decnet(parseinfo *);
extern void nv_decnetif(char *, idbtype *);
extern void nv_dnaccess(void);
extern void show_dnaccess(int);
extern void show_dnaccess_shell(int);
extern void show_dnglobal(void);
extern void show_dnmap(void);
extern void dn_showaccess(boolean, int);
extern void if_printf(boolean, char*, ...);
extern void dn_debug_cpacket(boolean match, int listnum, int n, int srcarea, 
			     int srcnode, int destarea, int destnode,
			     idbtype *idb, ulong srctype, uchar *srcname,
			     int srclen, ulong sgrp, ulong susr, ulong dsttype,
			     uchar *dstname, int dstlen, ulong dgrp,
			     ulong dusr, uchar *id, int idlen, boolean grant);
extern idbtype *dn_echo_interface;
extern boolean dn_idb_is_bilingual(dnnet *net, idbtype *idb);
extern void dn_show_neighbors(void);
extern boolean dn_source_dest_match(dnaccitemtype *, int, int);
extern void decnet_route(void);
extern void register_dec_route_routines (void);
extern void dn_fastsetup(hwidbtype *);
extern int dn_printf(const char **, char **,tt_soc *,va_list *,boolean,int,int,int,int);

/*
 * dnfast.c
 */

extern boolean dn_accesscheck(dndatapacket *, dnaccitemtype *);
extern dnnet *dn_address_translation(dndatapacket *, dnnet *);
extern dnroutetype **dn_get_route(dndatapacket *, dnnet *);
extern dnroutetype *dn_multipath(dndatapacket *, dnnet *, dnroutetype *);

/*
 * decnet_chain.c
 */
extern void decnet_parser_init(void);

/*
 * decnet_echo.c
 */
extern boolean parse_decnetaddr(char *, int *, int *);
/*
 * decnet_media.c
 */
extern void decnet_media_registry(void);
extern uchar dn_e_baseaddr[];
extern uchar dn_t_baseaddr[];
extern uchar dn_t_gwaddr[];
extern uchar dn_multi_e_node[];

/*
 * decnet_setup.c
 */
extern void decnet_setup_global(setup_struct *);
extern void decnet_setup_int(setup_struct *);
extern void decnet_setup_end(setup_struct *);

/*
 * decnet_route.c
 */
extern void decnet_host_command(parseinfo *);
extern void decnet_name_show_hosts (nametype *, char *);
extern void decnet_static_route_command(parseinfo *);
extern char *decnet_address_lookup(int, int);
extern void nv_decnet_host_cmd(parseinfo *);
extern void nv_decnet_route_cmd(parseinfo *);
extern void decnet_staticQ_update(ushort, ushort, idbtype *,
			           hwaddrtype *, ushort, ushort,
				   boolean, boolean);

extern decnet_staticQtype *decnet_search_staticQ(ushort, boolean);
extern void decnet_show_static(addrtype *);
extern void decnet_reinstate_static_routes(dnnet *);
extern void decnet_add_static_route(dnnet *, ushort, ushort,
				    ushort, ushort, idbtype *,
				    hwaddrtype *, boolean);
extern dnroutetype *decnet_handle_static_route(dnnet *, dnroutetype*, int);
extern void decnet_remove_static_route(dnnet *, ushort);
extern void decnet_add_default_route(ushort,ushort,ushort,idbtype *,
				     hwaddrtype *, boolean);
extern void decnet_remove_default_route(void);

#endif __DECNET_EXTERNS_H__
