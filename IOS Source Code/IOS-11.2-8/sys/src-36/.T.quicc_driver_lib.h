
quicc_driver_lib.h,1209
#define __QUICC_DRIVER_LIB_H__58,2138
#define DPRAM 82,2666
#define QUICC_REGS 83,2692
typedef void (*scc_handler_t)scc_handler_t85,2734
typedef void (*scc_handler_t)(hwidbtype *, paktype paktype85,2734
typedef void (*bd_status_handler_t)bd_status_handler_t86,2789
typedef void (*bd_status_handler_t)(hwidbtype *, ushort)ushort86,2789
typedef void (*line_handler_t)line_handler_t87,2848
typedef void (*line_handler_t)(hwidbtype hwidbtype87,2848
#define QUICC_DRIVER_INSTANCE 89,2894
#define RXBD_T 143,6331
#define TXBD_T 144,6357
typedef struct quicc_instance quicc_instance146,6384
} quicc_instance_t;quicc_instance_t162,6696
#define QUICC_INSTANCE_SIZE 165,6718
#define QUICC_BDSTAT_TX_RCNT(QUICC_BDSTAT_TX_RCNT166,6773
#define QUICC_PORT_NO(QUICC_PORT_NO168,6827
#define QUICC_BDSTAT_RX_PKT 170,6874
#define QUICC_BDSTAT_TX_PKT 171,6953
#define QUICC_BDSTAT_RX_DEFAULT 173,7009
#define QUICC_BDSTAT_TX_DEFAULT 174,7087
#define QUICC_TX_KICK 180,7384
typedef struct quicc_interrupt quicc_interrupt206,8305
} quicc_interrupt_t;quicc_interrupt_t211,8428
#define RX_ENABLED 215,8472
#define RX_DISABLED	216,8497
#define TX_ENABLED	217,8522
#define TX_DISABLED	218,8546
