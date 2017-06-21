/* $Id: mcitest.c,v 3.1.58.1 1996/05/17 10:23:00 ppearce Exp $
 * $Source: /release/112/cvs/Xboot/diag/mci/mcitest.c,v $
 *------------------------------------------------------------------
 * mci_test.c -- MCI Diagnostic Routines
 *
 * 3-February-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mcitest.c,v $
 * Revision 3.1.58.1  1996/05/17  10:23:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.46.1  1996/04/03  13:11:18  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1  1995/11/09  09:07:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:14:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1.52.1  1995/10/21  01:02:25  getchell
 * Branch: Arkansas_branch
 * Ark_Vip_branch commit
 *
 * Revision 1.1.86.1  1995/07/24  16:55:23  getchell
 * Branch: VIP_branch
 * Sync VIP_branch between VIP_baseline_950523 and VIP_baseline_950720
 *
 * Revision 2.1  1995/06/07  19:13:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:58  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../defs.h"
#include "../../types.h"
#include "../h/ether.h"
#include "../h/if_mci.h"
#include "../h/if_fci.h"
#include "../../ctype.h"
#include "../../test.h"
#include "../../cpu.h"
#include "../../reason.h"
#include "../../extern.h"
#include "../../stdio.h"
#include "../h/mci_err.h"

/*
 *  Amount of time to wait in ms for an MCI to reset:
 */

#define RESET_TIME	250
#define BUFLEN		30
#define BUF_OVERHEAD	10
#define BUF_WORDS	750
#define MAXCTRLLRS	16
#define MAXPACKET	1500
#define MEMA_BASE	0100
#define MEMD_STEP	1	/* 1 == look at every word of memory */
#define AF_STEP		0x1
#define PKTLENGTH	60

/*
 *  Function declarations
 */

extern boolean	store_error(), quit_display(), mci_xnvt();
extern short	touch_device();
extern void	init_error_struct(), print_errors(), show_err_mask(),
    		show_auto_results(), get_number();
extern int	mci_xnvs(), applique_type();
boolean		bus_test(), mema_test(), memd_test(), mci_select(),
    		allocate_buffers(), mci_aftest(), check_one(), mci_setmask(),
    		mci_ping(), mci_setfilter(), af_readwrite();
mcitype 	*setup_unit();
uchar		mci2char();
void		alloc_diag_mem(), mci_config(), mci_command(), mci_display(),
    		mci_address(), type_mema(), type_memd(), execute_command(),
    		mci_autotest(), mci_experttest(), mci_test(), mci_reset(),
    		memd_readwrite(), memd_write(), memd_read(), memd_load(),
    		mema_readwrite(), mci_printaddr(), stationaddress(),
    		mci_basic_debug(), mema_load(), mema_write(), mema_read(),
    		mci_debug_ports(), mci_search();
char		*devicestring();

/*
 *  Global variables
 */

extern MCI_TEST *diag_results[];
mcitype		*mcidata[MAXCTRLLERS];
boolean		reporting, verbose, looping;
int		errors, default_interface, serial_clk_div,
		serial_prescale, abort_count;
uchar		*packet1, *packet2;

/*
 *  alloc_diag_mem()
 *  allocate memory for mci and diag data structures
 */

void alloc_diag_mem()
{
    int i;
    
    for (i = 0; i < MAXCTRLLERS; ++i) {
	mcidata[i] = (mcitype * )(0x1000 + (sizeof(mcitype) * i));
	diag_results[i] = (MCI_TEST * )(0x1000 +
					(sizeof(mcitype) * MAXCTRLLERS) +
					(sizeof(MCI_TEST)) * i);
    }  /*  end for(i)  */
    
    packet1 = (uchar *)(0x1000 + (sizeof(mcitype) * MAXCTRLLERS) +
			(sizeof(MCI_TEST)) * MAXCTRLLERS);
    packet2 = (uchar *)(0x1000 + (sizeof(mcitype) * MAXCTRLLERS) +
			(sizeof(MCI_TEST)) * MAXCTRLLERS) + MAXPACKET;
    
}  /*  end alloc_diag_mem()  */

/*
 * mci_command
 * Expert's MCI test routines
 */

void mci_command()
{
    register mcitype	*mci;
    int			argument, command, i, mcis_found, rcount, linect, size;
    char		cmdchar;
    char		buff[BUFLEN];
    boolean		parsed;
    ushort		passes;
    
    rcount = 1;
    reporting = TRUE;
    looping = FALSE;
    verbose = TRUE;
    serial_clk_div = 2;			/* Set serial clock to 2M */
    serial_prescale = MCI_CLK_GEN2;
    alloc_diag_mem();    
    init_error_struct(MAXCTRLLRS);		/* prepare the error storage area */
    if ((mcis_found = mci_findcontroller()) == 0)
	printf("\nNo MCI cards responding\n");
    argument = command = default_interface = 0;
    mci = setup_unit();
    if (mci->unit >= 0)
        mci_config(mci);
    abortstate = ABORT_INTERNAL;
    buserrhandler = buserrnote;
    while (TRUE) {
	if (mci == NULL)
	    return;
	cmdchar = '?';
	printf("\nMain command (?ABCDEFGHIJKLMNOPQ) [%c]: ", cmdchar);
	parsed = (int) gets(buff);
	if (!null(buff) && parsed)
	    cmdchar = cvu(buff[0]);
	switch (cmdchar) {
	  case 'A':
	    mci_basic_debug(mci);
	    break;
	  case 'B':
	    mci_experttest(mci);
	    break;
          case 'C':
            (void)bus_test(mci, &linect);
            break;
          case 'D':
            (void)mema_test(mci, &linect);
            break;
          case 'E':
            (void)memd_test(mci, &linect);
            break;
          case 'F':
            (void)ethermem_test(mci, &linect, default_interface);
            break;
          case 'G':
            (void)mci_aftest(mci, default_interface);
            break;
          case 'I':
            (void)mci_ping(mci, default_interface);
            break;
          case 'J':
            if ((i = mci_xnvs(mci)) == 0) {
            	printf("\nNo NVRAM found!");
            	break;
            }
            size = 0x1FFF + ((i - 1) * 0x2000);
            (void)mci_xnvt(mci, size, rcount, TRUE, TRUE, &linect);
            break;
          case 'K':
            default_interface = ((default_interface + 1) & 3);
            printf("\nSelecting interface %d", default_interface);
            (void)mci_select(mci, default_interface);
            break;
	  case 'O':
	    execute_command(mci, &argument, &command);
	    break;
	  case 'P':
	    mci_debug_ports(mci, &default_interface);
	    break;
	  case 'H':
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
	    printf("\nJ - Run XMEM test once");
	    printf("\nK - Select default interface");
	    printf("\nL - List all errors");
	    printf("\nN - Initialize (clear) errors");
	    printf("\nM - Read and Write XBUS");
    	    printf("\nO - Execute command code");
	    printf("\nP - Debug MCI ports");
	    printf("\nQ - Quit");
	    printf("\n\n'C' rules of radix type-in and display apply.\n");
	    break;
	  case 'L':
	    show_all_errors(mci, MCI_HARDWARE, 0);
	    break;
	  case 'M':
	    mci_xmemt(mci);
	    break;
	  case 'N':
	    init_error_struct(MAXCTRLLRS);
	    break;
	  case 'Q':
	    return;
	    break;
	  case 'V':
	    printf("\nMCITest $Revision: 3.1.58.1 $ on $Date: 1996/05/17 10:23:00 $ by $Author: ppearce $.");
	    break;
	  default:
	    printf("\nUnknown command \"%c\"", cmdchar);
	    break;
	    
	}  /*  end switch(cmdchar)  */
	
    }  /*  end while()  */
    
    abortstate = ABORT_IGNORE;
    buserrhandler = 0;
    
}  /*  end mci_command()  */

/*
 *  mci_basic_debug
 *  loop BUS, MEMA, MEMD tests, fill MEMA, MEMD with data patterns,
 *  various R/W loops
 */

void mci_basic_debug(mci)
    mcitype *mci;
{
    char	buffer[BUFLEN], cmdchar;
    int		mema_hi, mema_lo, memd_hi, memd_lo, linect;
    ushort	address, unit, data, sresult;
    boolean	parsed;
    
    
    memd_lo = linect = 0;
    mema_lo = MEMA_BASE;
    mema_hi = MEMA_WORDS;
    
    /*  Find out how big MEMD is  */
    
    mci->regptr->argreg = 1;
    mci->regptr->cmdreg = MCI_CMD_MEMSIZE;
    memd_hi = mci->regptr->argreg;
    
    verbose = reporting = FALSE;
    while (TRUE) {
	abort = FALSE;
    	cmdchar = '?';
    	printf("\nBasic Debug command (?ABCDEFGHIJKLMNOPQ) [%c]: ", cmdchar);
    	parsed = (int) gets(buffer);
	if (!null(buffer) && parsed)
	    cmdchar = cvu(buffer[0]);
    	switch (cmdchar) {
     	  case 'A':
     	    get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
       	    printf("\nRunning BUS test with 0x%-04x until BREAK...", data);
       	    while (TRUE) {
     	    	mci->regptr->argreg = (ushort) data;
     	    	sresult = mci->regptr->argreg;
     	    	if (abort)
		    break;
       	    }
       	    break;
     	  case 'B':
     	    printf("\nRunning MEMA test until BREAK...");
     	    looping = TRUE;
       	    while (TRUE) {
     	  	(void) mema_test(mci, &linect);
     	  	if (abort)
		    break;
       	    }
     	    looping = FALSE;
       	    break;
     	  case 'C':
     	    printf("\nRunning MEMD test until BREAK...");
     	    looping = TRUE;
       	    while (TRUE) {
     	    	(void) memd_test(mci, &linect);
     	    	if (abort)
		    break;
       	    }
     	    looping = FALSE;
       	    break;
     	  case 'D':
     	    get_number(&address, "\nEnter memory address: ", 0, MEMA_WORDS);
       	    get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
       	    mema_readwrite(mci, address, data);
       	    break;
     	  case 'E':
     	    get_number(&address, "\nEnter memory address: ", 0, memd_hi);
       	    get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
       	    memd_readwrite(mci, address, data);
       	    break;
     	  case 'F':
            get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
     	    mema_load(mci, data);
     	    break;
     	  case 'G':
     	    get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
     	    memd_load(mci, data);
     	    break;
     	  case 'H':
     	  case '?':
     	    printf("\n\nA - Loop BUS test with data pattern");
     	    printf("\nB - Loop MEMA test");
     	    printf("\nC - Loop MEMD test");
     	    printf("\nD - MEMA read/write loop");
     	    printf("\nE - MEMD read/write loop");
     	    printf("\nF - Fill MEMA with data pattern");
     	    printf("\nG - Fill MEMD with data pattern");
     	    printf("\nI - List MEMA contents");
     	    printf("\nJ - List MEMD contents");
     	    printf("\nK - Write MEMA once");
     	    printf("\nL - Read MEMA once");
     	    printf("\nM - Write MEMD once");
     	    printf("\nN - Read MEMD once");
     	    printf("\nO - Search MEMA and MEMD for data pattern");
     	    printf("\nP - List errors");
     	    printf("\nH or ? - Display this menu");
     	    printf("\nQ - Quit\n");
     	    break;
     	  case 'I':
     	    mci_address(&mema_lo, &mema_hi);
       	    type_mema(mci, mema_lo, mema_hi);
       	    break;
     	  case 'J':
     	    mci_address(&memd_lo, &memd_hi);
       	    type_memd(mci, memd_lo, memd_hi);
       	    break;
     	  case 'K':
     	    get_number(&address, "\nEnter memory address: ", 0, MEMA_WORDS);
       	    get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
       	    mema_write(mci, address, data);
       	    break;
     	  case 'L':
     	    get_number(&address, "\nEnter memory address: ", 0, MEMA_WORDS);
     	    mema_read(mci, address);
     	    break;
     	  case 'M':
     	    get_number(&address, "\nEnter memory address: ", 0, memd_hi);
       	    get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
       	    memd_write(mci, address, data);
       	    break;
     	  case 'N':
     	    get_number(&address, "\nEnter memory address: ", 0, memd_hi);
       	    memd_read(mci, address);
       	    break;
     	  case 'O':
     	    mci_search(mci);
     	    break;
     	  case 'Q':
     	    verbose = reporting = TRUE;
     	    return;
     	    break;
     	  case 'P':
     	    show_all_errors(mci, MCI_HARDWARE, 0);
     	    break;
     	  default:
     	    printf("\nUnknown command \"%c\"", cmdchar);
     	    break;
	    
     	}  /*  end switch(cmdchar)  */
	
    }  /*  end while(TRUE)  */
    
} /*  end mci_basic_debug()  */

/*
 *  mema_load
 *  loads mema with data pattern
 */

void mema_load(mci, data)
    mcitype *mci;
    short data;
{
    int i;
    
    mci->regptr->argreg = MEMA_BASE;
    mci->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    for (i = MEMA_BASE; i < MEMA_WORDS; i++) {
    	mci->regptr->argreg = data;
    	mci->regptr->cmdreg = MCI_CMD_MEMA_WRITE;
    }
    
}  /*  end mema_load()  */


/*
 *  mema_write
 *  write data to mema address
 */

void mema_write(mci, address, data)
    mcitype *mci;
    ushort address, data;
{
    printf("\nWriting 0x%x to MEMA location 0x%x once...", data, address);
    mci->regptr->argreg = address;
    mci->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    mci->regptr->argreg = data;
    mci->regptr->cmdreg = MCI_CMD_MEMA_WRITE;
    
}  /*  end mema_write()  */


/*
 *  mema_read
 *  read from mema address
 */

void mema_read(mci, address)
    mcitype *mci;
    ushort address;
{
    mci->regptr->argreg = address;
    mci->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    mci->regptr->cmdreg = MCI_CMD_MEMA_READ;
    printf("\nRead 0x%x from MEMA location 0x%x.", mci->regptr->argreg, address);
    
}  /*  end mema_read()  */


/*
 *  mci_port_debug
 *  mci interface debug routines
 */

void mci_debug_ports(mci, unit)
    mcitype *mci;
    int *unit;
{
    char	buffer[BUFLEN], cmdchar;
    ushort	address, data;
    boolean	parsed;
    int		testbytes = 8;
    int		testlongs = 4;
    int		testdata = 0x1234;
    int 	linect = 0;
    
    verbose = reporting = FALSE;
    while (TRUE) {
	abort = FALSE;
    	cmdchar = '?';
	printf("\nPort Debug command (?ABCDEFGHQ) [%c]: ", cmdchar);
	parsed = (int) gets(buffer);
	if (!null(buffer) && parsed)
	    cmdchar = cvu(buffer[0]);
	switch (cmdchar) {
	  case 'A':
	    printf("\nRunning AF MEM test until BREAK...");
	    looping = TRUE;
	    while(TRUE) {
	    	(void)ethermem_test(mci, &linect, *unit);
		if (abort)
		    break;
	    }
	    looping = FALSE;
	    break;
	  case 'B':
	    printf("\nRunning AF test until BREAK...");
	    looping = TRUE;
	    while (TRUE) {
	    	(void)mci_aftest(mci, *unit);
		if (abort)
		    break;
	    }
	    looping = FALSE;
	    break;
	  case 'C':
	    printf("\nRunning PING test until BREAK...");
	    looping = TRUE;
	    while (TRUE) {
	    	(void)mci_ping(mci, *unit);
		if (abort)
		    break;
	    }
	    looping = FALSE;
	    break;
	  case 'D':
	    *unit = ((*unit + 1) & 3);
	    printf("\nSelecting interface %d", *unit);
	    (void)mci_select(mci, *unit);
	    break;
	  case 'E':
	    show_all_errors(mci, MCI_HARDWARE, 0);
	    break;
	  case 'F':
	    get_number(&address, "\nEnter memory address: ", 0, AF_WORDS);
	    get_number(&data, "\nEnter data byte: ", 0, 255);
	    (void)af_readwrite(mci, *unit, address, data);
	    break;
	  case 'G':
    	    cmdchar = '?';
	    while (cmdchar != 'Q') {
		printf("\nSerial Port Baudrate change (?ABCDEFGHQ) [%c]: ", cmdchar);
		parsed = (int) gets(buffer);
		if (!null(buffer) && parsed)
		    cmdchar = cvu(buffer[0]);
		switch (cmdchar) {
		  case 'A':
		    serial_clk_div = 1;
		    serial_prescale = MCI_CLK_GEN2;
		    break;
		  case 'B':
		    serial_clk_div = 1;
		    serial_prescale = MCI_CLK_GEN3;
		    break;
		  case 'C':
		    serial_clk_div = 2;
		    serial_prescale = MCI_CLK_GEN2;
		    break;
		  case 'D':
		    serial_clk_div = 3;
		    serial_prescale = MCI_CLK_GEN2;
		    break;
		  case 'E':
		    serial_clk_div = 2;
		    serial_prescale = MCI_CLK_GEN3;
		    break;
		  case 'F':
		    serial_clk_div = 4;
		    serial_prescale = MCI_CLK_GEN2;
		    break;
		  case 'G':
		    serial_clk_div = 32;
		    serial_prescale = MCI_CLK_GEN2;
		    break;
		  case 'H':
		  case '?':
		    printf("\n\nA - 4.00 Meg");
		    printf("\nB - 2.66 Meg");
		    printf("\nC - 2.00 Meg");
		    printf("\nD - 1.66 Meg");
		    printf("\nE - 1.33 Meg");
		    printf("\nF - 1.00 Meg");
		    printf("\nG - 125K");
		    printf("\nH or ? - Display this menu");
		    printf("\nQ - Quit\n");
		    break;
		  case 'Q':
		    break;		
		  default:
		    printf("\nUnknown command \"%c\"", cmdchar);
		    break;
		}  /*  end switch(cmdchar)  */
	    }  /* end while (cmdchar != 'Q') */
	    cmdchar = 'G';
	    break;
	  case 'H':
	  case '?':
	    printf("\n\nA - Loop AF MEM test");
	    printf("\nB - Loop AF test");
	    printf("\nC - Loop PING test");
	    printf("\nD - Select Default Interface");
	    printf("\nE - List all errors");
	    printf("\nF - AF MEM write/read loop");
	    printf("\nG - Change Serial Baudrate");
	    printf("\nH or ? - Display this menu");
	    printf("\nQ - Quit\n");
	    break;
	  case 'Q':
	    verbose = reporting = TRUE;
	    return;
	    break;
	  default:
	    printf("\nUnknown command \"%c\"", cmdchar);
	    break;
	    
	    
	}  /*  end switch(cmdchar)  */
	
    }  /*  end while(TRUE)  */
    
}  /*  end mci_debug_ports()  */

/*
 * mci_findcontroller
 * Find all MCI controllers in system
 */

mci_findcontroller()
{
    int		i, total;
    short	*mcibase;
    mcitype	*mci;
    
    for (i = 0; i < MAXCTRLLERS; ++i)
	bzero((uchar * )mcidata[i], sizeof(mcitype));
    
    for (i = 0, total = 0; i < MAXCTRLLERS; i++) {
	mci = mcidata[i];
	mcibase = (short * ) MCISTART + MCIBYTES * i;
	(void)touch_device(mcibase);
	if (buserrcnt == 0) {
	    mci->unit = i;
	    total++;
	} else
	    mci->unit = -1;
	mci->regptr = (mciregtype * ) mcibase;
    }
    return(total);
    
}  /*  end mci_findcontroller()  */

/*
 * mci_display
 * Display controller information prior to autotesting
 */

char	*mcihead = "\n\nMCI  memd  ucode   type   NVM  0 1 2 3 ";

void mci_display()
{
    ushort	sresult, typecode;
    boolean	onceflag, verbose_hold;
    mcitype	*mci;
    mciregtype	*regptr;
    int		i, j, linect, temp;
    
    for (i = 0, linect = 0, onceflag = FALSE; i < MAXCTRLLERS; i++) {
	mci = mcidata[i];
	if (mci->unit < 0)
	    continue;
	regptr = mci->regptr;
	if (!onceflag) {
	    printf(mcihead);
	    onceflag = TRUE;
	}
	printf("\n%-3d ", mci->unit);
	verbose_hold = verbose;
	verbose = FALSE;
	if (bus_test(mci, &linect) == FALSE) {
	    printf("  -- BUS/ALU test failed.  Unable to read config info.");
	    continue;
	}
	verbose = verbose_hold;
	
	/*
	 * Discover major configuration information
	 */
	
	mci_reset(mci);
	regptr->argreg = MCI_MEMSIZE_MEMA;
	regptr->cmdreg = MCI_CMD_MEMSIZE;
	mci->mema_size = regptr->argreg;
	regptr->argreg = MCI_MEMSIZE_MEMD;
	regptr->cmdreg = MCI_CMD_MEMSIZE;
	mci->memd_size = regptr->argreg;
	regptr->argreg = MCI_HW_VERSION;
	regptr->cmdreg = MCI_CMD_VERSION;
	mci->hw_version = regptr->argreg;
	regptr->argreg = MCI_SW_VERSION;
	regptr->cmdreg = MCI_CMD_VERSION;
	mci->sw_version = regptr->argreg;
	regptr->cmdreg = MCI_CMD_CAPABILITY;
	mci->capability = regptr->argreg;
	
	/*
	 * Scan controller finding interfaces
	 */
	
	for (j = 0; j < MCI_MAXUNITS; j++) {
	    mci->interfacetype[j] = -1;
	    regptr->argreg = j;
	    regptr->cmdreg = MCI_CMD_SELECT;
	    sresult = mci->regptr->cmdreg;
	    if (sresult == MCI_RSP_OKAY) {
		typecode = regptr->argreg;
		mci->protocoltype[j] = (typecode >> 8);
		mci->interfacetype[j] = (typecode &= 0xFF);
	    }
	}
	
	/*
	 * Report controller configuration
	 */
	
	temp = (mci->sw_version & 0xff00) >> 8;
	printf("%-4dK %-3d.%3d", (mci->memd_size + 1) / 512, temp, (mci->sw_version & 0xff));
	temp = (mci->hw_version & 0xff00) >> 8;
	printf(" %-3d.%3d", temp, (mci->hw_version & 0xff));
	temp = mci_xnvs(mci);
	if (temp == 0) 
	    printf("none ");
	if (temp == 1) 
	    printf(" 16K ");
	if (temp == 2) 
	    printf(" 32K ");
	if (temp == 3) 
	    printf(" 48K ");
	if (temp == 4) 
	    printf(" 64K ");
	for (j = 0; j < MCI_MAXUNITS; j++)
	    printf(" %c", mci2char(mci, j));
	printf(" %c", (mci->capability & 0x1) ? 'Y' : '-');
	printf(" %c", (mci->capability & 0x2) ? 'Y' : '-');
	for (j = 0; j < MCI_MAXUNITS; j++)
	    mci_printaddr(mci, j);
    }
    printf("\n");
    
}  /*  end mci_display()  */

/*
 * mci2char
 * Return character code indicating MCI interace type and capability
 */

uchar mci2char(mci, index)
    mcitype *mci;
    int	index;
{
    int	typecode;
    
    
    typecode = mci->interfacetype[index];
    if (typecode == -1)
	return('-');
    switch (typecode) {
      case MCI_TYPE_ETHER:
	return('E');
      case MCI_TYPE_SERIAL:
	switch (mci->protocoltype[index]) {
	  case 0:  
	    return('S');
	  case 0x40:  
	    return('T');
	  default: 
	    return('?');
	}
      default:
	return('?');
	
    }  /*  end switch(typecode)  */
    
}  /*  end mci2char()  */

/*
 * mci_printaddr
 * Print 48 bit interface address or spaces or error message
 */

void mci_printaddr(mci, unit)
    mcitype *mci;
    int	unit;
{
    ushort	sresult;
    int		i;
    
    if (mci->interfacetype[unit] != MCI_TYPE_ETHER)
	return;
    if (!mci_select(mci, unit)) {
	printf(" select error ");
	return;
    }
    printf("  ");
    for (i = 0; i < ETHERWORDS; i++) {
	if (i != 0)
	    printf(".");
	mci->regptr->argreg = 2 * i;
	mci->regptr->cmdreg = MCI_CMD_STATION;
	sresult = mci->regptr->argreg;
	printf("%-02x", sresult & 0xFF);
	mci->regptr->argreg = (2 * i) + 1;
	mci->regptr->cmdreg = MCI_CMD_STATION;
	sresult = mci->regptr->argreg;
	printf("%-02x", sresult & 0xFF);
    }
    
}  /*  end mci_printaddr()  */

/*
 * setup_unit
 * Determine which unit, set up data structures
 */

mcitype *setup_unit()
{
    register	mcitype *mci;
    char	buff[BUFLEN];
    boolean	parsed;
    int		unit;
    
    /*
     * Figure out number of units.
     */
    
    unit = 0;
    printf("\nController number [%d]: ", unit);
    if (parsed = (int) gets(buff)) {
	if (!null(buff))
	    unit = parse_unsigned(buff);
	parsed = ((unit >= 0) && (unit < MAXCTRLLERS));
    }
    if (!parsed) {
	printf("\n%% Bad controller number -- assuming 0");
	unit = 0;
    }
    mci = mcidata[unit];
    if (mci->unit < 0)
	printf("\nMCI: controller %d is not responding.\n", unit);
    return(mci);
    
}  /*  end setup_unit()  */

/*
 * mci_address
 * Get a range of memory addresses
 */

void mci_address(low, high)
    int	*low, *high;
{
    register int	value;
    char		buff[BUFLEN];
    boolean		parsed;
    
    while (!abort) {
	value = *low;
	printf("Low address [%#x]: ", value);
	if (parsed = (int) gets(buff)) {
	    if (!null(buff))
		value = parse_unsigned(buff);
	    parsed = ((value >= 0) && (value <= 0xFFFF));
	}
	if (!parsed)
	    continue;
	*low = value;
	value = *high;
	printf("High address [%#x]: ", value);
	if (parsed = (int) gets(buff)) {
	    if (!null(buff))
		value = parse_unsigned(buff);
	    parsed = ((value > 0) && (value <= 0xFFFF));
	}
	if (!parsed)
	    continue;
	*high = value;
	return;
	
    }  /*  end while(!abort)  */
    
    abort = FALSE;
    
}  /*  end mci_address()  */

/*
 * type_mema
 * Type out a range of locations in mema
 */

void type_mema(mci, low, high)
    register mcitype *mci;
    int	low, high;
{
    mciregtype		*regptr;
    register int	i;
    ushort		sresult;
    int			linect;
    
    linect = 0;
    regptr = mci->regptr;
    for (i = low; i < high + 1; i++) {
	regptr->argreg = i;
	regptr->cmdreg = MCI_CMD_MEMA_SELECT;
	regptr->cmdreg = MCI_CMD_MEMA_READ;
	sresult = regptr->argreg;
    if ((i & 0x7) == (low & 0x7)){
    	if (quit_display(linect, NULL))
            return;
        printf("\nMEMA 0x%-04x: 0x%-04x", i, sresult);
        linect++;
    }
    else
        printf(" %-04x", sresult);
    }
    
}  /*  end type_mema()  */


/*
 * type_memd
 * Type out a range of locations in memd
 */

void type_memd(mci, low, high)
    register mcitype *mci;
    int	low, high;
{
    mciregtype		*regptr;
    register int	i;
    ushort		result;
    int			linect;
    
    linect = 0;
    regptr = mci->regptr;
    for (i = low; i < high + 1; i++) {
	regptr->argreg = i;
	regptr->cmdreg = MCI_CMD_MEMD_SELECT;
	result = regptr->readshort;
    if ((i & 0x7) == (low & 0x7)){
    	if (quit_display(linect, NULL))
            return;
        printf("\nMEMD 0x%-04x: 0x%-04x", i, result);
        linect++;
    }
    else
        printf(" %-04x", result);
    }
    
}  /*  end type_memd()  */

/*
 * execute_command
 * Given a command code, execute and return result
 */

void execute_command(mci, argument, command)
    register mcitype *mci;
    int	*argument, *command;
{
    char	buff[BUFLEN];
    ushort sresult, response;
    boolean parsed;
    int	i, j;
    
    printf("\nArgument [%d]: ", *argument);
    if (parsed = (int) gets(buff)) {
	if (!null(buff))
	    *argument = parse_unsigned(buff);
	parsed = (*argument >= 0);
    }
    if (!parsed) {
	printf("\n%% Bad argument");
	return;
    }
    printf("\nCommand [%d]: ", *command);
    if (parsed = (int) gets(buff)) {
	if (!null(buff))
	    *command = parse_unsigned(buff);
	parsed = (*command >= 0);
    }
    if (!parsed) {
	printf("\n%% Bad command code");
	return;
    }
    sresult = 0;
    response = 0;
    mci->regptr->argreg = *argument;
    mci->regptr->cmdreg = *command;
    if (*command == MCI_CMD_RESET)
	delay(RESET_TIME);
    response = mci->regptr->cmdreg;
    sresult = mci->regptr->argreg;
    
    if (response != MCI_RSP_OKAY)
	printf("\nBad response: 0x%-04x", response);
    else
	printf("\nValue returned: 0x%-04x or 0%-06o", sresult, sresult);
    
}  /*  end execute_command()  */

/*
 * mci_autotest
 * Automatic scan and test of MCI units
 */

void mci_autotest()
{
    int		unit, total, i;
    mcitype	*mci;
    
    alloc_diag_mem();    
    init_error_struct(MAXCTRLLRS);
    serial_clk_div = 2;			/* Set serial clock to 2M */
    serial_prescale = MCI_CLK_GEN2;
    total = mci_findcontroller();
    if (total == 0) {
	printf("\nNo MCI cards present\n");
	return;
    }
    verbose = FALSE;
    reporting = FALSE;
    looping = FALSE;
    mci_display();
    abortstate = ABORT_INTERNAL;
    for (unit = 0; unit < MAXCTRLLERS && !abort; unit++) {
	    mci = mcidata[unit];
	    if (mci->unit < 0)
	        continue;
	    mci_test(mci, 1);
	
    }
    
    abortstate = ABORT_IGNORE;
    show_auto_results();
    abort = FALSE;
    
}  /*  end mci_autotest()  */

/*
 * mci_experttest
 * Let an expert run the test.
 */

void mci_experttest(mci)
    register mcitype *mci;
{
    long	passes;
    char	buff[BUFLEN];
    boolean	parsed;
    
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
	printf("\nVerbose mode [y]: ");
	verbose = (gets(buff) && cvu(*buff) == 'N') ? FALSE : TRUE;
    } else
	verbose = FALSE;
    
    /*
     * Do the dirty work.
     */
    
    mci_test(mci, passes);
    
    if (!verbose)
        show_auto_results();
    
}  /*  end mci_experttest()  */

/*
 * mci_test
 * Workhorse test routine.
 */

void mci_test(mci, passes)
    mcitype *mci;
{
    int	i, linect, size;
    
    linect = errors = 0;
    if (!verbose)
	printf("\nTesting MCI card %d ...", mci->unit);
    while (passes--) {
	
	/*
	 * Run bus tests
	 */
	
	if (bus_test(mci, &linect) == FALSE)
	    return;
	
	/*
	 * Report configuration
	 */
	
	mci_config(mci);
	
	/*
	 * Run mema tests
	 */
	
	if (mema_test(mci, &linect) == FALSE)
	    return;
	
	/*
	 * Run memd tests
	 */
	
	if (memd_test(mci, &linect) == FALSE)
	    return;
	
	/*
	 * Run Ethernet/Serial interface diagnostics
	 */
	
	for (i = 0; i < MCI_MAXUNITS; i++) {
	    if (mci->interfacetype[i] == MCI_TYPE_ETHER) {
		if (ethermem_test(mci, &linect, i) == FALSE)
		    continue;
		if (mci_aftest(mci, i) == FALSE)
		    continue;
	    }
	    if ((mci->interfacetype[i] == MCI_TYPE_ETHER) ||
		(mci->interfacetype[i] == MCI_TYPE_SERIAL))
		(void)mci_ping(mci, i);
	}
	
	/*
	 * Run Xbus Nov RAM memory diagnostics if necessary
	 */
	
	
        if ((i = mci_xnvs(mci)) != 0) {
            size = 0x1FFF + ((i - 1) * 0x2000);
	    if (mci_xnvt(mci, size, 1, reporting, verbose, &linect) == FALSE)
	    	return;
        }
    }
    
}  /*  end mci_test()  */

/*
 * bus_test
 * Make sure we can read/write commands to this device
 */

boolean bus_test(mci, linect)
    register mcitype *mci;
    int	*linect;
{
    register uint	i;
    ushort		sresult;
    
    /*
     * Make sure we can read and write the ALU registers
     */
    
    errors = 0;
    if (verbose)
	printf("\nMCI: starting BUS test...");
    for (i = 0; i < MEMD_WORDS; i++) {
	mci->regptr->argreg = (ushort) i;
	sresult = mci->regptr->argreg;
	if (sresult != i) {
	    errors++;
	    if (!store_error(&diag_results[mci->unit]->bus_test, i, sresult, 0, 0, 0, 0, 0))
		return(FALSE);                     /*  all bits failed  */
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: BUS/ALU problem: wrote 0x%-04x, read 0x%-04x",
		       i, sresult);
	    }
	}
    }
    if (verbose && !errors)
	printf("[OK]");
    if (errors == 0)
	diag_results[mci->unit]->bus_test.stat = STAT_PASS;
    if (errors)
	return(FALSE);
    else
	return(TRUE);
    
}  /*  end bus_test()  */

/*
 * mci_config
 * Report configuration of this controller.
 */

void mci_config(mci)
    mcitype *mci;
{
    register int	i;
    ushort		sresult, typecode, capability;
    
    /*
     * Reset controller.
     */
    
    mci_reset(mci);
    
    /*
     * Read version number.
     */
    
    mci->regptr->cmdreg = MCI_CMD_VERSION;
    sresult = mci->regptr->argreg;
    if (verbose)
	printf("\nMCI: firmware version number is 0x%-04x", sresult);
    
    /*
     * Determine initial memory size
     */
    
    mci->regptr->cmdreg = MCI_CMD_MEMSIZE;
    sresult = mci->regptr->argreg;
    if (verbose)
	printf("\nMCI: memory size is 0x%-04x words", sresult);
    
    /*
     * Scan controller reporting interfaces
     */
    
    for (i = 0; i < MCI_MAXUNITS; i++) {
	if (verbose)
	    printf("\nMCI: interface %d ", i);
	mci->interfacetype[i] = -1;
	mci->regptr->argreg = i;
	mci->regptr->cmdreg = MCI_CMD_SELECT;
	sresult = mci->regptr->cmdreg;
	if (sresult != MCI_RSP_OKAY) {
	    if (verbose)
		printf("not present - code %#x", sresult);
	} else {
	    typecode = mci->regptr->argreg;
	    capability = (typecode >> 8);
	    typecode &= 0xFF;
	    if (verbose)
		printf("is %s, marketing code %#x",
		       devicestring(typecode), capability);
	    mci->interfacetype[i] = typecode;
	    if (typecode == MCI_TYPE_ETHER)
		stationaddress(mci, i);
	}
    }
    
}  /*  end mci_config()  */

/*
 * mema_test
 * Run tests of mema
 * Returns FALSE if user quit out of error display
 */

boolean mema_test(mci, linect)
    mcitype *mci;
    int	*linect;
{
    register ushort	i;
    ushort		sresult;
    
    /*
     * mema -- write zeros
     */
    
    errors = 0;
    if (verbose)
	printf("\nMCI: writing MEMA with zeros...");
    mci->regptr->argreg = MEMA_BASE;
    mci->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    for (i = MEMA_BASE; i < MEMA_WORDS; i++) {
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = MCI_CMD_MEMA_WRITE;
    }
    mci->regptr->argreg = MEMA_BASE;
    mci->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    for (i = MEMA_BASE; i < MEMA_WORDS; i++) {
	mci->regptr->cmdreg = MCI_CMD_MEMA_READ;
	sresult = mci->regptr->argreg;
	if (sresult != 0 && !looping) {
	    errors++;
	    if (!store_error(&diag_results[mci->unit]->mema_test, 0, sresult, i, 0 ,0 ,0 ,0))
	    	return(FALSE);                    /*  all bits are bad  */
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: wrote MEMA 0x%-04x with zeros, read 0x%-04x",
		       i, sresult);
	    }
	}
    }
    
    /*
     * mema -- write ones
     */
    
    if (verbose)
	printf("\nMCI: writing MEMA with all ones...");
    mci->regptr->argreg = MEMA_BASE;
    mci->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    for (i = MEMA_BASE; i < MEMA_WORDS; i++) {
	mci->regptr->argreg = 0xFFFF;
	mci->regptr->cmdreg = MCI_CMD_MEMA_WRITE;
    }
    mci->regptr->argreg = MEMA_BASE;
    mci->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    for (i = MEMA_BASE; i < MEMA_WORDS; i++) {
	mci->regptr->cmdreg = MCI_CMD_MEMA_READ;
	sresult = mci->regptr->argreg;
	if (sresult != 0xFFFF && !looping) {
	    errors++;
	    if (!store_error(&diag_results[mci->unit]->mema_test, 0xFFFF, sresult, i, 0, 0, 0, 0))
	    	return(FALSE);                      /*  all bits are bad  */
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: wrote MEMA 0x%-04x with ones, read 0x%-04x",
		       i, sresult);
	    }
	}
    }
    
    /*
     * mema -- write address + 1
     */
    
    if (verbose)
	printf("\nMCI: writing MEMA with address+1...");
    mci->regptr->argreg = MEMA_BASE;
    mci->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    for (i = MEMA_BASE; i < MEMA_WORDS; i++) {
	mci->regptr->argreg = i + 1;
	mci->regptr->cmdreg = MCI_CMD_MEMA_WRITE;
    }
    mci->regptr->argreg = MEMA_BASE;
    mci->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    for (i = MEMA_BASE; i < MEMA_WORDS; i++) {
	mci->regptr->cmdreg = MCI_CMD_MEMA_READ;
	sresult = mci->regptr->argreg;
	if (sresult != (i + 1) && !looping) {
	    errors++;
	    if (!store_error(&diag_results[mci->unit]->mema_test, i + 1, sresult, i, 0, 0, 0, 0))
	    	return(FALSE);                    /*  all bits are bad  */
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: wrote MEMA loc %#x with 0x%-04x, read 0x%-04x",
		       i, i + 1, sresult);
	    }
	}
    }
    if (errors == 0 && !looping)
	diag_results[mci->unit]->mema_test.stat = STAT_PASS;
    if (errors)
	return(FALSE);
    else
    	return(TRUE);
    
}  /*  end mema_test()  */

/*
 * memd_test
 * Run tests of memd
 * Returns FALSE if user quit out of error display
 */

boolean memd_test(mci, linect)
    mcitype *mci;
    int	*linect;
{
    register uint	i;
    uint		sresult, expect, size;
    
    /* Find out how big memd is */
    
    mci->regptr->argreg = 1;
    mci->regptr->cmdreg = MCI_CMD_MEMSIZE;
    size = mci->regptr->argreg;
    
    /*
     * memd - all zeros
     */
    
    errors = 0;
    if (verbose)
	printf("\nMCI: writing MEMD with all zeros...");
    
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    for (i = 0; i < size; i += MEMD_STEP)
	mci->regptr->write0short = 0L;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    for (i = 0; i < size; i += MEMD_STEP) {
	sresult = mci->regptr->readshort;
	if (sresult != 0 && !looping) {
	    errors++;
	    if (!store_error(&diag_results[mci->unit]->memd_test, 0, sresult, i, 0, 0, 0, 0))
	    	return(FALSE);                    /*  all bits are bad  */
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: wrote MEMD loc %#x with zeros, read 0x%-04x",
		       i, sresult);
	    }
	}
    }
    
    /*
     * memd - all ones
     */
    
    if (verbose)
	printf("\nMCI: writing MEMD with all ones...");
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    for (i = 0; i < size; i += MEMD_STEP)
	mci->regptr->write0short = 0xFFFF;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    for (i = 0; i < size; i += MEMD_STEP) {
	sresult = mci->regptr->readshort;
	if (sresult != 0xFFFF && !looping) {
	    errors++;
	    if (!store_error(&diag_results[mci->unit]->memd_test, 0xFFFF, sresult, i, 0, 0, 0, 0))
	    	return(FALSE);                    /*  all bits are bad  */
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: wrote MEMD loc %#x with ones, read 0x%-04x",
		       i, sresult);
	    }
	}
    }
    
    /*
     * memd - address+1
     */
    
    if (verbose)
	printf("\nMCI: writing MEMD with address+1...");
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    for (i = 0; i < size; i += MEMD_STEP)
	mci->regptr->write0short = (i + 1);
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    for (i = 0; i < size; i += MEMD_STEP) {
	expect = (i + 1);
	sresult = mci->regptr->readshort;
	if (sresult != expect && !looping) {
	    errors++;
	    if (!store_error(&diag_results[mci->unit]->memd_test, expect, sresult, i, 0, 0, 0, 0))
	    	return(FALSE);                    /*  all bits are bad  */
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: wrote MEMD loc %#x with 0x%-04x, read 0x%-04x",
		       i, expect, sresult);
	    }
	}
    }
    if (errors == 0 && !looping)
	diag_results[mci->unit]->memd_test.stat = STAT_PASS;
    if (errors)
	return(FALSE);
    else
	return(TRUE);
    
}  /*  end memd_test()  */

/*
 * ethermem_test
 * Run memory test over Ethernet address filter memories
 */

boolean ethermem_test(mci, linect, unit)
    mcitype *mci;
    int	*linect;
    int	unit;
{
    register ushort	i;
    ushort		sresult, data;
    
    /*
     * Select the interface
     */
    
    if (!mci_select(mci, unit)) {
	if (!looping)
	    (void)store_error(&diag_results[mci->unit]->add_flt_mem[unit], 0, 0, 0, 0, 0, 0, 0);
	return(FALSE);
    }
    
    /*
     * Ethernet address memory -- write zeros
     */
    
    errors = 0;
    if (verbose)
	printf("\nMCI: writing AF MEM%d memory with zeros...", unit);
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    for (i = 0; i < AF_WORDS; i += AF_STEP) {
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = MCI_CMD_AF_WRITE;
    }
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    for (i = 0; i < AF_WORDS; i += AF_STEP) {
	mci->regptr->cmdreg = MCI_CMD_AF_READ;
	sresult = mci->regptr->argreg;
	sresult &= 0xFF00;
	if (sresult != 0 && !looping) {
	    errors++;
	    if(!store_error(&diag_results[mci->unit]->add_flt_mem[unit], 0, sresult, i, 0, 0, 0, 0))
		return(FALSE);
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: wrote AF MEM%d loc %#x with zeros, read 0x%-04x",
		       unit, i, sresult);
	    }
	}
    }
    
    /*
     * Ethernet address memory -- write ones
     */
    
    if (verbose)
	printf("\nMCI: writing AF MEM%d memory with ones...", unit);
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    for (i = 0; i < AF_WORDS; i += AF_STEP) {
	mci->regptr->argreg = 0xFFFF;
	mci->regptr->cmdreg = MCI_CMD_AF_WRITE;
    }
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    for (i = 0; i < AF_WORDS; i += AF_STEP) {
	mci->regptr->cmdreg = MCI_CMD_AF_READ;
	sresult = mci->regptr->argreg;
	sresult &= 0xFF00;
	if (sresult != 0xFF00 && !looping) {
	    errors++;
    	    if(!store_error(&diag_results[mci->unit]->add_flt_mem[unit], 0xFF00, sresult, i, 0, 0, 0, 0))
		return(FALSE);
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: wrote AF MEM%d loc %#x with ones, read 0x%-04x",
		       unit, i, sresult);
	    }
	}
    }
    
    /*
     * Ethernet address memory -- write address + 1
     */
    
    if (verbose)
	printf("\nMCI: writing AF MEM%d memory with address+1...", unit);
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    for (i = 0; i < AF_WORDS; i += AF_STEP) {
	mci->regptr->argreg = ((i + 1) << 8);	/* data write in high byte */
	mci->regptr->cmdreg = MCI_CMD_AF_WRITE;
    }
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    for (i = 0; i < AF_WORDS; i += AF_STEP) {
	mci->regptr->cmdreg = MCI_CMD_AF_READ;
	sresult = mci->regptr->argreg;
	sresult &= 0xFF00;
	data = ((i + 1) << 8);
	if (sresult != data && !looping) {
	    errors++;
	    if(!store_error(&diag_results[mci->unit]->add_flt_mem[unit], ((i + 1) << 8), sresult, i, 0, 0, 0, 0))
		return(FALSE);
	    if (reporting) {
		if (quit_display((*linect)++, NULL))
		    return(FALSE);
		printf("\nMCI: wrote AF MEM%d loc %#x with 0x%-04x, read 0x%-04x",
		       unit, i, data, sresult);
	    }
	}
    }
    if (errors == 0 && !looping)
	diag_results[mci->unit]->add_flt_mem[unit].stat = STAT_PASS;
    if (errors)
	return(FALSE);
    else
	return(TRUE);
    
}  /*  end ethermem_test()  */

/*
 * devicestring
 * Translate an MCI device type code into a string
 */

char	*devicestring(typecode)
    ushort typecode;
{
    switch ((int) typecode) {
      case MCI_TYPE_ETHER: 
	return("Ethernet");
      case MCI_TYPE_SERIAL: 
	return("Serial");
      default:
	if (reporting)
	    printf("unknown - %#x", typecode);
	return(NULL);
    }
    
}  /*  end devicestring()  */

/*
 * mci_select
 * Select the specified interface
 */

boolean mci_select(mci, unit)
    mcitype *mci;
    int	unit;
{
    ushort sresult;
    
    mci->regptr->argreg = (ushort) unit;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    sresult = mci->regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	errors++;
	if (verbose & (sresult != MCI_RSP_ERR_NPRS))
	    printf("\nMCI: unable to select interface %d - response is 0x%x",
		   unit, sresult);
	return(FALSE);
    }
    return(TRUE);
    
}  /*  end mci_select()  */


/*
 * mci_reset
 * Reset controller and wait for MCI to get its act together.
 */

void mci_reset(mci)
    mcitype *mci;
{
    int	i;
    
    mci->regptr->cmdreg = MCI_CMD_RESET;
    delay(RESET_TIME);
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_INTERRUPT;
    for (i = 0; i < MCI_MAXUNITS; i++) {
	(void)mci_select(mci, i);
	mci->regptr->argreg = MCI_IF_DISABLE;
	mci->regptr->cmdreg = MCI_CMD_STATUS;
    }
    
}  /*  end mci_reset()  */

/*
 * allocate_buffers
 * Allocate some buffers for us to play with.
 */

boolean allocate_buffers(mci, unit, overhead, size, offset, transmit, receive)
    mcitype *mci;
    int	unit, overhead, size, offset, transmit, receive;
{
    mciregtype	*regptr;
    ushort	sresult;
    int		i;
    
    if (verbose)
	printf("\nBuffers for %d: %d words (%d overhead)",
	       unit, size, overhead);
    regptr = mci->regptr;
    regptr->argreg = overhead;
    regptr->cmdreg = MCI_CMD_OVERHEAD;		/* set overhead */
    regptr->argreg = size;
    regptr->cmdreg = MCI_CMD_BUFFERSIZE;		/* set data size */
    if (verbose)
	printf(", %d transmit", transmit);
    for (i = 0; i < transmit; i++) {
	regptr->argreg = 0;
	regptr->cmdreg = MCI_CMD_TX_ALLOCATE;
	sresult = regptr->cmdreg;
	if (sresult != MCI_RSP_OKAY) {
	    errors++;
	    if (reporting)
		printf("\nMCI: TX_ALLOCATE failed - %#x", sresult);
	    return (FALSE);
	}
    }
    
    /*
     * Allocate some RX buffers.
     */
    
    if (verbose)
	printf(", %d receive (%d offset)", receive, offset);
    if (!mci_select(mci, unit))
	return(FALSE);
    for (i = 0; i < receive; i++) {
	regptr->argreg = offset;
	regptr->cmdreg = MCI_CMD_RX_ALLOCATE;
	sresult = regptr->cmdreg;
	if (sresult != MCI_RSP_OKAY) {
	    errors++;
	    if (reporting)
		printf("\nMCI: RX_ALLOCATE failed - %#x", sresult);
	    return (FALSE);
	}
    }
    
    /*
     * Set up transmit queue limits
     */
    
    if (!mci_select(mci, unit))
	return(FALSE);
    regptr->argreg = transmit;
    regptr->cmdreg = MCI_CMD_QUEUE;
    regptr->argreg = transmit;
    regptr->cmdreg = MCI_CMD_SLIMIT;
    
    return(TRUE);
    
}  /*  end allocate_buffers()  */

/*
 * stationaddress
 * Read and print out station address of an interface
 */

void stationaddress(mci, unit)
    mcitype *mci;
    int	unit;
{
    register ushort	i;
    ushort		sresult;
    
    if (!mci_select(mci, unit))
	return;
    if (verbose)	 {
	printf(", address is: 0x");
    	for (i = 0; i < ETHERBYTES; i++) {
	    mci->regptr->argreg = i;
	    mci->regptr->cmdreg = MCI_CMD_STATION;
	    sresult = mci->regptr->argreg;
	    printf("%-02x", sresult & 0xFF);
	}
    }
    
}  /*  end stationaddress()  */

/*
 * mci_aftest
 * Exhaustively test the address filter mechanism.
 * This could take a while.....
 */

boolean mci_aftest(mci, unit)
    mcitype *mci;
    int	unit;
{
    ushort	fsm, octet;
    int		linect;
    
    mci_reset(mci);				/* reset the world */
    errors = 0;
    if (!mci_select(mci, unit)) {		/* select interface */
	if (!looping) 
	    (void)store_error(&diag_results[mci->unit]->add_flt[unit], 0, 0, 0,
			      0, 0, 0, 0);
	return(FALSE);
    }    
    
    /*
     * No overhead, 100 data words, no offset, one transmit, two receive
     */
    
    if (!allocate_buffers(mci, unit, 0, 100, 0, 1, 2)) {
	if (!looping)
	    (void)store_error(&diag_results[mci->unit]->add_flt[unit], 0, 0, 0,
			      0, 0, 0, 0);
	return(FALSE);
    }
    
    /*
     * Set interface status to loopback mode
     */
    
    mci->regptr->argreg = MCI_IF_LOOPBACK;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    linect = 0;
    
    /*
     * Detailed check of octet 0 of FSM 0
     */
    
    if (verbose)
	printf("\nMCI: AF%d - detailed test of octet 0, fsm 0", unit);
    if (check_one(mci, unit, 0, 0, &linect, 1) == FALSE)
	return(FALSE);
    
    /*
     * Very quick test of other octets and FSM's
     */
    
    for (octet = 0; octet < ETHERBYTES ; octet++) {
	for (fsm = 0; fsm < 8; fsm++) {
	    if (verbose)
		printf("\nMCI: AF%d - quick test of octet %d, fsm %d",
		       unit, octet, fsm);
	    if (check_one(mci, unit, fsm, octet, &linect, 75) == FALSE)
		return(FALSE);
	}
    }
    if (errors == 0 && !looping) 
	diag_results[mci->unit]->add_flt[unit].stat = STAT_PASS;
    if (errors)
	return(FALSE);
    else
	return(TRUE);
    
}  /*  end mci_aftest()  */

/*
 * check_one
 * Given a FSM and octet number, do an exhaustive check
 */

boolean check_one(mci, unit, fsm, octet, linect, skipcount)
    mcitype *mci;
    int	unit, fsm, octet;
    int	*linect;
    int	skipcount;
{
    register int	i;
    register ushort	data, test;
    boolean		temp;
    uchar		packet[PKTLENGTH], rpacket[PKTLENGTH];
    
    for (data = 0; data < 256; data += skipcount) {
	mci->regptr->argreg = unit;
	mci->regptr->cmdreg = MCI_CMD_SELECT;
	if (!mci_setmask(mci, fsm, octet, data)) {
	    if (!looping)
		(void)store_error(&diag_results[mci->unit]->add_flt[unit], 0, 0, 0,
				  fsm, octet, 0, 0);
	    return(FALSE);
	}
	
	for (test = 0; test < 256; test += skipcount) {
	    for (i = 0; i < ETHERBYTES; i++)
		packet[i] = 0;
	    packet[octet] = test;
	    temp = (af_sndrcv(mci, unit, packet, rpacket, PKTLENGTH));
	    switch (temp) {
	      case TRUE:
		for (i = 0; i < ETHERBYTES; i++) {
		    if (packet[i] != rpacket[i] && !looping) {
			errors++;
			(void)store_error(&diag_results[mci->unit]->add_flt[unit], packet[i],
					  rpacket[i], 0, fsm, octet, test, data);
			if (!reporting)
			    break;
			if (quit_display((*linect)++, NULL))
			    return(FALSE);
			printf("\nMCI: AF%d - rcvd unexpected packet", unit);
			printf(": octet %d, FSM %d, sent %#x, enabled %#x",
			       octet, fsm, test, data);
			if (quit_display((*linect)++, NULL))
			    return(FALSE);
			printf("\nSent: %e, Rcvd: %e", packet, rpacket);
			if (!more())
			    return(FALSE);
			break;
		    }
		}
		break;
		
	      case FALSE:
		
		/*
		 * Did not receive packet.  Okay only if target data.
		 */
		
		if (test != data && !looping) {
		    errors++;
		    (void)store_error(&diag_results[mci->unit]->add_flt[unit], 0, 0, 0,
				      fsm, octet, test, data);
		    if (!reporting)
			break;
		    if (quit_display((*linect)++, NULL))
			return(FALSE);
		    printf("\nMCI: AF%d - did not rcv expected packet", unit);
		    printf(": octet %d, FSM %d, sent %#x, enabled %#x",
			   octet, fsm, test, data);
		    if (quit_display((*linect)++, NULL))
			return(FALSE);
		    printf("\nSent: %e", packet);
		    if (!more())
			return(FALSE);
		}
		break;
		
	      default:
		
		/*
		 * Some other error happened.  Quit to preserve MCI state.
		 */
		
		errors++;
		if (!looping) 
		    (void)store_error(&diag_results[mci->unit]->add_flt[unit], 0, 0, 0,
				      fsm, octet, 0, 0);
		return(FALSE);
		break;
	    }
	}
    }
    return(TRUE);
    
}  /*  end check_one()  */

/*
 * mci_setmask
 * Set up an address mask for a particular test
 */

boolean mci_setmask(mci, fsm, octet, data)
    register mcitype *mci;
    ushort fsm, octet, data;
{
    register ushort i, accept, address;
    
    /*
     * Compute acceptance mask and disable receiver.
     */
    
    accept = ((1 << fsm) << 8);
    mci->regptr->argreg = MCI_IF_DISABLE;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    
    /*
     * Refuse everything
     */
    
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    mci->regptr->argreg = 0;
    for (i = 0; i < AF_WORDS; i++)
	mci->regptr->cmdreg = MCI_CMD_AF_WRITE;
    
    /*
     * Accept an all zeros address, except for that one octet.
     */
    
    for (i = 0; i < ETHERBYTES; i++) {
	if (i != octet) {
	    mci->regptr->argreg = (i << 8);
	    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
	    mci->regptr->argreg = accept;
	    mci->regptr->cmdreg = MCI_CMD_AF_WRITE;
	}
    }
    
    /*
     * Receive all values of the selected octet
     */
    
    address = (octet << 8);
    mci->regptr->argreg = address;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    mci->regptr->argreg = accept;
    for (i = address; i < address + 256; i++)
	mci->regptr->cmdreg = MCI_CMD_AF_WRITE;
    
    /*
     * Except that one particular address
     */
    
    address = (octet << 8) | (data & 0xFF);
    mci->regptr->argreg = address;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_AF_WRITE;
    
    /*
     * Enable receiver
     */
    
    mci->regptr->argreg = MCI_IF_ENABLE;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    if (mci->regptr->cmdreg != MCI_RSP_OKAY) {
	errors++;
	if (reporting)
	    printf("\nMCI: RX enabled failed");
	return(FALSE);
    }
    return(TRUE);
    
}  /*  end mci_setmask()  */

/*
 * af_sndrcv
 * Send a packet and wait for something to come back or not
 * Returns TRUE if a packet was received, FALSE otherwise.
 * Returns -1 if some other error.
 */

af_sndrcv(mci, unit, packet, rpacket, len)
    mcitype *mci;
    int	unit, len;
    ulong *packet;
    ulong *rpacket;
{
    ushort	sresult;
    int		waiting;
    
    mci->regptr->argreg = unit;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
    sresult = mci->regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	errors++;
	if (reporting) {
	    printf("\nMCI: TX0_RESERVE failed - %#x", sresult);
	    printf("\n  Possible output hang on interface %d", unit);
	}
	return(-1);
    }
    
    /*
     * Set pointer to start of buffer (TX0_SELECT).
     */
    
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_TX0_SELECT;
    
    /*
     * Put header bytes into transmit buffer.
     * Then write byte length and start transmission.
     */
    
    mci->regptr->write0long = *packet++;
    mci->regptr->write0long = *packet;
    mci->regptr->argreg = unit;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    mci->regptr->argreg = len | MCI_TX_HEARSELF;
    mci->regptr->cmdreg = MCI_CMD_TX0_START;
    waiting = 1000;
    while (waiting--) {
	mci->regptr->cmdreg = MCI_CMD_RX_SETUP;
	sresult = mci->regptr->argreg;
	if (sresult != 0xFFFF)
	    break;
    }
    if (waiting <= 0)
	return(FALSE);
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_RX_SELECT;
    *rpacket++ = mci->regptr->readlong;
    *rpacket = mci->regptr->readlong;
    mci->regptr->cmdreg = MCI_CMD_RX_FLUSH;
    return((sresult == unit) ? TRUE : FALSE);
    
}  /*  end af_sndrcv()  */

/*
 * ping_test
 * This could take a while.....
 */

boolean mci_ping(mci, unit)
    mcitype *mci;
    int	unit;
{			  
    ushort		rlength;
    int			linect, temp1;
    register int	i;
    register ushort	data;
    boolean		temp;
    uchar		*tpacket, *rpacket;
    MCI_ERROR		*test;
    
    tpacket = packet1;
    rpacket = packet2;
    abort_count = 0;
    
    if (mci->interfacetype[unit] == MCI_TYPE_ETHER)
	test = &diag_results[mci->unit]->ether_ping_test[unit];
    else
	test = &diag_results[mci->unit]->serial_ping_test[unit];
    
    mci_reset(mci);				/* reset the world */
    errors = 0;
    if (!mci_select(mci, unit))  {		/* select interface */
	if (!looping) 
	    (void)store_error(test, 0, 0, 0, 1, 0, 0, 0);
	if (reporting) {
	    printf("\nPING%d ERROR: Unable to select interface.", unit);
	    ++linect;    
	}
	return(FALSE);
    }
    /*
     * No overhead, 800 data words, no offset, one transmit, two receive
     */
    
    if (!allocate_buffers(mci, unit, 0, 800, 0, 1, 2)) {
	if (!looping) 
	    (void)store_error(test, 0, 0, 0, 1, 0, 0, 1);
	if (reporting){
	    printf("\nPING%d ERROR: Unable to allocate buffers.", unit);
	    ++linect;    
	}
	return(FALSE);
    }
    
    /*
     * Set interface loopback mode
     */
    
    linect = 0;
    
    for(i = 0; i < MAXPACKET; i++)
	tpacket[i] = (uchar) (i + 1);
    
    if (mci->interfacetype[unit] == MCI_TYPE_ETHER) {
	if (!mci_setfilter(mci,tpacket)) {
	    if (!looping) 
		(void)store_error(test, 0, 0, 0, 1, 0, 0, 2);
	    if (reporting) {
		printf("\nPING%d ERROR: Unable to setup address filter.", unit);
		++linect;
	    }
	    return(FALSE);
	}
	mci->regptr->argreg = MCI_IF_NOLOOPBACK;
	mci->regptr->cmdreg = MCI_CMD_STATUS;
    }
    else if (mci->interfacetype[unit] == MCI_TYPE_SERIAL) {
        temp1 = applique_type(mci, unit);
        if (temp1 != MCI_DCE_RS232)  {
    		if (!looping)
        	    (void)store_error(test, (ushort)MCI_DCE_RS232, (ushort)temp1, 0, 1, 0, 0, 3);
    		if (reporting) {
    	    		printf("\nPING%d ERROR: Applique type wrong or reporting incorrectly.", unit);
    	    		printf("\n		Should be RS232_DCE (0x%x), reports 0x%x", MCI_DCE_RS232, temp1);
    		}
    		return (FALSE);
        }
        mci->regptr->argreg = MCI_IF_DISABLE;
        mci->regptr->cmdreg = MCI_CMD_STATUS;
        mci->regptr->argreg = serial_clk_div;
        mci->regptr->cmdreg = MCI_CMD_CLOCKRATE;
        mci->regptr->argreg = serial_prescale;
        mci->regptr->cmdreg = MCI_CMD_CLOCKTYPE;
        mci->regptr->argreg = MCI_IF_ENABLE;
        mci->regptr->cmdreg = MCI_CMD_STATUS;
        temp1 = mci->regptr->argreg;
        if (temp1 != MCI_RSP_OKAY && !looping) {
        	(void)store_error(test, 0, (ushort)temp1, 0, 1, 0, 0, 4);
        	if (reporting) {
    			printf("\nPING%d ERROR: Unable to enable port.  Response = 0x%x", unit, temp1);
    			++linect;
        	}
        	return (FALSE);
        }
	mci->regptr->argreg = MCI_IF_LOOPBACK;
	mci->regptr->cmdreg = MCI_CMD_STATUS;

    } else {
	if (!looping)
	    	(void)store_error(test, 0, 0, 0, 1, 0, 0, 5);
	if (reporting)
    	   	printf("\nPING%d ERROR: Unknown interface type.", unit);
        return (FALSE);
    }
    
    for(i = PKTLENGTH; i < MAXPACKET; i++) {
	if(verbose && (i == PKTLENGTH))
	    printf("\nMCI: PING%d - ", unit);
	else if (((i & 0x1f) == 0) && verbose)
		printf("!");			       
	temp = (ping(mci, unit, tpacket, rpacket, i));
	switch (temp) {
	  case TRUE:
	    rlength = mci->regptr->rxlength;
	    if (rlength != i && !looping) {
		errors++;
		(void)store_error(test, 0, 0, 0, i, (int)rlength, 0, 6);
		if (reporting) {
		    printf("\nPING%d ERROR - packet length sent : %d, received %d",
			   unit, i, rlength);
		    ++linect;
		}
		return(FALSE);
	    }
	    for (data = 0; data < i; data++) {
		if (tpacket[data] != rpacket[data] && !looping) {
		    errors++;
		    (void)store_error(test, (ushort)tpacket[data], (ushort)rpacket[data], (ushort)data, i, (int)rlength, 0, 7);
		    if (!reporting)
			break;
		    else {
			printf("\nPING%d ERROR: Data mismatch on packet length %d", unit, i);
			++linect;
			printf("\n     byte %d should be 0x%x, was 0x%x",
			       data, tpacket[data], rpacket[data]);
			++linect;
		    }
		    return(FALSE);
		}
	    }
	    break;
	    
	  case FALSE:
	    
	    /*
	     * Did not receive packet.
	     */
	    
	    errors++;
	    if (!looping)
		(void)store_error(test, 0, 0, 0, i, 0, 0, 8);
	    if (!reporting)
		break;
	    else {
		printf("\nPING%d ERROR: - did not rcv expected packet", unit);
		++linect;
		printf(": sent packet length %d", i);
		++linect;
	    }
	    return(FALSE);
	    
	  default:
	    
	    /*
	     * Some other error happened.  Quit to preserve MCI state.
	     */
	    
	    errors++;
	    if (!looping)
		(void)store_error(test, 0, 0, 0, 0, 0, 0, 0);
	    return(FALSE);
	    break;
	    
	    
	}
    }
    if (errors == 0 && !looping)
	test->stat = STAT_PASS;
    if (errors)
	return(FALSE);
    else
	return(TRUE);
    
}  /*  end mci_ping()  */

/*
 * mci_setfilter
 * Set up an address mask for a particular test
 */

boolean mci_setfilter(mci, packet)
    register mcitype *mci;
    uchar *packet;
{
    register ushort i, accept, address;
    
    /*
     * Compute acceptance mask and disable receiver.
     */
    
    accept = (1 << 8);
    mci->regptr->argreg = MCI_IF_DISABLE;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    
    /*
     * Refuse everything
     */
    
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
    mci->regptr->argreg = 0;
    for (i = 0; i < AF_WORDS; i++)
	mci->regptr->cmdreg = MCI_CMD_AF_WRITE;
    
    /*
     * Accept packet address.
     */
    
    for (i = 0; i < ETHERBYTES; i++) {
	mci->regptr->argreg = (i << 8) + packet[i];
	mci->regptr->cmdreg = MCI_CMD_AF_SELECT;
	mci->regptr->argreg = accept;
	mci->regptr->cmdreg = MCI_CMD_AF_WRITE;
    }
    
    /*
     * Enable receiver
     */
    
    mci->regptr->argreg = MCI_IF_ENABLE;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    if (mci->regptr->cmdreg != MCI_RSP_OKAY) {
	errors++;
	if (reporting)
	    printf("\nMCI: RX enabled failed");
	return(FALSE);
    }
    return(TRUE);
    
}  /*  end mci_setfilter()  */

/*
 * ping
 * Send a packet and wait for something to come back or not
 * Returns TRUE if a packet was received, FALSE otherwise.
 * Returns -1 if some other error.
 */

ping(mci, unit, packet, rpacket, len)
    mcitype *mci;
    int	unit, len;
    ulong *packet;
    ulong *rpacket;
{
    ushort	sresult, rstatus, rlength;
    int		waiting, i;
    
    mci->regptr->argreg = unit;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
    sresult = mci->regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	errors++;
	if (reporting) {
	    printf("\nMCI: TX0_RESERVE failed - %#x", sresult);
	    printf("\n? Possible output hang on interface %d", unit);
	}
	return(-1);
    }
    
    /*
     * Set pointer to start of buffer (TX0_SELECT).
     */
    
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_TX0_SELECT;
    
    /*
     * Put packet into transmit buffer.
     * Then write byte length and start transmission.
     */
    
    for(i = 0; i <= len; i += 4)
	mci->regptr->write0long = *packet++;
    mci->regptr->argreg = unit;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    if (mci->interfacetype[unit] == MCI_TYPE_ETHER)
	mci->regptr->argreg = len | MCI_TX_HEARSELF;
    else 
	mci->regptr->argreg = len;
    
    mci->regptr->cmdreg = MCI_CMD_TX0_START;
    
    /*
     * Wait for packet to come back.
     *
     */
    
    if (cpu_type == CPU_CSC3) /* Adjust for processor speed */
	waiting = 15;
    else
	waiting = 1;
    waiting = waiting * serial_clk_div * serial_prescale * len;
    while (waiting--) {
	mci->regptr->cmdreg = MCI_CMD_RX_SETUP;
	sresult = mci->regptr->argreg;
	if (sresult != 0xFFFF) {
	    rlength = mci->regptr->rxlength;
	    rstatus = (mci->regptr->rxstatus & MCI_SX_MASK);    
	    if ((rlength == 0) || ((rstatus & MCI_SX_ABORT) == MCI_SX_ABORT)) {
                mci->regptr->cmdreg = MCI_CMD_RX_FLUSH;
		abort_count++;
	    }
	    else
		break;
	}
    }
    if (waiting <= 0)
	return(FALSE);
    
    /*
     * Read packet if it comes back.
     */
    
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_RX_SELECT;
    for(i = 0; i <= len; i += 4)
	*rpacket++ = mci->regptr->readlong;
    mci->regptr->cmdreg = MCI_CMD_RX_FLUSH;
    return((sresult == unit) ? TRUE : FALSE);
    
}  /*  end ping()  */

/*
 * mci_search
 * Search mema and memd for a pattern
 */

void mci_search(mci)
    mcitype *mci;
{
    char	buff[BUFLEN];
    mciregtype	*regptr;
    ushort	pattern, result;
    int		linect, i, size;
    
    pattern = 0;
    printf("\nPattern: ");
    if (gets(buff))
	pattern = (ushort) parse_unsigned(buff);
    else
	return;
    linect = 0;
    regptr = mci->regptr;
    printf("\nSearching for 0x%-04x...", pattern);
    for (i = 0; i < MEMA_WORDS; i++) {
	regptr->argreg = i;
	regptr->cmdreg = MCI_CMD_MEMA_SELECT;
	regptr->cmdreg = MCI_CMD_MEMA_READ;
	result = regptr->argreg;
	if (result == pattern) {
	    if (quit_display(linect++, NULL))
		return;
	    printf("\nMEMA 0x%-04x: 0x%-04x", i, result);
	}
    }
    
    /*  Find out how big MEMD is  */
    
    mci->regptr->argreg = 1;
    mci->regptr->cmdreg = MCI_CMD_MEMSIZE;
    size = mci->regptr->argreg;
    
    for (i = 0; i < size; i++) {
	regptr->argreg = i;
	regptr->cmdreg = MCI_CMD_MEMD_SELECT;
	result = regptr->readshort;
	if (result == pattern) {
	    if (quit_display(linect++, NULL))
		return;
	    printf("\nMEMD 0x%-04x: 0x%-04x", i, result);
	}
    }
    
}  /*  end mci_search()  */

/*
 * memd_readwrite
 * Special read/write testing for scoping memd failures.
 */

void memd_readwrite(mci, loc, data)
    mcitype *mci;
    ushort loc;
    ushort data;
{
    mciregtype	*regptr;
    ushort	result;
    
    regptr = mci->regptr;
    abort = FALSE;
    printf("\nWriting MEMD location 0x%-04x with data 0x%-04x. Go to Reading with BREAK", loc, data);
    while (!abort) {
	regptr->argreg = loc;
	regptr->cmdreg = MCI_CMD_MEMD_SELECT;
	regptr->write0short = data;
    }
    abort = FALSE;
    printf("\nReading MEMD location 0x%-04x.  End test with BREAK", loc);
    while (!abort) {
	regptr->argreg = loc;
	regptr->cmdreg = MCI_CMD_MEMD_SELECT;
	result = regptr->readshort;
    }
    abort = FALSE;
    printf("\nRead a 0x%-04x from MEMD location 0x%-04x", data, loc);
    
}  /*  end memd_readwrite()  */


/* memd_load
 * Load up all of memd with data
 */

void memd_load(mci, data)
    mcitype *mci;
    ushort data;
{
    mciregtype	*regptr;
    ushort	pointer;
    int		i, size;
    
    /*  Find out how big MEMD is  */
    
    mci->regptr->argreg = 1;
    mci->regptr->cmdreg = MCI_CMD_MEMSIZE;
    size = mci->regptr->argreg;
    
    regptr = mci->regptr;
    regptr->argreg = 0;
    regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    for (i = 0; i <= size; i++)
	regptr->write0short = data;
    
}  /*  end memd_load()  */

/*
 * memd_write
 * Write once and return
 */

void memd_write(mci, loc, data)
    mcitype *mci;
    ushort loc;
    ushort data;
{
    mciregtype	*regptr;
    
    printf("\nWriting 0x%x to MEMD location 0x%x once...", data, loc);
    regptr = mci->regptr;
    regptr->argreg = loc;
    regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    regptr->write0short = data;
    
}  /*  end memd_write()  */


/*
 * memd_read
 * Read once from selected location
 */

void memd_read(mci, loc)
    mcitype *mci;
    ushort loc;
    
{
    mciregtype	*regptr;
    ushort	result;
    
    regptr = mci->regptr;
    regptr->argreg = loc;
    regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    result = regptr->readshort;
    
    printf("\nRead a 0x%-04x from MEMD location 0x%-04x", result, loc);
    
}  /*  end memd_read()  */

/*
 * mema_readwrite
 * Special read/write testing for scoping mema failures.
 */

void mema_readwrite(mci, loc, data)
    mcitype *mci;
    ushort loc;
    ushort data;
{
    mciregtype	*regptr;
    ushort	result;
    
    regptr = mci->regptr;
    result = 0;
    abort = FALSE;
    printf("\nWriting MEMA location 0x%-03x with data 0x%-04x. Go to Reading with BREAK",
	   loc, data);
    while (!abort) {
	regptr->argreg = loc;
	regptr->cmdreg = MCI_CMD_MEMA_SELECT;
	mci->regptr->argreg = data;
	mci->regptr->cmdreg = MCI_CMD_MEMA_WRITE;
    }
    abort = FALSE;
    printf("\nReading MEMA location 0x%-03x. End test with BREAK", loc);
    while (!abort) {
	regptr->argreg = loc;
	regptr->cmdreg = MCI_CMD_MEMA_SELECT;
	mci->regptr->cmdreg = MCI_CMD_MEMA_READ;
	result = mci->regptr->argreg;
    }
    printf("\nRead a 0x%-03x from MEMA location 0x%-04x", result, loc);
    abort = FALSE;
    
}  /*  end mema_readwrite()  */

/*
 * addressfilter_readwrite
 * Special read/write testing for scoping af mem failures.
 */

boolean af_readwrite(mci, unit, address, data)
    mcitype *mci;
    int unit;
    ushort address;
    ushort data;
{
    mciregtype	*regptr;
    ushort	result;
    
    abort = FALSE;
    regptr = mci->regptr;
    
    /*
     * Select the interface after resetting world
     */
    
    mci_reset(mci);
    if (!mci_select(mci, unit))
	return(FALSE);
    printf("\nWriting AF%d location 0x%-04x with data 0x%-04x.  Go to Reading with BREAK",
	   unit, address, data);
    while (!abort) {
	regptr->argreg = address;
	regptr->cmdreg = MCI_CMD_AF_SELECT;
	regptr->argreg = data << 8;
	regptr->cmdreg = MCI_CMD_AF_WRITE;
    }
    abort = FALSE;
    printf("\nReading AF%d location 0x%-04x.  End test with BREAK", unit, address);
    while (!abort) {
	regptr->argreg = address;
	regptr->cmdreg = MCI_CMD_AF_SELECT;
	regptr->cmdreg = MCI_CMD_AF_READ;
	result = regptr->argreg;
    }
    abort = FALSE;
    return(TRUE);
    
}  /*  end af_readwrite()  */

/*  end mcitest.c  */


