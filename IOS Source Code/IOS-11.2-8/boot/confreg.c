/* $Id: confreg.c,v 3.2.58.2 1996/05/10 23:16:45 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/confreg.c,v $
 *------------------------------------------------------------------
 * confreg.c -- configuration register output routine
 *
 * January 1987, Greg Satz
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: confreg.c,v $
 * Revision 3.2.58.2  1996/05/10  23:16:45  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.2.58.1  1996/05/09  13:59:35  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.2.88.1  1996/04/27  06:12:48  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.78.1  1996/04/08  01:34:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  09:06:42  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.50.1  1996/03/03  21:18:46  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/02  01:06:23  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.32.1  1996/02/29  07:41:44  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  07:40:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:26  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/10/03  07:04:26  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  19:11:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "reason.h"
#include "extern.h"
#include "cpu.h"
#include "stdio.h"
#include "ctype.h"
#include "prom2.h"

#if defined(PAN) || defined(XX) || defined(SYNALC)
extern void reset_confreg();
#endif /* PAN, XX or SYNALC */

#ifdef PAN
#include "../src-68-c3000/nonvol_x2444.h"
#endif

#ifdef SYNALC
#include "../src-68-alc/nonvol_x2444.h"
#endif

#ifdef RP1
#include "../src-68-c7000/nvmonvars.h"
#endif


/*
 * options
 * Output options in reasonable format
 */

void print_option(name, flag, enabled_string, disabled_string, trailer_string)
    char *name;			/* Option name string */
    u_int flag;			/* True if option is enabled */
    char *enabled_string;	/* Optional string to print if flag is true */
    char *disabled_string;	/* Optional string to print if flag is false */
    char *trailer_string;	/* Optional string to print at end */
{
    puts(name);
    if (flag) {
	if (enabled_string)
	    puts(enabled_string);
    } else {
	if (disabled_string)
	    puts(disabled_string);
    }
    if (trailer_string)
	puts(trailer_string);
}


void options (args)
    char *args;			/* Pointer to arguments */
{
    char *enabled = "enabled\n";
    char *disabled = "disabled\n";
    short bootfile;
#if defined(PAN) & defined(PAN_ORIGINAL)
    u_char phys_confreg;
#endif /* PAN and PAN_ORIGINAL */
#if defined(RP1)
    u_short tmp_confreg;
#endif
    long temp;
    char c;

#if defined (PAN) || defined(XX) || defined(RP1) || defined(SYNALC)

    /* Look to see if any arguments are present on the command line */

    if (*args) {
	if (*args++ == '/') {
	    while (isspace(*args))
	      args++;
	    if (*args == 'R') {
#if defined (PAN) || defined(XX) || defined(RP1) || defined (SYNALC)

		args++;
		if(*args) {
		    while (isspace(*args))
			args++;
		    if (*args == '0' && *(args+1) == 'X' &&
			isxdigit(*(args+2))) {
			temp = gethexnum(args+2);
#if defined (RP1)
			tmp_confreg = read_nv_short((ushort *)config_ptr);
			if ((temp & CONSOLE_MASK) != 
			    (tmp_confreg & CONSOLE_MASK)) {
			    puts("Baud rate change ... confirm [y/n]?  ");
			    c = getchar();
			    putchar('\n');
			    if(c != 'y' && c != 'Y') return;
			}
			write_nv_short((ushort *)config_ptr,temp);
#else 
			if((temp & CONSOLE_MASK) != 
			   (*(u_short *)config_ptr & CONSOLE_MASK)) {
			    puts("Baud rate change ... confirm [y/n]?  ");
			    c = getchar();
			    putchar('\n');
			    if(c != 'y' && c != 'Y') return;
			}
			if (cpu_type == CPU_CANCUN) {
#if !defined(XX)
			    /* 
			     * Need to write Config value to config EEPROM
			     * one byte at a time.  Code below is
			     * to assure that we write in big endian
			     * format.
			     */
			    extern void eeprom_nvwritebyte();
			    eeprom_nvwritebyte(((u_short)temp) >> 8, 
					       (u_char *)config_ptr,0);
			    eeprom_nvwritebyte((temp) & 0xff, 
					       (u_char *)config_ptr,1);
#endif /* !defined(XX) */
			} else {
			    *(u_short *)config_ptr = temp;
			}
#endif /* not RP1 */
			return;
		    }
		    if(*args) {
			puts(illarg);
			return;
		    }
		}
#endif /* PAN, XX, or RP1 or SYNALC */
		reset_confreg();
		return;
	    }
	}
	puts(illarg);
	return;
    }
#endif /* PAN, XX, or RP1 or SYNALC */

#if defined(PAN) & defined(PAN_ORIGINAL)
    phys_confreg = *(u_char *)phys_confreg_ptr;
    puts("Bit#\tPhysical Configuration register option settings:\n");
    print_option("7\tDiagnostic mode ", ((~phys_confreg) & PHYS_DIAG_MASK),
		 disabled, enabled, NULL);
    print_option("6\tWatchdog timer ", ((~phys_confreg) & PHYS_WATCHDOG_MASK),
		 enabled, disabled, NULL);
    puts("5-4\tConsole speed is ");
    bootfile = ((~phys_confreg) & PHYS_CONSOLE_MASK) >> PHYS_CONSOLE_SHIFT;
    switch (bootfile) {
      case BAUDRATE_9600:
	puts("9600");
	break;
      case BAUDRATE_4800:
	puts("4800");
	break;
      case BAUDRATE_1200:
	puts("1200");
	break;
      case BAUDRATE_2400:
	puts("2400");
	break;
      default:
	puts("unknown");
	break;
    }
    puts("\n3-0\tBoot ");
    bootfile = phys_confreg & BOOT_FIELD;
    switch (bootfile) {
    case 0:				/* monitor */
	puts("to ROM monitor\n");
	break;
    case 1:				/* rom code */
	puts("default system software\n");
	break;
    default:
	/*
	 * Print the netboot file name in lowercase:
	 */
	printf("file is %s (or 'boot system' command)\n", makebfile(bootfile));
	break;
    }
    puts("The physical configuration register overrides the NVRAM configuration register\n\n");
#endif /* PAN & PAN_ORIGINAL */

    printf("Configuration register = %#x at last boot\n", (ushort)~(*((ushort *)&config)));
    puts("Bit#\tConfiguration register option settings:\n");
    print_option("15\tDiagnostic mode ", config.diag, disabled, enabled,
		 NULL);
    print_option("14\tIP broadcasts ", config.network, "do not ", NULL,
		 "have network numbers\n");
    print_option("13\t", config.romboot, "Do not boot ", "Boot ",
		 "default ROM software if network boot fails\n");
    puts("12-11\tConsole speed is ");
    switch (config.consspeed) {
      case BAUDRATE_9600:
	puts("9600");
	break;
      case BAUDRATE_4800:
	puts("4800");
	break;
      case BAUDRATE_1200:
	puts("1200");
	break;
      case BAUDRATE_2400:
	puts("2400");
	break;
      default:
	puts("unknown");
	break;
    }
    puts(" baud\n");
    print_option("10\tIP broadcasts with ", config.allzeros, "ones\n",
		 "zeros\n", NULL);

#if (!defined(PAN)) && (!defined(XX)) && (!defined(SYNALC))
    print_option("09\t", config.bootstrap, "Do not use ", "Use ",
		 "secondary bootstrap\n");
#endif PAN or XX or SYNALC

    print_option("08\tBreak ", config.breakenable, enabled, disabled, NULL);
    print_option("07\tOEM ", config.oem, disabled, enabled, NULL);

    print_option("06\tIgnore configuration ", config.ign_conf, disabled, enabled,
		 NULL);

#ifdef RP1
    print_option("04\tFast boot ", config.fastboot, disabled, enabled, NULL);
#endif

    bootfile = ~config.bootfile & 0xf;
    puts("03-00\tBoot ");
    switch (bootfile) {
    case 0:				/* monitor */
	puts("to ROM monitor\n");
	break;
    case 1:				/* rom code */
	puts("default system software\n");
	break;
    default:
	/*
	 * Print the netboot file name in lowercase:
	 */
#ifdef RP1
	if (~config.fastboot & 1)
	    printf("via [%s]\n",getvar(nvram->evartbl,"BOOT"));
	else
#endif
	printf("file is %s (or 'boot system' command)\n", makebfile(bootfile));
	break;
    }
    putchar('\n');
}


#ifdef PAN /* defined (SYNALC) */
void write_mac (c)
    uchar *c;
{
    int i, j;
    ushort mac;
    uchar *mac_ptr;
    uchar temp;
    uchar mac_array[4];

    i = j = 0;
    mac = 0;

    /*
     * Check for an argument
     */
    if (*c) {
	while (isspace(*c))
	    c++;

	/*
	 * Make sure the number looks right
	 */
	if (*c++ == '0' && *c++ == 'X' && isxdigit(*c)) {
	    puts("This will change the current MAC address, are you sure(y/n)?  ");
	    temp = getchar();
	    putchar('\n');
	    if (temp != 'y' && temp != 'Y') return;

	    /*
	     * Pick off the address, word by word, and load it into the
	     * cookie
	     */
	    mac_ptr = mac_array;
	    (void)x2444_io(X2444_CMD_WREN, 8, 0); /* Enable writing */
	    for (i = 0; i < 3; i++) {
		for (j = 0; j < 4; j++)
		    mac_array[j] = *c++;
		mac = (ushort)gethexnum(mac_ptr);
		(void)x2444_io(X2444_CMD_WRITE | (2 + i * 2) | 
			       mac << 8, 24, 0);
	    }
	    /*
	     * All done, clean up
	     */
	    (void)x2444_io(X2444_CMD_STO, 8, 0); /* Store data in EEPROM */
	    delay(10); /* need to wait for data to write to eeprom */
	    (void)x2444_io(X2444_CMD_WRDS, 8, 0); /* Disable writing */
	}
    } else
	puts(illarg);
}
#endif
