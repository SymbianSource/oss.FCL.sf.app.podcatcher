ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\Podcast.mif
HEADERFILE=$(EPOCROOT)epoc32\include\Podcast.mbg
DPATH=..\data\icons

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :	
		mifconv $(ICONTARGETFILENAME) /h$(HEADERFILE) /c16,1\
		$(DPATH)\appicon.svg\
		$(DPATH)\shows\audio.svg\
		$(DPATH)\shows\audio_dl.svg\
		$(DPATH)\shows\audio_dl_active.svg\
		$(DPATH)\shows\audio_dl_active_new.svg\
		$(DPATH)\shows\audio_dl_failed.svg\
		$(DPATH)\shows\audio_dl_failed_new.svg\
		$(DPATH)\shows\audio_dl_new.svg\
		$(DPATH)\shows\audio_dl_queued.svg\
		$(DPATH)\shows\audio_dl_queued_new.svg\
		$(DPATH)\shows\audio_dl_suspended.svg\
		$(DPATH)\shows\audio_dl_suspended_new.svg\
		$(DPATH)\shows\audio_new.svg\
		$(DPATH)\shows\video.svg\
		$(DPATH)\shows\video_dl.svg\
		$(DPATH)\shows\video_dl_active.svg\
		$(DPATH)\shows\video_dl_active_new.svg\
		$(DPATH)\shows\video_dl_failed.svg\
		$(DPATH)\shows\video_dl_failed_new.svg\
		$(DPATH)\shows\video_dl_new.svg\
		$(DPATH)\shows\video_dl_queued.svg\
		$(DPATH)\shows\video_dl_queued_new.svg\
		$(DPATH)\shows\video_dl_suspended.svg\
		$(DPATH)\shows\video_dl_suspended_new.svg\
		$(DPATH)\shows\video_new.svg\
		$(DPATH)\shows\feed.svg\
		$(DPATH)\tabs\tab_feeds.svg\
		$(DPATH)\tabs\tab_new.svg\
		$(DPATH)\tabs\tab_queue.svg\
		$(DPATH)\tabs\tab_queue1.svg\
		$(DPATH)\tabs\tab_queue2.svg\
		$(DPATH)\tabs\tab_queue3.svg\
		$(DPATH)\toolbar\add_feed.svg\
		$(DPATH)\toolbar\delete.svg\
		$(DPATH)\toolbar\delete_all.svg\
		$(DPATH)\toolbar\download_all.svg\
		$(DPATH)\toolbar\download_pause.svg\
		$(DPATH)\toolbar\download_play.svg\
		$(DPATH)\toolbar\mark_new.svg\
		$(DPATH)\toolbar\mark_new_all.svg\
		$(DPATH)\toolbar\mark_old.svg\
		$(DPATH)\toolbar\mark_old_all.svg\
		$(DPATH)\toolbar\settings.svg\
		$(DPATH)\toolbar\update.svg\
		$(DPATH)\toolbar\update_all.svg\
		$(DPATH)\toolbar\update_cancel.svg\
		$(DPATH)\toolbar\download.svg		
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

