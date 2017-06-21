/* $Id: rsp_ci_test.h,v 3.2.62.1 1996/08/12 16:07:36 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_ci_test.h,v $
 *------------------------------------------------------------------
 * CI test commands
 *
 * November 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_ci_test.h,v $
 * Revision 3.2.62.1  1996/08/12  16:07:36  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:47:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:57:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum {
    TEST_CI_READ = 1,
    TEST_CI_WRITE,
    TEST_CI_START,
    TEST_CI_STOP,
    TEST_CI_ACK,
};

/*
 * test ci x9241 ADDR	read-wcr WCR
 *			write-wcr WCR WCR-VALUE
 *			read-reg WCR REG
 *			write-reg WCR REG REG-VALUE
 *			reg-to-wcr REG WCR
 *			wcr-to-reg WCR REG
 *			reg-to-allwcrs REG
 *			allwcrs-to-reg REG
 *			disable WCR
 *
 * OBJ(int,1)	X9241 address (unit)
 * OBJ(int,2)	X9241 command
 * OBJ(int,3)	WCR number
 * OBJ(int,4)	REG number
 * OBJ(int,5)	WCR-VALUE or REG-VALUE
 */

EOLNS	(tc_x9241_eol, test_ci_x9241);

NUMBER	(tc_wrval, tc_x9241_eol, NONE,
	 OBJ(int,5), 0, 63, "value to write");

TESTVAR	(tc_regv_test, tc_wrval, tc_x9241_eol, NONE, NONE, no_alt,
	 OBJ(int,2), WRITE_REG);

NUMBER	(tc_reg_eol, tc_regv_test, NONE,
	 OBJ(int,4), 0, 3, "register number");

NUMBER	(tc_wcr_reg_eol, tc_reg_eol, NONE,
	 OBJ(int,3), 0, 3, "wcr number");

TESTVAR	(tc_wcrv_eol_test, tc_wrval, tc_x9241_eol, NONE, NONE, no_alt,
	 OBJ(int,2), WRITE_WCR);

NUMBER	(tc_wcr_wcrv_eol, tc_wcrv_eol_test, NONE,
	 OBJ(int,3), 0, 3, "wcr number");

NUMBER	(tc_reg_wcr_eol, tc_wcr_wcrv_eol, NONE,
	 OBJ(int,4), 0, 3, "register number");

KEYWORD_ID(tc_allwcrs2reg, tc_reg_eol, NONE,
	   OBJ(int,2), XFR_ALLWCRS2REG,
	   "all-wcrs-to-reg", "copy wcr to register on all pots", 0);

KEYWORD_ID(tc_reg2allwcrs, tc_reg_eol, tc_allwcrs2reg,
	   OBJ(int,2), XFR_REG2ALLWCRS,
	   "reg-to-all-wcrs", "copy register to wcr on all pots ", 0);

KEYWORD_ID(tc_wcr2reg, tc_wcr_reg_eol, tc_reg2allwcrs,
	   OBJ(int,2), XFR_WCR2REG,
	   "wcr-to-reg", "copy wcr to register", 0);

KEYWORD_ID(tc_reg2wcr, tc_reg_wcr_eol, tc_wcr2reg,
	   OBJ(int,2), XFR_REG2WCR,
	   "reg-to-wcr", "copy register to wcr", 0);

KEYWORD_ID(tc_writereg, tc_wcr_reg_eol, tc_reg2wcr,
	   OBJ(int,2), WRITE_REG,
	   "write-reg", "write data register", 0);

KEYWORD_ID(tc_readreg, tc_wcr_reg_eol, tc_writereg,
	   OBJ(int,2), READ_REG,
	   "read-reg", "read data register", 0);

KEYWORD_ID(tc_writewcr, tc_wcr_wcrv_eol, tc_readreg,
	   OBJ(int,2), WRITE_WCR,
	   "write-wcr", "write wiper counter register", 0);

KEYWORD_ID(tc_readwcr, tc_wcr_wcrv_eol, tc_writewcr,
	   OBJ(int,2), READ_WCR,
	   "read-wcr", "read wiper counter register", 0);

KEYWORD_ID(tc_disable, tc_wcr_wcrv_eol, tc_readwcr,
	   OBJ(int,2), DISABLE_POT,
	   "disable", "disable potentiometer", 0);

NUMBER	(tc_x9241addr, tc_disable, NONE,
	 OBJ(int,1), 0, 15, "X9241 address");

KEYWORD	(test_ci_x9241, tc_x9241addr, NONE,
	 "x9241", "X9241 commands", 0);

/*
 * test ci shutdown-override [ off ]
 *
 * OBJ(int,1) == 1 if "off" keyword is present
 */

EOLNS	(tc_over_eol, test_ci_override);

KEYWORD_ID(tc_over_off, tc_over_eol, tc_over_eol,
	   OBJ(int,1), 1,
	   "off", "enable automatic power shutdown", 0);

KEYWORD	(tc_override, tc_over_off, test_ci_x9241,
	 "shutdown-override", "inhibit automatic power shutdown", 0);

/*
 * test ci temperature-source { slot # }
 *
 * OBJ(int,1) == { slot # }
 * OBJ(int,2) == { inlet | hotpoint | exhaust }
 */

EOLNS	(tempsource_eol, test_ci_tempsource);

KEYWORD_ID(tempsource_exhaust, tempsource_eol, NONE,
	   OBJ(int,2), TEMPSOURCE_EXHAUST,
	   exhaust_str, "Air exhaust sensor", 0);

KEYWORD_ID(tempsource_hotpoint, tempsource_eol, tempsource_exhaust,
	   OBJ(int,2), TEMPSOURCE_HOTPOINT,
	   hotpoint_str, "Hot spot sensor", 0);

KEYWORD_ID(tempsource_inlet, tempsource_eol, tempsource_hotpoint,
	   OBJ(int,2), TEMPSOURCE_INLET,
	   inlet_str, "Air inlet sensor", 0);

NUMBER	(tempsource, tempsource_inlet, NONE,
	 OBJ(int,1), 0, MAX_SLOTS - 1, "source slot");

KEYWORD	(tc_tempsel, tempsource, tc_override,
	 "temperature-source", "source for power-shutdown temperature", 0);

/*
 * test ci ...
 */

KEYWORD	(test_ci, tc_tempsel, ALTERNATE,
	 "ci", NULL /* NOHELP */, PRIV_ROOT|PRIV_NOHELP);

#undef	ALTERNATE
#define	ALTERNATE	test_ci
