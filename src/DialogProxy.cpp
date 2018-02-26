/**********************************************************************
zyGrib: meteorological GRIB file viewer
Copyright (C) 2008-2012 - Jacques Zaninetti - http://www.zygrib.org

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include <QMessageBox>
#include <cmath>
#include <cassert>
#include <QNetworkProxy>

#include "DialogProxy.h"
#include "Util.h"


//-------------------------------------------------------------------------------
DialogProxy::DialogProxy (QWidget *parent) : DialogBoxBase (parent)
{
    QLabel *label;
    QFrame *ftmp;
    frameGui = createFrameGui(this);
    
    layout = new QGridLayout(this);
    int lig=0;
    //-------------------------
    lig ++;
    QFont font;
    font.setBold(true);
    label = new QLabel(tr("Type of internet connection"), this);
    label->setFont(font);
    layout->addWidget( label,    lig,0, 1,-1, Qt::AlignCenter);
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    layout->addWidget( frameGui,  lig,0,   1, 2);
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    btOK     = new QPushButton(tr("Ok"), this);
    btCancel = new QPushButton(tr("Cancel"), this);
    layout->addWidget( btOK,    lig,0);
    layout->addWidget( btCancel, lig,1);
    
    //===============================================================
    lineProxyHostname->setText (Util::decode(Util::getSetting("httpProxyHostname", "").toString()));
    lineProxyPort->setText (Util::getSetting("httpProxyPort", "").toString());
    lineProxyUserName->setText (Util::decode(Util::getSetting("httpProxyUsername", "").toString()));
    lineProxyUserPassword->setText (Util::decode(Util::getSetting("httpProxyUserPassword", "").toString()));
	
    bool usep = Util::getSetting ("httpUseProxy", false).toBool(); 
    btUseProxy->setChecked (usep);
    btDontUseProxy->setChecked (!usep);
    slotUseProxyChanged ();

    lineZygribForumUserName->setText (Util::decode(Util::getSetting("zyGribForumUserName", "").toString()));
    lineZygribForumUserPassword->setText (Util::decode(Util::getSetting("zyGribForumUserPassword", "").toString()));
	
    usep = Util::getSetting ("strictHttpDownload", false).toBool(); 
    btStrictHttpDownload->setChecked (usep);
    btStandardDownload->setChecked (!usep);
    slotTrueHttpDownloadChanged ();

	//===============================================================
    connect(btUseProxy,     SIGNAL(clicked()), this, SLOT(slotUseProxyChanged()));
    connect(btDontUseProxy, SIGNAL(clicked()), this, SLOT(slotUseProxyChanged()));
    connect(btStandardDownload, SIGNAL(clicked()), this, SLOT(slotTrueHttpDownloadChanged()));
    connect(btStrictHttpDownload, SIGNAL(clicked()), this, SLOT(slotTrueHttpDownloadChanged()));
    connect(btCancel, SIGNAL(clicked()), this, SLOT(slotBtCancel()));
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
}

//-------------------------------------------------------------------------------
void DialogProxy::slotUseProxyChanged ()
{
    bool usep = btUseProxy->isChecked();
    lineProxyHostname->setEnabled (usep);
    lineProxyPort->setEnabled (usep);
    lineProxyUserName->setEnabled (usep);
    lineProxyUserPassword->setEnabled (usep);
    cbProxyType->setEnabled (usep);
	for (int i=0; i < listProxyLabels.size(); i++) {
		listProxyLabels[i]->setEnabled (usep);
	}
}
//-------------------------------------------------------------------------------
void DialogProxy::slotTrueHttpDownloadChanged ()
{
    bool usep = btStrictHttpDownload->isChecked();
    lineZygribForumUserName->setEnabled (usep);
    lineZygribForumUserPassword->setEnabled (usep);
	for (int i=0; i < listDownloadLabels.size(); i++) {
		listDownloadLabels[i]->setEnabled (usep);
	}
}

//-------------------------------------------------------------------------------
void DialogProxy::slotBtOK()
{
    Util::setSetting("httpUseProxy", btUseProxy->isChecked());
    Util::setSetting("httpProxyHostname", Util::encode(lineProxyHostname->text()));
    Util::setSetting("httpProxyPort", lineProxyPort->text());
    Util::setSetting("httpProxyUsername", Util::encode(lineProxyUserName->text()));
    Util::setSetting("httpProxyUserPassword", Util::encode(lineProxyUserPassword->text()));
    Util::setSetting("httpProxyType", 
					 cbProxyType->itemData (cbProxyType->currentIndex() ).toInt() );
	Util::setApplicationProxy ();

    Util::setSetting("strictHttpDownload", btStrictHttpDownload->isChecked());
    Util::setSetting("zyGribForumUserName", Util::encode(lineZygribForumUserName->text()));
    Util::setSetting("zyGribForumUserPassword", Util::encode(lineZygribForumUserPassword->text()));
	
    accept();
}
//-------------------------------------------------------------------------------
void DialogProxy::slotBtCancel()
{
    reject();
}

//=============================================================================
// GUI
//=============================================================================
QFrame *DialogProxy::createFrameGui(QWidget *parent)
{
    QFrame * frm = new QFrame(parent);
    QFrame * ftmp;
    QLabel * label;
    QGridLayout  *lay = new QGridLayout(frm);
	lay->setContentsMargins (0,0,0,0);
    int lig=0;
    //-------------------------------------------
	// Proxy or not proxy
    //-------------------------------------------
    QButtonGroup *grp = new QButtonGroup(frm);
    btDontUseProxy    = new QRadioButton(tr("Direct connection to internet"), frm);
    grp->addButton(btDontUseProxy);
    btUseProxy     = new QRadioButton(tr("Connection with a proxy"), frm);
    grp->addButton(btUseProxy);
    QHBoxLayout  *ltmp = new QHBoxLayout ();
	ltmp->addWidget (btDontUseProxy);
	ltmp->addWidget (btUseProxy);
    lig ++;
    lay->addLayout (ltmp, lig,0,   1, 2);	
    //-------------------------
    lig ++;
    label = new QLabel (tr("Proxy type: "), frm);
	listProxyLabels.push_back (label);
    lay->addWidget (label,    lig,0, Qt::AlignRight);
	cbProxyType = new QComboBox (frm);
	cbProxyType->addItem (tr("System default proxy"), (int) QNetworkProxy::DefaultProxy);
	cbProxyType->addItem (tr("HTTP proxy"), (int) QNetworkProxy::HttpProxy);
	cbProxyType->addItem (tr("SOCKS5 proxy"), (int) QNetworkProxy::Socks5Proxy);
    lay->addWidget (cbProxyType, lig,1, Qt::AlignLeft);

    int pval = Util::getSetting ("httpProxyType", QNetworkProxy::HttpProxy).toInt();
    int ind = cbProxyType->findData (pval);
    cbProxyType->setCurrentIndex (ind);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Proxy server: "), frm);
	listProxyLabels.push_back (label);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lineProxyHostname = new QLineEdit(frm);
    lineProxyHostname->setFixedWidth(320);
    lay->addWidget( lineProxyHostname, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Port number :"), frm);
	listProxyLabels.push_back (label);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lineProxyPort = new QLineEdit(frm);
    lineProxyPort->setFixedWidth(60);
    
    lineProxyPort->setValidator(new QIntValidator(0,65536, this));
    lay->addWidget( lineProxyPort, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("User * :"), frm);
	listProxyLabels.push_back (label);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lineProxyUserName = new QLineEdit(frm);
    lineProxyUserName->setFixedWidth(320);
    lay->addWidget( lineProxyUserName, lig,1, Qt::AlignLeft);
    lig ++;
    label = new QLabel(tr("Password * :"), frm);
	listProxyLabels.push_back (label);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lineProxyUserPassword = new QLineEdit(frm);
    lineProxyUserPassword->setFixedWidth(320);
    lineProxyUserPassword->setEchoMode(QLineEdit::Password);
    lay->addWidget( lineProxyUserPassword, lig,1, Qt::AlignLeft);
    lig ++;
    label = new QLabel(tr("(* if needed)"), frm);
	listProxyLabels.push_back (label);
    lay->addWidget( label,    lig,0, 1,2, Qt::AlignLeft);
	
    //-------------------------------------------
	// Strict HTTP Connection or standard
    //-------------------------------------------
    lig ++;
    ftmp = new QFrame(frm); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    QButtonGroup *grp2 = new QButtonGroup (frm);
	btStandardDownload = new QRadioButton (tr("Standard download"));
    grp2->addButton (btStandardDownload);
	btStrictHttpDownload = new QRadioButton (tr("Strict HTTP download"));
    grp2->addButton (btStrictHttpDownload);
    ltmp = new QHBoxLayout ();
	ltmp->addWidget (btStandardDownload);
	ltmp->addWidget (btStrictHttpDownload);
    lig ++;
    lay->addLayout (ltmp, lig,0,   1, 2);	
    //-------------------------
    lig ++;
    label = new QLabel(tr("Forum login :"), frm);
	listDownloadLabels.push_back (label);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lineZygribForumUserName = new QLineEdit(frm);
    lineZygribForumUserName->setFixedWidth(320);
    lay->addWidget( lineZygribForumUserName, lig,1, Qt::AlignLeft);
    lig ++;
    label = new QLabel(tr("Forum password :"), frm);
	listDownloadLabels.push_back (label);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lineZygribForumUserPassword = new QLineEdit(frm);
    lineZygribForumUserPassword->setFixedWidth(320);
    lineZygribForumUserPassword->setEchoMode(QLineEdit::Password);
    lay->addWidget( lineZygribForumUserPassword, lig,1, Qt::AlignLeft);
    lig ++;
    label = new QLabel(tr("You must have a valid account on www.zygrib.org forum."), frm);
	listDownloadLabels.push_back (label);
    lay->addWidget( label,    lig,0, 1,2, Qt::AlignLeft);
	
    return frm;
}











