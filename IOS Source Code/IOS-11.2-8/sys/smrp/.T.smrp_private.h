
smrp_private.h,11600
#define __SMRP_PRIVATE_H__53,1906
typedef ulong SMRPPortID;SMRPPortID60,2125
typedef struct SMRPIdbsb_ SMRPIdbsb_68,2317
} SMRPIdbsb;SMRPIdbsb72,2431
    SMRP_REQUEST_TIMER,76,2478
    SMRP_RESPONSE_TIMER,77,2502
    SMRP_GROUPMEMBER_TIMER,78,2527
    SMRP_GROUPCREATOR_TIMER79,2555
    MCMaxNetwork 86,2737
    MCMaxNetwork = 0xffffff86,2737
    MCMaximumDistance 90,2776
    MCMaximumDistance = 0xff,90,2776
    MCPortParentUnreachable 91,2834
    MCPortParentUnreachable = 0xfe 91,2834
enum GroupSocket GroupSocket95,2928
    MCSocketStart 96,2947
    MCSocketEnd 97,2970
    MaxRetries 101,3003
    RequestInterval 102,3023
    RequestInterval = 10 * ONESEC,102,3023
    ResponseInterval 103,3058
    ResponseInterval = 2 * RequestInterval 103,3058
    ResponseInterval = 2 * RequestInterval * (MaxRetries 103,3058
    CreatorInterval 104,3121
    CreatorInterval = 60 * ONESEC,104,3121
    MemberInterval 105,3156
    MemberInterval = 30 * ONESEC,105,3156
    HelloInterval 106,3190
    HelloInterval = 10 * ONESEC,106,3190
    NodeAgeInterval 107,3223
    NodeAgeInterval = 3 * HelloInterval,107,3223
    PurgeRouteInterval 108,3264
    PurgeRouteInterval = ONEMIN,108,3264
    PrimaryRequestInterval 109,3297
    PrimaryRequestInterval = ONESEC,109,3297
    PrimaryRequestRetries 110,3334
    SecondaryRequestInterval 111,3365
    SecondaryRequestInterval = ONESEC,111,3365
    SecondaryRequestRetries 112,3404
    MemberRequestInterval 113,3437
    MemberRequestInterval = 5 * ONESEC,113,3437
    HolddownInterval 114,3477
    HolddownInterval = RequestInterval 114,3477
enum TwoHighBits TwoHighBits121,3722
    MCDatagram 122,3741
    MCDatagram = 0x00,122,3741
    MCRequestCommand 123,3764
    MCRequestCommand = 0x40,123,3764
    MCPositiveResponse 124,3793
    MCPositiveResponse = 0x80,124,3793
    MCNegativeResponse 125,3824
    MCNegativeResponse = 0xC0125,3824
enum SixLowBits SixLowBits128,3858
    MCData 129,3876
    MCHello 130,3892
    MCPrimaryNode 131,3909
    MCSecondaryNode 132,3932
    MCDistanceVector 133,3957
    MCCreateGroup 134,3983
    MCDeleteGroup 135,4006
    MCJoinGroup 136,4029
    MCLeaveGroup 137,4050
    MCGroupCreator 138,4072
    MCGroupMember 139,4096
    MCAddEntry 140,4120
    MCRemoveEntry 141,4141
    MCTunnelNode 142,4165
    MCLocateNode 143,4188
    MCNotifyNode 144,4211
enum CiscoInternal CiscoInternal147,4237
    MCBadDatagram 148,4285
    MCBadDatagram = 0xFF148,4285
enum MulticastDataType MulticastDataType151,4318
    MCMulticastDatagram 152,4343
    MCMulticastDatagram = MCData 152,4343
    MCMulticastDatagram = MCData + MCDatagram152,4343
enum HelloType HelloType155,4393
    MCHelloDatagram 156,4410
    MCHelloDatagram = MCHello 156,4410
    MCHelloDatagram = MCHello + MCDatagram156,4410
enum PrimaryNodeType PrimaryNodeType159,4457
    MCPrimaryRequest 160,4480
    MCPrimaryRequest = MCPrimaryNode 160,4480
    MCPrimaryRequest = MCPrimaryNode + MCRequestCommand,160,4480
    MCPrimaryConfirm 161,4537
    MCPrimaryConfirm = MCPrimaryNode 161,4537
    MCPrimaryConfirm = MCPrimaryNode + MCPositiveResponse,161,4537
    MCPrimaryReject 162,4596
    MCPrimaryReject = MCPrimaryNode 162,4596
    MCPrimaryReject = MCPrimaryNode + MCNegativeResponse162,4596
enum SecondaryNodeType SecondaryNodeType165,4657
    MCSecondaryRequest 166,4682
    MCSecondaryRequest = MCSecondaryNode 166,4682
    MCSecondaryRequest = MCSecondaryNode + MCRequestCommand,166,4682
    MCSecondaryConfirm 167,4743
    MCSecondaryConfirm = MCSecondaryNode 167,4743
    MCSecondaryConfirm = MCSecondaryNode + MCPositiveResponse,167,4743
    MCSecondaryReject 168,4806
    MCSecondaryReject = MCSecondaryNode 168,4806
    MCSecondaryReject = MCSecondaryNode + MCNegativeResponse168,4806
enum AddEntryType AddEntryType171,4871
    MCAddRequest 172,4891
    MCAddRequest = MCAddEntry 172,4891
    MCAddRequest = MCAddEntry + MCRequestCommand,172,4891
    MCAddConfirm 173,4941
    MCAddConfirm = MCAddEntry 173,4941
    MCAddConfirm = MCAddEntry + MCPositiveResponse173,4941
enum CreateGroupType CreateGroupType176,4996
    MCCreateRequest 177,5019
    MCCreateRequest = MCCreateGroup 177,5019
    MCCreateRequest = MCCreateGroup + MCRequestCommand,177,5019
    MCCreateConfirm 178,5075
    MCCreateConfirm = MCCreateGroup 178,5075
    MCCreateConfirm = MCCreateGroup + MCPositiveResponse,178,5075
    MCCreateReject 179,5133
    MCCreateReject = MCCreateGroup 179,5133
    MCCreateReject = MCCreateGroup + MCNegativeResponse179,5133
enum DeleteGroupType DeleteGroupType182,5193
    MCDeleteRequest 183,5216
    MCDeleteRequest = MCDeleteGroup 183,5216
    MCDeleteRequest = MCDeleteGroup + MCRequestCommand,183,5216
    MCDeleteConfirm 184,5272
    MCDeleteConfirm = MCDeleteGroup 184,5272
    MCDeleteConfirm = MCDeleteGroup + MCPositiveResponse,184,5272
    MCDeleteReject 185,5330
    MCDeleteReject = MCDeleteGroup 185,5330
    MCDeleteReject = MCDeleteGroup + MCNegativeResponse185,5330
enum RemoveEntryType RemoveEntryType188,5390
    MCRemoveRequest 189,5413
    MCRemoveRequest = MCRemoveEntry 189,5413
    MCRemoveRequest = MCRemoveEntry + MCRequestCommand,189,5413
    MCRemoveConfirm 190,5469
    MCRemoveConfirm = MCRemoveEntry 190,5469
    MCRemoveConfirm = MCRemoveEntry + MCPositiveResponse190,5469
enum JoinGroupType JoinGroupType193,5530
    MCJoinRequest 194,5551
    MCJoinRequest = MCJoinGroup 194,5551
    MCJoinRequest = MCJoinGroup + MCRequestCommand,194,5551
    MCJoinConfirm 195,5603
    MCJoinConfirm = MCJoinGroup 195,5603
    MCJoinConfirm = MCJoinGroup + MCPositiveResponse,195,5603
    MCJoinReject 196,5657
    MCJoinReject = MCJoinGroup 196,5657
    MCJoinReject = MCJoinGroup + MCNegativeResponse196,5657
enum GroupCreatorType GroupCreatorType199,5713
    MCCreatorRequest 200,5737
    MCCreatorRequest = MCGroupCreator 200,5737
    MCCreatorRequest = MCGroupCreator + MCRequestCommand,200,5737
    MCCreatorConfirm 201,5795
    MCCreatorConfirm = MCGroupCreator 201,5795
    MCCreatorConfirm = MCGroupCreator + MCPositiveResponse201,5795
enum GroupMemberType GroupMemberType204,5858
    MCMemberRequest 205,5881
    MCMemberRequest = MCGroupMember 205,5881
    MCMemberRequest = MCGroupMember + MCRequestCommand,205,5881
    MCMemberConfirm 206,5937
    MCMemberConfirm = MCGroupMember 206,5937
    MCMemberConfirm = MCGroupMember + MCPositiveResponse,206,5937
    MCMemberReject 207,5995
    MCMemberReject = MCGroupMember 207,5995
    MCMemberReject = MCGroupMember + MCNegativeResponse207,5995
enum LeaveGroupType LeaveGroupType210,6055
    MCLeaveRequest 211,6077
    MCLeaveRequest = MCLeaveGroup 211,6077
    MCLeaveRequest = MCLeaveGroup + MCRequestCommand,211,6077
    MCLeaveConfirm 212,6131
    MCLeaveConfirm = MCLeaveGroup 212,6131
    MCLeaveConfirm = MCLeaveGroup + MCPositiveResponse,212,6131
    MCLeaveReject 213,6187
    MCLeaveReject = MCLeaveGroup 213,6187
    MCLeaveReject = MCLeaveGroup + MCNegativeResponse213,6187
enum DistanceVectorType DistanceVectorType216,6245
    MCDistanceRequest 217,6271
    MCDistanceRequest = MCDistanceVector 217,6271
    MCDistanceRequest = MCDistanceVector + MCRequestCommand,217,6271
    MCDistanceConfirm 218,6332
    MCDistanceConfirm = MCDistanceVector 218,6332
    MCDistanceConfirm = MCDistanceVector + MCPositiveResponse218,6332
enum NotifyType NotifyType221,6398
    MCNotifyRequest 222,6416
    MCNotifyRequest = MCNotifyNode 222,6416
    MCNotifyRequest = MCNotifyNode + MCRequestCommand,222,6416
    MCNotifyConfirm 223,6471
    MCNotifyConfirm = MCNotifyNode 223,6471
    MCNotifyConfirm = MCNotifyNode + MCPositiveResponse,223,6471
    MCNotifyReject 224,6528
    MCNotifyReject = MCNotifyNode 224,6528
    MCNotifyReject = MCNotifyNode + MCNegativeResponse224,6528
#define SMRP_NOTIFY_SIZE 226,6586
enum LocateType LocateType228,6614
    MCLocateRequest 229,6632
    MCLocateRequest = MCLocateNode 229,6632
    MCLocateRequest = MCLocateNode + MCRequestCommand,229,6632
    MCLocateConfirm 230,6687
    MCLocateConfirm = MCLocateNode 230,6687
    MCLocateConfirm = MCLocateNode + MCPositiveResponse,230,6687
    MCLocateReject 231,6744
    MCLocateReject = MCLocateNode 231,6744
    MCLocateReject = MCLocateNode + MCNegativeResponse231,6744
enum TunnelType TunnelType234,6803
    MCTunnelRequest 235,6821
    MCTunnelRequest = MCTunnelNode 235,6821
    MCTunnelRequest = MCTunnelNode + MCRequestCommand,235,6821
    MCTunnelConfirm 236,6876
    MCTunnelConfirm = MCTunnelNode 236,6876
    MCTunnelConfirm = MCTunnelNode + MCPositiveResponse,236,6876
    MCTunnelReject 237,6933
    MCTunnelReject = MCTunnelNode 237,6933
    MCTunnelReject = MCTunnelNode + MCNegativeResponse237,6933
} MCHeader;MCHeader245,7109
#define SMRP_HEADER_SIZE 246,7121
#define SMRP_PAKTYPE_OFFSET 247,7148
#define SMRP_SEQNUM_OFFSET 248,7178
#define SMRP_GROUPADDR_OFFSET 249,7207
#define SMRP_VECTOR_SIZE 254,7274
} MCDistancePacket;MCDistancePacket259,7361
#define SMRP_TUNNEL_PKT_SIZE 264,7407
} MCTunnelPacket;MCTunnelPacket272,7615
#define SMRP_REJECT_SIZE 274,7634
typedef enum RequestState_ RequestState_276,7662
} RequestState;RequestState280,7798
enum ErrorValues ErrorValues282,7815
    MCNoError 283,7834
    MCPacketLength 284,7888
    MCBadState 285,7942
    MCTableFull 286,7996
    MCRangeFull 287,8050
    MCListFull 288,8104
    MCRouteNotFound 289,8158
    MCPortNotFound 290,8212
    MCGroupNotFound 291,8266
    MCNotCreator 292,8320
    MCNoResponse 293,8374
    MCNoTunnelNeeded 294,8428
} NodeState;NodeState309,8844
smrp_GetIdbsb 347,9907
smrp_GetPacketType(354,10067
smrp_GetPacketModifiers(361,10188
smrp_IsTransactionPacket(367,10271
smrp_IsRequestPacket(373,10364
smrp_IsResponsePacket(379,10468
smrp_datastart 385,10558
smrp_datastart_offset 391,10669
    SMRP_INPUT,400,10941
    SMRP_OUTPUT,401,10957
    SMRP_NO_BUFFERS,402,10974
    SMRP_MCDATA_IN,403,10995
    SMRP_MCDATA_OUT,404,11015
    SMRP_HELLO_IN,405,11036
    SMRP_HELLO_OUT,406,11055
    SMRP_REQUESTS_IN,407,11075
    SMRP_REQUESTS_OUT,408,11097
    SMRP_NOTIFY_IN,409,11120
    SMRP_NOTIFY_OUT,410,11140
    SMRP_CONFIRMS_IN,411,11161
    SMRP_CONFIRMS_OUT,412,11183
    SMRP_REJECTS_IN,413,11206
    SMRP_REJECTS_OUT,414,11227
    SMRP_PNODE_IN,415,11249
    SMRP_PNODE_OUT,416,11268
    SMRP_SNODE_IN,417,11288
    SMRP_SNODE_OUT,418,11307
    SMRP_DISTANCE_IN,419,11327
    SMRP_DISTANCE_OUT,420,11349
    SMRP_CREATE_IN,421,11372
    SMRP_DELETE_IN,422,11392
    SMRP_DELETE_OUT,423,11412
    SMRP_JOIN_IN,424,11433
    SMRP_JOIN_OUT,425,11451
    SMRP_LEAVE_IN,426,11470
    SMRP_LEAVE_OUT,427,11489
    SMRP_LOCATE_IN,428,11509
    SMRP_TUNNEL_IN,429,11529
    SMRP_CREATOR_OUT,430,11549
    SMRP_MEMBER_IN,431,11571
    SMRP_MEMBER_OUT,432,11591
    SMRP_ADDGRPENTRY_IN,433,11612
    SMRP_ADDGRPENTRY_OUT,434,11637
    SMRP_REMGRPENTRY_IN,435,11663
    SMRP_REMGRPENTRY_OUT,436,11688
    SMRP_NO_FORWARD,437,11714
    SMRP_FASTFWD_IN,438,11735
    SMRP_FASTFWD_OUT,439,11756
    SMRP_FORWARD_DUPS,440,11778
    SMRP_FORWARDS,441,11801
    SMRP_BAD_PORTID,442,11820
    SMRP_PORT_DOWN,443,11841
    SMRP_INPUT_ERR,444,11861
    SMRP_BADVERS,445,11881
    SMRP_RUNT,446,11899
    SMRP_BAD_PKTTYPE,447,11914
    SMRP_UNEXPECTED_PKT,448,11936
    SMRP_MAXTRAFFIC	449,11961
#define SMRPASSERT(SMRPASSERT452,12004
} SMRP_fn_data;SMRP_fn_data461,12247
