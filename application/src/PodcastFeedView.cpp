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

#include "PodcastFeedView.h"
#include "PodcastAppUi.h"
#include "ShowEngine.h"
#include "SettingsEngine.h"
#include "PodcastApp.h"
#include "PodcastUtils.h"
#include "PodcastFeedViewUpdater.h"
#include "Podcast.hrh"
#include <caknfileselectiondialog.h> 
#include <podcast.rsg>
#include <podcast.mbg>
#include <gulicon.h>
#include <aknquerydialog.h>
#include <BAUTILS.H> 
#include <pathinfo.h> 
#include <caknmemoryselectiondialog.h> 
#include <caknmemoryselectiondialog.h> 
#include <caknfilenamepromptdialog.h> 
#include "Podcatcher.pan"
#include <aknmessagequerydialog.h>

const TInt KMaxFeedNameLength = 100;
const TInt KMaxUnplayedFeedsLength =64;
const TInt KADayInHours = 24;
#define KMaxMessageLength 200
#define KMaxTitleLength 100

_LIT(KFeedFormat, "%d\t%S\t%S%S");
enum 
{
 EFeedIcon
};

CPodcastFeedView* CPodcastFeedView::NewL(CPodcastModel& aPodcastModel)
    {
    CPodcastFeedView* self = CPodcastFeedView::NewLC(aPodcastModel);
    CleanupStack::Pop( self );
    return self;
    }

CPodcastFeedView* CPodcastFeedView::NewLC(CPodcastModel& aPodcastModel)
    {
    CPodcastFeedView* self = new ( ELeave ) CPodcastFeedView(aPodcastModel);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CPodcastFeedView::CPodcastFeedView(CPodcastModel& aPodcastModel):iPodcastModel(aPodcastModel)
	{
	iFirstActivateAfterLaunch = ETrue;
	}
#define KAsterisk iEikonEnv->EikAppUi()->Application()->BitmapStoreName()
void CPodcastFeedView::ConstructL()
	{
	DP("CPodcastFeedView::ConstructL BEGIN");
	//_LIT(KAsterisk, "*");
	BaseConstructL(R_PODCAST_FEEDVIEW);
	iNeverUpdated = iEikonEnv->AllocReadResourceL(R_PODCAST_FEEDS_NEVER_UPDATED);
	iFeedsFormat = iEikonEnv->AllocReadResourceL(R_PODCAST_FEEDS_STATUS_FORMAT);
	CPodcastListView::ConstructL();
	iPodcastModel.FeedEngine().AddObserver(this);
	CArrayPtr< CGulIcon >* icons = new(ELeave) CArrayPtrFlat< CGulIcon >(1);
	CleanupStack::PushL( icons );
	
	CFbsBitmap* bitmap = NULL;
	CFbsBitmap* mask = NULL;
	// Load the bitmap for empty icon	
	TFileName fname = KAsterisk;
	TParsePtr parser(fname);

	SetEmptyTextL(R_PODCAST_NO_FEEDS);
	
	// Load svg.-image and mask with a single call
		AknIconUtils::CreateIconL(bitmap,
		                          mask,
		                          iEikonEnv->EikAppUi()->Application()->BitmapStoreName(),
		                          EMbmPodcastFeed,
		                          EMbmPodcastFeed_mask);
	    
	/*bitmap = iEikonEnv->CreateBitmapL(KAsterisk,EMbmPodcastFeed_40x40);
	 * */
	CleanupStack::PushL( bitmap );		
	// Load the mask for feed icon	
	//mask = iEikonEnv->CreateBitmapL(KAsterisk,EMbmPodcastFeed_40x40m );	
	CleanupStack::PushL( mask );
	// Append the feed icon to icon array
	icons->AppendL( CGulIcon::NewL( bitmap, mask ) );
	CleanupStack::Pop(2); // bitmap, mask
	iListContainer->SetListboxIcons( icons );
	CleanupStack::Pop(icons); // icons
    iUpdater = CPodcastFeedViewUpdater::NewL(*this);
	DP("CPodcastFeedView::ConstructL END");
	}
    
CPodcastFeedView::~CPodcastFeedView()
    {
	iPodcastModel.FeedEngine().RemoveObserver(this);
	delete iFeedsFormat;
	delete iNeverUpdated;
	delete iUpdater;
	iFeedIdForIconArray.Close();
    }

void CPodcastFeedView::UpdateItemL(TInt aIndex)
	{
	__ASSERT_DEBUG(iListContainer->IsVisible(), Panic(EPodcatcherPanicFeedView));
	__ASSERT_ALWAYS(iItemIdArray.Count() > aIndex, Panic(EPodcatcherPanicFeedView));

	const RFeedInfoArray& sortedItems = iPodcastModel.FeedEngine().GetSortedFeeds();
	__ASSERT_ALWAYS(sortedItems.Count() > aIndex, Panic(EPodcatcherPanicFeedView));

	// Update UID of for the feed at aIndex
	iItemIdArray[aIndex] = sortedItems[aIndex]->Uid();
	
	// Prepare data to update the listbox item with
	FormatFeedInfoListBoxItemL(*sortedItems[aIndex], EFalse);
	
	// If nothing has changed, we are done here
	if (iListboxFormatbuffer == iItemArray->MdcaPoint(aIndex))
		{
		return;
		}

	// Something has changed, update the listbox item
	TListItemProperties itemProps;			
	itemProps.SetDimmed(EFalse);	
	iItemArray->Delete(aIndex);	
	iItemArray->InsertL(aIndex, iListboxFormatbuffer);
	iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(aIndex, itemProps);
	// If item is visible, redraw it
	if (iListContainer->Listbox()->TopItemIndex() <= aIndex
			&& iListContainer->Listbox()->BottomItemIndex() >= aIndex)
		{
		iListContainer->Listbox()->DrawItem(aIndex);
		}
	}

void CPodcastFeedView::UpdateItemsComplete()
	{
	}

TUid CPodcastFeedView::Id() const
	{
	return KUidPodcastFeedViewID;
	}
		
void CPodcastFeedView::DoActivateL(const TVwsViewId& aPrevViewId,
	                                  TUid aCustomMessageId,
	                                  const TDesC8& aCustomMessage)
	{
	DP("CPodcastFeedView::DoActivateL BEGIN");
	CPodcastListView::DoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);
	
	if (aPrevViewId.iViewUid == KUidPodcastShowsViewID)
		{
		// back key from shows view
		iViewingShows = EFalse;
		}
	
		UpdateListboxItemsL();		

	// when we receive a UID argument, this comes from search view after
	// a new feed has been added
	if (aCustomMessageId.iUid != 0)
		{
		TUint feedUid = aCustomMessageId.iUid;
		ShowItem(feedUid);
		TBuf<KMaxMessageLength> message;
		iEikonEnv->ReadResourceL(message, R_ADD_FEED_SUCCESS);
		if(ShowQueryMessageL(message))
			{
			iPodcastModel.FeedEngine().UpdateFeedL(feedUid);
			}
		}
		
	if (iFirstActivateAfterLaunch)
		{
		iFirstActivateAfterLaunch = EFalse;
		}

	DP("CPodcastFeedView::DoActivateL END");
	}

void CPodcastFeedView::DoDeactivate()
	{
	iUpdater->StopUpdate();
	CPodcastListView::DoDeactivate();
	}


void CPodcastFeedView::HandleListBoxEventL(CEikListBox* /* aListBox */, TListBoxEvent aEventType)
	{
//	DP("CPodcastFeedView::HandleListBoxEventL BEGIN");

	switch(aEventType)
		{
	case EEventEnterKeyPressed:
	case EEventItemDoubleClicked:
	case EEventItemActioned:
			{
			const RFeedInfoArray* sortedItems = NULL;
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
			sortedItems = &iPodcastModel.FeedEngine().GetSortedFeeds();

			if(index >= 0 && index < sortedItems->Count())
				{
				iPodcastModel.SetActiveFeedInfo((*sortedItems)[index]);			
				iViewingShows = ETrue;
				AppUi()->ActivateLocalViewL(KUidPodcastShowsViewID,  TUid::Uid(0), KNullDesC8());
				}
			}
		break;
	default:
		break;
		}
//	DP("CPodcastFeedView::HandleListBoxEventL END");
	}

void CPodcastFeedView::FeedUpdateAllCompleteL(TFeedState /*aState*/)
	{
	DP("FeedUpdateAllCompleteL");
	iFeedUpdating = 0;
	}

void CPodcastFeedView::FeedDownloadStartedL(TFeedState /*aState*/, TUint aFeedUid)
	{
	// Update status text
	iFeedUpdating = aFeedUid;
	UpdateFeedInfoStatusL(aFeedUid, ETrue);
	}

void CPodcastFeedView::FeedDownloadFinishedL(TFeedState aState,TUint aFeedUid, TInt aError)
	{
	switch(aError)
		{
		case KErrCouldNotConnect:
			{
			if(aState == MFeedEngineObserver::EFeedManualUpdate)
				{
				TBuf<KMaxMessageLength> message;
				iEikonEnv->ReadResourceL(message, R_PODCAST_CONNECTION_ERROR);
				ShowErrorMessageL(message);
				}
			}
			break;
		default: // Do nothing
			break;
		}
	UpdateFeedInfoStatusL(aFeedUid, EFalse);
	}

void CPodcastFeedView::UpdateFeedInfoStatusL(TUint aFeedUid, TBool aIsUpdating)
	{
	DP("CPodcastFeedView::UpdateFeedInfoStatusL BEGIN");
	const RFeedInfoArray& feeds = iPodcastModel.FeedEngine().GetSortedFeeds();

	// Find the index for the feed i both the feed-array and the listbox 
	TInt feedsIdx = KErrNotFound;
	TInt listboxIdx = KErrNotFound;
	for (TInt i = 0; i < feeds.Count(); i++)
		{
		if (feeds[i]->Uid() == aFeedUid)
			{
			feedsIdx = i;
			break;
			}
		}
	for (TInt j = 0; j < iItemIdArray.Count(); j++)
		{
		if (iItemIdArray[j] == aFeedUid)
			{
			listboxIdx = j;
			break;
			}
		}
		
	if (feedsIdx != KErrNotFound && listboxIdx != KErrNotFound)
		{
		// TODO In the long run we want to move the sorting resposibility from
		// CFeedEngine to CPodcastFeedView.
		// Hackish fix to make sure the listbox is sorted.
		if (listboxIdx != feedsIdx)
			{
			iItemIdArray.Remove(listboxIdx);
			iItemIdArray.InsertL(aFeedUid, feedsIdx);
			iItemArray->Delete(listboxIdx);
			iItemArray->InsertL(feedsIdx, KNullDesC);
			iListContainer->Listbox()->HandleItemAdditionL();
			}
		// Update the listbox info
		UpdateFeedInfoDataL(feeds[feedsIdx], feedsIdx, aIsUpdating);
		//TODO sort the listbox after update
		}
		// Update all visible listbox items that are affected by sorting and update.
		TInt minIdx = Max(Min(feedsIdx, listboxIdx), iListContainer->Listbox()->TopItemIndex());
		TInt maxIdx = Min(Max(feedsIdx, listboxIdx), iListContainer->Listbox()->BottomItemIndex());
		for (TInt k = minIdx; k <= maxIdx; k++)
			{
			iListContainer->Listbox()->DrawItem(k);
			}
		DP("CPodcastFeedView::UpdateFeedInfoStatusL END");
	}

void CPodcastFeedView::FormatFeedInfoListBoxItemL(CFeedInfo& aFeedInfo, TBool aIsUpdating)
	{
	DP("CPodcastFeedView::FormatFeedInfoListBoxItemL BEGIN");
	TBuf<KMaxShortDateFormatSpec*2> updatedDate;
	TBuf<KMaxUnplayedFeedsLength> unplayedShows;
	TUint unplayedCount = 0;
	TUint showCount = 0;
	TInt iconIndex = EFeedIcon;
	
	if(aIsUpdating)
		{
		iEikonEnv->ReadResourceL(updatedDate, R_PODCAST_FEEDS_IS_UPDATING);
		unplayedShows = KNullDesC();			
		}
	else
		{
		// we will get a leave if there are no shows for this feed, for instance, which is fine
		TRAP_IGNORE(iPodcastModel.FeedEngine().GetStatsByFeedL(aFeedInfo.Uid(), showCount, unplayedCount));	
		
		if (unplayedCount) {
			unplayedShows.Format(*iFeedsFormat, unplayedCount);
		} else {
			unplayedShows.Zero();
		}
		
		if (aFeedInfo.LastUpdated().Int64() == 0) 
			{
			updatedDate.Copy(*iNeverUpdated);					
			unplayedShows.Zero();
			}
		else 
			{
			TTime now;
			TTimeIntervalHours interval;
			now.HomeTime();
			now.HoursFrom(aFeedInfo.LastUpdated(), interval);
			if (interval.Int() < KADayInHours) 
				{
				aFeedInfo.LastUpdated().FormatL(updatedDate, KTimeFormat());
				}
			else 
				{
				aFeedInfo.LastUpdated().FormatL(updatedDate, KDateFormatShort());
				}
			}
		
		if(aFeedInfo.LastError() != KErrNone)
			{
			GetFeedErrorText(unplayedShows, aFeedInfo.LastError());
			updatedDate.Zero();
			}
		}
	
	iconIndex = iFeedIdForIconArray.Find(aFeedInfo.Uid());
	DP1("    iconIndex = %d", iconIndex);
	if(iconIndex == KErrNotFound && aFeedInfo.FeedIcon() != NULL && aFeedInfo.ImageFileName().Length() > 0 && 
			aFeedInfo.FeedIcon()->SizeInPixels().iHeight > 0 &&
			aFeedInfo.FeedIcon()->SizeInPixels().iWidth > 0)
		{
		// Hopefully temporary haxx to prevent double delete. I would prefer if
		// this could be solved with a little better design.
		CFbsBitmap* bmpCopy = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(bmpCopy);
		bmpCopy->Duplicate(aFeedInfo.FeedIcon()->Handle());
		CArrayPtr<CGulIcon>* icons = iListContainer->ListboxIcons();

		icons->AppendL( CGulIcon::NewL(AknIconUtils::CreateIconL(bmpCopy), NULL));
		
		iFeedIdForIconArray.Append(aFeedInfo.Uid());
		CleanupStack::Pop(bmpCopy);			
		iconIndex = icons->Count()-1;
		}	
	else 
		{
		iconIndex++;
		}	

	if (unplayedShows.Length() > 0 && updatedDate.Length() > 0) {
		unplayedShows.Insert(0,_L(", "));
	}
		
	iListboxFormatbuffer.Format(KFeedFormat(), iconIndex, &(aFeedInfo.Title()), &updatedDate,  &unplayedShows);
	DP("CPodcastFeedView::FormatFeedInfoListBoxItemL END");
	}

void CPodcastFeedView::ImageOperationCompleteL(TInt aError, TUint aHandle, CPodcastModel& /*aPodcastModel*/)
	{
	if (aError == KErrNone) {
		UpdateFeedInfoStatusL(aHandle, EFalse);
		}
	}

void CPodcastFeedView::UpdateFeedInfoDataL(CFeedInfo* aFeedInfo, TInt aIndex, TBool aIsUpdating )
	{
	DP("CPodcastFeedView::UpdateFeedInfoDataL BEGIN");
	TListItemProperties itemProps;			
	itemProps.SetDimmed(aIsUpdating);	
	FormatFeedInfoListBoxItemL(*aFeedInfo, aIsUpdating);
	
	TPtrC compareTo((*iItemArray)[aIndex]);
	
	if (iListboxFormatbuffer.Compare(compareTo) != 0) {
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
	//iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(aIndex, itemProps);
	DP("CPodcastFeedView::UpdateFeedInfoDataL END");
	}


void CPodcastFeedView::UpdateListboxItemsL()
	{
	DP("CPodcastFeedView::UpdateListboxItemsL BEGIN");
	// No reason to do any work if it isn't going to show..
	if(!iListContainer->IsVisible())
		{
		return;
		}
	
	TInt nbrItems = iPodcastModel.FeedEngine().GetSortedFeeds().Count();
	if (nbrItems > 0)
		{
		// Ensure that there are as many elements in iItemIdArray as in FeedEngine
		while (iItemIdArray.Count() < nbrItems)
			{
			iItemIdArray.AppendL(0);
			}
		while (iItemIdArray.Count() > nbrItems)
			{
			iItemIdArray.Remove(iItemIdArray.Count() - 1);
			}
		
		// Ensure that there are as many elements in iItemArray as in FeedEngine
		while (iItemArray->Count() < nbrItems)
			{
			iItemArray->AppendL(KNullDesC);
			TListItemProperties itemProps;
			iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(iItemArray->Count() - 1, itemProps);
			}
		while (iItemArray->Count() > nbrItems)
			{
			iItemArray->Delete(iItemArray->Count() - 1);
			}
		iUpdater->StartUpdate(nbrItems);
		}
	else
		{
		// No feeds at all in the list , add dummy list item
		TBuf<KMaxFeedNameLength> itemName;
		iEikonEnv->ReadResourceL(itemName, R_PODCAST_FEEDS_NO_FEEDS);
		iItemArray->Reset();
		iItemIdArray.Reset();
	
		TListItemProperties itemProps;
		itemProps.SetDimmed(ETrue);
		itemProps.SetHiddenSelection(ETrue);								
		iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(0, itemProps);
		}
	iListContainer->Listbox()->HandleItemAdditionL();
	DP("CPodcastFeedView::UpdateListboxItemsL END");
	}

/** 
 * Command handling function intended for overriding by sub classes. 
 * Default implementation is empty.  
 * @param aCommand ID of the command to respond to. 
 */
void CPodcastFeedView::HandleCommandL(TInt aCommand)
	{
	DP("CPodcastFeedView::HandleCommandL BEGIN");

	switch(aCommand)
		{
		case EPodcastAddFeed:
			HandleAddFeedL();
			break;
		case EPodcastImportFeeds:
			HandleImportFeedsL();
			break;
		case EPodcastExportFeeds:
			HandleExportFeedsL();
			break;
		case EPodcastEditFeed:
			HandleEditFeedL();
			break;
		case EPodcastDeleteFeedHardware:
		case EPodcastDeleteFeed:
			HandleRemoveFeedL();
			break;
		case EPodcastUpdateAllFeeds:
			{
			iPodcastModel.FeedEngine().UpdateAllFeedsL();
			}break;
		case EPodcastUpdateFeed:
			{
			HandleUpdateFeedL();
			}break;
		case EPodcastCancelUpdateAllFeeds:
			{
			if(iFeedUpdating)
				{
				iPodcastModel.FeedEngine().CancelUpdateAllFeeds();
				}
			}break;
		case EPodcastShowInfo:
			DisplayFeedInfoDialogL();
			break;
		default:
			CPodcastListView::HandleCommandL(aCommand);
			break;
		}
	
	DP("CPodcastFeedView::HandleCommandL END");
	}

void CPodcastFeedView::DisplayFeedInfoDialogL()
	{
	const RFeedInfoArray* sortedItems = NULL;
	TInt index = iListContainer->Listbox()->CurrentItemIndex();
	sortedItems = &iPodcastModel.FeedEngine().GetSortedFeeds();

	if(index >= 0 && index < sortedItems->Count())
		{
		CFeedInfo *info = (*sortedItems)[index];
		HBufC* description = info->Description().AllocL();
		HBufC* title = info->Title().AllocL();
		CAknMessageQueryDialog* note = new ( ELeave ) CAknMessageQueryDialog( description, title );
							
		note->PrepareLC( R_SHOW_INFO_NOTE ); // Adds to CleanupStack
		note->RunLD();
		}
	}

void CPodcastFeedView::HandleAddFeedL()
	{
	TInt selection;
	CDesCArrayFlat* array = iCoeEnv->ReadDesC16ArrayResourceL(R_FEEDVIEW_ADD_URL_OR_SEARCH_ARRAY );
	CleanupStack::PushL( array );
	
	CAknListQueryDialog* dialog = new ( ELeave ) CAknListQueryDialog( &selection );
	CleanupStack::PushL( dialog );
	dialog->PrepareLC( R_FEEDVIEW_ADD_URL_OR_SEARCH );
	CleanupStack::Pop( dialog );
	
	dialog->SetItemTextArray( array );
	dialog->SetOwnershipType( ELbmDoesNotOwnItemArray );
	
	if ( dialog->RunLD() )
		{
		if (selection == 0)
			{
			// Enter URL selected
			HandleAddFeedUrlL();
			} 
		else
			{
			// Search selected
			HandleAddFeedSearchL();
			}	
		}
	CleanupStack::PopAndDestroy( array );		
	}

void CPodcastFeedView::ShowItem(TUint aUid)
	{
	TInt listIndex = -1;
	for (TUint i=0;i<iItemIdArray.Count();i++)
		{
		if (iItemIdArray[i] == aUid)
			{
			listIndex = i;
			}
		}
		
	if (listIndex != -1)
		iListContainer->Listbox()->ScrollToMakeItemVisible(listIndex);

	}

void CPodcastFeedView::HandleAddFeedUrlL()
	{
	TBuf<KFeedUrlLength> url;
	url.Copy(_L("http://"));
	
	CAknTextQueryDialog * dlg =CAknTextQueryDialog::NewL(url);
	dlg->PrepareLC(R_PODCAST_ADD_FEED_DLG);

	HBufC* prompt = iEikonEnv->AllocReadResourceLC(R_PODCAST_ADDFEED_PROMPT);
	dlg->SetPromptL(*prompt);
	CleanupStack::PopAndDestroy(prompt);
	
	if(dlg->RunLD())
		{
		PodcastUtils::FixProtocolsL(url);

		CFeedInfo* newFeedInfo = CFeedInfo::NewL();
		CleanupStack::PushL(newFeedInfo);
		newFeedInfo->SetUrlL(url);
		newFeedInfo->SetTitleL(newFeedInfo->Url());
		
		TBool added = iPodcastModel.FeedEngine().AddFeedL(*newFeedInfo);
		
		if (added)
			{
			UpdateListboxItemsL();
			
			// ask if users wants to update it now
			TBuf<KMaxMessageLength> message;
			iEikonEnv->ReadResourceL(message, R_ADD_FEED_SUCCESS);
			if(ShowQueryMessageL(message))
				{
				ShowItem(newFeedInfo->Uid());
				iPodcastModel.FeedEngine().UpdateFeedL(newFeedInfo->Uid());
				}
			}
		else
			{
			TBuf<KMaxMessageLength> message;
			iEikonEnv->ReadResourceL(message, R_ADD_FEED_EXISTS);
			ShowErrorMessageL(message);
			}		
		
		CleanupStack::PopAndDestroy(newFeedInfo);
		}
	}

void CPodcastFeedView::HandleAddFeedSearchL()
	{
	TBuf<KFeedUrlLength> url;
	
	CAknTextQueryDialog * dlg =CAknTextQueryDialog::NewL(url);
	dlg->PrepareLC(R_PODCAST_ADD_FEED_DLG);

	HBufC* prompt = iEikonEnv->AllocReadResourceLC(R_PODCAST_SEARCHFEED_PROMPT);
	dlg->SetPromptL(*prompt);
	dlg->SetPredictiveTextInputPermitted(ETrue);
	CleanupStack::PopAndDestroy(prompt);
	
	if(dlg->RunLD())
		{		
		HBufC *waitText = iEikonEnv->AllocReadResourceLC(R_SEARCHING);
		ShowWaitDialogL(*waitText);
		CleanupStack::PopAndDestroy(waitText);	
	
		iOpmlState = EOpmlSearching;
		TRAPD(err, iPodcastModel.FeedEngine().SearchForFeedL(url));
		
		if (err != KErrNone)
			{
			delete iWaitDialog;
			iOpmlState = EOpmlIdle;
			}
		}
	}

void CPodcastFeedView::HandleEditFeedL()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();
	
	if(index < iItemArray->MdcaCount() && index >= 0)
		{
		CFeedInfo *info = iPodcastModel.FeedEngine().GetSortedFeeds()[index];

		TBuf<KFeedTitleLength> title;
		title.Copy(info->Title());
		TBuf<KFeedUrlLength> url;
		url.Copy(info->Url());
		CAknMultiLineDataQueryDialog  *dlg = CAknMultiLineDataQueryDialog ::NewL(title, url);
	
		if (dlg->ExecuteLD(R_PODCAST_EDIT_FEED_DLG)) 
			{
			
			if(info->Url().Compare(url) != 0)
				{
				TBuf<KMaxMessageLength> dlgMessage;
				iEikonEnv->ReadResourceL(dlgMessage, R_ADD_FEED_REPLACE);

				// Ask the user if it is OK to remove all shows
				if ( ShowQueryMessageL(dlgMessage))
					{
					PodcastUtils::FixProtocolsL(url);
					
					//----- HACK ---- //
					CFeedInfo* temp = CFeedInfo::NewLC();
					temp->SetUrlL(url);
					TBool added = iPodcastModel.FeedEngine().AddFeedL(*temp);
					if (added) {
						// The Feed URL did not exist
						// Remove the temp entry so that the correct entry could be changed
						iPodcastModel.FeedEngine().RemoveFeedL(temp->Uid());	
						
						// we remove the existing feed
						iPodcastModel.FeedEngine().RemoveFeedL(info->Uid());	
						
						CFeedInfo* newFeed = CFeedInfo::NewLC();
						newFeed->SetUrlL(url);
						newFeed->SetTitleL(title);
						
						iPodcastModel.FeedEngine().AddFeedL(*newFeed);
						CleanupStack::PopAndDestroy(newFeed);
						UpdateListboxItemsL();
					} else {
						// the feed existed. Object deleted in AddFeed.	
						TBuf<KMaxMessageLength> dlgMessage;
						iEikonEnv->ReadResourceL(dlgMessage, R_ADD_FEED_EXISTS);
						ShowErrorMessageL(dlgMessage);
					}
					CleanupStack::PopAndDestroy(temp);
				}
			} else { // no url change, maybe title?
				// Update the title
				if (info->Title().Compare(title) != 0)
				{
					info->SetTitleL(title);
					info->SetCustomTitle();	
					iPodcastModel.FeedEngine().UpdateFeedInfoL(info);
					UpdateListboxItemsL();
				}
			}
		}
		}
	}

void CPodcastFeedView::HandleRemoveFeedL()
	{
	if(iListContainer->Listbox() != NULL)
		{
		TInt index = iListContainer->Listbox()->CurrentItemIndex();
	
		if(index < iItemArray->MdcaCount() && index >= 0)
			{
			CFeedInfo *info = iPodcastModel.FeedEngine().GetFeedInfoByUid(iItemIdArray[index]);
			TBuf<KMaxMessageLength> templ;
			TBuf<KMaxMessageLength> message;
			iEikonEnv->ReadResourceL(templ, R_PODCAST_REMOVE_FEED_PROMPT);
			message.Format(templ, &info->Title());					
			if(ShowQueryMessageL(message))
				{
				iPodcastModel.FeedEngine().RemoveFeedL(iItemIdArray[index]);
				iItemArray->Delete(index);
				iItemIdArray.Remove(index);
				iListContainer->Listbox()->HandleItemRemovalL();
				iListContainer->Listbox()->DrawNow();
				}					
			}
		UpdateListboxItemsL();
		}
	}

void CPodcastFeedView::HandleUpdateFeedL()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();
	
	if(index < iItemArray->MdcaCount() && index >= 0)
		{
		CFeedInfo *info = iPodcastModel.FeedEngine().GetSortedFeeds()[index];
		iPodcastModel.FeedEngine().UpdateFeedL(info->Uid());
		}
	}

void CPodcastFeedView::HandleImportFeedsL()
	{
	CAknMemorySelectionDialog* memDlg = 
		CAknMemorySelectionDialog::NewL(ECFDDialogTypeNormal, ETrue);
	CleanupStack::PushL(memDlg);
	CAknMemorySelectionDialog::TMemory memory = 
		CAknMemorySelectionDialog::EPhoneMemory;

	if (memDlg->ExecuteL(memory))
		{
		TFileName importName;
	
		if (memory==CAknMemorySelectionDialog::EMemoryCard)
		{
			importName = PathInfo:: MemoryCardRootPath();
		}
		else
		{
			importName = PathInfo:: PhoneMemoryRootPath();
		}

		CAknFileSelectionDialog* dlg = CAknFileSelectionDialog::NewL(ECFDDialogTypeNormal, R_PODCAST_IMPORT_PODCAST);
		CleanupStack::PushL(dlg);

		dlg->SetDefaultFolderL(importName);
		
		if(dlg->ExecuteL(importName))
			{
			if(importName.Length()>0)
				{
				iPodcastModel.FeedEngine().ImportFeedsL(importName);
				UpdateListboxItemsL();
				}
			}
		CleanupStack::PopAndDestroy(dlg);
		}
	CleanupStack::PopAndDestroy(memDlg);								
	}

void CPodcastFeedView::HandleExportFeedsL()
	{
	CAknMemorySelectionDialog* memDlg = 
		CAknMemorySelectionDialog::NewL(ECFDDialogTypeSave, ETrue);
	CleanupStack::PushL(memDlg);
	CAknMemorySelectionDialog::TMemory memory = 
		CAknMemorySelectionDialog::EPhoneMemory;

	if (memDlg->ExecuteL(memory))
		{
		TFileName pathName;
		
		if (memory==CAknMemorySelectionDialog::EMemoryCard)
		{
			pathName = PathInfo::MemoryCardRootPath();
		}
		else
		{
			pathName = PathInfo::PhoneMemoryRootPath();
		}

		CAknFileSelectionDialog* dlg = CAknFileSelectionDialog::NewL(ECFDDialogTypeSave, R_PODCAST_EXPORT_FEEDS);
		CleanupStack::PushL(dlg);
								
		if(dlg->ExecuteL(pathName))
			{
			CAknFileNamePromptDialog *fileDlg = CAknFileNamePromptDialog::NewL(R_PODCAST_FILENAME_PROMPT_DIALOG);
			CleanupStack::PushL(fileDlg);
			fileDlg->SetPathL(pathName);
			TFileName fileName;
			if (fileDlg->ExecuteL(fileName) && fileName.Length() > 0)
				{
				pathName.Append(fileName);
				TFileName temp;
				iPodcastModel.FeedEngine().ExportFeedsL(temp);
				RFs fs;
				fs.Connect();
				BaflUtils::CopyFile(fs, temp, pathName);
				BaflUtils::DeleteFile(fs,temp);
				fs.Close();
				}
			CleanupStack::PopAndDestroy(fileDlg);
			}
		CleanupStack::PopAndDestroy(dlg);
	}
	CleanupStack::PopAndDestroy(memDlg);									
	}

void CPodcastFeedView::CheckResumeDownloadL()
	{
	// if there are shows queued for downloading, ask if we should resume now
	RShowInfoArray showsDownloading;
	iPodcastModel.ShowEngine().GetShowsDownloadingL(showsDownloading);

	if (showsDownloading.Count() > 0)
		{
		TBuf<KMaxMessageLength> msg;
		iEikonEnv->ReadResourceL(msg, R_PODCAST_ENABLE_DOWNLOADS_PROMPT);
	
		if (ShowQueryMessageL(msg))
			{
			// need to suspend downloads before ResumeDownloadL will work :)
			iPodcastModel.SettingsEngine().SetDownloadSuspended(ETrue);
			// resume downloading if user says yes
			iPodcastModel.ShowEngine().ResumeDownloadsL();
			}
		else
			{
			// we disable downloading if user says no
			iPodcastModel.SettingsEngine().SetDownloadSuspended(ETrue);
			}
		}
	
	// if no shows in queue, we keep whichever state suspend is in
	showsDownloading.ResetAndDestroy();
	}

void CPodcastFeedView::OpmlParsingComplete(TInt aError, TUint aNumFeedsImported)
	{
	TRAP_IGNORE(OpmlParsingCompleteL(aError, aNumFeedsImported));
	}

void CPodcastFeedView::OpmlParsingCompleteL(TInt aError, TUint aNumFeedsImported)
	{
	DP("CPodcastFeedView::OpmlParsingComplete BEGIN");
	
	switch (aError)
		{
		case KErrCouldNotConnect:
			{
			TBuf<KMaxMessageLength> message;
			iEikonEnv->ReadResourceL(message, R_PODCAST_CONNECTION_ERROR);
			delete iWaitDialog;
			iOpmlState = EOpmlIdle;
			ShowErrorMessageL(message);
			}
			break;
		case KErrNone: 
		default:			// we don't do more error handling here, just show 0 imported feeds
		switch (iOpmlState)
			{
			case EOpmlIdle:
				break;
			case EOpmlImporting:
				{
				UpdateListboxItemsL();
				delete iWaitDialog;
				iOpmlState = EOpmlIdle;
					
				TBuf<KMaxMessageLength> message;
				TBuf<KMaxMessageLength> templ;
				iEikonEnv->ReadResourceL(templ, R_IMPORT_FEED_SUCCESS);
				message.Format(templ, aNumFeedsImported);
				
				if(ShowQueryMessageL(message))
					{
					HandleCommandL(EPodcastUpdateAllFeeds);
					}
				}
				break;
			case EOpmlSearching:
				delete iWaitDialog;
				iWaitDialog = NULL;
				if (iPodcastModel.FeedEngine().GetSearchResults().Count() == 0)
					{
					TBuf<KMaxMessageLength> message;
					iEikonEnv->ReadResourceL(message, R_SEARCH_NORESULTS);
					ShowErrorMessageL(message);
					}
				else
					{
					AppUi()->ActivateLocalViewL(KUidPodcastSearchViewID,  TUid::Uid(0), KNullDesC8());
					}
				iOpmlState = EOpmlIdle;
				break;
			default:
				break;
			}
		}
	
	DP("CPodcastFeedView::OpmlParsingComplete END");
	}

void CPodcastFeedView::DialogDismissedL(TInt /*aButtonId*/)
	{
	iPodcastModel.FeedEngine().CancelUpdateAllFeeds();
	}

void CPodcastFeedView::GetFeedErrorText(TDes &aErrorMessage, TInt aErrorCode)
	{
	TRAP_IGNORE(((CPodcastAppUi*)AppUi())->GetErrorTextL(aErrorMessage,aErrorCode));
	}

TBool CPodcastFeedView::ViewingShows()
	{
	return iViewingShows;
	}

void CPodcastFeedView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	if(aResourceId == R_PODCAST_FEEDVIEW_MENU)
		{
		aMenuPane->SetItemDimmed(EPodcastUpdateAllFeeds, iFeedUpdating);
		aMenuPane->SetItemDimmed(EPodcastCancelUpdateAllFeeds, !iFeedUpdating);
		aMenuPane->SetItemDimmed(EPodcastSettings, iFeedUpdating);
		aMenuPane->SetItemDimmed(EPodcastFeedFeedMenu, iFeedUpdating);
//		aMenuPane->SetItemDimmed(EPodcastImportExportFeeds, iUpdatingRunning);
		}
	}

void CPodcastFeedView::CheckConfirmExit()
	{
	DP("CPodcastFeedView::CheckConfirmExit");
	RShowInfoArray showsDownloading;
	iPodcastModel.ShowEngine().GetShowsDownloadingL(showsDownloading);
	TUint count = showsDownloading.Count();
	showsDownloading.ResetAndDestroy();
	showsDownloading.Close();

	if (count > 0 && !iPodcastModel.SettingsEngine().DownloadSuspended())
		{
		TBuf<256> msg;
		iEikonEnv->ReadResourceL(msg, R_EXIT_SHOWS_DOWNLOADING);
		
		if (!ShowQueryMessageL(msg))
			{
			return;
			}
		}
	
	AppUi()->Exit();
	}

TKeyResponse CPodcastFeedView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType == EEventKey)
		{
			switch (aKeyEvent.iCode) {
			case EKeyBackspace:
			case EKeyDelete:
				HandleCommandL(EPodcastDeleteFeedHardware);
				break;
			default:
				break;
			}
		}
		return CPodcastListView::OfferKeyEventL(aKeyEvent, aType);
	}

