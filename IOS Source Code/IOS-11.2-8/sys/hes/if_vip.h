/* $Id: if_vip.h,v 3.8.6.8 1996/08/20 00:35:02 cheilek Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_vip.h,v $
 *------------------------------------------------------------------
 * if_vip.h:  VIP specific constants & definitions
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_vip.h,v $
 * Revision 3.8.6.8  1996/08/20  00:35:02  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.8.6.7  1996/07/10  02:37:47  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.8.6.6  1996/06/10  02:00:59  fsunaval
 * CSCdi57738:  VIP2 SW: FDDI PA doesnt show claim, beacon counter at RSP
 * level.
 * Branch: California_branch
 * 1.Fix the claim, beacon and trace counters.
 * 2.Make the MIB in the RSP aware about single/multimode interface.
 * 3.Remove excess debugging stuff.
 *
 * Revision 3.8.6.5  1996/05/08  04:31:57  tboyle
 * CSCdi54363:  SW-VIP2: Dropped packets are unaccountable in FDDI-PA.
 * Branch: California_branch
 *
 * Revision 3.8.6.4  1996/05/06  19:27:22  fsunaval
 * CSCdi56768:  Update trt_neg
 * Branch: California_branch
 * 1.Pass one more parameter throught the MIB loveletter.
 *
 * Revision 3.8.6.3  1996/04/30  07:00:47  fsunaval
 * CSCdi51250:  SW-VIP1: Host MIB updating needs to be completed
 * Branch: California_branch
 * 1.Code changes for formation and transfer of SMT Messages to RSP
 *   in form of love letters.
 *
 * Revision 3.8.6.2  1996/03/21  22:40:51  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.8.6.1  1996/03/18  19:42:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.6  1996/03/18  23:19:58  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.5  1996/02/08  07:22:24  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.4  1996/01/25  12:09:46  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.2.2.3  1996/01/15  10:20:01  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.2  1995/12/23  04:39:59  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.2.2.1  1995/12/11  18:47:56  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.8  1996/03/06  01:37:49  jwelder
 * Vip hardware Love Letter added to pass vip Dram Size
 * CSCdi50800:  Show Diagbus changed to show VIP1 Dram Size
 *
 * Revision 3.7  1996/02/08  22:37:45  sthormod
 * CSCdi48062:  address info does not appear int  show cont tok x/y/z
 * The patch for CSCdi44907 fixed FEIP loveletters, but broke them for
 * VIP/4R.   Increase VIP_MAX_LV_LTR to handle larger 4R loveletters.
 *
 * Revision 3.6  1996/01/23  21:15:45  dlobete
 * CSCdi44907:  FEIP Interface counters are in error and mean-ing-less
 * The love-letter affair is over.
 *
 * Revision 3.5.2.1  1996/01/24  21:47:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/20  00:10:10  dlobete
 * CSCdi44907:  FEIP Interface counters are in error and mean-ing-less
 * Love-letter code path shared by both VIP and FEIP was only
 * working for the VIP.
 *
 * Revision 3.4  1995/12/26  23:08:23  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.3  1995/12/02  05:50:14  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.2  1995/11/17  09:19:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  19:18:22  getchell
 * Placeholders for VIP development
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Constants used for Crash traceback mechanism
 */

#define CRASH_TRACEBACK_COUNT   10
#define DBUS_CRASH_LEN (312+(CRASH_TRACEBACK_COUNT*sizeof(ulong)))

/*
 * VIP error statistics love letter formats.
 * 
 * Note : This data structure is common between RP/RSP code
 * and IOS running on a VIP card. However, it is also a superset
 * of the data structure used by the FEIP micro-code which does
 * not get built out of the same source repository. So please ensure
 * that it always remains a perfect superset of the feip data structure.
 */

/* 
 * VIP General loveletter types
 */
#define VIP_LL_STATS        1
#define VIP_LL_EEPROM	    2
#define VIP_LL_HW           3
#define VIP_LL_SW           4


/*
 * 4T love letter types
 */
#define VIP_LL_4T_STATUS	20

/* 
 * 4R love letter types
 */
#define VIP_LL_4R_ERR_MSG      100
   #define VIP_4R_INIT_FAIL      1
   #define VIP_4R_OPEN           2
   #define VIP_4R_CFG_BRG_FAIL   3 
   #define VIP_4R_ADP_CHK        4
   #define VIP_4R_LAN_STATUS     5
#define VIP_LL_4R_SMT_STATS    101
#define VIP_LL_4R_ERROR_STATS  102

#define VIP_LL_1F_MIBS         200
#define VIP_LL_1F_LUVL         210

/*
 * VIP 1F essential MIB info 
 */
typedef struct {
    ulong            ll_type;
    ulong            fddi_trt_neg;
    ulong            multimode;
    ushort           ECMState;
    ushort           CF_State;
    ushort           RMTState;
    ushort           NeighborType[2];
    ushort           PMDClass[2];
    ushort           ConnectState[2];
    ushort           PCMState[2];
    ushort           PC_LS[2];
    uchar            Ler_Estimate[2];
    boolean          SM_MA_Avail;
} vip_1f_mib_info;

/*
 * offset to look for loveletter encap per media
 */
#define VIP_LL_ETHER_OFFSET   6
#define VIP_LL_TOKEN_OFFSET   10
#define VIP_LL_SERIAL_OFFSET  1

/*
 * VIP MIB errors
 */
typedef struct {
    ulong output_error;
    ulong output_babbles;
    ulong output_lost_carrier;
    ulong feip_no_carrier;
    ulong output_late_collision;
    ulong output_excessive_collision;
    ulong output_collision;
    ulong output_underrun;
    ulong output_deferred;
    ulong input_error;
    ulong runts;
    ulong feip_multicast;
    ulong giants;
    ulong feip_watchdog;
    ulong input_frame;
    ulong input_crc;
    ulong input_overrun;
    ulong input_resource;
    ulong input_abort;
    ulong input_drops;

    /*
     * Add other media specific errors here for non-ethernet
     */

    /*
     * 4T-specific fields.
     */
    ulong s4t_sig_status;
    ulong s4t_pa_sw_version;
    ulong s4t_pa_hw_version;
    ulong s4t_pa_is_dead_jim;
    ulong s4t_applique_id;

    /*
     * 1F-specific fields.
     */
     ulong fddi_claims;
     ulong fddi_beacons;
     ulong fddi_trace_count;

} vip_ll_errors;

/*
 * Love-letter template for VIP-style love-letters.
 */
typedef struct {
    ulong ll_type;
    vip_ll_errors data;
} vip_ll;

/*
 * Love-letter to pass up IDE EEPROM info for PAs.
 */
typedef struct {
    ulong ll_type;
    ulong eeprom_version;
    ulong controller_type;
    ulong hardware_version;
    ulong serial_number;
    ulong part_number;
    ulong test_history;
    ulong rma_number;
    ulong board_revision;
    ulong macaddr;
    ulong flags;
} vip_ll_eeprom;

/*
 * Love-letter to pass up VIP hardware Information Such
 * as Memory size.
 */
typedef struct {
    ulong ll_type;
    ulong mem_size;
    ulong io_size;
} vip_ll_hw;

/*
 * Love-letter to pass up VIP software Information Such
 * as the IOS identification string.
 */

#define VIP_IOS_VER_MAX_STRLEN 100

typedef struct {
    ulong ll_type;
    uchar ios_version[VIP_IOS_VER_MAX_STRLEN];
} vip_ll_sw;

/*
 * Structure for storing selected PA EEPROM information.
 */
typedef struct {
    ulong eeprom_version;
    ulong controller_type;
    ulong hardware_version;
    ulong serial_number;
    ulong part_number;
    ulong board_revision;
    ulong software_version;
    ulong pa_type;		/* i.e. FCI_TYPE_VIP_xxx */
    ulong pa_ifcount;
    void  *private_pa_info;
} pa_info_slots;

#define VIP_DISABLE_TIME     5	    /* Time for system software to
				       wait before issuing new commands
				       after a disable */

/*
 * This value is checked by the vip kernel when running on an RSP.
 * Currently the largest loveletter is "vip_4r_smt_ll".  Anyone
 * adding/changing a vip loveletter should verify that their structure
 * is less than VIP_MAX_LV_LTR
 */
#define VIP_MAX_LV_LTR       1500 /* vrk 116 */
                                    /* Size returned when a global
                                       pointers command is issued. */

/*
 * Prototypes
 */
extern void 	vip_parse_love_letter(hwidbtype *, uchar *, ulong);
extern boolean 	vip_sniff_encap(mcitype *, hwidbtype *);
extern boolean  vip_controller(ushort);
extern void 	cbus_initialize_pa_info(hwidbtype *hwidb, mcitype *mci);
extern void 	cbus_priv_pa_info_op(hwidbtype *hwidb, boolean do_init,
				     int slot);
extern void     vip_parse_eeprom_info(hwidbtype *, uchar *);
extern void	vip_parse_hw_info(hwidbtype *, uchar *);
extern void	vip_parse_sw_info(hwidbtype *, uchar *);
extern void     vip_print_context(char *, void *);

/* end of file */
