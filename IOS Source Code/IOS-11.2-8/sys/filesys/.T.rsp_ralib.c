
rsp_ralib.c,2547
#define strncpy 87,2899
#define STATIC 101,3142
#define WINDOW_SIZE 111,3462
#define ATTRIB_WINDOW_SIZE 112,3526
#define PCMCIA_BASE 113,3590
#define PCMCIA_WINDOW_0_BASE 114,3638
#define PCMCIA_WINDOW_1_BASE 115,3686
#define PCMCIA_ATTRIB_WINDOW_0_BASE 116,3734
#define PCMCIA_ATTRIB_WINDOW_1_BASE 117,3782
#define INTERNAL_FLASH_BASE 118,3830
#define FAST_WINDOW_TIMING_SET 120,3879
#define FAST_WINDOW_SETUP_TIME 121,3932
#define FAST_WINDOW_COMMAND_TIME 122,3972
#define FAST_WINDOW_RECOVERY_TIME 123,4012
#define SLOW_WINDOW_TIMING_SET 124,4052
#define SLOW_WINDOW_SETUP_TIME 125,4105
#define SLOW_WINDOW_COMMAND_TIME 126,4145
#define SLOW_WINDOW_RECOVERY_TIME 127,4185
#define RALIB_MAX_DEV_NUM 137,4659
#define RALIB_DEV_NUM_PCMCIA_SOCKET_0 138,4718
#define RALIB_DEV_NUM_PCMCIA_SOCKET_1 139,4759
#define RALIB_DEV_NUM_INTERNAL_FLASH 140,4800
typedef struct ralib_dev_rec_ ralib_dev_rec_197,7953
} ralib_dev_rec_t;ralib_dev_rec_t214,8358
#define DEVICE_TYPE_UNKNOWN 218,8443
#define DEVICE_TYPE_PCMCIA 219,8482
#define DEVICE_TYPE_INTERNAL_FLASH 220,8521
#define get_dev_num_from_socket(get_dev_num_from_socket237,9064
#define get_socket_from_dev_num(get_socket_from_dev_num254,9641
void ralib_event_card_ready 271,10159
void ralib_event_card_busy 310,11205
void ralib_event_card_protected 334,11761
void ralib_event_card_writable 351,12149
void ralib_event_card_inserted 384,13095
void ralib_event_card_removed 431,14706
int ralib_status 487,16370
static uchar rsp_read_reg8(549,17888
static void rsp_write_reg8(569,18417
int ralib_control 594,19228
void ralib_init_device_table 714,21911
ralib_vector_table_t *ralib_init 856,27010
int ralib_get_dev_num 998,30913
const char *ralib_get_device_names(1041,32200
static void set_window 1064,32949
static int pcmcia_read 1103,34196
static int pcmcia_write 1228,38288
static int pcmcia_attrib_read 1354,42502
static int pcmcia_attrib_write 1443,45251
static int flash32_read 1527,47757
static int flash32_write 1693,52492
int ralib_read 1848,57484
int ralib_write 1918,59602
int ralib_attrib_read 1989,61737
int ralib_attrib_write 2058,63862
int ralib_device_init 2120,65502
int ralib_device_reset 2166,66636
ulong ralib_time 2213,67700
void ralib_yield 2240,68243
void ralib_remove_device 2276,69092
void ralib_insert_device 2308,69971
ulong ralib_get_pcmcia_base_local_addr 2340,70940
ulong ralib_get_flash_base_local_addr 2356,71433
ushort rsp_disable_io_write_buffer 2368,71657
void rsp_restore_io_write_buffer 2394,72152
