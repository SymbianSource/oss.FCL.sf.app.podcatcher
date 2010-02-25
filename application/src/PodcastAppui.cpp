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

#include "PodcastAppui.h"
#include <Podcast.rsg>
#include "Podcast.hrh"
#include "PodcastFeedView.h"
#include "PodcastShowsView.h"
#include "PodcastQueueView.h"
#include "PodcastSettingsView.h"
#include "PodcastSearchView.h"
#include "ShowEngine.h"
#include "PodcastModel.h"
#include "debug.h"
#include "..\help\podcatcher.hlp.hrh"
#include "PodcastApp.h"

#include <HLPLCH.H>
#include <avkon.hrh>

const TUint KDelayLaunch = 1000;

CPodcastAppUi::CPodcastAppUi(CPodcastModel* aPodcastModel):iPodcastModel(aPodcastModel)
	{
	
	}

void CPodcastAppUi::ConstructL()
    {
    DP("CPodcastAppUi::ConstructL() BEGIN");
    BaseConstructL(CAknAppUi::EAknEnableSkin); 

    DP("Constructing FeedView");
	iFeedView = CPodcastFeedView::NewL(*iPodcastModel);
	this->AddViewL(iFeedView);

    DP("Constructing ShowsView");
	iShowsView = CPodcastShowsView::NewL(*iPodcastModel);
	this->AddViewL(iShowsView);

    DP("Constructing QueueView");
    iQueueView = CPodcastQueueView::NewL(*iPodcastModel);
	this->AddViewL(iQueueView);

    DP("Constructing SearchView");
	iSearchView = CPodcastSearchView::NewL(*iPodcastModel);
	this->AddViewL(iSearchView);
	
    DP("Constructing SettingsView");
	iSettingsView = CPodcastSettingsView::NewL(*iPodcastModel);
	this->AddViewL(iSettingsView);
	
	iNaviPane =( CAknNavigationControlContainer * ) StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
	NaviShowTabGroupL();
	
	// start a timer to let ConstructL finish before we display any dialogs or start downloading
	// this provides another entry point in HandleTimeout below
	iStartTimer = CTimeout::NewL(*this);
	iStartTimer->After(KDelayLaunch);
    DP("CPodcastAppUi::ConstructL() END");
    }

CPodcastAppUi::~CPodcastAppUi()
    {
    if(iNaviDecorator)
    	{
    	iNaviPane->Pop(iNaviDecorator);
    	delete iNaviDecorator;
    	}
	delete iStartTimer;
    }

// -----------------------------------------------------------------------------
// CPodcastAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CPodcastAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    // no implementation required 
    }

// -----------------------------------------------------------------------------
// CPodcastAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// -----------------------------------------------------------------------------
//
void CPodcastAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
            {
            Exit();
            break;
            }
        case EEikCmdExit:
        	{
			TApaTask task(CEikonEnv::Static()->WsSession());
			task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
			task.SendToBackground(); 
			break;
        	}
	case EPodcastHelp:
        	{
        	CArrayFix<TCoeHelpContext>* buf = CPodcastAppUi::AppHelpContextL();		
        	HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
        	}
        	break;      	
        default:
            break;      
        }
    }

CArrayFix<TCoeHelpContext>* CPodcastAppUi::HelpContextL() const
   { 
    CArrayFixFlat<TCoeHelpContext>* array = 
                new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(array);
    // todo: view detection doesn't seem to work
    if (ViewShown(KUidPodcastSearchViewID)) {
		array->AppendL(TCoeHelpContext(KUidPodcast,KContextSettings));
    } else {
		array->AppendL(TCoeHelpContext(KUidPodcast,KContextApplication));
    }
	
    CleanupStack::Pop(array);
    return array;
	}

void CPodcastAppUi::NaviShowTabGroupL()
	{
	iNaviDecorator = iNaviPane->CreateTabGroupL();
	
	iTabGroup = STATIC_CAST(CAknTabGroup*, iNaviDecorator->DecoratedControl());
	iTabGroup->SetTabFixedWidthL(EAknTabWidthWithTwoTabs);

	HBufC *label1 = iEikonEnv->AllocReadResourceLC(R_TABGROUP_FEEDS);
	iTabGroup->AddTabL(KTabIdFeeds,*label1);
		
	HBufC *label3 = iEikonEnv->AllocReadResourceLC(R_TABGROUP_QUEUE);
	iTabGroup->AddTabL(KTabIdQueue,*label3);
	
//	HBufC *label4 = iEikonEnv->AllocReadResourceLC(R_TABGROUP_SEARCH);			
//	iTabGroup->AddTabL(KTabIdSearch,*label4);
//	
//	CleanupStack::PopAndDestroy(label4);
	CleanupStack::PopAndDestroy(label3);
	CleanupStack::PopAndDestroy(label1);
	
	iTabGroup->SetActiveTabByIndex(0);
	iTabGroup->SetObserver(this);

	iNaviPane->Pop();
	iNaviPane->PushL(*iNaviDecorator);
	}

void CPodcastAppUi::TabChangedL (TInt aIndex)
	{
	DP("CPodcastListView::TabChangedL ");
	
	TUid newview = TUid::Uid(0);
	TUid messageUid = TUid::Uid(0);
	
	if (aIndex == KTabIdFeeds) {
		newview = KUidPodcastFeedViewID;
	} else if (aIndex == KTabIdQueue) {
		newview = KUidPodcastQueueViewID;
	} else {
		User::Leave(KErrTooBig);
	}
	
	if(newview.iUid != 0)
		{			
		ActivateLocalViewL(newview,  messageUid, KNullDesC8());
		}
	}

void CPodcastAppUi::SetActiveTab(TInt aIndex) {
	iTabGroup->SetActiveTabByIndex(aIndex);
}

void CPodcastAppUi::HandleTimeout(const CTimeout& /*aId*/, TInt /*aError*/)
	{
	iFeedView->CheckResumeDownload();
	}

void CPodcastAppUi::UpdateQueueTab(TInt aQueueLength)
	{
	if (aQueueLength == 0)
		{
		HBufC *queue = iEikonEnv->AllocReadResourceLC(R_TABGROUP_QUEUE);
		iTabGroup->ReplaceTabL(KTabIdQueue, *queue);
		CleanupStack::PopAndDestroy(queue);
		}
	else
		{
		HBufC *queueTemplate = iEikonEnv->AllocReadResourceLC(R_TABGROUP_QUEUE_COUNTER);
		HBufC *queueCounter = HBufC::NewLC(queueTemplate->Length()+2);
		queueCounter->Des().Format(*queueTemplate, aQueueLength);
		
		iTabGroup->ReplaceTabL(KTabIdQueue, *queueCounter);
		CleanupStack::PopAndDestroy(queueCounter);
		CleanupStack::PopAndDestroy(queueTemplate);	
		}
	}

void CPodcastAppUi::TabLeft()
	{
	TInt ati = iTabGroup->ActiveTabIndex();
	if(ati > 0) {
		SetActiveTab(ati-1);
		TabChangedL(ati-1);
	}
	}

void CPodcastAppUi::TabRight()
	{
	TInt ati = iTabGroup->ActiveTabIndex();
	
	if(ati < iTabGroup->TabCount()-1) {
		SetActiveTab(ati+1);
		TabChangedL(ati+1);
	}
	}
