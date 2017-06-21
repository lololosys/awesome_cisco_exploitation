
c7100_ralib.c,2642
#define strncpy 94,3178
#define WINDOW_SIZE 116,3769
#define ATTRIB_WINDOW_SIZE 117,3830
#define PCMCIA_WINDOW_0_BASE 119,3896
#define PCMCIA_WINDOW_1_BASE 120,3958
#define PCMCIA_ATTRIB_WINDOW_0_BASE 121,4020
#define PCMCIA_ATTRIB_WINDOW_1_BASE 122,4078
#define INTERNAL_FLASH_BASE 123,4136
#define FAST_WINDOW_TIMING_SET 125,4189
#define FAST_WINDOW_SETUP_TIME 126,4243
#define FAST_WINDOW_COMMAND_TIME 127,4284
#define FAST_WINDOW_RECOVERY_TIME 128,4325
#define SLOW_WINDOW_TIMING_SET 129,4366
#define SLOW_WINDOW_SETUP_TIME 130,4420
#define SLOW_WINDOW_COMMAND_TIME 131,4461
#define SLOW_WINDOW_RECOVERY_TIME 132,4502
#define RALIB_MAX_DEV_NUM 148,4807
#define RALIB_DEV_NUM_PCMCIA_SOCKET_0 149,4868
#define RALIB_DEV_NUM_PCMCIA_SOCKET_1 150,4910
#define RALIB_DEV_NUM_INTERNAL_FLASH 151,4952
typedef struct ralib_dev_rec_ ralib_dev_rec_208,8100
} ralib_dev_rec_t;ralib_dev_rec_t225,8505
#define DEVICE_TYPE_UNKNOWN 229,8590
#define DEVICE_TYPE_PCMCIA 230,8629
#define DEVICE_TYPE_INTERNAL_FLASH 231,8668
#define get_dev_num_from_socket(get_dev_num_from_socket247,9205
#define get_socket_from_dev_num(get_socket_from_dev_num263,9776
uchar ralib_check_gt64010_rev 273,10152
uchar clpd6729_pcmcia_regs_offset 349,12099
void ralib_event_card_ready 376,12819
static void c7100_internal_flash_vpp 413,13777
void ralib_event_card_busy 441,14468
void ralib_event_card_protected 468,15069
void ralib_event_card_writable 484,15456
void ralib_event_card_inserted 515,16395
void ralib_event_card_removed 585,18641
int ralib_status 648,20393
int ralib_control 715,22246
void ralib_init_device_table 841,24803
ralib_vector_table_t *ralib_init 973,29636
int ralib_get_dev_num 1121,33668
const char *ralib_get_device_names(1163,34949
static void set_window 1185,35692
static ushort c7100_pcmcia16_read 1228,37215
static void c7100_pcmcia16_write 1301,39611
static int pcmcia_read 1328,40585
static int pcmcia_write 1459,44878
static int pcmcia_attrib_read 1594,49265
static int pcmcia_attrib_write 1686,52076
static int flash32_read 1774,54630
static int flash32_write 1948,59405
int ralib_read 2109,64437
int ralib_write 2185,66690
int ralib_attrib_read 2262,68976
int ralib_attrib_write 2336,71230
int ralib_device_init 2403,72991
int ralib_device_reset 2450,74217
ulong ralib_time 2498,75373
void ralib_yield 2524,75912
void ralib_remove_device 2558,76761
void ralib_insert_device 2591,77637
ulong ralib_get_pcmcia_base_local_addr 2623,78606
ulong ralib_get_flash_base_local_addr 2656,79449
ushort rsp_disable_io_write_buffer 2664,79736
void rsp_restore_io_write_buffer 2672,80003
