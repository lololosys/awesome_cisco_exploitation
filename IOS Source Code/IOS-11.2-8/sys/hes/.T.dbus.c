
dbus.c,4583
#define C7500_CHASSIS_INTERFACE_SLOT	383,14115
#define CIP2_MIN_HWVER 385,14156
dbus_write 392,14301
dbus_write_cbus_disabled 401,14420
void dbus_prot_wr 418,14851
slottype* get_slotp 422,14963
inline dgtype* pslot_to_dbus 427,15026
int dbus_to_pslot 433,15120
dbus_read_reg 443,15304
dbus_write_reg 452,15407
#define DBUS_TIMEOUT 461,15555
#define DBUS_RESET_CMDFAIL 462,15592
#define DBUS_MAX_RETRY_ATTEMPTS 463,15629
dbus_timeout_recovery 465,15681
dbus_read_reg_recovery 487,16123
dbus_write_reg_recovery 526,16961
dbus_io_reg_read 555,17560
dbus_io_reg_write 586,18075
dbus_io_status_reg_read 614,18557
dbus_io_status_reg_write 627,18793
dbus_timeout_recovery_capable 639,19024
dbus_io_status_bit 650,19186
void dbus_restore_io_status_bits 682,19821
static inline void dbus_enabled_led_on 699,20346
dgtype* dbus_rpslot_to_dbus 712,20623
inline boolean dbus_parallel_inline 722,20834
static inline boolean dbus_swdbus_inline 737,21079
boolean dbus_wait_for_sw_ready 760,21694
#define DBUS_NO_RESPONSE_VAL 799,22588
#define DBUS_NO_RESPONSE_TIMEOUT 800,22622
static ushort get_dbus_controller_type 812,23201
read_dbus_controller_type 836,24093
static inline boolean dbus_io_regs 873,25099
void slot_enabled_led_on 893,25495
static inline uchar dbus_slot_enable_start 904,25669
inline void dbus_slot_enable_finish 924,26122
inline uchar dbus_slot_enable 947,26760
static inline void dbus_set_loaded 956,26948
void dbus_xilinx_reload 969,27193
static boolean dbus_wcs_loaded 1022,28635
static inline ushort dbus_soft_reset_start 1032,28916
static inline int dbus_compute_soft_reset_delay 1054,29557
static inline void dbus_soft_reset_finish 1065,29800
inline void dbus_ctrlr_reset 1078,30075
static inline ushort dbus_hard_reset_start 1092,30403
static inline int dbus_compute_hard_reset_delay 1120,31278
static inline void dbus_hard_reset_finish 1138,31831
inline void dbus_hard_ctrlr_reset 1146,32046
void dbus_slot_reset 1159,32339
static uchar dbus_ctrlr_start_common 1170,32509
uchar dbus_ctrlr_start 1190,33064
uchar dbus_ctrlr_start_nowaited 1198,33285
void dbus_ctrlr_start_nowaited_finish 1206,33532
boolean slot_enabled 1211,33654
boolean cxbus_card_present 1223,33877
boolean cxbus_ipc_capable 1257,34659
boolean rsp_present 1275,34975
inline boolean switchproc_present 1296,35392
inline boolean swbus_present 1314,35855
boolean download_installed_microcode 1323,36051
ushort dbus_hw_version 1368,37572
ushort dbus_hw_revision 1374,37726
ushort dbus_controller_type 1387,38077
boolean srswitch_pslot 1428,39083
boolean ssip_smip_pslot 1444,39403
boolean is_cip2_hw 1455,39658
ulong dbus_serial_number 1470,39927
static boolean verify_and_get_card_type 1483,40297
char* card_type_to_str 1502,40844
dbus_reset_recovery_counters 1547,42357
dbus_reset_slot_vectors 1561,42614
dbus_set_slot_vectors 1578,42977
verify_driver_support 1595,43362
boolean dbus_validate 1621,43843
void mark_removed_cards 1718,46446
static void dbus_hard_reset_all_controllers 1743,46987
void dbus_soft_reset_all_specific_controllers 1814,49058
void dgbus_analyze 1893,51396
chassis_type_t which_7000_series 2038,55915
static void slot_init 2043,55991
void disable_slot 2071,56851
dbus_reset_IP_slots 2133,59038
dbus_init_slot 2203,61006
dbus_startup 2219,61330
dbus_print_recovery_data 2230,61494
static void print_dbus_card 2247,61913
void show_diagbus 2650,74138
static boolean aip_download 2700,75759
static boolean ctrlr_needs_flowcntrl 2768,77506
#define MAX_FLOW_ATTEMPTS 2787,77908
static boolean ctrlr_caught_breath 2788,77937
boolean download_microcode_from_ram 2831,78738
#define DBUS_MAX_BYTES_PER_DL_PASS 2969,83378
boolean download_microcode_chunk_from_ram 3070,86461
boolean dbus_slot_enabled 3268,92217
void dbus_mci_internal_clock_enable 3283,92586
void dbus_mci_internal_clock_disable 3290,92756
void dbus_mci_nrzi_enable 3297,92929
void dbus_mci_nrzi_disable 3304,93061
void dbus_mci_invert_txc_enable 3310,93194
void dbus_mci_invert_txc_disable 3317,93336
void dbus_write_fip_leds 3332,93716
boolean board_has_instr_error 3356,94465
void dbus_error_scan 3370,94756
boolean ctrlr_7500_compat 3442,96306
static void dbus_check_crash 3531,98542
boolean dbus_check_msg 3557,99038
void dbus_logger_drain 3664,101675
#define SLOT_MAJVERSION 3689,102150
#define SLOT_MINVERSION 3690,102176
#define SLOT_EDITVERSION 3691,102202
static void dbus_init 3700,102371
#define DBUS_MAJVERSION 3721,102777
#define DBUS_MINVERSION 3722,102803
#define DBUS_EDITVERSION 3723,102829
