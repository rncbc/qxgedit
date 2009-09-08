// qxgeditCombo.cpp
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
#include "qxgeditCombo.h"


//-------------------------------------------------------------------------
// qxgeditCombo - Custom combo-box widget.
//

// Constructor.
qxgeditCombo::qxgeditCombo ( QWidget *pParent )
	: XGParamWidget<QComboBox> (pParent), m_pParam(NULL)
{
	QComboBox::setMinimumWidth(140);
	QComboBox::setMaximumHeight(QFontMetrics(font()).lineSpacing() + 2);
	QComboBox::setMaxVisibleItems(16);

	QObject::connect(this,
		SIGNAL(activated(int)),
		SLOT(comboActivated(int)));
}


// Destructor.
qxgeditCombo::~qxgeditCombo (void)
{
}


// Nominal value accessors.
void qxgeditCombo::set_value ( unsigned short iValue, Observer *pSender )
{
	if (m_pParam == NULL)
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

unsigned short qxgeditCombo::value (void) const
{
	return (m_pParam ? m_pParam->value() : 0);
}


// Specialty parameter accessors.
void qxgeditCombo::set_param ( XGParam *pParam, Observer */*pSender*/ )
{
	m_pParam = pParam;

	QComboBox::clear();
	QComboBox::setPalette(QPalette());

	XGParamMap *pParamMap = param_map();
	if (pParamMap) {
		const XGParamMap::Keys& keys = pParamMap->keys();
		XGParamMap::Keys::const_iterator iter = keys.constBegin();
		for (; iter != keys.constEnd(); ++iter)
			QComboBox::addItem(iter.value(), iter.key());
	}

	if (m_pParam) {
		int iCombo = QComboBox::findData(m_pParam->value());
		if (iCombo >= 0)
			QComboBox::setCurrentIndex(iCombo);
		QComboBox::setToolTip(m_pParam->text());
	}
}

XGParam *qxgeditCombo::param (void) const
{
	return m_pParam;
}


// Internal widget slots.
void qxgeditCombo::comboActivated ( int iCombo )
{
	set_value((unsigned short) QComboBox::itemData(iCombo).toUInt(), observer());
	emit valueChanged(value());
}


// end of qxgeditCombo.cpp
