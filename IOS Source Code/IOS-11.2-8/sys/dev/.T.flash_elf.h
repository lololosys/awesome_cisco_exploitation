
flash_elf.h,819
#define __FLASH_ELF_H__30,940
#define ELF 45,1401
#define UNKNOWN_IMAGE_OR_DATA_FILE 47,1419
#define UNCOMPRESSED_68K_RFR_IMAGE 48,1459
#define UNCOMPRESSED_68K_RFF_IMAGE 49,1499
#define UNCOMPRESSED_MOP_RFR_IMAGE 50,1539
#define UNCOMPRESSED_MOP_RFF_IMAGE 51,1579
#define COMPRESSED_68K_IMAGE 52,1619
#define COMPRESSED_68K_RFR_IMAGE 53,1659
#define COMPRESSED_68K_RFF_IMAGE 54,1699
#define COMPRESSED_MOP_RFR_IMAGE 55,1739
#define COMPRESSED_MOP_RFF_IMAGE 56,1779
static inline boolean elf_image_RFF_check 66,2083
static inline boolean elf_image_flash_write_checks 76,2354
static boolean elf_image_RFF_exec_checks 87,2610
static inline int elf_image_RFR_exec_checks 93,2776
static ulong *sniff_image_type 116,3466
static int flash_download_verify 135,3895
static boolean flash_boot_verify 160,4526
