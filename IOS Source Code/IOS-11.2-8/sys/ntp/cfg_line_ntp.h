/* $Id: cfg_line_ntp.h,v 3.1.2.2 1996/08/19 18:54:33 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/cfg_line_ntp.h,v $
 *------------------------------------------------------------------
 * NTP line command parser chains
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_line_ntp.h,v $
 * Revision 3.1.2.2  1996/08/19  18:54:33  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.2.1  1996/04/16  19:02:56  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:24:27  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* [no] ntp pps-discipline <signal> [inverted] [pps-offset <microseconds>]
*			[stratum <n>]
*
* OBJ(int,1) = REFCLK_ATOM_PPS
* OBJ(int,2) = fudge offset
* OBJ(int,3) = PPS signal (NTP_PPS_xxx)
* OBJ(int,4) = stratum (or STRATUM_UNSPEC)
* OBJ(int,5) = 0
* OBJ(int,6) = NTP_PPS_INVERTED if "inverted" specified
* OBJ(int,7) = TRUE if "stratum" was specified
*
* Use a PPS pulse on this line to discipline the clock
*/

pdecl(ntp_line_pps_dis_opts);

EOLS    (ntp_line_pps_dis_eol, ntp_line_cmd, NTP_PPS_DIS_LINE_CMD);

NUMBER  (ntp_line_pps_dis_stratum_number, ntp_line_pps_dis_opts,
	 no_alt, OBJ(int,4), 0, NTP_MAXSTRATUM-1, "Stratum number");
KEYWORD_ID (ntp_line_pps_dis_stratum, ntp_line_pps_dis_stratum_number,
	    ntp_line_pps_dis_eol, OBJ(int,7), TRUE, "stratum",
	    "Reference clock stratum", PRIV_CONF);
SPLIT	(ntp_line_pps_dis_stratum_test, ntp_line_pps_dis_stratum,
	 ntp_line_pps_dis_eol, OBJ(int,4), STRATUM_UNSPEC);

SIGNED_DEC  (ntp_line_pps_dis_offset_value, ntp_line_pps_dis_opts, no_alt,
	     OBJ(int,2), -MAXINT, MAXINT, "Offset in microseconds");
KEYWORD (ntp_line_pps_dis_offset, ntp_line_pps_dis_offset_value,
	 ntp_line_pps_dis_stratum_test, "pps-offset", "Clock offset",
	 PRIV_CONF);
SPLIT	(ntp_line_pps_dis_off_test, ntp_line_pps_dis_offset,
	 ntp_line_pps_dis_stratum_test, OBJ(int,2), 0);

NOP	(ntp_line_pps_dis_opts, ntp_line_pps_dis_off_test, NONE);

KEYWORD_ID (ntp_line_pps_dis_invert, ntp_line_pps_dis_opts,
	    ntp_line_pps_dis_opts, OBJ(int,6), NTP_PPS_INVERTED, "inverted",
	    "PPS signal is inverted", PRIV_CONF);

KEYWORD_ID (ntp_line_pps_dis_ri, ntp_line_pps_dis_invert,
	    no_alt, OBJ(int,3), NTP_PPS_RI,
	    "ri", "PPS on RI", PRIV_CONF);
IFELSE	(ntp_line_pps_dis_ri_test, ntp_line_pps_dis_ri,
	 no_alt, ntp_pps_support_test(csb, NTP_PPS_RI));
KEYWORD_ID (ntp_line_pps_dis_dsr, ntp_line_pps_dis_invert,
	    ntp_line_pps_dis_ri_test, OBJ(int,3), NTP_PPS_DSR,
	    "dsr", "PPS on DSR", PRIV_CONF);
IFELSE	(ntp_line_pps_dis_dsr_test, ntp_line_pps_dis_dsr,
	 ntp_line_pps_dis_ri_test, ntp_pps_support_test(csb, NTP_PPS_DSR));
KEYWORD_ID (ntp_line_pps_dis_dtr, ntp_line_pps_dis_invert,
	    ntp_line_pps_dis_dsr_test, OBJ(int,3), NTP_PPS_DTR,
	    "dtr", "PPS on DTR", PRIV_CONF);
IFELSE	(ntp_line_pps_dis_dtr_test, ntp_line_pps_dis_dtr,
	 ntp_line_pps_dis_dsr_test, ntp_pps_support_test(csb, NTP_PPS_DTR));
KEYWORD_ID (ntp_line_pps_dis_cts, ntp_line_pps_dis_invert,
	    ntp_line_pps_dis_dtr_test, OBJ(int,3), NTP_PPS_CTS,
	    "cts", "PPS on CTS", PRIV_CONF);
IFELSE	(ntp_line_pps_dis_cts_test, ntp_line_pps_dis_cts,
	 ntp_line_pps_dis_dtr_test, ntp_pps_support_test(csb, NTP_PPS_CTS));
KEYWORD_ID (ntp_line_pps_dis_rts, ntp_line_pps_dis_invert,
	    ntp_line_pps_dis_cts_test, OBJ(int,3), NTP_PPS_RTS,
	    "rts", "PPS on RTS", PRIV_CONF);
IFELSE	(ntp_line_pps_dis_rts_test, ntp_line_pps_dis_rts,
	 ntp_line_pps_dis_cts_test, ntp_pps_support_test(csb, NTP_PPS_RTS));
KEYWORD_ID (ntp_line_pps_dis_dcd, ntp_line_pps_dis_invert,
	    ntp_line_pps_dis_rts_test, OBJ(int,3), NTP_PPS_DCD,
	    "dcd", "PPS on DCD", PRIV_CONF);
IFELSE	(ntp_line_pps_dis_dcd_test, ntp_line_pps_dis_dcd,
	 ntp_line_pps_dis_rts_test, ntp_pps_support_test(csb, NTP_PPS_DCD));

NOPREFIX(ntp_line_pps_dis_no, ntp_line_pps_dis_dcd_test, ntp_line_pps_dis_eol);
NVGENS	(ntp_line_pps_dis_nv, ntp_line_pps_dis_no, ntp_line_cmd,
	 NTP_PPS_DIS_LINE_CMD);
SET	(ntp_line_pps_dis_set_2, ntp_line_pps_dis_nv, OBJ(int,4),
	 STRATUM_UNSPEC);
SET	(ntp_line_pps_dis_set_1, ntp_line_pps_dis_set_2, OBJ(int,1),
	 REFCLK_ATOM_PPS);
KEYWORD (ntp_line_pps_dis, ntp_line_pps_dis_set_1, no_alt, "pps-discipline",
	 "Use PPS pulse to discipline system clock", PRIV_CONF);
IFELSE	(ntp_line_pps_dis_test, ntp_line_pps_dis, no_alt,
	 ntp_refclock_present(REFCLK_ATOM_PPS));


/******************************************************************************
* [no] ntp refclock <manufacturer> <model> pps <signal> [inverted]
*      [timestamp-offset <microseconds>][pps-offset <microseconds>]
*      [stratum <n>]
*
* OBJ(int,1) = manufacturer/model
* OBJ(int,2) = PPS offset
* OBJ(int,3) = PPS signal (NTP_PPS_xxx)
* OBJ(int,4) = stratum
* OBJ(int,5) = timestamp offset
* OBJ(int,6) = NTP_PPS_INVERTED if inverting the timestamp
* OBJ(int,7) = TRUE if "stratum" was specified
*
* Configure a reference clock on this line
*/

pdecl(ntp_line_refclock_opts);

EOLS    (ntp_line_refclock_eol, ntp_line_cmd, NTP_REFCLOCK_LINE_CMD);

NUMBER  (ntp_line_refclock_stratum_number, ntp_line_refclock_opts,
	 no_alt, OBJ(int,4), 0, NTP_MAXSTRATUM-1, "Stratum number");
KEYWORD_ID (ntp_line_refclock_stratum, ntp_line_refclock_stratum_number,
	    ntp_line_refclock_eol, OBJ(int,7), TRUE, "stratum",
	    "Reference clock stratum",  PRIV_CONF);
SPLIT	(ntp_line_refclock_stratum_test, ntp_line_refclock_stratum,
	 ntp_line_refclock_eol, OBJ(int,4), 0);

SIGNED_DEC  (ntp_line_pps_off_value, ntp_line_refclock_opts, no_alt,
	     OBJ(int,2), -MAXINT, MAXINT, "Offset in microseconds");
KEYWORD (ntp_line_refclock_pps_off, ntp_line_pps_off_value,
	 ntp_line_refclock_stratum_test, "pps-offset",
	 "Offset of PPS pulse", PRIV_CONF);
SPLIT	(ntp_line_refclock_pps_off_test, ntp_line_refclock_pps_off,
	 ntp_line_refclock_stratum_test, OBJ(int,2), 0);

SIGNED_DEC  (ntp_line_ts_off_value, ntp_line_refclock_opts, no_alt,
	     OBJ(int,5), -MAXINT, MAXINT, "Offset in microseconds");
KEYWORD (ntp_line_refclock_ts_off, ntp_line_ts_off_value,
	 ntp_line_refclock_pps_off_test, "timestamp-offset",
	 "Offset of timestamp", PRIV_CONF);
SPLIT	(ntp_line_refclock_ts_off_test, ntp_line_refclock_ts_off,
	 ntp_line_refclock_pps_off_test, OBJ(int,5), 0);

NOP	(ntp_line_refclock_opts, ntp_line_refclock_ts_off_test, NONE);

KEYWORD_ID (ntp_line_refclock_invert, ntp_line_refclock_opts,
	    ntp_line_refclock_opts, OBJ(int,6), NTP_PPS_INVERTED, "inverted",
	    "PPS signal is inverted", PRIV_CONF);

KEYWORD_ID (ntp_line_refclock_pps_ri, ntp_line_refclock_invert,
	    no_alt, OBJ(int,3), NTP_PPS_RI,
	    "ri", "PPS on RI", PRIV_CONF);
IFELSE	(ntp_line_refclock_ri_test, ntp_line_refclock_pps_ri,
	 no_alt, ntp_pps_support_test(csb, NTP_PPS_RI));
KEYWORD_ID (ntp_line_refclock_pps_dsr, ntp_line_refclock_invert,
	    ntp_line_refclock_ri_test, OBJ(int,3), NTP_PPS_DSR,
	    "dsr", "PPS on DSR", PRIV_CONF);
IFELSE	(ntp_line_refclock_dsr_test, ntp_line_refclock_pps_dsr,
	 ntp_line_refclock_ri_test, ntp_pps_support_test(csb, NTP_PPS_DSR));
KEYWORD_ID (ntp_line_refclock_pps_dtr, ntp_line_refclock_invert,
	    ntp_line_refclock_dsr_test, OBJ(int,3), NTP_PPS_DTR,
	    "dtr", "PPS on DTR", PRIV_CONF);
IFELSE	(ntp_line_refclock_dtr_test, ntp_line_refclock_pps_dtr,
	 ntp_line_refclock_dsr_test, ntp_pps_support_test(csb, NTP_PPS_DTR));
KEYWORD_ID (ntp_line_refclock_pps_cts, ntp_line_refclock_invert,
	    ntp_line_refclock_dtr_test, OBJ(int,3), NTP_PPS_CTS,
	    "cts", "PPS on CTS", PRIV_CONF);
IFELSE	(ntp_line_refclock_cts_test, ntp_line_refclock_pps_cts,
	 ntp_line_refclock_dtr_test, ntp_pps_support_test(csb, NTP_PPS_CTS));
KEYWORD_ID (ntp_line_refclock_pps_rts, ntp_line_refclock_invert,
	    ntp_line_refclock_cts_test, OBJ(int,3), NTP_PPS_RTS,
	    "rts", "PPS on RTS", PRIV_CONF);
IFELSE	(ntp_line_refclock_rts_test, ntp_line_refclock_pps_rts,
	 ntp_line_refclock_cts_test, ntp_pps_support_test(csb, NTP_PPS_RTS));
KEYWORD_ID (ntp_line_refclock_pps_dcd, ntp_line_refclock_invert,
	    ntp_line_refclock_rts_test, OBJ(int,3), NTP_PPS_DCD,
	    "dcd", "PPS on DCD", PRIV_CONF);
IFELSE	(ntp_line_refclock_dcd_test, ntp_line_refclock_pps_dcd,
	 ntp_line_refclock_rts_test, ntp_pps_support_test(csb, NTP_PPS_DCD));
KEYWORD_ID (ntp_line_refclock_pps_none, ntp_line_refclock_invert,
	    ntp_line_refclock_dcd_test, OBJ(int,3), NTP_PPS_NONE,
	    "none", "No PPS signal available", PRIV_CONF);

NVGENS	(ntp_line_refclock_nv, ntp_line_refclock_pps_none, ntp_line_cmd,
	 NTP_REFCLOCK_LINE_CMD);
KEYWORD (ntp_line_refclock_pps, ntp_line_refclock_nv, no_alt,
	 "pps", "Pulse-per-second signal line", PRIV_CONF);

KEYWORD_ID (ntp_line_refclock_nmea, ntp_line_refclock_pps, no_alt,
	    OBJ(int,1), REFCLK_GPS_NMEA, "nmea",
	    "NMEA 103-v2.0 Protocol", PRIV_CONF);
IFELSE	(ntp_line_refclock_nmea_test, ntp_line_refclock_nmea,
	 no_alt, ntp_refclock_present(REFCLK_GPS_NMEA));

KEYWORD_ID (ntp_line_refclock_trimble, ntp_line_refclock_pps,
	    ntp_line_refclock_nmea_test, OBJ(int,1), REFCLK_TRIMBLE, "trimble",
	    "Trimble Navigation TSIP Protocol", PRIV_CONF);
IFELSE	(ntp_line_refclock_trimble_test, ntp_line_refclock_trimble,
	 ntp_line_refclock_nmea_test, ntp_refclock_present(REFCLK_TRIMBLE));

KEYWORD_ID (ntp_line_refclock_telsol, ntp_line_refclock_pps,
	    ntp_line_refclock_trimble_test, OBJ(int,1), REFCLK_TELSOL,
	    "telecom-solutions", "Telecom Solutions GPS", PRIV_CONF);
IFELSE	(ntp_line_refclock_telsol_test, ntp_line_refclock_telsol,
	 ntp_line_refclock_trimble_test, ntp_refclock_present(REFCLK_TELSOL));

NOPREFIX(ntp_line_refclock_no, ntp_line_refclock_telsol_test,
	 ntp_line_refclock_eol);
KEYWORD (ntp_line_refclock, ntp_line_refclock_no,
	 ntp_line_pps_dis_test, "refclock", "NTP Reference Clock", PRIV_CONF);

KEYWORD (ntp_line, ntp_line_refclock, ALTERNATE, "ntp", "Configure NTP",
	 PRIV_CONF);

IFELSE  (ntp_line_test, ntp_line, no_alt, ntp_any_refclocks_present());

#undef ALTERNATE
#define ALTERNATE ntp_line_test
