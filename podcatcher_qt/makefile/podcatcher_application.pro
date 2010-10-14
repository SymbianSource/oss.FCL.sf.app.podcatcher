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
QT					       +=	core gui

TARGET						=	podcatcher_qt
TEMPLATE					=	app


SOURCES						+=	..\src\main.cpp\
								..\src\podcatcherwin.cpp

HEADERS						+=	..\inc\podcatcherwin.h

FORMS    					+=	..\forms\podcatcherwin.ui

CONFIG						+=	mobility
MOBILITY 					= 

symbian {
    TARGET.UID3				=	0xe905eb0b
    # TARGET.CAPABILITY		+= 
    TARGET.EPOCSTACKSIZE	=	0x14000
    TARGET.EPOCHEAPSIZE		=	0x020000 0x800000	
}

LIBS						+=	-lpodcastengine.dll
