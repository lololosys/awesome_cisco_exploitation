/* $Id: v120.h,v 3.3.62.1 1996/07/28 07:33:17 irfan Exp $
 * $Source: /release/112/cvs/Xsys/wan/v120.h,v $
 *------------------------------------------------------------------
 * v120.h - support for V.120 protocol
 *
 * July, 1995, Bill May
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: v120.h,v $
 * Revision 3.3.62.1  1996/07/28  07:33:17  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.3  1995/11/17  18:05:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:51:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  23:30:26  wmay
 * Placeholder for CS_Arkansas_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define V120_LLIBYTES 2
#define V120_CONTROLBYTES 2
#define V120_ENCAPBYTES (V120_LLIBYTES + V120_CONTROLBYTES)
#define V120_UNNUMBERED_ENCAPBYTES (V120_LLIBYTES + 1)

#define V120_K_VALUE	7
#define V120_N200	3
#define V120_MAX_SEQ	128
#define V120_MAX_IFRAME_SIZE 260

/* first byte of V120 header */
#define V120_LLI0_MASK	0xfc
#define V120_CR_MASK	0x02

/* 2nd byte of V120 header */
#define V120_LLI1_MASK	0xfe

/* Initial (unassigned) value for LLI (13 bits) = 256  */
#define V120_LLI_DEFAULT  0x100

#define V120_CONTROL_IFRAME_MASK	0x01
#define V120_CONTROL_MASK	0x03
#define V120_CONTROL_S_FORMAT	0x01
#define V120_CONTROL_U_FORMAT	0x03

/*
 * Number of bytes in a Supervisory and unnumbered packet
 */
#define V120_S_FORMAT_SIZE	4
#define V120_U_FORMAT_SIZE	3

#define V120_SUPERVISORY_PF_MASK 0x01
#define V120_SUPERVISORY_N_MASK 0xfe

/*
 * Supervisory packet values
 */
#define V120_RR_VALUE	0x01
#define V120_RNR_VALUE	0x05
#define V120_REJ_VALUE	0x09

#define V120_UNNUMBERED_PF_MASK	0x10
/*
 * Unnumbered packet values
 * below values are with P/F bit masked out
 */
#define V120_SABME_VALUE	0x6f
#define V120_DM_VALUE		0x0f
#define V120_UI_VALUE		0x03
#define V120_DISC_VALUE		0x43
#define V120_UA_VALUE		0x63
#define V120_FRMR_VALUE		0x87

#define V120_FRMR_F_VALUE	(V120_FRMR_VALUE | V120_UNNUMBERED_PF_MASK)
#define V120_FRMR_W_MASK	0x01
#define V120_FRMR_X_MASK	0x02
#define V120_FRMR_Y_MASK	0x04
#define V120_FRMR_Z_MASK	0x08

/*
 * V120 header definitions
 */
#define V120_HEAD_EXT	0x80
#define V120_HEAD_BREAK	0x40
#define V120_HEAD_C2	0x08
#define V120_HEAD_C1	0x04
#define V120_HEAD_BF	0x03

#define V120_HEAD_EXT_DR	0x40
#define V120_HEAD_EXT_SR	0x20
#define V120_HEAD_EXT_RR	0x10
#define V120_HEAD_EXT_ALL (V120_HEAD_EXT_DR | V120_HEAD_EXT_SR | \
				 V120_HEAD_EXT_RR)
/*
 * The v120_packet_type enum will classify each packet for
 * easy state machine handling.  We use these numbers as an offset
 * for the debug v120 packet display
 */
enum v120_packet_type {
    V120_IFRAME	= 0,		/* Information Frame */
    V120_RR	= 1,		/* Receiver Ready */
    V120_RNR	= 2,		/* Receiver Not Ready */
    V120_REJ	= 3,		/* Reject */
    V120_SABME	= 4,		/* Set Async Balanced Mode Extended */
    V120_DM	= 5,		/* Disconnected Mode */
    V120_UI	= 6,		/* Unnumbered Information */
    V120_DISC	= 7,		/* Disconnect */
    V120_UA	= 8,		/* Unnumbered Acknowledgement */
    V120_FRMR	= 9,		/* Frame Reject */
    V120_BAD	= 10,		/* Bad frame received */
};
	
/*
 * And here are the possible states.  The TEI_ASSIGNED state is a
 * legacy from q.921.  It is sometimes called the multiframe not
 * established state in the V.120 spec
 */
enum v120_states {
    V120_TEI_ASSIGNED_STATE,
    V120_AWAITING_ESTABLISHMENT_STATE,
    V120_ESTABLISHED_STATE,
    V120_TIMER_RECOVERY_STATE,
    V120_AWAITING_RELEASE_STATE,
};

/*
 * v120_save_type - structure for variables changed in the hwidb.
 * Be nice and use the same names as in the hwidb
 */
typedef struct v120_save_type {
    encapstype	enctype;
    iqueue_t	iqueue;
    parse_packet_t	parse_packet;
    vencap_t	vencap;
    lineaction_t lineaction;
    linestate_t linestate;
    periodic_t	periodic;
} v120_save_type;

struct v120_info_type_ {
    struct v120_info_type_ *next;	/* pointer to next v120 in box */
    hwidbtype  	*hwptr;			/* pointer to controlling hwidb */
    tt_soc	*v120_vty;		/* pointer to vty for this interface */
    sys_timestamp	t200;		/* T200 timer */
    sys_timestamp	t203;		/* t203 timer */
    v120_save_type	save;		/* Place to store hooked variables */
    queuetype	unackQ;			/* Unacknowledgement queue - for tx */
    ulong lli;				/* lli for this V.120 session */
    boolean	multi_frame;		/* TRUE to send using IFRAMES */
    boolean	header_extension;	/* TRUE to send 2nd byte of header */
    boolean	remote_busy;		/* TRUE if remote sent an RNR	*/
    boolean	reject_exception;	/* Reject exception flag for q921 */
    boolean	ack_pending;		/* Ack pending flag for q921 */
    boolean	I_called_death;		/* True if I called death */
    boolean	params_from_call;
    enum v120_states	state;		/* state from above */
    ushort	header_bytes;
    uchar	va;			/* Acknowledge state variable V(A) */
    uchar	vs;			/* Send state variable V(S) */
    uchar	vr;			/* Receive State variable V(R) */
    uchar	rc;			/* Retransmission Count */
};


/*
 * v120_packet_is_command
 * returns a TRUE if the C/R field is set to 1
 */
static inline boolean v120_packet_is_command (paktype *pak)
{
    return(!(*pak->datagramstart & V120_CR_MASK));
}

/*
 * v120_get_nr
 * This will return the N(R) value for either supervisory or iframes
 */
static inline uchar v120_get_nr (paktype *pak)
{
    uchar temp;

    temp = *(pak->datagramstart + V120_LLIBYTES + 1) & V120_SUPERVISORY_N_MASK;
    return(temp >> 1);
}

/*
 * v120_get_ns
 * returns the N(S) value from an I frame
 */
static inline uchar v120_get_ns (paktype *pak)
{
    uchar temp;
    temp = *(pak->datagramstart + V120_LLIBYTES);
    temp &= V120_SUPERVISORY_N_MASK;
    return(temp >> 1);
}

/*
 * v120_iframe_pf, v120_supervisory_pf
 * returns the P/F bit value from an I frame or a supervisory frame
 */
static inline uchar v120_iframe_pf (paktype *pak)
{
    return((*(pak->datagramstart + V120_LLIBYTES + 1) &
	    V120_SUPERVISORY_PF_MASK));
}

#define v120_supervisory_pf v120_iframe_pf

/*
 * v120_unnumbered_pf
 * returns the value of the unnumbered P/F bit
 */
static inline uchar v120_unnumbered_pf (paktype *pak)
{
    return(*(pak->datagramstart + V120_LLIBYTES) & V120_UNNUMBERED_PF_MASK);
}

/*
 * v120_lli_to_num
 * Will translate the lli number in the packet to a real number
 */
static inline ulong v120_lli_to_num (uchar *ptr)
{
    ulong temp;
    temp = (*ptr & V120_LLI0_MASK) << 5;
    temp |= ((*(ptr + 1) & V120_LLI0_MASK) >> 2);

    return temp;
}

/*
 * v120_write_lli
 * This routine will write the 2 header bytes that contain the
 * lli and C/R data.  pak->datagramstart should point to where the
 * lli will go.
 */
static inline void v120_write_lli (paktype *pak, v120_info_type *v120,
				   boolean response)
{
    pak->datagramstart[0] = ((v120->lli >> 5) & ~0x03) |
	(response ? V120_CR_MASK : 0x0);
    pak->datagramstart[1] = (v120->lli << 1) | 0x01;
}

/*
 * v120_write_supervisory_header
 * Given a pak structure, it will write a supervisory frame header
 * at pak->datagramstart
 * type is the supervisory message header to write.
 * response is TRUE if a response is to be sent, FALSE for a command
 * pf is the value of the Supervisory PF bit
 */
static inline void v120_write_supervisory_header (paktype *pak,
						  v120_info_type *v120,
						  uchar type,
						  boolean response,
						  uchar pf)
{
    v120_write_lli(pak, v120, response);
    pak->datagramstart[2] = type;
    pak->datagramstart[3] = (v120->vr << 1) | pf;
    pak->datagramsize = V120_S_FORMAT_SIZE;
}
/*
 * v120_write_unnumbered_header
 * Given a pak structure, it will write a unnumbered frame header
 * at pak->datagramstart
 * type is the unnumbered message header to write.  It should have
 * the pf bit already set or cleared.
 * response is TRUE if a response is to be sent, FALSE for a command
 */
static inline void v120_write_unnumbered_header (paktype *pak,
						 v120_info_type *v120,
						 uchar type,
						 boolean response)
{
    v120_write_lli(pak, v120, response);
    pak->datagramstart[2] = type;

    pak->datagramsize = V120_U_FORMAT_SIZE;
}

/*
 * v120_increment_sequence
 * This will return the next sequence number
 */
static inline uchar v120_increment_sequence (uchar seq)
{
    seq++;
    if (seq >= V120_MAX_SEQ)
	return(0);
    return(seq);
}

/*
 * Timer routines.  These routines will start/stop the t200 and t203
 * counters
 */
static inline void v120_stop_t200 (v120_info_type *v120)
{
    TIMER_STOP(v120->t200);
}
static inline void v120_start_t200 (v120_info_type *v120)
{
    TIMER_START(v120->t200, ONESEC);
}
static inline void v120_stop_t203 (v120_info_type *v120)
{
    TIMER_STOP(v120->t200);
}
static inline void v120_start_t203 (v120_info_type *v120)
{
    TIMER_START(v120->t203, (10 * ONESEC));
}

static inline boolean v120_t200_running (v120_info_type *v120)
{
    return(TIMER_RUNNING(v120->t200));
}

/* from v120.c */
extern boolean v120_send_data (paktype *, v120_info_type *);

/* from v120_vty.c */
extern void v120_data_to_vty(v120_info_type *, paktype *);
extern void v120_check_vty_txq(v120_info_type *);
extern boolean v120_init_vty(v120_info_type *, v120_parameters_type *);
extern void v120_stop_vty(v120_info_type *);
extern void v120_init_vty_vector(void);
