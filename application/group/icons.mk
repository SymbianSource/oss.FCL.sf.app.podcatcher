ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\Podcast.mif
HEADERFILE=$(EPOCROOT)epoc32\include\Podcast.mbg
DPATH=..\data\images
TPATH=..\data\images\toolbars

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
		$(DPATH)\Audio.svg\
		$(DPATH)\Audio_new.svg\
		$(DPATH)\Audio_queued.svg\
		$(DPATH)\Audio_downloading.svg\
		$(DPATH)\Audio_downloaded.svg\
		$(DPATH)\Audio_downloaded_new.svg\
		$(DPATH)\Audio_failed.svg\
		$(DPATH)\Audio_suspended.svg\
		$(DPATH)\Video.svg\
		$(DPATH)\Video_new.svg\
		$(DPATH)\Video_queued.svg\
		$(DPATH)\Video_downloading.svg\
		$(DPATH)\Video_downloaded.svg\
		$(DPATH)\Video_downloaded_new.svg\
		$(DPATH)\Video_failed.svg\
		$(DPATH)\Video_suspended.svg\
		$(DPATH)\Feed.svg\
		$(TPATH)\add_pod.svg\
		$(TPATH)\delete.svg\
		$(TPATH)\dwn_pause.svg\
		$(TPATH)\dwn_start.svg\
		$(TPATH)\reload.svg\
		$(TPATH)\reload_all.svg\
		$(TPATH)\settings.svg\
		$(TPATH)\cancel.svg\
		$(TPATH)\remove.svg\
		$(TPATH)\remove_all.svg\
		$(TPATH)\download.svg\
		$(TPATH)\markold.svg\
		$(TPATH)\marknew.svg\

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

