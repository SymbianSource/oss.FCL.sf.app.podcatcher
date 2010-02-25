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
#include <e32base.h>
#include <TXTETEXT.H> // for ELineBreak
#include "PodcastUtils.h"
#include "debug.h"
#include "FeedEngine.h" // for KMaxDescriptionLength
			
EXPORT_C void PodcastUtils::FixProtocolsL(TDes &aUrl)
	{
	HBufC* urlCopy = aUrl.AllocLC();
	TPtr urlCopyPtr (urlCopy->Des());
	urlCopyPtr.LowerCase();
	
	// url is always present so access that								
	// Some pod links are written in format itpc://mylink.net/podcast.xml
	// Symbian HTTP stack does not like itpc:// 
	// Try to use a HTTP instead.
	TInt p = urlCopyPtr.Find(KItpcPrefix);
	if (p >= 0)
		{
		aUrl.Delete(p, KItpcPrefix().Length());
		}

	// Some pod links are written in format pcast://mylink.net/podcast.xml
	// Symbian HTTP stack does not like itpc:// 
	// Try to use a HTTP instead.
	p = urlCopyPtr.Find(KPcastPrefix);
	if (p >= 0)
		{
		aUrl.Delete(p, KPcastPrefix().Length());
		}

	// The URL must start with http://, otherwise the HTTP stack fails.
	TInt pos = urlCopyPtr.Find(KURLPrefix);
	if (pos == KErrNotFound)
		{
		HBufC* newUrl = HBufC::NewL(aUrl.Length() + KURLPrefix().Length());
		TPtr ptr = newUrl->Des();
		ptr.Append(KURLPrefix());
		ptr.Append(aUrl);

		// replace the url buffer
		aUrl.Copy(*newUrl);
		delete newUrl;					
		}
	
	CleanupStack::PopAndDestroy(urlCopy);
	}


EXPORT_C void PodcastUtils::CleanHtmlL(TDes &str)
{
#ifdef UIQ
	_LIT(KLineBreak, "\r\n");
#else
	const TChar KLineBreak(CEditableText::ELineBreak); 
#endif
	_LIT(KNewLine, "\n");
	ReplaceString(str, KNewLine, KNullDesC);

//	DP2("CleanHtml %d, %S", str.Length(), &str);
	TInt startPos = str.Locate('<');
	TInt endPos = str.Locate('>');
	//DP3("length: %d, startPos: %d, endPos: %d", str.Length(), startPos, endPos);
	HBufC* tmpBuf = HBufC::NewLC(KMaxDescriptionLength);
	TPtr tmp(tmpBuf->Des());
	while (startPos != KErrNotFound && endPos != KErrNotFound && endPos > startPos) {
		//DP1("Cleaning out %S", &str.Mid(startPos, endPos-startPos+1));
		tmp.Copy(str.Left(startPos));
		TPtrC ptr=str.Mid(startPos, endPos-startPos+1);
		_LIT(KHtmlBr, "<br>");
		_LIT(KHtmlBr2, "<br />");
		_LIT(KHtmlBr3, "<br/>");
		_LIT(KHtmlP, "<p>");
		if (ptr.CompareF(KHtmlBr)== 0 || ptr.CompareF(KHtmlBr2)== 0 || ptr.CompareF(KHtmlBr3)== 0)
			{
			tmp.Append(KLineBreak);
			}
		else if (ptr.CompareF(KHtmlP) == 0)
			{
			tmp.Append(KLineBreak);
			tmp.Append(KLineBreak);
			}
		
		if (str.Length() > endPos+1) {
			tmp.Append(str.Mid(endPos+1));
		}
		
		str.Copy(tmp);
		startPos = str.Locate('<');
		endPos = str.Locate('>');
	}
	
	str.Trim();
	_LIT(KAmp, "&amp;");
	_LIT(KQuot, "&quot;");
	_LIT(KNbsp, "&nbsp;");
	_LIT(KCopy, "&copy;");
	_LIT(KCopyReplacement, "(c)");
	if(str.Locate('&') != KErrNotFound) {
		ReplaceString(str, KAmp, KNullDesC);
		ReplaceString(str, KQuot, KNullDesC);
		ReplaceString(str, KNbsp, KNullDesC);
		ReplaceString(str, KCopy, KCopyReplacement);
	}
	ReplaceChar(str, '"', '\'');
	
	CleanupStack::PopAndDestroy(tmpBuf);
}

EXPORT_C void PodcastUtils::ReplaceString(TDes & aString, const TDesC& aStringToReplace, const TDesC& aReplacement)
	{
	TInt pos=aString.Find(aStringToReplace);
	TInt offset = 0;
	while (pos != KErrNotFound)
		{
		aString.Replace(offset+pos, aStringToReplace.Length(), aReplacement);
		offset += pos + aStringToReplace.Length()+1;
		if (offset > aString.Length())
			{
			return;
			}

		pos=aString.Mid(offset).Find(aStringToReplace);
		}
	}

EXPORT_C void PodcastUtils::ReplaceChar(TDes & aString, TUint aCharToReplace, TUint aReplacement)
	{
	TInt strLen=aString.Length();
	for (TInt i=0; i < strLen; i++)
		{
		if (aString[i] == aCharToReplace)
			{
			aString[i] = aReplacement;
			}
		}
	}

EXPORT_C void PodcastUtils::EnsureProperPathName(TFileName &aPath)
	{
	// from the SDK: The following characters cannot occur in the path: < >: " / |*
	
	ReplaceChar(aPath, '/', '_'); // better not to add \\ in case we have multiple /
	ReplaceChar(aPath, ':', '_');
	ReplaceChar(aPath, '?', '_');
	ReplaceChar(aPath, '|', '_');
	ReplaceChar(aPath, '*', '_');
	ReplaceChar(aPath, '<', '_');
	ReplaceChar(aPath, '>', '_');
	ReplaceChar(aPath, '"', '_');

	//buf.Append(_L("\\"));
	}

EXPORT_C void PodcastUtils::FileNameFromUrl(const TDesC& aUrl, TFileName &aFileName)
	{
	TInt pos = aUrl.LocateReverse('/');
	
	if (pos != KErrNotFound) 
		{	
		TPtrC str = aUrl.Mid(pos+1);
		pos = str.Locate('?');
		if (pos != KErrNotFound) 
			{
			aFileName.Copy(str.Left(pos));
			} 
		else 
			{
			aFileName.Copy(str);
			}
		}
	DP2("FileNameFromUrl in: %S, out: %S", &aUrl, &aFileName);
	}

EXPORT_C void PodcastUtils::SQLEncode(TDes &aString)
	{
	ReplaceString(aString, _L("\""), _L("\"\""));
	//ReplaceString(aString, _L("'"), _L("''"));	
	}

EXPORT_C void PodcastUtils::XMLEncode(TDes &aString)
	{
	ReplaceString(aString, _L("\""), _L("&quot;"));	
	ReplaceString(aString, _L("<"), _L("&lt;"));
	ReplaceString(aString, _L(">"), _L("&gt;"));
	ReplaceString(aString, _L("&"), _L("&amp;"));
	}
