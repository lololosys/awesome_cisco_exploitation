
flash_dvr_spec.h,894
#define FLASH_HDR_MAGIC 43,1488
#define FLASH_FHDR_MAGIC 45,1557
#define FLASH_HDR_SIZE 49,1648
#define FLASH_FHDR_SIZE 50,1708
#define MAX_ROMS 52,1736
#define FLASH_OVERHEAD(FLASH_OVERHEAD54,1757
#define FLASH_AVAILABLE(FLASH_AVAILABLE55,1834
#define MIN_FILE_SIZE 56,1904
#define FLASH_FILE_CNT 59,1981
#define FLASH_FILE_CNT 61,2067
#define FLASH_NAME_LEN 64,2155
typedef struct flash_file_hdr flash_file_hdr72,2309
} flash_fhdr;flash_fhdr79,2536
struct flash_t_ flash_t_81,2551
#define MAX_ROMS_LIST 100,3579
typedef struct flash_header flash_header103,3682
} flash_header;flash_header109,3981
enum flash_status flash_status111,3998
    FR_NOT_QUERIED,112,4018
    FR_NOT_VALID,113,4038
    FR_READ_ONLY,114,4056
    FR_READ_WRITE115,4074
typedef struct flash_geom flash_geom118,4100
} flash_geom;flash_geom150,5699
struct flash_info flash_info158,5774
