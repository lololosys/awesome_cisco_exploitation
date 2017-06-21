/* $Id: mec_test.c,v 3.1.58.1 1996/05/17 10:23:11 ppearce Exp $
 * $Source: /release/112/cvs/Xboot/diag/mec/mec_test.c,v $
 *------------------------------------------------------------------
 * mec_test.c -- MEC specific routines.
 *
 * 22-Feburary-1991, Bruce Sherry
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mec_test.c,v $
 * Revision 3.1.58.1  1996/05/17  10:23:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.46.1  1996/04/03  13:11:26  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1  1995/11/09  09:07:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:14:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1.52.1  1995/10/21  01:02:31  getchell
 * Branch: Arkansas_branch
 * Ark_Vip_branch commit
 *
 * Revision 1.1.86.1  1995/07/24  16:55:28  getchell
 * Branch: VIP_branch
 * Sync VIP_branch between VIP_baseline_950523 and VIP_baseline_950720
 *
 * Revision 2.1  1995/06/07  19:13:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:57:09  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../h/std_dci.h"

#define MEC_OFFS        1
#define MEC_START       60
#define MEC_END         1500
#define MEC_SKIP        1
#define MEC_APP_REGS    0
#define MEC_MEMA_START	0x340
#define MEC_MEMA_END	0x800

/*
 *  Function declarations
 */

void	 mec_command(),mec_autotest(), mec_experttest(), mec_test();

/*
 * mec_command
 * Expert's DCI test routines
 */

void mec_command()
{
    register mcitype    *cbus;
    int                 argument, command, i, rcount, linect, size,
    flag, ahigh, alow, mhigh, mlow, afhigh, aflow,
    arg1, arg2, memd_sadr,memd_enadr;
    char                cmdchar;
    char                buff[BUFLEN];
    boolean             parsed;
    ushort              passes;
    
    pkt_offset = MEC_OFFS;
    pkt_start = MEC_START;
    pkt_end = MEC_END;
    pkt_skip = MEC_SKIP;
    mema_start = MEC_MEMA_START;
    mema_end = MEC_MEMA_END;
    standard_packet = TRUE;
    ping_wait = 50;
    rcount = 1;
    reporting = TRUE;
    looping = FALSE;
    verbose = TRUE;
    ping_loopback = FALSE;
    printf("\nMEC Diagnostic.");
    dci_alloc_diag_mem();
    init_error_struct(FCI_MAX_DCI_CTRLRS); /* prepare the error storage area */
    cbus = cBus_findcontroller();
    if (cbus->unit < 0)
        printf("\nCan't find cBus Controller!!\n");
    argument = command = default_interface = 0;
    dci_config(cbus, FCI_ETHER_CONTROLLER);
    dci_setup_unit(cbus);
    abortstate = ABORT_INTERNAL;
    buserrhandler = buserrnote;
    cmdchar = '?';
    while (TRUE) {
        if (cbus == NULL)
            return;
        printf("\nMain diag command (?ABCDEFGHIJKLNOPQSZ) [%c]: ", cmdchar);
        parsed = (int) gets(buff);
        if (!null(buff) && parsed)
            cmdchar = cvu(buff[0]);
        switch (cmdchar) {
          case 'A':             /* Debug basic tests */
            mec_basic_debug(cbus);
            break;
          case 'B':             /* Do basic tests */
            mec_experttest(cbus);
            break;
          case 'D':             /* Run MEMA TEST once */
            (void)dci_mema_test(cbus, &linect);
            break;                  
          case 'E':             /* Run MEMD TEST once */
            (void)dci_memd_test(cbus, &linect);
            break;
          case 'F':             /* Run AF MEM test once */
            (void)dci_afmem_test(cbus, &linect, default_interface);
            break;
          case 'G':             /* Run AF test once */
            (void)mec_aftest(cbus, default_interface);
            break;
          case 'H':             /* Display this menu */
          case '?':
            printf("\n\nA - Debug basic tests");
            printf("\nB - Do basic tests");
            printf("\nC - Run BUS TEST once");
            printf("\nD - Run MEMA TEST once");
            printf("\nE - Run MEMD TEST once");
            printf("\nF - Run AF MEM test once");
            printf("\nG - Run AF test once");
            printf("\nH or ? - Display this menu");
            printf("\nI - Run PING test once");
            printf("\nJ - Execute MEC command");
            printf("\nK - Select default interface");
            printf("\nL - List all errors");
            printf("\nN - Initialize (clear) errors");
            printf("\nO - Execute command code");
            printf("\nP - Debug MEC ports");
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
          case 'K':             /* Select default interface */
            default_interface = (default_interface + 1);
            if (default_interface >= FCI_MAX_CTRLR_INTERFACES)
                default_interface = 0;
            printf("\nSelecting interface %d", default_interface);
            (void)dci_select(cbus, board_to_test, default_interface);
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
          case 'P':             /* Debug DS3 ports */
            mec_debug_ports(cbus, &default_interface, MEC_OFFS, MEC_START, MEC_END, MEC_SKIP);
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
          case 'Q':             /* Quit */
            return;
            break;
          case 'V':             /* Print version information */
            printf("\nmec_test $Revision: 3.1.58.1 $ on $Date: 1996/05/17 10:23:11 $ by $Author: ppearce $.");
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
    
}  /*  end mec_command()  */

/*
 * mec_setup
 *
 * mec specific port setup commands
 *
 */

boolean mec_setup(cbus, unit, tpacket, bytes, words, test, linect)
    mcitype *cbus;
    register uchar  *tpacket;
    int unit, bytes, words, *linect;
    MCI_ERROR *test = NULL;
{
    if (!dci_setfilter(cbus,tpacket, bytes, words)) {
	if (!looping)
	    (void)store_error(test, 0, 0, 0, 1, 0, 0, 2);
	if (reporting) {
	    printf("\nPING%d ERROR: Unable to setup address filter.", unit);
	    ++linect;
	}
	return(FALSE);
    }

/* set the loopback mode */

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

    return(TRUE);
}   /* end mec_setup */

/*
 * mec_autotest
 * Automatic scan and test of MEC units
 */

void mec_autotest()
{
    int         unit, total, i;
    mcitype     *cBus;
    
    pkt_offset = MEC_OFFS;
    pkt_start = MEC_START;
    pkt_end = MEC_END;
    pkt_skip = MEC_SKIP;
    mema_start = MEC_MEMA_START;
    mema_end = MEC_MEMA_END;
    standard_packet = TRUE;
    ping_wait = 2;
    reporting = FALSE;
    looping = FALSE;
    verbose = FALSE;
    ping_loopback = FALSE;
    dci_alloc_diag_mem();    
    init_error_struct(FCI_MAX_DCI_CTRLRS);
    cBus = cBus_findcontroller();
    if (cBus->unit < 0) {
        printf("\nCan't find cBus Controller!!\n");
        return;
    }
    dci_config(cBus, FCI_ETHER_CONTROLLER);
    if (board_to_test < 0) {
        printf("\nNo MEC cards present\n");
        return;
    }
    for (; board_to_test < FCI_MAX_DCI_CTRLRS && !abort; board_to_test++) {
        if ((dcidata[board_to_test]->ctrlr_unit == board_to_test) &&
            (dcidata[board_to_test]->ctrlr_type == FCI_ETHER_CONTROLLER)) {
            dci_setup_unit(cBus);
            mec_test(cBus, 1);
        }
    }
    show_dci_auto_results(cBus);
    abort = FALSE;
    
}  /*  end mec_autotest()  */

/*
 * mec_experttest
 * Let an expert run the test.
 */

void mec_experttest(cbus)
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
    
    mec_test(cbus, passes);
    
    if (!verbose)
        show_dci_auto_results(cbus);
    
}  /*  end mec_experttest()  */

/*
 * mec_test
 * Workhorse test routine.
 */

void mec_test(cbus, passes)
    mcitype *cbus;
{
    int i, linect, size;
    
    linect = errors = 0;
    if (!verbose)
        printf("\nTesting MEC card %d ...", board_to_test);
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
         * Run Ethernet interface diagnostics
         */
        
        if (!verbose)
            printf(" ports");
        for (i = 0; i < FCI_MAX_CTRLR_INTERFACES; i++) {
            if (cbus->interfacetype[i + (6 * FCI_MAX_CTRLR_INTERFACES)]
		   		 == MCI_TYPE_ETHER) {
                if (!verbose)
                    printf(" %d", i);
                if (dci_afmem_test(cbus, &linect, i) == FALSE)
                    continue;
                if (mec_aftest(cbus, i) == FALSE) {
                    continue;
                }
                (void)dci_ping_test(cbus, i);
            }
        }
    }
    
}  /*  end mec_test()  */


