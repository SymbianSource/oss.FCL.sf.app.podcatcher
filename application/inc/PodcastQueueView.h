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

#ifndef PODCASTQUEUEVIEWH
#define PODCASTQUEUEVIEWH 

#include <aknview.h>
#include <aknlists.h> 
#include <eiklbo.h>
#include <AknToolbarObserver.h>
#include <AknToolbar.h>
#include "ShowEngine.h"
#include "FeedEngine.h"
#include "PodcastModel.h"
#include "Podcast.hrh"
#include "PodcastListView.h"

class CPodcastQueueView : public CPodcastListView, public MEikListBoxObserver, 
	public MFeedEngineObserver, public MShowEngineObserver
	{
public: 
	static CPodcastQueueView* NewL(CPodcastModel& aPodcastModel);
	static CPodcastQueueView* NewLC(CPodcastModel& aPodcastModel);
	~CPodcastQueueView();
protected:
	void ConstructL();
	CPodcastQueueView(CPodcastModel& aPodcastModel);

	/**
	 * Returns views id, intended for overriding by sub classes.
	 * @return id for this view.
	 */
	TUid Id() const;

	/** 
	 * Handles a view activation and passes the message of type 
	 * @c aCustomMessageId. This function is intended for overriding by 
	 * sub classes. This function is called by @c AknViewActivatedL().
	 * @param aPrevViewId Specifies the view previously active.
	 * @param aCustomMessageId Specifies the message type.
	 * @param aCustomMessage The activation message.
	 */
	void DoActivateL(const TVwsViewId& aPrevViewId,
			TUid aCustomMessageId,
			const TDesC8& aCustomMessage);

	/** 
	 * View deactivation function intended for overriding by sub classes. 
	 * This function is called by @c AknViewDeactivated().
	 */
	void DoDeactivate();

	void UpdateListboxItemsL();

	// From // MEikListBoxObserverClass
	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
	// From showengine observer
	void ShowListUpdatedL();  	
	void ShowDownloadUpdatedL(TInt aBytesOfCurrentDownload, TInt aBytesTotal);	
	void DownloadQueueUpdatedL(TInt aDownloadingShows, TInt aQueuedShows);
	void ShowDownloadFinishedL(TUint aShowUid, TInt aError) ;
	// from MFeedEngineObserver
	void FeedDownloadStartedL(TFeedState aState,TUint aFeedUid);
	void FeedDownloadFinishedL(TFeedState aState,TUint aFeedUid, TInt aError);
	void FeedUpdateAllCompleteL(TFeedState aState);
	void OpmlParsingComplete(TUint /*aNumFeedsImported*/) {}

	/** 
	 * Command handling function intended for overriding by sub classes. 
	 * Default implementation is empty.  
	 * @param aCommand ID of the command to respond to. 
	 */
	void HandleCommandL(TInt aCommand);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

	void UpdateFeedUpdateStateL();
	void UpdateShowItemL(TUint aUid, TInt aSizeDownloaded);
	void UpdateShowItemDataL(CShowInfo* aShowInfo,TInt aIndex, TInt aSizeDownloaded = KErrNotFound);
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	void FormatShowInfoListBoxItemL(CShowInfo& aShowInfo, TInt aSizeDownloaded = 0);

	void HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& aPenEventScreenLocation);

private:
	void GetShowIcons(CShowInfo* aShowInfo, TInt& aIconIndex);
	void UpdateToolbar();

	
private:
	
	CPodcastModel& iPodcastModel;
	TBool iProgressAdded;	
	TInt iLastImageHandlerError;
	TBool iSetTitlebarImage;
	};

#endif // PODCASTSHOWSVIEWH


