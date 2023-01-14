// qxgeditCheck.cpp
//
/****************************************************************************
   Copyright (C) 2005-2023, rncbc aka Rui Nuno Capela. All rights reserved.

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
#include "qxgeditCheck.h"


//-------------------------------------------------------------------------
// qxgeditCheck - Custom check-box widget.
//

// Constructor.
qxgeditCheck::qxgeditCheck ( QWidget *pParent )
	: XGParamWidget<QCheckBox> (pParent), m_pParam(nullptr)
{
	QObject::connect(this,
		SIGNAL(toggled(bool)),
		SLOT(checkToggled(bool)));
}


// Destructor.
qxgeditCheck::~qxgeditCheck (void)
{
}


// Nominal value accessors.
void qxgeditCheck::set_value ( unsigned short iValue, Observer *pSender )
{
	if (m_pParam == nullptr)
		return;

	if (iValue < m_pParam->min())
		iValue = m_pParam->min();
	if (iValue > m_pParam->max() && m_pParam->max() > m_pParam->min())
		iValue = m_pParam->max();

	bool bValueChanged = (iValue != m_pParam->value());

	m_pParam->set_value(iValue, pSender);

	QCheckBox::setChecked(iValue > 0);

	QPalette pal;
	if (QCheckBox::isEnabled()
		&& iValue != m_pParam->def()) {
		const QColor& rgbBase
			= (pal.window().color().value() < 0x7f
				? QColor(Qt::darkYellow).darker()
				: QColor(Qt::yellow).lighter());
		pal.setColor(QPalette::Base, rgbBase);
	//	pal.setColor(QPalette::Text, Qt::black);
	}
	QCheckBox::setPalette(pal);

	if (bValueChanged)
		emit valueChanged(iValue);
}

unsigned short qxgeditCheck::value (void) const
{
	return (m_pParam ? m_pParam->value() : 0);
}


// Specialty parameter accessors.
void qxgeditCheck::set_param ( XGParam *pParam, Observer */*pSender*/ )
{
	m_pParam = pParam;

	QCheckBox::setPalette(QPalette());

	if (m_pParam) {
		QCheckBox::setText(m_pParam->label());
		QCheckBox::setChecked(m_pParam->value() > 0);
		QCheckBox::setToolTip(m_pParam->text());
	}
}

XGParam *qxgeditCheck::param (void) const
{
	return m_pParam;
}


// Value settler public slot.
void qxgeditCheck::setValue ( unsigned short iValue )
{
	set_value(iValue, observer());
}


// Internal widget slots.
void qxgeditCheck::checkToggled ( bool bCheck )
{
	set_value((bCheck ? 127 : 0), observer());
	emit valueChanged(value());
}


// end of qxgeditCheck.cpp
