
rp_ralib.c,899
#define delay(delay69,2322
#define STATIC81,2628
#define MAX_YIELD_TIME	82,2643
#define RALIB_MAX_DEV_NUM 89,2888
typedef struct ralib_dev_rec_ ralib_dev_rec_110,4060
} ralib_dev_rec_t;ralib_dev_rec_t121,4303
#define DEVICE_TYPE_UNKNOWN 125,4381
#define DEVICE_TYPE_PCMCIA 126,4420
void ralib_init_device_table 139,4765
ralib_vector_table_t *ralib_init 170,5525
STATIC int ralib_read 221,7089
STATIC int ralib_write 310,10453
STATIC int ralib_status 379,12636
STATIC int ralib_control 447,14933
STATIC void ralib_yield 502,16313
STATIC ulong ralib_time 541,17353
int ralib_device_init 569,17945
int ralib_device_reset 643,20472
int ralib_get_dev_num 687,21725
const char *ralib_get_device_names 731,23039
int STATIC ralib_attrib_read 747,23500
STATIC int ralib_attrib_write 764,24132
ushort rsp_disable_io_write_buffer 779,24698
void rsp_restore_io_write_buffer 788,24969
