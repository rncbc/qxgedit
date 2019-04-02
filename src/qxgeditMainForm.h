// qxgeditMainForm.h
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

#ifndef __qxgeditMainForm_h
#define __qxgeditMainForm_h

#include "ui_qxgeditMainForm.h"


// Forward declarations...
class qxgeditOptions;
class qxgeditMidiDevice;
class qxgeditXGMasterMap;

class QSocketNotifier;
class QTreeWidget;
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

	void contentsChanged();

protected slots:

	void fileNew();
	void fileOpen();
	void fileOpenRecent();
	void fileSave();
	void fileSaveAs();
	void fileExit();

	void viewMenubar(bool bOn);
	void viewStatusbar(bool bOn);
	void viewToolbar(bool bOn);
	void viewRandomize();
	void viewOptions();

	void helpAbout();
	void helpAboutQt();

	void stabilizeForm();

	void updateRecentFilesMenu();

	void masterResetButtonClicked();

	void reverbResetButtonClicked();
	void chorusResetButtonClicked();
	void variationResetButtonClicked();

	void multipartResetButtonClicked();
	void multipartComboActivated(int);
	void multipartVoiceComboActivated(int);
	void multipartVoiceChanged();
	void multipartPartModeChanged(unsigned short);

	void drumsetupResetButtonClicked();
	void drumsetupComboActivated(int);
	void drumsetupVoiceComboActivated(int);
	void drumsetupNoteComboActivated(int);

	void uservoiceResetButtonClicked();
	void uservoiceComboActivated(int);
	void uservoiceElementComboActivated(int);
	void uservoiceSendButtonClicked();
	void uservoiceAutoSendCheckToggled(bool);
	void uservoiceElementChanged(unsigned short);

	void uservoiceLoadPresetFile(const QString&);
	void uservoiceSavePresetFile(const QString&);

	void rpnReceived(unsigned char, unsigned short, unsigned short);
	void nrpnReceived(unsigned char, unsigned short, unsigned short);
	void sysexReceived(const QByteArray&);

	void handle_sigusr1();
	void handle_sigterm();

protected:

	void closeEvent(QCloseEvent *pCloseEvent);
	void dragEnterEvent(QDragEnterEvent *pDragEnterEvent);
	void dropEvent(QDropEvent *pDropEvent);

	bool queryClose();

	bool newSession();
	bool openSession();
	bool saveSession(bool bPrompt);
	bool closeSession();

	bool loadSessionFile(const QString& sFilename);
	bool saveSessionFile(const QString& sFilename);

	void updateRecentFiles(const QString& sFilename);

	void masterReset();

	bool isRandomizable() const;

private:

	// The Qt-designer UI struct...
	Ui::qxgeditMainForm m_ui;

	// Instance variables...
	qxgeditOptions     *m_pOptions;
	qxgeditMidiDevice  *m_pMidiDevice;
	qxgeditXGMasterMap *m_pMasterMap;

	QSocketNotifier *m_pSigusr1Notifier;
	QSocketNotifier *m_pSigtermNotifier;

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

	// Instrument/Normal Voice combo-box view soft-mutex.
	int m_iMultipartVoiceUpdate;

	// Uservoice element combo-box soft-mutex.
	int m_iUservoiceElementUpdate;

	// Kind-of singleton reference.
	static qxgeditMainForm *g_pMainForm;
};


#endif	// __qxgeditMainForm_h


// end of qxgeditMainForm.h
