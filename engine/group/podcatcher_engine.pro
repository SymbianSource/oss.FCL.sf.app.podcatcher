#-----------------------------------------------------------------------------
# Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, Marcin Pogorzelski, EmbedDev AB
#
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# EmbedDev AB - initial contribution.
#
# Contributors: Marcin Pogorzelski
#
# Description: QT pro file.
#
#-----------------------------------------------------------------------------
TARGET = podcastengine


QT -= core
QT -= gui
TEMPLATE = lib
CONFIG += dll
CONGIH -=QT

ENABLE_MPX_INTEGRATION = 0;
INCLUDEPATH += /epoc32/include/mw
INCLUDEPATH += /epoc32/include/platform
INCLUDEPATH += /epoc32/include/platform/mw
INCLUDEPATH += /epoc32/include/stdapis
INCLUDEPATH += ../inc


SOURCES	+=	..\src\HttpClient.cpp \
			..\src\HttpEventHandler.cpp \
			..\src\FeedParser.cpp \
			..\src\FeedEngine.cpp \
			..\src\ShowInfo.cpp \
			..\src\FeedInfo.cpp \
			..\src\FeedTimer.cpp \
			..\src\ShowEngine.cpp \
			..\src\SettingsEngine.cpp \
			..\src\MetaDataReader.cpp \
			..\src\OpmlParser.cpp \
			..\src\PodcastUtils.cpp \
			..\src\PodCastModel.cpp \ 
			..\src\ImageHandler.cpp \
 			..\src\ConnectionEngine.cpp
			
symbian {
	TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0xA11F867F
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.CAPABILITY += NetworkServices UserEnvironment
	contains( ENABLE_MPX_INTEGRATION, 1 ) {
		TARGET.CAPABILITY += WriteUserData ReadDeviceData WriteDeviceData MultimediaDD
    }
	
			
	BLD_INF_RULES.prj_exports += "..\config\podcatcher.sqlite z:\private\A0009D00\podcatcher.sqlite.template"
	BLD_INF_RULES.prj_exports += "..\config\defaultfeeds.xml z:\private\A0009D00\defaultfeeds.xml"
	BLD_INF_RULES.prj_exports += "..\config\testfeeds.xml c:\data\testfeeds.xml"
	BLD_INF_RULES.prj_platforms = DEFAULT
}

LIBS	+=	-lsqlite3.dll -lbafl.dll -lefsrv.dll -lmediaclientaudio.dll \
			-linetprotutil.dll -lcharconv.dll -lxmlframework.dll -lhttp.dll \
			-lflogger.dll -lezlib.dll -lestor.dll  -lcommdb.dll -lesock.dll \
			-lmmfcontrollerframework.dll -lapgrfx.dll -lcone.dll -lapmime.dll \
			-lcmmanager.dll -lfbscli.dll -limageconversion.dll -lbitmaptransforms.dll \
			-leikcore.dll -lcommonui.dll -lapparc.dll -lavkon.dll -lcentralrepository.dll

contains( ENABLE_MPX_INTEGRATION, 1 ) {
		LIBS	+=	mpxcollectionhelper.dll
}			

