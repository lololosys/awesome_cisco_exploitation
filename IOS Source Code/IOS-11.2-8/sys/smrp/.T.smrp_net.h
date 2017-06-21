
smrp_net.h,634
#define __SMRP_NET_H__33,1085
#define SMRP_UNICASTADDR_SIZE 73,2850
typedef ulong UnicastAddr;UnicastAddr74,2882
typedef ulong SMRPGroupID;SMRPGroupID75,2939
    AllNodesAddr 78,2974
    AllEndpointsAddr 79,2996
    AllEntitiesAddr 80,3022
    AllNodesGroup 84,3058
    AllEndpointsGroup 85,3081
    AllEntitiesGroup 86,3108
typedef struct SMRPNet_ SMRPNet_115,4484
} SMRPNet;SMRPNet121,4731
typedef struct SMRPCallDowns_ SMRPCallDowns_205,8270
} SMRPCallDowns;SMRPCallDowns222,9185
typedef boolean (*GroupToMCfn)GroupToMCfn224,9203
typedef boolean (*GroupToMCfn)(hwidbtype *, SMRPGroupID, uchar uchar224,9203
