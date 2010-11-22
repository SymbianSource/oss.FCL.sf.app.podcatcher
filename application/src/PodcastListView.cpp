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

#include "PodcastListView.h"
#include "Podcast.hrh"
#include "PodcastAppUi.h"
#include "constants.h"
#include <podcast.rsg>
#include <aknlists.h> 
#include <aknviewappui.h>
#include <aknnotedialog.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <akntabgrp.h>
#include <aknquerydialog.h>
#include <barsread.h>
#include <akntitle.h>
#include <akniconarray.h>
#include <EIKCLBD.H>
#include <aknmessagequerydialog.h>
#include "buildno.h"

const TInt KDefaultGran = 5;

CPodcastListContainer::CPodcastListContainer()
{
}

void CPodcastListContainer::SetContainerListener(MContainerListener *aContainerListener)
	{
	iContainerListener = aContainerListener;
	}

TKeyResponse CPodcastListContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse response = iListbox->OfferKeyEventL(aKeyEvent, aType);
	if (iContainerListener)
		iContainerListener->OfferKeyEventL(aKeyEvent, aType);
	
	return response;
}

void CPodcastListContainer::ConstructL( const TRect& aRect, TInt aListboxFlags )
{
	DP("CPodcastListContainer::ConstructL BEGIN");
	CreateWindowL();

	iBgContext = 
		    CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain, 
		                                              aRect, 
		                                              ETrue );
		
	 // Set the windows size
    SetRect( aRect );    
    iListbox =static_cast<CEikFormattedCellListBox*>( new (ELeave) CAknDoubleLargeStyleListBox);
    iListbox->ConstructL(this, aListboxFlags);
    iListbox->SetMopParent( this );
	iListbox->SetContainerWindowL(*this);
	iListbox->CreateScrollBarFrameL(ETrue);
	iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto );
	
	iListbox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );

	iListbox->SetSize(aRect.Size());
	iListbox->MakeVisible(ETrue);
    MakeVisible(EFalse);

	 // Set the windows size
    SetRect( aRect ); 

	// Activate the window, which makes it ready to be drawn
    ActivateL();   
    DP("CPodcastListContainer::ConstructL END");
}

TInt CPodcastListContainer::CountComponentControls() const
    {
    return 1; // return number of controls inside this container
    }

CCoeControl* CPodcastListContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListbox;
        default:
            return NULL;
        }
    }

void CPodcastListContainer::HandleResourceChange(TInt aType)
{
	CCoeControl::HandleResourceChange(aType);
	if ( aType==KEikDynamicLayoutVariantSwitch )
		{
		TRect rect;
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
		SetRect( rect );
		
		if ( iBgContext )
		   	{
		   	iBgContext->SetRect( rect );
		   	}
		DrawNow();
		}
//
//	switch( aType )
//    	{
//	    case KEikDynamicLayoutVariantSwitch:
//		    SetRect(iEikonEnv->EikAppUi()->ClientRect());
//		    break;
//	    }
}

void CPodcastListContainer::ScrollToVisible() {
	if (iListbox != NULL) {
		iListbox->ScrollToMakeItemVisible(iListbox->CurrentItemIndex());
	}
}
void CPodcastListContainer::SizeChanged()
{
	DP2("CPodcastListContainer::SizeChanged(), width=%d, height=%d",Size().iWidth, Size().iHeight);
	if(iListbox != NULL)
	{
		iListbox->SetSize(Size());
	}
}

CEikFormattedCellListBox* CPodcastListContainer::Listbox()
{
	return iListbox;
}

void CPodcastListContainer::SetListboxObserver(MEikListBoxObserver *aObserver)
	{
	iListbox->SetListBoxObserver(aObserver);
	}
		
void CPodcastListContainer::SetListboxIcons(CArrayPtr< CGulIcon >* aIcons)
{
	iListbox->ItemDrawer()->ColumnData()->SetIconArray(aIcons);
}

CArrayPtr<CGulIcon>* CPodcastListContainer::ListboxIcons()
	{
	return iListbox->ItemDrawer()->FormattedCellData()->IconArray();
	}

void CPodcastListContainer::SetListboxTextArrays(CDesCArray* aPortraitArray, CDesCArray* aLandscapeArray)
	{
	iListbox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iListbox->Model()->SetItemTextArray(aPortraitArray);
	}

CPodcastListContainer::~CPodcastListContainer()
{
	delete iListbox;
	delete iBgContext;
}

void CPodcastListContainer::SetEmptyText(const TDesC &aText)
	{
	iListbox->View()->SetListEmptyTextL(aText);
	}

void CPodcastListContainer::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	MAknsControlContext* cc = AknsDrawUtils::ControlContext(this);
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsDrawUtils::Background(skin, cc, (CCoeControl*) this, gc, aRect);
	}


TTypeUid::Ptr CPodcastListContainer::MopSupplyObject( TTypeUid aId )
    {
    if (iBgContext )
    {
      return MAknsControlContext::SupplyMopObject( aId, iBgContext );
    }
    return CCoeControl::MopSupplyObject(aId);
    }

void CPodcastListContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	if (iContainerListener)
		iContainerListener->PointerEventL(aPointerEvent);

	// Call base class HandlePointerEventL() if not a long tap
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

CPodcastListView::CPodcastListView()
{
}

void CPodcastListView::ConstructL()
{
	DP("CPodcastListView::ConstructL BEGIN");

	iListContainer = new (ELeave) CPodcastListContainer;
	TRect rect = ClientRect();
	
	iListContainer->ConstructL(rect, iListboxFlags);
	iListContainer->SetMopParent(this);
	iListContainer->ActivateL();
	
	iItemArray = new (ELeave)CDesCArrayFlat(KDefaultGran);
	iListContainer->Listbox()->Model()->SetItemTextArray(iItemArray);
	iListContainer->Listbox()->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

	iListContainer->SetContainerListener(this);
	iListContainer->SetListboxObserver(this);
        
	DP("CPodcastListView::ConstructL END");
}

void CPodcastListView::HandleViewRectChange()
{    
	if ( iListContainer )
	{
        iListContainer->SetRect( ClientRect() );
	}
}

void CPodcastListView::HandleStatusPaneSizeChange()
{
	DP2("CPodcastListView::HandleStatusPaneSizeChange(), width=%d, height=%d", ClientRect().Width(), ClientRect().Height());

	HandleViewRectChange();
}

    
CPodcastListView::~CPodcastListView()
    {
    if(iListContainer)
    	{
    	AppUi()->RemoveFromStack(iListContainer);
    	delete iListContainer;
    	}
         
    delete iItemArray;
    iItemIdArray.Close();
    }

	
void CPodcastListView::DoActivateL(const TVwsViewId& /*aPrevViewId */,
	                                  TUid /*aCustomMessageId */,
									  const TDesC8& /* aCustomMessage */)
{	
	DP("CPodcastListView::DoActivateL() BEGIN");
	
	if(iListContainer)
	{
		iListContainer->SetSize(ClientRect().Size());
		iListContainer->SetMopParent(this);
		
		AppUi()->AddToStackL(*this, iListContainer);
		iListContainer->MakeVisible(ETrue);
	}
	
	CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
		      ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
	titlePane->SetTextToDefaultL();

	iListContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
	DP("CPodcastListView::DoActivateL() END");

}

void CPodcastListView::DoDeactivate()
{
	DP("CPodcastListView::DoDeactivate() BEGIN");
	if ( iListContainer )
	{
		AppUi()->RemoveFromViewStack( *this, iListContainer);
		iListContainer->MakeVisible(EFalse);
	}
	DP("CPodcastListView::DoDeactivate() END");
}

/** 
* Command handling function intended for overriding by sub classes. 
* Default implementation is empty.  
* @param aCommand ID of the command to respond to. 
*/
void CPodcastListView::HandleCommandL(TInt aCommand)
{
	DP1("CPodcastListView::HandleCommandL=%d", aCommand);
	switch(aCommand)
	{
	case EPodcastHide:
		AppUi()->HandleCommandL(EEikCmdExit);
		break;
	case EAknSoftkeyBack:
		{
		AppUi()->ActivateViewL(iPreviousView);
		((CPodcastAppUi*)AppUi())->SetActiveTab(KTabIdFeeds);
		}
		break;
	case EPodcastSettings:
		AppUi()->ActivateLocalViewL(KUidPodcastSettingsViewID);
		break;
	case EPodcastAbout:
		RunAboutDialogL();
		break;
	default:
		AppUi()->HandleCommandL(aCommand);
	}
}

TBool CPodcastListView::IsVisible()
	{
	return iListContainer->IsVisible();
	}

void CPodcastListView::RunAboutDialogL()
{
	HBufC *aboutTextTitle = iEikonEnv->AllocReadResourceLC(R_ABOUT_TITLE);
	
	HBufC *aboutTextTemplate = iEikonEnv->AllocReadResourceLC(R_ABOUT_BODY);
	TBuf<255> aboutText;
	aboutText.Format(*aboutTextTemplate, BUILD_NO);
	CleanupStack::PopAndDestroy(aboutTextTemplate);
	
	HBufC *aboutTextBody = aboutText.AllocLC();
	
	CAknMessageQueryDialog* note = new ( ELeave ) CAknMessageQueryDialog(aboutTextBody, aboutTextTitle );
						
	note->PrepareLC( R_SHOW_INFO_NOTE ); // Adds to CleanupStack
	note->RunLD();

	CleanupStack::Pop(aboutTextBody);
	CleanupStack::Pop(aboutTextTitle);
}

void CPodcastListView::SetEmptyTextL(TInt aResourceId)
	{
	HBufC* emptyText =  iEikonEnv->AllocReadResourceLC(aResourceId);
	iListContainer->SetEmptyText(*emptyText);
	CleanupStack::PopAndDestroy(emptyText);	
	}

void CPodcastListView::ShowOkMessageL(TDesC &aText)
	{
	CAknNoteDialog* dlg= new(ELeave) CAknNoteDialog();
	CleanupStack::PushL(dlg);
	dlg->SetTextL(aText);
	CleanupStack::Pop(dlg);
	dlg->ExecuteLD(R_MESSAGEDLG_OK);
	}

void CPodcastListView::ShowErrorMessageL(TDesC &aText)
	{
	CAknNoteDialog* dlg= new(ELeave) CAknNoteDialog();
	CleanupStack::PushL(dlg);
	dlg->SetTextL(aText);
	CleanupStack::Pop(dlg);
	dlg->ExecuteLD(R_ERRORDLG_OK);
	}

TInt CPodcastListView::ShowQueryMessageL(TDesC &aText)
	{
	CAknQueryDialog* dlg= new(ELeave) CAknQueryDialog();
	
	CleanupStack::PushL(dlg);
	dlg->SetPromptL(aText);
	CleanupStack::Pop(dlg);
	return dlg->ExecuteLD(R_QUERYDLG);
	}

void CPodcastListView::PointerEventL(const TPointerEvent& aPointerEvent)
	{
	//DP1("CPodcastListView::PointerEventL, iType=%d", aPointerEvent.iType);
	// Pass the pointer event to Long tap detector component
	}

void CPodcastListView::ShowWaitDialogL(TDesC &aWaitText)
	{
	DP("CPodcastListView::ShowWaitDialogL BEGIN");
	
	if (iWaitDialog) {
		User::Leave(KErrInUse);
	}

	iWaitDialog=new(ELeave) CAknWaitDialog(reinterpret_cast<CEikDialog**>(&iWaitDialog), EFalse);
	iWaitDialog->SetCallback(this);
	iWaitDialog->ExecuteLD(R_WAITDLG);
	iWaitDialog->SetTextL(aWaitText);
	DP("CPodcastListView::ShowWaitDialogL END");
	}

TKeyResponse CPodcastListView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType==EEventKey)
		{
		switch (aKeyEvent.iCode)
			{
			case EKeyRightArrow:
				((CPodcastAppUi*)AppUi())->TabRightL();
				return EKeyWasConsumed;
			case EKeyLeftArrow:
				((CPodcastAppUi*)AppUi())->TabLeftL();
				return EKeyWasConsumed;
			}
		}
	return EKeyWasNotConsumed;
	}

