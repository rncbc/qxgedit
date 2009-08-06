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
#include "qxgeditDrop.h"

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
	m_pDrop  = new qxgeditDrop();

	m_iBusy = 0;

	m_pLabel->setAlignment(Qt::AlignCenter);
	m_pKnob->setSingleStep(7);
	m_pKnob->setNotchesVisible(true);
	m_pSpin->setAlignment(Qt::AlignCenter);

	int iMaximumHeight = QFontMetrics(QWidget::font()).lineSpacing() + 2;
	m_pSpin->setMaximumHeight(iMaximumHeight);
	m_pDrop->setMaximumHeight(iMaximumHeight);

	QVBoxLayout *pVBoxLayout = new QVBoxLayout();
	pVBoxLayout->setMargin(0);
	pVBoxLayout->setSpacing(0);
	pVBoxLayout->addWidget(m_pLabel);
	pVBoxLayout->addWidget(m_pKnob);
	pVBoxLayout->addWidget(m_pSpin);
	pVBoxLayout->addWidget(m_pDrop);
	QWidget::setLayout(pVBoxLayout);

	setMaximumSize(QSize(56, 76));

	QObject::connect(m_pKnob,
		SIGNAL(valueChanged(int)),
		SLOT(knobValueChanged(int)));
	QObject::connect(m_pSpin,
		SIGNAL(valueChanged(unsigned short)),
		SLOT(spinValueChanged(unsigned short)));
	QObject::connect(m_pDrop,
		SIGNAL(valueChanged(unsigned short)),
		SLOT(dropValueChanged(unsigned short)));

//	m_pDrop->hide();
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
	if (m_iBusy == 0) {
		m_iBusy++;
		XGParam *pParam = param();
		if (pParam) {
			if (m_pSpin->isVisible())
				m_pSpin->setValue(iValue);
			else
				m_pDrop->setValue(iValue);
			m_pKnob->setValue(int(pParam->value()));
		}
		m_iBusy--;
	}
}

unsigned short qxgeditDial::value (void) const
{
	return m_pSpin->value();
}


// Specialty parameter accessors.
void qxgeditDial::set_param ( XGParam *pParam )
{
	if (m_iBusy > 0)
		return;

	m_iBusy++;

	if (pParam && pParam->name()) {
		QWidget::setEnabled(true);
		m_pLabel->setText(pParam->label());
		m_pKnob->setMinimum(int(pParam->min()));
		m_pKnob->setMaximum(int(pParam->max()));
		m_pKnob->setDefaultValue(int(pParam->def()));
		m_pKnob->setValue(int(pParam->value()));
		if (pParam->gets(pParam->min())) {
			m_pSpin->setParam(NULL);
			m_pSpin->hide();
			m_pDrop->setParam(pParam);
			m_pDrop->show();
		} else {
			m_pSpin->setParam(pParam);
			m_pSpin->show();
			m_pDrop->setParam(NULL);
			m_pDrop->hide();
		}
		QWidget::setToolTip(pParam->text());
	} else {
		QWidget::setEnabled(false);
		m_pLabel->clear();
		m_pSpin->setParam(NULL);
		m_pSpin->show();
		m_pDrop->setParam(NULL);
		m_pDrop->hide();
	}

	m_iBusy--;
}

XGParam *qxgeditDial::param (void) const
{
	if (m_pSpin->isVisible())
		return m_pSpin->param();
	else
	if (m_pDrop->isVisible())
		return m_pDrop->param();
	else
		return NULL;
}


// Internal widget slots.
void qxgeditDial::knobValueChanged ( int iKnobValue )
{
	set_value(iKnobValue);
	emit valueChanged(value());
}

void qxgeditDial::spinValueChanged ( unsigned short iSpinValue )
{
	set_value(iSpinValue);
	emit valueChanged(value());
}

void qxgeditDial::dropValueChanged ( unsigned short iDropValue )
{
	set_value(iDropValue);
	emit valueChanged(value());
}


// end of qxgeditDial.cpp
