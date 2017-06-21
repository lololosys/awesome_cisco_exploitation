/* $Id: arap_fcrbis.h,v 3.2 1995/11/17 08:38:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_fcrbis.h,v $
 *------------------------------------------------------------------
 * Declarations
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: arap_fcrbis.h,v $
 * Revision 3.2  1995/11/17  08:38:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:04:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define P(args) args

/* negotiated parameters in XID */
#define PARM_GROUP_ID		0xf0	/* ISB 8885, Addendum 3 */
#define PARM_PARM_ID_V42	0x00
#define PARM_PARM_ID_P0		0x01
#define PARM_PARM_ID_P1		0x02
#define PARM_PARM_ID_P2		0x03

/* control code words (compression mode) */
#define	CCW_ETM		0x00	/* enter transparent mode */
#define	CCW_FLUSH	0x01	/* flush data */
#define	CCW_STEPUP	0x02	/* stepup code word size */

/* command code words (transparent mode) */
#define	CCW_ECM		0x00	/* enter compression mode */
#define	CCW_EID		0x01	/* escape character in data */
#define	CCW_RESET	0x02	/* force reinitialization */

/* escape char cycling */
#define	ESCAPE_CYCLE	51

/*
 * v.42bis dictionary node
 */
typedef struct {
    unsigned char byte;		/* character */
    unsigned short parent;		/* ptr to parent node */
    unsigned short node;		/* chain of nodes w/same parent */
    unsigned short leaf;		/* chain of leafs w/same parent */
} node_t;

/*
 * v.42bis state block
 */
typedef struct {
    /* connection */
    void *connection;

    /* values from spec */
    short n1;			/* maximum codeword size (bits) */
    short n2;			/* total number of codewords */
#define	N3	8		/* character size (bits) */
#define	N4	256		/* characters in alphabet (2^n3) */
#define	N5	(N4+N6)		/* index # of 1st entry to store a string */
#define	N6	3		/* # of control words */
    unsigned char n7;			/* maximum string length */

    /* dictionary */
#define	CODES		2048	/* max # of codewords */
#define	LOG2_CODES	11	/* log2(CODES) (max # of codeword bits) */
    node_t dictionary[CODES];
#define DICT(i) (&state->dictionary[i])
#define CODE(n) ((n) - state->dictionary)

    unsigned short c1;			/* next dictionary entry */
    unsigned char c2;			/* current codeword size */
    unsigned short c3;			/* threshhold for codeword size change */

    unsigned char string_size;		/* # bytes in string so far */
    unsigned short last_match;		/* index of last match of "string" */
    unsigned short last_new;		/* index of last new node */
    unsigned short last_decode;
    unsigned char last_decode_size;

    unsigned char escape;		/* escape character */
    boolean transparent;	/* are we in transparent mode? */
    boolean decode_only;	/* are we decode side ? */

    unsigned char dump_indent;		/* indentation dumping dict. tree */
    boolean debug_encode_bytes;
    boolean debug_encode;
    boolean debug_decode_bytes;
    boolean debug_decode;
    boolean debug_flow;

    unsigned char word_size;		/* local # bits to decode into */
    boolean exception_next;	/* do exception processing; next ch */
    boolean escaped;		/* have we just gotten an escape? */
    boolean just_flushed;	/* did we just flush? */
    boolean dict_full;		/* is dictionary full? */
    boolean error;		/* has an error been detected? */

    /* decode bytes->codeword state */
    unsigned long bits_waiting;	/* decode holder */
    unsigned char bits_remaining;	/* # bits waiting in holder now */

    unsigned char *input_ptr;
    unsigned short input_size;

    /* encode codeword->bytes state */
    unsigned long bits_acc;		/* encode accumulator */
    unsigned char bits_used;		/* # bits packed in acc now */

    unsigned char *output_buffer;	/* ptr to work buffer */
    unsigned char *output_ptr;		/* current ptr into buffer */
    unsigned short output_size;	/* current work size */
    unsigned short output_max;		/* size of work buffer */
#define PUT(ch) \
    if (state->output_size < state->output_max) \
    { *state->output_ptr++ = (ch); state->output_size++; } \
	else {  char error_string[50]; \
		sprintf(error_string, "\nsize %d, max %d", state->output_size, \
			state->output_max);\
		v42bis_c_error(state, error_string); }

    /* i/o */
    void *push_context;
    void (*push_func) P((void *, u_char *, int));

    /* statistics for compressibility */
    unsigned short bytes_in;		/* total bytes input to compress */
    unsigned short bytes_out;		/* total bytes output from compress */
    long        bits_out_other_mode;    /* output if we were in other mode */
    long        bits_out_this_mode;     /* since last transition */
    ushort      bytes_since_last_check; /* since last compression test */
#define bits_out_if_compressed          bits_out_other_mode
#define bits_out_while_compressed       bits_out_this_mode
#define bits_out_if_transparent         bits_out_other_mode
#define bits_out_while_transparent      bits_out_this_mode
} v42bis_t;

/*
  define hysteresis for compressed/transparent mode switch

  WINDOW_FULL defines how many bits we look at
  WINDOW_MIN_BITS is the min bits of difference required for a change
*/
#define WINDOW_FULL(n)          (n & 0xfffffc00)        /* 1024 bits */
#define WINDOW_MIN_BITS         16*N3                   /* 128 bits */
#define WINDOW_CHECK_BYTES      32                      /* check every 32 */

#ifdef DEBUG_ENABLED
#define V_FLOW(s)	if (state->debug_flow) logf s;

#define EN_DEBUG(s)	\
    if (state->debug_encode) { \
	logf_prefix(state->decode_only ? "decode: " : "encode: "); \
	logf s; }
#define EN_S_DEBUG(s)	\
    if (state->debug_encode > 1) { \
	logf_prefix(state->decode_only ? "decode: " : "encode: "); \
	logf s; }
#define EN_DEBUG_ON	(state->debug_encode)

#define DE_DEBUG(s)	\
    if (state->debug_decode) { logf_prefix("decode: "); logf s; }
#define DE_DEBUG_ON	(state->debug_decode)

#define E_DEBUG(s)	if (state->debug_encode_bytes) logf s;
#define D_DEBUG(s)	if (state->debug_decode_bytes) logf s;
#else
#define V_FLOW(s)		/* #s */
#define EN_DEBUG(s)		/* #s */
#define EN_S_DEBUG(s)
#define DE_DEBUG(s)		/* #s */
#define E_DEBUG(s)		/* #s */
#define D_DEBUG(s)		/* #s */
#define EN_DEBUG_ON	FALSE
#define DE_DEBUG_ON	FALSE
#endif

/*
 * mybuffer -- Tim Kolar's output technique
 */
typedef struct mybuffer_ {
    u_int poll;
    u_char buf[1024 * 8 + 128];
    int buflen;
} mybuffer;


/*
 * v42bis connection type
 */
typedef struct {
    /* negotiated options */
    unsigned char neg_p0;		/* negotiated value of p0 */
    unsigned short neg_p1;		/* negotiated value of p1 */
    unsigned char neg_p2;		/* negotiated value of p2 */

    unsigned char default_p0;		/* default value of p0 */
    unsigned short default_p1;		/* default value of p1 */
#define MIN_P1	512
#define DEF_P1	2048
    unsigned short default_p2;		/* default value of p2 */
#define MIN_P2	6
    /* #define DEF_P2	8 */
#define DEF_P2	250
#define MAX_P2	250

    boolean compress_init_resp;	/* comp. in initiator->responder dir */
    boolean compress_resp_init;	/* comp. in responder->initiator dir */
    boolean got_p0;		/* got negitated XID options */
    boolean got_p1;
    boolean got_p2;
    boolean got_unknown_p;	/* got unknown option */

    v42bis_t *encode;		/* encode state */
    v42bis_t *decode;		/* decode state */
    mybuffer *outbuffer;	/* buffer that output goes to */
    paktype *bufpak;
} v42bis_connection_t;

/* turn a "state" into a connection */
#define CONN(s)	((v42bis_connection_t *)(s)->connection)


int fcr_do_v42(v42bis_connection_t *, u_char *, int, queuetype *, int);
int fcr_undo_v42(v42bis_connection_t *, u_char *, int, queuetype *, int );
v42bis_connection_t *fcr_init_v42bis(void);
void fcr_destroy_v42bis(v42bis_connection_t *, int);
void v42bis_connection_init(v42bis_connection_t *);
void v42bis_connection_init_buffers(v42bis_connection_t *,
				     u_char *, int, uchar *, int);
void v42bis_connection_init_push(v42bis_connection_t *, void *,
				  void (*) (void *, u_char *, int),
				  void (*) (void *, u_char *, int));
void v42bis_encode_buffer(v42bis_t *, u_char *, int);
void v42bis_encode_flush(v42bis_t *);
void v42bis_push(v42bis_t *);
void v42bis_decode_buffer(v42bis_t *, u_char *, int);
void v42bis_disconnect(v42bis_t *, char *);
unsigned short v42bis_decode_codeword(v42bis_t *, unsigned char);
