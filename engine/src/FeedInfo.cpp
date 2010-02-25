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

#include "FeedInfo.h"
#include <e32hashtab.h>
#include <fbs.h>

EXPORT_C CFeedInfo* CFeedInfo::NewL()
	{
	CFeedInfo* self = new (ELeave) CFeedInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CFeedInfo* CFeedInfo::NewLC()
	{
	CFeedInfo* self = new (ELeave) CFeedInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CFeedInfo* CFeedInfo::CopyL() const
	{
	CFeedInfo* copy = CFeedInfo::NewLC();	
	copy->SetUrlL(Url());
	copy->SetTitleL(Title());
	copy->SetDescriptionL(Description());
	copy->SetImageUrlL(ImageUrl());
	copy->SetLinkL(Link());
	copy->SetBuildDate(BuildDate());
	copy->SetLastUpdated(LastUpdated());
	copy->SetImageFileNameL(ImageFileName());
	copy->iFeedIcon->Duplicate(iFeedIcon->Handle());
	if(CustomTitle())
		{
		copy->SetCustomTitle();
		}
	
	copy->SetLastError(LastError());
	copy->SetFeedIconIndex(FeedIconIndex());
	CleanupStack::Pop(copy);
	return copy;
	}
CFeedInfo::CFeedInfo()
	{
	iCustomTitle = EFalse;
	iFeedIconIndex = -1;
	}

EXPORT_C CFeedInfo::~CFeedInfo()
	{
	delete iUrl;
	delete iTitle;
	delete iDescription;
	delete iImageUrl;
	delete iImageFileName;
	delete iLink;
	delete iFeedIcon;	
	}

void CFeedInfo::ConstructL()
	{
	iFeedIcon = new (ELeave) CFbsBitmap;
	}

EXPORT_C const TDesC& CFeedInfo::Url() const
	{
	return iUrl ? *iUrl : KNullDesC();
	}

EXPORT_C void CFeedInfo::SetUrlL(const TDesC &aUrl) 
	{
	if (iUrl)
		{
		delete iUrl;
		iUrl = NULL;
		}
		
	iUrl = aUrl.AllocL();		
	iUid = DefaultHash::Des16(Url());
	}

EXPORT_C const TDesC& CFeedInfo::Title() const
	{
	return iTitle ? *iTitle : KNullDesC();
	}

EXPORT_C void CFeedInfo::SetTitleL(const TDesC &aTitle)
	{
	if(iTitle)
		{
		delete iTitle;
		iTitle = NULL;
		}		
	iTitle = aTitle.AllocL();
	}

EXPORT_C const TDesC& CFeedInfo::Description() const
	{
	return iDescription ? *iDescription : KNullDesC();
	}

EXPORT_C void CFeedInfo::SetDescriptionL(const TDesC &aDescription)
	{
	if (iDescription)
		{
		delete iDescription;
		iDescription = NULL;
		}
	
	iDescription = aDescription.AllocL(); 
	}

EXPORT_C const TDesC& CFeedInfo::ImageUrl() const
	{
	return iImageUrl ? *iImageUrl : KNullDesC();
	}

EXPORT_C void CFeedInfo::SetImageUrlL(const TDesC &aImageUrl)
	{
	if (iImageUrl)
		{
		delete iImageUrl;
		iImageUrl = NULL;	
		}	
	iImageUrl = aImageUrl.AllocL();
	}

EXPORT_C const TDesC& CFeedInfo::Link() const
	{
	return iLink ? *iLink : KNullDesC();
	}

EXPORT_C void CFeedInfo::SetLinkL(const TDesC& aLink)
	{
	if (iLink)
		{
		delete iLink;
		iLink = NULL;
		}
	iLink = aLink.AllocL();
	}

EXPORT_C TTime CFeedInfo::BuildDate() const
	{
	return iBuildDate;
	}

EXPORT_C void CFeedInfo::SetBuildDate(TTime aBuildDate)
	{
	iBuildDate = aBuildDate;
	}

EXPORT_C TTime CFeedInfo::LastUpdated() const
	{
	return iLastUpdated;
	}

EXPORT_C void CFeedInfo::SetLastUpdated(TTime aUpdated)
	{
	iLastUpdated = aUpdated;
	}

EXPORT_C TUint CFeedInfo::Uid() const
	{
	return iUid;
	}

EXPORT_C const TDesC& CFeedInfo::ImageFileName() const
	{
	return iImageFileName ? *iImageFileName : KNullDesC();
	}

EXPORT_C void CFeedInfo::SetImageFileNameL(const TDesC& aFileName)
	{
	if (iImageFileName)
		{
		delete iImageFileName;
		iImageFileName = NULL;
		}
	iImageFileName = aFileName.AllocL();	
	}

EXPORT_C TBool CFeedInfo::CustomTitle() const
	{
	return iCustomTitle;
	}

EXPORT_C void CFeedInfo::SetCustomTitle()
	{
	iCustomTitle = ETrue;
	}

EXPORT_C void CFeedInfo::SetLastError(TInt aLastError)
	{
	iLastError = aLastError;
	}

EXPORT_C TInt CFeedInfo::LastError() const
	{
	return iLastError;
	}

EXPORT_C CFbsBitmap* CFeedInfo::FeedIcon() const
	{
	return iFeedIcon;
	}

EXPORT_C void CFeedInfo::SetFeedIcon(CFbsBitmap* aBitmapToClone)
	{
	iFeedIcon->Duplicate(aBitmapToClone->Handle());
	}

void CFeedInfo::ImageOperationCompleteL(TInt /*aError*/)
	{
	
	}

EXPORT_C TInt CFeedInfo::FeedIconIndex() const
	{
	return iFeedIconIndex;
	}

EXPORT_C void CFeedInfo::SetFeedIconIndex(TInt aIndex)
	{
	iFeedIconIndex = aIndex;
	}
