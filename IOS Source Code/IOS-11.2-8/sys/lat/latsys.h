/* $Id: latsys.h,v 3.2 1995/11/17 17:34:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latsys.h,v $
 *------------------------------------------------------------------
 * Copyright (c) 1990-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latsys.h,v $
 * Revision 3.2  1995/11/17  17:34:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  05:06:32  smackie
 * Remove scary pointer arithmetic from LAT buffer handling. Use a direct
 * embedded pointer to the packet header instead. (CSCdi40672)
 *
 * Revision 2.1  1995/06/07  21:26:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    latsys		lat system specific definitions

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

#ifndef LATSYS_H
#define LATSYS_H	0		/* Indicate file has been read	*/

/*----------------------------------------------------------------------
    Define LAT configuration options. Set option to 1 to enable the
    feature, zero to disable.
 -----------------------------------------------------------------------*/
#define LE_HOST		1		/* Support host sessions	*/
#define LE_HOST_AS	1		/* Support host AS  messages	*/

#define	LE_SERVER	1		/* Support server sessions	*/
#define LE_SERVER_LS	1		/* Support server LS  messages	*/
#define LE_SERVER_AGENT 1		/* Support agent solicitation   */

#define LE_SUBJECT_HIC	1		/* Support subject HIC messages	*/
#define LE_OBJECT_HIC	1		/* Support object  HIC messages	*/

#define LE_OBJECTS	0		/* Supports objects 		*/
#define LE_HS_STATS	1		/* Support host statistics	*/
#define LE_ND_STATS	1		/* Support node statistics	*/
#define LE_UPPER	1		/* Store strings in upper	*/

/*----------------------------------------------------------------------
    The following two configuration options may be disabled at run-time
    if they are selected by setting the global values le_nd_retain and
    le_ls_dups to zero.

    Notes, if the options are not selected, they may not be enabled at
    runtime.
 -----------------------------------------------------------------------*/
#define LE_ND_RETAIN	1		/* Retain node/services		*/
#define	LE_LS_DUPS	1		/* Allow duplicate ls servces	*/

/*----------------------------------------------------------------------
    Define views of the LAT engine structures.
 -----------------------------------------------------------------------*/
#define CB_VIEW(x)	*x		/* Circuit block		*/
#define SB_VIEW(x)	*x		/* Session block		*/
#define	AS_VIEW(x)	*x		/* Service, advertised 		*/
#define LS_VIEW(x)	*x		/* Service, learned		*/
#define ND_VIEW(x)	*x		/* Node structure		*/
#define OB_VIEW(x)	*x		/* Object block			*/
#define HI_VIEW(x)	*x		/* Host-initiated block		*/
#define QR_VIEW(x)	*x		/* Queued request		*/
#define IF_VIEW(x)	*x		/* LAT input  frame		*/
#define OF_VIEW(x)	*x		/* LAT output frame		*/

/*----------------------------------------------------------------------
    Define macro to correctly orient word fields in message. Little
    indian machines (808x, VMS, etc) should define macro as a null.
 -----------------------------------------------------------------------*/
#define	swap(x)		(lit2h(x))
/*----------------------------------------------------------------------
    Define message byte points and the output frame buffer.
 -----------------------------------------------------------------------*/
#define	DLL_MAX		1500		/* Maximum output message size  */
#define INP_MAX		0		/* Maximum input message size   */

    typedef byte IF_VIEW(if_byte);	/* Pointer to input  frames	*/
    typedef byte OF_VIEW(of_byte);	/* Pointer to output frames	*/

/* Hack alert: This structure must match the corresponding lat_* fields
 * in packet.h
 */

    typedef struct of
    {
	struct of OF_VIEW(of_link);	/* Link for transmit queue	*/
	struct of OF_VIEW(of_i_o);	/* Link for internal I/O	*/
        of_byte	of_data;		/* Pointer to output data	*/
	int	of_size;		/* Size of output frame		*/
	word	of_dst[3];		/* Destination address		*/
	idbtype *of_idb;		/* Output interface             */
	paktype *of_pak;		/* Buffer                       */
    } of_node, OF_VIEW(of_ptr);

#define OF_LENGTH	0x0fff		/* Length field in of_size	*/
#define OF_REXMIT	0x1000		/* Retransmit frame		*/
#define OF_OUTPUT	0x2000		/* Frame is being output	*/
#define	OF_DELETE	0x4000		/* Delete frame on I/O complete	*/

/*----------------------------------------------------------------------
    Define LAT external data types.
 -----------------------------------------------------------------------*/
    typedef void *	aid_type;	/* Advertised service ident	*/
    typedef void *	oid_type;	/* Object  identifer		*/
    typedef void *	sid_type;	/* Session identifer		*/

#define AID_NULL	(aid_type)(-1)	/* Null advertised service id	*/
#define SID_NULL	(sid_type)(-1)	/* Null session identifer	*/
#define OID_NULL	(oid_type)(-1)	/* Null object  identifier	*/

/*----------------------------------------------------------------------
    Define timer constants for system.
 -----------------------------------------------------------------------*/
#define MSEC_TIC	10		/* System uses 10 msec   timer	*/
#define SECS_TOC	10		/* System uses 10 second timer	*/

/*----------------------------------------------------------------------
    Define maximums for various structures. The CB_MAXIMUM is used at
    compile time to define the maximum number of circuits. The other
    values are initial maximums and may be changed at runtime. A value
    of zero means the engine does not enforce a maximum.
 -----------------------------------------------------------------------*/
#define CB_MAXIMUM	0		/* Maximum circuit blocks	*/
#define SB_MAXIMUM	0		/* Maximum session blocks	*/
#define AS_MAXIMUM	0		/* Maximum advertised services	*/
#define LS_MAXIMUM	0		/* Maximum learned services	*/
#define ND_MAXIMUM	0		/* Maximum remote nodes		*/
#define OB_MAXIMUM	0		/* Maximum objects		*/
#define HI_MAXIMUM	0		/* Maximum HIC requests		*/
#define QR_MAXIMUM	0		/* Maximum queued requests	*/

/*----------------------------------------------------------------------
    Define circuit constants for system.
 -----------------------------------------------------------------------*/
#define	CB_VC_MSEC	80		/* Circuit interval    (msec)	*/
#define CB_HD_MSEC	40		/* Circuit host delay  (msec)	*/
#define CB_HD_MAX_MSEC  2550            /* Circuit max host delay (msec)*/
#define CB_KA_SECS	20		/* Keep-alive interval (secs)	*/
#define CB_PROGRESS	60		/* Host progress interval (s)	*/
#define CB_R1_UNIT	4		/* Retransmit, first pass (vc)	*/
#define CB_DEF_TRY	8		/* Default circuit retries	*/
#define CB_MAX_SES	255		/* Maximum sessions/circuit	*/
#define CB_S_RCV_EXTRA	0		/* Extra receive buffers (S)	*/
#define CB_H_RCV_EXTRA	0		/* Extra receive buffers (H)	*/

/*----------------------------------------------------------------------
    Define session constants for system.
 -----------------------------------------------------------------------*/
#define	SB_MAX_DATA	0xff		/* Maximum DATA-A slot		*/
#define SB_MAX_ATTN	0xff		/* Maximum ATTENTION slot	*/

#define SB_REJECTS	16		/* Maximum pending rejects	*/

/*----------------------------------------------------------------------
    Define host-initiated constants for this system.
 -----------------------------------------------------------------------*/
#define HI_STS_SEC	60		/* Default status interval	*/
#define HI_CMD_SEC	1		/* Default SI/CMD interval	*/
#define HI_DEF_TRY	2		/* Default HIC attempts		*/

/*----------------------------------------------------------------------
    Define advertised service constants for system.
 -----------------------------------------------------------------------*/
#define AS_MC_SEC	20		/* Default multicast interval	*/

/*----------------------------------------------------------------------
    Define learned service constants for system.
 -----------------------------------------------------------------------*/
#define LS_UPDATES	5		/* Multicast multiplier for ls	*/
#define LS_MC_DEF	256		/* Default mc for permanent ls	*/

/*----------------------------------------------------------------------
    Define remote node constants for system. Note the age maximum
    cannot exceed 10 hours. Set to zero if no maximum.
 -----------------------------------------------------------------------*/
#define ND_AGE_MAX	(60 * 60 * 5)	/* Maximum age of node  (sec)	*/
#define ND_INTERVAL	(60 * 30)	/* Purge interval       (sec)	*/

/*----------------------------------------------------------------------
    Define LAT dimensions.

    CB_TABLES is a compile time constant which defines the size of
    hs_cb_table array and hence is the absolute maximum of circuits.
    Define as one greater than the maximum number of circuits which
    will be allowed.

    AS_OBJECTS is a compile time constant which defines the maximum
    objects which can be associated with an advertised circuit.
 -----------------------------------------------------------------------*/

#define CB_TABLES	96		/* Maximum number of circuits	*/

#define AS_OBJECTS	8		/* Maximum objects w/as_node	*/

#define ND_HASHES	3		/* Depth of nd hash in common	*/
#define nd_hash(x)	(x % ND_HASHES)

#define SB_HASHES	3		/* Depth of sb hash in cb	*/
#define sb_hash(x)	(x % SB_HASHES)

/*----------------------------------------------------------------------
    Define the various LAT name strings. The first byte is a count
    of the remaining characters.
 -----------------------------------------------------------------------*/
#define SC_STR		2		/* Service classes		*/
#define	ND_STR		17		/* Node name			*/
#define SV_STR		17		/* Service name			*/
#define OB_STR		17		/* Object  name			*/
#define	ID_STR		65		/* Identification		*/

/*----------------------------------------------------------------------
    Define the maximum field size for groups. Groups are handled in
    same fashion as name strings, i.e. first byte is count.
 -----------------------------------------------------------------------*/
#define GROUPS		33		/* Process groups codes	0-255	*/

/*----------------------------------------------------------------------
    Define the masks and data_b structure for DATA-B processing in 
    the engine.  Both objects and sessions use this information as
    objects refer to local port configurations, and sessions refer
    to remote port configurations, local and remote transparency modes,
    and local and remote status/break conditions.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
    Define the XON/XOFF characters.
 -----------------------------------------------------------------------*/
#define	DB_XON		0x011		/* ^Q				*/
#define DB_XOFF		0x013		/* ^S				*/

/*----------------------------------------------------------------------
    Define the data-b data fields.
 -----------------------------------------------------------------------*/
#define DB_E_PARITY	0x02		/* db_char_parity valid		*/
#define DB_E_RECV	0x04		/* db_baud_recv   valid		*/
#define DB_E_XMIT	0x08		/* db_baud_xmit   valid		*/
#define DB_E_BELL	0x10		/* db_bell_mode   valid		*/
#define DB_E_MODE	0x20		/* db_char_mode   valid		*/
#define DB_E_STATUS	0X40		/* sb_xxx_data_b_ valid		*/

#define DB_E_PORT	DB_E_PARITY | DB_E_RECV | DB_E_XMIT | DB_E_BELL | DB_E_MODE

     typedef struct
     {
	int	db_flags;		/* Enabled field flags		*/
	byte	db_type;		/* Object type flags		*/
	byte	db_config;		/* Configuration flags		*/
	byte	db_char_parity;		/* Character size, parity	*/
	byte	db_bell_mode;		/* Mode for handling bell	*/
	word	db_baud_xmit;		/* Transmit speed (server->tt)	*/
	word	db_baud_recv;		/* Receive  speed (tt->server)	*/
    } data_b;

#define LAT_DATABUF LAT_MAXCREDITS*256	/* Size of data buffer */
#define LAT_OUTBOUND_CREDITS 2		/* Credits for outbbound connections */
#define LAT_INBOUND_CREDITS LAT_MAXCREDITS
					/* Credits for inbound connections */
#define SVC_CLASSES 33			/* Size of service class mask */

/*
 * define a useful macro LATSTRING that returns printf args for a sized
 * string like the LAT package uses...
 */
#define LATSTRING(a) a[0], &a[1]

#endif
