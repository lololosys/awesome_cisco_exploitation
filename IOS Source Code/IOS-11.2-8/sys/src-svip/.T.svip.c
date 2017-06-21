
svip.c,1448
int flush_io_wbuffer 202,7007
void turn_on_pa_led 212,7194
void turn_off_pa_led 227,7515
boolean pas_pa_present 244,7891
void pas_control_power 263,8357
need_turbo_setting(269,8432
void pas_control_reset 301,9095
void pas_deactivate_bay 344,9885
void pas_nmi_dispatcher 361,10391
void vip_stop_microcode 389,10927
void test_parity_io 412,11415
typedef struct mgmt_vector_t mgmt_vector_t418,11494
} mgmt_vector;mgmt_vector421,11563
struct critical_mgmt_handler_list_t critical_mgmt_handler_list_t423,11579
boolean add_critical_mgmt_handler(432,11790
boolean del_critical_mgmt_handler(464,12623
void dispatch_critical_mgmt_handlers(503,13653
void recover_from_cya_hang 541,14690
static void svip_cbus_stall_handler 564,15432
static inline void throttle_netio 661,18652
static inline void throttle_off_inline 687,19127
void netint_throttle_off 699,19397
static void process_throttle_off 709,19594
void svip_netint_throttle_init 724,19849
void svip_netint_throttle_setup 750,20492
boolean svip_netint_throttle_check 770,20955
void svip_sched_loop_end 799,21557
void sched_alloc_command 812,21808
void vip_netint_throttle_show 861,23127
void install_cbus_stall_handler 877,23714
void install_cbus_handler 908,24441
static void svip_error_interrupt_handler 929,24932
static void svip_read_bus_error_handler 1173,31936
void install_svip_error_handler 1195,32390
get_pa_conf_type(1233,33330
is_channelized(1287,34650
