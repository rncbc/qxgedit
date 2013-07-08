// qxgeditSpin.h
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

#ifndef __qxgeditSpin_h
#define __qxgeditSpin_h

#include <QAbstractSpinBox>

// Forward declarations.
class XGParam;
class XGParamObserver;


//-------------------------------------------------------------------------
// qxgeditSpin - A custom QSpinBox

class qxgeditSpin : public QAbstractSpinBox
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditSpin(QWidget *pParent = 0);
	// Destructor.
	~qxgeditSpin();

	// Specialty parameter accessors.
	void setParam(XGParam *pParam, XGParamObserver *pSender = NULL);
	XGParam *param() const;

	// Value accessors.
	void setValue(unsigned short iValue, XGParamObserver *pSender = NULL);
	unsigned short value() const;

protected slots:

	// Pseudo-fixup slot.
	void editingFinishedSlot();
	void valueChangedSlot(const QString&);

signals:

	// Common value change notification.
	void valueChanged(unsigned short);
	void valueChanged(const QString&);

protected:

	// Mark that we got actual value.
	void showEvent(QShowEvent *);

	// Inherited/override methods.
	QValidator::State validate(QString& sText, int& iPos) const;
	void fixup(QString& sText) const;
	void stepBy(int iSteps);
	StepEnabled stepEnabled() const;

	// Value/text format converters.
	unsigned short valueFromText(const QString& sText) const;
	QString textFromValue(unsigned short iValue) const;

private:

	// Instance variables:
	// - XG parameter reference.
	XGParam *m_pParam;
};


#endif  // __qxgeditSpin_h

// end of qxgeditSpin.h
