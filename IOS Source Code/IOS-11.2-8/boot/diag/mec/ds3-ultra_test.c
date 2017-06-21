/* $Id: ds3-ultra_test.c,v 3.1.58.1 1996/05/17 10:23:08 ppearce Exp $
 * $Source: /release/112/cvs/Xboot/diag/mec/ds3-ultra_test.c,v $
 *------------------------------------------------------------------
 * d3-ultra_test.c -- DS3/UltraNetwork specific routines.
 *
 * 22-Feburary-1991, Bruce Sherry
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ds3-ultra_test.c,v $
 * Revision 3.1.58.1  1996/05/17  10:23:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.46.1  1996/04/03  13:11:25  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1  1995/11/09  09:07:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:14:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1.52.1  1995/10/21  01:02:29  getchell
 * Branch: Arkansas_branch
 * Ark_Vip_branch commit
 *
 * Revision 1.1.86.1  1995/07/24  16:55:26  getchell
 * Branch: VIP_branch
 * Sync VIP_branch between VIP_baseline_950523 and VIP_baseline_950720
 *
 * Revision 2.1  1995/06/07  19:13:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:57:07  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../h/std_dci.h"

#define BREAK_POOL      1
#define BREAK_LENGTH    1496
#define BREAK_WAIT      200
#define BREAK_SPACE     (ulong)(BREAK_LENGTH + 25 + 20)
#define PACKET_SPACE    (ulong)((65536 - BREAK_SPACE) - 266)
#define ULT_OFFS        0
#define ULT_START       40
#define ULT_END         8400
#define ULT_SKIP        8
#define ULT_APP_REGS    0
#define ULT_MEMA_START	0x100
#define ULT_MEMA_END	0x800
#define DS3_OFFS        0
#define DS3_START       5
#define DS3_END         1500
#define DS3_SKIP        1
#define DS3_APP_REGS    2
#define DS3_MEMA_START	0x100
#define DS3_MEMA_END	0x800

/*
 *  Function declarations
 */

void 	ds3_command(), ultra_command(), ds3_experttest(), ultra_experttest(),
    ds3_test(), ultra_test(), ds3_autotest(), ultra_autotest(),
    ultra_test_bothways(), xmit_loop(), dci_ports_on(), fill_xmit_buff(),
    ds3_test_bothways(), ds3_ultra_ports(), display_error_counters();
boolean	dci_pingit(), ping_ports(), multiport_ping(), enqueue(), dci_bothways(),
    dci_bothways_test(), one_way_ping(), ds3_setup(), ultra_setup(),
    dci_bothways_scan(), setup_both();

short	send_setup();

static char         ds3_loop;
static boolean      ds3_clock, ping_loop_save, break_allocated;

/*
 * ds3_command
 * Expert's DCI test routines
 */

void ds3_command()
{
    register mcitype    *cbus;
    int                 argument, command, i, rcount, linect, size,
    flag, ahigh, alow, mhigh, mlow, afhigh, aflow,
    arg1, arg2, memd_sadr,memd_enadr;
    char                cmdchar;
    char                buff[BUFLEN];
    boolean             parsed;
    ushort              passes;
    
    pkt_offset = DS3_OFFS;
    pkt_start = DS3_START;
    pkt_end = DS3_END;
    pkt_skip = DS3_SKIP;
    mema_start = DS3_MEMA_START;
    mema_end = DS3_MEMA_END;
    standard_packet = TRUE;
    ping_wait = 1000;
    park_buffers = 3;
    rcount = 1;
    reporting = TRUE;
    looping = FALSE;
    ping_loopback = FALSE;
    ds3_clock = TRUE;
    ds3_loop = 'A';
    printf("\nDS3 Diagnostic.");
    dci_alloc_diag_mem();
    init_error_struct(FCI_MAX_DCI_CTRLRS); /* prepare the error storage area */
    cbus = cBus_findcontroller();
    if (cbus->unit < 0)
        printf("\nCan't find cBus Controller!!\n");
    argument = command = default_interface = 0;
    dci_config(cbus, FCI_DS3_CONTROLLER);
    dci_setup_unit(cbus);
    abortstate = ABORT_INTERNAL;
    buserrhandler = buserrnote;
    cmdchar = '?';
    while (TRUE) {
        if (cbus == NULL)
            return;
	verbose = TRUE;
        printf("\nMain command (?ABCDEHIJKLNOPQSZ) [%c]: ", cmdchar);
        parsed = (int) gets(buff);
        if (!null(buff) && parsed)
            cmdchar = cvu(buff[0]);
        switch (cmdchar) {
          case 'A':             /* Debug basic tests */
            mec_basic_debug(cbus);
            break;
          case 'B':             /* Do basic tests */
            ds3_experttest(cbus);
            break;
	  case 'C':             /* Display Error Counters */
	    display_error_counters(cbus);
	    break;
          case 'D':             /* Run MEMA TEST once */
            (void)dci_mema_test(cbus, &linect);
            break;
          case 'E':             /* Run MEMD TEST once */
            (void)dci_memd_test(cbus, &linect);
            break;
	  case 'F':             /* DS3 loopbacK mode */ 
	    printf("\nA - Applique Loopback");
	    printf("\nD - local DTE Loopback");
	    printf("\nL - local Line Loopback");
	    printf("\nR - Remote line Loopback");
	    printf("\nQ - Quit to main menu\n");
	    cmdchar = 'A';
            do{
                printf("\nCustom DS3 Loopback Menu [%c]: ", cmdchar);
                parsed = (int) gets(buff);
                if (!null(buff) && parsed)
                    cmdchar = cvu(buff[0]);
                switch (cmdchar) {
		  case 'A':
		    ping_loopback = ping_loop_save;
		    ds3_loop = cmdchar;
		    break;
		  case 'D':
		  case 'L':
		  case 'R':
		    ping_loop_save = ping_loopback;
		    ping_loopback = FALSE;		    
		    ds3_loop = cmdchar;
		    break;
		  case 'Q':
		    break;
		  default:
		    printf("\n Bad Selection, try ADLR or Q");
		    break;
		}
	    }
	    while (cmdchar != 'Q');
	    cmdchar = 'F';
	    break;

          case 'H':             /* Display this menu */
          case '?':
            printf("\n\nA - Debug basic tests");
            printf("\nB - Do basic tests");
	    printf("\nC - Display Error Counters");
            printf("\nD - Run MEMA TEST once");
            printf("\nE - Run MEMD TEST once");
	    printf("\nF - Set DS3 loopback mode");
            printf("\nH or ? - Display this menu");
            printf("\nI - Run PING test once");
            printf("\nJ - Execute DS3 command");
	    printf("\nK - Set DS3 clocK mode");
            printf("\nL - List all errors");
            printf("\nN - Initialize (clear) errors");
            printf("\nO - Execute command code");
            printf("\nP - Debug DS3 port");
            printf("\nR - Special DS3 port tests");
            printf("\nQ - Quit");
            printf("\nS - Select board to test");
            printf("\nZ - Change Test Parameters");
            printf("\n\n'C' rules of radix type-in and display apply.\n");
            break;
          case 'I':             /* Run PING test once */
            (void)dci_ping_test(cbus, default_interface);
            break;
          case 'J':             /* Execute MEC command */
            dci_execute_2commands(cbus, &arg1, &arg2, &rcount);
            break;
	  case 'K':             /* DS3 clocK mode */
	    printf("\nI - Internal clock");
	    printf("\nN - Normal clock mode");
	    cmdchar = 'N';
	    do {
		printf("\n Set DS3 clock menu [%c]: ", cmdchar);
                parsed = (int) gets(buff);
                if (!null(buff) && parsed)
                    cmdchar = cvu(buff[0]);
                switch (cmdchar) {
		  case 'I':
		    ds3_clock = TRUE;
		    break;
		  case 'N':
		    ds3_clock = FALSE;
		    break;
		  case 'Q':
		    break;
		  default:
		    printf("\n Bad Selection, try I, N or Q");
		    break;
		}
	    }
	    while (cmdchar != 'Q');
	    cmdchar = 'K';
	    break;
          case 'L':             /* List all errors */
            show_all_errors(cbus, FCI_HARDWARE, board_to_test);
            break;
          case 'N':             /* Initialize (clear) errors */
            init_error_struct(FCI_MAX_DCI_CTRLRS);
            break;
          case 'O':             /* Execute command code */
            execute_command(cbus, &argument, &command);
            break;
          case 'P':             /* Debug DCI ports */
            mec_debug_ports(cbus, &default_interface, DS3_OFFS, DS3_START, DS3_END, DS3_SKIP);
            break;
          case 'Q':             /* Quit */
            return;
            break;
          case 'R':             /* Bothways_test */
	    ds3_ultra_ports(cbus);
            break;
          case 'S':             /* Select board to test */
            printf("\nNew board number [%d] ", board_to_test);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    board_to_test = parse_unsigned(buff);
                parsed = ((board_to_test >= 0) && (board_to_test <= 15));
            }
            if (!parsed) {
                printf("\n%% Bad board number, assuming 0");
                board_to_test = 0;
            }
            break;
            
          case 'V':             /* Print version information */
            printf("\nds3-ultra_test $Revision: 3.1.58.1 $ on $Date: 1996/05/17 10:23:08 $ by $Author: ppearce $.");
	    dci_revision();
            break;
          case 'W':             /* Select check memory location */ 
            printf("\nNew memory check location [0x%x] ", memcheck);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    memcheck = parse_unsigned(buff);
            }
            break;
          case 'Z':             /* Change Test Parameters */
            test_parameters();
            break;
          default:
            printf("\nUnknown command \"%c\"", cmdchar);
            break;
            
        }  /*  end switch(cmdchar)  */
        
        abort = FALSE;
    }  /*  end while()  */
    
    abortstate = ABORT_IGNORE;
    buserrhandler = 0;
    
}  /*  end ds3_command()  */

/*
 * ultra_command
 * Expert's DCI test routines
 */

void ultra_command()
{
    register mcitype    *cbus;
    int                 argument, command, i, rcount, linect, size,
    flag, ahigh, alow, mhigh, mlow, afhigh, aflow,
    arg1, arg2, memd_sadr,memd_enadr;
    char                cmdchar;
    char                buff[BUFLEN];
    boolean             parsed;
    ushort              passes;
    
    pkt_offset = ULT_OFFS;
    pkt_start = ULT_START;
    pkt_end = ULT_END;
    pkt_skip = ULT_SKIP;
    mema_start = ULT_MEMA_START;
    mema_end = ULT_MEMA_END;
    standard_packet = TRUE;
    ping_wait = 50;
    park_buffers = 3;
    rcount = 1;
    reporting = TRUE;
    looping = FALSE;
    ping_loopback = FALSE;
    printf("\nUltraNet Diagnostic.");
    dci_alloc_diag_mem();
    init_error_struct(FCI_MAX_DCI_CTRLRS); /* prepare the error storage area */
    cbus = cBus_findcontroller();
    if (cbus->unit < 0)
        printf("\nCan't find cBus Controller!!\n");
    argument = command = default_interface = 0;
    dci_config(cbus, FCI_DS3_CONTROLLER);
    dci_setup_unit(cbus);
    abortstate = ABORT_INTERNAL;
    buserrhandler = buserrnote;
    cmdchar = '?';
    while (TRUE) {
        if (cbus == NULL)
            return;
	verbose = TRUE;
        printf("\nMain command (?ABCDEHIJLNOPRQSZ) [%c]: ", cmdchar);
        parsed = (int) gets(buff);
        if (!null(buff) && parsed)
            cmdchar = cvu(buff[0]);
        switch (cmdchar) {
          case 'A':             /* Debug basic tests */
            mec_basic_debug(cbus);
            break;
          case 'B':             /* Do basic tests */
            ultra_experttest(cbus);
            break;
	  case 'C':             /* Display Error Counters */
	    display_error_counters(cbus);
	    break;
          case 'D':             /* Run MEMA TEST once */
            (void)dci_mema_test(cbus, &linect);
            break;
          case 'E':             /* Run MEMD TEST once */
            (void)dci_memd_test(cbus, &linect);
            break;
          case 'H':             /* Display this menu */
          case '?':
            printf("\n\nA - Debug basic tests");
            printf("\nB - Do basic tests");
	    printf("\nC - Display Error Counters");
            printf("\nD - Run MEMA TEST once");
            printf("\nE - Run MEMD TEST once");
            printf("\nH or ? - Display this menu");
            printf("\nI - Run PING test once");
            printf("\nJ - Execute Ultra command");
            printf("\nL - List all errors");
            printf("\nN - Initialize (clear) errors");
            printf("\nO - Execute command code");
            printf("\nP - Debug Ultra port");
            printf("\nQ - Quit");
            printf("\nR - Special Ultra port tests");
            printf("\nS - Select board to test");
            printf("\nZ - Change Test Parameters");
            printf("\n\n'C' rules of radix type-in and display apply.\n");
            break;
          case 'I':             /* Run PING test once */
            (void)dci_ping_test(cbus, default_interface);
            break;
          case 'J':             /* Execute MEC command */
            dci_execute_2commands(cbus, &arg1, &arg2, &rcount);
            break;
          case 'L':             /* List all errors */
            show_all_errors(cbus, FCI_HARDWARE, board_to_test);
            break;
          case 'N':             /* Initialize (clear) errors */
            init_error_struct(FCI_MAX_DCI_CTRLRS);
            break;
          case 'O':             /* Execute command code */
            execute_command(cbus, &argument, &command);
            break;
          case 'P':             /* Debug DCI ports */
            mec_debug_ports(cbus, &default_interface, ULT_OFFS, ULT_START, ULT_END, ULT_SKIP);
            break;
          case 'Q':             /* Quit */
            return;
            break;
          case 'R':             /* Bothways_test */
	    ds3_ultra_ports(cbus);
            break;
          case 'S':             /* Select board to test */
            printf("\nNew board number [%d] ", board_to_test);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    board_to_test = parse_unsigned(buff);
                parsed = ((board_to_test >= 0) && (board_to_test <= 15));
            }
            if (!parsed) {
                printf("\n%% Bad board number, assuming 0");
                board_to_test = 0;
            }
            break;
          case 'V':             /* Print version information */
            printf("\nds3_ultra_test $Revision: 3.1.58.1 $ on $Date: 1996/05/17 10:23:08 $ by $Author: ppearce $.");
	    dci_revision();
            break;
          case 'W':             /* Select check memory location */ 
            printf("\nNew memory check location [0x%x] ", memcheck);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    memcheck = parse_unsigned(buff);
            }
            break;
          case 'Z':             /* Change Test Parameters */
            test_parameters();
            break;
          default:
            printf("\nUnknown command \"%c\"", cmdchar);
            break;
            
        }  /*  end switch(cmdchar)  */
        
        abort = FALSE;
    }  /*  end while()  */
    
    abortstate = ABORT_IGNORE;
    buserrhandler = 0;
    
}  /*  end ultra_command()  */

/*
 * ds3_ultra_ports
 *
 * special test menu for ds3_ultra ports.
 *
 */

void ds3_ultra_ports(cbus)
    register mcitype *cbus;
{
    char                cmdchar, direction;
    char                buff[BUFLEN];
    boolean             parsed, send;
    int                 linect, i = 0;
    ushort              hold;
    
    cmdchar = '?';
    while (TRUE) {
        printf("\nDS3/Ultra special Port tests (?ABCDEFGHIJKLMNOPQSTUV) [%c]: ", cmdchar);
        parsed = (int) gets(buff);
        if (!null(buff) && parsed)
            cmdchar = cvu(buff[0]);
        switch (cmdchar) {
          case 'A':             /* multiport_ping */
            multiport_ping(cbus);
            break;
          case 'B':             /* One-Way_ping */
	    direction = 'S';
	    printf("\n Which direction, send or receive [%c]: ", direction);
	    parsed = (int) gets(buff);
	    if (!null(buff) && parsed)
		direction = cvu(buff[0]);
	    if (direction == 'S')
		send = TRUE;
	    else
		send = FALSE;
            one_way_ping(cbus, send);
            break;
          case 'C':             /* Bothways_test */
            dci_bothways_test(cbus);
            break;
          case 'D':
            printf("\n\nNew Starting packet length: [%d] ", pkt_start);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    pkt_start = parse_unsigned(buff);
            }
            break;
          case 'E':
            printf("\n\nNew Ending packet length: [%d] ", pkt_end);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    pkt_end = parse_unsigned(buff);
            }
            break;
          case 'F':         /* Loop Multiport Ping test */
            printf("\nRunning Multiport_ping test until BREAK...");
            verbose = reporting = FALSE;
	    looping = TRUE;
            while(TRUE) {
                (void)multiport_ping(cbus);
                if (abort)
                    break;
            }
            verbose = reporting = TRUE;
            looping = FALSE;
            break;
          case 'G':         /* Loop One-WAy Ping test */
            printf("\nRunning One-Way test until BREAK...");
	    direction = 'S';
	    printf("\n Which direction, send or receive [%c]: ", direction);
	    parsed = (int) gets(buff);
	    if (!null(buff) && parsed)
		direction = cvu(buff[0]);
	    if (direction == 'S')
		send = TRUE;
	    else
		send = FALSE;
            verbose = reporting = FALSE;
	    looping = TRUE;
            while(TRUE) {
                (void)one_way_ping(cbus, send);
                if (abort)
                    break;
            }
            verbose = reporting = TRUE;
            looping = FALSE;
            break;
          case 'H':             /* Display this menu */
          case '?':
            printf("\nA - Run Multiport Ping test once");
            printf("\nB - Run a One-Way Ping test once");
            printf("\nC - Run Bothways test once");
            printf("\nD - Change Starting Packet length");
            printf("\nE - Change Ending packet length");
            printf("\nF - Loop Multi-port test till BREAK");
            printf("\nG - Loop One-Way test till BREAK");
            printf("\n?H - Print this menu");
            printf("\nI - Loop Bothways test till BREAK");
            printf("\nJ - Repeat Multi-port test till BREAK or ERROR");
            printf("\nK - Repeat One-Way test till BREAK or ERROR");
            printf("\nL - Repeat Bothways test till BREAK or ERROR");
            printf("\nM - Run Fourways test once");
            printf("\nN - Loop Fourways test till BREAK");
            printf("\nP - Repeat Fourways test till BREAK or ERROR");
            printf("\nP - Change number of receive buffers to park under interface");
            printf("\nQ - Quit to Main Menu");
	    printf("\nS - Bothways Scan from start to ending length");
	    printf("\nT - Repeat Bothways Scan till BREAK or ERROR");
	    printf("\nU - Repeat Fourways Scan till BREAK or ERROR");
	    printf("\nV - Display all error counters");
	    break;
          case 'I':         /* Loop Bothways Ping test */
            printf("\nRunning Bothways_ping test until BREAK...");
            verbose = reporting = FALSE;
	    looping = TRUE;
            while(TRUE) {
                (void)dci_bothways_test(cbus);
                if (abort)
                    break;
            }
            verbose = reporting = TRUE;
            looping = FALSE;
            break;
          case 'J':         /* Repeat Multiport ping test till ERROR */
            printf("\nRunning Multiport Ping test until ERROR or BREAK...");
            verbose = reporting = TRUE;
            looping = FALSE;
	    i = 0;
            while (multiport_ping(cbus, &linect)) {
		i++;
                if (abort)
                    break;
	    }
	    printf("\nRan Multiport Ping test %d times.", i);
            break;
          case 'K':         /* Repeat One-Way ping test till ERROR */
            printf("\nRunning One-Way Ping test until ERROR or BREAK...");
	    direction = 'S';
	    printf("\n Which direction, send or receive [%c]: ", direction);
	    parsed = (int) gets(buff);
	    if (!null(buff) && parsed)
		direction = cvu(buff[0]);
	    if (direction == 'S')
		send = TRUE;
	    else
		send = FALSE;
            verbose = reporting = TRUE;
            looping = FALSE;
	    i = 0;
            while (one_way_ping(cbus, send)) {
		i++;
                if (abort)
                    break;
	    }
	    printf("\nRan One-Way Ping test %d times.", i);
            break;
          case 'L':         /* Repeat Bothways ping test till ERROR */
            printf("\nRunning Bothways Ping test until ERROR or BREAK...");
            verbose = reporting = TRUE;
            looping = FALSE;
	    i = 0;
            while (dci_bothways_test(cbus)) {
		i++;
                if (abort)
                    break;
	    }
	    printf("\nRan Bothways Ping test %d times.", i);
            break;
          case 'M':             /* Fourways_test */
            dci_fourways_test(cbus);
            break;
          case 'N':         /* Loop Fourways Ping test */
            printf("\nRunning Fourways_ping test until BREAK...");
            verbose = reporting = FALSE;
	    looping = TRUE;
            while(TRUE) {
                (void)dci_fourways_test(cbus);
                if (abort)
                    break;
            }
            verbose = reporting = TRUE;
            looping = FALSE;
            break;
          case 'O':         /* Repeat Fourways ping test till ERROR */
            printf("\nRunning Fourways Ping test until ERROR or BREAK...");
            verbose = reporting = TRUE;
            looping = FALSE;
	    i++;
            while (dci_fourways_test(cbus)) {
		i++;
                if (abort)
                    break;
	    }
	    printf("\nRan Fourways Ping test %d times.", i);
            break;
          case 'P':
            printf("\n\nNew number of receive buffers to park: [%d] ", park_buffers);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    park_buffers = parse_unsigned(buff);
		if ((park_buffers < 1) ||(park_buffers > 8)) {
		    printf("\nOnly 1-8 buffers allowed, changing to 3");
		    park_buffers = 3;
		}
            }
            break;
          case 'Q':             /* Quit */
            return;
            break;
	  case 'S':             /* Scan Bothways test */
	    hold = pkt_end;
	    i = 0;
	    for (pkt_end = pkt_start; pkt_end <= hold; pkt_end += pkt_skip) {
		i++;
		if ((!dci_bothways_test(cbus)) || abort)
		    break;
	    }
	    printf("\nRan Bothways Ping test %d times.", i);
	    pkt_end = hold;
	    break;
	  case 'T':             /* Repeat Scan Bothways test till Error */
	    hold = pkt_end;
	    pkt_end += pkt_skip;
	    i = 0;
	    do {
		if (pkt_end >= hold)
		    pkt_end = pkt_start;
		else
		    pkt_end += pkt_skip;
		i++;
	    }
	    while ((dci_bothways_test(cbus)) && !abort);
	    printf("\nRan Bothways Ping test %d times.", i);
	    pkt_end = hold;
	    break;
	  case 'U':             /* Repeat Scan Fourways test till Error */
	    hold = pkt_end;
	    pkt_end += pkt_skip;
	    i = 0;
	    do {
		if (pkt_end >= hold)
		    pkt_end = pkt_start;
		else
		    pkt_end += pkt_skip;
		i++;
	    }
	    while ((dci_fourways_test(cbus)) && !abort);
	    printf("\nRan Fourways Ping test %d times.", i);
	    pkt_end = hold;
	    break;
	  case 'V':             /* Display all error counters */
	    for (i = 0; i < FCI_MAX_DCI_CTRLRS; i++) {
		if (dcidata[i]->ctrlr_type == FCI_DS3_CONTROLLER) {
		    printf("\nError Counters for card %d", i);
		    ex_cbus_command(cbus, 3, i * 6);
		    display_error_counters(cbus);
		}
	    }
	    break;
        }  /*  end switch(cmdchar)  */
	abort = FALSE;
	
    }  /*  end while(TRUE)  */
    
}  /*  end ds3_ultra_ports()  */


/*
 * ds3_autotest
 * Automatic scan and test of DS3 units
 */

void ds3_autotest()
{
    int         unit, total, i;
    mcitype     *cBus;
    
    pkt_offset = DS3_OFFS;
    pkt_start = DS3_START;
    pkt_end = DS3_END;
    pkt_skip = DS3_SKIP;
    mema_start = DS3_MEMA_START;
    mema_end = DS3_MEMA_END;
    standard_packet = TRUE;
    ping_wait = 1000;
    park_buffers = 3;
    verbose = FALSE;
    reporting = FALSE;
    looping = FALSE;
    ping_loopback = FALSE;
    ds3_clock = TRUE;
    ds3_loop = 'A';
    dci_alloc_diag_mem();    
    init_error_struct(FCI_MAX_DCI_CTRLRS);
    cBus = cBus_findcontroller();
    if (cBus->unit < 0) {
        printf("\nCan't find cBus Controller!!\n");
        return;
    }
    dci_config(cBus, FCI_DS3_CONTROLLER);
    if (board_to_test < 0) {
        printf("\nNo HSCI cards present\n");
        return;
    }
    for (; board_to_test < FCI_MAX_DCI_CTRLRS && !abort; board_to_test++) {
        if ((dcidata[board_to_test]->ctrlr_unit == board_to_test) &&
            (cBus->interfacetype[board_to_test * 6] == FCI_TYPE_DS3) &&
            (dcidata[board_to_test]->ctrlr_type == FCI_DS3_CONTROLLER)) {
            dci_setup_unit(cBus);
            ds3_test(cBus, 1);
        }
    }
    show_dci_auto_results(cBus);
    abort = FALSE;
    
}  /*  end ds3_autotest()  */

void ultra_autotest()
{
    int         unit, total, i;
    mcitype     *cBus;
    
    pkt_offset = ULT_OFFS;
    pkt_start = ULT_START;
    pkt_end = ULT_END;
    pkt_skip = ULT_SKIP;
    mema_start = ULT_MEMA_START;
    mema_end = ULT_MEMA_END;
    standard_packet = TRUE;
    ping_wait = 2;
    park_buffers = 3;
    verbose = FALSE;
    reporting = FALSE;
    looping = FALSE;
    ping_loopback = FALSE;
    dci_alloc_diag_mem();    
    init_error_struct(FCI_MAX_DCI_CTRLRS);
    cBus = cBus_findcontroller();
    if (cBus->unit < 0) {
        printf("\nCan't find cBus Controller!!\n");
        return;
    }
    dci_config(cBus, FCI_DS3_CONTROLLER);
    if (board_to_test < 0) {
        printf("\nNo HSCI cards present\n");
        return;
    }
    for (; board_to_test < FCI_MAX_DCI_CTRLRS && !abort; board_to_test++) {
        if ((dcidata[board_to_test]->ctrlr_unit == board_to_test) &&
            (cBus->interfacetype[board_to_test * 6] == FCI_TYPE_ULTRA) &&
            (dcidata[board_to_test]->ctrlr_type == FCI_DS3_CONTROLLER)) {
            dci_setup_unit(cBus);
            ultra_test(cBus, 1);
        }
    }
    show_dci_auto_results(cBus);
    abort = FALSE;
    
}  /*  end ultra_autotest()  */

/*
 * ds3_experttest
 * Let an expert run the test.
 */

void ds3_experttest(cbus)
    register mcitype *cbus;
{
    long        passes;
    char        buff[BUFLEN];
    boolean     parsed;
    
    /*
     * Get number of passes
     */
    
    passes = 1;
    printf("\nPasses [%d]: ", passes);
    if (parsed = (int) gets(buff)) {
        if (!null(buff))
            passes = parse_unsigned(buff);
        parsed = (passes > 0);
    }
    if (!parsed) {
        printf("\n%% Bad pass count");
        return;
    }
    
    /*
     * Ask about verbose and error reporting modes.
     */
    
    printf("\nReport errors [y]: ");
    reporting = (gets(buff) && cvu(*buff) == 'N') ? FALSE : TRUE;
    if (reporting) {
        printf("\nVerbose mode [n]: ");
        verbose = (gets(buff) && cvu(*buff) == 'Y') ? TRUE : FALSE;
    } else
        verbose = FALSE;
    
    /*
     * Do the dirty work.
     */
    
    ds3_test_bothways(cbus, passes);
    
    if (!verbose)
        show_dci_auto_results(cbus);
    
}  /*  end ds3_experttest()  */

/*
 * ultra_experttest
 * Let an expert run the test.
 */

void ultra_experttest(cbus)
    register mcitype *cbus;
{
    long        passes;
    char        buff[BUFLEN];
    boolean     parsed;
    
    /*
     * Get number of passes
     */
    
    passes = 1;
    printf("\nPasses [%d]: ", passes);
    if (parsed = (int) gets(buff)) {
        if (!null(buff))
            passes = parse_unsigned(buff);
        parsed = (passes > 0);
    }
    if (!parsed) {
        printf("\n%% Bad pass count");
        return;
    }
    
    /*
     * Ask about verbose and error reporting modes.
     */
    
    printf("\nReport errors [y]: ");
    reporting = (gets(buff) && cvu(*buff) == 'N') ? FALSE : TRUE;
    if (reporting) {
        printf("\nVerbose mode [n]: ");
        verbose = (gets(buff) && cvu(*buff) == 'Y') ? TRUE : FALSE;
    } else
        verbose = FALSE;
    
    /*
     * Do the dirty work.
     */
    
    ultra_test_bothways(cbus, passes);
    
    if (!verbose)
        show_dci_auto_results(cbus);
    
}  /*  end ultra_experttest()  */

/*
 * ds3_test
 * Workhorse test routine.
 */

void ds3_test(cbus, passes)
    mcitype *cbus;
{
    int i, linect, size;
    
    linect = errors = 0;
    if (!verbose)
        printf("\nTesting DS3 card %d ...", board_to_test);
    while (passes--) {
        
        /*
         * Run mema tests
         */
        
        if (dci_mema_test(cbus, &linect) == FALSE)
            return;
        
        /*
         * Run memd tests
         */
        
        if (dci_memd_test(cbus, &linect) == FALSE)
            return;
        
        /*
         * Run interface diagnostics
         */
        
        (void)dci_ping_test(cbus, 0);
    }
    
}  /*  end ds3_test()  */

/*
 * ds3_test_bothways
 * Workhorse test routine.
 */

void ds3_test_bothways(cbus, passes)
    mcitype *cbus;
{
    int i, linect, size;
    
    linect = errors = 0;
    if (!verbose)
        printf("\nTesting DS3 card %d ...", board_to_test);
    while (passes--) {
        
        /*
         * Run mema tests
         */
        
        if (dci_mema_test(cbus, &linect) == FALSE)
            return;
        
        /*
         * Run memd tests
         */
        
        if (dci_memd_test(cbus, &linect) == FALSE)
            return;
        
        /*
         * Run interface diagnostics
         */
        
        (void)multiport_ping(cbus);
    }
    
}  /*  end ds3_test_bothways()  */

/*
 * ultra_test
 * Workhorse test routine.
 */

void ultra_test(cbus, passes)
    mcitype *cbus;
{
    int i, linect, size;
    
    linect = errors = 0;
    if (!verbose)
        printf("\nTesting UltraNet card %d ...", board_to_test);
    while (passes--) {
        
        /*
         * Run mema tests
         */
        
        if (dci_mema_test(cbus, &linect) == FALSE)
            return;
        
        /*
         * Run memd tests
         */
        
        if (dci_memd_test(cbus, &linect) == FALSE)
            return;
        
        /*
         * Run interface diagnostics
         */
        
        (void)dci_ping_test(cbus, 0);
    }
    
}  /*  end ultra_test()  */
/*
 * ultra_test_bothways
 * Workhorse test routine.
 */

void ultra_test_bothways(cbus, passes)
    mcitype *cbus;
{
    int i, linect, size;
    
    linect = errors = 0;
    if (!verbose)
        printf("\nTesting UltraNet card %d ...", board_to_test);
    while (passes--) {
        
        /*
         * Run mema tests
         */
        
        if (dci_mema_test(cbus, &linect) == FALSE)
            return;
        
        /*
         * Run memd tests
         */
        
        if (dci_memd_test(cbus, &linect) == FALSE)
            return;
        
        /*
         * Run interface diagnostics
         */
        
        (void)multiport_ping(cbus);
    }
    
}  /*  end ultra_test()  */

/*
 * ds3_setup
 *
 * ds3 specific port setup commands
 *
 */

#define CPLEN 40

boolean ds3_setup(cbus, unit, snd_loop, send, errors)
    mcitype *cbus;
    int unit;
    boolean snd_loop, send;
    int *errors;
{
    int j;
    ulong clear_packet[CPLEN / 4]
        = {0x12345678, CPLEN-16, 0x80000000, 0x1010202, 0x4040808,
               0x10102020, 0x40408080, 0xfefefdfd, 0xfbfbf7f7, 0xefefdfdf},
        cp_receive[10];
    
    (void) dci_command(cbus, FCI_HSCI_BUFFER_PARK, park_buffers);
    if (ds3_clock)
	dci_command(cbus, 21, 1); /* set internal clock mode */
    else
	dci_command(cbus, 21, 0); /* clear internal clock mode */

    cbus->regptr->argreg = MCI_IF_DISABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    switch (ds3_loop) {
      case 'A':
	dci_command(cbus, 39, 0); /* no strange loopback */
	if (ping_loopback) {
	    cbus->regptr->argreg = MCI_IF_LOOPBACK;
	    cbus->regptr->cmdreg = MCI_CMD_STATUS;
	}
	else {
	    cbus->regptr->argreg = MCI_IF_NOLOOPBACK;
	    cbus->regptr->cmdreg = MCI_CMD_STATUS;
	}
	break;
      case 'D':
	dci_command(cbus, 39, 3); /* local DTE loopback */	    
	cbus->regptr->argreg = MCI_IF_NOLOOPBACK;
	cbus->regptr->cmdreg = MCI_CMD_STATUS;
	break;
      case 'L':
	dci_command(cbus, 39, 1); /* local line loopback */	    
	cbus->regptr->argreg = MCI_IF_NOLOOPBACK;
	cbus->regptr->cmdreg = MCI_CMD_STATUS;
	break;
      case 'R':
	dci_command(cbus, 39, 2); /* romote line loopback */	    
	cbus->regptr->argreg = MCI_IF_NOLOOPBACK;
	cbus->regptr->cmdreg = MCI_CMD_STATUS;
	break;
    }    
    dci_command(cbus, 20, 1); /* set the TA bit) */
    cbus->regptr->argreg = MCI_IF_ENABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    if (cbus->regptr->cmdreg != MCI_RSP_OKAY) {
	errors++;
	if (reporting)
	    printf("\nDS3_setup: RX enable failed");
	return(FALSE);
    }
    delay(70);

/* clear out the hardware just in case */

    (void) send_setup(cbus, unit, clear_packet, cp_receive, CPLEN, 0);
    for (j = 0; j <= 14; j++)
	ex_cbus_command (cbus, MCI_CMD_ERRORS, j);
    return(TRUE);
}   /* end ds3_setup */

/*
 * ultra_setup
 *
 * ultra specific port setup commands
 *
 */

boolean ultra_setup(cbus, unit, errors)
    mcitype *cbus;
    int unit;
    int *errors;
{
    int j;
    ulong clear_packet[CPLEN / 4]
	= {0x12345678, CPLEN-16, 0x80000000, 0x1010202, 0x4040808,
	       0x10102020, 0x40408080, 0xfefefdfd, 0xfbfbf7f7, 0xefefdfdf},

	cp_receive[10];
    
    (void) dci_command(cbus, FCI_HSCI_BUFFER_PARK, park_buffers);
    (void) dci_command(cbus, FCI_HSCI_CRC_HACK, 1);
    dci_command(cbus, 42, 0);
    cbus->regptr->argreg = MCI_IF_ENABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    if (cbus->regptr->cmdreg != MCI_RSP_OKAY) {
	errors++;
	if (reporting)
	    printf("\nUltra_setup: RX enable1 failed");
	return(FALSE);
    }

    /*
     * Set interface loopback mode
     */
    
    cbus->regptr->argreg = MCI_IF_DISABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    if (ping_loopback) {
        cbus->regptr->argreg = MCI_IF_LOOPBACK;
        cbus->regptr->cmdreg = MCI_CMD_STATUS;
    }
    else {
        cbus->regptr->argreg = MCI_IF_NOLOOPBACK;
        cbus->regptr->cmdreg = MCI_CMD_STATUS;
    }
    cbus->regptr->argreg = MCI_IF_ENABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    if (cbus->regptr->cmdreg != MCI_RSP_OKAY) {
	errors++;
	if (reporting)
	    printf("\nUltra_setup: RX enable2 failed");
	return(FALSE);
    }

/* creat pool for break packet */

    ex_cbus_command(cbus, FCI_CMD_OVERHEAD, 0);
    ex_cbus_command(cbus, FCI_CMD_BUFFERSIZE, (BREAK_LENGTH / 2) + 10);
    cbus->regptr->argreg = BREAK_POOL;
    cbus->regptr->cmdreg = MCI_CMD_TX_ALLOCATE;
    if (cbus->regptr->cmdreg != MCI_RSP_OKAY) {
	if (reporting)
	    printf("\nUltra_setup: tx_allocate for Break failed - %#x", cbus->regptr->cmdreg);
	return(FALSE);
    }

/* send break packet */

    cbus->regptr->argreg = BREAK_POOL;
    cbus->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
    cbus->regptr->argreg = 0;
    cbus->regptr->cmdreg = MCI_CMD_TX0_SELECT;
    cbus->regptr->argreg = BREAK_LENGTH;
    cbus->regptr->cmdreg = FCI_CMD_TX0_ENQUEUE;
    (void) dci_command(cbus, 57, 0); /* send the break packet */
    delay (BREAK_WAIT); /* wait for the break packet to finish */

/* clear out the hardware just in case */

    (void) send_setup(cbus, unit, clear_packet, cp_receive, CPLEN, 0);
    for (j = 0; j <= 14; j++)
	ex_cbus_command (cbus, MCI_CMD_ERRORS, j);

    return(TRUE);
}   /* end ultra_setup */

/*
 * setup_both
 *
 * port setup commands for bothways test
 *
 */

boolean setup_both(cbus, board1, board2, type, errors)
    mcitype *cbus;
    int board1, board2;
    boolean type;
    int *errors;
{
    int board[2], i, j;
    ulong clear_packet[CPLEN / 4]
	= {0x12345678, CPLEN-16, 0x80000000, 0x1010202, 0x4040808,
	       0x10102020, 0x40408080, 0xfefefdfd, 0xfbfbf7f7, 0xefefdfdf},

	cp_receive[10];

    board[0] = board1;
    board[1] = board2;

/* allocate buffers for the break packets */
    if (!break_allocated)
	for (i = 0; i < 2; i++) {
	    ex_cbus_command(cbus, FCI_CMD_OVERHEAD, 0);
	    ex_cbus_command(cbus, FCI_CMD_BUFFERSIZE, (BREAK_LENGTH / 2) + 10);
	    cbus->regptr->argreg = BREAK_POOL;
	    cbus->regptr->cmdreg = MCI_CMD_TX_ALLOCATE;
	    if (cbus->regptr->cmdreg != MCI_RSP_OKAY) {
		if (reporting)
		    printf("\nsetup_both: tx_allocate for Break failed - %#x", cbus->regptr->cmdreg);
		return(FALSE);
	    }
	}
    break_allocated = TRUE;

/* set up the interfaces */
    for (i = 0; i < 2; i++) {
	dci_select(cbus, board[i], 0);
	(void) dci_command(cbus, FCI_HSCI_BUFFER_PARK, park_buffers);
	(void) dci_command(cbus, MCI_CMD_STATUS, MCI_IF_NOLOOPBACK);
	if (type == FCI_TYPE_DS3) {
	    if (ds3_clock)
		(void) dci_command(cbus, 21, 1); /* set internal clock mode */
	    else
		(void) dci_command(cbus, 21, 0); /* clear int clock mode */
	    dci_command(cbus, 39, 0); /* no strange loopback */
	    dci_command(cbus, 20, 1); /* set the TA bit) */
	}
	if (type == FCI_TYPE_ULTRA) { 
	    (void) dci_command(cbus, FCI_HSCI_CRC_HACK, 1);
	    (void) dci_command(cbus, 42, 0); /* clear command transmit */
	}
	cbus->regptr->argreg = MCI_IF_ENABLE;
	cbus->regptr->cmdreg = MCI_CMD_STATUS;
	if (cbus->regptr->cmdreg != MCI_RSP_OKAY) {
	    errors++;
	    if (reporting)
		printf("\nSetup_Both: RX enable1 failed, unit %d", board[i] * 6);
	    return(FALSE);
	if (type == FCI_TYPE_DS3)
	    delay(100);
	}
    }
    
/* send the break packets */
    if (type == FCI_TYPE_ULTRA) {
	for (i = 0; i < 2; i++) {
	    dci_select(cbus, board[i], 0);
	    cbus->regptr->argreg = BREAK_POOL;
	    cbus->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
	    cbus->regptr->argreg = 0;
	    cbus->regptr->cmdreg = MCI_CMD_TX0_SELECT;
	    cbus->regptr->argreg = BREAK_LENGTH;
	    cbus->regptr->cmdreg = FCI_CMD_TX0_ENQUEUE;
	    (void) dci_command(cbus, 57, 0); /* send the break packet */
	}
	delay (BREAK_WAIT); /* wait for the break packets to finish */
    }

/* send the clearing packets to clear out the hardware */
    for (i = 0; i < 2; i++) {
	dci_select(cbus, board[i], 0);
	(void) send_setup(cbus, board[i] * 6, clear_packet, cp_receive, CPLEN, 0);
    }

/* clear the error counters in case they saw anything */
    for (i = 0; i < 2; i++) {
	dci_select(cbus, board[i], 0);
	for (j = 0; j <= 14; j++)
	    ex_cbus_command (cbus, MCI_CMD_ERRORS, j);
    }
    return(TRUE);
}   /* end setup_both */

short send_setup(cbus, unit, packet, rpacket, len, offset)
    mcitype *cbus;
    int     unit, len;
    ulong   *packet;
    ulong   *rpacket;
    ushort  offset;
{
    ushort      sresult = 0, rstatus, rlength, freebuffer = 0, new_loc_0,
    packetr = FALSE;
    int         waiting, i;
    ulong	*rec_packet;
    
    /*
     * Set pointer to start of buffer (TX0_SELECT).
     */
    
    cbus->regptr->argreg = POOL0;
    cbus->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
    if ((sresult = cbus->regptr->cmdreg) != MCI_RSP_OKAY) {
        printf("\nping: TX0_RESERVE failed - %#x", sresult);
        return(-1);
    }
    
    cbus->regptr->argreg = 0;
    cbus->regptr->cmdreg = MCI_CMD_TX0_SELECT;
    sresult = cbus->regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
        errors++;
        if (reporting)
            printf("\nping: TX0_SELECT failed, result = 0x%x", sresult);
        return(-1);
    }
    
    /*
     * Put packet into transmit buffer.
     * Then write byte length and start transmission.
     */
    
    for(i = 0; i <= len; i += 4)
        cbus->regptr->write0long = *packet++;
    
    packet_length = len;
    if (cbus->interfacetype[unit] == MCI_TYPE_ETHER)
        cbus->regptr->argreg = len | MCI_TX_HEARSELF;
    else
        cbus->regptr->argreg = len;
    
    cbus->regptr->cmdreg = MCI_CMD_TX0_START;
    
    /*
     * Wait for packet to come back.
     *
     */
    
    waiting = ping_wait + (len * 2);
    while (waiting--) {
        cbus->regptr->cmdreg = MCI_CMD_RX_SETUP;
        sresult = cbus->regptr->argreg;
        if (sresult != 0xFFFF) {
	    pkt_recvd = packetr = TRUE;
	    cbus->regptr->argreg = 0;
	    cbus->regptr->cmdreg = MCI_CMD_RX_SELECT;
	    rec_packet = rpacket;
            for(i = 0; i < (len + (2 * offset)); i += 4)
        	*rec_packet++ = cbus->regptr->readlong;
            cbus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
        }
    }
    return(packetr);
}  /*  end send_setup()  */

/*
 *  display_error_counters
 *  prints a display with all the ds3-ultra error counters
 */

void display_error_counters(cbus)
    mcitype     *cbus;
{
    int         i;
    ushort      sresult, rstatus;
    
    for(i = 0; i <= 13; i++){
        cbus->regptr->argreg = i;
        cbus->regptr->cmdreg = MCI_CMD_ERRORS;
	if ((rstatus = cbus->regptr->cmdreg) != MCI_RSP_OKAY) {
	    printf("\nError: ERROR_COMMAND failed, response 0x%-04x", rstatus);
	    return;
	}
	sresult = cbus->regptr->argreg;
	switch (i) {
	  case 0:
	    printf("\nGIANTS:         %-4d", sresult);
	    break;
	  case 1:
	    printf("    NO_BUFFERS:     %-4d", sresult);
	    break;
	  case 4:
	    printf("    OVERRUNS:       %-4d", sresult);
	    break;
	  case 5:
	    printf("\nCRC_ERRORS:     %-4d", sresult);
	    break;
	  case 8:
	    printf("    UNDERFLOWS:     %-4d", sresult);
	    break;
	  case 11:
	    printf("    APPLIQUE ERRORS:%-4d", sresult);
	    break;
	  case 12:
	    printf("\nPARITY_ERRORS:  %-4d", sresult);
	    break;
	  case 13:
	    printf("    RX_THROTTLE:    %-4d", sresult);
	    break;
	}
    }
}

/*
 *  xmit_loop
 *  uses pkt_start for packet length and continuously tries
 *  to xmit the packet.  Does no error checking and doesn't care
 *  if the packet is received.
 */

void xmit_loop(cbus, unit)
    mcitype     *cbus;
    int         unit;
{
    register uchar      *tpacket, *rpacket;
    register int        i,j;
    boolean             parsed, next;
    ushort                      length, data, address, sresult, if_type;
    char                        cmdchar, buff[BUFLEN];
    int                         waiting;
    
    tpacket = packet1;
    rpacket = packet2;
    
    /*  get packet length to xmit  */
    
    cmdchar = 'Y';
    length = pkt_start;
    printf("\nEnter packet length: [%d]", length);
    if (parsed = (int) gets(buff)) {
        if (!null(buff))
            length = parse_unsigned(buff);
    }
    
    /*  set up the transmit buffer  */
    
    cmdchar = 'Y';
    printf("\nFill transmit buffer with default data? [%c]", cmdchar);
    parsed = (int) gets(buff);
    if (!null(buff) && parsed)
        cmdchar = cvu(buff[0]);
    if (cmdchar != 'Y') {
        get_number(&data, "\nEnter data pattern: ", 0, 255);
        for (i = 0; i < length; i++)
            tpacket[i] = (uchar)data;
    }
    else {
        for (i = 0; i < length; i++) {
            j = i & 0x3f;
            if (j < 16)
                tpacket[i] = (1 << (j/2));
            else if (j < 32)
                tpacket[i] = ((1 << ((j & 0xf) / 2)) ^ 0xff);
            else if ((j & 2) == 0)
                tpacket[i] = 0x55;
            else
                tpacket[i] = 0xaa;
            
        }  /*  end for(i)  */
    }
    
    /*  allow buffer to be edited  */
    
    cmdchar = 'N';
    printf("\nDo you wish to edit the transmit buffer? [%c]", cmdchar);
    parsed = (int) gets(buff);
    if (!null(buff) && parsed)
        cmdchar = cvu(buff[0]);
    next = TRUE;
    while ((cmdchar == 'Y') && (address <= length)) {
        get_number(&address, "\nEnter offset into buffer: ", 0, length);
        while ((next) && (address <= length)) {
            printf("\noffset = %d, data is %x, new data = ", address, tpacket[(int)address]);
            get_number(&data, "", 0,255);
            tpacket[(int)address] = (uchar)data;
            printf("\nEdit next byte? [%c]", cmdchar);
            parsed = (int) gets(buff);
            if (!null(buff) && parsed) 
                cmdchar = cvu(buff[0]);
            if (cmdchar == 'Y') {
                address += 1;
                continue;
            }
            else {
                next = FALSE;
                cmdchar = 'Y';
            }
            
        }  /*  end while(next)  */
        
        printf("\nEdit another location? [%c]", cmdchar);
        parsed = (int) gets(buff);
        if (!null(buff) && parsed) 
            cmdchar = cvu(buff[0]);
        if (cmdchar == 'Y') {
            next = TRUE;
            continue;
        }
        else
            break;
        
    }  /*  end while(cmdchar)  */
    
    reporting = abort = verbose = FALSE;
    cbus_reset(cbus);           /*  reset everything  */
    dci_soft_reset(cbus);
    dci_allocate_buffers(cbus, board_to_test, unit, 0, pkt_end, 7, pkt_offset, 1, 4);  /*  get some buffers  */
    
    /*  now we loop until break  */
    
    printf("\nTransmitting packets until BREAK....");
    while (!abort) {
        cbus->regptr->cmdreg = MCI_CMD_RX_SETUP;        /*  just in case...  */
        if ((sresult = cbus->regptr->argreg) != 0xFFFF)
            cbus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
        cbus->regptr->argreg = POOL0;
        cbus->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
        if ((sresult = cbus->regptr->cmdreg) != MCI_RSP_OKAY) {
	    if (reporting) {
		printf("\nloop: TX0_RESERVE failed - %#x", sresult);
		printf("\nRAN OUT OF BUFFERS!!!");
	    }
	    return;
	}
	if_type = cbus->interfacetype[board_to_test * FCI_MAX_CTRLR_INTERFACES + unit];
	switch (if_type) {
	  case FCI_TYPE_ULTRA:
	    ultra_setup(cbus, board_to_test, FALSE, FALSE,
			&errors);
	    break;
	  case FCI_TYPE_DS3:
	    ds3_setup(cbus, board_to_test, FALSE, FALSE,
		      &errors);
	    break;
	    
	  default:
	    if (reporting)
		printf("\nPING%d ERROR: Bad interface type.",
		       board_to_test * FCI_MAX_CTRLR_INTERFACES + unit );
	    break;
	    
	} /* end switch (if_type) */
	
	cbus->regptr->argreg = 0;
	cbus->regptr->cmdreg = MCI_CMD_TX0_SELECT;
	
	/*  put packet into xmit buffer  */
	
	fill_xmit_buff(cbus, length, tpacket);
	
	/*  let 'er rip...  */
	
	cbus->regptr->argreg = length;
	cbus->regptr->cmdreg = MCI_CMD_TX0_START;
	waiting = ping_wait + (length * 2);
	while (waiting--)
	    if (abort)
		break;
	
    }  /*  end while(!abort)  */
    
    reporting = verbose = TRUE;
    
}  /*  end xmit_loop()  */

/*
 *  fill_xmit_buff
 */

void fill_xmit_buff(cbus, len, packet)
    mcitype *cbus;
    int len;
    ulong *packet;
{
    int i;
    
    for (i=0; i <= len; i += 4)
        cbus->regptr->write0long = *packet++;
    
}  /*  end fill_xmit_buff  */

/*
 * Applique
 *
 * By Bruce Sherry, Access Associates Inc.
 * January 30, 1991
 *
 * Test appliques
 *
 * routines:
 *  dci_pingit: sends and receives 1 packet on the specified ports.
 *  ping_ports: calls dci_pingit to send all packet sizes.
 *  dci_ports_on: sets the clocks on the specified port to the specified
 *    divisor.
 *  multiport_ping: Main routine, determins which ports to use, which types
 *    of applique to test, and so forth.
 *
 */

int aborts;

/*   
 * dci_pingit
 *
 * Sends one packet on the "out" port, and receives it on the "in" port.
 * Compares data and lengths to be sure it got the right packet.
 *
 * Parameters:
 *      cBus: interface board to use.
 *      out: port to send the packets from.
 *      in:  port to receive the packets from.
 *  packet: pointer to the data to send.
 *      rpacket: pointer to the place to put the received packet.
 *      len: Length of the packet.
 *      report: Print error messages if errors happen.
 * 
 * Returns:
 *      True if things went OK.
 *      False if not.
 *
 */

boolean dci_pingit(cBus, out, in, tpacket, rpacket, len, talking)
    mcitype *cBus;
    int out, in, len;
    ulong *tpacket;
    ulong *rpacket;
    boolean talking;
{
    ushort sresult, rstatus = 0;
    int waiting, rlength = 0, i;
    uchar *sent, *recvd;
    
    sent = (uchar*) tpacket;
    recvd = (uchar*) rpacket;
    cBus->regptr->argreg = out;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    cBus->regptr->argreg = 0;
    cBus->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
    sresult = cBus->regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
        if (talking) {
            printf("\nMultiport_ping: TX0_RESERVE failed - %#x", sresult);
            printf("\n  Possible output hang on interface %d", out);
        }
        return(FALSE);
    }
    
    /*
     * Set pointer to start of buffer (TX0_SELECT).
     */
    
    cBus->regptr->argreg = 0;
    cBus->regptr->cmdreg = MCI_CMD_TX0_SELECT;
    
    /*
     * Put packet into transmit buffer.
     * Then write byte length and start transmission.
     */
    
    for(i = 0; i <= len; i += 4)
        cBus->regptr->write0long = *tpacket++;
    cBus->regptr->argreg = out;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    cBus->regptr->argreg = len;
    cBus->regptr->cmdreg = MCI_CMD_TX0_START;
    
    /*
     * Wait for packet to come back.
     *
     */
    
    cBus->regptr->argreg = in;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    waiting = (2 * len) + ping_wait;
    while (waiting--) {
        cBus->regptr->cmdreg = MCI_CMD_RX_SETUP;
        sresult = cBus->regptr->argreg;
        rlength = cBus->regptr->rxlength;
        rstatus = (cBus->regptr->rxstatus & MCI_SX_MASK);    
        if (sresult != 0xFFFF) {
            if ((rlength == 0) || ((rstatus & MCI_SX_ABORT) == MCI_SX_ABORT)) {
                cBus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
                aborts++;
            }
            else
                break;
        }
    }
    if (waiting <= 0){
        if (talking)
            printf("\nMultiport_ping Error: Packet length %d sent, but not received!!!",
		   len);
        pkt_recvd = FALSE;
        return(FALSE);
    }
    
    if (sresult != in) {
        if (talking)
            printf("\nMultiport_ping Error: Packet received on port%x in error, SB %x",
                   sresult, in);
        return(FALSE);
    }
    /*
     * Read packet if it comes back.
     */
    
    pkt_recvd = TRUE;
    cBus->regptr->argreg = 0;
    cBus->regptr->cmdreg = MCI_CMD_RX_SELECT;
    for(i = 0; i <= len; i += 4)
        *rpacket++ = cBus->regptr->readlong;
    cBus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
    if (rlength != len) {
	
        if (talking) {
            printf("\nMultiport_ping Error: packet length sent : %d, received %d",
                   len, rlength);
            printf("\n                Argreg = 0x%x, receive status = 0x%x",
                   sresult, rstatus);
        }
        for (i = 0; i < len; i++) {
            if (recvd[i] != sent[i]) {
                if (talking)
                    printf("\n                Data Miscompare on byte %d, is 0x%x, SB 0x%x", i, recvd[i], sent[i]);
                break;
	    }
        }
        return(FALSE);
    }
    if (rstatus != 0) {
        if (talking)
            printf("\nMultiport_ping Error: receive status = 0x%x, should be 0.",
                   rstatus);
        return(FALSE);
    }
    return(TRUE);
}

/*
 * ping_ports
 *
 * This routine sends a series of packets out the "out" port, expecting them
 * to come back on the "in" port.
 *
 * Parameters:
 *      cBus: interface board to use.
 *      out: port to send the packets from.
 *      in:  port to receive the packets from.
 * 
 * Returns:
 *      True if things went OK.
 *      False if not.
 *
 */

boolean ping_ports(cBus, out, in)
    mcitype *cBus;
    int out, in;
    
{
    uchar *tpacket;
    uchar *rpacket;
    int i, j, step;
    
    tpacket = packet1;
    rpacket = packet2;
    
    if (standard_packet) {
	for(i = 0; i < MAXPACKET; i++) {
	    j = i & 0x3f;
	    if (j < 16)
		tpacket[i] = (1 << (j / 2));
	    else if (j < 32)
		tpacket[i] = ((1 << ((j & 0xF) / 2)) ^ 0xFF);
	    else if ((j & 2) == 0)
		tpacket[i] = 0x55;
	    else
		tpacket[i] = 0xAA;
	}
    }
    else
        for (i = 0; i < MAXPACKET; i++)
            tpacket[i] = custom_packet[i];
    
    if (((pkt_end - pkt_start) / 0x20) <= 52)
	step = 0x1f;
    else
	step = 0xff;
    
    for(i = pkt_start; i <= pkt_end && !abort; i += pkt_skip) {
        if (!dci_pingit(cBus, out, in, tpacket, rpacket, i, verbose)) {
            if (verbose) {
                printf("\nMultiport_ping ERROR: - did not rcv expected packet");
                printf(": sent packet length %d", i);
	    }
            return(FALSE);
	}
	
        for (j = 0; j < i; j++) {
            if (rpacket[j] != tpacket[j]) {
                if (verbose)
                    printf("\nMultiport_ping Error: Data Miscompare on byte %d, is 0x%x, SB 0x%x",
		           i, rpacket[j], tpacket[j]);
                return(FALSE);
            }
            
        }
	
        if (verbose) /* got a good packet, print ! */
	    if (i == pkt_start)
		printf("\nMultiport_ping: send on %d - ", out);
	    else if ((((i - pkt_start) & step) == 0) || (i == pkt_end))
		printf("!");
    }
    return(TRUE);
}

/*
 * multiport_ping
 *
 * This routine checks that a board can SEND, RECEIVE, and LOOPBACK
 * by being connected to another board to use as a reference.  The
 * reference board is in a higher numbered slot from the board_to_test.
 */

boolean multiport_ping(cBus)
    mcitype *cBus;
    
{
    int app1, app2;
    int port1, port2;
    int standard_board, i;
    char buf[10];
    uchar *tpacket;
    uchar *rpacket;
    ushort buf_cnt, tx_rx_cnt;
    MCI_ERROR *test = NULL;
    
    char  send = 'N';         /* Send status                          */
    char  recv = 'N';         /* Receive status                       */
    char  lcllp = 'N';        /* Loop Command local loop              */
    char  lcrlp = 'N';        /* Loop Command remote loop             */
    char  rcllp = 'N';        /* Receive Loop local loop              */
    char  rcrlp = 'N';        /* Receive Loop remote loop             */
    char  both = 'N';         /* Bothways Test Status                 */
    
    break_allocated = FALSE;
    aborts = 0;
    port1 = board_to_test * FCI_MAX_CTRLR_INTERFACES;
    test = &diag_results[board_to_test]->serial_ping_test[port1];
    standard_board = board_to_test + 1;
    port2 = -1;
    for (; standard_board < FCI_MAX_DCI_CTRLRS; standard_board++)
        if (dcidata[standard_board]->ctrlr_type == FCI_DS3_CONTROLLER) {
            port2 = standard_board * FCI_MAX_CTRLR_INTERFACES;
            break;
        }
    if (port2 < 0) {
        printf("\nOnly 1 HSCI found, Need 2 HSCIs");
        return(FALSE);
    }    break_allocated = FALSE;

    
    app1 = cBus->interfacetype[port1];
    app2 = cBus->interfacetype[port2];
    
    cbus_reset(cBus);           /*  reset everything  */
    dci_soft_reset(cBus);
    
    buf_cnt = ((PACKET_SPACE / (pkt_end + 25 + 20)) / 2) * 2;
    tx_rx_cnt = buf_cnt / 2;
    
    dci_allocate_buffers(cBus, board_to_test, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt);
    dci_allocate_buffers(cBus, standard_board, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt);
    
    /*
     * Check transmit
     */
    
    if (!setup_both(cBus, port1 / FCI_MAX_CTRLR_INTERFACES,
	       port2 / FCI_MAX_CTRLR_INTERFACES, app1, &errors))
	return(FALSE);
    
    if (ping_ports(cBus, port1, port2))
	send = 'P';
    else {
	send = 'F';
        if (!looping)
            (void)store_error(test, 0, 0, 0, 1, 0, 0, 9);
    }
    
    /*
     * Check receive
     */
    
    if (send == 'P') {
	if (ping_ports(cBus, port2, port1))
	    recv = 'P';
	else {
	    recv = 'F';
	    if (!looping)
		(void)store_error(test, 0, 0, 0, 1, 0, 0, 10);
	}
    }
    
    /*
     * Check bothways at once
     */
    
    if ((send == 'P') && (recv == 'P')) {
	if (dci_bothways(cBus, port1, port2, pkt_end, reporting, tx_rx_cnt, tx_rx_cnt))
	    both = 'P';
	else {
	    both = 'F';
	    if (!looping)
		(void)store_error(test, 0, 0, 0, 1, 0, 0, 12);
	}
    }

    /*
     * Check loopback
     */
    
    if ((send == 'P') && (recv == 'P') && (both == 'P')) {
	dci_select(cBus, board_to_test, 0);
	ex_cbus_command(cBus, MCI_CMD_STATUS, 2);

	if(ping_ports(cBus, port1, port1))
	    lcllp = 'P';
	else {
	    lcllp = 'F';
	    if (!looping)
	        (void)store_error(test, 0, 0, 0, 1, 0, 0, 11);
	}
    }
    
    if (verbose) {
        printf("\n\nFinal Status:\n    Send Receive Bothways Loopback");
        printf("\n      %c     %c       %c        %c\n", send, recv, both, lcllp);
    }
    if ((send == 'F') || (recv == 'F') || (lcllp == 'F') || (both == 'F'))
	return(FALSE);
    test->stat = STAT_PASS;
    return (TRUE);
    
} /* end multiport_ping */


/*
 * one_way_ping
 *
 * Use the full duplex Multiport ping setup to send data in just 1 direction.
 *
 */

boolean one_way_ping(cBus, send)
    mcitype *cBus;
    boolean send;
{
    int app1, app2;
    int port1, port2;
    int standard_board, i;
    uchar *tpacket;
    uchar *rpacket;
    ushort buf_cnt, tx_rx_cnt;
    MCI_ERROR *test = NULL;
    boolean test_stat = FALSE;
    break_allocated = FALSE;
    
    aborts = 0;
    port1 = board_to_test * FCI_MAX_CTRLR_INTERFACES;
    test = &diag_results[board_to_test]->serial_ping_test[port1];
    standard_board = board_to_test + 1;
    port2 = -1;
    for (; standard_board < FCI_MAX_DCI_CTRLRS; standard_board++)
        if (dcidata[standard_board]->ctrlr_type == FCI_DS3_CONTROLLER) {
            port2 = standard_board * FCI_MAX_CTRLR_INTERFACES;
            break;
        }
    if (port2 < 0) {
        printf("\nOnly 1 HSCI found, Need 2 HSCIs");
        return(FALSE);
    }
    
    app1 = cBus->interfacetype[port1];
    app2 = cBus->interfacetype[port2];
    
    cbus_reset(cBus);           /*  reset everything  */
    dci_soft_reset(cBus);
    
    buf_cnt = ((PACKET_SPACE / (pkt_end + 25 + 20)) / 2) * 2;
    tx_rx_cnt = buf_cnt / 2;
    
    dci_allocate_buffers(cBus, board_to_test, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt);
    dci_allocate_buffers(cBus, standard_board, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt);
    
    if (!setup_both(cBus, port1 / FCI_MAX_CTRLR_INTERFACES,
	       port2 / FCI_MAX_CTRLR_INTERFACES, app1, &errors))
	return(FALSE);
    
    if (send) {
	/*
	 * Check transmit
	 */
	
	if (ping_ports(cBus, port1, port2))
	    test_stat = TRUE;
	else {
	    if (!looping)
		(void)store_error(test, 0, 0, 0, 1, 0, 0, 9);
	}
    }
    else {
	
	/*
	 * Check receive
	 */
	
	if (ping_ports(cBus, port2, port1))
	    test_stat = TRUE;
	else {
	    if (!looping)
		(void)store_error(test, 0, 0, 0, 1, 0, 0, 10);
	}
    }
    if (test_stat) {
	test->stat = STAT_PASS;
	if (verbose)
	    printf("\nOne-Way test passed!!");
    }
    return (test_stat);
    
} /* end one_way_ping */

/*
 * dci_bothways_test
 *
 * Run bothways stand-alone.
 *
 * Parameters:
 *  cBus:       MCI device to use.
 * Returns:     True if ok, False if not.
 */

boolean dci_bothways_test(cBus)
    mcitype *cBus;
    
{
    int app1, app2;
    int port1, port2;
    int standard_board, i;
    char buf[10];
    uchar *tpacket;
    uchar *rpacket;
    ushort buf_cnt, tx_rx_cnt;
    break_allocated = FALSE;
    
    port1 = board_to_test * FCI_MAX_CTRLR_INTERFACES;
    standard_board = board_to_test + 1;
    port2 = -1;
    for (; standard_board < FCI_MAX_DCI_CTRLRS; standard_board++)
        if (dcidata[standard_board]->ctrlr_type == FCI_DS3_CONTROLLER) {
            port2 = standard_board * FCI_MAX_CTRLR_INTERFACES;
            break;
        }
    if (port2 < 0) {
        printf("\nOnly 1 HSCI found, Need 2 HSCIs");
        return(FALSE);
    }
    
    app1 = cBus->interfacetype[port1];
    app2 = cBus->interfacetype[port2];

    cbus_reset(cBus);           /*  reset everything  */
    
    dci_soft_reset(cBus);
    
    buf_cnt = ((PACKET_SPACE / (pkt_end + 25 + 20)) / 2) * 2;
    tx_rx_cnt = buf_cnt / 2;
    
    dci_allocate_buffers(cBus, board_to_test, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt + 1);
    dci_allocate_buffers(cBus, standard_board, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt + 1);
    
    if (!setup_both(cBus, board_to_test, standard_board, app1, &errors))
	return(FALSE);
    
    if (!dci_bothways(cBus, port1, port2, pkt_end, reporting, tx_rx_cnt, tx_rx_cnt))
        return(FALSE);
    if (verbose) {
        printf("\nBothways test passed");
    }
    return (TRUE);
    
} /* end bothways_test() */

/*
 * dci_fourways_test
 *
 * Run fourways stand-alone.
 *
 * Parameters:
 *  cBus:       MCI device to use.
 * Returns:     True if ok, False if not.
 */

boolean dci_fourways_test(cBus)
    mcitype *cBus;
    
{
    int port0 = 0, port1 = 6, port2 = 12, port3 = 18, app1;
    int board, board_0 = 0, board_1 = 1, board_2 = 2, board_3 = 3, i;
    char buf[10];
    uchar *tpacket;
    uchar *rpacket;
    ushort buf_cnt, tx_rx_cnt;
    break_allocated = FALSE;

    for (board = 0; board < FCI_MAX_DCI_CTRLRS; board++)
        if (dcidata[board]->ctrlr_type != FCI_DS3_CONTROLLER) {
	    printf("\nNot enough HSCIs found, Need 4 HSCIs");
	    return(FALSE);
        }

    app1 = cBus->interfacetype[port1];

    cbus_reset(cBus);           /*  reset everything  */
    
    dci_soft_reset(cBus);
    
    buf_cnt = ((PACKET_SPACE / (pkt_end + 25 + 20)) / 4) * 2;
    tx_rx_cnt = buf_cnt / 2;
    
    dci_allocate_buffers(cBus, board_0, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt + 1);
    dci_allocate_buffers(cBus, board_1, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt + 1);
    dci_allocate_buffers(cBus, board_2, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt + 1);
    dci_allocate_buffers(cBus, board_3, 0, 0, pkt_end, buf_cnt, pkt_offset, tx_rx_cnt, tx_rx_cnt + 1);
    
    if (!setup_both(cBus, board_0, board_1, cBus->interfacetype[board_0 * 6], &errors))
	return(FALSE);
    if (!setup_both(cBus, board_2, board_3, cBus->interfacetype[board_2 * 6], &errors))
	return(FALSE);
    
    if (!dci_fourways(cBus, pkt_end, reporting, tx_rx_cnt, tx_rx_cnt))
        return(FALSE);
    if (verbose) {
        printf("\nBothways test passed");
    }
    return (TRUE);
    
} /* end fourways_test() */

/*
 * enqueue
 *
 * put packets into the queue for transmission on TX0, with ENQUE
 *  after each one.  Does not issue TX0_START!!
 *
 * Parameters:
 *  cBus:       MCI device to use.
 *  port:       Interface to use.
 *  count:      of packets to put on the queue.
 *  packet:     pointer to packet data.
 *  length:     of packet to move.
 *  sent:       count of packets sent.
 *
 * Returns:     True if ok, False if not.
 */

boolean enqueue(cBus, port, count, packet, length, sent, talking)
    int port;
    mcitype *cBus;
    int count, *sent;
    ulong *packet;
    ushort length;
    boolean talking;
    
{
    int i;
    ulong *tpacket;
    ushort sresult, j;
    
    if (debug_level == 14)
        printf("\nEnqueueing buffers on port %d - ", port);
    cBus->regptr->argreg = port;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    for (j = 1; count > 0; count--) {
	tpacket = packet;
        cBus->regptr->argreg = 0;
	cBus->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
	sresult = cBus->regptr->cmdreg;
	if (sresult != MCI_RSP_OKAY) {
	    if (talking) {
		printf("\nMultiport_ping: TX0_RESERVE failed - %#x", sresult);
		printf("\n  Possible output hang on interface %d", port);
	    }
	    return(FALSE);
	}
        
	/*
	 * Set pointer to start of buffer (TX0_SELECT).
	 */
        
	cBus->regptr->argreg = 0;
	cBus->regptr->cmdreg = MCI_CMD_TX0_SELECT;
        
	/*
	 * Put packet into transmit buffer.
	 */
	if (standard_packet) {
	    cBus->regptr->write0short = ((port / 6) << 12) + j++;
	    cBus->regptr->write0short = length;
	    tpacket++;
	}
	else
	    cBus->regptr->write0long = *tpacket++;
	for(i = 4; i <= length; i += 4)
	    cBus->regptr->write0long = *tpacket++;
	/*
	 * Enqueue the transmit buffer, don't send it.
	 */
        
	cBus->regptr->argreg = length;
	cBus->regptr->cmdreg = FCI_CMD_TX0_ENQUEUE;
	*sent += 1;
        if (debug_level == 14)
	    printf("%d ", count);
    }
    return (TRUE);
}   /* end enqueue */


/*   
 * dci_bothways
 *
 * Sent some packets in both directions at once from two separate interfaces
 *
 * Parameters:
 *  cbus:       MCI device to use
 *  port1:      Port on board to be tested.
 *  port2:      Standard board interface.
 *  len:        Length of packet to use.
 *  talking:  Report errors or not.
 *  at_once:    Number of packets to start transmitting on one interface.
 *  cnt_each_way:   Total number of packets to send in each direction.
 *
 * Returns:     True if pass, False if fail.
 */

boolean dci_bothways(cBus, port1, port2, len, talking, at_once, cnt_each_way)
    mcitype *cBus;
    int port1, port2, len, at_once, cnt_each_way;
    boolean talking;
{
    ulong *tpacket;
    ulong *rpacket;
    ushort sresult, rlength = 0, rstatus = 0, *recvds;
    int waiting, bad_packets = 0, i, j;
    int sent_on_1 = 0, sent_on_2 = 0;
    int recvd_on_1 = 0, recvd_on_2 = 0;
    uchar *sent, *recvd;
    
    tpacket = (ulong *) packet1;
    rpacket = (ulong *) packet2;
    if (verbose)
	printf("\nBothways - ");
    
    /*
     * Setup transmit data
     */
    
    packet_length = pkt_end;
    sent = (uchar*) tpacket;
    if (standard_packet) {
				/* Standard data now includes a header
				 * which has:
				 * port number in the first byte
				 * packet number in the second byte
				 * packet length in the next word.
				 */
	tpacket[0] = 0;
	for(i = 4; i < packet_length; i++) {
	    j = (i - 4) & 0x3f;
	    if (j < 16)
		sent[i] = (1 << (j / 2));
	    else if (j < 32)
		sent[i] = ((1 << ((j & 0xF) / 2)) ^ 0xFF);
	    else if ((j & 2) == 0)
		sent[i] = 0x55;
	    else
		sent[i] = 0xAA;
	}
    }
    else
        for (i = 0; i < packet_length; i++)
            tpacket[i] = custom_packet[i];
    
    recvd = (uchar*) rpacket;
    recvds = (ushort*) rpacket;
    if (!enqueue(cBus, port1, at_once, tpacket, len, &sent_on_1, talking))
        return(FALSE);
    if (!enqueue(cBus, port2, at_once, tpacket, len, &sent_on_2, talking))
        return(FALSE);
    
    /*
     * Start transmission on both ports
     */
    
    cBus->regptr->argreg = board_to_test;
    cBus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    cBus->regptr->argreg = port1;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    dci_command(cBus, FCI_CTRLR_TX_START, 0);
    cBus->regptr->argreg = port2 / 6;
    cBus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    cBus->regptr->argreg = port2;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    dci_command(cBus, FCI_CTRLR_TX_START, 0);
    
    /*
     * Wait for packets to come back.
     *
     */
    
    waiting = ping_wait + (len * 2);
    while (((recvd_on_1 < cnt_each_way) || (recvd_on_2 < cnt_each_way)) &&
           (waiting > 0) && (bad_packets < cnt_each_way) && !abort) {
        cBus->regptr->cmdreg = MCI_CMD_RX_SETUP;
        sresult = cBus->regptr->argreg;
        if (sresult == 0xFFFF)
            waiting--;
        else {
            rlength = cBus->regptr->rxlength;
            rstatus = (cBus->regptr->rxstatus & MCI_SX_MASK);

	    /*
	     * Read the packet
	     */

	    cBus->regptr->argreg = 0;
	    cBus->regptr->cmdreg = MCI_CMD_RX_SELECT;

	    if (talking) {
		for (i = 0; i <= len; i += 4)
		    rpacket [i/4] = cBus->regptr->readlong;
		if ((sresult != port1) && (sresult != port2)) {
		    if (talking) {
			printf("\nMultiport_ping Error: packet received on port %d in error",
			       sresult);
			printf("\n      length = %d, Status = 0x%-04x", rlength, rstatus);
			
			printf("\n      sent%d: %d, %d: %d, received%d: %d, %d: %d",
			       port1, sent_on_1, port2, sent_on_2, port1,
			       recvd_on_1, port2, recvd_on_2);
			printf("\n      Header: packet #%d from %d, with length %d",
			       recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		    }
		    return(FALSE);
		}
		else
		    if (sresult == port1) {
			printf("!");
			recvd_on_1++;
		    }
		    else if (sresult == port2) {
			printf("#");
			recvd_on_2++;
		    }
            
		if (rlength != len) {
		    printf("\nMultiport_ping Error: packet length sent : %d, received %d from port %d",
			   len, rlength, sresult);
		    printf("\n                Argreg = 0x%x, receive status = 0x%x",
			   sresult, rstatus);
		    printf("\n      sent%d: %d, %d: %d, received%d: %d, %d: %d",
			   port1, sent_on_1, port2, sent_on_2, port1,
			   recvd_on_1, port2, recvd_on_2);
		    printf("\n      Header: packet #%d from %d, with length %d",
			   recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
                }
		if (rstatus != 0) {
		    printf("\nMultiport_ping Error: receive status = 0x%x, should be 0.",
			   rstatus);
		    printf("\n      Header: packet #%d from %d, with length %d",
			   recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		    return(FALSE);
		}
		
		/*  
		 * Check the packet that came back.
		 */
		
		if (((sresult == port1) && ((recvd[0] >> 4) != (port2 / 6))) ||
		    ((sresult == port2) && ((recvd[0] >> 4) != (port1 / 6)))) {
		    printf("\n Multiport_ping Error: Header board # error.", sresult);
		    printf("\n                Argreg = 0x%x, receive status = 0x%x",
			   sresult, rstatus);
		    printf("\n      sent%d: %d, %d: %d, received%d: %d, %d: %d",
			   port1, sent_on_1, port2, sent_on_2, port1,
			   recvd_on_1, port2, recvd_on_2);
		    printf("\n      Header: packet #%d from %d, with length %d",
			   recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		    return(FALSE);
                }
		
		if (((sresult == port1) && ((recvds[0] & 0xFFF) != recvd_on_1)) ||
		    ((sresult == port2) && ((recvds[0] & 0xFFF) != recvd_on_2))) {
		    printf("\n Multiport_ping Error: Header sequence # error from port %d.", sresult);
		    printf("\n                Argreg = 0x%x, receive status = 0x%x",
			   sresult, rstatus);
		    printf("\n      sent%d: %d, %d: %d, received%d: %d, %d: %d",
			   port1, sent_on_1, port2, sent_on_2, port1,
			   recvd_on_1, port2, recvd_on_2);
		    printf("\n      Header: packet #%d from %d, with length %d",
			   recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
	    return(FALSE);
                }
		
		if (recvds[1] != len) {
		    printf("\n Multiport_ping Error: Header length error from port %d on length %d", sresult, len);
		    printf("\n                Argreg = 0x%x, receive status = 0x%x",
			   sresult, rstatus);
		    printf("\n      sent%d: %d, %d: %d, received%d: %d, %d: %d",
			   port1, sent_on_1, port2, sent_on_2, port1,
			   recvd_on_1, port2, recvd_on_2);
		    printf("\n      Header: packet #%d from %d, with length %d",
			   recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		    return(FALSE);
                }
		
		for (i = 4; i < len; i++)
		    if (recvd[i] != sent[i]) {
			printf("\n Multiport_ping Error: Data Miscompare on byte %d from port %d, is 0x%x, SB 0x%x",
			       i, sresult, recvd[i], sent[i]);
			printf("\n      sent%d: %d, %d: %d, received%d: %d, %d: %d",
			       port1, sent_on_1, port2, sent_on_2, port1,
			       recvd_on_1, port2, recvd_on_2);
		    	ex_cbus_command (cBus, 20, 0412);
		    	printf("\n    Buffer Address: 0x%-04x", ex_cbus_command (cBus, 18, 0));
			return(FALSE);
		    }
	    }
		
	    /*
	     * Turn the packet around and send it again on the same interface.
	     */
	    
	    if (sresult == port1)
		if (sent_on_2 < cnt_each_way) {
		    cBus->regptr->argreg = port2;
		    sent_on_2++;
		    cBus->regptr->cmdreg = MCI_CMD_RX_MOVE;
		}
		else
		    cBus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    else if (sresult == port2)
		if (sent_on_1 < cnt_each_way) {
		    cBus->regptr->argreg = port1;
		    sent_on_1++;
		    cBus->regptr->cmdreg = MCI_CMD_RX_MOVE;
		}
		else
		    cBus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    waiting = ping_wait + (len * 2);
	    
	}   /* end of got a packet */
	
    }   /* end while waiting for packets */
    
    if ((waiting == 0) || (bad_packets >= cnt_each_way) || abort) {
	if (talking) {
	    if (recvd_on_1 != sent_on_1)
		printf("\nBothways: Error: sent %d packets from port%d to port%d, received %d",
		       sent_on_2, port2, port1, recvd_on_1);
	    if (recvd_on_2 != sent_on_2)
		printf("\nBothways: Error: sent %d packets from port%d to port%d, received %d",
                       sent_on_1, port1, port2, recvd_on_2);
	    printf("\nBothways: Total bad packets = %d", bad_packets);
        }
        return (FALSE);
    }
    return (TRUE);
} /* dci_bothways */
/*   
 * dci_fourways
 *
 * Sent some packets in both directions at once from two separate interfaces
 *
 * Parameters:
 *  cbus:       MCI device to use
 *  len:        Length of packet to use.
 *  talking:  Report errors or not.
 *  at_once:    Number of packets to start transmitting on one interface.
 *  cnt_each_way:   Total number of packets to send in each direction.
 *
 * Returns:     True if pass, False if fail.
 */

boolean dci_fourways(cBus, len, talking, at_once, cnt_each_way)
    mcitype *cBus;
    int len, at_once, cnt_each_way;
    boolean talking;
{
    int port0 = 0, port1 = 6, port2 = 12, port3 = 18;
    int board_0 = 0, board_1 = 1, board_2 = 2, board_3 = 3;
    ulong *tpacket;
    ulong *rpacket;
    ushort sresult, rlength = 0, rstatus = 0, *recvds;
    int waiting, bad_packets = 0, i, j;
    int sent_on_0 = 0, sent_on_1 = 0, sent_on_2 = 0, sent_on_3 = 0;
    int recvd_on_0 = 0, recvd_on_1 = 0, recvd_on_2 = 0, recvd_on_3 = 0;
    uchar *sent, *recvd;
    
    tpacket = (ulong *) packet1;
    rpacket = (ulong *) packet2;
    if (verbose)
	printf("\nBothways - ");
    
    /*
     * Setup transmit data and prepare to send standard packet
     */
    
    packet_length = pkt_end;
    sent = (uchar*) tpacket;
    if (standard_packet) {
	/* Standard data now includes a header
	 * which has:
	 * port number in the first byte
	 * packet number in the second byte
	 * packet length in the next word
	 * The standard packet has 0s in all
	 * these bytes.
	 */
	tpacket[0] = 0;
	for(i = 4; i < packet_length; i++) {
	    j = (i - 4) & 0x3f;
	    if (j < 16)
		sent[i] = (1 << (j / 2));
	    else if (j < 32)
		sent[i] = ((1 << ((j & 0xF) / 2)) ^ 0xFF);
	    else if ((j & 2) == 0)
		sent[i] = 0x55;
	    else
		sent[i] = 0xAA;
	}
    }
    else
        for (i = 0; i < packet_length; i++)
            tpacket[i] = custom_packet[i];
    
    recvd = (uchar*) rpacket;
    recvds = (ushort*) rpacket;
    if (!enqueue(cBus, port0, at_once, tpacket, len, &sent_on_0, talking))
        return(FALSE);
    if (!enqueue(cBus, port1, at_once, tpacket, len, &sent_on_1, talking))
        return(FALSE);
    if (!enqueue(cBus, port2, at_once, tpacket, len, &sent_on_2, talking))
        return(FALSE);
    if (!enqueue(cBus, port3, at_once, tpacket, len, &sent_on_3, talking))
        return(FALSE);
    
    /*
     * Start transmission on all four ports
     */
    
    cBus->regptr->argreg = board_0;
    cBus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    cBus->regptr->argreg = port0;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    dci_command(cBus, FCI_CTRLR_TX_START, 0);
    cBus->regptr->argreg = board_2;
    cBus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    cBus->regptr->argreg = port2;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    dci_command(cBus, FCI_CTRLR_TX_START, 0);
    cBus->regptr->argreg = board_1;
    cBus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    cBus->regptr->argreg = port1;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    dci_command(cBus, FCI_CTRLR_TX_START, 0);
    cBus->regptr->argreg = board_3;
    cBus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    cBus->regptr->argreg = port3;
    cBus->regptr->cmdreg = MCI_CMD_SELECT;
    dci_command(cBus, FCI_CTRLR_TX_START, 0);
    
    /*
     * Wait for packets to come back.
     *
     */
    
    waiting = ping_wait + (len * 2);
    while (((recvd_on_0 < cnt_each_way) || (recvd_on_1 < cnt_each_way) ||
	    (recvd_on_2 < cnt_each_way) || (recvd_on_3 < cnt_each_way)) &&
           (waiting > 0) && (bad_packets < cnt_each_way) && !abort) {
        cBus->regptr->cmdreg = MCI_CMD_RX_SETUP;
        sresult = cBus->regptr->argreg;
        if (sresult == 0xFFFF)
            waiting--;
        else {
            rlength = cBus->regptr->rxlength;
            rstatus = (cBus->regptr->rxstatus & MCI_SX_MASK);
	    
	    /*
	     * Read the packet
	     */
	    
	    cBus->regptr->argreg = 0;
	    cBus->regptr->cmdreg = MCI_CMD_RX_SELECT;
	    
	    if (talking) {
		for (i = 0; i <= len; i += 4)
		    rpacket [i/4] = cBus->regptr->readlong;
		if (sresult == port0) {
		    printf("0");
		    recvd_on_0++;
		}
		else if (sresult == port1) {
		    printf("1");
		    recvd_on_1++;
		}
		else if (sresult == port2) {
		    printf("2");
		    recvd_on_2++;
		}
		else if (sresult == port3) {
		    printf("3");
		    recvd_on_3++;
		}
		else {
		    if (talking) {
			printf("\nMultiport_ping Error: packet received on port %d in error",
			       sresult);
			printf("\n      length = %d, Status = 0x%-04x", rlength, rstatus);
			
			printf("\n      sent0: %d, 6: %d, 12: %d, 18: %d, received0: %d, 6: %d, 12: %d, 18: %d",
			       sent_on_0, sent_on_1, sent_on_2, sent_on_3,
			       recvd_on_0, recvd_on_1, recvd_on_2, recvd_on_3);
			printf("\n      Header: packet #%d from %d, with length %d",
			       recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		    }
		    return(FALSE);
		}
		
		if (rlength != len) {
		    printf("\nMultiport_ping Error: packet length sent : %d, received %d from port %d",
			   len, rlength, sresult);
		    printf("\n                Argreg = 0x%x, receive status = 0x%x",
			   sresult, rstatus);
		    printf("\n      sent0: %d, 6: %d, 12: %d, 18: %d, received0: %d, 6: %d, 12: %d, 18: %d",
			   sent_on_0, sent_on_1, sent_on_2, sent_on_3,
			   recvd_on_0, recvd_on_1, recvd_on_2, recvd_on_3);
		    printf("\n      Header: packet #%d from %d, with length %d",
		           recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		}
		if (rstatus != 0) {
		    printf("\nMultiport_ping Error: receive status = 0x%x, should be 0.",
			   rstatus);
		    printf("\n      sent0: %d, 6: %d, 12: %d, 18: %d, received0: %d, 6: %d, 12: %d, 18: %d",
			   sent_on_0, sent_on_1, sent_on_2, sent_on_3,
			   recvd_on_0, recvd_on_1, recvd_on_2, recvd_on_3);
		    printf("\n      Header: packet #%d from %d, with length %d",
		           recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		    return(FALSE);
		}
		
		/*  
		 * Check the packet that came back.
		 */
		
		if (((sresult == port0) && ((recvd[0] >> 4) != (port2 / 6))) ||
		    ((sresult == port1) && ((recvd[0] >> 4) != (port3 / 6))) ||
		    ((sresult == port2) && ((recvd[0] >> 4) != (port0 / 6))) ||
		    ((sresult == port3) && ((recvd[0] >> 4) != (port1 / 6)))) {
		    printf("\n Multiport_ping Error: Header board # error.", sresult);
		    printf("\n                Argreg = 0x%x, receive status = 0x%x",
			   sresult, rstatus);
		    printf("\n      sent0: %d, 6: %d, 12: %d, 18: %d, received0: %d, 6: %d, 12: %d, 18: %d",
			   sent_on_0, sent_on_1, sent_on_2, sent_on_3,
			   recvd_on_0, recvd_on_1, recvd_on_2, recvd_on_3);
		    printf("\n      Header: packet #%d from %d, with length %d",
		           recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		    return(FALSE);
		}
		
		if (((sresult == port0) && ((recvds[0] & 0xFFF) != recvd_on_0)) ||
		    ((sresult == port1) && ((recvds[0] & 0xFFF) != recvd_on_1)) ||
		    ((sresult == port2) && ((recvds[0] & 0xFFF) != recvd_on_2)) ||
		    ((sresult == port3) && ((recvds[0] & 0xFFF) != recvd_on_3))) {
		    printf("\n Multiport_ping Error: Header sequence # error from port %d.", sresult);
		    printf("\n                Argreg = 0x%x, receive status = 0x%x",
			   sresult, rstatus);
		    printf("\n      sent0: %d, 6: %d, 12: %d, 18: %d, received0: %d, 6: %d, 12: %d, 18: %d",
			   sent_on_0, sent_on_1, sent_on_2, sent_on_3,
			   recvd_on_0, recvd_on_1, recvd_on_2, recvd_on_3);
		    printf("\n      Header: packet #%d from %d, with length %d",
		           recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		    return(FALSE);
		}
		
		if (recvds[1] != len) {
		    printf("\n Multiport_ping Error: Header length error from port %d on length %d", sresult, len);
		    printf("\n                Argreg = 0x%x, receive status = 0x%x",
			   sresult, rstatus);
		    printf("\n      sent0: %d, 6: %d, 12: %d, 18: %d, received0: %d, 6: %d, 12: %d, 18: %d",
			   sent_on_0, sent_on_1, sent_on_2, sent_on_3,
			   recvd_on_0, recvd_on_1, recvd_on_2, recvd_on_3);
		    printf("\n      Header: packet #%d from %d, with length %d",
		           recvds[0] & 0xFFF, recvd[0] >> 4, recvds[1]);
		    return(FALSE);
		}
		
		for (i = 4; i < len; i++)
		    if (recvd[i] != sent[i]) {
			printf("\n Multiport_ping Error: Data Miscompare on byte %d from port %d, is 0x%x, SB 0x%x",
			       i, sresult, recvd[i], sent[i]);
			printf("\n      sent0: %d, 6: %d, 12: %d, 18: %d, received0: %d, 6: %d, 12: %d, 18: %d",
			       sent_on_0, sent_on_1, sent_on_2, sent_on_3,
			       recvd_on_0, recvd_on_1, recvd_on_2, recvd_on_3);
			ex_cbus_command (cBus, 20, 0412);
			printf("\n    Buffer Address: 0x%-04x", ex_cbus_command (cBus, 18, 0));
			return(FALSE);
		    }
	    }
	    
	    /*
	     * Turn the packet around and send it again on the same interface.
	     */
	    
	    if (sresult == port0)
		if (sent_on_2 < cnt_each_way) {
		    cBus->regptr->argreg = port2;
		    sent_on_2++;
		    cBus->regptr->cmdreg = MCI_CMD_RX_MOVE;
		}
		else
		    cBus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    else if (sresult == port1)
		if (sent_on_3 < cnt_each_way) {
		    cBus->regptr->argreg = port3;
		    sent_on_3++;
		    cBus->regptr->cmdreg = MCI_CMD_RX_MOVE;
		}
		else
		    cBus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    else if (sresult == port2)
		if (sent_on_0 < cnt_each_way) {
		    cBus->regptr->argreg = port0;
		    sent_on_0++;
		    cBus->regptr->cmdreg = MCI_CMD_RX_MOVE;
		}
		else
		    cBus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    else if (sresult == port3)
		if (sent_on_1 < cnt_each_way) {
		    cBus->regptr->argreg = port1;
		    sent_on_1++;
		    cBus->regptr->cmdreg = MCI_CMD_RX_MOVE;
		}
		else
		    cBus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    waiting = ping_wait + (len * 2);
	    
	}   /* end of got a packet */
	
    }   /* end while waiting for packets */
    
    if ((waiting == 0) || (bad_packets >= cnt_each_way) || abort) {
	if (talking) {
	    if (recvd_on_0 != sent_on_2)
		printf("\nBothways: Error: sent %d packets from port12 to port0, received %d",
		       sent_on_2, recvd_on_0);
	    if (recvd_on_1 != sent_on_3)
		printf("\nBothways: Error: sent %d packets from port18 to port6, received %d",
		       sent_on_3, recvd_on_1);
	    if (recvd_on_2 != sent_on_0)
		printf("\nBothways: Error: sent %d packets from port0 to port12, received %d",
		       sent_on_0, recvd_on_2);
	    if (recvd_on_3 != sent_on_1)
		printf("\nBothways: Error: sent %d packets from port6 to port18, received %d",
		       sent_on_1, recvd_on_3);
	    printf("\nBothways: Total bad packets = %d", bad_packets);
	}
	return (FALSE);
    }
    return (TRUE);
} /* end fourways() */

/*  end ds3-ultra_test.c  */

