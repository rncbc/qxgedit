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

#include <QWidget>

// Forward declarations.
class QLabel;
class qxgeditKnob;
class qxgeditSpin;


//-------------------------------------------------------------------------
// qxgeditDial - Custom composite widget.

class qxgeditDial : public QWidget
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditDial(QWidget *pParent = 0);
	// Destructor.
	~qxgeditDial();

	// Text label accessor.
	void setText(const QString& sText);
	QString text() const;
	
	// Value getter.
	unsigned short value() const;

	// Minimum value accessors.
	void setMinimum(unsigned short iMinimum);
	unsigned short minimum() const;

	// Maximum value accessors.
	void setMaximum(unsigned short iMaximum);
	unsigned short maximum() const;

	// Default value accessors.
	void setDefaultValue(unsigned short iDefaultValue);
	unsigned short defaultValue() const;

	// Specialty functors setters.
	void setGetv(float (*pfnGetv)(unsigned short));
	void setGetu(unsigned short (*pfnGetu)(float));

signals:

	void valueChanged(unsigned short);

public slots:

	// Value setter.
	void setValue(unsigned short iValue);

protected slots:

	// Internal widget slots.
	void knobValueChanged(int);
	void spinValueChanged(unsigned short);

private:

	// Widget members.
	QLabel      *m_pLabel;
	qxgeditKnob *m_pKnob;
	qxgeditSpin *m_pSpin;

	// Fake-mutex.
	int m_iBusy;
};


#endif  // __qxgeditDial_h

// end of qxgeditDial.h
