/* $Id: cfg_int_atm_cbus.h,v 3.2.60.2 1996/04/25 23:11:57 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/hes/cfg_int_atm_cbus.h,v $
 *------------------------------------------------------------------
 * AIP platform specific ATM configuration commands.
 *
 * February, 1995  Walter Friedrich
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_atm_cbus.h,v $
 * Revision 3.2.60.2  1996/04/25  23:11:57  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.60.1.2.1  1996/03/30  03:04:34  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands
 * Branch: ATM_Cal_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:39:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:21:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  14:18:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:15:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/19  07:12:16  awu
 * CSCdi36778:  <show atm vc nn> rejects valid vcd numbers.
 *
 * Revision 2.1  1995/07/02  10:51:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.4  1995/06/27  00:30:32  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.3  1995/06/21  18:16:33  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/11  03:42:41  awu
 * CSCdi34590:  router incorrectly configs extra rate-queue at boot time.
 *
 * Revision 2.1  1995/06/07  22:03:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * ATM ds3-scramble
 *
 */
EOLS	(ci_atm_ds3scramble_eol, atm_interface_commands, ATM_CMD_DS3SCRAMBLE);
KEYWORD	(ci_atm_ds3scramble, ci_atm_ds3scramble_eol, ALTERNATE,
	"ds3-scramble", "Enable DS3 Scrambling", PRIV_CONF);

/******************************************************************
 * ATM maxvc <n>
 *
 * OBJ(int,1) = Maximum # of VCs to support
 */
static keyword_options max_vcs_options[]= {
  { "512","Max VCs",512 },
  { "1024",NULL,1024 },
  { "2048","",2048 },
  { NULL,NULL,0 }
};

EOLS	(ci_atm_maxvc_eol, atm_aip_interface_commands, ATM_CMD_VC);

KEYWORD_OPTIONS(ci_atm_maxvc_num,ci_atm_maxvc_eol,no_alt,
		max_vcs_options,OBJ(int,1),PRIV_CONF,0);

NVGENS	(ci_atm_maxvc_nvgen, ci_atm_maxvc_num, 
	atm_aip_interface_commands, ATM_CMD_VC);
KEYWORD	(ci_atm_maxvc, ci_atm_maxvc_nvgen, ci_atm_ds3scramble,
	"maxvc", "Maximum number of VCs", PRIV_CONF);

/******************************************************************
 * ATM mid_per_vc <n>
 *
 * OBJ(int,1) = MIDs per VC
 */
static keyword_options max_mid_vc_options[]= {
  { "16"," MIDs per VC",16 },
  { "32",NULL,32 },
  { "64",NULL,64 },
  { "128",NULL,128 },
  { "256",NULL,256 },
  { "512",NULL,512 },
  { "1024","",1024 },
  { NULL,NULL,0 }
};

EOLS	(ci_atm_mid_vc_eol, atm_aip_interface_commands, ATM_CMD_MID_PER_VC);

KEYWORD_OPTIONS(ci_atm_mid_vc_num,ci_atm_mid_vc_eol,no_alt,
		max_mid_vc_options,OBJ(int,1),PRIV_CONF,0);

NVGENS	(ci_atm_mid_vc_nvgen, ci_atm_mid_vc_num, 
	atm_aip_interface_commands, ATM_CMD_MID_PER_VC);
KEYWORD	(ci_atm_mid_vc, ci_atm_mid_vc_nvgen, ci_atm_maxvc,
	"mid-per-vc", "Maximum MIDs per VC", PRIV_CONF);

/******************************************************************
 * ATM vc_per_vp <n>
 *
 * OBJ(int,1) = VCIs per VPIs
 */
static keyword_options vcs_per_vp_options[]= {
  { "16"," VCs per VP",16 },
  { "32",NULL,32 },
  { "64",NULL,64 },
  { "128",NULL,128 },
  { "256",NULL,256 },
  { "512",NULL,512 },
  { "1024","",1024 },
  { NULL,NULL,0 }
};

EOLS    (ci_atm_vc_vp_eol, atm_interface_commands, ATM_CMD_VC_PER_VP);
 
KEYWORD_OPTIONS(ci_atm_vc_vp_num,ci_atm_vc_vp_eol,no_alt,
		vcs_per_vp_options,OBJ(int,1),PRIV_CONF,0);

NVGENS  (ci_atm_vc_vp_nvgen, ci_atm_vc_vp_num,
        atm_interface_commands, ATM_CMD_VC_PER_VP);
KEYWORD (ci_atm_vc_vp, ci_atm_vc_vp_nvgen, ci_atm_mid_vc,
        "vc-per-vp", "ATM VCIs per VPI", PRIV_CONF);

/******************************************************************
 * ATM rawq-size
 *
 * OBJ(int,1) = RawQ Size
 */
EOLS	(ci_atm_rawq_eol, atm_aip_interface_commands, ATM_CMD_RAWQ);

NUMBER	(ci_atm_rawq_num, ci_atm_rawq_eol, no_alt, OBJ(int,1),
	8, 1024, "RawQueue size");

NVGENS	(ci_atm_rawq_nvgen, ci_atm_rawq_num, 
	atm_aip_interface_commands, ATM_CMD_RAWQ);

KEYWORD	(ci_atm_rawq, ci_atm_rawq_nvgen, ci_atm_vc_vp,
	"rawq-size", "ATM Raw Cell Queue size", PRIV_CONF);

/******************************************************************
 * ATM vp-filter 
 *
 * OBJ(int,1) = VP Filter value(HEX)
 */
EOLS	(ci_atm_vpf_eol, atm_aip_interface_commands, ATM_CMD_VPFILTER);

HEXDIGIT	(ci_atm_vpf_num, ci_atm_vpf_eol, no_alt, OBJ(int,1),
		 ATM_VP_FILTER_MIN, ATM_VP_FILTER_MAX, "VP Filter value(Hex)");

NVGENS	(ci_atm_vpf_nvgen, ci_atm_vpf_num, 
	atm_aip_interface_commands, ATM_CMD_VPFILTER);
KEYWORD	(ci_atm_vpf, ci_atm_vpf_nvgen, ci_atm_rawq, 
	"vp-filter", "ATM Virtual Path Filter", PRIV_CONF);


/******************************************************************
 * ATM txbuff <num>
 *
 * OBJ(int,1) = TX buffers
 */
EOLS	(ci_atm_txbuff_eol, atm_aip_interface_commands, ATM_CMD_TXBUFF);

NUMBER	(ci_atm_tx_num, ci_atm_txbuff_eol, no_alt, OBJ(int,1), 0, 512, 
	"Transmit buffers");

NVGENS	(ci_atm_txbuff_nvgen, ci_atm_tx_num,
	atm_aip_interface_commands, ATM_CMD_TXBUFF);
KEYWORD	(ci_atm_txbuff, ci_atm_txbuff_nvgen, ci_atm_vpf,
	"txbuff", "ATM Transmit buffers", PRIV_CONF);

/******************************************************************
 * ATM rxbuff <num>
 *
 * OBJ(int,1) = RX buffers
 */
EOLS	(ci_atm_rxbuff_eol, atm_aip_interface_commands, ATM_CMD_RXBUFF);

NUMBER	(ci_atm_rx_num, ci_atm_rxbuff_eol, no_alt, OBJ(int,1), 0, 512, 
	"Receive buffers");

NVGENS	(ci_atm_rxbuff_nvgen, ci_atm_rx_num,
	atm_aip_interface_commands, ATM_CMD_RXBUFF);
KEYWORD	(ci_atm_rxbuff, ci_atm_rxbuff_nvgen, ci_atm_txbuff,
	"rxbuff", "ATM Receive buffers", PRIV_CONF);
 
/******************************************************************
 * ATM exception-queue <num>
 *
 * OBJ(int,1) = exception queue buffers
 */
EOLS	(ci_atm_excq_eol, atm_aip_interface_commands, ATM_CMD_EXCQ);

NUMBER	(ci_atm_excq_num, ci_atm_excq_eol, no_alt, OBJ(int,1), 8, 256, 
	"Exception-Queue buffers");

NVGENS	(ci_atm_excq_nvgen, ci_atm_excq_num,
	atm_aip_interface_commands, ATM_CMD_EXCQ);
KEYWORD	(ci_atm_excq, ci_atm_excq_nvgen, ci_atm_rxbuff,
	"exception-queue", "ATM Exception-Queue buffers", PRIV_CONF);

/******************************************************************
 * ATM framing [g832adm]
 * OBJ(int,1) = Framing value
 */
EOLS    (ci_atm_e3framing_eol, atm_interface_commands, ATM_CMD_E3FRAMING);
 
KEYWORD_ID(ci_atm_e3framing_g832adm, ci_atm_e3framing_eol, no_alt,
	OBJ(int,1), ATM_FRAMING_E3G832ADM, "g832adm",
	"E3 Framing G.832 ADM", PRIV_CONF);
 
KEYWORD (ci_atm_e3framing_kwd, ci_atm_e3framing_g832adm, no_alt,
	"framing", "ATM Framing style", PRIV_CONF);

ASSERT  (ci_atm_e3framing, ci_atm_e3framing_kwd, ci_atm_excq,
	is_atm_e3(csb->interface->hwptr));

/******************************************************************
 * ATM aal <aal34> or <aal5>
 *
 * OBJ(int,1) = (0 == AAL5) or (1 == AAL3/4)
 *
 * Note: aal3/4 is required for smds, so this part needs to
 * be here and needs to be ahead of the "smds-address" cmd.
 */

EOLS	(ci_atm_aal_eol, atm_aip_interface_commands, ATM_CMD_AAL);

KEYWORD_ID(ci_atm_aal34, ci_atm_aal_eol, no_alt, 
	OBJ(int,1), 1, "aal3/4", "AAL3/4 encapsulation", PRIV_CONF);

KEYWORD_ID(ci_atm_aal5, ci_atm_aal_eol, ci_atm_aal34,
	OBJ(int,1), 0, "aal5", "AAL5 encapsulation", PRIV_CONF);

KEYWORD	(ci_atm_aal, ci_atm_aal5, ci_atm_e3framing,
	"aal", "ATM AAL encapsulation", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_atm_aal

