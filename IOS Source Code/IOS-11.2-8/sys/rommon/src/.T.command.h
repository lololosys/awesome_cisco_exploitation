
command.h,966
#define __COMMAND_H__29,876
#define WCS_WR_CMD_REG 38,1044
#define WCS_WR_DAT_REG 39,1083
#define WCS_LOADED 40,1122
#define WCS_RESET_ROM_ADDRESS 41,1161
#define WCS_INCREMENT_ROM_ADDRESS 42,1223
#define WCS_READ_ROM 43,1285
#define WCS_RD_CMD_REG 44,1347
#define WCS_RD_DAT_REG 45,1386
#define WCS_INIT_CNT 48,1445
#define WCS_WR_RAM 49,1471
#define WCS_WR_PAR 50,1497
#define WCS_RD_RAM 51,1523
#define WCS_RD_PAR 52,1549
#define WCS_IMAGE_TRANSFER 53,1575
#define WCS_DIAG 54,1607
#define UCODE_TYPE 58,1636
#define VPLD_TYPE 59,1663
#define DBUS_TYPE 60,1690
#define UNIX_COMPRESSION 62,1718
#define NOT_COMPRESSED 63,1745
#define TRANSFER_FROM_RSP 64,1772
#define TRANSFER_TO_RSP 65,1800
typedef struct wcs_image_header_ wcs_image_header_82,2412
} wcs_image_header;wcs_image_header104,3253
typedef enum wcs_state_ wcs_state_106,3274
	     reading_data, image_mode} wcs_state;wcs_state107,3352
#define OFFSETOF(OFFSETOF109,3396
