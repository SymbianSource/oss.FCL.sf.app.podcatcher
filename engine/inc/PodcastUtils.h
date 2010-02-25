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

#ifndef PODCASTUTILS_H_
#define PODCASTUTILS_H_

#include <e32cmn.h>

_LIT(KURLPrefix, "http://");
_LIT(KItpcPrefix, "itpc://");
_LIT(KPcastPrefix, "pcast://");			

class PodcastUtils
	{
public:
	IMPORT_C static void FixProtocolsL(TDes &aUrl);
	IMPORT_C static void CleanHtmlL(TDes &str);
	IMPORT_C static void ReplaceString(TDes & aString, const TDesC& aStringToReplace, const TDesC& aReplacement);
	IMPORT_C static void ReplaceChar(TDes & aString, TUint aCharToReplace, TUint aReplacement);
	IMPORT_C static void EnsureProperPathName(TFileName &aPath);
	IMPORT_C static void FileNameFromUrl(const TDesC &aUrl, TFileName &aFileName);
	IMPORT_C static void SQLEncode(TDes &aString);
	IMPORT_C static void XMLEncode(TDes &aString);
};

#endif /* PODCASTUTILS_H_ */
