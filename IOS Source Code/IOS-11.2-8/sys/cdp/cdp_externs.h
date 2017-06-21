/* $Id: cdp_externs.h,v 3.2.60.4 1996/08/15 00:44:42 tylin Exp $
 * $Source: /release/112/cvs/Xsys/cdp/cdp_externs.h,v $
 *------------------------------------------------------------------
 * Support for CDP (Cisco Discovery Protocol)
 *
 * September 1994, Arun Sastry
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cdp_externs.h,v $
 * Revision 3.2.60.4  1996/08/15  00:44:42  tylin
 * CSCdi66084:  cdp should send updates on lowest numbered vlan
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/07/18  18:14:38  tylin
 * CSCdi62693:  Command cdp neighbors detail generates alignment
 * corrections (IPX)
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/06/28  07:20:47  tylin
 * CSCdi60805:  7513 restarted by error - a SegV exception, PC 0x600EBE98
 *    Avoid sign extension problem for cdp_hash().
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/15  21:13:07  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:49:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/06/29  01:21:49  asastry
 * CSCdi36442:  <cdp enable> on interface should not override <no cdp run>
 *
 * Revision 2.3  1995/06/27  07:21:15  smackie
 * Convert CDP to use platform_get_string(). Tidy up subsystem entry
 * points to use the right prototype. (CSCdi36461)
 *
 * Revision 2.2  1995/06/08  04:18:50  asastry
 * CSCdi35395:  CDP should have registry callback for interface
 * initialization
 *
 * Revision 2.1  1995/06/07  20:15:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "cdp.h"

#ifndef __CDP_EXTERNS_H__
#define __CDP_EXTERNS_H__

/*
 * cdp.c
 */
extern void cdp_init_idb(idbtype *, boolean);
extern boolean cdp_ever_start;
extern boolean cdp_explicitly_disarmed;
extern boolean cdp_running;
extern ushort cdp_xmit_frequency;
extern ushort cdp_holdtime;
extern ulong cdp_traffic[];
extern uchar cdp_multicast[];
extern uchar cdp_tr_func_addr[];
extern ulong cdp_global_device_number;
extern cdp_add_func_t add_tlv_func[];
extern cdp_decode_func_t cdp_decode_func[];

extern void cdp_init(subsystype *);
extern void cdp_start_process(void);
extern void cdp_enqueue(paktype *);
extern process cdp_prot(void);
extern void cdp_clean_up(void);
extern ushort cdp_checksum(ushort *, int);
extern void cdp_handle_packet(paktype *);
extern void cdp_send_periodic_updates_on_swidb(idbtype *);
extern void cdp_send_periodic_updates(void);
extern void cdp_send_update(idbtype *, uchar);
extern boolean cdp_insert_ip_info(uchar **, idbtype *, int *, int *,
				  ushort *);
extern boolean cdp_insert_clns_info(uchar **, idbtype *, int *, int *,
				    ushort *);
extern boolean cdp_insert_novell_info(uchar **, idbtype *, int *, int *,
				      ushort *);
extern boolean cdp_insert_decnet_info(uchar **, idbtype *, int *, int *, ushort *);
extern boolean cdp_insert_apple_info(uchar **, idbtype *, int *, int *, ushort *);
extern boolean cdp_insert_device_name(cdp_info_type *, idbtype *, ushort *);
extern boolean cdp_insert_address(cdp_info_type *, idbtype *, ushort *);
extern boolean cdp_insert_port_id(cdp_info_type *, idbtype *, ushort *);
extern boolean cdp_insert_capabilities(cdp_info_type *, idbtype *, ushort *);
extern boolean cdp_insert_version(cdp_info_type *, idbtype *, ushort *);
extern void cdp_init_platform_strings(void);
extern boolean cdp_insert_hardware(cdp_info_type *, idbtype *, ushort *);
extern void cdp_if_down_notify(idbtype *);
extern void cdp_reload_notify(void);
extern void cdp_if_up_notify(idbtype *);
extern void cdp_add_registry_calls(void);
extern boolean cdp_supported_int(idbtype *);

/*
 * cdp_chain.c
 */
void cdp_parser_init(void);

/*
 * cdp_media.c
 */
extern void cdp_media_registry(void);
extern void cdp_macaddrs_update(idbtype *);
/*
 * cdp_parse.c
 */
extern void display_cdp_int_info(idbtype *);
extern void cdp_intf_enable(idbtype *, boolean);
extern void cdp_intf_disable(idbtype *);
extern void show_cdpglobal(void);
extern void show_cdp_counters(void);
extern void show_cdp_host(char *, boolean, boolean);
extern void show_cdp_neighbors(idbtype *, boolean);
extern void show_one_cdp_neighbor(cdp_cache_type *, boolean);
extern void cdp_print_capabilities_info(ulong, boolean);
extern char *cdp_ret_int_abbr(char *);
extern void cdp_print_addresses_info (cdp_cache_type *);
extern void cdp_print_ip(addrtype *);
extern void cdp_print_clns(addrtype *);
extern void cdp_print_novell(addrtype *);
extern void cdp_print_decnet(addrtype *);
extern void cdp_print_apple(addrtype *);
extern void cdp_clear_table(void);
/*
 * cdp2.c
 */
extern cdp_hash_type cdp_hash(char *, int);
extern cdp_cache_type *cdp_walk_hash(int *, cdp_cache_type *);
extern void cdp_hash_insert(cdp_cache_type *);
extern cdp_cache_type *cdp_hash_lookup(char *, uchar, idbtype *, ulong *);
extern void cdp_insert_packet_info(cdp_hdrtype *, uint, idbtype *,
				   cdp_cache_type *, boolean);
extern void cdp_handle_device_info(cdp_info_type *, cdp_cache_type *, boolean);
extern void cdp_handle_address_info(cdp_info_type *, cdp_cache_type *,
				    boolean);
extern void cdp_handle_port_id_info(cdp_info_type *, cdp_cache_type *,
				    boolean);
extern void cdp_handle_capabilities_info(cdp_info_type *, cdp_cache_type *,
					 boolean);
extern void cdp_handle_version_info(cdp_info_type *, cdp_cache_type *,
				    boolean);
extern void cdp_handle_platform_info(cdp_info_type *, cdp_cache_type *,
				     boolean);
extern void cdp_handle_ip_address(int *, uchar **, uchar **, int *);
extern void cdp_handle_clns_address(int *, uchar **, uchar **, int *);
extern void cdp_handle_novell_address(int *, uchar **, uchar **, int *);
extern void cdp_handle_apple_address(int *, uchar **, uchar **, int *);
extern void cdp_handle_decnet_address(int *, uchar **, uchar **, int *);
extern void cdp_cache_ager(void);
extern void cdp_delete_cache_entry(cdp_cache_type *);

#endif __CDP_EXTERNS_H__
