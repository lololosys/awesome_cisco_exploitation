/* $Id: cpu_mips.c,v 3.2.58.1 1996/03/21 23:30:01 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k/cpu_mips.c,v $
 *------------------------------------------------------------------
 * cpu_mips.c
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994 - 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Utility routines for R4000/Orion CPU.
 *------------------------------------------------------------------
 * $Log: cpu_mips.c,v $
 * Revision 3.2.58.1  1996/03/21  23:30:01  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/12  19:46:20  mbeesley
 * Branch: ELC_branch
 * Compile out dram_verify() for the Predator platform.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:41  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "cpu_mips.h"
#include "mon_defs.h"
#include "queryflags.h"
#include "signal_4k.h"

/*
 * External variables and functions
 */
extern char *cache_alg[];
extern int optind;
extern char *optarg;
extern int speed_bd(), speed_nobd();
extern int mem_verify();
extern long vec_start, vec_end;

int sys_state;

/*
 * CPU Mode strings
 */
char *mode_strs[] = {
    "Kernel", "Supervisor", "User", "Illegal"
    };

/*
 * Register Names
 */
char *reg_names[] = {
    "zero  ", "AT    ", "v0    ", "v1    ", 
    "a0    ", "a1    ", "a2    ", "a3    ",
    "t0    ", "t1    ", "t2    ", "t3    ",
    "t4    ", "t5    ", "t6    ", "t7    ",
    "s0    ", "s1    ", "s2    ", "s3    ",
    "s4    ", "s5    ", "s6    ", "s7    ",
    "t8    ", "t9    ", "k0    ", "k1    ",
    "gp    ", "sp    ", "s8    ", "ra    "
    };

/*
 * orion_config : display the config register for an Orion/R4000
 */
void
orion_config(struct orion_config *c)
{
    printf("Master Checker     [CM]=%d, %s\n", c->cm, 
	   c->cm?"Enabled":"Disabled");
    printf("Sys Clk Ratio      [EC]=%d, div by %d\n", c->ec, c->ec+1);
    printf("Data Pattern       [EP]=%d\n", c->ep);
    printf("2nd Cache Blk Size [SB]=%d\n", c->sb);
    printf("2nd Cache Mode     [SS]=%d, %s\n", c->ss, 
	   c->ss?"Separated":"Mixed");
    printf("2nd Cache Port     [SW]=%d, %s\n", c->sw, 
	   c->sw?"64 bit":"128 bit");
    printf("Sys Port Width     [EW]=%d, %s\n", c->ew, 
	   c->ew?"32 bit":"64 bit");
    printf("2nd Cache Present  [SC]=%d, %s\n", c->sc, 
	   c->sc?"Absent":"Present");
    printf("Dirty Shared       [SM]=%d, %s\n", c->sm, 
	   c->sm?"Disabled":"Enabled");
    printf("Kernel Endian      [BE]=%d, %s\n", c->be, 
	   c->be?"Big":"Little");
    printf("ECC Mode           [EM]=%d, %s\n", c->em, 
	   c->em?"ECC":"Parity");
    printf("Block Ordering     [EB]=%d, %s\n", c->eb, 
	   c->eb?"Sequential":"Sub-block");
    printf("Inst Cache Size    [IC]=%d, %d bytes\n", c->ic,
	   (1<<(12+c->ic)));
    printf("Data Cache Size    [DC]=%d, %d bytes\n", c->dc,
	   (1<<(12+c->dc)));
    printf("I Cache Blk Size   [IB]=%d, %s\n", c->ib, 
	   c->ib?"32 bytes":"16 bytes");
    printf("D Cache Line Size  [DB]=%d, %s\n", c->db, 
	   c->db?"32 bytes":"16 bytes");
    printf("SC Update Rule     [CU]=%d, %s\n", c->cu, 
	   c->cu?"Cacheable Coherent":"Specified by TLB");
    printf("KSeg0 Coherency    [K0]=%d, %s\n", c->k0, cache_alg[c->k0]);
}

/*
 * orion_sreg : display the status register for an Orion/R4000
 */
void
orion_sreg(struct orion_sreg *s)
{
    int i, temp;

    printf("Coprocessors       [CU]=%d\n", s->cu);
    printf("Reduced Power      [RP]=%d, %s\n", s->rp, 
	   s->rp?"Reduced Clock":"Full Speed");
    printf("FPU Registers      [FR]=%d, %d registers\n", s->fr, s->fr?32:16);
    printf("Reverse Endian     [RE]=%d, %s\n", s->re, 
	   s->re?"Enabled":"Disabled");
    printf("Vector locations   [BV]=%d, %s\n", s->bev, 
	   s->bev?"Bootstrap":"Normal");
    printf("TLB Shutdown       [TS]=%d\n", s->ts);
    printf("Soft Reset         [SR]=%d\n", s->sr);
    printf("Hit / Miss         [CH]=%d\n", s->ch);
    printf("ECC bits used      [CE]=%d\n", s->ce);
    printf("Cache/ECC excepts  [DE]=%d, %s\n", s->de, 
	   s->de?"No":"Yes");
    for (i=7; i>=0; i--) {
	temp = (s->im>>i) & 0x01;
	printf("Interrupt Mask %d  [IP%d]=%d, %s\n", i, i, temp, 
	       temp?"Enabled":"Disabled");
    }
    printf("Kernel Addressing  [KX]=%d, %d bit\n", s->kx, s->kx?64:32);
    printf("Supervisor Addr.   [SX]=%d, %d bit\n", s->sx, s->sx?64:32);
    printf("User Addressing    [UX]=%d, %d bit\n", s->ux, s->ux?64:32);
    printf("Present Mode      [KSU]=%d, %s\n", s->ksu, mode_strs[s->ksu]);
    printf("Error Level       [ERL]=%d, %s\n", s->erl,
	   s->erl?"Error":"Normal");
    printf("Exception Level   [EXL]=%d, %s\n", s->exl,
	   s->exl?"Exception":"Normal");
    printf("Interrupt Mask     [IE]=%d, %s\n", s->ie, 
	   s->ie?"Enabled":"Disabled");
}


/*
 * cpu_config : Print out some usefull information concerning the CPU and
 * allow the user to set options
 * cpu [-bnedhs:ucK:S:U:I:D:]
 *     -b : Set vectors to Bootstrap
 *     -n : Set vectors to Normal
 *     -e : Enable Cache Error Exceptions
 *     -d : Disable Cache Error Exceptions
 *     -h : Show help information
 *     -s : Set Stack pointers to KSeg0 or KSeg1 [0|1]
 *     -u : Set KSeg0 to Un-cached
 *     -c : Set KSeg0 to Cached
 *     -K : Set Kernel Mode to 64/32 bit [32|64]
 *     -S : Set Supervisor Mode to 64/32 bit [32|64]
 *     -U : Set User Mode to 64/32 bit [32|64]
 *     -f : Flush I/D Caches
 *     -q : Enable quite alignment emulation mode
 *     -v : Enable verbose alignment emulation mode
 */
int
cpu_config(int argc, char *argv[])
{
    static char *optstr = "bnedhs:ucK:S:U:fqv";
    unsigned long config = getcp0_config();
    unsigned long sreg = getcp0_sreg();
    unsigned long prid = getcp0_prid();
    long bev = -1, i, tmp;

    if ( argc == 1 ) {
	printf("Config Reg       : 0x%.8x\n", config);
	printf("Status Reg       : 0x%.8x\n", sreg);
	printf("Cause Reg        : 0x%.8x\n", getcp0_cause());
	printf("Count Reg        : 0x%.8x\n", getcp0_count());
	printf("Compare Reg      : 0x%.8x\n", getcp0_compare());
	printf("Stack Pointer    : 0x%.8x\n", getcpu_sp());
	printf("Implem/Rev       : %d/%d.%d\n", (prid>>8) & 0xff, (prid>>4) & 0x0f,
	       (prid & 0x0f));
	printf("\nConfig Register\n");
	printf("---------------\n");
	orion_config( (struct orion_config *)&config);
	printf("\nStatus Register\n");
	printf("---------------\n");
	orion_sreg( (struct orion_sreg *)&sreg );
	return(0);
    }
    while ( (i=getopt(argc, argv, optstr)) >= 0) {
	switch (i) {
	case 'b': 
	    bev = 1; break;
	case 'n': 
	    bev = 0; break;
	case 'e': 
	    sreg &= ~SREG_DE; setcp0_sreg(sreg); break;
	case 'd': 
	    sreg |= SREG_DE; setcp0_sreg(sreg); break;
	case 'u':
	    cache_flush(TLB_UNCACHED, -1);
	    break;
	case 's':
	    if ( getnum(optarg,10,&tmp) == 0 || (tmp != 0 && tmp != 1))
		goto usage;
	    move_stacks(tmp);
	    break;
	case 'c': 
	    cache_flush(TLB_CACHE_NONCOHERENT, -1);
	    break;
	case 'K':
	    if ( getnum(optarg,10,&tmp) == 0 || (tmp != 32 && tmp != 64))
		goto usage;
	    sreg = (tmp == 32 ) ? sreg & ~SREG_KX : sreg | SREG_KX;
	    setcp0_sreg(sreg);
	    break;
	case 'S':
	    if ( getnum(optarg,10,&tmp) == 0 || (tmp != 32 && tmp != 64))
		goto usage;
	    sreg = (tmp == 32 ) ? sreg & ~SREG_SX : sreg | SREG_SX;
	    setcp0_sreg(sreg);
	    break;
	case 'U':
	    if ( getnum(optarg,10,&tmp) == 0 || (tmp != 32 && tmp != 64))
		goto usage;
	    sreg = (tmp == 32 ) ? sreg & ~SREG_UX : sreg | SREG_UX;
	    setcp0_sreg(sreg);
	    break;
	case 'f':
	    cache_flush(0, -1);
	    break;
	case 'q':
	    (NVRAM)->diagflag |= D_QUIETMODE;
	    break;
	case 'v':
	    (NVRAM)->diagflag &= ~D_QUIETMODE;
	    break;
	case 'h':
	default: 
	    goto usage; break;
	}
    }
    if ( argc != optind ) {
usage:
	printf("usage : %s [-%s]\n", argv[0], optstr);
	printf("        -b         - Set vectors to Bootstrap\n");
	printf("        -n         - Set vectors to Normal\n");
	printf("        -e         - Enable Cache Error exceptions\n");
	printf("        -d         - Disable Cache Error exceptions\n");
	printf("        -h         - Show this information\n");
	printf("        -s 0|1     - Set Stack pointers to KSeg0 (0), Kseg1 (1)\n");
	printf("        -u         - Set KSeg0 to Uncached\n");
	printf("        -c         - Set KSeg0 to Cached\n");
	printf("        -v         - Enable verbose mis-alignment emulation\n");
	printf("        -q         - Enable quiet mis-alignment emulation\n");
	printf("        -K 32|64   - Set Kernel address mode\n");
	printf("        -S 32|64   - Set Supervisor address mode\n");
	printf("        -U 32|64   - Set User address mode\n");
	printf("        -f         - Flush the I/D Caches\n");
	return(1);
    }
    if ( bev != -1 )
	move_vectors(bev);
    return(0);
}

static int alarm_cnt;

/*
 * counter_fcn : Increment the counter
 */
static int
counter_fcn(int signal)
{
    alarm_cnt += 1;
    alarm(100);            /* set the alarm again */
    return((int)SIG_IGN);
}

/*
 * cleanup : Clean up if the user breaks out of the speed loop
 */
static int
cleanup(void)
{
    alarm(0);
    return((int)SIG_DFL);
}

/*
 * speed_lp : Timed speed loop
 */
int
speed_lp(int argc, char *argv[])
{
    long passes = 0x1000, i, cnt_bd;
    SIG_PF savalr, savint;
    int (*fcn_bd)(), (*fcn_nobd)();
    unsigned volatile int *addr, *end;
    
    optind = 1;
    if ( argc == 1 ) {
	query_user(QU_START | QU_SIZE | QU_PASSES, &(NVRAM)->start,
		   &(NVRAM)->length, &passes);
    } else if (argc < 4 || 
	       getnum(argv[optind++], 16, &(NVRAM)->start) == 0 ||
	       getnum(argv[optind++], 16, &(NVRAM)->length) == 0 ||
	       getnum(argv[optind++], 16, &passes) == 0 ||
	       (argc != optind) ) {
	printf("usage : %s start length passes\n", argv[0]);
	return(1);
    }

    /*
     * trap signals and initialize
     */
    alarm_cnt = 0;
    addr = (unsigned volatile int *)(NVRAM)->start;
    end = (unsigned volatile int *)((NVRAM)->start + (NVRAM)->length);
    savalr = signal(SIGALRM, (SIG_PF)counter_fcn);
    savint = signal(SIGINT, (SIG_PF)cleanup);
    if ( getcp0_sreg() & SREG_BEV ) {       /* Running with Bootsrap Vectors */
	fcn_bd = speed_bd;
	fcn_nobd = speed_nobd;
    } else {                                /* Running with Normal Vectors   */
	fcn_bd = (int (*)())(PHY_TO_KSEG0(ROM_OFFSET((int)speed_bd)-VECTOR_OFFSET));
	fcn_nobd = (int (*)())(PHY_TO_KSEG0(ROM_OFFSET((int)speed_nobd)-VECTOR_OFFSET));
    }

    /*
     * Measure performance when the read is in the BD slot
     */
    alarm(100);
    for (i=0; i<passes; i++)
	(*fcn_nobd)(addr, end);
    alarm(0);
    cnt_bd = alarm_cnt;
    alarm_cnt = 0;

    /*
     * Measure performance when the read is NOT in the BD slot
     */
    alarm(100);
    for (i=0; i<passes; i++)
	(*fcn_nobd)(addr, end);
    alarm(0);

    /*
     * Restore signals and printout the results
     */
    signal(SIGALRM, savalr);
    signal(SIGINT, savint);
    printf("Integer read in Branch Delay Slot : %d, (approx. %d secs)\n",
	   cnt_bd, cnt_bd / 10);
    printf("Integer read NOT in B. Delay Slot : %d, (approx. %d secs)\n",
	   alarm_cnt, alarm_cnt / 10);
    return(0);
}

/*
 * dram_mem_error:
 * Process an error from mem_verify
 */
void dram_mem_error (unsigned long addr, unsigned long exp, unsigned long got)
{
    /*
     * print message
     */
    printf("DRAM error: addr= 0x%x, expected= 0x%x, got= 0x%x\n",
	   addr, exp, got);
}

#if !defined(RSP) && !defined(RSP2) && !defined(PREDATOR)

/*
 * dram_verify : Verify DRAM operation
 */
int dram_verify (int argc, char *argv[])
{
    int passes = 0, i, mode = 1, shmem = 0;
    SIG_PF savint;
    int (*fcn)(), sreg;
    unsigned volatile int *addr, *end;
    
    while ( (i=getopt(argc, argv, "rwash")) >= 0) {
	switch (i) {
	case 'r': 
	    mode = 1; break;
	case 'w': 
	    mode = 0; break;
	case 'a':
	    mode = 2; break;
	case 's':
	    shmem = 1; break;
	case 'h':
	    mode = 3; break;
	default:
	    goto usage; break;
	}
    }

    if (argc != optind) {
usage:
	printf("usage: %s [-rwahs]\n", argv[0]);
	printf("  -r : write pattern, loop forever read/verify\n");
	printf("  -w : fill with 0, write/read/verify forever\n");
	printf("  -a : write alternate 1's,0's, loop forever read/verify\n");
	printf("  -h : Split tha address range in two\n");
	printf("  -s : execute from shared memory\n");
	return(1);
    }

    query_user(QU_START | QU_SIZE | QU_PASSES, &(NVRAM)->start,
		   &(NVRAM)->length, &passes);
    
    /*
     * trap signals and initialize
     */
    addr = (unsigned volatile int *)(NVRAM)->start;
    end = (unsigned volatile int *)((NVRAM)->start + (NVRAM)->length);
    savint = signal(SIGINT, (SIG_PF)cleanup);

    /*
     * Calculate the function pointer
     */
    if (shmem) {

	/*
	 * Execute from shared memory
	 */
	fcn = (int (*)())(PHY_TO_KSEG0(ROM_OFFSET((int)mem_verify)
				       - VECTOR_OFFSET + 0x08000000));

	/*
	 * Copy the code
	 */
	fillword((unsigned *)KSEG1_ADRSPC_SHAREDMEM, 0x10000, 0);
	movbyte( (char *)&vec_start, (char *)KSEG1_ADRSPC_SHAREDMEM,
		(int)(&vec_end) - (int)(&vec_start) );
	cache_flush(0, -1);

	/*
	 * Check that the address will not clober the code in sharedmem
	 */
	if ((long)addr >= ADRSPC_K0BASE && (long)addr < ADRSPC_K1BASE) {
	    if (((long)addr >= KSEG1_TO_KSEG0(KSEG1_ADRSPC_SHAREDMEM)) &&
		((long)addr < KSEG1_TO_KSEG0(KSEG1_ADRSPC_SHAREDMEM+0x1000))) {
		printf("error: starting address, 0x%x, is too low\n", addr);
		return(1);
	    }
	} else if ((long)addr >= ADRSPC_K1BASE && (long)addr < ADRSPC_KSBASE) {
	    if (((long)addr >= KSEG1_ADRSPC_SHAREDMEM) &&
		((long)addr < (KSEG1_ADRSPC_SHAREDMEM+0x1000))) {
		printf("error: starting address, 0x%x, is too low\n", addr);
		return(1);
	    }
	}

    } else if ( getcp0_sreg() & SREG_BEV ) {
	/*
	 * Bootstrap vectors
	 */
	fcn = mem_verify;
    } else {
	/* 
	 * Running with Normal Vectors
	 */
	fcn = (int (*)())(PHY_TO_KSEG0(ROM_OFFSET((int)mem_verify)
				       - VECTOR_OFFSET));
	/*
	 * Check that the address will not clober low core
	 */
	if ((long)addr >= ADRSPC_K0BASE && (long)addr < ADRSPC_K1BASE) {
	    if ((long)addr < KSEG1_TO_KSEG0(KSEG1_ADRSPC_RAM + 0x1000)) {
		printf("error: starting address, 0x%x, is too low\n", addr);
		return(1);
	    }
	} else if ((long)addr >= ADRSPC_K1BASE && (long)addr < ADRSPC_KSBASE) {
	    if ((long)addr < (KSEG1_ADRSPC_RAM + 0x1000)) {
		printf("error: starting address, 0x%x, is too low\n", addr);
		return(1);
	    }
	}
    }

    /*
     * Zero out the memory to start with, and do the test. Turn
     * off parity errors while we do this just in case the user is
     * using a cachable address
     */
    sreg = getcp0_sreg();
    setcp0_sreg(sreg | SREG_DE);
    fillword((unsigned *)addr, (NVRAM)->length+0x100, 0);
    setcp0_sreg(sreg);
    if (!passes) {
	printf("starting test with infinite passes, use <Break> to stop\n");
    } else {
	printf("starting test, %d passes, use <Break> to stop\n", passes);
    }
    (*fcn)(addr, end, passes, mode);

    /*
     * Restore signals
     */
    signal(SIGINT, savint);
    return(0);
}

#endif /* RSP, RSP2 and Predator */

/* End of Module */
