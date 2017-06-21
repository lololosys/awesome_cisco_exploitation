
flash_dvr_les.c,2834
    #define DATA_BUS_REVERSED 99,3784
#define UNKNOWN_CHIP_STR 120,4196
#define INIT_FAILED_MSG 121,4236
static char *flash_manu_code 173,6444
static int flash_cmnd_set 183,6719
static char *flash_name 193,6990
ulong flash_chip_size 203,7257
static ulong flash_sector_size(214,7542
static inline ulong flash_got_exp 225,7987
static void flash_inc_retries 244,8500
static boolean flash_fast_prog_err 255,8764
static boolean flash_prog_err 262,9022
static ulong flash_erase_err 267,9201
static inline void flash_delay 279,9570
static inline ulong fl_cmd 287,9731
static boolean run_from_flash 296,9934
static inline ulong write32v(310,10387
static inline ulong read32v(319,10569
static ulong les_flash_erase 339,11278
static ulong les_flash_read 395,12826
static ulong les_flash_write 434,13643
static boolean flash_zero 663,20784
static boolean flash_prog_quad_ram 718,22395
static boolean flash_prog_ram 748,23892
static ulong flash_erase_ram 786,24783
static boolean flash_008_prog_quad 808,25424
static boolean flash_008_prog 873,27641
static boolean flash_data_verify 1025,32895
static boolean flash_008_erase_op 1054,33654
static ulong flash_008_erase 1235,40172
static boolean flash_010_prog_quad 1285,41707
static boolean flash_010_prog 1344,43357
static boolean flash_010_erase_op 1475,47846
static ulong flash_010_erase 1641,53158
static boolean flash_040_prog_quad 1687,54737
static boolean flash_040_prog 1754,57252
static boolean flash_040_erase_op 1882,61955
static ulong flash_erase_040 2055,68263
static void flash_print_chips_known 2099,69455
static void les_flash_info 2119,70132
void les_flash_chipinfo_init 2208,73191
static void flash_geom_relink 2251,74781
static uchar flash_chip_id_byte_reversal 2377,79322
static ushort flash_chip_id_short_reversal 2394,79812
int flash_chip_identify 2407,80142
ushort flash_probe_memory_type 2426,80618
static void flash_query_8_bit_040 2455,81511
static void flash_query_8_bit_040_end 2469,82138
static void flash_query_16_bit_040 2474,82200
static void flash_query_16_bit_040_end 2491,82995
static void flash_query_32_bit_040 2496,83058
static void flash_query_32_bit_040_end 2516,83841
static void flash_query_8_bit_oth 2521,83904
static void flash_query_8_bit_oth_end 2538,84607
static void flash_query_16_bit_oth 2543,84669
static void flash_query_16_bit_oth_end 2563,85586
static void flash_query_32_bit_oth 2573,85768
static void flash_query_32_bit_oth_end 2595,86576
void flash_bank_query_wrapper 2600,86639
static flash_geom_t *flash_geom_create 2787,93040
static ushort flash_query_roms 2837,94684
void les_flash_dvr_init 3146,106259
static void les_flash_driver_init 3189,107561
#define LESFLASH_MAJVERSION 3205,108028
#define LESFLASH_MINVERSION 3206,108058
#define LESFLASH_EDITVERSION 3207,108088
