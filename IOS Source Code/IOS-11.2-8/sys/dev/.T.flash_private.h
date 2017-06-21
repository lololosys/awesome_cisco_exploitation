
flash_private.h,1565
#define FLASH_HDR_MAGIC 47,1760
#define FLASH_FHDR_MAGIC 48,1802
#define MAX_RETRIES 50,1845
#define MC_TEST_PATTERN	52,1870
#define MC_NVRAM_ADDRESS	54,1908
#define MC_NVRAM_SIZE	55,1963
#define MC_VPP_CONTROL	56,2020
#define MC_ROM_ADDRESS	57,2084
#define MC_ROM0	58,2117
#define MC_ROM8	59,2150
#define MAX_ROMS	60,2188
#define MAX_FLASH_BANKS 61,2209
#define MAX_ROM_MFGS	62,2243
#define MAX_ROM_ADDR 63,2274
#define MCPLUS_MAX_SIZE 64,2347
#define VPP_ON	69,2483
#define VPP_OFF	70,2506
#define VPP_MASK	71,2530
#define MC_MAXIN	76,2591
#define MC_DEF_TIMES	77,2612
#define MC_DEF_START	78,2636
#define MC_DEF_LEN	79,2662
#define MC_DEF_DATA	80,2691
#define FLASH_READ_CMD	86,2764
#define FLASH_INFO_CMD	87,2789
#define FLASH_ERASE_CMD	88,2817
#define FLASH_ERASE_VER	89,2846
#define FLASH_PROG_CMD	90,2875
#define FLASH_PROG_VER	91,2903
#define FLASH_RESET_CMD	92,2931
#define FLHDR_RESERVED 98,3012
typedef struct flash_header flash_header108,3395
} flash_header;flash_header112,3521
#define FLASH_NAME_LEN 118,3598
typedef struct flash_file_hdr flash_file_hdr119,3667
} flash_fhdr;flash_fhdr126,3856
struct flash_write_args flash_write_args131,3936
#define FFH_FLAGS_DELETED 138,4039
#define FLASH_HDR_SIZE 140,4076
#define FLASH_FHDR_SIZE 141,4120
#define FLASH_OVERHEAD(FLASH_OVERHEAD142,4163
#define FLASH_AVAILABLE(FLASH_AVAILABLE143,4240
#define MIN_FILE_SIZE 144,4310
#define FLASH_FILE_CNT 146,4368
struct flash_t_ flash_t_148,4449
struct flash_info flash_info177,6203
#define MIN(MIN187,6406
