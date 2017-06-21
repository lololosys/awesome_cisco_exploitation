/* $Id: vines_netrpc.h,v 3.4.12.3 1996/08/07 09:03:44 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_netrpc.h,v $
 *------------------------------------------------------------------
 * vines_netrpc.h - Support for the vines Network Remote
 *		    Procedure Call (NetRPC) protocol.
 *
 * February 1993, David Hampton
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_netrpc.h,v $
 * Revision 3.4.12.3  1996/08/07  09:03:44  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/07/23  13:30:25  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.4.12.1  1996/05/11  16:17:36  thille
 * CSCdi51397:  Duplicate strings in vines code
 * Branch: California_branch
 * Clean up some duplicate literals and save 1140 bytes in gs7-j-m.
 *
 * Revision 3.4  1996/02/14  18:30:58  ehk
 * CSCdi48247:  vines bluemail clients on serverless cannot get time
 * changes to avoid unaligned memory access and compiler padding in
 * VINES packet structures.
 *
 * Revision 3.3  1996/02/08  20:13:58  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.2  1995/11/17  17:58:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/21  20:53:24  slin
 * CSCdi39242:  vines invalid string length in data
 *
 * Revision 2.1  1995/06/07  23:14:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Common external variables
 */
extern boolean vinesnetrpc_debug;
extern const int     vrpc_frame_lengths[];
extern char   *const vrpc_names[];

/*
 * Structure declarations
 */
STRUCTURE_REF(vrpc_call);
STRUCTURE_REF(vrpc_reject);
STRUCTURE_REF(vrpc_return);
STRUCTURE_REF(vrpc_abort);
STRUCTURE_REF(vrpc_search);
STRUCTURE_REF(vrpc_returnaddress);

/**********************************************************************
 *
 *	   VINES SESSION AND PRESENTATION LAYER DEFINITIONS
 *
 **********************************************************************/

/*
 * Individual definitions
 */

/*
 * Type 0 - Call
 */
struct vrpc_call_ {
    ushort type;
    ushort id;
    ulong  PACKED(program);
    ushort version;
    ushort procedure;
    uchar  data[0];
};

/*
 * Type 1 - Reject
 */
struct vrpc_reject_ {
    ushort type;
    ushort id;
    ushort reason;
};

/*
 * Type 2 - Return
 */
struct vrpc_return_ {
    ushort type;
    ushort id;
    uchar  data[0];
};

/*
 * Type 3 - Abort
 */
struct vrpc_abort_ {
    ushort type;
    ushort id;
    ushort reason;
};

/*
 * Type 4 - Search
 * Type 6 - Search All
 */
struct vrpc_search_ {
    ushort type;
    ushort revision;
    ushort min_s;
    ushort max_s;
    ushort userdef_number;
    ulong  PACKED(min_l);
    ulong  PACKED(max_l);
    uchar  userdef_length;
    uchar  userdef[0];
    uchar  info1_length;
    uchar  info1[0];
    uchar  info2_length;
    uchar  info2[0];
    uchar  info3_length;
    uchar  info3[0];
};

/*
 * Type 5 - Return Address
 */
struct vrpc_returnaddress_ {
    ushort type;
    ushort proto_rev;
    ushort service_rev_s;
    vinesaddress address;
    ulong PACKED(service_rev_l);
};

/*
 * The monolithic definition.
 */
struct vinesnetrpctype_ {
    union {
	vrpc_call call;
	vrpc_reject reject;
	vrpc_return retn;
	vrpc_abort abort;
	vrpc_search search;
	vrpc_returnaddress retnaddress;
	vrpc_search searchall;
	struct {
	    ushort  type;
	    uchar  data[0];
	} unknown;
    } m;
};

/*
 * Field definitions.
 */
#define VRPC_CALL		  0
#define VRPC_REJECT		  1
#define VRPC_RETURN		  2
#define VRPC_ABORT		  3
#define VRPC_SEARCH		  4
#define VRPC_RETURNADDRESS	  5
#define VRPC_SEARCHALL		  6
#define VRPC_UNKNOWN		  7

#define VRPC_REJ_UNKN_PROGRAM	  0
#define VRPC_REJ_UNKN_VERSION	  1
#define VRPC_REJ_UNKN_PROCEDURE	  2
#define VRPC_REJ_BAD_DATA_TYPE	  3
#define VRPC_REJ_UNSPECIFIED	 -1

/***********************************************************************
 *
 *			       INLINES
 *
 ***********************************************************************/

/*
 * vrpc_getbuffer
 *
 * Get a buffer for an RPC message that will be sent directly to
 * another server/router.
 */
static inline paktype *vrpc_getbuffer (int type, int length)
{
    paktype *pak;
    int hdr_length = vrpc_frame_lengths[type];

    pak = vipc_getbuffer(hdr_length + length);
    if (pak) {
	pak->application_start = pak->session_start + hdr_length;
	pak->datagramsize = hdr_length + length;
	((vinesnetrpctype *)pak->session_start)->m.call.type = type;
    }
    return(pak);
}

/*
 * vrpc_b_getbuffer
 *
 * Get a buffer for an RPC message that will be sent as a network
 * layer broadcast.
 */
static inline paktype *vrpc_b_getbuffer (int length)
{
    paktype *pak;
    int hdr_length = vrpc_frame_lengths[VRPC_CALL];

    pak = vipc_short_getbuffer(hdr_length + length);
    if (pak) {
	pak->application_start = pak->session_start + hdr_length;
	pak->datagramsize = hdr_length + length;
	((vinesnetrpctype *)pak->session_start)->m.call.type = VRPC_CALL;
    } else {
	if (vinespkt_debug)
	    buginf("%n%s%s", vines_VINES_str, 
		   vines_cannot_alloc_netrpc_packet_msg);
    }
    return(pak);
}

typedef enum good_bad_ {
    BAD  = 0,
    GOOD = 1
} good_bad;

/*
 * vrpc_socket_debug
 *
 * Clean up the rest of the code by putting this common sequence into an
 * inline function.
 */
static inline good_bad vrpc_check_args (
    vinesipcsocket *socket,
    paktype *pak,
    int type)
{
    extern const char bad_rpc_socket[];
    extern const char bad_rpc_packet[];

    if (!socket) {
	if (vinesnetrpc_debug)
	    buginf(bad_rpc_socket, vrpc_names[type]);
	return(BAD);
    }
    if (!pak) {
	if (vinesnetrpc_debug)
	    buginf(bad_rpc_packet, vrpc_names[type]);
	return(BAD);
    }
    return(GOOD);
}

/*
 * The following inlines are needed because Banyan packs its data
 * structures before transmitting them.  These routines handle insertion
 * and extraction of the data fields without cluttering up the C routines
 * so that they are ureadable.
 *
 * See vines_services.h for many more of these routines.  These two
 * are here because they are needed for Search/ReturnAddress messages.
 */
static inline uchar *vines_insert_smstring (
    uchar *start,
    char *data,
    int maxlength)
{
    vinessmallstring *string;
    int length;

    length = strlen(data);
    if (length >= maxlength)
	length = maxlength-1;
    string = (vinessmallstring *)start;
    string->length = length;
    sstrncpy(string->data, data, length+1); /* bec strncpy counts terminator */
    return(start + 1 + length);
}

static inline uchar *vines_extract_smstring (
    uchar *start,
    char *data,
    int maxlength)
{
    vinessmallstring *string;
    int length;

    string = (vinessmallstring *)start;
    length = string->length;
    if (length >= maxlength)
	buginf(vines_invalid_string_len_msg, vines_VINES_str);
    sstrncpy(data, string->data, length+1); /* bec strncpy counts terminator */
    return(start + 1 + length);
}


/***********************************************************************
 *
 *			     PROTOTYPES
 *
 ***********************************************************************/


/* vines_netrpc.c */
extern void vrpc_send_long_call(vinesipcsocket *, paktype *, ulong, ushort, ushort, ushort, ushort);
extern void vrpc_send_broadcast_call(paktype *, vinesidbtype *, vinesidbtype *, ulong, ushort, ushort, ushort, ushort, ushort, ushort);
extern void vrpc_send_reject(vinesipcsocket *, ushort);
extern void vrpc_send_abort(vinesipcsocket *, ushort);
extern void vrpc_send_return(vinesipcsocket *, paktype *);
extern void vrpc_send_search(ushort, ulong, ulong, ushort, char *, char *, char *, char *, ushort, ushort, int);
extern void vrpc_send_returnaddress(vinesipcsocket *, ushort, ulong, ushort, ushort);
