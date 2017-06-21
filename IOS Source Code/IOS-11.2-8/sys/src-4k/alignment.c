/* $Id: alignment.c,v 3.6.4.6 1996/09/14 00:01:52 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/alignment.c,v $
 *------------------------------------------------------------------
 * alignment.c - the misalignment handler and logging subsystem
 *
 * April 1994, Michael Beesley
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: alignment.c,v $
 * Revision 3.6.4.6  1996/09/14  00:01:52  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.6.4.5  1996/08/28  13:10:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.4.4  1996/07/09  21:15:06  getchell
 * Modify read bus exception handler and DCL interrrupt handler to deal
 * with dbus accesses to non-existent card.
 * CSCdi58555:  bus exception caused by dbus access to non-existant card
 * Branch: California_branch
 *
 * Revision 3.6.4.3  1996/05/10  23:53:59  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.6.4.2  1996/03/21  23:46:22  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.6.4.1  1996/03/18  21:57:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:32:49  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/03/14  19:12:04  mbeesley
 * CSCdi51627:  Alignment handler will not fix up PCI memory addresses
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/12/11  19:16:31  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.6  1996/03/09  01:18:55  hampton
 * Migrate processor specific files to src-xxx directories.  [CSCdi51128]
 *   Step 2: Update all files which reference the moved files to point to
 *   their new locations.
 *
 * Revision 3.5  1996/02/18  19:42:25  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.4  1996/02/08  02:20:39  achandra
 * CSCdi42964:  ALIGN-3-SPURIOUS message rate-limited
 *
 * Revision 3.3  1995/12/07  22:32:39  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  18:40:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:20:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/11  01:28:54  schaefer
 * CSCdi37736: misaligned register access crashes misalignment handler
 * keep misaligned handler from "fixing" accesses to registers or
 * off the end of memory
 *
 * Revision 2.3  1995/07/14  09:36:22  schaefer
 * CSCdi36176: misalign/spurious access traceback broken for leaf functions
 * also, remove unused code
 *
 * Revision 2.2  1995/06/20  07:18:19  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:52:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "logger.h"
#include "../src-4k/msg_alignment.c"	/* Not a typo, see logger.h */
#include "subsys.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "gdb_4k.h"
#include "../os/region.h"
#include "alignment.h"
#include <ciscolib.h>

/*
 * Local variables
 */

boolean alignment_activated;
alignment_t *align;

const align_access_t align_access[] = {
    { 1, TRUE, 0, },		/* "16 bit read, byte aligned" */
    { 1, TRUE, 1, },		/* "32 bit read, byte aligned" */
    { 2, TRUE, 1, },		/* "32 bit read, halfword aligned" */
    { 1, TRUE, 2, },		/* "64 bit read, byte aligned" */
    { 2, TRUE, 2, },		/* "64 bit read, halfword aligned" */
    { 4, TRUE, 2, },		/* "64 bit read, word aligned" */
    { 1, FALSE, 0, },		/* "16 bit write, byte aligned" */
    { 1, FALSE, 1, },		/* "32 bit write, byte aligned" */
    { 2, FALSE, 1, },		/* "32 bit write, halfword aligned" */
    { 1, FALSE, 2, },		/* "64 bit write, byte aligned" */
    { 2, FALSE, 2, },		/* "64 bit write, halfword aligned" */
    { 4, FALSE, 2, },		/* "64 bit write, word aligned" */
};

/*
 * r4k_set_cpu_reg:
 *
 * Set a register in the context (sign extend it if 64 bit and ext set)
 */

void r4k_set_cpu_reg(r4k_context_t *sc, int index, int value,
			    boolean ext)
{
    /*
     * 64bit context. Check extend flag.
     */
    if (ext) {
	sc->regs[index*2] = (value & 0x80000000) ? -1 : 0;
    } else {
	sc->regs[index*2] = 0;
    }

    sc->regs[REG_INDEX(index)] = value;
}

/*
 * r4k_get_cpu_reg:
 *
 * Return a register from the context
 */

int r4k_get_cpu_reg (r4k_context_t *sc, int index)
{
    return(sc->regs[REG_INDEX(index)]);
}

/*
 * r4k_get_cpu_addr:
 *
 * Return the address referenced in an instruction
 */
void *
r4k_get_cpu_addr (r4k_context_t *sc)
{
    union instr_r4k *op;

    if (sc->cause & CAUSE_BD_SLOT)
        op = (union instr_r4k *)(sc->epc + 4);
    else
        op = (union instr_r4k *)(sc->epc);

    return((uchar *)(r4k_get_cpu_reg(sc, op->i_t.rs) +op->i_t.s_imd));
}

/*
 * branch_emulate
 *
 * Emulate a branch if signal in bd slot
 */

int branch_emulate(r4k_context_t *sc)
{
    union instr_r4k *op = (union instr_r4k *)sc->epc;

    if (!(sc->cause & CAUSE_BD_SLOT)) {
	sc->epc += 4;
	return(0);
    }

    switch ( op->i_t.op ) {

    case OPCODE_SPECIAL:
	
	switch ( op->r_t.func ) {
	    
	case OPCODE_JALR:
	    r4k_set_cpu_reg(sc, op->r_t.rd, (sc->epc + 8), TRUE);
	    sc->epc = r4k_get_cpu_reg(sc, op->r_t.rs);
	    break;
	case OPCODE_JR:
	    sc->epc = r4k_get_cpu_reg(sc, op->r_t.rs);
	    break;
	default: 
	    return(1);
	}
	break;

    case OPCODE_REGIMM:

	switch ( op->r_t.rt ) {
	case OPCODE_BGEZ:
	case OPCODE_BGEZL:
	    if (r4k_get_cpu_reg(sc, op->i_t.rs) < 0)
		sc->epc += 8;
	    else
		sc->epc += (4 + (op->i_t.s_imd << 2));
	    break;
	case OPCODE_BGEZAL:
	case OPCODE_BGEZALL:
	    r4k_set_cpu_reg(sc, R4K_RA_REG, sc->epc + 8, TRUE);
	    if (r4k_get_cpu_reg(sc, op->i_t.rs) < 0)
		sc->epc += 8;
	    else
		sc->epc += (4 + (op->i_t.s_imd << 2));
	    break;
	case OPCODE_BLTZ:
	case OPCODE_BLTZL:
	    if (r4k_get_cpu_reg(sc, op->i_t.rs) >= 0) 
		sc->epc += 8;
	    else
		sc->epc += (4 + (op->i_t.s_imd << 2));
	    break;
	case OPCODE_BLTZAL:
	case OPCODE_BLTZALL:
	    r4k_set_cpu_reg(sc, R4K_RA_REG, sc->epc + 8, TRUE);
	    if (r4k_get_cpu_reg(sc, op->i_t.rs) >= 0)
		sc->epc += 8;
	    else
		sc->epc += (4 + (op->i_t.s_imd << 2));
	    break;
	default: 
	    return(1);
	}
	break;

    case OPCODE_J:
	sc->epc = (sc->epc & R4K_HI_ADDR_MASK) + (op->j_t.target << 2);
	break;
	
    case OPCODE_JAL:
	r4k_set_cpu_reg(sc, R4K_RA_REG, sc->epc + 8, TRUE);
	sc->epc = (sc->epc & R4K_HI_ADDR_MASK) + (op->j_t.target << 2);
	break;
	
    case OPCODE_BEQ:
    case OPCODE_BEQL:
	if (r4k_get_cpu_reg(sc, op->i_t.rs) != r4k_get_cpu_reg(sc, op->i_t.rt))
	    sc->epc += 8;
	else
	    sc->epc += (4 + (op->i_t.s_imd << 2));
	break;

    case OPCODE_BGTZ:
    case OPCODE_BGTZL:
	if (r4k_get_cpu_reg(sc, op->i_t.rs) <= 0)
	    sc->epc += 8;
	else
	    sc->epc += (4 + (op->i_t.s_imd << 2));
	break;
	
    case OPCODE_BLEZ:
    case OPCODE_BLEZL:
	if (r4k_get_cpu_reg(sc, op->i_t.rs) > 0)
	    sc->epc += 8;
	else
	    sc->epc += (4 + (op->i_t.s_imd << 2));
	break;

    case OPCODE_BNE:
    case OPCODE_BNEL:
	if (r4k_get_cpu_reg(sc, op->i_t.rs) == r4k_get_cpu_reg(sc, op->i_t.rt))
	    sc->epc += 8;
	else
	    sc->epc += (4 + (op->i_t.s_imd << 2));
	break;

    default:
	return(1);
    }

    /*
     * Succeeded
     */
    return(0);
}

/*
 * emulate_load_store
 *
 * Emulate a load/store for the misaligned handler
 */

static int emulate_load_store(ushort *access, ushort *read, r4k_context_t *sc)
{
    int size;
    union instr_r4k *op;
    uchar *addr;
    r4k_context_t align_ctx;

    if (sc->cause & CAUSE_BD_SLOT)
	op = (union instr_r4k *)(sc->epc + 4);
    else
	op = (union instr_r4k *)(sc->epc);

    addr = (unsigned char *)(r4k_get_cpu_reg(sc, op->i_t.rs) + op->i_t.s_imd);
    *read = FALSE;

    /*
     * Ensure that the memory being accessed is present
     */
    switch (op->i_t.op) {
      case OPCODE_LHU:
      case OPCODE_LH:
      case OPCODE_SH:
	size = 2;
	break;

      case OPCODE_LWU:
      case OPCODE_LW:
      case OPCODE_SW:
	size = 4;
	break;

      default:
	return 1;		/* indicate failure */
    }

    bcopy(sc, &align_ctx, sizeof(r4k_context_t));
    if (!touch_device(addr) ||
	!touch_device(addr + (size - 1))) {
	bcopy(&align_ctx, sc, sizeof(r4k_context_t));	/* restore context */
	return(1);					/* return failure  */
    }

    switch (op->i_t.op) {
    case OPCODE_LHU:
	r4k_set_cpu_reg(sc, op->i_t.rt, (GETSHORT(addr) & 0xffff), FALSE);
	*access = RTYPE_HW_BYTE;
	*read = TRUE;
	break;
    case OPCODE_LH:
	r4k_set_cpu_reg(sc, op->i_t.rt, GETSHORT(addr), TRUE);
	*access = RTYPE_HW_BYTE;
	*read = TRUE;
	break;
    case OPCODE_LW:
        r4k_set_cpu_reg(sc, op->i_t.rt, GETLONG(addr), TRUE);
	if ((int)addr & 0x01)
	    *access = RTYPE_LW_BYTE;
	else
	    *access = RTYPE_LW_HALF;
	*read = TRUE;
	break;
    case OPCODE_LWU:
        r4k_set_cpu_reg(sc, op->i_t.rt, GETLONG(addr), FALSE);
	if ((int)addr & 0x01)
	    *access = RTYPE_LW_BYTE;
	else
	    *access = RTYPE_LW_HALF;
	*read = TRUE;
	break;
    case OPCODE_SH:
	PUTSHORT(addr, r4k_get_cpu_reg(sc, op->i_t.rt));
	*access = WTYPE_HW_BYTE;
	break;
    case OPCODE_SW:
	PUTLONG(addr, r4k_get_cpu_reg(sc, op->i_t.rt));
	if ((int)addr & 0x01)
	    *access = WTYPE_LW_BYTE;
	else
	    *access = WTYPE_LW_HALF;
	break;
    }

    /*
     * return success
     */
    return(0);
}

/*
 * check_load_inst:
 * Check that the illegal access was a read cycle. If so, return the
 * instr_r4k pointer. Else return NULL.
 */
union instr_r4k *
check_load_inst (r4k_context_t *sc)
{
    union instr_r4k *op;
 
    if (sc->cause & CAUSE_BD_SLOT)
        op = (union instr_r4k *)(sc->epc + 4);
    else
        op = (union instr_r4k *)(sc->epc);
 
    if (op->i_t.op == OPCODE_LB ||
        op->i_t.op == OPCODE_LBU ||
        op->i_t.op == OPCODE_LHU ||
        op->i_t.op == OPCODE_LH  ||
        op->i_t.op == OPCODE_LW  ||
        op->i_t.op == OPCODE_LWU) {
        return(op);
    } else {
        return(NULL);
    }
}

/*
 * check_null_read:
 * Check that the illegal access was a read cycle. If so, return the
 * destination register number. Else return -1.
 */
int
check_null_read (r4k_context_t *sc)
{
    union instr_r4k *op;

    op = check_load_inst(sc);
    if (op == NULL) {
	return(-1);
    } else {
	return(op->i_t.rt);
    }
}

#define MAX_INSTR_COUNT	65535	/* limit length of search for function end */

/*
 * determine if the instruction at the given PC is part of a leaf function.
 * doesn't work on last instruction in function (branch delay slot)
 */
static boolean
is_leaf (ulong* pc)
{
    uint instr_count;
    boolean ret_delay;
    union instr_r4k *op;

    ret_delay = FALSE;
    op = (union instr_r4k*) pc;
    for (instr_count = 0; instr_count < MAX_INSTR_COUNT; ++instr_count) {
	if (!is_valid_pc(op))
	    return FALSE;

	if ((op->op_code == OPCODE_JAL) ||
	    (op->op_code == OPCODE_JALR))
	    return FALSE;

	if (op->op_code == OPCODE_JR_RA) {
	    ++op;				/* look in delay slot */
	    ret_delay = TRUE;			/* for last instruction */
	}

	/*
	 * Look for reading/writing the RA register 
	 * to/from the stack frame
	 */
	if ((op->i_t.op == OPCODE_LW || op->i_t.op == OPCODE_SW) &&
	    (op->i_t.rt == R4K_RA_REG) && 
	    (op->i_t.rs == R4K_SP_REG))
	    return FALSE;

	if (ret_delay)
	    return TRUE;

	++op;
    }
    return FALSE;
}

/*
 * save the specified number of stack frames into an array, and
 * clear to zero unused array elements.
 */
void
r4k_pc_trace (ulong* trace, uint nframes, r4k_context_t* sc)
{
    int frame_index;
    ulong *pc, *sp;

    if (sc->cause & CAUSE_BD_SLOT) {
	pc = (ulong*) (sc->epc + 4);
    } else {
	pc = (ulong*) sc->epc;
    }

    frame_index = 0;

    if (is_leaf((ulong*) sc->epc)) {
	trace[frame_index++] = (ulong) pc;
	pc = (ulong*) r4k_get_cpu_reg(sc, R4K_RA_REG);
    }

    sp = (ulong*) r4k_get_cpu_reg(sc, R4K_SP_REG);

    while (frame_index < nframes) {
	trace[frame_index++] = (ulong) pc;
	sp = trace_caller(sp, &pc);
	if (sp == NULL)
	    break;
    } 
    while (frame_index < nframes)
	trace[frame_index++] = 0;
    return;
}

/*
 * handle_null_pointer :
 * handle accesses to low addresses. If the system has been setup to 
 * 'emulate' reads from low addresses, log the error, 'emulate' the 
 * cycle and continue. Else print a message and crash the box by 
 * re-executing the cycle without re-enabling the handler.
 */
static void
handle_null_pointer (int cause, r4k_context_t *sc)
{
    int i, regnum;
    alignment_entry_t *entry;
    align_traceback_t trace;
    
    /*
     * If we are not recovering from Null pointers or it was a write
     * cycle to a low address, print a message and crash.
     */
    regnum = check_null_read(sc);
    if (regnum == -1) {

	/*
	 * We are not recovering from Null pointers or it was a write
	 */
	ttyprintf(CONTTY,
		  "\n%%ALIGN-1-FATAL: Illegal access to a low address\n");
	ttyprintf(CONTTY, " addr=%#x, pc=%#x, ra=%#x, sp=%#x\n",
		  sc->badvaddr, sc->epc,
		  r4k_get_cpu_reg(sc, R4K_RA_REG),
		  r4k_get_cpu_reg(sc, R4K_SP_REG));
	msgdef_explanation(
	    "The router identified a fatal error in a software
	    component and could not correct it.");
	msgdef_recommended_action(
	    "Copy the error message exactly as it appears, and report
	    it to your technical support representative.");
	msgdef_ddts_component("");


	return;
    }
    
    /*
     * We are recovering from Null pointers
     */
    entry = NULL;
    r4k_pc_trace(trace.pc, ALIGN_FRAMES, sc);

    for (i=0; i<align->nulltail; i++) {
	entry = &align->nulltable[i];
	if (bcmp(&entry->trace, &trace, sizeof(trace)) == 0) {
	    entry->count += 1;
	    entry = NULL;
	    break;
	}
    }

    /*
     * If we ran off the end without matching, creat a new entry ...
     */
    if (i == align->nulltail && align->nulltail < NULL_TABLE_MAX) {
	entry = &align->nulltable[align->nulltail++];
	entry->count = 0;
    }

    /*
     * If we have an entry, write it
     */
    if (entry) {
	entry->message_printed = FALSE;
	entry->addr = sc->badvaddr;
	entry->trace = trace;
	entry->count = 1;
	align->null_msg_needed = TRUE;
	align->nullrecorded += 1;
    }
    align->nulltotal += 1;

    /*
     * If we were in a branch delay slot, figure out where to return to
     * and re-install the handlers
     */
    if (branch_emulate(sc))
	return;

    /*
     * Fill the destination register with 0 (null pointers read zero)
     */
    r4k_set_cpu_reg(sc, regnum, 0, FALSE);
    align->sigsegv_save = signal(SIGSEGV, (SIG_PF)unaligned_handler);
    align->sigbus_save = signal(SIGBUS, (SIG_PF)unaligned_handler);
}

/*
 * Returns TRUE if the address is aligned properly and lies within
 * the text segment
 */
boolean
is_valid_pc (void *address)
{
    ulong addr;
    regiontype *region;

    addr = (ulong) address;
    if (addr & 0x03)
	return FALSE;
    region = region_find_by_addr(address);
    if (region == NULL)
	return FALSE;
    if (region_get_class(region) == REGION_CLASS_IMAGETEXT)
	return TRUE;
    return FALSE;
}

/*
 * return TRUE if fixup of misaligned access should be attempted
 */
static boolean
is_valid_ram (void *address)
{
    regiontype *region;

    region = region_find_by_addr(address);
    if (region == NULL)
	return FALSE;
    switch (region_get_class(region)) {
      case REGION_CLASS_LOCAL:
      case REGION_CLASS_IOMEM:
      case REGION_CLASS_PCIMEM:
      case REGION_CLASS_FAST:
      case REGION_CLASS_CBUS:
      case REGION_CLASS_NVRAM:
      case REGION_CLASS_IMAGETEXT:
      case REGION_CLASS_IMAGEDATA:
      case REGION_CLASS_IMAGEBSS:
	return TRUE;

      default:
	break;
    }
    return FALSE;
}

/*
 * unaligned_handler 
 *
 * Service the unaligned trap and return
 */

int unaligned_handler(int sig, int code, r4k_context_t *sc)
{
    alignment_entry_t *entry;
    int i, cause;
    ushort access, read;
    align_traceback_t trace;

    signal(SIGSEGV, align->sigsegv_save);
    signal(SIGBUS, align->sigbus_save);

    /*
     * Get the Cause bits
     */
    cause = (sc->cause & CAUSE_MASK) >> CAUSE_SHIFT;

    if (!alignment_activated || cause == CAUSE_BUS_DATA) {
	/*
	 * We are not activated or we took a bus error on data load/store, 
	 * so re-execute without re-installing handler which will cause 
	 * the default signal behaviour
	 */

	return(0);
    }
    
    /*
     * Check for a bus error on Instruction fetch. If so write epc into
     * badvadddr (the cpu does not do this on bus error).
     */
    if (cause == CAUSE_BUS_INSTR)
	sc->badvaddr = sc->epc;

    /*
     * Check for a mis-aligned or errant PC
     */
    if (!is_valid_pc((void *)sc->epc)) {
	ttyprintf(CONTTY, "\n%%ALIGN-1-FATAL: Corrupted program counter\n");
	ttyprintf(CONTTY, " pc=%#x, ra=%#x, sp=%#x\n", sc->epc, 
		  r4k_get_cpu_reg(sc, R4K_RA_REG),
		  r4k_get_cpu_reg(sc, R4K_SP_REG));
	
	return(0);
    }	

    /*
     * Check for data access at a low address
     */
    if (sc->badvaddr >= 0 && sc->badvaddr < ALIGN_ADDR_LIMIT) {
	handle_null_pointer(cause, sc);
	return(0);
    }

    if (!is_valid_ram((void*) sc->badvaddr)) {
	return(0);
    }

    /*
     * Search through table, find matching PC (if any) and:
     *
     * . If the PC isn't already in our list, make a new entry and put it in.
     *
     * . Flag the once-per-minute task to print a warning, if necessary.
     */

    entry = NULL;
    r4k_pc_trace(trace.pc, ALIGN_FRAMES, sc);

    for (i=0; i < align->tail; i++) {
	entry = &align->table[i];

	if (bcmp(&entry->trace, &trace, sizeof(trace)) == 0) {
	    entry->count++;
	    entry = NULL;
	    break;
	}
    }

    /*
     * If we ran off the end without matching, then make a new entry...
     */

    if (i == align->tail && align->tail < ALIGN_TABLE_MAX) {
	entry = &align->table[align->tail++];
	entry->count = 0;
    }
    
    /*
     * Write the entry.
     */

    if (entry) {
	entry->message_printed = FALSE;
	
	entry->addr = sc->badvaddr;
	entry->trace = trace;
	align->align_msg_needed = TRUE;
    }

    align->total += 1;


    /*
     * Perform the operation, and figure out where to go
     */
    if (emulate_load_store(&access, &read, sc)) {
	return(0);
    } else {
	if (read)
	    align->reads += 1;
	else
	    align->writes += 1;
	if (entry) {
	    entry->access_type = access;
	    entry->count += 1;
	    align->recorded += 1;
	}
	if (branch_emulate(sc)) {
	    return(0);
	}
	align->sigsegv_save = signal(SIGSEGV, (SIG_PF)unaligned_handler);
	align->sigbus_save = signal(SIGBUS, (SIG_PF)unaligned_handler);
    }
    return(0);
}


/*
 * alignment_one_min
 *
 * Show error message if alignment data has accumulated, or the table is 
 * full...
 */

void alignment_one_min(void)
{
    alignment_entry_t *ptr;
    int i;

    if (!alignment_activated) return;

    /* Are we full?  If so, only print this once. */

    if (align->tail >= ALIGN_TABLE_MAX && align->database_full == FALSE) {
	(*kernel_errmsg)(&msgsym(FULL,ALIGN));
	align->database_full = TRUE;
    }

    /* Do we have more work to do? */

    if (!align->logging) return;

    if (align->align_msg_needed) {

	/*
	 * Scan the table and print the misalignments that don't have
	 * the "printed" flag set.
	 */

	for (i=0; i < align->tail; i++) {
	    ptr = &align->table[i];

	    if (!ptr->message_printed) {
		(*kernel_errmsg)(&msgsym(CORRECT,ALIGN),
		       ptr->trace.pc[0],
		       (ptr->access_type <= RTYPE_MAX)? "read":"writ",
		       ptr->addr);
		ptr->message_printed = TRUE;
	    }
	}
	align->align_msg_needed = FALSE;
    }

    /*
     * Print out Null pointer messages if applicable
     */
    if (align->null_msg_needed) {

	/*
	 * Scan the table and print Null pointer accesses that don't have
	 * the "printed" flag set.
	 */
	for (i=0; i < align->nulltail; i++) {
	    ptr = &align->nulltable[i];
	    if (!ptr->message_printed) {
		(*kernel_errmsg)(&msgsym(SPURIOUS, ALIGN), ptr->trace.pc[0],
				 ptr->addr);
		ptr->message_printed = TRUE;
	    }
	}
	align->align_msg_needed = FALSE;
    }
}

/*
 * alignment_handler_disable
 *
 * Disable correcting MIPS mis-aligned accesses
 */

void alignment_handler_disable (void)
{
    if (alignment_activated) {
	signal(SIGSEGV, align->sigsegv_save);
	signal(SIGBUS, align->sigbus_save);

	free(align);
	alignment_activated = FALSE;
    }
}

/*
 * alignment_handler_enable()
 *
 * Enable correcting MIPS mis-aligned accesses
 */

void alignment_handler_enable (void)
{
    if (!alignment_activated) {

	align = malloc_named(sizeof(alignment_t), "Alignment Data");

	if (!align) {
	    alignment_activated = FALSE;
	    return;
	}
	    
	alignment_activated = TRUE;
	align->logging = ALIGN_LOG_DEFAULT;
	
	align->align_msg_needed = FALSE;
	align->null_msg_needed = FALSE;
	align->database_full = FALSE;

	align->total = 0;
	align->recorded = 0;
	align->tail = 0;
	align->reads = 0;
	align->writes = 0;
	align->nulltotal = 0;
	align->nullrecorded = 0;
	align->nulltail = 0;

	align->sigsegv_save = signal(SIGSEGV, (SIG_PF)unaligned_handler);
	align->sigbus_save = signal(SIGBUS, (SIG_PF)unaligned_handler);
    }
}


static void alignment_subsys_init(subsystype* subsys)
{
    alignment_activated  = FALSE;
    alignment_handler_enable();

    reg_add_onemin(alignment_one_min, "alignment_one_min");

}

#define ALIGN_MAJOR_VERSION 1
#define ALIGN_MINOR_VERSION 0
#define ALIGN_EDIT_VERSION 2

SUBSYS_HEADER(alignment,
	      ALIGN_MAJOR_VERSION, ALIGN_MINOR_VERSION, ALIGN_EDIT_VERSION,
	      alignment_subsys_init, SUBSYS_CLASS_KERNEL,
	      NULL, NULL);
