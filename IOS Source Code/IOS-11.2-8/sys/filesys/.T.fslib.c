
fslib.c,4037
#define strncpy 476,20776
#define STATIC481,20835
#define STATIC 483,20856
#define FSLIB_MIN_SUPPORTED_VERSION 503,21576
#define FSLIB_MAX_SUPPORTED_VERSION 504,21623
struct fslib_vector_table_t *FSLIB_init 610,28423
int FSLIB_open 739,34228
int FSLIB_read 994,41909
int FSLIB_close 1066,43915
int FSLIB_lseek 1187,47816
int FSLIB_write 1293,50667
int FSLIB_fstat 1363,52476
int FSLIB_stat 1414,53794
int FSLIB_istat 1456,54968
#define MONLIB_END_NAME_OFFSET 1524,56797
#define MONLIB_START_NAME_OFFSET 1525,56871
boolean check_monlib_mismatch 1526,56935
int FSLIB_fsstat 1588,58807
int FSLIB_delete 1858,66635
int FSLIB_undelete 1941,69103
int FSLIB_squeeze 2068,73290
STATIC fslib_error get_busy_info 2284,79714
STATIC fslib_error allocate_squeeze_buffer 2342,81485
STATIC void free_squeeze_buffer 2378,82352
STATIC fslib_error get_squeeze_state 2412,83402
STATIC fslib_error find_squeeze_log_rec 2531,87884
STATIC fslib_error build_ram_sector 2668,92018
STATIC fslib_error write_ram_sector_to_flash 2863,98635
STATIC fslib_error write_squeeze_log 2960,102173
STATIC fslib_error copy_sector_to_sector 3071,106135
int FSLIB_getdents 3142,108554
int FSLIB_settype 3275,112445
void FSLIB_geterrortext 3359,114721
int FSLIB_getdevicenames(3658,125251
#define	BFLASH_DEV	3676,125589
static int print_chip_info 3678,125621
int FSLIB_ioctl 3730,126686
int FSLIB_fioctl 3766,127464
fslib_device_t *init_device 3795,128705
fslib_device_t *get_dev 3951,133590
STATIC fslib_device_t *get_new_dev 3986,134444
STATIC void remove_dev 4022,135265
STATIC fslib_ptr find_next_new_file_hdr_ptr 4075,136945
STATIC fslib_ptr find_file_hdr_by_name 4228,141363
STATIC fslib_ptr find_file_hdr_by_index 4386,146163
STATIC fslib_error find_file_hdr_by_phys_ptr 4539,150792
STATIC fslib_error read_file_hdr 4625,153242
STATIC fslib_error write_file_hdr 4709,155807
STATIC void fix_in_use_file_hdr 4774,157971
STATIC void fix_created_file_hdr 4854,160744
STATIC fslib_error create_new_file 5000,165041
STATIC fslib_error mark_file_deleted 5108,168252
STATIC fslib_error mark_file_undeleted 5146,169441
STATIC boolean valid_file_id 5184,170580
STATIC boolean valid_dev_id 5232,171931
STATIC boolean valid_file_name 5302,174003
STATIC boolean valid_file_flags 5368,175881
STATIC boolean valid_whence 5404,176930
STATIC boolean valid_file_hdr 5440,177947
boolean valid_dib 5550,180972
STATIC ulong dev_read 5652,184349
STATIC void dev_seek 5768,188237
STATIC ulong dev_write 5832,190471
STATIC fslib_ptr skip_bad_sectors 5945,194252
STATIC int get_region 5985,195428
STATIC fslib_ptr get_end_of_region 6033,197263
STATIC int good_sector 6090,198927
STATIC void convert_file_hdr_to_dirent 6150,200691
STATIC void convert_file_hdr_to_stat 6183,201764
STATIC ulong get_sector_num 6224,203032
STATIC fslib_ptr get_sector_ptr 6248,203735
STATIC ulong get_bytes_left_this_sector 6271,204446
STATIC ulong get_bytes 6300,205349
STATIC fslib_error check_crc 6344,206689
STATIC ulong calc_crc 6381,207758
STATIC fslib_file_rec_t *get_file_rec 6520,211517
STATIC fslib_file_rec_t *get_new_file_rec 6553,212412
int convert_ralib_error_code 6629,214918
#define ADJUST_END_OF_STRING_OFFSET 6670,216823
STATIC void print_squeeze_status_update 6672,216862
STATIC void print_status_update 6740,218646
STATIC fslib_error check_sector_erase 6770,219487
STATIC fslib_error erase_sector 6835,221325
STATIC fslib_error switch_to_spare_sector 6900,223355
int FSLIB_build_dib(7047,229151
STATIC boolean device_being_format 7132,232419
STATIC int get_device_info_block 7152,232760
STATIC char *copy_monlib_to_buffer 7183,233452
STATIC fslib_prog_rec_t *find_prog_table_entry 7293,235859
STATIC boolean check_block_erased 7313,236309
STATIC boolean check_bad_sector 7330,236707
STATIC boolean read_flash_to_buf 7340,236944
STATIC char *erase_all_sectors 7370,237607
STATIC int copy_monlib_to_dev 7497,240554
STATIC int copy_bad_sector_map_to_dev 7527,241269
STATIC int format_device 7575,242427
