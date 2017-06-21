
cbus_test.c,4764
#define BUFLEN 115,4033
#define BUF_WORDS 116,4054
#define MEC_MEMA_WORDS 117,4079
#define MEC_AF_WORDS	118,4115
#define AF_STEP 119,4146
#define WAITDELAY 121,4167
#define RXOFFSET 122,4192
#define TESTBYTES 124,4213
#define TESTLONGS 125,4234
#define DEFAULT_TRT 173,5219
#define DEFAULT_TMAX 174,5251
#define DEFAULT_TVX 175,5283
#define CLAIM_BUF_SIZE	176,5307
#define CLAIM_DA_OFFSET	177,5334
#define CLAIM_SA_OFFSET	178,5361
#define BEACON_SA_OFFSET 179,5389
void dci_command 198,5820
static ushort cbus_ctrlr_select 267,7523
ushort cbus_select 281,7828
static void cbus_reset 319,8752
static void cbus_soft_reset 344,9284
static void cbus_reset_daughter 365,9760
static void cbus_create_pool 393,10385
static void cbus_rx_buffersetup 429,11138
static void cbus_tx_buffersetup 521,13627
static void bia_stationaddress 549,14180
static char *cbus_devicename 575,14768
static void atm_specific 596,15454
static void ether_odd_byte_start 851,22185
static void ether_specific 974,25565
static void af_rw_cmd 1030,26841
static void fddi_transmit_sabme 1134,29361
static void fddi_blast 1217,31486
static void fddi_set_address 1396,35589
static void fddi_select_controller 1416,35958
static void fddi_memdtest 1437,36367
static void fddi_fill_af 2016,56371
static void fddi_bypass_switch 2072,57715
static void fddi_hold_reg 2158,60194
static void fddi_alu_dump_registers 2189,60939
static void fddi_put_endec_reg 2274,64342
static ushort fddi_get_endec_reg 2307,65110
static void fddi_dump_endec 2343,65958
static void fddi_endec_write_reg 2374,66955
static void fddi_endec_read_reg 2421,68007
static boolean fddi_formac_reset 2463,69022
static ushort fddi_get_formac_reg 2489,69653
static void fddi_put_formac_reg 2506,70057
static void fddi_dump_formac 2531,70542
static void fddi_read_formac_reg 2565,71668
static void fddi_write_formac_reg 2592,72367
static void fddi_claim_buffer 2629,73282
static void fddi_formac 2687,74773
static void fddi_enable_fddi 2925,80505
static void fddi_force_claim_beacon 2977,81931
void cbus_address 3042,83422
static boolean cbus_more 3085,84520
static boolean cbus_quit_test 3115,84995
static void cbus_type_mema 3137,85395
static void cbus_type_memd 3169,86052
static void cbus_type_afmem 3202,86727
static void cbus_daughter_mema 3237,87623
static void fddi_loop_transmit 3281,88730
static void fddi_loop_receive 3471,93694
static void fddi_cam_access 3526,95014
static void fddi_cam_dump 3622,98135
static mcitype *cbus_setup_unit 3707,100893
#define CBUS1_MEMA_BASE 3782,102868
#define CBUS2_MEMA_BASE	3783,102898
static boolean cbus_mema_test 3785,102929
#define MEMD_STEP 3884,105717
#define MEMD_BASE 3885,105779
static boolean cbus_memd_test 3887,105838
static void fddi_mema_readwrite 3991,108705
static boolean fddi_af_readwrite 4026,109591
static void fddi_memd_readwrite 4068,110605
static void fddi_memd_write 4107,111497
static void fddi_memd_read 4133,112103
static void fddi_memd_load 4152,112517
static void cbus_execute_special 4169,112839
static void cbus_execute_command 4265,115791
static void cbus_daughter_command 4330,117262
static boolean cbus_bus_test 4408,119246
static void cbus_config 4441,119985
static boolean fddi_af_test 4522,121990
static boolean mec_af_test 4645,125031
static void cbus_create_memory 4761,128128
static void cbus_basic_tests 4785,128922
static void fddi_keepalive_adjust 4875,130664
static void cbus_list 4888,130842
static void cbus_follow_lists 4944,132217
#define MEMA_LIST 4946,132264
static void cbus_interactive_list 4959,132683
static void cbus_search 5006,133625
void fddi_psistart 5063,134892
void fddi_psiclear 5068,134941
void fddi_psiend 5074,135009
void fddi_psistart 5086,135149
void fddi_psiclear 5091,135195
void fddi_psiend 5096,135241
boolean psipending 5101,135285
static void cbus_cache_lists 5112,135437
#define CA_VERSION	5114,135484
#define HASH_BASE 5115,135509
static void cbus_fci_page 5225,138326
static void lowcore_command 5351,142287
static void fsip_type_sram 5364,142521
#define MAX_FSIP_SRAM 5420,143977
static void fsip_specific 5427,144065
static void fddi_specific 5791,153777
static void cbus_find_dci 5978,158488
static int bce_create 6022,159679
static ushort bce_read 6079,161385
static ushort bce_write 6119,162389
static ushort bce_free 6160,163388
static void ab_commands 6194,164181
#define AB_HASH_BASE 6198,164243
#define AB_HASH_BASE 6200,164279
void cbus_mci_command 6669,177302
dgtype* pslot_to_dbus(6877,182956
void dbus_prot_wr 6881,183033
static void cbus_test_init 6891,183163
#define	CBUSTST_MAJVERSION	6900,183284
#define	CBUSTST_MINVERSION	6901,183313
#define	CBUSTST_EDITVERSION	6902,183342
