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

#ifndef PODCASTMODEL_H
#define PODCASTMODEL_H
#include <e32base.h>
#include <eikenv.h>
#include <commdb.h>
#include <CommDbConnPref.h>
#include <es_sock.h>
#include <http/rhttpsession.h>
#include <cmmanager.h>

#include "FeedInfo.h"
#include "ShowInfo.h"
#include "debug.h"
#include "sqlite3.h"
#include "ImageHandler.h"

// SQLite leaks memory when sorting, so to test our own memory leaks we need to disable this
//#define DONT_SORT_SQL
class RCmManager;
class CFeedEngine;
class CSoundEngine;
class CShowEngine;
class CSettingsEngine;
class CCommsDatabase;
class CConnectionEngine;

class TPodcastIAPItem
{
public:
	TUint32 iIapId;
	TBuf<KCommsDbSvrMaxFieldLength> iServiceType;
	TBuf<KCommsDbSvrMaxFieldLength> iBearerType;
};

/**
 * This class handles application storage needs and ownership of audioplayer, resource lists etc.
 */
class CPodcastModel : public CBase, public MImageHandlerCallback
{
public:
	IMPORT_C static CPodcastModel* NewL();
	IMPORT_C ~CPodcastModel();
	IMPORT_C CFeedEngine& FeedEngine();
	IMPORT_C CShowEngine& ShowEngine();
	IMPORT_C CSoundEngine& SoundEngine();
	IMPORT_C CSettingsEngine& SettingsEngine();
	IMPORT_C CConnectionEngine& ConnectionEngine();
	IMPORT_C CShowInfo* PlayingPodcast();
	IMPORT_C void SetPlayingPodcast(CShowInfo* aPodcast);
	IMPORT_C void PlayPausePodcastL(CShowInfo * aPodcast, TBool aPlayOnInit = EFalse);
	IMPORT_C CFeedInfo* ActiveFeedInfo();
	IMPORT_C void SetActiveFeedInfo(CFeedInfo* aFeedInfo);
	
	RFs& FsSession();
	
	IMPORT_C RShowInfoArray& ActiveShowList();
	
	void SetActiveShowList(RShowInfoArray& aShowArray);
	
	IMPORT_C void UpdateIAPListL();
	IMPORT_C void UpdateSNAPListL();
	IMPORT_C CDesCArrayFlat* IAPNames();
	IMPORT_C RArray<TPodcastIAPItem>& IAPIds();
	IMPORT_C CDesCArrayFlat* SNAPNames();
	IMPORT_C RArray<TPodcastIAPItem>& SNAPIds();

	void SetProxyUsageIfNeededL(RHTTPSession& aSession);
	void GetProxyInformationForConnectionL(TBool& aIsUsed, HBufC*& aServerName, TUint32& aPort);
	TInt GetIapId();
	
	sqlite3* DB();
	
	IMPORT_C void GetAllShows();
	IMPORT_C void GetNewShows();
	IMPORT_C void GetShowsDownloaded();
	IMPORT_C void GetShowsDownloading();
	IMPORT_C void GetShowsByFeed(TUint aFeedUid);
	IMPORT_C void MarkSelectionPlayed();
	
	TInt FindActiveShowByUid(TUint aUid);
	IMPORT_C TBool IsFirstStartup();
	IMPORT_C CImageHandler& ImageHandler();
protected:
	CPodcastModel();
	void ConstructL();
	// From ImageHandler
	void ImageOperationCompleteL(TInt aError);
private:	
   CShowInfo* iPlayingPodcast;
   
   CFeedEngine* iFeedEngine;
   CShowEngine* iShowEngine;
   CSoundEngine* iSoundEngine;
   CSettingsEngine *iSettingsEngine;
   CConnectionEngine* iConnectionEngine;
   RShowInfoArray iActiveShowList;
   CFeedInfo *iActiveFeed;   
   
   RFs iFsSession;
   RArray<TPodcastIAPItem> iIapIdArray;
   CDesCArrayFlat* iIapNameArray;
   
   RArray<TPodcastIAPItem> iSNAPIdArray;
   CDesCArrayFlat* iSNAPNameArray;
   CCommsDatabase* iCommDB;  
   
   sqlite3* iDB;
   RCmManager iCmManager;
   TBool iIsFirstStartup;
   CImageHandler* iImageHandler;
};

#endif // PODCASTMODEL_H
