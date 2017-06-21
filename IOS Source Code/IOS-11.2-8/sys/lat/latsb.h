/* $Id: latsb.h,v 3.2 1995/11/17 17:34:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latsb.h,v $
 *------------------------------------------------------------------
 * $Log: latsb.h,v $
 * Revision 3.2  1995/11/17  17:34:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    latsb		define lat engine session block

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef LATSB_H
#define LATSB_H         0		/* Indicate file has been read  */

/*----------------------------------------------------------------------
    Define the signals to the environment level (used by sx_signal()).
 -----------------------------------------------------------------------*/
#define SX_DATA_B_SET	1		/* Change remote configuration	*/
#define SX_DATA_B_REP	2		/* Report local configuration	*/

#define SX_FLUSH	3		/* Flush seen from remote	*/
#define SX_FORCE_XON	4		/* Force XON seen from remote	*/

/*----------------------------------------------------------------------
    Define the signals to the environment level (used by sx_session()).
 -----------------------------------------------------------------------*/
#define SX_AS_ACCESS	1		/* Session starting (service)	*/
#define SX_HI_ACCESS	2		/* Session starting (HIC)	*/
#define SX_CONNECT	3		/* Session started		*/
#define SX_REJECT	4		/* Session rejected		*/
#define SX_STOP		5		/* Stop session			*/
#define SX_CIRCUIT	6		/* Circuit deleted (timeout)	*/
#define SX_TIMEOUT	7		/* Solicit attempt timed out	*/
#define SX_RESOURCE	8		/* Insufficient resources	*/
#define SX_CONNECT_HIC	9		/* Session starting (HIC)	*/

/*----------------------------------------------------------------------
    Define the session flags.
 -----------------------------------------------------------------------*/
#define	SB_DATA_A	0x0001		/* Send DATA_A to remote	*/
#define SB_DATA_B	0x0002		/* Send DATA_B to remote	*/

#define SB_FORCE_XON	0x0004		/* Send force XON to remote	*/
#define SB_FLUSH	0x0008		/* Send flush  to remote	*/
#define	SB_CREDITS	0x0010		/* Send credits to remote	*/

#define SB_DELAY_A	0x0080		/* Set DATA_A after DATA_B	*/

#define SB_NODE		0x0100		/* Session to specific node	*/
#define SB_DEL_NONE	0x0200		/* Delete w/ no   notification	*/
#define SB_DEL_STOP	0x0400		/* Delete w/ stop notification	*/
#define SB_HIC		0x0800		/* HIC session			*/

#define SB_STALL	0x1000		/* Stall slot processing	*/
#define	SB_STOP		0x2000		/* Send STOP			*/
#define	SB_START	0x4000		/* Send START			*/
#define SB_REJECT	0x8000		/* Send REJECT			*/

#define SB_BLOCK	(SB_STALL | SB_STOP | SB_START | SB_REJECT)

/*----------------------------------------------------------------------
    Define the DATA-B slot types
 -----------------------------------------------------------------------*/
#define	SB_DATA_B_UNK	0		/* DATA-B slot type unknown	*/
#define SB_DATA_B_SET	1		/* Change remote's configuration*/
#define	SB_DATA_B_REP	2		/* Report local's configuration	*/

/*----------------------------------------------------------------------
    Define the DATA-B modes
 -----------------------------------------------------------------------*/
#define	SB_INTERACTIVE	0		/* Interactive session		*/
#define SB_PASSALL	1		/* Passall  mode		*/
#define SB_PASSTHRU	2		/* Passthru mode		*/

/*----------------------------------------------------------------------
    Define the session block.
 -----------------------------------------------------------------------*/
    typedef struct sb
    {
	struct sb SB_VIEW(sb_link[2]);	/* Link to next session (SID)	*/
	struct sb SB_VIEW(sb_slot[2]);	/* Link to next session (slot)	*/
	struct cb CB_VIEW(sb_cb);	/* Pointer to circuit block	*/
	struct ob OB_VIEW(sb_ob);	/* Pointer tp object  block	*/
	sid_type sb_sid;		/* Session  identification	*/
	
#if CME_SLAT
	cid_type sb_cid;		/* CM identifier		*/
#endif	
	word	sb_flags;		/* Session flags		*/
	word	sb_req_no;		/* HIC entry identification	*/
	byte	sb_rmt_ses_id;		/* Remote session id 		*/
	byte	sb_lcl_ses_id;		/* Local  session id 		*/
	byte	sb_rmt_credits;		/* Credits extended from remote	*/
	byte	sb_lcl_credits;		/* Credits to extend to remote	*/
    	byte	sb_ext_credits;		/* Credits extended  to remote	*/
	byte	sb_stop_reason;		/* Stop reason code		*/
	byte	sb_max_data;		/* Maximum DATA-A slot size	*/
	byte	sb_max_attn;		/* Maximum ATTENTION slot size	*/

	byte	sb_data_b_type;		/* Type of DATA-B to issue	*/
	data_b	sb_rmt_data;		/* Current remote configuration	*/
	data_b	sb_tmp_data;		/* Requested remote config.	*/
	byte	sb_data_b_lcl_mode;	/* Session mode (local)		*/
	byte	sb_data_b_rmt_mode;	/* Session mode	(remote)	*/
	byte	sb_lcl_data_b_status;	/* Character status (local)	*/
	byte	sb_lcl_data_b_status_char;/* Character value (local)	*/
	byte	sb_rmt_data_b_status;	/* Character status (remote)	*/
	byte	sb_rmt_data_b_status_char;/* Character value (remote)	*/

	byte	sb_name[SV_STR];	/* Remote service/port name	*/
	byte	sb_port[OB_STR];	/* Remote port name		*/
	byte	sb_password[SV_STR];	/* Remote password string	*/
	byte	sb_sc;			/* Service class		*/
    } sb_node, SB_VIEW(sb_ptr);
#endif
