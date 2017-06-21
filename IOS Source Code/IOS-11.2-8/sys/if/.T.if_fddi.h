
if_fddi.h,3450
#define __IF_FDDI_H__53,1777
#define MAX_RTTIMER_COUNT	59,1977
#define get_pcm_state_string(get_pcm_state_string65,2180
#define get_ecm_state_string(get_ecm_state_string66,2249
#define get_ecm_sig_string(get_ecm_sig_string67,2318
#define get_cfm_state_string(get_cfm_state_string68,2386
#define get_rmt_state_string(get_rmt_state_string69,2455
#define get_phy_type_string(get_phy_type_string70,2524
#define get_mac_type_string(get_mac_type_string71,2592
#define get_mac_intr_string(get_mac_intr_string72,2660
#define	fddi_cbuginf	78,2918
#define	bprintf	83,3125
#define DBG_SYS_TMR	85,3156
#define DBG_PHY_IND	86,3197
#define DBG_PHY_REQ	87,3238
#define DBG_PCM_STATE	88,3279
#define DBG_PCM_TOUT	89,3322
#define DBG_PCM_TVAL	90,3365
#define DBG_PCM_RVAL	91,3410
#define DBG_PCM_SIG	92,3455
#define DBG_PCM_NOISE	93,3496
#define DBG_PCM_OTHER	94,3540
#define DBG_LEM_FAIL	95,3585
#define DBG_CFM_STATE	96,3628
#define DBG_CEM_OTHER	97,3667
#define DBG_CEM_SIG	98,3712
#define DBG_RMT_STATE	99,3753
#define DBG_RMT_TOUT	100,3796
#define DBG_RMT_OTHER	101,3839
#define DBG_MAC_INTR	102,3884
#define DBG_ECM_STATE	103,3927
#define DBG_ECM_SIG	104,3967
#define DBG_ECM_OTHER	105,4005
#define fddi_cbug_sys_tmr(fddi_cbug_sys_tmr107,4049
#define fddi_cbug_phy_ind(fddi_cbug_phy_ind110,4147
#define fddi_cbug_phy_req(fddi_cbug_phy_req115,4300
#define fddi_cbug_pcm_state(fddi_cbug_pcm_state120,4453
#define fddi_cbug_pcm_tout(fddi_cbug_pcm_tout125,4617
#define fddi_cbug_pcm_noise(fddi_cbug_pcm_noise129,4741
#define fddi_cbug_pcm_tval(fddi_cbug_pcm_tval133,4867
#define fddi_cbug_pcm_rval(fddi_cbug_pcm_rval139,5028
#define fddi_cbug_pcm_signal(fddi_cbug_pcm_signal145,5189
#define fddi_cbug_pcm_other(fddi_cbug_pcm_other150,5353
#define fddi_cbug_lem_fail(fddi_cbug_lem_fail155,5505
#define fddi_cbug_ecm_state(fddi_cbug_ecm_state159,5629
#define fddi_cbug_ecm_signal(fddi_cbug_ecm_signal163,5756
#define fddi_cbug_ecm_other(fddi_cbug_ecm_other167,5885
#define fddi_cbug_cfm_state(fddi_cbug_cfm_state171,5998
#define fddi_cbug_cem_other(fddi_cbug_cem_other175,6123
#define fddi_cbug_rmt_state(fddi_cbug_rmt_state180,6275
#define fddi_cbug_mac_intr(fddi_cbug_mac_intr185,6439
#define fddi_cbug_rmt_tout(fddi_cbug_rmt_tout190,6578
#define fddi_cbug_rmt_other(fddi_cbug_rmt_other194,6702
#define A_INDEX 208,7075
#define B_INDEX 209,7112
#define S_INDEX 210,7149
#define PRIM_MAC_INDEX 211,7186
} SMTCfgType, *SMTCfgPtr;SMTCfgPtr235,7971
} MACCfgType, *MACCfgPtr;MACCfgPtr258,8873
} PATHRESCfgType, *PATHRESCfgPtr;PATHRESCfgPtr274,9375
} PATHCfgType, *PATHCfgPtr;PATHCfgPtr293,9949
} PORTCfgType, *PORTCfgPtr;PORTCfgPtr318,10804
#define EC_STATE_MAXID	333,11147
#define PC_STATE_MAXID	334,11174
#define CF_STATE_MAXID	335,11201
#define RM_STATE_MAXID	336,11229
#define PC_SIGNAL_MAXID	337,11256
#define EC_SIGNAL_MAXID	338,11284
#define PHY_TYPE_MAXID	339,11312
#define MAC_TYPE_MAXID	340,11339
struct SmtMsg SmtMsg345,11398
} SMTMsgType;SMTMsgType357,11946
} StnDCBType, *StnDCBPtr;StnDCBPtr384,12535
#define STADATA(STADATA385,12562
#define STAINST(STAINST386,12613
#define SMTCFG(SMTCFG387,12657
#define MACCFG(MACCFG388,12715
#define PATHCFG(PATHCFG389,12773
#define PATHRESCFG(PATHRESCFG390,12833
#define PORTCFG(PORTCFG391,12923
#define HWIDBDATA(HWIDBDATA392,12983
#define	FIND_FDDI_STATION_ID(FIND_FDDI_STATION_ID432,14353
