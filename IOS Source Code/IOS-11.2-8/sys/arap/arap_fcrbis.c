/* $Id: arap_fcrbis.c,v 3.2 1995/11/17 08:38:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_fcrbis.c,v $
 *------------------------------------------------------------------
 * V.42bis compress/decompression
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * v42bis compress/decompression
 * 
 * This is an attempt to code v42bis compression and decompresson
 * from the spec.
 * I must have been crazy when I decided to do this.
 * 
 * from the CCITT V.42bis Data compression proceedures for data circuit
 * terminating equipment (DCE) using error correction proceedures.
 * 
 * Recommendation V.42bis, Geneva 1990
 * 
 *------------------------------------------------------------------
 * $Log: arap_fcrbis.c,v $
 * Revision 3.2  1995/11/17  08:38:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:04:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define logf buginf
#define logf_prefix buginf
/* #define DEBUG_ENABLED */
/* #define DEBUG */
/* #define NEVER_SEND_COMPRESSED */
#define SIMPLE_EXPANSION_TEST
/*#define IN_ORDER*/
#define WORD16

#include "master.h"
#include "arap_mnpllvl.h"
#include "arap_mnpdat.h"
#include "arap_public.h"


/* bitmaps[# of bits] */
static ushort bit_masks[16] = {
    0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f,
    0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff
    };

/* local routines */
int decode_xid_params P((v42bis_t *state, uchar *parms, int len));
void v42bis_encode_codeword P((v42bis_t *state, ushort value));
void v42bis_c_error P((v42bis_t *state, char *msg));
void v42bis_transition_to_compressed P((v42bis_t *state));
void v42bis_transition_to_transparent P((v42bis_t *state));
int mnpsend(mnp_t *, uchar *, int);
int mnpreceive(mnp_t *, uchar *, int);



char *show_char(uchar ch)
{
    static char dec[20];

    if (' ' <= ch && ch <= '~') {
	dec[0] = ch;
	dec[1] = 0;
	return dec;
    }

    sprintf(dec, "0x%02x", ch);
    return dec;
}

/*
  decode_xid_params()

  decode compression negotiation packet as per V42bis spec.

  note: this is not used (there is code in the mnp LR routines to do
  this), but is included for completeness.
*/

int decode_xid_params(v42bis_t *state, uchar *params, int len)
{
    int group_len;
    v42bis_connection_t *conn = CONN(state);

    /* must be at least id + len */
    if (len < 2)
	return 0;

    /* ISB 8885, Addendum 3 */
    if (*params != PARM_GROUP_ID)
	return 0;

    /* msb followed by lsb */
    group_len = (params[1] << 8) | params[2];

    if (group_len <= 0)
	return 0;

    /* advance */
    params += 3;

    /* parse the "table A-1/V.42bis" XID parameters */

    while (group_len > 1) {
	uchar param_id, param_len;

	param_id = *params++;
	param_len = *params++;

	switch (param_id) {
	  case PARM_PARM_ID_V42:
	    if (param_len != 3)
		return 0;

	    if (params[0] != 'V' || params[1] != '4' || params[2] != '2')
		return 0;
	    break;

	  case PARM_PARM_ID_P0:
	    if (param_len != 1)
		return 0;

	    conn->compress_init_resp = 0;
	    conn->compress_resp_init = 0;

	    if (params[0] & 0x01)
		conn->compress_init_resp++;

	    if (params[0] & 0x02)
		conn->compress_resp_init++;

	    conn->neg_p0 = params[0];
	    conn->got_p0++;
	    break;

	  case PARM_PARM_ID_P1:
	    if (param_len != 2)
		return 0;

	    conn->neg_p1 = (params[0] << 8) | params[1];
	    conn->got_p1++;

	    /* section 5.1 */
	    if (conn->neg_p1 < MIN_P1) {
		v42bis_disconnect(state, "negotiated p1 below spec");
		return 0;
	    }

	    if (conn->neg_p1 > CODES) {
		v42bis_disconnect(state, "negotiated p1 too big");
		return 0;
	    }

	    if (conn->neg_p1 > conn->default_p1)
		conn->neg_p1 = conn->default_p1;
	    break;

	  case PARM_PARM_ID_P2:
	    if (param_len != 2)
		return 0;

	    conn->neg_p2 = params[0];
	    conn->got_p2++;

	    /* section 5.1 */
	    if (conn->neg_p2 < MIN_P2 || conn->neg_p2 > MAX_P2) {
		v42bis_disconnect(state, "negotiated p2 outside spec");
		return 0;
	    }

	    if (conn->neg_p2 > conn->default_p2)
		conn->neg_p2 = conn->default_p2;
	    break;
	    
	  default:
	    conn->got_unknown_p++;
	    break;
	}

	params += param_len;
	group_len -= param_len;
    }

    /* humm... unused bytes */
    if (group_len != 0)
	return 0;

    return 1;
}

/*
  v42bis_push()

  perform PUSH on output stream.  accumlated bytes are pushed
  out.
*/


void v42bis_push(v42bis_t *state)
{
    V_FLOW(("v42bis_push(%lx)", state));

#ifdef DEBUG_PUSH
    logf("push %d bytes", state->output_size);
#endif

    state->bytes_out += state->output_size;

    if (state->push_func) {
	(*state->push_func)(state->push_context,
			    state->output_buffer, state->output_size);

	state->output_ptr = state->output_buffer;
	state->output_size = 0;
    }
}

/*
  v42bis_init_dictionary()

  init dictionary in accordance with section 6.2 and 7.2
  
  this is used at init time and in response to a CCW_RESET
*/

void v42bis_init_dictionary(v42bis_t *state)
{
    int i;
    node_t *n;

    /* initialize dictionary tree */
    for (i = 0, n = state->dictionary; i < state->n2; i++, n++) {
	n->byte = 0;
	n->parent = 0;
	n->node = 0;
	n->leaf = 0;
    }

    /* section 6.2 */
    state->c1 = N5;	/* next codeword */
    state->c2 = N3 + 1;	/* codeword size (bits) */
    state->c3 = N4 * 2;	/* threshold */

    state->transparent = TRUE;
    state->escape = 0;
    state->escaped = FALSE;
    state->exception_next = FALSE;

    /* initialize searching  */
    state->last_match = 0;
    state->last_new = 0;
    state->string_size = 0;
}

/*
  v42bis_init()

  implements C-INIT semantics
*/

void v42bis_init(v42bis_t *state)
{
    V_FLOW(("v42bis_init()"));

    /* our defaults */
    state->n1 = LOG2_CODES;		/* max codeword size (bits) */
    state->n2 = CONN(state)->neg_p1;	/* total # of codewords */
    state->n7 = CONN(state)->neg_p2;	/* max string length */

    /* init dictionary */
    v42bis_init_dictionary(state);

    /* initialize encode/decode */
    state->bits_acc = 0;
    state->bits_used = 0;
    state->word_size = 8*sizeof(unsigned short);

    state->bits_waiting = 0;
    state->bits_remaining = 0;
    state->error = FALSE;
}

#ifdef DEBUG_ENABLED
/*
  itobits()

  turn an integer bitfield into an ascii representation (i.e. "01010101")
*/

char *itobits(ushort word, int bits)
{
    static char buf[33];
    int i;

    if (bits > 32) bits = 32;

    for (i = bits-1; i >= 0; i--)
	buf[(bits-1)-i] = word & (1 << i) ? '1' : '0';

    buf[bits] = 0;
    return buf;
}
#endif

/*
  v42bis_decode_codeword()

  decode n-bit codewords from a bytesteam.
*/

ushort v42bis_decode_codeword(v42bis_t *state, uchar value)
{
    register ushort codeword;

    V_FLOW(("v42bis_decode_codeword(%x) c2=%d", value, state->c2));

    /* simple case */
    if (state->c2 == 8 || state->transparent)
	return value;

    /* not-so-simple case */
    D_DEBUG(("before: waiting %06x, bits_remaining %d",
	     state->bits_waiting, state->bits_remaining));

    /* add in these 8 bits */
    state->bits_waiting |= ((ulong)value) << state->bits_remaining;
    state->bits_remaining += 8;

    /* do we have a codeword ? */
    if (state->bits_remaining >= state->c2) {
	D_DEBUG(("input %04x %s",
		 state->bits_waiting & bit_masks[state->c2],
		 itobits(state->bits_waiting & bit_masks[state->c2],
			 state->c2)));

	codeword = state->bits_waiting & bit_masks[state->c2];

	state->bits_waiting >>= state->c2;
	state->bits_remaining -= state->c2;

	D_DEBUG(("after: waiting %04x, bits_remaining %d (data)",
		 state->bits_waiting, state->bits_remaining));

	return codeword;
    }

    D_DEBUG(("after: waiting %04x, bits_remaining %d (no data)",
	     state->bits_waiting, state->bits_remaining));

    return -1;
}

/*
  v42bis_decode_codeword_flush()

  "flush" decoding of codewords, returning the last codeword
*/

ushort v42bis_decode_codeword_flush(v42bis_t *state)
{
    ushort codeword = -1;

    if (state->bits_remaining)
	codeword = state->bits_waiting & bit_masks[state->c2];

    state->bits_waiting = 0;
    state->bits_remaining = 0;

    return codeword;
}

/*
  v42bis_encode_codeword()

  encode n-bit codewords into a bytesteam.

  This routine makes use of the fact that the code words will be always
  be smaller than 16 bits.  An "accumulator" is used with several state
  variables to keep track of how much of the accumulator is in use at
  any given time.

  The code works for wordsizes of 8 and 16 bits.  It is assumed that the
  output is a byte stream.  No assumptions are made about alignment of
  data.

  note: this routine needs to be "flushed" to get out the value left
  in the accumulator.

  jbp@fcr.com 09/13/92, 19:52
*/

void v42bis_encode_codeword(v42bis_t *state, ushort value)
{
    register uchar bits_free, bits_new, bits_residual;

    EN_DEBUG(("v42bis_encode_codeword(%d 0x%x) c2=%d",
	      value, value, state->c2));

    /* simple case */
    if (state->c2 == 8 || state->transparent) {
	E_DEBUG(("put acc %02x %s", value & 0xff, itobits(value & 0xff, 8)));

	PUT(value);
	if (state->transparent)
	    state->bits_out_while_transparent += N3;
	else
	    state->bits_out_while_compressed += N3;
	return;
    }

    state->bits_out_while_compressed += state->c2;

    /* not-so-simple case */
    E_DEBUG(("before: acc %04x, bits_used %d",
	     state->bits_acc, state->bits_used));

    /* place new value in appropriate bit positions */
    state->bits_acc |= ((ulong)value) << state->bits_used;

    /* housekeeping */
    bits_free = state->word_size - state->bits_used;
    bits_new = bits_free < state->c2 ? bits_free : state->c2;
    bits_residual = state->c2 - bits_new;

    E_DEBUG(("bits_free %d, bits_new %d, bits_residual %d",
	     bits_free, bits_new, bits_residual));

#ifdef DEBUG
    if (state->bits_used + bits_new >= 31)
	logf("acc oflo, size %d", state->bits_used + bits_new);
#endif

    /* do we have a full codeword in the accumulator? */
    if (state->bits_used + bits_new == state->word_size) {

	if (state->word_size == 16) {
	    E_DEBUG(("put acc %06x %s",
		     state->bits_acc, itobits(state->bits_acc, 24)));

	    PUT(state->bits_acc);
	    PUT(state->bits_acc >> 8);
	} else {
	    E_DEBUG(("put acc %02x %s",
		     state->bits_acc & 0xff,
		     itobits(state->bits_acc & 0xff, 8)));

	    PUT(state->bits_acc);
	}

	E_DEBUG(("value 0x%x, bits_used %d, acc 0x%x",
		 value, state->bits_used, value >> state->bits_used));

	/* account for left over bits */
	state->bits_acc = value >> (state->c2 - bits_residual);
	state->bits_used = bits_residual;
    } else
	state->bits_used += bits_new;

    E_DEBUG(("after: acc %06x, bits_used %d",
	     state->bits_acc, state->bits_used));
}

/*
  v42bis_encode_codeword_flush()

  flush partial assembly of codeword into 16 bit accumulator
*/

void v42bis_encode_codeword_flush(v42bis_t *state)
{
    V_FLOW(("v42bis_encode_codeword_flush() bits_used %d", state->bits_used));

    if (state->bits_used) {
	E_DEBUG(("put acc (flush) %02x %s",
		 state->bits_acc & 0xff,
		 itobits(state->bits_acc & 0xff, 8)));

	PUT(state->bits_acc);
    }

    if (state->bits_used > 8) {
	E_DEBUG(("put acc (flush2) %02x %s",
		 (state->bits_acc>>8) & 0xff,
		 itobits((state->bits_acc>>8) & 0xff, 8)));

	PUT(state->bits_acc >> 8);
    }

#ifdef DEBUG
    if (state->bits_used > 16)
	logf("flush: bits_used %d", state->bits_used);
#endif

    state->bits_used = 0;
    state->bits_acc = 0;
}

/*
  v42bis_encode_value()

  encode a codeword value, noting if it's size exceeds C3, and
  doing any required STEPUPs
*/

void v42bis_encode_value(v42bis_t *state, ushort value)
{
    V_FLOW(("v42bis_encode_value(%lx, 0x%x)", state, value));

#ifdef DEBUG
    /* sanity check */
    if (value >= 8192) {
	logf("encode_value() value too big, %d", value);
	exit(1);
    }
#endif

    /* section 7.4 */

    /* check codeword size */
    while (value >= state->c3) {
	EN_DEBUG(("stepup: value %d, max %d", value, state->c3));

	v42bis_encode_codeword(state, CCW_STEPUP);
	state->c2++;
	state->c3 *= 2;
    }

    v42bis_encode_codeword(state, value);
}

#ifdef UNIT_TEST_VERBOSE
void v42bis_comp_test_report(v42bis_t *state)
{
    logf("in/out %d/%d = %4.2f; out_this_mode %ld, out_other_mode %ld",
	 state->bytes_in, state->bytes_out,
	 (double)state->bytes_in / (double)state->bytes_out,
	 state->bits_out_this_mode, state->bits_out_other_mode);
}
#endif

/*
  decide if we should transition from tranparent to compressed or
  visa versa.
*/

void v42bis_apply_compression_test(v42bis_t *state)
{
    if (state->just_flushed || state->exception_next)
	return;

#ifdef UNIT_TEST_PROGRESS
    {
	static int times = 0;
	if (++times == 1000) {
	    times = 0;
	    v42bis_comp_test_report(state);
	}
    }
#endif

#ifdef UNIT_TEST_FORCE
    /* force consistant behavior across all input */
    if (!state->transparent) {
	state->bits_out_while_transparent = 0;
	return;
    } else {
	state->bits_out_if_transparent = 0;
#undef WINDOW_CHECK_BYTES
#define WINDOW_CHECK_BYTES 0
	if (state->bits_out_while_transparent > 64*N3)
	    v42bis_transition_to_compressed(state);
	return;
    }
#endif

    /* bound check to recent history */
    if (WINDOW_FULL(state->bits_out_this_mode)) {
	state->bits_out_this_mode = 0;
	state->bits_out_other_mode = 0;
    }

    if (!state->transparent) {
	/* compressing */
	if ((state->bits_out_while_compressed -
	     state->bits_out_if_transparent) > WINDOW_MIN_BITS)
	    v42bis_transition_to_transparent(state);
    } else {
	/* transparent */
#ifdef NEVER_SEND_COMPRESSED
	return;
#endif
	/* transparent */
	if ((state->bits_out_while_transparent -
	     state->bits_out_if_compressed) > WINDOW_MIN_BITS)
	    v42bis_transition_to_compressed(state);
    }

#ifdef DEBUG_PUSH
    logf("ratio in/out %d/%d = %f",
	   state->bytes_in, state->bytes_out,
	   (double)state->bytes_in / (double)state->bytes_out);
#endif
}

/*
  v42bis_encode_buffer()

  implements C-DATA semantics on encode side
  
  encode a buffer full of data...
*/

void v42bis_encode_buffer(v42bis_t *state, uchar *string, int insize)
{
    uchar ch;
    boolean hit, duplicate;
    ushort hit_node;
    node_t *n, *dead, *p;

    V_FLOW(("v42bis_encode_buffer(%lx, %lx, %d)", state, string, insize));

    if (insize == 0)
	return;

    V_FLOW(("v42bis_encode: input %*s", insize, string));

    state->bytes_in += insize;

    /* section 6.3 */

    while (insize > 0) {
	/* "search" dictionary for string + character */
	ch = string[0];

	hit = FALSE;
	duplicate = FALSE;
	hit_node = state->last_match;
	p = DICT(state->last_match);

	EN_S_DEBUG(("last_match %d, string_size %d, insize %d, ch %d '%s'",
		    state->last_match, state->string_size, insize,
		    ch, show_char(ch)));

	if (state->last_match == 0) {
	    /*
	     * "the code word associated with each root node shall be N6 (the
	     * number of control codewords) plus the ordinal value of the
	     * character represented by the node"
	     */

	    state->last_match = ch + N6;
	    state->string_size = 1;

	    EN_S_DEBUG(("codeword for root %d, '%s' = %d",
			ch + N6, show_char(ch), CODE(DICT(ch + N6))));

	    p = DICT(ch + N6);
	    p->byte = ch;

	    hit = TRUE;
 	    hit_node = state->last_match;

	    /* consume input */
	    goto consume_input;
	}

	/* we're at a node; search it's leaves */
	for (n = DICT(DICT(state->last_match)->leaf);
	     CODE(n) && insize > 0;)
	{
	    EN_S_DEBUG(("  checking leaf node %d", CODE(n)));

	    if (n->byte == *string) {
		/* hit - check leafs */
		EN_S_DEBUG(("  hit: "));

		hit_node = CODE(n);
		p = n;
		state->last_match = CODE(n);

		if (state->just_flushed || hit_node == state->last_new)
		{
		    EN_S_DEBUG(("leaving search, node == last created"));
		    hit = FALSE;
		    duplicate = TRUE;

		    /* backup to previous node */
		    hit_node = n->parent;
		    state->last_match = n->parent;
		    break;
		}

		hit = TRUE;
		state->string_size++;

#ifdef never
		string++;
		insize--;

		/* if no leafs, exit now - we're at the end */
		if (n->leaf == 0) {
		    EN_S_DEBUG(("leaving search, no leaf"));
		    break;
		}

		n = DICT(n->leaf);
		EN_S_DEBUG(("continuing search, leaf %d", CODE(n)));
		continue;
#else
		EN_S_DEBUG(("exiting search, leaf %d", CODE(n)));
		goto consume_input;
#endif
	    } else {
		EN_S_DEBUG(("  miss: "));
		hit = FALSE;
	    }

	    if (n->node == 0) {
		EN_S_DEBUG(("leaving search, no node"));
		break;
	    }

	    n = DICT(n->node);
	    EN_S_DEBUG(("continuing search, node %d", CODE(n)));
	}

	EN_S_DEBUG(("search done, n %d, insize %d, next %d '%s' %s %s",
		    CODE(n), insize, string[0], show_char(string[0]),
		    hit ? "hit" : "miss", duplicate ? "duplicate" : ""));

#ifdef never
	/* we're matching but we ran out of characters */
	if (hit && insize == 0)
	    return;
#endif

	if (!hit && duplicate) {
	    boolean ok_to_output;

	    EN_S_DEBUG(("duplicate"));

	    ok_to_output =
		!state->just_flushed &&
		    !state->exception_next &&
			!state->decode_only;

	    state->exception_next = FALSE;

	    if (ok_to_output)
	    {
		if (!state->transparent)
		    v42bis_encode_value(state, hit_node);
		else {
		    state->bits_out_if_compressed += state->c2;

		    /* check if we should go compressed */
		    if (state->bytes_since_last_check > WINDOW_CHECK_BYTES) {
			state->bytes_since_last_check = 0;
			v42bis_apply_compression_test(state);
		    }
		}
	    }

	    /* string = string + character */
	    state->string_size++;

	    /* reset match to unmatched character */
	    state->last_match = 0;
	    state->string_size = 0;
	    state->last_new = 0;

	    state->just_flushed = 0;

	    /* don't advance, "string = unmatched character" */
	    continue;
	}

	/* last char did not match or already in dictionary */
	if (!hit && !duplicate)
	{
	    boolean ok_to_output;

	    EN_S_DEBUG(("update dictionary"));

	    ok_to_output =
		!state->just_flushed &&
		    !state->exception_next &&
			!state->decode_only;

	    state->exception_next = FALSE;

	    if (ok_to_output)
	    {
		if (!state->transparent)
		    v42bis_encode_value(state, hit_node);
		else {
		    state->bits_out_if_compressed += state->c2;

		    /* check if we should go compressed */
		    if (state->bytes_since_last_check > WINDOW_CHECK_BYTES) {
			state->bytes_since_last_check = 0;
			v42bis_apply_compression_test(state);
		    }
		}
	    }

	    state->just_flushed = 0;

	    /* "add string + character to dictionary" */

	    /* section 6.4a */

	    /* string too big? */
	    if (state->string_size >= state->n7) {
		EN_DEBUG(("string size (%d) > n7 (%d)",
			  state->string_size, state->n7));

		/* reset match */
		state->last_match = 0;
		state->string_size = 0;

		/* we were in the match routine, reset last new */
		state->last_new = 0;
		continue;
	    }

	    /* pick a new code word */
	    n = DICT(state->c1);
	    state->last_new = CODE(n);

	    EN_DEBUG(("adding new node %d = %d '%s', parent %d",
		      CODE(n), string[0], show_char(string[0]), CODE(p)));

	    /* attach "string + character" */
	    n->byte = string[0];
	    n->parent = hit_node;
#ifdef DEBUG
	    if (CODE(n) == hit_node)
		logf("creating loop! node %d", CODE(n));
#endif
	    n->node = 0;

	    /* XXX should be in ord(ch) order to allow faster search */
	    n->node = p->leaf;
	    p->leaf = CODE(n);

	    /* section 6.5 */

	    /* recover dictionary entries */
	    do {
		state->c1++;

		if (state->c1 > (state->n2 - 1)) {
		    state->c1 = N5;
		    state->dict_full = TRUE;
		}

		dead = DICT(state->c1);

		/* find terminal nodes (i.e. leaf == 0) */
	    } while (/*dead->parent != 0 &&*/ dead->leaf != 0);

	    /* terminal nodes with parents are eligible */
	    if (CODE(dead) && /* <- I think this is not needed */
		/*dead->parent && */dead->leaf == 0 &&
		state->dict_full)
	    {
		/* go to parent, disconnect from chain */
		node_t *parent = DICT(dead->parent);

		EN_DEBUG(("recovering dead node %d", CODE(dead)));

		/* if first on parents list, fix parent */
		if (DICT(parent->leaf) == dead)
		    parent->leaf = dead->node;
		else
		    /* else search parents list, fix sibling */
		    for (parent = DICT(DICT(dead->parent)->leaf);
			 CODE(parent); parent = DICT(parent->node))
		    {
			if (DICT(parent->node) == dead) {
			    parent->node = dead->node;
			    break;
			}
		    }

		/* mark node free */
		dead->parent = 0;
		dead->leaf = 0;
	    } /* dead node */

	    /* if we added a node, reset "string" */
/*reset_match: */
	    state->last_match = 0;
	    state->string_size = 0;
	    state->just_flushed = 0;

	    /*
	     * this is a "safe time" to do compression test, as we've just
	     * done an update...
	     */
	    if (!state->decode_only) {
		if (state->bytes_since_last_check > WINDOW_CHECK_BYTES) {
		    state->bytes_since_last_check = 0;
		    v42bis_apply_compression_test(state);
		}
	    }

	    /* don't advance, "string = unmatched character" */
	    continue;
	} /* (!hit && !duplicate) */

consume_input:
	string++;
	insize--;
	state->bytes_since_last_check++;

	/* section 9.2 */
/*check_escape: */
	/* escape processing */
	if (state->transparent) {
	    if (!state->decode_only)
		v42bis_encode_value(state, ch);

	    if (ch == state->escape) {
		if (!state->decode_only) {
		    v42bis_encode_value(state, CCW_EID);
		    state->escape += ESCAPE_CYCLE;
		}
	    }
	} else {
	    /* compressed, cycle escape character */
	    if (ch == state->escape && !state->decode_only)
		state->escape += ESCAPE_CYCLE;

	    state->bits_out_if_transparent += N3;
	}

	state->just_flushed = 0;
    }
}

/*
  implements C-FLUSH semantics
*/

void v42bis_encode_flush(v42bis_t *state)
{
    V_FLOW(("v42bis_encode_flush() string_size %d, last_match %d",
	  state->string_size, state->last_match));

    if (state->just_flushed)
	return;

    if (state->transparent) {
	/* transparent, send any buffered characters */
    } else {
	/* compressed */

	/* section 7.9a */
	/* output partial match, if any */
	if (state->string_size) {
	    /* section 7.8.2 */
	    v42bis_encode_value(state, state->last_match);
	}

	state->just_flushed = 1;

	v42bis_encode_value(state, CCW_FLUSH);
	v42bis_encode_codeword_flush(state);
    }

    v42bis_push(state);
}

void v42bis_transition_to_compressed(v42bis_t *state)
{
    V_FLOW(("v42bis_transition_to_compressed()"));

#ifdef UNIT_TEST_VERBOSE
    logf("v42bis_transition_to_compressed()");
    v42bis_comp_test_report(state);
#endif

    if (state->transparent) {
	/* section 7.8.1a */
	v42bis_encode_value(state, state->escape);

	/* section 7.8.1b */
	v42bis_encode_value(state, CCW_ECM);
	v42bis_encode_codeword_flush(state);

	/* enter compressed mode */
	state->transparent = FALSE;

	state->bits_out_if_transparent = 0;
	state->bits_out_while_compressed = 0;
    }
}

void v42bis_transition_to_transparent(v42bis_t *state)
{
    V_FLOW(("v42bis_transition_to_transparent()"));

#ifdef UNIT_TEST_VERBOSE
    logf("v42bis_transition_to_transparent()");
    v42bis_comp_test_report(state);
#endif

    /* check counters for overflow */

    if (!state->transparent) {
	/* output partial match, if any */
	if (state->string_size) {
	    /* section 7.8.2 */
	    v42bis_encode_value(state, state->last_match);
	}

	/* section 7.8.2c */
	v42bis_encode_value(state, CCW_ETM);

	/* section 7.8.2d */
	v42bis_encode_codeword_flush(state);

	/* section 7.8.2e */
	/* enter transparent mode */
	state->transparent = TRUE;

	/* reset compressibility test */
	state->bits_out_if_compressed = 0;
	state->bits_out_while_transparent = 0;
    }
}

void v42bis_signal_reset(v42bis_t *state)
{
    if (!state->transparent) {
	/* change to transparent */
	v42bis_transition_to_transparent(state);

	/* counteract side effect */
	state->exception_next = FALSE;
    }

    v42bis_encode_value(state, state->escape);
    v42bis_encode_value(state, CCW_RESET);
}

/*
  expand a codeword into it's string

  follow chain of "parent" to root and then expand the node characters
  one by one.
*/

uchar v42bis_decode_match(v42bis_t *state, ushort codeword, int *psize)
{
    node_t *path[256];
    int path_size = 0;
    node_t *base;
    int i;

    V_FLOW(("v42bis_decode_match(%d)", codeword));

    for (base = DICT(codeword); CODE(base); base = DICT(base->parent)) {
	path[path_size++] = base;
	if (path_size >= 256) {
	    v42bis_c_error(state, "path_size exceeds 256!");
	    break;
	}
#ifdef DEBUG
	if (base == DICT(base->parent)) {
	    logf("loop! node %d", CODE(base));
	    break;
	}
#endif
    }

    /* XXX this should not be done here! */
    if (codeword < N5 && DICT(codeword)->byte == 0)
	DICT(codeword)->byte = codeword - N6;

    D_DEBUG(("path_size %d", path_size));
 
    for (i = path_size - 1; i >= 0; i--) {
	D_DEBUG(("put byte %02x '%s'",
		 path[i]->byte, show_char(path[i]->byte)));

	if (path[i]->byte == state->escape)
	    state->escape += ESCAPE_CYCLE;

	PUT(path[i]->byte);
    }

    *psize = path_size;

    /* return first (prefix) char of string */
    return path[path_size-1]->byte;
}

/*
  decode L-DATA semantics on the decode side

  decode a buffer full of data...
*/

void v42bis_decode_buffer(v42bis_t *state, uchar *data, int datasize)
{
    ushort codeword;
    uchar ch;

    V_FLOW(("v42bis_decode_buffer() %d bytes", datasize));

    while (datasize--) {
	if (data) {
	    /* we have a buffer */
	    D_DEBUG(("pull 0x%x", *data & 0xff));
	    codeword = v42bis_decode_codeword(state, *data++);
	} else {
	    /* no input buffer, flush */
	    codeword = v42bis_decode_codeword_flush(state);
	    datasize = 0;
	}

	DE_DEBUG(("codeword %d (0x%x)", codeword, codeword));

	/* if decode did not return a value, return */
	if (codeword == 0xffff) {
	    /* no data */
	    D_DEBUG(("no data"));
	    continue;
	}

	if (state->transparent) {
	    /* transparent mode */

	    /* escaped - look at next codeword */
	    if (state->escaped) {
		state->escaped = FALSE;

		DE_DEBUG(("escape codeword"));

		/* section 5.8d */
		if (codeword >= 3 && codeword <= 255) {
		    logf("v42: received reserved code word (%d)", codeword);
		    v42bis_c_error(state, "received reserved code word");
		    break;
		}

		switch (codeword) {
		  case CCW_ECM:
		    DE_DEBUG(("enter compression mode"));
		    state->transparent = FALSE;

		    /* set up for decode */
		    state->last_decode = state->last_match;
		    state->last_decode_size = state->string_size;

		    state->exception_next = TRUE;
		    break;

		  case CCW_EID:
		    DE_DEBUG(("escape id"));

		    codeword = state->escape;
		    state->escape += ESCAPE_CYCLE;
		    goto decode_encode;
		    break;

		  case CCW_RESET:
		    DE_DEBUG(("reset"));

		    v42bis_init_dictionary(state);
		    break;
		}
	    } else {
		/* escape? */
		if (codeword == state->escape) {
		    DE_DEBUG(("escape prefix"));
		    state->escaped = TRUE;
		    continue;
		}

	      decode_encode:
		/* save data in output buffer */
		PUT(codeword);

		/* encode to build dictionary */
		ch = codeword;
		v42bis_encode_buffer(state, &ch, 1);
	    }
	} else {
	    int size;

	    /* compression mode */
	    switch (codeword) {
	      case CCW_ETM:
		DE_DEBUG(("enter transparent mode"));

		v42bis_decode_codeword_flush(state);
		state->transparent = TRUE;
		state->last_match = state->last_decode;
		state->string_size = state->last_decode_size;
		state->last_new = 0;

		state->just_flushed = 1;
		break;

	      case CCW_FLUSH:
		DE_DEBUG(("flush"));

		/* terminate search */
		state->last_match = 0;
		state->string_size = 0;
		state->last_match = state->last_decode;
		state->string_size = state->last_decode_size;
		state->last_new = 0;

		/* reset codeword decode machine */
		state->bits_waiting = 0;
		state->bits_remaining = 0;
		break;

	      case CCW_STEPUP:
		DE_DEBUG(("stepup"));

		/* section 5.8a */;
		if (state->c2 + 1 > state->n1) 
		    v42bis_c_error(state, "received STEPUP; c2 exceeds max");
		else
		    state->c2++;
		break;

	      default:
		/* regular codeword */

		/* section 5.8b */
		if (codeword == state->c1) {
#ifdef DEBUG
		    logf(state, "received codeword equal to c1");
#endif
		    continue;
		}

		/* section 5.8c */
		if (codeword >= N5 && state->dictionary[codeword].parent == 0)
		{
#ifdef DEBUG
		    logf("received unused codeword %d, full %d, c1 %d",
			 codeword, state->dict_full, state->c1);
#endif
		    v42bis_c_error(state, "received unused codeword");
		}

		ch = v42bis_decode_match(state, codeword, &size);

		/*
		 * umm... "New dictionary entries shall be created using
		 * the proceedure defined in section 6.4, with the first
		 * (prefix) character of the most recently decoded string
		 * being appended to the previously decoded string."
		 *
		 * what a pain this was to get right!
		 */

		/* section 8 */
		state->last_match = state->last_decode;
		state->string_size = state->last_decode_size;

		v42bis_encode_buffer(state, &ch, 1);

		state->last_decode = codeword;
		state->last_decode_size = size;
	    } 
	}
    }

    v42bis_push(state);
}

/*
  v42bis_decode_flush()

  flush codeword decoder and push out data
*/

void v42bis_decode_flush(v42bis_t *state)
{
    V_FLOW(("v42bis_decode_flush()"));

    v42bis_decode_buffer(state, (uchar *)0, 1);

    v42bis_push(state);
}

/*
  v42bis_c_error()

  implements C-ERROR semantics
*/

void v42bis_c_error(v42bis_t *state, char *reason_string)
{
    logf("v42bis: C-ERROR with '%s' on %s",
	 reason_string,
	 state == ((v42bis_connection_t *)state->connection)->encode ? "encode" : "decode");

    state->error = TRUE;

    v42bis_exit_handler();
}

void v42bis_disconnect(v42bis_t *state, char *reason_string)
{
    logf("v42bis: disconnect with '%s'", reason_string);
    state->error = TRUE;

    v42bis_exit_handler();
}

void v42bis_init_buffer(v42bis_t *state, uchar *buf, int size)
{
    state->output_buffer = buf;
    state->output_ptr = buf;
    state->output_size = 0;
    state->output_max = size;
}

/*
*/
void v42bis_connection_init(v42bis_connection_t *conn)
{
    conn->default_p0 = 3;
    conn->default_p1 = DEF_P1;	/* total # of codewords */
    conn->default_p2 = DEF_P2;	/* max string length */

    conn->neg_p0 = conn->default_p0;	/* direction of compression */
    conn->neg_p1 = conn->default_p1;	/* total # of codewords */
    conn->neg_p2 = conn->default_p2;	/* max string length */

    /* encode side */
    conn->encode->connection = (void *)conn;
    conn->encode->decode_only = FALSE;

    v42bis_init(conn->encode);

    /* decode side */
    conn->decode->connection = (void *)conn;
    conn->decode->decode_only = TRUE;

    v42bis_init(conn->decode);
}

void v42bis_connection_init_buffers(v42bis_connection_t *conn, 
				uchar *e_buf, int e_size, 
				uchar *d_buf, int d_size)
{
    v42bis_init_buffer(conn->encode, e_buf, e_size);
    v42bis_init_buffer(conn->decode, d_buf, d_size);
}

void v42bis_connection_init_push(v42bis_connection_t *conn, void *context, 
				void (*e_push)(void *, u_char *, int), 
				void (*d_push)(void *, u_char *, int))
{
    conn->encode->push_func = e_push;
    conn->encode->push_context = context;
    conn->decode->push_func = d_push;
    conn->decode->push_context = context;
}

/* ------------- debug -------------- */

#ifdef DEBUG

void v42bis_dumptree_follownode(v42bis_t *state, ushort node)
{
    int i;
    node_t *n = DICT(node);

    for (i = 0; i < state->dump_indent; i++)
	fprintf(stderr, "  ");

    fprintf(stderr, "code %d; char %d '%s' parent %d, node %d, leaf %d\n",
	   node, n->byte, show_char(n->byte), n->parent, n->node, n->leaf);

    if (n->node)
	v42bis_dumptree_follownode(state, n->node);

    state->dump_indent++;

    if (n->leaf)
	v42bis_dumptree_follownode(state, n->leaf);

    state->dump_indent--;
}

void v42bis_dumptree(v42bis_t *state, char *name)
{
    int i;

    fprintf(stderr, "%s codewords:\n", name);
    for (i = 0; i < CODES; i++)
	if (state->dictionary[i].byte) {
	    node_t *n = &state->dictionary[i];

	    fprintf(stderr, "code %d; char %d '%s' parent %d, node %d, leaf %d\n",
		   i, n->byte, show_char(n->byte),
		   n->parent, n->node, n->leaf);
	}

    state->dump_indent = 0;

    fprintf(stderr, "%s tree:\n", name);
    for (i = 0; i < N5; i++)
	if (state->dictionary[i].byte) {
	    node_t *n = &state->dictionary[i];

	    fprintf(stderr, "code %d; root node, %d '%s', leaf %d:\n",
		   i, n->byte, show_char(n->byte), n->leaf);

	    if (state->dictionary[i].leaf) {
		state->dump_indent = 1;
		v42bis_dumptree_follownode(state, n->leaf);
	    }
	}
}

void v42bis_connection_dumptree(v42bis_connection_t *conn)
{
    int i;

    fprintf(stderr, "\nv42bis_connection_dumptree()\n");

    v42bis_dumptree(conn->encode, "encode");
    v42bis_dumptree(conn->decode, "decode");
}

#endif	/* DEBUG */


/* ------------- external interface -------------- */

void v42bis_mnp_set_debug(mnp_t *mnp)
{
    mnp->v42bis.decode->debug_decode = 0;
    mnp->v42bis.decode->debug_encode = 0;

    switch (mnp->debug_v42) {
      case 3:
	mnp->v42bis.decode->debug_flow = TRUE;
	mnp->v42bis.encode->debug_flow = TRUE;
	/* fall through */

      case 2:
	mnp->v42bis.decode->debug_decode_bytes = TRUE;
	mnp->v42bis.decode->debug_encode_bytes = TRUE;

	mnp->v42bis.encode->debug_encode_bytes = TRUE;

	mnp->v42bis.decode->debug_decode++;
	mnp->v42bis.decode->debug_encode++;

	/* fall through */

      case 1:
	mnp->v42bis.decode->debug_decode++;
	mnp->v42bis.decode->debug_encode++;

	mnp->v42bis.encode->debug_encode = TRUE;
	break;
      case 0:
	break;
    }
}

void v42bis_mnp_epush P((void *, uchar *buffer, int));

void v42bis_mnp_init(mnp_t *mnp)
{
    /* set internal debugging switches */
    v42bis_mnp_set_debug(mnp);

    /* init the connection (both encode and decode */
    v42bis_connection_init(&mnp->v42bis);

    /* give the encode side a perminant buffer */
    v42bis_connection_init_buffers(&mnp->v42bis,
				   mnp->v42_ebuf, sizeof(mnp->v42_ebuf),
				   (uchar *)0, 0);

    /* give the encode side a push routine */
    v42bis_connection_init_push(&mnp->v42bis, (void *)mnp,
				v42bis_mnp_epush,
				(void (*) (void *, u_char *, int)) 0);
}

/*
  v42bis_send()

  send data to V.42bis connection

  input:	unsigned char *buffer; 	pointer to user data buffer
	     	int buflen;		length of user data buffer

  output:	int retcode - if positive, the number of data bytes
		              copied from the user data buffer; 
			      if negative, link error code
*/

int v42bis_send(void *mnparg, uchar *bufptr, int buflen)
{
    mnp_t *mnp = mnparg;
    if (!mnp->neg_v42bis || !(mnp->lcb.lr_parm & LR_COMPRESSION))
	return mnpsend(mnp, bufptr, buflen);

    FLOW(("v42bis_send(%lx, %lx, %d)", mnp, bufptr, buflen));

#ifdef V42_DUMP_ENABLED
    if (mnp->v42_dump) {
	mnp->v42_size = buflen;
	mnp->v42_type = 2;
	write(mnp->v42_dump, &mnp->v42_esize, 4);
	write(mnp->v42_dump, bufptr, buflen);
    }
#endif

    v42bis_encode_buffer(mnp->v42bis.encode, bufptr, buflen);

    v42bis_encode_flush(mnp->v42bis.encode);

    return buflen;
}

void v42bis_mnp_epush(void *mnp, uchar *buffer, int size)
{
   /* FLOW(("v42bis_mnp_epush(%lx, %d)", buffer, size)); 

    M_DEBUG(("v42bis: sending %d bytes", size)); */

    mnpsend((mnp_t *)mnp, buffer, size);
}


/*
  v42bis_receive()

  receive data from V.42bis connection

  input:	unsigned char *buffer;	pointer to user buffer
		int buflen;		length of user buffer

  output:	int retcode;	if positive, the number of data bytes
				copied into the user data buffer;
				if negative, link error code
*/

int v42bis_receive(mnp_t *mnp, uchar *bufptr, int buflen)
{
    int mnp_size, v42_size;

    if (!mnp->neg_v42bis || !(mnp->lcb.lr_parm & LR_COMPRESSION))
	return mnpreceive(mnp, bufptr, buflen);

    FLOW(("v42bis_receive(%lx, %lx, %d)", mnp, bufptr, buflen));

    mnp_size = mnpreceive(mnp, mnp->v42_dbuf, 128);
/*			  sizeof(mnp->v42_dbuf)); */

    M_DEBUG(("v42bis: mnp returns %d bytes", mnp_size));

    /* error (<0) or no input (==0) */
    if (mnp_size <= 0)
	return mnp_size;

#ifdef V42_DUMP_ENABLED
    if (mnp->v42_dump) {
	mnp->v42_size = mnp_size;
	mnp->v42_type = 2;
	write(mnp->v42_dump, &mnp->v42_size, 4 + mnp_size);
    }
#endif

    /* tell v42bis to decode into caller's buffer */
    v42bis_init_buffer(mnp->v42bis.decode, bufptr, buflen);

    /* decode everything we got from mnp */
    v42bis_decode_buffer(mnp->v42bis.decode, mnp->v42_dbuf, mnp_size);

    /* see how much we got decompressed */
    v42_size = mnp->v42bis.decode->output_size;

    if (v42_size) {
	M_DEBUG(("v42bis_receive returns %d", v42_size));
    }

    return v42_size;
}

