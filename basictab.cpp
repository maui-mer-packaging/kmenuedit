/*
 *   Copyright (C) 2000 Matthias Elter <elter@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */


#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfileinfo.h>
#include <qgroupbox.h>
#include <qhbox.h>

#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kdialog.h>
#include <kkeybutton.h>
#include <klineedit.h>
#include <kicondialog.h>
#include <kdesktopfile.h>
#include <kurlrequester.h>
#include <kfiledialog.h>
#include <kcombobox.h>
#include "khotkeys.h"

#include "menuinfo.h"

#include "basictab.h"
#include "basictab.moc"

BasicTab::BasicTab( QWidget *parent, const char *name )
  : QWidget(parent, name)
{
    _menuFolderInfo = 0;
    _menuEntryInfo = 0;

    QGridLayout *layout = new QGridLayout(this, 6, 2,
                                          KDialog::marginHint(),
                                          KDialog::spacingHint());

    // general group
    QGroupBox *general_group = new QGroupBox(this);
    QGridLayout *grid = new QGridLayout(general_group, 4, 2,
                                        KDialog::marginHint(),
                                        KDialog::spacingHint());

    general_group->setAcceptDrops(false);
	
    // setup line inputs
    _nameEdit = new KLineEdit(general_group);
	_nameEdit->setAcceptDrops(false);
    _commentEdit = new KLineEdit(general_group);
	_commentEdit->setAcceptDrops(false);
    _execEdit = new KURLRequester(general_group);
	_execEdit->lineEdit()->setAcceptDrops(false);
    _launchCB = new QCheckBox(i18n("Enable &launch feedback"), general_group);

    // setup labels
    _nameLabel = new QLabel(_nameEdit, i18n("&Name:"), general_group);
    _commentLabel = new QLabel(_commentEdit, i18n("&Comment:"), general_group);
    _execLabel = new QLabel(_execEdit, i18n("Co&mmand:"), general_group);
    grid->addWidget(_nameLabel, 0, 0);
    grid->addWidget(_commentLabel, 1, 0);
    grid->addWidget(_execLabel, 2, 0);

    // connect line inputs
    connect(_nameEdit, SIGNAL(textChanged(const QString&)),
            SLOT(slotChanged()));
    connect(_commentEdit, SIGNAL(textChanged(const QString&)),
            SLOT(slotChanged()));
    connect(_execEdit, SIGNAL(textChanged(const QString&)),
            SLOT(slotChanged()));
    connect(_launchCB, SIGNAL(clicked()), SLOT(launchcb_clicked()));

    // add line inputs to the grid
    grid->addMultiCellWidget(_nameEdit, 0, 0, 1, 1);
    grid->addMultiCellWidget(_commentEdit, 1, 1, 1, 1);
    grid->addMultiCellWidget(_execEdit, 2, 2, 1, 1);
    grid->addMultiCellWidget(_launchCB, 3, 3, 0, 1);

    // setup icon button
    _iconButton = new KIconButton(general_group);
    _iconButton->setFixedSize(52,52);
    connect(_iconButton, SIGNAL(clicked()), SLOT(slotChanged()));
    grid->addMultiCellWidget(_iconButton, 0, 1, 2, 2);

    // add the general group to the main layout
    layout->addMultiCellWidget(general_group, 0, 0, 0, 1);

    // path group
    _path_group = new QGroupBox(this);
    QVBoxLayout *vbox = new QVBoxLayout(_path_group, KDialog::marginHint(),
                                        KDialog::spacingHint());

    QHBox *hbox = new QHBox(_path_group);
    hbox->setSpacing(KDialog::spacingHint());

    _pathLabel = new QLabel(i18n("&Work path:"), hbox);

    _pathEdit = new KURLRequester(hbox);
    _pathEdit->setMode(KFile::Directory | KFile::LocalOnly);
	_pathEdit->lineEdit()->setAcceptDrops(false);

    _pathLabel->setBuddy(_pathEdit);

    connect(_pathEdit, SIGNAL(textChanged(const QString&)),
            SLOT(slotChanged()));
    vbox->addWidget(hbox);
    layout->addMultiCellWidget(_path_group, 1, 1, 0, 1);

    // terminal group
    _term_group = new QGroupBox(this);
    vbox = new QVBoxLayout(_term_group, KDialog::marginHint(),
			   KDialog::spacingHint());

    _terminalCB = new QCheckBox(i18n("Run in term&inal"), _term_group);
    connect(_terminalCB, SIGNAL(clicked()), SLOT(termcb_clicked()));
    vbox->addWidget(_terminalCB);

    hbox = new QHBox(_term_group);
    hbox->setSpacing(KDialog::spacingHint());
    _termOptLabel = new QLabel(i18n("Terminal &options:"), hbox);
    _termOptEdit = new KLineEdit(hbox);
	_termOptEdit->setAcceptDrops(false);
    _termOptLabel->setBuddy(_termOptEdit);

    connect(_termOptEdit, SIGNAL(textChanged(const QString&)),
            SLOT(slotChanged()));
    vbox->addWidget(hbox);
    layout->addMultiCellWidget(_term_group, 2, 2, 0, 1);

    _termOptEdit->setEnabled(false);

    // uid group
    _uid_group = new QGroupBox(this);
    vbox = new QVBoxLayout(_uid_group, KDialog::marginHint(),
                           KDialog::spacingHint());

    _uidCB = new QCheckBox(i18n("Run as a &different user"), _uid_group);
    connect(_uidCB, SIGNAL(clicked()), SLOT(uidcb_clicked()));
    vbox->addWidget(_uidCB);

    hbox = new QHBox(_uid_group);
    hbox->setSpacing(KDialog::spacingHint());
    _uidLabel = new QLabel(i18n("&Username:"), hbox);
    _uidEdit = new KLineEdit(hbox);
	_uidEdit->setAcceptDrops(false);
    _uidLabel->setBuddy(_uidEdit);

    connect(_uidEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotChanged()));
    vbox->addWidget(hbox);
    layout->addMultiCellWidget(_uid_group, 3, 3, 0, 1);

    _uidEdit->setEnabled(false);

    layout->setRowStretch(0, 2);

    // key binding group
    general_group_keybind = new QGroupBox(this);
    layout->addMultiCellWidget( general_group_keybind, 4, 4, 0, 1 );
    // dummy widget in order to make it look a bit better
    layout->addWidget( new QWidget(this), 5, 0 );
    layout->setRowStretch( 5, 4 );
    QGridLayout *grid_keybind = new QGridLayout(general_group_keybind, 3, 1,
                                                KDialog::marginHint(),
                                                KDialog::spacingHint());

    //_keyEdit = new KLineEdit(general_group_keybind);
    //_keyEdit->setReadOnly( true );
    //_keyEdit->setText( "" );
    //QPushButton* _keyButton = new QPushButton( i18n( "Change" ),
    //                                           general_group_keybind );
    //connect( _keyButton, SIGNAL( clicked()), this, SLOT( keyButtonPressed()));
    _keyEdit = new KKeyButton(general_group_keybind);
    grid_keybind->addWidget(new QLabel(_keyEdit, i18n("Current shortcut &key:"), general_group_keybind), 0, 0);
    connect( _keyEdit, SIGNAL(capturedShortcut(const KShortcut&)),
             this, SLOT(slotCapturedShortcut(const KShortcut&)));
    grid_keybind->addWidget(_keyEdit, 0, 1);
    //grid_keybind->addWidget(_keyButton, 0, 2 );

    if (!KHotKeys::present())
       general_group_keybind->hide();

    //disable all group at the begining.
    //because there is not file selected.
    _nameEdit->setEnabled(false);
    _commentEdit->setEnabled(false);
    _execEdit->setEnabled(false);
    _launchCB->setEnabled(false);
    _nameLabel->setEnabled(false);
    _commentLabel->setEnabled(false);
    _execLabel->setEnabled(false);
    _path_group->setEnabled(false);
    _term_group->setEnabled(false);
    _uid_group->setEnabled(false);
    // key binding part
    general_group_keybind->setEnabled( false );
}

void BasicTab::enableWidgets(bool isDF, bool isDeleted)
{
    // set only basic attributes if it is not a .desktop file
    _nameEdit->setEnabled(!isDeleted);
    _commentEdit->setEnabled(!isDeleted);
    _iconButton->setEnabled(!isDeleted);
    _execEdit->setEnabled(isDF && !isDeleted);
    _launchCB->setEnabled(isDF && !isDeleted);
    _nameLabel->setEnabled(!isDeleted);
    _commentLabel->setEnabled(!isDeleted);
    _execLabel->setEnabled(isDF && !isDeleted);

    _path_group->setEnabled(isDF && !isDeleted);
    _term_group->setEnabled(isDF && !isDeleted);
    _uid_group->setEnabled(isDF && !isDeleted);
    general_group_keybind->setEnabled( isDF && !isDeleted );

    _termOptEdit->setEnabled(isDF && !isDeleted && _terminalCB->isChecked());
    _termOptLabel->setEnabled(isDF && !isDeleted && _terminalCB->isChecked());

    _uidEdit->setEnabled(isDF && !isDeleted && _uidCB->isChecked());
    _uidLabel->setEnabled(isDF && !isDeleted && _uidCB->isChecked());
}

void BasicTab::setFolderInfo(MenuFolderInfo *folderInfo)
{
    blockSignals(true);
    _menuFolderInfo = folderInfo;
    _menuEntryInfo = 0;

    _nameEdit->setText(folderInfo->caption);
    _commentEdit->setText(folderInfo->comment);
    _iconButton->setIcon(folderInfo->icon);

    // clean all disabled fields and return 
    _execEdit->lineEdit()->setText("");
    _pathEdit->lineEdit()->setText("");
    _termOptEdit->setText("");
    _uidEdit->setText("");
    _launchCB->setChecked(false);
    _terminalCB->setChecked(false);
    _uidCB->setChecked(false);
    _keyEdit->setShortcut(0);

    enableWidgets(false, folderInfo->hidden);
    blockSignals(false);
}

void BasicTab::setEntryInfo(MenuEntryInfo *entryInfo)
{
    blockSignals(true);
    _menuFolderInfo = 0;
    _menuEntryInfo = entryInfo;

    KDesktopFile *df = entryInfo->desktopFile();

    _nameEdit->setText(df->readName());
    _commentEdit->setText(df->readComment());
    _iconButton->setIcon(df->readIcon());

    // key binding part
    if( KHotKeys::present())
    {
        _keyEdit->setShortcut( entryInfo->shortcut() );
    }

    _execEdit->lineEdit()->setText(df->readPathEntry("Exec"));
    _pathEdit->lineEdit()->setText(df->readPath());
    _termOptEdit->setText(df->readEntry("TerminalOptions"));
    _uidEdit->setText(df->readEntry("X-KDE-Username"));

    _launchCB->setChecked(df->readBoolEntry("X-KDE-StartupNotify", true));
      
    if(df->readNumEntry("Terminal", 0) == 1)
        _terminalCB->setChecked(true);
    else
        _terminalCB->setChecked(false);

    _uidCB->setChecked(df->readBoolEntry("X-KDE-SubstituteUID", false));

    enableWidgets(true, entryInfo->hidden);
    blockSignals(false);
}

void BasicTab::apply()
{
    if (_menuEntryInfo)
    {
        _menuEntryInfo->setDirty();
        _menuEntryInfo->setCaption(_nameEdit->text());
        _menuEntryInfo->setIcon(_iconButton->icon());
        
        KDesktopFile *df = _menuEntryInfo->desktopFile();
        df->writeEntry("Comment", _commentEdit->text());
        df->writePathEntry("Exec", _execEdit->lineEdit()->text());
        df->writePathEntry("Path", _pathEdit->lineEdit()->text());

        if (_terminalCB->isChecked())
            df->writeEntry("Terminal", 1);
        else
            df->writeEntry("Terminal", 0);

        df->writeEntry("TerminalOptions", _termOptEdit->text());
        df->writeEntry("X-KDE-SubstituteUID", _uidCB->isChecked());
        df->writeEntry("X-KDE-Username", _uidEdit->text());
        df->writeEntry("X-KDE-StartupNotify", _launchCB->isChecked());
    }
    else
    {
        _menuFolderInfo->setCaption(_nameEdit->text());
        _menuFolderInfo->setComment(_commentEdit->text());
        _menuFolderInfo->setIcon(_iconButton->icon());
    }
}

void BasicTab::slotChanged()
{
    if (signalsBlocked())
       return;
    apply();
    if (_menuEntryInfo)
       emit changed( _menuEntryInfo );
    else
       emit changed( _menuFolderInfo );
}

void BasicTab::launchcb_clicked()
{
    slotChanged();
}

void BasicTab::termcb_clicked()
{
    _termOptEdit->setEnabled(_terminalCB->isChecked());
    _termOptLabel->setEnabled(_terminalCB->isChecked());
    slotChanged();
}

void BasicTab::uidcb_clicked()
{
    _uidEdit->setEnabled(_uidCB->isChecked());
    _uidLabel->setEnabled(_uidCB->isChecked());
    slotChanged();
}

void BasicTab::slotCapturedShortcut(const KShortcut& cut)
{
    if (signalsBlocked())
       return;

    _keyEdit->setShortcut(cut);

    if ( KHotKeys::present() )
    {
       _menuEntryInfo->setShortcut( _keyEdit->shortcut() );
    }
    if (_menuEntryInfo)
       emit changed( _menuEntryInfo );
}

