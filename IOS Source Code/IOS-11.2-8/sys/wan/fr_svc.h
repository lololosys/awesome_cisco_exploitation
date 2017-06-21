/* $Id: fr_svc.h,v 3.1.64.11 1996/08/03 23:41:26 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_svc.h,v $
 *------------------------------------------------------------------
 * fr_svc.h 
 *
 * May 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_svc.h,v $
 * Revision 3.1.64.11  1996/08/03  23:41:26  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.1.64.10  1996/07/29  22:11:32  snyder
 * CSCdi64472:  declare some arrays const in shr_frame, shr_frame_svc
 *              2380 out of data, 40 image
 * Branch: California_branch
 *
 * Revision 3.1.64.9  1996/07/03  01:57:47  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1.64.8  1996/06/30  22:23:40  shankar
 * CSCdi59475:  Change units for Be/Bc from bytes to bits
 * Branch: California_branch
 * Images that do not include SVCs fail to link.
 *
 * Revision 3.1.64.7  1996/06/27  21:38:18  skoh
 * CSCdi59475:  Change units for Be/Bc from bytes to bits
 * Branch: California_branch
 *
 * Revision 3.1.64.6  1996/06/22  19:27:31  shankar
 * CSCdi61102:  Parameter Negotiation for SVCs needs a little more checking
 * Branch: California_branch
 *
 * Revision 3.1.64.5  1996/06/21  22:10:07  shankar
 * CSCdi60623:  Calling party router hung in FR SVC test with Cascade
 * switch
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/06/12  14:09:11  shankar
 * CSCdi57333:  FR SVC Timer handler needs SVC event intelligence
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/05/24  21:21:24  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/05/15  05:54:41  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/25  23:23:34  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/24  04:11:26  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/11/09  13:49:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/01  18:47:54  shankar
 * Initial commit
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FR_WATCHED_BOOL__
#define __FR_WATCHED_BOOL__
#include "../h/sched.h"
#endif

watched_queue *iframeQ;

/* 
 * direction definition to help the svc_ie_print routine. 
 */
# define	IN 	1
# define 	OUT	2

/*
 * Generic Table Handling for Q.933 State Transtions.
 * Used by the Q.933 L3 state machines.
 */
typedef void (*EVENTFUNC)(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
				uint, struct fr_svc_params *);

/* 
 * Each entry in the table consists of a (event, function) combination. 
 * In any given state, when "message" is received, "EventFunc" is 
 * executed. 
 */
typedef struct StateTable_ {
    uchar               message;                /* Incoming Message*/
    EVENTFUNC		EventFunc;
}   StateTable;

/*
 * Generic Table Handling for NLI State Transtions.
 */
typedef void (*NLEVENTFUNC)(fr_svc_nli_type *, paktype *, staticmap_type *, 
				uint, uint);

typedef struct NLStateTable_ {
    uchar               message;                /* Incoming Message*/
    NLEVENTFUNC		EventFunc;
}   NLStateTable;


/*
 * CIR and other Defaults
 * No particular rationale for picking 56000. We want to provision
 * as little as possible and force the user to configure parameters.
 */
# define FR_SVC_IN_CIR_DEF		56000
# define FR_SVC_OUT_CIR_DEF		56000
# define FR_SVC_MIN_IN_CIR_DEF		56000
# define FR_SVC_MIN_OUT_CIR_DEF		56000	
# define FR_SVC_IN_CBS_DEF		7000		/* bytes */
# define FR_SVC_OUT_CBS_DEF		7000		/* bytes */
# define FR_SVC_IN_EBS_DEF		7000		/* bytes */
# define FR_SVC_OUT_EBS_DEF		7000		/* bytes */
# define FR_SVC_MAG_DEF			3		/* power of 10 */
# define FR_SVC_MULT_DEF		56		/* 56000 */
# define FR_SVC_IDLE_TIMER		120 * ONESEC	
# define FR_SVC_IN_FMIF			1500
# define FR_SVC_OUT_FMIF		1500

# define FR_SVC_DEF_PRIORITY		1	/* exclusive */
						/* see enum svc_type */

# define FR_SVC_HOLDQ_SIZE		40

/* 
 * User side timer defaults
 * Page 184-186, Q.931
 */
# define T303_DEFAULT		4 * ONESEC
# define T305_DEFAULT		30 * ONESEC
# define T308_DEFAULT		4 * ONESEC
# define T309_DEFAULT		90 * ONESEC
# define T310_DEFAULT		10 * ONESEC
# define T322_DEFAULT		4 * ONESEC

# define T303_RETRY_COUNT		2
# define T308_RETRY_COUNT		2
# define T322_RETRY_COUNT		7

/* 
 * We need a holdq for outgoing data while a call is being setup.
 */
# define NLI_DEF_Q_SIZE			10

/* 
 * Randomly picked a max value for max calls per IDB. There is 
 * no functional dependence on this value.
 */
# define MAX_CALL_REF_PER_IDB           64
# define MAX_CALL_ID_PER_IDB            MAX_CALL_REF_PER_IDB

/* 
 * If the MAX_MAPS_PER_NLI value is changed, then the fr_svc_nli_type
 * structure in frame_relay.c needs a corresponding change. 
 */
# define MAX_MAPS_PER_NLI		32

# define MAX_ILLEGAL_IE_PER_MSG		2


/* 
 * Assuming that 20 bytes will cover all calling and 
 * called party addresses. 
 */
# define MAX_ADDRESS_LENGTH		20

/* 
 * is the extension bit 1 or 0?
 * Define masks for the extension bit and the information.
 */
# define EXT_BIT_1			0x80
# define EXT_BIT_0			0x00
# define EXT_BIT_0_MASK			0x7F
# define  EXT_BIT_1_MASK		0x80

/* 
 * The message header for q.933 call control. 
 */
typedef struct message_header_ {
	uchar	protocol_discriminator;	/* 0000 1000 */
	uchar 	call_reference[3];
	uchar 	message_type; 		/* Call control msg. type */
	uchar 	data[0];		/* Information Elements */
} message_header;

/* 
 * Protocol Discriminator
 * simplest of the whole lot, must always be of type ANSI.
 * value is 0x08.
 */
# define PROT_DISC_IE_ID		0x08
# define PROT_DISC_IE_LENGTH		1

/* 
 * Call Reference
 * Ussed to identify the call or facility registration/cancellation
 * request at the user-network interface to which the particular 
 * message applies. It does not have a end-end significance. 
 */
# define MAX_CALL_REF_PER_BOX		0xFFFF
# define CALL_REF_IE_ID			0x02
# define CALL_REF_IE_LENGTH		0x02	
# define CALL_REF_TOTAL_LENGTH		0x03
# define CALL_REF_FLAG_MASK		0x7F
# define FR_SVC_CALL_REF                0x02
# define FR_SVC_PID                     0xFF
# define T_ACTIVITY_DEF                 30 * ONESEC

typedef struct call_reference_ {
	uchar 	length; 		/* should always be 2 */
	uchar 	byte2;			/* 0000 0000 */
	uchar 	byte3;	 		/* 0000 0000 */
} call_reference;

/* 
 * The purpose of the bearer capability IE is to indicate a requested CCITT
 * recommendation I.233 bearer service to be provided by the network. For 
 * FR SVCs, digital, frame mode is always chosen.
 * Length is 4 octets, Identifier is 0x04.
 * octet 3 = 1 00 01000b = 0x88. CCITT coding standard, digital info
 * octet 4 = 1 01 00000b = Frame mode, reserved bits.
 * 
 * Ref: Page 28 of Q.933
 */
# define BEARER_CAPABILITY_IE_ID	0x04
# define BEARER_CAP_IE_LENGTH 		3
# define BEARER_CAP_HDR_LENGTH 		2
# define BEARER_CAP_IE_TOTAL_LENGTH 	5
# define BEARER_CAP_BYTE3		0x88
# define BEARER_CAP_BYTE4		0xA0
# define BEARER_CAP_BYTE5_A		0xCE
# define BEARER_CAP_BYTE5_B		0xCF
typedef struct bearer_capability_ {
	uchar 	ie_id;			/* IE ID is 0x04 */
	uchar	length;			/* length is 3 */
	uchar 	byte3;			/* digital_info, coding_std, ext */
	uchar 	byte4;			/* rsvd, xfer_mode (frame), ext */
	uchar	byte5;			/* layer 2 protocol information */
} bearer_cap_ie;

/* 
 * This IE indicates the requested or assigned DLCI. 
 * Length is 4 octets.  Identifier is 0x19.
 * Bit 8 of octet 3 is 0.
 * Bit 7 of octet 3 id the preferred/exclusive bit.
 * Bit 6 of octet 3 is the most significant DLCI bit. 
 * Bit 8 of octet 4 is 1.
 * Bits 7-4 are DLCI bits.
 * Bits 1-3 of octet 4 are reserved.
 */
# define DLCI_IE_ID			0x19
# define DLCI_IE_TOTAL_LENGTH 		4
# define DLCI_IE_HDR_LENGTH 		2
# define DLCI_IE_LENGTH 		2
# define DLCI_SPARE_BITS		3
# define DLCI_LOW_BITS_MASK		0x000F
# define DLCI_HI_BITS_MASK		0x003F
# define DLCI_EXCLUSIVE			0x40
# define DLCI_PREFERRED			0x00
typedef struct dlci_ie_ {
	uchar 	ie_id;			/* Indentifier is 0x19 */
	uchar	length;			/* Max length is 4 octets */
	uchar	dlci_u;			/* most significant dlci bits */
	uchar 	dlci_l;			/* Least significant dlci bits */
} dlci_ie;

/* 
 * Purpose is to indicate requested core service quality
 * parameter values to be used for the call. 
 * Maximum length is 27 octets, Identifier is 0x48
 */
# define LINK_LAYER_CORE_PARAM_IE_ID	0x48
# define LINK_CORE_IE_HDR_LENGTH	2
# define LINK_CORE_IE_MAX_LENGTH	29
# define CONNECT_LINK_CORE_IE_MAX_LENGTH	24
# define LINK_CORE_IE_TOTAL_LENGTH	31
# define MAX_FMIF_ID			0x09
# define THROUGHPUT_ID			0x0A
# define MIN_ACCEP_THROUGHPUT_ID	0x0B
# define COMMITED_BURST_SIZE_ID		0x0D
# define EXCESS_BURST_SIZE_ID		0x0E
# define CBS_MAG_ID			0x10
# define EBS_MAG_ID			0x11
# define UPPER_MUL_BITS			0x000F
# define BIT_FIELD_SIZE			7
# define MAG_LEFT_SHIFT			4
# define LOWER_BITS_MASK		0x007F
# define UPPER_BITS_MASK		0x7F00

typedef struct link_core_ie_ {
	uchar 	ie_id;				/* Identifier is 0x48 */
	uchar 	length;				/* length of IE */
	uchar 	max_fmif_size_field_id;		/* frame mode info field, 0x09 */

	uchar	max_out_fmif_size_a;		/* out FMIF size upper bits */
	uchar	max_out_fmif_size_b;		/* out FMIF size lower bits */

	uchar	max_inc_fmif_size_a;		/* in FMIF size upper bits */
	uchar	max_inc_fmif_size_b;		/* in FMIF size upper bits */

	uchar 	thruput_field_id;		/* Identifier is 0x0A */
	uchar   out_thruput_a;			/* Throughput and magnitude */
	uchar 	out_multiplier;			/* rest of multiplier */

	uchar	inc_thruput_b; 			/* Throughput and magnitude */
	uchar 	inc_multiplier;			/* rest of multiplier */

	uchar	minimum_thruput_id;		/* Identifier is 0x0B */
	uchar   out_min_thruput_a;		/* Min Throughput and magnitude */
	uchar   out_min_thruput_mult;		/* rest of multiplier */

	uchar   inc_min_thruput_a;		/* Min Throughput and magnitude */
	uchar   inc_min_thruput_mult;		/* rest of multiplier */

	uchar	cbs_id;				/* Committed Burst size */
	uchar   out_cbs_a;			/* Upper order bits of outgoing CBS */
	uchar   out_cbs_b;			/* Lower order bits of outgoing CBS */

	uchar   inc_cbs_a;			/* Upper order bits of incoming CBS */
	uchar   inc_cbs_b;			/* Lower order bits of incoming CBS */

	uchar 	ebs_id;				/* Excess Burst Size */
	uchar   out_ebs_a;			/* Upper order bits of outgoing EBS */
	uchar   out_ebs_b;			/* Lower order bits of outgoing EBS */

	uchar   inc_ebs_a;			/* Upper order bits of incoming ECBS */
	uchar   inc_ebs_b;			/* Lower order bits of incoming ECBS */

	uchar	cbs_mag_id;			/* See FRFTC95.090 */
	uchar 	cbs_magnitude;			/* See FRFTC95.090 */

	uchar	ebs_mag_id;			/* See FRFTC95.090 */
	uchar 	ebs_magnitude;			/* See FRFTC95.090 */
} link_core_ie;

/* 
 * The link core IE within the CONNECT message follows a 
 * different format as compared to the SETUP message. 
 */
typedef struct connect_link_core_ie_ {
	uchar 	ie_id;				/* Identifier is 0x48 */
	uchar 	length;				/* length of IE */
	uchar 	max_fmif_size_field_id;		/* frame mode info field, 0x09 */

	uchar	max_out_fmif_size_a;		/* out FMIF size upper bits */
	uchar	max_out_fmif_size_b;		/* out FMIF size lower bits */

	uchar	max_inc_fmif_size_a;		/* in FMIF size upper bits */
	uchar	max_inc_fmif_size_b;		/* in FMIF size upper bits */

	uchar 	thruput_field_id;		/* Identifier is 0x0A */
	uchar   out_thruput_a;			/* Throughput and magnitude */
	uchar 	out_multiplier;			/* rest of multiplier */

	uchar	inc_thruput_b; 			/* Throughput and magnitude */
	uchar 	inc_multiplier;			/* rest of multiplier */

	uchar	cbs_id;				/* Committed Burst size */
	uchar   out_cbs_a;			/* Upper order bits of outgoing CBS */
	uchar   out_cbs_b;			/* Lower order bits of outgoing CBS */

	uchar   inc_cbs_a;			/* Upper order bits of incoming CBS */
	uchar   inc_cbs_b;			/* Lower order bits of incoming CBS */

	uchar 	ebs_id;				/* Excess Burst Size */
	uchar   out_ebs_a;			/* Upper order bits of outgoing EBS */
	uchar   out_ebs_b;			/* Lower order bits of outgoing EBS */

	uchar   inc_ebs_a;			/* Upper order bits of incoming ECBS */
	uchar   inc_ebs_b;			/* Lower order bits of incoming ECBS */

	uchar	cbs_mag_id;			/* See FRFTC95.090 */
	uchar 	cbs_magnitude;			/* See FRFTC95.090 */

	uchar	ebs_mag_id;			/* See FRFTC95.090 */
	uchar 	ebs_magnitude;			/* See FRFTC95.090 */
} connect_link_core_ie;
/* 
 * Calling party number ie
 * If included, it identifies the origin of a call
 * Length is variable. Identifier is 0x6C. 
 */
# define CALLING_PARTY_NUMBER_ID	0x6C
# define CALLING_PARTY_IE_HDR_LENGTH	2
# define TYPE_UNKNOWN_NUMBER		0x0
# define TYPE_INTERNATIONAL_NUMBER	0x1
# define NUMBER_IS_E164			0x01
# define NUMBER_IS_X121			0x03

# define NUMBER_TYPE_MASK		0x70
# define NUMBER_PLAN_MASK		0x0F
# define PRESNT_IND_MASK		0x60
# define SCREEN_IND_MASK		0x03
# define MAX_ADDRESS_LENGTH		20

typedef struct calling_party_number_ie_ {
	uchar 	ie_id;			/* IE ID is 0x6C */
	uchar 	length;			/* Length is variable */
	uchar 	numb_type_id;		/* E164 or X121, National/I'national */
	uchar	indicators;		/* presentation and screenning */
	uchar	address[MAX_ADDRESS_LENGTH];	/* Allocate maximum */
} calling_pty_number_ie;

/* 
 * Calling party subaddress IE
 * Length is ??. Identifier is 0x6D
 */
# define CALLING_PARTY_SUBADDR_ID		0x6D
# define CALLING_PARTY_SUBADDR_HDR_LNGTH	2
# define CALLING_PARTY_SUBADDR_LENGTH		1
# define USER_SPECIFIED_SUBADDR			0xA0

typedef struct calling_party_subaddr_ie_ {
	uchar 	ie_id;			/* IE ID is 0x6D */
	uchar	length;			/* Length is variable */
	uchar 	type_of_addr;		/* NSAP or user specified */
	uchar   address;		/* 1 byte is enough for now */
} calling_pty_subaddr_ie;

/* 
 * Called party number ie
 * Included by the user, Does not appear to be optional.
 * Identifies the destination of a call
 * Length is ??. Identifier is 0x70. 
 */
# define CALLED_PARTY_NUMBER_ID		0x70
# define CALLED_PARTY_HDR_LENGTH	2
typedef struct called_party_ie_ {
	uchar 	ie_id;			/* IE ID is 0x70 */
	uchar	length;			/* Length is variable */
	uchar 	numb_type_id;		/* E164 or X121, National or I'national*/
	uchar	address[MAX_ADDRESS_LENGTH];	/* Allocate maximum */
} called_pty_ie;

/* 
 * Called party subaddress IE
 * Length is ??. Identifier is 0x71
 */
# define CALLED_PARTY_SUBADDR_ID	0x71
# define CALLED_PARTY_SUBADDR_HDR_LNGTH	2
typedef struct called_party_subaddr_ie_ {
	uchar 	ie_id;			/* IE ID is 0x71 */
	uchar	length;			/* Length is variable */
	uchar 	type_of_addr;		/* numbering plan, type etc */
	uchar   address;		/* 1 byte is enough for now */
} called_pty_subaddr_ie;

/* 
 * Connected number IE
 * This IE indicates the number which is connected to a call. 
 * The connected number may be different from the called party
 * number because of changes during the lifetime of a call. 
 * Reference: Q.951 Document that talks about Calling Line 
 * and Connected Line Presentations and Restrictions.
 */
# define CONN_PARTY_NUMBER_ID			0x4C
# define CONN_PARTY_IE_HDR_LENGTH		2
typedef struct connected_party_ie_ {
	uchar 	ie_id;                  /* IE ID is 0x4C */
	uchar 	length;			/* Length is variable */
	uchar	numb_type_id;       	/* E164 or X121, National or International */
	uchar   indicators;        	/* See Q951 */
	uchar   address[MAX_ADDRESS_LENGTH];	/* Allocate maximum */
} connected_pty_ie ;

/* 
 * Connected number subaddress IE
 * Used to identify the subaddress associated with the connected number 
 * of a call. 
 * Reference: Q.951
 */
# define CONN_PARTY_NUMBER_SUBADDR_ID		0x4D
# define CONN_PARTY_SUBADDR_IE_HDR_LENGTH	2
typedef struct connected_num_subaddr_ie_ {
	uchar 	ie_id;			/* IE ID is 0x4D */
	uchar	length;			/* Length is variable */
	uchar 	type_of_addr;		/* numbering plan, type etc */
	uchar   address;		/* 1 byte is enough for now */
} connected_num_subaddr_ie;

/* 
 * The purpose of the transit network selection IE is to 
 * identify one requested transit network. This IE may be 
 * repeated in a message to select a sequence of transit 
 * networks through which a call must pass. 
 * length is ??, Identifier is 0x78;
 */
# define TRANSIT_NETWORK_IE_ID		0x78
# define TRANSIT_NETWORK_HDR_LENGTH	2
# define TRANSIT_NETWORK_IE_LENGTH	4
# define USER_SPECIFIED_NETW		0x00
# define NATIONAL_NETW			0x02
# define INTERNATIONAL_NETW		0x03

# define UNKNOWN_NET_ID_PLAN		0x00
# define CARRIER_ID_PLAN		0x01
# define DATA_NETW_ID_PLAN		0x03
typedef struct transit_network_selection_ie_ {
	uchar 	ie_id;			/* IE ID is 0x78 */
	uchar	length;			/* length is variable */
	uchar 	nwk_type_plan;		/* Transit network, ignore */
	uchar   nwk_id;			/* ignore */
} transit_network_selection_ie;


/* 
 * Low layer compatibility IE
 * Provides a means which should be used for compatibility
 * checking by an addressed entity i.e. a remote user or
 * an interworking unit or a high layer function network node
 * addressed by the calling user. 
 * Max. length is 8 octets. Identifier is 0x7C. 
 */
# define LOW_LAYER_COMPATIBILITY_IE_ID	0x7C
# define LOW_LAYER_COMP_IE_LENGTH 	3
# define LOW_LAYER_COMP_IE_HDR_LENGTH 	2
# define LOW_LAYER_COMP_IE_TOTAL_LENGTH 5
# define LLC_IE_CODING_STNDRD		0x00
# define LLC_BYTE3			0x88
# define INFO_XFER_CAPABILITY		0x08
# define FRAME_XFER_MODE		0xA0
# define LLC_IE_CCITT_Q22		0xCE
typedef struct low_layer_compatibility_ie_ {
	uchar 	ie_id;			/* IE ID is 0x7C */
	uchar	length;			/* length is 8 bytes */
	uchar 	info_cdg_std;		/* unrestricted digital */
	uchar 	xfer_mode;		/* frame mode 01 */
	uchar 	user_info_l2_prot;	/* core aspects of Q922, 0F */
} low_layer_compatibility_ie;

/* 
 * Used to convey information between users.
 * Max length is 131 octets, Identifier is 0x7E.
 */
# define USER_USER_IE_ID		0x7E
# define USER_USER_IE_HDR_LENGTH	2
# define USER_USER_IE_LENGTH		2
typedef struct user_user_ie_ {
	uchar 	ie_id;		/* IE ID is 0x7E */
	uchar 	length;		/* length i svariable */
	uchar 	prot_disc;	/* protocol for info */
	uchar 	user_info;	/* user-user info */
} user_user_ie;

/* 
 * FMIF IE
 * The FMIF is the maximum number of data octets after the address field
 * and before the FCS field in a frame mode frame. If FMIF is symmetrical 
 * in both directions, octets 3c and 3d are absent and octets 3a and 3b
 * indicate the size in both directions. 
 * The FMIF is configurable using the N203 parameter. 
 */
# define FMIF_IE_ID		0x09
# define FMIF_HDR_LENGTH	1
# define FMIF_IE_LENGTH		2
# define FMIF_UPPER_BITS	0x7F
# define FMIF_LOWER_BITS	0x7F
# define FMIF_UPPER_BITS_SHIFT	7	/* Move bits to appropriate position */
					/* Get rid of extension bit */
typedef struct fmif_ie_ {
	uchar 	fmif_ie_id;
	uchar 	outgoing_fmif_a;
	uchar 	outgoing_fmif_b;
	uchar 	incoming_fmif_a;
	uchar 	incoming_fmif_b;
} fmif_ie;

/* 
 * Throughput IE
 * The throughput for the call. Throughput is the avaerage number of 
 * FMIF field bits transferred per second across a UNI in one direction
 * measured over a duration T. 
 */
# define THROUGHPUT_IE_ID	0x0A
# define THROUGHPUT_IE_HDR_LENGTH	1
# define THROUGHPUT_IE_LENGTH		2
# define THRGHPT_MULT_UPPER_BITS	0x0F
# define THRGHPT_MULT_LOWER_BITS	0x7F
# define THROUHPUT_MSB_BITS		7
# define THRGHPT_MAG_MASK		0x07
typedef struct throughput_ {
	uchar 	throughput_ie_id;
	uchar 	out_mag_mult_a;
	uchar 	out_mult_b;
	uchar 	inc_mag_mult_a;
	uchar 	inc_mult_b;
} throughput;

/* 
 * Minimum acceptable throughput
 * The lowest throughput value that the calling user is willing to accept 
 * for the call. If the network is unable to support this throughput, 
 * the call is cleared. 
 * ID is 0x0B
 */
# define MIN_ACCEPTABLE_THROUGHPUT	0x0B
# define MIN_THROUGHPUT_IE_HDR_LENGTH	1
# define MIN_THROUGHPUT_IE_LENGTH	2
# define MAGNITUDE_MASK			0x70
# define MULTIPLIER_MSB_MASK		0xF0
# define MULTIPLIER_LSB_MASK		0x7F

typedef struct min_accept_throughput_ie_ {
	uchar 	min_throughput_ie_id; 
	uchar   out_mag_mult_a;
	uchar   out_mult_b;
	uchar   inc_mag_mult_a;
	uchar   inc_mult_b;
} min_accept_throughput_ie;

/* 
 * Committed Burst Size
 * Maximum amount of data (in bits) that the network agrees to transfer 
 * under normal conditions, over a time T. This data may or may not be 
 * interrupted. T is calculated using the methods shown in Q.933. page 36.
 */
# define COMMITTED_BURST_SZ_IE_ID	0x0D
# define CBSZ_IE_HDR_LENGTH		1
# define CBSZ_IE_LENGTH			2
# define BURST_SIZE_MASK		0x7F
# define BURST_SIZE_SHIFT		7

typedef struct committed_burst_size_ie_ {
	uchar 	comm_burst_sz_ie_id;
	uchar 	out_cbs_a;
	uchar   out_cbs_b;
	uchar 	inc_cbs_a;
	uchar   inc_cbs_b;
} committed_burst_size_ie;

/* 
 * Excess burst size
 * The maximum amount of uncommitted data in bits that the network will 
 * attempt to deliver over a time T. this data may appear in one frame or 
 * many frames. Excess burst frames may be marked as DE by the network. 
 * Identifier is 0x0E.
 */

# define EXCESS_BURST_SZ_IE_ID		0x0E
# define EBSZ_IE_HDR_LENGTH		1
# define EBSZ_IE_LENGTH			2

typedef struct excess_burst_size_ie_ {
	uchar 	excess_burst_sz_ie_id;
	uchar 	out_ebs_a;
	uchar   out_ebs_b;
	uchar 	inc_ebs_a;
	uchar   inc_ebs_b;
} excess_burst_size_ie;

typedef struct cbs_mag_ie_ {
	uchar 	cbs_mag_ie_id;
	uchar 	cbs_mag;
} cbs_mag_ie;

typedef struct ebs_mag_ie_ {
	uchar 	ebs_mag_ie_id;
	uchar 	ebs_mag;
} ebs_mag_ie;

/* 
 * Illegal IE
 */

typedef struct illegal_ie_ {
	uchar illegal_ie_id;
	uchar length;
} illegal_ie;

/* 
 * We are overlaying a template over a packet buffer. 
 * need to be careful and use PACKED generously. 
 */
typedef struct svc_setup_msg_ {
	struct 	message_header_ PACKED(msgh);
	struct	bearer_capability_ PACKED(bcie);
	/* 
	 * As per page 12 of the FR SVC IA, the dlci 
	 * IE is NOT included in the user to network 
	 * direction. TEMPORARY
	 */
	struct 	link_core_ie_ PACKED(llcoreie);
	struct 	calling_party_number_ie_ PACKED(cgpie);
	struct 	calling_party_subaddr_ie_ PACKED(cgpsie);
	struct 	called_party_ie_ PACKED(cdpie);
	struct 	called_party_subaddr_ie_ PACKED(cdpsie);
	struct 	low_layer_compatibility_ie_ PACKED(llcie);
	struct 	user_user_ie_ PACKED(uuie);
} svc_setup_msg;

/* 
 * We are overlaying a template over a packet buffer. 
 * need to be careful and use PACKED generously. 
 */
typedef struct svc_call_proc_msg_ {
	struct 	message_header_ PACKED(msgh);
	struct 	dlci_ie_ PACKED(dlcie);
} svc_call_proc_msg;

/* 
 * We are overlaying a template over a packet buffer. 
 * need to be careful and use PACKED generously. 
 */
typedef struct svc_connect_msg_ {
	struct 	message_header_ PACKED(msgh);
	struct 	dlci_ie_ PACKED(dlcie);
	struct 	connect_link_core_ie_ PACKED(llcoreie);
	struct 	connected_party_ie_ PACKED(cdpie);	
	struct 	connected_num_subaddr_ie_ PACKED(cdpsie); 	
	struct 	user_user_ie_ PACKED(uuie);
} svc_connect_msg;

/* 
 * Cause_ie
 * SEE page 16 of the SVC Implementation agreement for details.
 * The cause IE is used to describe the reason for generating certain 
 * messages, to provide diagnostic information in the event of procedural
 * errors and to indicate the location of the cause originator.
 */

# define CAUSE_IE_ID		0x08
# define CAUSE_IE_HDR_LENGTH	2
# define CAUSE_IE_LENGTH	3
# define CODING_STD_LOCATION	0x80
typedef struct cause_ie_ {
	uchar 	ie_id;			/* IE ID is 0x08 */
	uchar	length;			/* variable length */
	uchar 	cdg_std_location;	/* ignore */
	uchar 	cause_value;		/* put in a cause value */
	uchar 	diagnostics;		/* ignore */
} cause_ie;

/* 
 * We are overlaying a template over a packet buffer. 
 * need to be careful and use PACKED generously. 
 */
typedef struct svc_disconnect_msg_ {
	struct 	message_header_ PACKED(msgh);
	struct 	cause_ie_ PACKED(cie);
} svc_disconnect_msg;

/* 
 * We are overlaying a template over a packet buffer. 
 * need to be careful and use PACKED generously. 
 */
typedef struct svc_release_msg_ {
	struct 	message_header_ PACKED(msgh);
	struct 	cause_ie_ PACKED(cie);
} svc_release_msg;

typedef struct svc_release_msg svc_rel_complete_msg;

/* 
 * call_state_ie
 * Describes the current state of a call 
 * Maximum length is 3 octets. 
 * Ref: Q931, page 72-73 and Q933, page24.
 */
# define CALL_STATE_IE_ID		0x14
# define CALL_STATE_IE_HDR_LENGTH	2
typedef struct 	call_state_ie_ {
	uchar	ie_id;
	uchar	length;
	uchar 	call_state;
} call_state_ie;

/* 
 * We are overlaying a template over a packet buffer. 
 * need to be careful and use PACKED generously. 
 */
typedef struct svc_status_msg_ {
	struct	message_header_ PACKED(msgh);
	struct 	cause_ie_ PACKED(cie);
	struct 	call_state_ie_ PACKED(csie);
} svc_status_msg;

/* 
 * We are overlaying a template over a packet buffer. 
 * need to be careful and use PACKED generously. 
 */
typedef struct svc_status_enq_ {
	struct  message_header_ PACKED(msgh);
} svc_status_enq;


/* 
 * Allow the user many debugging options. Remove any that 
 * are deemed unnecessary once the producy matures. 
 */
# define L3_DBG		fr_l3_debug || fr_svc_detd_debug
# define L3CC_DBG	fr_l3_debug || fr_svc_detd_debug
# define L3IE_DBG	fr_l3ie_debug || fr_svc_detd_debug
# define NLI_DBG	fr_nli_debug || fr_svc_detd_debug

/* 
 * generic definition and help string array for various things.
 * Used primarily by debug statements. 
 */
const typedef struct generic_def_ {
	ushort 	type;
	uchar 	*help;
} generic_def;

/* 
 * List of all known NLI messages. 
 */
enum nli_msgs {
	NL_CALL_REQ = 1, 
	NL_CALL_IND, 
	NL_CALL_CNF,
	NL_CALL_REJ, 
	NL_REL_REQ, 
	NL_REL_IND, 
	NL_REL_CNF, 
	NL_REL_REJ, 
	L3_CALL_REL_REQ, 
	L3_CALL_REQ, 
	NL_CALL_ACK, 
	NL_CALL_NACK, 
	NL_MSG_UNKNOWN
};

/* 
 * List of all known NLI states. 
 */
enum nli_state {
	STATE_NL_NULL, 
	STATE_NL_CALL_REQ, 
	STATE_NL_CALL_IND, 
	STATE_NL_CALL_CNF,
	STATE_NL_CALL_REJ, 
	STATE_NL_REL_REQ, 
	STATE_NL_REL_IND, 
	STATE_NL_REL_CNF, 
	STATE_NL_REL_REJ,
	STATE_L3_CALL_REL_REQ,
	STATE_L3_CALL_REQ,
	STATE_NL_CALL_ACK,
	STATE_NL_CALL_NACK
};
/* 
 * Testing with NT switch resulted in a setting of EXCLUSIVE, always.
 * shankar 10/95
 */
enum svc_type {
	PREFERRED = 1,
	EXCLUSIVE
};


#define 	UNKNOWN_MSG	0xFF

/* 
 * CALL_PROCEEDING : Sent by the called user to calling user to
 * indicate that the requested call establishment has been initiated
 * and no more call establishment information will be accepted. 
 *
 * CONNECT : Sent by the called uer to the network and by the network
 * to the calling user to indicate call acceptance by the called user. 
 * 
 * DISCONNECT : Sent by the user to the network to clear the call 
 * or by the network to the user that the call has been cleared. 
 * 
 * RELEASE : Sent by the user or the network to indicate that the 
 * equipment sending the message has disconnected the call and intends
 * to release the dlci and the call reference and that the receiving
 * equipment should release the DLCI and prepare to release the 
 * call reference after sending RELEASE_COMPLETE. 
 * 
 * RELEASE_COMPLETE : User to network indicating that the equipoment
 * sending the message has released the call reference. 
 *
 * SETUP : Sent by the calling user to the network and by the 
 * network to the called user to initiate call establishment. 
 * 
 * STATUS : Message sent by the user or the network in response 
 * to a STATUS_ENQUIRY message or at any time to report certain 
 * error conditions as listed in @5.8 of Q.931.
 * 
 * STATUS_ENQUIRY : sent by the user or the network to solicit a 
 * STATUS message from the peer layer 3 entity. Sending a STATUS 
 * message in response to a STATUS_ENQUIRY is mandatory. 
 */


# define MSG_CALL_PROCEEDING 	0x02
# define MSG_SETUP		0x05
# define MSG_CONNECT 		0x07
# define MSG_DISCONNECT		0x45
# define MSG_RELEASE		0x4D
# define MSG_RELEASE_COMPLETE	0x5A
# define MSG_STATUS_ENQUIRY	0x75
# define MSG_STATUS		0x7D
# define MSG_UNKNOWN		0xAA

/* 
 * Randomly assigned identifiers. Feel free to change if 
 * a need arises. 
 * shankar 10/95. 
 *
 * NOTE: Values 0xf1 - 0xf8 are preserved for DL_ primitives as in fr_l2.h
 *	 If really needs to change this, check out layer 2 as well. 6/96
 */
# define T303_TIMEOUT		0xF9
# define T305_TIMEOUT		0xFA
# define T308_TIMEOUT		0xFB
# define T310_TIMEOUT		0xFC
# define T322_TIMEOUT		0xFD
# define TIDL_TIMEOUT		0xFE
# define RELEASE_REQ		0xE1
# define DISCONNECT_REQ		0xE2
# define SPL_RELEASE_REQ	0xE3
# define SETUP_REQUEST		0xE4
# define REJECT_REQUEST		0xE5
# define SETUP_RESPONSE		0xE6


enum call_states {
	STATE_NULL = 0, 		/* No call exists */
	STATE_CALL_INITIATED = 1, 	/* user requested call establishment */
					/* from network */
	STATE_CALL_PROCEEDING = 3, 	/* network has received all relevant */
					/* info to make the call*/
	STATE_CALL_PRESENT = 6, 	/* User has received a request but */
					/* has not responded yet */
	STATE_INCOMING_CALL_PROC = 9,	/* State for an incoming call when the */
					/* the user has sent acknowledgement */
					/* the user has received all relevant */
					/* info to start call establishment */
	STATE_ACTIVE = 10,		/* User has been awarded the call */
	STATE_DISCONNECT_REQUEST = 11,	/* User has requested the network to */
					/* clear the end-to-end connection */
	STATE_DISCONNECT_INDICATION = 12,
					/* User has been asked to disconnect */
					/* by the network */
	STATE_RELEASE_REQUEST = 19, 	/* User has requested the network to */
					/* release and is waiting for a response */
};

# define STATE_INC_CALL_PROC STATE_INCOMING_CALL_PROC
# define STATE_DISC_REQUEST STATE_DISCONNECT_REQUEST
# define STATE_DISC_IND STATE_DISCONNECT_INDICATION


# define NO_ERRORS 			0
# define IS_OKAY			NO_ERRORS
# define UNALLOC_NUMBER			1
# define NO_ROUTE_TO_TRANSIT_NET	2
# define NO_ROUTE_TO_DESTINATION	3
# define CHANNEL_UNACCEPTABLE		6
# define CALL_AWARDED_OVER_CHANNEL	7
# define NORMAL_CALL_CLEARING		16
# define USER_BUSY			17
# define NO_USER_RESPONDING		18
# define NO_ANSWER_FROM_USER		19
# define CALL_REJECTED			21
# define NUMBER_CHANGED			22
# define NON_SELECTED_USER_CLEARING	26
# define DEST_OUT_OF_ORDER		27
# define INVALID_NUMBER_FORMAT		28
# define FACILITY_REJECTED		29
# define RESP_TO_STATUS_ENQ		30
# define NORMAL_UNSPECIFIED		31
# define NO_CIRCUIT_AVAILABLE		34
# define NETWORK_OUT_OF_ORDER		38
# define TEMPORARY_FAILURE		41
# define SWITCH_CONGESTION		42
# define ACCESS_INFO_DISCARDED		43
# define REQ_CIRCUIT_NOT_AVAILABLE	44
# define RESOURCE_UNAVAILABLE		47
# define QOS_UNAVAILABLE		49
# define REQ_FACILITY_NOT_SUBSCRIBED	50
# define BEARER_CAP_NOT_AUTHORIZED	57
# define BEARER_CAP_NOT_AVAILABLE	58
# define SERVICE_NOT_AVAILABLE		63
# define BEARER_CAP_NOT_IMPLEMENTED	65
# define CHANNEL_TYPE_NOT_IMPLEMENTED	66
# define REQ_FACILITY_NOT_IMPLEMENTED	69
# define ONLY_DIG_INFO_IS_AVAILABLE	70
# define SERVICE_OPTION_NOT_IMPLEMENTED	79
# define INVALID_CALL_REF_VALUE		81
# define CHANNEL_DOES_NOT_EXIST		82
# define CALL_ID_IN_USE			84
# define NO_CALL_SUSPENDED		85
# define CALL_HAS_BEEN_CLEARED		86
# define INCOMPATIBLE_DESTINATION	88
# define INVALID_TRANSIT_NET_SELECTION	91
# define INVALID_MESSAGE		95
# define MANDATORY_IE_MISSING		96
# define MESSAGE_TYPE_NOT_IMPLEMENTED	97
# define MESSAGE_INCOMPATIBLE_WITH_CALL	98
# define IE_NOT_IMPLEMENTED		99
# define INVALID_IE_CONTENTS		100
# define MSG_NOT_COMPATIBLE_CALL_STATE	101
# define RECOVERY_ON_TIMER_EXPIRY	102
# define PROT_ERROR_UNSPECIFIED		111
# define INTERWORKING_UNSPECIFIED	127


/* 
 * The following are NOT q.931 sanctioned values. I 
 * grabbed a range of numbers in order to provide
 * better error handling. 
 * shankar 5/95
 */
# define GLOBAL_CALL_REF		161
# define INVALID_CALL_REF_LENGTH	162
# define INVALID_PROT_DISC		163
# define INVALID_IE_TYPE		164
# define INVALID_IE_LENGTH		165
# define UNKNOWN_BEARER_CAP_BYTE3	166
# define UNKNOWN_BEARER_CAP_BYTE4	167
# define UNKNOWN_BEARER_CAP_BYTE5	168
# define NO_MEMORY_TO_ASSIGN		169
# define NO_FR_MEMORY			170
# define END_OF_IE			171
# define ILLEGAL_EXTENSION_BIT		172
# define INVALID_CONTENTS_IN_FIELD	173
# define IE_TOO_LONG			174
# define IE_TOO_SHORT			175
# define PROBLEM_WITH_CALL_PARAMETERS	176

# define PROBLEM_WITH_REGISTRY_LOOP	190

typedef struct error_message_def_ {
	int 	index;
	ulong 	value;
	uchar	*name;
	uchar	*help;
} error_message_def;

/* Declarations */

/* IE procesing */
/* Top level functions */
extern process l3_ie_parse_process (void);
extern void l3_ie_parse (hwidbtype *, paktype *);

	/* Mandatory parts of incoming messages */
extern int check_protocol_discriminator (uchar **);
extern int check_call_reference (uchar **);
extern int check_dlci_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_bearer_cap_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_cg_party_number_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_cg_party_subaddr_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_cd_party_number_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_cd_party_subaddr_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_con_party_number_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_con_party_subaddr_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_xit_net_sel_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_ll_comp_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_fmif_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_thruput_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_min_thruput_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_cbs_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_ebs_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_cbs_mag_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern int check_ebs_mag_ie(fr_svc_info_type *, uchar **, uint, uchar);
extern void fr_svc_send_msg(hwidbtype *, fr_svc_info_type *, ushort,
				uint, struct fr_svc_params *);
extern void svc_outgoing_msg_init (void);
extern void svc_ie_print (char *, char *, char, uint);
extern boolean svc_process_l3_event(paktype *, fr_svc_info_type *, uint, uint, 
					struct fr_svc_params *, uint);

extern void fr_svc_init_ie_mgmt_area(fr_svc_info_type *);
extern fr_svc_info_type *fr_svc_lookup(hwidbtype *, uint);
extern uchar *fr_svc_bypass_illegal_ie(uchar **);
extern void svc_incoming_msg_init(void);
extern boolean check_if_ie_is_legal(uchar, uchar *);


/* Processing of various messages */
extern int process_incoming_setup_msg(paktype *, fr_svc_info_type *, uchar **, uint);
extern int process_incoming_call_proc_msg(paktype *, fr_svc_info_type *, uchar **, uint);
extern int process_incoming_connect_msg(paktype *, fr_svc_info_type *, uchar **, uint);
extern int process_incoming_disconnect_msg(paktype *, fr_svc_info_type *, uchar **, uint);
extern int process_incoming_release_msg(paktype *, fr_svc_info_type *, uchar **, uint);
extern int process_incoming_release_comp_msg(paktype *, fr_svc_info_type *, uchar **, uint);
extern int process_incoming_status_msg(paktype *, fr_svc_info_type *, uchar **, uint);
extern int process_incoming_status_enquiry_msg(paktype *, fr_svc_info_type *, uchar **, uint);

/* L3CC print routines */
extern char *l3cc_st(uchar);
extern char *l3cc_ev(uchar);
extern char *fr_svc_table_search(uchar, generic_def*);

/* print definition of given cause value */
extern generic_def cause_value_def[];
extern char *cause_def(uchar);

/* NL interface */
extern fr_svc_nli_type *fr_svc_nli_lookup(fr_idb_struct_type *, uint);
extern uint fr_svc_select_next_call_id(hwidbtype *);
extern void fr_svc_initialize_nli (fr_idb_struct_type *, paktype *, 
				fr_svc_nli_type *, staticmap_type *);
extern void fr_svc_rem_nli_from_idb_queue (fr_idb_struct_type *, staticmap_type *map,fr_svc_nli_type *);
extern void fr_svc_add_nli_to_idb_queue (fr_idb_struct_type *, fr_svc_nli_type *);
extern boolean fr_svc_l3if_action (fr_idb_struct_type *, paktype *, staticmap_type *, 
						uint, uchar);
extern boolean  svc_process_nli_event(fr_svc_nli_type *, paktype *, staticmap_type *, 
						uchar, uint);
extern void add_fr_svc_to_idb_queue(hwidbtype *, fr_svc_info_type *);
extern void rem_fr_svc_from_idb_queue (hwidbtype *, fr_svc_info_type *);

extern uint fr_svc_select_next_call_ref (hwidbtype *);

extern void NLx_Unknown (fr_svc_nli_type *, paktype *, staticmap_type *, uint, uint);
extern void NL0_CallInd(fr_svc_nli_type *, paktype *, staticmap_type *, uint, uint);
extern void NLx_CallInd(fr_svc_nli_type *, paktype *, staticmap_type *, uint, uint);
extern void NLx_CallCnf(fr_svc_nli_type *, paktype *, staticmap_type *, uint, uint);
extern void NLx_RelInd(fr_svc_nli_type *, paktype *, staticmap_type *, uint, uint);
extern void NL0_L3CallReq(fr_svc_nli_type *, paktype *, staticmap_type *, uint, uint);
extern void NLx_L3CallRej(fr_svc_nli_type *, paktype *, staticmap_type *, uint, uint);
extern void NLx_RelCnf(fr_svc_nli_type *, paktype *, staticmap_type *, uint, uint);
extern void NL1_CallInd(fr_svc_nli_type *, paktype *, staticmap_type *, uint, uint);
extern maplist_type *fr_svc_nli_map_search(fr_svc_info_type *);
extern boolean nli_parameter_negotiation(fr_svc_info_type *, maplist_type *);
extern boolean fr_svc_send_msg_to_nli(hwidbtype *, paktype *, fr_svc_info_type *, uint);

extern char *print_nl_event(uchar);
extern char *print_nl_state(uchar);

/* Formulate outgoing messages */
extern void add_prot_disc_ie(fr_svc_info_type *, uchar **, struct fr_svc_params *);
extern void add_dlci_ie(fr_svc_info_type *, uchar **, struct fr_svc_params *);
extern void add_link_lyr_core_ie(fr_svc_info_type *, uchar **, struct fr_svc_params *);
extern void add_connect_link_lyr_core_ie(fr_svc_info_type *, uchar **, struct fr_svc_params *);
extern void add_llc_ie(fr_svc_info_type *, uchar **, struct fr_svc_params *);
extern void add_user_user_ie(fr_svc_info_type *, uchar **, struct fr_svc_params *);
extern void add_cause_ie(fr_svc_info_type *, uchar **, struct fr_svc_params *);
extern void add_cs_ie(fr_svc_info_type *, uchar **, struct fr_svc_params *);

/* defines in fr_svc.c */
extern void fr_map_nvgen (staticmap_type *);
extern void fr_svc_mapclass_nvgen (mapclass_type *);
extern void fr_mapclass_command (parseinfo *);
extern void fr_params_init (struct fr_svc_params *);
extern void fr_svc_addr_block_init (svc_addr_block *);
extern int fr_svc_magmult_to_value(uchar, uint);
extern void fr_svc_t303_expired (hwidbtype *, fr_idb_struct_type *, fr_svc_info_type *);
extern void fr_svc_t305_expired (hwidbtype *, fr_idb_struct_type *, fr_svc_info_type *);
extern void fr_svc_t308_expired (hwidbtype *, fr_idb_struct_type *, fr_svc_info_type *);
extern void fr_svc_t309_expired (hwidbtype *, fr_idb_struct_type *, fr_svc_info_type *);
extern void fr_svc_t310_expired (hwidbtype *, fr_idb_struct_type *, fr_svc_info_type *);
extern void fr_svc_t322_expired (hwidbtype *, fr_idb_struct_type *, fr_svc_info_type *);
extern void fr_svc_init(void);
extern boolean fr_supported_protocol (uint);
extern void fr_map_command (parseinfo *);
extern watched_boolean *fr_svc_timer_bool;
extern forktype fr_svc_timer_handler(void);
extern fr_svc_info_type *fr_assign_svc_mem(void);
extern void fr_svc_l2_to_l3 (uchar, fr_idb_struct_type *, paktype *);
extern void fr_svc_l3_to_l2 (uchar, fr_idb_struct_type *, paktype *);
extern void fr_staticmap_add ( maplist_type *, hwaddrtype *, addrtype *, uint, 
						char *, svc_addr_block *);
extern void fr_svc_periodic(hwidbtype *);
/* fr_svc_l3_sdl_u.c */
extern fr_svc_nli_type *fr_svc_call_id_to_nli(paktype *, fr_idb_struct_type *, fr_svc_info_type *, uint);
extern void FRU0_Setup (hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU0_Unexpected (hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU0_Release (hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU0_ReleaseComplete(hwidbtype *, fr_svc_info_type *, paktype *, 
							uint, uint, struct fr_svc_params *);
extern void FRU0_Status(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);
extern void FRU0_SetupRequest(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU0_Unknown_Msg(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRUx_Unexpected(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRUx_Setup(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);
extern void FRUx_StatusEnquiry(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRUx_Status(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);
extern void FRUx_Disconnect(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);
extern void FRUx_Release(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);
extern void FRUx_ReleaseComplete(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);
extern void FRU0_Unknown_Msg(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);
extern void FRUx_Unknown_Msg(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);
extern void FRUx_Idletoolong(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);

extern void FRU1_CallProceeding(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU1_Connect(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU1_Disconnect(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU1_Release(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU1_ReleaseComplete(hwidbtype *, fr_svc_info_type *, paktype *, 
							uint, uint, struct fr_svc_params *);
extern void FRU1_DisconnectReq(hwidbtype *, fr_svc_info_type *, paktype *, 
							uint, uint, struct fr_svc_params *);
extern void FRU3_Connect(hwidbtype *, fr_svc_info_type *, paktype *, uint, uint, 
							struct fr_svc_params *);
extern void FRU3_ReleaseRequest(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRUx_ReleaseRequest(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRUx_SplReleaseRequest(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU6_Disconnect(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU6_Release(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU11_Disconnect(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU11_Release(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU12_Release(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU12_ReleaseRequest(hwidbtype *, fr_svc_info_type *, paktype *, 
							uint, uint, struct fr_svc_params *);
extern void FRU19_Release(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU19_ReleaseComplete(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void FRU19_Status(hwidbtype *, fr_svc_info_type *, paktype *, uint, 
							uint, struct fr_svc_params *);
extern void frame_relay_svc_startup (subsystype *subsys);
extern void lapf_init(void);
extern void fr_svc_determine_mag_and_mult (uint *, uint *);
extern void fr_svc_mapclass_remove (mapclass_type *class_ptr);
