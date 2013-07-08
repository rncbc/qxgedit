// qxgeditSpin.cpp
//
/****************************************************************************
   Copyright (C) 2005-2013, rncbc aka Rui Nuno Capela. All rights reserved.

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
qxgeditSpin::qxgeditSpin ( QWidget *pParent )
	: QAbstractSpinBox(pParent), m_pParam(NULL)
{
	QAbstractSpinBox::setAccelerated(true);

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
	if (m_pParam) {
		QAbstractSpinBox::lineEdit()->setText(textFromValue(m_pParam->value()));
		QAbstractSpinBox::interpretText();
	}
}


// Nominal value accessors.
void qxgeditSpin::setValue ( unsigned short iValue, XGParamObserver *pSender )
{
	if (m_pParam == NULL)
		return;

	int iCursorPos = QAbstractSpinBox::lineEdit()->cursorPosition();

	if (iValue < m_pParam->min())
		iValue = m_pParam->min();
	if (iValue > m_pParam->max() && m_pParam->max() > m_pParam->min())
		iValue = m_pParam->max();

	bool bValueChanged = (iValue != m_pParam->value());

	m_pParam->set_value(iValue, pSender);

	QPalette pal;
	if (QAbstractSpinBox::isEnabled()
		&& iValue != m_pParam->def()) {
		const QColor& rgbBase
			= (pal.window().color().value() < 0x7f
				? QColor(Qt::darkYellow).darker()
				: QColor(Qt::yellow).lighter());
		pal.setColor(QPalette::Base, rgbBase);
	//	pal.setColor(QPalette::Text, Qt::black);
	}
	QAbstractSpinBox::setPalette(pal);

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
		return (m_pParam ? m_pParam->value() : 0);
	}
}


// Specialty functors setters.
void qxgeditSpin::setParam ( XGParam *pParam, XGParamObserver *pSender )
{
	m_pParam = pParam;

	QAbstractSpinBox::setPalette(QPalette());

	if (m_pParam)
		setValue(m_pParam->value(), pSender);
}

XGParam *qxgeditSpin::param (void) const
{
	return m_pParam;
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
	if (ch == '.' || ch == '-' || ch.isDigit())
		return QValidator::Acceptable;
	else
		return QValidator::Invalid;
}


void qxgeditSpin::fixup ( QString& sText ) const
{
#ifdef CONFIG_DEBUG_0
	qDebug("qxgeditSpin[%p]::fixup(\"%s\")",
		this, sText.toUtf8().constData());
#endif

	sText = textFromValue(m_pParam ? m_pParam->value() : 0);
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
	if (m_pParam) {
		if (iValue < m_pParam->max() || m_pParam->min() >= m_pParam->max())
			flags |= StepUpEnabled;
		if (iValue > m_pParam->min())
			flags |= StepDownEnabled;
	}

	return flags;
}


// Value/text format converters.
unsigned short qxgeditSpin::valueFromText ( const QString& sText ) const
{
	if (m_pParam == NULL)
		return sText.toUShort();

	const QString& sSpecialValueText = specialValueText();
	if (!sSpecialValueText.isEmpty() && sText == sSpecialValueText)
		return m_pParam->min();

	float fValue = 0.0f;
	if (sText.indexOf('k') >= 0)
		fValue = QString(sText).remove('k').toFloat() * 1000.0f;
	else
		fValue = sText.toFloat();
	return m_pParam->getu(fValue);
}

QString qxgeditSpin::textFromValue ( unsigned short iValue ) const
{
	if (m_pParam == NULL)
		return QString::number(iValue);

	const QString& sSpecialValueText = specialValueText();
	if (!sSpecialValueText.isEmpty() && iValue == m_pParam->min())
		return sSpecialValueText;

	float fValue = m_pParam->getv(iValue);
	if (fValue >= 1000.0f) {
		fValue /= 1000.0f;
		return QString::number(fValue) + 'k';
	} else {
		return QString::number(fValue);
	}
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
