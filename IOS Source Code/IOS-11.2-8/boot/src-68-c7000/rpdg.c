/* $Id: rpdg.c,v 3.1.58.1 1996/08/07 17:48:16 kmitchel Exp $
 * $Source: /release/112/cvs/Xboot/src-68-c7000/rpdg.c,v $
 *------------------------------------------------------------------
 * rpdg.c -- command parser for monitor software that tests the RP
 * diagnostic bus functions.
 *
 * December 1991, Stan Mantiply
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All Rights Reserved.
 *------------------------------------------------------------------
 * $Log: rpdg.c,v $
 * Revision 3.1.58.1  1996/08/07  17:48:16  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  09:10:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.4  1995/05/03 21:29:56  sthormod
 * CSCdi30508:  ROM monitor reset_io does not work on the C7000
 * Do a dbus reset to every occupied slot.
 *
 * Revision 1.3  1995/04/04  18:35:59  schaefer
 * CSCdi31819: netbooting rsp/10.3(2.0.3) fails if MIP is configured
 * fix the C7000 ROM monitor (again)
 *
 * Revision 1.2  1995/03/30  22:58:43  schaefer
 * CSCdi31653: writes to dbus registers need protection
 * C7000 ROM monitor needs this treatment too
 *
 * Revision 1.1  1993/09/24  16:34:36  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.3  1993/07/01  23:11:13  hampton
 * Bring modular system to Aries 9.21(0.12) patch level.
 *
 * Revision 1.2  1993/03/30  00:24:00  mdb
 * fix minor problems found by gcc 2.x warnings.
 *
 * Revision 1.1  1993/01/30  01:12:04  whp4
 * Add RP1 support.
 *
 *
 * $Endlog$
 */

/*
 * This routine determines which functions are being requested and parses
 * input commands.
 *
 * This sub function of the monitor will perform the following functions:
 *
 *	a		All slots display
 *	o		One slot display
 *	w		Walking tests on all slots
 *	k		Walking tests on a specified slot
 *	i		Infinite passes of all dBus tests on selected slots
 *	e		Error array display
 *
 *	p		Primitive Slot access
 *	s		Scope loop
 *
 *	c		writable Control store tests
 *	r		ROM read tests
 *	m		Move ROM to WCS and verify parity
 *
 *	n		iNitialize global variables
 *
 *	q		Quit and return to higher level interpreter
 *	h or ?		Help for commands
 *      l               List I/O map of card cage
 *      d               Dump registers for one slot
 */
#include "../defs.h"
#include "../stdio.h"
#include "../ctype.h"
#include "../cpu.h"
#include "../statreg.h"
#include "../prom2.h"
#include "../reason.h"
#include "../extern.h"
#include "../test.h"
#include "../save_stack.h"

#define PACKED(item)	item

#include "../../sys/hes/dgbus.h"

typedef struct dgtype_ dgtype;

extern int reset_io();
extern u_long clock;

#define NSLOTS	16
#define N_LOW_ADDRESS_BITS	6

#define DGERRORBASE		0x003F8000
#define GLOBALSTORAGE		0x003f4000
#define WCSDATABASE		0x003a0000
#define WCSRESULTSBASE		0x003c0000

#define MAGIC_ENTRY		0xf1e2d3c4

#define MAX_WCS_CHIPS				8
#define MAX_WCS_WORDS				4096
#define N_WCS_COMMAND_BITS			8
#define N_WCS_DATA_BITS				16

#define N_ROM_BYTES				0x20000

typedef struct dg_error_ {
    u_long err_count;
    u_short err_last_actual;
    u_short err_last_expected;
} dg_error;

typedef struct dgerrcnt_ {
    dg_error no_response;
    dg_error invalid_slot_and_complement;
    dg_error invalid_CxBus_number;
    dg_error invalid_Logical_Slot;
    dg_error dBus_Read_status;
    dg_error dbus_adrs_5_0_walking_one_errors;
    dg_error dbus_adrs_5_0_walking_zero_errors;
    dg_error dbus_data_7_0_walking_one_errors;
    dg_error dbus_data_7_0_walking_zero_errors;
    dg_error WCS_Zeros_errors;
    dg_error WCS_Ones_errors;
    dg_error WCS_Addr_in_Addr_errors;
    dg_error WCS_Comp_Addr_in_Addr_errors;
    dg_error WCS_aaaa_errors;
    dg_error WCS_5555_aaaa_errors;
    dg_error WCS_Walking_Ones_Zeros_errors;
    dg_error WCS_parity_detection;
    dg_error WCS_cntrl_instr_err_detection;
} dgerrcnt;

#define N_DG_ERRORS		18  /* elements in above and below vectors */

char *WCS_test_descriptions[] = {
    "no response",
    "slot and complement",
    "CxBus number",
    "Logical slot",
    "dBus Read status",
    "dBus adrs walking ones",
    "dBus adrs walking zeros",
    "dBus data walking ones",
    "dBus data walking zeros",
    "WCS zeros",
    "WCS ones",
    "WCS addr in addr",
    "WCS comp addr in addr",
    "WCS aaaa",
    "WCS 5555 and aaaa",
    "WCS walking 1's and 0's",
    "WCS parity detection",
    "WCS cntrl instr err detection"
    };

typedef struct hi_mem_globals_ {
    u_long diag_previous_entry;
    u_long Total_Bus_Error_Count;
    u_long Total_Error_Count;
    u_long requested_iterations;
    u_long actual_iterations;
    u_long WCS_words;
    u_long WCS_starting_row;
    u_long WCS_tests_vector;
    u_long long_buserrcnt;
    u_short Slots_to_test_vector;
    u_short line_count;
    u_short line_limit;
    u_short WCS_valid_opcode_vector;
    u_char Slot;
    u_char Slot_Address;
    u_char Write_Data;
    u_char first_WCS_test_ID;
    u_char last_WCS_test_ID;
    u_char num_WCS_chips[16];
    char diag_read_or_write;
    char verbose;
    char Zero_Error_structure;
    char all_slots;
    char all_rows;
    char WCS_chip_num_status;
    boolean temp_abort;
} hi_mem_globals;


inline dgtype *pslot_to_dbus (int slot)
{
    return &((dgtype*) DGBUSSTART)[slot];
}

/*
 * support protected writes to dbus registers
 * the C7000 doesn't need this protection
 */
void
dbus_prot_wr (uint slot, protected_uchar *reg, uchar value)
{
    reg->value = value;
}

rpdiagtest()

{
    char *c;
    char comm;
    char old_abortstate;
    u_char dgdata;
    u_char dgresult;
    u_long temp;
    u_long temp_count;
    u_char temp_b;
    char temp_c;
    dgtype *dgbus;
    u_char slot_and_complement;
    u_char CxBus_number;
    long indx;
    long jndx;
    long kndx;
    u_long scope_loop_total;
    u_long limit;

    u_short *WCS_data;
    u_short *WCS_results;

    dgerrcnt *dg_errors;
    hi_mem_globals *glbl;

    u_long last_clock;

    WCS_data = (u_short *)WCSDATABASE;
    WCS_results = (u_short *)WCSRESULTSBASE;

    dg_errors = (dgerrcnt *)DGERRORBASE;
    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    reset_io();			/* Reset all DMA devices so they stop */
    
    printf("Diagnostic bus test for RP\n\n");
    diag_help();

    if (glbl->diag_previous_entry != MAGIC_ENTRY)
	init_hi_mem_globals(dg_errors);

    old_abortstate = abortstate;

    for (;;) {

	abortstate = ABORT_IGNORE;
	puts(">>");
	gets(bufu.buf);
	abortstate = ABORT_INTERNAL;
	
	c = bufu.buf;
	while (*c && isspace(*c))
	    c++;		/* skip leading spaces */
	if (*c == '\0')
	    continue;
	comm = *c;
	/* find command argument if present */
	while (*c && !isspace(*c) && *c != '/')
	    c++;		/* skip rest of command */
	while (*c && isspace(*c))
	    c++;		/* skip leading spaces */
	
	switch (comm) {

	case 'A':		/* All slots display */

	    temp = glbl->Total_Error_Count;
	    for (indx = 0; indx < NSLOTS; indx++) {
		disp_slot(dg_errors, indx, 'y');
		if (abort) {
		    abort = FALSE;
		    printf("\nAborted\n");
		    break;
		}
	    }
	    if (temp == glbl->Total_Error_Count)
		printf("No detected errors\n");
	    break;


	case 'O':		/* One slot display */

	    get_Slot();
	    printf("Using physical slot [%u]!\n ", glbl->Slot);
	    temp = glbl->Total_Error_Count;
	    disp_slot(dg_errors, glbl->Slot, 'y');
	    if (temp == glbl->Total_Error_Count)
		printf("No detected errors\n");
	    break;


    	case 'W':		/* Walking bits test on all slots */

	    temp = glbl->Total_Error_Count;
	    for (indx = 0; indx < NSLOTS; indx++) {
		walk_slot(dg_errors, indx, 'y');
		if (abort) {
		    abort = FALSE;
		    printf("\nAborted\n");
		    break;
		}
	    }
	    printf("\n");
	    if (temp == glbl->Total_Error_Count)
		printf("No detected errors\n");
	    break;


    	case 'K':		/* Walking bits test on a specified slot */

	    get_Slot();
	    printf("Using physical slot [%u]!\n", glbl->Slot);
	    temp = glbl->Total_Error_Count;
	    walk_slot(dg_errors, glbl->Slot, 'y');
	    printf("\n");
	    if (temp == glbl->Total_Error_Count)
		printf("No detected errors\n");
	    break;

    	case 'I':	  /* Infinite passes of all tests on selected slots */

	    get_verbose();
	    get_Zero_Error_structure();
	    get_requested_iterations();
	    get_Slots_to_test_vector();
	    glbl->actual_iterations = 0;
	    abort = glbl->temp_abort = FALSE;

	    if (glbl->Zero_Error_structure == 'y')
		zero_error_vector(dg_errors);

	    if (glbl->requested_iterations)
		while (glbl->requested_iterations > glbl->actual_iterations) {
		    if (glbl->requested_iterations > glbl->actual_iterations +
			1000)
			limit = 1000;
		    else
			limit = glbl->requested_iterations -
			    glbl->actual_iterations;
		    for (indx = 0; indx < limit; indx++) {
			for (jndx = 0; jndx < NSLOTS; jndx++) {
			    if (glbl->Slots_to_test_vector & 1 << jndx) {
				disp_slot(dg_errors, jndx, glbl->verbose);
				walk_slot(dg_errors, jndx, glbl->verbose);
				if (abort) {
				    glbl->temp_abort = abort;
				    break;
				}
			    }
			}
			if (glbl->temp_abort)
			    break;
		    }
		    glbl->actual_iterations += indx;
		    printf("\r%u passes, total errors: %u",
			   glbl->actual_iterations, glbl->Total_Error_Count);
		    if (glbl->temp_abort) {
			abort = glbl->temp_abort = FALSE;
			printf("\nAborted\n");
			break;
		    }
		}
	    else
		while(TRUE) {
		    for (indx = 0; indx < 1000; indx++) {
			for (jndx = 0; jndx < NSLOTS; jndx++) {
			    if (glbl->Slots_to_test_vector & 1 << jndx) {
				disp_slot(dg_errors, jndx, glbl->verbose);
				walk_slot(dg_errors, jndx, glbl->verbose);
				if (abort) {
				    glbl->temp_abort = abort;
				    break;
				}
			    }
			}
			if (glbl->temp_abort)
			    break;
		    }
		    glbl->actual_iterations += indx;
		    printf("\r%u passes, total errors: %u",
			   glbl->actual_iterations, glbl->Total_Error_Count);
		    if (glbl->temp_abort) {
			abort = glbl->temp_abort = FALSE;
			printf("\nAborted\n");
			break;
		    }
		}
	    printf("\n");
	    break;
		    
	case 'E':		/* Error array display */

	    printf("Address of array %x\n", dg_errors);
	    printf("%u passes, total errors: %u total bus errors: %u\n",
			       glbl->actual_iterations,
			       glbl->Total_Error_Count,
			       glbl->Total_Bus_Error_Count);
	    get_all_slots();
	    if (glbl->all_slots == 'y') {
		abort = FALSE;
		for (indx = 0; indx < NSLOTS; indx++) {
		    display_error_vector(dg_errors, indx);
		    if (abort) {
			abort = FALSE;
			printf("\nAborted\n");
			break;
		    }
		}
	    } else {
		get_Slot();
		display_error_vector(dg_errors, glbl->Slot);
	    }
	    printf("\n");
	    break;
	    
	case 'D':		/* Dump slot registers */

	    get_Slot();
	    dump_reg_slot(dg_errors, glbl->Slot, 'y');
	    break;

	case 'P':		/* Primitive Slot access */

	    get_Slot();
	    get_Slot_Address();
	    get_diag_read_or_write();
	    if (glbl->diag_read_or_write == 'w') {
		get_Write_Data();

		dgbus = (dgtype *)((glbl->Slot << 6) + DGBUSSTART);
		start_buserr_count_region();
		*(u_char *)((u_long)dgbus | (u_long)glbl->Slot_Address) =
		    glbl->Write_Data;
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop"); 
		asm("	nop");
		asm("	nop");
		asm("	nop");
		buserrhandler = 0;

		printf("Wrote %x, to slot %u at address %x\n",
		       glbl->Write_Data, glbl->Slot, glbl->Slot_Address);
	    } else {
		dgbus = (dgtype *)((glbl->Slot << 6) + DGBUSSTART);

		start_buserr_count_region();
		temp_b = *(u_char *)((u_long)dgbus |
				     (u_long)glbl->Slot_Address);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		buserrhandler = 0;
		printf("Read %x from slot %u at address %x\n",
		       temp_b, glbl->Slot, glbl->Slot_Address);
	    }
	    break;

	case 'S':		/* Scope loop */

	    scope_loop_total = 0;

	    get_Slot();
	    get_Slot_Address();
	    get_diag_read_or_write();
	    printf("\nHit break key to abort.\n");
	    if (glbl->diag_read_or_write == 'w') {
		get_Write_Data();
		dgbus = (dgtype *)((glbl->Slot << 6) + DGBUSSTART);
		for (;;) {
		    start_buserr_count_region();
		    for (indx = 0; indx < 1000; indx++) {
			*(u_char *)((u_long)dgbus |
				    (u_long)glbl->Slot_Address) =
					glbl->Write_Data;
			asm("	nop");
			asm("	nop");
			asm("	nop");
			asm("	nop");
			asm("	nop");
			asm("	nop");
			asm("	nop");
			asm("	nop");
		    }
		    buserrhandler = 0;
		    
		    scope_loop_total += indx;
		    printf("\rWritten %u", scope_loop_total);
		    if (buserrcnt)
			printf("    with %u bus errors", buserrcnt);
		    if (abort) {
			abort = FALSE;
			printf("\nAborted\n");
			break;
		    }
		}
	    } else {
		dgbus = (dgtype *)((glbl->Slot << 6) + DGBUSSTART);

		start_buserr_count_region();
		temp_b = *(u_char *)((u_long)dgbus |
				     (u_long)glbl->Slot_Address);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop"); 
		asm("	nop");
		asm("	nop");
		asm("	nop");
		buserrhandler = 0;
		printf("Read %x from slot %u at address %x\n",
		       temp_b, glbl->Slot, glbl->Slot_Address);

		for (;;) {
		    start_buserr_count_region();
		    for (indx = 0; indx < 100000; indx++) {
			temp_b = *(volatile u_char *)((u_long)dgbus |
				   (u_long)glbl->Slot_Address);
			asm("	nop");
			asm("	nop");
			asm("	nop");
			asm("	nop");
			asm("	nop"); 
			asm("	nop"); 
			asm("	nop"); 
			asm("	nop"); 
		    }
		    buserrhandler = 0;

		    scope_loop_total += indx;
		    printf("\rRead %u, last data: %2x", scope_loop_total,
			   temp_b);
		    if (abort) {
			abort = FALSE;
			printf("\nAborted\n");
			break;
		    }
		}
	    }
	    break;

	case 'C':	/* Writable Control Store (WCS) tests */

	    get_verbose();
	    if (glbl->verbose == 'y')
		get_line_limit();
	    get_Zero_Error_structure();
	    get_requested_iterations();
	    glbl->actual_iterations = 0;
	    abort = glbl->temp_abort = FALSE;

	    if (glbl->Zero_Error_structure == 'y')
		zero_error_vector(dg_errors);

	    get_WCS_words();
	    get_WCS_tests_vector();
	    get_all_slots();
	    if (glbl->all_slots == 'y') {
		while (TRUE) {
		    get_WCS_chip_num_status();
		    if (glbl->WCS_chip_num_status == 'n') {
			get_Slot();
			get_num_WCS_chips();
		    } else
			break;
		}
		if (glbl->requested_iterations)
		    while (glbl->requested_iterations >
			   glbl->actual_iterations) {
			if (glbl->requested_iterations >
			    glbl->actual_iterations + 1)
			    limit = glbl->actual_iterations + 1;
			else
			    limit = glbl->requested_iterations;
			for (; glbl->actual_iterations < limit;
			     glbl->actual_iterations++) {
			    for (jndx = 0; jndx < NSLOTS; jndx++) {
				WCS_tests(dg_errors, jndx, glbl->verbose,
					  WCS_data, WCS_results);
				if (abort) {
				    glbl->temp_abort = abort;
				    break;
				}
			    }
			    if (glbl->temp_abort)
				break;
			}
			printf("\r%u passes, total errors: %u total "
			       "bus errors: %u",
			       glbl->actual_iterations,
			       glbl->Total_Error_Count,
			       glbl->Total_Bus_Error_Count);
			if (glbl->temp_abort) {
			    abort = glbl->temp_abort = FALSE;
			    printf("\nAborted\n");
			    break;
			}
		    }
		else
		    while(TRUE) {
			temp = glbl->actual_iterations + 1;
			for (; glbl->actual_iterations < temp;
			     glbl->actual_iterations++) {
			    for (jndx = 0; jndx < NSLOTS; jndx++) {
				WCS_tests(dg_errors, jndx, glbl->verbose,
					  WCS_data, WCS_results);
				if (abort) {
				    glbl->temp_abort = abort;
				    break;
				}
			    }
			    if (glbl->temp_abort)
				break;
			}
			printf("\r%u passes, total errors: %u total "
			       "bus errors: %u",
			       glbl->actual_iterations,
			       glbl->Total_Error_Count,
			       glbl->Total_Bus_Error_Count);
			if (glbl->temp_abort) {
			    abort = glbl->temp_abort = FALSE;
			    printf("\nAborted\n");
			    break;
			}
		    }
	    } else {
		get_Slot();
		get_num_WCS_chips();
		if (glbl->requested_iterations)
		    while (glbl->requested_iterations >
			   glbl->actual_iterations) {
			if (glbl->requested_iterations >
			    glbl->actual_iterations + 1)
			    limit = glbl->actual_iterations + 1;
			else
			    limit = glbl->requested_iterations;
			for (; glbl->actual_iterations < limit;
			     glbl->actual_iterations++) {
			    WCS_tests(dg_errors, glbl->Slot, glbl->verbose,
				      WCS_data, WCS_results);
			    if (abort) {
				glbl->temp_abort =abort;
				break;
			    }
			}
			printf("\r%u passes, total errors: %u total "
			       "bus errors: %u",
			       glbl->actual_iterations,
			       glbl->Total_Error_Count,
			       glbl->Total_Bus_Error_Count);
			if (glbl->temp_abort) {
			    abort = glbl->temp_abort = FALSE;
			    printf("\nAborted\n");
			    break;
			}
		    }
		else
		    while(TRUE) {
			temp = glbl->actual_iterations + 1;
			for (; glbl->actual_iterations < temp;
			     glbl->actual_iterations++) {
			    WCS_tests(dg_errors, glbl->Slot, glbl->verbose,
				      WCS_data, WCS_results);
			    if (abort) {
				glbl->temp_abort = abort;
				break;
			    }
			}
			printf("\r%u passes, total errors: %u total "
			       "bus errors: %u",
			       glbl->actual_iterations,
			       glbl->Total_Error_Count,
			       glbl->Total_Bus_Error_Count);
			if (glbl->temp_abort) {
			    abort = glbl->temp_abort = FALSE;
			    printf("\nAborted\n");
			    break;
			}
		    }
	    }
	    printf("\n");
	    break;

	case 'R':		/* ROM read test */

	    get_verbose();
	    abort = glbl->temp_abort = FALSE;
	    get_Slot();
	    dgbus = (dgtype *)((glbl->Slot << 6) + DGBUSSTART);

	    glbl->Total_Bus_Error_Count = 0;
/*
 * Initialize the arrays to ensure every byte will not match if no data is read
 * into them.  Missing cards generate bus errors which store nothing into the
 * array.
 */
	    for (indx = 0; indx < N_ROM_BYTES; indx++) {
		((u_char *)WCS_data)[indx] = (u_char)indx;
		((u_char *)WCS_results)[indx] = ~(u_char)indx;
	    }
	    start_buserr_count_region();
/*
 * The diag control register bit settings are to prohibit any ucode from
 * loading (via ROM) or running, and to allow access by this test to the ROM.
 */
	    dbus_prot_wr(glbl->Slot, &dgbus->diag_control, WCS_DBUS_CMD_ENABLE);
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    dbus_prot_wr(glbl->Slot, &dgbus->diag_status, 0);		/* Reset */
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    dgbus->ucode_command_and_data =
		RESET_ROM_ADDRESS_REGISTER << 4 | 0;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    end_buserr_count_region();
	    read_ROM(dg_errors, glbl->Slot, glbl->verbose, WCS_data);

	    for (indx = 0; indx < N_ROM_BYTES; indx++) {
		if (((u_char *)WCS_data)[indx] != (u_char)indx)
		    break;
	    }
	    if (indx == N_ROM_BYTES) {
		printf("No data received from ROM\n");
		break;
	    }
	    glbl->Total_Error_Count = 0;
	    for (glbl->actual_iterations = 0; !abort;
		 glbl->actual_iterations++) {
		start_buserr_count_region();
		dgbus->ucode_command_and_data =
		    RESET_ROM_ADDRESS_REGISTER << 4 | 0;
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		end_buserr_count_region();
		read_ROM(dg_errors, glbl->Slot, glbl->verbose, WCS_results);
		for (jndx = 0; jndx < N_ROM_BYTES; jndx++)
		    if (((u_char *)WCS_results)[jndx] !=
			((u_char *)WCS_data)[jndx]) {
			glbl->Total_Error_Count++;
			if (glbl->verbose == 'y') {
			    check_flow_control();
			    printf("At: %5x, actual: %2x, expected: %2x\n",
				   jndx, ((u_char *)WCS_results)[jndx],
				   ((u_char *)WCS_data)[jndx]);
			}
			if (abort) {
			    glbl->temp_abort = abort;
			    break;
			}
		    }
		printf("\r%u passes, total errors: %u total bus errors: %u",
		       glbl->actual_iterations,
		       glbl->Total_Error_Count,
		       glbl->Total_Bus_Error_Count);
		if (glbl->temp_abort) {
		    abort = glbl->temp_abort = FALSE;
		    break;
		}
	    }
	    printf("\nAborted\n");
	    break;


	case 'M':		/* Move ROM to WCS and verify parity*/
	    
	    get_Slot();
	    get_verbose();
	    if (glbl->verbose == 'y') {
		get_all_rows();
		get_line_limit();
	    }
	    get_WCS_words();
	    get_num_WCS_chips();

	    dgbus = (dgtype *)((glbl->Slot << 6) + DGBUSSTART);
	    abort = glbl->temp_abort = FALSE;
	    glbl->Total_Bus_Error_Count = 0;
/*
 * Initialize WCS to zeros.
 */
	    for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS; indx++)
		WCS_data[indx] = 0;
	    
	    write_read_compare(dg_errors, glbl->Slot, glbl->verbose, WCS_data,
			       WCS_results, glbl->first_WCS_test_ID);
	    
	    start_buserr_count_region();
/*
 * The diag control register bit settings are to prohibit any ucode from
 * loading (via ROM) or running.
 */
	    dbus_prot_wr(glbl->Slot, &dgbus->diag_control, WCS_DBUS_CMD_ENABLE);
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    dbus_prot_wr(glbl->Slot, &dgbus->diag_status, 0);		/* Reset */
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    dgbus->ucode_command_and_data =
		RESET_ROM_ADDRESS_REGISTER << 4 | 0;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
/*
 * This allows the WCS to be loaded from ROM.  Also, it prevents the micro-
 * processor from running.
 */
	    dbus_prot_wr(glbl->Slot, &dgbus->diag_control, 0);
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    
	    for (indx = 0; TRUE; indx++) {
		temp_b = dbus_prot_rd(&dgbus->diag_control);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		
		printf("\r%u passes, diag_control: %2x %s %s",
		       indx, temp_b,
		       temp_b & WCS_LOADED ? "(LOADED)" : "",
		       temp_b & WCS_LOADING ? "(LOADING)" : "");
		if (temp_b & WCS_LOADED)
		    break;
		if (abort) {
		    glbl->temp_abort = abort;
		    break;
		}
	    }
	    if (glbl->temp_abort) {
		abort = glbl->temp_abort = FALSE;
		printf("\nAborted\n");
		break;
	    }
	    printf("\n");
/*
 * The diag control register bit settings are to prohibit any ucode from
 * loading (via ROM) or running, and to allow access by this test to the WCS
 * chips.
 */
	    dbus_prot_wr(glbl->Slot, &dgbus->diag_control, WCS_DBUS_CMD_ENABLE);
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    dbus_prot_wr(glbl->Slot, &dgbus->diag_status, 0);		/* Reset */
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    end_buserr_count_region();
	    
	    write_WCS_register(dg_errors, glbl->Slot, glbl->verbose,
			       SPC_REG_INITIALIZE_COUNTER, 0);
/*
 * Direct the WCS Controller to forward the next one bit per chip to the WCS's
 * data register.  This is a dummy operation to force a Command to Data
 * transition to the SPC which will make it execute the previous command.
 */
	    start_buserr_count_region();
	    dgbus->ucode_command_and_data =
		WRITE_WCS_SPC_DATA_REGISTER << 4 | 0;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    dgbus->ucode_command_and_data = 0x69;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    end_buserr_count_region();
	    for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS;
		 indx += MAX_WCS_CHIPS)
		read_WCS(dg_errors, glbl->Slot, glbl->verbose,
			 &WCS_results[indx]);

	    glbl->Total_Error_Count = 0;
	    	    
	    for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS;
		 indx += MAX_WCS_CHIPS) {
		
		temp_count = 0;
		for (jndx = MAX_WCS_CHIPS - glbl->num_WCS_chips[glbl->Slot];
		     jndx < MAX_WCS_CHIPS; jndx++)
		    for (kndx = 0; kndx < 16; kndx++)
			if (WCS_results[indx + jndx] & 1 << kndx)
			    temp_count++;
	    
		if (glbl->verbose == 'y' && ((glbl->all_rows == 'y') ||
				   temp_count & 1)) {
		    check_flow_control();
		    check_more();
		    printf("Slot %2u WCS row: %u  ", glbl->Slot,
			   indx / MAX_WCS_CHIPS);
		    for (jndx = MAX_WCS_CHIPS -
			 glbl->num_WCS_chips[glbl->Slot];
			 jndx < MAX_WCS_CHIPS; jndx++)
			printf("%4x ", WCS_results[indx + jndx]);
		    printf("parity is %s (%u ones)\n",
			   temp_count & 1 ? "ODD" : "even", temp_count);
		}

		if (temp_count & 1)
		    update_dg_errors_8(&dg_errors[glbl->Slot].
				       WCS_parity_detection, 0, 0);
		if (abort) {
		    glbl->temp_abort = abort;
		    break;
		}
	    }
	    printf("\n%u rows, total WCS parity errors: %u total "
		   "bus errors: %u\n",
		   indx / MAX_WCS_CHIPS,
		   glbl->Total_Error_Count,
		   glbl->Total_Bus_Error_Count);
	    if (glbl->temp_abort) {
		abort = glbl->temp_abort = FALSE;
		printf("\nAborted\n");
		break;
	    }
	    break;

	case 'F':

	    last_clock = clock;
	    temp = 0;
	    for (;;) {
		fast_write_read_compare(dg_errors, glbl->Slot, glbl->verbose,
					WCS_data, WCS_results, 3);
		printf("\rpass: %u, time: %u", temp, clock - last_clock);
		last_clock = clock;
		temp++;
		if (abort) {
		    abort = FALSE;
		    break;
		}
	    }
	    break;

	case 'N':	/* Initialize global variables */
	    init_hi_mem_globals();
	    break;

        case '?':
	case 'H':		/* help */
	    diag_help();
	    break;
	    
	case 'Q':		/* Quit */
	    abortstate = old_abortstate;
	    return;
	    break;
	    

	case 'L':		/* List I/O map */
	    temp = glbl->Total_Error_Count;
	    for (indx = 0; indx < NSLOTS; indx++) {
		iomap_slot(dg_errors, indx, 'y');
		if (abort) {
		    abort = FALSE;
		    printf("\nAborted\n");
		    break;
		}
	    }
	    break;


	default:
	Huh:
	    puts("What?\n");
	    break;
	}
	
        flush();	/* Make sure that the UART is drained */
    }
}

diag_help ()
{
    printf("a            All slots display\n");
    printf("o            One slot display\n");
    printf("w            Walking tests on all slots\n");
    printf("k            Walking tests on a specified slot\n");
    printf("i            Infinite passes of all dBus tests on selected slots\n");
    printf("e            Error array display\n");

    printf("p            Primitive Slot access\n");
    printf("s            Scope loop\n");

    printf("c            Writeable Control store tests\n");
    printf("r            ROM read tests\n");
    printf("m            Move ROM to WCS and verify parity\n");

    printf("n            INitialize global variables\n");

    printf("q            Quit and return to higher level interpreter\n");
    printf("h or ?       Help for commands\n");
    printf("l            List I/O map of card cage\n");
    printf("d            Dump registers for one slot\n");
}


disp_slot (dg_base, slot, verbose)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
{
    dgtype *dgbus;
    u_char slot_and_complement;
    u_char CxBus_number;
    u_char temp_b;

    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
    
    if (!touch_device(dgbus)) {
	if (verbose == 'y') {
	    check_flow_control();
	    printf("No response from slot %u\n",slot);
	}
	update_dg_errors_8(&dg_base[slot].no_response, 0, 0);
    }
    start_buserr_count_region();

    slot_and_complement = dgbus->slot_and_comp;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    if ((slot != (slot_and_complement >> 4 & 0xf)) ||
	((slot_and_complement >> 4 & 0xf) != (~slot_and_complement & 0xf))) {
	if (verbose == 'y') {
	    check_flow_control();
	    printf("Slot %2u reported %2x (Hex) for its slot and complement\n",
		   slot, slot_and_complement);
	}
	update_dg_errors_8(&dg_base[slot].invalid_slot_and_complement,
			 slot_and_complement, (slot << 4 | (~slot & 0xf)));
    }
    temp_b = dgbus->CxBus_and_logical_slot;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    CxBus_number = temp_b >> 4 & 0x3;
    if (CxBus_number != ASSUMED_CXBUS_NUMBER) {
	if (verbose == 'y') {
	    check_flow_control();
	    printf("Slot %2u reported CxBus number %u in error\n",slot,
		   CxBus_number);
	}
	update_dg_errors_8(&dg_base[slot].invalid_CxBus_number,
			 CxBus_number, ASSUMED_CXBUS_NUMBER);
    }
    if (verbose == 'y') {
	check_flow_control();
	printf("Slot %2u reported Logical slot %2u\n",slot, temp_b & 0xf);
    }	
    dgbus->CxBus_and_logical_slot = slot;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    temp_b = dgbus->CxBus_and_logical_slot;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    if ((temp_b & 0xf) != slot) {
	if (verbose == 'y') {
	    check_flow_control();
	    printf("Slot %2u reported Logical slot %2u expected %u\n",
		   slot, temp_b & 0xf, slot);
	}
	update_dg_errors_8(&dg_base[slot].invalid_Logical_Slot,
			   temp_b & 0xf, slot);
    }
/*
 * The last command address is not writable, but the request to do so will be
 * recorded in it.
 */
    dbus_prot_wr(slot, &dgbus->last_command_or_address, 0);
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    temp_b = dbus_prot_rd(&dgbus->last_command_or_address);
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    if (temp_b & DBUS_READ) {
	if (verbose == 'y') {
	    check_flow_control();
	    printf("Slot %2u reported Read when operation was a Write\n",slot);
	}
	update_dg_errors_8(&dg_base[slot].dBus_Read_status,
			   temp_b & DBUS_READ, 0);
    }
    temp_b = dbus_prot_rd(&dgbus->last_command_or_address);
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    if (!(temp_b & DBUS_READ)) {
	if (verbose == 'y') {
	    check_flow_control();
	    printf("Slot %2u reported Write when operation was a Read\n",slot);
	}
	update_dg_errors_8(&dg_base[slot].dBus_Read_status,
			   temp_b & DBUS_READ, DBUS_READ);
    }
/*
 * Toggle the card's LED.
 */
    temp_b = dbus_prot_rd(&dgbus->diag_control);
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    dbus_prot_wr(slot, &dgbus->diag_control, temp_b ^ ENABLE_LED);
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    dbus_prot_wr(slot, &dgbus->diag_status, 0);			/* Reset */
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    end_buserr_count_region();
}

/*
** Dump slot registers for particular slot on dBus
**
** jac, 5/13/92
*/

dump_reg_slot (dg_base, slot, verbose)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
{
    dgtype *dgbus;
    u_char slot_and_complement;
    u_char temp_b;
    u_char reg_num;
    u_char reg_offset;

    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);

    if (!touch_device(dgbus)) {
	check_flow_control();
	printf("\nSlot %u:  No response\n", slot);
	update_dg_errors_8(&dg_base[slot].no_response, 0, 0);
    }
    start_buserr_count_region();

    for (reg_num = 0; reg_num < 64; reg_num += 8) {
	check_flow_control();
	printf("\nSlot %u, register 0x%-02x:",slot, reg_num);  /* bogus - */
        for (reg_offset = 0; reg_offset < 8; reg_offset++) {
            start_buserr_count_region();
	    temp_b = *(u_char *)((u_long)dgbus | (reg_num + reg_offset));
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
            buserrhandler = 0;
	    check_flow_control();
	    printf("  %-02x",temp_b);  /* bogus - for bogus printf */
            if (reg_offset == 3) {
                printf("  ");
            }
        }
        if (reg_num == 24) {
            printf("\n");
        }
    }
    printf("\n");

    end_buserr_count_region();
}


/*
** I/O map a particular slot on dBus
**
** jac, 5/12/92
*/

iomap_slot (dg_base, slot, verbose)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
{
    dgtype *dgbus;
    u_char slot_and_complement;
    u_char temp_b;

    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);

    if (!touch_device(dgbus)) {
/*	check_flow_control();
	printf("Slot %u:  No response\n",slot);
	update_dg_errors_8(&dg_base[slot].no_response, 0, 0); */
        return;
    }
    start_buserr_count_region();

    slot_and_complement = dgbus->slot_and_comp;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    if ((slot != (slot_and_complement >> 4 & 0xf)) ||
	((slot_and_complement >> 4 & 0xf) != (~slot_and_complement & 0xf))) {
	check_flow_control();
	printf("*** Slot %u reported slot and complement of 0x%2x\n",
		   slot, slot_and_complement);
	update_dg_errors_8(&dg_base[slot].invalid_slot_and_complement,
			 slot_and_complement, (slot << 4 | (~slot & 0xf)));
    }
    check_flow_control();
    printf("Slot %u:  ", slot);

    temp_b = dgbus->eeprom.controller_type;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    check_flow_control();
    printf("Type = %u, ", temp_b);

    switch (temp_b) {
    case 0:
        printf("Ethernet Interface Processor");
        break;
    case 2:
        printf("SwBus Serial Processor");
        break;
    case 3:
        printf("HSSI Interface Processor");
        break;
    case 5:
        printf("Token Ring Interface Processor");
        break;
    case 6:
        printf("FDDI-T Interface Processor");
        break;
    case 7:
        printf("Serial Interface Processor");
        break;
    case 11:
        printf("Switch Processor");
        break;
    default:
        printf("Unknown interface");
        break;
    }

    temp_b = dgbus->eeprom.hardware_version_high;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    check_flow_control();
    printf("\n              Hardware version = %u.", temp_b);

    temp_b = dgbus->eeprom.hardware_version_low;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    check_flow_control();
    printf("%u, ", temp_b);

    temp_b = dgbus->eeprom.flags;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    check_flow_control();
    printf("Vendor flags = 0x%x\n", temp_b);

    end_buserr_count_region();
}

walk_slot (dg_base, slot, verbose)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
{
    dgtype *dgbus;
    u_char slot_and_complement;
    u_char CxBus_number;

    u_char walking_bit;

    short jndx;
    short kndx;
    volatile u_char temp_b;
/*
 * To verify correct transmission, reception, and decoding of DBUS_ADRS[9:6]
 * (physical slot number) read the Physical Slot Number Register verifying
 * both the true and complement values.  This also ensures that one and only
 * one card responds to a given slot number.
 */
    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
    if (!touch_device(dgbus)) {
	if (verbose == 'y') {
	    check_flow_control();
	    printf("No response from slot %u\n",slot);
	}
	update_dg_errors_8(&dg_base[slot].no_response, 0, 0);
    }
    start_buserr_count_region();

    slot_and_complement = dgbus->slot_and_comp;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    if ((slot != (slot_and_complement >> 4 & 0xf)) ||
	((slot_and_complement >> 4 & 0xf) != (~slot_and_complement & 0xf))) {
	if (verbose == 'y') {
	    check_flow_control();
	    printf("Slot %2u reported %2x (Hex) for its slot and complement\n",
		   slot, slot_and_complement);
	}
	update_dg_errors_8(&dg_base[slot].invalid_slot_and_complement,
			 slot_and_complement, (slot << 4 | (~slot & 0xf)));
    }
/*
 * To verify correct transmission and reception of DBUS_READ and
 * DBUS_ADRS[5:0] walk a one and then a zero through DBUS_ADRS[5:0] while
 * reading and then reading the Last Command/Address Register.
 */
    for (jndx = 1; jndx >= 0; jndx--) {
	for (kndx = 0; kndx < N_LOW_ADDRESS_BITS; kndx++) {
	    walking_bit = 1 << kndx;
	    if (!(jndx & 1))
		walking_bit = ~walking_bit;
	    walking_bit &= 0x3f;
            flush();	/* Make sure that the UART is drained */
            start_buserr_count_region();
	    temp_b = *(u_char *)((u_long)dgbus | (u_long)walking_bit);
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    temp_b = dbus_prot_rd(&dgbus->last_command_or_address);
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
            buserrhandler = 0;
	    temp_b &= ~0x40;		/* Clear Read/Write bit */
	    if (temp_b != walking_bit) {
		if (verbose == 'y') {
                    flush();	/* Make sure that the UART is drained */
		    check_flow_control();
		    printf("Slot %2u actual: %2x expected: %2x while "
			   "verifying DBUS_ADRS[5:0]\n", slot,
			   temp_b, walking_bit);
                    flush();	/* Make sure that the UART is drained */
		}
		if (jndx & 1)
		    update_dg_errors_8(&dg_base[slot].
				     dbus_adrs_5_0_walking_one_errors,
				     temp_b, walking_bit);
		else
		    update_dg_errors_8(&dg_base[slot].
				     dbus_adrs_5_0_walking_zero_errors,
				     temp_b, walking_bit);
	    }
	}
    }
/*
 * To verify correct operation of DBUS_DATA[7:0] walk a one then a zero through
 * the Microcode Command/Data Register reading back the result of each write.
 *
 * The diag control register bit settings are to prohibit any ucode from
 * loading or running, and to prohibit any results of this test's changes to
 * the ucode command and data register to effect the WCS chips.
 */
    start_buserr_count_region();
    dbus_prot_wr(slot, &dgbus->diag_control, WCS_LOADED);
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop"); 
    asm("	nop");
    asm("	nop");
    asm("	nop");

    buserrhandler = 0;
    for (jndx = 1; jndx >= 0; jndx--) {
	for (kndx = 0; kndx < 8; kndx++) {
	    walking_bit = 1 << kndx;
	    if (!(jndx & 1))
		walking_bit = ~walking_bit;
            start_buserr_count_region();
	    dgbus->ucode_command_and_data = walking_bit;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop"); 
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    temp_b = dgbus->ucode_command_and_data;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop"); 
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    buserrhandler = 0;
	    if (temp_b != walking_bit) {
		if (verbose == 'y') {
		    flush();	/* Make sure that the UART is drained */
		    check_flow_control();
		    printf("Slot %2u actual: %2x expected: %2x while"
			   " verifying DBUS_DATA[7:0]\n", slot,
			   temp_b, walking_bit);
		    flush();	/* Make sure that the UART is drained */
		}
		if (jndx & 1)
		    update_dg_errors_8(&dg_base[slot].
				     dbus_data_7_0_walking_one_errors,
				     temp_b, walking_bit);
		else
		    update_dg_errors_8(&dg_base[slot].
				     dbus_data_7_0_walking_zero_errors,
				     temp_b, walking_bit);
	    }
	}
    }
    end_buserr_count_region();
}

WCS_tests(dg_base, slot, verbose, WCS_data, WCS_results)
    dg_error dg_base[][N_DG_ERRORS];
    u_char slot;
    char verbose;
    u_short *WCS_data;
    u_short *WCS_results;
{
    hi_mem_globals *glbl;
    dgtype *dgbus;
    u_char temp_test_ID;
    u_char temp_b;
    u_char temp_b_1;
    long indx;
    long jndx;
    long kndx;
    u_long temp;
    u_long temp_1;
    u_long temp_count;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;
    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
    temp_test_ID = glbl->first_WCS_test_ID;
/*
 * Memory test: are zeros valid in all locations?
 */
    if (glbl->WCS_tests_vector & 1 << temp_test_ID) {
	for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS; indx++)
	    WCS_data[indx] = 0;
	write_read_compare(dg_base, slot, verbose, WCS_data, WCS_results,
			   temp_test_ID);
	if (glbl->temp_abort)
	    return;
    }
/*
 * Memory test: are ones valid in all locations?
 */
    temp_test_ID++;
    if (glbl->WCS_tests_vector & 1 << temp_test_ID) {
	for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS; indx++)
	    WCS_data[indx] = 0xffff;
	write_read_compare(dg_base, slot, verbose, WCS_data, WCS_results,
			   temp_test_ID);
	if (glbl->temp_abort)
	    return;
    }
/*
 * Memory test: is addressing correct?
 */
    temp_test_ID++;
    if (glbl->WCS_tests_vector & 1 << temp_test_ID) {
	for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS; indx++)
	    WCS_data[indx] = (u_short)indx;
	write_read_compare(dg_base, slot, verbose, WCS_data, WCS_results,
			   temp_test_ID);
	if (glbl->temp_abort)
	    return;
    }
/*
 * Memory test: is complement of addressing correct?
 */
    temp_test_ID++;
    if (glbl->WCS_tests_vector & 1 << temp_test_ID) {
	for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS; indx++)
	    WCS_data[indx] = ~(u_short)indx;
	write_read_compare(dg_base, slot, verbose, WCS_data, WCS_results,
			   temp_test_ID);
	if (glbl->temp_abort)
	    return;
    }
/*
 * Diag bus test: while it may not look like it, this pattern sends zeros then
 * ones in alternating bytes.
 */
    temp_test_ID++;
    if (glbl->WCS_tests_vector & 1 << temp_test_ID) {
	for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS; indx++)
	    WCS_data[indx] = 0xaaaa;
	write_read_compare(dg_base, slot, verbose, WCS_data, WCS_results,
			   temp_test_ID);
	if (glbl->temp_abort)
	    return;
    }
/*
 * Diag bus test: while it may not look like it, this pattern sends 0xaa then
 * 0x55 in alternating bytes.
 */
    temp_test_ID++;
    if (glbl->WCS_tests_vector & 1 << temp_test_ID) {
	for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS; indx += 2) {
	    WCS_data[indx] = 0x5555;
	    WCS_data[indx + 1] = 0xaaaa;
	}
	write_read_compare(dg_base, slot, verbose, WCS_data, WCS_results,
			   temp_test_ID);
	if (glbl->temp_abort)
	    return;
    }
/*
 * Diag bus test: while it may not look like it, this pattern walks a one then
 * a zero from low order to high order on the bus.
 */
    temp_test_ID++;
    if (glbl->WCS_tests_vector & 1 << temp_test_ID) {
	for (indx = 0; indx < glbl->WCS_words * MAX_WCS_CHIPS; indx += 8) {
	    WCS_data[indx] = 0x7f80;
	    WCS_data[indx + 1] = 0xbf40;
	    WCS_data[indx + 2] = 0xdf20;
	    WCS_data[indx + 3] = 0xef10;
	    WCS_data[indx + 4] = 0xf708;
	    WCS_data[indx + 5] = 0xfb04;
	    WCS_data[indx + 6] = 0xfd02;
	    WCS_data[indx + 7] = 0xfe01;
	}
	write_read_compare(dg_base, slot, verbose, WCS_data, WCS_results,
			   temp_test_ID);
	if (glbl->temp_abort)
	    return;
    }
/*
 * Memory parity detection test.  Walks a one then a zero through the first
 * word (max width 128 bits) of the WCS.  At each iteration releases the
 * microprocessor to run and ensures a parity error is detected.
 */
    temp_test_ID++;
    if (glbl->WCS_tests_vector & 1 << temp_test_ID) {
/*
 * Initialize 128 bits to zeros (second pass ones).  This vector has each bit
 * toggled (and toggled again) to generate the walking patterns.
 */
	for (indx = 0; indx < 2; indx++) {
	    for (jndx = MAX_WCS_CHIPS - 1; jndx >= 0; jndx--) {
		WCS_data[jndx] = indx ? 0xffff : 0;
		WCS_data[(4094 * MAX_WCS_CHIPS) + jndx] = indx ? 0xffff : 0;
	    }
	    for (jndx = 0; jndx < glbl->num_WCS_chips[slot] * N_WCS_DATA_BITS;
		 jndx++) {
		WCS_data[MAX_WCS_CHIPS - 1 - (jndx >> 4)] ^= 1 << (jndx & 0xf);
		WCS_data[(4094 * MAX_WCS_CHIPS) + MAX_WCS_CHIPS - 1 -
			 (jndx >> 4)] ^= 1 << (jndx & 0xf);

		start_buserr_count_region();
/*
 * The diag control register bit settings are to prohibit any ucode from
 * loading (via ROM) or running, and to allow access by this test to the WCS
 * chips.
 */
		dbus_prot_wr(slot, &dgbus->diag_control, WCS_DBUS_CMD_ENABLE);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		dbus_prot_wr(slot, &dgbus->diag_status, 0);		/* Reset */
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		temp_b = dbus_prot_rd(&dgbus->diag_status);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		end_buserr_count_region();

		if (temp_b & WCS_PARITY_ERROR) {
		    if (verbose == 'y') {
			check_flow_control();
			check_more();
			printf("Slot %2u failed: %u  parity flag on after"
			       " a reset\n", slot, jndx);
		    }
		    update_dg_errors_8(&dg_base[slot][temp_test_ID], 0, 0);
		}
		temp = glbl->WCS_words;
		glbl->WCS_words = 1;
		write_read_compare(dg_base, slot, verbose, WCS_data,
				   WCS_results, temp_test_ID);
/*
 * And for the EIP which starts executing at 0xfff.
 */
		temp_1 = glbl->WCS_starting_row;
		glbl->WCS_starting_row = 4094;
		glbl->WCS_words = 4095;
		write_read_compare(dg_base, slot, verbose, WCS_data,
				   WCS_results, temp_test_ID);
		glbl->WCS_starting_row = temp_1;
		glbl->WCS_words = temp;

		write_WCS_register(dg_base, slot, verbose,
				   SPC_REG_INITIALIZE_COUNTER, 0);
/*
 * Direct the WCS Controller to forward the next one bit per chip to the WCS's
 * data register.  This is a dummy operation to force a Command to Data
 * transition to the SPC which will make it execute the previous command.
 */
		start_buserr_count_region();
		dgbus->ucode_command_and_data =
		    WRITE_WCS_SPC_DATA_REGISTER << 4 | 0;
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		dgbus->ucode_command_and_data = 0x69;
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
/*
 * The diag control register bit settings are to release the ucode to run.
 */
		dbus_prot_wr(slot, &dgbus->diag_control, MASTER_ENABLE | WCS_LOADED);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		temp_b = dbus_prot_rd(&dgbus->diag_status);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		end_buserr_count_region();

		if (!(temp_b & WCS_PARITY_ERROR)) {
		    if (verbose == 'y') {
			check_flow_control();
			check_more();
			printf("Slot %2u failed: %u  ", slot, jndx);
			for (kndx = MAX_WCS_CHIPS - glbl->num_WCS_chips[slot];
			     kndx < MAX_WCS_CHIPS; kndx++)
			    printf("%4x ", WCS_data[kndx]);
			printf("%s\n", WCS_test_descriptions[temp_test_ID]);
		    }
		    update_dg_errors_8(&dg_base[slot][temp_test_ID], 0, 0);
		}
		WCS_data[MAX_WCS_CHIPS - 1 - (jndx >> 4)] ^= 1 << (jndx & 0xf);
		WCS_data[(4094 * MAX_WCS_CHIPS) + MAX_WCS_CHIPS - 1 -
			 (jndx >> 4)] ^= 1 << (jndx & 0xf);

		if (abort) {
		    glbl->temp_abort = TRUE;
		    return;
		}
	    }
	}
    }
/*
 * WCS controller instruction error test.  Sends invalid instructions to the 
 * controller and verifies the error reports.
 */
    temp_test_ID++;
    if (glbl->WCS_tests_vector & 1 << temp_test_ID) {

	for (indx = 0; indx < 16; indx++)
	    if (indx != 6)			/* Six is invalid but the
						  hardware doesn't detect it */
	    if (!(glbl->WCS_valid_opcode_vector & 1 << indx)) {

		start_buserr_count_region();
/*
 * The diag control register bit settings are to prohibit any ucode from
 * loading (via ROM) or running, and to allow access by this test to the WCS
 * chips.
 */
		dbus_prot_wr(slot, &dgbus->diag_control, WCS_DBUS_CMD_ENABLE);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		dbus_prot_wr(slot, &dgbus->diag_status, 0);		/* Reset */
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		temp_b = dbus_prot_rd(&dgbus->diag_status);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		end_buserr_count_region();

		if (temp_b & WCS_CNTRL_INSTR_ERROR) {
		    if (verbose == 'y') {
			check_flow_control();
			check_more();
			printf("Slot %2u failed: %u  WCS cntrl flag on "
			       " after a reset\n", slot, indx);
		    }
		    update_dg_errors_8(&dg_base[slot][temp_test_ID], 0, 0);
		}
		start_buserr_count_region();
/*
 * Write the invalid opcode.
 */
		dgbus->ucode_command_and_data = indx << 4;
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		temp_b = dbus_prot_rd(&dgbus->diag_status);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		end_buserr_count_region();

		if (!(temp_b & WCS_CNTRL_INSTR_ERROR)) {
		    if (verbose == 'y') {
			check_flow_control();
			check_more();
			printf("Slot %2u failed: %u  ", slot, indx);
			printf("%s\n", WCS_test_descriptions[temp_test_ID]);
		    }
		    update_dg_errors_8(&dg_base[slot][temp_test_ID], 0, 0);
		}
		start_buserr_count_region();
/*	
 * Send two more (valid) commands then check for error indicator's presence.
 * And that one not two buserrors occured.
 */
		dgbus->ucode_command_and_data =
		    SET_CONTROL_REGISTER_WCS_LOADED;
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		dgbus->ucode_command_and_data =
		    SET_CONTROL_REGISTER_WCS_LOADED;
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		temp_b_1  = dbus_prot_rd(&dgbus->diag_status);
		asm("	nop");
		asm("	nop");
		asm("	nop");
		asm("	nop");
		end_buserr_count_region();
	    
		if (glbl->long_buserrcnt != 1) {
		    if (verbose == 'y') {
			check_flow_control();
			check_more();
			printf("Slot %2u failed: %u  WCS cntrl instr "
			       "incorrect bus error behavior\n", slot, indx);
		    }
		    update_dg_errors_8(&dg_base[slot][temp_test_ID], 0, 0);
		} else {
/*
 * Remove the intentional bus error from the error count.
 */
		    glbl->Total_Bus_Error_Count--;
		}
		if (temp_b & WCS_CNTRL_INSTR_ERROR != temp_b_1 &
		    WCS_CNTRL_INSTR_ERROR) {
		    if (verbose == 'y') {
			check_flow_control();
			check_more();
			printf("Slot %2u failed: %u  WCS cntrl instr "
			       "error flag inconsistent\n", slot, indx);
		    }
		    update_dg_errors_8(&dg_base[slot][temp_test_ID], 0, 0);
		}
		if (abort) {
		    glbl->temp_abort = TRUE;
		    return;
		}
	    }
    }	
}

write_read_compare(dg_base, slot, verbose, WCS_data, WCS_results, err_ndx)
    dg_error dg_base[][N_DG_ERRORS];
    u_char slot;
    char verbose;
    u_short *WCS_data;
    u_short *WCS_results;
    u_char err_ndx;
{
    dgtype *dgbus;

    u_long temp;
    u_long temp_count;
    u_short indx;
    u_short jndx;
    hi_mem_globals *glbl;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
    start_buserr_count_region();
/*
 * The diag control register bit settings are to prohibit any ucode from
 * loading (via ROM) or running, and to allow access by this test to the WCS
 * chips.
 */
    dbus_prot_wr(slot, &dgbus->diag_control, WCS_DBUS_CMD_ENABLE);
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    dbus_prot_wr(slot, &dgbus->diag_status, 0);		/* Reset */
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    end_buserr_count_region();

    write_WCS_register(dg_base, slot, verbose, SPC_REG_INITIALIZE_COUNTER,
		       glbl->WCS_starting_row);

    for (indx = glbl->WCS_starting_row * MAX_WCS_CHIPS;
	 indx < glbl->WCS_words * MAX_WCS_CHIPS; indx += MAX_WCS_CHIPS)
	write_WCS(dg_base, slot, verbose, &WCS_data[indx]);

    write_WCS_register(dg_base, slot, verbose, SPC_REG_INITIALIZE_COUNTER,
		       glbl->WCS_starting_row);

/*
 * Direct the WCS Controller to forward the next one bit per chip to the WCS's
 * data register.  This is a dummy operation to force a Command to Data
 * transition to the SPC which will make it execute the previous command.
 */
    start_buserr_count_region();
    dgbus->ucode_command_and_data = WRITE_WCS_SPC_DATA_REGISTER << 4 | 0;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    dgbus->ucode_command_and_data = 0x69;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    end_buserr_count_region();
    for (indx = glbl->WCS_starting_row * MAX_WCS_CHIPS;
	 indx < glbl->WCS_words * MAX_WCS_CHIPS; indx += MAX_WCS_CHIPS)
	read_WCS(dg_base, slot, verbose, &WCS_results[indx]);

    temp_count = dg_base[slot][err_ndx].err_count;

    for (indx = glbl->WCS_starting_row * MAX_WCS_CHIPS;
	 indx < glbl->WCS_words * MAX_WCS_CHIPS; indx += MAX_WCS_CHIPS) {
	for (jndx = MAX_WCS_CHIPS - glbl->num_WCS_chips[slot]; jndx <
	     MAX_WCS_CHIPS; jndx++)
	    if (WCS_data[indx + jndx] != WCS_results[indx + jndx]) {
		if (verbose == 'y') {
		    check_flow_control();
		    check_more();
		    printf("Slot %2u actual: %4x expected: %4x test: %s\n", 
			   slot, WCS_results[indx + jndx], 
			   WCS_data[indx + jndx],
			   WCS_test_descriptions[err_ndx]);
		}
		update_dg_errors_16(&dg_base[slot][err_ndx],
				    WCS_results[indx + jndx],	
				    WCS_data[indx + jndx]);
	    }
	if (abort) {
	    glbl->temp_abort = TRUE;
	    return;
	}
    }
    if (verbose == 'n') {
	if (temp_count != dg_base[slot][err_ndx].err_count) {
	    check_flow_control();
	    
	    temp = 0;
	    for (indx = glbl->first_WCS_test_ID;
		 indx <= glbl->last_WCS_test_ID; indx++)
		temp += dg_base[slot][indx].err_count;

	    printf("pass %u  pass errs %u  this test %u  Total %u %s\n",
		   glbl->actual_iterations, 
		   dg_base[slot][err_ndx].err_count - temp_count,
		   dg_base[slot][err_ndx].err_count,
		   temp,
		   WCS_test_descriptions[err_ndx]);
	}
    }
}



fast_write_read_compare(dg_base, slot, verbose, WCS_data, WCS_results, err_ndx)
    dg_error dg_base[][N_DG_ERRORS];
    u_char slot;
    char verbose;
    u_short *WCS_data;
    u_short *WCS_results;
    u_char err_ndx;
{
    dgtype *dgbus;

    u_long temp;
    u_long temp_count;
    u_short indx;
    u_short jndx;
    hi_mem_globals *glbl;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
/*
 * The diag control register bit settings are to prohibit any ucode from
 * loading (via ROM) or running, and to allow access by this test to the WCS
 * chips.
 */
    dbus_prot_wr(slot, &dgbus->diag_control, WCS_DBUS_CMD_ENABLE);

    write_WCS_register(dg_base, slot, verbose, SPC_REG_INITIALIZE_COUNTER, 0);

    fast_write_WCS(dg_base, slot, verbose, WCS_data);
}

/*
 * This routine writes 128 bits of data to the Writable Control Store via the
 * Diag bus (a.k.a. dbus).  The address of the data to write is presumed to be
 * on a 16 bit boundary.  The current location in the WCS is written and the 
 * initialization register incremented.
 */

fast_write_WCS (dg_base, slot, verbose, WCS_data)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
    u_char *WCS_data;
{
    dgtype *dgbus;

    short indx;
    short jndx;
    u_char temp_b;
    u_char temp_vctr[16];

    temp_b = 0;
    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
/*
 * Direct the WCS Controller to forward the next sixteen bits per chip to the
 * WCS's data register.
 */
    for (indx = 0; indx < 4096; indx++) {

	dgbus->ucode_command_and_data = WRITE_WCS_SPC_DATA_REGISTER << 4 | 4;
/*
 * This separate loop is intended to get the bytes over the bus faster to
 * stress the bus drivers.  Both removing the "nop"s (for bus error traps) and
 * linearizing the loop would make it go faster -- among other things.
 */
	dgbus->ucode_command_and_data = WCS_data[0];
	dgbus->ucode_command_and_data = WCS_data[1];
	dgbus->ucode_command_and_data = WCS_data[2];
	dgbus->ucode_command_and_data = WCS_data[3];
	dgbus->ucode_command_and_data = WCS_data[4];
	dgbus->ucode_command_and_data = WCS_data[5];
	dgbus->ucode_command_and_data = WCS_data[6];
	dgbus->ucode_command_and_data = WCS_data[7];
	dgbus->ucode_command_and_data = WCS_data[8];
	dgbus->ucode_command_and_data = WCS_data[9];
	dgbus->ucode_command_and_data = WCS_data[10];
	dgbus->ucode_command_and_data = WCS_data[11];
	dgbus->ucode_command_and_data = WCS_data[12];
	dgbus->ucode_command_and_data = WCS_data[13];
	dgbus->ucode_command_and_data = WCS_data[14];
	dgbus->ucode_command_and_data = WCS_data[15];
/*	
 * Direct the WCS Controller to forward the next eight bits per chip to the
 * WCS's command register.
 */
	dgbus->ucode_command_and_data =
	    WRITE_WCS_SPC_COMMAND_REGISTER << 4 | 3;

	dgbus->ucode_command_and_data = 0xff;
	dgbus->ucode_command_and_data = 0;
	dgbus->ucode_command_and_data = 0;
	dgbus->ucode_command_and_data = 0;

	dgbus->ucode_command_and_data = 0xff;
	dgbus->ucode_command_and_data = 0xff;
	dgbus->ucode_command_and_data = 0;
	dgbus->ucode_command_and_data = 0;
    }
}


write_WCS (dg_base, slot, verbose, WCS_data)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
    u_short *WCS_data;
{
    dgtype *dgbus;

    short indx;
    short jndx;
    u_char temp_b;
    u_char temp_vctr[16];

    temp_b = 0;
    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
    
    start_buserr_count_region();
/*
 * Direct the WCS Controller to forward the next sixteen bits per chip to the
 * WCS's data register.
 */
    dgbus->ucode_command_and_data = WRITE_WCS_SPC_DATA_REGISTER << 4 | 4;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    for (indx = 0; indx < N_WCS_DATA_BITS; indx++)
	for (jndx = 0; jndx < MAX_WCS_CHIPS; jndx++) {
	    temp_vctr[indx] = temp_vctr[indx] << 1;
	    temp_vctr[indx] |= 1 & (WCS_data[jndx] >> indx);
	}
/*
 * This separate loop is intended to get the bytes over the bus faster to
 * stress the bus drivers.  Both removing the "nop"s (for bus error traps) and
 * linearizing the loop would make it go faster -- among other things.
 */
    for (indx = 0; indx < N_WCS_DATA_BITS; indx++) {
	dgbus->ucode_command_and_data = temp_vctr[indx];
	asm("	nop");
	asm("	nop");
	asm("	nop");
 	asm("	nop");
    }
/*
 * Direct the WCS Controller to forward the next eight bits per chip to the
 * WCS's command register.
 */
    dgbus->ucode_command_and_data = WRITE_WCS_SPC_COMMAND_REGISTER << 4 | 3;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    temp_b = SPC_WRITE_REGISTER_AND_INCREMENT_INITIALIZE_COUNTER << 4 |
	SPC_REG_RAM_OUTPUT;
    for (indx = 0; indx < N_WCS_COMMAND_BITS; indx++) {
	if (temp_b & 1) {
	    dgbus->ucode_command_and_data = 0xff;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	} else {
	    dgbus->ucode_command_and_data = 0;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	}
	temp_b = temp_b >> 1;
    }
    end_buserr_count_region();
}


/*
 * This routine reads 128 bits of data from the Writable Control Store via the
 * Diag bus (a.k.a. dbus).  The address of where to place the data to be read
 * is presumed to be on a 16 bit boundary.  The current location in the WCS is
 * read and the initialization register incremented.
 */

read_WCS (dg_base, slot, verbose, WCS_data)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
    u_short *WCS_data;
{
    dgtype *dgbus;

    short indx;
    short jndx;
    u_char temp_b;
    u_char temp_vctr[16];

    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
    
    start_buserr_count_region();
/*
 * Direct the WCS Controller to forward the next eight bits per chip to the
 * WCS's command register.
 */
    dgbus->ucode_command_and_data = WRITE_WCS_SPC_COMMAND_REGISTER << 4 | 3;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    temp_b = SPC_READ_REGISTER_AND_INCREMENT_INITIALIZE_COUNTER << 4 |
	SPC_REG_RAM_OUTPUT;
    for (indx = 0; indx < N_WCS_COMMAND_BITS; indx++) {
	if (temp_b & 1) {
	    dgbus->ucode_command_and_data = 0xff;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	} else {
	    dgbus->ucode_command_and_data = 0;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	}
	temp_b = temp_b >> 1;
    }
/*
 * Direct the WCS Controller to forward the next sixteen bits per chip from the
 * WCS's data register to the RP via the Diag bus.
 */
    dgbus->ucode_command_and_data = READ_WCS_SPC_DATA_REGISTER << 4 | 4;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    for (indx = 0; indx < N_WCS_DATA_BITS; indx++) {
	temp_vctr[indx] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
    }
    for (indx = 0; indx < N_WCS_DATA_BITS; indx++) {
	for (jndx = 0; jndx < MAX_WCS_CHIPS; jndx++)
	    WCS_data[jndx] = WCS_data[jndx] >> 1 |
		(u_short)temp_vctr[indx] << jndx + 8 &
		    1 << N_WCS_DATA_BITS - 1;
    }
    end_buserr_count_region();
}



/*
 * This routine writes 16 bits of data to a specified register of all the
 * present Writable Control Store (WCS) chips via the Diag bus (a.k.a. dbus).
 */

write_WCS_register (dg_base, slot, verbose, WCS_register, WCS_data)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
    u_char WCS_register;
    u_short WCS_data;
{
    dgtype *dgbus;

    short indx;
    short jndx;
    u_char temp_b;

    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
    
    start_buserr_count_region();
/*
 * Direct the WCS Controller to forward the next sixteen bits per chip to the
 * WCS's data register.
 */
    dgbus->ucode_command_and_data = WRITE_WCS_SPC_DATA_REGISTER << 4 | 4;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    for (indx = 0; indx < N_WCS_DATA_BITS; indx++) {
	if (WCS_data & 1) {
	    dgbus->ucode_command_and_data = 0xff;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	} else {
	    dgbus->ucode_command_and_data = 0;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	}
	WCS_data = WCS_data >> 1;
    }
/*
 * Direct the WCS Controller to forward the next eight bits per chip to the
 * WCS's command register.
 */
    dgbus->ucode_command_and_data = WRITE_WCS_SPC_COMMAND_REGISTER << 4 | 3;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    temp_b = SPC_WRITE_REGISTER << 4 | WCS_register;
    for (indx = 0; indx < N_WCS_COMMAND_BITS; indx++) {
	if (temp_b & 1) {
	    dgbus->ucode_command_and_data = 0xff;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	} else {
	    dgbus->ucode_command_and_data = 0;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	}
	temp_b = temp_b >> 1;
    }
    end_buserr_count_region();
}



/*
 * This routine reads 16 bits of data from a specified register of the zeroth
 * Writable Control Store (WCS) chip via the Diag bus (a.k.a. dbus).
 */

read_WCS_register (dg_base, slot, verbose, WCS_register)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
    u_char WCS_register;
{
    u_short WCS_register_data;

    dgtype *dgbus;

    short indx;
    short jndx;
    u_char temp_b;

    WCS_register_data = 0;
    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
    
    start_buserr_count_region();
/*
 * Direct the WCS Controller to forward the next eight bits per chip to the
 * WCS's command register.
 */
    dgbus->ucode_command_and_data = WRITE_WCS_SPC_COMMAND_REGISTER << 4 | 3;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");
    temp_b = SPC_READ_REGISTER << 4 | WCS_register;
    for (indx = 0; indx < N_WCS_COMMAND_BITS; indx++) {
	if (temp_b & 1) {
	    dgbus->ucode_command_and_data = 0xff;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	} else {
	    dgbus->ucode_command_and_data = 0;
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	    asm("	nop");
	}
	temp_b = temp_b >> 1;
    }
/*
 * Direct the WCS Controller to forward the next sixteen bits per chip from the
 * WCS's data register to the RP via the Diag bus.
 */
    dgbus->ucode_command_and_data = READ_WCS_SPC_DATA_REGISTER << 4 | 4;
    asm("	nop");
    asm("	nop");
    asm("	nop");
    asm("	nop");

    for (indx = 0; indx < N_WCS_DATA_BITS; indx++) {
	temp_b = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	WCS_register_data = WCS_register_data >> 1 |
	    (u_short)(temp_b & 1) << N_WCS_DATA_BITS - 1;
    }
    end_buserr_count_region();
    return(WCS_register_data);
}

read_ROM (dg_base, slot, verbose, ROM_data)
    dgerrcnt *dg_base;
    u_char slot;
    char verbose;
    u_char *ROM_data;
{
    dgtype *dgbus;
    u_long indx;

    dgbus = (dgtype *)((slot << 6) + DGBUSSTART);
    
    start_buserr_count_region();
    for (indx = 0; indx < N_ROM_BYTES; indx += 16) {
	dgbus->ucode_command_and_data = READ_ROM << 4 | 4;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 1] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 2] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 3] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 4] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 5] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 6] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 7] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 8] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 9] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 10] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 11] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 12] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 13] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 14] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
	ROM_data[indx + 15] = dgbus->ucode_command_and_data;
	asm("	nop");
	asm("	nop");
	asm("	nop");
	asm("	nop");
    }
	end_buserr_count_region();
}

void long_buserrnote()
{
    hi_mem_globals *glbl;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;
    glbl->long_buserrcnt++;
}

start_buserr_count_region()
{
    hi_mem_globals *glbl;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;
    abortstate = ABORT_INTERNAL;
    buserrhandler = &long_buserrnote;
    glbl->long_buserrcnt = 0;
    buserrcnt = 0;
}

end_buserr_count_region()
{
    hi_mem_globals *glbl;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;
    glbl->Total_Bus_Error_Count += glbl->long_buserrcnt;
    buserrhandler = 0;
}

check_flow_control()
{
    char temp_c;

    if (console->gsr & GSR_A_RXRDY) {
	if ( (temp_c = getch()) == 0x13 ) {	/* ^S (XOFF) */
	    while ( TRUE ) {
		if (console->gsr & GSR_A_RXRDY) {
		    if ((temp_c = getch()) == 0x11) /* ^Q (XON) */
			break;
		}
	    }
	}
    }
}

check_more()
{
    hi_mem_globals *glbl;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    if (glbl->line_limit) {
	if (glbl->line_count++ >= glbl->line_limit) {
	    glbl->line_count = 0;
	    printf("Page full, type return to continue: ");
	    gets(bufu.buf);
	}
    }
}

update_dg_errors_8(err_nd, actual, expected)
    dg_error *err_nd;
    u_char actual;
    u_char expected;
{
    hi_mem_globals *glbl;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    glbl->Total_Error_Count++;
    err_nd->err_count++;
    err_nd->err_last_actual = (u_short)actual;
    err_nd->err_last_expected = (u_short)expected;
}

update_dg_errors_16(err_nd, actual, expected)
    dg_error *err_nd;
    u_short actual;
    u_short expected;
{
    hi_mem_globals *glbl;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    glbl->Total_Error_Count++;
    err_nd->err_count++;
    err_nd->err_last_actual = actual;
    err_nd->err_last_expected = expected;
}

zero_error_vector(dg_base)
    dg_error dg_base[][N_DG_ERRORS];
{
    hi_mem_globals *glbl;
    u_short indx;
    u_short jndx;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    glbl->Total_Bus_Error_Count = 0;
    glbl->Total_Error_Count = 0;
    glbl->line_count = 0;

    for (indx = 0; indx < NSLOTS; indx++) {
	for (jndx = 0; jndx <= glbl->last_WCS_test_ID; jndx++) {
	    dg_base[indx][jndx].err_count = 0;
	    dg_base[indx][jndx].err_last_actual = 0;
	    dg_base[indx][jndx].err_last_expected = 0;
	}
    }
}

display_error_vector(dg_base, slot)
    dg_error dg_base[][N_DG_ERRORS];
    u_char slot;
{
    hi_mem_globals *glbl;
    u_short indx;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    check_flow_control();
    printf("\n\nCount:     Actual:  Expected:   Type:");

    for (indx = 0; indx <= glbl->last_WCS_test_ID; indx++) {
	check_flow_control();
	printf("\n%10u %8x %8x    %s",
	       dg_base[slot][indx].err_count,
	       dg_base[slot][indx].err_last_actual,
	       dg_base[slot][indx].err_last_expected,
	       WCS_test_descriptions[indx]);
    }
}


init_hi_mem_globals (dg_base)
    dgerrcnt *dg_base;
{
    hi_mem_globals *glbl;
    u_short indx;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    glbl->diag_previous_entry = MAGIC_ENTRY;
    glbl->Total_Error_Count = 0;
    glbl->Total_Bus_Error_Count = 0;
    glbl->requested_iterations = 0;
    glbl->actual_iterations = 0;
    glbl->WCS_words = MAX_WCS_WORDS;
    glbl->WCS_starting_row = 0;
    glbl->WCS_tests_vector = -1;
    glbl->Slots_to_test_vector = 0;
    glbl->line_count = 0;
    glbl->line_limit = 0;
    glbl->WCS_valid_opcode_vector = 1 << WRITE_WCS_SPC_COMMAND_REGISTER |
				     1 << WRITE_WCS_SPC_DATA_REGISTER |
				     1 << SET_CONTROL_REGISTER_WCS_LOADED |
		       		     1 << RESET_ROM_ADDRESS_REGISTER |
		    		     1 << INCREMENT_ROM_ADDRESS_REGISTER |
				     1 << READ_ROM |
			    	     1 << READ_WCS_SPC_COMMAND_REGISTER |
				     1 << READ_WCS_SPC_DATA_REGISTER;
    glbl->Slot = 0;
    glbl->Slot_Address = 0;
    glbl->Write_Data = 0;
    for (indx = 0; indx < NSLOTS; indx++)
	glbl->num_WCS_chips[indx] = 8;
    glbl->diag_read_or_write = 'r';
    glbl->verbose = 'n';
    glbl->Zero_Error_structure = 'y';
    glbl->all_slots = 'y';
    glbl->all_rows = 'n';
    glbl->WCS_chip_num_status = 'y';
    glbl->temp_abort = FALSE;
    abort = FALSE;

    glbl->first_WCS_test_ID = &dg_base[0].WCS_Zeros_errors -
				  &dg_base[0].no_response;
    glbl->last_WCS_test_ID = &dg_base[0].WCS_cntrl_instr_err_detection -
		    &dg_base[0].no_response;
}

get_Slot()
{
    hi_mem_globals *glbl;
    long temp;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    while (TRUE) {
	printf("Physical slot number [%u]? (valid values are 0 to 15): ",
	       glbl->Slot);
	gets(bufu.buf);
	if ( tolower(bufu.buf[1]) == 'x' )
	    printf("This parameter is in decimal\n");
	else {
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp < NSLOTS))
		glbl->Slot = temp;
	    break;
	}
    }
}

get_Slot_Address ()
{
    hi_mem_globals *glbl;
    long temp;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    printf("Address within slot %u, [%x]? (valid values are 0 to 3f): ",
	   glbl->Slot, glbl->Slot_Address);
    gets(bufu.buf);
    if ( tolower(bufu.buf[1]) == 'x' ) 
	bufu.buf[0] = bufu.buf[1] = '0';
    temp = gethexnum(bufu.buf);
    if ((temp >= 0) && (temp < 0x40))
	glbl->Slot_Address = temp;
}

get_diag_read_or_write()
{
    hi_mem_globals *glbl;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    printf("Read or Write access to slot %u at %x [%c]? ",
	   glbl->Slot, glbl->Slot_Address, glbl->diag_read_or_write);
    gets(bufu.buf);
    if (tolower(bufu.buf[0]) == 'w' || tolower(bufu.buf[0]) == 'r')
	glbl->diag_read_or_write = tolower(bufu.buf[0]);
}

get_Write_Data()
{
    hi_mem_globals *glbl;
    long temp;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    printf("Write data (hex 8 bits) [%x]? ",glbl->Write_Data);
    gets(bufu.buf);
    if ( tolower(bufu.buf[1]) == 'x' ) 
	bufu.buf[0] = bufu.buf[1] = '0';
    temp = gethexnum(bufu.buf);
    if ((temp >= 0) && (temp <= 0xff))
	glbl->Write_Data = temp;
}

get_verbose ()
{
    hi_mem_globals *glbl;
    char temp_c;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    printf("Verbose [%c]? ", glbl->verbose);
    gets(bufu.buf);
    temp_c = tolower(bufu.buf[0]);
    if ((temp_c == 'y') || (temp_c == 'n'))
	glbl->verbose = temp_c;
}

get_Zero_Error_structure()
{
    hi_mem_globals *glbl;
    char temp_c;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    printf("Zero error counters [%c]? ", glbl->Zero_Error_structure);
    gets(bufu.buf);
    temp_c = tolower(bufu.buf[0]);
    if ((temp_c == 'y') || (temp_c == 'n'))
	glbl->Zero_Error_structure = temp_c;
}

get_requested_iterations()
{
    hi_mem_globals *glbl;
    u_long temp;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    while (TRUE) {
	printf("Iterations [%u]? (zero means infinite) ",
	       glbl->requested_iterations);
	gets(bufu.buf);
	if ( tolower(bufu.buf[1]) == 'x' ) 
	    printf("This parameter is in decimal\n");
	else {
	    temp = getnum(bufu.buf);
	    if (temp != 0xffffffff)
		glbl->requested_iterations = temp;
	    break;
	}
    }
}

get_WCS_words()
{
    hi_mem_globals *glbl;
    u_long temp;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    while (TRUE) {
	printf("Number of WCS words to be tested: [%u]? ",
	       glbl->WCS_words);
	gets(bufu.buf);
	if ( tolower(bufu.buf[1]) == 'x' ) 
	    printf("This parameter is in decimal\n");
	else {
	    temp = getnum(bufu.buf);
	    if ((temp > 0) && (temp <= MAX_WCS_WORDS))
		glbl->WCS_words = temp;
	    break;
	}
    }
}

get_line_limit()
{
    hi_mem_globals *glbl;
    long temp;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    while (TRUE) {
	printf("Terminal lines [%u]? (0 means infinite)", glbl->line_limit);
	gets(bufu.buf);
	if ( tolower(bufu.buf[1]) == 'x' )
	    printf("This parameter is in decimal\n");
	else {
	    temp = getnum(bufu.buf);
	    if (temp >= 0 && temp <= 65535)
		glbl->line_limit = (u_short)temp;
	    break;
	}
    }
}

get_all_slots()
{
    hi_mem_globals *glbl;
    char temp_c;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    printf("All slots [%c]? ", glbl->all_slots);
    gets(bufu.buf);
    temp_c = tolower(bufu.buf[0]);
    if ((temp_c == 'y') || (temp_c == 'n'))
	glbl->all_slots = temp_c;
}

get_all_rows()
{
    hi_mem_globals *glbl;
    char temp_c;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    printf("Display nonerror rows [%c]? ", glbl->all_rows);
    gets(bufu.buf);
    temp_c = tolower(bufu.buf[0]);
    if ((temp_c == 'y') || (temp_c == 'n'))
	glbl->all_rows = temp_c;
}

get_num_WCS_chips()
{
    hi_mem_globals *glbl;
    long temp;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    while (TRUE) {
	printf("Number of WCS chips on the card in slot %u, [%u]? ",
	       glbl->Slot, glbl->num_WCS_chips[glbl->Slot]);
	gets(bufu.buf);
	if ( tolower(bufu.buf[1]) == 'x' ) 
	    printf("This parameter is in decimal\n");
	else {
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp <= MAX_WCS_CHIPS))
		glbl->num_WCS_chips[glbl->Slot] = temp;
	    break;
	}
    }
}

get_WCS_chip_num_status()
{
    hi_mem_globals *glbl;
    u_short indx;
    char temp_c;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    printf("Using:");
    for (indx = 0; indx < NSLOTS; indx += 4)
	printf("  %u  %u  %u  %u    ", glbl->num_WCS_chips[indx],
	       glbl->num_WCS_chips[indx + 1],
	       glbl->num_WCS_chips[indx + 2],
	       glbl->num_WCS_chips[indx + 3]);
    printf("\n    as the number of WCS chips per slot.  OK? [%c]: ",
	   glbl->WCS_chip_num_status);
    gets(bufu.buf);
    temp_c = tolower(bufu.buf[0]);
    if ((temp_c == 'y') || (temp_c == 'n'))
	glbl->WCS_chip_num_status = temp_c;
}

get_WCS_tests_vector()
{
    hi_mem_globals *glbl;
    u_long temp;
    u_short indx;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    while (TRUE) {
	printf("Test selection:\n");
	for (indx = glbl->first_WCS_test_ID;
	     indx <= glbl->last_WCS_test_ID; indx++) {
	    printf("%s\t%u\t%s\n", ((glbl->WCS_tests_vector & 1 <<
				     indx) ? "Yes" : "No"), indx,
		   WCS_test_descriptions[indx]);
	}
	while (TRUE) {
	    printf("Type the test ID to toggle selection status: ");
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		printf("This parameter is in decimal\n");
	    else
		break;
	}
	temp = getnum(bufu.buf);
	if ((temp >= glbl->first_WCS_test_ID) &&
	    (temp <= glbl->last_WCS_test_ID))
	    glbl->WCS_tests_vector ^= 1 << temp;
	else
	    break;
    }
}

get_Slots_to_test_vector()
{
    hi_mem_globals *glbl;
    long temp;
    u_short indx;

    glbl = (hi_mem_globals *)GLOBALSTORAGE;

    while (TRUE) {
	printf("Slot selection:\n");
	for (indx = 0; indx < NSLOTS; indx++)
	    printf("%s\t%u\n", ((glbl->Slots_to_test_vector & 1 <<
				 indx) ? "Yes" : "No"), indx);
	while (TRUE) {
	    printf("Type the slot ID to toggle selection status: ");
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		printf("This parameter is in decimal\n");
	    else
		break;
	}
	temp = getnum(bufu.buf);
	if ((temp >= 0) && (temp < NSLOTS))
	    glbl->Slots_to_test_vector ^= 1 << temp;
	else
	    break;
    }
}

void reset_dgbus (void)
{
    int slot;
    dgtype *dgbus;

    for (slot = 0; slot < NSLOTS; slot++) {
	dgbus = pslot_to_dbus(slot);
	if (touch_device(dgbus)) 
	    dbus_prot_wr(slot, &dgbus->diag_status, 0);  /* Reset device */
    }
}
