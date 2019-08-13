// qxgeditDrop.h
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

#ifndef __qxgeditDrop_h
#define __qxgeditDrop_h

#include <QComboBox>

// Forward declarations.
class XGParam;
class XGParamObserver;


//-------------------------------------------------------------------------
// qxgeditDrop - Custom drop-down list widget.

class qxgeditDrop : public QComboBox
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditDrop(QWidget *pParent = 0);
	// Destructor.
	~qxgeditDrop();

	// Specialty parameter accessors.
	void setParam(XGParam *pParam, XGParamObserver *pSender = nullptr);
	XGParam *param() const;

	// Value accessors.
	void setValue(unsigned short iValue, XGParamObserver *pSender = nullptr);
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


#endif  // __qxgeditDrop_h

// end of qxgeditDrop.h
