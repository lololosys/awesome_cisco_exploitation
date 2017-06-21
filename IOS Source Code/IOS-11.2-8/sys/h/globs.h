/* $Id: globs.h,v 3.13.10.16 1996/08/15 02:31:52 hampton Exp $
 * $Source: /release/112/cvs/Xsys/h/globs.h,v $
 *------------------------------------------------------------------
 * globs.h -- external storage and function declarations
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: globs.h,v $
 * Revision 3.13.10.16  1996/08/15  02:31:52  hampton
 * Change some function arguments to be 'const'.  [CSCdi66102]
 * Branch: California_branch
 *
 * Revision 3.13.10.15  1996/08/07  08:58:33  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.10.14  1996/08/03  23:22:29  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.13.10.13  1996/07/30  17:39:13  dblair
 * CSCdi59050:  multilink broken when dialing Ascend with 2 phone numbers
 * Branch: California_branch
 * add is_printable_string utility
 *
 * Revision 3.13.10.12  1996/07/08  22:17:06  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.13.10.11  1996/06/17  08:30:54  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.13.10.10  1996/06/12  19:09:18  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xx_bits (partial commit part 1)
 * Branch: California_branch
 *
 * Revision 3.13.10.9  1996/06/05  20:52:09  wmay
 * CSCdi59462:  DSX1 remote interface loopback process needs help
 * add serial_packet_debug to global list
 * Branch: California_branch
 *
 * Revision 3.13.10.8  1996/05/17  11:15:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.13.2.3  1996/05/05  23:05:02  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.13.2.2  1996/04/26  07:52:42  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.13.2.1  1996/04/03  14:25:24  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.13.10.7  1996/05/09  14:13:00  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.10.6  1996/05/08  14:43:46  widmer
 * CSCdi57047:  Bad sync moved prototypes back to globs.h
 * Branch: California_branch
 * Remove removed prototypes that moved back in.
 *
 * Revision 3.13.10.5  1996/05/04  00:42:12  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 * - New interafce type: BVI(Bridge-group Virtual Interface).
 * - New fast-switching flavor: FS_BVI.
 * - Introduce idb->tbridge_bridge_linktypes, idb->user_configured_mtu and
 *   pak->irb_rcvd_input.
 *
 * Constrained Multicast Flooding
 * - Declare tbifd_type.
 * - Define the CMF parser link points.
 * - Declare the return_one stub function.
 * - Declare the Transparent Bridging registry.
 * - Add tbridge_cmf to the fastswitch registry.
 *
 * Name Access List
 *
 * Revision 3.13.10.4.4.1  1996/04/27  06:35:11  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.10.4  1996/04/19  15:15:17  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.13.10.3  1996/04/15  14:57:39  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.13.10.2.6.2  1996/04/10  23:47:18  cakyol
 * change misleading variable name
 * from "global_boottime_config_version"
 * to "global_parsed_swversion".
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.13.10.2.6.1  1996/04/08  01:45:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.10.2  1996/03/21  22:37:45  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.13.10.1  1996/03/18  19:37:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.14.1  1996/03/25  02:18:55  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13  1996/02/27  05:59:31  afactor
 * CSCdi47194:  Add cookie capability for jalapeno mechanical option
 *
 * Revision 3.12  1996/02/13  20:49:35  hampton
 * Clean up includes of gdb.h and references to breakpoint.  [CSCdi48844]
 *
 * Revision 3.11.24.2  1996/03/22  05:25:31  cakyol
 * Found a better way of determining the version number of the
 * config file at boot time.  Leverage from parsing of the
 * "version xx.yy" command.  Define a new variable which
 * represents this value named "global_boottime_config_version".
 * Restore all the other files back to what they were before this.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.11.24.1  1996/03/20  23:45:57  cakyol
 * The 11.2 LECS parser can now FULLY recognize pre 11.2 LECS
 * commands .  As a result of this, we also have a new systemwide
 * global uint variable named "global_nvconfig_version", which
 * reflects the nvram version number in it.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.11  1995/12/26  23:06:39  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add rpratt's code for MIB/queue manipulation.
 *
 * Revision 3.10  1995/12/21  16:28:55  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.9  1995/12/15  08:00:42  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.8  1995/12/10  21:01:46  smackie
 * Next on a very special Geraldo. Unused global booleans and the files
 * that loved them. (CSCdi45340)
 *
 * Revision 3.7  1995/12/07  22:12:28  hampton
 * Add a couple of new routines for naming memory blocks.  Also add a
 * routine for specifying the allocator of the memory block.  This routine
 * is used by realloc, strdup, et al. to indicate the what code really
 * requested the allocation of memory.  [CSCdi45209]
 *
 * Revision 3.6  1995/12/03  17:56:28  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.5  1995/11/22  05:41:44  eschaffe
 * CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
 * option.
 * Obsolete PROTOCONVERT flag in makefiles.  The control for defining
 * protocolconversion for an image is now platform_specific.
 *
 * Revision 3.4  1995/11/21  07:28:48  lpereira
 * CSCdi44255:  APPN build is broken with the PortReady changes
 * Moved the enum HEADTAIL from globs.h to types.h so that interface.h
 * can be included without including globs.h. Also added a forward
 * reference to list_header in types.h.
 *
 * Revision 3.3  1995/11/17  09:12:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:23:54  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:35:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.19  1995/11/08  20:58:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.18  1995/10/04  21:25:38  albright
 * CSCdi36672:  EIGRP neigbor tables not reflect correct uptime
 *
 * Add support for displaying a seconds based timestamp.
 *
 * Revision 2.17  1995/09/25  08:34:15  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.16  1995/09/18  19:34:29  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.15  1995/09/13  07:01:29  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.14  1995/09/13 04:07:00  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.13  1995/09/12 16:24:12  hampton
 * Make the unix platform compile again.  [CSCdi39966]
 *
 * Revision 2.12  1995/09/09 00:51:12  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.11  1995/08/22  00:53:00  slin
 * CSCdi36007:  show apple route detail halts system
 *
 * Revision 2.10  1995/07/27  07:30:41  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.9  1995/07/22 06:05:13  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Fix over-zealousness in limiting output of "Building Configuration..."
 *
 * Revision 2.8  1995/07/12  23:43:46  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.7  1995/06/30 05:20:57  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.6  1995/06/21 08:54:38  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.5  1995/06/18  06:03:03  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase I--add centralized processing for malloc failures.
 *
 * Revision 2.4  1995/06/17  06:25:33  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.3  1995/06/16  17:07:23  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Move passive timer declarations to a new file.  Eliminate some obsolete
 * macros.
 *
 * Revision 2.2  1995/06/16  04:26:58  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:35:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __GLOBS_H__
#define __GLOBS_H__

/*
 * Include the definitions for the printf() facility. This does not
 * belong here, but there are references throughout the system to
 * printf() and family, and it used to be here, so this is a necessary
 * evil.
 */
#include "../os/printf.h"

/*
 * Include ANSI C definition. We moved many definitions from globs.h
 * into there proper ANSI places. For now we include them here to avoid
 * touching every file in the system. Eventually, this should go away
 * and only files that require these includes should get them.
 */
#include "setjmp.h"

/*
 * Ubiquitous constants
 */
#define IEEEBYTES 6
#define MAX_IPPROCS	32
#define MAX_NOVPROCS    32


enum INOUT {
    INPUT,
    OUTPUT
};

enum SRCDST {
    SOURCE,
    DESTINATION
};
#define GET_SOURCE SOURCE
#define GET_DEST DESTINATION

#define PASSWORDLEN	25
#define PASSWORDLEN_ENCRYPTED	(PASSWORDLEN * 2 + 2)

/*
 * These declarations are for intrinsic C types.  Compound types would
 * require including various header files with the attendant complexity.
 */

extern char *hostname;			/* our host name */
extern char *boot_reason;		/* reason why system was rebooted */
extern uint config_bytes;		/* bytes in a configuration buffer */
extern boolean system_running;		/* TRUE if system has started scheduler */
extern boolean system_configured;	/* TRUE if a configuration files read */
extern boolean system_loading;		/* TRUE if loading a system */
extern boolean hotstall_flag;		/* TRUE if processing a hotswap */
extern watched_boolean *net_throttle_flag; /* interfaces in throttled state */
extern int nconlines;			/* total number of console lines */
extern int nttylines;			/* total count of async tty lines */
extern int nauxlines;			/* total count of auxiliary lines */
extern int nvtylines;			/* total count of virtual lines */
extern int ndummyttylines;		/* total count of dummy tty lines */
extern int ALLlines;			/* total line count */
extern int freeLineBase;		/* first free line, drivers update */
extern int VTYBase;
extern int AuxBase;
extern int nnets;			/* total non-virtual interfaces */
extern int nethernets;			/* total number of Ethernets */
extern int netherhubports;              /* total number of ether hub ports */
extern int nserialnets;  		/* total number of serial interfaces */
extern int nas_serialnets;  		/* number of serial(sync/async) ints */
extern int nTRs;			/* total number of Token Rings */
extern int nbrinets;                    /* number of Basic Rate interfaces */
extern int nfddinets;                   /* number of FDDI interfaces */
extern int nhssinets;                   /* number of HSSI interfaces */
extern int npcbusnets;                  /* number of PCbus interfaces */
extern int natmnets;			/* number of ATM interfaces */
extern int nchannelnets;                /* number of channel interfaces */
extern int nfethernets;                 /* number of FastEthernet interfaces */
extern int ne1ports;                   	/* number of Channelized E1 ports */
extern int nt1ports;                   	/* number of Channelized T1 ports */
extern int nse1ports;                  	/* number of Channelized E1 ports in SMIP */
extern int nst1ports;                  	/* number of Channelized T1 ports in SMIP */
extern int nSSIP;                       /* number of SSIP controllers */
extern int nSMIP;                       /* number of SMIP controllers */
extern int ssip_ser;                    /* sum of all serials on all SSIP's */     
extern char *version;			/* version of the software */

/*
** the version number that was in the config file
** when "version XX.Y" command was last parsed.
*/
extern uint global_parsed_swversion;
extern char *nomemory;

extern boolean romcode;			/* Are we running from ROM? */

extern boolean decimaltty_enable;	/* service decimal-tty */
extern boolean subnetzero_enable;	/* ip subnet-zero */
extern boolean netconfig_enable;	/* service config */
extern boolean domain_enable;		/* ip domain-lookup */
extern boolean arapmnp4_debug;
extern boolean arapv42bis_debug;
extern boolean arapinternal_debug;
extern boolean arapmemory_debug;
extern boolean linenumber_enable;	/* service linenumber */
extern boolean execwait_enable;		/* service exec-wait */
extern boolean password_encryption_enable; /* service password-encryption */
extern boolean exec_callback_enable;    /* service exec callback */
extern boolean protocolconversion;
extern boolean bridge_enable;
extern boolean srswitch_enable;
extern boolean x25_enable;
extern boolean slavecore_enable;
extern boolean slavelog_enable;
extern boolean lat_enable;
extern int ciscopro_image;
extern boolean ciscopro_platform;
extern boolean ciscoet_platform;
extern boolean log_init;
extern long nvsize, nvused;

extern boolean arp_debug;
extern boolean buffer_debug;
extern boolean dlsw_peer_debug;
extern boolean dlsw_csm_debug;
extern boolean dnsix_nat_debug;
extern boolean dnsix_dmdp_debug;
extern boolean egp_debug;
extern boolean egpevent_debug;
extern boolean frame_relay_debug;
extern boolean frpkt_debug;
extern boolean icmppkt_debug;
extern boolean igrp_debug;
extern boolean ippkt_debug;
extern boolean ipsecurity_debug;
extern boolean lapb_debug;
extern boolean modem_debug;
extern boolean netbios_name_cache_debug;
extern boolean pad_debug;
extern boolean rif_debug;
extern boolean rip_debug;
extern boolean route_debug;
extern boolean sdlc_debug;
extern boolean sdllc_debug;
extern boolean serial_debug;
extern boolean serial_packet_debug;
extern boolean srb_debug;
extern boolean srbevent_debug;
extern boolean stun_debug;
extern boolean stun_tg_debug;
extern boolean stun_packet_debug;
extern boolean subsys_debug;
extern boolean tokenring_debug;
extern boolean tokenevent_debug;
extern boolean udp_debug;
extern boolean x25_debug;
extern boolean x25event_debug;
extern boolean xnspkt_debug;
extern boolean xnsrip_debug;
extern boolean ospf_debug;
extern boolean ospf_debug_events;
extern boolean ospf_debug_flood;
extern boolean ospf_debug_adj;
extern boolean ospf_debug_spf;
extern boolean ospf_debug_spf_monitor;
extern boolean ospf_debug_tree;
extern boolean igrp2_packet_debug;
extern boolean igrp2_fsm_debug;
extern boolean ipigrp2_debug;
extern boolean lnx_debug;
extern boolean lnx_error_debug;
extern boolean qllc_state_debug;
extern boolean qllc_event_debug;
extern boolean qllc_packet_debug;
extern boolean qllc_timer_debug;
extern boolean qllc_x25_debug;

extern boolean systeminit_flag;
extern boolean router_enable;
extern uchar ip_secopt_bytes;
extern uchar ip_secsys_low;
extern uchar ip_secsys_high;
extern uchar ip_secsys_auth_low;
extern uchar ip_secsys_auth_high;
extern boolean vty_logging;

extern boolean bootstrap_enable;

/*
 * define data structure for named things.
 * NAMEDTHINGMACRO defines those fields common to all named things.
 * This allows common code to find named things in queues, and so on.
 */

#define NAMEDTHINGNAMELEN 20
#define NAMEDTHINGMACRO \
	char  name[NAMEDTHINGNAMELEN+1];


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
/*
 * Common routines
 *
 * note - to work properly on r4k platforms, SAVE_CALLER() must
 * precede all function calls in a non-inline function.  While it is
 * syntactically correct to use SAVE_CALLER() in an inline function or
 * first in a nested block, it will not operate correctly in those
 * situations.
 */
#define SAVE_CALLER()	caller_t saved_caller_pc = cpu_caller()
#define caller()	((ulong) saved_caller_pc)
extern ulong current_pc(void);

extern int nethash(ulong);

/*
 * os/queue.c
 */
extern boolean checkqueue (queuetype *, void *);
extern boolean queueBLOCK(queuetype *);
extern void *dequeue(queuetype *);
extern void *p_dequeue(queuetype *);
extern void enqueue(queuetype *, void *);
extern void p_enqueue(queuetype *, void *);
extern void p_requeue(queuetype *, void *);
extern void p_swapqueue(queuetype *, void *, void *);
extern void p_unqueue(queuetype *, void *);
extern void p_unqueuenext(queuetype *, void **);
extern void queue_init(queuetype *, int);
extern void requeue(queuetype *, void *);
extern void unqueue(queuetype *, void *);
extern void insqueue(queuetype *, void *, void *);
extern void *remqueue(queuetype *, void *, void *);
extern boolean buffer_validate(paktype *);
extern int queryqueuedepth(queuetype *);
extern void *peekqueuehead(queuetype *);

/*
 * os/free.c
 */ 

extern void *malloc(size_t size);
extern void *malloc_aligned(uint size, uint align);
extern void *malloc_named(uint size, const char *name);
extern void *malloc_named_aligned(uint size, const char *name, uint align);
extern void *malloc_iomem(uint size);
extern void *malloc_named_iomem(uint size, const char *name);
extern void *malloc_iomem_aligned(uint size, uint align);
extern void *malloc_named_iomem_aligned(uint size, uint align, const char *);
extern void *malloc_pcimem(uint size);
extern void *malloc_named_pcimem(uint size, const char *name);
extern void *malloc_fast(uint size);
extern void *malloc_named_fast(uint size, const char *name);
extern void memory_new_owner(void *mem);
extern void memory_specify_allocator(void *mem, ulong allocator_pc);
extern void free(void *ptr);
extern void mem_lock(void *ptr);
extern int mem_refcount(void *ptr);
extern boolean validmem(void *location);
extern boolean validlink(void *location);
extern void *realloc(void *ptr, size_t size);
extern void setstring(char **, const char *);
extern void setstring_named(char **, const char *, char *);

/*
 * os/init.c
 */

extern int startup_ttynum;
extern boolean systeminitBLOCK(queuetype *);

/*
 * os/init_68k.c
 */

extern boolean touch_device(void *);

/*
 * Prototypes for os/logger.c
 */

extern void buginf(const char *, ...);

/*
 * Prototypes for os/ttysrv.c
 */

extern boolean psipending(void);
extern boolean psipending2(tt_soc *);
extern boolean psiset(tt_soc *);
extern void psioff(tt_soc *);
extern void psion(tt_soc *);

/*
 * os/time_util.c
 */
extern boolean system_clock_init(void);
extern int sprint_dhms(char *, sys_timestamp);
extern int sprint_ticks_in_dhms(char *instring, long ticks);
extern void get_timestamp(volatile sys_timestamp *timestamp);
extern void get_timestamp32(volatile ulong *timestamp32);
extern void print_dhms(sys_timestamp);
extern void print_ticks_in_dhms(long deltatime);
extern void print_uptime(void);
extern void timestamp_printf_init(void);

/*
 * os/util.c
 */

extern boolean ExactToSPsmatch(char *, char *);
extern boolean confirm(char *);
extern boolean doit(void);
extern boolean int_array_op(int *, int *, int, int, boolean);
extern boolean partialmatch(char const *, char *);
extern boolean return_false(void *, ...);
extern boolean return_true(void *, ...);
extern fuzzy return_fuz_false(void *, ...);
extern fuzzy return_fuz_true(void *, ...);
extern boolean strmatch(char *, char *, boolean);
extern boolean t_number(char *);
extern boolean yes_or_no(const char *, boolean, boolean);
extern boolean yes_or_no2(jmp_buf *, const char *, boolean, boolean);
extern char *findarg(char *);
extern char *read_string2(jmp_buf *, char *, const char *, char *);
extern int nmatch(char *, char *const [], const ushort [], int, int *);
extern long return_zero(void);
extern long return_one(void);
extern uchar *return_null(void *, ...);
extern uint xnethash(uchar *, ushort);
extern ulong get_num_in_range(const char *, int, ulong, ulong, ulong);
extern ulong get_num_in_range2(jmp_buf *, const char *, int, ulong, ulong, ulong);
extern ulong parse_hex_long(char *);
extern ulong parse_decimal_long(char *, boolean *);
extern ulong parse_unsigned(char *, boolean *);
extern char *octet(char *, ulong *, char );
extern char *strdup(const char *);
extern char *strdup_named(const char *, const char *);
extern void notset(int);
PRINTF_FUNCTION(extern int safe_sprintf(char **, uint *, const char *, ...),
		3, 4);
extern void strcat_with_realloc(char **, uint *, char *);
extern void *queuepick(void *, int);
extern void lw_insert(dqueue_t *, dqueue_t *);
extern void lw_remove(dqueue_t *);
extern void o_init(dqueue_t *, void *);
extern void o_enqueue(dqueue_t *, dqueue_t *);
extern dqueue_t *o_dequeue(dqueue_t *);
extern void o_unqueue(dqueue_t *);
extern void return_nothing(void *, ...);
extern boolean instring (char *, char *);
extern boolean is_printable_string(const char *, int);

/*
 * os/stacks.c
 */
extern uint onintstack(void);
extern int stack_remaining(void);
extern void process_stacks(void);

/*
 * sun/asm.s
 */

extern boolean ipttl(uchar *);
extern boolean memhammer(int, int, int, int);
extern boolean memhammer_byte(int, int, int, int);
extern boolean memhammer_word(int, int, int, int);
extern int get_interrupt_level(void);
extern leveltype set_interrupt_level(int);
extern leveltype raise_interrupt_level(int);
extern ulong emt_call(int, ...);
extern ushort swap_16bit_word(ushort);
extern ulong swap_32bit_word(ulong);
extern ulong vaxorder(ulong);
extern ulong *caller_frame(void);
extern ulong *trace_caller(ulong *, ulong **);
extern void (**getvbr(void))(void);
extern void (**setvbr(void (**)(void)))(void);
extern void (*exceptionHandler(int, void (*)(void)))(void);

extern ushort ipcrc(ushort *, int);
extern ushort iphcrc(ushort *, int);
extern ushort iphdrchk(ushort *);
extern ushort iphtcrc(ushort *);
extern void bwcopy(uchar const *, ushort *, int);
extern void exception_handler(void);
extern void exception_jump(void fn(void));
extern void mci2mci(volatile void *, volatile void *, int);
extern void reset_interrupt_level(leveltype);
extern void resume(void);
extern void return_from_exception(void);
extern void _rom_monitor_return(void);
extern void scopy(void *, void *, int);
extern void scopy2(void *, void *, int, ulong);
extern void wbcopy(ushort const *, uchar *, int);
extern void cache_flush(void);

/*
 * sun/asm2.s
 */

extern void mci2buffer(volatile void *, void *, int);
extern void mci2buffer2(volatile void *, void *, int, ulong);
extern void buffer2mci(void *, volatile void *, int);
extern void buffer2mcis(void *, volatile void *, int);

/*
 * os/nv_common.c
 */

extern void nv_add(boolean, char *, ...);
extern void nv_add_encrypted(boolean, char *, ...);
extern void nv_write(boolean, char *, ...);
extern void nv_write_encrypted(boolean, char *, ...);

/*
 * os/exception.h
 */
extern boolean breakpoint(void);
extern void crashdump(int);

/*
 * if/network.c
 */
struct idbtype_;
struct hwidbtype_;


#endif __GLOBS_H__
