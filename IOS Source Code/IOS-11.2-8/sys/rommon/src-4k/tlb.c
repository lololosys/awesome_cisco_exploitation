/* $Id: tlb.c,v 3.2.58.1 1996/03/21 23:30:15 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k/tlb.c,v $
 *------------------------------------------------------------------
 * tlb.c
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * TLB routines for R4000/Orion CPU.
 *------------------------------------------------------------------
 * $Log: tlb.c,v $
 * Revision 3.2.58.1  1996/03/21  23:30:15  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:49  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:45:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "signal_4k.h"
#include "cpu_mips.h"
#include "mon_defs.h"
#include "queryflags.h"
#include "error.h"

/*
 * Externals
 */
extern int optind;
extern unsigned long berr_addr;
extern unsigned short cpu_flags;

/*
 * Tables
 */
char *cache_alg[] = {
    "c, nc, wt, no wa",
    "c, nc, wt, wa",
    "uncached",
    "c, nc, wb",
    "coherent excl.",
    "coherent excl. on write",
    "coherent update on write",
    "reserved"
    };


/*
 * Locals and forward declarations
 */
static unsigned int asid;
static SIG_PF savfcn;
void init_tlb(void);

/*
 * Error strings for TLB test
 */
static char *eaddr_ld = "incorrect error address for TLB load exception";
static char *eaddr_sw = "incorrect error address for TLB save exception";
static char *ld_err   = "received unexpected TLB exception on load";
static char *sw_err   = "received unexpected TLB exception on save";

/*
 * tlb_cleanup
 */
static int 
tlb_cleanup(void)
{
    asid = 0;
    setasid(0);
    signal(SIGINT, savfcn);        /* Restore handler       */
    return((int)SIG_DFL);          /* Do default action too */
}

/*
 * page_shift : 
 * Return shift needed for pfn with mask. Return -1 on illegal mask
 */
static int
page_shift(int mask)
{
    switch (mask) {
    case 0x000: return(0); break;
    case 0x003: return(2); break;
    case 0x00F: return(4); break;
    case 0x03F: return(6); break;
    case 0x0FF: return(8); break;
    case 0x3FF: return(10); break;
    case 0xFFF: return(12); break;
    default:    return(-1); break;
    }
}

/*
 * print_tlb_hdr
 */
static void
print_tlb_hdr(void)
{
    printf("Abbreviation: c: cacheable, nc: non-coherent, wt: write through\n");
    printf("              wa: write allocate, wb: write back\n");
    printf("\nIndex\tVPN2\t  ASID\t    D V G\tMask\tSize\n");
    printf("\tPFN(E)\t  PFN(O)\t\tCache\n");
}

/*
 * print_tlb
 */
static void
print_tlb(int index, struct tlb_t *tlb)
{
    printf("%d\t0x%0.6x  0x%0.2x\t    %d %d %d\t0x%0.3x\t(%d Kbytes)\n",
	   index, tlb->hi.vpn2, tlb->hi.asid, tlb->lo0.d, tlb->lo0.v,
	   tlb->lo0.g, tlb->pmask.mask, (0x4 << page_shift(tlb->pmask.mask)));
    printf("\t0x%0.6x  0x%0.6x\t\t0x%0.2x\t%s\n", tlb->lo0.pfn, tlb->lo1.pfn,
	   tlb->lo0.c, cache_alg[tlb->lo0.c]);
}

/*
 * tlbdump : Dump the TLB, default to complete TLB
 * tlbdump [start] [length]
 */
int
tlbdump(int argc, char *argv[])
{
    unsigned count, start;
    struct tlb_t tlb;

    /*
     * Process command line arguments
     */
    optind = 1;    
    if ( argc == 1 ) {
	count = TLB_ENTRIES; 
	start = 0;
    } else if ( getnum(argv[optind++], 10, (long *)&(NVRAM)->start) == 0 ||
        (argc == 3 && getnum(argv[optind++], 10, (long *)&(NVRAM)->length) 
	 == 0) ||
	(argc > 3) ) {
	printf("usage : %s [start] [length]\n", argv[0]);
	return(1);
    } else {
	start = (NVRAM)->start;
	count = (argc == 2) ? (TLB_ENTRIES) : (NVRAM)->length + start;
    }

    /*
     * Loop through the TLB and print out
     */
    savfcn = signal(SIGINT, (SIG_PF)tlb_cleanup);
    print_tlb_hdr();
    while ( start < TLB_ENTRIES && start < count) {
	if ( gettlb(start, &tlb) ) {
	    printf("%s: gettlb failed\n", argv[0]);
	    setasid(asid);
	    signal(SIGINT, savfcn);
	    return(1);
	}
	print_tlb(start, &tlb);
	start += 1;
    }

    /*
     * Return success
     */
    setasid(asid);
    signal(SIGINT, savfcn);
    return(0);
}

/*
 * tlbpid : Display / Set the process identifier
 * tlbpid [new pid]
 */
int
tlbpid(int argc, char *argv[])
{
    unsigned long temp;

    optind = 1;
    if ( argc > 1 ) {
	if ( getnum(argv[optind++], 16, &temp) == 0 ||
	     argc > 2 || temp > TLB_MAX_ASID ) {
	    printf("usage : %s [asid]\n", argv[0]);
	    return(1);
	}
	savfcn = signal(SIGINT, (SIG_PF)tlb_cleanup);
	setasid(temp);
	asid = getasid();
	if ( asid != temp ) {
	    printf("Warning : set asid as 0x%x, but returned as 0x%x\n",
		   temp, asid);
	}
    } else {
	savfcn = signal(SIGINT, (SIG_PF)tlb_cleanup);
	temp = getasid();
	if ( asid != temp ) {
	    printf("Warning : getasid() returned 0x%x, expected 0x%x\n",
		   temp, asid);
	}
	printf("Current ASID = 0x%x\n", temp);
    }
    signal(SIGINT, savfcn);
    return(0);
}


/*
 * tlbmap : Establish a virtual / physical address mapping
 * tlbmap [-dgvucexw] Vaddress Paddress PageMask [index]
 */
int
tlbmap(int argc, char *argv[])
{
    struct tlb_t tlb;
    struct entryhi_t hireg;
    int i, tmp, shift;
    unsigned long vaddr, paddr, pfn, mask = 0, index = 8;
    static char optstr[] = "dgvnaucexw";

    savfcn = signal(SIGINT, (SIG_PF)tlb_cleanup);
    filbyte((char *)&tlb, sizeof(struct tlb_t), 0);
    tlb.lo0.c = tlb.lo1.c = TLB_UNCACHED;
    while ( (i=getopt(argc, argv, optstr)) >= 0) {
	switch (i) {
	case 'd': tlb.lo0.d = tlb.lo1.d = 1;
	    break;
	case 'g': tlb.lo0.g = tlb.lo1.g = 1;
	    break;
	case 'v': tlb.lo0.v = tlb.lo1.v = 1;
	    break;
	case 'n': tlb.lo0.c  = tlb.lo1.c = TLB_WT_NO_WRITE_ALLOCATE;
	    break;
	case 'a': tlb.lo0.c  = tlb.lo1.c = TLB_WT_WRITE_ALLOCATE;
	    break;
	case 'u' : tlb.lo0.c = tlb.lo1.c = TLB_UNCACHED;
	    break;
	case 'c' : tlb.lo0.c = tlb.lo1.c = TLB_CACHE_NONCOHERENT;
	    break;
	case 'e' : tlb.lo0.c = tlb.lo1.c = TLB_CACHE_COH_EXCL;
	    break;
	case 'x' : tlb.lo0.c = tlb.lo1.c = TLB_CACHE_EXCL_WRITE;
	    break;
	case 'w' : tlb.lo0.c = tlb.lo1.c = TLB_CACHE_UPDATE_WRITE;

	default  : goto usage;
	    break;
	}
    }
    if ( argc < 4 ||
	 (argc > optind && getnum(argv[optind++], 16, &vaddr) == 0) ||
	 (argc > optind && getnum(argv[optind++], 16, &paddr) == 0) ||
	 (argc > optind && getnum(argv[optind++], 16, &mask) == 0) ||
	 page_shift(mask) == -1 ||
	 (argc > optind && getnum(argv[optind++], 10, &index) == 0) ||
	 argc != optind || index > TLB_ENTRIES ) {
usage:
	printf("usage : %s [-%s] vaddr paddr pagemask [index]\n", 
	       argv[0], optstr);
	printf("pagemask : [0x0,0x3,0xF,0x3F,0xFF,0x3FF,0xFFF]\n");
	printf("index    : [0..%d]\n", TLB_ENTRIES-1);
	printf("-d : Set dirty\n");
	printf("-v : Set valid\n");
	printf("-g : Set global\n");
	printf("-n : Set cached, write through, no write allocate\n");
	printf("-a : Set cached, write through, write allocate\n");
	printf("-u : Set uncached\n");
	printf("-c : Set cached, noncoherent, write back\n");
	printf("-e : Set cached, coherent exclusive\n");
	printf("-x : Set cached, coherent exclusive on write\n");
	printf("-w : Set cached, update on write\n");
	signal(SIGINT, savfcn);
	return(1);
    }
    hireg.dummy = 0;
    hireg.vpn2 = (vaddr >> TLB_HI_VPN2SHIFT);
    hireg.asid = tlb.hi.asid = getasid();
    if ( tmp = tlbprobe(&hireg), tmp >= 0 && tmp != index) {
	printf("%s: address 0x%x is mapped by entry %d\n",
	       argv[0], vaddr, tmp);
	setasid(asid);
	signal(SIGINT, savfcn);
	return(1);
    }
    tlb.pmask.mask = mask;
    tlb.hi.vpn2 = (vaddr >> TLB_HI_VPN2SHIFT);
    shift = page_shift(mask);
    pfn = paddr >> (TLB_HI_VPN2SHIFT-1+shift);
    tlb.lo0.pfn = pfn << shift;
    tlb.lo1.pfn = (pfn+1) << shift;
    settlb(index, &tlb);
    setasid(asid);
    signal(SIGINT, savfcn);
    return(0);
}


/*
 * tlbflush : Flush the TLB
 */
int 
tlbflush(int argc, char *argv[])
{
    unsigned count, start;
    struct tlb_t tlb;

    /*
     * Process command line arguments
     */
    optind = 1;    
    if ( argc == 1 ) {
	count = TLB_ENTRIES; 
	start = 0;
    } else if ( getnum(argv[optind++], 10, &(NVRAM)->start) == 0 ||
        (argc == 3 && getnum(argv[optind++], 10, &(NVRAM)->length) == 0) ||
	(argc > 3) ) {
	printf("usage : %s [start] [length]\n", argv[0]);
	return(1);
    } else {
	start = (NVRAM)->start;
	count = (argc == 2) ? (TLB_ENTRIES) : (NVRAM)->length + start;
    }

    /*
     * Loop through the TLB and invalidate
     * Set the Valid and Global flags to Off
     */
    savfcn = signal(SIGINT, (SIG_PF)tlb_cleanup);
    while ( start < TLB_ENTRIES && start < count) {
	gettlb(start, &tlb);
	tlb.lo0.v = tlb.lo1.v = 0;
	tlb.lo0.g = tlb.lo1.g = 0;
	settlb(start, &tlb);
	start += 1;
    }
    setasid(asid);
    signal(SIGINT, savfcn);
    return(0);
}

/*
 * tlbvir : Search for a mapping for the virtual address
 * tlbvir vaddr
 */
int 
tlbvir(int argc, char *argv[])
{
    struct tlb_t tlb;
    struct entryhi_t hireg;
    unsigned long vaddr;
    int tmp;

    optind = 1;
    if ( argc < 2 || getnum(argv[optind++], 16, &vaddr) == 0 ||
	 argc != optind ) {
	printf("usage : %s vaddr\n", argv[0]);
	return(1);
    }
    savfcn = signal(SIGINT, (SIG_PF)tlb_cleanup);
    hireg.vpn2 = (vaddr >> TLB_HI_VPN2SHIFT);
    hireg.asid = getasid();
    if ( tmp = tlbprobe(&hireg), tmp >= 0 ) {
	gettlb(tmp, &tlb);
	print_tlb_hdr();
	print_tlb(tmp, &tlb);
    } else {
	printf("%s : no mapping found for vaddr 0x%x\n", argv[0], vaddr);
    }
    setasid(asid);
    signal(SIGINT, savfcn);
    return(0);
}

/*
 * tlbphy : Search TLB for virtual addresses mapping to physical address
 * tlbphy addr
 */
int
tlbphy(int argc, char *argv[])
{
    struct tlb_t tlb;
    unsigned long addr, paddr;
    int found = -1, index;

    optind = 1;
    if ( argc < 2 || getnum(argv[optind++], 16, &paddr) == 0 ||
	 argc != optind ) {
	printf("usage : %s paddr\n", argv[0]);
	return(1);
    }
    savfcn = signal(SIGINT, (SIG_PF)tlb_cleanup);
    addr = (paddr >> (TLB_HI_VPN2SHIFT-1));
    for (index = 0; index < TLB_ENTRIES; index++) {
	gettlb(index, &tlb);
	if ( tlb.lo0.pfn == addr || tlb.lo1.pfn == addr ) {
	    if ( found < 0 )
		print_tlb_hdr();
	    print_tlb(index, &tlb);
	    found += 1;
	}
    }
    if ( found < 0 ) 
	printf("%s : no mapping found for paddr 0x%x\n", argv[0], paddr);
    setasid(asid);
    signal(SIGINT, savfcn);
    return(0);
}

/*
 * init_tlb : Initialize the TLB. All entries to invalid, local, 
 * uncached, 4Kb page with unique ASID
 */
void
init_tlb(void)
{
    struct tlb_t tlb;
    int index;

    filbyte( (char *)&tlb, sizeof(struct tlb_t), 0);
    tlb.lo0.c = tlb.lo1.c = TLB_UNCACHED;
    setasid(0);
    for (index = 0; index < TLB_ENTRIES; index++ ) {
	tlb.hi.asid = index + 1;
	settlb(index, &tlb);
    }
    asid = 0;
    setasid(asid);
}

/*
 * get_tlbflags : Needed due to optimizer
 */
int
get_tlbflags(void)
{
    return(cpu_flags);
}

/*
 * tlb_rd
 */
int 
tlb_rd(unsigned char *addr, unsigned long *eaddr, unsigned long *datap)
{     
    cpu_flags = 0;
    *datap = *(unsigned long *)(addr);
    BUSERR_NOP_X4();
    *eaddr = berr_addr;
    return( get_tlbflags() );
}

/*
 * tlb_wr
 */
int 
tlb_wr(unsigned char *addr, unsigned long *eaddr, unsigned long data)
{     
    cpu_flags = 0;
    *(unsigned long *)(addr) = data;
    BUSERR_NOP_X4();
    *eaddr = berr_addr;
    return( get_tlbflags() );
}

/*
 * dotlb_test : Perform test on selected TLB
 */
static char *
dotlb_test(int i, unsigned char *paddr, unsigned char *taddr, int page)
{
    struct tlb_t tlb, tlb_tmp;
    int addr, psize, shift, rw;
    unsigned int pfn;
    unsigned long eaddr, data;

    setasid(asid);
    psize = 0x1000 << page_shift(page);

    /*
     * Check that the physical address does not produce bus errors
     * on read and writes
     */
    rw = 0;         /* read/write */

    if ( chkberr( PHY_TO_KSEG1(paddr), BW_32BITS, rw) ) {
	return("read/write to page start caused bus errors");
    } else if ( chkberr( PHY_TO_KSEG1(paddr+(psize<<1)-sizeof(int)),
			     BW_32BITS, rw) ) {
	return("read/write to page end caused bus errors");
    }

    /*
     * all entries invalid
     */
    if ( tlb_rd(taddr, &eaddr, &data) != CAUSE_TLB_LOAD ) {
	return("did not receive TLB exception on load");
    } else if ( eaddr != (int)taddr ) {
	return(eaddr_ld);
    }
    if ( tlb_wr(taddr, &eaddr, 0) != CAUSE_TLB_SAVE ) {
	return("did not receive TLB exception on save");
    } else if ( eaddr != (int)taddr ) {
	return(eaddr_sw);
    }

    gettlb(i, &tlb);
    movbyte((char *)&tlb, (char *)&tlb_tmp, sizeof(struct tlb_t));
    
    /*
     * valid, dirty, global, incorrect ASID
     */
    tlb.lo0.v = tlb.lo1.v = 1;
    tlb.lo0.g = tlb.lo1.g = 1;
    tlb.lo0.d = tlb.lo1.d = 1;
    shift = page_shift(page);
    pfn = (unsigned int)paddr >> (TLB_HI_VPN2SHIFT-1+shift);
    tlb.lo0.pfn = pfn << shift;
    tlb.lo1.pfn = (pfn+1) << shift;
    tlb.pmask.mask = page;
    tlb.hi.asid = i+1;
    settlb(i, &tlb);
    setasid(asid);
    if ( tlb_rd(taddr, &eaddr, &data)) {
	return(ld_err);
    }
    if ( tlb_wr(taddr, &eaddr, 0)) {
	return(sw_err);
    }
    
    /*
     * valid, clean, global, incorrect ASID
     */
    tlb.lo0.d = tlb.lo1.d = 0;
    settlb(i, &tlb);
    setasid(asid);
    if ( tlb_wr(taddr, &eaddr, 0) != CAUSE_TLB_MOD ) {
	return("did not receive TLB exception on save to clean page");
    } else if ( eaddr != (int)taddr ) {
	return(eaddr_sw);
    }
    
    /*
     * valid, dirty, local, incorrect ASID
     */
    tlb.lo0.g = tlb.lo1.g = 0;
    tlb.lo0.d = tlb.lo1.d = 1;
    settlb(i, &tlb);
    setasid(asid);
    if ( tlb_rd(taddr, &eaddr, &data) != CAUSE_TLB_LOAD ) {
	return("did not receive TLB exception on load with bad ASID");
    } else if ( eaddr != (int)taddr ) {
	return(eaddr_ld);
    }
    if ( tlb_wr(taddr, &eaddr, 0) != CAUSE_TLB_SAVE ) {
	return("did not receive TLB exception on save with bad ASID");
    } else if ( eaddr != (int)taddr ) {
	return(eaddr_sw);
    }
    
    /*
     * valid, dirty, local, correct ASID. Do it at the start/end of
     * even/odd page and at one location past page end
     */
    setasid(i+1);
    /* first location of even page */
    if ( tlb_rd(taddr, &eaddr, &data)) {
	return(ld_err);
    }
    if ( tlb_wr(taddr, &eaddr, 0)) {
	return(sw_err);
    }
    /* last location of even page */
    if ( tlb_rd(taddr+(psize-4), &eaddr, &data)) {
	return(ld_err);
    }
    if ( tlb_wr(taddr+(psize-sizeof(int)), &eaddr, 0)) {
	return(sw_err);
    }
    /* first location of odd page */
    if ( tlb_rd(taddr+psize, &eaddr, &data)) {
	return(ld_err);
    }
    if ( tlb_wr(taddr+psize, &eaddr, 0)) {
	return(sw_err);
    }
    /* last location of odd page */
    if ( tlb_rd(taddr+((psize<<1)-sizeof(int)), &eaddr, &data)) {
	return(ld_err);
    }
    if ( tlb_wr(taddr+((psize<<1)-sizeof(int)), &eaddr, 0)) {
	return(sw_err);
    }
    /* first location past odd page */
    if ( tlb_rd(taddr+(psize<<1), &eaddr, &data) != CAUSE_TLB_LOAD ) {
	return("did not receive TLB exception on load past odd page end");
    } else if ( eaddr != (int)taddr+(psize<<1) ) {
	return(eaddr_ld);
    }
    if ( tlb_wr(taddr+(psize<<1), &eaddr, 0) != CAUSE_TLB_SAVE ) {
	return("did not receive TLB exception on save past odd page end");
    } else if ( eaddr != (int)taddr+(psize<<1) ) {
	return(eaddr_sw);
    }
    
    for (addr = 0; addr < (psize<<1); addr+=4)
	*(unsigned int *)(PHY_TO_KSEG1(paddr+addr)) = addr;
    for (addr = 0; addr < (psize<<1); addr+=4) {
	if ( tlb_rd(taddr+addr, &eaddr, &data) ) {
	    return(ld_err);
	}
	if ( data != addr ) {
	    return("write via KSeg1, read thru TLB produced errors");
	}
    }
    
    /*
     * Re-initialize for next tlb entry
     */
    setasid(asid);
    settlb(i, &tlb_tmp);
    return( (char *)0 );
}

/*
 * test_tlb : Test the TLB for Orion/R4000
 * Tests each TLB entry, using page sizes of 4K and 16K, uncached.
 * Checks that for each setting that approptiate exception is
 * taken/not taken and the appropriate writes get completed/aborted
 * Each of the cases are :
 *    - all entries invalid
 *    - entry is valid, dirty, global, incorrect ASID
 *    - entry is valid, clean, global, incorrect ASID
 *    - entry is valid, dirty, local, incorrect ASID
 *    - entry is valid, dirty, local, correct ASID
 */
int
test_tlb(unsigned char *paddr, int indiag)
{
    int i;
    char *err;
    SIG_PF busfcn;

    testname("tlb translation");
    /*
     * Check parameters
     */
    if ( (int)paddr & 0x3FFF ) {
	cterr('f',0,"Test location must be on a 16K page boundary...aborting");
	return(1);
    }
    if ( indiag ) 
	prpass(testpass, (char *)0);

    
    /*
     * Initialize TLB, all invalid, asid to 0x00, and init signals
     */
    savfcn = signal(SIGINT, (SIG_PF)tlb_cleanup);
    busfcn = signal(SIGBUS, SIG_IGN);
    init_tlb();

    /*
     * Loop through each TLB entry and test
     */
    for (i=0; i<TLB_ENTRIES; i++) {

	if ( !indiag )
	    printf("testing entry %d\r", i);
	if ( err = dotlb_test(i, paddr, 0, TLB_PAGESIZE_4K), err ) {
	    cterr('f',0,err);
	    init_tlb();
	    signal(SIGINT, savfcn);
	    signal(SIGBUS, busfcn);
	    return(1);
	}
	if ( err = dotlb_test(i, paddr, 0, TLB_PAGESIZE_16K), err ) {
	    cterr('f',0,err);
	    init_tlb();
	    signal(SIGINT, savfcn);
	    signal(SIGBUS, busfcn);
	    return(1);
	}
    }

    /*
     * Successfull completion
     */
    prcomplete(testpass, errcount, 0);
    init_tlb();
    signal(SIGINT, savfcn);
    signal(SIGBUS, busfcn);
    return(0);
}

/*
 * tlbtest : Run TLB tests from the command line
 */
int 
tlbtest(int argc, char *argv[])
{
    int res;

    /*

     * Process the command line arguments
     */
    optind = 1;
    if ( argc == 1 ) {
	query_user( QU_START, &(NVRAM)->start );
    } else if ( getnum(argv[optind++], 16, &(NVRAM)->start) == 0 ||
	        argc != optind ) {
	printf("usage : %s paddr\n", argv[0]);
	return(1);
    }
    res =  test_tlb((unsigned char *)((NVRAM)->start), 0);
    if ( !res )
	printf("completed without errors\n");
    return(res);
}

/*
 * tlb_diag : Run the TLB test from the menus
 */
int
tlb_diag(void)
{
    /*
     * Run the TLB test on NVRAM, 64k from the base
     */
    return( test_tlb((unsigned char *)(PHY_ADRSPC_NVRAM + 0x10000), 1) );
}

static char *tlb_scan_msg = "tlb error from location";

/*
 * tlbscan : Scan a range for TLB exceptions, integer by integer
 * tlbscan [addr] [length]
 */
int
tlbscan(int argc, char *argv[])
{
    char prev_acc_stat = -1;
    volatile unsigned char *addr, *end;
    unsigned long temp, eaddr;
    char buffer[120];
    char *bptr;
    SIG_PF busfcn;

    optind = 1;
    if(argc == 1) {
	query_user( QU_START | QU_SIZE , &(NVRAM)->start, &(NVRAM)->length);
    } else {
	if(getnum(argv[optind++],16,&(NVRAM)->start) == 0 ||
	   (argc == 3 && getnum(argv[optind++],16,&(NVRAM)->length) == 0) || 
	   optind != argc) {
	    printf("usage: %s [start] [length]\n",argv[0]);
	    return(1);
	}
    }

    addr = (unsigned char *)((NVRAM)->start & ~0x3);
    end = addr + (NVRAM)->length;
    printf("scanning from %#x to %#x with logging\n", addr, end);
    busfcn = signal(SIGBUS,SIG_IGN);
    bptr = buffer;

    for(; addr < end; addr+=4) {
	if ( tlb_rd((unsigned char *)addr, &eaddr, &temp) ) {
	    if(prev_acc_stat != 1) {   /* no fault on previous access */
		if(prev_acc_stat == 0) {  /* not first time through loop */
		    sprintf(bptr, "%#x\n", addr-4);
		    logprintf(buffer);
		    bptr = buffer;
		}
		bptr += sprintf(bptr, "%s %#x to ", tlb_scan_msg, addr);
		prev_acc_stat = 1;  /* set for next access */
	    }
	} else {  /* this access did not cause an error */
	    if(prev_acc_stat != 0) {  /* previous access caused an error */
		if(prev_acc_stat > 0) {  /* not first time through loop */
		    bptr += sprintf(bptr, "%#x\n", addr-4);
		}
		bptr += sprintf(bptr, "no %s %#x to ", tlb_scan_msg, addr);
		prev_acc_stat = 0;  /* set for next access */
	    }
	}
    }
    sprintf(bptr, "%#x\n", addr-1);
    logprintf(buffer);
    signal(SIGBUS,busfcn);
    return(0);
}

/* End of Module */








