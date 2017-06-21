/* $Id: cfg_mapclass_fr-traffic-shape.h,v 3.1.2.2 1996/05/20 05:37:54 skoh Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_mapclass_fr-traffic-shape.h,v $
 *------------------------------------------------------------------
 * Frame Relay Traffic Shaping and VC Queueing in Map Class
 *
 * March 1996, Charles Poe
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_mapclass_fr-traffic-shape.h,v $
 * Revision 3.1.2.2  1996/05/20  05:37:54  skoh
 * CSCdi56032:  New SVC commands in 11.2 (partial)
 * Branch: California_branch
 * Syntax change in fr svc map-class subcommands.
 *
 * Revision 3.1.2.1  1996/05/15  05:54:36  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/26  05:47:38  ccpoe
 * placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "fr_vcq.h"

/*******************************************************************
 *      becn-response-enable
 */

EOLS 	(cmc_fr_becn_eol, fr_traffic_command, FR_TRAFFIC_BECN);

NVGENS  (cmc_fr_becn_nvgen, cmc_fr_becn_eol,
	 fr_traffic_command, FR_TRAFFIC_BECN);

KEYWORD_MM (cmc_fr_becn, cmc_fr_becn_nvgen, no_alt,
	 "becn-response-enable", "Enable rate adjustment in response to BECN",
	 PRIV_CONF, 3);

/*******************************************************************
 *	traffic-rate <average-rate> [<peak-rate>]
 */
EOLS 	(cmc_fr_traffic_eol, fr_traffic_command, FR_TRAFFIC_RATE);

NUMBER  (cmc_fr_traffic_peak, cmc_fr_traffic_eol, cmc_fr_traffic_eol,
	 OBJ(int,2), 600, 1500000, "Peak rate (CIR + EIR)");

NUMBER  (cmc_fr_traffic_cir, cmc_fr_traffic_peak, no_alt,
	 OBJ(int,1), 600, 1500000, "Committed Information Rate (CIR)");

NVGENS  (cmc_fr_traffic_rate_nvgen, cmc_fr_traffic_cir,
	 fr_traffic_command, FR_TRAFFIC_RATE);

KEYWORD (cmc_fr_traffic_rate, cmc_fr_traffic_rate_nvgen, cmc_fr_becn,
	 "traffic-rate", "VC traffic rate", PRIV_CONF);

/*********************************************************************
 *	priority-group
 */

EOLS	(cmc_fr_priority_eol, fr_traffic_command, FR_VCQ_PRIORITY);

NUMBER  (cmc_fr_priority_num, cmc_fr_priority_eol, no_alt,
	 OBJ(int,1), 1, PRIORITY_MAXTYPE-1, "Priority group number");

NVGENS	(cmc_fr_priority_nvgen, cmc_fr_priority_num,
	 fr_traffic_command, FR_VCQ_PRIORITY);

KEYWORD (cmc_fr_prioritygroup, cmc_fr_priority_nvgen, cmc_fr_traffic_rate,
	 "priority-group", "VC priority queueing", PRIV_CONF);

/**********************************************************************
 *      custom-queue-list <1-16>
 */

EOLS	(cmc_fr_customq_eol, fr_traffic_command, FR_VCQ_CUSTOM);

NUMBER  (cmc_fr_customq_num, cmc_fr_customq_eol, no_alt,
	 OBJ(int,1), 1, PRIORITY_MAXTYPE-1, "Custom queue list number");

NVGENS	(cmc_fr_customq_nvgen, cmc_fr_customq_num,
	 fr_traffic_command, FR_VCQ_CUSTOM);

KEYWORD	(cmc_fr_customq, cmc_fr_customq_nvgen, cmc_fr_prioritygroup,
	 "custom-queue-list", "VC custom queueing", PRIV_CONF);

/***************************************************************
 * [no] frame-relay be {in|out} <rate, bits>
 */
EOLS	(cmc_fr_be_eol, fr_traffic_command, FR_MAP_CLASS_EBS);
PARAMS_KEYONLY(cmc_fr_outbe, cmc_fr_be_eol, "out", OBJ(int,1),
	       0, 16000000, fr_traffic_command, FR_MAP_CLASS_OUT_EBS,
	       "Outgoing Be", "Bits", PRIV_CONF);
PARAMS_KEYONLY(cmc_fr_inbe, cmc_fr_outbe, "in", OBJ(int,1),
	       0, 16000000, fr_traffic_command, FR_MAP_CLASS_IN_EBS,
	       "Incoming Be", "Bits", PRIV_CONF);
NUMBER  (cmc_fr_be_num, cmc_fr_be_eol, cmc_fr_inbe,
	 OBJ(int,1), 0, 16000000, "Applied to both Incoming/Outgoing Be, Bits");
KEYWORD	(cmc_fr_be, cmc_fr_be_num, cmc_fr_customq, "be", 
	 "Excess burst size (Be), Default = 56000 bits",
	 PRIV_CONF);
/***************************************************************
 * [no] frame-relay bc {in|out} <rate, bits>
 */
EOLS	(cmc_fr_bc_eol, fr_traffic_command, FR_MAP_CLASS_CBS);
PARAMS_KEYONLY(cmc_fr_outbc, cmc_fr_bc_eol, "out", OBJ(int,1),
	       2000, 16000000, fr_traffic_command, FR_MAP_CLASS_OUT_CBS,
	       "Outgoing Bc", "Bits", PRIV_CONF);
PARAMS_KEYONLY(cmc_fr_inbc, cmc_fr_outbc, "in", OBJ(int,1),
	       2000, 16000000, fr_traffic_command, FR_MAP_CLASS_IN_CBS,
	       "Incoming Bc", "Bits", PRIV_CONF);
NUMBER  (cmc_fr_bc_num, cmc_fr_bc_eol, cmc_fr_inbc,
	 OBJ(int,1), 2000, 16000000, "Applied to both Incoming/Outgoing Bc, Bits");
KEYWORD	(cmc_fr_bc, cmc_fr_bc_num, cmc_fr_be, "bc", 
	"Committed burst size (Bc), Default = 56000 bits",
	PRIV_CONF);
/***************************************************************
 * [no] frame-relay cir {in|out} <rate, bps>
 */
EOLS	(cmc_fr_cir_eol, fr_traffic_command, FR_MAP_CLASS_CIR);

NUMBER  (cmc_fr_cir_num, cmc_fr_cir_eol, no_alt, OBJ(int,1), 4000, 
	 45000000, "Applied to both Incoming/Outgoing CIR, Bits per second");

PARAMS_KEYONLY(cmc_fr_outcir, cmc_fr_cir_num, "out", OBJ(int,1),
	       4000, 45000000, fr_traffic_command, FR_MAP_CLASS_OUT_CIR,
	       "Outgoing CIR", "Bits per second", PRIV_CONF);
PARAMS_KEYONLY(cmc_fr_incir, cmc_fr_outcir, "in", OBJ(int,1),
	       4000, 45000000, fr_traffic_command, FR_MAP_CLASS_IN_CIR,
	       "Incoming CIR", "Bits per second", PRIV_CONF);

KEYWORD	(cmc_fr_cir, cmc_fr_incir, cmc_fr_bc, "cir", 
	 "Committed Information Rate (CIR), Default = 56000 bps", PRIV_CONF);
/***************************************************************/

#undef  ALTERNATE
#define ALTERNATE       cmc_fr_cir





