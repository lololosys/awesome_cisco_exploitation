/* $Id: sse_private.h,v 3.2.60.2 1996/04/29 09:20:39 gchristy Exp $
 * $Source: /release/112/cvs/Xsys/hes/sse_private.h,v $
 *------------------------------------------------------------------
 * sse_private.h Definitions for the SSE
 *
 * Sept., 1993 Tony Li
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse_private.h,v $
 * Revision 3.2.60.2  1996/04/29  09:20:39  gchristy
 * CSCdi50875:  SSE: handle low memory conditions more gracefully
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:43:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:48:23  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:21:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:33:51  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/08/08  08:55:57  tli
 * CSCdi37030:  SSE: input i/f MIP fails to SSE switch
 * Be sure to set the routing capability bits for all channel groups,
 * unless CRB is enabled.
 *
 * Revision 2.1  1995/06/07  20:45:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SSE_REFRESH	(60*ONESEC)   /* seconds between automatic refreshes */
#define SSE_MINSLEEP	(10*ONESEC)   /* seconds min delay between refreshes */
#define SSE_UNMAPPED	-1	      /* unmapped addresses */
#define SSE_MAXHANG	1	      /* number of hangs before hard fail */
#define SSE_FREE_WATCHDOG 70000	      /* free this many nodes before whine */

#define SSE_SIMULATOR_ICB 0x45
#define SSE_ICB_SHIFT	2	      /* shift icb before passing to ucode */

/*
 * SSE instructions
 */

enum SSE_INST {
    SSE_INST_NOP		= 0x00,
    SSE_INST_CALL		= 0x01,
    SSE_INST_HANG		= 0x02,
    SSE_INST_RET		= 0x03,
    SSE_INST_NEXT_DMA		= 0x04,
    SSE_INST_AND_PIPE		= 0x05,
    SSE_INST_LD_COUNT1		= 0x06,
    SSE_INST_LD_COUNT0		= 0x07,
    SSE_INST_SET_DEC		= 0x08,
    SSE_INST_RST_DEC		= 0x09,
    SSE_INST_LD_SPAGE		= 0x0A,
    SSE_INST_XOR_SREG0		= 0x0B,
    SSE_INST_RD_RAND		= 0x0C,
    SSE_INST_RD_CKSUM		= 0x0D,
    SSE_INST_DONE		= 0x0E,
    SSE_INST_LD_RSLT		= 0x10,	/* Add 0-f to get register */
    SSE_INST_LD_SREG		= 0x20,	/* Add 0-f to get register */
    SSE_INST_RD_SREG		= 0x30,	/* Add 0-f to get register */
    SSE_INST_ACC_CKSUM		= 0x40,
    SSE_INST_RD_BYTE		= 0x80
};

#define SSE_INST_OK		(SSE_INST_ACC_CKSUM|SSE_INST_RD_BYTE)
#define SSE_INST_OUTCLASS	(SSE_INST_LD_RSLT + 0)
#define SSE_INST_OUTICB 	(SSE_INST_LD_RSLT + 1)
#define SSE_INST_OUTRWLEN 	(SSE_INST_LD_RSLT + 3)
#define SSE_INST_OUTPTR1 	(SSE_INST_LD_RSLT + 6)
#define SSE_INST_OUTPTR2 	(SSE_INST_LD_RSLT + 7)
#define SSE_INST_OUTIPXTC 	(SSE_INST_LD_RSLT + 8)
#define SSE_INST_OUTSNAPLEN 	(SSE_INST_LD_RSLT + 9)
#define SSE_INST_OUTDST1   	(SSE_INST_LD_RSLT + 8)
#define SSE_INST_OUTDST2   	(SSE_INST_LD_RSLT + 9)
#define SSE_INST_OUTSRC1   	(SSE_INST_LD_RSLT + 0xA)
#define SSE_INST_OUTSRC2   	(SSE_INST_LD_RSLT + 0xB)
#define SSE_INST_LD_SREG_0 	(SSE_INST_LD_SREG)
#define SSE_INST_LD_SREG_1 	(SSE_INST_LD_SREG + 1)
#define SSE_INST_LD_SREG_2 	(SSE_INST_LD_SREG + 2)
#define SSE_INST_LD_SREG_6 	(SSE_INST_LD_SREG + 6)
#define SSE_INST_LD_SREG_D 	(SSE_INST_LD_SREG + 0xD)
#define SSE_INST_LD_SREG_E 	(SSE_INST_LD_SREG + 0xE)
#define SSE_INST_LD_SREG_F 	(SSE_INST_LD_SREG + 0xF)
#define SSE_INST_LD_IPDST	SSE_INST_LD_SREG_6
#define SSE_INST_LD_IPTCPFLAGS	SSE_INST_LD_SREG_D
#define SSE_INST_LD_IPSPORT_MSB	SSE_INST_LD_SREG_E
#define SSE_INST_LD_IPSPORT_LSB	SSE_INST_LD_SREG_F
#define SSE_INST_LD_IPDPORT_MSB	SSE_INST_LD_SREG_0
#define SSE_INST_LD_IPDPORT_LSB	SSE_INST_LD_SREG_1
#define SSE_INST_RD_SREG_0 	(SSE_INST_RD_SREG)
#define SSE_INST_RD_SREG_1 	(SSE_INST_RD_SREG + 1)
#define SSE_INST_RD_SREG_2 	(SSE_INST_RD_SREG + 2)
#define SSE_INST_RD_SREG_6	(SSE_INST_RD_SREG + 6)
#define SSE_INST_RD_SREG_A	(SSE_INST_RD_SREG + 0xA)
#define SSE_INST_RD_SREG_B	(SSE_INST_RD_SREG + 0xB)
#define SSE_INST_RD_SREG_C	(SSE_INST_RD_SREG + 0xC)
#define SSE_INST_RD_SREG_D 	(SSE_INST_RD_SREG + 0xD)
#define SSE_INST_RD_SREG_E 	(SSE_INST_RD_SREG + 0xE)
#define SSE_INST_RD_SREG_F 	(SSE_INST_RD_SREG + 0xF)
#define SSE_INST_RD_IPTOS	SSE_INST_RD_SREG_1
#define SSE_INST_RD_IPSRC	SSE_INST_RD_SREG_2
#define SSE_INST_RD_IPDST	SSE_INST_RD_SREG_6
#define SSE_INST_RD_IPXHC	SSE_INST_RD_SREG_6
#define SSE_INST_RD_IPPRO	SSE_INST_RD_SREG_C
#define SSE_INST_RD_IPFO_MSB	SSE_INST_RD_SREG_A
#define SSE_INST_RD_IPFO_LSB	SSE_INST_RD_SREG_B
#define SSE_INST_RD_IPTCPFLAGS	SSE_INST_RD_SREG_D
#define SSE_INST_RD_IPSPORT_MSB	SSE_INST_RD_SREG_E
#define SSE_INST_RD_IPSPORT_LSB	SSE_INST_RD_SREG_F
#define SSE_INST_RD_IPDPORT_MSB	SSE_INST_RD_SREG_0
#define SSE_INST_RD_IPDPORT_LSB	SSE_INST_RD_SREG_1

/*
 * SSE output classes, replicated in sseasm.w
 */

enum SSE_CLASS {
    SSE_CLASS_UNKNOWN	= 0,
    SSE_CLASS_FLUSH	= 1,
    SSE_CLASS_IP	= 2,
    SSE_CLASS_AUTON	= 3,
    SSE_CLASS_IPX	= 4,
    SSE_CLASS_BRIDGE	= 5,
    SSE_CLASS_SRB	= 6,
    SSE_CLASS_CLNP	= 0xB,
};

/*
 * Routing protocol mask bits, replicated in sseasm.w
 */

#define SSE_ROUTING_ATALK	0x01
#define SSE_ROUTING_VINES	0x02
#define SSE_ROUTING_APOLLO	0x04
#define SSE_ROUTING_NOVELL	0x08
#define SSE_ROUTING_IP		0x10
#define SSE_ROUTING_DECNET	0x20
#define SSE_ROUTING_XNS		0x40
#define SSE_ROUTING_CLNS	0x80
#define SSE_ROUTING_ALL		0xFF

/*
 * SSE data structures
 * 
 * We refer to tree memory simply as "program memory".  This is solely so we
 * don't give too many folks ideas on how the SSE is programmed.
 *
 * Program memory - On board tree memory.
 * Rewrite memory - On board rewrite memory.
 * Protocol internal tree - Intermediate tree generated before code
 * 	generation. 
 * Protocol shadow program memory - The protocol specific copy of shadow
 * 	program memory.
 * Protocol shadow rewrite memory - The protocol specific copy of shadow
 *	rewrite memory.
 *
 */

#define SSE_MAXINDEX MAX_SSE_PROTOCOL+1
#define SSE_MAXEXTRA 10			/* Number of extra addrs per match */

/*
 * Number of extra instruction addresses to reserve for match nodes.
 */

#define SSE_IP_EXTRA_INST 3
#define SSE_IPX_EXTRA_INST 5
#define SSE_SRB_EXTRA_INST 3
#define SSE_CLNS_EXTRA_INST 4

/*
 * Define low memory thresholds.
 */

#define SSE_LOW_TREEMEM_THRESH 1024
#define SSE_LOW_RWMEM_THRESH 8192

/*
 * SSE protocol internal nodes 
 */

typedef ushort treeaddrtype;
typedef int    physaddrtype;
typedef uchar  treecmptype;
typedef uchar  treeinsttype;

/*
 * Shadow memory
 */

typedef struct shadowcelltype_ {
    treeaddrtype  ptr;
    treecmptype value;
    treeinsttype inst;
} shadowcelltype;

typedef struct shadowwordtype_ {
    shadowcelltype lt, eq, gt;
} shadowwordtype;

/*
 * Intermediate tree nodes
 */

typedef enum {
    SPIN_COMPARE,
    SPIN_MATCH,
    SPIN_RANDOM,
    SPIN_COMPARE_FINAL_TERM,
    SPIN_COMPARE_NXT_TERM
} spin_type;

/*
 * Shadow rewrite memory
 */

typedef struct rwdbtype_ {
    struct rwdbtype_ *next;		/* Next entry */
    int physaddr;			/* Address is rewrite memory */
    int start;				/* Starting address */
    uchar proto;			/* Protocol for this entry */
    uchar len;				/* Length of encaps, in bytes */
    uchar encaps[0];			/* Placeholder for encapsulation */
} rwdbtype;

typedef struct matchtype_ {
    struct matchtype_ *next;		/* next entry */
    struct spintype_  *node;		/* point to match node */
} matchtype;

typedef struct spin_cell_ {
    struct spintype_ *addr;		/* next cell */
    treeinsttype inst;			/* SSE instruction */
    boolean grant;
} spin_cell;

struct spin_word {
    spin_cell lt, eq, gt;		/* cell info for <=> */
};

struct spin_match {
    uint usage;				/* usage so far */
    rwdbtype *outrw;			/* output rw entry */
    physaddrtype extra[SSE_MAXEXTRA];	/* other addresses for match */
    int protoinfo2;			/* more protocol specific info */
    uchar extracount;			/* number of other addrs */
    uchar remainder;			/* remaining bytes to read */
};

typedef struct spintype_ {
    struct spintype_ *walk;		/* backpointer for tree walks */
					/* also next pointer for list ops */
    uint time;				/* time this was visited for walks */
    physaddrtype physaddr;		/* physical address */
    union nodeinfotype {
	struct spin_word compare;	/* comparison info */
	struct spin_match match;	/* match info */
    } nodeinfo;
    int protoinfo;		 	/* protocol specific info */
    ushort depth;			/* depth of tree */
    ushort parents;			/* number of parents */
    spin_type type;				/* type of this node */
    treecmptype value;			/* value for comparison */
    uchar protocol;			/* protocol using this node */
} spintype;

#define LTADDR 		nodeinfo.compare.lt.addr
#define EQADDR 		nodeinfo.compare.eq.addr
#define GTADDR 		nodeinfo.compare.gt.addr
#define LTGRANT		nodeinfo.compare.lt.grant
#define EQGRANT		nodeinfo.compare.eq.grant
#define GTGRANT		nodeinfo.compare.gt.grant
#define LTINST		nodeinfo.compare.lt.inst
#define EQINST		nodeinfo.compare.eq.inst
#define GTINST		nodeinfo.compare.gt.inst
#define OUTRW  		nodeinfo.match.outrw
#define EXTRA  		nodeinfo.match.extra
#define EXTRACOUNT	nodeinfo.match.extracount
#define REMAINDER	nodeinfo.match.remainder

/*
 * SSE statistics.
 */
typedef struct ssestattype_ {
    uint words_used;
    uint rwbytes_used;
    uint spin_used;
} ssestattype;

/*
 * Description of an SSE
 */

struct ssetype_ {
    /*
     * General overhead
     */
    mcitype *cbus;			/* Pointer back to our cbus */
    ulong state;			/* State of this SSE */
    spintype *inputif;			/* ICB tree for this SSE */
    rwdbtype *rewrite_table[NETHASHLEN]; /* Rewrite database */
    matchtype *match_cache[NETHASHLEN]; /* Match node cache */
    rangelist *freetreememory;		/* Memory management */
    rangelist *freerwmemory;		/* Memory management */
    boolean real;			/* TRUE if it's a real SSE */
    boolean ucode_disabled;		/* TRUE if the microcode is off */
    ssestattype overheadstat;		/* Overhead counters */
    uint rwbytes_avail;			/* Total phys rewrite memory */
    uint rwbytes_used;			/* Rewrite memory used */
    uint words_avail;			/* Total tree memory */
    uint words_used;			/* Tree memory used */
    uint hangcount;			/* Number of hangs */
    uint outofmem_count;		/* Out of memory Conditions */

    /*
     * Per routed protocol
     */
    spintype *protocol_tree[SSE_MAXINDEX]; /* Protocol trees */
    uint depth[SSE_MAXINDEX];		/* Depth of tree */
    ssestattype stats[SSE_MAXINDEX];	/* Usage stats */
    uint rebuilds[SSE_MAXINDEX];	/* Number of rebuilds */

    /*
     * Protocol specific information.  We normally pass a pointer to this
     * around. 
     */
    sse_protoinfo protoinfo;
};

#define SSE_STAT_STRING "     %-8d       %-8d"
#define SSE_INTERN_STRING "        %-8d"
#define SSE_DEPTH_STRING "  %-5d  %-8d" 

/*
 * Shadow memory, as we keep it in list form.
 */
typedef struct shadowlistmemtype_ {
    struct shadowlistmemtype_ *next;
    physaddrtype physaddr;
    shadowcelltype lt, eq, gt;
} shadowlistmemtype;

/*
 * State for sse->state.  Note that this is a bitvector, NOT a state
 * machine.  This is probably NOT the minimum set of bits necessary.
 */

#define SSE_STATE_HWRESET	 0x01	/* set if the hardware is in an */
					/* unknown state */
#define SSE_STATE_SWRESET	 0x02   /* set if the software is in an */
					/* unknown state */
#define SSE_STATE_INITIALIZED	 0x08	/* set if data structure initialized */
#define SSE_STATE_UCODE_ON	 0x10	/* set if ucode on */
#define SSE_STATE_UCODE_DISABLED 0x20	/* set if ucode disabled by hand or */
					/* error */

#define SSE_STATE_ATTENTION	(SSE_STATE_HWRESET| \
				 SSE_STATE_SWRESET)

/*
 * Other stuff
 */

typedef enum {
    OPCODE_WALK,
    OPCODE_ABORT,
    OPCODE_PARENT,
    OPCODE_CALL
} sse_tree_walk_opcodes;

/*
 * Codes for FCI_SSE_CONTROL
 */
#define SSE_OFF 0			/* unimplemented in ucode so far */
#define SSE_ON	1

/*
 * Magic memory locations
 */

#define SSE_INITREG 	((uchar *)0x11b00000)
#define SSE_REWRITE	((uchar *)0x11c00000)
#define SSE_REWRITE_END ((uchar *)0x11c3ffff)
#define SSE_TREE	((uchar *)0x11200000)
#define SSE_TREE_END	((uchar *)0x112fffff)
#define SSE_STEPSIZE	0x10000

/*
 * Inline code
 */

/*
 * sse_init_sw
 * Initialize a shadow word.
 */

static inline void sse_init_sw (shadowwordtype *sw, treecmptype value,
			 treeinsttype inst, treeaddrtype ptr) 
{
    sw->lt.value = sw->eq.value = sw->gt.value = value;
    sw->lt.inst = sw->eq.inst = sw->gt.inst = inst;
    sw->lt.ptr = sw->eq.ptr = sw->gt.ptr = ptr;
}

/*
 * sse_init_ucond_spin
 * Set up a spin node for unconditional execution of an instruction.
 * Also initialize TREE.DATA.
 */

static inline void sse_init_ucond_spin (spintype *spin, treecmptype value,
					 treeinsttype inst)
{
    spin->value = value;
    spin->nodeinfo.compare.lt.inst = spin->nodeinfo.compare.eq.inst =
	spin->nodeinfo.compare.gt.inst = inst;
}

/*
 * sse_spinref_plus1
 * Add one additional link to a spin node which is already linked into
 * tree.
 */

static inline void sse_spinref_plus1 (spintype *spin)
{
    spin->parents++;
    mem_lock(spin);
}

/*
 * sse_spinref_plus2
 * Add two additional links to a spin node which is already linked into
 * tree.
 */

static inline void sse_spinref_plus2 (spintype *spin)
{
    spin->parents += 2;
    mem_lock(spin);
    mem_lock(spin);
}

/*
 * sse_spinref_minus1
 * Remove a link to a spin node whose links have been over counted.
 */

static inline void sse_spinref_minus1 (spintype *spin)
{
    spin->parents--;
    free(spin);
}

/*
 * sse_init_spin_links
 * Link all spin node branches together, for unconditional execution.
 */

static inline void sse_init_spin_links (spintype *old, spintype *new)
{
    old->LTADDR = old->GTADDR = old->EQADDR = new;
    sse_spinref_plus2(new);
}

extern boolean sse_manager_running;


/*
 * sse_poke_treemem
 * Write a word to tree memory
 */

static inline void sse_poke_treemem (ssetype *sse, int addr, shadowwordtype
				     *sw) 
{
    shadowwordtype *treeptr;
    
    if ((addr < 0) || (addr > sse->words_avail)) {
	errmsg(&msgsym(SWFAILURE, SSE), "bad program memory write for %#x",
	       addr);
	return;
    }

    if (sse->real) {
	treeptr = (shadowwordtype *)(SSE_TREE + (addr<<4));
	*treeptr = *sw;
    }
}


/*
 * Prototypes
 */
extern spintype *sse_compiletree;
extern const treeaddrtype sse_backpatch_table[];

typedef spintype *(*sse_recalltype)(ssetype *sse, spintype *spin,
				    uint localtime, ssestattype *stat); 
typedef int (*sse_callback)(ssetype *sse, spintype *spin, ssestattype
			    *stat, queuetype *queue);

/*
 * sse.c
 */ 
extern void sse_hw_off(mcitype *cbus);
extern void sse_sw_reset(ssetype *sse, uint treememsize, uint rwmemsize);
extern spintype *sse_malloc_spin(int protocol, ssestattype *stat);
extern treeaddrtype sse_malloc_treeaddr(ssetype *sse, ssestattype *stat);
extern void sse_free_treeaddr(ssetype *sse, ssestattype *stat, int addr);
extern void sse_init_spin(spintype *spin, treecmptype value, treeinsttype
			  lt_inst, treeinsttype eq_inst, treeinsttype
			  gt_inst);  
extern void sse_init_sw(shadowwordtype *sw, treecmptype value, treeinsttype
			inst, treeaddrtype ptr); 
extern void sse_peek_treemem(ssetype *sse, int addr, shadowwordtype *sw);
extern ushort sse_peek_rwmem(ssetype *sse, int addr);
extern void sse_poke_rwmem(ssetype *sse, char *source, int addr, int len);
extern int sse_tree_walk(ssetype *sse, spintype *spin, sse_callback func,
			 sse_recalltype recall, ssestattype *stat,
			 queuetype *queue);
extern int sse_safe_tree_walk(ssetype *sse, spintype *spin, sse_callback
			      func, sse_recalltype recall, ssestattype
			      *stat, queuetype *queue);
extern spintype *sse_free_recall(ssetype *sse, spintype *spin, uint
				 localtime, ssestattype *stat); 
extern void sse_slow_free_tree(ssetype *sse, spintype *spin, ssestattype
			       *stat);
extern void sse_free_tree(ssetype *sse, spintype *spin, ssestattype *stat);
extern int sse_gc_tree(ssetype *sse, spintype *spin, ssestattype *stat,
		       queuetype *queue);
extern int sse_address_node(ssetype *sse, spintype *spin, ssestattype
			    *stat, queuetype *queue);
extern int sse_tree_depth(ssetype *sse, spintype *spin, ssestattype *stat,
			  queuetype *queue);
extern void clear_sse(parseinfo *csb);
extern char sse_node_type(spin_type type);
extern int sse_show_spin(ssetype *sse, spintype *spin, ssestattype *stat,
			 queuetype *queue);
extern void show_sse_internals(spintype *spin, int base, sse_callback func);
extern void show_sse(parseinfo *csb);
extern char *sse_protocol_name(int protocol);
extern void copy_sse_command(parseinfo *csb);
extern void test_sse(parseinfo *csb);
extern void sse_finish_reset(mcitype *cbus, uint treesz, uint rwmemsz,
			     boolean real);

/*
 * sse_spp.c
 */ 
extern void show_sse_memory(ssetype *sse, int addr);
extern void show_sse_rewrite(ssetype *sse, int addr);

/*
 * sse_debug.c
 */ 
extern void sse_debug_init(void);
