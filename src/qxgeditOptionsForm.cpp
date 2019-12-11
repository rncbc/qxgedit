// qxgeditOptionsForm.cpp
//
/****************************************************************************
   Copyright (C) 2005-2019, rncbc aka Rui Nuno Capela. All rights reserved.

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
#include "qxgeditOptionsForm.h"

#include "qxgeditOptions.h"

#include "qxgeditMidiDevice.h"

#include "qxgeditPaletteForm.h"

#include <QMessageBox>
#include <QPushButton>
#include <QValidator>

#include <QStyleFactory>


// Default (empty/blank) name.
static const char *g_pszDefName = QT_TRANSLATE_NOOP("qxgeditOptionsForm", "(default)");


//----------------------------------------------------------------------------
// qxgeditOptionsForm -- UI wrapper form.

// Constructor.
qxgeditOptionsForm::qxgeditOptionsForm (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QDialog(pParent, wflags)
{
	// Setup UI struct...
	m_ui.setupUi(this);

	// No settings descriptor initially (the caller will set it).
	m_pOptions = nullptr;

	// Initialize dirty control state.
	m_iDirtyCount = 0;

	// MIDI specialties.
	m_iMidiInputsChanged  = 0;
	m_iMidiOutputsChanged = 0;

	// Try to fix window geometry.
	m_ui.MidiInputListView->setMaximumHeight(72);
	m_ui.MidiOutputListView->setMaximumHeight(72);

	adjustSize();

	// UI signal/slot connections...
	QObject::connect(m_ui.MidiInputListView,
		SIGNAL(itemSelectionChanged()),
		SLOT(midiInputsChanged()));
	QObject::connect(m_ui.MidiOutputListView,
		SIGNAL(itemSelectionChanged()),
		SLOT(midiOutputsChanged()));
	QObject::connect(m_ui.ConfirmResetCheckBox,
		SIGNAL(stateChanged(int)),
		SLOT(changed()));
	QObject::connect(m_ui.ConfirmRemoveCheckBox,
		SIGNAL(stateChanged(int)),
		SLOT(changed()));
	QObject::connect(m_ui.CompletePathCheckBox,
		SIGNAL(stateChanged(int)),
		SLOT(changed()));
	QObject::connect(m_ui.MaxRecentFilesSpinBox,
		SIGNAL(valueChanged(int)),
		SLOT(changed()));
	QObject::connect(m_ui.RandomizePercentSpinBox,
		SIGNAL(valueChanged(double)),
		SLOT(changed()));
	QObject::connect(m_ui.BaseFontSizeComboBox,
		SIGNAL(editTextChanged(const QString&)),
		SLOT(changed()));
	QObject::connect(m_ui.StyleThemeComboBox,
		SIGNAL(activated(int)),
		SLOT(changed()));
	QObject::connect(m_ui.ColorThemeComboBox,
		SIGNAL(activated(int)),
		SLOT(changed()));
	QObject::connect(m_ui.ColorThemeToolButton,
		SIGNAL(clicked()),
		SLOT(editColorThemes()));
	QObject::connect(m_ui.DialogButtonBox,
		SIGNAL(accepted()),
		SLOT(accept()));
	QObject::connect(m_ui.DialogButtonBox,
		SIGNAL(rejected()),
		SLOT(reject()));
}


// Destructor.
qxgeditOptionsForm::~qxgeditOptionsForm (void)
{
}


// Populate (setup) dialog controls from settings descriptors.
void qxgeditOptionsForm::setOptions ( qxgeditOptions *pOptions )
{
	// Set reference descriptor.
	m_pOptions = pOptions;

	// MIDI devices listings...
	m_ui.MidiInputListView->clear();
	m_ui.MidiOutputListView->clear();
	qxgeditMidiDevice *pMidiDevice = qxgeditMidiDevice::getInstance();
	if (pMidiDevice) {
		m_ui.MidiInputListView->addItems(pMidiDevice->inputs());
		m_ui.MidiOutputListView->addItems(pMidiDevice->outputs());
	}
	// MIDI Inputs...
	QStringListIterator ins(m_pOptions->midiInputs);
	while (ins.hasNext()) {
		QListIterator<QListWidgetItem *> iter(
			m_ui.MidiInputListView->findItems(ins.next(), Qt::MatchExactly));
		while (iter.hasNext())
			iter.next()->setSelected(true);
	}
	// MIDI Outputs...
	QStringListIterator outs(m_pOptions->midiOutputs);
	while (outs.hasNext()) {
		QListIterator<QListWidgetItem *> iter(
			m_ui.MidiOutputListView->findItems(outs.next(), Qt::MatchExactly));
		while (iter.hasNext())
			iter.next()->setSelected(true);
	}
	// But (re)start clean...
	m_iMidiInputsChanged  = 0;
	m_iMidiOutputsChanged = 0;

	// Other options finally.
	m_ui.ConfirmResetCheckBox->setChecked(m_pOptions->bConfirmReset);
	m_ui.ConfirmRemoveCheckBox->setChecked(m_pOptions->bConfirmRemove);
	m_ui.CompletePathCheckBox->setChecked(m_pOptions->bCompletePath);
	m_ui.MaxRecentFilesSpinBox->setValue(m_pOptions->iMaxRecentFiles);
	m_ui.RandomizePercentSpinBox->setValue(m_pOptions->fRandomizePercent);
	if (m_pOptions->iBaseFontSize > 0)
		m_ui.BaseFontSizeComboBox->setEditText(QString::number(m_pOptions->iBaseFontSize));
	else
		m_ui.BaseFontSizeComboBox->setCurrentIndex(0);

	// Custom display options...
	resetColorThemes(m_pOptions->sColorTheme);
	resetStyleThemes(m_pOptions->sStyleTheme);

	// Done. Restart clean.
	m_iDirtyCount = 0;
	stabilizeForm();
}


// Retrieve the editing options, if the case arises.
qxgeditOptions *qxgeditOptionsForm::options (void) const
{
	return m_pOptions;
}


// Accept settings (OK button slot).
void qxgeditOptionsForm::accept (void)
{
	// MIDI connections options.
	qxgeditMidiDevice *pMidiDevice = qxgeditMidiDevice::getInstance();
	if (pMidiDevice) {
		// MIDI Inputs...
		if (m_iMidiInputsChanged > 0) {
			m_pOptions->midiInputs.clear();
			QListIterator<QListWidgetItem *> iter1(
				m_ui.MidiInputListView->selectedItems());
			while (iter1.hasNext())
				m_pOptions->midiInputs.append(iter1.next()->text());
			pMidiDevice->connectInputs(m_pOptions->midiInputs);
			m_iMidiInputsChanged = 0;
		}
		// MIDI Outputs...
		if (m_iMidiOutputsChanged > 0) {
			m_pOptions->midiOutputs.clear();
			QListIterator<QListWidgetItem *> iter2(
				m_ui.MidiOutputListView->selectedItems());
			while (iter2.hasNext())
				m_pOptions->midiOutputs.append(iter2.next()->text());
			pMidiDevice->connectOutputs(m_pOptions->midiOutputs);
			m_iMidiOutputsChanged = 0;
		}
	}

	// Save options...
	if (m_iDirtyCount > 0) {
		// Display options...
		m_pOptions->bConfirmReset   = m_ui.ConfirmResetCheckBox->isChecked();
		m_pOptions->bConfirmRemove  = m_ui.ConfirmRemoveCheckBox->isChecked();
		m_pOptions->bCompletePath   = m_ui.CompletePathCheckBox->isChecked();
		m_pOptions->iMaxRecentFiles = m_ui.MaxRecentFilesSpinBox->value();
		m_pOptions->fRandomizePercent = float(m_ui.RandomizePercentSpinBox->value());
		m_pOptions->iBaseFontSize   = m_ui.BaseFontSizeComboBox->currentText().toInt();
		// Custom options...
		const QString sOldStyleTheme = m_pOptions->sStyleTheme;
		if (m_ui.StyleThemeComboBox->currentIndex() > 0)
			m_pOptions->sStyleTheme = m_ui.StyleThemeComboBox->currentText();
		else
			m_pOptions->sStyleTheme.clear();
		const QString sOldColorTheme = m_pOptions->sColorTheme;
		if (m_ui.ColorThemeComboBox->currentIndex() > 0)
			m_pOptions->sColorTheme = m_ui.ColorThemeComboBox->currentText();
		else
			m_pOptions->sColorTheme.clear();
		// Check whether restart is needed or whether
		// custom options maybe set up immediately...
		int iNeedRestart = 0;
		if (m_pOptions->sStyleTheme != sOldStyleTheme) {
			if (m_pOptions->sStyleTheme.isEmpty()) {
				++iNeedRestart;
			} else {
				QApplication::setStyle(
					QStyleFactory::create(m_pOptions->sStyleTheme));
			}
		}
		if (m_pOptions->sColorTheme != sOldColorTheme) {
			if (m_pOptions->sColorTheme.isEmpty()) {
				++iNeedRestart;
			} else {
				QPalette pal;
				if (qxgeditPaletteForm::namedPalette(
						&m_pOptions->settings(), m_pOptions->sColorTheme, pal))
					QApplication::setPalette(pal);
			}
		}
		// Show restart message if needed...
		if (iNeedRestart > 0) {
			QMessageBox::information(this,
				tr("Information"),
				tr("Some settings may be only effective\n"
				"next time you start this application."));
		}
		// Reset dirty flag.
		m_iDirtyCount = 0;
	}

	// Save/commit to disk.
	m_pOptions->saveOptions();

	// Just go with dialog acceptance
	QDialog::accept();
}


// Reject settings (Cancel button slot).
void qxgeditOptionsForm::reject (void)
{
	bool bReject = true;

	// Check if there's any pending changes...
	if (m_iDirtyCount > 0) {
		switch (QMessageBox::warning(this,
			tr("Warning"),
			tr("Some settings have been changed.\n\n"
			"Do you want to apply the changes?"),
			QMessageBox::Apply |
			QMessageBox::Discard |
			QMessageBox::Cancel)) {
		case QMessageBox::Apply:
			accept();
			return;
		case QMessageBox::Discard:
			break;
		default:    // Cancel.
			bReject = false;
		}
	}

	if (bReject)
		QDialog::reject();
}


// Dirty up settings.
void qxgeditOptionsForm::changed (void)
{
	m_iDirtyCount++;
	stabilizeForm();
}


// Custom color palette theme manager.
void qxgeditOptionsForm::editColorThemes (void)
{
	qxgeditPaletteForm form(this);
	form.setSettings(&m_pOptions->settings());

	QString sColorTheme;
	int iDirtyColorTheme = 0;

	const int iColorTheme
		= m_ui.ColorThemeComboBox->currentIndex();
	if (iColorTheme > 0) {
		sColorTheme = m_ui.ColorThemeComboBox->itemText(iColorTheme);
		form.setPaletteName(sColorTheme);
	}

	if (form.exec() == QDialog::Accepted) {
		sColorTheme = form.paletteName();
		++iDirtyColorTheme;
	}

	if (iDirtyColorTheme > 0 || form.isDirty()) {
		resetColorThemes(sColorTheme);
		changed();
	}
}


// Custom color palette themes settler.
void qxgeditOptionsForm::resetColorThemes ( const QString& sColorTheme )
{
	m_ui.ColorThemeComboBox->clear();
	m_ui.ColorThemeComboBox->addItem(
		tr(g_pszDefName));
	m_ui.ColorThemeComboBox->addItems(
		qxgeditPaletteForm::namedPaletteList(&m_pOptions->settings()));

	int iColorTheme = 0;
	if (!sColorTheme.isEmpty()) {
		iColorTheme = m_ui.ColorThemeComboBox->findText(sColorTheme);
		if (iColorTheme < 0)
			iColorTheme = 0;
	}
	m_ui.ColorThemeComboBox->setCurrentIndex(iColorTheme);
}


// Custom widget style themes settler.
void qxgeditOptionsForm::resetStyleThemes ( const QString& sStyleTheme )
{
	m_ui.StyleThemeComboBox->clear();
	m_ui.StyleThemeComboBox->addItem(
		tr(g_pszDefName));
	m_ui.StyleThemeComboBox->addItems(QStyleFactory::keys());

	int iStyleTheme = 0;
	if (!sStyleTheme.isEmpty()) {
		iStyleTheme = m_ui.StyleThemeComboBox->findText(sStyleTheme);
		if (iStyleTheme < 0)
			iStyleTheme = 0;
	}
	m_ui.StyleThemeComboBox->setCurrentIndex(iStyleTheme);
}



// MIDI I/O changes
void qxgeditOptionsForm::midiInputsChanged (void)
{
	m_iMidiInputsChanged++;
	changed();
}


void qxgeditOptionsForm::midiOutputsChanged (void)
{
	m_iMidiOutputsChanged++;
	changed();
}


// Stabilize current form state.
void qxgeditOptionsForm::stabilizeForm (void)
{
	bool bValid = (m_iDirtyCount > 0);

	m_ui.DialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(bValid);
}


// end of qxgeditOptionsForm.cpp

