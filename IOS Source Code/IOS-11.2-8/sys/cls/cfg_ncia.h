/* $Id: cfg_ncia.h,v 3.1.2.3 1996/08/12 16:00:47 widmer Exp $
 * $Source: /release/112/cvs/Xsys/cls/cfg_ncia.h,v $
 *------------------------------------------------------------------
 * cfg_ncia.h - Definition for NCIA Commands Parser
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ncia.h,v $
 * Revision 3.1.2.3  1996/08/12  16:00:47  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.2  1996/06/12  06:51:32  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:41:42  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.3  1996/05/14  18:41:01  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/05/03  22:05:53  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:06  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:08:53  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*************************************************************************
 * ncia client <server_num> <ipaddress> <vmac> [sna|all]
 * no ncia client <server_num> <ipaddress> <vmac> [sna|all]
 */
EOLS	(cfg_ncia_client_eol, ncia_command, CONFIG_NCIA_CLIENT);

SET     (set_ncia_support_all, cfg_ncia_client_eol, OBJ(int, 2),
        NCIA_SNA_NETBIOS);

KEYWORD	(cfg_ncia_support_all, set_ncia_support_all, cfg_ncia_client_eol,
        "all", "Support all types of traffic", PRIV_CONF);

KEYWORD	(cfg_ncia_support_sna, cfg_ncia_client_eol, cfg_ncia_support_all,
        "sna-only", "Support SNA only", PRIV_CONF);

SET     (set_ncia_support_sna, cfg_ncia_support_sna, OBJ(int, 2),
        NCIA_SNA_ONLY);

ENADDR	(cfg_ncia_client_mac, set_ncia_support_sna, no_alt,
        OBJ(hwaddr, 1), "NCIA Client Virtual Macaddr");

IPADDR  (cfg_ncia_client_ip, cfg_ncia_client_mac, no_alt,
        OBJ(paddr,1), "NCIA Client IP Address");

NUMBER  (cfg_ncia_client_server_num, cfg_ncia_client_ip, no_alt,
        OBJ(int, 1), 1, 1, "NCIA Server Number");

KEYWORD (cfg_ncia_client, cfg_ncia_client_server_num, no_alt,
	 "client",
	 "NCIA Client Configuration Command",
	 PRIV_CONF);

/*************************************************************************
 * ncia server <server_num> <ipaddress> <server_mac> <vmac> [range] 
 *             [inbound-only] [keepalive <second>] [tcp_keepalive <minute>]
 * no ncia server 
 */
EOLS	(cfg_ncia_server_eol, ncia_command, CONFIG_NCIA_SERVER);

NUMBER  (cfg_ncia_tcp_keepalive_value, cfg_ncia_server_eol, cfg_ncia_server_eol,
        OBJ(int, 5), 0, 20, "NCIA tcp keepalive (in minute)");

KEYWORD (cfg_ncia_tcp_keepalive, cfg_ncia_tcp_keepalive_value, cfg_ncia_server_eol,
         "tcp_keepalive", "NCIA tcp keepalive value in minute", PRIV_CONF);

NUMBER  (cfg_ncia_keepalive_value, cfg_ncia_tcp_keepalive, no_alt,
        OBJ(int, 4), 0, 1200, "NCIA keepalive (in second)");

KEYWORD (cfg_ncia_keepalive, cfg_ncia_keepalive_value,cfg_ncia_tcp_keepalive, 
         "keepalive", "NCIA keepalive value in second", PRIV_CONF);

KEYWORD_ID (cfg_ncia_inbound, cfg_ncia_keepalive, cfg_ncia_keepalive,
        OBJ(int, 3), TRUE,
        "inbound-only", "Support in-bound connection only", PRIV_CONF);

NUMBER  (cfg_ncia_client_mac_range, cfg_ncia_inbound, no_alt,
        OBJ(int, 2), 1, 4095, "NCIA Virtual Macaddr Range");

ENADDR	(cfg_ncia_client_start_mac, cfg_ncia_client_mac_range, no_alt,
        OBJ(hwaddr, 2), "NCIA Virtual Macaddr for Client");

ENADDR	(cfg_ncia_server_mac, cfg_ncia_client_start_mac, no_alt,
        OBJ(hwaddr, 1), "NCIA Server Virtual Macaddr");

IPADDR  (cfg_ncia_server_ip, cfg_ncia_server_mac, no_alt,
        OBJ(paddr,1), "NCIA Server IP Address");

NUMBER  (cfg_ncia_server_num, cfg_ncia_server_ip, no_alt,
        OBJ(int, 1), 1, 1, "NCIA Server Number");

SET     (set_ncia_tcp_keepalive_value, cfg_ncia_server_num, OBJ(int, 5),
        NCIA_DEFAULT_TCP_KEEPALIVE);

SET     (set_ncia_keepalive_value, set_ncia_tcp_keepalive_value, OBJ(int, 4),
        NCIA_DEFAULT_NDLC_KEEPALIVE);

KEYWORD (cfg_ncia_server, set_ncia_keepalive_value, cfg_ncia_client,
	 "server",
	 "NCIA Server Configuration Command",
	 PRIV_CONF);

/*************************************************************************
 * ncia rsrb <rsrb_virtual_ring> <rsrb_bridge> <local_ring> <ncia_bridge>
 *           <ncia_ring> <virtual_macaddr>
 * no ncia rsrb 
 */
EOLS	(cfg_ncia_rsrb_eol, ncia_command, NCIA_RSRB);

ENADDR	(cfg_ncia_rsrb_virtual_macaddr, cfg_ncia_rsrb_eol, no_alt,
        OBJ(hwaddr, 1), "NCIA RSRB Virtual Macaddr");

NUMBER  (cfg_ncia_ring, cfg_ncia_rsrb_virtual_macaddr, no_alt,
        OBJ(int, 5), 1, 4095, "NCIA Ring Number");

NUMBER  (cfg_ncia_bridge, cfg_ncia_ring, no_alt,
        OBJ(int, 4), 1, 15, "NCIA Bridge Number");

NUMBER  (cfg_ncia_local_ring, cfg_ncia_bridge, no_alt,
        OBJ(int, 3), 1, 4095, "Local Ring Number");

NUMBER  (cfg_ncia_rsrb_bridge, cfg_ncia_local_ring, no_alt,
        OBJ(int, 2), 1, 15, "RSRB Bridge Number");

NUMBER  (cfg_ncia_rsrb_virtual_ring, cfg_ncia_rsrb_bridge, no_alt,
        OBJ(int, 1), 1, 4095, "RSRB Virtual Ring Number");

KEYWORD (cfg_ncia_rsrb, cfg_ncia_rsrb_virtual_ring, cfg_ncia_server,
	 "rsrb",
	 "Remote Source Route Bridge command",
	 PRIV_CONF /* |PRIV_HIDDEN */);

NVGENNS	(cfg_ncia_nvgen, cfg_ncia_rsrb, ncia_command);

KEYWORD	(cfg_ncia, cfg_ncia_nvgen, ALTERNATE, "ncia",
	 "Native Client Interface Architecture", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE cfg_ncia

