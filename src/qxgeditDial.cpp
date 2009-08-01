// qxgeditDial.cpp
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
#include "qxgeditDial.h"

#include "qxgeditKnob.h"
#include "qxgeditSpin.h"

#include "XGParam.h"

#include <QVBoxLayout>
#include <QLabel>


//-------------------------------------------------------------------------
// qxgeditDial - Custom composite widget.
//

// Constructor.
qxgeditDial::qxgeditDial ( QWidget *pParent )
	: XGParamWidget<QWidget> (pParent)
{
	m_pLabel = new QLabel();
	m_pKnob  = new qxgeditKnob();
	m_pSpin  = new qxgeditSpin();

	m_iBusy = 0;

	m_pLabel->setAlignment(Qt::AlignCenter);
	m_pKnob->setSingleStep(7);
	m_pKnob->setNotchesVisible(true);
	m_pSpin->setAlignment(Qt::AlignCenter);
	m_pSpin->setMaximumHeight(QFontMetrics(font()).lineSpacing() + 2);

	QVBoxLayout *pVBoxLayout = new QVBoxLayout();
	pVBoxLayout->setMargin(0);
	pVBoxLayout->setSpacing(0);
	pVBoxLayout->addWidget(m_pLabel);
	pVBoxLayout->addWidget(m_pKnob);
	pVBoxLayout->addWidget(m_pSpin);
	QWidget::setLayout(pVBoxLayout);

	setMaximumSize(QSize(52, 72));

	QObject::connect(m_pKnob,
		SIGNAL(valueChanged(int)),
		SLOT(knobValueChanged(int)));
	QObject::connect(m_pSpin,
		SIGNAL(valueChanged(unsigned short)),
		SLOT(spinValueChanged(unsigned short)));
}


// Destructor.
qxgeditDial::~qxgeditDial (void)
{
}


// Text label accessor.
void qxgeditDial::setText ( const QString& sText )
{
	m_pLabel->setText(sText);
}

QString qxgeditDial::text (void) const
{
	return m_pLabel->text();
}


// Nominal value accessors.
void qxgeditDial::set_value ( unsigned short iValue )
{
	m_pSpin->setValue(iValue);
//	m_pKnob->setValue(int(m_pSpin->value()));
}

unsigned short qxgeditDial::value (void) const
{
	return m_pSpin->value();
}


// Specialty parameter accessors.
void qxgeditDial::set_param ( XGParam *pParam )
{
	m_pSpin->setParam(pParam);

	if (pParam && pParam->name()) {
		QString sText = QString(pParam->name()).simplified();
	//	m_pLabel->setText(sText);
		m_pKnob->setMinimum(int(pParam->min()));
		m_pKnob->setMaximum(int(pParam->max()));
		m_pKnob->setDefaultValue(int(pParam->def()));
		m_pKnob->setValue(int(pParam->def()));
		if (pParam->unit())
			sText += QString(" (%1)").arg(pParam->unit());
		QWidget::setToolTip(sText);
		QWidget::setEnabled(true);
	} else {
	//	m_pLabel->clear();
		QWidget::setEnabled(false);
	}
}

XGParam *qxgeditDial::param (void) const
{
	return m_pSpin->param();
}


// Internal widget slots.
void qxgeditDial::knobValueChanged ( int iKnobValue )
{
	if (m_iBusy == 0) {
		m_iBusy++;
		m_pSpin->setValue(iKnobValue);
		emit valueChanged(value());
		m_iBusy--;
	}
}

void qxgeditDial::spinValueChanged ( unsigned short iSpinValue )
{
	if (m_iBusy == 0) {
		m_iBusy++;
		m_pKnob->setValue(int(iSpinValue));
		emit valueChanged(value());
		m_iBusy--;
	}
}


// end of qxgeditDial.cpp
