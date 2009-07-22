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

#include <QLineEdit>

#include <cmath>


//-------------------------------------------------------------------------
// qxgeditSpin - Instance spin-box widget class.
//

// Constructor.
qxgeditSpin::qxgeditSpin ( QWidget *pParent )
	: QAbstractSpinBox(pParent)
{
	m_iValue = 0;
	m_iMinimumValue = 0;
	m_iMaximumValue = 0;
	m_iDefaultValue = 0;

	m_pfnGetv = NULL;
	m_pfnGetu = NULL;

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
	QAbstractSpinBox::lineEdit()->setText(textFromValue(m_iValue));
	QAbstractSpinBox::interpretText();
}


// Nominal value accessors.
void qxgeditSpin::setValue ( unsigned short iValue )
{
	int iCursorPos = QAbstractSpinBox::lineEdit()->cursorPosition();

	if (iValue < m_iMinimumValue)
		iValue = m_iMinimumValue;
	if (iValue > m_iMaximumValue && m_iMaximumValue > m_iMinimumValue)
		iValue = m_iMaximumValue;
	
	bool bValueChanged = (iValue != m_iValue);

	m_iValue = iValue;

	if (QAbstractSpinBox::isEnabled()) {
		QPalette pal;
		if (m_iValue != m_iDefaultValue) {
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
		QAbstractSpinBox::lineEdit()->setText(textFromValue(m_iValue));
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
		return m_iValue;
	}
}


// Minimum value accessors.
void qxgeditSpin::setMinimum ( unsigned short iMinimum )
{
	m_iMinimumValue = iMinimum;
}

unsigned short qxgeditSpin::minimum (void) const
{
	return m_iMinimumValue;
}


// Maximum value accessors.
void qxgeditSpin::setMaximum ( unsigned short iMaximum )
{
	m_iMaximumValue = iMaximum;
}

unsigned short qxgeditSpin::maximum (void) const
{
	return m_iMaximumValue;
}


// Default value accessor.
void qxgeditSpin::setDefaultValue ( unsigned short iDefaultValue )
{
	m_iDefaultValue = iDefaultValue;
}

unsigned short qxgeditSpin::defaultValue (void) const
{
	return m_iDefaultValue;
}


// Specialty functors setters.
void qxgeditSpin::setGetv ( float (*pfnGetv)(unsigned short) )
{
	m_pfnGetv = pfnGetv;
}

void qxgeditSpin::setGetu ( unsigned short (*pfnGetu)(float) )
{
	m_pfnGetu = pfnGetu;
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

	sText = textFromValue(m_iValue);
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
	if (iValue < m_iMaximumValue || m_iMinimumValue >= m_iMaximumValue)
		flags |= StepUpEnabled;
	if (iValue > m_iMinimumValue)
		flags |= StepDownEnabled;

	return flags;
}


// Value/text format converters.
unsigned short qxgeditSpin::valueFromText ( const QString& sText ) const
{
	return (m_pfnGetu ? m_pfnGetu(sText.toFloat()) : sText.toUShort());
}

QString qxgeditSpin::textFromValue ( unsigned short iValue ) const
{
	return QString::number(m_pfnGetv ? m_pfnGetv(iValue) : float(iValue));
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
