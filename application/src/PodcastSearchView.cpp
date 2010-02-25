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

#include "PodcastSearchView.h"
#include "PodcastAppUi.h"
#include "FeedEngine.h"
#include "ShowEngine.h"
#include "SettingsEngine.h"
#include "PodcastApp.h"
#include "PodcastUtils.h"
#include <caknfileselectiondialog.h> 
#include <podcast.rsg>
#include <podcast.mbg>
#include <gulicon.h>
#include <eikenv.h>
#include <e32const.h>
#include <eikdialg.h>
#include <aknquerydialog.h>
#include <caknmemoryselectiondialog.h> 
#include <caknfilenamepromptdialog.h> 
#include <BAUTILS.H> 
#include <pathinfo.h> 
#include <f32file.h>
#include <akntoolbarextension.h>
#include <akntitle.h>

const TInt KMaxFeedNameLength = 100;
const TInt KDefaultGran = 5;
const TInt KNumberOfFilesMaxLength = 4;
#define KMaxMessageLength 200
#define KMaxTitleLength 100
const TInt KMimeBufLength = 100;

_LIT(KUnknownUpdateDateString, "?/?");
_LIT(KSearchResultFormat, "%d\t%S\t%S");

CPodcastSearchView* CPodcastSearchView::NewL(CPodcastModel& aPodcastModel)
    {
    CPodcastSearchView* self = CPodcastSearchView::NewLC(aPodcastModel);
    CleanupStack::Pop( self );
    return self;
    }

CPodcastSearchView* CPodcastSearchView::NewLC(CPodcastModel& aPodcastModel)
    {
    CPodcastSearchView* self = new ( ELeave ) CPodcastSearchView(aPodcastModel);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CPodcastSearchView::CPodcastSearchView(CPodcastModel& aPodcastModel):iPodcastModel(aPodcastModel)
{
}

void CPodcastSearchView::ConstructL()
{
	BaseConstructL(R_PODCAST_SEARCHVIEW);
	CPodcastListView::ConstructL();
	iPodcastModel.FeedEngine().AddObserver(this);
	CArrayPtr< CGulIcon >* icons = new(ELeave) CArrayPtrFlat< CGulIcon >(1);
	CleanupStack::PushL( icons );
	
	// Load the bitmap for empty icon	
	CFbsBitmap* bitmap = NULL;
	CFbsBitmap* mask = NULL;//	
	
	// Load the bitmap for feed icon	

	// Load svg.-image and mask with a single call
		AknIconUtils::CreateIconL(bitmap,
		                          mask,
		                          iEikonEnv->EikAppUi()->Application()->BitmapStoreName(),
		                          EMbmPodcastFeed,
		                          EMbmPodcastFeed_mask);
		/*
	bitmap = iEikonEnv->CreateBitmapL( _L("*"),EMbmPodcastFeed_40x40);*/
	CleanupStack::PushL( bitmap );		
	// Load the mask for feed icon	
	/*mask = iEikonEnv->CreateBitmapL( _L("*"),EMbmPodcastFeed_40x40m );	
	*/
	CleanupStack::PushL( mask );		
	// Append the feed icon to icon array
	icons->AppendL( CGulIcon::NewL( bitmap, mask ) );
	CleanupStack::Pop(2); // bitmap, mask
	
	iListContainer->Listbox()->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
	CleanupStack::Pop(icons); // icons

	iListContainer->Listbox()->SetListBoxObserver(this);
	
	SetEmptyTextL(R_PODCAST_EMPTY_SEARCH);
}
    
CPodcastSearchView::~CPodcastSearchView()
    {
	iPodcastModel.FeedEngine().RemoveObserver(this);
 
    if(iLongTapDetector)
        delete iLongTapDetector, iLongTapDetector = NULL;

    if(iStylusPopupMenu)
        delete iStylusPopupMenu, iStylusPopupMenu = NULL;

    }

TUid CPodcastSearchView::Id() const
{
	return KUidPodcastSearchViewID;
}
		
void CPodcastSearchView::DoActivateL(const TVwsViewId& aPrevViewId,
	                                  TUid aCustomMessageId,
	                                  const TDesC8& aCustomMessage)
{
	UpdateToolbar();
	
	 CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
		      ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
	
	HBufC* title = iEikonEnv->AllocReadResourceLC(R_SEARCH_RESULTS);
	titlePane->SetTextL( *title, ETrue );
	CleanupStack::PopAndDestroy(title);
    
	CPodcastListView::DoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);
	iPreviousView = TVwsViewId(KUidPodcast, KUidPodcastFeedViewID);
}

void CPodcastSearchView::DoDeactivate()
{
	CPodcastListView::DoDeactivate();
	CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
		     ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
	titlePane->SetTextToDefaultL();
}


void CPodcastSearchView::HandleListBoxEventL(CEikListBox* /* aListBox */, TListBoxEvent aEventType)
{
	DP("CPodcastSearchView::HandleListBoxEventL BEGIN");
	switch(aEventType)
	{
	case EEventEnterKeyPressed:
	case EEventItemDoubleClicked:
	case EEventItemActioned:
		{
		HandleCommandL(EPodcastAddSearchResult);
		}
		break;
	default:
		break;
	}
	DP("CPodcastSearchView::HandleListBoxEventL END");
}

void CPodcastSearchView::UpdateListboxItemsL()
	{
	DP("CPodcastSearchView::UpdateListboxItemsL BEGIN");
	if(!iListContainer->IsVisible()) {
		DP("CPodcastSearchView::UpdateListboxItemsL END (not visible)");
		return;
	}
	const RFeedInfoArray* searchItems = NULL;
	searchItems = &iPodcastModel.FeedEngine().GetSearchResults();
	TInt len = searchItems->Count();
	TListItemProperties itemProps;
	iListContainer->Listbox()->Reset();
	iListContainer->Listbox()->ItemDrawer()->ClearAllPropertiesL();
	iItemIdArray.Reset();
	iItemArray->Reset();
		
	if (len > 0) 
		{
		for (int i=0;i<len;i++) 
			{				
			CFeedInfo *fi = (*searchItems)[i];
			iItemIdArray.Append(fi->Uid());
			TInt iconIndex = 0;
			
			TBuf<512> descr;
			descr.Copy(fi->Description().Left(512));
			
			iListboxFormatbuffer.Format(KSearchResultFormat(), iconIndex, &fi->Title(), &descr);
			iItemArray->AppendL(iListboxFormatbuffer);
			iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(i, itemProps);
			}
		} 
	else 
		{					
		TBuf<KMaxFeedNameLength> itemName;
		iEikonEnv->ReadResourceL(itemName, R_PODCAST_NO_SEARCH_RESULTS);
		iItemArray->Reset();
		iItemIdArray.Reset();

		TListItemProperties itemProps;
		itemProps.SetDimmed(ETrue);
		itemProps.SetHiddenSelection(ETrue);								
		iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(0, itemProps);
		}
	iListContainer->Listbox()->HandleItemAdditionL();
	DP("CPodcastSearchView::UpdateListboxItemsL END");
	}

/** 
 * Command handling function intended for overriding by sub classes. 
 * Default implementation is empty.  
 * @param aCommand ID of the command to respond to. 
 */
void CPodcastSearchView::HandleCommandL(TInt aCommand)
	{
	//CloseToolbarExtension();
	switch(aCommand)
		{
		case EPodcastAddSearchResult:
			{
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
		
			if(index < iItemArray->MdcaCount() && index >= 0)
				{
				CFeedInfo *newInfo = iPodcastModel.FeedEngine().GetSearchResults()[index];
				
				// ask if user wants to add the feed
				TBuf<KMaxMessageLength> templ;
				TBuf<KMaxMessageLength> message;
								
				iEikonEnv->ReadResourceL(templ, R_ADD_FEED_QUERY);
				message.Format(templ, &newInfo->Title());
				if(ShowQueryMessage(message)) {
					TBool added = iPodcastModel.FeedEngine().AddFeedL(*newInfo);
					
					if (added)
						{					
						// ask if user wants to update it now
						TBuf<KMaxMessageLength> message;
						iEikonEnv->ReadResourceL(message, R_ADD_FEED_SUCCESS);
						if(ShowQueryMessage(message))
							{
							CFeedInfo *info = iPodcastModel.FeedEngine().GetFeedInfoByUid(newInfo->Uid());
							
							iPodcastModel.ActiveShowList().Reset();
							iPodcastModel.SetActiveFeedInfo(info);			
							AppUi()->ActivateLocalViewL(KUidPodcastShowsViewID,  TUid::Uid(0), KNullDesC8());
							iPodcastModel.FeedEngine().UpdateFeedL(info->Uid());
							}
						}
					else
						{
						TBuf<KMaxMessageLength> message;
						iEikonEnv->ReadResourceL(message, R_ADD_FEED_EXISTS);
						ShowErrorMessage(message);
						}		
				}
				}
			}
			break;
		default:
			CPodcastListView::HandleCommandL(aCommand);
			break;
		}
		UpdateToolbar();
	}

void CPodcastSearchView::OpmlParsingComplete(TUint /*aNumFeedsImported*/)
	{
	DP("CPodcastSearchView::OpmlParsingComplete BEGIN");
	UpdateListboxItemsL();
	UpdateToolbar();
	DP("CPodcastSearchView::OpmlParsingComplete END");
	}

void CPodcastSearchView::UpdateToolbar()
{
	TBool disableAdd = iItemArray->MdcaCount() == 0 || iSearchRunning;
	
	CAknToolbar* toolbar = Toolbar();
	
	if (toolbar)
		{
		toolbar->SetItemDimmed(EPodcastAddSearchResult, disableAdd, ETrue );
		toolbar->HideItem(EPodcastSearch, iSearchRunning, ETrue );
		toolbar->HideItem(EPodcastCancelUpdateAllFeeds, !iSearchRunning, ETrue);
		}
}
