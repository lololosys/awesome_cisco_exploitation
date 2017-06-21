/* $Id: exec_test_vines.h,v 3.3.30.1 1996/08/12 16:09:09 widmer Exp $
 * $Source: /release/112/cvs/Xsys/vines/exec_test_vines.h,v $
 *------------------------------------------------------------------
 * E X E C _ T E S T _ V I N E S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_test_vines.h,v $
 * Revision 3.3.30.1  1996/08/12  16:09:09  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/08  03:07:29  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.2  1996/02/01  23:34:54  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.1  1996/02/01  05:14:20  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/22  06:57:56  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:48:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:27:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define VINES_TEST_BUILD	1
#define VINES_TEST_CHECKSUM	2
#define VINES_TEST_DATA		3
#define VINES_TEST_END		4
#define VINES_TEST_FLUSH	5
#define VINES_TEST_GENERATE	6
#define VINES_TEST_SEND		7
#define VINES_TEST_SET		8

#define VINES_GEN_FULL		1
#define VINES_GEN_CHANGE	2
#define VINES_GEN_NULL		3

#define VINES_SEND_AGAIN	 1
#define VINES_SEND_ARP		 2
#define VINES_SEND_BLAST	 3
#define VINES_SEND_CUSTOM	 4
#define VINES_SEND_ECHO		 5
#define VINES_SEND_END		 6
#define VINES_SEND_ICP		 8
#define VINES_SEND_IPC		 9
#define VINES_SEND_LOOP		10
#define VINES_SEND_MENU		11
#define VINES_SEND_NETRPC	12
#define VINES_SEND_RTP		13
#define VINES_SEND_SERVICE	14
#define VINES_SEND_SRTP		15
#define VINES_SEND_SPP		16

#define VINES_SET_LOCAL		 1
#define VINES_SET_ORIGIN	 2
#define VINES_SET_PERIODIC_TIMER 3
#define VINES_SET_RC_TIMER	 4
#define VINES_SET_REASS_TIMER	 5
#define VINES_SET_RETRY_TIMER	 6
#define VINES_SET_STATE		 7

#define VINES_SS_BUMP		1
#define VINES_SS_ENDOFTIME	2
#define VINES_SS_PERIODIC	3
#define VINES_SS_RESET	        4

#define VINES_ST_SUMMARY       1
#define VINES_ST_DETAIL        2
#define VINES_ST_NEED          3
#define VINES_ST_WELCOME       4
#define VINES_ST_COLDDUMP      5
#define VINES_ST_ADD           6
#define VINES_ST_DELETE        7

/**************************************************
 **************************************************
 *
 * Send a packet
 */

EOLS	   (vt_send_eol, vines_test_command, VINES_TEST_SEND);

SET	   (vt_send_menu, vt_send_eol, OBJ(int,1), VINES_SEND_MENU);

/**************************************************
 *
 * Send an service message (Application Layer)
 */

NUMBER     (vts_svc_proc, vt_send_eol, no_alt,
	    OBJ(int,4), 0, 65535, "Service procedure");
NUMBER     (vts_svc_sport, vts_svc_proc, no_alt,
	    OBJ(int,3), 0, 65535, "Source port");
NUMBER     (vts_svc_dport, vts_svc_sport, no_alt,
	    OBJ(int,2), 0, 65535, "Service WKP");
VINADDR	   (vts_svc_daddr, vts_svc_dport, no_alt,
	    OBJ(paddr,1), "Destination address");
KEYWORD_ID (vt_send_svc, vts_svc_daddr, vt_send_menu,
	    OBJ(int,1), VINES_SEND_SERVICE,
	    "svc", "Send a VINES service message", PRIV_OPR);

/**************************************************
 *
 * Send an service message (Application Layer)
 */

NUMBER     (vts_rpc_arg2, vt_send_eol, no_alt,
	    OBJ(int,6), 0, MAXUINT, "Message argument 2");
NUMBER     (vts_rpc_arg1, vts_rpc_arg2, no_alt,
	    OBJ(int,5), 0, MAXUINT, "Message argument 1");

NUMBER     (vts_rpc_arg, vt_send_eol, no_alt,
	    OBJ(int,5), 0, MAXUINT, "Message argument");

KEYWORD_ID_MM(vts_rpc_type_searchall, vts_rpc_arg1, no_alt,
	      OBJ(int,4), 6,
	      "searchall", "Send a VINES NetRPC 'search all' message",
	      PRIV_OPR, 7);
KEYWORD_ID (vts_rpc_type_returnaddress, vts_rpc_arg, vts_rpc_type_searchall,
	    OBJ(int,4), 5,
	    "returnaddress", "Send a VINES NetRPC 'return address' message",
	    PRIV_OPR);
KEYWORD_ID (vts_rpc_type_search, vts_rpc_arg1, vts_rpc_type_returnaddress,
	    OBJ(int,4), 4,
	    "search", "Send a VINES NetRPC 'search' message", PRIV_OPR);
KEYWORD_ID (vts_rpc_type_abort, vts_rpc_arg, vts_rpc_type_search,
	    OBJ(int,4), 3,
	    "abort", "Send a VINES NetRPC 'abort' message", PRIV_OPR);
KEYWORD_ID (vts_rpc_type_return, vt_send_eol, vts_rpc_type_abort,
	    OBJ(int,4), 2,
	    "return", "Send a VINES NetRPC 'return' message", PRIV_OPR);
KEYWORD_ID (vts_rpc_type_reject, vts_rpc_arg, vts_rpc_type_return,
	    OBJ(int,4), 1,
	    "reject", "Send a VINES NetRPC 'reject' message", PRIV_OPR);
KEYWORD_ID (vts_rpc_type_call, vts_rpc_arg, vts_rpc_type_reject,
	    OBJ(int,4), 0,
	    "call", "Send a VINES NetRPC 'call' message", PRIV_OPR);
NUMBER     (vts_rpc_sport, vts_rpc_type_call, no_alt,
	    OBJ(int,3), 1, 65535, "Source port");
NUMBER     (vts_rpc_dport, vts_rpc_sport, no_alt,
	    OBJ(int,2), 1, 65535, "Service WKP");
VINADDR	   (vts_rpc_daddr, vts_rpc_dport, no_alt,
	    OBJ(paddr,1), "Destination address");
KEYWORD_ID (vt_send_rpc, vts_rpc_daddr, vt_send_svc,
	    OBJ(int,1), VINES_SEND_NETRPC,
	    "netrpc", "Send a VINES NetRPC message", PRIV_OPR);

/**************************************************
 *
 * Send an SPP packet (Transport Layer)
 */

SET        (vts_spp_macaddr_set, vt_send_eol, OBJ(int,3), TRUE);
ENADDR	   (vts_spp_macaddr, vts_spp_macaddr_set, vt_send_eol,
	    OBJ(hwaddr,1), "Source MAC address");
SET        (vts_spp_saddr_set, vts_spp_macaddr, OBJ(int,2), TRUE);
VINADDR	   (vts_spp_saddr, vts_spp_saddr_set, vt_send_eol,
	    OBJ(paddr,2), "Source address");
VINADDR	   (vts_spp_daddr, vts_spp_saddr, no_alt,
	    OBJ(paddr,1), "Destination address");
KEYWORD_ID (vt_send_spp, vts_spp_daddr, vt_send_rpc,
	    OBJ(int,1), VINES_SEND_SPP,
	    "spp", "Send a VINES SPP packet", PRIV_OPR);

/**************************************************
 *
 * Send an Sequenced RTP packet
 */

KEYWORD_ID (vts_srtp_reinit, vt_send_eol, no_alt,
	    OBJ(int,2), VSRTP_REINIT,
	    "reinit", "Reinit packet", PRIV_OPR);
KEYWORD_ID (vts_srtp_update, vt_send_eol, vts_srtp_reinit,
	    OBJ(int,2), VSRTP_UPDATE,
	    "update", "Update packet", PRIV_OPR);

/* XXX <rif-string> - this loops to gather all the RIF numbers.  */
pdecl(vt_srtp_rif_1);
WHITESPACE (vt_srtp_rif_3b, vt_send_eol, vt_srtp_rif_1);
HELP	   (vt_srtp_rif_3a, vt_srtp_rif_1, "H.H...\t");
CHARACTER  (vt_srtp_rif_3,  vt_srtp_rif_3a, vt_srtp_rif_3b, '.');
/* Make sure that the user doesn't attempt to give more than we can handle */
PRINT	   (vt_srtp_rif_2b, no_alt, "RIF string too long");
TESTVAR	   (vt_srtp_rif_2a, vt_srtp_rif_2b, vt_srtp_rif_3,
	    NONE, NONE, NONE, OBJ(int,5), SRB_MAXRIF/2);
EVAL	   (vt_srtp_rif_2,  vt_srtp_rif_2a,
	    csb->uiarray[GETOBJ(int,5)++] = GETOBJ(int,6));
HELP	   (vt_srtp_rif_1a, vt_srtp_rif_2, "H.H...\t");
HEXADECIMAL(vt_srtp_rif_1,  vt_srtp_rif_1a, no_alt, OBJ(int,6));
HEXADECIMAL(vt_srtp_rif_0,  vt_srtp_rif_1a, vt_send_eol, OBJ(int,6));
HELP	   (vt_srtp_rif_rif, vt_srtp_rif_0, "  H.H...\tRIF entry\n");

ENADDR	   (vts_srtp_redir_nbrmacaddr, vt_srtp_rif_rif, no_alt,
	    OBJ(hwaddr,1), "Neighbor MAC level address");
NUMBER     (vts_srtp_redir_nbrseq, vts_srtp_redir_nbrmacaddr, no_alt,
	    OBJ(int,10), 0, MAXUINT, "Neighbor Sequence");
NUMBER     (vts_srtp_redir_nbrflags, vts_srtp_redir_nbrseq, no_alt,
	    OBJ(int,9), 1, 255, "Neighbor Flags");
NUMBER     (vts_srtp_redir_nbrmetric, vts_srtp_redir_nbrflags, no_alt,
	    OBJ(int,4), 1, 65535, "Neighbor Metric");
VINADDR	   (vts_srtp_redir_nbraddr, vts_srtp_redir_nbrmetric, no_alt,
	    OBJ(paddr,4), "Neighbor network level address");
NUMBER     (vts_srtp_redir_targetseq, vts_srtp_redir_nbraddr, no_alt,
	    OBJ(int,8), 0, MAXUINT, "Target Sequence");
NUMBER     (vts_srtp_redir_targetflags, vts_srtp_redir_targetseq, no_alt,
	    OBJ(int,7), 1, 255, "Target Flags");
NUMBER     (vts_srtp_redir_targetmetric, vts_srtp_redir_targetflags, no_alt,
	    OBJ(int,3), 1, 65535, "Target Metric");
VINADDR	   (vts_srtp_redir_targetaddr, vts_srtp_redir_targetmetric, no_alt,
	    OBJ(paddr,3), "Target network level address");
VINADDR	   (vts_srtp_redir_srcaddr, vts_srtp_redir_targetaddr, no_alt,
	    OBJ(paddr,2), "Source network level address");
KEYWORD_ID (vts_srtp_redirect, vts_srtp_redir_srcaddr, vts_srtp_update,
	    OBJ(int,2), VSRTP_REDIRECT,
	    "redirect", "Redirect packet", PRIV_OPR);


SET        (vts_srtp_req_data_set, vt_send_eol, OBJ(int,5), TRUE);
HEXNUM     (vts_srtp_req_data, vts_srtp_req_data_set, vt_send_eol,
	    OBJ(int,4), "Request data");
HEXNUM     (vts_srtp_req_type, vts_srtp_req_data, no_alt,
	    OBJ(int,3), "Request type");
KEYWORD_ID (vts_srtp_request, vts_srtp_req_type, vts_srtp_redirect,
	    OBJ(int,2), VSRTP_REQUEST,
	    "request", "Request packet", PRIV_OPR);

VINADDR	   (vts_srtp_daddr, vts_srtp_request, no_alt,
	    OBJ(paddr,1), "Destination address");
KEYWORD_ID (vt_send_srtp, vts_srtp_daddr, vt_send_spp,
	    OBJ(int,1), VINES_SEND_SRTP,
	    "srtp", "Send an SRTP packet", PRIV_OPR);

/**************************************************
 *
 * Send an RTP packet
 */

/* XXX <rif-string> - this loops to gather all the RIF numbers.  */
pdecl(vt_rif_1);
WHITESPACE (vt_rif_3b, vt_send_eol, vt_rif_1);
HELP	   (vt_rif_3a, vt_rif_1, "H.H...\t");
CHARACTER  (vt_rif_3,  vt_rif_3a, vt_rif_3b, '.');
/* Make sure that the user doesn't attempt to give more than we can handle */
PRINT	   (vt_rif_2b, no_alt, "RIF string too long");
TESTVAR	   (vt_rif_2a, vt_rif_2b, vt_rif_3,
	    NONE, NONE, NONE, OBJ(int,5), SRB_MAXRIF/2);
EVAL	   (vt_rif_2,  vt_rif_2a,
	    csb->uiarray[GETOBJ(int,5)++] = GETOBJ(int,6));
HELP	   (vt_rif_1a, vt_rif_2, "H.H...\t");
HEXADECIMAL(vt_rif_1,  vt_rif_1a, no_alt, OBJ(int,6));
HEXADECIMAL(vt_rif_0,  vt_rif_1a, vt_send_eol, OBJ(int,6));
HELP	   (vt_rif_rif, vt_rif_0, "  H.H...\tRIF entry\n");

ENADDR	   (vts_redir_nbrmacaddr, vt_rif_rif, no_alt,
	    OBJ(hwaddr,1), "Neighbor MAC level address");
NUMBER     (vts_redir_nbrmetric, vts_redir_nbrmacaddr, no_alt,
	    OBJ(int,4), 1, 65535, "Target Metric");
VINADDR	   (vts_redir_nbraddr, vts_redir_nbrmetric, no_alt,
	    OBJ(paddr,4), "Neighbor network level address");
NUMBER     (vts_redir_targetmetric, vts_redir_nbraddr, no_alt,
	    OBJ(int,3), 1, 65535, "Target Metric");
VINADDR	   (vts_redir_targetaddr, vts_redir_targetmetric, no_alt,
	    OBJ(paddr,3), "Target network level address");
VINADDR	   (vts_redir_srcaddr, vts_redir_targetaddr, no_alt,
	    OBJ(paddr,2), "Source network level address");
VINADDR	   (vts_redir_dstaddr, vts_redir_srcaddr, no_alt,
	    OBJ(paddr,1), "Destination network level address");
KEYWORD_ID (vts_rtp_redirect, vts_redir_dstaddr, no_alt,
	    OBJ(int,2), VRTP_REDIRECT,
	    "redirect", "Send a VINES redirect packet", PRIV_OPR);

NUMBER     (vts_rtp_hello_count, vt_send_eol, no_alt,
	    OBJ(int,4), 1, 5000, "Number of hellos to send");
SET        (vts_rtp_hello_set, vts_rtp_hello_count, OBJ(int,3), TRUE);
ENADDR	   (vts_rtp_hello_macaddr, vts_rtp_hello_set, no_alt,
	    OBJ(hwaddr,1), "Starting neighbor MAC level address");
INTERFACE  (vts_rtp_hello_if, vts_rtp_hello_macaddr, no_alt,
	    OBJ(idb,1), IFTYPE_IEEE);
VINADDR	   (vts_rtp_hello_netaddr, vts_rtp_hello_if, no_alt,
	    OBJ(paddr,1), "Starting source network level address");
KEYWORD_ID (vts_rtp_hello, vts_rtp_hello_netaddr, vts_rtp_redirect,
	    OBJ(int,2), VRTP_UPDATE,
	    "hello", "Send one or more VINES hello packets", PRIV_OPR);

KEYWORD_ID (vt_send_rtp, vts_rtp_hello, vt_send_srtp,
	    OBJ(int,1), VINES_SEND_RTP,
	    "rtp", "Send an RTP packet", PRIV_OPR);

/**************************************************
 *
 * Send a loopback packet
 */

INTERFACE  (vt_send_loop_if, vt_send_eol, no_alt,
	    OBJ(idb,1), IFTYPE_IEEE);
KEYWORD_ID (vt_send_loop, vt_send_loop_if, vt_send_rtp,
	    OBJ(int,1), VINES_SEND_LOOP,
	    "loop", "Send a VINES loop packet", PRIV_OPR);

/**************************************************
 *
 * Send a IPC packet (Transport Layer)
 */

KEYWORD_ID (vt_send_ipc, vts_spp_daddr, vt_send_loop,
	    OBJ(int,1), VINES_SEND_IPC,
	    "ipc", "Send a VINES IPC packet", PRIV_OPR);

/**************************************************
 *
 * Send a ICP packet (Control Protocol)
 */

NUMBER     (vts_icp_value, vt_send_eol, no_alt,
	    OBJ(int,3), 1, 65535, "Metric value");
KEYWORD_ID (vts_icp_metric, vts_icp_value, no_alt,
	    OBJ(int,2), VICP_EXCEPTION,
	    "metric", "An icp metric notification packet", PRIV_OPR);
NUMBER     (vts_icp_code, vt_send_eol, no_alt,
	    OBJ(int,3), 1, 65535, "Exception code");
KEYWORD_ID (vts_icp_exception, vts_icp_code,
	    vts_icp_metric, OBJ(int,2), VICP_EXCEPTION,
	    "exception", "An icp exception notification packet", PRIV_OPR);
VINADDR	   (vts_icp_srcaddr, vts_icp_exception, no_alt,
	    OBJ(paddr,2), "Source network level address");
VINADDR	   (vts_icp_dstaddr, vts_icp_srcaddr, no_alt,
	    OBJ(paddr,1), "Destination network level address");
KEYWORD_ID (vt_send_icp, vts_icp_dstaddr, vt_send_ipc,
	    OBJ(int,1), VINES_SEND_ICP,
	    "icp", "Send a VINES ICP packet", PRIV_OPR);

/**************************************************
 *
 * Send a MAC level Echo packet
 */

INTERFACE  (vt_send_echo_if, vt_send_eol, no_alt,
	    OBJ(idb,1), IFTYPE_IEEE);
KEYWORD_ID (vt_send_echo, vt_send_echo_if, vt_send_ipc,
	    OBJ(int,1), VINES_SEND_ECHO,
	    "echo", "Send a VINES echo packet", PRIV_OPR);

/**************************************************
 *
 * Send a custom vines packet
 */

KEYWORD_ID (vt_send_custom, vt_send_eol, vt_send_echo,
	    OBJ(int,1), VINES_SEND_CUSTOM,
	    "custom", "Send a custom VINES packet", PRIV_OPR);

/**************************************************
 *
 * Blast a number of IPC packets (Transport Layer)
 */

NUMBER     (vts_blast_count, vts_spp_daddr, no_alt,
	    OBJ(int,9), 1, 5000, "Number of IPC packets to send");
KEYWORD_ID (vt_send_blast, vts_blast_count, vt_send_custom,
	    OBJ(int,1), VINES_SEND_BLAST,
	    "blast", "Send a number of VINES IPC packet", PRIV_OPR);

/**************************************************
 *
 * Send a ARP packet
 */

INTERFACE  (vts_arp_if, vt_send_eol, no_alt,
	    OBJ(idb,1), IFTYPE_IEEE);
VINADDR	   (vts_arp_ap_addr, vts_arp_if, no_alt,
	    OBJ(paddr,1), "Assigned network level address");
KEYWORD_ID (vts_arp_a_resp, vts_arp_ap_addr, no_alt,
	    OBJ(int,2), VARP_ASS_RESP,
	    "response", "Arp service response", PRIV_OPR);
VINADDR	   (vts_arp_aq_addr, vts_arp_if, no_alt,
	    OBJ(paddr,1), "Address to request assignment from");
KEYWORD_ID (vts_arp_a_req, vts_arp_aq_addr, vts_arp_a_resp,
	    OBJ(int,2), VARP_ASS_REQ,
	    "request", "Arp service request", PRIV_OPR);
KEYWORD    (vts_arp_assign, vts_arp_a_req, no_alt,
	    "assignment", "Arp assignment request or response", PRIV_OPR);
KEYWORD_ID (vts_arp_q_resp, vts_arp_if, no_alt,
	    OBJ(int,2), VARP_SVC_RESP,
	    "response", "Arp query response", PRIV_OPR);
KEYWORD_ID (vts_arp_q_req, vts_arp_if, vts_arp_q_resp,
	    OBJ(int,2), VARP_SVC_REQ,
	    "request", "Arp query request", PRIV_OPR);
KEYWORD    (vts_arp_service, vts_arp_q_req, vts_arp_assign,
	    "service", "Arp query request or response", PRIV_OPR|PRIV_NOHELP);
KEYWORD    (vts_arp_query, vts_arp_q_req, vts_arp_service,
	    "query", "Arp query request or response", PRIV_OPR);
TESTVAR    (vts_arp_number_test, vts_arp_aq_addr, NONE,
	    vts_arp_if, vts_arp_ap_addr,
	    NONE, OBJ(int,2), VARP_ASS_REQ);
NUMBER     (vts_arp_number, vts_arp_number_test, vts_arp_query,
	    OBJ(int,2), 0, 3, "Arp code");
KEYWORD_ID (vt_send_arp, vts_arp_number, vt_send_blast,
	    OBJ(int,1), VINES_SEND_ARP,
	    "arp", "Send a VINES ARP packet", PRIV_OPR);

/**************************************************
 *
 * Resend the previous packet
 */

KEYWORD_ID (vt_send_again, vt_send_eol, vt_send_arp,
	    OBJ(int,1), VINES_SEND_AGAIN,
	    "again", "Repeat the last VINES packet sent", PRIV_OPR);

/**************************************************
 *
 * Exit send mode
 */

KEYWORD_ID (vt_send_end, vt_send_eol, vt_send_again,
	    OBJ(int,1), VINES_SEND_END,
	    "end", "Exit send mode", PRIV_MIN);

/**************************************************
 *
 * Top level for send mode
 */

NOP	   (vt_send_top, vt_send_end, NONE);

/**************************************************
 **************************************************
 *
 * TOP LEVEL VINES TEST COMMANDS
 */

/**************************************************
 *
 * StreetTalk things
 */

EOLNS	   (vt_st_eol, vines_test_st_command);

STRING     (vt_st_del_org, vt_st_eol, no_alt, OBJ(string,2),
	    "org name");
STRING     (vt_st_del_group, vt_st_del_org, no_alt, OBJ(string,1),
	    "group name");
KEYWORD_ID (vt_st_del, vt_st_del_group, no_alt,
	    OBJ(int,1), VINES_ST_DELETE,
	    "delete", "Delete a local group", PRIV_OPR);

NUMBER     (vt_st_add_priority, vt_st_eol, no_alt,
	    OBJ(int,2), 0, 15, "Priority (0 = highest)");
STRING     (vt_st_add_org, vt_st_add_priority, no_alt, OBJ(string,2),
	    "org name");
STRING     (vt_st_add_group, vt_st_add_org, no_alt, OBJ(string,1),
	    "group name");
KEYWORD_ID (vt_st_add, vt_st_add_group, vt_st_del,
	    OBJ(int,1), VINES_ST_ADD,
	    "add", "Add a local group", PRIV_OPR);

VINADDR	   (vt_st_cold_addr, vt_st_eol, no_alt,
	    OBJ(paddr,1), "Destination address");
KEYWORD_ID (vt_st_cold, vt_st_cold_addr, vt_st_add,
	    OBJ(int,1), VINES_ST_COLDDUMP,
	    "colddump", "Send a streettalk cold dump packet", PRIV_OPR);
KEYWORD_ID (vt_st_welcome, vt_st_eol, vt_st_cold,
	    OBJ(int,1), VINES_ST_WELCOME,
	    "welcome", "Send a streettalk welcome packet", PRIV_OPR);
VINADDR	   (vt_st_need_addr, vt_st_eol, no_alt,
	    OBJ(paddr,1), "Destination address");
KEYWORD_ID (vt_st_need, vt_st_need_addr, vt_st_welcome,
	    OBJ(int,1), VINES_ST_NEED,
	    "need", "Send a streettalk need packet", PRIV_OPR);
KEYWORD_ID (vt_st_detail, vt_st_eol, vt_st_need,
	    OBJ(int,1), VINES_ST_DETAIL,
	    "detail", "Send a streettalk detail packet", PRIV_OPR);
KEYWORD_ID (vt_st_summary, vt_st_eol, vt_st_detail,
	    OBJ(int,1), VINES_ST_SUMMARY,
	    "summary", "Send a streettalk summary packet", PRIV_OPR);
KEYWORD    (vt_st, vt_st_summary, no_alt,
	    "st", "Send a vines streettalk packet", PRIV_OPR);

/**************************************************
 *
 * Server Service things
 */

EOLNS	   (vt_ss_eol, vines_test_ss_command);
KEYWORD_ID (vt_ss_reset, vt_ss_eol, no_alt,
	    OBJ(int,1), VINES_SS_RESET,
	    "reset", "Reset the clock to the current time", PRIV_OPR);
KEYWORD_ID (vt_ss_periodic, vt_ss_eol, vt_ss_reset,
	    OBJ(int,1), VINES_SS_PERIODIC,
	    "periodic", "Call the periodic routine", PRIV_OPR);
KEYWORD_ID (vt_ss_endoftime, vt_ss_eol, vt_ss_periodic,
	    OBJ(int,1), VINES_SS_ENDOFTIME,
	    "end-of-time", "Set the clock to -1", PRIV_OPR);
KEYWORD_ID (vt_ss_bump, vt_ss_eol, vt_ss_endoftime,
	    OBJ(int,1), VINES_SS_BUMP,
	    "bump", "Bump the clock by 12 hours", PRIV_OPR);
KEYWORD    (vt_ss, vt_ss_bump, vt_st,
	    "ss", "Do Server Service things", PRIV_OPR);

/**************************************************
 *
 * Set values
 */

EOLS	   (vt_set_eol, vines_test_command, VINES_TEST_SET);

HEXNUM     (vts_state_num, vt_set_eol, no_alt,
	    OBJ(int,2), "New state value");
VINADDR    (vts_state_addr, vts_state_num, no_alt,
	    OBJ(paddr,1), "Neighbor to change");
KEYWORD_ID (vt_set_state, vts_state_addr, no_alt,
	    OBJ(int,1), VINES_SET_STATE,
	    "state", "Send a SRTP state", PRIV_OPR);

NUMBER     (vts_retry_num, vt_set_eol, no_alt,
	    OBJ(int,2), 0, MAXUINT, "New time in seconds");
VINADDR    (vts_retry_addr, vts_retry_num, no_alt,
	    OBJ(paddr,1), "Neighbor to change");
KEYWORD_ID (vt_set_retry, vts_retry_addr, vt_set_state,
	    OBJ(int,1), VINES_SET_RETRY_TIMER,
	    "retry-timer", "Send the retry timer value", PRIV_OPR);

NUMBER     (vts_reass_num, vt_set_eol, no_alt,
	    OBJ(int,2), 0, MAXUINT, "New time in seconds");
VINADDR    (vts_reass_addr, vts_reass_num, no_alt,
	    OBJ(paddr,1), "Neighbor to change");
KEYWORD_ID (vt_set_reass, vts_reass_addr, vt_set_retry,
	    OBJ(int,1), VINES_SET_REASS_TIMER,
	    "reass-timer", "Send the reassembly timer value", PRIV_OPR);

NUMBER     (vts_rc_num, vt_set_eol, no_alt,
	    OBJ(int,2), 0, MAXUINT, "New time in seconds");
VINADDR    (vts_rc_addr, vts_rc_num, no_alt,
	    OBJ(paddr,1), "Neighbor to change");
KEYWORD_ID (vt_set_rc, vts_rc_addr, vt_set_reass,
	    OBJ(int,1), VINES_SET_RC_TIMER,
	    "rc-timer", "Send the roll call timer value", PRIV_OPR);

NUMBER     (vts_periodic_num, vt_set_eol, no_alt,
	    OBJ(int,2), 0, MAXUINT, "New time in seconds");
VINADDR    (vts_periodic_addr, vts_periodic_num, no_alt,
	    OBJ(paddr,1), "Neighbor to change");
KEYWORD_ID (vt_set_periodic, vts_periodic_addr, vt_set_rc,
	    OBJ(int,1), VINES_SET_PERIODIC_TIMER,
	    "periodic-timer", "Send the periodic timer value", PRIV_OPR);

HEXNUM     (vts_origin_num, vt_set_eol, no_alt,
	    OBJ(int,2), "New origin timestamp");
VINADDR    (vts_origin_addr, vts_origin_num, no_alt,
	    OBJ(paddr,1), "Neighbor to change");
KEYWORD_ID (vt_set_origin, vts_origin_addr, vt_set_periodic,
	    OBJ(int,1), VINES_SET_ORIGIN,
	    "origin", "Send a SRTP origin timestamp", PRIV_OPR);

HEXNUM     (vts_local_num, vt_set_eol, no_alt,
	    OBJ(int,2), "New local timestamp");
KEYWORD_ID (vt_set_local, vts_local_num, vt_set_origin,
	    OBJ(int,1), VINES_SET_LOCAL,
	    "local", "Send the local timestamp", PRIV_OPR);

KEYWORD    (vt_set, vt_set_local, vt_ss,
	    "set", "Send a VINES value", PRIV_OPR);

/**************************************************
 *
 * Send a packet or enter send mode
 */

KEYWORD (vt_send, vt_send_again, vt_set,
	 "send", "Send a VINES packet", PRIV_OPR);

/**************************************************
 *
 * Flush tables (aka "vines clear" command)
 */

EOLS	   (vt_generate_eol, vines_test_command, VINES_TEST_GENERATE);
KEYWORD_ID (vt_generate_null, vt_generate_eol, no_alt,
	    OBJ(int,1), VINES_GEN_NULL,
	    "null", "srtp null updates", PRIV_OPR);
KEYWORD_ID (vt_generate_change, vt_generate_eol, vt_generate_null,
	    OBJ(int,1), VINES_GEN_CHANGE,
	    "change", "srtp change updates", PRIV_OPR);
KEYWORD_ID (vt_generate_full, vt_generate_eol, vt_generate_change,
	    OBJ(int,1), VINES_GEN_FULL,
	    "full", "srtp full updates", PRIV_OPR);
KEYWORD    (vt_generate_srtp, vt_generate_full, no_alt,
	    "srtp", "srtp updates", PRIV_OPR);
KEYWORD	   (vt_generate, vt_generate_srtp, vt_send,
	    "generate", "generate information", PRIV_OPR);

/**************************************************
 *
 * Flush tables (aka "vines clear" command)
 */

EOLS	(vt_flush_eol, vines_test_command, VINES_TEST_FLUSH);
KEYWORD	(vt_flush, vt_flush_eol, vt_generate,
	 "flush", "Flush tables", PRIV_OPR);

/**************************************************
 *
 * Bye.
 */

EOLS	(vt_end_eol, vines_test_command, VINES_TEST_END);
KEYWORD	(vt_end, vt_end_eol, vt_flush,
	 "end", "Exit VINES test mode", PRIV_OPR);

/**************************************************
 *
 * Set up data values
 */

EOLS	(vt_data_eol, vines_test_command, VINES_TEST_DATA);
HEXNUM  (vt_data_10, vt_data_eol,vt_data_eol, OBJ(int,10), "Data value 10");
HEXNUM  (vt_data_9,  vt_data_10, vt_data_eol, OBJ(int,9),  "Data value 9");
HEXNUM  (vt_data_8,  vt_data_9,	 vt_data_eol, OBJ(int,8),  "Data value 8");
HEXNUM  (vt_data_7,  vt_data_8,	 vt_data_eol, OBJ(int,7),  "Data value 7");
HEXNUM  (vt_data_6,  vt_data_7,	 vt_data_eol, OBJ(int,6),  "Data value 6");
HEXNUM  (vt_data_5,  vt_data_6,	 vt_data_eol, OBJ(int,5),  "Data value 5");
HEXNUM  (vt_data_4,  vt_data_5,	 vt_data_eol, OBJ(int,4),  "Data value 4");
HEXNUM  (vt_data_3,  vt_data_4,	 vt_data_eol, OBJ(int,3),  "Data value 3");
HEXNUM  (vt_data_2,  vt_data_3,	 vt_data_eol, OBJ(int,2),  "Data value 2");
HEXNUM  (vt_data_1,  vt_data_2,	 vt_data_eol, OBJ(int,1),  "Data value 1");
KEYWORD	(vt_data, vt_data_1, vt_end,
	 "data", "Set data values used in various places", PRIV_OPR);

/**************************************************
 *
 * Test the checksum routine
 */

EOLS	(vt_checksum_eol, vines_test_command, VINES_TEST_CHECKSUM);
KEYWORD	(vt_checksum, vt_checksum_eol, vt_data,
	 "checksum", "Checksum test", PRIV_OPR);

/**************************************************
 *
 * Build vines tables
 */

EOLS	 (vt_build_eol, vines_test_command, VINES_TEST_BUILD);
INTERFACE(vt_build_if, vt_build_eol, vt_build_eol, OBJ(idb,1), IFTYPE_ANYSUB);
NUMBER   (vt_build_nbrs, vt_build_if, no_alt,
	  OBJ(int,3), 1, 1000000, "Neighbors");
VINESNET (vt_build_start, vt_build_nbrs, no_alt,
	  OBJ(paddr,1), "Starting network");
NUMBER   (vt_build_nets, vt_build_start, no_alt,
	  OBJ(int,1), 1, 1000000, "Network numbers");
KEYWORD	 (vt_build, vt_build_nets, vt_checksum,
	  "build", "Build tables", PRIV_OPR);

/**************************************************
 *
 * Top level for vines test mode
 */

NOP	(top_test_vines, vt_build, NONE);


/******************************************************************
 * test vines
 */
EOLNS	(priv_test_vines_eol, vines_test);
KEYWORD	(priv_test_vines, priv_test_vines_eol, ALTERNATE,
	 "vines", "VINES diagnostic code", PRIV_OPR/*|PRIV_NOHELP*/);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_vines
