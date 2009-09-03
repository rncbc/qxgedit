// qxgeditCombo.h
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

#ifndef __qxgeditCombo_h
#define __qxgeditCombo_h

#include "XGParamWidget.h"

#include <QComboBox>


//-------------------------------------------------------------------------
// qxgeditCombo - Custom combo-box widget.

class qxgeditCombo : public XGParamWidget<QComboBox>
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditCombo(QWidget *pParent = 0);
	// Destructor.
	~qxgeditCombo();

	// Specialty parameter accessors.
	void set_param(XGParam *pParam, Observer *pSender);
	XGParam *param() const;

	// Value accessors.
	void set_value(unsigned short iValue, Observer *pSender);
	unsigned short value() const;

signals:

	// Value change signal.
	void valueChanged(unsigned short);

protected slots:

	// Internal widget slots.
	void comboActivated(int);

private:

	// Instance variables.
	XGParam *m_pParam;
};


#endif  // __qxgeditCombo_h

// end of qxgeditCombo.h
