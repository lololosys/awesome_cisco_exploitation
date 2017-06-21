/* $Id: fddi.h,v 3.1 1995/11/09 09:05:32 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/fddi.h,v $
 *------------------------------------------------------------------
 * fddi.h -- Fiber Distributed Data Interface
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddi.h,v $
 * Revision 3.1  1995/11/09  09:05:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:40  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ARPA encapsulation on FDDI -- who wants to do this.
 */
#define fddiarpastart e.encapc[ENCAPBYTES-14]
#define fddiarpafc e.encapc[ENCAPBYTES-13]
#define fddiarpadst e.encapc[ENCAPBYTES-14]  
#define fddiarpasrc e.encapc[ENCAPBYTES-8]
#define fddiarpatype e.encaps[ENCAPWORDS-1]

/*
 * 802 encapsulation on FDDI.
 */
#define fddiiso1start e.encapc[ENCAPBYTES-17]
#define fddiiso1fc e.encapc[ENCAPBYTES-16]
#define fddiiso1dst e.encapc[ENCAPBYTES-15]  
#define fddiiso1src e.encapc[ENCAPBYTES-9]
#define fddiiso1dsap e.encapc[ENCAPBYTES-3]
#define fddiiso1lsap e.encapc[ENCAPBYTES-2]
#define fddiiso1ctl e.encapc[ENCAPBYTES-1]

/*
 * 802 SNAP encapsulation on FDDI.
 */
#define fddiiso2start e.encapc[ENCAPBYTES-22]
#define fddiiso2fc e.encapc[ENCAPBYTES-21]
#define fddiiso2dst e.encapc[ENCAPBYTES-20]	
#define fddiiso2src e.encapc[ENCAPBYTES-14]
#define fddiiso2dsap e.encapc[ENCAPBYTES-8]
#define fddiiso2lsap e.encapc[ENCAPBYTES-7]
#define fddiiso2ctl e.encapc[ENCAPBYTES-6]
#define fddiiso2snap e.encapc[ENCAPBYTES-5]
#define fddiiso2oui e.encaps[ENCAPWORDS-2]
#define fddiiso2type e.encaps[ENCAPWORDS-1]

/*
 * Kludge for CLNS.
 */
#define fddiiso3start e.encapc[ENCAPBYTES-16]
#define fddiiso3fc e.encapc[ENCAPBYTES-15]
#define fddiiso3dst e.encapc[ENCAPBYTES-14]
#define fddiiso3src e.encapc[ENCAPBYTES-8]
#define fddiiso3dsap e.encapc[ENCAPBYTES-2]
#define fddiiso3lsap e.encapc[ENCAPBYTES-1]
#define fddiiso3ctl e.encapc[ENCAPBYTES-0]

/*
 * 802 SNAP encapsulation for apollo on FDDI.
 */
#define fddiapollo2start e.encapc[ENCAPBYTES-52]
#define fddiapollo2fc e.encapc[ENCAPBYTES-51
#define fddiapollo2dst e.encapc[ENCAPBYTES-50]
#define fddiapollo2src e.encapc[ENCAPBYTES-44]
#define fddiapollo2dsap e.encapc[ENCAPBYTES-38]
#define fddiapollo2lsap e.encapc[ENCAPBYTES-37]
#define fddiapollo2ctl e.encapc[ENCAPBYTES-36]
#define fddiapollo2snap e.encapc[ENCAPBYTES-35]
#define fddiapollo2oui e.encaps[ENCAPWORDS-17]
#define fddiapollo2type e.encaps[ENCAPWORDS-16]
#define fddiapolloprivate e.encapc[ENCAPBYTES-30]
#define fddiapollocksum1 e.encaps[ENCAPWORDS-15]
#define fddiapollolen1 e.encaps[ENCAPWORDS-14]
#define fddiapolloquest1 e.encaps[ENCAPWORDS-13]
#define fddiapollodnet1 e.encaps[ENCAPWORDS-12]
				  
#define MAXSNAPFDDISIZE		4470

#define FDDI_IGRP_DELAY		10
#define FDDI_IGRP_BANDWIDTH	100
#define FDDIADDRBYTES	6
#define SFDDIADDRBYTES	2


#define FDDI_ISO1_ENCAPSIZE	16
#define FDDI_ISO2_ENCAPSIZE	21
# define FDDI_ISO2_OVERHEAD	((FDDI_ISO2_ENCAPSIZE / 2) + 1)
#define FDDI_ISO3_ENCAPSIZE	15
#define FDDI_APOLLO_ISO2_ENCAPSIZE	51

#define FDDI_ASYNCHRONOUS_FRAME	0x00
#define FDDI_SYNCHRONOUS_FRAME  0x80    
#define FDDI_48BIT_ADDRESS	0x40
#define FDDI_LLC_FRAME		0x10
#define FDDI_PRIORITY_ZERO	0x00

#define FDDI_HOLDQ		25
#define MAXFDDISIZE		4500

#define MOVEMAC(from, f, to, t)	 *((ushort *)&to[t]) = *((ushort *)&from[f]); \
    *((ushort *)&to[t+2]) = *((ushort *)&from[f+2]); \
    *((ushort *)&to[t+4]) = *((ushort *)&from[f+4])

#define ZEROMAC(mac) 		*((ushort *) &mac[0]) = 0; \
    *((ushort *) &mac[2]) = 0; \
    *((ushort *) &mac[4]) = 0

/*
 * Default values for the FDDI (formac) controller.
 */
#define T_MIN_SYMBOLS		100000
#define T_MAX_SYMBOLS		4125000
#define FDDI_SYMBOLS_SECOND	25000000
#define FDDI_BYTES_SECOND	(FDDI_SYMBOLS_SECOND / 2)

#define DEFAULT_T_MIN		(T_MIN_SYMBOLS / 2)
#define DEFAULT_T_MAX		(T_MAX_SYMBOLS / 2)

#define DEFAULT_T_MIN_TIME	4000		/* 4000 microseconds */
#define DEFAULT_T_MAX_TIME	165000		/* 165000 microseconds */
#define DEFAULT_TRT_TIME	5000		/* 5000 microseconds */

#define DEFAULT_TRT	        ((FDDI_BYTES_SECOND / 1000) * \
				 (DEFAULT_TRT_TIME / 1000))
#define DEFAULT_TMAX		((FDDI_BYTES_SECOND / 1000) * \
				 (DEFAULT_T_MAX_TIME / 1000))
#define DEFAULT_TMIN		((FDDI_BYTES_SECOND / 1000) * \
				 (DEFAULT_T_MIN_TIME / 1000))

#define DEFAULT_T_TVX_TIME	2500		/* 2500 microseconds */
#define TVX_MIN_SYMBOLS		62500           /* equal to 2.5 milliseconds */
#define DEFAULT_TVX		(TVX_MIN_SYMBOLS / 2)

#define FDDI_FORMAC_INTERRUPT_MASK	0x3600

#define DEFAULT_PHY_A_TVAL	0x0008
#define DEFAULT_PHY_B_TVAL	0x020C

#define FDDI_DEFAULT_BURST_COUNT	3
/*
 * Contents of the FF (format bits).
 */ 
#define FDDI_FF_SMT_MAC_FRAME			0    
#define FDDI_FF_LLC_FRAME			1
#define FDDI_FF_RESERVED_IMPLEMENTOR_FRAME	2
#define FDDI_FF_RESERVED_FUTURE_FRAME		3		

/*
 * Contents of the ZZZZ (control bits)
 */
    
#define FDDI_ZZZZ_VOID_OR_TOKEN		0

#define NSA_FC  0x4F
#define SMT_FC	0x41

/*
 * Definitions of SMT frame_class.
 */
#define NIF_FC			0x01
#define SIF_CONFIGURATION_FC	0x02
#define SIF_OPERATION_FC	0x03
#define ECF_FC			0x04
#define RAF_FC			0x05
#define RDF_FC			0x06
#define ESF_FC			0xFF

/*
 * Interval for sending NIF.
 */
#define NIF_PERIODIC_INTERVAL	(ONESEC * 30)
/*
 * Definitions for SMT frame_type.
 */

#define ANNOUNCEMENT_FT		0x01
#define REQUEST_FT		0x02
#define RESPONSE_FT		0x03

#define SMT_OVERHEAD		4

/*
 * FDDI header type
 */
#define FDDIHDRSIZE		13	/* sizeof(fddihdrtype) == 14 ?? */

typedef struct fddihdrtype_ {
    uchar fc;
    uchar da[6];
    uchar sa[6];
} fddihdrtype;


/*
 * SMT header type.
 */
#define SMT_VERSION_5_0		0
#define SMT_VERSION_5_1		1

typedef struct smthdrtype_ {
    uchar frame_class;
    uchar frame_type;
    ushort version_id;
    ulong transaction_id;
    uchar station_id[8];
    ushort pad;
    ushort info_field_length;
} smthdrtype;

/* 
 * Defintion of the Upstream Neighbor Address (UNA) type
 */
typedef struct unatype_ {
    ushort code;
    ushort len;
    ushort reserved;
    uchar addr[6];
} unatype;

/* 
 * Defintion of the Station Descriptor type
 */
typedef struct station_descriptor_type_ {
    ushort code;
    ushort len;
    uchar station_type;
    uchar mac_count;
    uchar attach_count;
    uchar spur_count;
} station_descriptor_type;

/*
 * Definition of the Station State type
 */
typedef struct station_state_type_ {
    ushort code;
    ushort len;
    ushort reserved;
    uchar topology;
    uchar duplicate_address;
} station_state_type;

/*
 * Definition of the Time Stamp type.
 */
typedef struct time_stamp_type_ {
    ushort code;
    ushort len;
    ulong value[2];
} time_stamp_type;

/*
 * Definition of the Station Policy type.
 */
typedef struct station_policy_type_ {
    ushort code;
    ushort len;
    ushort configuration;
    ushort connection;
} station_policy_type;

/*
 * Definition of the MAC Neighbors type.
 */
typedef struct mac_neighbors_type_ {
    ushort code;
    ushort len;
    ushort pad;
    ushort mac_index;
    uchar una[6];
    uchar dna[6];
} mac_neighbors_type;

/*
 * Definition of the frame cacpabilities type.
 */
typedef struct frame_capabilities_type_ {
    ushort code;
    ushort len;
    ushort pad1;
    ushort mac_index;
    ushort pad2;
    ushort value;
} frame_capabilities_type;

/*
 * Definition of the Path Descriptor type.
 */
typedef struct path_descriptor_type_ {
    ushort code;
    ushort len;
    struct phy_record_type {
	ushort pad;
	uchar type;
	uchar state;
	uchar remote_type;
	uchar remote_mac;
	ushort index;
    } phy[2];
    uchar mac_addr[6];
    ushort index;
} path_descriptor_type;

/*
 * Definition of the Mac Status type.
 */
typedef struct mac_status_type_ {
    ushort code;
    ushort len;
    ushort pad;
    ushort index;
    ulong t_reg;
    ulong t_neg;
    ulong t_max;
    ulong tvx;
    ulong t_min;
    ulong sba;
    ulong frame_ct;
    ulong error_ct;
    ulong lost_ct;
} mac_status_type;

/*
 * Definition of the  PHY Link Error Rate Monitoring (LER) type.
 */
typedef struct ler_status_type_ {
    ushort code;
    ushort len;
    ushort pad1;
    ushort phy_index;
    uchar pad2;
#define DEFAULT_LER_CUTOFF	7
    uchar cutoff;
#define DEFAULT_LER_ALARM	8
    uchar alarm;
#define DEFAULT_LER_ESTIMATE	12
    uchar estimate;
    ulong lem_reject_ct;
    ulong lem_ct;
} ler_status_type;

typedef struct supported_version_type_ {
    ushort code;
    ushort len;
    ushort pad1;
    uchar count;
    uchar index;
    ushort number;
    ushort pad2;		/* only plan on supporting one (1) version */
} supported_version_type;

    
/*
 * Component definition of the Neighbor Information Frame (NIF) type.
 */
typedef struct nifinfotype_ {
    unatype una;
    station_descriptor_type station_descriptor;
    station_state_type station_state;
    frame_capabilities_type frame_capabilities;
} nifinfotype;

typedef struct niftype_ {
    smthdrtype smt;
    nifinfotype info;
} niftype;




/*
 * Component definition of the Request Denied Frame (RDF) type.
 */
typedef struct reason_code_type_ {
    ushort code;
    ushort len;
#define SMT_UNSUPPORTED_CLASS	1
#define SMT_BAD_VERSION		2
    ulong reason;
} reason_code_type;

typedef struct reject_frame_beginning_type_ {
    ushort code;
    ushort len;
    uchar reject_pad[3];
    uchar reject_data[0];
} reject_frame_beginning_type;

typedef struct rdfinfotype_ {
    reason_code_type rdf;
    supported_version_type supported_version;
    reject_frame_beginning_type data;
} rdfinfotype;

typedef struct rdftype_ {
    smthdrtype smt;
    rdfinfotype info;    
} rdftype;



/*
 * Component definition of the Echo Frame (ECF) response type.
 */
typedef struct echo_data_type_ {
    ushort code;
    ushort len;
    uchar echo_data[0];
} echo_data_type;

typedef struct echoinfotype_ {
    echo_data_type data;
} echoinfotype;

typedef struct echo_type_ {
    smthdrtype smt;
    echoinfotype data;
} echo_type;


typedef struct esf_id_type_ {
    ushort code;
    ushort len;	
    ushort pad;
    uchar id[6];
} esf_id_type;

/*
 * Component definition of SIF Configuration response type.
 */
typedef struct sifconfginfotype_ {
    time_stamp_type time;
    station_descriptor_type station_descriptor;
    supported_version_type supported_version;
    station_state_type station_state;
    station_policy_type station_policy;
    mac_neighbors_type neighbors;
    path_descriptor_type path;
} sifconfginfotype;


typedef struct sifconfgtype_ {
    smthdrtype smt;
    sifconfginfotype info;
} sifconfgtype;


#define SIF_OPERATION		0
#define SIF_CONFIGURATION	1
/*
 * Component definition of SIF Operation response type.
 */
typedef struct sifoperinfotype_ {
    time_stamp_type time;
    mac_status_type mac;
    ler_status_type ler[2];
} sifoperinfotype;

typedef struct sifopertype_ {
    smthdrtype smt;
    sifoperinfotype info;
} sifopertype;
/*
 * Optional structure definitions -- not currently supported.
 */
typedef struct frame_counters_type_ {
    ushort code;
    ushort len;
    ushort mac_index;
    ulong receive_ct;
    ulong transmit_ct;
} frame_counters_type;

typedef struct frame_not_copied_count_type_ {
    ushort code;	
    ushort len;
    ushort pad;
    ushort mac_index;
    ulong not_copied_ct;
} frame_not_copied_count_type;

typedef struct mac_priority_values_type_ {
    ushort code;
    ushort len;
    ushort pad;
    ushort mac_index;
    ulong t_pri_1;
    ulong t_pri_2;
    ulong t_pri_3;
    ulong t_pri_4;
    ulong t_pri_5;
    ulong t_pri_6;
    ulong t_pri_7;
} mac_priority_values_type;

typedef struct phy_eb_status_type_ {
    ushort code;
    ushort len;
    ushort phy_index;
    ushort eb_error_ct;
} phy_eb_status_type;

typedef struct manufacturer_code_type_ {
    ushort code;
    ushort len;
    uchar oui[3];
    uchar data[29];
} manufacturer_code_type;    

/*
 * Definitions of SMT frame_class.
 */
#define NIF_FC			0x01
#define SIF_CONFIGURATION_FC	0x02
#define SIF_OPERATION_FC	0x03
#define ECF_FC			0x04
#define RAF_FC			0x05
#define RDF_FC			0x06
#define ESF_FC			0xFF

/*
 * Definitions for SMT frame_type.
 */

#define ANNOUNCEMENT_FT		0x01
#define REQUEST_FT		0x02
#define RESPONSE_FT		0x03

/*
 * SMT header values for version_id and pad.
 */
#define SMT_VERSION_ID		0x0001
#define SMT_PAD			0x0000

/*
 * Information field of the SMT datagram contains parameter types
 * followed by parmeter lengths, followed by the paramter value. The
 * defined parameter type are below. M indicates mandatory; O indicates
 * an optional parameter. (wish there were more O's).
 */
#define	SMT_UNA			0x0001	/* M Upstream neighbor address 	*/
#define SMT_STATION_DESCRIPTOR	0x0002	/* M Station Descriptor		*/
#define SMT_STATION_STATE	0x0003	/* M Station state		*/
#define SMT_TIMESTAMP		0x0004	/* M Timestamp (for cisco ???)	*/
#define SMT_STATION_POLICY	0x0005	/* M Station policy		*/
#define SMT_PATH_LATENCY	0x0006	/* O Path Latency / per ring	*/
#define SMT_MAC_NEIGHBORS	0x0007	/* M MAC neighbors		*/
#define SMT_PATH_DESCRIPTOR	0x0008	/* M Path descriptors		*/
#define SMT_MAC_STATUS		0x0009	/* M MAC status			*/
#define SMT_LINK_ERROR_RATE	0x000a	/* M Phy link error rate (LER)	*/
#define SMT_MAC_FRAME_COUNT	0x000b	/* O MAC frame counts		*/
#define SMT_NOT_COPIED_COUNT	0x000c	/* O MAC frame not copied count	*/
#define SMT_MAC_PRIORITY	0x000d	/* O MAC priority values	*/
#define SMT_PHY_EB_STATUS	0x000e	/* O PHY elasticity status	*/
#define SMT_MANUFACTURER	0x000f	/* O Manufacturer desc field	*/
#define SMT_USER_FIELD		0x0010	/* O User field			*/
#define SMT_ECHO_DATA		0x0011	/* M Echo data			*/
#define SMT_REASON_CODE		0x0012	/* M Reason code (RDF)		*/
#define SMT_REJECTED_FRAME	0x0013	/* M Beginning of rejected frame*/
#define SMT_SUPPORTED_VERSION	0x0014	/* M Supported Version ONLY 1	*/
#define SMT_FRAME_CAPABILITIES	0x200B	/* M Smt 6,2		*/
#define SMT_ESF_ID		0xFFFF	/* O Extended service frame ID	*/

#define SMT_UNA_LEN		sizeof(unatype) - SMT_OVERHEAD
#define SMT_STATION_DESC_LEN	sizeof(station_descriptor_type) - SMT_OVERHEAD
#define SMT_STATION_STATE_LEN	sizeof(station_state_type) - SMT_OVERHEAD
#define SMT_TIME_STAMP_LEN	sizeof(time_stamp_type) - SMT_OVERHEAD
#define SMT_STATION_POLICY_LEN	sizeof(station_policy_type) - SMT_OVERHEAD
#define SMT_PATH_LATENCY_LEN	sizeof(path_latency_type) - SMT_OVERHEAD
#define SMT_MAC_NEIGHBORS_LEN   sizeof(mac_neighbors_type) - SMT_OVERHEAD
#define SMT_REASON_CODE_LEN	sizeof(reason_code_type) - SMT_OVERHEAD
#define SMT_MAC_STATUS_LEN	sizeof(mac_status_type) - SMT_OVERHEAD
#define SMT_LINK_ERROR_RATE_LEN sizeof(ler_status_type) - SMT_OVERHEAD
#define SMT_SUPPORTED_VERSION_LEN	sizeof(supported_version_type) - \
			       SMT_OVERHEAD
#define SMT_PATH_DESCRIPTOR_LEN sizeof(path_descriptor_type) - SMT_OVERHEAD
#define SMT_REJECTED_FRAME_LEN  sizeof(smthdrtype)
#define SMT_FRAME_CAPABILITIES_LEN	sizeof(frame_capabilities_type) - \
			       SMT_OVERHEAD

#define NIF_LENGTH		sizeof(nifinfotype)

#define FDDI_E_INDICATOR	0x80
#define FDDI_A_INDICATOR	0x40
#define FDDI_C_INDICATOR	0x20
#define FDDI_ERROR_THIS_NODE	0x10
#define FDDI_ADDRESS_RECOGNIZED	0x08
#define FDDI_MAC_FRAME		0x04
#define FDDI_SMT_FRAME		0x02
#define FDDI_IMPLEMENTER_FRAME	0x01

#define FDDI_SX_MASK		(FDDI_E_INDICATOR | FDDI_ERROR_THIS_NODE)

#define PHY_A		0
#define PHY_B		1

#define PC_OFF   	0
#define PC_BREAK	1
#define PC_TRACE	2
#define PC_CONNECT	3
#define PC_NEXT		4
#define PC_SIGNAL	5
#define PC_JOIN  	6
#define PC_VERIFY	7
#define PC_ACTIVE	8
#define PC_MAINT	9

#define SC_ISOLATED	0
#define SC_WRAP_A	1
#define SC_WRAP_B	2
#define SC_WRAP_AB	3	/* not supported -- only ONE (1) MAC */
#define SC_THRU_A	4
#define SC_THRU_B	5
#define SC_THRU_AB	6	/* not supported -- only ONE (1) MAC */

#define CF_JOIN		1
#define CF_LOOP		2
#define CF_EXIT		3

#define SC_JOIN_A	1
#define SC_LOOP_A	2
#define SC_EXIT_A	3
#define SC_LAST_A	SC_EXIT_A
#define SC_JOIN_B	4
#define SC_LOOP_B	5
#define SC_EXIT_B	6
#define SC_LAST_B	SC_EXIT_B

#define T_OUT		100	/* milliseconds	*/
#define TB_MIN		5	/* milliseconds */
#define C_MIN		2	/* milliseconds - really s/b 1.6 ms */
#define TB_MAX		50	/* milliseconds	*/

#define TL_MIN		30 	/* microseconds */

#define LS_MIN		480 	/* nanoseconds */

typedef struct phytype_ {
    idbtype *idb;
    mempointer regptr;
    ulong  pc_unit;
    ushort pc_state;
    ushort pc_neighbor;
    ushort bit_signal;
    ushort current_fiber_status;
    ushort interlock;
    ushort pc_state_signal;
    ulong tval;
    ulong rval;
    int pc_process_id;
    boolean pc_kill;
    boolean connect;
    int timer;
    int lem_ct;
    int transitions[PC_MAINT+1];
    char type;
} phytype;

typedef struct signal_bits_ {
    ulong fill31_16 : 16;
    ulong fill15_10 : 6;
    ulong remote_mac : 1;
    ulong loop_mac : 1;
    ulong lct_failed : 1;
    ulong lct_mac : 1;
#define LCT_SHORT	0
#define LCT_MEDIUM	2
#define LCT_LONG	1
#define LCT_EXTENDED	3
    ulong lct_duration : 2;
    ulong phy_comaptibility : 1;
    ulong remote_station : 2;
    ulong escape : 1;
} signal_bits;
	
#define QLS	0
#define MLS	1
#define HLS	2
#define ILS	3


/*
 * ENDEC status register values. See AMD Data Book.
 */
#define LSU_STATUS	0
#define NLS_STATUS	1
#define MLS_STATUS	2
#define ILS_STATUS	3
#define HLS_STATUS	4
#define QLS_STATUS	5
#define ALS_STATUS	6
#define OVUF_STATUS	7

/*
 * Convert status register values to bit positions.
 */
#define B_LSU	(1 << LSU_STATUS)
#define B_NLS	(1 << NLS_STATUS)
#define B_MLS	(1 << MLS_STATUS)
#define B_ILS	(1 << ILS_STATUS)
#define B_HLS	(1 << HLS_STATUS)
#define B_QLS	(1 << QLS_STATUS)
#define B_ALS	(1 << ALS_STATUS)
#define B_OVUF	(1 << OVUF_STATUS)

/*
 * FDDI options.
 */
#define FDDI_DUPLICATE_ADDRESS_CHK	0x0001
#define FDDI_SMT_FRAMES			0x0002
#define FDDI_LEM_SUPPORTED		0x0004
#define FDDI_CMT_SUPPORTED		0x0008
#define FDDI_VERSION_128		0x0010
/*
 * Command flag
 */
#define FDDI_NUMERIC_ARGS		0x0001

/*
 * Phy type -- indicated via the PCM signalling process.
 */
#define PC_TYPE_MASK			0x6

#define PC_TYPE_A			0x0
#define PC_TYPE_B			0x4
#define PC_TYPE_S			0x2
#define PC_TYPE_M			0x6 

/*
 * Time-out stuck beacon situation.
 */
#define RMT_BEACON_TIME 	(9 * ONESEC)





