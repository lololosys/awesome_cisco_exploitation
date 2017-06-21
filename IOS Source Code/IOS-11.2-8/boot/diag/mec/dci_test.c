/* $Id: dci_test.c,v 3.2.58.1 1996/05/17 10:23:05 ppearce Exp $
 * $Source: /release/112/cvs/Xboot/diag/mec/dci_test.c,v $
 *------------------------------------------------------------------
 * dci_test.c -- Fast Switching Complex Common Diagnostic Routines
 *
 * 5-September-1990, Bruce Sherry
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dci_test.c,v $
 * Revision 3.2.58.1  1996/05/17  10:23:05  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.46.1  1996/04/03  13:11:22  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  07:42:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:07:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:14:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  19:13:47  hampton
 * Bump version numbers from 1.x to 2.x.
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
#include "../h/dci_diag_defs.h"

/*
 *  Function declarations
 */

extern boolean  store_error(), quit_display(), mci_xnvt(), mec_setup(),
		ds3_setup(), ultra_setup();
extern short    touch_device();
extern void     init_error_struct(), print_errors(), show_err_mask(),
		show_dci_auto_results(), get_number(), type_mema(), type_memd();
extern int      mci_xnvs(), applique_type();
boolean         dci_select(), dci_mema_test(), dci_memd_test(),
		dci_afmem_test(), dci_af_readwrite(), mec_aftest(),
		dci_check_one(), dci_setmask(), dci_ping_test(), dci_setfilter();
mcitype         *cBus_findcontroller();
void            dci_alloc_diag_mem(), dci_execute_2commands(), 
		dci_config(), dci_soft_reset(), dci_revision(),
		mec_search(), select_mec_mema(), write_mec_mema(),
		dci_mema_load(), dci_mema_write(), dci_mema_read(),
		dci_mema_readwrite(), dci_type_mema(), select_mec_memd(),
		write_mec_memd(), dci_memd_load(), dci_memd_write(),
		dci_memd_read(), dci_memd_readwrite(),
		select_mec_af(), write_mec_af(), dci_type_afmem(),
		display_buffers(), 
		cbus_type_mema(), write_cbus_mema(), select_cbus_mema(),
		test_parameters(), dci_setup_unit(),
		fill_xmit_buff();
ushort          dci_command(), read_mec_mema(), read_mec_memd(),
		read_mec_af(), read_cbus_mema(), ex_cbus_command(),
                dci_allocate_buffers();
short           dci_af_sndrcv(), dci_ping();

/*
 *  Global variables
 */

extern          MCI_TEST *diag_results[];
extern          char baddr[];
mcitype         *mcidata[MAXCTRLLERS];
dcitype         *dcidata[FCI_MAXCTRLRS];
boolean         reporting, verbose, looping, ping_loopback, pkt_recvd,
		standard_packet;
int             errors, default_interface, board_to_test, fci_loopback_mode,
		serial_prescale, abort_count, total_wait, total_pkts;
uchar           *packet1, *packet2, *packet3, *custom_packet;
ushort          packet_length, before_reserve, debug_level, longest,
		icb_loc_0, memcheck, mema_start, mema_end,
		pkt_offset, pkt_start, pkt_end, pkt_skip, ping_wait,
		park_buffers;

/*
 *  dci_revision()
 *  Print code revision information.
 */

void dci_revision()
{
    printf("\ndci_test $Revision: 3.2.58.1 $ on $Date: 1996/05/17 10:23:05 $ by $Author: ppearce $.");
}

/*
 *  dci_alloc_diag_mem()
 *  allocate memory for mci and diag data structures
 */

void dci_alloc_diag_mem()
{
    int i;
    uchar *end;
    
    end = (uchar *) DATA_LOCATION;
    for (i = 0; i < MAXCTRLLERS; ++i) {
        mcidata[i] = (mcitype * ) end;
        end = (uchar *) end + sizeof(mcitype);
    }  /*  end for(i)  */
    
    packet1 = (uchar *) end;
    end += MAXPACKET;
    packet2 = (uchar *) end + MAXPACKET;
    end += MAXPACKET;
    packet3 = (uchar *) end + MAXPACKET;
    end += MAXPACKET;
    
    for (i = 0; i < FCI_MAXCTRLRS; ++i){
        diag_results[i] = (MCI_TEST * )(end + sizeof(MCI_TEST));
        end = (uchar *) end + sizeof(MCI_TEST);
        dcidata[i] = (dcitype * ) end;
        end = (uchar *) end + sizeof(dcitype);
    }
    
}  /*  end dci_alloc_diag_mem()  */

/*
 *  test_parameters
 *  Change things like packet offset, starting & ending length, and so on.
 */

void test_parameters()
{
    char                cmdchar;
    char                buff[BUFLEN];
    boolean             parsed;
    int			i, j;
    
    cmdchar = '?';
    printf("\nWarning: No error checking on these values!\n");
    while (TRUE) {
        printf("\nTest Parameters (?CDEHLOQST) [%c]: ", cmdchar);
        parsed = (int) gets(buff);
        if (!null(buff) && parsed)
            cmdchar = cvu(buff[0]);
	switch (cmdchar) {
	  case 'C':
	    cmdchar = 'C';
	    printf("\nC    Use Custom Data Packet");
	    printf("\nD    Display Custom Data Packet");
	    printf("\nE    Edit Packet Data");
	    printf("\nI    Initialize to Standard Packet");
	    printf("\nQ    Quit to Test Parameters menu\n");
	    do{
		printf("\nCustom Data-Packet Menu [%c]: ", cmdchar);
        	parsed = (int) gets(buff);
        	if (!null(buff) && parsed)
            	    cmdchar = cvu(buff[0]);
        	switch (cmdchar) {
	  	  case 'C':
		    custom_packet = packet3;
		    cmdchar = 'Y';
		    printf("\nUse Custom Packet? [%c]: ", cmdchar);
                    standard_packet = (gets(buff) && cvu(buff[0]) == 'Y') ? FALSE : TRUE;
		    if (!standard_packet) {
		    	printf("\nInitialize data to standard packet? [Y]: ", cmdchar);
			if (gets(buff) && (cvu(buff[0]) == 'Y')) {
      	 		    for(i = 0; i < MAXPACKET; i++) {
            			j = i & 0x3f;
            			if (j < 16)
            			    custom_packet[i] = (1 << (j / 2));
            			else if (j < 32)
            			    custom_packet[i] = ((1 << ((j & 0xF) / 2)) ^ 0xFF);
            			else if ((j & 2) == 0)
            			    custom_packet[i] = 0x55;
            			else
            			    custom_packet[i] = 0xAA;
    			    }
			}
		    }
		    cmdchar = 'C';
		    break;
		  case 'D':
            	    j = 0;
            	    custom_packet = packet3;
            	    for (i = 0; i < MAXPACKET; i++)
                	if ((i & 0xf) == 0) {
                    	    if (quit_display(j, NULL))
                        	break;
                    	    printf("\n0x%-03x %-02x", i, custom_packet[i] & 0xFF);
                    	    j++;
                	}    
                	else
                    	    printf(" %-02x", custom_packet[i] & 0xFF);
            	    break;
		  case 'E':
		    printf("\nEdit Custom Packet");
		    printf("\n Starting Location [0]: ");
		    if ((parsed = (int) gets(buff))) {
			if (!null(buff))
			    i = parse_unsigned(buff) - 1;
			else
			    i = -1;
			do{
			    i++;
			    printf("\nPacket[0x%-04x] = 0x%02x : ", i, custom_packet[i]);
			    parsed = (int) gets(buff);
			    if ((!null(buff)) && (buff[0] != '.'))
				custom_packet[i] = parse_unsigned(buff);
			}
			while (buff[0] != '.');
		    }
		    break;
		  case 'I':
      	 	    for(i = 0; i < MAXPACKET; i++) {
            	    	j = i & 0x3f;
            	    	if (j < 16)
            	    	    custom_packet[i] = (1 << (j / 2));
            	    	else if (j < 32)
            	    	    custom_packet[i] = ((1 << ((j & 0xF) / 2)) ^ 0xFF);
            	    	else if ((j & 2) == 0)
            	    	    custom_packet[i] = 0x55;
            	    	else
            	    	    custom_packet[i] = 0xAA;
    		    }
		    break;
		}
	    }
	    while (cmdchar != 'Q');
	    cmdchar = 'C';
	    break;
          case 'D':             /* Select debug level */
            printf("\n     Current debug actions\nLev     Action");
            printf("\n0    Debug off, no action.");
            printf("\n2    Print longest & average wait for a packet in AF_TEST");
            printf("\n          along with number of received packets.");
            printf("\n3    Print last free buffer pointer before send of packet");
            printf("\n          that timed out");
            printf("\n4    Tell if packet was received on incorrect port");
            printf("\n5    Print free buffer pointer before & after send, and after flush");
            printf("\n          along with packet length after receive");
            printf("\n6    Display buffer headers at end of Allocate_buffers");
            printf("\n7-12 Check DMEM location for change at several places in AF_TEST");
            printf("\n13   check interface type in Ping");
            printf("\n14   Note enqueing buffers in Bothways test.");
            printf("\n\nNew debug level [%d] ", debug_level);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    debug_level = parse_unsigned(buff);
                parsed = ((debug_level >= 0) && (debug_level <= 14));
            }
            if (!parsed) {
                printf("\n%% Bad debug_level, assuming 0");
                debug_level = 0;
            }
            break;
          case 'E':
            printf("\n\nNew Ending packet length: [%d] ", pkt_end);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    pkt_end = parse_unsigned(buff);
            }
            break;
          case 'O':
            printf("\n\nNew packet Offset: [%d] ", pkt_offset);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    pkt_offset = parse_unsigned(buff);
            }
            break;
          case 'H':             /* Display this menu */
          case '?':
	    printf("\nC - Custom Packet Data");
            printf("\nD - Debug level");
            printf("\nE - Ping Ending packet length");
            printf("\nL - Ping Loopback Mode");
            printf("\nO - Ping packet Offset");
            printf("\nQ - Quit to Main Menu");
            printf("\nS - Ping Starting packet length");
            printf("\nT - Ping Step length");
            if (debug_level == 7)
                printf("\nW - Change memory check location");
            break;
          case 'L':             /* Ping Loopback Mode */
            printf("\nRun Ping test in Loopback [n]: ");
            ping_loopback = (gets(buff) && cvu(*buff) == 'Y') ? TRUE : FALSE;
            break;
          case 'P':             /* Ping Wait Multiplier */
            printf("\n\nNew Additional Ping Wait: [%d] ", ping_wait);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    ping_wait = parse_unsigned(buff);
            }
            break;
          case 'Q':             /* Quit */
            return;
            break;
          case 'S':
            printf("\n\nNew Starting packet length: [%d] ", pkt_start);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    pkt_start = parse_unsigned(buff);
            }
            break;
          case 'T':
            printf("\n\nNew Ping sTep length: [%d] ", pkt_skip);
            if (parsed = (int) gets(buff)) {
                if (!null(buff))
                    pkt_skip = parse_unsigned(buff);
            }
            break;
          default:
            printf("\nUnknown command \"%c\"", cmdchar);
            break;
            
        }  /*  end switch(cmdchar)  */
    }  /*  end while()  */
}
/*
 *  mec_basic_debug
 *  loop BUS, MEMA, MEMD tests, fill MEMA, MEMD with data patterns,
 *  various R/W loops
 */

void mec_basic_debug(cbus)
    mcitype *cbus;
{
    char        buffer[BUFLEN], cmdchar;
    int         mema_hi, mema_lo, memd_hi, memd_lo, linect;
    ushort      address, unit, data, sresult;
    boolean     parsed;
    
    
    memd_lo = linect = 0;
    mema_lo = mema_start;
    mema_hi = mema_end;
    
    /*  Find out how big MEMD is  */
    
    cbus->regptr->argreg = 1;
    cbus->regptr->cmdreg = MCI_CMD_MEMSIZE;
    memd_hi = cbus->regptr->argreg;
    
    verbose = reporting = FALSE;
    cmdchar = '?';
    while (TRUE) {
        abort = FALSE;
        printf("\nBasic Debug command (?BCDEFGHIJKLMNOQRSTU) [%c]: ", cmdchar);
        parsed = (int) gets(buffer);
        if (!null(buffer) && parsed)
            cmdchar = cvu(buffer[0]);
        switch (cmdchar) {
          case 'B':         /* Loop MEMA test */
            printf("\nRunning MEMA test until BREAK...");
            looping = TRUE;
            while (TRUE) {
                (void) dci_mema_test(cbus, &linect);
                if (abort)
                    break;
            }
            looping = FALSE;
            break;
          case 'C':         /* Loop MEMD test */
            printf("\nRunning MEMD test until BREAK...");
            looping = TRUE;
            while (TRUE) {
                (void) dci_memd_test(cbus, &linect);
                if (abort)
                    break;
            }
            looping = FALSE;
            break;
          case 'D':         /* MEMA read/write loop */
            get_number(&address, "\nEnter memory address: ", 0, mema_end);
            get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
            dci_mema_readwrite(cbus, address, data);
            break;
          case 'E':         /* MEMD read/write loop */
            get_number(&address, "\nEnter memory address: ", 0, memd_hi);
            get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
            dci_memd_readwrite(cbus, address, data);
            break;
          case 'F':         /* Fill MEMA with data pattern */
            get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
            dci_mema_load(cbus, data);
            break;
          case 'G':         /* Fill MEMD with data pattern */
            get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
            dci_memd_load(cbus, data);
            break;
          case 'H':         /* Display this menu */
          case '?':
            printf("\nB - Loop MEMA test");
            printf("\nC - Loop MEMD test");
            printf("\nD - MEMA read/write loop");
            printf("\nE - MEMD read/write loop");
            printf("\nF - Fill MEMA with data pattern");
            printf("\nG - Fill MEMD with data pattern");
            printf("\nI - List MEMA contents");
            printf("\nJ - List MEMD contents (from cbus)");
            printf("\nK - Write MEMA once");
            printf("\nL - Read MEMA once");
            printf("\nM - Write MEMD once");
            printf("\nN - Read MEMD once");
            printf("\nO - Search MEMA and MEMD for data pattern");
            printf("\nP - List errors");
            printf("\nH or ? - Display this menu");
            printf("\nQ - Quit\n");
            printf("\nR - List CBUS MEMA contents");
            printf("\nS - Write CBUS MEMA once");
            printf("\nT - Loop MEMA test till ERROR");
            printf("\nU - Loop MEMD test till ERROR");
            break;
          case 'I':         /* List MEMA contents */
            mci_address(&mema_lo, &mema_hi);
            dci_type_mema(cbus, mema_lo, mema_hi);
            break;
          case 'J':         /* List MEMD contents */
            mci_address(&memd_lo, &memd_hi);
            type_memd(cbus, memd_lo, memd_hi);
            break;
          case 'K':         /* Write MEMA once */
            get_number(&address, "\nEnter memory address: ", 0, mema_end);
            get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
            dci_mema_write(cbus, address, data);
            break;
          case 'L':         /* Read MEMA once */
            get_number(&address, "\nEnter memory address: ", 0, mema_end);
            dci_mema_read(cbus, address);
            break;
          case 'M':         /* Write MEMD once */
            get_number(&address, "\nEnter memory address: ", 0, memd_hi);
            get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
            dci_memd_write(cbus, address, data);
            break;
          case 'N':         /* Read MEMD once */
            get_number(&address, "\nEnter memory address: ", 0, memd_hi);
            dci_memd_read(cbus, address);
            break;
          case 'O':         /* Search MEMA and MEMD for data pattern */
            mec_search(cbus);
            break;
          case 'P':         /* List errors */
            show_all_errors(cbus, FCI_HARDWARE, board_to_test);
            break;
          case 'Q':         /* Quit */
            verbose = reporting = TRUE;
            return;
            break;
          case 'R':
            mci_address(&memd_lo, &memd_hi);
            type_mema(cbus, memd_lo, memd_hi);
            break;
          case 'S':         /* Write CBUS MEMA once */
            get_number(&address, "\nEnter memory address: ", 0, mema_end);
            get_number(&data, "\nEnter data pattern: ", 0, 0xffff);
            select_cbus_mema(cbus, address);
            write_cbus_mema(cbus, data);
            break;
          case 'T':         /* Loop MEMA test till ERROR */
            printf("\nRunning MEMA test until ERROR or BREAK...");
            verbose = reporting = TRUE;
            looping = FALSE;
            while (dci_mema_test(cbus, &linect))
                if (abort)
                    break;
            verbose = reporting = FALSE;
            break;
          case 'U':         /* Loop PING test till ERROR */
            printf("\nRunning PING test until ERROR or BREAK...");
            verbose = reporting = TRUE;
            looping = FALSE;
            while (dci_memd_test(cbus, &linect))
                if (abort)
                    break;
            verbose = reporting = FALSE;
            break;
          default:
            printf("\nUnknown command \"%c\"", cmdchar);
            break;
            
        }  /*  end switch(cmdchar)  */
        
    }  /*  end while(TRUE)  */
    
} /*  end mec_basic_debug()  */

/*
 *  mec_port_debug
 *  mec interface debug routines
 */

void mec_debug_ports(cbus, unit)
    mcitype *cbus;
    int     *unit;
{
    char        buffer[BUFLEN], cmdchar, *packet;
    ushort      address, data, i, app_read_data;
    boolean     parsed;
    int         testbytes = 8;
    int         testlongs = 4;
    int         testdata = 0x1234;
    int         linect = 0, memd_hi, memd_lo;
    int         low = 0, high = 0x1FFF;
    short       app_reg_num = 0, app_reg_data = 0;
    
    verbose = reporting = FALSE;
    cmdchar = '?';
    while (TRUE) {
        abort = FALSE;
        printf("\nPort Debug command (?ABCDEFGHIJKLMNQRT) [%c]: ", cmdchar);
        parsed = (int) gets(buffer);
        if (!null(buffer) && parsed)
            cmdchar = cvu(buffer[0]);
        switch (cmdchar) {
          case 'A':         /* Loop AF MEM test */
            printf("\nRunning AF MEM test until BREAK...");
            looping = TRUE;
            while(TRUE) {
                (void)dci_afmem_test(cbus, &linect, *unit);
                if (abort)
                    break;
            }
            looping = FALSE;
            break;
          case 'B':         /* Loop AF test */
            printf("\nRunning AF test until BREAK...");
            looping = TRUE;
            while (TRUE) {
                (void)mec_aftest(cbus, *unit);
                if (abort)
                    break;
            }
            looping = FALSE;
            break;
          case 'C':         /* Loop PING test */
            printf("\nRunning PING test until BREAK...");
            looping = TRUE;
            while (TRUE) {
                (void)dci_ping_test(cbus, *unit);
                if (abort)
                    break;
            }
            looping = FALSE;
            break;
          case 'D':         /* Select Default Interface */
            *unit = (*unit + 1);
            if (*unit > 23)
                *unit = 0;
            printf("\nSelecting interface %d", *unit);
            (void)dci_select(cbus, board_to_test, *unit);
            break;
          case 'E':
            show_all_errors(cbus, FCI_HARDWARE, board_to_test);
            break;
          case 'F':         /* AF MEM write/read loop */
            get_number(&address, "\nEnter memory address: ", 0, AF_WORDS);
            get_number(&data, "\nEnter data byte: ", 0, 255);
            (void)dci_af_readwrite(cbus, *unit, address, data);
            break;
          case 'G':         /* Display AF memory */
            mci_address(&low, &high);
            dci_type_afmem(cbus, low, high);
            break;
          case 'H':         /* Display this menu */
          case '?':
            printf("\n\nA - Loop AF MEM test");
            printf("\nB - Loop AF test");
            printf("\nC - Loop PING test");
            printf("\nD - Select Default Interface");
            printf("\nE - List all errors");
            printf("\nF - AF MEM write/read loop");
            printf("\nG - Display AF memory");
            printf("\nH or ? - Display this menu");
            printf("\nI - Display free buffer headers");
            printf("\nJ - List MEMD contents (from cbus)");
            printf("\nK - Special Transmit Loop no checking");
            printf("\nL - Loop Ping test till ERROR");
            printf("\nM - Write Applique register");
            printf("\nN - Read Applique register");
            printf("\nQ - Quit\n");
            printf("\nR - Display Receive buffer");
            printf("\nT - Display Transmit buffer");
            break;
          case 'I':         /* Display Buffer headers */
            display_buffers(cbus, POOL0);
            break;
          case 'J':         /* List MEMD contents */
            mci_address(&memd_lo, &memd_hi);
            type_memd(cbus, memd_lo, memd_hi);
            break;
          case 'K':
            xmit_loop(cbus, *unit);
            break;
          case 'L':         /* Loop PING test till ERROR */
            printf("\nRunning PING test until ERROR or BREAK...");
            verbose = reporting = TRUE;
            looping = FALSE;
            while (dci_ping_test(cbus, *unit))
                if (abort)
                    break;
            verbose = reporting = FALSE;
            break;
          case 'M':         /* Write Applique Register */
            printf("\nApplique Register number [%d] ", app_reg_num);
            if (parsed = (int) gets(buffer)) {
                if (!null(buffer))
                    app_reg_num = parse_unsigned(buffer);
                parsed = ((app_reg_num >= 0) && (app_reg_num <= 15));
            }
            if (!parsed) {
                printf("\n%% Bad register number, assuming 0");
                app_reg_num = 0;
            }
            dci_command(cbus, FCI_CMD_APPLIQUE, app_reg_num);
            printf("\nData to write [%d] ", app_reg_data);
            if (parsed = (int) gets(buffer)) {
                if (!null(buffer))
                    app_reg_data = parse_unsigned(buffer);
            }
            dci_command(cbus, FCI_CMD_APP_WRITE, app_reg_data);
            break;
          case 'N':         /* Read Applique Register */
            printf("\nApplique Register number [%d] ", app_reg_num);
            if (parsed = (int) gets(buffer)) {
                if (!null(buffer))
                    app_reg_num = parse_unsigned(buffer);
                parsed = ((app_reg_num >= 0) && (app_reg_num <= 15));
            }
            if (!parsed) {
                printf("\n%% Bad register number, assuming 0");
                app_reg_num = 0;
            }
            dci_command(cbus, FCI_CMD_APPLIQUE, app_reg_num);
            app_read_data = dci_command(cbus, FCI_CMD_APP_READ, 0);
            printf("\nApplique port 0x%x = 0x%-04x", app_reg_num, app_read_data);
            break;
          case 'Q':         /* Quit */
            verbose = reporting = TRUE;
            return;
            break;
          case 'R':         /* Display Receive buffer */
            if (pkt_recvd) {
                linect = 0;
                packet = (char*) packet2;
                for (i = 0; i < packet_length; i++)
                    if ((i & 0xf) == 0) {
                        if (quit_display(linect, NULL))
                            break;
                        printf("\n0x%-03x %-02x", i, packet[i + (2 * pkt_offset)] & 0xFF);
                        linect++;
                    }    
                    else
                        printf(" %-02x", packet[i + (2 * pkt_offset)] & 0xFF);
            }
            else
                printf("\nNO Packet was received!");
            break;
          case 'T':         /* Display Transmit buffer */
            linect = 0;
            packet = (char*) packet1;
            for (i = 0; i < packet_length; i++)
                if ((i & 0xf) == 0) {
                    if (quit_display(linect, NULL))
                        break;
                    printf("\n0x%-03x %-02x", i, packet[i + (2 * pkt_offset)] & 0xFF);
                    linect++;
                }    
                else
                    printf(" %-02x", packet[i + (2 * pkt_offset)] & 0xFF);
            break;
          default:
            printf("\nUnknown command \"%c\"", cmdchar);
            break;
            
            
        }  /*  end switch(cmdchar)  */
        
    }  /*  end while(TRUE)  */
    
}  /*  end mci_debug_ports()  */

/*
 * cBus_findcontroller
 * Find all MCI controllers in system
 */

mcitype *cBus_findcontroller()
{
    int         i;
    short       *cBusbase, temp;
    ushort      result;
    mcitype     *cBus;
    
    for (i = 0; i < MAXCTRLLERS; ++i)
        bzero((uchar * )mcidata[i], sizeof(mcitype));
    
    for (i = 0; i < MAXCTRLLERS; i++) {
        cBus = mcidata[i];
        cBusbase = (short * ) MCISTART + MCIBYTES * i;
        (void)touch_device(cBusbase);
        if (buserrcnt == 0) {
            cBus->regptr = (mciregtype * ) cBusbase;
            cBus->unit = i;
            cBus->regptr->argreg = MCI_HW_VERSION;
            cBus->regptr->cmdreg = MCI_CMD_VERSION;
            result = cBus->regptr->argreg;
	    temp = (result & 0xff00) >> 8;
            if ((temp == FCI2_HARDWARE) || (temp == FCI_HARDWARE)) {
		printf("\nCbus hardware: %-3d.%3d", temp, result & 0xff);
        	cBus->regptr->argreg = MCI_SW_VERSION;
	        cBus->regptr->cmdreg = MCI_CMD_VERSION;
        	result = cBus->regptr->argreg;
		temp = (result & 0xff00) >> 8;
		printf("  software: %-3d.%3d", temp, result & 0xff);
                cBus->regptr->argreg = FCI_HARD_RESET;
                cBus->regptr->cmdreg = MCI_CMD_RESET;
                delay(HARD_RESET_TIME);
                cBus->regptr->argreg = FCI_SOFT_RESET;
                cBus->regptr->cmdreg = MCI_CMD_RESET;
                delay(RESET_TIME);
                return (cBus);
            }
        } else
            cBus->unit = -1;
        cBus->regptr = (mciregtype * ) cBusbase;
    }
    return(cBus);
    
}  /*  end cBus_findcontroller()  */

/*
 * dci_setup_unit
 * Determine which unit, set up data structures
 */

void dci_setup_unit (cbus)
    register mcitype *cbus;
{
    register uchar *cbusbase;
    char buff[BUFLEN];
    boolean parsed;
    
    if (board_to_test < 0) {
        printf("\nselect board to test [%d]: ", board_to_test);
        if (parsed = (int) gets(buff)) {
            if (!null(buff))
                board_to_test = parse_unsigned(buff);
            parsed = ((board_to_test >= 0) &&
                      (board_to_test < FCI_MAXCTRLRS));
        }
        if (!parsed) {
            printf("\n%% bad board number - assuming 2");
            board_to_test = 2;
        }
    }
    cbus->regptr->argreg = board_to_test;
    cbus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    
    /*
     * read the current setting of the fci loopback state.
     */
    cbus->regptr->argreg = 0440;
    cbus->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    cbus->regptr->cmdreg = MCI_CMD_MEMA_READ;
    if (cbus->regptr->argreg != 0)
        fci_loopback_mode = TRUE;
    else
        fci_loopback_mode = FALSE;
    
}   /* end dci_setup_unit */

/*
 * Report configuration of this Fast Switching Complex.
 */

void dci_config(cBus, type_to_test)
    mcitype *cBus;
    ushort  type_to_test;
{
    register int        i;
    mciregtype          *regptr;
    ushort              indx, jndx, kndx, sresult, typecode, capability,
    dcicount = 0, temp;
    
    /*
     * Determine the number of controllers (interfaces) supported by cBus.
     */
    board_to_test = -1;      /* no boards yet */
    regptr = cBus->regptr;
    
    for (indx = 0; indx < FCI_MAX_DCI_CTRLRS; indx++) {
        regptr->argreg = indx;
        regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
        if ((sresult = regptr->cmdreg) != FCI_RSP_OKAY) {
            cBus->dci[indx] = NULL;
            continue;
        }
        
        /*
         * The "who" command returns the type of controller present.
         */
        regptr->cmdreg = FCI_CTRLR_WHO;
        sresult = regptr->argreg;
        if ((sresult & 0xFF) == 0xFF) {
            dcidata[indx]->ctrlr_unit = -1;
            continue;
        }
        sresult = ((sresult >> 3) & 0x7);
        if (sresult == type_to_test)
            dcidata[indx]->ctrlr_unit = indx;
        else
            dcidata[indx]->ctrlr_unit = -1;
        
        /*
         * Print a header if necessary.
         */
        if (dcicount++ < 1)
            printf ("\n\nDCI  Type   SW      HW    0 1 2 3 4 5");
        
        /*
         * Daughter controller exists -- create memory structure.
         */
        cBus->dci[indx] = dcidata[indx];
        dcidata[indx]->ctrlr_type = sresult;
        dcidata[indx]->regptr = regptr;
        dcidata[indx]->fci = cBus;
        
        printf ("\n %d   ", indx);
        switch (sresult) {
          case FCI_ETHER_CONTROLLER:
            printf ("MEC ");
            if ((board_to_test < 0) && (type_to_test == FCI_ETHER_CONTROLLER))
                board_to_test = indx;
            break;
          case FCI_FDDI_CONTROLLER:
            printf ("FDDI");
            if ((board_to_test < 0) && (type_to_test == FCI_FDDI_CONTROLLER))
                board_to_test = indx;
            break;
          case FCI_SERIAL_CONTROLLER:
            printf ("SER ");
            if ((board_to_test < 0) && (type_to_test == FCI_SERIAL_CONTROLLER))
                board_to_test = indx;
            break;
          case FCI_DS3_CONTROLLER:
            printf ("HSCI"); /* Change to new official name 11-28-90 */
            if ((board_to_test < 0) && (type_to_test == FCI_DS3_CONTROLLER))
                board_to_test = indx;
            break;
          default:
            printf ("0x%1x ", sresult);
        }
        
        /*
         * Get the hardware/software version of the controller.
         */
        dcidata[indx]->sw_version = dci_command
            (cBus, FCI_CMD_VERSION, FCI_SW_VERSION);
        dcidata[indx]->hw_version = dci_command
            (cBus, FCI_CMD_VERSION, FCI_HW_VERSION);
        
        temp = (dcidata[indx]->sw_version & 0xff00) >> 8;
        printf(" %-3d.%3d", temp, (dcidata[indx]->sw_version & 0xff));
        temp = (dcidata[indx]->hw_version & 0xff00) >> 8;
        printf(" %-3d.%3d", temp, (dcidata[indx]->hw_version & 0xff));
        
        kndx = (indx * FCI_MAX_CTRLR_INTERFACES);
        for (jndx = kndx; jndx < (kndx + FCI_MAX_CTRLR_INTERFACES);
             jndx++) {
            regptr->argreg = jndx;
            regptr->cmdreg = FCI_CMD_SELECT;
            if ((sresult = regptr->cmdreg) != FCI_RSP_OKAY) {
                cBus->interfacetype[jndx] = -1;
                continue;
            }
            sresult = cBus->interfacetype[jndx] = regptr->argreg;
            switch (sresult) {
              case FCI_TYPE_ETHER:
                printf (" E");
                break;
              case FCI_TYPE_SERIAL:
                printf (" S");
                break;
              case FCI_TYPE_FDDI:
                printf (" F");
                break;
              case FCI_TYPE_ULTRA:
                printf (" U");
                break;
              case FCI_TYPE_DS3:
                printf (" D");
                break;
              default:
                printf (" ?");
            }
            cBus->interfacetype[jndx] = sresult;
        }
    }
    
}  /*  end dci_config()  */

/*
 * dci_execute_2commands
 * given a command code, execute 2 cmds and return result
 * This code is borrowed from execute_command.
 */

void dci_execute_2commands(cbus, arg1, arg2)
    register mcitype *cbus;
    int *arg1,     *arg2;
{
    char buff[BUFLEN];
    ushort sresult, response;
    boolean parsed;
    
    printf("\nCommand[%d]: ", *arg2 );
    if (parsed = (int) gets(buff)) {
        if (!null(buff))
            *arg2    = parse_unsigned(buff);
        parsed = (*arg2    >= 0);
    }
    if (!parsed) {
        printf("\n%% bad interface cmd");
        return;
    }
    
    
    printf("\nArgument [%d]: ", *arg1);
    if (parsed = (int) gets(buff)) {
        if (!null(buff))
            *arg1     = parse_unsigned(buff);
        parsed = (*arg1     >= 0);
    }
    if (!parsed) {
        printf("\n%% bad interface arg");
        return;
    }
    
    sresult = 0;
    response = 0;
    
    cbus->regptr->argreg = *arg1;
    cbus->regptr->cmdreg = FCI_CTRLR_ARGUMENT;  /*      */
    
    response = cbus->regptr->cmdreg;
    sresult = cbus->regptr->argreg;
    
    if (response != MCI_RSP_OKAY)   {
        printf("\nbad response: 0x%-04x", response);
        return;
    }
    /*   printf("\nvalue returned: 0x%-04x or 0%-06o", sresult, sresult); */
    
    cbus->regptr->argreg = *arg2;
    cbus->regptr->cmdreg = FCI_CTRLR_CMD;          /*   */
    response = cbus->regptr->cmdreg;
    sresult = cbus->regptr->argreg;
    
    if (response != MCI_RSP_OKAY)   {
        printf("\nbad response: 0x%-04x", response);
        return;
    }
    else {
        printf("\nresponse ret:   0x%-04x", response);
        printf("\nvalue returned: 0x%-04x or 0%-06o", sresult, sresult);
    }
    
}

ushort dci_command (cbus, cmd, arg)
    register mcitype *cbus;
    ushort cmd, arg;
{
    ushort sresult, response;
    
    cbus->regptr->argreg = arg;
    cbus->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    response = cbus->regptr->cmdreg;
    sresult = cbus->regptr->argreg;
    
    if (response != MCI_RSP_OKAY)   {
        printf("\ndci_command1 error: Argreg: 0x%-04x   Cmdreg: 0%-04x",sresult, response);
        printf("\n command was: 0x%-04x  Argument was: 0%-04x", cmd, arg);
        return (-1);
    }
    
    cbus->regptr->argreg = cmd;
    cbus->regptr->cmdreg = FCI_CTRLR_CMD;
    response = cbus->regptr->cmdreg;
    sresult = cbus->regptr->argreg;
    
    if (response != MCI_RSP_OKAY)   {
        printf("\ndci_command2 error: Argreg: 0x%-04x   Cmdreg: 0%-04x",sresult, response);
        printf("\n command was: 0x%-04x  Argument was: 0%-04x", cmd, arg);
        return (-1);
    }
    return (sresult);
}

ushort ex_cbus_command (cbus, cmd, arg)
    register mcitype *cbus;
    ushort cmd, arg;
{
    ushort sresult, response;
    
    cbus->regptr->argreg = arg;
    cbus->regptr->cmdreg = cmd;
    response = cbus->regptr->cmdreg;
    sresult = cbus->regptr->argreg;
    
    if (response != MCI_RSP_OKAY)   {
        printf("\nex_cbus_command error: Argreg: 0x%-04x   Cmdreg: 0%-04x",sresult, response);
        printf("\n command was: 0x%-04x  Argument was: 0%-04x", cmd, arg);
        return (-1);
    }
    return (sresult);
}

/*
 * cbus_reset
 * reset controller and wait for mci to get its act together.
 */

void cbus_reset(cbus)
    mcitype *cbus;
{
    int i;
    
    cbus->regptr->argreg = HARD_RESET;
    cbus->regptr->cmdreg = MCI_CMD_RESET;
    delay(RESET_TIME);
}

/*
 * dci_soft_reset
 * reset controller and wait for mci to get its act together.
 */

void dci_soft_reset(cbus)
    mcitype *cbus;
{
    cbus->regptr->argreg = FCI_SOFT_RESET;
    cbus->regptr->cmdreg = MCI_CMD_RESET;
    delay(RESET_TIME);
}

/*
 * mec_search
 * Search mema and memd for a pattern
 */

void mec_search(cbus)
    mcitype *cbus;
{
    char        buff[BUFLEN];
    mciregtype  *regptr;
    ushort      pattern, result;
    int         linect, i, size;
    
    pattern = 0;
    printf("\nPattern: ");
    if (gets(buff))
        pattern = (ushort) parse_unsigned(buff);
    else
        return;
    linect = 0;
    regptr = cbus->regptr;
    printf("\nSearching for 0x%-04x...", pattern);
    select_mec_mema(cbus,0);
    for (i = 0; i < mema_end; i++) {
        result = read_mec_mema(cbus);
        if (result == pattern) {
            if (quit_display(linect++, NULL))
                return;
            printf("\nMEMA 0x%-04x: 0x%-04x", i, result);
        }
    }
    
    /*  Find out how big MEMD is  */
    
    cbus->regptr->argreg = 1;
    cbus->regptr->cmdreg = MCI_CMD_MEMSIZE;
    size = cbus->regptr->argreg;
    
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
    
}  /*  end mec_search()  */

/*
 * dci_select
 * Select the specified interface
 */

boolean dci_select(cbus, board, unit)
    mcitype *cbus;
    int board, unit;
{
    ushort sresult;
    
    cbus->regptr->argreg = (ushort) board;
    cbus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    sresult = cbus->regptr->cmdreg;
    if (sresult < 0x8000) {
        cbus->regptr->argreg = (ushort) (board * FCI_MAX_CTRLR_INTERFACES) + unit;
        cbus->regptr->cmdreg = MCI_CMD_SELECT;
        sresult = cbus->regptr->cmdreg;
        if (sresult != MCI_RSP_OKAY) {
            errors++;
            if (reporting & (sresult != MCI_RSP_ERR_NPRS))
                printf("\nMEC: unable to select interface %d - response is 0x%x",
                       unit, sresult);
            return(FALSE);
        }
        return(TRUE);
    }
    else {
        if (reporting & (sresult == 0x8010))
            printf("\nMEC: unable to select board %d - timeout!", board);
        else
            printf("\nMEC: unable to select board %d - response is 0x%x",
                   board, sresult);
        return(FALSE);
    }
}  /*  end dci_select()  */

/*
 * cbus_type_mema
 * Type out a range of locations in mema
 */

void cbus_type_mema(cbus, low, high)
    register mcitype *cbus;
    int low, high;
{
    register int        i;
    ushort              sresult;
    int                 linect;
    
    linect = 0;
    select_cbus_mema(cbus,low);
    for (i = low; i < high + 1; i++) {
        sresult = read_cbus_mema(cbus);
        if ((i & 0x7) == (low & 0x7)){
            if (quit_display(linect, NULL))
                return;
            printf("\nMEMA 0x%-04x: 0x%-04x", i, sresult);
            linect++;
        }
        else
            printf(" %-04x", sresult);
    }
    
}  /*  end cbus_type_mema()  */

/*
 * select_cbus_mema
 */

void select_cbus_mema (cbus, address)
    register mcitype *cbus;
    ushort address;
{
    ex_cbus_command (cbus, 20, address);
}
/*
 * write_cbus_mema
 */

void write_cbus_mema (cbus, value)
    register mcitype *cbus;
    ushort value;
{
    ex_cbus_command (cbus, 19, value);
}
/*
 * read_cbus_mema
 */

ushort read_cbus_mema (cbus)
    register mcitype *cbus;
{
    return (ex_cbus_command (cbus, 18, 0));  /* 0 because we don't care about arg */
}



/*
 * select_mec_mema
 */

void select_mec_mema (cbus, address)
    register mcitype *cbus;
    ushort address;
{
    dci_command (cbus, 17, address);
}
/*
 * write_mec_mema
 */

void write_mec_mema (cbus, value)
    register mcitype *cbus;
    ushort value;
{
    dci_command (cbus, 18, value);
}
/*
 * read_mec_mema
 */

ushort read_mec_mema (cbus)
    register mcitype *cbus;
{
    return (dci_command (cbus, 19, 0));  /* 0 because we don't care about arg */
}


/*
 * dci_mema_test
 * Run tests of mema
 * Returns FALSE if user quit out of error display
 */

boolean dci_mema_test(cbus, linect)
    mcitype *cbus;
    int *linect;
{
    register ushort     i;
    ushort              sresult;
    
    /*
     * mema -- write zeros
     */
    
    errors = 0;
    if (verbose)
        printf("\nDCI: writing MEMA with zeros...");
    select_mec_mema(cbus,mema_start);
    for (i = mema_start; i < mema_end && !abort; i++) {
        write_mec_mema(cbus,0);
    }
    select_mec_mema(cbus,mema_start);
    for (i = mema_start; i < mema_end && !abort; i++) {
        sresult = read_mec_mema(cbus);
        if (sresult != 0 && !looping) {
            errors++;
            if (!store_error(&diag_results[board_to_test]->mema_test, 0, sresult, i, 0 ,0 ,0 ,0))
                return(FALSE);                    /*  all bits are bad  */
            if (reporting) {
                if (quit_display((*linect)++, NULL))
                    return(FALSE);
                printf("\nDCI: wrote MEMA 0x%-04x with zeros, read 0x%-04x",
                       i, sresult);
            }
        }
    }
    
    /*
     * mema -- write ones
     */
    
    if (verbose)
        printf("\nDCI: writing MEMA with all ones...");
    select_mec_mema(cbus,mema_start);
    for (i = mema_start; i < mema_end && !abort; i++) {
        write_mec_mema(cbus,0xFFFF);
    }
    select_mec_mema(cbus,mema_start);
    for (i = mema_start; i < mema_end && !abort; i++) {
        sresult = read_mec_mema(cbus);
        if (sresult != 0xFFFF && !looping) {
            errors++;
            if (!store_error(&diag_results[board_to_test]->mema_test, 0xFFFF, sresult, i, 0, 0, 0, 0))
                return(FALSE);                      /*  all bits are bad  */
            if (reporting) {
                if (quit_display((*linect)++, NULL))
                    return(FALSE);
                printf("\nDCI: wrote MEMA 0x%-04x with ones, read 0x%-04x",
                       i, sresult);
            }
        }
    }
    
    /*
     * mema -- write address + 1
     */
    
    if (verbose)
        printf("\nDCI: writing MEMA with address+1...");
    select_mec_mema(cbus,mema_start);
    for (i = mema_start; i < mema_end && !abort; i++) {
        write_mec_mema(cbus,i + 1);
    }
    select_mec_mema(cbus,mema_start);
    for (i = mema_start; i < mema_end && !abort; i++) {
        sresult = read_mec_mema(cbus);
        if (sresult != (i + 1) && !looping) {
            errors++;
            if (!store_error(&diag_results[board_to_test]->mema_test, i + 1, sresult, i, 0, 0, 0, 0))
                return(FALSE);                    /*  all bits are bad  */
            if (reporting) {
                if (quit_display((*linect)++, NULL))
                    return(FALSE);
                printf("\nDCI: wrote MEMA loc %#x with 0x%-04x, read 0x%-04x",
                       i, i + 1, sresult);
            }
        }
    }
    if (errors == 0 && !looping && !abort)
        diag_results[board_to_test]->mema_test.stat = STAT_PASS;
    if (errors || abort)
        return(FALSE);
    else
        return(TRUE);
    
}  /*  end dci_mema_test()  */

/*
 *  dci_mema_load
 *  loads mema with data pattern
 */

void dci_mema_load(cbus, data)
    mcitype *cbus;
    short data;
{
    int i;
    
    select_mec_mema(cbus,mema_start);
    for (i = mema_start; i < mema_end; i++) {
        write_mec_mema(cbus,data);
    }
    
}  /*  end dci_mema_load()  */


/*
 *  dci_mema_write
 *  write data to mema address
 */

void dci_mema_write(cbus, address, data)
    mcitype *cbus;
    ushort address, data;
{
    printf("\nWriting 0x%x to MEMA location 0x%x once...", data, address);
    select_mec_mema(cbus,address);
    write_mec_mema(cbus,data);
    
}  /*  end dci_mema_write()  */


/*
 *  dci_mema_read
 *  read from mema address
 */

void dci_mema_read(cbus, address)
    mcitype *cbus;
    ushort address;
{
    select_mec_mema(cbus,address);
    printf("\nRead 0x%x from MEMA location 0x%x.", read_mec_mema(cbus), address);
    
}  /*  end dci_mema_read()  */

/*
 * dci_mema_readwrite
 * Special read/write testing for scoping mema failures.
 */

void dci_mema_readwrite(cbus, loc, data)
    mcitype *cbus;
    ushort loc;
    ushort data;
{
    mciregtype  *regptr;
    ushort      result;
    
    regptr = cbus->regptr;
    result = 0;
    abort = FALSE;
    printf("\nWriting MEMA location 0x%-03x with data 0x%-04x. Go to Reading with BREAK",
           loc, data);
    while (!abort) {
        select_mec_mema(cbus,loc);
        write_mec_mema(cbus,data);
    }
    abort = FALSE;
    printf("\nReading MEMA location 0x%-03x. End test with BREAK", loc);
    while (!abort) {
        select_mec_mema(cbus,loc);
        result = read_mec_mema(cbus);
    }
    printf("\nRead a 0x%-03x from MEMA location 0x%-04x", result, loc);
    abort = FALSE;
    
}  /*  end dci_mema_readwrite()  */

/*
 * dci_type_mema
 * Type out a range of locations in mema
 */

void dci_type_mema(cbus, low, high)
    register mcitype *cbus;
    int low, high;
{
    register int        i;
    ushort              sresult;
    int                 linect;
    
    linect = 0;
    select_mec_mema(cbus,low);
    for (i = low; i < high + 1; i++) {
        sresult = read_mec_mema(cbus);
        if ((i & 0x7) == (low & 0x7)){
            if (quit_display(linect, NULL))
                return;
            printf("\nMEMA 0x%-04x: 0x%-04x", i, sresult);
            linect++;
        }
        else
            printf(" %-04x", sresult);
    }
    
}  /*  end dci_type_mema()  */
/*
 * select_mec_memd
 */

void select_mec_memd (cbus, address)
    register mcitype *cbus;
    ushort address;
{
    dci_command (cbus, 8, address);
}
/*
 * write_mec_memd
 */

void write_mec_memd (cbus, value)
    register mcitype *cbus;
    ushort value;
{
    dci_command (cbus, 9, value);
}
/*
 * read_mec_memd
 */

ushort read_mec_memd (cbus)
    register mcitype *cbus;
{
    return (dci_command (cbus, 10, 0));  /* 0 because we don't care about arg */
}

/*
 * dci_memd_test
 * Run tests of memd
 * Returns FALSE if user quit out of error display
 */

boolean dci_memd_test(cbus, linect)
    mcitype *cbus;
    int *linect;
{
    register uint       i;
    uint                sresult, size;
    
    /* Find out how big memd is */
    
    cbus->regptr->argreg = 1;
    cbus->regptr->cmdreg = MCI_CMD_MEMSIZE;
    size = cbus->regptr->argreg;
    
    /*
     * memd - all zeros
     */
    
    errors = 0;
    if (verbose)
        printf("\nDCI: writing MEMD with all zeros...");
    
    for (i = MEMD_BASE; i < size && !abort; i += MEMD_STEP){
        select_mec_memd(cbus,i);
        write_mec_memd(cbus,0);
    }
    for (i = MEMD_BASE; i < size && !abort; i += MEMD_STEP) {
        select_mec_memd(cbus,i);
        sresult = read_mec_memd(cbus);
        if (sresult != 0 && !looping) {
            errors++;
            if (!store_error(&diag_results[board_to_test]->memd_test, 0, sresult, i, 0, 0, 0, 0))
                return(FALSE);                    /*  all bits are bad  */
            if (reporting) {
                if (quit_display((*linect)++, NULL))
                    return(FALSE);
                printf("\nDCI: wrote MEMD loc %#x with zeros, read 0x%-04x",
                       i, sresult);
            }
        }
    }
    
    /*
     * memd - all ones
     */
    
    if (verbose)
        printf("\nDCI: writing MEMD with all ones...");
    for (i = MEMD_BASE; i < size && !abort; i += MEMD_STEP){
        select_mec_memd(cbus,i);
        write_mec_memd(cbus,0xFFFF);
    }
    for (i = MEMD_BASE; i < size && !abort; i += MEMD_STEP) {
        select_mec_memd(cbus,i);
        sresult = read_mec_memd(cbus);
        if (sresult != 0xFFFF && !looping) {
            errors++;
            if (!store_error(&diag_results[board_to_test]->memd_test, 0xFFFF, sresult, i, 0, 0, 0, 0))
                return(FALSE);                    /*  all bits are bad  */
            if (reporting) {
                if (quit_display((*linect)++, NULL))
                    return(FALSE);
                printf("\nDCI: wrote MEMD loc %#x with ones, read 0x%-04x",
                       i, sresult);
            }
        }
    }
    
    /*
     * memd - address+1
     */
    
    if (verbose)
        printf("\nDCI: writing MEMD with address+1...");
    for (i = MEMD_BASE; i < size && !abort; i += MEMD_STEP){
        select_mec_memd(cbus,i);
        write_mec_memd(cbus,i + 1);
    }
    for (i = MEMD_BASE; i < size && !abort; i += MEMD_STEP) {
        select_mec_memd(cbus,i);
        sresult = read_mec_memd(cbus);
        if (sresult != (i + 1) && !looping) {
            errors++;
            if (!store_error(&diag_results[board_to_test]->memd_test, (i + 1), sresult, i, 0, 0, 0, 0))
                return(FALSE);                    /*  all bits are bad  */
            if (reporting) {
                if (quit_display((*linect)++, NULL))
                    return(FALSE);
                printf("\nDCI: wrote MEMD loc %#x with 0x%-04x, read 0x%-04x",
                       i, (i + 1), sresult);
            }
        }
    }
    
    if (errors == 0 && !looping && !abort)
        diag_results[board_to_test]->memd_test.stat = STAT_PASS;
    if (errors || abort)
        return(FALSE);
    else
        return(TRUE);
    
}  /*  end dci_memd_test()  */

/* dci_memd_load
 * Load up all of memd with data
 */

void dci_memd_load(cbus, data)
    mcitype *cbus;
    ushort data;
{
    mciregtype  *regptr;
    ushort      pointer;
    int         i, size;
    
    /*  Find out how big MEMD is  */
    
    cbus->regptr->argreg = 1;
    cbus->regptr->cmdreg = MCI_CMD_MEMSIZE;
    size = cbus->regptr->argreg;
    
    for (i = 0; i <= size; i++)
        select_mec_memd(cbus,i);
    write_mec_memd(cbus,data);
    
}  /*  end dci_memd_load()  */

/*
 * dci_memd_write
 * Write once and return
 */

void dci_memd_write(cbus, loc, data)
    mcitype *cbus;
    ushort loc;
    ushort data;
{
    printf("\nWriting 0x%x to MEMD location 0x%x once...", data, loc);
    select_mec_memd(cbus,loc);
    write_mec_memd(cbus,data);
    
}  /*  end dci_memd_write()  */


/*
 * dci_memd_read
 * Read once from selected location
 */

void dci_memd_read(cbus, loc)
    mcitype *cbus;
    ushort loc;
    
{
    ushort      result;
    
    select_mec_memd(cbus,loc);
    result = read_mec_memd(cbus);
    
    printf("\nRead a 0x%-04x from MEMD location 0x%-04x", result, loc);
    
}  /*  end dci_memd_read()  */

/*
 * dci_memd_readwrite
 * Special read/write testing for scoping memd failures.
 */

void dci_memd_readwrite(cbus, loc, data)
    mcitype *cbus;
    ushort loc;
    ushort data;
{
    mciregtype  *regptr;
    ushort      result = 0;
    
    regptr = cbus->regptr;
    abort = FALSE;
    printf("\nWriting MEMD location 0x%-04x with data 0x%-04x. Go to Reading with BREAK", loc, data);
    while (!abort) {
        select_mec_memd(cbus,loc);
        write_mec_memd(cbus,data);
    }
    abort = FALSE;
    printf("\nReading MEMD location 0x%-04x.  End test with BREAK", loc);
    while (!abort) {
        select_mec_memd(cbus,loc);
        result = read_mec_memd(cbus);
    }
    abort = FALSE;
    printf("\nRead a 0x%-04x from MEMD location 0x%-04x", result, loc);
    
}  /*  end dci_memd_readwrite()  */

/*
 * select_mec_af
 */

void select_mec_af (cbus, address)
    register mcitype *cbus;
    ushort address;
{
    dci_command (cbus, 34, address);
}
/*
 * write_mec_af
 */

void write_mec_af (cbus, value)
    register mcitype *cbus;
    ushort value;
{
    dci_command (cbus, 36, value);
}
/*
 * read_mec_af
 */

ushort read_mec_af (cbus)
    register mcitype *cbus;
{
    return (dci_command (cbus, 35, 0));  /* 0 because we don't care about arg */
}

/*
 * dci_afmem_test
 * Run memory test over Ethernet address filter memories
 */

boolean dci_afmem_test(cbus, linect, unit)
    mcitype *cbus;
    int *linect;
    int unit;
{
    register ushort     i;
    ushort              sresult, data;
    int                 af_words;
    
    /*
     * Select the interface
     */
    
    if (!dci_select(cbus, board_to_test, unit)) {
        if (!looping)
            (void)store_error(&diag_results[board_to_test]->add_flt_mem[unit], 0, 0, 0, 0, 0, 0, 0);
        return(FALSE);
    }
    
    /*
     * Ethernet address memory -- write zeros
     */
    
    errors = 0;
    if (verbose)
        printf("\nDCI writing AF MEM%d memory with zeros...", unit);
    af_words = dci_command (cbus, 50, 0);
    select_mec_af(cbus,0);
    for (i = 0; i < af_words && !abort; i += AF_STEP) {
        write_mec_af(cbus,0);
    }
    select_mec_af(cbus,0);
    for (i = 0; i < af_words && !abort; i += AF_STEP) {
        sresult = read_mec_af(cbus) & 0xFF00;
        if (sresult != 0 && !looping) {
            errors++;
            if(!store_error(&diag_results[board_to_test]->add_flt_mem[unit], 0, sresult, i, 0, 0, 0, 0))
                return(FALSE);
            if (reporting) {
                if (quit_display((*linect)++, NULL))
                    return(FALSE);
                printf("\nDCI wrote AF MEM%d loc %#x with zeros, read 0x%-04x",
                       unit, i, sresult);
            }
        }
    }
    
    /*
     * Ethernet address memory -- write ones
     */
    
    if (verbose)
        printf("\nDCI writing AF MEM%d memory with ones...", unit);
    select_mec_af(cbus,0);
    for (i = 0; i < af_words && !abort; i += AF_STEP) {
        write_mec_af(cbus,0xFFFF);
    }
    select_mec_af(cbus,0);
    for (i = 0; i < af_words && !abort; i += AF_STEP) {
        sresult = read_mec_af(cbus) & 0xFF00;
        if (sresult != 0xFF00 && !looping) {
            errors++;
            if(!store_error(&diag_results[board_to_test]->add_flt_mem[unit], 0xFF00, sresult, i, 0, 0, 0, 0))
                return(FALSE);
            if (reporting) {
                if (quit_display((*linect)++, NULL))
                    return(FALSE);
                printf("\nDCI wrote AF MEM%d loc %#x with ones, read 0x%-04x",
                       unit, i, sresult);
            }
        }
    }
    
    /*
     * Ethernet address memory -- write address + 1
     */
    
    if (verbose)
        printf("\nDCI writing AF MEM%d memory with address+1...", unit);
    select_mec_af(cbus,0);
    for (i = 0; i < af_words && !abort; i += AF_STEP) {
        write_mec_af(cbus,((i + 1) << 8));
    }
    select_mec_af(cbus,0);
    for (i = 0; i < af_words && !abort; i += AF_STEP) {
        sresult = read_mec_af(cbus) & 0xFF00;
        data = ((i + 1) << 8);
        if (sresult != data && !looping) {
            errors++;
            if(!store_error(&diag_results[board_to_test]->add_flt_mem[unit], ((i + 1) << 8), sresult, i, 0, 0, 0, 0))
                return(FALSE);
            if (reporting) {
                if (quit_display((*linect)++, NULL))
                    return(FALSE);
                printf("\nDCI wrote AF MEM%d loc %#x with 0x%-04x, read 0x%-04x",
                       unit, i, data, sresult);
            }
        }
    }
    if (errors == 0 && !looping && !abort)
        diag_results[board_to_test]->add_flt_mem[unit].stat = STAT_PASS;
    if (errors || abort)
        return(FALSE);
    else
        return(TRUE);
    
}  /*  end dci_afmem_test()  */

/*
 * dci_addressfilter_readwrite
 * Special read/write testing for scoping af mem failures.
 */

boolean dci_af_readwrite(cbus, unit, address, data)
    mcitype *cbus;
    int unit;
    ushort address;
    ushort data;
{
    mciregtype  *regptr;
    ushort      result = 0;
    
    abort = FALSE;
    regptr = cbus->regptr;
    
    /*
     * Select the interface after resetting world
     */
    
    dci_soft_reset(cbus);
    if (!dci_select(cbus, board_to_test, unit))
        return(FALSE);
    printf("\nWriting AF%d location 0x%-04x with data 0x%-04x.  Go to Reading with BREAK",
           unit, address, data);
    while (!abort) {
        select_mec_af(cbus,address);
        write_mec_af(cbus,data << 8);
    }
    abort = FALSE;
    printf("\nReading AF%d location 0x%-04x.  End test with BREAK", unit, address);
    while (!abort) {
        select_mec_af(cbus,address);
        result = read_mec_af(cbus) & 0xFF00;
    }
    printf("\nRead a 0x%-03x from AF location 0x%-04x", result, address);
    abort = FALSE;
    return(TRUE);
    
}  /*  end dci_af_readwrite()  */

/*
 * dci_type_afmem
 * type out a range of locations in Address Filter Memory
 */

void dci_type_afmem (cbus, low, high)
    register mcitype *cbus;
    int low, high;
{
    register int i;
    ushort result;
    int linect;
    
    linect = 0;
    select_mec_af(cbus,low);
    for (i = low; i < high+1; i++) {
        result = (read_mec_af(cbus) >> 8) & 0xFF;
        if ((i & 0xf) == (low & 0xf)){
            if (quit_display(linect, NULL))
                return;
            printf("\nAFmem 0x%-04x: 0x%-02x", i, result);
            linect++;
        }
        else
            printf(" %-02x", result);
        
    }
}

/*
 * dci_allocate_buffers
 * allocate some buffers for us to play with.
 */

ushort dci_allocate_buffers(cbus, board, unit, overhead, size, count, offset, transmit, receive)
    mcitype *cbus;
    int board, unit, overhead, size, count, offset, transmit, receive;
{
    mciregtype *regptr;
    ushort sresult;
    int i, cnt = 0;
    
    dci_select(cbus, board, unit);
    regptr = cbus->regptr;
    if (verbose) {
        printf("\nbuffers for %d: %d words (%d overhead)", board * 6 + unit, (size + 1) / 2, overhead);
        printf("%d tx, %d rx (%d offset)", transmit, receive, offset);
    }
    
    regptr->argreg = 0;                 /* No overhead */
    regptr->cmdreg = FCI_CMD_OVERHEAD;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_BUFFERSIZE;
    regptr->argreg = POOL7;
    regptr->cmdreg = FCI_CMD_TX_ALLOCATE;
    /*
     * select pool index 0
     */
    regptr->argreg = POOL0;
    regptr->cmdreg = MCI_CMD_BUFFERSIZE;
    regptr->argreg = FCI_ASSIGN_POOLINDEX;
    regptr->cmdreg = MCI_CMD_RX_ALLOCATE;
    
    /* 
     * set maximum byte count for this interface.
     */
    regptr->argreg = size; 
    regptr->cmdreg = MCI_CMD_BUFFERSIZE;
    regptr->argreg = FCI_MAX_BUFFERSIZE;
    regptr->cmdreg = MCI_CMD_RX_ALLOCATE;
    
    /*
     * set relative receive offset.
     */
    regptr->argreg = offset;
    regptr->cmdreg = MCI_CMD_BUFFERSIZE;
    regptr->argreg = FCI_RX_BUFFER_OFFSET;
    regptr->cmdreg = MCI_CMD_RX_ALLOCATE;
    
    /*
     * create buffer queue for pool 0
     */
    regptr->argreg = overhead;
    regptr->cmdreg = MCI_CMD_OVERHEAD;
    /* size here is in words + 24 bytes slop */
    regptr->argreg = (size + 25) / 2;
    regptr->cmdreg = MCI_CMD_BUFFERSIZE;
    for (i = 0; i < count; i++, cnt++) {
        regptr->argreg = POOL0;
        regptr->cmdreg = MCI_CMD_TX_ALLOCATE;
        sresult = regptr->cmdreg;
        if ((sresult != MCI_RSP_OKAY) && (sresult != 0x8009)) {
            if (reporting)
                printf("\ncbus: tx_allocate failed - %#x", sresult);
            return;
        }
    }
    /*
     * Display buffer headers
     */
    
    if (debug_level == 6)
        display_buffers(cbus, POOL0);
    
    /*
     * set up receive queue limits
     */
    regptr->argreg = receive;
    regptr->cmdreg = MCI_CMD_BUFFERSIZE;
    regptr->argreg = FCI_RX_BUFFER_LIMIT;
    regptr->cmdreg = MCI_CMD_RX_ALLOCATE;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
        if (reporting)
            printf("\ncbus: rx_allocate failed - %#x", sresult);
        return;
    }
    
    /*
     * set up transmit queue limits
     */
    regptr->argreg = transmit;
    regptr->cmdreg = MCI_CMD_QUEUE;
    regptr->argreg = transmit;
    regptr->cmdreg = MCI_CMD_SLIMIT;
    regptr->argreg = POOL7;
    regptr->cmdreg = MCI_CMD_BUFFERSIZE;
    regptr->argreg = FCI_RX_ERROR;
    regptr->cmdreg = FCI_CMD_EXECUTE;
    return (cnt);
}

/*
 * display_buffers
 * Follow the linked list of buffers in the selected pool, and display.
 */

void display_buffers(cbus, pool)
    mcitype *cbus;
    ushort pool;
{
    ushort link, owner, txstart, bytes, status, qptr,
    rxtype, rxprotocol, rxptrhack, unused, overhead, current, data, buf_cnt = 0;
    
    current = pool + 0xEC;
    printf("\nFree buffer queue is 0xEC");
    get_number(&current, "\nDCI Buffers. Que head: ", 0, 0xffff);
    if (current == 0xEC) {
	cbus->regptr->argreg = current;
	cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
	current = cbus->regptr->readshort;
    }
    printf("\n addr  link   ICB  txsta bytes  stat  qptr  rxty rxpro rxptr unused data");
    while (current != 0) {
	buf_cnt++;
        cbus->regptr->argreg = current;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        link = cbus->regptr->readshort;
        owner = cbus->regptr->readshort;
        txstart = cbus->regptr->readshort;
        bytes = cbus->regptr->readshort;
        status = cbus->regptr->readshort;
        qptr = cbus->regptr->readshort;
        rxtype = cbus->regptr->readshort;
        rxprotocol = cbus->regptr->readshort;
        rxptrhack = cbus->regptr->readshort;
        unused = cbus->regptr->readshort;
        data = cbus->regptr->readshort;
        printf("\n0x%-04x  %-04x  %-4x  %-04x  %-4d  %-04x  %-04x  %-04x  %-04x  %-04x  %-04x",
               current, link, owner, txstart, bytes, status, qptr, rxtype, rxprotocol, rxptrhack, unused, data);
        if (current == link) {
            printf("\nError: Buffer pointing to itself!");
            return;
        }
        current = link;
    }
    printf("\nTotal Buffers = %d", buf_cnt);
}       /* end display_buffers */

/*
 * mec_aftest
 * Exhaustively test the address filter mechanism.
 * This could take a while.....
 */


boolean mec_aftest(cbus, unit)
    mcitype *cbus;
    int unit;
{
    ushort      fsm, octet;
    int         linect, af_words;
    
    cbus_reset(cbus);
    dci_soft_reset(cbus);                             /* reset the world */
    errors = total_wait = total_pkts = 0;
    longest = PKTWAIT;
    
    if (!dci_select(cbus, board_to_test, unit)) {               /* select interface */
        if (!looping)
            (void)store_error(&diag_results[board_to_test]->add_flt[unit], 0, 0, 0,
                              0, 0, 0, 0);
        return(FALSE);
    }
    
    /*
     * No overhead, 100 data bytes, offset of 1, one transmit, four receive
     */
    
    dci_allocate_buffers(cbus, board_to_test, unit, 0, 100, 10, 1, 1, 4);
    
    /*
     * Set interface status to loopback mode
     */
    
    cbus->regptr->argreg = MCI_IF_DISABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    cbus->regptr->argreg = MCI_IF_LOOPBACK;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    cbus->regptr->argreg = MCI_IF_ENABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    if (cbus->regptr->cmdreg != MCI_RSP_OKAY) {
	errors++;
	if (reporting)
	    printf("\nUltra: RX enable failed");
	return(FALSE);
    }
    af_words = dci_command (cbus, 50, 0);
    if (verbose)
        printf("\nDCI:   Address filter size : 0x%x", af_words);
    linect = 0;
    
    if (debug_level == 7) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        icb_loc_0 = cbus->regptr->readshort;
    }
    
    /*
     * Detailed check of octet 0 of FSM 0
     */
    
    if (verbose) {
        printf("\nDCI: AF%d - detailed test of octet 0, fsm 0", unit);
        linect++;
    }
    if (dci_check_one(cbus, unit, 0, 0, &linect, 1, MECBYTES, af_words) == FALSE) {
        if (debug_level == 2)
            printf("\n longest wait = %d, avg = %d, for %d packets.",
                   (PKTWAIT - longest), (total_wait / total_pkts), total_pkts);
        return(FALSE);
    }
    
    /*
     * Very quick test of other octets and FSM's
     */
    
    for (octet = 0; octet < MECBYTES && !abort; octet++) {
        for (fsm = 0; fsm < 8 && !abort; fsm++) {
            if (verbose) {
                printf("\nDCI: AF%d - quick test of octet %d, fsm %d",
                       unit, octet, fsm);
                linect++;
            }
            if (dci_check_one(cbus, unit, fsm, octet, &linect, 75, MECBYTES, af_words) == FALSE) {
                if (debug_level == 2)
                    printf("\n longest wait = %d, avg = %d, for %d packets.",
                           (PKTWAIT - longest), (total_wait / total_pkts), total_pkts);
                return(FALSE);
            }
        }
    }
    if (errors == 0 && !looping && !abort)
        diag_results[board_to_test]->add_flt[unit].stat = STAT_PASS;
    if (debug_level == 2)
        printf("\n longest wait = %d, avg = %d, for %d packets.",
               (PKTWAIT - longest), (total_wait / total_pkts), total_pkts);
    if (errors || abort)
        return(FALSE);
    else
        return(TRUE);
    
}  /*  end mec_aftest()  */

/*
 * dci_check_one
 * Given a FSM and octet number, do an exhaustive check
 */

boolean dci_check_one(cbus, unit, fsm, octet, linect, skipcount, testbytes, af_words)
    mcitype *cbus;
    int unit, fsm, octet, *linect, skipcount, testbytes, af_words;
    
{
    register int        i;
    register ushort     sresult;
    ushort              inhibit, test;
    boolean             temp;
    register uchar      *packet, *rpacket;
    
    packet = packet1;
    rpacket = packet2;
    packet_length = PKTLENGTH;
    for (i = 0; i < packet_length; i++)
        packet[i] = 0;
    
    for (inhibit = 0; inhibit < 256 && !abort; inhibit += skipcount) {
        cbus->regptr->argreg = (board_to_test * FCI_MAX_CTRLR_INTERFACES) + unit;
        cbus->regptr->cmdreg = MCI_CMD_SELECT;
        if ((sresult = cbus->regptr->cmdreg) != MCI_RSP_OKAY) {
            if (reporting)
                printf("\ndci_check_one: select failed - %#x", sresult);
            return(FALSE);
        }
        if (!dci_setmask(cbus, fsm, octet, inhibit, testbytes, af_words)) {
            if (!looping)
                (void)store_error(&diag_results[board_to_test]->add_flt[unit], 0, 0, 0,
                                  fsm, octet, 0, 0);
            return(FALSE);
        }
        for (test = 0; test < 256; test += skipcount) {
            packet[octet] = test;
            temp = (dci_af_sndrcv(cbus, unit, packet, rpacket, PKTLENGTH));
            switch (temp) {
              case TRUE:
                /*
                 * Received packet.  Okay only if not target data.
                 */
                
                if (test != inhibit) {
                    
                    /* start at 2 and go 2 byte long for receive offset */
                    for (i = 0; i < testbytes; i++) {
                        if (packet[i] != rpacket[i + 2] && !looping) {
                            errors++;
                            (void)store_error(&diag_results[board_to_test]->add_flt[unit], packet[i],
                                              rpacket[i + 2], 0, fsm, octet, test, inhibit);
                            if (!reporting)
                                break;
                            if (quit_display((*linect)++, NULL))
                                return(FALSE);
                            printf("\nDCI: AF%d - data miscompare on rcvd packet", unit);
                            printf(": octet %d, FSM %d, sent %#x, inhibited %#x",
                                   octet, fsm, test, inhibit);
                            if (quit_display((*linect)++, NULL))
                                return(FALSE);
                            if (!more())
                                return(FALSE);
                        }
                    }
                }
                else 
                    if (!looping) {
                        errors++;
                        (void)store_error(&diag_results[board_to_test]->add_flt[unit], packet[i],
                                          rpacket[i], 0, fsm, octet, test, inhibit);
                        if (!reporting)
                            break;
                        if (quit_display((*linect)++, NULL))
                            return(FALSE);
                        printf("\nDCI: AF%d - rcvd unexpected packet", unit);
                        printf(": octet %d, FSM %d, sent %#x, inhibited %#x",
                               octet, fsm, test, inhibit);
                        if (quit_display((*linect)++, NULL))
                            return(FALSE);
                        if (!more())
                            return(FALSE);
                    }
                break;
                
              case FALSE:
                
                /*
                 * Did not receive packet.  Okay only if target data.
                 */
                
                if (test != inhibit && !looping) {
                    errors++;
                    (void)store_error(&diag_results[board_to_test]->add_flt[unit], 0, 0, 0,
                                      fsm, octet, test, inhibit);
                    if (!reporting)
                        break;
                    if (quit_display((*linect)++, NULL))
                        return(FALSE);
                    printf("\nDCI: AF%d - did not rcv expected packet", unit);
                    printf(": octet %d, FSM %d, sent %#x, inhibited %#x",
                           octet, fsm, test, inhibit);
                    if (quit_display((*linect)++, NULL))
                        return(FALSE);
                    if (!more())
                        return(FALSE);
                }
                break;
                
              default:
                
                /*
                 * Some other error happened.  Quit to preserve DCIstate.
                 */
                
                errors++;
                if (!looping)
                    (void)store_error(&diag_results[board_to_test]->add_flt[unit], 0, 0, 0,
                                      fsm, octet, 0, 0);
                return(FALSE);
                break;
            }
        }
    }
    return(TRUE);
    
}  /*  end dci_check_one()  */

/*
 * dci_setmask
 * Set up an address mask for a particular test
 */

boolean dci_setmask(cbus, fsm, octet, data, testbytes, af_words)
    mcitype *cbus;
    ushort fsm, octet, data;
    int testbytes, af_words;
    
{
    register ushort i, accept, address;
    ushort low, high, sresult;
    
    /*
     * Compute acceptance mask and disable receiver.
     */
    
    accept = ((1 << fsm) << 8);
    cbus->regptr->argreg = MCI_IF_DISABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    
    /*
     * Refuse everything
     */
    
    cbus->regptr->argreg = 0;
    cbus->regptr->cmdreg = MCI_CMD_AF_SELECT;
    for (i = 0; i < af_words; i++) {
        cbus->regptr->argreg = 0;
        cbus->regptr->cmdreg = MCI_CMD_AF_WRITE;
        if ((sresult = cbus->regptr->cmdreg) != MCI_RSP_OKAY) {
            if (reporting)
                printf("\ndci_setmask: 0 fill failed - %#x", sresult);
            return(FALSE);
        }
    }
    
    /*
     * Accept an all zeros address, except for that one octet.
     */
    
    for (i = 0; i < testbytes; i++) {
        if (i != octet) {
            cbus->regptr->argreg = (i << 8);
            cbus->regptr->cmdreg = MCI_CMD_AF_SELECT;
            cbus->regptr->argreg = accept;
            cbus->regptr->cmdreg = MCI_CMD_AF_WRITE;
            if ((sresult = cbus->regptr->cmdreg) != MCI_RSP_OKAY) {
                if (reporting)
                    printf("\ndci_setmask: accept 0 failed - %#x", sresult);
                return(FALSE);
            }
        }
    }
    
    /*
     * Receive all values of the selected octet
     */
    
    address = (octet << 8);
    cbus->regptr->argreg = address;
    cbus->regptr->cmdreg = MCI_CMD_AF_SELECT;
    for (i = address; i < address + 256; i++) {
        cbus->regptr->argreg = accept;
        cbus->regptr->cmdreg = MCI_CMD_AF_WRITE;
    }
    
    /*
     * Except that one particular address
     */
    
    address = (octet << 8) | (data & 0xFF);
    cbus->regptr->argreg = address;
    cbus->regptr->cmdreg = MCI_CMD_AF_SELECT;
    cbus->regptr->argreg = 0;
    cbus->regptr->cmdreg = MCI_CMD_AF_WRITE;
    
    /*
     * Enable receiver
     */
    
    cbus->regptr->argreg = MCI_IF_ENABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    if (cbus->regptr->cmdreg != MCI_RSP_OKAY) {
        errors++;
        if (reporting)
            printf("\nDCI RX enabled failed");
        return(FALSE);
    }
    return(TRUE);
    
}  /*  end dci_setmask()  */

/*
 * dci_af_sndrcv
 * Send a packet and wait for something to come back or not
 * Returns TRUE if a packet was received, FALSE otherwise.
 * Returns -1 if some other error.
 */

short dci_af_sndrcv(cbus, unit, packet, rpacket, len)
    mcitype *cbus;
    int unit, len;
    ulong *packet;
    ulong *rpacket;
{
    ushort      sresult = 0, rxlength, freebuffer = 0, new_loc_0;
    int         waiting, i;
    
    cbus->regptr->argreg = (board_to_test * FCI_MAX_CTRLR_INTERFACES) + unit;
    cbus->regptr->cmdreg = MCI_CMD_SELECT;
    
    if (debug_level == 7) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d before TX0_Reserve",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    /*
     * Set pointer to start of buffer (TX0_SELECT).
     */
    
    cbus->regptr->argreg = POOL0;
    cbus->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
    if ((sresult = cbus->regptr->cmdreg) != MCI_RSP_OKAY) {
        if (reporting)
            printf("\ndci: TX0_RESERVE failed - %#x", sresult);
        return(-1);
    }
    
    cbus->regptr->argreg = 0;
    cbus->regptr->cmdreg = MCI_CMD_TX0_SELECT;
    sresult = cbus->regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
        errors++;
        if (reporting)
            printf("\nDCI: TX0_SELECT failed, result = 0x%x", sresult);
        return(-1);
    }
    
    /*
     * Put header bytes into transmit buffer.
     * Then write byte length and start transmission.
     */
    
    for (i = 0; i < MECBYTES; i += 4)
        cbus->regptr->write0long = *packet++;
    
    if (debug_level == 8) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d after packet write",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    if (debug_level == 3) {
        cbus->regptr->argreg = 0xEC;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        freebuffer = cbus->regptr->readshort;
    }
    
    cbus->regptr->argreg = (board_to_test * FCI_MAX_CTRLR_INTERFACES) + unit;
    cbus->regptr->cmdreg = MCI_CMD_SELECT;
    cbus->regptr->argreg = len | MCI_TX_HEARSELF;
    cbus->regptr->cmdreg = MCI_CMD_TX0_START;
    if (cpu_type == CPU_CSC3) /* Adjust for processor speed */
        waiting = PKTWAIT;
    else
        waiting = 1000;
    
    if (debug_level == 9) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d after TX0_Start",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    do {
        cbus->regptr->cmdreg = MCI_CMD_RX_SETUP;
        sresult = cbus->regptr->argreg;
        if (sresult != 0xFFFF) {
            if (sresult == ((board_to_test * FCI_MAX_CTRLR_INTERFACES) + unit))
                break;
            else
                if (debug_level == 4)
                    printf("\nDCI: af_sndrcv: Received packet in error: 0x%x",
                           sresult);
        }
    }
    while (--waiting);
    if (waiting <= 0) {
        if (debug_level == 3)
            printf("\nDCI Debug: Free Buffer Pointer before send = 0x%-04x", freebuffer);
        return(FALSE);
    }
    if (debug_level == 10) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d after RX_Setup",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    total_wait += PKTWAIT - waiting;
    total_pkts++;
    if (waiting < longest) {
        longest = waiting;
    }
    cbus->regptr->argreg = 0;
    cbus->regptr->cmdreg = MCI_CMD_RX_SELECT;
    
    /*
     * Check buffer pointer after SETUP
     */
    
    if (debug_level == 5) {
        cbus->regptr->argreg = 0412;
        cbus->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
        cbus->regptr->cmdreg = MCI_CMD_MEMA_READ;
        sresult = cbus->regptr->argreg;
        printf("\nDCI Debug: Buffer Pointer before read = 0x%-04x", sresult);
    }
    
    /* start at 2 and go 2 byte long for receive offset */
    for (i = 0; i < (MECBYTES + 2); i += 4)
        *rpacket++ = cbus->regptr->readlong;
    
    if (debug_level == 11) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d after RX_data_transfer",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    rxlength = cbus->regptr->rxlength;
    cbus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
    
    if (debug_level == 12) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d after RX0_Flush",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    /*
     * display packet length
     */
    
    if (rxlength != PKTLENGTH) {
        printf("\nDCI: Packet length SB %d is %d", PKTLENGTH, rxlength);
        return(FALSE);
    }
    else
        if (debug_level == 5)
            printf("\n DCI debug: packet length = %d", rxlength);
    
    /*
     * Check buffer pointer after FLUSH
     */
    
    if (debug_level == 5) {
        cbus->regptr->argreg = 0412;
        cbus->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
        cbus->regptr->cmdreg = MCI_CMD_MEMA_READ;
        sresult = cbus->regptr->argreg;
        printf("\nDCI Debug: Buffer Pointer after FLUSH = 0x%-04x", sresult);
    }
    return(TRUE);
    
}  /*  end dci_af_sndrcv()  */

/*
 * dci_ping_test
 * This could take a while.....
 */

boolean dci_ping_test(cbus, unit)
    mcitype *cbus;
    int     unit;
{
    ushort              rlength;
    int                 linect, temp1, af_words, if_type;
    register int        i;
    register ushort     data, j;
    boolean             temp;
    register uchar      *tpacket, *rpacket;
    MCI_ERROR           *test = NULL;
    
    tpacket = packet1;
    rpacket = packet2;
    abort_count = 0;
    
    if_type = cbus->interfacetype[unit + (board_to_test * FCI_MAX_CTRLR_INTERFACES)];
    if (debug_level == 13)
        printf("\ninterface type = %d", if_type);
    switch (if_type) {
      case FCI_TYPE_ETHER:
        test = &diag_results[board_to_test]->ether_ping_test[unit];
        break;
      case FCI_TYPE_SERIAL:
      case FCI_TYPE_ULTRA:
      case FCI_TYPE_DS3:
        test = &diag_results[board_to_test]->serial_ping_test[unit];
        break;
    }
    
    cbus_reset(cbus);
    dci_soft_reset(cbus);                             /* reset the world */
    
    if (!dci_select(cbus, board_to_test, unit))  {              /* select interface */
        if (!looping)
            (void)store_error(test, 0, 0, 0, 1, 0, 0, 0);
        if (reporting) {
            printf("\nPING%d ERROR: Unable to select interface.", unit);
            ++linect;
        }
        return(FALSE);
    }
    
    /*
     * No overhead, pkt_end data bytes, one transmit, four receive
     */ 
    
    dci_allocate_buffers(cbus, board_to_test, unit, 0, pkt_end, 7, pkt_offset, 1, 4);
    
    af_words = dci_command(cbus, 50, 0);
    linect = 0;
    if (verbose && (af_words > 0)) {
        printf("\nDCI:   Address filter size : 0x%x", af_words);
        linect++;
    }
    
    errors = 0;
    
    linect = 0;
   
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
    
    switch (if_type) {
      case FCI_TYPE_ETHER:
        if (debug_level == 13)
            printf("\ninterface type = FCI_TYPE_ETHER");
	if (!mec_setup(cbus, unit, tpacket, MECBYTES, af_words, test, &linect))
	    return(FALSE);
        break;
        
      case FCI_TYPE_SERIAL:
      case FCI_TYPE_ULTRA:
        if (debug_level == 13)
            printf("\ninterface type = ultra");
	if (!ultra_setup(cbus, unit, &errors)) {
            if (!looping)
        	(void)store_error(test, 0, 0, 0, 1, 0, 0, 13);
	    return(FALSE);
	}
        break;
      case FCI_TYPE_DS3:
        if (debug_level == 13)
            printf("\ninterface type = DS3");
	if (!ds3_setup(cbus, unit, &errors)) {
            if (!looping)
        	(void)store_error(test, 0, 0, 0, 1, 0, 0, 13);
	    return(FALSE);
	}
        break;
        
      default:
        if (!looping)
            (void)store_error(test, 0, 0, 0, 1, 0, 0, 5);
        if (reporting)
            printf("\nPING%d ERROR: Bad interface type.", unit);
        return (FALSE);
        break;
        
    } /* end switch (if_type) */
    
    for(i = pkt_start; i <= pkt_end && !abort; i += pkt_skip) {
        if(verbose && (i == pkt_start))
            printf("\nDCI: PING%d - ", unit);
        
        temp = (dci_ping(cbus, unit, tpacket, rpacket, i, pkt_offset));
        switch (temp) {
          case TRUE:
            rlength = cbus->regptr->rxlength;
            if (rlength != i) {
                if (!looping) {
                    errors++;
                    (void)store_error(test, 0, 0, 0, i, (int)rlength, 0, 6);
                    if (reporting) {
                        printf("\nPING%d ERROR - packet length sent : %d, received %d",
                               unit, i, rlength);
                        ++linect;
                    }
                }
                return(FALSE);
            }
	    if (verbose) /* got a packet of the correct length, print ! */
                if ((pkt_end - pkt_start) / 0x20 <= 45) {
        	    if (((i - pkt_start) & 0x1f) == 0)
            	        printf("!");
		}
                else /* pkt_skip > 1 */ {
            	    if (((i - pkt_start) & 0xff) == 0)
                        printf("!");
		}
            /* start at 2 and go 2 byte long for receive offset */
            for (data = 0; data < i; data++) {
                if (tpacket[data] != rpacket[data + (2 * pkt_offset)]) {
                    if (!looping) {
                        errors++;
                        (void)store_error(test, (ushort)tpacket[data], (ushort)rpacket[data], (ushort)data, i, (int)rlength, 0, 7);
                        if (!reporting)
                            break;
                        else {
                            printf("\nPING%d ERROR: Data mismatch on packet length %d", unit, i);
                            ++linect;
                            printf("\n     byte %d should be 0x%x, was 0x%x",
                                   data, tpacket[data], rpacket[data + (2 * pkt_offset)]);
                            ++linect;
                        }
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
                return(FALSE);
            else {
                printf("\nPING%d ERROR: - did not rcv expected packet", unit);
                ++linect;
                printf(": sent packet length %d", i);
                ++linect;
            }
            return(FALSE);
            
          default:
            
            /*
             * Some other error happened.  Quit to preserve DCIstate.
             */
            
            errors++;
            if (!looping)
                (void)store_error(test, 0, 0, 0, 0, 0, 0, 0);
            return(FALSE);
        }
    }
    if (errors == 0 && !looping && !abort)
        test->stat = STAT_PASS;
    if (errors || abort)
        return(FALSE);
    else
        return(TRUE);
    
}  /*  end dci_ping_test()  */

/*
 * dci_setfilter
 * Set up an address mask for a particular test
 */

boolean dci_setfilter(cbus, packet, testbytes, af_words)
    register mcitype *cbus;
    uchar *packet;
    int testbytes, af_words;
{
    register ushort i, accept, address;
    
    /*
     * Compute acceptance mask and disable receiver.
     */
    
    accept = (1 << 8);
    cbus->regptr->argreg = MCI_IF_DISABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    
    /*
     * Refuse everything
     */
    
    cbus->regptr->argreg = 0;
    cbus->regptr->cmdreg = MCI_CMD_AF_SELECT;
    for (i = 0; i < af_words; i++) {
        cbus->regptr->argreg = 0;
        cbus->regptr->cmdreg = MCI_CMD_AF_WRITE;
    }
    
    /*
     * Accept packet address.
     */
    
    for (i = 0; i < testbytes; i++) {
        cbus->regptr->argreg = (i << 8) + packet[i];
        cbus->regptr->cmdreg = MCI_CMD_AF_SELECT;
        cbus->regptr->argreg = accept;
        cbus->regptr->cmdreg = MCI_CMD_AF_WRITE;
    }
    
    /*
     * Enable receiver
     */
    
    cbus->regptr->argreg = MCI_IF_ENABLE;
    cbus->regptr->cmdreg = MCI_CMD_STATUS;
    if (cbus->regptr->cmdreg != MCI_RSP_OKAY) {
        errors++;
        if (reporting)
            printf("\nDCI RX enable failed");
        return(FALSE);
    }
    return(TRUE);
    
}  /*  end dci_setfilter()  */

/*
 * dci_ping
 * Send a packet and wait for something to come back or not
 * Returns TRUE if a packet was received, FALSE otherwise.
 * Returns -1 if some other error.
 */

short dci_ping(cbus, unit, packet, rpacket, len, offset)
    mcitype *cbus;
    int     unit, len;
    ulong   *packet;
    ulong   *rpacket;
    ushort  offset;
{
    ushort      sresult = 0, rstatus, rlength, freebuffer = 0, new_loc_0;
    int         waiting, i;
    
    if (debug_level == 7) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d before TX0_Reserve",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
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
    
    if (debug_level == 8) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d after packet write",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    if (debug_level == 3) {
        cbus->regptr->argreg = 0xEC;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        freebuffer = cbus->regptr->readshort;
    }
    
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
    
    waiting = ping_wait + len + 50;
    if (debug_level == 9) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d after TX0_Start",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    while (waiting--) {
        cbus->regptr->cmdreg = MCI_CMD_RX_SETUP;
        sresult = cbus->regptr->argreg;
        if (sresult != 0xFFFF) {
            if (sresult == ((board_to_test * FCI_MAX_CTRLR_INTERFACES) + unit))
                break;
            else {
                if (debug_level == 4)
                    printf("\nDCI: dci_ping: Received packet in error: 0x%x",
                           sresult);
		cbus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    }
        }
    }
    if (waiting <= 0) {
        if (debug_level == 3)
            printf("\nDCI Debug: Free Buffer Pointer before send = 0x%-04x", freebuffer);
        pkt_recvd = FALSE;
        return(FALSE);
    }
    
    if (debug_level == 10) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d after RX_Setup",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    /*
     * Read packet if it comes back.
     */
    
    pkt_recvd = TRUE;
    cbus->regptr->argreg = 0;
    cbus->regptr->cmdreg = MCI_CMD_RX_SELECT;
    
    /*
     * Check buffer pointer after SETUP
     */
    
    if (debug_level == 5) {
        cbus->regptr->argreg = 0412;
        cbus->regptr->cmdreg = MCI_CMD_MEMA_SELECT;
        cbus->regptr->cmdreg = MCI_CMD_MEMA_READ;
        sresult = cbus->regptr->argreg;
        printf("\nDCI Debug: Buffer Pointer before read = 0x%-04x", sresult);
    }
    
    /* start 
       /* start at 2 and go 2 byte long for receive offset */
        for(i = 0; i <= (len + (2 * offset)); i += 4)
            *rpacket++ = cbus->regptr->readlong;
    
    if (debug_level == 11) {
        cbus->regptr->argreg = memcheck;
        cbus->regptr->cmdreg = MCI_CMD_MEMD_SELECT;
        new_loc_0 = cbus->regptr->readshort;
        if (new_loc_0 != icb_loc_0) {
            printf("\nDMEM location 0x%x  was %d, changed to %d after RX_data_transfer",
                   memcheck, icb_loc_0, new_loc_0);
            icb_loc_0 = new_loc_0;
            cbus->regptr->argreg = FCI_CCBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf("\n CCBPTR = 0x%x,", sresult);
            cbus->regptr->argreg = FCI_ICBPTR;
            cbus->regptr->cmdreg = FCI_CMD_READ_REGISTER;
            sresult = cbus->regptr->argreg;
            printf(" ICBPTR = 0x%x,", sresult);
        }
    }
    
    cbus->regptr->cmdreg = MCI_CMD_RX_FLUSH;
    return((sresult == ((board_to_test * FCI_MAX_CTRLR_INTERFACES) + unit)) ? TRUE : FALSE);
    
}  /*  end dci_ping()  */

