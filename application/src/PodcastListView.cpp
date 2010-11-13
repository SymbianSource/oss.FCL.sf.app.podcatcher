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
#include <akntoolbarextension.h>
#include <aknquerydialog.h>
#include <barsread.h>
#include <akntitle.h>
#include <akniconarray.h>
#include <EIKCLBD.H>

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
	
	iListboxLandscape = new (ELeave) CAknSingleLargeStyleListBox;
	iListboxLandscape->ConstructL(this, aListboxFlags);
	iListboxLandscape->SetMopParent( this );
	iListboxLandscape->SetContainerWindowL(*this);
	iListboxLandscape->CreateScrollBarFrameL(ETrue);
	iListboxLandscape->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto );
	iListboxLandscape->SetSize(aRect.Size());
	iListboxLandscape->MakeVisible(EFalse);

	iListboxPortrait = new (ELeave) CAknDoubleLargeStyleListBox;
	iListboxPortrait->ConstructL(this, aListboxFlags);
	iListboxPortrait->SetMopParent( this );
	iListboxPortrait->SetContainerWindowL(*this);
	iListboxPortrait->CreateScrollBarFrameL(ETrue);
	iListboxPortrait->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto );
	iListboxPortrait->SetSize(aRect.Size());
	iListboxPortrait->MakeVisible(EFalse);
	
	if (aRect.Width() > aRect.Height())
		{
		iLandscape = ETrue;
		iListbox = iListboxLandscape;
		iListboxLandscape->MakeVisible(ETrue);
		}
	else 
		{
		iLandscape = EFalse;
		iListboxPortrait->MakeVisible(ETrue);
		iListbox = (CEikColumnListBox*) iListboxPortrait;
		}
 	
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
        	if (iLandscape)
        		return iListboxLandscape;
        	else
        		return iListboxPortrait;
        default:
            return NULL;
        }
    }

void CPodcastListContainer::SetLongTapDetectedL(TBool aLongTapDetected)
	{
	DP("CPodcastListContainer::SetLongTapDetectedL BEGIN");
//	iLongTapDetected = aLongTapDetected;
//	
//	if (aLongTapDetected == EFalse)
//		{
//		TPointerEvent event;
//		event.iType = TPointerEvent::EButton1Up;
//		//CCoeControl::HandlePointerEventL(event);
//		}
	DP("CPodcastListContainer::SetLongTapDetectedL END");
	}

void CPodcastListContainer::HandleResourceChange(TInt aType)
{
	switch( aType )
    	{
	    case KEikDynamicLayoutVariantSwitch:
		    SetRect(iEikonEnv->EikAppUi()->ClientRect());
		    break;
	    }
}

void CPodcastListContainer::ScrollToVisible() {
	if (iListbox != NULL) {
		iListbox->ScrollToMakeItemVisible(iListbox->CurrentItemIndex());
	}
}
void CPodcastListContainer::SizeChanged()
{
	DP2("CPodcastListContainer::SizeChanged() BEGIN, width=%d, height=%d",Size().iWidth, Size().iHeight);

	iLandscape = Size().iWidth > Size().iHeight;

	if (iContainerListener)
		iContainerListener->SizeChanged();
	
	if (iLandscape)
		{
		iListboxPortrait->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
		iListboxPortrait->UpdateScrollBarsL();
		iListboxPortrait->MakeVisible(EFalse);

		iListboxLandscape->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto );
		iListboxLandscape->MakeVisible(ETrue);
		iListboxLandscape->SetFocus(ETrue, EDrawNow);
		iListbox = iListboxLandscape;
		}
	else
		{
		iListboxLandscape->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
		iListboxLandscape->UpdateScrollBarsL();
		iListboxLandscape->MakeVisible(EFalse);	
		
		iListboxPortrait->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto );
		iListboxPortrait->MakeVisible(ETrue);
		iListboxPortrait->SetFocus(ETrue, EDrawNow);
		iListbox = (CEikColumnListBox*) iListboxPortrait;
		}

	iListbox->SetSize(Size());
    ActivateL();  		
	DrawNow();
	DP("CPodcastListContainer::SizeChanged END");
}

CEikColumnListBox* CPodcastListContainer::Listbox()
{
	return iListbox;
}

void CPodcastListContainer::SetListboxObserver(MEikListBoxObserver *aObserver)
	{
	iListboxLandscape->SetListBoxObserver(aObserver);
	iListboxPortrait->SetListBoxObserver(aObserver);
	}
		
void CPodcastListContainer::SetListboxIcons(CArrayPtr< CGulIcon >* aIcons)
{
	iListboxLandscape->ItemDrawer()->ColumnData()->SetIconArray(aIcons);
	iListboxPortrait->ItemDrawer()->FormattedCellData()->SetIconArrayL(aIcons);
}

CArrayPtr<CGulIcon>* CPodcastListContainer::ListboxIcons()
	{
	if (iListboxLandscape == iListbox)
		return iListboxLandscape->ItemDrawer()->ColumnData()->IconArray();
	else
		return iListboxPortrait->ItemDrawer()->FormattedCellData()->IconArray();
	}

void CPodcastListContainer::SetListboxTextArrays(CDesCArray* aPortraitArray, CDesCArray* aLandscapeArray)
	{
	iListboxLandscape->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iListboxLandscape->Model()->SetItemTextArray(aLandscapeArray);
	iListboxPortrait->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iListboxPortrait->Model()->SetItemTextArray(aPortraitArray);
	}

CPodcastListContainer::~CPodcastListContainer()
{
	DP("CPodcastListContainer::~CPodcastListContainer BEGIN");
	iListboxLandscape->ItemDrawer()->ColumnData()->SetIconArray(NULL);
	delete iListboxPortrait;
	delete iListboxLandscape;
	delete iBgContext;
	DP("CPodcastListContainer::~CPodcastListContainer END");
}

void CPodcastListContainer::SetEmptyText(const TDesC &aText)
	{
	iListboxPortrait->View()->SetListEmptyTextL(aText);
	iListboxLandscape->View()->SetListEmptyTextL(aText);
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
	if (!iLongTapDetected)
		{
		CCoeControl::HandlePointerEventL(aPointerEvent);
		}
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
	iItemArrayShort = new (ELeave)CDesCArrayFlat(KDefaultGran);
		
	iListContainer->SetListboxTextArrays(iItemArray, iItemArrayShort);
	iListContainer->SetContainerListener(this);
	iListContainer->SetListboxObserver(this);
	
	if (Toolbar()) {
		iToolbar = Toolbar();
		iToolbar->SetToolbarObserver(this);
	}
	
	iLongTapDetector = CAknLongTapDetector::NewL(this);
        
	DP("CPodcastListView::ConstructL END");
}

void CPodcastListView::HandleViewRectChange()
{    
	TBool wasVisible = iListContainer->IsVisible();

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
    delete iItemArrayShort;
    delete iLongTapDetector;
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
	CAknNoteDialog* dlg = new(ELeave) CAknNoteDialog();
	HBufC *aboutTextTemplate = iEikonEnv->AllocReadResourceLC(R_ABOUT_TEXT);
	TBuf<255> aboutText;
	aboutText.Format(*aboutTextTemplate, BUILD_NO);
	dlg->SetTextL(aboutText);
	CleanupStack::PopAndDestroy(aboutTextTemplate);
	dlg->ExecuteLD(R_DLG_ABOUT);
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

void CPodcastListView::CloseToolbarExtension()
{
	CAknToolbar* toolbar = Toolbar();
	if (toolbar) {
		CAknToolbarExtension* toolbarExtension = toolbar->ToolbarExtension();
		if (toolbarExtension) {
		toolbarExtension->SetShown( EFalse );
		}
	}
}

void CPodcastListView::PointerEventL(const TPointerEvent& aPointerEvent)
	{
	//DP1("CPodcastListView::PointerEventL, iType=%d", aPointerEvent.iType);
	// Pass the pointer event to Long tap detector component
	iLongTapDetector->PointerEventL(aPointerEvent);
	}


void CPodcastListView::HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& /* aPenEventScreenLocation */)
{
	DP("CPodcastListView::HandleLongTapEventL BEGIN");
	iListContainer->SetLongTapDetectedL(ETrue);
	
	const TInt KListboxDefaultHeight = 19; // for some reason it returns 19 for an empty listbox in S^1
	TInt lbHeight = iListContainer->Listbox()->CalcHeightBasedOnNumOfItems(
			iListContainer->Listbox()->Model()->NumberOfItems()) - KListboxDefaultHeight;

    if(iStylusPopupMenu && aPenEventLocation.iY < lbHeight)
    {
		iStylusPopupMenu->ShowMenu();
		iStylusPopupMenu->SetPosition(aPenEventLocation);
    }
    
	DP("CPodcastListView::HandleLongTapEventL END");
}


void CPodcastListView::DynInitToolbarL (TInt /*aResourceId*/, CAknToolbar * /*aToolbar*/)
	{
	}


void CPodcastListView::OfferToolbarEventL(TInt aCommand)
	{
	HandleCommandL(aCommand);
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

