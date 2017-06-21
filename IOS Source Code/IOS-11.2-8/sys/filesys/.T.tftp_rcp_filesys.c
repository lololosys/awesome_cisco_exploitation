
tftp_rcp_filesys.c,2621
#define	REMOTE_SERVER_ACCESS_TIMEOUT	89,2742
#define	TFTP_RCP_DEV_GO	91,2790
#define	TFTP_RCP_DEV_DONE	92,2817
#define	TFTP_RCP_DEV_ERROR	93,2845
#define	TFTP_RCP_DEV_TIMEOUT	94,2874
typedef struct remote_file_blk_remote_file_blk_96,2906
} remote_file_blk_t;remote_file_blk_t112,3678
static boolean dev_get_server 117,3744
static boolean server_dev_sleep 159,4758
static boolean valid_file_flags 201,5597
static fs_file_rec_t *create_file_rec 219,6010
#define	FS_TFTP_FILE_NAME_LEN	256,6894
#define	FS_TFTP_DEV_NAME_LEN	257,6941
#define	FS_TFTP_OPEN_FILE_NUM	259,6985
#define	TFTP_SERVER_NAME_LEN	260,7051
#define	TFTP_DATA_BUFFER_SIZE	261,7110
#define	TFTP_DEV_NAME	263,7180
static boolean tftp_callback_test 281,7710
static int TFTP_open 307,8366
static boolean tftp_callback_proc_data 450,11639
static forktype tftp_dev_proc 493,12567
static int get_server_data 531,13607
static int TFTP_read 586,14724
static int TFTP_close 669,16512
static int TFTP_write 717,17512
static int TFTP_fstat 781,18879
static int TFTP_getdevicenames(802,19322
static void TFTP_geterrortext 814,19555
static int TFTP_ioctl 825,19788
static int TFTP_fioctl 861,20542
static int TFTP_delete 893,21164
static int TFTP_fsstat 898,21241
static int TFTP_lseek 909,21514
static int TFTP_stat 957,22447
static int TFTP_istat 965,22584
static int TFTP_undelete 973,22713
static int TFTP_squeeze 981,22842
static int TFTP_settype 989,22954
static int TFTP_getdents 997,23087
#define	FS_RCP_FILE_NAME_LEN	1011,23441
#define	FS_RCP_DEV_NAME_LEN	1012,23487
#define	FS_RCP_OPEN_FILE_NUM	1014,23530
#define	RCP_SERVER_NAME_LEN	1015,23595
#define	RCP_DATA_BUFFER_SIZE	1016,23652
#define	RCP_DEV_NAME	1018,23714
static boolean rcp_callback_test 1039,24282
static long rcp_callback_fetch_data 1062,24837
static forktype rcp_dev_proc_write 1098,25701
static int RCP_open 1128,26385
static boolean rcp_callback_proc_data 1237,29063
static forktype rcp_dev_proc 1275,29990
static int RCP_read 1311,30837
static int RCP_close 1380,32369
static int RCP_write 1420,33150
static int RCP_fstat 1517,35301
static int RCP_getdevicenames(1538,35741
static void RCP_geterrortext 1550,35971
static int RCP_ioctl 1561,36203
static int RCP_fioctl 1597,36954
static int RCP_delete 1629,37573
static int RCP_fsstat 1634,37649
static int RCP_lseek 1645,37921
static int RCP_stat 1692,38850
static int RCP_istat 1700,38986
static int RCP_undelete 1708,39114
static int RCP_squeeze 1716,39242
static int RCP_settype 1724,39353
static int RCP_getdents 1732,39485
static void tftp_rcp_subsys_init 1746,39825
