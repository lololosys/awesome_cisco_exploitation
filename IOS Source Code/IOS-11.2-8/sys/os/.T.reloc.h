
reloc.h,1538
#define __RELOC_H__34,1131
#define FALSE 37,1173
#define TRUE 41,1239
#define MIN_RELOC_IMAGE_VER	44,1283
#define RELOC_IMAGE_VER	45,1359
#define RELOC_IMAGE_MAJOR_VER	47,1396
#define SIMPLE_CHECKSUM_ALGORITHM	49,1452
#define TC_ALGORITHM	50,1488
#define IC_ALGORITHM 51,1536
#define SHC_ALGORITHM 52,1590
#define BSS_OBJECT_ALIGN 54,1645
#define MAX_BSS_ENTRY 55,1711
#define MAX_LONG_RELOCATION_ENTRY 56,1789
#define MAX_RELOCATION_ENTRY 58,1899
#define TYPE_COMP 66,2206
#define TYPE_LZHUF 67,2231
#define TYPE_HPUX 68,2257
#define SUN_TOOLVER 69,2282
#define TYPE_SUN 70,2307
#define COMP_MAGIC 71,2332
#define LZHUF_MAGIC 72,2358
#define RELOC_MAGIC1 73,2385
struct sun_exec sun_exec75,2414
#define SUN_AOUT_HDR_SIZE 87,2892
#define ALIGNED_BLOCK_SIZE	88,2943
#define RELOC_HEADER_LEN 89,2974
struct old_spl_hdr old_spl_hdr95,3149
#define PAD_BRA_SIZE 111,3783
struct special_header special_header114,3865
#define ALIGN_TO_512_BYTE_BOUNDARY 271,11252
struct reloc_image_internal_header reloc_image_internal_header275,11416
enum REGIONS REGIONS300,12202
    REGION_TEXT,301,12217
    REGION_TEXT, REGION_DATA,301,12217
    REGION_TEXT, REGION_DATA, REGION_BSS,301,12217
    REGION_MAX302,12259
#define SP_HDR_SIZE 305,12280
typedef unsigned char           RELOC_OFFSET_TYPE;RELOC_OFFSET_TYPE306,12345
#define RELOC_OFFSET_SIZE 307,12396
struct relocation_table relocation_table309,12450
struct long_relocation_table long_relocation_table317,12718
struct bss_entry bss_entry325,12996
