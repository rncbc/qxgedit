// qxgeditMainForm.cpp
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

#include "qxgeditMainForm.h"

#include "qxgeditAbout.h"
#include "qxgeditOptions.h"

#include "qxgeditMidiDevice.h"

#include "qxgeditDial.h"

#include "qxgeditOptionsForm.h"

#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QUrl>

#include <QStatusBar>
#include <QLabel>

#include <QDragEnterEvent>
#include <QCloseEvent>
#include <QDropEvent>

#if defined(WIN32)
#undef HAVE_SIGNAL_H
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

// Specialties for thread-callback comunication.
#define QXGEDIT_SYSEX_EVENT QEvent::Type(QEvent::User + 1)


//----------------------------------------------------------------------------
// qxgeditMainForm -- UI wrapper form.
//
qxgeditMainForm *qxgeditMainForm::g_pMainForm = NULL;

// Constructor.
qxgeditMainForm::qxgeditMainForm (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QMainWindow(pParent, wflags)
{
	// Setup UI struct...
	m_ui.setupUi(this);

	// Pseudo-singleton reference setup.
	g_pMainForm = this;

	// Initialize some pointer references.
	m_pOptions = NULL;
	m_pMidiDevice = NULL;
	m_pParamMasterMap = NULL;
#ifdef XGPARAM_WIDGET_MAP
	m_pParamWidgetMap = NULL;
#endif

	// We'll start clean.
	m_iUntitled   = 0;
	m_iDirtyCount = 0;

#ifdef HAVE_SIGNAL_H
	// Set to ignore any fatal "Broken pipe" signals.
	::signal(SIGPIPE, SIG_IGN);
#endif

	// Create some statusbar labels...
	const QSize pad(4, 0);
	QLabel *pLabel;

	// Track status.
	pLabel = new QLabel(tr("Session"));
	pLabel->setAlignment(Qt::AlignLeft);
	pLabel->setToolTip(tr("Current session name"));
	pLabel->setAutoFillBackground(true);
	m_statusItems[StatusName] = pLabel;
	statusBar()->addWidget(pLabel, 2);

	// Session modification status.
	pLabel = new QLabel(tr("MOD"));
	pLabel->setAlignment(Qt::AlignHCenter);
	pLabel->setMinimumSize(pLabel->sizeHint() + pad);
	pLabel->setToolTip(tr("Session modification state"));
	pLabel->setAutoFillBackground(true);
	m_statusItems[StatusMod] = pLabel;
	statusBar()->addWidget(pLabel);

	// Some actions surely need those
	// shortcuts firmly attached...
	addAction(m_ui.viewMenubarAction);

	// Ah, make it stand right.
	setFocus();

	// UI signal/slot connections...
	QObject::connect(m_ui.fileNewAction,
		SIGNAL(triggered(bool)),
		SLOT(fileNew()));
	QObject::connect(m_ui.fileOpenAction,
		SIGNAL(triggered(bool)),
		SLOT(fileOpen()));
	QObject::connect(m_ui.fileSaveAction,
		SIGNAL(triggered(bool)),
		SLOT(fileSave()));
	QObject::connect(m_ui.fileSaveAsAction,
		SIGNAL(triggered(bool)),
		SLOT(fileSaveAs()));
	QObject::connect(m_ui.fileExitAction,
		SIGNAL(triggered(bool)),
		SLOT(fileExit()));

	QObject::connect(m_ui.viewMenubarAction,
		SIGNAL(triggered(bool)),
		SLOT(viewMenubar(bool)));
	QObject::connect(m_ui.viewStatusbarAction,
		SIGNAL(triggered(bool)),
		SLOT(viewStatusbar(bool)));
	QObject::connect(m_ui.viewToolbarAction,
		SIGNAL(triggered(bool)),
		SLOT(viewToolbar(bool)));
	QObject::connect(m_ui.viewOptionsAction,
		SIGNAL(triggered(bool)),
		SLOT(viewOptions()));

	QObject::connect(m_ui.helpAboutAction,
		SIGNAL(triggered(bool)),
		SLOT(helpAbout()));
	QObject::connect(m_ui.helpAboutQtAction,
		SIGNAL(triggered(bool)),
		SLOT(helpAboutQt()));

	QObject::connect(m_ui.fileMenu,
		SIGNAL(aboutToShow()),
		SLOT(updateRecentFilesMenu()));
}


// Destructor.
qxgeditMainForm::~qxgeditMainForm (void)
{
	// Free designated devices.
	if (m_pMidiDevice)
		delete m_pMidiDevice;
#ifdef XGPARAM_WIDGET_MAP
	if (m_pParamWidgetMap)
		delete m_pParamWidgetMap;
#endif
	if (m_pParamMasterMap)
		delete m_pParamMasterMap;

	// Pseudo-singleton reference shut-down.
	g_pMainForm = NULL;
}


// kind of singleton reference.
qxgeditMainForm *qxgeditMainForm::getInstance (void)
{
	return g_pMainForm;
}


// Make and set a proper setup options step.
void qxgeditMainForm::setup ( qxgeditOptions *pOptions )
{
	// We got options?
	m_pOptions = pOptions;

	// Initial decorations toggle state.
	m_ui.viewMenubarAction->setChecked(m_pOptions->bMenubar);
	m_ui.viewStatusbarAction->setChecked(m_pOptions->bStatusbar);
	m_ui.viewToolbarAction->setChecked(m_pOptions->bToolbar);

	// Initial decorations visibility state.
	viewMenubar(m_pOptions->bMenubar);
	viewStatusbar(m_pOptions->bStatusbar);
	viewToolbar(m_pOptions->bToolbar);

	// Try to restore old window positioning.
	m_pOptions->loadWidgetGeometry(this);

	// Primary startup stabilization...
	updateRecentFilesMenu();

	// XG master database...
	m_pParamMasterMap = new XGParamMasterMap();
#ifdef XGPARAM_WIDGET_MAP
	m_pParamWidgetMap = new XGParamWidgetMap();
#endif

	// Start proper devices...
	m_pMidiDevice = new qxgeditMidiDevice(QXGEDIT_TITLE);
    m_pMidiDevice->setNotifyObject(this);
    m_pMidiDevice->setNotifySysexType(QXGEDIT_SYSEX_EVENT);

	// And respective connections...
	m_pMidiDevice->connectInputs(m_pOptions->midiInputs);
	m_pMidiDevice->connectOutputs(m_pOptions->midiOutputs);

	// Change to last known session dir...
	if (!m_pOptions->sSessionDir.isEmpty())
		QDir::setCurrent(m_pOptions->sSessionDir);

	// Setup UI widgets...
	QWidget *pCentralWidget = centralWidget();
	const QFont& font = pCentralWidget->font();
	pCentralWidget->setFont(QFont(font.family(), font.pointSize() - 2));

	m_ui.MasterTuneDial->set_param_map(&(m_pParamMasterMap->SYSTEM), 0x00); // MASTER TUNE
	m_ui.MasterVolumeDial->set_param_map(&(m_pParamMasterMap->SYSTEM), 0x04); // MASTER VOLUME
	m_ui.MasterTransposeDial->set_param_map(&(m_pParamMasterMap->SYSTEM), 0x06); // MASTER TRANSPOSE

	// Is any session pending to be loaded?
	if (!m_pOptions->sSessionFile.isEmpty()) {
		// Just load the prabably startup session...
		if (loadSessionFile(m_pOptions->sSessionFile))
			m_pOptions->sSessionFile.clear();
	} else {
		// Open up with a new empty session...
		newSession();
	}

	// Make it ready :-)
	statusBar()->showMessage(tr("Ready"), 3000);
}


// Window close event handlers.
bool qxgeditMainForm::queryClose (void)
{
	bool bQueryClose = closeSession();

	// Try to save current general state...
	if (m_pOptions) {
		// Try to save current positioning.
		if (bQueryClose) {
			// Save decorations state.
			m_pOptions->bMenubar = m_ui.MenuBar->isVisible();
			m_pOptions->bStatusbar = statusBar()->isVisible();
			m_pOptions->bToolbar = m_ui.ToolBar->isVisible();
			// Save main windows state.
			m_pOptions->saveWidgetGeometry(this);
		}
	}

	return bQueryClose;
}


void qxgeditMainForm::closeEvent ( QCloseEvent *pCloseEvent )
{
	// Let's be sure about that...
	if (queryClose()) {
		pCloseEvent->accept();
		QApplication::quit();
	} else {
		pCloseEvent->ignore();
	}
}


// Window drag-n-drop event handlers.
void qxgeditMainForm::dragEnterEvent ( QDragEnterEvent* pDragEnterEvent )
{
	// Accept external drags only...
	if (pDragEnterEvent->source() == NULL
		&& pDragEnterEvent->mimeData()->hasUrls()) {
		pDragEnterEvent->accept();
	} else {
		pDragEnterEvent->ignore();
	}
}


void qxgeditMainForm::dropEvent ( QDropEvent* pDropEvent )
{
	// Accept externally originated drops only...
	if (pDropEvent->source())
		return;

	const QMimeData *pMimeData = pDropEvent->mimeData();
	if (pMimeData->hasUrls()) {
		QString sFilename
			= QDir::toNativeSeparators(pMimeData->urls().first().toLocalFile());
		// Close current session and try to load the new one...
		if (!sFilename.isEmpty() && closeSession())
			loadSessionFile(sFilename);
	}
}


// Custom event handler.
void qxgeditMainForm::customEvent ( QEvent *pEvent )
{
	switch (pEvent->type()) {
	case QXGEDIT_SYSEX_EVENT:
		sysexEvent(static_cast<qxgeditMidiSysexEvent *> (pEvent));
		// Fall thru...
	default:
		break;
	}
}


// SYSEX Event handler.
bool qxgeditMainForm::sysexEvent ( qxgeditMidiSysexEvent *pSysexEvent )
{
	return sysexData(pSysexEvent->data(), pSysexEvent->len());
}

bool qxgeditMainForm::sysexData ( unsigned char *data, unsigned short len )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditMainForm::sysexData(%p, %u)", data, len);
#endif

	bool ret = false;
	unsigned short i;

	if (data[0] == 0xf0 && data[len - 1] == 0xf7) {
		 // SysEx (actually)...
		if (data[1] == 0x43) {
			// Yamaha ID...
			unsigned char mode  = (data[2] & 0x70);
		//	unsigned char devno = (data[2] & 0x0f);
			if (data[3] == 0x4c) {
				// XG Model ID...
				if (mode == 0x00) {
					// Native Bulk Dump...
					unsigned short size = (data[4] << 7) + data[5];
					unsigned char cksum = 0;
					for (i = 0; i < size + 5; ++i) {
						cksum += data[4 + i];
						cksum &= 0x7f;
					}
					if (data[9 + size] == 0x80 - cksum) {
						unsigned char high = data[6];
						unsigned char mid  = data[7];
						unsigned char low  = data[8];
						for (i = 0; i < size; ++i) {
							// Parameter Change...
							unsigned short n
								= sysexXGParam(high, mid, low + i, &data[9 + i]);
							if (n < 1)
								break;
							if (n > 1)
								i += (n - 1);
						}
						ret = (i == (size - 6));
					}
				}
				else
				if (mode == 0x10) {
					// Parameter Change...
					unsigned char high = data[4];
					unsigned char mid  = data[5];
					unsigned char low  = data[6];
					ret = (sysexXGParam(high, mid, low, &data[7]) > 0);
				}
			}
		}
	}
	
	return ret;
}

unsigned short qxgeditMainForm::sysexXGParam (
	unsigned char high, unsigned char mid, unsigned char low,
	unsigned char *data )
{
	XGParam *param = m_pParamMasterMap->find_param(high, mid, low);
	if (param == NULL)
		return 0;

	param->set_value(param->value_data(data));

#ifdef CONFIG_DEBUG
	fprintf(stderr, "< %02x %02x %02x",
		param->high(),
		param->mid(),
		param->low());
	const char *name = param->name();
	if (name) {
		unsigned short c = param->value();
		const char *s = param->gets(c);
		const char *u = param->unit();
		fprintf(stderr, " %s", name);
		fprintf(stderr, " %g", param->getv(c));
		if (s) fprintf(stderr, " %s", s);
		if (u) fprintf(stderr, " %s", u);
	}
	fprintf(stderr, " >\n");
#endif

	return param->size();
}


//-------------------------------------------------------------------------
// qxgeditMainForm -- Session file stuff.

// Format the displayable session filename.
QString qxgeditMainForm::sessionName ( const QString& sFilename )
{
	bool bCompletePath = (m_pOptions && m_pOptions->bCompletePath);
	QString sSessionName = sFilename;
	if (sSessionName.isEmpty())
		sSessionName = tr("Untitled") + QString::number(m_iUntitled);
	else if (!bCompletePath)
		sSessionName = QFileInfo(sSessionName).baseName();
	return sSessionName;
}


// Create a new session file from scratch.
bool qxgeditMainForm::newSession (void)
{
	// Check if we can do it.
	if (!closeSession())
		return false;

	// Ok, increment untitled count.
	m_iUntitled++;

	// Stabilize form.
	m_sFilename.clear();
//	m_iDirtyCount = 0;

	stabilizeForm();

	return true;
}


// Open an existing sampler session.
bool qxgeditMainForm::openSession (void)
{
	if (m_pOptions == NULL)
		return false;

	// Ask for the filename to open...
	QString sFilename;

	QString sExt("syx");
	const QString& sTitle  = tr("Open Session") + " - " QXGEDIT_TITLE;
	const QString& sFilter = tr("Session files (*.%1)").arg(sExt);
#if QT_VERSION < 0x040400
	sFilename = QFileDialog::getOpenFileName(this,
		sTitle, m_pOptions->sSessionDir, sFilter);
#else
	// Construct open-file session/template dialog...
	QFileDialog fileDialog(this,
		sTitle, m_pOptions->sSessionDir, sFilter);
	// Set proper open-file modes...
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::ExistingFile);
	fileDialog.setHistory(m_pOptions->recentFiles);
	fileDialog.setDefaultSuffix(sExt);
	// Stuff sidebar...
	QList<QUrl> urls(fileDialog.sidebarUrls());
	urls.append(QUrl::fromLocalFile(m_pOptions->sSessionDir));
	fileDialog.setSidebarUrls(urls);
	// Show dialog...
	if (!fileDialog.exec())
		return false;
	// Have the open-file name...
	sFilename = fileDialog.selectedFiles().first();
#endif

	// Have we cancelled?
	if (sFilename.isEmpty())
		return false;

	// Check if we're going to discard safely the current one...
	if (!closeSession())
		return false;

	// Load it right away.
	return loadSessionFile(sFilename);
}


// Save current sampler session with another name.
bool qxgeditMainForm::saveSession ( bool bPrompt )
{
	if (m_pOptions == NULL)
		return false;

	// Suggest a filename, if there's none...
	QString sFilename = m_sFilename;
	if (sFilename.isEmpty())
		bPrompt = true;

	// Ask for the file to save...
	if (bPrompt) {
		// Prompt the guy...
		QString sExt("syx");
		const QString& sTitle  = tr("Save Session") + " - " QXGEDIT_TITLE;
		const QString& sFilter = tr("Session files (*.%1)").arg(sExt);
	#if QT_VERSION < 0x040400
		sFilename = QFileDialog::getSaveFileName(this,
			sTitle, sFilename, sFilter);
	#else
		// Construct save-file session/template dialog...
		QFileDialog fileDialog(this,
			sTitle, sFilename, sFilter);
		// Set proper save-file modes...
		fileDialog.setAcceptMode(QFileDialog::AcceptSave);
		fileDialog.setFileMode(QFileDialog::AnyFile);
		fileDialog.setHistory(m_pOptions->recentFiles);
		fileDialog.setDefaultSuffix(sExt);
		// Stuff sidebar...
		QList<QUrl> urls(fileDialog.sidebarUrls());
		urls.append(QUrl::fromLocalFile(m_pOptions->sSessionDir));
		fileDialog.setSidebarUrls(urls);
		// Show save-file dialog...
		if (!fileDialog.exec())
			return false;
		// Have the save-file name...
		sFilename = fileDialog.selectedFiles().first();
	#endif
		// Have we cancelled it?
		if (sFilename.isEmpty())
			return false;
		// Enforce extension...
		if (QFileInfo(sFilename).suffix() != sExt) {
			sFilename += '.' + sExt;
			// Check if already exists...
			if (sFilename != m_sFilename && QFileInfo(sFilename).exists()) {
				if (QMessageBox::warning(this,
					tr("Warning") + " - " QXGEDIT_TITLE,
					tr("The file already exists:\n\n"
					"\"%1\"\n\n"
					"Do you want to replace it?")
					.arg(sessionName(sFilename)),
					QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
					return false;
			}
		}
	}

	// Save it right away.
	return saveSessionFile(sFilename);
}


// Close current session.
bool qxgeditMainForm::closeSession (void)
{
	bool bClose = true;

	// Are we dirty enough to prompt it?
	if (m_iDirtyCount > 0) {
		switch (QMessageBox::warning(this,
			tr("Warning") + " - " QXGEDIT_TITLE,
			tr("The current session has been changed:\n\n"
			"\"%1\"\n\n"
			"Do you want to save the changes?")
			.arg(sessionName(m_sFilename)),
			QMessageBox::Save |
			QMessageBox::Discard |
			QMessageBox::Cancel)) {
		case QMessageBox::Save:
			bClose = saveSession(false);
			// Fall thru....
		case QMessageBox::Discard:
			break;
		default:    // Cancel.
			bClose = false;
			break;
		}
	}

	// If we may close it, dot it.
	if (bClose) {
		// TODO: Actually close session...
		// We're now clean, for sure.
		m_iDirtyCount = 0;
	}

	return bClose;
}


// Load a session from specific file path.
bool qxgeditMainForm::loadSessionFile ( const QString& sFilename )
{
	// Tell the world we'll take some time...
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	//	TODO: Actually load the file...
	bool bResult = false;

	// We're formerly done.
	QApplication::restoreOverrideCursor();

	// Save as default session directory.
	if (m_pOptions)
		m_pOptions->sSessionDir = QFileInfo(sFilename).absolutePath();

	stabilizeForm();

	return bResult;
}


// Save current session to specific file path.
bool qxgeditMainForm::saveSessionFile (	const QString& sFilename )
{
	// Tell the world we'll take some time...
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	//	TODO: Actually save the file...
	bool bResult = false;

	// We're formerly done.
	QApplication::restoreOverrideCursor();

	// Save as default session directory.
	if (m_pOptions)
		m_pOptions->sSessionDir = QFileInfo(sFilename).absolutePath();

	stabilizeForm();

	return bResult;
}


//-------------------------------------------------------------------------
// qxgeditMainForm -- File Action slots.

// Create a new sampler session.
void qxgeditMainForm::fileNew (void)
{
	// Of course we'll start clean new.
	newSession();
}


// Open an existing sampler session.
void qxgeditMainForm::fileOpen (void)
{
	// Open it right away.
	openSession();
}


// Open a recent file session.
void qxgeditMainForm::fileOpenRecent (void)
{
	// Retrive filename index from action data...
	QAction *pAction = qobject_cast<QAction *> (sender());
	if (pAction && m_pOptions) {
		int iIndex = pAction->data().toInt();
		if (iIndex >= 0 && iIndex < m_pOptions->recentFiles.count()) {
			QString sFilename = m_pOptions->recentFiles[iIndex];
			// Check if we can safely close the current session...
			if (!sFilename.isEmpty() && closeSession())
				loadSessionFile(sFilename);
		}
	}
}


// Save current sampler session.
void qxgeditMainForm::fileSave (void)
{
	// Save it right away.
	saveSession(false);
}


// Save current sampler session with another name.
void qxgeditMainForm::fileSaveAs (void)
{
	// Save it right away, maybe with another name.
	saveSession(true);
}


// Exit application program.
void qxgeditMainForm::fileExit (void)
{
	// Go for close the whole thing.
	close();
}


//-------------------------------------------------------------------------
// qxgeditMainForm -- View Action slots.

// Show/hide the main program window menubar.
void qxgeditMainForm::viewMenubar ( bool bOn )
{
	m_ui.MenuBar->setVisible(bOn);
}


// Show/hide the main program window statusbar.
void qxgeditMainForm::viewStatusbar ( bool bOn )
{
	statusBar()->setVisible(bOn);
}


// Show/hide the file-toolbar.
void qxgeditMainForm::viewToolbar ( bool bOn )
{
	m_ui.ToolBar->setVisible(bOn);
}


// Show options dialog.
void qxgeditMainForm::viewOptions (void)
{
	if (m_pOptions == NULL)
		return;

	// Load the current setup settings.
	bool bOldCompletePath   = m_pOptions->bCompletePath;
	int  iOldMaxRecentFiles = m_pOptions->iMaxRecentFiles;
	int  iOldBaseFontSize   = m_pOptions->iBaseFontSize;
	// Load the current setup settings.
	qxgeditOptionsForm optionsForm(this);
	optionsForm.setOptions(m_pOptions);
	if (optionsForm.exec()) {
		enum { RestartSession = 1, RestartProgram = 2, RestartAny = 3 };
		// Check wheather something immediate has changed.
		int iNeedRestart = 0;
		if (iOldBaseFontSize != m_pOptions->iBaseFontSize)
			iNeedRestart |= RestartProgram;
		if (( bOldCompletePath && !m_pOptions->bCompletePath) ||
			(!bOldCompletePath &&  m_pOptions->bCompletePath) ||
			(iOldMaxRecentFiles != m_pOptions->iMaxRecentFiles))
			updateRecentFilesMenu();
		// Warn if something will be only effective on next time.
		if (iNeedRestart & RestartAny) {
			QString sNeedRestart;
			if (iNeedRestart & RestartSession)
				sNeedRestart += tr("session");
			if (iNeedRestart & RestartProgram) {
				if (!sNeedRestart.isEmpty())
					sNeedRestart += tr(" or ");
				sNeedRestart += tr("program");
			}
			// Show restart needed message...
			QMessageBox::information(this,
				tr("Information") + " - " QXGEDIT_TITLE,
				tr("Some settings may be only effective\n"
				"next time you start this %1.")
				.arg(sNeedRestart));
		}
	}

	// This makes it.
	stabilizeForm();
}


//-------------------------------------------------------------------------
// qxgeditMainForm -- Help Action slots.

// Show information about application program.
void qxgeditMainForm::helpAbout (void)
{
	// Stuff the about box text...
	QString sText = "<p>\n";
	sText += "<b>" QXGEDIT_TITLE " - " + tr(QXGEDIT_SUBTITLE) + "</b><br />\n";
	sText += "<br />\n";
	sText += tr("Version") + ": <b>" QXGEDIT_VERSION "</b><br />\n";
	sText += "<small>" + tr("Build") + ": " __DATE__ " " __TIME__ "</small><br />\n";
#ifdef CONFIG_DEBUG
	sText += "<small><font color=\"red\">";
	sText += tr("Debugging option enabled.");
	sText += "</font></small><br />";
#endif
	sText += "<br />\n";
	sText += tr("Website") + ": <a href=\"" QXGEDIT_WEBSITE "\">" QXGEDIT_WEBSITE "</a><br />\n";
	sText += "<br />\n";
	sText += "<small>";
	sText += QXGEDIT_COPYRIGHT "<br />\n";
	sText += "<br />\n";
	sText += tr("This program is free software; you can redistribute it and/or modify it") + "<br />\n";
	sText += tr("under the terms of the GNU General Public License version 2 or later.");
	sText += "</small>";
	sText += "</p>\n";

	QMessageBox::about(this, tr("About") + " " QXGEDIT_TITLE, sText);
}


// Show information about the Qt toolkit.
void qxgeditMainForm::helpAboutQt (void)
{
	QMessageBox::aboutQt(this);
}


//-------------------------------------------------------------------------
// qxgeditMainForm -- Main window stabilization.

void qxgeditMainForm::stabilizeForm (void)
{
	// Update the main application caption...
	QString sSessionName = sessionName(m_sFilename);
	if (m_iDirtyCount > 0)
		sSessionName += ' ' + tr("[modified]");
	setWindowTitle(sSessionName + " - " QXGEDIT_TITLE);

	// Update the main menu state...
	m_ui.fileSaveAction->setEnabled(m_iDirtyCount > 0);

	// Recent files menu.
	m_ui.fileOpenRecentMenu->setEnabled(m_pOptions->recentFiles.count() > 0);

	m_statusItems[StatusName]->setText(sessionName(m_sFilename));

	if (m_iDirtyCount > 0)
		m_statusItems[StatusMod]->setText(tr("MOD"));
	else
		m_statusItems[StatusMod]->clear();
}


// Update the recent files list and menu.
void qxgeditMainForm::updateRecentFiles ( const QString& sFilename )
{
	if (m_pOptions == NULL)
		return;

	// Remove from list if already there (avoid duplicates)
	int iIndex = m_pOptions->recentFiles.indexOf(sFilename);
	if (iIndex >= 0)
		m_pOptions->recentFiles.removeAt(iIndex);
	// Put it to front...
	m_pOptions->recentFiles.push_front(sFilename);
}


// Update the recent files list and menu.
void qxgeditMainForm::updateRecentFilesMenu (void)
{
	if (m_pOptions == NULL)
		return;

	// Time to keep the list under limits.
	int iRecentFiles = m_pOptions->recentFiles.count();
	while (iRecentFiles > m_pOptions->iMaxRecentFiles) {
		m_pOptions->recentFiles.pop_back();
		iRecentFiles--;
	}

	// Rebuild the recent files menu...
	m_ui.fileOpenRecentMenu->clear();
	for (int i = 0; i < iRecentFiles; i++) {
		const QString& sFilename = m_pOptions->recentFiles[i];
		if (QFileInfo(sFilename).exists()) {
			QAction *pAction = m_ui.fileOpenRecentMenu->addAction(
				QString("&%1 %2").arg(i + 1).arg(sessionName(sFilename)),
				this, SLOT(fileOpenRecent()));
			pAction->setData(i);
		}
	}
}


//-------------------------------------------------------------------------
// qxgeditMainForm -- Messages window form handlers.

// Messages output methods.
void qxgeditMainForm::showMessage( const QString& s )
{
	statusBar()->showMessage(s, 3000);
}

void qxgeditMainForm::showMessageError( const QString& s )
{
	QMessageBox::critical(this,
		tr("Error") + " - " QXGEDIT_TITLE, s, QMessageBox::Ok);
}


// end of qxgeditMainForm.cpp
