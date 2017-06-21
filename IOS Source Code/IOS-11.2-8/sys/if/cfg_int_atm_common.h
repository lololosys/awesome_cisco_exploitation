/* $Id: cfg_int_atm_common.h,v 3.1.4.4 1996/05/24 21:09:48 schrupp Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_atm_common.h,v $
 *------------------------------------------------------------------
 * cfg_int_atm_common.h:
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_atm_common.h,v $
 * Revision 3.1.4.4  1996/05/24  21:09:48  schrupp
 * CSCdi58635:  ATM: ESI cleanup
 * Branch: California_branch
 *
 *  - Deregister addresses when a new prefix is added by ILMI
 *  - Deregister addresses when ESI/NSAP is changed from command line
 *  - Reregister addresses even if the new prefix is the same as the old
 *  - Deregister addresses when subinterface is shut
 *  - Reregister addresses when subinterface is no shutD
 *  - Don't allow qsaal and ilmi pvc's on subinterfaces
 *  - Init atm_db->peeruni_ifname
 *
 * Revision 3.1.4.3  1996/05/09  14:29:44  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.4.1.2.3  1996/05/04  01:34:15  jwjang
 * CSCdi56712:  atm uni-version cmd is allowed while interface is up
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.4.1.2.2  1996/05/01  17:25:53  jwjang
 * CSCdi56347:  add atm uni-version interface command
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.4.1.2.1  1996/04/27  07:03:36  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.4.2  1996/05/07  16:00:04  rzagst
 * CSCdi45984:  CBUS-3-OUTHUNG: ATM3/0: tx0 output hung (800E = queue full)
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:13:02  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  04:35:16  rzagst
 * Place holder for cfg_int_atm_common.h
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * ATM pvc <vc no.> <vpi>,<vci> <encaps> [MIDlow MIDhigh] [Peak Avg.[Burst]]
 *	[INARP <time in seconds>]
 *	[OAM <time in seconds>]
 * no ATM pvc <vc no.>
 *
 * OBJ(int,1) = <vc no.>
 * OBJ(int,2) = <vpi>
 * OBJ(int,3) = <vci>
 * OBJ(int,4) = AAL + SNAP values 
 * OBJ(int,5) = Peak rate in Kbps.
 * OBJ(int,6) = Average rate in Kbps.
 * OBJ(int,7) = Burst size in normally in cells can be in 32 cell bursts
 * OBJ(int,8) = MID low
 * OBJ(int,9) = MID high
 *	(AAL5SNAP, AAL5MUX, AAL5NLPID, AAL5FRNLPID, AAL34SMDS)
 * OBJ(int,10) = AAL5MUX protocol(if applicable)
 * OBJ(int,11) = InARP frequency(in minutes) 0 == disabled
 * OBJ(int,12) = OAM loopback frequency(in seconds) 0 == DISABLED
 * OBJ(int,20) = LANE pvc-types.
 */
EOLS	(ci_atm_pvc_eol, atm_interface_commands, ATM_CMD_PVC);

NUMBER	(ci_atm_pvc_oamfreq, ci_atm_pvc_eol, ci_atm_pvc_eol, OBJ(int,12),
	1, 600, "OAM loopback frequency(seconds)");

KEYWORD_ID(ci_atm_pvc_oam, ci_atm_pvc_oamfreq, ci_atm_pvc_eol, 
	OBJ(int,12), ATM_VC_OAM, "oam",
	"OAM loopback enable", PRIV_CONF|PRIV_SUBIF);

LINK_TRANS(ci_atm_pvc_inarp_return_here, ci_atm_pvc_oam);
LINK_TRANS(ci_atm_pvc_inarp_extend_here, ci_atm_pvc_oam);

IFELSE	(ci_atm_pvc_inarp_chk, ci_atm_pvc_inarp_extend_here, ci_atm_pvc_oam,
         atm_inarp_on_aal(GETOBJ(int,4))) ;


/*
 * Burst pvc range is chipset dependent.
 */

static void ci_atm_pvc_burst_cells (parseinfo *csb, uint *lower, uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr &&
	csb->interface->hwptr->atm_db) {
	*lower = 1;
	*upper = csb->interface->hwptr->atm_db->def_config.max_burst;
    }
}

/* This portion of the atm pvc command is used for platforms such as
 * the AIP that require a burstsize of 32 cell burst vs. the normal
 * configuring of burst size in cells.
 */
#ifdef USING_32_CELL_BURSTS
NUMBER_FUNC (ci_atm_pvc_burst, ci_atm_pvc_inarp_chk, ci_atm_pvc_inarp_chk,
	    OBJ(int,7), ci_atm_pvc_burst_cells,
	     "Burst size in number of 32 cell bursts");

#else
NUMBER_FUNC (ci_atm_pvc_burst, ci_atm_pvc_inarp_chk, ci_atm_pvc_inarp_chk,
	    OBJ(int,7), ci_atm_pvc_burst_cells,
	     "Burst cell size in number of cells");

#endif

NUMBER_FUNC	(ci_atm_pvc_avg, ci_atm_pvc_burst, no_alt, OBJ(int,6),
		 ci_atm_pvc_avg_range, "Average rate(Kbps)");

static void ci_atm_pvc_peak_range (parseinfo *csb, uint *lower, uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr &&
	csb->interface->hwptr->atm_db) {
	*lower = csb->interface->hwptr->atm_db->def_config.min_peak_rate;
	*upper = csb->interface->hwptr->atm_db->plimrate*1000;
    }
}

NUMBER_FUNC	(ci_atm_pvc_peak, ci_atm_pvc_avg, ci_atm_pvc_inarp_chk,
		 OBJ(int,5), ci_atm_pvc_peak_range, "Peak rate(Kbps)"); 
/*
 * Fetch the LINK type for a MUX VC 
 */

PROTONAME (ci_atm_mux_protocol, ci_atm_pvc_peak, no_alt, OBJ(int,10),
	   PMATCH_APOLLO PMATCH_APPLETALK PMATCH_DECNET PMATCH_IP
	   PMATCH_NOVELL PMATCH_VINES PMATCH_XNS);

KEYWORD_ID(ci_atm_pvc_qsaal, ci_atm_pvc_peak, no_alt,
	OBJ(int,4), ATM_VC_AALQSAAL, "qsaal",
	"Signalling AAL", PRIV_CONF);

NUMBER	(ci_atm_pvc_midhigh, ci_atm_pvc_peak, no_alt,
	OBJ(int,9), 0, 1023, "MID High value");

NUMBER	(ci_atm_pvc_midlow, ci_atm_pvc_midhigh, ci_atm_pvc_eol,
	OBJ(int,8), 0, 1023, "MID Low value");
	
KEYWORD_ID(ci_atm_pvc_aal34smds, ci_atm_pvc_midlow, ci_atm_pvc_qsaal,
	OBJ(int,4), ATM_VC_AAL34SMDS, "aal34smds", 
	"AAL3/4+SMDS Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_atm_pvc_aal5ilmi, ci_atm_pvc_peak, ci_atm_pvc_aal34smds,
	OBJ(int,4), ATM_VC_AAL5ILMI, "ilmi", 
	"ILMI", PRIV_CONF);

/*
 * RLFS: Wait to turn this stuff on.
 *
KEYWORD_ID(ci_atm_pvc_aal5frnlpid, ci_atm_pvc_peak, ci_atm_pvc_aal34smds,
	OBJ(int,4), ATM_VC_AAL5FRNLPID, "aal5frnlpid", 
	"AAL5+FR/NLPID Encapsulation", PRIV_CONF|PRIV_SUBIF);
 */

KEYWORD_ID(ci_atm_pvc_aal5nlpid, ci_atm_pvc_peak, ci_atm_pvc_aal5ilmi,
	OBJ(int,4), ATM_VC_AAL5NLPID, "aal5nlpid", 
	"AAL5+NLPID Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_atm_pvc_aal5mux, ci_atm_mux_protocol, ci_atm_pvc_aal5nlpid,
	OBJ(int,4), ATM_VC_AAL5MUX, "aal5mux", 
	"AAL5+MUX Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_atm_pvc_aal5snap, ci_atm_pvc_peak, ci_atm_pvc_aal5mux,
	OBJ(int,4), ATM_VC_AAL5SNAP, "aal5snap", 
	"AAL5+LLC/SNAP Encapsulation", PRIV_CONF|PRIV_SUBIF);

static void ci_atm_vci_num_range (parseinfo *csb, uint *lower, uint *upper)
{
    
    if (csb && csb->interface && csb->interface->hwptr &&
	csb->interface->hwptr->atm_db) {
	/*
	 * max vci bits contains 2 ^^  max vci value allowed.
	 */
	/* vpi of 0 vci of 0 not allowed */
	if (GETOBJ(int,2) == 0)
	{
	    *lower = 1;
	} else {
	    *lower = 0;
	}
	*upper = atm_calc_max_vpivci(csb->interface->
				 hwptr->atm_db->config.max_vci_bits);
  }
    
}


NUMBER_FUNC (ci_atm_pvc_vci, ci_atm_pvc_aal5snap, no_alt, OBJ(int,3),
	     ci_atm_vci_num_range,"VCI number");

/*
 * function that co-works w/ NUMBER_FUNC macro to retrieve the current
 * vpi range.
 */
static void ci_atm_vpi_num_range (parseinfo *csb, uint *lower, uint *upper)
{
    
    if (csb && csb->interface && csb->interface->hwptr &&
	csb->interface->hwptr->atm_db) {
	/*
	 * max vpi bits contains 2 ^^  max vpi value allowed.
	 */
	*lower = 0;
	*upper = atm_calc_max_vpivci(csb->interface->hwptr->
				 atm_db->config.max_vpi_bits);

  }
    
}



NUMBER_FUNC (ci_atm_pvc_vpi, ci_atm_pvc_vci, no_alt, OBJ(int,2),
	     ci_atm_vpi_num_range,"VPI number");


NOPREFIX	(ci_atm_pvc_noprefix, ci_atm_pvc_vpi, ci_atm_pvc_eol);

/*
 * function that co-works w/ NUMBER_FUNC macro to retrieve the current
 * range of the VCD.
 */
static void ci_atm_pvc_num_range (parseinfo *csb, uint *lower, uint *upper)
{
    
    if (csb && csb->interface && csb->interface->hwptr &&
	csb->interface->hwptr->atm_db) {
	/*
	 * AIP has VC table w/ VCD in the range of [0..config.max_intf_vcs-1].
         * However, since VCD=0 is reserved by the AIP, usr is only allowed 
	 * to use VCD's in the range of [1..config.max_intf_vcs-1]. For
	 * compatability the other interfaces also don't use vcd=0.
	 */
	*lower = 1;
	*upper = csb->interface->hwptr->atm_db->config.max_intf_vcs - 1;
  }
    
}

NUMBER_FUNC	(ci_atm_pvc_num, ci_atm_pvc_noprefix, no_alt, OBJ(int,1),
	 ci_atm_pvc_num_range, "VCD number");

NVGENS	(ci_atm_pvc_nvgen, ci_atm_pvc_num,
	atm_interface_commands, ATM_CMD_PVC);
KEYWORD	(ci_atm_pvc, ci_atm_pvc_nvgen, ALTERNATE,
	"pvc", "Create a PVC", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ATM rate-queue <Qnum> <Mbps>
 *
 * OBJ(int,1) = Qnum
 * OBJ(int,2) = Mbps
 */
EOLS    (ci_atm_rateq_eol, atm_interface_commands, ATM_CMD_RATEQ);

static void ci_atm_rateq_rate_range (parseinfo *csb, uint *lower,
                                     uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr &&
        csb->interface->hwptr->atm_db) {
        *lower = 1;
        *upper = csb->interface->hwptr->atm_db->plimrate;
    }
}
NUMBER_FUNC     (ci_atm_rateq_rate, ci_atm_rateq_eol, no_alt, OBJ(int,2),
                 ci_atm_rateq_rate_range, "Mbps");

NVGENS  (ci_atm_rate_nvgen, ci_atm_rateq_rate,
        atm_interface_commands, ATM_CMD_RATEQ);

static void ci_atm_rateq_no_range (parseinfo *csb, uint *lower,
				   uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr &&
	csb->interface->hwptr->atm_db) {
	*lower = 0;
	*upper = csb->interface->hwptr->atm_db->ratequeue_no - 1;
    }
}

NUMBER_FUNC (ci_atm_rateq_queue_no,ci_atm_rate_nvgen,no_alt,OBJ(int,1),
	     ci_atm_rateq_no_range,"RateQ Number");

NVGENS  (ci_atm_rateq_nvgen, ci_atm_rateq_queue_no,
        atm_interface_commands, ATM_CMD_RATEQ);

KEYWORD (ci_atm_rateq, ci_atm_rateq_nvgen, ci_atm_pvc,
        "rate-queue", "ATM Rate Queue", PRIV_CONF);

/***************************************************************
 * ATM [no] atm sig-traffic-shaping [strict]
 */
EOLS    (ci_atm_sig_strict_shaping_eol, atm_interface_commands,
         ATM_CMD_SIG_TRAF_SHAPE);
 
KEYWORD (ci_atm_sig_strict_shaping, ci_atm_sig_strict_shaping_eol, no_alt,
         "strict",
         "Traffic shaping must match signalled traffic", PRIV_CONF);
 
NOPREFIX (ci_atm_sig_traffic_shaping_noprefix, ci_atm_sig_strict_shaping,
         ci_atm_sig_strict_shaping_eol);
 
KEYWORD (ci_atm_sig_traffic_shaping, ci_atm_sig_traffic_shaping_noprefix, 
	 ci_atm_rateq,
         "sig-traffic-shaping",
         "Traffic shaping must match signalled traffic", PRIV_CONF);



/***************************************************************
 * ATM uni-version [3.0 | 3.1]
 * OBJ(int,1) = [3.0 | 3.1]
 *
 */
EOLS (ci_atm_uni_eol, atm_interface_commands, ATM_CMD_UNI_VERSION);
KEYWORD_ID(ci_atm_uni_31, ci_atm_uni_eol, no_alt,
           OBJ(int,1), ATMUNI_VERSION_31,
           "3.1", "UNI Version 3.1", PRIV_CONF);
KEYWORD_ID(ci_atm_uni_30, ci_atm_uni_eol, ci_atm_uni_31,
           OBJ(int,1), ATMUNI_VERSION_30,
           "3.0", "UNI Version 3.0", PRIV_CONF);
NOPREFIX (ci_atm_uni_noprefix,ci_atm_uni_30,ci_atm_uni_eol);
NVGENS (ci_atm_uni_nvgen, ci_atm_uni_noprefix, atm_interface_commands,
                                                 ATM_CMD_UNI_VERSION);
KEYWORD (ci_atm_uni, ci_atm_uni_nvgen, ci_atm_sig_traffic_shaping,
         "uni-version", "UNI Version", PRIV_CONF);



/*****************************************************************
 * [no] ATM class <name>
 */

EOLS    (ci_atm_class_eol, atm_interface_commands, ATM_CMD_CLASS);

STRING  (ci_atm_class_name, ci_atm_class_eol, no_alt,
         OBJ(string, 1), "ATM map class name");

NOPREFIX (ci_atm_class_noprefix, ci_atm_class_name, ci_atm_class_eol);

NVGENS  (ci_atm_class_nvgen, ci_atm_class_noprefix, atm_interface_commands,
         ATM_CMD_CLASS);

KEYWORD (ci_atm_class, ci_atm_class_nvgen, ci_atm_uni,
         "class", "Configure default map class name", PRIV_CONF);

/******************************************************************
 * ATM SONET [STS-3c | STM-1]
 * OBJ(int,1) = (1 == STM_1)
 */
EOLS	(ci_atm_sonet_eol, atm_interface_commands, ATM_CMD_SONET);

KEYWORD_ID(ci_atm_sonet_stm1, ci_atm_sonet_eol, no_alt, 
	OBJ(int,1), ATM_SONET_STM1, "stm-1", 
	"SONET in STM-1 mode", PRIV_CONF);

/*
 * Until we get more SONET options, stm-1 is the only option.
 * [NO] puts you back to STS-3C

KEYWORD_ID(ci_atm_sonet_sts3c, ci_atm_sonet_eol, ci_atm_sonet_stm1, 
	OBJ(int,1), ATM_SONET_STS3C, "sts-3c", 
	"SONET in STS-3C mode", PRIV_CONF);
 */

KEYWORD	(ci_atm_sonet, ci_atm_sonet_stm1, ci_atm_class,
	"sonet", "ATM SONET mode", PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	ci_atm_sonet
