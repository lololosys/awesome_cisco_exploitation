/* $Id: pgen_private.h,v 3.4 1995/12/21 16:56:53 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/pgen/pgen_private.h,v $
 *------------------------------------------------------------------
 * P G E N _ P R I V A T E . H
 *
 * Declarations for things that are shared among packet generator
 * modules, but not visible outside the packet generator
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_private.h,v $
 * Revision 3.4  1995/12/21  16:56:53  hampton
 * Remove old timer related declarations or obsolete conditionally
 * compiled code.  [CSCdi45965]
 *
 * Revision 3.3  1995/11/17  18:56:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:59:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/14  01:52:08  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.2  1995/06/09  13:18:07  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/* Version information (major, minor, edit) */
#define PGEN_VERSION_STRING "2.2.0"

/* Size of the buffer into which input hex data are decoded by the parser */
#define PGEN_HEXBUFSIZE 8192

/* Size of the default incoming TFTP buffer */
#define PGEN_DEFTFTP 65536

/* Number of MEMD buffers to preallocate for each packet for RSP fast send */
#define PGEN_NUMBUF  9

/* Parameters controlling how the packet generator acts when it can't
 * get a packet into a hold queue.
 */
#define PGEN_RETRYDISMS 128	/* #times to retry before check_suspend*/
#define PGEN_RETRYSUSP 1024	/* #times to retry before long dismiss */
#define PGEN_SUSPTIME 8		/* How long to dismiss (ms) */

/* Packet generator context. One of these exists for each instance
 * of the packet generator. A pointer in the CSB is used to locate
 * it.
 */
struct pgen_context_ {
    queuetype workspace;	/* Queue of packets constituting workspace */
    ulong bufsize;		/* Size to use for TFTP buffer */
    idbtype *interface;		/* Default interface for new packets */
    unsigned batchdepth;	/* # recursive TFTP batch loads active */
    int timeoffs;		/* Offset applied to parsed times */
    /* The rest of the context is a save area for the add and patch functions,
     * which work by building up information in the context, then calling
     * pgen_do_add_patch to actually do whatever needs to be done.
     */
    unsigned apfunc;		/* Add/patch function code */
    unsigned apfirst;		/* First packet in add/patch range */
    unsigned aplast;		/* Last packet in add/patch range */
    unsigned apedfunc;		/* Code for "byte", "insert", or "delete" */
    unsigned apedoffs;		/* Offset for edit function */
    unsigned apttosend;		/* Add/patch time-to-send */
    unsigned apfixlen;		/* Add/patch force length value */
    unsigned aprepeat;		/* Add/patch repeat count */
    unsigned apinterval;	/* Add/patch minimum packet interval */
    idbtype *apif;		/* Add/patch interface */
    char *apname;		/* Pointer to name string */
    ulong apedlen;		/* Length of data add/patch buffer */
    uchar apeddata[PGEN_HEXBUFSIZE]; /* add/patch data buffer */
};

#define PGEND(csb) (csb->pgendata)
#define SETPGEND(csb,ptr) (csb->pgendata = ptr)

/* Each packet in the workspace has a number of attributes, and we keep
 * them all in fields in the buffer overhead that we don't use for anything
 * else. Ugly, but keeps us from having to have packet generator fields in
 * the packet structure. In addition to these, we use datagramsize to hold
 * the packet length and pgen_output to hold the interface on which to send.
 * We set up datagramstart and linktype for use by datagram_out, holdq_enqueue,
 * and the drivers, but don't really use them ourselves.
 */
#define pgen_rptcount length           /* Repeat count (ulong) */
#define pgen_interval atalk_dstfqa     /* Min pak interval usecs (ulong) */
#define pgen_ttosend clns_flags        /* Time to send in usecs (ulong) */
#define pgen_tick atalk_srcfqa         /* Time to send in mS (ulong) */
#define pgen_nameptr dataptr	       /* Name for the packet (uchar *) */
#define pgen_output if_input           /* Save if_output which driver change */
#define pgen_bufpool mac_start         /* Preallocated MEMD buffer pool */
#define pgen_bufcur  mci_status        /* Next MEMD buffer to transmit */
#define pgen_bufptr  mci_status        /* Pagent MEMD buffer address */

/* hwidb field redefines */
#define pgen_countmode input_defqcount    /* Interface in fast count mode flag */

/* The parse chain for packet generation commands */
pdecl(top_pgen);

/* Values used for csb->which in the parser */
#define PGEN_ADDPATCH 1
#define PGEN_CLEAR 2
#define PGEN_COUNT 3
#define PGEN_EXAMINE 4
#define PGEN_INTERFACE 5
#define PGEN_SORT 6
#define PGEN_BUFFER 7
#define PGEN_DUMPCMDS 8
#define PGEN_LOAD 9
#define PGEN_REPLAY 10
#define PGEN_QUIT 11
#define PGEN_VERSION 12

/* The parse chain for hex mode */
pdecl(top_pgen_hex);

/* csb->which for hex data entry mode */
#define PGEN_HEX_DATA 1
#define PGEN_HEX_QUIT 2

/* Codes passed back in parser objects to select various functions */
#define PGEN_ADD 0		/* Doing an "add" as opposed to a "patch" */
#define PGEN_PATCH 1		/* Doing a "patch" as opposed to an "add" */

/* Edit types for add/patch */
#define PGEN_EDNONE 0		/* No data editing function needed */
#define PGEN_EDINS 1		/* "insert" specified */
#define PGEN_EDBYTE 2		/* "byte" (overwrite data) specified */
#define PGEN_EDDEL 3		/* "delete" specified */

/* File types for load */
#define PGEN_LDCMDS 0		/* Read a file full of commands */
#define PGEN_LDSNIF 1		/* Read a Sniffer-format binary capture file */

/* Target types for dump */
#define PGEN_DMPTTY 0		/* Dump to user's TTY */
#define PGEN_DMPTFTP 1		/* Dump to file on TFTP server */
#define PGEN_DMPTRCP 2		/* Dump to file on RCP server */

/* Various count command options */
#define PGEN_COUNT_OFF 0		/* Put interface in normal mode */
#define PGEN_COUNT_ON  1		/* Put interface in count-only mode */
#define PGEN_COUNT_STATUS  2		/* Print interface mode */

/* Routines that implement the packet generator commands */
extern void pgen_add_patch_command (parseinfo *, pgen_context *, unsigned,
				    char *, char *, unsigned, unsigned,
				    unsigned, uchar *, unsigned, unsigned,
				    unsigned, unsigned, idbtype *, char *);
extern void pgen_clear_command (pgen_context *, char *, char *);
extern void pgen_count_command (pgen_context *, unsigned, idbtype *);
extern void pgen_examine_command (pgen_context *, char *, char *, unsigned);
extern void pgen_interface_command (pgen_context *, idbtype *);
extern void pgen_sort_command (pgen_context *, char *, char *);
extern void pgen_buffer_command (pgen_context *, unsigned);
extern void pgen_dump_cmds_command (pgen_context *, char *, char *, unsigned,
				    char *, addrtype *);
extern void pgen_load_command (pgen_context *, unsigned, int, unsigned,
                               char *, addrtype *);
extern void pgen_replay_command (pgen_context *, char *, char *,
				 int, unsigned, unsigned);
extern void pgen_quit_command (parseinfo *, int);
extern void pgen_version_command (parseinfo *);

/* Utility and miscellaneous routines from pgen_cmds.c */
extern boolean pgen_rangechk (pgen_context *, char *, char *, unsigned *,
			      unsigned *, boolean);
extern void pgen_do_add_patch (pgen_context *);
extern void dump_mem (void *, unsigned, void *);
extern void pgen_parse_buffer (char *, pgen_context *);
extern unsigned pgen_dumppaks (pgen_context *, unsigned, unsigned,
			       char *, tt_soc *);
extern unsigned btprintf (char **, tt_soc *, char *, ...);

/* Workspace manipulation routines from pgen_ws.c */
extern unsigned pgen_wscount (queuetype *);
extern paktype *pgen_wspick (queuetype *, unsigned);
extern paktype *pgen_wsnew (queuetype *, unsigned, unsigned);
extern paktype *pgen_wsgrow (queuetype *, unsigned, unsigned);
extern void pgen_wsdel (queuetype *, unsigned);
extern void pgen_wssort (queuetype *, unsigned, unsigned);
extern void pgen_wsfreeall (queuetype *);
extern unsigned pgen_wsbyname (queuetype *, char *);

/* Data sending routines from pgen_send.c (or platform specific file) */
extern void pgen_send (paktype *, paktype *, int, int);
extern void pgen_fastsend (paktype *, paktype *, int, int);

/* Incoming packet fast counting routine */
extern void pgen_count (hwidbtype *, unsigned);
extern void pgen_fastcount (hwidbtype *, unsigned);

/* Sniffer data reading routine from pgen_snif.c */
extern void pgen_sniftows (pgen_context *, char *, unsigned);

/* Capture mode routine used to swap FDDI MAC addresses */
extern inline void capture_swap_fddi_hdr (paktype *);

/* The pgen parser initialization routine */
extern void pgen_parser_init (void);

/* pgen subsys initialization routine */
extern void pgen_init(subsystype *);

