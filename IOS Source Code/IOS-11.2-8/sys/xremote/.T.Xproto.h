
Xproto.h,18503
#define XPROTO_H44,1502
#define sz_xSegment 78,3025
#define sz_xPoint 79,3047
#define sz_xRectangle 80,3067
#define sz_xArc 81,3091
#define sz_xConnClientPrefix 82,3110
#define sz_xConnSetupPrefix 83,3142
#define sz_xConnSetup 84,3172
#define sz_xPixmapFormat 85,3197
#define sz_xDepth 86,3224
#define sz_xVisualType 87,3244
#define sz_xWindowRoot 88,3270
#define sz_xTimecoord 89,3296
#define sz_xHostEntry 90,3320
#define sz_xCharInfo 91,3344
#define sz_xFontProp 92,3368
#define sz_xTextElt 93,3391
#define sz_xColorItem 94,3413
#define sz_xrgb 95,3438
#define sz_xGenericReply 96,3456
#define sz_xGetWindowAttributesReply 97,3484
#define sz_xGetGeometryReply 98,3524
#define sz_xQueryTreeReply 99,3556
#define sz_xInternAtomReply 100,3586
#define sz_xGetAtomNameReply 101,3617
#define sz_xGetPropertyReply 102,3649
#define sz_xListPropertiesReply 103,3681
#define sz_xGetSelectionOwnerReply 104,3716
#define sz_xGrabPointerReply 105,3754
#define sz_xQueryPointerReply 106,3786
#define sz_xGetMotionEventsReply 107,3819
#define sz_xTranslateCoordsReply 108,3855
#define sz_xGetInputFocusReply 109,3891
#define sz_xQueryKeymapReply 110,3925
#define sz_xQueryFontReply 111,3957
#define sz_xQueryTextExtentsReply 112,3987
#define sz_xListFontsReply 113,4024
#define sz_xGetFontPathReply 114,4054
#define sz_xGetImageReply 115,4086
#define sz_xListInstalledColormapsReply 116,4115
#define sz_xAllocColorReply 117,4158
#define sz_xAllocNamedColorReply 118,4189
#define sz_xAllocColorCellsReply 119,4225
#define sz_xAllocColorPlanesReply 120,4261
#define sz_xQueryColorsReply 121,4298
#define sz_xLookupColorReply 122,4330
#define sz_xQueryBestSizeReply 123,4362
#define sz_xQueryExtensionReply 124,4396
#define sz_xListExtensionsReply 125,4431
#define sz_xSetMappingReply 126,4466
#define sz_xGetKeyboardControlReply 127,4497
#define sz_xGetPointerControlReply 128,4536
#define sz_xGetScreenSaverReply 129,4574
#define sz_xListHostsReply 130,4609
#define sz_xSetModifierMappingReply 131,4639
#define sz_xError 132,4678
#define sz_xEvent 133,4699
#define sz_xKeymapEvent 134,4720
#define sz_xReq 135,4747
#define sz_xResourceReq 136,4765
#define sz_xCreateWindowReq 137,4791
#define sz_xChangeWindowAttributesReq 138,4822
#define sz_xChangeSaveSetReq 139,4863
#define sz_xReparentWindowReq 140,4894
#define sz_xConfigureWindowReq 141,4927
#define sz_xCirculateWindowReq 142,4961
#define sz_xInternAtomReq 143,4994
#define sz_xChangePropertyReq 144,5022
#define sz_xDeletePropertyReq 145,5055
#define sz_xGetPropertyReq 146,5088
#define sz_xSetSelectionOwnerReq 147,5118
#define sz_xConvertSelectionReq 148,5154
#define sz_xSendEventReq 149,5189
#define sz_xGrabPointerReq 150,5217
#define sz_xGrabButtonReq 151,5247
#define sz_xUngrabButtonReq 152,5276
#define sz_xChangeActivePointerGrabReq 153,5307
#define sz_xGrabKeyboardReq 154,5349
#define sz_xGrabKeyReq 155,5380
#define sz_xUngrabKeyReq 156,5406
#define sz_xAllowEventsReq 157,5434
#define sz_xGetMotionEventsReq 158,5463
#define sz_xTranslateCoordsReq 159,5497
#define sz_xWarpPointerReq 160,5531
#define sz_xSetInputFocusReq 161,5561
#define sz_xOpenFontReq 162,5593
#define sz_xQueryTextExtentsReq 163,5620
#define sz_xListFontsReq 164,5654
#define sz_xSetFontPathReq 165,5681
#define sz_xCreatePixmapReq 166,5710
#define sz_xCreateGCReq 167,5741
#define sz_xChangeGCReq 168,5768
#define sz_xCopyGCReq 169,5795
#define sz_xSetDashesReq 170,5820
#define sz_xSetClipRectanglesReq 171,5848
#define sz_xCopyAreaReq 172,5884
#define sz_xCopyPlaneReq 173,5911
#define sz_xPolyPointReq 174,5939
#define sz_xPolySegmentReq 175,5967
#define sz_xFillPolyReq 176,5997
#define sz_xPutImageReq 177,6024
#define sz_xGetImageReq 178,6051
#define sz_xPolyTextReq 179,6078
#define sz_xImageTextReq 180,6105
#define sz_xCreateColormapReq 181,6133
#define sz_xCopyColormapAndFreeReq 182,6166
#define sz_xAllocColorReq 183,6204
#define sz_xAllocNamedColorReq 184,6233
#define sz_xAllocColorCellsReq 185,6267
#define sz_xAllocColorPlanesReq 186,6301
#define sz_xFreeColorsReq 187,6336
#define sz_xStoreColorsReq 188,6365
#define sz_xStoreNamedColorReq 189,6394
#define sz_xQueryColorsReq 190,6428
#define sz_xLookupColorReq 191,6457
#define sz_xCreateCursorReq 192,6487
#define sz_xCreateGlyphCursorReq 193,6518
#define sz_xRecolorCursorReq 194,6554
#define sz_xQueryBestSizeReq 195,6586
#define sz_xQueryExtensionReq 196,6618
#define sz_xChangeKeyboardControlReq 197,6650
#define sz_xBellReq 198,6689
#define sz_xChangePointerControlReq 199,6711
#define sz_xSetScreenSaverReq 200,6750
#define sz_xChangeHostsReq 201,6783
#define sz_xListHostsReq 202,6812
#define sz_xChangeModeReq 203,6839
#define sz_xRotatePropertiesReq 204,6867
#define sz_xReply 205,6902
#define sz_xGrabKeyboardReply 206,6923
#define sz_xListFontsWithInfoReply 207,6956
#define sz_xSetPointerMappingReply 208,6994
#define sz_xGetKeyboardMappingReply 209,7032
#define sz_xGetPointerMappingReply 210,7071
#define sz_xListFontsWithInfoReq 211,7109
#define sz_xPolyLineReq 212,7144
#define sz_xPolyArcReq 213,7171
#define sz_xPolyRectangleReq 214,7197
#define sz_xPolyFillRectangleReq 215,7229
#define sz_xPolyFillArcReq 216,7265
#define sz_xPolyText8Req 217,7295
#define sz_xPolyText16Req 218,7323
#define sz_xImageText8Req 219,7352
#define sz_xImageText16Req 220,7381
#define sz_xSetPointerMappingReq 221,7411
#define sz_xForceScreenSaverReq 222,7446
#define sz_xSetCloseDownModeReq 223,7480
#define sz_xClearAreaReq 224,7514
#define sz_xSetAccessControlReq 225,7542
#define sz_xGetKeyboardMappingReq 226,7576
#define sz_xSetModifierMappingReq 227,7612
#define sz_xPropIconSize 228,7648
#define sz_xChangeKeyboardMappingReq 229,7676
#define Window 237,7964
#define Drawable 238,7986
#define Font 239,8010
#define Pixmap 240,8030
#define Cursor 241,8052
#define Colormap 242,8074
#define GContext 243,8098
#define Atom 244,8122
#define VisualID 245,8142
#define Time 246,8166
#define KeyCode 247,8186
#define KeySym 248,8208
#define X_TCP_PORT 250,8231
#define xTrue 252,8285
#define xFalse 253,8308
typedef CARD16 KeyButMask;KeyButMask256,8333
} xConnClientPrefix;xConnClientPrefix270,8723
} xConnSetupPrefix;xConnSetupPrefix278,9017
} xConnSetup;xConnSetup296,9781
} xPixmapFormat;xPixmapFormat304,9953
    } xDepth;xDepth313,10138
    } xVisualType;xVisualType326,10421
} xWindowRoot;xWindowRoot341,10987
} xTimecoord;xTimecoord354,11289
} xHostEntry;xHostEntry360,11376
} xCharInfo;xCharInfo369,11545
} xFontProp;xFontProp374,11617
} xTextElt;xTextElt383,11882
} xColorItem;xColorItem391,12055
} xrgb;xrgb396,12138
typedef CARD8 KEYCODE;KEYCODE398,12147
    } xGenericReply;xGenericReply422,12808
    } xGetWindowAttributesReply;xGetWindowAttributesReply450,13494
    } xGetGeometryReply;xGetGeometryReply464,13826
    } xQueryTreeReply;xQueryTreeReply477,14111
    } xInternAtomReply;xInternAtomReply490,14383
    } xGetAtomNameReply;xGetAtomNameReply504,14735
    } xGetPropertyReply;xGetPropertyReply517,15092
    } xListPropertiesReply;xListPropertiesReply531,15388
    } xGetSelectionOwnerReply;xGetSelectionOwnerReply544,15669
    } xGrabPointerReply;xGrabPointerReply557,15954
typedef xGrabPointerReply xGrabKeyboardReply;xGrabKeyboardReply559,15980
    } xQueryPointerReply;xQueryPointerReply571,16304
    } xGetMotionEventsReply;xGetMotionEventsReply584,16576
    } xTranslateCoordsReply;xTranslateCoordsReply597,16872
    } xGetInputFocusReply;xGetInputFocusReply610,17159
    } xQueryKeymapReply;xQueryKeymapReply618,17367
} xQueryFontReply;xQueryFontReply642,18138
    } xQueryTextExtentsReply;xQueryTextExtentsReply653,18468
    } xListFontsReply;xListFontsReply667,18764
} xListFontsWithInfoReply;xListFontsWithInfoReply691,19579
    } xGetFontPathReply;xGetFontPathReply705,19872
    } xGetImageReply;xGetImageReply718,20146
    } xListInstalledColormapsReply;xListInstalledColormapsReply732,20438
    } xAllocColorReply;xAllocColorReply745,20747
    } xAllocNamedColorReply;xAllocNamedColorReply757,21075
    } xAllocColorCellsReply;xAllocColorCellsReply770,21362
    } xAllocColorPlanesReply;xAllocColorPlanesReply782,21647
    } xQueryColorsReply;xQueryColorsReply796,21943
    } xLookupColorReply;xLookupColorReply808,22272
    } xQueryBestSizeReply;xQueryBestSizeReply821,22562
    } xQueryExtensionReply;xQueryExtensionReply837,22908
    } xListExtensionsReply;xListExtensionsReply850,23187
    } xSetMappingReply;xSetMappingReply864,23466
typedef xSetMappingReply xSetPointerMappingReply;xSetPointerMappingReply865,23490
typedef xSetMappingReply xSetModifierMappingReply;xSetModifierMappingReply866,23540
    } xGetPointerMappingReply;xGetPointerMappingReply879,23879
} xGetKeyboardMappingReply;xGetKeyboardMappingReply892,24152
} xGetModifierMappingReply;xGetModifierMappingReply905,24426
    } xGetKeyboardControlReply;xGetKeyboardControlReply917,24766
    } xGetPointerControlReply;xGetPointerControlReply931,25108
    } xGetScreenSaverReply;xGetScreenSaverReply946,25458
    } xListHostsReply;xListHostsReply960,25755
} xError;xError985,26341
#define ELFlagFocus 1020,27337
#define ELFlagSameScreen 1021,27371
} xEvent;xEvent1214,32267
    } xKeymapEvent;xKeymapEvent1223,32537
#define XEventSize 1227,32584
} xReply;xReply1277,34270
} xReq;xReq1295,34690
    } xResourceReq;xResourceReq1309,35140
} xCreateWindowReq;xCreateWindowReq1325,35499
} xChangeWindowAttributesReq;xChangeWindowAttributesReq1333,35643
} xChangeSaveSetReq;xChangeSaveSetReq1340,35771
} xReparentWindowReq;xReparentWindowReq1348,35925
} xConfigureWindowReq;xConfigureWindowReq1357,36087
} xCirculateWindowReq;xCirculateWindowReq1364,36214
} xInternAtomReq;xInternAtomReq1372,36434
} xChangePropertyReq;xChangePropertyReq1383,36697
} xDeletePropertyReq;xDeletePropertyReq1391,36839
} xGetPropertyReq;xGetPropertyReq1405,37128
} xSetSelectionOwnerReq;xSetSelectionOwnerReq1414,37288
    } xConvertSelectionReq;xConvertSelectionReq1423,37482
} xSendEventReq;xSendEventReq1439,37812
} xGrabPointerReq;xGrabPointerReq1453,38094
} xGrabButtonReq;xGrabButtonReq1467,38391
} xUngrabButtonReq;xUngrabButtonReq1476,38560
} xChangeActivePointerGrabReq;xChangeActivePointerGrabReq1486,38743
} xGrabKeyboardReq;xGrabKeyboardReq1496,38960
} xGrabKeyReq;xGrabKeyReq1507,39195
} xUngrabKeyReq;xUngrabKeyReq1516,39358
} xAllowEventsReq;xAllowEventsReq1523,39470
} xGetMotionEventsReq;xGetMotionEventsReq1531,39616
} xTranslateCoordsReq;xTranslateCoordsReq1539,39778
} xWarpPointerReq;xWarpPointerReq1549,40010
} xSetInputFocusReq;xSetInputFocusReq1557,40150
} xOpenFontReq;xOpenFontReq1566,40345
    } xQueryTextExtentsReq;xQueryTextExtentsReq1573,40459
} xListFontsReq;xListFontsReq1581,40653
typedef xListFontsReq xListFontsWithInfoReq;xListFontsWithInfoReq1583,40671
} xSetFontPathReq;xSetFontPathReq1591,40879
} xCreatePixmapReq;xCreatePixmapReq1600,41056
} xCreateGCReq;xCreateGCReq1609,41219
} xChangeGCReq;xChangeGCReq1617,41351
} xCopyGCReq;xCopyGCReq1625,41501
} xSetDashesReq;xSetDashesReq1634,41717
} xSetClipRectanglesReq;xSetClipRectanglesReq1642,41874
} xClearAreaReq;xClearAreaReq1651,42064
} xCopyAreaReq;xCopyAreaReq1661,42307
} xCopyPlaneReq;xCopyPlaneReq1672,42578
} xPolyPointReq;xPolyPointReq1680,42727
typedef xPolyPointReq xPolyLineReq;xPolyLineReq1682,42749
} xPolySegmentReq;xPolySegmentReq1692,43035
typedef xPolySegmentReq xPolyArcReq;xPolyArcReq1694,43059
typedef xPolySegmentReq xPolyRectangleReq;xPolyRectangleReq1695,43096
typedef xPolySegmentReq xPolyFillRectangleReq;xPolyFillRectangleReq1696,43139
typedef xPolySegmentReq xPolyFillArcReq;xPolyFillArcReq1697,43186
typedef struct _FillPolyReq _FillPolyReq1699,43228
} xFillPolyReq;xFillPolyReq1708,43419
typedef struct _PutImageReq _PutImageReq1711,43441
} xPutImageReq;xPutImageReq1722,43699
} xGetImageReq;xGetImageReq1732,43908
} xPolyTextReq;xPolyTextReq1743,44172
typedef xPolyTextReq xPolyText8Req;xPolyText8Req1745,44193
typedef xPolyTextReq xPolyText16Req;xPolyText16Req1746,44229
} xImageTextReq;xImageTextReq1755,44415
typedef xImageTextReq xImageText8Req;xImageText8Req1757,44437
typedef xImageTextReq xImageText16Req;xImageText16Req1758,44475
} xCreateColormapReq;xCreateColormapReq1767,44660
} xCopyColormapAndFreeReq;xCopyColormapAndFreeReq1775,44808
} xAllocColorReq;xAllocColorReq1784,44998
} xAllocNamedColorReq;xAllocNamedColorReq1793,45190
} xAllocColorCellsReq;xAllocColorCellsReq1801,45356
} xAllocColorPlanesReq;xAllocColorPlanesReq1809,45540
} xFreeColorsReq;xFreeColorsReq1817,45691
} xStoreColorsReq;xStoreColorsReq1824,45810
    } xStoreNamedColorReq;xStoreNamedColorReq1834,46103
} xQueryColorsReq;xQueryColorsReq1841,46227
} xLookupColorReq;xLookupColorReq1850,46483
} xCreateCursorReq;xCreateCursorReq1861,46764
} xCreateGlyphCursorReq;xCreateGlyphCursorReq1872,47060
} xRecolorCursorReq;xRecolorCursorReq1881,47292
} xQueryBestSizeReq;xQueryBestSizeReq1893,47535
} xQueryExtensionReq;xQueryExtensionReq1901,47728
} xSetModifierMappingReq;xSetModifierMappingReq1907,47844
} xSetPointerMappingReq;xSetPointerMappingReq1913,47983
} xGetKeyboardMappingReq;xGetKeyboardMappingReq1922,48146
} xChangeKeyboardMappingReq;xChangeKeyboardMappingReq1931,48332
} xChangeKeyboardControlReq;xChangeKeyboardControlReq1938,48456
} xBellReq;xBellReq1944,48586
} xChangePointerControlReq;xChangePointerControlReq1953,48782
} xSetScreenSaverReq;xSetScreenSaverReq1962,48983
} xChangeHostsReq;xChangeHostsReq1971,49147
    } xListHostsReq;xListHostsReq1977,49244
    } xChangeModeReq;xChangeModeReq1983,49340
typedef xChangeModeReq xSetAccessControlReq;xSetAccessControlReq1985,49363
typedef xChangeModeReq xSetCloseDownModeReq;xSetCloseDownModeReq1986,49408
typedef xChangeModeReq xForceScreenSaverReq;xForceScreenSaverReq1987,49453
    } xRotatePropertiesReq;xRotatePropertiesReq1996,49675
#define X_Reply	2002,49730
#define X_Error	2003,49769
#define X_CreateWindow 2007,49823
#define X_ChangeWindowAttributes 2008,49879
#define X_GetWindowAttributes 2009,49929
#define X_DestroyWindow 2010,49976
#define X_DestroySubwindows 2011,50018
#define X_ChangeSaveSet 2012,50063
#define X_ReparentWindow 2013,50105
#define X_MapWindow 2014,50147
#define X_MapSubwindows 2015,50189
#define X_UnmapWindow 2016,50231
#define X_UnmapSubwindows 2017,50273
#define X_ConfigureWindow 2018,50317
#define X_CirculateWindow 2019,50361
#define X_GetGeometry 2020,50405
#define X_QueryTree 2021,50447
#define X_InternAtom 2022,50489
#define X_GetAtomName 2023,50531
#define X_ChangeProperty 2024,50573
#define X_DeleteProperty 2025,50616
#define X_GetProperty 2026,50659
#define X_ListProperties 2027,50701
#define X_SetSelectionOwner 2028,50744
#define X_GetSelectionOwner 2029,50790
#define X_ConvertSelection 2030,50836
#define X_SendEvent 2031,50881
#define X_GrabPointer 2032,50923
#define X_UngrabPointer 2033,50965
#define X_GrabButton 2034,51007
#define X_UngrabButton 2035,51049
#define X_ChangeActivePointerGrab 2036,51091
#define X_GrabKeyboard 2037,51143
#define X_UngrabKeyboard 2038,51185
#define X_GrabKey 2039,51228
#define X_UngrabKey 2040,51270
#define X_AllowEvents 2041,51312
#define X_GrabServer 2042,51361
#define X_UngrabServer 2043,51409
#define X_QueryPointer 2044,51459
#define X_GetMotionEvents 2045,51509
#define X_TranslateCoords 2046,51562
#define X_WarpPointer 2047,51620
#define X_SetInputFocus 2048,51669
#define X_GetInputFocus 2049,51720
#define X_QueryKeymap 2050,51771
#define X_OpenFont 2051,51820
#define X_CloseFont 2052,51866
#define X_QueryFont 2053,51913
#define X_QueryTextExtents 2054,51955
#define X_ListFonts 2055,52002
#define X_ListFontsWithInfo 2056,52046
#define X_SetFontPath 2057,52089
#define X_GetFontPath 2058,52132
#define X_CreatePixmap 2059,52175
#define X_FreePixmap 2060,52225
#define X_CreateGC 2061,52273
#define X_ChangeGC 2062,52319
#define X_CopyGC 2063,52365
#define X_SetDashes 2064,52409
#define X_SetClipRectangles 2065,52456
#define X_FreeGC 2066,52511
#define X_ClearArea 2067,52555
#define X_CopyArea 2068,52610
#define X_CopyPlane 2069,52656
#define X_PolyPoint 2070,52703
#define X_PolyLine 2071,52750
#define X_PolySegment 2072,52796
#define X_PolyRectangle 2073,52845
#define X_PolyArc 2074,52896
#define X_FillPoly 2075,52941
#define X_PolyFillRectangle 2076,52987
#define X_PolyFillArc 2077,53042
#define X_PutImage 2078,53091
#define X_GetImage 2079,53137
#define X_PolyText8 2080,53180
#define X_PolyText16 2081,53227
#define X_ImageText8 2082,53275
#define X_ImageText16 2083,53323
#define X_CreateColormap 2084,53372
#define X_FreeColormap 2085,53424
#define X_CopyColormapAndFree 2086,53474
#define X_InstallColormap 2087,53531
#define X_UninstallColormap 2088,53584
#define X_ListInstalledColormaps 2089,53639
#define X_AllocColor 2090,53699
#define X_AllocNamedColor 2091,53747
#define X_AllocColorCells 2092,53800
#define X_AllocColorPlanes 2093,53853
#define X_FreeColors 2094,53907
#define X_StoreColors 2095,53955
#define X_StoreNamedColor 2096,54004
#define X_QueryColors 2097,54057
#define X_LookupColor 2098,54106
#define X_CreateCursor 2099,54155
#define X_CreateGlyphCursor 2100,54205
#define X_FreeCursor 2101,54260
#define X_RecolorCursor 2102,54308
#define X_QueryBestSize 2103,54359
#define X_QueryExtension 2104,54410
#define X_ListExtensions 2105,54462
#define X_ChangeKeyboardMapping 2106,54514
#define X_GetKeyboardMapping 2107,54557
#define X_ChangeKeyboardControl 2108,54600
#define X_GetKeyboardControl 2109,54659
#define X_Bell 2110,54715
#define X_ChangePointerControl 2111,54758
#define X_GetPointerControl 2112,54801
#define X_SetScreenSaver 2113,54844
#define X_GetScreenSaver 2114,54897
#define X_ChangeHosts 2115,54950
#define X_ListHosts 2116,55000
#define X_SetAccessControl 2117,55048
#define X_SetCloseDownMode 2118,55106
#define X_KillClient 2119,55149
#define X_RotateProperties	2120,55193
#define X_ForceScreenSaver	2121,55231
#define X_SetPointerMapping 2122,55269
#define X_GetPointerMapping 2123,55312
#define X_SetModifierMapping	2124,55355
#define X_GetModifierMapping	2125,55395
#define X_NoOperation 2126,55435
