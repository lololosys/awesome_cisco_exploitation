/* $Id: rsvp_objects.h,v 1.1.4.6 1996/08/20 10:24:36 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_objects.h,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_objects.h,v $
 * Revision 1.1.4.6  1996/08/20  10:24:36  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/08/16  17:21:05  asastry
 * CSCdi65504:  Make 'debug IP RSVP detail' command dump packet contents
 * in the event of an error condition.
 *
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/08/15  00:28:14  asastry
 * CSCdi65504:  Make <debug ip rsvp> display only the summarized debug
 * output.
 *
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/08/09  00:18:22  asastry
 * CSCdi65504:  Allow <debug ip rsvp detail> command to be RESV-only or
 * PATH-only
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/13  16:45:58  fred
 * CSCdi57500:  update rsvp from draft 8 to draft 11 objects
 *         This is done with an ifdef at this point as some EFT sites
 *         need draft 8 code; these will be removed later
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:22  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RSVP_OBJECT__
#define __RSVP_OBJECT__
#define RSVP_FRAGMENTATION
#include "ieee_float.h"
#include "../rsvp/rsvp_debug.h"
#define RSVP_BUGINF(address, message) {\
 if(rsvp_acl_debug(rsvp_debug, address)) \
        buginf message; \
 }
#define RSVP_RESV_BUGINF(address, message) {\
 if (rsvp_acl_debug(rsvp_debug, address) && \
     ((rsvp_debug  && !rsvp_detail_flag) || \
      (rsvp_detail_flag & RSVP_RESV_DEBUG_FLAG))) \
        buginf message; \
 }

#define RSVP_PATH_BUGINF(address, message) {\
 if (rsvp_acl_debug(rsvp_debug, address) && \
     ((rsvp_debug && !rsvp_detail_flag) || \
      (rsvp_detail_flag & RSVP_PATH_DEBUG_FLAG))) \
        buginf message; \
 }

#define RSVP_DUMP_OBJECT(head, debug_flag, length) {\
  if (rsvp_print_detail(head->message_type, debug_flag)) \
        rsvp_dump_object(head, length); \
 }

/***********************************************************************
 * Message Data Structures
 *
 ***********************************************************************/

/*
 * object base unit
 */
STRUCTURE_REF (rsvp_object);
struct rsvp_object_ {
    ushort length;			/* length in bytes, > 0, multiple of four */
    uchar class;			/* class of object, major grouping */
    uchar type;				/* type of object, minor grouping */
    uchar data[0];
};

#define RSVP_VERSION 1

typedef
enum rsvp_message_types_ {
    RSVP_MSG_PATH=1, RSVP_MSG_RESV, RSVP_MSG_PERR, RSVP_MSG_RERR,
    RSVP_MSG_PTEAR, RSVP_MSG_RTEAR, RSVP_MSG_CONFIRM
    }
rsvp_message_types;

STRUCTURE_REF (rsvp_header);
struct rsvp_header_ {
#ifdef DRAFT10
    unsigned version:4;			/* RSVP Version */
    unsigned flag0:1;			/* usage undefined */
    unsigned flag1:1;			/* usage undefined */
    unsigned flag2:1;			/* usage undefined */
    unsigned flag3:1;			/* usage undefined */
    uchar    message_type;		/* type of message */
    ushort   cksum;			/* IP Checksum */
    ushort   rsvp_length;		/* length of RSVP data */
    uchar    reserved_byte;		/* reserved */
    uchar    sending_ttl;		/* ttl message is sent using */
    ulong    message_id;		/* message ID for RSVP Fragmentation */
    ulong    fragment;			/* fragmentation field */
#define RSVP_MORE_FRAGS	     0x01000000 /* more fragments follow */
#define RSVP_FRAGMENT_OFFSET 0x00FFFFFF /* offset in message */
#else  DRAFT11
    unsigned version:4;			/* RSVP Version */
    unsigned flag0:1;			/* usage undefined */
    unsigned flag1:1;			/* usage undefined */
    unsigned flag2:1;			/* usage undefined */
    unsigned flag3:1;			/* usage undefined */
    uchar    message_type;		/* type of message */
    ushort   cksum;			/* IP Checksum */
    uchar    sending_ttl;		/* ttl message is sent using */
    uchar    reserved_byte;		/* reserved */
    ushort   rsvp_length;		/* length of RSVP data */
#endif
    rsvp_object rsvp_first_object[0];
};

#define RSVP_CLASS_FORWARD_MASK 0x80
#define RSVP_CLASS_NULL 0
#define RSVP_CLASS_SESSION 1
#define RSVP_CLASS_SESSION_GROUP 2
#define RSVP_CLASS_HOP 3
#define RSVP_CLASS_INTEGRITY 4
#define RSVP_CLASS_TIME_VALUES 5
#define RSVP_CLASS_ERROR_SPEC 6
#define RSVP_CLASS_SCOPE 7
#define RSVP_CLASS_STYLE 8
#define RSVP_CLASS_FLOWSPEC 9
#define RSVP_CLASS_FILTER_SPEC 10
#define RSVP_CLASS_SENDER_TEMPLATE 11
#define RSVP_CLASS_SENDER_TSPEC 12
#define RSVP_CLASS_ADSPEC 13
#define RSVP_CLASS_POLICY_DATA 14
#define RSVP_CLASS_RESV_CONFIRM 15
#define RSVP_CLASS_MAX 15

#define RSVP_TYPE_IPV4 1
#define RSVP_TYPE_IPV6 2
/*
 * A NULL object has a Class-Num of zero, and its C-Type is ignored.
 * Its length must be at least 4, but can be any multiple of 4. A
 * NULL object may appear anywhere in a sequence of objects, and its
 * contents will be ignored by the receiver.
 */
STRUCTURE_REF (rsvp_null);
struct rsvp_null_ {
    rsvp_object base;
};

/*
 * Contains the IP destination address(DestAddress) and possibly a
 * generalized destination port, to define a specific session for the
 * other objects that follow. Required in every RSVP message.
 */
STRUCTURE_REF (rsvp_session_ipv4);
struct rsvp_session_ipv4_ {
    rsvp_object base;
    ipaddrtype destination;		/* unicast of multicast address */
    uchar protocol;
    uchar flags;
#define RSVP_SESSION_POLICE 0x01
    ushort udp_dest_port;		/* UDP/TCP Port */
};

/*
 * Carries the IP address of the RSVP-capable node that sent this
 * message. This document refers to a RSVP_HOP object as a PHOP
 * ("previous hop") object for downstream messages or as a NHOP("next
 * hop") object for upstream messages.
 */
STRUCTURE_REF (rsvp_hop_ipv4);
struct rsvp_hop_ipv4_ {
    rsvp_object base;
    ipaddrtype neighbor;		/* next RSVP system */
    void *lif_handle;			/* interface identifier */
};

/*
 * tells us the rate at which refreshes will be sent to us
 */
#define RSVP_REFRESH_MS (30*1000)
#define RSVP_CLOCK_TICK RSVP_REFRESH_MS/2
STRUCTURE_REF (rsvp_refresh);
struct rsvp_refresh_ {
    rsvp_object base;
    ulong refresh_ms;			/* number of milliseconds */
};

/*
 * Specifies an error, in a PERR or RERR message.
 */
enum rsvp_error_types {
    RSVP_ERROR_ADMISSION = 1,		/* admission failure */
    RSVP_ERROR_ADMINISTRATIVE,		/* policy failed */
    RSVP_ERROR_NO_PATH_INFORMATION,	/* no path info */
    RSVP_ERROR_NO_SENDER_INFORMATION,	/* not this sender */
    RSVP_ERROR_AMBIGUOUS_PATH,		/* pick one, guys */
    RSVP_ERROR_AMBIGUOUS_FILTER_SPECIFICATION, /* aw come on */
    RSVP_ERROR_INCOMPATIBLE_STYLE,	/* unmergable styles */
    RSVP_ERROR_MISSING_OBJECT = 11,	/* missing pieces */
    RSVP_ERROR_UNKNOWN_OBJECT,		/* sound of head scratching */
    RSVP_ERROR_UNKNOWN_CLASS_TYPE,	/* sound of head scratching */
    RSVP_OBJECT_ERROR,			/* incorrectly formatted */
    RSVP_TRAFFIC_ERROR = 21,		/* implementation specific */
    RSVP_SYSTEM_ERROR,			/* implementation specific */
    RSVP_ERROR_CONFIRM			/* confirmation of reservation */
    };

#define RSVP_ERROR_INTEGRITY RSVP_TRAFFIC_ERROR /* until this is defined */

STRUCTURE_REF (rsvp_error_ipv4);
struct rsvp_error_ipv4_ {
    rsvp_object base;
    ipaddrtype error_node;
    uchar flags;
#define RSVP_LUB_USED 0x01
    uchar error_code;
    ushort error_value;
};

/*
 * A list of folks who would like to know that a reservation got installed.
 */
STRUCTURE_REF (rsvp_confirm_ipv4);
struct rsvp_confirm_ipv4_ {
    rsvp_object base;
    ipaddrtype receiver;		/* source */
};

/*
 * An explicit specification of the scope for forwarding a RESV message.
 */
STRUCTURE_REF (rsvp_scope_ipv4);
struct rsvp_scope_ipv4_ {
    rsvp_object base;
    ipaddrtype source[0];		/* list of sources */
};

/*
 * Defines the reservation style plus style-specific information that
 * is not a FLOWSPEC or FILTER_SPEC object, in a RESV message.
 */
#define RSVP_DISTINCT (1 << 3)
#define RSVP_SHARED (2 << 3)
#define RSVP_SHARING_MASK (RSVP_DISTINCT | RSVP_SHARED)

#define RSVP_SCOPE_WILD 1
#define RSVP_SCOPE_EXPLICIT 2
#define RSVP_SCOPE_MASK 0x07

#define RSVP_WF_OPTION (RSVP_SHARED | RSVP_SCOPE_WILD)
#define RSVP_FF_OPTION (RSVP_DISTINCT | RSVP_SCOPE_EXPLICIT)
#define RSVP_SE_OPTION (RSVP_SHARED | RSVP_SCOPE_EXPLICIT)

STRUCTURE_REF (rsvp_style);
struct rsvp_style_ {
    rsvp_object base;
    ulong reservation_type;
};

/*
 * Defines a desired QoS, in a RESV message.
 */
enum    qos_service_type {
    QOS_CONTROLLED_LOAD=    5,		/* Controlled Load Service */
    QOS_GUARANTEED =        3,		/* Guaranteed service */
    QOS_PREDICTIVE =        2,		/* Predictive service */
    QOS_CONTROLLED_DELAY =  1,		/* Controlled delay service */
    QOS_TSPEC =             0		/* Traffic specification */
    };

enum rsvp_spec_types { INTSRV = 2 };

enum intsrv_services {
	INTSRV_GENERAL = 1,
	INTSRV_GTD = 2,
	INTSRV_CLOAD = 5
};

#define INTSRV_MAX_DELAY ((ulong) 0xFFFFFFFF)
enum intsrv_field_name {
	INTSRV_NON_IS_HOPS = 1, INTSRV_COMPOSED_NON_IS_HOPS,
	INTSRV_IS_HOPS, INTSRV_COMPOSED_IS_HOPS,
	INTSRV_PATH_BANDWIDTH, INTSRV_MIN_PATH_BANDWIDTH,
	INTSRV_IF_LATENCY, INTSRV_PATH_LATENCY,
	INTSRV_MTU, INTSRV_COMPOSED_MTU,

	INTSRV_TOKEN_BUCKET_TSPEC = 127,
	INTSRV_GTD_RSPEC = 130,

    	INTSRV_DELAY = 131,	/* Gtd Parameter C - Max Delay Bound - bytes */
	INTSRV_MAX_JITTER,	/* Gtd Parameter D - Max Jitter */
    	INTSRV_E2E_DELAY,	/* Gtd Parameter Ctot */
	INTSRV_E2E_MAX_JITTER,	/* Gtd Parameter Dtot */
    	INTSRV_SHP_DELAY,	/* Gtd Parameter Csum */
	INTSRV_SHP_MAX_JITTER	/* Gtd Parameter Dsum */
};

STRUCTURE_REF(intsrv_tspec);
struct intsrv_tspec_ {
    ushort	version;		/* version upper two bits=0, rest is fill */
    ushort	length_in_words;	/* length of entire spec */
					/* = sizeof(intsrv_tspec)/4-1 */
    uchar	service_id;		/* INTSRV_GENERAL */
    uchar	fill;
    ushort	service_length;		/* length of service field */
					/* = sizeof(intsrv_tspec)/4-2 */

    uchar	parameter_id_tspec;	/* INTSRV_TOKEN_BUCKET_TSPEC */
    uchar	flags_tspec;		/* 0 */
    ushort	parameter_length;	/* = sizeof(intsrv_tspec)/4-3 */

    ieee_float	rate;
    ieee_float	depth;
    ieee_float	peak;
    ulong	min_unit;
    ulong	max_unit;
};

STRUCTURE_REF(intsrv_cload_flowspec);
struct intsrv_cload_flowspec_ {
    ushort	version;		/* version upper two bits=0, rest is fill */
    ushort	length_in_words;	/* length of entire spec */
					/* = sizeof(intsrv_cload_flowspec)/4-1 */
    uchar	service_id;		/* INTSRV_CLOAD */
    uchar	fill;
    ushort	service_length;		/* length of cload service field */
					/* = sizeof(intsrv_cload_flowspec)/4-2 */

    uchar	parameter_id_tspec;	/* INTSRV_TOKEN_BUCKET_TSPEC */
    uchar	flags_tspec;		/* 0 */
    ushort	param_length;		/* length of cload service field */
					/* = sizeof(intsrv_cload_flowspec)/4-3 */
    ieee_float	rate;
    ieee_float	depth;
    ieee_float	peak;
    ulong	min_unit;
    ulong	max_unit;
};

STRUCTURE_REF(intsrv_real_object);
struct intsrv_real_object_ {
    uchar	parameter_id;
    uchar	flags;
    ushort	length;
    ieee_float	value;	
};
STRUCTURE_REF(intsrv_integer_object);
struct intsrv_integer_object_ {
    uchar	parameter_id;
    uchar	flags;
    ushort	length;
    ulong	value;	
};
static inline boolean
rsvp_valid_integer_object(intsrv_integer_object *object,
			  uchar parameter_id)
{
    if (object->parameter_id != parameter_id ||
	object->flags != 0 || object->length != 1)
	return FALSE;
    else
	return TRUE;
}
static inline void
rsvp_build_integer_object(intsrv_integer_object *object,
			  uchar parameter_id,
			  ulong	value)
{
    object->parameter_id = parameter_id;
    object->flags = 0;
    object->length = 1;
    object->value = value;
}
static inline boolean
rsvp_valid_real_object(intsrv_real_object *object,
			uchar parameter_id)
{
    if (object->parameter_id != parameter_id ||
	object->flags != 0 || object->length != 1)
	return FALSE;
    else
	return TRUE;
}
static inline void
rsvp_build_real_object(intsrv_real_object *object,
			uchar parameter_id,
			ulong	value)
{
    object->parameter_id = parameter_id;
    object->flags = 0;
    object->length = 1;
    object->value = value;
}
			  

STRUCTURE_REF(intsrv_adspec_general);
struct intsrv_adspec_general_ {
    uchar	service_id;		/* INTSRV_GENERAL */
#define RSVP_ADSPEC_BREAK 0x80
    uchar	break_bit;		/* set break bit for non-rsvp hop found */
    ushort	service_length;	/* length of general service field */
				/* = sizeof(intsrv_adspec_general)/4-1 */
    intsrv_integer_object	first_object[0];
    intsrv_integer_object	number_is_hops;
    intsrv_real_object		bandwidth;
    intsrv_real_object		latency;
    intsrv_integer_object	path_mtu;
};
STRUCTURE_REF(intsrv_gtd_adspec);
struct intsrv_gtd_adspec_ {
    uchar	service_id;	/* INTSRV_GTD */
    uchar	break_bit;		/* set break bit for non-rsvp hop found */
    ushort	service_length;	/* length of general service field */
				/* = sizeof(intsrv_gtd_adspec)/4-1 */
    intsrv_integer_object	first_object[0];
    intsrv_integer_object	ctot;
    intsrv_integer_object	dtot;
    intsrv_integer_object	csum;
    intsrv_integer_object	dsum;
    intsrv_integer_object	other[0];
};

/*
 * As per the spec (draft-ietf-intserv-rsvp-use-xx.txt), the minimum length
 * of a GS data fragment is 8 *WORDS*.
 */
#define RSVP_MIN_GTD_SPEC_LENGTH 8

STRUCTURE_REF(intsrv_cload_adspec);
struct intsrv_cload_adspec_ {
    uchar	service_id;	/* INTSRV_CLOAD */
    uchar	break_bit;		/* set break bit for non-rsvp hop found */
    ushort	service_length;	/* length of general service field */
				/* = sizeof(intsrv_cload_adspec)/4-1 */
    intsrv_integer_object	first_object[0];
};
STRUCTURE_REF(intsrv_template_adspec);
struct intsrv_template_adspec_ {
    ushort	version;		/* version upper two bits=0, rest is fill */
    ushort	length_in_words;	/* length of entire spec */
					/* = sizeof(intsrv_template_adspec)/4-1 */

    intsrv_adspec_general	general;
    intsrv_gtd_adspec		gtd;
    intsrv_cload_adspec		cload;
};

STRUCTURE_REF(intsrv_gtd_flowspec);
struct intsrv_gtd_flowspec_ {
    ushort	version;		/* version upper two bits=0, rest is fill */
    ushort	length_in_words;	/* length of entire spec */
					/* = sizeof(intsrv_gtd_flowspec)/4-1 */
    uchar	service_id;		/* INTSRV_GTD */
    uchar	fill;
    ushort	service_length;		/* length of gtd service field */
					/* = sizeof(intsrv_gtd_flowspec)/4-2 */

    uchar	parameter_id_tspec;	/* INTSRV_TOKEN_BUCKET_TSPEC */
    uchar	flags_tspec;		/* 0 */
    ushort	param1_length;		/* length of gtd service field */
					/* = 5 words */
    ieee_float	rate;
    ieee_float	depth;
    ieee_float	peak;
    ulong	min_unit;
    ulong	max_unit;

    uchar	parameter_id_rspec;	/* INTSRV_GTD_RSPEC */
    uchar	flags_rspec;		/* 0 */
    ushort	param2_length;		/* 2 */
    ieee_float	requested_rate;		/* same rate as 'rate'? */
    ulong	slack;
};

STRUCTURE_REF(isi_flowspec);
struct isi_flowspec_ {
    enum qos_service_type qos_service;	/* type of specification */
    ulong maximum_burst;		/* token bucket depth */
    ulong average_bit_rate;		/* token bucket rate */
    ulong maximum_e2e_delay;		/* target delay (ms) */
    ulong service_level;		/* service level */
};
STRUCTURE_REF(isi_tspec);
struct isi_tspec_ {
    ulong maximum_burst;
    ulong average_bit_rate;
};

STRUCTURE_REF (rsvp_flowspec);
struct rsvp_flowspec_ {
    rsvp_object base;
    union {
	char space[128];		/* make space for large specs */
	intsrv_gtd_flowspec gtd;	/* guaranteed flow spec */
	intsrv_cload_flowspec cload;	/* controlled load flow spec */
    } flow;
};

/*
 * Defines a subset of session data packets that should receive the
 * desired QoS (specified by an FLOWSPEC object), in a RESV message.
 */
STRUCTURE_REF (rsvp_filter_ipv4);
struct rsvp_filter_ipv4_ {
    rsvp_object base;
    ipaddrtype source;			/* source sending data */
    ushort unused;
    ushort udp_source_port;		/* port number */
};

/*
 * Contains a sender IP address and perhaps some additional
 * demultiplexing information to identify a sender, in a PATH
 * message.
 */
STRUCTURE_REF (rsvp_template_ipv4);
struct rsvp_template_ipv4_ {
    rsvp_object base;
    ipaddrtype source;			/* source sending data */
    ushort unused;
    ushort udp_source_port;		/* port number */
};

/*
 * Defines the traffic characteristics of a sender's data stream, in a
 * PATH message.
 */
STRUCTURE_REF (rsvp_tspec);
struct rsvp_tspec_ {
    rsvp_object base;
    union {
	char space[128];		/* make space for large specs */
	intsrv_tspec gtd;		/* guaranteed t spec */
	intsrv_tspec cload;		/* controlled load t spec */
    } t;
};

/*
 * Carries an Adspec containing OPWA data, in a PATH message.
 */
STRUCTURE_REF (rsvp_adspec);
struct rsvp_adspec_ {
    rsvp_object base;
    union {
	char space[128];		/* make space for large specs */
	intsrv_template_adspec gtd;	/* guaranteed advertising spec */
    } ad;
};

/*
 * Carries information that will allow a local policy module to decide
 * whether an associated reservation is administratively permitted.
 * May appear in a PATH or RESV message.
 */
STRUCTURE_REF (rsvp_policy);
struct rsvp_policy_ {
    rsvp_object base;
};

/*
 * Contains cryptographic data to authenticate the originating node,
 * and perhaps to verify the contents, of this RSVP message.
 */
#define RSVP_TYPE_CRYPTO_INTEGRITY 1
STRUCTURE_REF (rsvp_integrity);
struct rsvp_integrity_ {
    rsvp_object base;
    unsigned key_number;
    unsigned sequence;
    uchar md5_digest[16];
};

void rsvp_display_flowspec(void *object);
void rsvp_display_tspec(void *object);
void rsvp_display_adspec(void *object);

#endif /* __RSVP_OBJECT__ */
