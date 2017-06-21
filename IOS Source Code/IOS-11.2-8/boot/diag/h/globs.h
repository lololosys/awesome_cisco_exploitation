/* $Id: globs.h,v 3.1.58.1 1996/06/11 22:42:47 pst Exp $
 * $Source: /release/112/cvs/Xboot/diag/h/globs.h,v $
 *------------------------------------------------------------------
 * globs.h -- external storage and function declarations
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: globs.h,v $
 * Revision 3.1.58.1  1996/06/11  22:42:47  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xx_bits
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  09:05:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:41  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * These declarations are for intrinsic C types.  Compound types would
 * require including various header files with the attendant complexity.
 */

extern char *banner_motd;		/* message of the day banner */
extern char *boot_reason;		/* reason why system was rebooted */
extern int nnets;			/* total number of interfaces */
extern boolean netsareup;		/* TRUE if all interfaces active */
extern boolean pup_running;		/* is PUP active */
extern boolean chaos_running;		/* is CHAOS running */
extern boolean xns_running;		/* is XNS running */
extern boolean dn_running;		/* is DECnet running */
extern boolean atalk_running;		/* is Appletalk running */
extern int nconlines;			/* total number of console lines */
extern int nttylines;			/* total count of async tty lines */
extern int nlptlines;			/* total count of parallel printers */
extern int nvtylines;			/* total count of virtual lines */
extern int ALLlines;			/* total line count */
extern int VTYBase;
extern char Pprompt[];			/* Prompt for superuser mode */
extern char Eprompt[];			/* Tip EXEC prompt string */

extern char *hostname;			/* our host name */
extern char *default_domain;		/* default domain name */
extern char version[];			/* version of the software */
extern long version_number;		/* release number of the software */
extern ushort configregister;
extern long load_average;
extern char *noasm;

/* Common routines */

extern whoami();			/* Performs name lookup service */
#ifdef c_plusplus
extern legalline(char *);		/* Verify a line number */
extern int stackleft(ushort *, ulong);	/* Figures out free stack */
extern int createlevel(int, mempointer *, char *);/* assign interrupt stacks*/
extern boolean queueBLOCK(queuetype *);	/* block on queue empty */
extern char *parse_interface(char *, ...);
extern char *encstring(int);
extern ushort getshort(uchar *);
extern void bscopy(mempointer, mempointer, int);
extern void bswap(mempointer, int);
#else
extern legalline();			/* Verify a line number */
extern int stackleft();			/* Figures out free stack */
extern int createlevel();		/* assign interrupt level stacks */
extern boolean queueBLOCK();		/* block on queue empty */
extern char *parse_interface();
extern char *encstring();
extern ushort getshort();
extern void bscopy();
extern void bwcopy();
extern void wbcopy();
extern void bswap();
#endif
extern int freememory();		/* return number of free bytes */
extern boolean lowonmemory();		/* TRUE if not much memory left */
extern ulong caller();


/*
 * Recent Additions
 */

#ifdef CPU_8086
int cfork (void (*)(),ulong,short,char *,short);
void edisms( boolean (*)(),long);
void tdisms( ulong );
mempointer malloc(ushort);
/* ulong emt_call(short); */
#endif

#ifdef c_plusplus
ulong emt_call(int,...);
#else
ulong emt_call();
#endif

#ifdef c_plusplus
ulong elapsed(ulong);
void enqueue(queuetype *, mempointer *);
void p_enqueue(queuetype *, mempointer *);
void requeue(queuetype *, mempointer *);
mempointer dequeue(queuetype *);
mempointer p_dequeue(queuetype *);
void unqueue(queuetype *, mempointer *);
char *deblank(char *);
char *findarg(char *);
#else
ulong elapsed();
void enqueue();
void p_enqueue();
void requeue();
mempointer dequeue();
mempointer p_dequeue();
void unqueue();
char *deblank();
char *findarg();
#endif

extern boolean subnetzero_enable;
extern boolean probe_enable;
extern boolean iso2_enable;
extern boolean netconfig_enable;
extern boolean domain_enable;
extern boolean ipname_enable;
extern boolean finger_enable;
extern boolean pad_enable;
extern boolean padtotcp_enable;
extern boolean tcptopad_enable;
extern boolean nagle_enable;
extern boolean ipisup;
extern boolean etherstackf;
extern boolean log_init;
extern ulong nvsize;
extern boolean x25_present;

extern boolean arp_debug;
extern boolean broadcast_debug;
extern boolean buffers_debug;
extern boolean chaos_debug;
extern boolean chaospkt_debug;
extern boolean egp_debug;
extern boolean egpevent_debug;
extern boolean gwinfo_debug;
extern boolean hdh_debug;
extern boolean hello_debug;
extern boolean icmppkt_debug;
extern boolean igrp_debug;
extern boolean impevent_debug;
extern boolean imp_debug;
extern boolean ippkt_debug;
extern boolean lapb_debug;
extern boolean modem_debug;
extern boolean pad_debug;
extern boolean pkt_debug;
extern boolean pktevent_debug;
extern boolean printer_debug;
extern boolean puppkt_debug;
extern boolean rip_debug;
extern boolean route_debug;
extern boolean sched_debug;
extern boolean security_debug;
extern boolean securityv_debug;
extern boolean serial_debug;
extern boolean slip_debug;
extern boolean slipv_debug;
extern boolean tcp_debug;
extern boolean udp_debug;
extern boolean x25_debug;
extern boolean x25event_debug;
extern boolean xnspkt_debug;
extern boolean xnsrip_debug;

extern int ipdebug_acl;

extern boolean router_running;
extern boolean router_enable;
extern boolean rcache_enable;
extern long ipcache_version;
extern uchar ip_secopt_bytes;
extern uchar ip_secsys_low;
extern uchar ip_secsys_high;

#ifdef c_plusplus
ushort ipcrc(ushort *, int);
#else
ushort ipcrc();
#endif
