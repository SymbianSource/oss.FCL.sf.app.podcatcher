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

#include <e32std.h>
#include "SettingsEngine.h"
#include "SoundEngine.h"
#include "PodcastModel.h"
#include "ShowInfo.h"
#include "ShowEngine.h"

CSoundEngine* CSoundEngine::NewL(CPodcastModel& aPodcastModel)
{
	CSoundEngine* self = new (ELeave) CSoundEngine(aPodcastModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CSoundEngine::~CSoundEngine()
{	
	iObservers.Close();
}

CSoundEngine::CSoundEngine(CPodcastModel& aPodcastModel): iPodcastModel(aPodcastModel)
{
}

void CSoundEngine::ConstructL()
{   
}

EXPORT_C void CSoundEngine::AddObserver(MSoundEngineObserver* aObserver)
{
	iObservers.Append(aObserver);
}

void CSoundEngine::RemoveObserver(MSoundEngineObserver* observer)
	{
	TInt index = iObservers.Find(observer);
	
	if (index > KErrNotFound)
		{
		iObservers.Remove(index);
		}
	}

void CSoundEngine::OpenFileL(const TDesC& aFileName, TBool aPlayOnInit)
{
	iState = ESoundEngineNotInitialized;
	iMaxPos = 0;
	iLastOpenedFileName= aFileName;
	
	iPlayOnInit = aPlayOnInit;
	iState = ESoundEngineOpening;
}

const TFileName& CSoundEngine::LastFileName()
{
	return iLastOpenedFileName;
}

EXPORT_C TTimeIntervalMicroSeconds CSoundEngine::Position()
{
	TTimeIntervalMicroSeconds pos = 0;
	
	if(iState > ESoundEngineOpening)
	{
		
	}

	// store maximum position, we need this if we get interrupted by a phone call
	if (pos > iMaxPos) {
		iMaxPos = pos;
	}
	return iMaxPos;
}

EXPORT_C void CSoundEngine::SetPosition(TUint aPos)
{
	if(iState > ESoundEngineOpening)
	{
		TTimeIntervalMicroSeconds pos = ((TUint64)aPos)*1000000;
		if(iState == ESoundEnginePlaying)
		{
			//iPlayer->Pause();
		}

		iMaxPos = pos;
		//iPlayer->SetPosition(pos);
		
		if(iState == ESoundEnginePlaying)
		{
			//iPlayer->Play();
		}
	}
}

EXPORT_C void CSoundEngine::Play()
{
	if(iState > ESoundEngineOpening)
	{
		//iPlayer->SetPosition(iMaxPos);
		//iPlayer->Play();
		iState = ESoundEnginePlaying;

		NotifyPlaybackStarted();
	}
}

EXPORT_C void CSoundEngine::Stop(TBool aMarkPlayed)
{
	if(iState > ESoundEngineOpening)
	{
		if (aMarkPlayed) {
			// seem to need to do this here, even though we do it in MapcPlayComplete
			iPodcastModel.PlayingPodcast()->SetPlayState(EPlayed);
		}
		iState = ESoundEngineStopped;
		SetPosition(0);
		//iPlayer->Stop();
		//iPlayer->Close();
		iMaxPos = 0;
		
		NotifyPlaybackStopped();
	}
}

EXPORT_C void CSoundEngine::Pause(TBool aOverrideState)
{
	DP("Pause");
	if(iState > ESoundEngineOpening || aOverrideState)
	{
		iState = ESoundEnginePaused;
		//iPlayer->Pause();

		// had a crash here, so we check for NULL first
		if (iPodcastModel.PlayingPodcast() != NULL) {
			iPodcastModel.PlayingPodcast()->SetPosition(iMaxPos);
		}
	}
}

EXPORT_C TSoundEngineState CSoundEngine::State()
{
	return iState;
}

void CSoundEngine::NotifyPlaybackStopped()
	{
	for (int i=0;i<iObservers.Count();i++) {
		TRAPD(err, iObservers[i]->PlaybackStoppedL());
	}
	
	}


void CSoundEngine::NotifyPlaybackStarted()
	{
	for (int i=0;i<iObservers.Count();i++) {
		TRAPD(err, iObservers[i]->PlaybackStartedL());
	}
	
	}

void CSoundEngine::NotifyPlaybackInitialized()
	{
	for (int i=0;i<iObservers.Count();i++) {
		TRAPD(err, iObservers[i]->PlaybackInitializedL());
	}
	}

void CSoundEngine::NotifyVolumeChanged()
	{
	TInt max = 0;//iPlayer->MaxVolume();
	
	TInt vol = 0;
	//iPlayer->GetVolume(vol);
	
	DP2("NotifyVolumeChanged, vol=%d, max=%d", vol, max);
	for (int i=0;i<iObservers.Count();i++) {
		TRAPD(err, iObservers[i]->VolumeChanged(vol, max));
	}
	
	}
