/* $Id: latmsg.h,v 3.2 1995/11/17 17:34:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latmsg.h,v $
 *------------------------------------------------------------------
 * $Log: latmsg.h,v $
 * Revision 3.2  1995/11/17  17:34:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    latmsg		lat virtual circuit protocol definitions

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef LATMSG_H
#define LATMSG_H    0			/* Indicate file has been read	*/

/*-----------------------------------------------------------------------
    Declare the LAT protocol message types and flags.
 -----------------------------------------------------------------------*/
#define LAT_RESPONSE	0x01		/* Response requested flag      */
#define LAT_MASTER	0x02		/* Master (server->host)        */
#define LAT_TYPE	0xfc		/* Type field mask              */

#define LAT_RUN		0x00		/* Run message                  */
#define LAT_START	0x04		/* Start message                */
#define LAT_HALT	0x08		/* Halt  message                */
#define LAT_SERVICE	0x28		/* Directory services message   */
#define LAT_COMMAND	0x30		/* Queue command message	*/
#define	LAT_STATUS	0x34		/* Queue status  message	*/
#define LAT_SOLICIT	0x38		/* Information solicit  message	*/
#define LAT_INFO	0x3c		/* Information response	message	*/

/*-----------------------------------------------------------------------
    Define various protocol limits
 -----------------------------------------------------------------------*/
#define LAT_MAX_VC	100		/* Maximum VC timer		*/
#define LAT_MIN_DLL	576		/* Minimum DLL size		*/
#define LAT_MAX_DLL	1518		/* Maximum DLL size		*/
#define LAT_MIN_LAT	5		/* Minimum LAT protocol		*/
#define LAT_MAX_LAT	5		/* Maximum LAT protocol		*/

/*-----------------------------------------------------------------------
    Define the LAT virtual circuit message structure. The msg_type field
    is common to all messages. The remaining fields apply only to circuit
    messages (LAT_RUN, LAT_START, and LAT_STOP).
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	cir_type;		/* LAT message type		*/
	byte	cir_slots;		/* Number of slots in message	*/
	word	cir_dst_id;		/* Destination ID, sequence	*/
	word	cir_src_id;		/* Source ID, sequence		*/
	byte	cir_seq;		/* Message sequence number	*/
	byte	cir_ack;		/* Message acknowledged number  */
    } cir_msg, IF_VIEW(cir_iptr), OF_VIEW(cir_optr);

/*-----------------------------------------------------------------------
    Define the LAT virtual circuit start message (fixed part).
 -----------------------------------------------------------------------*/
    typedef struct
    {
	word	str_dll_max;		/* Maximum message size         */
	byte	str_prot_ver;		/* LAT protocol version		*/
	byte	str_prot_eco;		/* LAT protocol ECO		*/
	byte	str_max_ses;		/* Maximum sessions/circuit	*/
	byte	str_rcv_extra;		/* Extra receive buffers	*/
	byte	str_vc_timer;		/* Circuit timer		*/
	byte	str_ka_timer;		/* Keep alive timer		*/
	word	str_facility;		/* Facility number              */
	byte	str_prod_code;		/* Product  number              */
	byte	str_prod_ver;		/* Product  version		*/
    } str_msg, IF_VIEW(str_iptr), OF_VIEW(str_optr);


#define SUPERLAT_CODE	234		/* SuperLAT prodoct code	*/
#define SUPERLAT_VER	0x30		/* SuperLAT product version	*/

/*-----------------------------------------------------------------------
    Define the LAT virtual circuit halt message.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	halt_code;		/* Halt reason code             */
	byte	halt_len;		/* Reason text length           */
    } halt_msg, IF_VIEW(halt_iptr), OF_VIEW(halt_optr);

#define HALTC_NONE		0	/* No specific halt reason	*/
#define HALTC_DONE		1	/* No more session on circuit	*/
#define HALTC_BAD_MSG		2	/* Invalid message for state	*/
#define HALTC_VC_HALT		3	/* User level VC halt		*/
#define HALTC_NO_PROGRESS	4	/* No progress on circuit	*/
#define HALTC_TIMEOUT		5	/* Circuit timeout		*/
#define HALTC_RETRANSMIT	6	/* Retransmission limit reached	*/
#define HALTC_NO_RESOURCE	7	/* Not enough resources		*/
#define HALTC_TIMER_VALUE	8	/* Timer out of range		*/
#define HALTC_VC_EXCEEDED	9	/* Number VC exceeded		*/

/*-----------------------------------------------------------------------
    Define the LAT directory services message structure (fixed part).
    The message is then followed by counted fields of group masks,
    node name, node description, services, and classes.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	svc_type;		/* LAT message type		*/
	byte	svc_vc_timer;		/* Virtual circuit timer	*/
	byte	svc_prot_hi;		/* LAT protocol, high version	*/
	byte	svc_prot_lo;		/* LAT protocol, low  version	*/
	byte	svc_prot_ver;		/* LAT protocol version		*/
	byte	svc_prot_eco;		/* LAT protocol ECO		*/
	byte	svc_sequence;		/* Directory sequence number	*/
	byte	svc_changes;		/* Directory change flags	*/
	word	svc_dll_max;		/* Max ethernet message size	*/
	byte	svc_mc_timer;		/* Directory service timer	*/
	byte	svc_node_status;	/* Node status			*/
    } svc_msg, IF_VIEW(svc_iptr), OF_VIEW(svc_optr);

#define SVC_GROUPS	0x01		/* Group codes changed		*/
#define SVC_IDENT	0x02		/* Node identification changed	*/
#define SVC_SERVICES	0x04		/* Service names   changed	*/
#define SVC_RATING	0x08		/* Service ratings changed	*/
#define SVC_DESCRIPTOR	0x10		/* Service descriptors changed	*/
#define SVC_CLASS	0x20		/* Service class changed	*/
#define SVC_OTHER	0x80		/* Some other field changed	*/
#define SVC_CHANGE	0xff		/* All fields have changed	*/

#define SVC_NODE_OFF	1		/* Node down status		*/
#define SVC_CLASS_TERM	1		/* Interactive terminal service	*/
#define SVC_ANNC_EN	0x100		/* Svc. announcements enabled	*/
#define SVC_AGENT	0x200		/* Node is solicitation agent	*/
#define SVC_CLASS_XWIRE	3		/* X wire protocol		*/
#define SVC_CLASS_XFONT 4		/* X font load protocol		*/

#define SVC_OVERHEAD (sizeof(svc_msg) + GROUPS + ND_STR + ID_STR + SC_STR + 1)

/*-----------------------------------------------------------------------
    Define the LAT host-initiated solicit message header.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	si_type;		/* LAT message type		*/
	byte	si_format;		/* Procotol format		*/
	byte	si_prot_hi;		/* LAT protocol, high version	*/
	byte	si_prot_lo;		/* LAT protocol, low  version	*/
	byte	si_prot_ver;		/* LAT protocol version		*/
	byte	si_prot_eco;		/* LAT protocol ECO		*/
	word	si_dll_max;		/* Max ethernet message size	*/
	word	si_req_no;		/* Solicit request number	*/
	word	si_timer;		/* Response timer		*/
    } si_msg, IF_VIEW(si_iptr), OF_VIEW(si_optr);

#define SI_P_SC 	2		/* Required service class	*/

/*-----------------------------------------------------------------------
    Define the LAT host-initiated response message header.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	ri_type;		/* LAT message type		*/
	byte	ri_format;		/* Procotol format		*/
	byte	ri_prot_hi;		/* LAT protocol, high version	*/
	byte	ri_prot_lo;		/* LAT protocol, low  version	*/
	byte	ri_prot_ver;		/* LAT protocol version		*/
	byte	ri_prot_eco;		/* LAT protocol ECO		*/
	word	ri_dll_max;		/* Max ethernet message size	*/
	word	ri_req_no;		/* Solicit request number	*/
	word	ri_req_status;		/* Response status		*/
	word	ri_node_status;		/* Source node status		*/
	word	ri_node_addr[3];	/* Source node address		*/
	word	ri_mc_timer;		/* Source node MC timer		*/
    } ri_msg, IF_VIEW(ri_iptr), OF_VIEW(ri_optr);

#define	RI_ETHER	0x00		/* Node operates in Ethernet	*/
#define RI_802		0x01		/* Node operates in 802.3	*/
#define RI_ETHER_802	0x02		/* Node operates in both	*/

#define RI_NO_SERVICE	0x02		/* Node does not offer service	*/
#define RI_NO_ACCESS	0x04		/* Access code violation	*/

#define RI_DISABLED	0x01		/* Node is disabled		*/
#define RI_START	0x02		/* May send start slot to node	*/
#define RI_COMMAND	0x04		/* May send command to node	*/

#define RI_ENABLED	0x01		/* Service is enabled		*/
#define RI_QUEUE	0x02		/* Service supports queueing	*/

/*-----------------------------------------------------------------------
    Define the LAT host-initiated command message header.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	cmd_type;		/* LAT message type		*/
	byte	cmd_format;		/* Procotol format		*/
	byte	cmd_prot_hi;		/* LAT protocol, high version	*/
	byte	cmd_prot_lo;		/* LAT protocol, low  version	*/
	byte	cmd_prot_ver;		/* LAT protocol version		*/
	byte	cmd_prot_eco;		/* LAT protocol ECO		*/
	word	cmd_dll_max;		/* Max ethernet message size	*/
	word	cmd_req_no;		/* Command request number	*/
	word	cmd_que_no;		/* Queued entry identifier	*/
	byte	cmd_operation;		/* Command message operation	*/
	byte	cmd_modifier;		/* Command message modifier	*/
    } cmd_msg, IF_VIEW(cmd_iptr), OF_VIEW(cmd_optr);

#define CMD_ACCESS	1		/* Solicit access w/o queuing	*/
#define CMD_ACCESS_Q	2		/* Solicit access w/  queuing	*/
#define CMD_CANCEL	3		/* Cancel entry in queue	*/
#define CMD_STS_ENTRY	4		/* Send status of entry		*/
#define CMD_STS_QUEUE	5		/* Send status of queue		*/
#define CMD_STS_ALL	6		/* Send status of all entries	*/

#define CMD_SEND_TIMED	0x01		/* Send status periodically	*/
#define CMD_SEND_CHANGE	0x02		/* Send status on queue change	*/

#define CMD_P_SC	1		/* Service class parameter	*/
#define CMD_P_SERVER	2		/* Server location [SC 3]	*/
#define CMD_PASSWD	3		/* Password parm code		*/
/*-----------------------------------------------------------------------
    Define the LAT host-initiated response message header.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	sts_type;		/* LAT message type		*/
	byte	sts_format;		/* Procotol format		*/
	byte	sts_prot_hi;		/* LAT protocol, high version	*/
	byte	sts_prot_lo;		/* LAT protocol, low  version	*/
	byte	sts_prot_ver;		/* LAT protocol version		*/
	byte	sts_prot_eco;		/* LAT protocol ECO		*/
	word	sts_dll_max;		/* Max ethernet message size	*/
	word	sts_timer;		/* Status transmission timer	*/
    } sts_msg, IF_VIEW(sts_iptr), OF_VIEW(sts_optr);

/*-----------------------------------------------------------------------
    Define the request entry portion of a status message.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	req_length;		/* Length of entry		*/
	byte	req_status;		/* Entry status			*/
	byte	req_error;		/* Entry error reason		*/
	byte	req_none;		/* Reserved field		*/
	word	req_req_no;		/* Command request number	*/
	word	req_que_no;		/* Queue entry number		*/
	word	req_que_time;		/* Elapse time entry queued	*/
	word	req_p_queue;		/* Port  queue entry #		*/
	word	req_q_queue;		/* Whole queue entry #		*/
    } req_field, IF_VIEW(req_iptr), OF_VIEW(req_optr);

#define	STS_QUEUED	0x02		/* Entry queued			*/
#define	STS_STARTED	0x04		/* Entry started		*/
#define	STS_NO_TIMED	0x08		/* Periodic status n/a		*/
#define STS_NO_CHANGE	0x10		/* Queue changes   n/a		*/
#define STS_ERROR	0x80		/* Request error		*/

#endif
