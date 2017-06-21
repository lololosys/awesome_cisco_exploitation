
smttypes.h,9750
#define	SIZE_DEFAULT	65,2063
typedef uInt32   uTime;uTime74,2192
typedef struct PathConfigStructPathConfigStruct82,2415
} PathConfigType;PathConfigType88,2695
typedef Flag     FDDIActionReply;FDDIActionReply121,3864
typedef uChar    FDDIAvailablePaths;FDDIAvailablePaths124,3920
typedef uInt32   FDDICounter;FDDICounter127,3972
typedef uInt16   FDDICurrentPath;FDDICurrentPath130,4021
typedef uChar    FDDIFlag;FDDIFlag133,4067
typedef uChar    FDDILongAddressType[FDDILongAddressType136,4117
#define	SIZE_FDDILongAddressType	137,4158
typedef uInt16   FDDIPortType;FDDIPortType140,4209
typedef uChar    FDDIPermittedPaths;FDDIPermittedPaths143,4262
typedef uInt16   FDDIPresent;FDDIPresent146,4314
typedef uInt16   FDDIResourceId;FDDIResourceId149,4362
typedef struct ResourceTagStructResourceTagStruct152,4414
} FDDIResourceTag;FDDIResourceTag158,4571
#define	SIZE_FDDIResourceTag	160,4591
typedef uInt32   FDDITime;FDDITime163,4634
typedef uInt32   FDDITimer2sComp;FDDITimer2sComp166,4688
typedef struct TimeStampStructTimeStampStruct169,4739
} FDDITimeStamp;FDDITimeStamp173,4868
#define	SIZE_FDDITimeStamp	175,4886
typedef struct VendorMIBStructVendorMIBStruct178,4936
} FDDIVendorMIBType;FDDIVendorMIBType182,5039
typedef struct XDIConIndexStructXDIConIndexStruct185,5087
} XDIConIndexType;XDIConIndexType190,5205
typedef struct ManufacturerDataStructManufacturerDataStruct199,5332
} SMTManufacturerDataType;SMTManufacturerDataType203,5498
#define	SIZE_SMTManufacturerDataType	205,5526
typedef struct PeerWrapConditionDataStructPeerWrapConditionDataStruct208,5599
} SMTPeerWrapConditionDataType;SMTPeerWrapConditionDataType214,5760
#define	SIZE_SMTPeerWrapConditionDataType	216,5793
typedef struct SetCountStructSetCountStruct219,5857
} SMTSetCountType;SMTSetCountType223,5998
#define	SIZE_SMTSetCountType	225,6018
typedef struct StationIdStructStationIdStruct228,6104
} SMTStationIdType;SMTStationIdType232,6247
#define	SIZE_SMTStationIdType	234,6268
typedef uChar    SMTUserDataType[SMTUserDataType237,6320
#define	SIZE_SMTUserDataType	238,6358
typedef struct DuplicateAddressStructDuplicateAddressStruct247,6498
} MACDuplicateAddressType;MACDuplicateAddressType253,6661
typedef struct DuplicateAddressDataStructDuplicateAddressDataStruct256,6720
} MACDuplicateAddressDataType;MACDuplicateAddressDataType261,6867
#define	SIZE_MACDuplicateAddressDataType	263,6899
typedef struct FrameErrorDataStructFrameErrorDataStruct266,6969
} MACFrameErrorDataType;MACFrameErrorDataType275,7212
#define	SIZE_MACFrameErrorDataType	277,7238
typedef struct NeighborChangeStructNeighborChangeStruct280,7302
} MACNeighborChangeType;MACNeighborChangeType290,7581
typedef struct NeighborChangeDataStructNeighborChangeDataStruct293,7636
} MACNeighborChangeDataType;MACNeighborChangeDataType298,7781
#define	SIZE_MACNeighborChangeDataType	300,7811
typedef struct NotCopiedDataStructNotCopiedDataStruct303,7878
} MACNotCopiedDataType;MACNotCopiedDataType311,8098
#define	SIZE_MACNotCopiedDataType	313,8123
typedef struct MACRequestedPathsStructMACRequestedPathsStruct315,8161
} MACRequestedPathsType;MACRequestedPathsType319,8258
typedef struct MACPathChangeStructMACPathChangeStruct322,8305
} MACPathChangeType;MACPathChangeType328,8471
typedef struct MACPathChangeDataStructMACPathChangeDataStruct331,8518
} MACPathChangeDataType;MACPathChangeDataType336,8654
#define	SIZE_MACPathChangeDataType	338,8680
typedef uChar    MACShortAddressType[MACShortAddressType341,8742
typedef struct ConfigurationFailedStructConfigurationFailedStruct344,8821
} MACConfigurationFailedType;MACConfigurationFailedType351,9033
typedef struct ConfigurationFailedDataStructConfigurationFailedDataStruct353,9064
} MACConfigurationFailedDataType;MACConfigurationFailedDataType358,9224
#define	SIZE_MACConfigurationFailedDataType	360,9259
typedef struct SMTAddressChangedStructSMTAddressChangedStruct363,9342
} MACSMTAddressChangedType;MACSMTAddressChangedType370,9567
typedef struct SMTAddressChangedDataStructSMTAddressChangedDataStruct372,9596
} MACSMTAddressChangedDataType;MACSMTAddressChangedDataType377,9747
#define	SIZE_MACSMTAddressChangedDataType	379,9780
typedef struct EBErrorDataStructEBErrorDataStruct396,10068
} PORTEBErrorDataType;PORTEBErrorDataType401,10195
#define	SIZE_PORTEBErrorDataType	403,10219
typedef struct LCTFailedStructLCTFailedStruct406,10283
} PORTLCTFailedType;PORTLCTFailedType413,10461
typedef struct LCTFailedDataStructLCTFailedDataStruct415,10483
} PORTLCTFailedDataType;PORTLCTFailedDataType420,10612
#define	SIZE_PORTLCTFailedDataType	422,10638
typedef struct LerStructLerStruct425,10691
} PORTLerType;PORTLerType433,10891
typedef struct LerDataStructLerDataStruct436,10925
} PORTLerDataType;PORTLerDataType441,11039
#define	SIZE_PORTLerDataType	443,11059
typedef struct MACIndicatedStructMACIndicatedStruct446,11115
} PORTMACIndicatedType;PORTMACIndicatedType450,11203
#define	SIZE_PORTMACIndicatedType	452,11228
typedef struct PORTRequestedPathsStructPORTRequestedPathsStruct456,11367
} PORTRequestedPathsType;PORTRequestedPathsType460,11465
typedef struct PORTPathChangeStructPORTPathChangeStruct463,11513
} PORTPathChangeType;PORTPathChangeType471,11739
typedef struct PORTPathChangeDataStructPORTPathChangeDataStruct474,11787
} PORTPathChangeDataType;PORTPathChangeDataType479,11925
#define	SIZE_PORTPathChangeDataType	481,11952
typedef struct UndesiredConnectionStructUndesiredConnectionStruct484,12022
} PORTUndesiredConnectionType;PORTUndesiredConnectionType490,12187
typedef struct UndesiredConnectionDataTypeUndesiredConnectionDataType493,12253
} PORTUndesiredConnectionDataType;PORTUndesiredConnectionDataType498,12412
#define	SIZE_PORTUndesiredConnectionDataType	500,12448
typedef struct XMMultipleStructXMMultipleStruct511,12820
} XMMultipleType;XMMultipleType515,12918
typedef struct SMT8BitStructSMT8BitStruct532,13482
} TLV8BitType;TLV8BitType536,13563
typedef struct SMT16BitStructSMT16BitStruct541,13612
} TLV16BitType;TLV16BitType545,13694
typedef struct SMT32BitStructSMT32BitStruct552,13848
} TLV32BitType;TLV32BitType555,13904
typedef struct SMTIdStructSMTIdStruct562,14039
} TLVIdType;TLVIdType566,14122
#define	SIZE_TLVIdType	568,14136
typedef struct TLVPathConfigStructTLVPathConfigStruct573,14238
} TLVPathConfigType;TLVPathConfigType581,14570
#define	SIZE_TLVPathConfigType	583,14592
typedef struct TLVHdrStructTLVHdrStruct593,14895
} TLVHdrType;TLVHdrType597,14982
#define	SIZE_TLVHdrType	599,14997
typedef struct TLVParamStructTLVParamStruct607,15318
} TLVParamType;TLVParamType672,16825
typedef struct TLVSetCountStructTLVSetCountStruct674,16842
} TLVSetCountType;TLVSetCountType679,16964
#define	SIZE_TLVSetCountType	681,16984
#define	SMTPVAL	702,17626
#define	SMTPARAM8	703,17662
#define	SMTPARAM16	704,17709
#define	SMTPARAM32	705,17758
#define	SMTID	706,17807
#define	SMTADDR	707,17847
#define	SMTOTHER	708,17894
#define	SMTVENDOR	709,17939
#define	SMTVENDORID	710,17986
#define	SMTVENDORDATA	711,18046
#define	SMTVEND8	712,18117
#define	SMTVEND16	713,18184
#define	SMTVEND32	714,18253
#define	SMTVENDID	715,18322
#define	SMTVENDADDR	716,18386
#define	SMTGENERAL	717,18457
#define	MACPVAL	719,18507
#define	MACINDEX	720,18550
#define	MACPARAM8	721,18604
#define	MACPARAM16	722,18656
#define	MACPARAM32	723,18710
#define	MACID	724,18764
#define	MACADDR	725,18809
#define	MACOTHER	726,18861
#define	MACVENDOR	727,18911
#define	MACVENDORID	728,18967
#define	MACVENDORDATA\729,19038
#define	MACVEND8	731,19124
#define	MACVEND16	732,19200
#define	MACVEND32	733,19278
#define	MACVENDID	734,19356
#define	MACVENDADDR	735,19429
#define	MACGENERAL	736,19509
#define	PATHPVAL	738,19564
#define	PATHINDEX	739,19607
#define	PATHPARAM8	740,19662
#define	PATHPARAM16	741,19715
#define	PATHPARAM32	742,19770
#define	PATHOTHER	743,19825
#define	PATHVENDOR	744,19876
#define	PATHVENDORID	745,19933
#define	PATHVENDORDATA\746,20005
#define	PATHVEND8	748,20092
#define	PATHVEND16	749,20169
#define	PATHVEND32	750,20248
#define	PATHVENDID	751,20327
#define	PATHVENDADDR\752,20401
#define	PATHGENERAL	754,20488
#define	PORTPVAL	756,20544
#define	PORTINDEX	757,20587
#define	PORTPARAM8	758,20642
#define	PORTPARAM16	759,20695
#define	PORTPARAM32	760,20750
#define	PORTOTHER	761,20805
#define	PORTVENDOR	762,20856
#define	PORTVENDORID	763,20913
#define	PORTVENDORDATA\764,20985
#define	PORTVEND8	766,21072
#define	PORTVEND16	767,21149
#define	PORTVEND32	768,21228
#define	PORTVENDID	769,21307
#define	PORTVENDADDR\770,21381
#define	PORTGENERAL	772,21468
#define	VSOOBJECT	789,22069
#define	VSOTYPE	790,22118
#define	VSOINDEX	791,22162
#define	VSOVAL	792,22207
#define	VSOPARAM8	793,22249
#define	VSOPARAM16	794,22303
#define	VSOPARAM32	795,22359
#define	VSOID	796,22415
#define	VSOADDR	797,22462
#define	VSOOTHER	798,22516
#define	VSOGENERAL	799,22568
#define	MADDPTR(MADDPTR810,22898
# define	CSPDATA(CSPDATA825,23348
# define	FBMDATA(FBMDATA826,23396
# define	MIBDATA(MIBDATA827,23444
# define	CSPDATA(CSPDATA829,23527
# define	FBMDATA(FBMDATA830,23560
# define	MIBDATA(MIBDATA831,23593
# define	MAX_STATION_COUNT	836,23747
# define	MCONST 844,23973
typedef MACShortAddressType MACAddr16;MACAddr16858,24360
typedef FDDILongAddressType MACAddr48;MACAddr48859,24399
typedef FDDITimeStamp SMTTimeStamp;SMTTimeStamp860,24438
typedef uInt16   PC_LSType;PC_LSType861,24474
typedef PC_LSType LineState;LineState862,24502
typedef struct SMTSetCountStructSMTSetCountStruct864,24532
} SetCountType;SetCountType868,24676
