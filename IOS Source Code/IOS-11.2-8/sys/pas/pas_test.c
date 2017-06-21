/* $Id: pas_test.c,v 3.2.56.4 1996/05/21 10:00:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/pas_test.c,v $
 *------------------------------------------------------------------
 * pas_test.c -- subsystem support for PAS test commands
 *
 * May 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pas_test.c,v $
 * Revision 3.2.56.4  1996/05/21  10:00:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.56.3  1996/04/02  02:35:46  mbeesley
 * CSCdi49113:  test pas eeprom should ensure PA is powered on
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/03/21  23:24:26  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.56.1  1996/03/18  21:34:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:29:22  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/01/16  01:44:07  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/09  20:52:32  mbeesley
 * Branch: ELC_branch
 * Move port adaptor test support into pas_test.c.
 *
 * Revision 3.2  1995/11/17  18:55:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:22:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files 
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "subsys.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "interface.h"
#include "config.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "../hes/dgbus.h"

#include "if_pas.h"

#define	ALTERNATE	NONE
#include "pas_test_chain.h"
LINK_POINT(test_pas_commands, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request pas_test_chain_init_table[] = {
     { PARSE_ADD_TEST_CMD, &pname(test_pas_commands) },
     { PARSE_LIST_END, NULL }
 };

/*
 * Externals
 */
extern void transmit_test_add_commands(void);
extern void diag_exec_parser_init(void);

/*
 * Defintions
 */
#define BUFLEN 			80

/* 
 * pas_eeprom: 
 * Handle "test pas eeprom"
 */
void pas_eeprom (parseinfo *csb)
{
    char buff[BUFLEN];
    dgbus_eeprom_t eeprom_data;
    uint pa = 0;
    boolean parsed;
    char cmdchar = '?';
    uint addr = 0;
    char *data;
    int i;
    char rw = 'R';
    ulong data_byte = 0;
    pas_eeprom_type eeprom_type;

    /*
     * Get initial info: PA bay, EEPROM type
     */
    printf("\n Port Adaptor [%u]: ",pa);
    parsed = rdtty(buff, BUFLEN);
    if (!null(buff) && parsed) {
	pa = parse_unsigned(buff, &parsed);
	if (pa >= MAX_PA_BAYS){
	    printf("%%Illegal port adaptor number\n");
	    return;
	}
    }
    printf(" Use NMC93C46 ID EEPROM [y]: ");
    parsed = rdtty(buff, BUFLEN);
    eeprom_type = PAS_EEPROM_NMC93C46;
    if (!null(buff) && parsed) {
	if (toupper(buff[0]) == 'N') {
	    eeprom_type = PAS_EEPROM_X2444;
	}
    }

    /*
     * Power on the port adaptor
     */
    pas_control_power(pa, TRUE);
    DELAY(300);

    /*
     * Loop until the user quits or there is an error
     */
    while (TRUE) {
	
	printf("\n Port %d eeprom (? for help)[%c]:  ", pa, cmdchar);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed) {
	    cmdchar = toupper(buff[0]);
	}
	
	switch (cmdchar) {

	/* Print out a helpfull menu */
	   
	case 'H':
	case '?':
	    printf("\n  d - dump eeprom contents");
	    printf("\n  e - erase all locations (to 1)");
	    printf("\n  p - primitive access");
	    printf("\n  q - exit eeprom test");
	    printf("\n  z - zero eeprom");
	    printf("\n\n  'c' rules of radix type-in and display apply.\n");
	    break;

	/* Dump the EEPROM contents */

	case 'D':
	    if (!pas_access_ideeprom(pa, &eeprom_data, eeprom_type, 
				     PAS_EEPROM_CMD_READ, 16)) {
		printf("%%Read access failed\n");
		return;
	    }
	    data = (char *)&eeprom_data;
	    for (i=0; i < sizeof(dgbus_eeprom_t); i++) {
		if (!(i % 8))
		    printf("\n  PA %d, 0x%02x:  ",pa,i);
		printf("%02x  ", data[i] & 0xff);
	    }
	    if (i == 31)
		printf("\n");
	    
	    printf("\n");
	    break;
	
	/* erase the eeprom to all ff's */

	case 'E':
	    if (!pas_access_ideeprom(pa, &eeprom_data, eeprom_type, 
				     PAS_EEPROM_CMD_ERASE, 0)) {
		printf("%%Erase failed\n");
		return;
	    }
	    break;

	/* primtive access to eeprom */

	case 'P':
	    printf("\n  Address within PA %d eeprom, [0x%02x]? "
		   "(valid values are 0 to 0x1f):  ",
		   pa, addr);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed) {
		addr = parse_unsigned(buff,&parsed);
		if (addr > 0x1f){
		    printf("\n%%Illegal address \n");
		    return;
		}
	    }
	    printf("\n  Read or Write access to PA %d at 0x%02x [%c]? :  ",
		   pa, addr, rw);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed) 
		rw = toupper(buff[0]);
	    
	    if (!pas_access_ideeprom(pa, &eeprom_data, eeprom_type, 
				     PAS_EEPROM_CMD_READ, 16)) {
		printf("%%Read access failed\n");
		return;
	    }
	    data = (char *)&eeprom_data;
	    
	    if (rw == 'R') {
		printf("\n  Read %02x from pa %d at address 0x%02x\n",
		       data[addr] & 0xff, pa, addr);
		
	    } else if (rw == 'W') {
		printf("\n  Write data (hex 8 bits) [%02x]?:  ", data_byte);
		
		parsed = rdtty(buff, BUFLEN);
		if (!null(buff) && parsed) {
		    data_byte = parse_unsigned(buff,&parsed);
		    if (data_byte > 0xff){
			printf("\n%%Illegal value \n");
			return;
		    }
		}
		data[addr] = data_byte;
		if (!pas_access_ideeprom(pa, &eeprom_data, eeprom_type, 
					 PAS_EEPROM_CMD_WRITE, 0)) {
		    printf("%%Write access failed\n");
		    return;
		}
		printf("\n  Wrote 0x%02x, to pa %d at address 0x%02x\n",
		       data_byte, pa, addr);
	    } else {
		printf("\n%%Invalid selector\n");
		return;
	    }
	    addr++;
	    break;

	/* Quit */
  
	case 'Q':
	    return;

	/* Zero the eeprom */

	case 'Z':
	    if (!pas_access_ideeprom(pa, &eeprom_data, eeprom_type, 
				     PAS_EEPROM_CMD_ZERO, 0)) {
		printf("%%Zero failed\n");
		return;
	    }
	    break;
	
	/* Woops, user screwed up */

	default:
	    printf("%%Invalid selection\n");
	    break;

	}
    }
}

/*
 * pas_test_parser_init:
 * Initialize PAS test parser chains
 */
static void pas_test_parser_init (void)
{
    /*
     * Hook in the parser chain
     */
    parser_add_link_point(PARSE_ADD_TEST_PAS_CMD, "test pas",
                          &pname(test_pas_extend_here));
    parser_add_command_list(pas_test_chain_init_table,"PAS TEST commands");
}

/*
 * test_init
 * Initialize functions specific to a test image
 */
static void test_init (subsystype *subsys)
{
    transmit_test_add_commands();
    diag_exec_parser_init();
    pas_test_parser_init();
}

/*
 * TEST subsystem header
 */

#define TEST_MAJVERSION   1
#define TEST_MINVERSION   0
#define TEST_EDITVERSION  1

SUBSYS_HEADER(test, TEST_MAJVERSION, TEST_MINVERSION, TEST_EDITVERSION,
	      test_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);


/* end of file */
