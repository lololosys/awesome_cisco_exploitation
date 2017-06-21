/* $Id: rpev.c,v 3.1 1995/11/09 09:10:33 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/rpev.c,v $
 *------------------------------------------------------------------
 * env.c -- command parser for monitor software that tests the RP
 * environmental functions.
 *
 * December 1991, Stan Mantiply
 *
 * Copyright (c) 1991-1993 by cisco Systems, Inc.
 * All Rights Reserved.
 *------------------------------------------------------------------
 * $Log: rpev.c,v $
 * Revision 3.1  1995/11/09  09:10:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:38  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1993/03/30  00:24:02  mdb
 * fix minor problems found by gcc 2.x warnings.
 *
 * Revision 1.1  1993/01/30  01:12:06  whp4
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
 *	a            	All test points displayed
 *	d	         Definitions of all test points
 *	l	         Low pass filter test
 *	r	         Results of low pass filter test
 *	s	         Scope loop
 *	p	         Set global Power Supply controls
 *	c	         Set ADC controls for a specified test point
 *	q	         Quit and return to higher level interpreter
 *	h or ?	         Help for commands
 */
#include "../defs.h"
#include "../stdio.h"
#include "../ctype.h"
#include "../cpu.h"
#include "../statreg.h"
#include "../prom2.h"
#include "../reason.h"
#include "../extern.h"
#include "../save_stack.h"

extern int reset_io();

#define ENV_CTL			0x11120200
#define ENV_STS			0x11120300
#define ENV_MAX_TEST_PT		15
#define ENV_TEST_RESULT_BASE	0x3fc000
#define NSAMPLES		4096
#define POWER_SUPPLY_2_MSK	0x80
#define POWER_SUPPLY_1_MSK	0x40
#define FORMAT_MSK		0x20
#define POLARITY_MSK		0x10

#define MAGIC_ENTRY		0x7549fadc

#define IDPROMBASE		0x11131000
#define NIDPROMBYTES		32

u_char ADC_Format_and_Polarity[ENV_MAX_TEST_PT + 1];
u_char Power_Supply_Control;
u_long test_point;
u_long envm_previous_entry;
char envm_read_or_write;


rpenvmtest()

{
    char *c;
    char comm;
    char temp_b;
    char old_abortstate;
    u_long env_result;
    long temp;
    u_long scope_loop_total;
    u_long dpmask;
    u_short dpshift;
    u_short dpstart;
    u_short dpend;

/*
 * !!!!!! Ask Larry about this.
 *  volatile u_short *env_control;
 */
    volatile u_long *env_control;
    volatile u_long *env_status;
    u_long indx;
    u_long jndx;

    char *env_id_str[] = {
    		"Test",
		"+12 Volts",
		"+5 Volts",
		"-12 Volts",
		"+24 Volts",
		"Reserved",
		"Reserved",
		"Exhaust temperature on Route Processor",
		"Slot 0 temperature",
		"Slot 1 temperature",
		"Slot 2 temperature",
		"Slot 3 temperature",
		"Slot 4 temperature",
		"Slot 5 temperature",
		"Route Processor temperature (slot 6)",
		"Inlet temperature"
		};

    u_long *env_test_results;

    u_char ID_prom_results[NIDPROMBYTES];
    volatile u_char *ID_prom;

    ID_prom = (u_char *)IDPROMBASE;

    env_control = (u_long *)ENV_CTL;
    env_status = (u_long *)ENV_STS;

    reset_io();			/* Reset all DMA devices so they stop */

    env_test_results = (u_long *) ENV_TEST_RESULT_BASE;
    dpshift = 0;
    dpmask = 0xffffffff;
    dpstart = 0;
    dpend = NSAMPLES - 1;

    printf("Environmental diagnostic for RP\n\n");
    env_help();

    if (envm_previous_entry != MAGIC_ENTRY) {
	envm_previous_entry = MAGIC_ENTRY;
	test_point = 0;
	envm_read_or_write = 'r';
	abort = FALSE;
/*
 * Set default ADC controls to "offset binary, bipolar" mode.
 */
	for (indx = 0; indx <= ENV_MAX_TEST_PT; indx++)
	    ADC_Format_and_Polarity[indx] = POLARITY_MSK;
	Power_Supply_Control = 0;
    }

    for (;;) {

	old_abortstate = abortstate;
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

	case 'A':		/* All probes display */
	    for (indx = 0; indx <= ENV_MAX_TEST_PT; indx++) {
	        *env_control = indx | ADC_Format_and_Polarity[indx] |
		    Power_Supply_Control;
		delay(10);
		env_result = *env_status;
		printf("%1x %2x %1x %1x     %s\n",
		       env_result >> 16 & 0xf,
		       env_result >> 8 & 0xff,
		       env_result >> 4 & 0xf,
		       env_result & 0xf,
		       env_id_str[indx]);
	    }
	    break;

	case 'D':		/* Definitions of all probes */
	    for (indx = 0; indx <= ENV_MAX_TEST_PT; indx++) {
		printf("%d     %s\n", indx, env_id_str[indx]);
	    }	
	    break;

	case 'L':		/* Low pass filter test */
    
	    printf("\nTest Point number [%d]? (valid values are 0 to 15)",
		   test_point);
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		goto Huh;
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp <= ENV_MAX_TEST_PT))
		test_point = temp;

	    test_point_controls_msg(test_point);
	    power_supply_controls_msg();
/*
 * Write the test point index, ADC control, and Power Supply control to the
 * environmental control unit.
 */
	    *env_control = test_point | ADC_Format_and_Polarity[test_point] |
		Power_Supply_Control;
/*
 * Read the results back as fast as possible and store them for subsequent
 * retrival.  !!! DO NOT PUT ANY CODE BETWEEN THE ABOVE WRITE AND THE BELOW
 * READS!!!
 */
	    for (indx = 0; indx < NSAMPLES; indx += 16) {
		env_test_results[indx] = *env_status;
		env_test_results[indx + 1] = *env_status;
		env_test_results[indx + 2] = *env_status;
		env_test_results[indx + 3] = *env_status;
		env_test_results[indx + 4] = *env_status;
		env_test_results[indx + 5] = *env_status;
		env_test_results[indx + 6] = *env_status;
		env_test_results[indx + 7] = *env_status;
		env_test_results[indx + 8] = *env_status;
		env_test_results[indx + 9] = *env_status;
		env_test_results[indx + 10] = *env_status;
		env_test_results[indx + 11] = *env_status;
		env_test_results[indx + 12] = *env_status;
		env_test_results[indx + 13] = *env_status;
		env_test_results[indx + 14] = *env_status;
		env_test_results[indx + 15] = *env_status;
	    }
	    printf("\nenv_test_results start at: %#x",env_test_results);

	    dpstart = 0;
	    dpend = 160;		/* NSAMPLES - 1; */
	    dpshift = 8;		/* shift first then mask */
	    dpmask = 0xff;

	    goto putspecifiedrange;

	case 'R':		/* Results print out */

	    printf("\nStarting Data Point [%d]? ",dpstart);
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		goto Huh;
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp < NSAMPLES))
		dpstart = temp;

	    printf("\nEnding Data Point [%d]? ",dpend);
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		goto Huh;
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp < NSAMPLES))
		dpend = temp;

	    printf("\nRight shift amount [%d]? ",dpshift);
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		goto Huh;
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp < 32))
		dpshift = temp;

	    printf("\nAnd mask [%x]? ",dpmask);
	    gets(bufu.buf);
	    if (bufu.buf[0] != '\0') {
		if ( tolower(bufu.buf[1]) == 'x' ) 
		    bufu.buf[0] = bufu.buf[1] = '0';
		dpmask = gethexnum(bufu.buf);
	    }

putspecifiedrange:
	    printf("\nResults for Test Point number [%d]", test_point);

	    for (indx = (dpstart >> 3 << 3); indx < dpend + 7 >> 3 << 3;
		 indx += 8) {
		if (!(((u_long)(&env_test_results[indx])) & 0xffL))
		    printf("\nData Point  %d (%x) through  %d (%x)",
			   indx, indx, indx + 63, indx + 63);
		if (abort) {
		    abort = FALSE;
		    printf("\nPrintout Aborted\n");
		    break;
		}
		if (console->gsr & GSR_A_RXRDY) {
		    if ( (temp_b = getch()) == 0x13 ) {	/* ^S (XOFF) */
			while ( TRUE ) {
			    if (console->gsr & GSR_A_RXRDY) {
				if ((temp_b = getch()) == 0x11) /* ^Q (XON) */
				    break;
			    }
			}
		    }
		}
		printf("\n %8x %8x %8x %8x %8x %8x %8x %8x",
		       env_test_results[indx] >> dpshift & dpmask,
		       env_test_results[indx + 1] >> dpshift & dpmask,
		       env_test_results[indx + 2] >> dpshift & dpmask,
		       env_test_results[indx + 3] >> dpshift & dpmask,
		       env_test_results[indx + 4] >> dpshift & dpmask,
		       env_test_results[indx + 5] >> dpshift & dpmask,
		       env_test_results[indx + 6] >> dpshift & dpmask,
		       env_test_results[indx + 7] >> dpshift & dpmask);
	    }
	    printf("\n");
	    break;

	case 'S':		/* Scope Loop */
    
	    scope_loop_total = 0;

	    printf("Read or Write [%c]? ", envm_read_or_write);
	    gets(bufu.buf);
	    if (tolower(bufu.buf[0]) == 'w' || tolower(bufu.buf[0]) == 'r')
		envm_read_or_write = tolower(bufu.buf[0]);
	    if (envm_read_or_write == 'w') {

	    printf("\nTest Point number [%d]? (valid values are 0 to 15): ",
		   test_point);
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		goto Huh;
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp <= ENV_MAX_TEST_PT))
		test_point = temp;

	    test_point_controls_msg(test_point);
	    power_supply_controls_msg();
	    printf("\n");
/*
 * The following computation was not removed from the loop by the optimizer.
 */
		temp = test_point | ADC_Format_and_Polarity[test_point] |
		    Power_Supply_Control;
		for (;;) {
		    for (indx = 0; indx < 500000; indx++)
/*
 * Write the test point index, ADC control, and Power Supply control to the
 * environmental control unit.
 */
			*env_control = temp;

		    scope_loop_total += indx;
		    printf("\rWritten %d", scope_loop_total);

		    if (abort) {
			abort = FALSE;
			printf("\nAborted\n");
			break;
		    }
		}
	    } else {
		*env_test_results = *env_status;
		printf("Read %x\n", *env_test_results);

		for (;;) {
		    for (indx = 0; indx < 500000; indx++)
/*
 * Read the test point index, ADC status, and Power Supply status from the
 * environmental control unit.
 */
			*env_test_results = *env_status;

		    scope_loop_total += indx;
		    printf("\rRead %d", scope_loop_total);

		    if (abort) {
			abort = FALSE;
			printf("\nAborted\n");
			break;
		    }
		}
	    }
	    break;

	case 'P':		/* Power Supply controls */

	    printf("Note: this Power Supply controls command does not \
directly access the hardware.");

	    printf("\nCurrent Power Supply controls are: %d or ",
		   (Power_Supply_Control >> 6));
	    power_supply_controls_msg();
	    printf("\nNote these bits are inverted, therefore the valid values\
 and their\nassociated meanings are:");
	    printf("\n0 = both power supplies on");
	    printf("\n1 = power supply 1 off and 2 on");
	    printf("\n2 = power supply 1 on and 2 off");
	    printf("\n3 = both power supplies off");

	    printf("\nPlease enter a Power Supply control value: ");
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		goto Huh;
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp <= 3))
		Power_Supply_Control = temp << 6;

	    break;
	    
	case 'C':		/* ADC controls */

	    printf("Test Point number [%d]? (valid values are 0 to 15)",
		   test_point);
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		goto Huh;
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp <= ENV_MAX_TEST_PT))
		test_point = temp;

	    test_point_controls_msg(test_point);

	    printf("\nThe valid values for ADC controls and their associated \
meanings are:");
	    printf("\n0 = straight binary, Unipolar");
	    printf("\n1 = offset binary, Bipolar");
	    printf("\n2 = 2's compliment binary, Unipolar ");
	    printf("\n3 = 2's compliment binary, Bipolar ");

	    printf("\nPlease enter the ADC control value for this test \
point: ");
	    gets(bufu.buf);
	    if ( tolower(bufu.buf[1]) == 'x' ) 
		goto Huh;
	    temp = getnum(bufu.buf);
	    if ((temp >= 0) && (temp <= 3))
		ADC_Format_and_Polarity[test_point] = temp << 4;

	    break;
	    
	case 'I':

	    printf("Initial values for error tests\n");

	    for (indx = 0; indx < NIDPROMBYTES; indx++)
		ID_prom_results[indx] = ID_prom[indx];

	    for (indx = 0; indx < NIDPROMBYTES / 2; indx += 4)
		printf("%2x %2x %2x %2x   ",
		       ID_prom_results[indx],
		       ID_prom_results[indx + 1],
		       ID_prom_results[indx + 2],
		       ID_prom_results[indx + 3]);
	    printf("\n");

	    for (indx = NIDPROMBYTES / 2; indx < NIDPROMBYTES; indx += 4)
		printf("%2x %2x %2x %2x   ",
		       ID_prom_results[indx],
		       ID_prom_results[indx + 1],
		       ID_prom_results[indx + 2],
		       ID_prom_results[indx + 3]);
	    printf("\n");

	    scope_loop_total = temp = 0;
	    for (;;) {
		for (indx = 0; indx < 500000; indx++)
/*
 * Read the ID prom and compare the results with the first read data.
 */
		    for (jndx = 0; jndx < NIDPROMBYTES; jndx++)
			if (ID_prom_results[jndx] != ID_prom[jndx])
			    temp++;

		scope_loop_total += indx;
		printf("\rRead %u with %u errors", scope_loop_total, temp);

		if (abort) {
		    abort = FALSE;
		    printf("\nAborted\n");
		    break;
		}
	    }
	    break;

	case '?':
	case 'H':		/* help */
	    env_help();
	    break;
	    
	case 'Q':		/* Quit */
	    abortstate = old_abortstate;
	    return;
	    break;
	    
	default:
	Huh:
	    puts("What?\n");
	    break;
	}
	
	flush();	/* Make sure that the UART is drained */
    }
}

test_point_controls_msg(tst_pt)
    u_long tst_pt;
{
    printf("\nUsing Test Point number [%d]! in ", tst_pt);

    switch (ADC_Format_and_Polarity[tst_pt] & (FORMAT_MSK | POLARITY_MSK)) {

    case 0:
	printf("straight binary, Unipolar ");
	break;
	
    case POLARITY_MSK:
	printf("offset binary, Bipolar ");
	break;
	
    case FORMAT_MSK:
	printf("2's compliment binary, Unipolar ");
	break;
	
    case FORMAT_MSK | POLARITY_MSK:
	printf("2's compliment binary, Bipolar ");
	break;
    }
    printf("mode\n");
}

power_supply_controls_msg()
{    
    switch (Power_Supply_Control & (POWER_SUPPLY_2_MSK | POWER_SUPPLY_1_MSK)) {
	
    case 0:
	printf("both power supplies on ");
	break;
	
    case POWER_SUPPLY_1_MSK:
	printf("power supply 1 off and 2 on ");
	break;
	
    case POWER_SUPPLY_2_MSK:
	printf("power supply 1 on and 2 off ");
	break;
	
    case POWER_SUPPLY_2_MSK | POWER_SUPPLY_1_MSK:
	printf("both power supplies off ");
	break;
    }
}

env_help ()
{
    printf("a            All test points displayed\n");
    printf("d            Definitions of all test points\n");
    printf("l            Low pass filter test\n");
    printf("r            Results of low pass filter test\n");
    printf("s            Scope loop\n");
    printf("p            Set global Power Supply controls\n");
    printf("c            Set ADC controls for a specified test point\n");
    printf("i            ID prom scope loop\n");
    printf("q            Quit and return to higher level interpreter\n");
    printf("h or ?       Help for commands\n");
    }
