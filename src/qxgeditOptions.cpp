// qxgeditOptions.cpp
//
/****************************************************************************
   Copyright (C) 2005-2025, rncbc aka Rui Nuno Capela. All rights reserved.

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

#include "qxgeditAbout.h"
#include "qxgeditOptions.h"

#include <QWidget>
#include <QFileInfo>
#include <QTextStream>

#include <QApplication>

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include <QCommandLineParser>
#include <QCommandLineOption>
#if defined(Q_OS_WINDOWS)
#include <QMessageBox>
#endif
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QDesktopWidget>
#endif


//-------------------------------------------------------------------------
// qxgeditOptions - Prototype settings structure (pseudo-singleton).
//

// Singleton instance pointer.
qxgeditOptions *qxgeditOptions::g_pOptions = nullptr;

// Singleton instance accessor (static).
qxgeditOptions *qxgeditOptions::getInstance (void)
{
	return g_pOptions;
}


// Constructor.
qxgeditOptions::qxgeditOptions (void)
	: m_settings(QXGEDIT_DOMAIN, QXGEDIT_TITLE)
{
	// Pseudo-singleton reference setup.
	g_pOptions = this;

	loadOptions();
}


// Default Destructor.
qxgeditOptions::~qxgeditOptions (void)
{
	saveOptions();

	// Pseudo-singleton reference shut-down.
	g_pOptions = nullptr;
}


// Explicit load method.
void qxgeditOptions::loadOptions (void)
{
	// And go into general options group.
	m_settings.beginGroup("/Options");

	// MIDI specific options...
	m_settings.beginGroup("/Midi");
	midiInputs  = m_settings.value("/Inputs").toStringList();
	midiOutputs = m_settings.value("/Outputs").toStringList();
	m_settings.endGroup();

	// Load display options...
	m_settings.beginGroup("/Display");
	bConfirmReset   = m_settings.value("/ConfirmReset", true).toBool();
	bConfirmRemove  = m_settings.value("/ConfirmRemove", true).toBool();
	bCompletePath   = m_settings.value("/CompletePath", true).toBool();
	iMaxRecentFiles = m_settings.value("/MaxRecentFiles", 5).toInt();
	fRandomizePercent = m_settings.value("/RandomizePercent", 20.0f).toFloat();
	iBaseFontSize   = m_settings.value("/BaseFontSize", 0).toInt();
	sStyleTheme     = m_settings.value("/StyleTheme", "Skulpture").toString();
	sColorTheme     = m_settings.value("/ColorTheme").toString();
	m_settings.endGroup();

	// And go into view options group.
	m_settings.beginGroup("/View");
	bMenubar   = m_settings.value("/Menubar", true).toBool();
	bStatusbar = m_settings.value("/Statusbar", true).toBool();
	bToolbar   = m_settings.value("/Toolbar", true).toBool();
	m_settings.endGroup();

	m_settings.endGroup(); // End of options group.

	// Last but not least, get the defaults.
	m_settings.beginGroup("/Default");
	sSessionDir = m_settings.value("/SessionDir").toString();
	sPresetDir  = m_settings.value("/PresetDir").toString();
	recentFiles = m_settings.value("/RecentFiles").toStringList();
	m_settings.endGroup();

	// (QS300) USER VOICE Specific options.
	m_settings.beginGroup("/Uservoice");
	bUservoiceAutoSend = m_settings.value("/AutoSend", false).toBool();
	m_settings.endGroup();
}


// Explicit save method.
void qxgeditOptions::saveOptions (void)
{
	// Make program version available in the future.
	m_settings.beginGroup("/Program");
	m_settings.setValue("/Version", PROJECT_VERSION);
	m_settings.endGroup();

	// And go into general options group.
	m_settings.beginGroup("/Options");

	// MIDI specific options...
	m_settings.beginGroup("/Midi");
	m_settings.setValue("/Inputs", midiInputs);
	m_settings.setValue("/Outputs", midiOutputs);
	m_settings.endGroup();

	// Save display options.
	m_settings.beginGroup("/Display");
	m_settings.setValue("/ConfirmReset", bConfirmReset);
	m_settings.setValue("/ConfirmRemove", bConfirmRemove);
	m_settings.setValue("/CompletePath", bCompletePath);
	m_settings.setValue("/MaxRecentFiles", iMaxRecentFiles);
	m_settings.setValue("/RandomizePercent", fRandomizePercent);
	m_settings.setValue("/BaseFontSize", iBaseFontSize);
	m_settings.setValue("/StyleTheme", sStyleTheme);
	m_settings.setValue("/ColorTheme", sColorTheme);
	m_settings.endGroup();

	// View options group.
	m_settings.beginGroup("/View");
	m_settings.setValue("/Menubar", bMenubar);
	m_settings.setValue("/Statusbar", bStatusbar);
	m_settings.setValue("/Toolbar", bToolbar);
	m_settings.endGroup();

	m_settings.endGroup(); // End of options group.

	// Default directories.
	m_settings.beginGroup("/Default");
	m_settings.setValue("/SessionDir", sSessionDir);
	m_settings.setValue("/PresetDir", sPresetDir);
	m_settings.setValue("/RecentFiles", recentFiles);
	m_settings.endGroup();

	// (QS300) USER VOICE Specific options.
	m_settings.beginGroup("/Uservoice");
	m_settings.setValue("/AutoSend", bUservoiceAutoSend);
	m_settings.endGroup();

	// Save/commit to disk.
	m_settings.sync();
}


//-------------------------------------------------------------------------
// Settings accessor.
//

QSettings& qxgeditOptions::settings (void)
{
	return m_settings;
}


//-------------------------------------------------------------------------
// Command-line argument stuff.
//

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)

void qxgeditOptions::show_error( const QString& msg )
{
#if defined(Q_OS_WINDOWS)
	QMessageBox::information(nullptr, QApplication::applicationName(), msg);
#else
	const QByteArray tmp = msg.toUtf8() + '\n';
	::fputs(tmp.constData(), stderr);
#endif
}

#else

// Help about command line options.
void qxgeditOptions::print_usage ( const QString& arg0 )
{
	QTextStream out(stderr);
	const QString sEot = "\n\t";
	const QString sEol = "\n\n";

	out << QObject::tr("Usage: %1 [options] [session-file]").arg(arg0) + sEol;
	out << QXGEDIT_TITLE " - " + QObject::tr(QXGEDIT_SUBTITLE) + sEol;
	out << QObject::tr("Options:") + sEol;
	out << "  -h, --help" + sEot +
		QObject::tr("Show help about command line options.") + sEol;
	out << "  -v, --version" + sEot +
		QObject::tr("Show version information") + sEol;
}

#endif


// Parse command line arguments into m_settings.
bool qxgeditOptions::parse_args ( const QStringList& args )
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)

	QCommandLineParser parser;
	parser.setApplicationDescription(
		QXGEDIT_TITLE " - " + QObject::tr(QXGEDIT_SUBTITLE));

	const QCommandLineOption& helpOption = parser.addHelpOption();
	const QCommandLineOption& versionOption = parser.addVersionOption();
	parser.addPositionalArgument("session-file",
		QObject::tr("Session file (.syx)"),
		QObject::tr("[session-file]"));

	if (!parser.parse(args)) {
		show_error(parser.errorText());
		return false;
	}

	if (parser.isSet(helpOption)) {
		show_error(parser.helpText());
		return false;
	}

	if (parser.isSet(versionOption)) {
		QString sVersion = QString("%1 %2\n")
			.arg(QXGEDIT_TITLE)
			.arg(QCoreApplication::applicationVersion());
		sVersion += QString("Qt: %1").arg(qVersion());
	#if defined(QT_STATIC)
		sVersion += "-static";
	#endif
	#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
		sVersion += ' ';
		sVersion += '(';
		sVersion += QApplication::platformName();
		sVersion += ')';
	#endif
		sVersion += '\n';
		show_error(sVersion);
		return false;
	}

	foreach (const QString& sArg, parser.positionalArguments()) {
		sessionFiles.append(QFileInfo(sArg).absoluteFilePath());
	}

#else

	int iCmdArgs = 0;

	QTextStream out(stderr);
	const QString sEol = "\n\n";
	const int argc = args.count();

	for (int i = 1; i < argc; ++i) {

		QString sArg = args.at(i);

		if (iCmdArgs > 0) {
			sessionFiles.append(QFileInfo(sArg).absoluteFilePath());
			++iCmdArgs;
			continue;
		}

		if (sArg == "-h" || sArg == "--help") {
			print_usage(args.at(0));
			return false;
		}
		else if (sArg == "-v" || sArg == "--version") {
			out << QString("%1: %2\n")
				.arg(QXGEDIT_TITLE)
				.arg(PROJECT_VERSION);
			out << QString("Qt: %1").arg(qVersion());
		#if defined(QT_STATIC)
			out << "-static";
		#endif
			out << '\n';
			return false;
		} else {
			// If we don't have one by now,
			// this will be the startup session file...
			sessionFiles.append(QFileInfo(sArg).absoluteFilePath());
			++iCmdArgs;
		}
	}

#endif

	// Alright with argument parsing.
	return true;
}


//---------------------------------------------------------------------------
// Widget geometry persistence helper methods.

void qxgeditOptions::loadWidgetGeometry ( QWidget *pWidget, bool bVisible )
{
	// Try to restore old form window positioning.
	if (pWidget) {
	//	if (bVisible) pWidget->show(); -- force initial exposure?
		m_settings.beginGroup("/Geometry/" + pWidget->objectName());
	#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		const QByteArray& geometry
			= m_settings.value("/geometry").toByteArray();
		if (!geometry.isEmpty())
			pWidget->restoreGeometry(geometry);
		if (geometry.isEmpty()) {
			QWidget *pParent = pWidget->parentWidget();
			if (pParent)
				pParent = pParent->window();
		#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			if (pParent == nullptr)
				pParent = QApplication::desktop();
		#endif
			if (pParent) {
				QRect wrect(pWidget->geometry());
				wrect.moveCenter(pParent->geometry().center());
				pWidget->move(wrect.topLeft());
			}
		} else {
			pWidget->restoreGeometry(geometry);
		}
	#else//--LOAD_OLD_GEOMETRY
		QPoint wpos;
		QSize  wsize;
		wpos.setX(m_settings.value("/x", -1).toInt());
		wpos.setY(m_settings.value("/y", -1).toInt());
		wsize.setWidth(m_settings.value("/width", -1).toInt());
		wsize.setHeight(m_settings.value("/height", -1).toInt());
		if (wpos.x() > 0 && wpos.y() > 0)
			pWidget->move(wpos);
		if (wsize.width() > 0 && wsize.height() > 0)
			pWidget->resize(wsize);
		else
	#endif
	//	pWidget->adjustSize();
		if (!bVisible)
			bVisible = m_settings.value("/visible", false).toBool();
		if (bVisible)
			pWidget->show();
		else
			pWidget->hide();
		m_settings.endGroup();
	}
}


void qxgeditOptions::saveWidgetGeometry ( QWidget *pWidget, bool bVisible )
{
	// Try to save form window position...
	// (due to X11 window managers ideossincrasies, we better
	// only save the form geometry while its up and visible)
	if (pWidget) {
		m_settings.beginGroup("/Geometry/" + pWidget->objectName());
	#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		m_settings.setValue("/geometry", pWidget->saveGeometry());
	#else//--SAVE_OLD_GEOMETRY
		const QPoint& wpos  = pWidget->pos();
		const QSize&  wsize = pWidget->size();
		m_settings.setValue("/x", wpos.x());
		m_settings.setValue("/y", wpos.y());
		m_settings.setValue("/width", wsize.width());
		m_settings.setValue("/height", wsize.height());
	#endif
		if (!bVisible) bVisible = pWidget->isVisible();
		m_settings.setValue("/visible", bVisible);
		m_settings.endGroup();
	}
}


// end of qxgeditOptions.cpp
