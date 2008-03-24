#ifndef _PERLCOREINTERFACE_H_
#define _PERLCOREINTERFACE_H_

//=============================================================================
//
//   File : perlcoreinterface.h
//   Creation date : Tue Jul 13 13:03:31 2004 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_kvs_runtimecontext.h"

#include <qstringlist.h>


#define KVI_PERLCORECTRLCOMMAND_EXECUTE "execute"

typedef struct _KviPerlCoreCtrlCommand_execute
{
	unsigned int           uSize;
	KviKvsRunTimeContext * pKvsContext;
	QString                szContext;
	QString                szCode;
	bool                   bExitOk;
	QString                szRetVal;
	QString                szError;
	QStringList            lWarnings;
	QStringList            lArgs;
	bool                   bQuiet;
} KviPerlCoreCtrlCommand_execute;

#define KVI_PERLCORECTRLCOMMAND_DESTROY "destroy"

typedef struct _KviPerlCoreCtrlCommand_destroy
{
	unsigned int uSize;
	QString      szContext;
} KviPerlCoreCtrlCommand_destroy;

#endif // !_PERLCOREINTERFACE_H_
