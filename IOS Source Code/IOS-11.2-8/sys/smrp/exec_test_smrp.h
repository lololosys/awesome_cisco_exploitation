/* $Id: exec_test_smrp.h,v 3.4.20.1 1996/08/12 16:07:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/smrp/exec_test_smrp.h,v $
 *------------------------------------------------------------------
 * Test commands for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_smrp.h,v $
 * Revision 3.4.20.1  1996/08/12  16:07:17  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/02/01  23:34:31  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/22  07:15:08  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:53:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/06  02:06:21  kleung
 * CSCdi41656:  Enhance SMRP test mode for sending/receiving data.
 *
 * Revision 2.4  1995/08/09  03:42:00  kleung
 * CSCdi38461:  Need to support serial interface for SMRP.
 *
 * Revision 2.3  1995/08/01  00:22:37  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.2  1995/07/25  21:29:26  slin
 * CSCdi37501:  Timing problem can cause legit forward entry to be removed.
 *
 * Revision 2.1  1995/06/07  22:45:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
enum {
    SMRP_TEST_CMD = 1,
    SMRP_TEST_CREATE,                 
    SMRP_TEST_DATA,                    
    SMRP_TEST_DELETE,                  
    SMRP_TEST_END,                     
    SMRP_TEST_JOIN,                    
    SMRP_TEST_LEAVE,                   
    SMRP_TEST_LOCATE,                  
    SMRP_TEST_PACKET,                  
    SMRP_TEST_ROUTE,
    SMRP_TEST_STOP,                   
    SMRP_TEST_TUNNEL                 
};


#define ALL_NODES                         1
#define ALL_ENDPTS                        2
#define ALL_ENTITIES                      3

/* Help string for packet type */
static const uchar pkt_type_help[] = "
  2HighBits:
    MCDatagram           0x00
    MCRequestCommand     0x40
    MCPositiveResponse   0x80
    MCNegativeResponse   0xC0

  6LowBits
    MCData               0x00
    MCHello              0x01
    MCPrimaryNode        0x02
    MCSecondaryNode      0x03
    MCDistanceVector     0x04
    MCCreateGroup        0x05
    MCDeleteGroup        0x06
    MCJoinGroup          0x07
    MCLeaveGroup         0x08
    MCGroupCreator       0x09
    MCGroupMember        0x0A
    MCAddEntry           0x0B
    MCRemoveEntry        0x0C
    MCTunnelNode         0x0D
    MCLocateNode         0x0E
    MCNotifyNode         0x0F
";

/*******************************************************************
 * test smrp 
 * 
 *     create [<num_group>] <interface>
 *     data <group> [on|off| <interface> <data>]
 *     delete [<num_group> <netrange>] <group> <interface>
 *     exit
 *     join <group> <interface>
 *     leave <group> <interface>
 *     locate <interface>
 *     packet [version | type | seq | group] <interface> [more <data>] 
 *     route <netrange> <distance> [<num of routes>] to appletalk <addr>
 *     stop
 *     tunnel [netrange | req | dist | addr | name] 
 *
 *******************************************************************/

/**************************************************
 *
 * Bye.
 */

EOLS	(smrp_test_end_eol, smrp_test_commands, SMRP_TEST_END);
KEYWORD	(smrp_test_end, smrp_test_end_eol, smrp_test_end_eol,
	 "exit", "Exit SMRP test mode", PRIV_OPR);

/*****************************************************************
 * Specify the interface.
 */
EOLS (smrp_test_interface_eol, smrp_test_commands, SMRP_TEST_CMD);
INTERFACE (smrp_test_interface, smrp_test_interface_eol,
	no_alt, OBJ(idb,1), IFTYPE_ANYSUB);

/*****************************************************************
 * smrp test tunnel <interface> range <netrange> requester <addr>
 *     distance <num> address <addr> name <string>
 */
EOLS (smrp_test_tunnel_eol, smrp_test_commands, SMRP_TEST_CMD);

TEXT (smrp_test_tunnel_name_str, smrp_test_tunnel_eol, no_alt,
        OBJ(string, 1), "Name string");

KEYWORD (smrp_test_tunnel_name, smrp_test_tunnel_name_str, no_alt,
	"name", "NBP name",
	PRIV_OPR);

ATADDR	(smrp_test_tunnel_addr_value, smrp_test_tunnel_name, no_alt,
	OBJ(paddr,1), "AppleTalk address of tunnel router");

KEYWORD_ID (smrp_test_tunnel_proto, smrp_test_tunnel_addr_value, 
        no_alt,
        OBJ(int,6), LINK_APPLETALK,
	"appletalk", "AppleTalk address",
	PRIV_OPR);

KEYWORD (smrp_test_tunnel_addr, smrp_test_tunnel_proto, 
        no_alt,
	"address", "Tunnel router address",
	PRIV_OPR);

NUMBER(smrp_test_tunnel_dist_value, smrp_test_tunnel_addr,
        no_alt,
        OBJ(int,5), 0, 255, "Distance");

KEYWORD (smrp_test_tunnel_dist, smrp_test_tunnel_dist_value, 
        no_alt,
	"distance", "Tunnel distance",
	PRIV_OPR);

NUMBER(smrp_test_tunnel_req_value, smrp_test_tunnel_dist,
        no_alt,
        OBJ(int,4), 0, 255, "Requester");

KEYWORD (smrp_test_tunnel_req, smrp_test_tunnel_req_value, 
        no_alt,
	"requester", "Tunnel requester",
	PRIV_OPR);

CRANGE (smrp_test_tunnel_range_value, smrp_test_tunnel_req, 
        no_alt,
        OBJ(int,2), OBJ(int,3));

KEYWORD (smrp_test_tunnel_range, smrp_test_tunnel_range_value, 
        no_alt,
	"range", "AppleTalk cable range",
	PRIV_OPR);

INTERFACE (smrp_test_tunnel_int, smrp_test_tunnel_range,
	no_alt, OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD_ID (smrp_test_tunnel, smrp_test_tunnel_int, smrp_test_end,
        OBJ(int,1), SMRP_TEST_TUNNEL,
	"tunnel", "Tunnel to a multicast router",
	PRIV_OPR);

/*****************************************************************
 * smrp test stop
 */
EOLS (smrp_test_stop_eol, smrp_test_commands, SMRP_TEST_STOP);
KEYWORD (smrp_test_stop, smrp_test_stop_eol, smrp_test_tunnel,
	"stop", "Stop the multicast test router",
	PRIV_OPR);

/*****************************************************************
 * smrp test route <netrange> <distance> [<num of routes>] to appletalk <addr>
 *       <interface>
 */
ATADDR	(smrp_test_route_addr, smrp_test_interface, no_alt,
	OBJ(paddr,1), "AppleTalk address of neighbor");

KEYWORD_ID (smrp_test_route_atalk, smrp_test_route_addr,
        no_alt, OBJ(int,6), LINK_APPLETALK,
	"appletalk", "AppleTalk neighbors", PRIV_USER);

KEYWORD (smrp_test_route_to, smrp_test_route_atalk, no_alt,
        "to", "Sent to SMRP neighbor",
	PRIV_OPR);

NUMBER(smrp_test_route_num, smrp_test_route_to, smrp_test_route_to,
        OBJ(int,5), 1, 10000, "Number of routes to send");

NUMBER(smrp_test_route_dist, smrp_test_route_num, no_alt,
        OBJ(int,4), 0, 255, "Distance");

SMRPRANGE (smrp_test_route_range, smrp_test_route_dist, 
        no_alt,
        OBJ(int,2), OBJ(int,3));

KEYWORD_ID (smrp_test_route, smrp_test_route_range,
        smrp_test_stop,
        OBJ(int,1), SMRP_TEST_ROUTE,
	"route", "SMRP distance vector packets",
	PRIV_OPR);

/*****************************************************************
 * smrp test packet [version | type | seq | group] <interface> [more <data>]
 */
EOLS (smrp_test_pkt_eol, smrp_test_commands, SMRP_TEST_CMD);

HEXDATA (smrp_test_pkt_more_value, smrp_test_pkt_eol, no_alt,
        OBJ(string, 1), OBJ(int,14), 
        "More data in packet", 256);

KEYWORD_ID (smrp_test_pkt_more, smrp_test_pkt_more_value, smrp_test_pkt_eol,
        OBJ(int,13), TRUE,
	"more", "More data",
	PRIV_OPR);

INTERFACE (smrp_test_pkt_interface, smrp_test_pkt_more,
	no_alt, OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD_ID (smrp_test_pkt_specmc_entities, smrp_test_pkt_interface,
        smrp_test_pkt_interface,
        OBJ(int,12), ALL_ENTITIES,
	"all-entities", "All entities multicast address",
	PRIV_OPR);

KEYWORD_ID (smrp_test_pkt_specmc_endpts, smrp_test_pkt_interface,
        smrp_test_pkt_specmc_entities,
        OBJ(int,12), ALL_ENDPTS,
	"all-endpoints", "All endpoints multicast address",
	PRIV_OPR);

KEYWORD_ID (smrp_test_pkt_specmc_nodes, smrp_test_pkt_interface,
        smrp_test_pkt_specmc_endpts,
        OBJ(int,12), ALL_NODES,
	"all-nodes", "All nodes multicast address",
	PRIV_OPR);

KEYWORD_ID (smrp_test_pkt_specmc, smrp_test_pkt_specmc_nodes, 
        smrp_test_pkt_interface,
        OBJ(int,11), TRUE,
	"special-multicast", "Special multicast addresses",
	PRIV_OPR);

SMRPTESTGRPADDR (smrp_test_pkt_grp_addr, smrp_test_pkt_specmc, no_alt,
	OBJ(int,9), OBJ(int,10));

KEYWORD_ID (smrp_test_pkt_group, smrp_test_pkt_grp_addr, smrp_test_pkt_specmc,
        OBJ(int,8), TRUE,
	"group", "SMRP group",
	PRIV_OPR);

NUMBER(smrp_test_seq_no, smrp_test_pkt_group, no_alt,
        OBJ(int,7), 0, 255, "Sequence number");

KEYWORD_ID (smrp_test_pkt_seq, smrp_test_seq_no, smrp_test_pkt_group,
        OBJ(int,6), TRUE,
	"sequence", "Sequence number",
	PRIV_OPR);

HEXDIGIT(smrp_test_pkt_type_value, smrp_test_pkt_seq, no_alt,
        OBJ(int, 5), 0, 0xff, pkt_type_help);

KEYWORD_ID (smrp_test_pkt_type, smrp_test_pkt_type_value, smrp_test_pkt_seq,
        OBJ(int,4), TRUE,
	"type", "Packet type",
	PRIV_OPR);

NUMBER(smrp_test_pkt_version_no, smrp_test_pkt_type, no_alt,
        OBJ(int,3), 0, 255, "Protocol version number");

KEYWORD_ID (smrp_test_pkt_version, smrp_test_pkt_version_no, smrp_test_pkt_type,
        OBJ(int,2), TRUE,
	"version", "Protocol version",
	PRIV_OPR);

KEYWORD_ID (smrp_test_packet, smrp_test_pkt_version, smrp_test_route,
        OBJ(int,1), SMRP_TEST_PACKET,
	"packet", "Send a custom packet",
	PRIV_OPR);

/*****************************************************************
 * smrp test locate <interface>
 */
KEYWORD_ID (smrp_test_locate, smrp_test_interface, smrp_test_packet,
        OBJ(int,1), SMRP_TEST_LOCATE,
	"locate", "Locate a multicast router",
	PRIV_OPR);

/*****************************************************************
 * smrp test leave <group> <interface>
 */
SMRPTESTGRPADDR (smrp_test_leave_grp, smrp_test_interface, no_alt,
	OBJ(int,2), OBJ(int,3));
KEYWORD_ID (smrp_test_leave, smrp_test_leave_grp, smrp_test_locate,
        OBJ(int,1), SMRP_TEST_LEAVE,
	"leave", "Leave a multicast group",
	PRIV_OPR);

/*****************************************************************
 * smrp test join <group> <interface>
 */
SMRPTESTGRPADDR (smrp_test_join_grp, smrp_test_interface, no_alt,
	OBJ(int,2), OBJ(int,3));
KEYWORD_ID (smrp_test_join, smrp_test_join_grp, smrp_test_leave,
        OBJ(int,1), SMRP_TEST_JOIN,
	"join", "Join a multicast group",
	PRIV_OPR);

/*****************************************************************
 * smrp test delete [<num_group> <netrange>] <group> <interface>
 */
SMRPTESTGRPADDR (smrp_test_delete_grp, smrp_test_interface, no_alt,
	OBJ(int,6), OBJ(int,7));

SET	(smrp_test_delete_value_set1, smrp_test_delete_grp, OBJ(int,5), TRUE);
SMRPRANGE(smrp_test_delete_range, smrp_test_delete_value_set1, no_alt,
	  OBJ(int,3), OBJ(int,4));

NUMBER	(smrp_test_delete_value, smrp_test_delete_range, smrp_test_delete_grp,
         OBJ(int,2), 1, 1000, "Number of groups to delete");

KEYWORD_ID(smrp_test_delete, smrp_test_delete_value, smrp_test_join,
	   OBJ(int,1), SMRP_TEST_DELETE,
	   "delete", "Delete a multicast group", PRIV_OPR);

/*****************************************************************
 * smrp test data [receive on|off] | send <group> [num] [size] 
 *      <interface> <data>]
 */
EOLS    (smrp_test_data_eol, smrp_test_commands, SMRP_TEST_CMD);
TEXT    (smrp_test_data_text, smrp_test_data_eol, no_alt,
        OBJ(string,1), "Data to be sent to group");
INTERFACE (smrp_test_data_int, smrp_test_data_text,
	no_alt, OBJ(idb,1), IFTYPE_ANYSUB);
NUMBER(smrp_test_data_pausecntnum, smrp_test_data_int,
        no_alt,
        OBJ(int,12), 1, 10000, "Number of milliseconds to pause");
KEYWORD_ID (smrp_test_data_pausecnt, smrp_test_data_pausecntnum, 
	smrp_test_data_int,
        OBJ(int,13), TRUE,
	"pause-count", "Pause after # of packets sent",
	PRIV_OPR);
NUMBER(smrp_test_data_pausetimenum, smrp_test_data_pausecnt,
        no_alt,
        OBJ(int,10), 1, 10000, "Number of milliseconds to pause");
KEYWORD_ID (smrp_test_data_pausetime, smrp_test_data_pausetimenum, 
	smrp_test_data_int,
        OBJ(int,11), TRUE,
	"pause-time", "Pause # of milliseconds between sends",
	PRIV_OPR);
NUMBER(smrp_test_data_sizenum, smrp_test_data_pausetime,
        no_alt,
        OBJ(int,8), 1, 576, "Number of bytes of data");
KEYWORD_ID (smrp_test_data_size, smrp_test_data_sizenum, smrp_test_data_pausetime,
        OBJ(int,9), TRUE,
	"size", "Data size (not custom data)",
	PRIV_OPR);
NUMBER(smrp_test_data_num, smrp_test_data_size,
        no_alt,
        OBJ(int,6), 1, 1000000, "Number of times to send data");
KEYWORD_ID (smrp_test_data_count, smrp_test_data_num, smrp_test_data_size,
        OBJ(int,7), TRUE,
	"count", "Number of data packets to send",
	PRIV_OPR);
SMRPTESTGRPADDR (smrp_test_data_grp, smrp_test_data_count, no_alt,
	OBJ(int,2), OBJ(int,3));
KEYWORD (smrp_test_data_send, smrp_test_data_grp, no_alt,
	"send", "Send data to a multicast group",
	PRIV_OPR);
KEYWORD_ID (smrp_test_data_off, smrp_test_data_eol, no_alt,
        OBJ(int,5), FALSE,
	"off", "Stop incoming data",
	PRIV_OPR);
KEYWORD_ID (smrp_test_data_on, smrp_test_data_eol, smrp_test_data_off,
        OBJ(int,5), TRUE,
	"on", "Allow incoming data",
	PRIV_OPR);
KEYWORD_ID (smrp_test_data_rcv, smrp_test_data_on, smrp_test_data_send,
        OBJ(int,4), TRUE,
	"receive", "Receive data mode",
	PRIV_OPR);
KEYWORD_ID (smrp_test_data, smrp_test_data_rcv, smrp_test_delete,
        OBJ(int,1), SMRP_TEST_DATA,
	"data", "Send or receive data",
	PRIV_OPR);

/*****************************************************************
 * smrp test create [<num_group>] <interface>
 */
SET	(smrp_test_create_value_set1, smrp_test_interface, OBJ(int,3), TRUE);
NUMBER	(smrp_test_create_value, smrp_test_create_value_set1,
	 smrp_test_interface,
         OBJ(int,2), 1, 1000, "Number of groups to create");

KEYWORD_ID(smrp_test_create, smrp_test_create_value, smrp_test_data,
	   OBJ(int,1), SMRP_TEST_CREATE,
	   "create", "Create a multicast group", PRIV_OPR);

/**************************************************
 *
 * Top level for smrp test mode
 */

NOP	(top_test_smrp, smrp_test_create, NONE);

/******************************************************************
 * test smrp
 */
EOLNS    (priv_test_smrp_eol, smrp_test);
KEYWORD (priv_test_smrp, priv_test_smrp_eol, ALTERNATE,
	 "smrp", NULL /*"Simple Multicast Routing Protocol Test commands"*/,
	 PRIV_OPR|PRIV_HIDDEN/*|PRIV_NOHELP*/);

#undef ALTERNATE
#define ALTERNATE priv_test_smrp
