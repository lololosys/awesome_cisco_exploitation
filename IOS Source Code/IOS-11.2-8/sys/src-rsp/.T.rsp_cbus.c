
rsp_cbus.c,3015
#define MAX_RX_BUFFER_POOLS	399,14022
#define CLASSIC_IPC_POOL 400,14052
#define TURBO_IPC_POOL 401,14132
#define NUM_IPC_POOLS 402,14202
#define EXTRA_BUFFER_POOLS 403,14270
#define NUM_PROCESS_POOLS	404,14351
#define MAX_BUFFER_POOLS	405,14432
#define MULTICAST_BUFHDRS	408,14577
#define DONT_OVER_ALLOCATE 411,14653
#define OVER_ALLOCATE_OKAY 412,14732
#define PROCESS_TX_PACKETS	419,14990
#define PROCESS_MINIBUF_COUNT	426,15178
#define PAGESIZE	428,15211
#define TWO_PAGESIZE 429,15234
#define THREE_PAGESIZE 430,15274
#define PAGEROOM	432,15314
#define ROUND_UP(ROUND_UP434,15337
#define CBUSBUFROUND(CBUSBUFROUND435,15393
#define CBUSOFFSETSIZE 436,15434
#define PROCESS_MINIBUF_SIZE	440,15572
#define CBUSBUFDELTA_START	441,15605
typedef struct bufpool_ bufpool_449,15832
} bufpool_t;bufpool_t458,16356
#define BUFPOOL_BADSIZE	460,16370
#define BUFPOOL_NONE	461,16399
typedef struct bpdata_t_ bpdata_t_467,16484
} bpdata_t;bpdata_t476,16974
#define RSP_LOVE_POOL_TYPE	478,16987
#define RSP_IPC_POOL_TYPE_TURBO 479,17016
#define RSP_IPC_POOL_TYPE_CLASSIC	480,17052
typedef struct carvdata_t_ carvdata_t_485,17154
} carvdata_t;carvdata_t504,18184
default_ctrlr_analyze 518,18391
default_ctrlr_init 532,18708
default_if_init 546,19025
#define RX_POOL(RX_POOL552,19153
memd_alloc_page_align 558,19257
memd_alloc 573,19582
#define FOR_ALL_POOLS(FOR_ALL_POOLS732,24360
#define FOR_ALL_RX_POOLS(FOR_ALL_RX_POOLS735,24469
find_bufpool 746,24720
hwidb_bw 768,25144
accumulate_bufdata 787,25515
discover_bufsizes 804,25830
per_ip_requirements 888,28168
define_nonrx_pools 988,30616
sort_by_bufsize 1050,32310
alloc_free_queues 1122,34140
carve_pool 1145,34570
alloc_nonrx_pools 1158,34831
#define PERCENT(PERCENT1178,35221
compute_rx_nb 1181,35278
alloc_rx_pools 1216,36145
alloc_leftover_buffers 1242,36512
write_bufpool 1278,37245
init_input_queues 1327,38315
disable_ip 1374,39854
reset_ip 1398,40565
invoke_ctrlr_init 1415,40855
static void set_cbus_txlimit 1448,41704
invoke_if_init 1457,41918
cdb_reset 1593,45954
idb_reset 1634,46750
void rsp_memd_carve 1700,48420
initial_memd_carve 1809,50802
start_slot 1828,51300
assign_cmdq 1886,53003
chassismib_newcard 1902,53333
ip_pagesize 2029,57167
ctrlr_analyze 2050,57565
clear_global_qa_data 2113,58874
static void rsp_shaping_support 2139,59351
int cbus_analyze 2153,59707
int cbus_slotunit2unit 2321,64606
boolean cbus_unit2slotunit 2327,64712
unsuitable_print_controllers 2341,65030
cybus_hw_version 2351,65246
void cbus_platform_registry_init 2356,65335
bufhdr_t *get_memd_buffer 2400,66803
bufhdr_t *get_ipc_memd_buffer 2440,67750
show_cbus 2486,68874
void rsp_print_global_queues 2629,73428
void rsp_print_if_queues 2672,74787
ucode_version_ck 2705,75506
boolean platform_cbus_reset_in_progress 2722,76053
rsp_test_cause_xbufhdr 2735,76263
rsp_test_cause_invrtn_type 2751,76574
rsp_test_cause_invrtn_hdr 2796,77718
rsp_test_cause_invrtn_bcastid 2841,78825
