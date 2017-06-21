
rp_flash_filesys.c,1980
#define rp_fs_debug_print 71,2006
#define rp_fs_debug_print(rp_fs_debug_print73,2069
int rp_fs_open_flash 91,2632
int rp_fs_read_flash 186,5919
static void rp_write_ffh_flags 219,6999
int rp_fs_close_flash 240,7766
static ushort update_odd_cksum(322,11215
int rp_fs_write_flash 347,11814
int rp_fs_delete_flash 403,13822
int rp_fs_fsstat_flash 438,14927
rp_flash_rec_t *rp_alloc_flash_rec 467,15729
void rp_free_flash_rec 481,16032
boolean is_new_flash_file 492,16309
boolean is_flash_device 506,16640
boolean is_flash_file 517,16946
static ulong rp_convert_ffh_flags_to_fs_type 532,17337
ulong rp_calc_crc 565,18116
static ulong rp_calc_flash_crc 641,20060
static void rp_convert_ffh_to_stat(653,20413
int rp_fs_stat_flash 682,21372
static void rp_convert_fs_type_to_ffh_flags 723,22583
int rp_fs_fstat_flash 741,23046
int rp_fs_fsstat_flash_dev 761,23605
int rp_fs_settype_flash 779,24166
int rp_fs_undelete_flash 820,25351
int rp_fs_squeeze_flash 835,25798
static void rp_convert_ffh_to_dirent(851,26263
int rp_fs_getdents_flash 868,26956
void rp_dir_flash 892,27534
void fs_init_rp_flash 903,27839
static boolean valid_file_flags 940,28944
static fs_file_rec_t *create_file_rec 957,29333
static int RP_FLASH_open 985,29930
static int RP_FLASH_close 1001,30283
static int RP_FLASH_read 1014,30560
static int RP_FLASH_write 1028,30868
static int RP_FLASH_stat 1042,31175
static int RP_FLASH_fstat 1047,31290
static int RP_FLASH_fsstat 1058,31520
static int RP_FLASH_delete 1063,31649
static int RP_FLASH_undelete 1071,31843
static int RP_FLASH_squeeze 1076,31973
static int RP_FLASH_getdents 1081,32083
static int RP_FLASH_settype 1086,32219
static void RP_FLASH_geterrortext 1091,32343
static int RP_FLASH_getdevicenames(1096,32469
static int RP_FLASH_ioctl 1105,32668
static int RP_FLASH_fioctl 1122,33005
static int RP_FLASH_lseek 1127,33094
static int RP_FLASH_istat 1132,33185
static void rp_flash_subsys_init 1140,33331
