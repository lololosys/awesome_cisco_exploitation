
ep_frm.h,1832
#define	EP_FRM_H219,8487
#define EP_OKAY	229,8854
#define EP_ERR_MEM_PH	230,8902
#define EP_ERR_MEM_PN	231,8968
#define EP_ERR_MEM_CP	232,9035
#define EP_ERR_MEM_CN	233,9103
#define EP_ERR_SIZ_PH	234,9171
#define EP_ERR_SIZ_PN	235,9239
#define EP_ERR_SIZ_CP	236,9306
#define EP_ERR_SIZ_CN	237,9358
#define OPEN_SDU	241,9515
#define LOCK_PET	242,9577
#define PASS_INTRANET	243,9635
#define PEND_CONN	247,9782
#define OPEN_NODE	248,9846
#define LOCK_NODE	249,9910
#define USED_NODE	250,9985
#define XCHG_KEYS	251,10037
#define TIME_KEYS	252,10100
#define KEY_ALLOC	253,10164
#define BAD_CONN	254,10229
#define	MG_REQ_SERVED	258,10383
#define MG_ADD_PET	259,10412
#define MG_DEL_PET	260,10464
#define MG_ADD_COT	261,10521
#define MG_DEL_COT	262,10573
#define MAX_SDU_PE	285,11493
#define DEF_PNODES	287,11556
#define DEF_PHASH	288,11616
#define DEF_CNODES	289,11676
#define DEF_CHASH	290,11736
#define DEF_LOCK	291,11802
#define DEF_OPEN	292,11857
#define DEF_INTRANET	293,11905
#define DEF_MAXKEYAGE	294,11976
#define DEF_SOKDEFS	295,12047
}	SokRng;SokRng308,12536
}	SokDef;SokDef318,12880
typedef SokDef far *SokPtr;SokPtr320,12924
struct cisco_CyDef cisco_CyDef323,13002
}	CyDef;CyDef349,13666
}	CIBdef;CIBdef361,14156
typedef CIBdef far *CIBptr;CIBptr363,14199
}	EntityID;EntityID373,14520
}	HashList;HashList380,14783
}	FreeList;FreeList387,15045
}	COTnode;COTnode398,15421
}	COTpage;COTpage409,15884
}	PETnode;PETnode420,16292
}	PEdefRec;PEdefRec428,16585
typedef PEdefRec PEdefs[PEdefs430,16634
}	cyInfo;cyInfo440,16949
}	epInfo;epInfo447,17096
}	csInfo;csInfo454,17246
}	epFrameInfo;epFrameInfo473,17830
}	MgRequest;MgRequest484,18195
#define CS_SEMAPHORE	496,18527
#define NNC_AUTO	497,18555
	#define NNC_UPE	500,18602
	#define NNC_UPE	502,18649
