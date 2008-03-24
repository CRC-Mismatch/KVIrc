//=============================================================================
//
//   File : edituser.cpp
//   Creation date : Tue Dec 26 2000 12:24:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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
#define _WANT_OPTION_FLAGS_
#define _EDITUSER_CPP_

#include "edituser.h"
#include "kvi_regusersdb.h"

#include "kvi_locale.h"
#include "kvi_ircmask.h"
#include "kvi_debug.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_file.h"
#include "kvi_filedialog.h"
#include "kvi_msgbox.h"
#include "kvi_fileutils.h"
#include "kvi_settings.h"
#include "kvi_stringconversion.h"
#include "kvi_options.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#ifdef COMPILE_USE_QT4
#include <qevent.h>
#include <q3header.h>
#include <q3vbox.h>
#else
#include <qheader.h>
#endif
#include "kvi_pointerhashtable.h"
#include <qimage.h>
#include <qstring.h>
#include <qcombobox.h>


#include <qstyle.h>
#include <qpainter.h>
#include "kvi_tal_hbox.h"
#include "kvi_tal_vbox.h"
#include <qinputdialog.h>

#include "wizard.h"
#include "dialog.h"


#ifdef COMPILE_INFO_TIPS
	#include <qtooltip.h>
#endif // COMPILE_INFO_TIPS

// kvi_app.cpp
extern KviRegisteredUsersDialog * g_pRegisteredUsersDialog;

KviRegisteredUserDataBase * g_pLocalRegisteredUserDataBase; // local copy!



KviReguserPropertiesDialog::KviReguserPropertiesDialog(QWidget * p,KviPointerHashTable<QString,QString> * dict)
: QDialog(p,"property_editor",true)
{
	m_pPropertyDict = dict;

	setCaption(__tr2qs("Property Editor"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LINUX)));

	QGridLayout * g = new QGridLayout(this,3,3,4,4);

#ifdef COMPILE_USE_QT4
	m_pTable = new Q3Table(this);
#else
	m_pTable = new QTable(this);
#endif
	g->addMultiCellWidget(m_pTable,0,1,0,1);

	m_pTable->setNumCols(2);
	m_pTable->setSelectionMode(Kvi_Tal_Table::NoSelection);

	m_pTable->horizontalHeader()->setLabel(0,__tr2qs("Property"));
	m_pTable->horizontalHeader()->setLabel(1,__tr2qs("Value"));

	m_pTable->setMinimumSize(250,250);
	//connect(m_pTable,SIGNAL(valueChanged(int,int)),this,SLOT(propertyValueChanged(int,int)));

	KviTalVBox * vb = new KviTalVBox(this);
	vb->setSpacing(4);
	g->addWidget(vb,0,2);

	m_pAddButton = new QPushButton(__tr2qs("&New"),vb);
	connect(m_pAddButton,SIGNAL(clicked()),this,SLOT(addClicked()));
	m_pAddButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));

	m_pDelButton = new QPushButton(__tr2qs("&Remove"),vb);
	connect(m_pDelButton,SIGNAL(clicked()),this,SLOT(delClicked()));
	m_pDelButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DELETEITEM)));

	KviTalHBox * b = new KviTalHBox(this);
	b->setSpacing(4);
	g->addMultiCellWidget(b,2,2,1,2);

	QPushButton * pb = new QPushButton(__tr2qs("&OK"),b);
	connect(pb,SIGNAL(clicked()),this,SLOT(okClicked()));
	pb->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));


	pb = new QPushButton(__tr2qs("Cancel"),b);
	connect(pb,SIGNAL(clicked()),this,SLOT(reject()));
	pb->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));

	g->setRowStretch(1,1);
	g->setColStretch(0,1);

	fillData();
}

KviReguserPropertiesDialog::~KviReguserPropertiesDialog()
{
}

void KviReguserPropertiesDialog::closeEvent(QCloseEvent *e)
{
	e->accept();
	//delete this;
}

void KviReguserPropertiesDialog::fillData()
{
	m_pTable->setNumRows(m_pPropertyDict->count());
	KviPointerHashTableIterator<QString,QString> it(*m_pPropertyDict);
	int row = 0;
	while(it.current())
	{
		m_pTable->setItem(row,0,new Kvi_Tal_TableItem(m_pTable,Kvi_Tal_TableItem::OnTyping,it.currentKey()));
		m_pTable->setItem(row,1,new Kvi_Tal_TableItem(m_pTable,Kvi_Tal_TableItem::OnTyping,*(it.current())));
		++row;
		++it;
	}
	if(m_pTable->numRows() == 0)m_pDelButton->setEnabled(false);
}

void KviReguserPropertiesDialog::okClicked()
{
	m_pPropertyDict->clear();

	int n = m_pTable->numRows();
	for(int i=0;i<n;i++)
	{
		QString szName = m_pTable->text(i,0);
		QString szValue = m_pTable->text(i,1);
		if((!szName.isEmpty()) && (!szValue.isEmpty()))
		{
			m_pPropertyDict->replace(szName,new QString(szValue));
		}
	}

	accept();
}


void KviReguserPropertiesDialog::addClicked()
{
	m_pTable->setNumRows(m_pTable->numRows() + 1);
	m_pTable->setItem(m_pTable->numRows() - 1,0,new Kvi_Tal_TableItem(m_pTable,Kvi_Tal_TableItem::OnTyping,""));
	m_pTable->setItem(m_pTable->numRows() - 1,1,new Kvi_Tal_TableItem(m_pTable,Kvi_Tal_TableItem::OnTyping,""));
	m_pDelButton->setEnabled(true);
}

void KviReguserPropertiesDialog::delClicked()
{
	int i = m_pTable->currentRow();

	if((i > -1) && (i < m_pTable->numRows()))
	{
		// remove row i
		m_pTable->clearCell(i,0);
		m_pTable->clearCell(i,1);

		for(;i < (m_pTable->numRows() - 1);i++)
		{
			m_pTable->swapRows(i,i+1);
		}
		m_pTable->setNumRows(m_pTable->numRows() - 1);
		if(m_pTable->numRows() == 0)m_pDelButton->setEnabled(false);
	}
}


KviReguserMaskDialog::KviReguserMaskDialog(QWidget * p,KviIrcMask * m)
: QDialog(p,"reguser_mask_editor",true)
{
	m_pMask = m;

	setCaption(__tr2qs("Mask Editor"));

	QGridLayout * g = new QGridLayout(this,3,2,4,4);

	QLabel * l = new QLabel(__tr2qs("Insert a mask for this user.<br>It can contain the wildcard characters '*' and '?'."),this);
	//l->setAlignment(Qt::AlignCenter);
	g->addMultiCellWidget(l,0,0,0,1);

	KviTalHBox * b = new KviTalHBox(this);
	g->addMultiCellWidget(b,1,1,0,1);

	m_pNickEdit = new QLineEdit(b);
	//m_pNickEdit->setMinimumWidth(120);
	m_pNickEdit->setAlignment(Qt::AlignRight);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNickEdit,__tr2qs("<center>This the <b>nickname</b> that will match this user, default value is the registered name.</center>"));
#endif

	l = new QLabel("<center><b>!</b></center>",b);
	l->setAlignment(Qt::AlignCenter);
	//l->setMinimumWidth(40);

	m_pUserEdit = new QLineEdit(b);
	//m_pUserEdit->setMinimumWidth(120);
	m_pUserEdit->setAlignment(Qt::AlignCenter);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pUserEdit,__tr2qs("<center>This the <b>username</b> that will match this user. <b>*</b> will match any username.</center>"));
#endif

	l = new QLabel("<center><b>@</b></center>",b);
	l->setAlignment(Qt::AlignCenter);
	//l->setMinimumWidth(40);

	m_pHostEdit = new QLineEdit(b);
	//m_pHostEdit->setMinimumWidth(120);
	m_pHostEdit->setAlignment(Qt::AlignLeft);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pHostEdit,__tr2qs("<center>This the <b>hostname</b> that will match this user. <b>*</b> will match any hostname.</center>"));
#endif

	// just a spacer
//	l = new QLabel("<nobr>&nbsp;<nobr>",this);
//	g->addMultiCellWidget(l,2,2,0,1);

//	QFrame * f = new QFrame(this);
//	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
//	g->addMultiCellWidget(f,3,3,0,1);

	b = new KviTalHBox(this);
	b->setSpacing(4);
	g->addWidget(b,2,1);

	QPushButton * pb = new QPushButton(__tr2qs("&OK"),b);
	connect(pb,SIGNAL(clicked()),this,SLOT(okClicked()));
	//pb->setMinimumWidth(120);

	pb = new QPushButton(__tr2qs("Cancel"),b);
	connect(pb,SIGNAL(clicked()),this,SLOT(reject()));
	//pb->setMinimumWidth(120);



	g->setColStretch(0,1);
	g->setRowStretch(0,1);

	m_pNickEdit->setText(m->nick());
	m_pUserEdit->setText(m->user());
	m_pHostEdit->setText(m->host());
}

KviReguserMaskDialog::~KviReguserMaskDialog()
{
}

void KviReguserMaskDialog::closeEvent(QCloseEvent *e)
{
	e->accept();
	//delete this;
}

void KviReguserMaskDialog::okClicked()
{
	KviStr szTmp = m_pNickEdit->text();
	if(szTmp.isEmpty())szTmp = "*";
	m_pMask->setNick(szTmp.ptr());

	szTmp = m_pUserEdit->text();
	if(szTmp.isEmpty())szTmp = "*";
	m_pMask->setUsername(szTmp.ptr());

	szTmp = m_pHostEdit->text();
	if(szTmp.isEmpty())szTmp = "*";
	m_pMask->setHost(szTmp.ptr());

	accept();
}




KviRegisteredUserEntryDialog::KviRegisteredUserEntryDialog(QWidget *p,KviRegisteredUser * r,bool bModal)
: KviTalTabDialog(p,"reguser_entry_editor",bModal)
{
	m_pUser = r;
	m_pCustomColor = new QColor();
	
	if(r)
	{
		QString col=r->getProperty("customColor");
		KviStringConversion::fromString(col,(*m_pCustomColor));
	}
	
	m_pPropertyDict = new KviPointerHashTable<QString,QString>(17,false);
	m_pPropertyDict->setAutoDelete(true);

	//setMinimumSize(400,450);

	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LINUX)));
	setCaption(__tr2qs("Registered User Entry"));

	QWidget * p1 = new QWidget(this);

	QGridLayout * g = new QGridLayout(p1,6,2,4,4);

	QLabel * l = new QLabel(__tr2qs("Name:"),p1);
	g->addWidget(l,0,0);

	m_pNameEdit = new QLineEdit(p1);
	g->addWidget(m_pNameEdit,0,1);

	l = new QLabel(__tr2qs("Comment:"),p1);
	g->addWidget(l,1,0);
	
	m_pCommentEdit = new QLineEdit(p1);
	g->addWidget(m_pCommentEdit,1,1);

	QFrame * f = new QFrame(p1);
	g->addMultiCellWidget(f,2,2,0,1);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	l = new QLabel(__tr2qs("Masks:"),p1);
	g->addMultiCellWidget(l,3,3,0,1);

	m_pMaskListBox = new KviTalListBox(p1);
	connect(m_pMaskListBox,SIGNAL(currentChanged(KviTalListBoxItem *)),this,SLOT(maskCurrentChanged(KviTalListBoxItem *)));
	m_pMaskListBox->setMinimumSize(300,200);

	g->addMultiCellWidget(m_pMaskListBox,4,4,0,1);

	KviTalHBox * b = new KviTalHBox(p1);
	g->addMultiCellWidget(b,5,5,0,1);
	b->setSpacing(4);

	m_pAddMaskButton = new QPushButton(__tr2qs("&Add..."),b);
	connect(m_pAddMaskButton,SIGNAL(clicked()),this,SLOT(addMaskClicked()));
	m_pAddMaskButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));

	m_pDelMaskButton = new QPushButton(__tr2qs("Re&move"),b);
	m_pDelMaskButton->setEnabled(false);
	connect(m_pDelMaskButton,SIGNAL(clicked()),this,SLOT(delMaskClicked()));
	m_pDelMaskButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DELETEITEM)));

	m_pEditMaskButton = new QPushButton(__tr2qs("&Edit"),b);
	m_pEditMaskButton->setEnabled(false);
	connect(m_pEditMaskButton,SIGNAL(clicked()),this,SLOT(editMaskClicked()));
	m_pEditMaskButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EDITITEM)));

	g->setRowStretch(4,1);
	g->setColStretch(1,1);

	addTab(p1,__tr2qs("Identity"));



	QWidget * p2 = new QWidget(this);

	g = new QGridLayout(p2,6,3,5,2);

	m_pNotifyCheck = new KviStyledCheckBox(__tr2qs("Notify when user is online"),p2);
	g->addMultiCellWidget(m_pNotifyCheck,0,0,0,2);

	l = new QLabel(__tr2qs("Notify nicknames:"),p2);
	l->setEnabled(m_pNotifyCheck->isChecked());
	g->addWidget(l,1,0);
	connect(m_pNotifyCheck,SIGNAL(toggled(bool)),l,SLOT(setEnabled(bool)));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNotifyCheck,__tr2qs("<center>You can enter a space separated list of nicknames.</center>"));
#endif


	m_pNotifyNick = new QLineEdit(p2);
	m_pNotifyNick->setEnabled(false);
	g->addMultiCellWidget(m_pNotifyNick,1,1,1,2);
	connect(m_pNotifyCheck,SIGNAL(toggled(bool)),m_pNotifyNick,SLOT(setEnabled(bool)));


	f = new QFrame(p2);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addMultiCellWidget(f,2,2,0,2);

	m_pAvatar = 0;
	if(r)
	{
		const char * av = r->getProperty("avatar");
		if(av)
		{
			m_pAvatar = new KviPixmap(av);
		}
	}
	if(!m_pAvatar)m_pAvatar = new KviPixmap();

	m_pAvatarSelector = new KviPixmapSelector(p2,__tr2qs("Avatar"),m_pAvatar,true);
	g->addMultiCellWidget(m_pAvatarSelector,3,3,0,2);

	f = new QFrame(p2);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addMultiCellWidget(f,4,4,0,2);

	m_pCustomColorCheck = new KviStyledCheckBox(__tr2qs("Use custom color in userlist"),p2);
	if(r)
		m_pCustomColorCheck->setChecked(r->getBoolProperty("useCustomColor"));
	g->addMultiCellWidget(m_pCustomColorCheck,5,5,0,1);

	m_pCustomColorSelector = new KviColorSelector(p2,QString::null,m_pCustomColor,1);
	g->addWidget(m_pCustomColorSelector,5,2);
	
	QPushButton * pb = new QPushButton(__tr2qs("All Properties..."),p2);
	connect(pb,SIGNAL(clicked()),this,SLOT(editAllPropertiesClicked()));
	g->addWidget(pb,6,2);

	g->setColStretch(1,1);
	g->setRowStretch(3,1);

	addTab(p2,__tr2qs("Properties"));

	// Ignore TAB
#ifdef COMPILE_USE_QT4
	Q3VBox * vb = new Q3VBox(this);
#else
	QVBox * vb = new QVBox(this);
#endif
	vb->setMargin(10);

	m_pIgnoreEnabled = new KviStyledCheckBox(__tr2qs("Enable ignore for this user"),vb);

	QGroupBox * gb = new QGroupBox(__tr2qs("Ignore features"),vb);
	connect(m_pIgnoreEnabled,SIGNAL(toggled(bool)),gb,SLOT(setEnabled(bool)));
	
	QVBoxLayout * layout = new QVBoxLayout(gb,20,3);
	
	m_pIgnoreQuery = new KviStyledCheckBox(__tr2qs("Ignore query-messages"),gb);
	layout->addWidget(m_pIgnoreQuery);

	m_pIgnoreChannel = new KviStyledCheckBox(__tr2qs("Ignore channel-messages"),gb);
	layout->addWidget(m_pIgnoreChannel);

	m_pIgnoreNotice = new KviStyledCheckBox(__tr2qs("Ignore notice-messages"),gb);
	layout->addWidget(m_pIgnoreNotice);

	m_pIgnoreCtcp = new KviStyledCheckBox(__tr2qs("Ignore ctcp-messages"),gb);
	layout->addWidget(m_pIgnoreCtcp);

	m_pIgnoreInvite = new KviStyledCheckBox(__tr2qs("Ignore invites"),gb);
	layout->addWidget(m_pIgnoreInvite);

	m_pIgnoreDcc = new KviStyledCheckBox(__tr2qs("Ignore DCCs"),gb);
	layout->addWidget(m_pIgnoreDcc);

	QWidget *w = new QWidget(vb);
	w->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);

	addTab(vb,__tr2qs("Ignore"));

	setCancelButton(__tr2qs("Cancel"));
	setOkButton(__tr2qs("&OK"));
	connect(this,SIGNAL(applyButtonPressed()),this,SLOT(okClicked()));
	connect(this,SIGNAL(cancelButtonPressed()),this,SLOT(reject()));

	if(r)
	{
		m_pNameEdit->setText(r->name());
		m_pCommentEdit->setText(r->getProperty("comment"));
		for(KviIrcMask * m = r->maskList()->first();m;m = r->maskList()->next())
		{
			QString mk = m->nick();
			mk += QChar('!');
			mk += m->user();
			mk += QChar('@');
			mk += m->host();
			m_pMaskListBox->insertItem(mk);
		}

		QString szNotifyNicks = r->getProperty("notify");
		if(!szNotifyNicks.isEmpty())
		{
			m_pNotifyCheck->setChecked(true);
			m_pNotifyNick->setText(szNotifyNicks);
			m_pNotifyNick->setEnabled(true);
		}
		
		if(r->propertyDict())
		{
			KviPointerHashTableIterator<QString,QString> it(*(r->propertyDict()));
			while(QString *s = it.current())
			{
				m_pPropertyDict->insert(it.currentKey(),new QString(*s));
				++it;
			}
		}

		m_pIgnoreEnabled->setChecked(r->ignoreEnagled());

		gb->setEnabled(r->ignoreEnagled());
		
		m_pIgnoreQuery->setChecked(r->ignoreFlags() & KviRegisteredUser::Query);
		m_pIgnoreChannel->setChecked(r->ignoreFlags() & KviRegisteredUser::Channel);
		m_pIgnoreNotice->setChecked(r->ignoreFlags() & KviRegisteredUser::Notice);
		m_pIgnoreCtcp->setChecked(r->ignoreFlags() & KviRegisteredUser::Ctcp);
		m_pIgnoreInvite->setChecked(r->ignoreFlags() & KviRegisteredUser::Invite);
		m_pIgnoreDcc->setChecked(r->ignoreFlags() & KviRegisteredUser::Dcc);
	} else {
		// default values
		if(!m_pIgnoreEnabled->isChecked())
		{
			gb->setEnabled(false);
		}
	}
}

void KviRegisteredUserEntryDialog::closeEvent(QCloseEvent *e)
{
	e->accept();
	okClicked();
	//delete this;
}

KviRegisteredUserEntryDialog::~KviRegisteredUserEntryDialog()
{
	delete m_pAvatar;
	delete m_pPropertyDict;
	delete m_pCustomColor;
}

void KviRegisteredUserEntryDialog::maskCurrentChanged(KviTalListBoxItem *it)
{
	m_pDelMaskButton->setEnabled(it);
	m_pEditMaskButton->setEnabled(it);
}

void KviRegisteredUserEntryDialog::okClicked()
{
	QString szGroup;
	if(m_pUser)
	{
		szGroup=m_pUser->group();
		g_pLocalRegisteredUserDataBase->removeUser(m_pUser->name());
	}

	QString name = m_pNameEdit->text();

	if(name.isEmpty())name = "user";

	KviRegisteredUser * u;

	QString szNameOk = name;

	int idx = 1;

	do {
		u = g_pLocalRegisteredUserDataBase->findUserByName(szNameOk);
		if(u)
		{
			KviQString::sprintf(szNameOk,"%Q%d",&name,idx);
			idx++;
		}
	} while(u);


	u = g_pLocalRegisteredUserDataBase->addUser(szNameOk);
	u->setGroup(szGroup);
	
	if(!u)
	{
		// ops... no way
		// FIXME: spit an error message ?
		debug("Ops.. something wrong with the regusers db");
		accept();
		return;
	}

	int cnt = m_pMaskListBox->count();
	idx = 0;
	while(cnt > 0)
	{
		QString mask = m_pMaskListBox->text(idx);
		KviIrcMask * mk = new KviIrcMask(mask);
		g_pLocalRegisteredUserDataBase->removeMask(*mk);
		g_pLocalRegisteredUserDataBase->addMask(u,mk);
		cnt--;
		idx++;
	}
	u->setProperty("comment",m_pCommentEdit->text());
	
	m_pAvatarSelector->commit();

	if(!m_pAvatar->isNull())
	{
		QString szPath = m_pAvatar->path();
		u->setProperty("avatar",szPath);
	}

	if(m_pNotifyCheck->isChecked())
	{
		QString szNicks = m_pNotifyNick->text();
	
		if(!szNicks.isEmpty())
		{
			u->setProperty("notify",szNicks);
		}
	}
	
	m_pPropertyDict->remove("notify");
	m_pPropertyDict->remove("avatar");

	KviPointerHashTableIterator<QString,QString> it(*m_pPropertyDict);
	while(QString *s = it.current())
	{
		u->setProperty(it.currentKey(),*s);
		++it;
	}

	u->setProperty("useCustomColor",m_pCustomColorCheck->isChecked());
	
	QString col;
	KviStringConversion::toString(m_pCustomColorSelector->getColor(),col);
	u->setProperty("customColor",col);

	
	int iIgnoreFlags=0;
	u->setIgnoreEnabled(m_pIgnoreEnabled->isChecked());
	if(m_pIgnoreQuery->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Query;
	if(m_pIgnoreChannel->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Channel;
	if(m_pIgnoreNotice->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Notice;
	if(m_pIgnoreCtcp->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Ctcp;
	if(m_pIgnoreInvite->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Invite;
	if(m_pIgnoreDcc->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Dcc;

	u->setIgnoreFlags(iIgnoreFlags);
	accept();
	g_pApp->optionResetUpdate(KviOption_resetUpdateGui);
}


void KviRegisteredUserEntryDialog::addMaskClicked()
{
	KviIrcMask mk;
	KviReguserMaskDialog * dlg = new KviReguserMaskDialog(this,&mk);
	if(dlg->exec() == QDialog::Accepted)
	{
		QString m = mk.nick();
		m += QChar('!');
		m += mk.user();
		m += QChar('@');
		m += mk.host();
		m_pMaskListBox->insertItem(m);
	}
	delete dlg;
}

void KviRegisteredUserEntryDialog::delMaskClicked()
{
	int idx = m_pMaskListBox->currentItem();
	if(idx == -1)return;
	m_pMaskListBox->removeItem(idx);
}

void KviRegisteredUserEntryDialog::editMaskClicked()
{
	int idx = m_pMaskListBox->currentItem();
	if(idx == -1)return;
	KviStr szM = m_pMaskListBox->text(idx);
	if(szM.isEmpty())return;

	KviIrcMask mk(szM.ptr());
	KviReguserMaskDialog * dlg = new KviReguserMaskDialog(this,&mk);
	if(dlg->exec() == QDialog::Accepted)
	{
		QString m = mk.nick();
		m += QChar('!');
		m += mk.user();
		m += QChar('@');
		m += mk.host();
		m_pMaskListBox->changeItem(m,idx);
	}
	delete dlg;
}

void KviRegisteredUserEntryDialog::editAllPropertiesClicked()
{
	m_pAvatarSelector->commit();

	if(m_pAvatar->isNull())
	{
		m_pPropertyDict->remove("avatar");
	} else {
		KviStr szPath = m_pAvatar->path();
		if(szPath.isEmpty())m_pPropertyDict->remove("avatar");
		else m_pPropertyDict->replace("avatar",new QString(szPath));
	}

	if(m_pNotifyCheck->isChecked())
	{
		QString szNicks = m_pNotifyNick->text();
	
		if(!szNicks.isEmpty())
		{
			m_pPropertyDict->replace("notify",new QString(szNicks));
		} else {
			m_pPropertyDict->remove("notify");
		}
	} else {
		m_pPropertyDict->remove("notify");
	}


	KviReguserPropertiesDialog * dlg = new KviReguserPropertiesDialog(this,m_pPropertyDict);
	if(dlg->exec() != QDialog::Accepted)
	{
		delete dlg;
		return;
	}
	delete dlg;

	QString * notify = m_pPropertyDict->find("notify");
	bool bGotIt = false;
	if(notify)
	{
		if(!notify->isEmpty())
		{
			bGotIt = true;
			m_pNotifyNick->setText(*notify);
		}
	}
	m_pNotifyCheck->setChecked(bGotIt);
	m_pNotifyNick->setEnabled(bGotIt);
	if(!bGotIt)m_pNotifyNick->setText("");

	QString * avatar = m_pPropertyDict->find("avatar");
	bGotIt = false;
	if(avatar)
	{
		if(!avatar->isEmpty())
			m_pAvatarSelector->setImagePath(*avatar);
	}

}




#include "edituser.moc"
