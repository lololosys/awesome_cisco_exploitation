/* $Id: cfg_mapclass_atm.h,v 3.2.60.1 1996/03/29 19:26:47 grobelch Exp $
 * $Source: /release/111/cvs/Xsys/if/cfg_mapclass_atm.h,v $
 *------------------------------------------------------------------
 * ATM map class configuration commands
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_mapclass_atm.h,v $
 * Revision 3.2.60.1  1996/03/29  19:26:47  grobelch
 *   Provide traffic shaping for SVCs when possible.
 *   Also:
 *   - Specify map-class rate parameters in kbps.
 *   - Provide a per-interface command to indicate
 *     whether SVC should be established if signalled
 *     parameters cannot be installed.
 *   - Don't attach ATM submaps to map-class block
 *   - Check for map-class params prior to using
 *     them (they can be null).
 *   - Fix help-strings for "clp1" parameters to
 *     indicate for clp0&1 flows.
 *   - Signal best effort if only pcr for clp0&1 are
 *     specified.
 *   - map-class parameters override map-list where
 *     they overlap.
 *   - Fix a couple of erroneous variable setting/usages
 *     in static-map handling.
 * CSCdi51550:  ATMSIG: SVCs should attempt to use traffic shaping
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:26:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * atm traffic-mgmt-options <n>
 * no atm traffic-mgmt-options [<n>]
 *
 * GETOBJ(int,1) = <n>
 */
PARAMS_KEYONLY  (cmc_atm_traff, no_alt,
                 "traffic-mgmt-options",
                 OBJ(int,1), ATM_MAP_MIN, ATM_MAP_MAX,
                 atm_mapclass_command, ATM_MAP_TRFMGT,
                 "Specify traffic management options",
                 "Traffic management options", PRIV_CONF);

/***************************************************************
 * atm backward-max-burst-size--clp1 <size>
 * no atm backward-max-burst-size-clp1 [<size>]
 *
 * GETOBJ(int,1) = <size>
 */
PARAMS_KEYONLY  (cmc_atm_bkmx1, cmc_atm_traff,
                 "backward-max-burst-size-clp1",
                 OBJ(int,1), ATM_MAP_MIN, ATM_MAP_MAX,
                 atm_mapclass_command, ATM_MAP_BKMX1,
                 "Specify backward maximum burst size with CLP = 0&1",
                 "Burst size in cells", PRIV_CONF);

/***************************************************************
 * atm forward-max-burst-size-clp1 <size>
 * no atm forward-max-burst-size-clp1 [<size>]
 *
 * GETOBJ(int,1) = <size>
 */
PARAMS_KEYONLY  (cmc_atm_fwmx1, cmc_atm_bkmx1,
                 "forward-max-burst-size-clp1",
                 OBJ(int,1), ATM_MAP_MIN, ATM_MAP_MAX,
                 atm_mapclass_command, ATM_MAP_FWMX1,
                 "Specify forward maximum burst size with CLP = 0&1",
                 "Burst size in cells", PRIV_CONF);

/***************************************************************
 * atm backward-max-burst-size-clp0 <size>
 * no atm backward-max-burst-size-clp0 [<size>]
 *
 * GETOBJ(int,1) = <size>
 */
PARAMS_KEYONLY  (cmc_atm_bkmx0, cmc_atm_fwmx1,
                 "backward-max-burst-size-clp0",
                 OBJ(int,1), ATM_MAP_MIN, ATM_MAP_MAX,
                 atm_mapclass_command, ATM_MAP_BKMX0,
                 "Specify backward maximum burst size with CLP = 0",
                 "Burst size in cells", PRIV_CONF);

/***************************************************************
 * atm forward-max-burst-size-clp0 <size>
 * no atm forward-max-burst-size-clp0 [<size>]
 *
 * GETOBJ(int,1) = <size>
 */
PARAMS_KEYONLY  (cmc_atm_fwmx0, cmc_atm_bkmx0,
                 "forward-max-burst-size-clp0",
                 OBJ(int,1), ATM_MAP_MIN, ATM_MAP_MAX,
                 atm_mapclass_command, ATM_MAP_FWMX0,
                 "Specify forward maximum burst size with CLP = 0",
                 "Burst size in cells", PRIV_CONF);


/***************************************************************
 * atm backward-sustainable-cell-rate-clp1 <rate>
 * no atm backward-sustainable-cell-rate-clp1 [<rate>]
 *
 * GETOBJ(int,1) = <rate>
 */
PARAMS_KEYONLY  (cmc_atm_bkst1, cmc_atm_fwmx0,
                 "backward-sustainable-cell-rate-clp1",
                 OBJ(int,1), ATM_MAP_RATE_MIN, ATM_MAP_RATE_MAX,
                 atm_mapclass_command, ATM_MAP_BKST1,
                 "Specify backward sustainable cell rate with CLP = 0&1",
                 "Rate in Kbps", PRIV_CONF);

/***************************************************************
 * atm forward-sustainable-cell-rate-clp1 <rate>
 * no atm forward-sustainable-cell-rate-clp1 [<rate>]
 *
 * GETOBJ(int,1) = <rate>
 */
PARAMS_KEYONLY  (cmc_atm_fwst1, cmc_atm_bkst1,
                 "forward-sustainable-cell-rate-clp1",
                 OBJ(int,1), ATM_MAP_RATE_MIN, ATM_MAP_RATE_MAX,
                 atm_mapclass_command, ATM_MAP_FWST1,
                 "Specify forward sustainable cell rate with CLP = 0&1",
                 "Rate in Kbps", PRIV_CONF);

/***************************************************************
 * atm backward-sustainable-cell-rate-clp0 <rate>
 * no atm backward-sustainable-cell-rate-clp0 [<rate>]
 *
 * GETOBJ(int,1) = <rate>
 */
PARAMS_KEYONLY  (cmc_atm_bkst0, cmc_atm_fwst1,
                 "backward-sustainable-cell-rate-clp0",
                 OBJ(int,1), ATM_MAP_RATE_MIN, ATM_MAP_RATE_MAX,
                 atm_mapclass_command, ATM_MAP_BKST0,
                 "Specify backward sustainable cell rate with CLP = 0",
                 "Rate in Kbps", PRIV_CONF);

/***************************************************************
 * atm forward_sustainable-cell-rate-clp0 <rate>
 * no atm forward_sustainable-cell-rate-clp0 [<rate>]
 *
 * GETOBJ(int,1) = <rate>
 */
PARAMS_KEYONLY  (cmc_atm_fwst0, cmc_atm_bkst0,
                 "forward-sustainable-cell-rate-clp0", 
		 OBJ(int,1), ATM_MAP_RATE_MIN, ATM_MAP_RATE_MAX,
                 atm_mapclass_command, ATM_MAP_FWST0,
                 "Specify forward sustainable cell rate with CLP = 0",
                 "Rate in Kbps", PRIV_CONF);

/***************************************************************
 * atm backward-peak-cell-rate-clp1 <rate>
 * no atm backward-peak-cell-rate-clp1 [<rate>]
 *
 * GETOBJ(int,1) = <rate>
 */

PARAMS_KEYONLY  (cmc_atm_bkpk1, cmc_atm_fwst0,
                 "backward-peak-cell-rate-clp1", 
		 OBJ(int,1), ATM_MAP_RATE_MIN, ATM_MAP_RATE_MAX,
                 atm_mapclass_command, ATM_MAP_BKPK1,
                 "Specify backward peak cell rate with CLP = 0&1",
                 "Rate in Kbps", PRIV_CONF);

/***************************************************************
 * atm forward-peak-cell-rate-clp1 <rate>
 * no atm forward-peak-cell-rate-clp1 [<rate>]
 *
 * GETOBJ(int,1) = <rate>
 */
PARAMS_KEYONLY  (cmc_atm_fwpk1, cmc_atm_bkpk1,
                 "forward-peak-cell-rate-clp1", 
		 OBJ(int,1), ATM_MAP_RATE_MIN, ATM_MAP_RATE_MAX,
                 atm_mapclass_command, ATM_MAP_FWPK1,
                 "Specify forward peak cell rate with CLP = 0&1",
                 "Rate in Kbps", PRIV_CONF);

/***************************************************************
 * atm backward-peak-cell-rate-clp0 <rate>
 * no atm backward-peak-cell-rate-clp0 [<rate>]
 *
 * GETOBJ(int,1) = <rate>
 */
PARAMS_KEYONLY  (cmc_atm_bkpk0, cmc_atm_fwpk1,
                 "backward-peak-cell-rate-clp0", 
		 OBJ(int,1), ATM_MAP_RATE_MIN, ATM_MAP_RATE_MAX,
                 atm_mapclass_command, ATM_MAP_BKPK0,
                 "Specify backward peak cell rate with CLP = 0",
                 "Rate in Kbps", PRIV_CONF);

/***************************************************************
 * atm forward-peak-cell-rate-clp0 <rate>
 * no atm forward-peak-cell-rate-clp0 [<rate>]
 *
 * GETOBJ(int,1) = <rate>
 */
PARAMS_KEYONLY	(cmc_atm_fwpk0, cmc_atm_bkpk0,
		 "forward-peak-cell-rate-clp0", 
		 OBJ(int,1), ATM_MAP_RATE_MIN, ATM_MAP_RATE_MAX,
		 atm_mapclass_command, ATM_MAP_FWPK0,
	 	 "Specify forward peak cell rate with CLP = 0",
		 "Rate in Kbps", PRIV_CONF);

/***************************************************************
 * [no] atm layer2-protocol-id <id>
 *
 */
EOLS    (cmc_atm_layer2id_eol, atm_mapclass_command, ATM_MAP_LAYER2ID);
HEXDIGIT(cmc_atm_layer2id_value, cmc_atm_layer2id_eol, no_alt,
         OBJ(int,1), 0, 0x1F, "BLL protocol id in Hex");
/*NVGENS  (cmc_atm_layer2id_nvgen, cmc_atm_layer2id_value,
         atm_mapclass_command, ATM_MAP_LAYER2ID);*/
KEYWORD (cmc_atm_layer2id, cmc_atm_layer2id_value, cmc_atm_fwpk0,
         "layer2-protocol-id", "BLL-IE layer 2 protocol id",
         PRIV_CONF);

/***************************************************************
 * [no] atm broadcast
 */
EOLS	(cmc_atm_broad_eol, atm_mapclass_command, ATM_MAP_BROAD);
KEYWORD	(cmc_atm_broad, cmc_atm_broad_eol, cmc_atm_layer2id,
	 "broadcast", "ATM pseudo-broadcast", PRIV_CONF);

/***************************************************************
 * [no] atm aal5mux 
 */
EOLS    (cmc_atm_aal5mux_eol, atm_mapclass_command, ATM_MAP_AAL5MUX);
KEYWORD (cmc_atm_aal5mux, cmc_atm_aal5mux_eol, cmc_atm_broad,
         "aal5mux", "ATM AAL5-MUX encap", PRIV_CONF);

/***************************************************************/
KEYWORD	(cmc_atm, cmc_atm_aal5mux, ALTERNATE,
           "atm", "Configure atm static map class", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE       cmc_atm
