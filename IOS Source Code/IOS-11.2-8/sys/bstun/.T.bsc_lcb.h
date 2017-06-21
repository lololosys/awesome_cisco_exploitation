
bsc_lcb.h,1384
#define __LCB_H97,3463
#define MAX_ADDRS 107,3606
#define MAX_TIMER_TYPES 108,3637
#define MAX_LCB_RETRY_CTRS 109,3668
#define MAX_LCB_FRAME_CTRS 110,3699
#define MAX_CUCB_RETRY_CTRS 111,3730
#define MAX_CUCB_FRAME_CTRS 112,3761
typedef struct cucb_t cucb_t;cucb_t116,3832
        } char_set_e;char_set_e124,3984
        } cu_state_e;cu_state_e128,4067
        } cu_reqstate_e;cu_reqstate_e132,4176
        } timer_type_e;timer_type_e136,4280
        } line_state_e;line_state_e140,4343
        } poll_role_e;poll_role_e144,4465
        } queue_origin_e;queue_origin_e148,4531
        } wack_status_e;wack_status_e152,4631
        } cucb_list_t;cucb_list_t158,4737
        } list_t;list_t164,4851
        } lcb_retry_ctrs_t;lcb_retry_ctrs_t168,4926
        } lcb_frame_ctrs_t;lcb_frame_ctrs_t172,5011
        } cucb_retry_ctrs_t;cucb_retry_ctrs_t176,5097
        } cucb_frame_ctrs_t;cucb_frame_ctrs_t180,5184
        } timer_table_t;timer_table_t184,5269
typedef void (*bcc_update_fn)bcc_update_fn186,5295
typedef void (*bcc_update_fn)( ushort, ushort*ushort186,5295
typedef void (*fsm_exec_t)fsm_exec_t188,5346
typedef void (*fsm_exec_t)(lcb_t* lcb, int event)event188,5346
struct cucb_t cucb_t192,5430
struct lcb_t_ lcb_t_217,7039
static inline uchar bsc_map_sel_to_pol 291,11425
static inline uchar bsc_map_pol_to_sel 296,11531
