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

#ifndef SHOWENGINE_H_
#define SHOWENGINE_H_

#include <e32base.h>
#include <APGCLI.H>
#include "constants.h"
#include "ShowInfo.h"
#include "PodcastModel.h"
#include "HttpClient.h"
#include "ShowEngineObserver.h"
#include "MetaDataReader.h"
#include "sqlite3.h"

class CShowEngine : public CBase, public MHttpClientObserver, public MMetaDataReaderObserver
{
public:
	IMPORT_C static CShowEngine* NewL(CPodcastModel& aPodcastModel);
	IMPORT_C virtual ~CShowEngine();
	
public:
	IMPORT_C void AddDownloadL(CShowInfo& info);
	IMPORT_C TBool RemoveDownloadL(TUint aUid);
	IMPORT_C void RemoveAllDownloads();

	IMPORT_C void SuspendDownloads();
	IMPORT_C void ResumeDownloadsL();

	IMPORT_C TInt GetNumDownloadingShows();
	IMPORT_C CShowInfo* ShowDownloading();
	IMPORT_C CShowInfo* GetShowByUidL(TUint aShowUid);
	IMPORT_C CShowInfo* GetNextShowByTrackL(CShowInfo* aShowInfo);
	
	// show access methods
	IMPORT_C void GetAllShowsL(RShowInfoArray &aArray);
	IMPORT_C void GetShowsByFeedL(RShowInfoArray &aArray, TUint aFeedUid);
	IMPORT_C void GetShowsDownloadedL(RShowInfoArray &aArray);
	IMPORT_C void GetNewShowsL(RShowInfoArray &aArray);
	IMPORT_C void GetShowsDownloadingL(RShowInfoArray &aArray);
	IMPORT_C CShowInfo* DBGetShowByFileNameL(TFileName aFileName);
	
	IMPORT_C TBool AddShowL(const CShowInfo& item);
	IMPORT_C void DeletePlayedShows(RShowInfoArray &aShowInfoArray);
	IMPORT_C void DeleteAllShowsByFeedL(TUint aFeedUid,TBool aDeleteFiles=ETrue);
	IMPORT_C void DeleteShowL(TUint aShowUid, TBool aRemoveFile=ETrue);
	IMPORT_C void DeleteOldShowsByFeed(TUint aFeedUid);
	
	IMPORT_C void AddObserver(MShowEngineObserver *observer);
	IMPORT_C void RemoveObserver(MShowEngineObserver *observer);

	IMPORT_C void NotifyShowListUpdatedL();
	IMPORT_C void UpdateShow(CShowInfo& aInfo);

	IMPORT_C void GetMimeType(const TDesC& aFileName, TDes& aMimeType);

	IMPORT_C CMetaDataReader& MetaDataReader();

private:
	// from HttpClientObserver, dont have to be public
	void Connected(CHttpClient* aClient);
	void Disconnected(CHttpClient* aClient);
	void CompleteL(CHttpClient* aClient, TInt aError);
	void Progress(CHttpClient* aHttpClient, int aBytes, int aTotalBytes);
	void DownloadInfo(CHttpClient* aClient, int aSize);
	void FileError(TUint aError);
	// from MetaDataReaderObserver
	void ReadMetaData(CShowInfo& aShowInfo);
	void ReadMetaDataCompleteL();
	
private:
	CShowEngine(CPodcastModel& aPodcastModel);
	void ConstructL();

	TBool GetShowL(CShowInfo *info);

	void NotifyDownloadQueueUpdatedL();
	void NotifyShowDownloadUpdatedL(TInt aBytesOfCurrentDownload, TInt aBytesTotal);
	void NotifyShowFinishedL(TInt aError);
	void DownloadNextShowL();

	static TInt CompareShowsByDate(const CShowInfo &a, const CShowInfo &b);
	static TBool CompareShowsByUid(const CShowInfo &a, const CShowInfo &b);
	static TInt CompareShowsByTitle(const CShowInfo &a, const CShowInfo &b);
	static TInt CompareShowsByTrackNo(const CShowInfo &a, const CShowInfo &b);
	
	void AddShowToMpxCollection(CShowInfo &aShowInfo);

private:
	// DB methods
	CShowInfo* DBGetShowByUidL(TUint aUid);
	void DBFillShowInfoFromStmtL(sqlite3_stmt *st, CShowInfo* showInfo);
	TBool DBAddShow(const CShowInfo& aItem);
	TBool DBUpdateShow(CShowInfo& aItem);
	void DBGetShowsByFeedL(RShowInfoArray& aShowArray, TUint aFeedUid);
	void DBGetAllShowsL(RShowInfoArray& aShowArray);
	void DBGetNewShowsL(RShowInfoArray& aShowArray);
	void DBGetDownloadedShowsL(RShowInfoArray& aShowArray);
	TBool DBDeleteAllShowsByFeed(TUint aFeedUid);
	void DBDeleteOldShowsByFeed(TUint aFeedUid);
	TBool DBDeleteShow(TUint aUid);
	void DBRemoveAllDownloads();
	void DBRemoveDownload(TUint aUid);
	void DBGetAllDownloadsL(RShowInfoArray& aShowArray);
	TUint DBGetDownloadsCount();
	void DBAddDownload(TUint aUid);
	CShowInfo* DBGetNextDownloadL();
	
private:
	CHttpClient* iShowClient;			
		
	CPodcastModel& iPodcastModel;

	// observers that will receive callbacks
    RArray<MShowEngineObserver*> iObservers;

	// The show we are currently downloading
	CShowInfo* iShowDownloading;
    TUint iDownloadErrors;
    
    CMetaDataReader* iMetaDataReader;
    
    RApaLsSession iApaSession;
	TBuf8<512> iRecogBuffer;
	
	sqlite3& iDB;
    TBuf<KDefaultSQLDataBufferLength> iSqlBuffer;
};

#endif /*SHOWENGINE_H_*/

