/* $Id: latco.h,v 3.2 1995/11/17 17:33:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latco.h,v $
 *------------------------------------------------------------------
 * $Log: latco.h,v $
 * Revision 3.2  1995/11/17  17:33:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    latco		define counters structure
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/
#ifndef LATCO_H
#define LATCO_H		0		/* Indicate file has been read	*/

/*----------------------------------------------------------------------
    Define the message error classes.
 -----------------------------------------------------------------------*/
#define MSG_OTHER	0x0000		/* Other errors			*/
#define CIR_S_ERROR	0x0001		/* Circuit start message	*/
#define CIR_R_ERROR	0x0002		/* Circuit run   message	*/
#define CIR_H_ERROR	0x0003		/* Circuit halt  message	*/
#define SVC_A_ERROR	0x0004		/* Service annoucement message	*/
#define HIC_SI_ERROR	0x0005		/* HIC solicit message 		*/
#define HIC_RI_ERROR	0x0006		/* HIC reponse message		*/
#define HIC_CMD_ERROR	0x0007		/* HIC command message 		*/
#define HIC_STS_ERROR	0x0008		/* HIC status  message		*/

/*-----------------------------------------------------------------------
    Define generic message field errors.
 -----------------------------------------------------------------------*/
#define	MSG_LENGTH	0x0000		/* Illegal message length	*/
#define FIELD_SLOTS	0x0100		/* Illegal # slots		*/
#define FIELD_CIR_ID	0x0200		/* Illegal circuit ID		*/
#define FIELD_GROUP	0x0300		/* Illegal group field		*/
#define FIELD_NODE	0x0400		/* Illegal node name		*/
#define FIELD_SERVICE	0x0500		/* Illegal service name		*/
#define FIELD_PORT	0x0600		/* Illegal port name		*/
#define FIELD_CLASS	0x0700		/* Illegal service class	*/

/*-----------------------------------------------------------------------
    Define errors specific to MSG_OTHER.
 -----------------------------------------------------------------------*/
#define FIELD_TYPE	0x0f00		/* Illegal message type		*/

/*-----------------------------------------------------------------------
    Define errors specific to CIR_S_ERROR.
 -----------------------------------------------------------------------*/
#define FIELD_VC_TIMER	0x0f00		/* Illegal circuit timer	*/

/*-----------------------------------------------------------------------
    Define errors specific to CIR_R_ERROR
 -----------------------------------------------------------------------*/
#define ESLOT_TYPE	0x0f00		/* Illegal slot, slot type	*/
#define ESLOT_DST_ID	0x0e00		/* Illegal slot, destination	*/
#define ESLOT_SRC_ID	0x0d00		/* Illegal slot, source		*/
#define ESLOT_CREDITS	0x0b00		/* Illegal slot, no credit	*/

/*----------------------------------------------------------------------
    Define the host/node common counters as per protocol specification.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	count	co_seconds;		/* Seconds since zeroed		*/
	count	co_msgs_recv;		/* Total run messages received	*/
	count	co_msgs_xmit;		/* Total run messages transmit	*/
	count	co_slot_recv;		/* Total slots received		*/
	count	co_slot_xmit;		/* Total slots transmitted	*/
	count	co_byte_recv;		/* Total bytes received		*/
	count	co_byte_xmit;		/* Total bytes transmitted	*/
	count	co_duplicates;		/* Total duplicate messages	*/
	count	co_retransmit;		/* Total retransmitted messages	*/
	count	co_msgs_bad;		/* Total bad circuit messages	*/
	count	co_slot_bad;		/* Total bad circuit slots	*/
	count	co_solicit_accept;	/* Total accepted host-initiate	*/
	count	co_solicit_reject;	/* Total rejected host-initiate	*/
	count	co_multiple_node;	/* Total multiple nodes seen	*/
    } co_node;
    
/*----------------------------------------------------------------------
    Define the host-only counters.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	count	ch_total_circuits;	/* Total circuits created	*/
	count	ch_total_sessions;	/* Total sessions created	*/
	count	ch_total_recv;		/* Total frames received	*/
	count	ch_total_xmit;		/* Total frames transmited	*/
	count	ch_total_illegal;	/* Total illegal frames		*/
	count	ch_total_timeouts;	/* Total circuit timeouts	*/
	count	ch_svc_xmit;		/* Total service messages xmit.	*/
	count	ch_svc_recv;		/* Total service messages recv.	*/
	count	ch_svc_used;		/* Total service messages used	*/
	count	ch_svc_discard;		/* Total service msgs. discarded*/
	word	ch_err_source[3];	/* Last error source address	*/
	idbtype *ch_idb;		/* Last error source interface  */
	word	ch_err_masks[9];	/* Message error masks		*/
    } ch_node;

/*----------------------------------------------------------------------
    Define counter macros/functions.
 -----------------------------------------------------------------------*/
#if LE_HS_STATS && LE_ND_STATS

#define count_1(x)   \
    if (hs->hs_stats.x != (count)~0L) hs->hs_stats.x++; \
    if (nd->nd_stats.x != (count)~0L) nd->nd_stats.x++

#define count_h(x)   \
    if (hs->hs_count.x != (count)~0L) hs->hs_count.x++

#define count_x(x,y) \
    if (hs->hs_stats.x < (count)~0xffL) hs->hs_stats.x += y; \
    if (nd->nd_stats.x < (count)~0xffL) nd->nd_stats.x += y

#endif

#if LE_HS_STATS && !LE_ND_STATS

#define count_1(x)   \
    if (hs->hs_stats.x != (count)~0L) hs->hs_stats.x++

#define count_h(x)   \
    if (hs->hs_count.x != (count)~0L) hs->hs_count.x++

#define count_x(x,y) \
    if (hs->hs_stats.x < (count)~0xffL) hs->hs_stats.x += y

#endif

#if !LE_HS_STATS && !LE_ND_STATS

#define count_1(x)
#define count_h(x)
#define count_x(x,y)

#endif

#endif
