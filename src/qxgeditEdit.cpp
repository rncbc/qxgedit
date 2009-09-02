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

#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>

#include <QRegExpValidator>


//-------------------------------------------------------------------------
// qxgeditEdit - Custom edit-box widget.
//

// Constructor.
qxgeditEdit::qxgeditEdit ( QWidget *pParent )
	: XGParamWidget<QWidget> (pParent), m_pParam(NULL)
{
	m_pLabel = new QLabel();
	m_pEdit  = new QLineEdit();

	m_pEdit->setValidator(new QRegExpValidator(QRegExp("[ 0-9A-Za-z]+"), this));

	QVBoxLayout *pVBoxLayout = new QVBoxLayout();
	pVBoxLayout->addWidget(m_pLabel);
	pVBoxLayout->addWidget(m_pEdit);
	pVBoxLayout->addSpacing(20);
	QWidget::setLayout(pVBoxLayout);

	QObject::connect(m_pEdit,
		SIGNAL(textChanged(const QString&)),
		SLOT(editChanged(const QString&)));
}


// Destructor.
qxgeditEdit::~qxgeditEdit (void)
{
}


// Nominal value accessors.
void qxgeditEdit::set_value ( unsigned short /*iValue*/ )
{
	if (m_pParam) {
		m_pEdit->setText(QString(
			QByteArray((const char *) m_pParam->data(), m_pParam->size()))
			.simplified());
	}
}

unsigned short qxgeditEdit::value (void) const
{
	return 0;
}


// Specialty parameter accessors.
void qxgeditEdit::set_param ( XGParam *pParam )
{
	m_pParam = static_cast<XGDataParam *> (pParam);

	if (m_pParam) {
		m_pLabel->setText(m_pParam->label());
		m_pEdit->setMaxLength(m_pParam->size());
		set_value(0);
	}
}

XGParam *qxgeditEdit::param (void) const
{
	return static_cast<XGParam *> (m_pParam);
}


// Internal widget slots.
void qxgeditEdit::editChanged ( const QString& sText )
{
	if (m_pParam) {
		m_pParam->set_data(
			(unsigned char *) sText.toAscii().data(),
			sText.length(),
			observer());
	}
}


// end of qxgeditEdit.cpp
