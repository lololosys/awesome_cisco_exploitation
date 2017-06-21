
me.c,2091
#define     ME_C 102,3935
#define     ID_REQUEST 133,4917
#define     ID_ASSIGNED 134,4991
#define     ID_DENIED 135,5065
#define     ID_CHK_REQ 136,5139
#define     ID_CHK_RESP 137,5213
#define     ID_REMOVE 138,5287
#define     ID_VERIFY 139,5361
#define     OUTSTANDING 143,5511
#define     ASSIGNED 144,5585
#define     MC_PARAM_REQ 148,5730
#define     MC_PARAM_CONF 149,5804
#define     MC_PARAM_ERR 150,5878
#define     NO_INTERNAL 151,5952
#define     MEI 153,6027
#define     FI 155,6102
#define     GI 156,6176
#define     GL0 157,6250
#define     GL1 158,6324
#define     PI0 159,6357
#define     PL0 160,6431
#define     PI1 161,6505
#define     PL1 162,6579
#define     PI2 163,6653
#define     PL2 164,6727
#define     PI3 165,6801
#define     PL3 166,6875
#define     FIRST_SEED 170,6980
#define     MULTIPLIER 171,7010
#define     INCREMENT 172,7041
#define     MAX_ME_CES 176,7084
struct LayerMgmtTeiRequestLayerMgmtTeiRequest246,10013
PUBLIC boolean me_activate_ok 302,12369
PUBLIC void me_set_activate_flag 316,12721
PUBLIC void ME_Go 330,13056
PUBLIC void ME_Init 398,14776
PRIVATE void near ME_Mail 477,17039
PRIVATE void near ME_Remove_TEI 493,17453
PUBLIC void ME_ShutDown 587,20064
PRIVATE void near LM_Go 659,22185
PRIVATE void near LM_MDL_ASS_IND 696,22687
PRIVATE void near LM_MDL_UI_DATA_IND 807,25924
PRIVATE void near LM_MC_PARAM_CONF 1032,33108
PRIVATE void near LM_MC_PARAM_ERR 1067,33890
LM_PersDeact 1103,34920
PRIVATE void near LM_T202_EXPIRY 1131,35552
PRIVATE BYTE near LM_GetCes 1279,40776
PRIVATE void near LM_PutTeiMsg 1299,41101
PRIVATE UINT16 near LM_GetRi 1321,41777
PRIVATE BYTE near LM_Search 1345,42344
PRIVATE void near LM_ShowTei 1366,42666
PRIVATE void near CM_Go 1388,42972
PRIVATE void near CM_MC_PARAM_REQ 1449,44063
PRIVATE void near CM_MDL_ERR_IND 1514,45802
PRIVATE void near CM_MDL_XID_CONF 1574,47646
PRIVATE void near CM_T204_EXPIRY 1627,48699
PRIVATE void near CM_GetParmMsg 1693,50454
PRIVATE void near CM_PutParmMsg 1720,51171
PRIVATE void near ME_Verify_TEI 1758,52305
