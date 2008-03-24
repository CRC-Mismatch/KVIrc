#ifndef _OPTW_IGNORE_H_
#define _OPTW_IGNORE_H_
//
//   File : optw_ignore.h
//   Creation date : Thu Feb 12 15:11:30 2002 GMT by Juanjo �lvarez
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

#include "kvi_optionswidget.h"
#include "kvi_selectors.h"

#define KVI_OPTIONS_WIDGET_ICON_KviIgnoreOptionsWidget KVI_SMALLICON_IGNORE
#define KVI_OPTIONS_WIDGET_NAME_KviIgnoreOptionsWidget __tr2qs_no_lookup("Ignore")
#define KVI_OPTIONS_WIDGET_PARENT_KviIgnoreOptionsWidget KviProtectionOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviIgnoreOptionsWidget __tr2qs_no_lookup("protection")

class KviIgnoreOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviIgnoreOptionsWidget(QWidget * parent);
	~KviIgnoreOptionsWidget();
public:
	KviBoolSelector * m_pIgnorePrivmsg;
	KviBoolSelector * m_pIgnoreNotice;
	KviBoolSelector * m_pVerboseIgnore;
protected slots:
	void enableVerbose(bool);
};

#endif //_OPTW_IGNORE_H_
