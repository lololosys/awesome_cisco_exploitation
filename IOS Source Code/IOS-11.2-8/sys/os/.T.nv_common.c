
nv_common.c,2333
typedef struct nv_private_area_ nv_private_area_282,10836
} nv_private_area;nv_private_area285,10931
typedef enum nv_private_status_ nv_private_status_287,11003
} nv_private_status;nv_private_status291,11101
static inline uchar *word_align 293,11123
#define DES_KEY_LENGTH 298,11263
#define NAC0_VERSION 306,11459
boolean nv_check_nvptr 320,11889
void nv_set_nvptr 325,11952
void nv_clear_nvptr 330,12004
uint nv_get_config_size 335,12054
int nv_check_getptr 360,12958
int nv_check_done 475,16209
int nv_adjust_version 599,19755
int nv_setup_uncompress 670,21679
void nv_restore_nvptr 741,23909
boolean nv_rom_supports_nvcompress 780,24974
void nv_configure 839,26204
nvtype *nv_review_buffer 939,28829
nvtype *nv_review_buffer_raw 996,30313
void nv_review 1015,30839
boolean nv_write_open 1068,32198
boolean nv_write_open_raw 1106,33134
int nv_write_close 1120,33329
int nv_write_close_raw 1164,34311
int nv_erase 1191,34786
int nv_erase_raw 1215,35338
boolean ok_to_write_nv 1250,36591
void nv_build_private_config_header 1367,39584
static void nv_build_private_config 1383,40245
static void nv_current_common 1470,42788
int nv_current_to_nv 1539,44348
int nv_current_to_buffer 1566,44957
int nv_buffer_to_nv 1596,45760
static char *nv_encrypt_getptr 1642,46997
boolean nv_encrypt 1664,47553
static boolean nv_do_encrypt(1697,48254
boolean nv_decrypt 1731,49162
static uchar *nv_get_private_area 1805,51287
void nv_write_internal 1849,52209
void nv_write 1948,54966
void nv_write_encrypted 1969,55601
void nv_write_private 1987,56144
void nv_write_raw 2005,56527
static void nv_add_internal 2042,57419
void nv_add 2111,59325
void nv_add_encrypted 2133,59950
void nv_add_private 2150,60387
void nv_line 2179,60987
static void nv_write_line 2247,62655
void nv_line_out 2263,62941
void nv_line_end 2361,66289
void nv_store_master_keys 2383,66717
boolean nv_there_are_keys 2392,66894
void nv_set_master_key 2397,66975
static char *nv_get_master_key 2436,67883
nvtype_private *nv_private_getptr 2464,68770
static nvtype_private *nv_private_isconfig 2487,69486
static nv_private_status nv_save_private_config 2518,70200
static void nvram_subsys_init 2545,70945
#define NVRAM_MAJVERSION 2553,71164
#define NVRAM_MINVERSION 2554,71191
#define NVRAM_EDITVERSION 2555,71218
