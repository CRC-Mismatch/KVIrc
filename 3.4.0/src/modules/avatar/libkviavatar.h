#ifndef _LIBKVIAVATAR_H_
#define _LIBKVIAVATAR_H_
//=============================================================================
//
//   File : libkviavatar.cpp
//   Creation date : Thu Nov 12 02:32:59 2004 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
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
#include <qdialog.h>

class QLineEdit;
class KviIrcConnection;

class KviAsyncAvatarSelectionDialog : public QDialog
{
	Q_OBJECT
public:
	KviAsyncAvatarSelectionDialog(QWidget * par,const QString &szInitialPath,KviIrcConnection * c);
	~KviAsyncAvatarSelectionDialog();
protected:
	QLineEdit        * m_pLineEdit;
	QString            m_szAvatarName;
	KviIrcConnection * m_pConnection;
protected:
	virtual void closeEvent(QCloseEvent * e);
	const QString &avatarName(){ return m_szAvatarName; };
protected slots:
	void okClicked();
	void cancelClicked();
	void chooseFileClicked();
};


#endif //!_LIBKVIAVATAR_H_
