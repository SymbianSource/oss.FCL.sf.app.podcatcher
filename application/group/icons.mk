ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\Podcast.mif
HEADERFILE=$(EPOCROOT)epoc32\include\Podcast.mbg
DPATH=..\data\images

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :	
		mifconv $(ICONTARGETFILENAME) /h$(HEADERFILE) /c16,1\
		$(DPATH)\podcatcher_64px.svg\
		$(DPATH)\new\Audio.svg\
		$(DPATH)\new\Audio_new.svg\
		$(DPATH)\new\Audio_queued.svg\
		$(DPATH)\new\Audio_downloading.svg\
		$(DPATH)\new\Audio_downloaded.svg\
		$(DPATH)\new\Audio_downloaded_new.svg\
		$(DPATH)\new\Audio_failed.svg\
		$(DPATH)\new\Audio_suspended.svg\
		$(DPATH)\new\Feed.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

