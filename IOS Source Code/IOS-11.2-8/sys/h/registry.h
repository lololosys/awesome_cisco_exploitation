/* $Id: registry.h,v 3.4.20.10 1996/08/27 23:25:00 sdurham Exp $
 * $Source: /release/112/cvs/Xsys/h/registry.h,v $
 *------------------------------------------------------------------
 * registry.h -- Registration Definitions
 *
 * 5-March-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: registry.h,v $
 * Revision 3.4.20.10  1996/08/27  23:25:00  sdurham
 * CSCdi66970:  %SYS-2-SUPNOT: Cant create Registry RMON
 * Branch: California_branch - ran out of dynamic registries.  add a few
 * more
 * to keep us going...
 *
 * Revision 3.4.20.9  1996/08/12  02:15:48  ppearce
 * CSCdi64450:  CSCdi59527 broke c4500-boot-m image
 * Branch: California_branch
 *   Add srb_core_registry that will definitely be included w/SRB subblock
 *
 * Revision 3.4.20.8  1996/07/11  07:50:03  gcox
 * CSCdi62515:  remove redundant field in registry struct - save 6K
 * Branch: California_branch
 *
 * Revision 3.4.20.7  1996/05/21  06:27:51  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.4.20.6  1996/05/17  11:16:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.2  1996/05/05  23:05:34  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.4.6.1  1996/03/22  19:08:14  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.4.20.5  1996/05/09  14:13:33  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.20.4  1996/05/04  00:42:31  wilber
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
 * Revision 3.4.20.3  1996/04/27  05:47:08  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.20.2.2.1  1996/04/27  06:35:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.20.2  1996/04/25  23:11:20  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.14.4  1996/04/26  00:50:47  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.14.3  1996/04/17  01:32:40  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.3.14.2  1996/01/26  20:25:16  agh
 * Branch: Dial1_branch
 * Initial Checkin of MLPVT Code.
 *
 * Revision 3.3.14.1  1996/01/18  00:16:49  syiu
 * Branch: Dial1_branch
 * PPP L2F registry support
 *
 * Revision 3.4.20.1  1996/04/19  15:15:31  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.4.34.1  1996/04/08  01:46:05  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.26.1  1996/03/22  09:35:50  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.10.1  1996/03/05  06:10:38  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.4.4.1  1996/02/27  20:41:27  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4  1996/01/25  11:17:27  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.3.8.1  1996/01/24  01:34:01  cyoung
 * VTP MIB support and dependant auto-config base. Part I
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3  1995/12/01  19:02:53  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 * Revision 3.2  1995/11/17  09:13:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:58:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/23  21:38:41  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Add an ISIS registry.  STRUCTURE_REF some structures.
 *
 * Revision 2.3  1995/08/30  16:21:54  richl
 * CSCdi39553:  Modularity: FR regcode global breaks modular-dlsw
 *
 * Revision 2.2  1995/07/07  22:01:31  smackie
 * Remove between 50 and 85K of text segment space by compiling out the
 * little-used registry service and function strings. (CSCdi36802)
 *
 * Revision 2.1  1995/06/07  20:36:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __REGISTRY_H__
#define __REGISTRY_H__

/*
 * Registry string support
 */

#ifdef REGISTRY_DISPLAY_STRINGS
#define REGISTRY_STRING(name)          name
#define REGISTRY_STRING_DEFINE(define) define
#else
#define REGISTRY_STRING(name)          NULL
#define REGISTRY_STRING_DEFINE(define) 
#endif

/*
 * A registry is a collection of associated services.
 * REG_SYS is created by the kernel to handle kernel specific
 * functions.  Additional registries can be created for application
 * specific schemes.
 */

typedef enum {
    REG_ILLEGAL    = -1,
    REG_UNKNOWN    =  0,
    REG_MINCOUNT   =  1,
    REG_SYS        =  REG_MINCOUNT, /* the kernel's registry */
    REG_MEDIA,			/* the Data Link layer registry */
    REG_FS,	        	/* the fast switching registry */
    REG_IP,			/* TCP/IP support */
    REG_CS,			/* communications server specific */
    REG_SRB_CORE,		/* Core Source-Route bridging registry */
    REG_SRT,			/* bridging support */
    REG_TBRIDGE,		/* Transparent Bridging registry */
    REG_CBUS,			/* cbus platform specific */
    REG_SSE,			/* SSE support */
    REG_PT,			/* Protocol Translator specific */
    REG_SNMP,			/* SNMP support */
    REG_IPX,			/* IPX support */
    REG_FLASH,			/* Flash related registry */
    REG_DIALER,			/* Dialer support */
    REG_PPP,                    /* PPP support */
    REG_CLNS,			/* CLNS support */
    REG_DLC,                    /* CLS-to-DLC registry */
    REG_SNANM,                  /* SNA network management registry */
    REG_LANMGR,                 /* Lan manger registry */
    REG_LANE,                   /* LAN Emulation registry */
    REG_PARTNER,		/* partner support */
    REG_FR_SVC,                 /* Frame Relay SVC specific registries */
    REG_AUTH,			/* Authentication, Authorization, and such */
    REG_ISIS,			/* ISIS support */
    REG_FRAME_RELAY,            /* Frame Relay driver */
    REG_IFMIB,                  /* IF MIB support */
    REG_ENVMIB,                 /* ENV MIB support */
    REG_CHASSISMIB,             /* CHASSIS MIB support */
    REG_RTT,                    /* Round Trip Time Monitor registry */
    REG_MLPVT,			/* MLPVT Group support */
    REG_CRYPTO,
    REG_CRYPTO_RSP,
    REG_VTP,			/* VTP MIB registry */
    REG_EIGRP,			/* EIGRP support */
    REG_DYNAMIC1,		/* Start of dynamic registries */
    REG_DYNAMIC2,
    REG_DYNAMIC3,
    REG_DYNAMIC4,
    REG_DYNAMIC5,
    REG_DYNAMIC6,
    REG_DYNAMIC7,
    REG_DYNAMIC8,
    REG_DYNAMIC9,
    REG_DYNAMIC10,
    REG_DYNAMIC11,
    REG_DYNAMIC12,
    REG_DYNAMIC13,
    REG_DYNAMIC14,
    REG_DYNAMIC15,
    REG_DYNAMIC16,
    REG_DYNAMIC17,
    REG_DYNAMIC18,
    REG_DYNAMIC19,
    REG_DYNAMIC20,
    REG_DYNAMIC21,
    REG_DYNAMIC22,
    REG_DYNAMIC23,
    REG_DYNAMIC24,
    REG_DYNAMIC25,
    REG_DYNAMIC26,
    REG_DYNAMIC27,
    REG_DYNAMIC28,
    REG_DYNAMIC29,
    REG_DYNAMIC30,
    REG_DYNAMIC31,
    REG_DYNAMIC32,
    REG_DYNAMIC33,
    REG_DYNAMIC34,
    REG_DYNAMIC35,
    REG_DYNAMIC36,
    REG_DYNAMIC37,
    REG_DYNAMIC38,
    REG_DYNAMIC39,
    REG_DYNAMIC40,
    REG_MAXCOUNT		/* allow this many registries  */
} regtype_t;


/*
 * Structures
 */

typedef struct functype_ {
    union {
	struct functype_ *next;  	/* used by linked lists */
	boolean assigned;	        /* used by case tables */
    } i;
    long index;
    union {
	long value;			/* used by SERVICE_VALUE */
	void *routine;			/* used by everything else */
    } j;

    REGISTRY_STRING_DEFINE(char *name;)
} functype;

typedef struct regservicetype_ {
    ushort      servcode;
    ushort      servtype;
    ushort      numargs;
    ushort      maxentries;
    functype   *function_default;
    functype   *function_table;
    functype   *function_queue;

    REGISTRY_STRING_DEFINE(char *name;)
} regservicetype;

typedef struct registrytype_ {
    int              servicecount;
    char const      *name;
    regservicetype  *services;
} registrytype;


/*
 * Types of services within a registry.
 *
 * - SERVICE_LIST is a list of functions that are executed sequentially
 *   by registry_list() or registry_loop().
 *
 * - SERVICE_CASE is a switch table of functions.
 *   Use registry_case() or registry_retval() to select a particular function.
 *
 * - SERVICE_VALUE is a table of values.
 *   Use registry_value() to lookup the constant for a particular index,
 *   e.g. ethertype to link code translations.
 *
 * - SERVICE_STUB is the degenerate case of SERVICE_LIST.  There is only
 *   ever one function and it may return a value.  See registry_stub().  
 */

#define  SERVICE_NONE      0x0000
#define  SERVICE_LIST      0x0001
#define  SERVICE_CASE      0x0002
#define  SERVICE_VALUE     0x0003
#define  SERVICE_STUB      0x0004
#define  SERVICE_PID_LIST  0x0005
#define  SERVICE_PID_CASE  0x0006
#define  SERVICE_ANY       0x0007

#define  SERVICE_MIN       SERVICE_LIST
#define  SERVICE_MAX       SERVICE_ANY

/*
 * A registered function can have from zero to ten arguments.
 */
#define SERVICE_0ARG   0
#define SERVICE_1ARG   1
#define SERVICE_2ARG   2
#define SERVICE_3ARG   3
#define SERVICE_4ARG   4
#define SERVICE_5ARG   5
#define SERVICE_6ARG   6
#define SERVICE_7ARG   7
#define SERVICE_8ARG   8
#define SERVICE_9ARG   9
#define SERVICE_10ARG 10

#define SERVICE_MAXARG 10


/*
 *                       Fast Registry Execution
 *
 * It is critical in the router's fast switching code that case registries
 * execute few instructions.  We do this by eliminating all sorts of
 * argument checking, inserting default functions into the unused
 * entries of a casearray and not using the reg_default functionality.
 * If the caller screws up, the system will probably crash.  This was
 * the case prior to registries as well.  
 *
 * We are trading off PPS numbers against robustness.  This macro should
 * only be used in the main line fast switching code.  I can think of
 * no good reason for it to be used elsewhere.
 *
 * When used, the REG and SERVICE are manifest constants.  INDEX is usually
 * an RXTYPE code and ARG is a hwidb pointer (high end) or a paktype
 * pointer (low end).  Depending on the circumstances and architecture,
 * this should compile to four to six instructions.
 */

typedef boolean (*void1)(void *); /* for type coercion in macro below */
typedef boolean (*void2)(void *, void *);
typedef boolean (*void3)(void *, void *, void *);
typedef boolean (*void4)(void *, void *, ulong, ulong);
extern registrytype registry_array[REG_MAXCOUNT];

#define REGISTRY_ROUTINE(REG, SERVICE, INDEX) \
   registry_array[REG].services[SERVICE].function_table[INDEX].j.routine
#define REGISTRY_DEFAULT_ROUTINE(REG, SERVICE) \
   registry_array[REG].services[SERVICE].function_default->j.routine
 
#define REGISTRY_FAST(REG, SERVICE, INDEX, ARG) \
   ((*(void1)REGISTRY_ROUTINE(REG,SERVICE,INDEX))(ARG))
 
#define REGISTRY_FAST2(REG, SERVICE, INDEX, ARG1, ARG2) \
   ((*(void2)REGISTRY_ROUTINE(REG,SERVICE,INDEX))(ARG1,ARG2))
 
#define REGISTRY_FAST_STUB(REG, SERVICE, ARG) \
   ((*(void1)REGISTRY_DEFAULT_ROUTINE(REG,SERVICE))(ARG))
 
#define REGISTRY_FAST_STUB2(REG, SERVICE, ARG1, ARG2) \
   ((*(void2)REGISTRY_DEFAULT_ROUTINE(REG,SERVICE))(ARG1,ARG2))
 
#define REGISTRY_FAST_STUB3(REG, SERVICE, ARG1, ARG2, ARG3) \
   ((*(void3)REGISTRY_DEFAULT_ROUTINE(REG,SERVICE))(ARG1,ARG2,ARG3))
 
#define REGISTRY_FAST_STUB4(REG, SERVICE, ARG1, ARG2, ARG3, ARG4) \
   ((*(void4)REGISTRY_DEFAULT_ROUTINE(REG,SERVICE))(ARG1,ARG2,ARG3,ARG4))



/*
 * Some convenient macros
 * Because sometimes the symbols are just too long.
 */

/*
 * BRIDGING_IF - are we bridging this protocol on this i/f?
 * ROUTING_IF - are we routing this protocol on this i/f?
 * NB: These are NOT complements of each other.
 */

#define BRIDGING_IF(idb, linktype) \
    (reg_invoke_proto_bridging_on_swidb(linktype, idb))

#define ROUTING_IF(idb, linktype) \
    (reg_invoke_proto_routing_on_swidb(linktype, idb))

/*
 * RUNNING - is protocol enabled anywhere in the system?
 * RUNNING_IF - is protocol enabled on this particular interface?
 */
#define RUNNING(linktype) \
    (reg_invoke_proto_running(linktype))

#define RUNNING_IF(linktype, idb) \
    (reg_invoke_proto_on_swidb(linktype, idb))

/*
 * External Prototypes
 */

/*
 * registry.c
 */

extern void registry_init(void);
extern regtype_t registry_create(regtype_t regcode, int servcount,
				 char *name);
extern void service_create(regtype_t regcode, ushort servcode,
			   ushort servtype,ushort maxentries,
			   ushort numargs, char *name);
extern boolean registry_add_case(regtype_t regcode, ushort servcode,
				 long index, void *routine, char *name);
extern boolean registry_add_list(regtype_t regcode, ushort servcode,
				 void *routine, char *name);
extern boolean registry_add_default(regtype_t regcode, ushort servcode,
				    void *routine, char *name);
extern boolean registry_add_stub(regtype_t regcode, ushort servcode,
				 void *routine, char *name);
extern boolean registry_add_value(regtype_t regcode, ushort servcode,
				  long index, ulong value, char *name);
extern boolean registry_add_default_value(regtype_t regcode, ushort servcode,
					  ulong value, char *name);
extern void registry_delete_case(regtype_t regcode, ushort servcode,
				 long index);
extern void registry_delete_default(regtype_t regcode, ushort servcode);
extern void registry_delete_list(regtype_t regcode, ushort servcode,
				 void *routine);
extern void registry_delete_stub(regtype_t regcode, ushort servcode);
extern boolean registry_add_pid_list(regtype_t regcode, ushort servcode,
				     pid_t pid, char *name);
extern void registry_delete_pid_list(regtype_t regcode, ushort servcode,
				     pid_t pid);
extern boolean registry_add_pid_case(regtype_t regcode, ushort servcode,
				     long index, pid_t pid, char *name);
extern void registry_delete_pid_case(regtype_t regcode, ushort servcode,
				     long index);
extern void registry_case(regtype_t regcode, ulong servcode,
			  long index, ...);
extern void registry_list(regtype_t regcode, ulong servcode, ...);
extern long registry_retval(regtype_t regcode, ulong servcode,
			    long index, ...);
extern ulong registry_value(regtype_t regcode, ulong servcode, long index);
extern boolean registry_case_used(regtype_t regcode, ulong servcode,
				  long index);
extern boolean registry_loop(regtype_t regcode, ulong servcode, ...);
extern long registry_stub(regtype_t regcode, ulong servcode, ...);
extern void registry_pid_list(regtype_t regcode, ulong servcode,
			      ulong signal,...);
extern void registry_pid_case(regtype_t regcode, ulong servcode,
			      long index, ulong signal, ...);
extern boolean registry_allocated(regtype_t regcode);
extern boolean registry_populated(regtype_t regcode);

#endif __REGISTRY_H__
