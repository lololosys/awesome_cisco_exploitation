
bsc_crc16_tst.c,396
#define dbug 72,2400
#define uchar 74,2465
#define Boolean 75,2493
#define uint 76,2515
typedef enum BSC_Input BSC_Input82,2617
SYN , SOB , DLE , GPH , EOB , ITB, NumInputs } BSC_Input;BSC_Input83,2642
#define NumStates 166,6643
typedef enum {a0, a1, a2, aX, aE} FsmActs;FsmActs183,7010
void ProcByteBCC 211,7609
void ProcByte 227,7880
int ProcFrame 270,8761
uint get_bcc 299,9522
