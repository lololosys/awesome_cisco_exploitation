
atm_oam.h,1310
#define	AIP_DM_PKT_OAM_VC	38,1412
#define	AIP_DM_PKT_OAM_DM	39,1476
#define	ATM_OAM_FAULT_MGMT	55,1990
#define	ATM_OAM_AIS_FUNC	57,2050
#define	ATM_OAM_FERF_FUNC	58,2104
#define	ATM_OAM_LOOP_FUNC	59,2160
#define ATM_OAM_LOOPIND	60,2220
#define	ATM_OAM_F5_SEGMENT	62,2282
#define	ATM_OAM_F5_ENDTOEND	63,2337
#define	ATM_OAM_F4_SEGMENT	64,2396
#define	ATM_OAM_F4_ENDTOEND	65,2451
typedef struct	atm_oam_hdr_t_ atm_oam_hdr_t_67,2511
} atm_oam_hdr_t;atm_oam_hdr_t76,2864
typedef struct	atm_oam_type_func_t_ atm_oam_type_func_t_78,2882
} atm_oam_type_func_t;atm_oam_type_func_t82,2983
#define ATM_VCI_MS_SHFT	84,3007
#define ATM_VCI_LS_MASK	85,3035
#define ATM_GET_VCI_MS(ATM_GET_VCI_MS86,3067
#define ATM_GET_VCI_LS(ATM_GET_VCI_LS87,3121
#define ATM_CONSTRUCT_VCI(ATM_CONSTRUCT_VCI88,3173
#define	ATM_OAM_CELL_SIZE	91,3263
#define ATM_CELLHDR_HEC_BYTES	92,3292
#define	ATM_CELL_ENCAPBYTES	93,3324
#define	ATM_OAM_ENCAPBYTES	94,3381
#define	ATM_OAM_FILL	96,3435
#define	ATM_OAM_FILLBYTES	97,3491
#define	ATM_OAM_UNUSEDBYTES	99,3549
#define	ATM_AIS_FAIL	104,3667
typedef	struct	atm_ais_ferf_func_t_ atm_ais_ferf_func_t_105,3694
} atm_ais_ferf_t;atm_ais_ferf_t108,3820
typedef struct	atm_loopback_func_t_ atm_loopback_func_t_111,3840
} atm_loopback_t;atm_loopback_t117,4111
