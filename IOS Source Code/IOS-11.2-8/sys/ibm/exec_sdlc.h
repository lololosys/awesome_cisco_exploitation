/* $Id: exec_sdlc.h,v 3.2 1995/11/17 09:22:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/exec_sdlc.h,v $
 *------------------------------------------------------------------
 * Exec commands for SDLC
 *
 * April 1995, Leo Pereira
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_sdlc.h,v $
 * Revision 3.2  1995/11/17  09:22:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:21:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

 
/*******************************************************************
 *  sdlc test <interface> <addr> [<iterations> | continuous | stop] 
 *            [string <string>]
 */
EOLS       (exec_sdlc_test_eol, sdlc_exec_test_command, EXEC_SDLC_TEST );

STRING     (exec_sdlc_def_string, exec_sdlc_test_eol, no_alt,
            OBJ(string,1), "Text to be sent in the Test frame" );

KEYWORD_ID (exec_sdlc_input_string, exec_sdlc_def_string, exec_sdlc_test_eol,   
	    OBJ(int,4), TRUE, "string", 
	    "Enter a string of data you wish to send", PRIV_OPR );

KEYWORD_ID (exec_sdlc_test_stop, exec_sdlc_test_eol, exec_sdlc_input_string,
	    OBJ(int,2), SDLC_TEST_STOP,  "stop", 
	    "Stop sending SDLC test frames", PRIV_OPR);

KEYWORD_ID (exec_sdlc_test_cont, exec_sdlc_input_string, exec_sdlc_test_stop,
	    OBJ(int,2), SDLC_TEST_CONTINUOUS, "continuous", 
	    "Send SDLC Test Frames continuously", PRIV_OPR );
 
NUMBER     (exec_sdlc_test_iter, exec_sdlc_input_string, 
            exec_sdlc_test_cont, OBJ(int,3), 1, 254, 
            "Number of iterations (1-254)" );

SET        (exec_sdlc_test_set_iter, exec_sdlc_test_iter,
	    OBJ(int,3), DEFAULT_TEST_ITERATIONS);

SET        (exec_sdlc_test_set_key, exec_sdlc_test_set_iter,
	    OBJ(int,2), SDLC_TEST_ITERATIONS);

HEXDIGIT   (exec_sdlc_test_addr, exec_sdlc_test_set_key, no_alt, 
           OBJ(int,1), 1, 0xFF, "SDLC Address in Hex" ); 

INTERFACE (exec_sdlc_test_int, exec_sdlc_test_addr, no_alt,
	   OBJ(idb,1), IFTYPE_SERIAL );

KEYWORD   (exec_sdlc_test, exec_sdlc_test_int, no_alt,
           "test", "SDLC interface", PRIV_OPR );

/******************************************************************/
KEYWORD (exec_sdlc, exec_sdlc_test, ALTERNATE,
	 "sdlc", "Send SDLC test frames", PRIV_OPR );

#undef	ALTERNATE
#define	ALTERNATE	exec_sdlc


