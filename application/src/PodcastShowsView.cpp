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

#define KPodcastImageWidth 160
#define KPodcastImageHeight 120
#define KPodcastDialogOffset 2

#define KOneHundredPercent 100

const TInt KSizeBufLen = 64;
const TInt KDefaultGran = 5;
_LIT(KSizeDownloadingOf, "%.1f/%.1f MB");
_LIT(KShowsSizeFormatS60, "%.1f MB");

_LIT(KShowFormat, "%d\t%S\t%S %S\t");
_LIT(KShowErrorFormat, "%d\t%S\t%S\t");
_LIT(KShowQueueFormat, "%d\t%S\t%S%S\t");

// these must correspond with TShowsIconIndex

const TUint KShowIconArrayIds[] =
	{
			EMbmPodcastAudio,
			EMbmPodcastAudio_mask,
			EMbmPodcastAudio_new,
			EMbmPodcastAudio_new_mask,
			EMbmPodcastAudio_queued,
			EMbmPodcastAudio_queued_mask,
			EMbmPodcastAudio_downloading,
			EMbmPodcastAudio_downloading_mask,
			EMbmPodcastAudio_downloaded,
			EMbmPodcastAudio_downloaded_mask,
			EMbmPodcastAudio_downloaded_new,
			EMbmPodcastAudio_downloaded_new_mask,
			EMbmPodcastAudio_failed,
			EMbmPodcastAudio_failed_mask,
			EMbmPodcastAudio_suspended,
			EMbmPodcastAudio_suspended_mask,
			0,
			0
	};

/** 
 *  This is an interal class to display a message query dialog with an image at the bottm
 */
class CPodcastImageMessageQueryDialog:public CAknMessageQueryDialog
	{
	public:
		/**
		 * C++ default constructor.
		 *
		 * @param aMessage Dialog box text.
		 * @param aHeader Header for the dialog.
		 * @deprecated 
		 */ 
		CPodcastImageMessageQueryDialog(TDesC* aMessage, TDesC* aHeader):CAknMessageQueryDialog(aMessage, aHeader)
    		{

    		}    

		~CPodcastImageMessageQueryDialog()
			{

			}

		void SetSizeAndPosition(const TSize& aSize)
			{
			CAknMessageQueryDialog::SetSizeAndPosition(aSize);

			TPoint pos = Position();		 
			TSize size = Size();

			CAknDialog::SetSizeAndPosition(aSize);		 		 

			pos.iY-=((aSize.iHeight-size.iHeight)-KPodcastDialogOffset);
			SetPosition(pos);	
			SetSize(aSize);
			}

	};

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
	
	iListContainer->Listbox()->SetListBoxObserver(this);
	
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
		// Load the bitmap for play icon	
		CFbsBitmap* bitmap= NULL;//iEikonEnv->CreateBitmapL( _L("*"), KIconArrayIds[pos]);
		CFbsBitmap* mask=  NULL;////iEikonEnv->CreateBitmapL( _L("*"), KIconArrayIds[pos+1] );
		AknIconUtils::CreateIconL(bitmap,
					                          mask,
					                          iEikonEnv->EikAppUi()->Application()->BitmapStoreName(),
					                          KShowIconArrayIds[pos],
					                          KShowIconArrayIds[pos+1]);	
		CleanupStack::PushL(bitmap);		
		CleanupStack::PushL(mask);
		
		// Append the play icon to icon array
		icons->AppendL(CGulIcon::NewL(bitmap, mask) );
		CleanupStack::Pop(2); // bitmap, mask
		pos+=2;
		}
		
	iListContainer->Listbox()->ItemDrawer()->FormattedCellData()->SetIconArrayL(icons);
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
	
	UpdateViewTitleL();
	
	CPodcastListView::DoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);
	iPreviousView = TVwsViewId(KUidPodcast, KUidPodcastFeedViewID);
	
	UpdateFeedUpdateStateL();
	UpdateToolbar();
	DP("CPodcastShowsView::DoActivateL END");
	}

void CPodcastShowsView::DoDeactivate()
	{
	CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
		     ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
	
//	titlePane->SetSmallPicture(NULL, NULL, ETrue);
	titlePane->SetPicture(NULL, NULL);
	titlePane->SetTextToDefaultL();
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

void CPodcastShowsView::ShowDownloadFinishedL(TUint /*aShowUid*/, TInt aError)
	{
	iProgressAdded = EFalse;
	
	switch(aError)
		{
		case KErrCouldNotConnect:
			{
			TBuf<KMaxMessageLength> message;
			iEikonEnv->ReadResourceL(message, R_PODCAST_CONNECTION_ERROR);
			ShowErrorMessage(message);
			}
			break;
		default: // Do nothing
			break;
		}
	}


void CPodcastShowsView::FeedDownloadStartedL(TFeedState /*aState*/, TUint aFeedUid)
	{
	// TODO make use of the fact that we know that the feed download is
	// started instead of checking feed engine states in UpdateFeedUpdateStateL.
	if (iPodcastModel.ActiveFeedInfo() != NULL
			&& iPodcastModel.ActiveFeedInfo()->Uid() == aFeedUid)
		{
		TRAP_IGNORE(UpdateFeedUpdateStateL());
		UpdateToolbar();
		}	
	}

void CPodcastShowsView::FeedDownloadFinishedL(TFeedState /*aState*/, TUint aFeedUid, TInt /*aError*/)
	{
	DP("CPodcastShowsView::FeedDownloadFinishedL BEGIN");
	// TODO make use of the fact that we know that the feed download is
	// finished instead of checking feed engine states in UpdateFeedUpdateStateL.
	if (iPodcastModel.ActiveFeedInfo() != NULL
			&& iPodcastModel.ActiveFeedInfo()->Uid() == aFeedUid)
		{
		TRAP_IGNORE(UpdateFeedUpdateStateL());
		TRAP_IGNORE(UpdateViewTitleL());
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
					HandleCommandL(EPodcastDownloadShow);
					break;
				case EQueued:
					AppUi()->ActivateLocalViewL(KUidPodcastQueueViewID,  TUid::Uid(0), KNullDesC8());
					((CPodcastAppUi*)AppUi())->SetActiveTab(KTabIdQueue);
					break;
				case EDownloaded:
					#pragma message("LAPER Replace activate playview with activate playback in mpx")
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
	TUint showDownloadingUid = iPodcastModel.ShowEngine().ShowDownloading() ? iPodcastModel.ShowEngine().ShowDownloading()->Uid() : 0;
	
	if (showDownloadingUid == aShowInfo->Uid())
		{
		aIconIndex = dlStop ? ESuspendedShowIcon : EDownloadingShowIcon;		
		}
	else
		{
		switch (aShowInfo->DownloadState())
			{
			case EDownloaded:
				if (aShowInfo->PlayState() == ENeverPlayed) {
					aIconIndex = EDownloadedNewShowIcon;
				} else {
					aIconIndex = EDownloadedShowIcon;
				}
				break;
			case ENotDownloaded:
				if (aShowInfo->PlayState() == ENeverPlayed) {
					aIconIndex = ENewShowIcon;
				} else {
					aIconIndex = EShowIcon;
				}
				break;
			case EQueued:
				aIconIndex = dlStop ? ESuspendedShowIcon : EQuedShowIcon;
				break;
			case EDownloading:
				aIconIndex = dlStop ? ESuspendedShowIcon : EDownloadingShowIcon;		
				break;
			case EFailedDownload:
				aIconIndex = EFailedShowIcon;
				break;
			}
		}
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
		TBuf<KSizeBufLen> errorBuffer;
		GetShowErrorText(errorBuffer, aShowInfo.LastError());
		iListboxFormatbuffer.Format(KShowErrorFormat(), iconIndex, &aShowInfo.Title(), &errorBuffer);
		}
	else	
		{
		if (infoSize.Length() > 0) {
			infoSize.Insert(0,_L(", "));
		}
		
		iListboxFormatbuffer.Format(KShowFormat(), iconIndex, &aShowInfo.Title(), &showDate, &infoSize);
		}
	}

void CPodcastShowsView::GetShowErrorText(TDes &aErrorMessage, TInt aErrorCode)
	{
	iEikonEnv->GetErrorText(aErrorMessage, aErrorCode);
	}

void CPodcastShowsView::UpdateShowItemDataL(CShowInfo* aShowInfo,TInt aIndex, TInt aSizeDownloaded)
{
	FormatShowInfoListBoxItemL(*aShowInfo, aSizeDownloaded);
	iItemArray->Delete(aIndex);
	if(aIndex>= iItemArray->MdcaCount())
		{
		iItemArray->AppendL(iListboxFormatbuffer);
		}
	else
		{
		iItemArray->InsertL(aIndex, iListboxFormatbuffer);
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
	if (iListContainer->IsVisible())
		{
		TListItemProperties itemProps;
		TInt len = 0;

		iPodcastModel.GetShowsByFeed(iPodcastModel.ActiveFeedInfo()->Uid());

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
				}
			else
				{
				iListContainer->Listbox()->ItemDrawer()->ClearAllPropertiesL();
				iListContainer->Listbox()->Reset();
				iItemIdArray.Reset();
				iItemArray->Reset();

				if (len > 0)
					{
					for (TInt i=0; i<len; i++)
						{
						CShowInfo *si = fItems[i];
						FormatShowInfoListBoxItemL(*si);
						iItemIdArray.Append(si->Uid());						
						iItemArray->AppendL(iListboxFormatbuffer);
						}
					}
				else
					{
					iItemArray->Reset();
					iItemIdArray.Reset();
					
					itemProps.SetDimmed(ETrue);
					itemProps.SetHiddenSelection(ETrue);
					}
				iListContainer->Listbox()->HandleItemAdditionL();
				}				
			}
		}
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
			SetShowPlayed(ETrue);
			break;
		case EPodcastMarkAsUnplayed:
			SetShowPlayed(EFalse);
			break;
		case EPodcastMarkAllPlayed:
			iPodcastModel.MarkSelectionPlayed();
			UpdateListboxItemsL();
			break;
		case EPodcastDeleteShow:
			DeleteShow();
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
					HBufC
							* str =
							iEikonEnv->AllocReadResourceLC(R_PODCAST_FEEDS_UPDATE_ERROR);
					User::InfoPrint(*str);
					CleanupStack::PopAndDestroy(str);
					}
				}
			}
			break;
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
	UpdateToolbar();
	}
	
void CPodcastShowsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_PODCAST_SHOWSVIEW_MENU)
		{
		TBool updatingState = iPodcastModel.FeedEngine().ClientState() != EIdle && iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid();
		aMenuPane->SetItemDimmed(EPodcastMarkAllPlayed, updatingState);
		}
}
	
void CPodcastShowsView::ImageOperationCompleteL(TInt aError)
	{
	iLastImageHandlerError = aError;
	if(iSetTitlebarImage)
		{
		iSetTitlebarImage = EFalse;
		if(aError == KErrNone)
			{
			CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
						 ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
			titlePane->SetSmallPicture(iPodcastModel.ImageHandler().ScaledBitmap(), NULL, ETrue);
			}
		else
			{
			iPodcastModel.ImageHandler().ScaledBitmap();
			}
			
		}
	else
		{
		CActiveScheduler::Stop();
		}
	}
	
void CPodcastShowsView::DisplayShowInfoDialogL()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();
	if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
		CShowInfo* info = iPodcastModel.ActiveShowList()[index];
		TUint32 feedUid = info->FeedUid();							
		CFeedInfo* feedInfo = iPodcastModel.FeedEngine().GetFeedInfoByUid(feedUid);
		
		CPodcastImageMessageQueryDialog* note = new ( ELeave ) CPodcastImageMessageQueryDialog( (TDesC*)&info->Description(), (TDesC*)&info->Title() );
							
		note->PrepareLC( R_SHOW_INFO_NOTE ); // Adds to CleanupStack
		
		if(feedInfo && feedInfo->ImageFileName().Length()>0)
			{
			CFbsBitmap * bitmap = new (ELeave) CFbsBitmap;
			CleanupStack::PushL(bitmap);
			
			TRAPD(loaderror, iPodcastModel.ImageHandler().LoadFileAndScaleL(bitmap, feedInfo->ImageFileName(), TSize(KPodcastImageWidth, KPodcastImageHeight), *this));
			
			if(loaderror == KErrNone)
				{
				CActiveScheduler::Start();
				if(iLastImageHandlerError == KErrNone)
					{	
					CEikImage* image = static_cast<CEikImage*>(note->ControlOrNull(EPodcastShowInfoImage));
					image->SetBitmap(bitmap);
					CleanupStack::Pop(bitmap);
					bitmap = NULL;
					}
				else
					{				
					CleanupStack::PopAndDestroy(bitmap);
					bitmap = NULL;
					}
				}
			else
				{
				CleanupStack::PopAndDestroy(bitmap);
				bitmap = NULL;
				}
			}												
		
		note->RunLD(); 
		}
	}

void CPodcastShowsView::UpdateToolbar()
{
	CAknToolbar* toolbar = Toolbar();
	
	RShowInfoArray &fItems = iPodcastModel.ActiveShowList();
	TInt itemCnt = fItems.Count();

	TBool hideDownloadShowCmd = EFalse;
	TBool dimDownloadShowCmd = EFalse;
	TBool hideSetPlayed = EFalse;
	TBool updatingState = iPodcastModel.FeedEngine().ClientState() != EIdle && 
			iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid();
	
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
	
	toolbar->HideItem(EPodcastUpdateFeed, updatingState, ETrue ); 
	toolbar->HideItem(EPodcastCancelUpdateAllFeeds, !updatingState, ETrue );
	
	if (hideDownloadShowCmd) {
		toolbar->HideItem(EPodcastDownloadShow, ETrue, ETrue );
		toolbar->HideItem(EPodcastDeleteShow, EFalse, ETrue);
		toolbar->SetItemDimmed(EPodcastDeleteShow, updatingState, ETrue);
	} else {
		toolbar->HideItem(EPodcastDownloadShow, EFalse, ETrue );
		toolbar->HideItem(EPodcastDeleteShow, ETrue, ETrue);
		toolbar->SetItemDimmed(EPodcastDownloadShow, updatingState || dimDownloadShowCmd, ETrue);	
	}
	
	if (hideSetPlayed) {
		toolbar->HideItem(EPodcastMarkAsPlayed, ETrue, ETrue );
		toolbar->HideItem(EPodcastMarkAsUnplayed, EFalse, ETrue );
		toolbar->SetItemDimmed(EPodcastMarkAsUnplayed, updatingState, ETrue);
	} else {
		toolbar->HideItem(EPodcastMarkAsPlayed, EFalse, ETrue );
		toolbar->HideItem(EPodcastMarkAsUnplayed, ETrue, ETrue );
		toolbar->SetItemDimmed(EPodcastMarkAsPlayed, updatingState, ETrue);
	}
}

void CPodcastShowsView::HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& /* aPenEventScreenLocation */)
{
	DP("CPodcastShowsView::HandleLongTapEventL BEGIN");
	
    if(iStylusPopupMenu)
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

void CPodcastShowsView::SetShowPlayed(TBool aPlayed)
	{

	TInt index = iListContainer->Listbox()->CurrentItemIndex();
				
	if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
		CShowInfo *info = iPodcastModel.ActiveShowList()[index];
		info->SetPlayState(aPlayed ? EPlayed : ENeverPlayed);
		iPodcastModel.ShowEngine().UpdateShow(*info);
		UpdateShowItemDataL(iPodcastModel.ActiveShowList()[index], index, 0);
		iListContainer->Listbox()->DrawItem(index);					
		}
	}

void CPodcastShowsView::DeleteShow()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();

	if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
		CShowInfo *info = iPodcastModel.ActiveShowList()[index];
		TBuf<KMaxMessageLength> msg;
		TBuf<KMaxMessageLength> templ;
		iEikonEnv->ReadResourceL(templ, R_PODCAST_DELETE_SHOW_PROMPT);
		msg.Format(templ, &(info->Title()));
		if (ShowQueryMessage(msg))
			{
			iPodcastModel.ShowEngine().DeleteShowL(iPodcastModel.ActiveShowList()[index]->Uid());
			
			// and mark as played, and not downloaded
			
			info->SetDownloadState(ENotDownloaded);
			info->SetPlayState(EPlayed);
			iPodcastModel.ShowEngine().UpdateShow(*info);
			
			UpdateShowItemDataL(iPodcastModel.ActiveShowList()[index], index, 0);
			iListContainer->Listbox()->DrawItem(index);					
			}
		}
	}

void CPodcastShowsView::DownloadQueueUpdatedL(TInt /*aDownloadingShows*/, TInt /*aQueuedShows*/)
	{
	//((CPodcastAppUi*)AppUi())->UpdateQueueTab(aDownloadingShows+aQueuedShows);
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
		 
		TBool updatingState = iPodcastModel.FeedEngine().ClientState() != EIdle && 
				iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid();

		if (updatingState) {
			SetEmptyTextL(R_PODCAST_EMPTY_LIST_UPDATING);		
		} else {
			SetEmptyTextL(R_PODCAST_EMPTY_LIST);
		}
		
		if(iPodcastModel.ActiveFeedInfo())
			{
			if (iPodcastModel.ActiveFeedInfo()->Title() != KNullDesC)
				{
				titlePane->SetTextL( iPodcastModel.ActiveFeedInfo()->Title(), ETrue );
				}
			}
		else
			{
			titlePane->SetPicture(NULL, NULL);
			titlePane->SetTextToDefaultL();
			}
		
	DP("CPodcastShowsView::UpdateViewTitleL END");
	}
