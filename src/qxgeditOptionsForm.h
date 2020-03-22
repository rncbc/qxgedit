// qxgeditOptionsForm.h
//
/****************************************************************************
   Copyright (C) 2005-2020, rncbc aka Rui Nuno Capela. All rights reserved.

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

#ifndef __qxgeditOptionsForm_h
#define __qxgeditOptionsForm_h

#include "ui_qxgeditOptionsForm.h"


// Forward declarations...
class qxgeditOptions;


//----------------------------------------------------------------------------
// qxgeditOptionsForm -- UI wrapper form.

class qxgeditOptionsForm : public QDialog
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditOptionsForm(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qxgeditOptionsForm();

	void setOptions(qxgeditOptions *pOptions);
	qxgeditOptions *options() const;

protected slots:

	void accept();
	void reject();

	void changed();

	void editColorThemes();

	void midiInputsChanged();
	void midiOutputsChanged();
	
	void stabilizeForm();

protected:

	// Custom color/style themes settlers.
	void resetColorThemes(const QString& sColorTheme);
	void resetStyleThemes(const QString& sStyleTheme);

private:

	// The Qt-designer UI struct...
	Ui::qxgeditOptionsForm m_ui;

	// Instance variables...
	qxgeditOptions *m_pOptions;
	int m_iDirtyCount;

	// MIDI specialties.
	int m_iMidiInputsChanged;
	int m_iMidiOutputsChanged;
};


#endif	// __qxgeditOptionsForm_h


// end of qxgeditOptionsForm.h
