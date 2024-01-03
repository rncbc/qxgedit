// qxgeditMainForm.cpp
//
/****************************************************************************
   Copyright (C) 2005-2024, rncbc aka Rui Nuno Capela. All rights reserved.

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

#include "qxgeditXGMasterMap.h"
#include "qxgeditMidiDevice.h"

#include "XGParamSysex.h"

#include "qxgeditDial.h"
#include "qxgeditCombo.h"

#include "qxgeditOptionsForm.h"
#include "qxgeditPaletteForm.h"

#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QUrl>

#include <QTreeWidget>
#include <QHeaderView>

#include <QStatusBar>
#include <QLabel>

#include <QDragEnterEvent>
#include <QCloseEvent>
#include <QDropEvent>

#include <QStyleFactory>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QMimeData>
#endif


//-------------------------------------------------------------------------
// LADISH Level 1 support stuff.

#ifdef HAVE_SIGNAL_H

#include <QSocketNotifier>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

// File descriptor for SIGUSR1 notifier.
static int g_fdSigusr1[2] = { -1, -1 };

// Unix SIGUSR1 signal handler.
static void qxgedit_sigusr1_handler ( int /* signo */ )
{
	char c = 1;

	(void) (::write(g_fdSigusr1[0], &c, sizeof(c)) > 0);
}

// File descriptor for SIGTERM notifier.
static int g_fdSigterm[2] = { -1, -1 };

// Unix SIGTERM signal handler.
static void qxgedit_sigterm_handler ( int /* signo */ )
{
	char c = 1;

	(void) (::write(g_fdSigterm[0], &c, sizeof(c)) > 0);
}

#endif	// HAVE_SIGNAL_H


//-------------------------------------------------------------------------
// qxgeditMainForm -- UI wrapper form.
//
qxgeditMainForm *qxgeditMainForm::g_pMainForm = nullptr;

// Constructor.
qxgeditMainForm::qxgeditMainForm ( QWidget *pParent )
	: QMainWindow(pParent)
{
	// Setup UI struct...
	m_ui.setupUi(this);
#if QT_VERSION < QT_VERSION_CHECK(6, 1, 0)
	QMainWindow::setWindowIcon(QIcon(":/images/qxgedit.png"));
#endif
	// Pseudo-singleton reference setup.
	g_pMainForm = this;

	// Initialize some pointer references.
	m_pOptions = nullptr;
	m_pMidiDevice = nullptr;
	m_pMasterMap = nullptr;

	// We'll start clean.
	m_iUntitled   = 0;
	m_iDirtyCount = 0;

	// Instrument/Normal Voice combo-box view soft-mutex.
	m_iMultipartVoiceUpdate = 0;

	// Uservoice element combo-box soft-mutex.
	m_iUservoiceElementUpdate = 0;

#ifdef HAVE_SIGNAL_H

	// Set to ignore any fatal "Broken pipe" signals.
	::signal(SIGPIPE, SIG_IGN);

	// LADISH Level 1 suport.
	
	// Initialize file descriptors for SIGUSR1 socket notifier.
	::socketpair(AF_UNIX, SOCK_STREAM, 0, g_fdSigusr1);
	m_pSigusr1Notifier
		= new QSocketNotifier(g_fdSigusr1[1], QSocketNotifier::Read, this);

	QObject::connect(m_pSigusr1Notifier,
		SIGNAL(activated(int)),
		SLOT(handle_sigusr1()));

	// Install SIGUSR1 signal handler.
	struct sigaction sigusr1;
	sigusr1.sa_handler = qxgedit_sigusr1_handler;
	sigemptyset(&sigusr1.sa_mask);
	sigusr1.sa_flags = 0;
	sigusr1.sa_flags |= SA_RESTART;
	::sigaction(SIGUSR1, &sigusr1, nullptr);

	// Initialize file descriptors for SIGTERM socket notifier.
	::socketpair(AF_UNIX, SOCK_STREAM, 0, g_fdSigterm);
	m_pSigtermNotifier
		= new QSocketNotifier(g_fdSigterm[1], QSocketNotifier::Read, this);

	QObject::connect(m_pSigtermNotifier,
		SIGNAL(activated(int)),
		SLOT(handle_sigterm()));

	// Install SIGTERM signal handler.
	struct sigaction sigterm;
	sigterm.sa_handler = qxgedit_sigterm_handler;
	sigemptyset(&sigterm.sa_mask);
	sigterm.sa_flags = 0;
	sigterm.sa_flags |= SA_RESTART;
	::sigaction(SIGTERM, &sigterm, nullptr);
	::sigaction(SIGQUIT, &sigterm, nullptr);

	// Ignore SIGHUP/SIGINT signals.
	::signal(SIGHUP, SIG_IGN);
	::signal(SIGINT, SIG_IGN);

#else	// HAVE_SIGNAL_H

	m_pSigusr1Notifier = nullptr;
	m_pSigtermNotifier = nullptr;
	
#endif	// !HAVE_SIGNAL_H

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
	QObject::connect(m_ui.viewRandomizeAction,
		SIGNAL(triggered(bool)),
		SLOT(viewRandomize()));
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

	QObject::connect(m_ui.MainTabWidget,
		SIGNAL(currentChanged(int)),
		SLOT(stabilizeForm()));
	QObject::connect(m_ui.SystemEffectToolBox,
		SIGNAL(currentChanged(int)),
		SLOT(stabilizeForm()));
}


// Destructor.
qxgeditMainForm::~qxgeditMainForm (void)
{
#ifdef HAVE_SIGNAL_H
	if (m_pSigusr1Notifier)
		delete m_pSigusr1Notifier;
	if (m_pSigtermNotifier)
		delete m_pSigtermNotifier;
#endif

	// Free designated devices.
	if (m_pMidiDevice)
		delete m_pMidiDevice;
	if (m_pMasterMap)
		delete m_pMasterMap;

	// Pseudo-singleton reference shut-down.
	g_pMainForm = nullptr;
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
	m_pOptions->loadWidgetGeometry(this, true);

	// Primary startup stabilization...
	updateRecentFilesMenu();

	// XG master database...
	m_pMasterMap = new qxgeditXGMasterMap();
	m_pMasterMap->set_auto_send(m_pOptions->bUservoiceAutoSend);

	// Start proper devices...
	m_pMidiDevice = new qxgeditMidiDevice(QXGEDIT_TITLE);

	QObject::connect(m_pMidiDevice,
		SIGNAL(receiveSysex(const QByteArray&)),
		SLOT(sysexReceived(const QByteArray&)));
	QObject::connect(m_pMidiDevice,
		SIGNAL(receiveRpn(unsigned char, unsigned short, unsigned short)),
		SLOT(rpnReceived(unsigned char, unsigned short, unsigned short)));
	QObject::connect(m_pMidiDevice,
		SIGNAL(receiveNrpn(unsigned char, unsigned short, unsigned short)),
		SLOT(nrpnReceived(unsigned char, unsigned short, unsigned short)));

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

	XGParamMap *SYSTEM    = &(m_pMasterMap->SYSTEM);
	XGParamMap *REVERB    = &(m_pMasterMap->REVERB);
	XGParamMap *CHORUS    = &(m_pMasterMap->CHORUS);
	XGParamMap *VARIATION = &(m_pMasterMap->VARIATION);
	XGParamMap *MULTIPART = &(m_pMasterMap->MULTIPART);
	XGParamMap *DRUMSETUP = &(m_pMasterMap->DRUMSETUP);
	XGParamMap *USERVOICE = &(m_pMasterMap->USERVOICE);

	// SYSTEM...
	QObject::connect(m_ui.MasterResetButton,
		SIGNAL(clicked()),
		SLOT(masterResetButtonClicked()));

	// SYSTEM widget mapping...
	m_ui.MasterTuneDial            -> set_param_map(SYSTEM, 0x00);
	m_ui.MasterVolumeDial          -> set_param_map(SYSTEM, 0x04);
	m_ui.MasterTransposeDial       -> set_param_map(SYSTEM, 0x06);

	// REVERB...
	QObject::connect(m_ui.ReverbResetButton,
		SIGNAL(clicked()),
		SLOT(reverbResetButtonClicked()));

	// REVERB widget mapping...
	m_ui.ReverbTypeCombo           -> set_param_map(REVERB, 0x00);
	m_ui.ReverbParam1Dial          -> set_param_map(REVERB, 0x02);
	m_ui.ReverbParam2Dial          -> set_param_map(REVERB, 0x03);
	m_ui.ReverbParam3Dial          -> set_param_map(REVERB, 0x04);
	m_ui.ReverbParam4Dial          -> set_param_map(REVERB, 0x05);
	m_ui.ReverbParam5Dial          -> set_param_map(REVERB, 0x06);
	m_ui.ReverbParam6Dial          -> set_param_map(REVERB, 0x07);
	m_ui.ReverbParam7Dial          -> set_param_map(REVERB, 0x08);
	m_ui.ReverbParam8Dial          -> set_param_map(REVERB, 0x09);
	m_ui.ReverbParam9Dial          -> set_param_map(REVERB, 0x0a);
	m_ui.ReverbParam10Dial         -> set_param_map(REVERB, 0x0b);
	m_ui.ReverbReturnDial          -> set_param_map(REVERB, 0x0c);
	m_ui.ReverbPanDial             -> set_param_map(REVERB, 0x0d);
	m_ui.ReverbParam11Dial         -> set_param_map(REVERB, 0x10);
	m_ui.ReverbParam12Dial         -> set_param_map(REVERB, 0x11);
	m_ui.ReverbParam13Dial         -> set_param_map(REVERB, 0x12);
	m_ui.ReverbParam14Dial         -> set_param_map(REVERB, 0x13);
	m_ui.ReverbParam15Dial         -> set_param_map(REVERB, 0x14);
	m_ui.ReverbParam16Dial         -> set_param_map(REVERB, 0x15);

	// CHORUS...
	QObject::connect(m_ui.ChorusResetButton,
		SIGNAL(clicked()),
		SLOT(chorusResetButtonClicked()));

	// CHORUS widget mapping...
	m_ui.ChorusTypeCombo           -> set_param_map(CHORUS, 0x20);
	m_ui.ChorusParam1Dial          -> set_param_map(CHORUS, 0x22);
	m_ui.ChorusParam2Dial          -> set_param_map(CHORUS, 0x23);
	m_ui.ChorusParam3Dial          -> set_param_map(CHORUS, 0x24);
	m_ui.ChorusParam4Dial          -> set_param_map(CHORUS, 0x25);
	m_ui.ChorusParam5Dial          -> set_param_map(CHORUS, 0x26);
	m_ui.ChorusParam6Dial          -> set_param_map(CHORUS, 0x27);
	m_ui.ChorusParam7Dial          -> set_param_map(CHORUS, 0x28);
	m_ui.ChorusParam8Dial          -> set_param_map(CHORUS, 0x29);
	m_ui.ChorusParam9Dial          -> set_param_map(CHORUS, 0x2a);
	m_ui.ChorusParam10Dial         -> set_param_map(CHORUS, 0x2b);
	m_ui.ChorusReturnDial          -> set_param_map(CHORUS, 0x2c);
	m_ui.ChorusPanDial             -> set_param_map(CHORUS, 0x2d);
	m_ui.ChorusReverbDial          -> set_param_map(CHORUS, 0x2e);
	m_ui.ChorusParam11Dial         -> set_param_map(CHORUS, 0x30);
	m_ui.ChorusParam12Dial         -> set_param_map(CHORUS, 0x31);
	m_ui.ChorusParam13Dial         -> set_param_map(CHORUS, 0x32);
	m_ui.ChorusParam14Dial         -> set_param_map(CHORUS, 0x33);
	m_ui.ChorusParam15Dial         -> set_param_map(CHORUS, 0x34);
	m_ui.ChorusParam16Dial         -> set_param_map(CHORUS, 0x35);

	// VARIATION...
	QObject::connect(m_ui.VariationResetButton,
		SIGNAL(clicked()),
		SLOT(variationResetButtonClicked()));

	// VARIATION widget mapping...
	m_ui.VariationTypeCombo        -> set_param_map(VARIATION, 0x40);
	m_ui.VariationParam1Dial       -> set_param_map(VARIATION, 0x42);
	m_ui.VariationParam2Dial       -> set_param_map(VARIATION, 0x44);
	m_ui.VariationParam3Dial       -> set_param_map(VARIATION, 0x46);
	m_ui.VariationParam4Dial       -> set_param_map(VARIATION, 0x48);
	m_ui.VariationParam5Dial       -> set_param_map(VARIATION, 0x4a);
	m_ui.VariationParam6Dial       -> set_param_map(VARIATION, 0x4c);
	m_ui.VariationParam7Dial       -> set_param_map(VARIATION, 0x4e);
	m_ui.VariationParam8Dial       -> set_param_map(VARIATION, 0x50);
	m_ui.VariationParam9Dial       -> set_param_map(VARIATION, 0x52);
	m_ui.VariationParam10Dial      -> set_param_map(VARIATION, 0x54);
	m_ui.VariationReturnDial       -> set_param_map(VARIATION, 0x56);
	m_ui.VariationPanDial          -> set_param_map(VARIATION, 0x57);
	m_ui.VariationReverbDial       -> set_param_map(VARIATION, 0x58);
	m_ui.VariationChorusDial       -> set_param_map(VARIATION, 0x59);
	m_ui.VariationConnectDial      -> set_param_map(VARIATION, 0x5a);
	m_ui.VariationPartDial         -> set_param_map(VARIATION, 0x5b);
	m_ui.VariationWheelDial        -> set_param_map(VARIATION, 0x5c);
	m_ui.VariationBendDial         -> set_param_map(VARIATION, 0x5d);
	m_ui.VariationCATDial          -> set_param_map(VARIATION, 0x5e);
	m_ui.VariationAC1Dial          -> set_param_map(VARIATION, 0x5f);
	m_ui.VariationAC2Dial          -> set_param_map(VARIATION, 0x60);
	m_ui.VariationParam11Dial      -> set_param_map(VARIATION, 0x70);
	m_ui.VariationParam12Dial      -> set_param_map(VARIATION, 0x71);
	m_ui.VariationParam13Dial      -> set_param_map(VARIATION, 0x72);
	m_ui.VariationParam14Dial      -> set_param_map(VARIATION, 0x73);
	m_ui.VariationParam15Dial      -> set_param_map(VARIATION, 0x74);
	m_ui.VariationParam16Dial      -> set_param_map(VARIATION, 0x75);

	// MULTIPART...
	m_ui.MultipartCombo->setMaxVisibleItems(16);
	m_ui.MultipartCombo->clear();
	for (int iPart = 0; iPart < 16; ++iPart)
		m_ui.MultipartCombo->addItem(tr("Part %1").arg(iPart + 1));

	// MULTIPART Instrument model/view combo-box...
	QTreeWidget *pMultipartVoiceListView = new QTreeWidget();
	pMultipartVoiceListView->header()->hide();
	pMultipartVoiceListView->setRootIsDecorated(true);
	pMultipartVoiceListView->setAllColumnsShowFocus(true);
	pMultipartVoiceListView->setUniformRowHeights(true);
	m_ui.MultipartVoiceCombo->setModel(pMultipartVoiceListView->model());
	m_ui.MultipartVoiceCombo->setView(pMultipartVoiceListView);
	m_ui.MultipartVoiceCombo->setMaxVisibleItems(16);
	m_ui.MultipartVoiceCombo->setMinimumContentsLength(24);
	m_ui.MultipartVoiceCombo->clear();

	QList<QTreeWidgetItem *> items;
	QTreeWidgetItem *pInstrumentItem;
	for (unsigned short i = 0; i < XGInstrument::count(); ++i) {
		XGInstrument instr(i);
		pInstrumentItem = new QTreeWidgetItem(pMultipartVoiceListView);
		pInstrumentItem->setFlags(Qt::ItemIsEnabled);
		pInstrumentItem->setText(0, instr.name());
		for (unsigned short j = 0; j < instr.size(); ++j) {
			XGNormalVoice voice(&instr, j);
			QTreeWidgetItem *pVoiceItem = new QTreeWidgetItem(pInstrumentItem);
			pVoiceItem->setText(0, voice.name());
		}
		items.append(pInstrumentItem);
	}
	pInstrumentItem = new QTreeWidgetItem(pMultipartVoiceListView);
	pInstrumentItem->setFlags(Qt::ItemIsEnabled);
	pInstrumentItem->setText(0, "Drums");
	for (unsigned short k = 0; k < XGDrumKit::count(); ++k) {
		XGDrumKit drumkit(k);
		QTreeWidgetItem *pVoiceItem = new QTreeWidgetItem(pInstrumentItem);
		pVoiceItem->setText(0, drumkit.name());
	}
	pInstrumentItem = new QTreeWidgetItem(pMultipartVoiceListView);
	pInstrumentItem->setFlags(Qt::ItemIsEnabled);
	pInstrumentItem->setText(0, "QS300 User");
	for (unsigned short iUser = 0; iUser < 32; ++iUser) {
		QTreeWidgetItem *pVoiceItem = new QTreeWidgetItem(pInstrumentItem);
		pVoiceItem->setText(0, QString("QS300 User %1").arg(iUser + 1));
	}
	pMultipartVoiceListView->addTopLevelItems(items);

	// MULTIPART Special values...
	m_ui.MultipartPanDial->setSpecialValueText(tr("Random"));

	QObject::connect(m_ui.MultipartResetButton,
		SIGNAL(clicked()),
		SLOT(multipartResetButtonClicked()));
	QObject::connect(m_ui.MultipartCombo,
		SIGNAL(activated(int)),
		SLOT(multipartComboActivated(int)));
	QObject::connect(m_ui.MultipartVoiceCombo,
		SIGNAL(activated(int)),
		SLOT(multipartVoiceComboActivated(int)));

	QObject::connect(m_ui.MultipartBankMSBDial,
		SIGNAL(valueChanged(unsigned short)),
		SLOT(multipartVoiceChanged()));
	QObject::connect(m_ui.MultipartBankLSBDial,
		SIGNAL(valueChanged(unsigned short)),
		SLOT(multipartVoiceChanged()));
	QObject::connect(m_ui.MultipartProgramDial,
		SIGNAL(valueChanged(unsigned short)),
		SLOT(multipartVoiceChanged()));
	QObject::connect(m_ui.MultipartPartModeDial,
		SIGNAL(valueChanged(unsigned short)),
		SLOT(multipartPartModeChanged(unsigned short)));

	// MULTIPART AmpEg...
	QObject::connect(
		m_ui.MultipartAmpEg, SIGNAL(attackChanged(unsigned short)),
		m_ui.MultipartAttackDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartAttackDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartAmpEg, SLOT(setAttack(unsigned short)));
	QObject::connect(
		m_ui.MultipartAmpEg, SIGNAL(decayChanged(unsigned short)),
		m_ui.MultipartDecayDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartDecayDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartAmpEg, SLOT(setDecay(unsigned short)));
	QObject::connect(
		m_ui.MultipartAmpEg, SIGNAL(releaseChanged(unsigned short)),
		m_ui.MultipartReleaseDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartReleaseDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartAmpEg, SLOT(setRelease(unsigned short)));

	// MULTIPART Filter...
	QObject::connect(
		m_ui.MultipartFilter, SIGNAL(cutoffChanged(unsigned short)),
		m_ui.MultipartCutoffDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartCutoffDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartFilter, SLOT(setCutoff(unsigned short)));
	QObject::connect(
		m_ui.MultipartFilter, SIGNAL(resonanceChanged(unsigned short)),
		m_ui.MultipartResonanceDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartResonanceDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartFilter, SLOT(setResonance(unsigned short)));

	// MULTIPART Pitch...
	QObject::connect(
		m_ui.MultipartPitch, SIGNAL(attackTimeChanged(unsigned short)),
		m_ui.MultipartAttackTimeDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartAttackTimeDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartPitch, SLOT(setAttackTime(unsigned short)));
	QObject::connect(
		m_ui.MultipartPitch, SIGNAL(attackLevelChanged(unsigned short)),
		m_ui.MultipartAttackLevelDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartAttackLevelDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartPitch, SLOT(setAttackLevel(unsigned short)));
	QObject::connect(
		m_ui.MultipartPitch, SIGNAL(releaseTimeChanged(unsigned short)),
		m_ui.MultipartReleaseTimeDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartReleaseTimeDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartPitch, SLOT(setReleaseTime(unsigned short)));
	QObject::connect(
		m_ui.MultipartPitch, SIGNAL(releaseLevelChanged(unsigned short)),
		m_ui.MultipartReleaseLevelDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartReleaseLevelDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartPitch, SLOT(setReleaseLevel(unsigned short)));

	// MULTIPART Vibrato...
	QObject::connect(
		m_ui.MultipartVibra, SIGNAL(delayChanged(unsigned short)),
		m_ui.MultipartDelayDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartDelayDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartVibra, SLOT(setDelay(unsigned short)));
	QObject::connect(
		m_ui.MultipartVibra, SIGNAL(rateChanged(unsigned short)),
		m_ui.MultipartRateDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartRateDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartVibra, SLOT(setRate(unsigned short)));
	QObject::connect(
		m_ui.MultipartVibra, SIGNAL(depthChanged(unsigned short)),
		m_ui.MultipartDepthDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.MultipartDepthDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.MultipartVibra, SLOT(setDepth(unsigned short)));

	// MULTIPART widget mapping...
	m_ui.MultipartElementDial      -> set_param_map(MULTIPART, 0x00);
	m_ui.MultipartBankMSBDial      -> set_param_map(MULTIPART, 0x01);
	m_ui.MultipartBankLSBDial      -> set_param_map(MULTIPART, 0x02);
	m_ui.MultipartProgramDial      -> set_param_map(MULTIPART, 0x03);
	m_ui.MultipartChannelDial      -> set_param_map(MULTIPART, 0x04);
	m_ui.MultipartPolyModeDial     -> set_param_map(MULTIPART, 0x05);
	m_ui.MultipartKeyAssignDial    -> set_param_map(MULTIPART, 0x06);
	m_ui.MultipartPartModeDial     -> set_param_map(MULTIPART, 0x07);
	m_ui.MultipartNoteShiftDial    -> set_param_map(MULTIPART, 0x08);
	m_ui.MultipartDetuneDial       -> set_param_map(MULTIPART, 0x09);
	m_ui.MultipartVolumeDial       -> set_param_map(MULTIPART, 0x0b);
	m_ui.MultipartVelDepthDial     -> set_param_map(MULTIPART, 0x0c);
	m_ui.MultipartVelOffsetDial    -> set_param_map(MULTIPART, 0x0d);
	m_ui.MultipartPanDial          -> set_param_map(MULTIPART, 0x0e);
	m_ui.MultipartNoteLowDial      -> set_param_map(MULTIPART, 0x0f);
	m_ui.MultipartNoteHighDial     -> set_param_map(MULTIPART, 0x10);
	m_ui.MultipartDryWetDial       -> set_param_map(MULTIPART, 0x11);
	m_ui.MultipartChorusDial       -> set_param_map(MULTIPART, 0x12);
	m_ui.MultipartReverbDial       -> set_param_map(MULTIPART, 0x13);
	m_ui.MultipartVariationDial    -> set_param_map(MULTIPART, 0x14);
	m_ui.MultipartRateDial         -> set_param_map(MULTIPART, 0x15);
	m_ui.MultipartDepthDial        -> set_param_map(MULTIPART, 0x16);
	m_ui.MultipartDelayDial        -> set_param_map(MULTIPART, 0x17);
	m_ui.MultipartCutoffDial       -> set_param_map(MULTIPART, 0x18);
	m_ui.MultipartResonanceDial    -> set_param_map(MULTIPART, 0x19);
	m_ui.MultipartAttackDial       -> set_param_map(MULTIPART, 0x1a);
	m_ui.MultipartDecayDial        -> set_param_map(MULTIPART, 0x1b);
	m_ui.MultipartReleaseDial      -> set_param_map(MULTIPART, 0x1c);
	m_ui.MultipartWheelPitchDial   -> set_param_map(MULTIPART, 0x1d);
	m_ui.MultipartWheelFilterDial  -> set_param_map(MULTIPART, 0x1e);
	m_ui.MultipartWheelAmplDial    -> set_param_map(MULTIPART, 0x1f);
	m_ui.MultipartWheelLFOPmodDial -> set_param_map(MULTIPART, 0x20);
	m_ui.MultipartWheelLFOFmodDial -> set_param_map(MULTIPART, 0x21);
	m_ui.MultipartWheelLFOAmodDial -> set_param_map(MULTIPART, 0x22);
	m_ui.MultipartBendPitchDial    -> set_param_map(MULTIPART, 0x23);
	m_ui.MultipartBendFilterDial   -> set_param_map(MULTIPART, 0x24);
	m_ui.MultipartBendAmplDial     -> set_param_map(MULTIPART, 0x25);
	m_ui.MultipartBendLFOPmodDial  -> set_param_map(MULTIPART, 0x26);
	m_ui.MultipartBendLFOFmodDial  -> set_param_map(MULTIPART, 0x27);
	m_ui.MultipartBendLFOAmodDial  -> set_param_map(MULTIPART, 0x28);
	m_ui.MultipartPBCheck          -> set_param_map(MULTIPART, 0x30);
	m_ui.MultipartCATCheck         -> set_param_map(MULTIPART, 0x31);
	m_ui.MultipartPCCheck          -> set_param_map(MULTIPART, 0x32);
	m_ui.MultipartCCCheck          -> set_param_map(MULTIPART, 0x33);
	m_ui.MultipartPATCheck         -> set_param_map(MULTIPART, 0x34);
	m_ui.MultipartNoteCheck        -> set_param_map(MULTIPART, 0x35);
	m_ui.MultipartRPNCheck         -> set_param_map(MULTIPART, 0x36);
	m_ui.MultipartNRPNCheck        -> set_param_map(MULTIPART, 0x37);
	m_ui.MultipartModCheck         -> set_param_map(MULTIPART, 0x38);
	m_ui.MultipartVolCheck         -> set_param_map(MULTIPART, 0x39);
	m_ui.MultipartPanCheck         -> set_param_map(MULTIPART, 0x3a);
	m_ui.MultipartExprCheck        -> set_param_map(MULTIPART, 0x3b);
	m_ui.MultipartHold1Check       -> set_param_map(MULTIPART, 0x3c);
	m_ui.MultipartPortaCheck       -> set_param_map(MULTIPART, 0x3d);
	m_ui.MultipartSostCheck        -> set_param_map(MULTIPART, 0x3e);
	m_ui.MultipartPedalCheck       -> set_param_map(MULTIPART, 0x3f);
	m_ui.MultipartBankCheck        -> set_param_map(MULTIPART, 0x40);
	m_ui.MultipartTuningC_Dial     -> set_param_map(MULTIPART, 0x41);
	m_ui.MultipartTuningCsDial     -> set_param_map(MULTIPART, 0x42);
	m_ui.MultipartTuningD_Dial     -> set_param_map(MULTIPART, 0x43);
	m_ui.MultipartTuningDsDial     -> set_param_map(MULTIPART, 0x44);
	m_ui.MultipartTuningE_Dial     -> set_param_map(MULTIPART, 0x45);
	m_ui.MultipartTuningF_Dial     -> set_param_map(MULTIPART, 0x46);
	m_ui.MultipartTuningFsDial     -> set_param_map(MULTIPART, 0x47);
	m_ui.MultipartTuningG_Dial     -> set_param_map(MULTIPART, 0x48);
	m_ui.MultipartTuningGsDial     -> set_param_map(MULTIPART, 0x49);
	m_ui.MultipartTuningA_Dial     -> set_param_map(MULTIPART, 0x4a);
	m_ui.MultipartTuningAsDial     -> set_param_map(MULTIPART, 0x4b);
	m_ui.MultipartTuningB_Dial     -> set_param_map(MULTIPART, 0x4c);
	m_ui.MultipartCATPitchDial     -> set_param_map(MULTIPART, 0x4d);
	m_ui.MultipartCATFilterDial    -> set_param_map(MULTIPART, 0x4e);
	m_ui.MultipartCATAmplDial      -> set_param_map(MULTIPART, 0x4f);
	m_ui.MultipartCATLFOPmodDial   -> set_param_map(MULTIPART, 0x50);
	m_ui.MultipartCATLFOFmodDial   -> set_param_map(MULTIPART, 0x51);
	m_ui.MultipartCATLFOAmodDial   -> set_param_map(MULTIPART, 0x52);
	m_ui.MultipartPATPitchDial     -> set_param_map(MULTIPART, 0x53);
	m_ui.MultipartPATFilterDial    -> set_param_map(MULTIPART, 0x54);
	m_ui.MultipartPATAmplDial      -> set_param_map(MULTIPART, 0x55);
	m_ui.MultipartPATLFOPmodDial   -> set_param_map(MULTIPART, 0x56);
	m_ui.MultipartPATLFOFmodDial   -> set_param_map(MULTIPART, 0x57);
	m_ui.MultipartPATLFOAmodDial   -> set_param_map(MULTIPART, 0x58);
	m_ui.MultipartAC1ControlDial   -> set_param_map(MULTIPART, 0x59);
	m_ui.MultipartAC1PitchDial     -> set_param_map(MULTIPART, 0x5a);
	m_ui.MultipartAC1FilterDial    -> set_param_map(MULTIPART, 0x5b);
	m_ui.MultipartAC1AmplDial      -> set_param_map(MULTIPART, 0x5c);
	m_ui.MultipartAC1LFOPmodDial   -> set_param_map(MULTIPART, 0x5d);
	m_ui.MultipartAC1LFOFmodDial   -> set_param_map(MULTIPART, 0x5e);
	m_ui.MultipartAC1LFOAmodDial   -> set_param_map(MULTIPART, 0x5f);
	m_ui.MultipartAC2ControlDial   -> set_param_map(MULTIPART, 0x60);
	m_ui.MultipartAC2PitchDial     -> set_param_map(MULTIPART, 0x61);
	m_ui.MultipartAC2FilterDial    -> set_param_map(MULTIPART, 0x62);
	m_ui.MultipartAC2AmplDial      -> set_param_map(MULTIPART, 0x63);
	m_ui.MultipartAC2LFOPmodDial   -> set_param_map(MULTIPART, 0x64);
	m_ui.MultipartAC2LFOFmodDial   -> set_param_map(MULTIPART, 0x65);
	m_ui.MultipartAC2LFOAmodDial   -> set_param_map(MULTIPART, 0x66);
	m_ui.MultipartPortamentoDial   -> set_param_map(MULTIPART, 0x67);
	m_ui.MultipartPortaTimeDial    -> set_param_map(MULTIPART, 0x68);
	m_ui.MultipartAttackLevelDial  -> set_param_map(MULTIPART, 0x69);
	m_ui.MultipartAttackTimeDial   -> set_param_map(MULTIPART, 0x6a);
	m_ui.MultipartReleaseLevelDial -> set_param_map(MULTIPART, 0x6b);
	m_ui.MultipartReleaseTimeDial  -> set_param_map(MULTIPART, 0x6c);
	m_ui.MultipartVelLowDial       -> set_param_map(MULTIPART, 0x6d);
	m_ui.MultipartVelHighDial      -> set_param_map(MULTIPART, 0x6e);

	// DRUMSETUP...
	m_ui.DrumsetupCombo->clear();
	for (int iDrumset = 0; iDrumset < 2; ++iDrumset)
		m_ui.DrumsetupCombo->addItem(tr("Drums %1").arg(iDrumset + 1));

	m_ui.DrumsetupVoiceCombo->setMaxVisibleItems(16);
	m_ui.DrumsetupVoiceCombo->clear();
	for (unsigned short k = 0; k < XGDrumKit::count(); ++k) {
		XGDrumKit drumkit(k);
		m_ui.DrumsetupVoiceCombo->addItem(drumkit.name());
	}

	m_ui.DrumsetupNoteCombo->setMaxVisibleItems(16);
	m_ui.DrumsetupNoteCombo->setMinimumContentsLength(24);
	drumsetupVoiceComboActivated(0);

	// DRUMSETUP Special values...
	m_ui.DrumsetupGroupDial->setSpecialValueText(tr("Off"));
	m_ui.DrumsetupPanDial->setSpecialValueText(tr("Random"));

	QObject::connect(m_ui.DrumsetupResetButton,
		SIGNAL(clicked()),
		SLOT(drumsetupResetButtonClicked()));
	QObject::connect(m_ui.DrumsetupCombo,
		SIGNAL(activated(int)),
		SLOT(drumsetupComboActivated(int)));
	QObject::connect(m_ui.DrumsetupVoiceCombo,
		SIGNAL(activated(int)),
		SLOT(drumsetupVoiceComboActivated(int)));
	QObject::connect(m_ui.DrumsetupNoteCombo,
		SIGNAL(activated(int)),
		SLOT(drumsetupNoteComboActivated(int)));

	// DRUMSETUP Filter...
	QObject::connect(
		m_ui.DrumsetupFilter, SIGNAL(cutoffChanged(unsigned short)),
		m_ui.DrumsetupCutoffDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.DrumsetupCutoffDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.DrumsetupFilter, SLOT(setCutoff(unsigned short)));
	QObject::connect(
		m_ui.DrumsetupFilter, SIGNAL(resonanceChanged(unsigned short)),
		m_ui.DrumsetupResonanceDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.DrumsetupResonanceDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.DrumsetupFilter, SLOT(setResonance(unsigned short)));

	// DRUMSETUP AmpEg...
	QObject::connect(
		m_ui.DrumsetupAmpEg, SIGNAL(attackChanged(unsigned short)),
		m_ui.DrumsetupAttackDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.DrumsetupAttackDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.DrumsetupAmpEg, SLOT(setAttack(unsigned short)));
	QObject::connect(
		m_ui.DrumsetupAmpEg, SIGNAL(decay1Changed(unsigned short)),
		m_ui.DrumsetupDecay1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.DrumsetupDecay1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.DrumsetupAmpEg, SLOT(setDecay1(unsigned short)));
	QObject::connect(
		m_ui.DrumsetupAmpEg, SIGNAL(decay2Changed(unsigned short)),
		m_ui.DrumsetupDecay2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.DrumsetupDecay2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.DrumsetupAmpEg, SLOT(setDecay2(unsigned short)));

	// DRUMSETUP widget mapping...
	m_ui.DrumsetupCoarseDial       -> set_param_map(DRUMSETUP, 0x00);
	m_ui.DrumsetupFineDial         -> set_param_map(DRUMSETUP, 0x01);
	m_ui.DrumsetupLevelDial        -> set_param_map(DRUMSETUP, 0x02);
	m_ui.DrumsetupGroupDial        -> set_param_map(DRUMSETUP, 0x03);
	m_ui.DrumsetupPanDial          -> set_param_map(DRUMSETUP, 0x04);
	m_ui.DrumsetupReverbDial       -> set_param_map(DRUMSETUP, 0x05);
	m_ui.DrumsetupChorusDial       -> set_param_map(DRUMSETUP, 0x06);
	m_ui.DrumsetupVariationDial    -> set_param_map(DRUMSETUP, 0x07);
	m_ui.DrumsetupKeyAssignDial    -> set_param_map(DRUMSETUP, 0x08);
	m_ui.DrumsetupNoteOffCheck     -> set_param_map(DRUMSETUP, 0x09);
	m_ui.DrumsetupNoteOnCheck      -> set_param_map(DRUMSETUP, 0x0a);
	m_ui.DrumsetupCutoffDial       -> set_param_map(DRUMSETUP, 0x0b);
	m_ui.DrumsetupResonanceDial    -> set_param_map(DRUMSETUP, 0x0c);
	m_ui.DrumsetupAttackDial       -> set_param_map(DRUMSETUP, 0x0d);
	m_ui.DrumsetupDecay1Dial       -> set_param_map(DRUMSETUP, 0x0e);
	m_ui.DrumsetupDecay2Dial       -> set_param_map(DRUMSETUP, 0x0f);

	// USERVOICE...
	m_ui.UservoiceCombo->setMaxVisibleItems(16);
	m_ui.UservoiceCombo->clear();
	for (int iUser = 0; iUser < 32; ++iUser)
		m_ui.UservoiceCombo->addItem(tr("QS300 User %1").arg(iUser + 1));

  	m_ui.UservoiceNameEdit->setMinimumWidth(200);

	m_ui.UservoiceElementCombo->clear();
	for (int iElem = 0; iElem < 2; ++iElem)
		m_ui.UservoiceElementCombo->addItem(tr("Element %1").arg(iElem + 1));

	m_ui.UservoiceAutoSendCheck->setChecked(m_pMasterMap->auto_send());

	QObject::connect(m_ui.UservoiceResetButton,
		SIGNAL(clicked()),
		SLOT(uservoiceResetButtonClicked()));
	QObject::connect(m_ui.UservoiceCombo,
		SIGNAL(activated(int)),
		SLOT(uservoiceComboActivated(int)));
	QObject::connect(m_ui.UservoiceElementCombo,
		SIGNAL(activated(int)),
		SLOT(uservoiceElementComboActivated(int)));
	QObject::connect(m_ui.UservoiceSendButton,
		SIGNAL(clicked()),
		SLOT(uservoiceSendButtonClicked()));
	QObject::connect(m_ui.UservoiceAutoSendCheck,
		SIGNAL(toggled(bool)),
		SLOT(uservoiceAutoSendCheckToggled(bool)));

	// USERVOICE Presets...
	QObject::connect(m_ui.UservoiceNameEdit,
		SIGNAL(loadPresetFile(const QString&)),
		SLOT(uservoiceLoadPresetFile(const QString&)));
	QObject::connect(m_ui.UservoiceNameEdit,
		SIGNAL(savePresetFile(const QString&)),
		SLOT(uservoiceSavePresetFile(const QString&)));

	// USERVOICE Elements...
	QObject::connect(m_ui.UservoiceElementDial,
		SIGNAL(valueChanged(unsigned short)),
		SLOT(uservoiceElementChanged(unsigned short)));

	// USERVOICE Pitch EG...
	QObject::connect(
		m_ui.UservoicePitchEg, SIGNAL(level0Changed(unsigned short)),
		m_ui.UservoicePEGLevel0Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoicePEGLevel0Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoicePitchEg, SLOT(setLevel0(unsigned short)));
	QObject::connect(
		m_ui.UservoicePitchEg, SIGNAL(level1Changed(unsigned short)),
		m_ui.UservoicePEGLevel1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoicePEGLevel1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoicePitchEg, SLOT(setLevel1(unsigned short)));
	QObject::connect(
		m_ui.UservoicePitchEg, SIGNAL(level2Changed(unsigned short)),
		m_ui.UservoicePEGLevel2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoicePEGLevel2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoicePitchEg, SLOT(setLevel2(unsigned short)));
	QObject::connect(
		m_ui.UservoicePitchEg, SIGNAL(level3Changed(unsigned short)),
		m_ui.UservoicePEGLevel3Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoicePEGLevel3Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoicePitchEg, SLOT(setLevel3(unsigned short)));
	QObject::connect(
		m_ui.UservoicePitchEg, SIGNAL(level4Changed(unsigned short)),
		m_ui.UservoicePEGLevel4Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoicePEGLevel4Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoicePitchEg, SLOT(setLevel4(unsigned short)));

	QObject::connect(
		m_ui.UservoicePitchEg, SIGNAL(rate1Changed(unsigned short)),
		m_ui.UservoicePEGRate1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoicePEGRate1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoicePitchEg, SLOT(setRate1(unsigned short)));
	QObject::connect(
		m_ui.UservoicePitchEg, SIGNAL(rate2Changed(unsigned short)),
		m_ui.UservoicePEGRate2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoicePEGRate2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoicePitchEg, SLOT(setRate2(unsigned short)));
	QObject::connect(
		m_ui.UservoicePitchEg, SIGNAL(rate3Changed(unsigned short)),
		m_ui.UservoicePEGRate3Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoicePEGRate3Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoicePitchEg, SLOT(setRate3(unsigned short)));
	QObject::connect(
		m_ui.UservoicePitchEg, SIGNAL(rate4Changed(unsigned short)),
		m_ui.UservoicePEGRate4Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoicePEGRate4Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoicePitchEg, SLOT(setRate4(unsigned short)));

	// USERVOICE Filter...
	m_ui.UservoiceFilter->setMaxResonance(63);
	QObject::connect(
		m_ui.UservoiceFilter, SIGNAL(cutoffChanged(unsigned short)),
		m_ui.UservoiceCutoffDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilter, SLOT(setCutoff(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFilter, SIGNAL(resonanceChanged(unsigned short)),
		m_ui.UservoiceResonanceDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceResonanceDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilter, SLOT(setResonance(unsigned short)));

	// USERVOICE Cutoff Scale...
	QObject::connect(
		m_ui.UservoiceCutoffScale, SIGNAL(break1Changed(unsigned short)),
		m_ui.UservoiceCutoffBreak1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffBreak1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceCutoffScale, SLOT(setBreak1(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffScale, SIGNAL(break2Changed(unsigned short)),
		m_ui.UservoiceCutoffBreak2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffBreak2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceCutoffScale, SLOT(setBreak2(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffScale, SIGNAL(break3Changed(unsigned short)),
		m_ui.UservoiceCutoffBreak3Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffBreak3Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceCutoffScale, SLOT(setBreak3(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffScale, SIGNAL(break4Changed(unsigned short)),
		m_ui.UservoiceCutoffBreak4Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffBreak4Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceCutoffScale, SLOT(setBreak4(unsigned short)));

	QObject::connect(
		m_ui.UservoiceCutoffScale, SIGNAL(offset1Changed(unsigned short)),
		m_ui.UservoiceCutoffOffset1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffOffset1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceCutoffScale, SLOT(setOffset1(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffScale, SIGNAL(offset2Changed(unsigned short)),
		m_ui.UservoiceCutoffOffset2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffOffset2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceCutoffScale, SLOT(setOffset2(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffScale, SIGNAL(offset3Changed(unsigned short)),
		m_ui.UservoiceCutoffOffset3Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffOffset3Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceCutoffScale, SLOT(setOffset3(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffScale, SIGNAL(offset4Changed(unsigned short)),
		m_ui.UservoiceCutoffOffset4Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceCutoffOffset4Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceCutoffScale, SLOT(setOffset4(unsigned short)));

	// USERVOICE Filter EG...
	QObject::connect(
		m_ui.UservoiceFilterEg, SIGNAL(level0Changed(unsigned short)),
		m_ui.UservoiceFEGLevel0Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFEGLevel0Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilterEg, SLOT(setLevel0(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFilterEg, SIGNAL(level1Changed(unsigned short)),
		m_ui.UservoiceFEGLevel1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFEGLevel1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilterEg, SLOT(setLevel1(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFilterEg, SIGNAL(level2Changed(unsigned short)),
		m_ui.UservoiceFEGLevel2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFEGLevel2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilterEg, SLOT(setLevel2(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFilterEg, SIGNAL(level3Changed(unsigned short)),
		m_ui.UservoiceFEGLevel3Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFEGLevel3Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilterEg, SLOT(setLevel3(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFilterEg, SIGNAL(level4Changed(unsigned short)),
		m_ui.UservoiceFEGLevel4Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFEGLevel4Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilterEg, SLOT(setLevel4(unsigned short)));

	QObject::connect(
		m_ui.UservoiceFilterEg, SIGNAL(rate1Changed(unsigned short)),
		m_ui.UservoiceFEGRate1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFEGRate1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilterEg, SLOT(setRate1(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFilterEg, SIGNAL(rate2Changed(unsigned short)),
		m_ui.UservoiceFEGRate2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFEGRate2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilterEg, SLOT(setRate2(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFilterEg, SIGNAL(rate3Changed(unsigned short)),
		m_ui.UservoiceFEGRate3Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFEGRate3Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilterEg, SLOT(setRate3(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFilterEg, SIGNAL(rate4Changed(unsigned short)),
		m_ui.UservoiceFEGRate4Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceFEGRate4Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceFilterEg, SLOT(setRate4(unsigned short)));

	// USERVOICE Level Scale...
	QObject::connect(
		m_ui.UservoiceLevelScale, SIGNAL(break1Changed(unsigned short)),
		m_ui.UservoiceLevelBreak1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelBreak1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceLevelScale, SLOT(setBreak1(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelScale, SIGNAL(break2Changed(unsigned short)),
		m_ui.UservoiceLevelBreak2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelBreak2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceLevelScale, SLOT(setBreak2(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelScale, SIGNAL(break3Changed(unsigned short)),
		m_ui.UservoiceLevelBreak3Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelBreak3Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceLevelScale, SLOT(setBreak3(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelScale, SIGNAL(break4Changed(unsigned short)),
		m_ui.UservoiceLevelBreak4Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelBreak4Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceLevelScale, SLOT(setBreak4(unsigned short)));

	QObject::connect(
		m_ui.UservoiceLevelScale, SIGNAL(offset1Changed(unsigned short)),
		m_ui.UservoiceLevelOffset1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelOffset1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceLevelScale, SLOT(setOffset1(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelScale, SIGNAL(offset2Changed(unsigned short)),
		m_ui.UservoiceLevelOffset2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelOffset2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceLevelScale, SLOT(setOffset2(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelScale, SIGNAL(offset3Changed(unsigned short)),
		m_ui.UservoiceLevelOffset3Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelOffset3Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceLevelScale, SLOT(setOffset3(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelScale, SIGNAL(offset4Changed(unsigned short)),
		m_ui.UservoiceLevelOffset4Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceLevelOffset4Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceLevelScale, SLOT(setOffset4(unsigned short)));

	// USERVOICE Amp EG...
	QObject::connect(
		m_ui.UservoiceAmpEg, SIGNAL(attackChanged(unsigned short)),
		m_ui.UservoiceAEGAttackDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAEGAttackDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceAmpEg, SLOT(setAttack(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAmpEg, SIGNAL(releaseChanged(unsigned short)),
		m_ui.UservoiceAEGReleaseDial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAEGReleaseDial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceAmpEg, SLOT(setRelease(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAmpEg, SIGNAL(decay1Changed(unsigned short)),
		m_ui.UservoiceAEGDecay1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAEGDecay1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceAmpEg, SLOT(setDecay1(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAmpEg, SIGNAL(decay2Changed(unsigned short)),
		m_ui.UservoiceAEGDecay2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAEGDecay2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceAmpEg, SLOT(setDecay2(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAmpEg, SIGNAL(level1Changed(unsigned short)),
		m_ui.UservoiceAEGLevel1Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAEGLevel1Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceAmpEg, SLOT(setLevel1(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAmpEg, SIGNAL(level2Changed(unsigned short)),
		m_ui.UservoiceAEGLevel2Dial, SLOT(setValue(unsigned short)));
	QObject::connect(
		m_ui.UservoiceAEGLevel2Dial, SIGNAL(valueChanged(unsigned short)),
		m_ui.UservoiceAmpEg, SLOT(setLevel2(unsigned short)));

  	m_ui.UservoiceNameEdit         -> set_param_map(USERVOICE, 0x00);
	m_ui.UservoiceElementDial      -> set_param_map(USERVOICE, 0x0b);
	m_ui.UservoiceLevelDial        -> set_param_map(USERVOICE, 0x0c);

	// Element 1
	m_ui.UservoiceWaveDial         -> setMaximumWidth(86);
	m_ui.UservoiceWaveDial         -> set_param_map(USERVOICE, 0x3d);
	m_ui.UservoiceNoteLowDial      -> set_param_map(USERVOICE, 0x3f);
	m_ui.UservoiceNoteHighDial     -> set_param_map(USERVOICE, 0x40);
	m_ui.UservoiceVelLowDial       -> set_param_map(USERVOICE, 0x41);
	m_ui.UservoiceVelHighDial      -> set_param_map(USERVOICE, 0x42);
	m_ui.UservoiceFEGVelCurveDial  -> set_param_map(USERVOICE, 0x43);
	m_ui.UservoiceLFOWaveDial      -> set_param_map(USERVOICE, 0x44);
	m_ui.UservoiceLFOPhaseDial     -> set_param_map(USERVOICE, 0x45);
	m_ui.UservoiceLFOSpeedDial     -> set_param_map(USERVOICE, 0x46);
	m_ui.UservoiceLFODelayDial     -> set_param_map(USERVOICE, 0x47);
	m_ui.UservoiceLFOFadeDial      -> set_param_map(USERVOICE, 0x48);
	m_ui.UservoiceLFOPitchDial     -> set_param_map(USERVOICE, 0x49);
	m_ui.UservoiceLFOCutoffDial    -> set_param_map(USERVOICE, 0x4a);
	m_ui.UservoiceLFOAmplDial      -> set_param_map(USERVOICE, 0x4b);
	m_ui.UservoiceNoteShiftDial    -> set_param_map(USERVOICE, 0x4c);
	m_ui.UservoiceDetuneDial       -> set_param_map(USERVOICE, 0x4d);
	m_ui.UservoicePitchScalingDial -> set_param_map(USERVOICE, 0x4e);
	m_ui.UservoicePitchCenterDial  -> set_param_map(USERVOICE, 0x4f);
	m_ui.UservoicePEGDepthDial     -> set_param_map(USERVOICE, 0x50);
	m_ui.UservoicePEGVelLevelDial  -> set_param_map(USERVOICE, 0x51);
	m_ui.UservoicePEGVelRateDial   -> set_param_map(USERVOICE, 0x52);
	m_ui.UservoicePEGScalingDial   -> set_param_map(USERVOICE, 0x53);
	m_ui.UservoicePEGCenterDial    -> set_param_map(USERVOICE, 0x54);
	m_ui.UservoicePEGRate1Dial     -> set_param_map(USERVOICE, 0x55);
	m_ui.UservoicePEGRate2Dial     -> set_param_map(USERVOICE, 0x56);
	m_ui.UservoicePEGRate3Dial     -> set_param_map(USERVOICE, 0x57);
	m_ui.UservoicePEGRate4Dial     -> set_param_map(USERVOICE, 0x58);
	m_ui.UservoicePEGLevel0Dial    -> set_param_map(USERVOICE, 0x59);
	m_ui.UservoicePEGLevel1Dial    -> set_param_map(USERVOICE, 0x5a);
	m_ui.UservoicePEGLevel2Dial    -> set_param_map(USERVOICE, 0x5b);
	m_ui.UservoicePEGLevel3Dial    -> set_param_map(USERVOICE, 0x5c);
	m_ui.UservoicePEGLevel4Dial    -> set_param_map(USERVOICE, 0x5d);
	m_ui.UservoiceResonanceDial    -> set_param_map(USERVOICE, 0x5e);
	m_ui.UservoiceVelocityDial     -> set_param_map(USERVOICE, 0x5f);
	m_ui.UservoiceCutoffDial       -> set_param_map(USERVOICE, 0x60);
	m_ui.UservoiceCutoffBreak1Dial -> set_param_map(USERVOICE, 0x61);
	m_ui.UservoiceCutoffBreak2Dial -> set_param_map(USERVOICE, 0x62);
	m_ui.UservoiceCutoffBreak3Dial -> set_param_map(USERVOICE, 0x63);
	m_ui.UservoiceCutoffBreak4Dial -> set_param_map(USERVOICE, 0x64);
	m_ui.UservoiceCutoffOffset1Dial-> set_param_map(USERVOICE, 0x65);
	m_ui.UservoiceCutoffOffset2Dial-> set_param_map(USERVOICE, 0x66);
	m_ui.UservoiceCutoffOffset3Dial-> set_param_map(USERVOICE, 0x67);
	m_ui.UservoiceCutoffOffset4Dial-> set_param_map(USERVOICE, 0x68);
	m_ui.UservoiceFEGVelLevelDial  -> set_param_map(USERVOICE, 0x69);
	m_ui.UservoiceFEGVelRateDial   -> set_param_map(USERVOICE, 0x6a);
	m_ui.UservoiceFEGScalingDial   -> set_param_map(USERVOICE, 0x6b);
	m_ui.UservoiceFEGCenterDial    -> set_param_map(USERVOICE, 0x6c);
	m_ui.UservoiceFEGRate1Dial     -> set_param_map(USERVOICE, 0x6d);
	m_ui.UservoiceFEGRate2Dial     -> set_param_map(USERVOICE, 0x6e);
	m_ui.UservoiceFEGRate3Dial     -> set_param_map(USERVOICE, 0x6f);
	m_ui.UservoiceFEGRate4Dial     -> set_param_map(USERVOICE, 0x70);
	m_ui.UservoiceFEGLevel0Dial    -> set_param_map(USERVOICE, 0x71);
	m_ui.UservoiceFEGLevel1Dial    -> set_param_map(USERVOICE, 0x72);
	m_ui.UservoiceFEGLevel2Dial    -> set_param_map(USERVOICE, 0x73);
	m_ui.UservoiceFEGLevel3Dial    -> set_param_map(USERVOICE, 0x74);
	m_ui.UservoiceFEGLevel4Dial    -> set_param_map(USERVOICE, 0x75);
	m_ui.UservoiceElementLevelDial -> set_param_map(USERVOICE, 0x76);
	m_ui.UservoiceLevelBreak1Dial  -> set_param_map(USERVOICE, 0x77);
	m_ui.UservoiceLevelBreak2Dial  -> set_param_map(USERVOICE, 0x78);
	m_ui.UservoiceLevelBreak3Dial  -> set_param_map(USERVOICE, 0x79);
	m_ui.UservoiceLevelBreak4Dial  -> set_param_map(USERVOICE, 0x7a);
	m_ui.UservoiceLevelOffset1Dial -> set_param_map(USERVOICE, 0x7b);
	m_ui.UservoiceLevelOffset2Dial -> set_param_map(USERVOICE, 0x7c);
	m_ui.UservoiceLevelOffset3Dial -> set_param_map(USERVOICE, 0x7d);
	m_ui.UservoiceLevelOffset4Dial -> set_param_map(USERVOICE, 0x7e);
	m_ui.UservoiceVelCurveDial     -> set_param_map(USERVOICE, 0x7f);
	m_ui.UservoicePanDial          -> set_param_map(USERVOICE, 0x80);
	m_ui.UservoiceAEGRateDial      -> set_param_map(USERVOICE, 0x81);
	m_ui.UservoiceAEGCenterDial    -> set_param_map(USERVOICE, 0x82);
	m_ui.UservoiceAEGDelayDial     -> set_param_map(USERVOICE, 0x83);
	m_ui.UservoiceAEGAttackDial    -> set_param_map(USERVOICE, 0x84);
	m_ui.UservoiceAEGDecay1Dial    -> set_param_map(USERVOICE, 0x85);
	m_ui.UservoiceAEGDecay2Dial    -> set_param_map(USERVOICE, 0x86);
	m_ui.UservoiceAEGReleaseDial   -> set_param_map(USERVOICE, 0x87);
	m_ui.UservoiceAEGLevel1Dial    -> set_param_map(USERVOICE, 0x88);
	m_ui.UservoiceAEGLevel2Dial    -> set_param_map(USERVOICE, 0x89);
	m_ui.UservoiceAEGOffsetDial    -> set_param_map(USERVOICE, 0x8a);
	m_ui.UservoiceAEGResonanceDial -> set_param_map(USERVOICE, 0x8c);

	// Make sure there's nothing pending...
	m_pMasterMap->reset_part_dirty();
	m_pMasterMap->reset_user_dirty();
	m_iDirtyCount = 0;

	// Is any session pending to be loaded?
	if (!m_pOptions->sessionFiles.isEmpty()) {
		// Just load the prabably startup session...
		if (loadSessionFile(m_pOptions->sessionFiles.first()))
			m_pOptions->sessionFiles.clear();
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
			// Specific options...
			if (m_pMasterMap)
				m_pOptions->bUservoiceAutoSend = m_pMasterMap->auto_send();
			// Save main windows state.
			m_pOptions->saveWidgetGeometry(this, true);
		}
	}

	return bQueryClose;
}


void qxgeditMainForm::closeEvent ( QCloseEvent *pCloseEvent )
{
	// Let's be sure about that...
	if (queryClose()) {
		pCloseEvent->accept();
	#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		QApplication::exit(0);
	#else
		QApplication::quit();
	#endif
	} else {
		pCloseEvent->ignore();
	}
}


// Window drag-n-drop event handlers.
void qxgeditMainForm::dragEnterEvent ( QDragEnterEvent* pDragEnterEvent )
{
	// Accept external drags only...
	if (pDragEnterEvent->source() == nullptr
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


// LADISH Level 1 -- SIGUSR1 signal handler.
void qxgeditMainForm::handle_sigusr1 (void)
{
#ifdef HAVE_SIGNAL_H

	char c;

	if (::read(g_fdSigusr1[1], &c, sizeof(c)) > 0)
		saveSession(false);

#endif
}


void qxgeditMainForm::handle_sigterm (void)
{
#ifdef HAVE_SIGNAL_H

	char c;

	if (::read(g_fdSigterm[1], &c, sizeof(c)) > 0)
		close();

#endif
}


// RPN Event handler.
void qxgeditMainForm::rpnReceived (
	unsigned char ch, unsigned short rpn, unsigned short val )
{
	if (m_pMasterMap)
		m_pMasterMap->set_rpn_value(ch, rpn, val);
}


// NRPN Event handler.
void qxgeditMainForm::nrpnReceived (
	unsigned char ch, unsigned short nrpn, unsigned short val )
{
	if (m_pMasterMap)
		m_pMasterMap->set_nrpn_value(ch, nrpn, val);
}


// SYSEX Event handler.
void qxgeditMainForm::sysexReceived ( const QByteArray& sysex )
{
	if (m_pMasterMap) {
		qxgeditXGMasterMap::SysexData sysex_data;
		m_pMasterMap->add_sysex_data(sysex_data,
			(unsigned char *) sysex.data(),
			(unsigned short) sysex.length());
		m_pMasterMap->set_sysex_data(sysex_data);
	}
}


//-------------------------------------------------------------------------
// qxgeditMainForm -- Session file stuff.

// Format the displayable session filename.
QString qxgeditMainForm::sessionName ( const QString& sFilename )
{
	const bool bCompletePath = (m_pOptions && m_pOptions->bCompletePath);
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

	// Reset it all...
	masterReset();

	// Ok, increment untitled count.
	m_iUntitled++;

	// Stabilize form.
	m_sFilename.clear();
	m_iDirtyCount = 0;

	stabilizeForm();

	return true;
}


// Open an existing sampler session.
bool qxgeditMainForm::openSession (void)
{
	if (m_pOptions == nullptr)
		return false;

	// Ask for the filename to open...
	QString sFilename;

	const QString sExt("syx");
	const QString& sTitle  = tr("Open Session");
	const QString& sFilter = tr("Session files (*.%1)").arg(sExt);
#if 0//QT_VERSION < QT_VERSION_CHECK(4, 4, 0)
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
	if (m_pOptions == nullptr)
		return false;

	// Suggest a filename, if there's none...
	QString sFilename = m_sFilename;
	if (sFilename.isEmpty())
		bPrompt = true;

	// Ask for the file to save...
	if (bPrompt) {
		// Prompt the guy...
		const QString sExt("syx");
		const QString& sTitle  = tr("Save Session");
		const QString& sFilter = tr("Session files (*.%1)").arg(sExt);
	#if 0//QT_VERSION < QT_VERSION_CHECK(4, 4, 0)
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
					tr("Warning"),
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
			tr("Warning"),
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
		// we're now clean, for sure.
		m_iDirtyCount = 0;
	}

	return bClose;
}


// Load a session from specific file path.
bool qxgeditMainForm::loadSessionFile ( const QString& sFilename )
{
	// Open the source file...
	QFile file(sFilename);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	// Tell the world we'll take some time...
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	// Reset it all...
	masterReset();

	int iSysex = 0;
	unsigned short iBuff = 0;
	unsigned char *pBuff = nullptr;
	unsigned short i = 0;

	// Read the file....
	qxgeditXGMasterMap::SysexData sysex_data;

	while (!file.atEnd()) {
		// (Re)allocate buffer...
		if (i >= iBuff) {
			unsigned char *pTemp = pBuff;
			iBuff += 1024;
			pBuff  = new unsigned char [iBuff];
			if (pTemp) {
				::memcpy(pBuff, pTemp, i);
				delete [] pTemp;
			}
		}
		// Read the next chunk...
		unsigned short iRead = file.read((char *) pBuff + i, iBuff - i) + i;
		while (i < iRead) {
			if (pBuff[i++] == 0xf7) {
				++iSysex;
				m_pMasterMap->add_sysex_data(sysex_data, pBuff, i);
				if (i < iRead) {
					::memmove(pBuff, pBuff + i, iRead -= i);
					i = 0;
				}
			}
		}
	}

	// Cleanup...
	if (pBuff)
		delete [] pBuff;	
	file.close();

	// Notify!
	m_pMasterMap->set_sysex_data(sysex_data, (iSysex > 0));

	// Deferred QS300 Bulk Dump feedback...
	for (unsigned short iUser = 0; iUser < 32; ++iUser) {
		if (m_pMasterMap->user_dirty(iUser)) {
			m_pMasterMap->send_user(iUser);
			m_pMasterMap->set_user_dirty(iUser, false);
		}
	}

	// Deferred XG Multi Part feedback...
	for (unsigned short iPart = 0; iPart < 16; ++iPart) {
		if (m_pMasterMap->part_dirty(iPart)) {
			m_pMasterMap->send_part(iPart);
			m_pMasterMap->set_part_dirty(iPart, false);
		}
	}

	// We're formerly done.
	QApplication::restoreOverrideCursor();

	// Reset the official session title.
	m_sFilename = sFilename;
	updateRecentFiles(sFilename);
	m_iDirtyCount = 0;

	// Save as default session directory.
	if (m_pOptions)
		m_pOptions->sSessionDir = QFileInfo(sFilename).absolutePath();

	stabilizeForm();

	return (iSysex > 0);
}


// Save current session to specific file path.
bool qxgeditMainForm::saveSessionFile ( const QString& sFilename )
{
	// Open the target file...
	QFile file(sFilename);
	if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
		return false;

	// Tell the world we'll take some time...
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	// XG Parameter changes...
	XGParamMasterMap::const_iterator iter = m_pMasterMap->constBegin();
	for (; iter != m_pMasterMap->constEnd(); ++iter) {
		XGParam *pParam = iter.value();
		if (pParam->high() == 0x11 || pParam->value() == pParam->def())
			continue;
		XGParamSysex sysex(pParam);
		file.write((const char *) sysex.data(), sysex.size());
	}

	// (QS300) USER VOICE Bulk Dumps, whether dirty...
	for (unsigned short iUser = 0; iUser < 32; ++iUser) {
		if (m_pMasterMap->user_dirty(iUser)) {
			XGUserVoiceSysex sysex(iUser);
			file.write((const char *) sysex.data(), sysex.size());
		}
	}

	file.close();

	// We're formerly done.
	QApplication::restoreOverrideCursor();

	// Reset the official session title.
	m_sFilename = sFilename;
	updateRecentFiles(sFilename);
	m_iDirtyCount = 0;

	// Save as default session directory.
	if (m_pOptions)
		m_pOptions->sSessionDir = QFileInfo(sFilename).absolutePath();

	stabilizeForm();

	return true;
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


// Randomize current parameter page view.
void qxgeditMainForm::viewRandomize (void)
{
	if (m_pMasterMap == nullptr)
		return;

	float p = 100.0f;

	if (m_pOptions) {
		p = m_pOptions->fRandomizePercent;
		if (m_pOptions->bConfirmReset) {
			QString sText;
			switch (m_ui.MainTabWidget->currentIndex()) {
			case 0: // SYSTEM / EFFECT page...
				switch (m_ui.SystemEffectToolBox->currentIndex()) {
				case 1: // REVERB section...
					sText = tr("REVERB / %1")
					.arg(m_ui.ReverbTypeCombo->currentText());
					break;
				case 2: // CHORUS section...
					sText = tr("CHORUS / %1")
					.arg(m_ui.ChorusTypeCombo->currentText());
					break;
				case 3: // VARIATION section...
					sText = tr("VARIATION / %1")
					.arg(m_ui.VariationTypeCombo->currentText());
					break;
				default:
					break;
				}
				break;
			case 1: // MULTI PART page...
				sText = tr("MULTI PART / %1")
				.arg(m_ui.MultipartCombo->currentText());
				break;
			case 2: // DRUM SETUP page...
				sText = tr("DRUM SETUP / %1 - %2")
				.arg(m_ui.DrumsetupCombo->currentText())
				.arg(m_ui.DrumsetupNoteCombo->currentText());
				break;
			case 3: // USER VOICE page...
				sText = tr("USER VOICE / %1 - %2")
				.arg(m_ui.UservoiceCombo->currentText())
				.arg(m_ui.UservoiceElementCombo->currentText());
				break;
			default:
				break;
			}
			if (QMessageBox::warning(this,
				tr("Warning"),
				tr("About to randomize current parameter values:\n\n"
				"%1.\n\n"
				"Are you sure?").arg(sText),
				QMessageBox::Ok | QMessageBox::Cancel)
				== QMessageBox::Cancel)
				return;
		}
	}

	switch (m_ui.MainTabWidget->currentIndex()) {
	case 0: // SYSTEM / EFFECT page...
		switch (m_ui.SystemEffectToolBox->currentIndex()) {
		case 1: // REVERB section...
			m_pMasterMap->REVERB.randomize_value(p);
			break;
		case 2: // CHORUS section...
			m_pMasterMap->CHORUS.randomize_value(p);
			break;
		case 3: // VARIATION section...
			m_pMasterMap->VARIATION.randomize_value(p);
			break;
		default:
			break;
		}
		break;
	case 1: // MULTI PART page...
		m_pMasterMap->randomize_part(m_ui.MultipartCombo->currentIndex(), p);
		break;
	case 2: // DRUM SETUP page...
		m_pMasterMap->randomize_drums(
			m_ui.DrumsetupCombo->currentIndex(),
			m_ui.DrumsetupNoteCombo->itemData(
				m_ui.DrumsetupNoteCombo->currentIndex()).toUInt(), p);
		break;
	case 3: // USER VOICE page...
		m_pMasterMap->randomize_user(m_ui.UservoiceCombo->currentIndex(), p);
		break;
	default:
		break;
	}

	stabilizeForm();
}


// Show options dialog.
void qxgeditMainForm::viewOptions (void)
{
	if (m_pOptions == nullptr)
		return;

	// Load the current setup settings.
	const bool    bOldCompletePath   = m_pOptions->bCompletePath;
	const int     iOldMaxRecentFiles = m_pOptions->iMaxRecentFiles;
	const int     iOldBaseFontSize   = m_pOptions->iBaseFontSize;
	const QString sOldStyleTheme     = m_pOptions->sStyleTheme;
	const QString sOldColorTheme     = m_pOptions->sColorTheme;
	// Load the current setup settings.
	qxgeditOptionsForm optionsForm(this);
	optionsForm.setOptions(m_pOptions);
	if (optionsForm.exec()) {
		// Check whether restart is needed or whether
		// custom options maybe set up immediately...
		int iNeedRestart = 0;
		if (m_pOptions->sStyleTheme != sOldStyleTheme) {
			if (m_pOptions->sStyleTheme.isEmpty()) {
				++iNeedRestart;
			} else {
				QApplication::setStyle(
					QStyleFactory::create(m_pOptions->sStyleTheme));
			}
		}
		if (m_pOptions->sColorTheme != sOldColorTheme) {
			if (m_pOptions->sColorTheme.isEmpty()) {
				++iNeedRestart;
			} else {
				QPalette pal;
				if (qxgeditPaletteForm::namedPalette(
						&m_pOptions->settings(), m_pOptions->sColorTheme, pal))
					QApplication::setPalette(pal);
			}
		}
		// Show restart message if needed...
		if (iOldBaseFontSize != m_pOptions->iBaseFontSize)
			++iNeedRestart;
		if (( bOldCompletePath && !m_pOptions->bCompletePath) ||
			(!bOldCompletePath &&  m_pOptions->bCompletePath) ||
			(iOldMaxRecentFiles != m_pOptions->iMaxRecentFiles))
			updateRecentFilesMenu();
		// Show restart needed message...
		if (iNeedRestart > 0) {
			QMessageBox::information(this,
				tr("Information"),
				tr("Some settings may be only effective\n"
				"next time you start this program."));
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
	QString sText = "<h1>" QXGEDIT_TITLE "</h1>\n";
	sText += "<p>" + tr(QXGEDIT_SUBTITLE) + "<br />\n";
	sText += "<br />\n";
	sText += tr("Version") + ": <b>" PROJECT_VERSION "</b><br />\n";
//	sText += "<small>" + tr("Build") + ": " CONFIG_BUILD_DATE "</small><br />\n";
#ifdef CONFIG_DEBUG
	sText += "<small><font color=\"red\">";
	sText += tr("Debugging option enabled.");
	sText += "</font></small>\n";
#endif
	sText += "<br />\n";
	sText += tr("Using: Qt %1").arg(qVersion());
#if defined(QT_STATIC)
	sText += "-static";
#endif
	sText += "<br />\n";
	sText += "<br />\n";
	sText += tr("Website") + ": <a href=\"" QXGEDIT_WEBSITE "\">" QXGEDIT_WEBSITE "</a><br />\n";
	sText += "<br />\n";
	sText += "<small>";
	sText += QXGEDIT_COPYRIGHT "<br />\n";
	sText += "<br />\n";
	sText += tr("This program is free software; you can redistribute it and/or modify it") + "<br />\n";
	sText += tr("under the terms of the GNU General Public License version 2 or later.");
	sText += "</small>";
	sText += "<br />\n";
	sText += "</p>\n";

	QMessageBox::about(this, tr("About"), sText);
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
	setWindowTitle(sSessionName);

	// Update the main menu state...
	m_ui.fileSaveAction->setEnabled(m_iDirtyCount > 0);

	// Randomize view menu.
	m_ui.viewRandomizeAction->setEnabled(isRandomizable());

	// Recent files menu.
	m_ui.fileOpenRecentMenu->setEnabled(m_pOptions->recentFiles.count() > 0);

	m_statusItems[StatusName]->setText(sessionName(m_sFilename));

	if (m_iDirtyCount > 0)
		m_statusItems[StatusMod]->setText(tr("MOD"));
	else
		m_statusItems[StatusMod]->clear();

	// QS300 User Voice dirty flag array status.
	if (m_pMasterMap) {
		unsigned short iUser = m_ui.UservoiceCombo->currentIndex();
		m_ui.UservoiceSendButton->setEnabled(m_pMasterMap->user_dirty_1(iUser));
		m_ui.UservoiceNameEdit->stabilizePreset();
	}
}


// Update the recent files list and menu.
void qxgeditMainForm::updateRecentFiles ( const QString& sFilename )
{
	if (m_pOptions == nullptr)
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
	if (m_pOptions == nullptr)
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

// XG System Reset...
void qxgeditMainForm::masterReset (void)
{
	XGParam *pParam = m_pMasterMap->find_param(0x00, 0x00, 0x7e);
	if (pParam)
		pParam->set_value_update(0);
}

void qxgeditMainForm::masterResetButtonClicked (void)
{
	if (m_pMasterMap == nullptr)
		return;

	if (m_pOptions && m_pOptions->bConfirmReset) {
		if (QMessageBox::warning(this,
			tr("Warning"),
			tr("About to reset all parameters to default:\n\n"
			"XG System.\n\n"
			"Are you sure?"),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Cancel)
			return;
	}

	masterReset();
}


void qxgeditMainForm::reverbResetButtonClicked (void)
{
	if (m_pMasterMap == nullptr)
		return;

	if (m_pOptions && m_pOptions->bConfirmReset) {
		if (QMessageBox::warning(this,
			tr("Warning"),
			tr("About to reset all parameters to default:\n\n"
			"REVERB / %1.\n\n"
			"Are you sure?").arg(m_ui.ReverbTypeCombo->currentText()),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Cancel)
			return;
	}

	m_pMasterMap->REVERB.reset();
}


void qxgeditMainForm::chorusResetButtonClicked (void)
{
	if (m_pMasterMap == nullptr)
		return;

	if (m_pOptions && m_pOptions->bConfirmReset) {
		if (QMessageBox::warning(this,
			tr("Warning"),
			tr("About to reset all parameters to default:\n\n"
			"CHORUS / %1.\n\n"
			"Are you sure?").arg(m_ui.ChorusTypeCombo->currentText()),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Cancel)
			return;
	}

	m_pMasterMap->CHORUS.reset();
}


void qxgeditMainForm::variationResetButtonClicked (void)
{
	if (m_pMasterMap == nullptr)
		return;

	if (m_pOptions && m_pOptions->bConfirmReset) {
		if (QMessageBox::warning(this,
			tr("Warning"),
			tr("About to reset all parameters to default:\n\n"
			"VARIATION / %1.\n\n"
			"Are you sure?").arg(m_ui.VariationTypeCombo->currentText()),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Cancel)
			return;
	}

	m_pMasterMap->VARIATION.reset();
}


// Switch the current MULTIPART section...
void qxgeditMainForm::multipartComboActivated ( int iPart )
{
	if (m_pMasterMap)
		m_pMasterMap->MULTIPART.set_current_key(iPart);
}


// Switch the current MULTIPART Instrument Normal Voice...
void qxgeditMainForm::multipartVoiceComboActivated ( int iVoice )
{
	if (m_iMultipartVoiceUpdate > 0)
		return;

	++m_iMultipartVoiceUpdate;

	const QModelIndex& parent
		= m_ui.MultipartVoiceCombo->view()->currentIndex().parent();

	// Check for a normal voice first...
	XGInstrument instr(parent.row());
	if (instr.group()) {
		XGNormalVoice voice(&instr, iVoice);
	#ifdef CONFIG_DEBUG
		qDebug("qxgeditMainForm::multipartVoiceComboActivated(%d)"
			" parent=(%d) [%s/%s]",
			iVoice, parent.row(),
			instr.name(), voice.name());
	#endif
		m_ui.MultipartPartModeDial->reset_value();
		m_ui.MultipartBankMSBDial->set_value_update(voice.bank() >> 7);
		m_ui.MultipartBankLSBDial->set_value_update(voice.bank() & 0x7f);
		m_ui.MultipartProgramDial->set_value_update(voice.prog());
	}
	else
	if (parent.row() == XGInstrument::count()) {
		// May it be a Drums voice...
		XGDrumKit drumkit(iVoice);
		if (drumkit.item()) {
		#ifdef CONFIG_DEBUG
			qDebug("qxgeditMainForm::multipartVoiceComboActivated(%d)"
				" parent=(%d) [Drums/%s]",
				iVoice, parent.row(),
				drumkit.name());
		#endif
			m_ui.MultipartPartModeDial->set_value_update(1); // DRUMS
			m_ui.MultipartBankMSBDial->set_value_update(drumkit.bank() >> 7);
			m_ui.MultipartBankLSBDial->set_value_update(drumkit.bank() & 0x7f);
			m_ui.MultipartProgramDial->set_value_update(drumkit.prog());
		}
	}
	else
	if (parent.row() == XGInstrument::count() + 1) {
	#ifdef CONFIG_DEBUG
		qDebug("qxgeditMainForm::multipartVoiceComboActivated(%d)"
			" parent=(%d) [QS300 User %d]",
			iVoice, parent.row(),
			iVoice + 1);
	#endif
		// Can only be a QS300 User voice...
		m_ui.MultipartPartModeDial->reset_value();
		m_ui.MultipartBankMSBDial->set_value_update(63);
		m_ui.MultipartBankLSBDial->set_value_update(0);
		m_ui.MultipartProgramDial->set_value_update(iVoice);
		// QS300 USser voice immediate conviniency...
		m_ui.UservoiceCombo->setCurrentIndex(iVoice);
		uservoiceComboActivated(iVoice);
	}

	--m_iMultipartVoiceUpdate;
}

void qxgeditMainForm::multipartVoiceChanged (void)
{
	if (m_iMultipartVoiceUpdate > 0)
		return;

	++m_iMultipartVoiceUpdate;

	const unsigned short iBank
		= (m_ui.MultipartBankMSBDial->value() << 7)
		| m_ui.MultipartBankLSBDial->value();
	const unsigned char iProg = m_ui.MultipartProgramDial->value();

	unsigned short i = 0;

	// Normal regular voice lookup...
	for (; i < XGInstrument::count(); ++i) {
		XGInstrument instr(i);
		int j = instr.find_voice(iBank, iProg);
		if (j >= 0) {
			XGNormalVoice voice(&instr, j);
		//	m_ui.MultipartVoiceCombo->showPopup();
			const QModelIndex& parent
				= m_ui.MultipartVoiceCombo->model()->index(i, 0);
			const QModelIndex& index
				= m_ui.MultipartVoiceCombo->model()->index(j, 0, parent);
		#ifdef CONFIG_DEBUG
			qDebug("qxgeditMainForm::multipartVoiceChanged(%d)"
				" parent=%d bank=%u prog=%u [%s/%s]",
				index.row(), parent.row(),
				iBank, iProg, instr.name(), voice.name());
		#endif
		//	m_ui.MultipartVoiceCombo->view()->setCurrentIndex(index);
			QModelIndex oldroot = m_ui.MultipartVoiceCombo->rootModelIndex();
			m_ui.MultipartVoiceCombo->setRootModelIndex(parent);
			m_ui.MultipartVoiceCombo->setCurrentIndex(index.row());
			m_ui.MultipartVoiceCombo->setRootModelIndex(oldroot);
		//	m_ui.MultipartPartModeDial->reset_value();
			break;
		}
	}

	// Drums voice lookup...
	if (i >= XGInstrument::count()) {
		unsigned short k = 0;
		for (; k < XGDrumKit::count(); ++k) {
			XGDrumKit drumkit(k);
			if (drumkit.bank() == iBank && drumkit.prog() == iProg) {
			//	m_ui.MultipartVoiceCombo->showPopup();
				const QModelIndex& parent
					= m_ui.MultipartVoiceCombo->model()->index(i, 0);
				const QModelIndex& index
					= m_ui.MultipartVoiceCombo->model()->index(k, 0, parent);
			#ifdef CONFIG_DEBUG
				qDebug("qxgeditMainForm::multipartVoiceChanged(%d)"
					" parent=%d bank=%u prog=%u [Drums/%s]",
					index.row(), parent.row(),
					iBank, iProg, drumkit.name());
			#endif
			//	m_ui.MultipartVoiceCombo->view()->setCurrentIndex(index);
				QModelIndex oldroot = m_ui.MultipartVoiceCombo->rootModelIndex();
				m_ui.MultipartVoiceCombo->setRootModelIndex(parent);
				m_ui.MultipartVoiceCombo->setCurrentIndex(index.row());
				m_ui.MultipartVoiceCombo->setRootModelIndex(oldroot);
			//	m_ui.MultipartPartModeDial->set_value_update(1); // DRUMS
				break;
			}
		}
		// QS300 voice lookup...
		if (k >= XGDrumKit::count() && iBank == (63 << 7)) {
		//	m_ui.MultipartVoiceCombo->showPopup();
			const QModelIndex& parent
				= m_ui.MultipartVoiceCombo->model()->index(++i, 0);
			const QModelIndex& index
				= m_ui.MultipartVoiceCombo->model()->index(iProg, 0, parent);
		#ifdef CONFIG_DEBUG
			qDebug("qxgeditMainForm::multipartVoiceChanged(%d)"
				" parent=%d bank=%u prog=%u [QS300 User %d]",
				index.row(), parent.row(),
				iBank, iProg, iProg + 1);
		#endif
		//	m_ui.MultipartVoiceCombo->view()->setCurrentIndex(index);
			QModelIndex oldroot = m_ui.MultipartVoiceCombo->rootModelIndex();
			m_ui.MultipartVoiceCombo->setRootModelIndex(parent);
			m_ui.MultipartVoiceCombo->setCurrentIndex(index.row());
			m_ui.MultipartVoiceCombo->setRootModelIndex(oldroot);
		//	m_ui.MultipartPartModeDial->reset_value();
			// QS300 User voice immediate conveniency...
			m_ui.UservoiceCombo->setCurrentIndex(iProg);
			uservoiceComboActivated(iProg);
		}
	}

	--m_iMultipartVoiceUpdate;
}


void qxgeditMainForm::multipartPartModeChanged ( unsigned short iPartMode )
{
	const bool bEnabled = (iPartMode == 0);

	m_ui.MultipartPedalCheck->setEnabled(bEnabled);
	m_ui.MultipartBankLSBDial->setEnabled(bEnabled);
	m_ui.MultipartPolyModeDial->setEnabled(bEnabled);
	m_ui.MultipartPortamentoBox->setEnabled(bEnabled);
	m_ui.MultipartTuningBox->setEnabled(bEnabled);
	m_ui.MultipartPATBox->setEnabled(bEnabled);
	m_ui.MultipartPitchEgBox->setEnabled(bEnabled);
}


void qxgeditMainForm::multipartResetButtonClicked (void)
{
	if (m_pMasterMap == nullptr)
		return;

	const int iPart = m_ui.MultipartCombo->currentIndex();

	if (m_pOptions && m_pOptions->bConfirmReset) {
		if (QMessageBox::warning(this,
			tr("Warning"),
			tr("About to reset all parameters to default:\n\n"
			"MULTI PART / %1.\n\n"
			"Are you sure?").arg(m_ui.MultipartCombo->currentText()),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Cancel)
			return;
	}

	m_pMasterMap->reset_part(iPart);
	multipartVoiceChanged();
}


// Switch the current DRUMSETUP section...
void qxgeditMainForm::drumsetupComboActivated ( int iDrumset )
{
	if (m_pMasterMap) {
		int iNote = m_ui.DrumsetupNoteCombo->currentIndex();
		unsigned short key = (unsigned short) (iDrumset << 7)
			+ m_ui.DrumsetupNoteCombo->itemData(iNote).toUInt();
		m_pMasterMap->DRUMSETUP.set_current_key(key);
	}
}

// Switch the current DRUMSETUP Drum Kit Voice...
void qxgeditMainForm::drumsetupVoiceComboActivated ( int iDrumKit )
{
	XGDrumKit drumkit(iDrumKit);
	if (drumkit.item()) {
		XGDrumKit stdkit(0); // Standard Kit (default)
		m_ui.DrumsetupNoteCombo->clear();
		for (unsigned short k = 13; k < 85; ++k) {
			QString sName;
			int i = drumkit.find_voice(k);
			if (i >= 0)  {
				sName = XGDrumVoice(&drumkit, i).name();
			} else if (stdkit.item()) {
				i = stdkit.find_voice(k);
				if (i >= 0)
					sName = XGDrumVoice(&stdkit, i).name();
			}
			if (!sName.isEmpty())
				sName += ' ';
			sName += QString("(%1)").arg(getsnote(k));
			m_ui.DrumsetupNoteCombo->addItem(sName, k);
		}
		const int iNote = m_ui.DrumsetupNoteCombo->findData(
			m_pMasterMap->DRUMSETUP.current_key());
		if (iNote >= 0)
			m_ui.DrumsetupNoteCombo->setCurrentIndex(iNote);
	}
}


void qxgeditMainForm::drumsetupNoteComboActivated ( int iNote )
{
	if (m_pMasterMap) {
		const int iDrumset = m_ui.DrumsetupCombo->currentIndex();
		const unsigned short drum_key
			= m_ui.DrumsetupNoteCombo->itemData(iNote).toUInt();
		const unsigned short key
			= (unsigned short) (iDrumset << 7) + drum_key;
		m_pMasterMap->DRUMSETUP.set_current_key(key);
		// Update drum voice/key defaults...
		const int iDrumKit
			= m_ui.DrumsetupVoiceCombo->currentIndex();
		XGDrumKit drumkit(iDrumKit);
		if (drumkit.item()) {
			const int i = drumkit.find_voice(drum_key);
			if (i >= 0) {
				const XGDrumVoice drum(&drumkit, i);
				m_ui.DrumsetupLevelDial->setValue(drum.level());
				m_ui.DrumsetupGroupDial->setValue(drum.group());
				m_ui.DrumsetupPanDial->setValue(drum.pan());
				m_ui.DrumsetupReverbDial->setValue(drum.reverb());
				m_ui.DrumsetupChorusDial->setValue(drum.chorus());
				m_ui.DrumsetupNoteOffCheck->setValue(drum.noteOff());
			}
		}
	}
}


void qxgeditMainForm::drumsetupResetButtonClicked (void)
{
	if (m_pMasterMap == nullptr)
		return;

	const int iDrumset = m_ui.DrumsetupCombo->currentIndex();

	if (m_pOptions && m_pOptions->bConfirmReset) {
		if (QMessageBox::warning(this,
			tr("Warning"),
			tr("About to reset all parameters to default:\n\n"
			"DRUM SETUP / %1.\n\n"
			"Are you sure?").arg(m_ui.DrumsetupCombo->currentText()),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Cancel)
			return;
	}

	XGParam *pParam = m_pMasterMap->find_param(0x00, 0x00, 0x7d);
	if (pParam)
		pParam->set_value_update(iDrumset);
}


// Switch the current USERVOICE section...
void qxgeditMainForm::uservoiceComboActivated ( int iUser )
{
	if (m_pMasterMap) {
		m_pMasterMap->USERVOICE.set_current_key(iUser);
		if (m_pMasterMap->user_dirty_1(iUser)) {
			m_pMasterMap->send_user(iUser);
			m_pMasterMap->set_user_dirty_1(iUser, false);
		}
		stabilizeForm();
	}
}


void qxgeditMainForm::uservoiceElementComboActivated ( int iElem )
{
	if (m_iUservoiceElementUpdate > 0)
		return;

	++m_iUservoiceElementUpdate;

	if (m_pMasterMap) {
		int iCurrentElem = m_pMasterMap->USERVOICE.current_element();
		if (m_ui.UservoiceElementDial->value() & (iElem + 1)) {
			m_pMasterMap->USERVOICE.set_current_element(iElem);
		} else {
			m_ui.UservoiceElementCombo->setCurrentIndex(iCurrentElem);
		}
		stabilizeForm();
	}

	--m_iUservoiceElementUpdate;
}


void qxgeditMainForm::uservoiceResetButtonClicked (void)
{
	if (m_pMasterMap == nullptr)
		return;

	const int iUser = m_ui.UservoiceCombo->currentIndex();

	if (m_pOptions && m_pOptions->bConfirmReset) {
		if (QMessageBox::warning(this,
			tr("Warning"),
			tr("About to reset all parameters to default:\n\n"
			"USER VOICE / %1.\n\n"
			"Are you sure?").arg(m_ui.UservoiceCombo->currentText()),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Cancel)
			return;
	}

	m_pMasterMap->reset_user(iUser);
	stabilizeForm();
}


void qxgeditMainForm::uservoiceSendButtonClicked (void)
{
	if (m_pMasterMap) {
		const int iUser = m_ui.UservoiceCombo->currentIndex();
		m_pMasterMap->send_user(iUser);
		m_pMasterMap->set_user_dirty_1(iUser, false);
		stabilizeForm();
	}
}


void qxgeditMainForm::uservoiceAutoSendCheckToggled ( bool bAuto )
{
	if (m_pMasterMap)
		m_pMasterMap->set_auto_send(bAuto);
}


void qxgeditMainForm::uservoiceElementChanged ( unsigned short iElems )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditMainForm::uservoiceElementChanged(%u)", iElems);
#endif

	if (m_pMasterMap) {
		int iCurrentElem = m_pMasterMap->USERVOICE.current_element();
		if ((iElems & (iCurrentElem + 1)) == 0) {
			iCurrentElem = iElems - 1;
			m_ui.UservoiceElementCombo->setCurrentIndex(iCurrentElem);
			m_pMasterMap->USERVOICE.set_current_element(iCurrentElem);
			stabilizeForm();
		}
	}
}


// Main dirty flag raiser.
void qxgeditMainForm::contentsChanged (void)
{
	m_iDirtyCount++;
	stabilizeForm();
}


// Check whether randomize current parameter page view is possible.
bool qxgeditMainForm::isRandomizable (void) const
{
	if (m_pMasterMap == nullptr)
		return false;

	switch (m_ui.MainTabWidget->currentIndex()) {
	case 0: // SYSTEM / EFFECT page...
		switch (m_ui.SystemEffectToolBox->currentIndex()) {
		case 1: // REVERB section...
		case 2: // CHORUS section...
		case 3: // VARIATION section...
			return true;
		default:
			break;
		}
		break;
	case 1: // MULTI PART page...
	case 2: // DRUM SETUP page...
	case 3: // USER VOICE page...
		return true;
	default:
		break;
	}

	return false;
}


//-------------------------------------------------------------------------
// qxgeditMainForm -- Uservoice preset slot handlers.

void qxgeditMainForm::uservoiceLoadPresetFile ( const QString& sFilename )
{
	// Open the source file...
	QFile file(sFilename);
	if (!file.open(QIODevice::ReadOnly))
		return;

	// Tell the world we'll take some time...
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	const int iUser = m_ui.UservoiceCombo->currentIndex();

	qxgeditXGMasterMap::SysexData sysex_data;

	const unsigned short len = 0x188; // = 392 bytes
	unsigned char  data[len];

	if (file.read((char *) data, len)) {
		// Make sure it's a QS300 SysEx bulk dump...
		if (data[1] == 0x43 && data[3] == 0x4b && data[6] == 0x11) {
			 // HACK! Correct checksum...
			data[7] = iUser;
			unsigned char cksum = 0;
			for (unsigned short i = 4; i < len - 2; ++i) {
				cksum += data[i];
				cksum &= 0x7f;
			}
			data[len - 2] = 0x80 - cksum;
			m_pMasterMap->add_sysex_data(sysex_data, data, len);
		}
	}
	file.close();

	const bool bResult = m_pMasterMap->set_sysex_data(sysex_data);

	// Deferred QS300 Bulk Dump feedback...
	if (bResult) {
		m_pMasterMap->set_user_dirty_2(iUser, false);
		if (m_pMasterMap->auto_send()) {
			m_pMasterMap->send_user(iUser);
			m_pMasterMap->set_user_dirty_1(iUser, false);
		}
	}

	// We're formerly done.
	QApplication::restoreOverrideCursor();
	
	stabilizeForm();

	// Any late warning?
	if (!bResult) {
		// Failure (maybe wrong preset)...
		QMessageBox::critical(this,
			tr("Error"),
			tr("Preset could not be loaded:\n\n"
			"\"%1\".\n\n"
			"Sorry.").arg(sFilename),
			QMessageBox::Cancel);		
	}
}


void qxgeditMainForm::uservoiceSavePresetFile ( const QString& sFilename )
{
	// Open the target file...
	QFile file(sFilename);
	if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
		return;

	// Tell the world we'll take some time...
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	const int iUser = m_ui.UservoiceCombo->currentIndex();

	// (QS300) USER VOICE Bulk Dumps, whether dirty...
	XGUserVoiceSysex sysex(iUser);
	file.write((const char *) sysex.data(), sysex.size());
	file.close();

	// Just make the preset dirty...
	m_pMasterMap->set_user_dirty_2(iUser, false);

	// We're formerly done.
	QApplication::restoreOverrideCursor();

	stabilizeForm();
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
	QMessageBox::critical(this, tr("Error"), s, QMessageBox::Ok);
}


// end of qxgeditMainForm.cpp
