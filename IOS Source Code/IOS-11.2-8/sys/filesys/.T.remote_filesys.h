
remote_filesys.h,4496
#define __remote_filesys_h__36,1107
#define RFS_OK 43,1363
#define RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER 44,1414
#define RFS_ERROR_SENDING_IPC_REQUEST 45,1465
#define RFS_ERROR_BUFFER_OVERFLOW 46,1516
#define RFS_FILE_NAME_MAX 51,1802
#define RFS_DEV_ID_MAX 52,1855
#define RFS_FILE_ID_MAX 54,1960
#define RFS_DATA_BUFFER_SIZE 55,2036
#define RFS_DIRENT_BUFFER_MAX 56,2077
#define RFS_DEVICE_NAMES_BUFFER_MAX 57,2134
#define RFS_ERROR_TEXT_MAX 58,2191
#define RFS_SERVER_MASTER_PORT_NAME 60,2249
#define RFS_SERVER_SLAVE_PORT_NAME 61,2318
typedef enum rfs_msg_type_ rfs_msg_type_67,2649
} rfs_msg_type;rfs_msg_type105,3415
typedef struct rfs_open_req_ rfs_open_req_115,3788
} rfs_open_req_t;rfs_open_req_t119,3894
typedef struct rfs_open_rsp_ rfs_open_rsp_121,3913
} rfs_open_rsp_t;rfs_open_rsp_t123,3958
typedef struct rfs_close_req_ rfs_close_req_126,3978
} rfs_close_req_t;rfs_close_req_t128,4024
typedef struct rfs_close_rsp_ rfs_close_rsp_130,4044
} rfs_close_rsp_t;rfs_close_rsp_t132,4099
typedef struct rfs_read_req_ rfs_read_req_135,4120
} rfs_read_req_t;rfs_read_req_t138,4187
typedef struct rfs_read_rsp_ rfs_read_rsp_140,4206
} rfs_read_rsp_t;rfs_read_rsp_t143,4293
typedef struct rfs_write_req_ rfs_write_req_146,4313
} rfs_write_req_t;rfs_write_req_t150,4419
typedef struct rfs_write_rsp_ rfs_write_rsp_152,4439
} rfs_write_rsp_t;rfs_write_rsp_t154,4489
typedef struct rfs_lseek_req_ rfs_lseek_req_157,4510
} rfs_lseek_req_t;rfs_lseek_req_t161,4592
typedef struct rfs_lseek_rsp_ rfs_lseek_rsp_163,4612
} rfs_lseek_rsp_t;rfs_lseek_rsp_t165,4664
typedef struct rfs_stat_req_ rfs_stat_req_168,4685
} rfs_stat_req_t;rfs_stat_req_t170,4756
typedef struct rfs_stat_rsp_ rfs_stat_rsp_172,4775
} rfs_stat_rsp_t;rfs_stat_rsp_t175,4848
typedef struct rfs_fstat_req_ rfs_fstat_req_178,4868
} rfs_fstat_req_t;rfs_fstat_req_t180,4914
typedef struct rfs_fstat_rsp_ rfs_fstat_rsp_182,4934
} rfs_fstat_rsp_t;rfs_fstat_rsp_t185,5008
typedef struct rfs_istat_req_ rfs_istat_req_188,5029
} rfs_istat_req_t;rfs_istat_req_t191,5116
typedef struct rfs_istat_rsp_ rfs_istat_rsp_193,5136
} rfs_istat_rsp_t;rfs_istat_rsp_t196,5210
typedef struct rfs_fsstat_req_ rfs_fsstat_req_199,5231
} rfs_fsstat_req_t;rfs_fsstat_req_t201,5302
typedef struct rfs_fsstat_rsp_ rfs_fsstat_rsp_203,5323
} rfs_fsstat_rsp_t;rfs_fsstat_rsp_t206,5407
typedef struct rfs_delete_req_ rfs_delete_req_209,5429
} rfs_delete_req_t;rfs_delete_req_t211,5502
typedef struct rfs_delete_rsp_ rfs_delete_rsp_213,5523
} rfs_delete_rsp_t;rfs_delete_rsp_t215,5579
typedef struct rfs_undelete_req_ rfs_undelete_req_218,5601
} rfs_undelete_req_t;rfs_undelete_req_t222,5707
typedef struct rfs_undelete_rsp_ rfs_undelete_rsp_224,5730
} rfs_undelete_rsp_t;rfs_undelete_rsp_t226,5788
typedef struct rfs_squeeze_req_ rfs_squeeze_req_229,5812
} rfs_squeeze_req_t;rfs_squeeze_req_t232,5900
typedef struct rfs_squeeze_rsp_ rfs_squeeze_rsp_234,5922
} rfs_squeeze_rsp_t;rfs_squeeze_rsp_t236,5979
typedef struct rfs_settype_req_ rfs_settype_req_239,6002
} rfs_settype_req_t;rfs_settype_req_t242,6097
typedef struct rfs_settype_rsp_ rfs_settype_rsp_244,6119
} rfs_settype_rsp_t;rfs_settype_rsp_t246,6176
typedef struct rfs_getdents_req_ rfs_getdents_req_249,6199
} rfs_getdents_req_t;rfs_getdents_req_t252,6273
typedef struct rfs_getdents_rsp_ rfs_getdents_rsp_254,6296
} rfs_getdents_rsp_t;rfs_getdents_rsp_t257,6394
typedef struct rfs_format_req_ rfs_format_req_260,6418
} rfs_format_req_t;rfs_format_req_t263,6508
typedef struct rfs_format_rsp_ rfs_format_rsp_265,6529
} rfs_format_rsp_t;rfs_format_rsp_t267,6585
typedef struct rfs_geterrortext_req_ rfs_geterrortext_req_270,6607
} rfs_geterrortext_req_t;rfs_geterrortext_req_t273,6690
typedef struct rfs_geterrortext_rsp_ rfs_geterrortext_rsp_275,6717
} rfs_geterrortext_rsp_t;rfs_geterrortext_rsp_t277,6794
typedef struct rfs_getdevicenames_req_ rfs_getdevicenames_req_280,6822
} rfs_getdevicenames_req_t;rfs_getdevicenames_req_t282,6885
typedef struct rfs_getdevicenames_rsp_ rfs_getdevicenames_rsp_284,6914
} rfs_getdevicenames_rsp_t;rfs_getdevicenames_rsp_t287,7025
typedef union ioctl_arg_ ioctl_arg_289,7054
} ioctl_arg_t;ioctl_arg_t294,7171
typedef struct rfs_ioctl_req_ rfs_ioctl_req_296,7187
} rfs_ioctl_req_t;rfs_ioctl_req_t301,7316
typedef struct rfs_ioctl_rsp_ rfs_ioctl_rsp_303,7336
} rfs_ioctl_rsp_t;rfs_ioctl_rsp_t306,7410
