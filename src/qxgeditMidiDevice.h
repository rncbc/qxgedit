// qxgeditMidiDevice.h
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

#ifndef __qxgeditMidiDevice_h
#define __qxgeditMidiDevice_h

#include <QObject>
#include <QEvent>
#include <QByteArray>

#include <alsa/asoundlib.h>


// Forward declarations.
class qxgeditMidiInputThread;


//----------------------------------------------------------------------------
// qxgeditMidiDevice -- MIDI Device interface object.

class qxgeditMidiDevice : public QObject
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditMidiDevice(const QString& sClientName);
	// Destructor.
	~qxgeditMidiDevice();

	// Pseudo-singleton reference.
	static qxgeditMidiDevice *getInstance();

	// ALSA client descriptor accessor.
	snd_seq_t *alsaSeq() const;
	int alsaClient() const;
	int alsaPort() const;

	// Event notifier widget settings.
	void setNotifyObject(QObject *pNotifyObject);
	QObject *notifyObject() const;

	void setNotifySysexType(QEvent::Type eNotifySysexType);
	QEvent::Type notifySysexType() const;

	// MIDI event capture method.
	void capture(snd_seq_event_t *pEv);

	// MIDI SysEx sender.
	void sendSysex(unsigned char *pSysex, unsigned short iSysex) const;

private:

	// Instance variables.
	snd_seq_t *m_pAlsaSeq;
	int        m_iAlsaClient;
	int        m_iAlsaPort;

	// The event notifier widget.
	QObject     *m_pNotifyObject;
	QEvent::Type m_eNotifySysexType;

	// Name says it all.
	qxgeditMidiInputThread *m_pInputThread;

	// Pseudo-singleton reference.
	static qxgeditMidiDevice *g_pMidiDevice;
};


//----------------------------------------------------------------------
// qxgeditMidiSysexEvent - MIDI SysEx custom event.
//

class qxgeditMidiSysexEvent : public QEvent
{
public:

	// Contructor.
	qxgeditMidiSysexEvent(QEvent::Type eType,
		unsigned char *pSysex, unsigned short iSysex)
		: QEvent(eType), m_data((const char *) pSysex, (int) iSysex) {}

	// Accessors.
	unsigned char *data() const
		{ return (unsigned char *) m_data.constData(); }
	unsigned short len() const
		{ return (unsigned short) m_data.length(); }

private:

	// Instance variables.
	QByteArray m_data;
};


#endif	// __qxgeditMidiDevice_h


// end of qxgeditMidiDevice.h
