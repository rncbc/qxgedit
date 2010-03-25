// qxgeditEdit.cpp
//
/****************************************************************************
   Copyright (C) 2005-2010, rncbc aka Rui Nuno Capela. All rights reserved.

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
#if QT_VERSION >= 0x040200
	m_pComboBox->setCompleter(NULL);
#endif
	m_pComboBox->setInsertPolicy(QComboBox::NoInsert);

	m_pOpenButton->setIcon(QIcon(":/images/formOpen.png"));
	m_pSaveButton->setIcon(QIcon(":/images/formSave.png"));
	m_pRemoveButton->setIcon(QIcon(":/images/formRemove.png"));

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
	m_iUpdatePreset++;
	m_pComboBox->setEditText(presetName());
	m_iUpdatePreset--;

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


// Retrieve current preset name (voice name)
QString qxgeditEdit::presetName() const
{
	QString sPreset;

	if (m_pParam) {
		sPreset = QString(
			QByteArray((const char *) m_pParam->data(), m_pParam->size()))
			.simplified();
	}

	return sPreset;
}


// Check whether current preset may be reset.
bool qxgeditEdit::queryPreset (void) 
{
	if (m_pParam == NULL)
		return false;

	const QString& sPreset = presetName();
	if (!sPreset.isEmpty()) {
		qxgeditOptions *pOptions = qxgeditOptions::getInstance();
		if (pOptions && pOptions->bConfirmReset) {
			qxgeditXGMasterMap *pMasterMap = qxgeditXGMasterMap::getInstance();
			if (pMasterMap &&
				pMasterMap->user_dirty_2(pMasterMap->USERVOICE.current_key())) {
				switch (QMessageBox::warning(this,
					tr("Warning") + " - " QXGEDIT_TITLE,
					tr("Some parameters have been changed:\n\n"
					"\"%1\".\n\nDo you want to save the changes?")
					.arg(sPreset),
					QMessageBox::Save |
					QMessageBox::Discard |
					QMessageBox::Cancel)) {
				case QMessageBox::Save:
					savePreset(sPreset);
					// Fall thru...
				case QMessageBox::Discard:
					break;
				default: // Cancel...
					m_iUpdatePreset++;
					m_pComboBox->setEditText(sPreset);
					m_iUpdatePreset--;
					return false;
				}
			}
		}
	}

	return true;
}


// Preset management slots...
void qxgeditEdit::changePreset ( const QString& sPreset )
{
	if (m_iUpdatePreset > 0)
		return;

	bool bLoadPreset = (!sPreset.isEmpty()
		&& m_pComboBox->findText(sPreset) >= 0);
	if (bLoadPreset && !queryPreset())
		return;

	if (m_pParam) {
		m_pParam->set_data(
			(unsigned char *) sPreset.toAscii().data(),
			sPreset.length(),
			observer());
		m_iDirtyPreset++;
	}

	if (bLoadPreset)
		loadPreset(sPreset);
	else
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
	settings.beginGroup(presetGroup());
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
		if (fi.exists() && queryPreset()) {
			// Get it loaded alright...
			m_iUpdatePreset++;
			emit loadPresetFile(sFilename);
			m_pComboBox->setEditText(fi.baseName());
			pOptions->sPresetDir = fi.absolutePath();
			m_iUpdatePreset--;
		}
	}

	refreshPreset();
	stabilizePreset();
}


void qxgeditEdit::savePreset (void)
{
	savePreset(m_pComboBox->currentText());
}

void qxgeditEdit::savePreset ( const QString& sPreset )
{
	if (sPreset.isEmpty())
		return;

	// We'll need this, sure.
	qxgeditOptions *pOptions = qxgeditOptions::getInstance();
	if (pOptions == NULL)
		return;

	// The current state preset is about to be saved...
	// this is where we'll make it...
	QSettings& settings = pOptions->settings();
	settings.beginGroup(presetGroup());
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
	} else if (pOptions->bConfirmRemove) {
		if (QMessageBox::warning(parentWidget(),
			tr("Warning") + " - " QXGEDIT_TITLE,
			tr("About to replace preset:\n\n"
			"\"%1\"\n\n"
			"Are you sure?")
			.arg(sPreset),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Cancel) {
			sFilename.clear();
		}
	}
	// We've a filename to save the preset
	if (!sFilename.isEmpty()) {
		if (QFileInfo(sFilename).suffix() != sExt)
			sFilename += '.' + sExt;
		// Get it saved alright...
		m_iUpdatePreset++;
		emit savePresetFile(sFilename);
		settings.setValue(sPreset, sFilename);
		pOptions->sPresetDir = QFileInfo(sFilename).absolutePath();
		m_iUpdatePreset--;
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
		if (QMessageBox::warning(parentWidget(),
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
	settings.beginGroup(presetGroup());
#ifdef QXGEDIT_REMOVE_PRESET_FILES
	const QString& sFilename = settings.value(sPreset).toString();
	if (QFileInfo(sFilename).exists())
		QFile(sFilename).remove();
#endif
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
		pOptions->settings().beginGroup(presetGroup());
		m_pComboBox->insertItems(0, pOptions->settings().childKeys());
		pOptions->settings().endGroup();
	}
	
	int iIndex = m_pComboBox->findText(sOldPreset);
	if (iIndex >= 0)
		m_pComboBox->setCurrentIndex(iIndex);
	else
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
			bDirty = pMasterMap->user_dirty_2(pMasterMap->USERVOICE.current_key());
	}

	m_pSaveButton->setEnabled(bEnabled && (!bExists || bDirty));
	m_pRemoveButton->setEnabled(bEnabled && bExists);
}


// Preset group path name.
QString qxgeditEdit::presetGroup (void)
{
	return "/Uservoice/Presets/";
}


// end of qxgeditEdit.cpp
