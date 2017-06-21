
xmodem_generic.h,893
#define __XMODEM_GENERIC_H__23,786
#define      SOH 27,864
#define	     STX	28,888
#define      EOT	29,909
#define      ACK 30,930
#define      NAK 31,953
#define	     CAN	32,976
#define	     ESC	33,997
#define      TIMEOUT 37,1036
#define      RETRYMAX 38,1062
#define	     BBUFSIZ	39,1183
#define	     SBUFSIZ	40,1231
#define      CRCCHR	41,1281
#define      SEENEOT 42,1335
#define      GOT_ABORT 43,1399
#define      AOUT_MAGIC 45,1427
#define      FALSE 47,1463
#define      TRUE 48,1489
#define      NULL 49,1515
enum ERROR_MSGS ERROR_MSGS52,1564
    SENDER_ABORT,53,1582
    SENDER_PROTO_MISMATCH,54,1600
    DATA_TIMEOUT,55,1627
    HDR_TIMEOUT,56,1645
    XMODEM_SYNC_ERROR,57,1662
    INVALID_IMAGE,58,1685
    INSUFFICIENT_FLASH,59,1704
    INSUFFICIENT_DRAM,60,1728
    TRUNCATED_IMAGE,61,1751
    MAX_XMODEM_ERRS62,1772
#define CRC16(CRC1665,1796
