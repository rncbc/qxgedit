// qxgeditEdit.cpp
//
/****************************************************************************
   Copyright (C) 2005-2009, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "qxgeditAbout.h"
#include "qxgeditEdit.h"

#include "qxgeditOptions.h"
#include "qxgeditXGMasterMap.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QToolButton>
#include <QLineEdit>

#include <QRegExpValidator>

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QUrl>


//-------------------------------------------------------------------------
// qxgeditEdit - Custom edit-box widget.
//

// Constructor.
qxgeditEdit::qxgeditEdit ( QWidget *pParent )
	: XGParamWidget<QWidget> (pParent), m_pParam(NULL)
{
	m_pComboBox     = new QComboBox();
	m_pOpenButton   = new QToolButton();
	m_pSaveButton   = new QToolButton();
	m_pRemoveButton = new QToolButton();

	m_pComboBox->setEditable(true);
	m_pComboBox->lineEdit()->setValidator(
		new QRegExpValidator(QRegExp("[0-9A-Za-z]+"), this));
	m_pComboBox->setInsertPolicy(QComboBox::NoInsert);

	m_pOpenButton->setIcon(QIcon(":/icons/formOpen.png"));
	m_pSaveButton->setIcon(QIcon(":/icons/formSave.png"));
	m_pRemoveButton->setIcon(QIcon(":/icons/formRemove.png"));

	m_pOpenButton->setToolTip(tr("Open Preset"));
	m_pSaveButton->setToolTip(tr("Save Preset"));
	m_pRemoveButton->setToolTip("Remove Preset");

	QHBoxLayout *pHBoxLayout = new QHBoxLayout();
	pHBoxLayout->setMargin(2);
	pHBoxLayout->setSpacing(4);
	pHBoxLayout->addWidget(m_pOpenButton);
	pHBoxLayout->addWidget(m_pComboBox);
	pHBoxLayout->addWidget(m_pSaveButton);
	pHBoxLayout->addWidget(m_pRemoveButton);
//	pHBoxLayout->addSpacing(20);
	QWidget::setLayout(pHBoxLayout);

	m_iUpdatePreset = 0;
	m_iDirtyPreset  = 0;

	// UI signal/slot connections...
	QObject::connect(m_pComboBox,
		SIGNAL(editTextChanged(const QString&)),
		SLOT(changePreset(const QString&)));
	QObject::connect(m_pComboBox,
		SIGNAL(activated(const QString &)),
		SLOT(loadPreset(const QString&)));
	QObject::connect(m_pOpenButton,
		SIGNAL(clicked()),
		SLOT(openPreset()));
	QObject::connect(m_pSaveButton,
		SIGNAL(clicked()),
		SLOT(savePreset()));
	QObject::connect(m_pRemoveButton,
		SIGNAL(clicked()),
		SLOT(removePreset()));
}


// Destructor.
qxgeditEdit::~qxgeditEdit (void)
{
}


// Nominal value accessors.
void qxgeditEdit::set_value ( unsigned short /*iValue*/, Observer */*pSender*/ )
{
	if (m_pParam) {
		m_pComboBox->setEditText(QString(
			QByteArray((const char *) m_pParam->data(), m_pParam->size()))
			.simplified());
	}

	refreshPreset();
	stabilizePreset();
}

unsigned short qxgeditEdit::value (void) const
{
	return 0;
}


// Specialty parameter accessors.
void qxgeditEdit::set_param ( XGParam *pParam, Observer */*pSender*/ )
{
	m_pParam = static_cast<XGDataParam *> (pParam);

	if (m_pParam) {
		m_pComboBox->lineEdit()->setMaxLength(m_pParam->size());
		QWidget::setToolTip(tr("Preset (%1)").arg(m_pParam->text()));
		set_value(0, observer());
	}
}

XGParam *qxgeditEdit::param (void) const
{
	return static_cast<XGParam *> (m_pParam);
}


// Preset management slots...
void qxgeditEdit::changePreset ( const QString& sPreset )
{
	if (m_pParam) {
		m_pParam->set_data(
			(unsigned char *) sPreset.toAscii().data(),
			sPreset.length(),
			observer());
	}

	if (m_iUpdatePreset > 0)
		return;

	if (!sPreset.isEmpty() && m_pComboBox->findText(sPreset) >= 0)
		m_iDirtyPreset++;

	stabilizePreset();
}


void qxgeditEdit::loadPreset ( const QString& sPreset )
{
	if (m_iUpdatePreset > 0 || sPreset.isEmpty())
		return;

	// We'll need this, sure.
	qxgeditOptions *pOptions = qxgeditOptions::getInstance();
	if (pOptions == NULL)
		return;

	m_iUpdatePreset++;

	QSettings& settings = pOptions->settings();
	settings.beginGroup("/Uservoice/Presets/");
	emit loadPresetFile(settings.value(sPreset).toString());
	settings.endGroup();

	m_iUpdatePreset--;

	refreshPreset();
	stabilizePreset();
}


void qxgeditEdit::openPreset (void)
{
	if (m_iUpdatePreset > 0)
		return;

	// We'll need this, sure.
	qxgeditOptions *pOptions = qxgeditOptions::getInstance();
	if (pOptions == NULL)
		return;

	// We'll assume that there's an external file...
	QString sFilename;

	// Prompt if file does not currently exist...
	const QString  sExt("syx");
	const QString& sTitle  = tr("Open Preset") + " - " QXGEDIT_TITLE;
	const QString& sFilter = tr("Preset files (*.%1)").arg(sExt);
#if QT_VERSION < 0x040400
	// Ask for the filename to save...
	sFilename = QFileDialog::getSaveFileName(parentWidget(),
		sTitle, pOptions->sPresetDir, sFilter);
#else
	// Construct save-file dialog...
	QFileDialog fileDialog(parentWidget(),
		sTitle, pOptions->sPresetDir, sFilter);
	// Set proper open-file modes...
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::ExistingFile);
	fileDialog.setDefaultSuffix(sExt);
	// Stuff sidebar...
	QList<QUrl> urls(fileDialog.sidebarUrls());
	urls.append(QUrl::fromLocalFile(pOptions->sSessionDir));
	urls.append(QUrl::fromLocalFile(pOptions->sPresetDir));
	fileDialog.setSidebarUrls(urls);
	// Show dialog...
	if (fileDialog.exec())
		sFilename = fileDialog.selectedFiles().first();
#endif
	// Have we a filename to load a preset from?
	if (!sFilename.isEmpty()) {
		QFileInfo fi(sFilename);
		if (fi.exists()) {
			// Get it loaded alright...
			m_iUpdatePreset++;
			emit loadPresetFile(sFilename);
			m_pComboBox->setEditText(fi.baseName());
			m_iUpdatePreset--;
			pOptions->sPresetDir = fi.absolutePath();
		}
	}

	refreshPreset();
	stabilizePreset();
}


void qxgeditEdit::savePreset (void)
{
	const QString& sPreset = m_pComboBox->currentText();
	if (sPreset.isEmpty())
		return;

	// We'll need this, sure.
	qxgeditOptions *pOptions = qxgeditOptions::getInstance();
	if (pOptions == NULL)
		return;

	// The current state preset is about to be saved...
	// this is where we'll make it...
	QSettings& settings = pOptions->settings();
	settings.beginGroup("/Uservoice/Presets/");
	// Sure, we'll have something complex enough
	// to make it save into an external file...
	const QString sExt("syx");
	QFileInfo fi(QDir(pOptions->sPresetDir), sPreset + '.' + sExt);
	QString sFilename = fi.absoluteFilePath();
	// Prompt if file does not currently exist...
	if (!fi.exists()) {
		const QString& sTitle  = tr("Save Preset") + " - " QXGEDIT_TITLE;
		const QString& sFilter = tr("Preset files (*.%1)").arg(sExt);
	#if QT_VERSION < 0x040400
		// Ask for the filename to save...
		sFilename = QFileDialog::getSaveFileName(parentWidget(),
			sTitle, sFilename, sFilter);
	#else
		// Construct save-file dialog...
		QFileDialog fileDialog(parentWidget(),
			sTitle, sFilename, sFilter);
		// Set proper open-file modes...
		fileDialog.setAcceptMode(QFileDialog::AcceptSave);
		fileDialog.setFileMode(QFileDialog::AnyFile);
		fileDialog.setDefaultSuffix(sExt);
		// Stuff sidebar...
		QList<QUrl> urls(fileDialog.sidebarUrls());
		urls.append(QUrl::fromLocalFile(pOptions->sSessionDir));
		urls.append(QUrl::fromLocalFile(pOptions->sPresetDir));
		fileDialog.setSidebarUrls(urls);
		// Show dialog...
		if (fileDialog.exec())
			sFilename = fileDialog.selectedFiles().first();
	#endif
	}
	// We've a filename to save the preset
	if (!sFilename.isEmpty()) {
		if (QFileInfo(sFilename).suffix() != sExt)
			sFilename += '.' + sExt;
		// Get it saved alright...
		m_iUpdatePreset++;
		emit savePresetFile(sFilename);
		settings.setValue(sPreset, sFilename);
		m_iUpdatePreset--;
		pOptions->sPresetDir = QFileInfo(sFilename).absolutePath();
	}
	settings.endGroup();

	refreshPreset();
	stabilizePreset();
}


void qxgeditEdit::removePreset (void)
{
	if (m_iUpdatePreset > 0)
		return;

	const QString& sPreset = m_pComboBox->currentText();
	if (sPreset.isEmpty())
		return;

	// We'll need this, sure.
	qxgeditOptions *pOptions = qxgeditOptions::getInstance();
	if (pOptions == NULL)
		return;

	// A preset entry is about to be removed;
	// prompt user if he/she's sure about this...
	if (pOptions->bConfirmRemove) {
		if (QMessageBox::warning(this,
			tr("Warning") + " - " QXGEDIT_TITLE,
			tr("About to remove preset:\n\n"
			"\"%1\"\n\n"
			"Are you sure?")
			.arg(sPreset),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Cancel)
			return;
	}

	// Go ahead...
	m_iUpdatePreset++;

	QSettings& settings = pOptions->settings();
	settings.beginGroup("/Uservoice/Presets/");
	const QString& sFilename = settings.value(sPreset).toString();
	if (QFileInfo(sFilename).exists())
		QFile(sFilename).remove();
	settings.remove(sPreset);
	settings.endGroup();

	m_iUpdatePreset--;

	refreshPreset();
	stabilizePreset();
}


// Widget refreshner-loader.
void qxgeditEdit::refreshPreset (void)
{
	if (m_iUpdatePreset > 0)
		return;

	m_iUpdatePreset++;

	const QString sOldPreset = m_pComboBox->currentText();
	m_pComboBox->clear();
	qxgeditOptions *pOptions = qxgeditOptions::getInstance();
	if (pOptions) {
		pOptions->settings().beginGroup("/Uservoice/Presets/");
		m_pComboBox->insertItems(0, pOptions->settings().childKeys());
		pOptions->settings().endGroup();
	}
	m_pComboBox->setEditText(sOldPreset);

	m_iDirtyPreset = 0;
	m_iUpdatePreset--;
}


// Preset control.
void qxgeditEdit::stabilizePreset (void)
{
	const QString& sPreset = m_pComboBox->currentText();
	bool bEnabled = (!sPreset.isEmpty());
	bool bExists  = (m_pComboBox->findText(sPreset) >= 0);
	bool bDirty   = (m_iDirtyPreset > 0);

	if (!bDirty) {
		qxgeditXGMasterMap *pMasterMap = qxgeditXGMasterMap::getInstance();
		if (pMasterMap)
			bDirty = pMasterMap->user_dirty(pMasterMap->USERVOICE.current_key());
	}

	m_pSaveButton->setEnabled(bEnabled && (!bExists || bDirty));
	m_pRemoveButton->setEnabled(bEnabled && bExists);
}


// end of qxgeditEdit.cpp
