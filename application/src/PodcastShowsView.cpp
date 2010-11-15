/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "PodcastShowsView.h"
#include "PodcastAppUi.h"
#include "ShowEngine.h"
#include "SettingsEngine.h"
#include "PodcastApp.h"
#include "imagehandler.h"
#include "constants.h"

#include <akntitle.h>
#include <podcast.rsg>
#include <podcast.mbg>
#include <gulicon.h>
#include <barsread.h>
#include <aknnotedialog.h>
#include <aknmessagequerydialog.h>

#define KMaxMessageLength 200

_LIT(KSizeDownloadingOf, "%.1f/%.1f MB");
_LIT(KShowsSizeFormatS60, "%.1f MB");

_LIT(KShowFormat, "%d\t%S\t%S%S");
_LIT(KShowFormatLandscape, "%d\t%S");

// these must correspond with TShowsIconIndex

const TUint KShowIconArrayIds[] =
	{
	//	EShowIcon = 0,
	EMbmPodcastAudio,
	EMbmPodcastAudio_mask,
	//	EShowIconNew,
	EMbmPodcastAudio_new,
	EMbmPodcastAudio_new_mask,
	//	EQuedShowIcon,
	EMbmPodcastAudio_dl_queued,
	EMbmPodcastAudio_dl_queued_mask,
	//	EQuedShowIconNew,
	EMbmPodcastAudio_dl_queued_new,
	EMbmPodcastAudio_dl_queued_new_mask,
	//	EDownloadingShowIcon,
	EMbmPodcastAudio_dl_active,
	EMbmPodcastAudio_dl_active_mask,
	//	EDownloadingShowIconNew,
	EMbmPodcastAudio_dl_active_new,
	EMbmPodcastAudio_dl_active_new_mask,
	//	EDownloadedShowIcon,
	EMbmPodcastAudio_dl,
	EMbmPodcastAudio_dl_mask,
	//	EDownloadedShowIconNew,
	EMbmPodcastAudio_dl_new,
	EMbmPodcastAudio_dl_new_mask,
	//	EFailedShowIcon,
	EMbmPodcastAudio_dl_failed,
	EMbmPodcastAudio_dl_failed_mask,
	//	EFailedShowIconNew,
	EMbmPodcastAudio_dl_failed_new,
	EMbmPodcastAudio_dl_failed_new_mask,
	//	ESuspendedShowIcon,
	EMbmPodcastAudio_dl_suspended,
	EMbmPodcastAudio_dl_suspended_mask,
	//	ESuspendedShowIconNew
	EMbmPodcastAudio_dl_suspended_new,
	EMbmPodcastAudio_dl_suspended_new_mask,
	//	EShowIcon = 0,
	EMbmPodcastVideo,
	EMbmPodcastVideo_mask,
	//	EShowIconNew,
	EMbmPodcastVideo_new,
	EMbmPodcastVideo_new_mask,
	//	EQuedShowIcon,
	EMbmPodcastVideo_dl_queued,
	EMbmPodcastVideo_dl_queued_mask,
	//	EQuedShowIconNew,
	EMbmPodcastVideo_dl_queued_new,
	EMbmPodcastVideo_dl_queued_new_mask,
	//	EDownloadingShowIcon,
	EMbmPodcastVideo_dl_active,
	EMbmPodcastVideo_dl_active_mask,
	//	EDownloadingShowIconNew,
	EMbmPodcastVideo_dl_active_new,
	EMbmPodcastVideo_dl_active_new_mask,
	//	EDownloadedShowIcon,
	EMbmPodcastVideo_dl,
	EMbmPodcastVideo_dl_mask,
	//	EDownloadedShowIconNew,
	EMbmPodcastVideo_dl_new,
	EMbmPodcastVideo_dl_new_mask,
	//	EFailedShowIcon,
	EMbmPodcastVideo_dl_failed,
	EMbmPodcastVideo_dl_failed_mask,
	//	EFailedShowIconNew,
	EMbmPodcastVideo_dl_failed_new,
	EMbmPodcastVideo_dl_failed_new_mask,
	//	ESuspendedShowIcon,
	EMbmPodcastVideo_dl_suspended,
	EMbmPodcastVideo_dl_suspended_mask,
	//	ESuspendedShowIconNew
	EMbmPodcastVideo_dl_suspended_new,
	EMbmPodcastVideo_dl_suspended_new_mask,
	EMbmPodcastFeed,
	EMbmPodcastFeed_mask,
	0,
	0
	};

const TInt KVideoIconOffset = 12;

CPodcastShowsView* CPodcastShowsView::NewL(CPodcastModel& aPodcastModel)
	{
	CPodcastShowsView* self = CPodcastShowsView::NewLC(aPodcastModel);
	CleanupStack::Pop(self);
	return self;
	}

CPodcastShowsView* CPodcastShowsView::NewLC(CPodcastModel& aPodcastModel)
	{
	CPodcastShowsView* self = new ( ELeave ) CPodcastShowsView(aPodcastModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPodcastShowsView::CPodcastShowsView(CPodcastModel& aPodcastModel) :
	iPodcastModel(aPodcastModel)
	{
	}

void CPodcastShowsView::ConstructL()
	{
	BaseConstructL(R_PODCAST_SHOWSVIEW);
	CPodcastListView::ConstructL();
	
	CreateIconsL();
		
	iPodcastModel.FeedEngine().AddObserver(this);
	iPodcastModel.ShowEngine().AddObserver(this);
	
	iStylusPopupMenu = CAknStylusPopUpMenu::NewL( this , TPoint(0,0));
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_SHOWVIEW_POPUP_MENU);
	iStylusPopupMenu->ConstructFromResourceL(reader);

	CleanupStack::PopAndDestroy();	
	}

void CPodcastShowsView::CreateIconsL()
	{
	CArrayPtr< CGulIcon>* icons = new(ELeave) CArrayPtrFlat< CGulIcon>(1);
	CleanupStack::PushL(icons);
	TInt pos = 0;
	while (KShowIconArrayIds[pos] > 0)
		{
		// Load the icon	
		CFbsBitmap* bitmap= NULL;
		CFbsBitmap* mask=  NULL;
		AknIconUtils::CreateIconL(bitmap,
					                          mask,
					                          iEikonEnv->EikAppUi()->Application()->BitmapStoreName(),
					                          KShowIconArrayIds[pos],
					                          KShowIconArrayIds[pos+1]);	
		CleanupStack::PushL(bitmap);		
		CleanupStack::PushL(mask);
		
		// Append the icon to icon array
		icons->AppendL(CGulIcon::NewL(bitmap, mask) );
		CleanupStack::Pop(2); // bitmap, mask
		pos+=2;
		}
		
	//iListContainer->Listbox()->ItemDrawer()->FormattedCellData()->SetIconArrayL(icons);
	iListContainer->SetListboxIcons(icons);
	CleanupStack::Pop(icons); // icons
	}

TKeyResponse CPodcastShowsView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType == EEventKey)
		{
		CShowInfo *activeShow = NULL;

		TInt index = iListContainer->Listbox()->CurrentItemIndex();
		if(index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
			activeShow = iPodcastModel.ActiveShowList()[index];
		}
		
		if (activeShow != NULL) {
			DP1("aKeyEvent.iCode=%d", aKeyEvent.iCode);
			switch (aKeyEvent.iCode) {
			case 117: 
			case '*':
			case EKeySpace:
				if (activeShow->PlayState() == EPlayed) {
					HandleCommandL(EPodcastMarkAsUnplayed);
				} else {
					HandleCommandL(EPodcastMarkAsPlayed);
				}
				break;
			case 106:
			case '#':
				if (activeShow->DownloadState() == ENotDownloaded) {
					HandleCommandL(EPodcastDownloadShow);
				}
				break;
			case EKeyBackspace:
			case EKeyDelete:
					HandleCommandL(EPodcastDeleteShow);
				break;
			default:
				break;
			}
			UpdateToolbar();
		}
	}
		return CPodcastListView::OfferKeyEventL(aKeyEvent, aType);
	}

CPodcastShowsView::~CPodcastShowsView()
	{
	iPodcastModel.ShowEngine().RemoveObserver(this);
	iPodcastModel.FeedEngine().RemoveObserver(this);
	
    if(iStylusPopupMenu)
        delete iStylusPopupMenu, iStylusPopupMenu = NULL;
	}


TUid CPodcastShowsView::Id() const
	{
	return KUidPodcastShowsViewID;
	}

void CPodcastShowsView::DoActivateL(const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId, const TDesC8& aCustomMessage)
	{
	DP("CPodcastShowsView::DoActivateL BEGIN");
	CPodcastListView::DoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);
	
	iPreviousView = TVwsViewId(KUidPodcast, KUidPodcastFeedViewID);
	
	iShowNewShows = (aCustomMessageId.iUid == 1);
	
	if (iShowNewShows)
		{
		CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
		cba->SetCommandSetL(R_PODCAST_CBA);
		cba->DrawDeferred();
		}
	else
		{
		CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
		cba->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_BACK);
		cba->DrawDeferred();		
		}
	
	UpdateViewTitleL();
	UpdateFeedUpdateStateL();
	DP("CPodcastShowsView::DoActivateL END");
	}

void CPodcastShowsView::DoDeactivate()
	{
	CPodcastListView::DoDeactivate();
	}

// Engine callback when new shows are available
void CPodcastShowsView::ShowListUpdatedL()
	{
	UpdateListboxItemsL();
	}

void CPodcastShowsView::ShowDownloadUpdatedL(TInt aBytesOfCurrentDownload, TInt /*aBytesTotal*/)
	{
	if (!iListContainer->IsVisible())
		{
		return;
		}
	
	CShowInfo *info = iPodcastModel.ShowEngine().ShowDownloading();
	if (info) 
		{
		UpdateShowItemL(info->Uid(), aBytesOfCurrentDownload);
		}		
	}

void CPodcastShowsView::ShowDownloadFinishedL(TUint aShowUid, TInt aError)
	{
	iProgressAdded = EFalse;
	CShowInfo *info = NULL;
	RShowInfoArray &fItems = iPodcastModel.ActiveShowList();
	
	for (TInt i=0;i<fItems.Count();i++)
		{
		if (fItems[i]->Uid() == aShowUid)
			{
			info = fItems[i];
			}
		}
	
	if (info == NULL) {
		return;
	}
	
	switch(aError)
		{
		case KErrCouldNotConnect:
			{
			TBuf<KMaxMessageLength> message;
			iEikonEnv->ReadResourceL(message, R_PODCAST_CONNECTION_ERROR);
			ShowErrorMessageL(message);
			}
			break;
		default:
			break;
		}
	UpdateListboxItemsL();
	}


void CPodcastShowsView::FeedDownloadStartedL(TFeedState /*aState*/, TUint aFeedUid)
	{
	// TODO make use of the fact that we know that the feed download is
	// started instead of checking feed engine states in UpdateFeedUpdateStateL.
	if (iPodcastModel.ActiveFeedInfo() != NULL
			&& iPodcastModel.ActiveFeedInfo()->Uid() == aFeedUid)
		{
		UpdateFeedUpdateStateL();
		UpdateToolbar();
		}	
	}

void CPodcastShowsView::FeedDownloadFinishedL(TFeedState /*aState*/, TUint aFeedUid, TInt /*aError*/)
	{
	DP("CPodcastShowsView::FeedDownloadFinishedL BEGIN");
	if (iListContainer->IsVisible() &&
			iPodcastModel.ActiveFeedInfo() != NULL &&
			iPodcastModel.ActiveFeedInfo()->Uid() == aFeedUid)
		{
		UpdateFeedUpdateStateL();
		UpdateViewTitleL();
		}
	DP("CPodcastShowsView::FeedDownloadFinishedL END");
	}

void CPodcastShowsView::HandleListBoxEventL(CEikListBox* /*aListBox*/,
		TListBoxEvent aEventType)
	{
	switch (aEventType)
		{
		case EEventEnterKeyPressed:		
		case EEventItemActioned:
		case EEventItemDoubleClicked:
			{
			RShowInfoArray &fItems = iPodcastModel.ActiveShowList();
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
			if (index>=0 && index< fItems.Count())
				{
				DP2("Handle event for podcast %S, downloadState is %d", &(fItems[index]->Title()), fItems[index]->DownloadState());
				CShowInfo *showInfo = fItems[index];
				
				switch (showInfo->DownloadState()) {
				case ENotDownloaded:
				case EFailedDownload:
					HandleCommandL(EPodcastDownloadShow);
					break;
				case EDownloading:
				case EQueued:
					AppUi()->ActivateLocalViewL(KUidPodcastQueueViewID,  TUid::Uid(0), KNullDesC8());
					((CPodcastAppUi*)AppUi())->SetActiveTab(KTabIdQueue);
					break;
				case EDownloaded:
					{
					TRAPD(err, iPodcastModel.PlayPausePodcastL(showInfo, ETrue));
					if (err != KErrNone)
						{
						HBufC *error = iEikonEnv->AllocReadResourceLC(R_ERROR_PLAYBACK_FAILED);
						ShowErrorMessageL(*error);
						CleanupStack::PopAndDestroy(error);
						}
					UpdateListboxItemsL();
					}
					break;
				default:
					break;
					}
				}
			}
			break;
		default:
			break;
		}
		UpdateToolbar();
	}

void CPodcastShowsView::GetShowIcons(CShowInfo* aShowInfo, TInt& aIconIndex)
	{
	TBool dlStop = iPodcastModel.SettingsEngine().DownloadSuspended();
	TBool isNew = aShowInfo->PlayState() == ENeverPlayed;
	DP1("downloadstate=%d", aShowInfo->DownloadState());
	switch (aShowInfo->DownloadState())
		{
		case EDownloaded:
			aIconIndex = isNew ? EDownloadedShowIconNew : EDownloadedShowIcon;
			break;
		case ENotDownloaded:
			aIconIndex = isNew ? EShowIconNew : EShowIcon;
			break;
		case EQueued:
			aIconIndex = dlStop ? (isNew ? ESuspendedShowIconNew : ESuspendedShowIcon) : 
								  (isNew ? EQuedShowIconNew : EQuedShowIcon);
			break;
		case EDownloading:
			aIconIndex = dlStop ? (isNew ? ESuspendedShowIconNew : ESuspendedShowIcon) : 
								  (isNew ? EDownloadingShowIconNew : EDownloadingShowIcon);	
			break;
		case EFailedDownload:
			aIconIndex = dlStop ? (isNew ? ESuspendedShowIconNew : ESuspendedShowIcon) : 
								  (isNew ? EFailedShowIconNew : EFailedShowIconNew);
			break;
		}
	
	if (aShowInfo->ShowType() == EVideoPodcast)
		{
		aIconIndex += KVideoIconOffset;
		}
	
	DP3("dlStop=%d, isNew=%d, aIconIndex=%d", dlStop, isNew, aIconIndex);
	}
	

void CPodcastShowsView::UpdateFeedUpdateStateL()
	{
	TBool listboxDimmed = EFalse;

	if (iPodcastModel.FeedEngine().ClientState() != EIdle && iPodcastModel.ActiveFeedInfo()
			!= NULL && iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid())
		{
		listboxDimmed = ETrue;
		}

	if ((iListContainer->Listbox()->IsDimmed() && !listboxDimmed) || (!iListContainer->Listbox()->IsDimmed() && listboxDimmed))
		{
		iListContainer->Listbox()->SetDimmed(listboxDimmed);
		}
	UpdateListboxItemsL();
	UpdateToolbar();
	}

void CPodcastShowsView::FormatShowInfoListBoxItemL(CShowInfo& aShowInfo, TInt aSizeDownloaded)
	{
	TBuf<32> infoSize;
	TInt iconIndex = 0;	
	TBuf<KMaxShortDateFormatSpec*2> showDate;
	GetShowIcons(&aShowInfo, iconIndex);	
	
	if(aSizeDownloaded > 0)
		{
		if (aShowInfo.ShowSize() > 0)
			{
				infoSize.Format(KSizeDownloadingOf(), ((float) aSizeDownloaded / (float) KSizeMb),
						((float)aShowInfo.ShowSize() / (float)KSizeMb));
			}
		else
			{
			infoSize.Format(KShowsSizeFormatS60(), (float)aSizeDownloaded / (float)KSizeMb);
			}
		}
	else if (aShowInfo.ShowSize() > 0)
		{
		infoSize.Format(KShowsSizeFormatS60(), (float)aShowInfo.ShowSize() / (float)KSizeMb);
		} 
	else {
		infoSize = KNullDesC();	
	}

	if (aShowInfo.PubDate().Int64() == 0)
		{
		showDate = KNullDesC();
		}
	else
		{
		aShowInfo.PubDate().FormatL(showDate, KDateFormatShort());
		}

	if(aShowInfo.LastError() != KErrNone)
		{
		GetShowErrorText(infoSize, aShowInfo.LastError());
		showDate.Zero();
		}
	
	if (infoSize.Length() > 0 && showDate.Length() > 0)
		{
		infoSize.Insert(0,_L(", "));
		}
		
	iListboxFormatbuffer.Format(KShowFormat(), iconIndex, &aShowInfo.Title(), &showDate, &infoSize);
	iListboxFormatbufferShort.Format(KShowFormatLandscape(), iconIndex, &aShowInfo.Title());
	}

void CPodcastShowsView::GetShowErrorText(TDes &aErrorMessage, TInt aErrorCode)
	{
	TRAP_IGNORE(((CPodcastAppUi*)AppUi())->GetErrorTextL(aErrorMessage,aErrorCode));
	}

void CPodcastShowsView::UpdateShowItemDataL(CShowInfo* aShowInfo,TInt aIndex, TInt aSizeDownloaded)
{
	FormatShowInfoListBoxItemL(*aShowInfo, aSizeDownloaded);
	iItemArray->Delete(aIndex);
	iItemArrayShort->Delete(aIndex);
	
	if(aIndex>= iItemArray->MdcaCount())
		{
		iItemArray->AppendL(iListboxFormatbuffer);
		iItemArrayShort->AppendL(iListboxFormatbufferShort);
		}
	else
		{
		iItemArray->InsertL(aIndex, iListboxFormatbuffer);
		iItemArrayShort->InsertL(aIndex, iListboxFormatbufferShort);
		}
}

void CPodcastShowsView::UpdateShowItemL(TUint aUid, TInt aSizeDownloaded)
{
	RShowInfoArray& array = iPodcastModel.ActiveShowList();
	
	for (int i=0;i<array.Count();i++) {
		if (array[i]->Uid() == aUid) {
			UpdateShowItemDataL(array[i], i, aSizeDownloaded);
			if (iListContainer->Listbox()->TopItemIndex() <= i &&
				iListContainer->Listbox()->BottomItemIndex() >= i) {
					iListContainer->Listbox()->DrawItem(i);
			}
		}
	}
}

void CPodcastShowsView::UpdateListboxItemsL()
	{
	DP("CPodcastShowsView::UpdateListboxItemsL BEGIN");
	if (iListContainer->IsVisible())
		{
		TListItemProperties itemProps;
		TInt len = 0;

		if (iShowNewShows)
			{
			iPodcastModel.GetNewShowsL();
			}
		else
			{
			iPodcastModel.GetShowsByFeedL(iPodcastModel.ActiveFeedInfo()->Uid());
			}
		
		RShowInfoArray &fItems = iPodcastModel.ActiveShowList();
		len = fItems.Count();

		if (iListContainer->Listbox() != NULL)
			{
			TBool allUidsMatch = EFalse;

			if (len == iListContainer->Listbox()->Model()->NumberOfItems())
				{
				allUidsMatch = ETrue;
				TUint itemId = 0;
				for (TInt loop = 0; loop< len; loop++)
					{
					itemId = iItemIdArray[loop];
					if (fItems[loop]->Uid() != itemId)
						{
						allUidsMatch = EFalse;
						break;
						}
					}
				}

			if (allUidsMatch && len > 0)
				{
				for (TInt loop = 0; loop< len; loop++)
					{
					UpdateShowItemDataL(fItems[loop], loop);
					}
				iListContainer->Listbox()->DrawNow();
				}
			else
				{
				//iListContainer->Listbox()->ItemDrawer()->ClearAllPropertiesL();
				iListContainer->Listbox()->Reset();
				iItemIdArray.Reset();
				iItemArray->Reset();
				iItemArrayShort->Reset();
				
				if (len > 0)
					{
					for (TInt i=0; i<len; i++)
						{
						CShowInfo *si = fItems[i];
						FormatShowInfoListBoxItemL(*si);
						iItemIdArray.Append(si->Uid());						
						iItemArray->AppendL(iListboxFormatbuffer);
						iItemArrayShort->AppendL(iListboxFormatbufferShort);
												
						}
					}
				else
					{
					iItemArray->Reset();
					iItemArrayShort->Reset();
					iItemIdArray.Reset();
					
					itemProps.SetDimmed(ETrue);
					itemProps.SetHiddenSelection(ETrue);
					}
				iListContainer->Listbox()->HandleItemAdditionL();
				}				
			}
		}
	DP("CPodcastShowsView::UpdateListboxItemsL END");
	}

/** 
 * Command handling function intended for overriding by sub classes. 
 * Default implementation is empty.  
 * @param aCommand ID of the command to respond to. 
 */
void CPodcastShowsView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EPodcastMarkAsPlayed:
			HandleSetShowPlayedL(ETrue);
			if (iShowNewShows) UpdateListboxItemsL();
			break;
		case EPodcastMarkAsUnplayed:
			HandleSetShowPlayedL(EFalse);
			if (iShowNewShows) UpdateListboxItemsL();
			break;
		case EPodcastMarkAllPlayed:
			iPodcastModel.MarkSelectionPlayedL();
			UpdateListboxItemsL();
			break;
		case EPodcastDeleteShow:
			HandleDeleteShowL();
			break;
		case EPodcastDownloadAll:
			HandleDownloadAllL();
			break;
		case EPodcastDownloadShow:
			{
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
			
			if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
				{
				iPodcastModel.ShowEngine().AddDownloadL(*iPodcastModel.ActiveShowList()[index]);
				UpdateShowItemL(iPodcastModel.ActiveShowList()[index]->Uid(),-1);
				}
			}
			break;
		case EPodcastUpdateFeed:
			{

			if (iPodcastModel.ActiveFeedInfo()->Url().Length()>0)
				{
				TRAPD(error, iPodcastModel.FeedEngine().UpdateFeedL(iPodcastModel.ActiveFeedInfo()->Uid()));

				if (error != KErrNone)
					{
					HBufC* str =
							iEikonEnv->AllocReadResourceLC(R_PODCAST_FEEDS_UPDATE_ERROR);
					User::InfoPrint(*str);
					CleanupStack::PopAndDestroy(str);
					}
				}
			}
			break;
		case EPodcastUpdateAllFeeds:
			{
			iPodcastModel.FeedEngine().UpdateAllFeedsL();
			UpdateToolbar();
			}break;
		case EPodcastCancelUpdateAllFeeds:
			iPodcastModel.FeedEngine().CancelUpdateAllFeeds();
			break;
		case EPodcastShowInfo:
			{
			DisplayShowInfoDialogL();
			}break;
		default:
			CPodcastListView::HandleCommandL(aCommand);
			break;
		}
	iListContainer->SetLongTapDetectedL(EFalse);

	UpdateToolbar();
	}
	
void CPodcastShowsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_PODCAST_SHOWSVIEW_MENU)
		{
		TBool showMarkAllPlayed = EFalse;
		for (int i=0;i<iPodcastModel.ActiveShowList().Count();i++)
			{
			CShowInfo* info = iPodcastModel.ActiveShowList()[i];
			if (info->PlayState() == ENeverPlayed)
				{
				showMarkAllPlayed = ETrue;
				break;
				}
			
			}
		
		TBool updatingState = iPodcastModel.FeedEngine().ClientState() != EIdle && iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid();
		aMenuPane->SetItemDimmed(EPodcastMarkAllPlayed, updatingState || !showMarkAllPlayed);

			
		}
}
	
void CPodcastShowsView::DisplayShowInfoDialogL()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();
	if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
		CShowInfo* info = iPodcastModel.ActiveShowList()[index];

		HBufC *title = info->Title().AllocL();
		HBufC *description = info->Description().AllocL();
		
		CAknMessageQueryDialog* note = new ( ELeave ) CAknMessageQueryDialog( description, title );
							
		note->PrepareLC( R_SHOW_INFO_NOTE ); // Adds to CleanupStack
		note->RunLD();
		}
	}

void CPodcastShowsView::HandleDownloadAllL()
	{
	
	TBuf<KMaxMessageLength> msg;
	iEikonEnv->ReadResourceL(msg, R_DOWNLOAD_ALL_QUERY);
	if (!ShowQueryMessageL(msg))
		{
		return;
		}
		
	for (int i=0;i<iPodcastModel.ActiveShowList().Count();i++)
		{
		CShowInfo* info = iPodcastModel.ActiveShowList()[i];

		if (info->DownloadState() == ENotDownloaded)
			{
			TRAP_IGNORE(iPodcastModel.ShowEngine().AddDownloadL(*info));
			}
		}
	}

void CPodcastShowsView::UpdateToolbar(TBool aVisible)
{
	CAknToolbar* toolbar = Toolbar();

	if (toolbar) {
		if (iListContainer->IsVisible()) {
			toolbar->SetToolbarVisibility(aVisible);
		}
	
		TBool updatingState = iPodcastModel.FeedEngine().ClientState() != EIdle && iPodcastModel.ActiveFeedInfo() && 
				iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid();
	
		if (iShowNewShows)
			{
			updatingState = iPodcastModel.FeedEngine().ClientState();
			toolbar->HideItem(EPodcastUpdateFeed, ETrue, ETrue ); 
			toolbar->HideItem(EPodcastUpdateAllFeeds, updatingState, ETrue ); 
			}
		else
			{
			toolbar->HideItem(EPodcastUpdateFeed, updatingState, ETrue ); 
			toolbar->HideItem(EPodcastUpdateAllFeeds, ETrue, ETrue ); 
			}

		toolbar->HideItem(EPodcastCancelUpdateAllFeeds, !updatingState, ETrue );
		toolbar->HideItem(EPodcastDownloadAll, !iShowNewShows, ETrue);

		if (iShowNewShows) 
			{
			toolbar->HideItem(EPodcastDownloadShow, ETrue, ETrue );
			
			TBool showDownloadAll = EFalse;
			for (int i=0;i<iPodcastModel.ActiveShowList().Count();i++)
				{
				CShowInfo* info = iPodcastModel.ActiveShowList()[i];
				if (info->DownloadState() == ENotDownloaded)
					{
					showDownloadAll = ETrue;
					}
				}
	
			toolbar->SetItemDimmed(EPodcastDownloadAll, !showDownloadAll, ETrue);
			}
		
		RShowInfoArray &fItems = iPodcastModel.ActiveShowList();
		TInt itemCnt = fItems.Count();
	
		TBool hideDownloadShowCmd = EFalse;
		TBool dimDownloadShowCmd = EFalse;
		TBool hideSetPlayed = EFalse;
	
			if(iListContainer->Listbox() != NULL)
			{
				TInt index = iListContainer->Listbox()->CurrentItemIndex();
				
				if(index>= 0 && index < itemCnt)
				{
					switch(fItems[index]->DownloadState())
						{
						case ENotDownloaded:
						case EFailedDownload:
							hideDownloadShowCmd = EFalse;
							dimDownloadShowCmd = EFalse;
							break;
						case EQueued:
						case EDownloading:
							hideDownloadShowCmd = EFalse;
							dimDownloadShowCmd = ETrue;
							break;
						case EDownloaded:
							hideDownloadShowCmd = ETrue;
							break;
						}
						
					if(fItems[index]->PlayState() == EPlayed) {
						hideSetPlayed = ETrue;
					}
				}
			}		
		
		if (hideDownloadShowCmd) {
			toolbar->HideItem(EPodcastDownloadShow, ETrue, ETrue );
			toolbar->HideItem(EPodcastDeleteShow, EFalse, ETrue);
			toolbar->SetItemDimmed(EPodcastDeleteShow, updatingState , ETrue);
		} else {
			toolbar->HideItem(EPodcastDownloadShow, EFalse, ETrue );
			toolbar->HideItem(EPodcastDeleteShow, ETrue, ETrue);
			toolbar->SetItemDimmed(EPodcastDownloadShow, updatingState || dimDownloadShowCmd || !itemCnt, ETrue);	
		}
		
		if (hideSetPlayed) {
			toolbar->HideItem(EPodcastMarkAsPlayed, ETrue, ETrue );
			toolbar->HideItem(EPodcastMarkAsUnplayed, EFalse, ETrue );
			toolbar->SetItemDimmed(EPodcastMarkAsUnplayed, updatingState, ETrue);
		} else {
			toolbar->HideItem(EPodcastMarkAsPlayed, EFalse, ETrue );
			toolbar->HideItem(EPodcastMarkAsUnplayed, ETrue, ETrue );
			toolbar->SetItemDimmed(EPodcastMarkAsPlayed, updatingState|| !itemCnt, ETrue);
		}
	}
}

void CPodcastShowsView::HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& /* aPenEventScreenLocation */)
{
	DP("CPodcastShowsView::HandleLongTapEventL BEGIN");

	iListContainer->SetLongTapDetectedL(ETrue);

	const TInt KListboxDefaultHeight = 19; // for some reason it returns 19 for an empty listbox in S^1
	TInt lbHeight = iListContainer->Listbox()->CalcHeightBasedOnNumOfItems(
			iListContainer->Listbox()->Model()->NumberOfItems()) - KListboxDefaultHeight;

    if(iStylusPopupMenu && aPenEventLocation.iY < lbHeight)
    {
		TInt index = iListContainer->Listbox()->CurrentItemIndex();
		if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
			{
			CShowInfo *info = iPodcastModel.ActiveShowList()[index];
			TBool hideDownloadShowCmd = info->DownloadState() != ENotDownloaded;
			TBool hideDeleteShowCmd = info->DownloadState() != EDownloaded;
			TBool hideMarkOld = info->PlayState() == EPlayed;
			
			iStylusPopupMenu->SetItemDimmed(EPodcastMarkAsPlayed, hideMarkOld);
			iStylusPopupMenu->SetItemDimmed(EPodcastMarkAsUnplayed, !hideMarkOld);
						
			iStylusPopupMenu->SetItemDimmed(EPodcastDownloadShow, hideDownloadShowCmd);
			iStylusPopupMenu->SetItemDimmed(EPodcastDeleteShow, hideDeleteShowCmd);
			}

		iStylusPopupMenu->ShowMenu();
		iStylusPopupMenu->SetPosition(aPenEventLocation);
    }
	DP("CPodcastShowsView::HandleLongTapEventL END");
}

void CPodcastShowsView::HandleSetShowPlayedL(TBool aPlayed)
	{

	TInt index = iListContainer->Listbox()->CurrentItemIndex();
				
	if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
		CShowInfo *info = iPodcastModel.ActiveShowList()[index];
		info->SetPlayState(aPlayed ? EPlayed : ENeverPlayed);
		iPodcastModel.ShowEngine().UpdateShowL(*info);
		UpdateShowItemDataL(iPodcastModel.ActiveShowList()[index], index, 0);
		iListContainer->Listbox()->DrawItem(index);					
		}
	}

void CPodcastShowsView::HandleDeleteShowL()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();

	if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
		CShowInfo *info = iPodcastModel.ActiveShowList()[index];
		TBuf<KMaxMessageLength> msg;
		TBuf<KMaxMessageLength> templ;
		iEikonEnv->ReadResourceL(templ, R_PODCAST_DELETE_SHOW_PROMPT);
		msg.Format(templ, &(info->Title()));
		if (ShowQueryMessageL(msg))
			{
			iPodcastModel.ShowEngine().DeleteShowL(iPodcastModel.ActiveShowList()[index]->Uid());
			
			// and mark as played, and not downloaded
			
			info->SetDownloadState(ENotDownloaded);
			info->SetPlayState(EPlayed);
			iPodcastModel.ShowEngine().UpdateShowL(*info);
			
			UpdateShowItemDataL(iPodcastModel.ActiveShowList()[index], index, 0);
			iListContainer->Listbox()->DrawItem(index);					
			}
		}
	}

void CPodcastShowsView::DownloadQueueUpdatedL(TInt aDownloadingShows, TInt aQueuedShows)
	{
	((CPodcastAppUi*)AppUi())->UpdateQueueTabL(aDownloadingShows+aQueuedShows);
	UpdateListboxItemsL();
	}

void CPodcastShowsView::FeedUpdateAllCompleteL(TFeedState /*aState*/)
	{
	UpdateListboxItemsL();
	UpdateToolbar();
	}

void CPodcastShowsView::UpdateViewTitleL()
	{
	DP("CPodcastShowsView::UpdateViewTitleL BEGIN");
	 CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
		      ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
		 
		TBool updatingState = iPodcastModel.FeedEngine().ClientState() != EIdle && iPodcastModel.ActiveFeedInfo() &&
				iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid();

		if (updatingState) {
			SetEmptyTextL(R_PODCAST_EMPTY_LIST_UPDATING);		
		} else {
			SetEmptyTextL(R_PODCAST_EMPTY_LIST);
		}
		
		if(iShowNewShows)
			{
			HBufC *title = iEikonEnv->AllocReadResourceLC(R_NEW_SHOWS);
			titlePane->SetTextL(*title);
			CleanupStack::PopAndDestroy(title);
			}
		else if(iPodcastModel.ActiveFeedInfo())
			{
			if (iPodcastModel.ActiveFeedInfo()->Title() != KNullDesC)
				{
				titlePane->SetTextL( iPodcastModel.ActiveFeedInfo()->Title(), ETrue );
				}
			}
		else
			{
			titlePane->SetTextToDefaultL();
			}
		
	DP("CPodcastShowsView::UpdateViewTitleL END");
	}
