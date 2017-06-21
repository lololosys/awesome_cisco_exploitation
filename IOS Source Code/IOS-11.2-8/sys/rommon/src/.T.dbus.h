
dbus.h,2791
#define __DBUS_H__57,1750
#define RESET_MASK 66,1895
#define COLD_RESET 67,1933
#define DBUS_REG_RESET 68,1971
#define DBUS_PIN_RESET 69,2009
#define DBUS_REGS 72,2066
#define DBUS_REGS 75,2161
#define DB_RESET_REG 84,2344
#define DB_STATUS_REG 85,2408
#define INTERNAL_ERROR_PLUS_ONE 86,2473
#define DB_PHYSICAL_SLOT_NUMBER 94,2719
#define DB_LAST_CMDADDR 95,2777
#define DB_STATUS 96,2835
#define RESET_ACTION 97,2891
#define DB_CONTROL 99,2949
#define DB_MICROCODE 100,3015
#define DB_EEPROM 101,3081
#define DB_PORT_STATUS 104,3147
#define DB_CONSOLE 105,3190
#define DB_DEBUG 106,3231
#define DB_LOGGER 107,3272
#define PORT_DATA 112,3379
#define PORT_DBUS_READ 113,3422
#define PORT_CMD_FAIL 115,3483
#define PORT_DBUS_ADDRS 116,3555
#define PORT_CMD_FAIL 119,3654
#define PORT_DBUS_ADDRS 120,3726
typedef struct data_port_type_ data_port_type_126,3874
} data_port_type;data_port_type132,4114
#define OPCODE 137,4179
#define LOGCOUNT 138,4201
#define PHYSICAL_SLOT_MASK 140,4224
#define LOGICAL_SLOT_MASK 141,4259
#define INTERNAL_ERROR 145,4330
#define INTERNAL_ERROR_PLUS_ONE 146,4366
#define WCS_PARITY_ERROR 151,4536
#define WCS_CNTRL_INSTR_ERROR 152,4572
#define MICROCODE_STATUS 153,4608
#define DBUS_INVALID_REQUEST 154,4644
#define DBUS_RSP_READY 155,4721
#define DBUS_PORT_IO_AVAIL 156,4805
#define DBUS_ERROR_MASK 158,4909
#define CONSOLE_PORT 162,4997
#define DEBUG_PORT 163,5030
#define LOGGER_PORT 164,5063
#define MAX_PORTS 165,5096
#define DBUS_PORT_CONSOLE 171,5236
#define DBUS_PORT_DEBUG 172,5274
#define DBUS_PORT_LOGGER 173,5312
#define TERM_CHAR 175,5398
#define BAD_DATA 178,5483
#define TICKS_PER_USEC 180,5522
#define CNTL_MASTER_ENABLE 183,5633
#define CNTL_LED 184,5669
#define CNTL_WCS_CMD_ENABLE 185,5705
#define CNTL_WCS_LOADED 186,5741
#define CNTL_WCS_LOADING 187,5777
typedef struct dbus_hardware_regs_ dbus_hardware_regs_199,6098
} dbus_hardware_regs;dbus_hardware_regs217,7078
typedef struct dbus_hardware_regs_ dbus_hardware_regs_221,7125
} dbus_hardware_regs;dbus_hardware_regs232,7691
typedef struct io_ports_ io_ports_239,7771
} io_ports;io_ports244,8080
typedef struct dgtype_ dgtype_247,8098
} dgtype;dgtype262,8555
#define PHYSICAL_MASK 270,8681
typedef enum control_state_ control_state_272,8721
                     cmd_enabled} control_state;control_state273,8792
typedef struct dbus_globals_ dbus_globals_276,8880
} dbus_globals;dbus_globals299,10022
typedef struct dbuscore_ dbuscore_303,10073
} dbuscore_t;dbuscore_t307,10224
static inline uchar dbus_addrs 320,10412
static inline uint return_opcode 334,10661
static inline uchar return_logcount 343,10826
static inline void disable_dcl_int(352,10943
static inline void enable_dcl_int(361,11103
