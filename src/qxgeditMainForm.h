// qxgeditMainForm.h
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

#ifndef __qxgeditMainForm_h
#define __qxgeditMainForm_h

#include "ui_qxgeditMainForm.h"


// Forward declarations...
class qxgeditOptions;
class qxgeditMidiDevice;
class qxgeditMidiSysexEvent;
class qxgeditXGParamMap;

class QLabel;


//----------------------------------------------------------------------------
// qxgeditMainForm -- UI wrapper form.

class qxgeditMainForm : public QMainWindow
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditMainForm(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qxgeditMainForm();

	static qxgeditMainForm *getInstance();

	void setup(qxgeditOptions *pOptions);

	QString sessionName(const QString& sFilename);

	void showMessage(const QString& s);
	void showMessageError(const QString& s);

public slots:

	void fileNew();
	void fileOpen();
	void fileOpenRecent();
	void fileSave();
	void fileSaveAs();
	void fileExit();

	void viewMenubar(bool bOn);
	void viewStatusbar(bool bOn);
	void viewToolbar(bool bOn);
	void viewOptions();

	void helpAbout();
	void helpAboutQt();

	void stabilizeForm();

	void updateRecentFilesMenu();

	void masterResetButtonClicked();

	void multipartComboActivated(int);

	void drumsetupComboActivated(int);
	void drumsetupNoteComboActivated(int);
	void drumsetupResetButtonClicked();

protected:

	void closeEvent(QCloseEvent *pCloseEvent);
	void dragEnterEvent(QDragEnterEvent *pDragEnterEvent);
	void dropEvent(QDropEvent *pDropEvent);
	void customEvent(QEvent *pEvent);

	bool queryClose();

	bool newSession();
	bool openSession();
	bool saveSession(bool bPrompt);
	bool closeSession();

	bool loadSessionFile(const QString& sFilename);
	bool saveSessionFile(const QString& sFilename);

	void updateRecentFiles(const QString& sFilename);

	// SYSEX Event handler.
	bool sysexEvent(qxgeditMidiSysexEvent *pSysexEvent);
	bool sysexData(unsigned char *data, unsigned short len);

	// XG Parameter state handler.
	unsigned short sysexXGParam(
		unsigned char high, unsigned char mid, unsigned char low,
		unsigned char *data);

private:

	// The Qt-designer UI struct...
	Ui::qxgeditMainForm m_ui;

	// Instance variables...
	qxgeditOptions    *m_pOptions;
	qxgeditMidiDevice *m_pMidiDevice;
	qxgeditXGParamMap *m_pParamMap;

	QString m_sFilename;
	int m_iUntitled;
	int m_iDirtyCount;

	// Status bar item indexes
	enum {
		StatusName    = 0,   // Active session track caption.
		StatusMod     = 1,   // Current session modification state.
		StatusItems   = 2    // Number of status items.
	};

	QLabel *m_statusItems[StatusItems];

	// Kind-of singleton reference.
	static qxgeditMainForm *g_pMainForm;
};


#endif	// __qxgeditMainForm_h


// end of qxgeditMainForm.h
