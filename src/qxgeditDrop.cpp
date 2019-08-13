// qxgeditDrop.cpp
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
#include "qxgeditDrop.h"

#include "XGParam.h"


//-------------------------------------------------------------------------
// qxgeditDrop - Custom drop-down list widget.
//

// Constructor.
qxgeditDrop::qxgeditDrop ( QWidget *pParent )
	: QComboBox(pParent), m_pParam(nullptr)
{
	QObject::connect(this,
		SIGNAL(activated(int)),
		SLOT(comboActivated(int)));
}


// Destructor.
qxgeditDrop::~qxgeditDrop (void)
{
}


// Nominal value accessors.
void qxgeditDrop::setValue ( unsigned short iValue, XGParamObserver *pSender )
{
	if (m_pParam == nullptr)
		return;

	if (iValue < m_pParam->min())
		iValue = m_pParam->min();
	if (iValue > m_pParam->max() && m_pParam->max() > m_pParam->min())
		iValue = m_pParam->max();

	bool bValueChanged = (iValue != m_pParam->value());

	m_pParam->set_value(iValue, pSender);

	int iCombo = QComboBox::findData(iValue);
	if (iCombo >= 0)
		QComboBox::setCurrentIndex(iCombo);

	QPalette pal;
	if (QComboBox::isEnabled()
		&& iValue != m_pParam->def()) {
		const QColor& rgbBase
			= (pal.window().color().value() < 0x7f
				? QColor(Qt::darkYellow).darker()
				: QColor(Qt::yellow).lighter());
		pal.setColor(QPalette::Base, rgbBase);
	//	pal.setColor(QPalette::Text, Qt::black);
	}
	QComboBox::setPalette(pal);

	if (bValueChanged)
		emit valueChanged(iValue);
}

unsigned short qxgeditDrop::value (void) const
{
	return (m_pParam ? m_pParam->value() : 0);
}


// Specialty parameter accessors.
void qxgeditDrop::setParam ( XGParam *pParam, XGParamObserver *pSender )
{
	m_pParam = pParam;

	QComboBox::clear();
	QComboBox::setPalette(QPalette());

	if (m_pParam) {
		unsigned short iValue = m_pParam->min();
		for (; m_pParam->max() >= iValue; ++iValue) {
			const char *pszItem = m_pParam->gets(iValue);
			if (pszItem)
				QComboBox::addItem(pszItem, iValue);
		}
		setValue(m_pParam->value(), pSender);
	}
}

XGParam *qxgeditDrop::param (void) const
{
	return m_pParam;
}


// Internal widget slots.
void qxgeditDrop::comboActivated ( int iCombo )
{
	setValue((unsigned short) QComboBox::itemData(iCombo).toUInt());
}


// end of qxgeditDrop.cpp
