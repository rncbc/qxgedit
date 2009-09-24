// qxgeditEdit.h
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

#ifndef __qxgeditEdit_h
#define __qxgeditEdit_h

#include "XGParamWidget.h"

#include <QWidget>

// Forward declarations.
class QComboBox;
class QToolButton;


//-------------------------------------------------------------------------
// qxgeditEdit - Custom edit-box widget.

class qxgeditEdit : public XGParamWidget<QWidget>
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditEdit(QWidget *pParent = 0);
	// Destructor.
	~qxgeditEdit();

	// Specialty parameter accessors.
	void set_param(XGParam *pParam, Observer *pSender);
	XGParam *param() const;

	// Value accessors.
	void set_value(unsigned short iValue, Observer *pSender);
	unsigned short value() const;

signals:

	void loadPresetFile(const QString&);
	void savePresetFile(const QString&);

public slots:

	void stabilizePreset();

protected slots:

	// Internal widget slots.
	void changePreset(const QString&);
	void openPreset();
	void savePreset();
	void removePreset();

protected:

	QString presetName() const;

	bool queryPreset(); 

	void loadPreset(const QString&);
	void savePreset(const QString&);

	void refreshPreset();

	static QString presetGroup();

private:

	// Instance variables.
	XGDataParam *m_pParam;

	// Widget members.
	QComboBox   *m_pComboBox;
	QToolButton *m_pOpenButton;
	QToolButton *m_pSaveButton;
	QToolButton *m_pRemoveButton;

	// Widget internal state.
	int m_iUpdatePreset;
	int m_iDirtyPreset;
};


#endif  // __qxgeditEdit_h

// end of qxgeditEdit.h
