// qxgeditDial.h
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

#ifndef __qxgeditDial_h
#define __qxgeditDial_h

#include "XGParamWidget.h"

#include <QWidget>

// Forward declarations.
class QLabel;
class qxgeditKnob;
class qxgeditSpin;
class qxgeditDrop;


//-------------------------------------------------------------------------
// qxgeditDial - Custom composite widget.

class qxgeditDial : public XGParamWidget<QWidget>
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditDial(QWidget *pParent = 0);
	// Destructor.
	~qxgeditDial();

	// Specialty parameter accessors.
	void set_param(XGParam *pParam);
	XGParam *param() const;

	// Value accessors.
	void reset_value();
	void set_value_update(unsigned short iValue);
	void set_value(unsigned short iValue);
	unsigned short value() const;

	// Text label accessor.
	void setText(const QString& sText);
	QString text() const;

	// Special value text accessor.
	void setSpecialValueText(const QString& sText);
	QString specialValueText() const;

signals:

	// Value change signal.
	void valueChanged(unsigned short);

public slots:

	// Value settler.
	void setValue(unsigned short);

protected slots:

	// Internal widget slots.
	void knobValueChanged(int);
	void spinValueChanged(unsigned short);
	void dropValueChanged(unsigned short);

private:

	// Widget members.
	QLabel      *m_pLabel;
	qxgeditKnob *m_pKnob;
	qxgeditSpin *m_pSpin;
	qxgeditDrop *m_pDrop;

	// Fake-mutex.
	int m_iBusy;
};


#endif  // __qxgeditDial_h

// end of qxgeditDial.h
