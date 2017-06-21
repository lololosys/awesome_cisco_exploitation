
flash_dvr_les.h,2203
#define __FLASH_DVR_LES_H__38,1329
#define MAX_CMND_SETS 40,1358
#define WRITE_TRIES 42,1383
#define ZERO_WRITE_TRIES 43,1406
#define ERASE_TRIES 44,1436
#define READY_LOOP_COUNT_008 46,1462
#define READY_LOOP_COUNT_040 47,1497
enum chip_type chip_type49,1533
    CHIP_TYPE_FLASH_ROM,50,1550
    CHIP_TYPE_RAM,51,1575
    CHIP_TYPE_ROM52,1594
#define TYPE_FLASH_ROM 55,1620
#define TYPE_RAM 56,1645
enum flash_status flash_status58,1665
    FR_NOT_QUERIED,59,1685
    FR_NOT_VALID,60,1705
    FR_READ_ONLY,61,1723
    FR_READ_WRITE,62,1741
    FR_RAM,63,1760
    FR_REMOVED64,1772
#define BUG(BUG68,1830
#define BUGELSE 69,1866
#define BUGEND 70,1891
#define BF_CACHE 73,1954
#define BF_MASK 74,2007
#define BF_WRTRIES 75,2062
#define BF_BOOT 76,2088
#define BF_PROBE 77,2114
#define BF_ERASE 78,2140
#define BF_VERIFY 79,2166
#define FLASH_GEOM 81,2193
} flash_geom_t;flash_geom_t118,4338
} flash_info_t;flash_info_t133,4885
#define FLASH_TABLE_END	135,4902
enum COMMAND_SET COMMAND_SET153,5684
 INVALID_SET,154,5703
 INTEL_010,155,5717
 INTEL_008,156,5771
 AMD_040,157,5848
 ANY_TYPE_OF_RAM158,5888
#define FLASH_COMMAND(FLASH_COMMAND168,6106
#define reset_cmd_010 170,6175
#define read_id_010 171,6237
#define erase_cmd_010 172,6297
#define erase_verify_010 173,6359
#define read_mem_010 174,6424
#define program_mem_010 175,6485
#define program_verify_010 176,6545
#define reset_cmd_008 180,6639
#define read_id_008 181,6701
#define erase_mem_008 182,6761
#define erase_cnf_008 183,6823
#define read_mem_008 184,6887
#define program_mem_008 185,6948
#define read_status_008 186,7008
#define clear_status_008 187,7069
#define erase_susp_008 188,7131
#define erase_resm_008 189,7194
#define reset_cmd_040 192,7283
#define read_id_040 193,7343
#define erase_chip_040 194,7401
#define erase_chip_cnf_040 195,7466
#define erase_sect_040 196,7532
#define erase_sect_cnf_040 197,7597
#define read_mem_040 198,7664
#define program_mem_040 199,7723
#define ADR_040_5555 200,7781
#define ADR_040_2AAA 201,7823
#define ADR_040_5555_VAL 202,7864
#define ADR_040_2AAA_VAL 203,7926
#define reset_cmd 209,8108
#define read_id 210,8163
