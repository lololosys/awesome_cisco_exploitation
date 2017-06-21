
if_ags_c7000.c,5925
#define	ALTERNATE	827,31562
char *cbus_mci_memtest 905,33018
void cbus_mci_txlimit 1104,38956
static void set_cbus_txlimit 1161,40503
#define PACKET_SNIFF_WORDS 1167,40641
#define PAGE_WORDS 1168,40717
#define PAGE_ONE 1169,40753
#define PAGE_255 1170,40789
#define PAGE_SHIFT 1171,40825
#define FATAL_ERR_OFFSET 1172,40856
#define FATAL_ERR_LOCK 1173,40887
#define BUFHDR_START 1174,40923
#define BUFHDR_PTR_OFFSET 1175,40959
#define BUFHDR_PG_OFFSET 1176,40990
#define BUFHDR_WORDS 1177,41021
#define BUFHDR_NEXTPTR_OFFSET 1178,41052
#define ALU_REGISTERS 1181,41131
#define BUFHDRS 1182,41163
#define PAGE_ZERO_WORDS 1183,41242
#define MEMA_CONTROL_WORDS 1184,41282
#define BUFHDRS 1189,41397
#define PAGE_ZERO_WORDS 1190,41428
#define MEMA_CONTROL_WORDS 1191,41459
#define ALU_REGISTERS 1192,41490
static void memd_lowcore_capture 1205,41871
static boolean cbus_dump_mem 1266,43720
static void show_cbus_lowcore 1286,44185
static void show_cbus_lowcore_current 1327,45221
static ushort cbus_mci_txreserve 1340,45539
static void cbus_ipc_txstart 1390,47009
static void cbus_mci_txstart 1448,49011
static void cbus_mci_txerror 1532,52076
static void cbus_mci_enable 1652,55704
void cbus_mci_output 1717,57710
char *cbus_mci_idb_typestring 1800,60122
void dci_add_interface 1862,62043
void mci_add_interface 1915,63724
static inline int cbus_get_pa_from_slotunit 1937,64253
static inline void cbus_mci_ether_idb_init 1946,64410
static inline void cbus_mci_serial_idb_init 2012,66430
static inline boolean cbus_mci_fddi_idb_init 2130,70851
static inline void cbus_mci_tr_idb_init 2187,72587
static inline void cbus_mci_hssi_idb_init 2256,75022
static inline void cbus_mci_aip_idb_init 2300,76565
static inline void cbus_mci_cip_idb_init 2332,77551
static inline void cbus_mci_led_init 2357,78372
boolean cbus_mci_idb_init 2391,79692
static boolean cbus_mci_bridge_rxtype_on_ctl 2642,85871
static void cbus_mci_rxtype_update 2689,87347
static void cbus_mci_ip_rxtypes_update 2756,88971
static void cbus_mci_decnet_rxtypes_update 2778,89622
static void cbus_mci_xns_rxtypes_update 2799,90230
static void cbus_mci_clns_rxtypes_update 2820,90820
static void cbus_mci_atalk_rxtypes_update 2841,91356
static void cbus_mci_novell_rxtypes_update 2863,91981
static void cbus_mci_apollo_rxtypes_update 2887,92731
static void cbus_mci_vines_rxtypes_update 2908,93337
void cbus_mci_rxtypes_update 2934,94048
static void cbus_throttle_controller 2961,94771
void cbus_mci_throttle 3012,96022
void cbus_mci_idb_throttle 3040,96639
static long cbus_errors 3067,97348
static void cbus_buffersize 3082,97607
void cbus_idb_rx_offset 3093,97964
void cbus_rx_buffer_parking 3106,98436
void cbus_idb_rx_max_byte_size 3116,98732
static void cbus_idb_rx_buffer_limit 3127,99046
void cbus_idb_rx_error 3138,99357
void cbus_idb_assign_pool 3148,99615
static inline int cbus_create_buffer 3159,99915
#define CBUS_CTLR(CBUS_CTLR3175,100208
#define CBUS2SLOT(CBUS2SLOT3176,100239
#define AGS_C7K(AGS_C7K3177,100264
#define CBUS_CTLR(CBUS_CTLR3179,100301
#define CBUS2SLOT(CBUS2SLOT3180,100327
#define AGS_C7K(AGS_C7K3182,100436
static void add_cbus_dci 3198,100879
static void add_cbus_ctlr 3301,104112
static int vip_get_if_type 3345,105271
static hwidbtype* idb_matching_slot 3387,106153
static void sp_eoir_reset 3444,107462
void sp_eoir_quiescere 3471,108095
static void cbus_restart_check 3612,112000
static void cbus_red_set_precedence 3636,112646
static int scan_slots_for_cbus_ctrlr 3646,112995
static inline boolean cbus_ctrlr_resyncable 3884,119497
static void fddi_smt_init 3897,119758
static inline boolean can_use_ext_int 3915,120179
int cbus_get_next_if_unit 3928,120363
static void cbus_shaping_support_7000 3942,120784
int cbus_analyze 3965,121451
static inline void cbus_fragment_packet 5075,150218
#define CBUS_MULTICAST_SRB_EXPLORER(CBUS_MULTICAST_SRB_EXPLORER5153,152191
#define CBUS_ETHER_RECEIVE	5166,152686
#define CBUS_HSSI_RECEIVE	5369,160288
#define	CBUS_MIP_RECEIVE	5656,172142
#define CBUS_FDDI_RECEIVE	5940,184520
#define CBUS_TOKEN_RECEIVE	6281,199961
#define CBUS_AIP_RECEIVE	6651,216191
static inline ushort channel_sniff_typecode 6870,225776
static inline ushort channel_sniff_vc 6881,226040
static inline void channel_sniff_rif 6892,226295
static inline paktype *cip_virtual_input_getbuffer 6920,227155
static void receive_encaps_pak 6948,227996
static boolean channel_encaps_known 6971,228479
#define CBUS_CHANNEL_RECEIVE 7028,229880
void cbus_interrupt 7165,239241
void cbus_daughter_cmd 7552,250006
#define DCI_MEMTEST_INDX_PLUS 7568,250446
static char *dci_memtest 7580,250903
#define SPEEDUP_DCI_MEMTEST7629,252109
void load_idb_buffer_info 7772,256150
typedef struct buffer_pool_type_ buffer_pool_type_7781,256391
} buffer_pool_type;buffer_pool_type7788,256825
static void cbus_buffer_carve 7791,256847
#define CBUSBUFDELTA 7889,259638
#define SP_MAX_BUFFERS	8006,263255
void cbus_init 8269,271133
static void fci_daughter_panic 8579,278956
void show_cbus 8625,280288
void cbus_buffers_command 8936,290034
void cbus_cache_init 9026,292517
int cbus_slotunit2unit 9088,294391
boolean cbus_unit2slotunit 9105,294795
void default_love_note_handler 9126,295336
void eip_platform_init 9181,296711
void fip_platform_init 9194,297073
void get_cbus_ctrlr_errors 9211,297531
void platform_idb_init 9316,301825
boolean ip_on_cbus2 9335,302258
void platform_req_interface_restart 9353,302637
void platform_req_card_restart 9371,303090
void platform_req_complex_restart 9389,303551
void platform_demand_complex_restart 9405,303858
void platform_light_off 9447,304758
void platform_light_on 9460,305009
void platform_update_lights 9473,305259
boolean platform_cbus_reset_in_progress 9489,305639
void platform_show_switch_counts 9497,305776
void trip_cbus_mci_txlimit(9520,306548
