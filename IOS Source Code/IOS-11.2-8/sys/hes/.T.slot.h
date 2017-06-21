
slot.h,1387
#define __SLOT_H__126,4560
#define CARDNAMELEN 138,4904
#define SPINNAKER_SP_SLOT 140,4928
#define UNKNOWN_SERIAL_NUMBER 142,4961
#define UNKNOWN_CARD_TYPE 143,5002
#define DBUS_IO_MAXLEN 146,5099
#define DBUS_MAX_ERRS 147,5126
#define VERSION_STR_LEN 149,5152
typedef void (*dbus_wr_t)dbus_wr_t168,5791
typedef void (*dbus_wr_t)(void *, uchar)uchar168,5791
typedef uchar (*dbus_io_rd_t)dbus_io_rd_t169,5833
typedef uchar (*dbus_io_rd_t)(uint, volatile uchar uchar169,5833
typedef void (*dbus_io_wr_t)dbus_io_wr_t170,5888
typedef void (*dbus_io_wr_t)(uint, volatile uchar *, uchar)uchar170,5888
#define DBUS_IO_TTY_CONSOLE 172,5950
#define DBUS_IO_TTY_DEBUG 173,5980
#define DBUS_IO_TTY_DEVICES 174,6010
#define SLOT_NUM_VIP_PAS 176,6041
struct slottype_ slottype_178,6072
#define FOR_ALL_SLOTS(FOR_ALL_SLOTS253,9985
#define FOR_ALL_NONEMPTY_SLOTS(FOR_ALL_NONEMPTY_SLOTS257,10094
#define FOR_ALL_ENABLED_SLOTS(FOR_ALL_ENABLED_SLOTS264,10291
#define FOR_ALL_ENABLED_IP_SLOTS(FOR_ALL_ENABLED_IP_SLOTS270,10423
#define BOARD_DISABLED	279,10867
#define BOARD_NONEXIST	280,10898
#define BOARD_ANALYZED	281,10929
#define BOARD_CCBNEEDED	282,10988
#define BOARD_WEDGED	283,11053
#define BOARD_UNSUITABLE 284,11121
#define BOARD_INVALID	285,11195
#define BOARD_IDBSREMOVED	286,11255
#define BOARD_RELOADED	290,11412
#define BOARD_DEBOUNCING	291,11478
