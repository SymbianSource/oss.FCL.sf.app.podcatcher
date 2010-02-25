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

#ifndef SOUND_ENGINE_H
#define SOUND_ENGINE_H
#include <e32base.h>

class CPodcastModel;

const TInt KVolumeSteps = 10;

enum TSoundEngineState
{
	ESoundEngineNotInitialized,
	ESoundEngineOpening,
	ESoundEnginePlaying,
	ESoundEnginePaused,
	ESoundEngineStopped
};

class MSoundEngineObserver
{
public:
	virtual void PlaybackInitializedL() = 0;
	virtual void PlaybackStartedL() = 0;
	virtual void PlaybackStoppedL() = 0;
	virtual void VolumeChanged(TUint aVolume, TUint aMaxVolume) = 0;
};


/**
 * This class handles all playback and audio associated resources that the application/client needs
 */
class CSoundEngine : public CBase
{
public:
	static CSoundEngine* NewL(CPodcastModel& aPodcastModel);
	~CSoundEngine();
	void OpenFileL(const TDesC& aFileName, TBool aPlayOnInit);
	IMPORT_C void Play();
	IMPORT_C void Stop(TBool aMarkPlayed=ETrue);
	IMPORT_C TTimeIntervalMicroSeconds Position();
	IMPORT_C void SetPosition(TUint aPos);
	
	IMPORT_C void Pause(TBool aOverrideState = EFalse);

	IMPORT_C TSoundEngineState State();
	IMPORT_C void AddObserver(MSoundEngineObserver* aObserver);
	void RemoveObserver(MSoundEngineObserver* aObserver);
			
	const TFileName& LastFileName();
	
private:
	void NotifyPlaybackStarted();
	void NotifyPlaybackStopped();
	void NotifyPlaybackInitialized();
	void NotifyVolumeChanged();
	
protected:
	CSoundEngine(CPodcastModel& aPodcastModel);
	void ConstructL();	
	
private:    
	CPodcastModel& iPodcastModel;
	TSoundEngineState iState;
	RArray<MSoundEngineObserver*> iObservers;
	TFileName	iLastOpenedFileName;
	TBool iPlayOnInit;
	TTimeIntervalMicroSeconds iMaxPos;
};

#endif // SOUND_ENGINE_H

