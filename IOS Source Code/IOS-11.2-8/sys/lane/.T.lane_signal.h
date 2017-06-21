
lane_signal.h,1333
#define _LANE_SIGNAL_H_106,3731
#define LANE_NO_NSAP 110,3800
#define LANE_EMPTY_NSAP 112,3897
#define LANE_ALL_ONES_ADDR 115,4016
#define LANE_ALL_ONES_NSAP 118,4180
#define LANE_LECS_FIXED_NSAP 121,4327
typedef enum lane_blli_t_ lane_blli_t_134,4652
} lane_blli_t;lane_blli_t141,4827
#define LANE_BLLI_MIN_PID 146,4901
#define LANE_BLLI_MAX_PID 147,4951
typedef enum lane_callback_code_t_ lane_callback_code_t_154,5045
} lane_callback_code_t;lane_callback_code_t167,5432
typedef enum lane_vcc_state_t_ lane_vcc_state_t_173,5491
} lane_vcc_state_t;lane_vcc_state_t186,5824
typedef enum lane_signal_return_t_ lane_signal_return_t_192,5896
} lane_signal_return_t;lane_signal_return_t196,6105
typedef struct lane_sig_register_t_ lane_sig_register_t_204,6230
} lane_sig_register_t;lane_sig_register_t208,6431
typedef struct lane_vcc_info_t_ lane_vcc_info_t_215,6516
} lane_vcc_info_t;lane_vcc_info_t241,7660
#define LANE_VCC_INFO_EMPTY 245,7719
typedef struct lane_signal_mcgroup_t_ lane_signal_mcgroup_t_255,8021
} lane_signal_mcgroup_t;lane_signal_mcgroup_t267,8591
#define LANE_MCGROUP_EMPTY 271,8662
#define FOR_ALL_LANE_SIG_VCCS(FOR_ALL_LANE_SIG_VCCS277,8875
#define FOR_ALL_LANE_MCGROUP_VCCS(FOR_ALL_LANE_MCGROUP_VCCS280,8981
#define FOR_ALL_LANE_SIG_REGS(FOR_ALL_LANE_SIG_REGS283,9088
