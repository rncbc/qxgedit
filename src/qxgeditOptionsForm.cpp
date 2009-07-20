// qxgeditOptionsForm.cpp
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

#include "qxgeditOptionsForm.h"

#include "qxgeditAbout.h"
#include "qxgeditOptions.h"

#include <QMessageBox>
#include <QValidator>


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
	m_pOptions = NULL;

	// Initialize dirty control state.
	m_iDirtyCount = 0;

	// Try to restore old window positioning.
	adjustSize();

	// UI signal/slot connections...
	QObject::connect(m_ui.CompletePathCheckBox,
		SIGNAL(stateChanged(int)),
		SLOT(changed()));
	QObject::connect(m_ui.MaxRecentFilesSpinBox,
		SIGNAL(valueChanged(int)),
		SLOT(changed()));
	QObject::connect(m_ui.BaseFontSizeComboBox,
		SIGNAL(editTextChanged(const QString&)),
		SLOT(changed()));
	QObject::connect(m_ui.OkPushButton,
		SIGNAL(clicked()),
		SLOT(accept()));
	QObject::connect(m_ui.CancelPushButton,
		SIGNAL(clicked()),
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

	// Other options finally.
	m_ui.CompletePathCheckBox->setChecked(m_pOptions->bCompletePath);
	m_ui.MaxRecentFilesSpinBox->setValue(m_pOptions->iMaxRecentFiles);
	if (m_pOptions->iBaseFontSize > 0)
		m_ui.BaseFontSizeComboBox->setEditText(QString::number(m_pOptions->iBaseFontSize));
	else
		m_ui.BaseFontSizeComboBox->setCurrentIndex(0);

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
	// Save options...
	if (m_iDirtyCount > 0) {
		// Display options...
		m_pOptions->bCompletePath   = m_ui.CompletePathCheckBox->isChecked();
		m_pOptions->iMaxRecentFiles = m_ui.MaxRecentFilesSpinBox->value();
		m_pOptions->iBaseFontSize   = m_ui.BaseFontSizeComboBox->currentText().toInt();
		// Reset dirty flag.
		m_iDirtyCount = 0;
	}

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
			tr("Warning") + " - " QXGEDIT_TITLE,
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


// Stabilize current form state.
void qxgeditOptionsForm::stabilizeForm (void)
{
	bool bValid = (m_iDirtyCount > 0);

	m_ui.OkPushButton->setEnabled(bValid);
}


// end of qxgeditOptionsForm.cpp

