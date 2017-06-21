# Declare macro definitions.

c       =
l       =

S	= ..\lat\		#LAT source directory

CME	= ..\cme\		#CME source directory

laths	= $(S)laths.h
latas	= $(S)latas.h
latcb	= $(S)latcb.h
lathi	= $(S)lathi.h
latls	= $(S)latls.h
latnd	= $(S)latnd.h
latob	= $(S)latob.h
latqr	= $(S)latqr.h
latsb	= $(S)latsb.h
laterr  = $(S)laterr.h
latmsg  = $(S)latmsg.h
latslt  = $(S)latslt.h

cmssys	= $(CME)cmesys.h

INC	= mtcsys.h latfnc.h sysfnc.h
COMPILE = TCC -c -ms -K -O -G -Z -I$(S);$(CME) -wuse -wrvl -wamb -wpro $(c) -o$*.OBJ $(S)$*.C

# Define LAT configuration options. Set option to 1 to enable the
# feature, zero to disable.

LE_HOST		= 1
LE_HOST_AS	= 1

LE_SERVER	= 1
LE_SERVER_LS	= 1

LE_SUBJECT_HIC	= 1
LE_OBJECT_HIC	= 1

LE_OBJECTS	= 1
LE_HS_STATS	= 1
LE_ND_STATS	= 1
LE_ND_RETAIN	= 1
LE_LS_DUPS	= 1
LE_UPPER	= 1
LE_CME		= 0

x_lat	      = latcom.obj aclclr.obj aclchk.obj aclcmp.obj aclset.obj \ 
		idlchk.obj idlclr.obj idlcmp.obj idlset.obj ticelp.obj \
		tocelp.obj ifchk.obj  ifmsg.obj  cbabo.obj  cbcre.obj  \
		cbdel.obj  cbhalt.obj cborun.obj cbostr.obj cbrout.obj \
		cbrxmt.obj ndcre.obj  nddel.obj  ndfail.obj ndlist.obj \
		ndshow.obj sb1inp.obj sb1out.obj sbcre.obj  sbdel.obj  \
		sbfind.obj sbsig.obj  sbstop.obj dbchgc.obj dbrepb.obj \
		dbrepc.obj dbreps.obj dbsetc.obj dbsetm.obj


!if $(LE_HOST)
x_host	      = cbhrun.obj cbhstr.obj sbacc.obj  sbrej.obj
!else
x_host 	      =
!endif

!if $(LE_SERVER)
x_server      = cbsrun.obj cbsstr.obj
!else
x_server      =	
!endif

!if $(LE_HS_STATS)
x_hs_stats    = hszero.obj
!else
x_hs_stats    =
!endif

!if $(LE_LS_DUPS)
x_ls_dups     = sbdups.obj
!else
x_ls_dups     =
!endif

!if $(LE_ND_STATS)
x_nd_stats    = ndzero.obj
!else
x_nd_stats    =
!endif

!if $(LE_ND_RETAIN)
x_nd_retain   = ndpur.obj
!else
x_nd_retain   =
!endif

!if $(LE_HOST_AS)
x_host_as     = ascre.obj  asdel.obj  aslist.obj asmsg.obj  asrset.obj \
		assetf.obj asseti.obj asshow.obj sbstr.obj
!else
x_host_as     = 
!endif

!if $(LE_SERVER_LS)
x_server_ls   = lscre.obj  lsdel.obj  lsdups.obj lslist.obj lslook.obj \
		lsmsg.obj  lsperm.obj lsrem.obj  lsrset.obj lsscan.obj \
		lsshow.obj lstmo.obj  sbopen.obj
!else
x_server_ls   = 
!endif

!if $(LE_OBJECTS)
x_objects     = obclon.obj obcre.obj  obdel.obj  obfind.obj oblist.obj \
    		obrel.obj  obren.obj  obrset.obj obsetf.obj obshow.obj
!else
x_objects     =
!endif

!if $(LE_OBJECTS) && $(LE_HOST_AS)
x_as_ob	      = asclro.obj asseto.obj
!else
x_as_ob	      =
!endif

!if $(LE_CME) && $(LE_HOST_AS)
x_as_cm	      = asclrp.obj assetp.obj
!else
x_as_cm	      =

!if ($(LE_OBJECTS) || $(LE_CME)) && $(LE_OBJECT_HIC)
x_queue       = qrcre.obj  qrdel.obj  qrfind.obj qrlist.obj qrscan.obj
!else
x_queue       =
!endif

!if $(LE_SUBJECT_HIC) | $(LE_OBJECT_HIC)
x_hic         = hicre.obj  hidel.obj  hifind.obj hiopen.obj hirej.obj \
		hises.obj histr.obj
!else
x_hic         =
!endif

!if $(LE_SUBJECT_HIC)
x_host_hic    = hiecan.obj hierpt.obj hihra.obj hihri.obj  hihsts.obj \
		hiinit.obj hiocmd.obj hiosi.obj hiqrpt.obj hiqsts.obj \
		sbria.obj
!else
x_host_hic    =
!endif

!if $(LE_OBJECT_HIC)
x_server_hic  = hioreq.obj hiora.obj  hiori.obj  hiosts.obj hiscmd.obj \
		hissi.obj
!else
x_server_hic  =
!endif


#
# Build LAT.EXE
#
lat.exe	: 		\
    $(x_lat)		\
    $(x_host)		\
    $(x_server)		\
    $(x_ls_dups)	\
    $(x_hs_stats)	\
    $(x_nd_stats)	\
    $(x_nd_retain)      \
    $(x_host_as)        \
    $(x_server_ls)	\
    $(x_objects)	\
    $(x_as_ob)          \
    $(x_as_cm)		\
    $(x_queue)          \
    $(x_hic)		\
    $(x_host_hic)	\
    $(x_server_hic)
	
#
# LAT common module.
#
latcom.obj	: $(S)latcom.c $(INC)	$(laths)  $(latas)  $(latcb)  \
    					$(lathi)  $(latls)  $(latnd)  \
    					$(latob)  $(latqr)  $(latsb)  \
    					$(latmsg) $(latslt)
    $(COMPILE)

#
# ACL, IDL functions.
#
aclchk.obj	: $(S)aclchk.c  $(INC)  $(latnd)  $(latmsg)
    $(COMPILE

aclclr.obj	: $(S)aclclr.c  $(INC)	$(laths)  $(latmsg)
    $(COMPILE)

aclcmp.obj	: $(S)aclcmp.c $(INC)	$(laths)
    $(COMPILE)

aclset.obj	: $(S)aclset.c $(INC)	$(laths)  $(latmsg)
    $(COMPILE)

idlchk.obj	: $(S)idlchk.c $(INC)	$(laths)  $(latnd)  $(latob)
    $(COMPILE)

idlclr.obj	: $(S)idlclr.c $(INC)	$(laths)  $(latob)
    $(COMPILE)

idlcmp.obj	: $(S)idlcmp.c $(INC)	$(laths)  $(latob)
    $(COMPILE)

idlset.obj	: $(S)idlset.c $(INC)	$(laths)  $(latob)
    $(COMPILE)

#
# Timer functions.
#
ticelp.obj	: $(S)ticelp.c $(INC)	$(laths)  $(latcb)  $(lathi)  \
    					$(latmsg)
    $(COMPILE)

tocelp.obj	: $(S)tocelp.c $(INC)	$(laths)  $(lathi)  $(latnd)  \
					$(latqr)
    $(COMPILE)

#
# Message processing functions.
#
ifchk.obj	: $(S)ifchk.c  $(INC)	$(laths)  $(latnd)  $(latmsg) \
    					$(latslt)
    $(COMPILE)

ifmsg.obj	: $(S)ifmsg.c  $(INC)	$(laths)  $(latcb)  $(latmsg)
    $(COMPILE)

#
# Circuit functions.
#
cbabo.obj	: $(S)cbabo.c  $(INC)	$(latcb)  $(latnd)  $(latmsg)
    $(COMPILE)

cbcre.obj	: $(S)cbcre.c  $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(mtcerr)
    $(COMPILE)

cbdel.obj	: $(S)cbdel.c  $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(latsb)  $(latmsg)
    $(COMPILE)

cbhalt.obj	: $(S)cbhalt.c  $(INC)	$(latmsg)
    $(COMPILE)

cbhrun.obj	: $(S)cbhrun.c $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(latsb)  $(latmsg) $(latslt)
    $(COMPILE)

cbhstr.obj	: $(S)cbhstr.c $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(latmsg)
    $(COMPILE)

cborun.obj	: $(S)cborun.c $(INC)	$(laths)  $(latcb)  $(latnd)  \
					$(latsb)  $(latmsg) $(latslt)
    $(COMPILE)

cbostr.obj	: $(S)cbostr.c $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(latmsg)
    $(COMPILE)

cbrout.obj	: $(S)cbrout.c $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(latmsg) $(latslt)
    $(COMPILE)

cbrxmt.obj	: $(S)cbrxmt.c $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(latmsg)
    $(COMPILE)

cbsrun.obj	: $(S)cbsrun.c $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(latsb)  $(latmsg) $(latslt)
    $(COMPILE)

cbsstr.obj	: $(S)cbsstr.c $(INC)	$(latcb)  $(latnd)  $(latmsg)
    $(COMPILE)

#
# Session functions.
#
sb1inp.obj	: $(S)sb1inp.c $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(latob)  $(latsb)  $(latmsg) \
    					$(latslt)
    $(COMPILE)

sb1out.obj	: $(S)sb1out.c $(INC)	$(laths)  $(latcb)  $(latob)  \
    					$(latsb)  $(latmsg) $(latslt) 
					
    $(COMPILE)

sbacc.obj	: $(S)sbacc.c  $(INC)	$(latsb)
    $(COMPILE)

sbcre.obj	: $(S)sbcre.c  $(INC)	$(laths)  $(latcb)  $(latnd)  \
    					$(latsb)  $(mtcerr)
    $(COMPILE)

sbdel.obj	: $(S)sbdel.c  $(INC)	$(laths)  $(latcb)  $(latnd)  \
					$(latob)  $(latsb)
    $(COMPILE)

sbdups.obj	: $(S)sbdups.c  $(INC)	$(laths)  $(latcb)  $(latls)  \
    					$(latnd)  $(latsb)  $(latslt)
    $(COMPILE)

sbfind.obj	: $(S)sbfind.c $(INC)	$(laths)  $(latcb)  $(latsb)  \
    					$(mtcerr)
    $(COMPILE)

sbopen.obj	: $(S)sbopen.c $(INC)	$(laths)  $(latcb)  $(latls)  \
    					$(latnd)  $(latob)  $(latsb)  \
					$(mtcerr)
    $(COMPILE)

sbrej.obj	: $(S)sbrej.c  $(INC)	$(latsb)
    $(COMPILE)

sbria.obj	: $(S)sbria.c  $(INC)   $(latsb)  $(lathi)  $(latls)  \
		  			$(latnd)  $(latcb)  $(latmsg) \
					$(latslt)

sbsig.obj	: $(S)sbsig.c  $(INC)	$(laths)  $(latcb)  $(latsb)
    $(COMPILE)

sbstop.obj	: $(S)sbstop.c $(INC)	$(latob)  $(latsb)
    $(COMPILE)

sbstr.obj	: $(S)sbstr.c  $(INC)	$(laths)  $(latas)  $(latob)  \
					$(latsb)  $(latmsg) $(latslt)
    $(COMPILE)

#
# Advertised service functions.
#
asclro.obj	: $(S)asclro.c $(INC)	$(latas)  $(latob)
    $(COMPILE)

ascre.obj	: $(S)ascre.c  $(INC)	$(laths)  $(latas)  $(mtcerr) \
    					$(latmsg)
    $(COMPILE)

asdel.obj	: $(S)asdel.c  $(INC)	$(laths)  $(latas)  $(latmsg)
    $(COMPILE)

aslist.obj	: $(S)aslist.c $(INC)	$(laths)  $(latas)  $(mtcerr)
    $(COMPILE)

asmsg.obj	: $(S)asmsg.c  $(INC)	$(laths)  $(latas)  $(latmsg)
    $(COMPILE)

asrset.obj	: $(S)asrset.c $(INC)	$(laths)  $(latas)
    $(COMPILE)

assetf.obj	: $(S)assetf.c $(INC)	$(laths)  $(latas)  $(mtcerr) \
    					$(latmsg)
    $(COMPILE)

asseti.obj	: $(S)asseti.c $(INC)	$(laths)  $(latas)  $(mtcerr) \
    					$(latmsg)
    $(COMPILE)

asseto.obj	: $(S)asseto.c $(INC)	$(latas)  $(latob)  $(mtcerr)
    $(COMPILE)

asshow.obj	: $(S)asshow.c $(INC)	$(laths)  $(latas)  $(mtcerr)
    $(COMPILE)

#
# Advertised service/CME functions.
#
asclrp.obj	: $(S)asclrp.c $(INC)	$(latas)  $(cmesys)
    $(COMPILE)

assetp.obj	: $(S)assetp.c $(INC)  S(latas)	  $(cmesys)
    $(COMPILE)

#
# Host\initiated functions.
#
hicre.obj	: $(S)hicre.c  $(INC)	$(laths)  $(lathi)  $(mtcerr)
    $(COMPILE)

hidel.obj	: $(S)hidel.c  $(INC)	$(laths)  $(lathi)  $(latob)  \
    					$(latqr)  $(latmsg)
    $(COMPILE)

hiecan.obj	: $(S)hiecan.c $(INC)	$(laths)  $(lathi)  $(latob)  \
    					$(latmsg)
    $(COMPILE)

hierpt.obj	: $(S)hierpt.c $(INC)	$(laths)  $(lathi)  $(latmsg)
    $(COMPILE)

hifind.obj	: $(S)hifind.c $(INC)	$(laths)  $(lathi)  $(mtcerr)
    $(COMPILE)

hihra.obj	: $(S)hihra.c  $(INC)   $(laths)  $(lathi)  $(latmsg) \
					$(latslt)
    $(COMPILE)

hihri.obj	: $(S)hihri.c  $(INC)	$(laths)  $(lathi)  $(latmsg) \
					$(latslt)
    $(COMPILE)

hihsts.obj	: $(S)hihsts.c $(INC)	$(laths)  $(lathi)  $(latmsg)
    $(COMPILE)

hiinit.obj	: $(S)hiinit.c $(INC)	$(laths)  $(lathi)  $(latnd)  \
    					$(latob)  $(mtcerr) $(latmsg)
    $(COMPILE)

hiocmd.obj	: $(S)hiocmd.c $(INC)	$(laths)  $(lathi)  $(latob)  \
    					$(latmsg)
    $(COMPILE)

hiopen.obj	: $(S)hiopen.c $(INC)	$(laths)  $(latcb)  $(lathi)  \
    					$(latnd)  $(latsb)  $(mtcerr) \
					$(latmsg) $(latslt)
    $(COMPILE)

hiora.obj	: $(S)hiora.c  $(INC)	$(laths)  $(latls)  $(lathi)  \
					$(latnd)  $(latmsg)
    $(COMPILE)

hioreq.obj	: $(S)hioreq.c $(INC)	$(laths)  $(lathi)  $(latmsg)
    $(COMPILE)

hiori.obj	: $(S)hiori.c  $(INC)	$(laths)  $(latas)  $(lathi)  \
    					$(latmsg)
    $(COMPILE)

hiosi.obj	: $(S)hiosi.c  $(INC)	$(laths)  $(lathi)  $(latob)  \
    					$(latmsg)
    $(COMPILE)

hiosts.obj	: $(S)hiosts.c $(INC)	$(laths)  $(lathi)  $(latmsg)
    $(COMPILE)

hiqrpt.obj	: $(S)hiqrpt.c $(INC)	$(laths)  $(lathi)  $(latnd)  \
					$(latmsg)
    $(COMPILE)

hiqsts.obj	: $(S)hiqsts.c $(INC)	$(laths)  $(lathi)  $(latnd)  \
					$(latmsg)
    $(COMPILE)

hirej.obj	: $(S)hirej.c  $(INC)   $(laths)  $(lathi)  $(latnd)  \
    					$(latmsg)
    $(COMPILE)
    
hiscmd.obj	: $(S)hiscmd.c $(INC)	$(laths)  $(lathi)  $(latnd)  \
    					$(latqr)  $(latmsg) $(latslt)
    $(COMPILE)

hises.obj	: $(S)hises.c $(INC)	$(laths)  $(lathi)  $(mtcerr)
    $(COMPILE)

hissi.obj	: $(S)hissi.c  $(INC)	$(laths)  $(lathi)  $(latmsg)
    $(COMPILE)

histr.obj	: $(S)histr.c  $(INC)   $(laths)  $(lathi)  $(latsb)   \
    					$(latslt)
    $(COMPILE)

#
# Host statistic functions.
#
hszero.obj	: $(S)hszero.c $(INC)	$(laths)
    $(COMPILE)

#
# Learned service functions.
#
lscre.obj	: $(S)lscre.c  $(INC)	$(laths)  $(latls)  $(latnd)  \
    					$(mtcerr) $(latmsg)
    $(COMPILE)

lsdel.obj	: $(S)lsdel.c  $(INC)	$(laths)  $(latls)  $(latnd)
    $(COMPILE)

lsdups.obj	: $(S)lsdups.c $(INC)	$(laths)  $(latls)  $(latnd) \
    					$(latob)
    $(COMPILE)

lslist.obj	: $(S)lslist.c $(INC)	$(laths)  $(latls)  $(latnd) \
					$(latob)  $(mtcerr)
    $(COMPILE)

lslook.obj	: $(S)lslook.c $(INC)	$(latls)
    $(COMPILE)

lsmsg.obj	: $(S)lsmsg.c  $(INC)	$(laths)  $(latls)  $(latnd)  \
    					$(latob)  $(latmsg)
    $(COMPILE)

lsperm.obj	: $(S)lsperm.c $(INC)	$(laths)  $(latls)  $(latnd)
    $(COMPILE)

lsrem.obj	: $(S)lsrem.c  $(INC)	$(latls)  $(latnd)  $(latob)
    $(COMPILE)

lsrset.obj	: $(S)lsrset.c $(INC)	$(laths)  $(latls)  $(latnd)
    $(COMPILE)

lsscan.obj	: $(S)lsscan.c $(INC)	$(laths)  $(latls)  $(latnd)  \
					$(latob)  $(mtcerr)
    $(COMPILE)

lsshow.obj	: $(S)lsshow.c $(INC)	$(laths)  $(latls)  $(latnd)  \
					$(latob)  $(mtcerr)
    $(COMPILE)

lstmo.obj	: $(S)lstmo.c  $(INC)	$(laths)  $(latls)  $(latnd)
    $(COMPILE)

#
# Node functions.
#
ndcre.obj	: $(S)ndcre.c  $(INC)	$(laths)  $(latnd)  $(mtcerr)
    $(COMPILE)

nddel.obj	: $(S)nddel.c  $(INC)	$(laths)  $(latnd)
    $(COMPILE)

ndfail.obj	: $(S)ndfail.c $(INC)	$(latls)  $(latnd)
    $(COMPILE)

ndlist.obj	: $(S)ndlist.c $(INC)	$(laths)  $(latnd)  $(mtcerr)
    $(COMPILE)

ndpur.obj	: $(S)ndpur.c  $(INC)	$(laths)  $(latls)  $(latnd)
    $(COMPILE)

ndshow.obj	: $(S)ndshow.c $(INC)	$(laths)  $(latnd)  $(mtcerr)
    $(COMPILE)

ndzero.obj	: $(S)ndzero.c $(INC)	$(laths)  $(latnd)
    $(COMPILE)

#
# Object functions.
#
obclon.obj	: $(S)obclon.c  $(INC)	$(latob)  $(mtcerr)
    $(COMPILE)

obcre.obj	: $(S)obcre.c  $(INC)	$(latob)  $(mtcerr)
    $(COMPILE)

obdel.obj	: $(S)obdel.c  $(INC)	$(laths)  $(latas)  $(latcb)  \
					$(lathi)  $(latob)  $(latsb)  \
					$(latmsg) $(latslt)
    $(COMPILE)

obfind.obj	: $(S)obfind.c $(INC)	$(latob)  $(mtcerr)
    $(COMPILE)

oblist.obj	: $(S)oblist.c $(INC)	$(latob)  $(mtcerr)
    $(COMPILE)

obrel.obj	: $(S)obrel.c  $(INC)	$(latob)
    $(COMPILE)

obren.obj	: $(S)obren.c  $(INC)	$(latob)  $(mtcerr)
    $(COMPILE)


obrset.obj	: $(S)obrset.c $(INC)	$(latob)
    $(COMPILE)


obsetf.obj	: $(S)obsetf.c $(INC)	$(latob)
    $(COMPILE)

obshow.obj	: $(S)obshow.c $(INC)	$(latob)  $(mtcerr)
    $(COMPILE)

#
# Queued request functions.
#
qrcre.obj	: $(S)qrcre.c  $(INC)	$(lathi)  $(latqr)  $(mtcerr)
    $(COMPILE)


qrdel.obj	: $(S)qrdel.c  $(INC)	$(lathi)  $(latqr)  $(latmsg)
    $(COMPILE)

qrfind.obj	: $(S)qrfind.c $(INC)	$(lathi)  $(latqr)  $(mtcerr)
    $(COMPILE)

qrlist.obj	: $(S)qrlist.c $(INC)	$(lathi)  $(latqr)  $(mtcerr)
    $(COMPILE)

qrscan.obj	: $(S)qrscan.c $(INC)	$(latas)  $(lathi)  $(latob)  \
					$(latqr)  $(latmsg) $(latslt)
    $(COMPILE)

#
# DATA-B functions.
#
dbchgc.obj	: $(S)dbchgc.c $(INC)	$(laths)  $(latcb)  $(latob)  \
    					$(latsb)  $(latslt)
    $(COMPILE)

dbrepb.obj	: $(S)dbrepb.c $(INC)	$(latob)  $(latsb)  $(latslt)
    $(COMPILE)

dbrepc.obj	: $(S)dbrepc.c $(INC)	$(latsb)
    $(COMPILE)

dbreps.obj	: $(S)dbreps.c $(INC)	$(latob)  $(latsb)
    $(COMPILE)

dbsetc.obj	: $(S)dbsetc.c $(INC)	$(laths)  $(latob)  $(latsb)  \
					$(latslt)
    $(COMPILE)

dbsetm.obj	: $(S)dbsetm.c $(INC)	$(latsb)  $(latslt)
    $(COMPILE)

