/* $Id: test.c,v 3.2.58.1 1996/05/10 23:17:10 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/test.c,v $
 *------------------------------------------------------------------
 * test.c -- parse the test command
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: test.c,v $
 * Revision 3.2.58.1  1996/05/10  23:17:10  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.2  1995/11/17  07:41:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:05:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "ctype.h"
#include "test.h"
#include "reason.h"
#include "extern.h"

char *function;			/* read or write function */
volatile short buserrcnt;	/* bus error count */

void rom_monitor_tests(), all_flash_tests();

test(s)
    register char *s;
{
    char comm;
    
    comm = *s;
    while (*s && !isspace(*s))
	s++;			/* skip rest of command */
    /* find command argument if present */
    while (*s && isspace(*s))
	s++;
    
    switch (comm) {

#ifdef SYSTEST
	case 'A':         /* invoke Applique test */
		check_applique();
		break;
	case 'C':         /* invoke MCI/SCI test */
		if (config.expert)
			mci_command();
		else {
			mci_autotest();
		}
		break;
	case 'D':	 /* invoke DS3 test */
		if (config.expert)
			ds3_command();
		else {
			ds3_autotest();
		}
		break;
	case 'E':	 /* invoke MEC test */
		if (config.expert)
			mec_command();
		else {
			mec_autotest();
		}
		break;
	case 'U':	 /* invoke ULTRA test */
		if (config.expert)
			ultra_command();
		else {
			ultra_autotest();
		}
		break;
#ifdef RP1
        case 'I':
		fc_all_flash_tests();
		break;

        case 'F':
		all_flash_tests();
		break;
#endif RP1

#endif

/*
 * Will probably need to change this #ifdef RP1 to also include other
 * arch. like MERLOT and DOSXX in the future...
 */
#ifdef RP1
        case 'G':
          rpdiagtest();
          break;

        case 'V':
          rpenvmtest();
          break;
#endif

      case 'M':			/* invoke memory test */
	memtest(s);
	break;
	
#if defined(CSC2) || defined(RP1)
      case 'P':			/* invoke IO/mem probe */
	ioprobetest();
	break;
#endif
	
#if defined DEBUG || defined XTESTS
      case 'X':
	rom_monitor_tests();	/* invoke ROM monitor test menu */
	break;
#endif
	
      case '?':
      case 'H':
#ifdef SYSTEST
	puts("A      MCI Applique test\n");
	puts("C      MCI/SCI test\n");
	puts("D      DS3 test\n");
	puts("E      MEC test\n");
#ifdef RP1
        puts("F      Local Flash tests\n");
        puts("I      External Flash Card tests\n");
#endif
#endif
#ifdef RP1
        puts("G      Diagnostic bus tests\n");
        puts("V      Environmental monitor tests\n");
#endif
	puts("M      Memory test\n");
	
#if defined(CSC2) || defined(RP1)
	puts("P      Probe IO/memory space\n");
#endif
	
#ifdef SYSTEST
	puts("U      UltraNetwork test\n");
#endif
#if defined(CSC2) || defined(RP1)
	puts("W      Worm memory test\n");
#endif

#if defined DEBUG || defined XTESTS
	puts("X      Extended firmware tests\n");
#endif
	
	break;
	
      default:
	puts(illarg);
	break;
    }

#ifdef RP1
    /*
     * Just being careful after diags.
     */
    sys_dirty = TRUE;    
    monlib_initialized = FALSE;
#endif
}

void buserrnote()
{
    buserrcnt++;
}
