
tcpdriver.h,1451
#define __TCPDRIVER_H__44,1560
#define TCP_CONG_INCREASE 48,1619
#define TCP_CONG_LOW 49,1695
#define TCP_CONG_MEDIUM 50,1729
#define TCP_CONG_HIGH 51,1763
#define TCP_DAMP_CONG 52,1797
typedef void (*tcpdriver_closef)tcpdriver_closef55,1916
typedef void (*tcpdriver_closef)(tcp_encaps tcp_encaps55,1916
typedef boolean (*tcpdriver_flushf)tcpdriver_flushf56,1972
typedef boolean (*tcpdriver_flow_ctrl)tcpdriver_flow_ctrl57,2017
typedef boolean (*tcpdriver_flow_ctrl)(tcp_encaps *, ulong,ulong57,2017
typedef ushort tcp_encaps_state;tcp_encaps_state59,2090
typedef uchar tcp_encaps_events;tcp_encaps_events60,2123
typedef uchar tcp_encaps_flags;tcp_encaps_flags61,2156
enum { TCPD_ALIVE,64,2215
enum { TCPD_ALIVE, TCPD_CLOSE,64,2215
enum { TCPD_ALIVE, TCPD_CLOSE, TCPD_ABORT 64,2215
struct tcp_encaps_ tcp_encaps_66,2261
#define TDEV_NOT_ALIVE	95,3265
#define TDEV_OUT_PEND	96,3324
#define TDEV_NEW_CONN	97,3383
#define TDEV_KILL_ME	98,3449
#define TDEV_ALL_EVENTS	99,3498
#define TDF_ACTIVATED	105,3565
#define TCP_ACTIVATIONS	108,3676
#define TD_ACTIVATIONS	110,3774
#define TCPDSEND_CLOSED	116,3953
#define TCPDSEND_QUEUED	117,4014
#define TCPDSEND_QFULL	118,4079
#define TCPDSEND_SENT	119,4140
#define TCPDSEND_OK(TCPDSEND_OK121,4202
#define TCPDSEND_ERROR(TCPDSEND_ERROR122,4278
#define TCPD_PAK_INPIPE	128,4394
    TCPD_KP_ENABLE,135,4479
    TCPD_KP_INTERVAL,136,4499
    TCPD_KP_ATTEMPTS,137,4521
