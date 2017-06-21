
filesys.c,1453
typedef struct fs_device_rec_ fs_device_rec_104,3994
} fs_device_rec_t;fs_device_rec_t112,4243
#define FS_MAX_DEVICE 114,4263
typedef struct fs_file_rec_ fs_file_rec_121,4608
} fs_file_rec_t;fs_file_rec_t127,4760
#define FS_MAX_FILES 129,4778
#define FILE_REC_FREE 132,4853
#define FILE_REC_IN_USE 133,4879
#define FS_ERROR_TOO_MANY_FILES_OPEN 138,5096
#define FS_ERROR_BAD_FILE_DESCRIPTOR 139,5136
#define FS_ERROR_BUFFER_OVERFLOW 140,5176
static int get_new_global_fd(145,5254
static int get_new_file_sys_type(164,5724
static fs_device_rec_t *get_dev(186,6439
static fs_device_rec_t *add_dev 221,7449
static fs_file_rec_t *find_file_rec(254,8354
static fs_file_rec_t *alloc_file_rec(280,8985
static void free_file_rec(307,9568
static int convert_error_code 329,10149
static void FS_subsys_init 347,10567
int FS_open 482,16622
int FS_close 539,18043
int FS_read 577,19216
int FS_write 611,20248
int FS_lseek 655,21980
int FS_stat 688,22960
int FS_fstat 720,23862
int FS_istat 755,24927
int FS_fsstat 788,25893
int FS_delete 820,26727
int FS_undelete 856,27852
int FS_squeeze 904,29759
int FS_settype 945,31220
int FS_ioctl 964,31614
int FS_getdents 1001,32868
void FS_geterrortext 1036,34035
int FS_getdevicenames 1071,35317
int FS_register 1153,37686
const char *get_file_name_by_file_id 1241,39973
const char *get_dev_id_by_file_id 1268,40452
void fileid2filename 1299,41128
void fileid2devid 1311,41368
