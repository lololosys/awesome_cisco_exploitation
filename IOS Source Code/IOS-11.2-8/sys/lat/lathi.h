/* $Id: lathi.h,v 3.2 1995/11/17 17:33:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lathi.h,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lathi.h,v $
 * Revision 3.2  1995/11/17  17:33:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

 /*-----------------------------------------------------------------------
    lathi		define host-initiated structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef LATHI_H
#define LATHI_H		0		/* Indicate file has been read	*/

/*----------------------------------------------------------------------
    Define the HIC flags.
 -----------------------------------------------------------------------*/
#define HI_MULTI	0x10		/* Can include in multi status	*/
#define HI_SUBJECT	0x20		/* Subject system entry		*/
#define HI_OBJECT	0x40		/* Object  system entry		*/
#define HI_DELETE	0x80		/* Delete after output		*/

#define HI_RESPONSE	0x01		/* Response to SI received	*/
#define HI_AGENT	0x02		/* Response is from an agent	*/
#define HI_SBINIT	0x04		/* Request is from 'sbopen'	*/

/*----------------------------------------------------------------------
    Define the signals to the environment level (used by hx_session())
 -----------------------------------------------------------------------*/
#define HX_TIMEOUT	1		/* Request attempt timed out	*/
#define HX_REJECTED	2		/* Request rejected by server	*/
#define HX_ARRIVED	3		/* HIC request arrived		*/
#define HX_STATUS	4		/* Status request complete	*/
#define HX_CANCEL	5		/* HIC request canceled		*/
#define HX_ACCEPT	6		/* HIC request accepted		*/

/*----------------------------------------------------------------------
    Define the host-initiated structure.
 -----------------------------------------------------------------------*/
    typedef struct hi
    {
	struct hi HI_VIEW(hi_link[2]);	/* Pointer to next structure	*/
	struct ob OB_VIEW(hi_ob);	/* Pointer to object block	*/
	struct qr QR_VIEW(hi_qr);	/* Pointer to queue entry	*/
	sid_type hi_sid;		/* Session identification	*/
#if CME_SLAT
	cid_type hi_cid;		/* CME Connection identifier	*/
#endif
	int	hi_tics;		/* Tics to timer elapse		*/
	word	hi_retries;		/* Remaining retransmissions	*/
	word	hi_dll_max;		/* Maximum frame size		*/
	byte	hi_flags;		/* HIC control flags		*/
	byte	hi_function;		/* HIC message function		*/
	byte	hi_command;		/* Command message type		*/
	byte	hi_modifier;		/* Command message modifier	*/
	byte	hi_status;		/* Request status		*/
	byte	hi_error;		/* Request reason		*/
	word	hi_req_no;		/* Remote  request number	*/
	word	hi_que_no;		/* Queue entry number		*/
	word	hi_que_time;		/* Total queue entries		*/
	word	hi_p_queue;		/* Port  queue position		*/
	word	hi_q_queue;		/* Queue entry position		*/
	word	hi_dest_addr[3];	/* Destination Ethernet address	*/
	idbtype	*hi_idb;		/* Interface for remote system  */
	byte	hi_node[ND_STR];	/* Remote node name 		*/
	byte	hi_service[SV_STR];	/* Service name			*/
	byte	hi_port[OB_STR];        /* Port    name 		*/
	byte	hi_sc;			/* Service class		*/
	byte	hi_pass[SV_STR];	/* Password			*/
#if LE_SUBJECT_HIC
#if LE_SERVER
	struct	sb SB_VIEW(hi_sb);	/* Session block pointer	*/
#endif	
	byte	hi_resp_status;		/* Response messages status	*/
 	word	hi_save_status;		/* Responder node status	*/
	word	hi_save_addr[3];	/* Responder node Enet address	*/
	byte	hi_save_node[ND_STR];	/* Responder node name		*/
	byte	hi_save_rating;		/* Responder service rating	*/
	idbtype *hi_save_idb;		/* Responder interface		*/
#endif	
    } hi_node, HI_VIEW(hi_ptr);

extern hi_node hi_dummy;
#endif
