
signal.h,811
#define __SIGNAL_H__26,815
#define	SIGHUP	31,866
#define	SIGINT	32,896
#define	SIGQUIT	33,938
#define	SIGILL	34,978
#define	SIGTRAP	35,1021
#define	SIGIOT	36,1056
#define SIGABRT 37,1095
#define	SIGEMT	38,1133
#define	SIGFPE	39,1172
#define	SIGKILL	40,1220
#define	SIGBUS	41,1249
#define	SIGSEGV	42,1283
#define	SIGSYS	43,1331
#define	SIGPIPE	44,1383
#define	SIGALRM	45,1447
#define	SIGTERM	46,1484
#define	SIGUSR1	47,1547
#define	SIGUSR2	48,1594
#define	SIGCLD	49,1641
#define SIGCHLD 50,1682
#define	SIGPWR	51,1725
#define SIGWDOG	53,1769
#define SIGEXIT	54,1820
#define SIGMAX 56,1885
#define SIGDIAG 61,2006
typedef void (*signal_handler)signal_handler73,2156
#define SIG_ERR	83,2468
#define SIG_DFL	84,2499
#define SIG_IGN	85,2529
struct signal_table_ signal_table_92,2608
