// qxgeditSpin.cpp
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
#include "qxgeditSpin.h"

#include "XGParam.h"

#include <QLineEdit>

#include <cmath>


//-------------------------------------------------------------------------
// qxgeditSpin - Instance spin-box widget class.
//

// Constructor.
qxgeditSpin::qxgeditSpin ( QWidget *parent )
	: QAbstractSpinBox(parent), m_param(0)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 2, 0)
	QAbstractSpinBox::setAccelerated(true);
#endif

	QObject::connect(this,
		SIGNAL(editingFinished()),
		SLOT(editingFinishedSlot()));
	QObject::connect(QAbstractSpinBox::lineEdit(),
		SIGNAL(textChanged(const QString&)),
		SLOT(valueChangedSlot(const QString&)));
}


// Destructor.
qxgeditSpin::~qxgeditSpin (void)
{
}


// Mark that we got actual value.
void qxgeditSpin::showEvent ( QShowEvent */*pShowEvent*/ )
{
	if (m_param) {
		QAbstractSpinBox::lineEdit()->setText(textFromValue(m_param->value()));
		QAbstractSpinBox::interpretText();
	}
}


// Nominal value accessors.
void qxgeditSpin::setValue ( unsigned short iValue )
{
	if (m_param == NULL)
		return;

	int iCursorPos = QAbstractSpinBox::lineEdit()->cursorPosition();

	if (iValue < m_param->min())
		iValue = m_param->min();
	if (iValue > m_param->max() && m_param->max() > m_param->min())
		iValue = m_param->max();
	
	bool bValueChanged = (iValue != m_param->value());

	m_param->set_value(iValue);

	if (QAbstractSpinBox::isEnabled()) {
		QPalette pal;
		if (iValue != m_param->def()) {
			const QColor& rgbBase
				= (pal.window().color().value() < 0x7f
					? QColor(Qt::darkYellow).darker()
					: QColor(Qt::yellow).lighter());
			pal.setColor(QPalette::Base, rgbBase);
		//	pal.setColor(QPalette::Text, Qt::black);
		}
		QAbstractSpinBox::setPalette(pal);
	}

	if (QAbstractSpinBox::isVisible()) {
		QAbstractSpinBox::lineEdit()->setText(textFromValue(iValue));
		QAbstractSpinBox::interpretText();
	}

	if (bValueChanged)
		emit valueChanged(iValue);

	QAbstractSpinBox::lineEdit()->setCursorPosition(iCursorPos);
}

unsigned short qxgeditSpin::value (void) const
{
	if (QAbstractSpinBox::isVisible()) {
		return valueFromText(QAbstractSpinBox::text());
	} else {
		return (m_param ? m_param->value() : 0);
	}
}


// Specialty functors setters.
void qxgeditSpin::setParam ( XGParam *param )
{
	m_param = param;

	QAbstractSpinBox::setEnabled(m_param && m_param->name());
}

XGParam *qxgeditSpin::param (void) const
{
	return m_param;
}


// Inherited/override methods.
QValidator::State qxgeditSpin::validate ( QString& sText, int& iPos ) const
{
#ifdef CONFIG_DEBUG_0
	qDebug("qxgeditSpin[%p]::validate(\"%s\", %d)",
		this, sText.toUtf8().constData(), iPos);
#endif

	if (iPos == 0)
		return QValidator::Acceptable;

	const QChar& ch = sText[iPos - 1];
	if (ch == '.')
		return QValidator::Acceptable;
	if (ch.isDigit())
		return QValidator::Acceptable;

	return QValidator::Invalid;
}


void qxgeditSpin::fixup ( QString& sText ) const
{
#ifdef CONFIG_DEBUG_0
	qDebug("qxgeditSpin[%p]::fixup(\"%s\")",
		this, sText.toUtf8().constData());
#endif

	sText = textFromValue(m_param ? m_param->value() : 0);
}


void qxgeditSpin::stepBy ( int iSteps )
{
#ifdef CONFIG_DEBUG_0
	qDebug("qxgeditSpin[%p]::stepBy(%d)", this, iSteps);
#endif

	int iCursorPos = QAbstractSpinBox::lineEdit()->cursorPosition();
	
	int iValue = int(value()) + iSteps;
	if (iValue < 0)
		iValue = 0;
	setValue(iValue);

	QAbstractSpinBox::lineEdit()->setCursorPosition(iCursorPos);
}


QAbstractSpinBox::StepEnabled qxgeditSpin::stepEnabled (void) const
{
	StepEnabled flags = StepNone;

	unsigned short iValue = value();
	if (m_param) {
		if (iValue < m_param->max() || m_param->min() >= m_param->max())
			flags |= StepUpEnabled;
		if (iValue > m_param->min())
			flags |= StepDownEnabled;
	}

	return flags;
}


// Value/text format converters.
unsigned short qxgeditSpin::valueFromText ( const QString& sText ) const
{
	return (m_param ? m_param->getu(sText.toFloat()) : sText.toUShort());
}

QString qxgeditSpin::textFromValue ( unsigned short iValue ) const
{
	return QString::number(m_param ? m_param->getv(iValue) : float(iValue));
}


// Pseudo-fixup slot.
void qxgeditSpin::editingFinishedSlot (void)
{
#ifdef CONFIG_DEBUG_0
	qDebug("qxgeditSpin[%p]::editingFinishedSlot()", this);
#endif

	// Kind of final fixup.
	setValue(value());
}


// Textual value change notification.
void qxgeditSpin::valueChangedSlot ( const QString& sText )
{
#ifdef CONFIG_DEBUG_0
	qDebug("qxgeditSpin[%p]::valueChangedSlot(\"%s\")",
		this, sText.toUtf8().constData());
#endif

	// Forward this...
	emit valueChanged(sText);
}



// end of qxgeditSpin.cpp
