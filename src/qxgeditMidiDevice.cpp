// qxgeditMidiDevice.cpp
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

#include "qxgeditAbout.h"
#include "qxgeditMidiDevice.h"

#include <QThread>
#include <QApplication>


//----------------------------------------------------------------------
// class qxgeditMidiInputThread -- MIDI input thread (singleton).
//

class qxgeditMidiInputThread : public QThread
{
public:

	// Constructor.
	qxgeditMidiInputThread(qxgeditMidiDevice *pMidiDevice)
		: QThread(), m_pMidiDevice(pMidiDevice), m_bRunState(false) {}

	// Run-state accessors.
	void setRunState(bool bRunState)
		{ m_bRunState = bRunState; }
	bool runState() const
		{ return m_bRunState; }

protected:

	// The main thread executive.
	void run()
	{
		snd_seq_t *pAlsaSeq = m_pMidiDevice->alsaSeq();
		if (pAlsaSeq == NULL)
			return;

		int nfds;
		struct pollfd *pfds;

		nfds = snd_seq_poll_descriptors_count(pAlsaSeq, POLLIN);
		pfds = (struct pollfd *) alloca(nfds * sizeof(struct pollfd));
		snd_seq_poll_descriptors(pAlsaSeq, pfds, nfds, POLLIN);

		m_bRunState = true;

		int iPoll = 0;
		while (m_bRunState && iPoll >= 0) {
			// Wait for events...
			iPoll = poll(pfds, nfds, 200);
			while (iPoll > 0) {
				snd_seq_event_t *pEv = NULL;
				snd_seq_event_input(pAlsaSeq, &pEv);
				// Process input event - ...
				// - enqueue to input track mapping;
				m_pMidiDevice->capture(pEv);
			//	snd_seq_free_event(pEv);
				iPoll = snd_seq_event_input_pending(pAlsaSeq, 0);
			}
		}
	}

private:

	// The thread launcher engine.
	qxgeditMidiDevice *m_pMidiDevice;

	// Whether the thread is logically running.
	bool m_bRunState;
};


//----------------------------------------------------------------------------
// qxgeditMidiDevice -- MIDI Device interface object.

// Pseudo-singleton reference.
qxgeditMidiDevice *qxgeditMidiDevice::g_pMidiDevice = NULL;

// Constructor.
qxgeditMidiDevice::qxgeditMidiDevice ( const QString& sClientName )
	: QObject(NULL)
{
	// Set pseudo-singleton reference.
	g_pMidiDevice = this;

	m_pAlsaSeq    = NULL;
	m_iAlsaClient = -1;
	m_iAlsaPort   = -1;

	m_pInputThread = NULL;

	// Open new ALSA sequencer client...
	if (snd_seq_open(&m_pAlsaSeq, "hw", SND_SEQ_OPEN_DUPLEX, 0) >= 0) {
		// Set client identification...
		QString sName = sClientName;
		snd_seq_set_client_name(m_pAlsaSeq, sName.toAscii().constData());
		m_iAlsaClient = snd_seq_client_id(m_pAlsaSeq);
		// Create duplex port
		sName += " MIDI 1";
		m_iAlsaPort = snd_seq_create_simple_port(m_pAlsaSeq,
			sName.toAscii().constData(),
			SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE |
			SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
			SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);
		// Create and start our own MIDI input queue thread...
		m_pInputThread = new qxgeditMidiInputThread(this);
		m_pInputThread->start(QThread::TimeCriticalPriority);
	}
}


qxgeditMidiDevice::~qxgeditMidiDevice (void)
{
	// Reset pseudo-singleton reference.
	g_pMidiDevice = NULL;

	// Last but not least, delete input thread...
	if (m_pInputThread) {
		// Try to terminate executive thread,
		// but give it a bit of time to cleanup...
		if (m_pInputThread->isRunning()) {
			m_pInputThread->setRunState(false);
		//	m_pInputThread->terminate();
			m_pInputThread->wait();
		}
		delete m_pInputThread;
		m_pInputThread = NULL;
	}

	if (m_pAlsaSeq) {
		snd_seq_delete_simple_port(m_pAlsaSeq, m_iAlsaPort);
		m_iAlsaPort   = -1;
		snd_seq_close(m_pAlsaSeq);
		m_iAlsaClient = -1;
		m_pAlsaSeq    = NULL;
	}
}


// Pseudo-singleton reference (static).
qxgeditMidiDevice *qxgeditMidiDevice::getInstance (void)
{
	return g_pMidiDevice;
}


// ALSA sequencer client descriptor accessor.
snd_seq_t *qxgeditMidiDevice::alsaSeq (void) const
{
	return m_pAlsaSeq;
}

int qxgeditMidiDevice::alsaClient (void) const
{
	return m_iAlsaClient;
}

int qxgeditMidiDevice::alsaPort (void) const
{
	return m_iAlsaPort;
}


// Event notifier widget settings.
void qxgeditMidiDevice::setNotifyObject ( QObject *pNotifyObject )
{
	m_pNotifyObject = pNotifyObject;
}

QObject *qxgeditMidiDevice::notifyObject (void) const
{
	return m_pNotifyObject;
}

void qxgeditMidiDevice::setNotifySysexType ( QEvent::Type eNotifySysexType )
{
	m_eNotifySysexType = eNotifySysexType;
}

QEvent::Type qxgeditMidiDevice::notifySysexType (void) const
{
	return m_eNotifySysexType;
}


// MIDI event capture method.
void qxgeditMidiDevice::capture ( snd_seq_event_t *pEv )
{
	// Must be to ourselves...
	if (pEv->dest.port |= m_iAlsaPort)
		return;

#ifdef CONFIG_DEBUG
	// - show event for debug purposes...
	fprintf(stderr, "MIDI In  %06lu 0x%02x", (unsigned long) pEv->time.tick, pEv->type);
	if (pEv->type == SND_SEQ_EVENT_SYSEX) {
		fprintf(stderr, " sysex {");
		unsigned char *data = (unsigned char *) pEv->data.ext.ptr;
		for (unsigned int i = 0; i < pEv->data.ext.len; i++)
			fprintf(stderr, " %02x", data[i]);
		fprintf(stderr, " }\n");
	} else {
		for (unsigned int i = 0; i < sizeof(pEv->data.raw8.d); i++)
			fprintf(stderr, " %3d", pEv->data.raw8.d[i]);
		fprintf(stderr, "\n");
	}
#endif

	// Post SysEx event...
	if (pEv->type == SND_SEQ_EVENT_SYSEX && m_pNotifyObject) {
		// Post the stuffed event...
		QApplication::postEvent(m_pNotifyObject,
			new qxgeditMidiSysexEvent(m_eNotifySysexType,
				(unsigned char *) pEv->data.ext.ptr,
				(unsigned short)  pEv->data.ext.len));
	}
}


void qxgeditMidiDevice::sendSysex (
	unsigned char *pSysex, unsigned short iSysex ) const
{
	// Don't do anything else if engine
	// has not been activated...
	if (m_pAlsaSeq == NULL)
		return;

#ifdef CONFIG_DEBUG
	fprintf(stderr, "qxgeditMidiDevice::sendSysex(%p, %u)", pSysex, iSysex);
	fprintf(stderr, " sysex {");
	for (unsigned short i = 0; i < iSysex; ++i)
		fprintf(stderr, " %02x", pSysex[i]);
	fprintf(stderr, " }\n");
#endif

	// Initialize sequencer event...
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);

	// Addressing...
	snd_seq_ev_set_source(&ev, m_iAlsaPort);
	snd_seq_ev_set_subs(&ev);

	// The event will be direct...
	snd_seq_ev_set_direct(&ev);

	// Just set SYSEX stuff and send it out..
	ev.type = SND_SEQ_EVENT_SYSEX;
	snd_seq_ev_set_sysex(&ev, iSysex, pSysex);
	snd_seq_event_output_direct(m_pAlsaSeq, &ev);
}


// MIDI Input(readable) / Output(writable) device list.
static const char *c_pszItemSep = " / ";

QStringList qxgeditMidiDevice::deviceList ( bool bReadable ) const
{
	QStringList list;

	if (m_pAlsaSeq == NULL)
		return list;

	unsigned int uiPortFlags;
	if (bReadable)
		uiPortFlags = SND_SEQ_PORT_CAP_READ  | SND_SEQ_PORT_CAP_SUBS_READ;
	else
		uiPortFlags = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;

	snd_seq_client_info_t *pClientInfo;
	snd_seq_port_info_t   *pPortInfo;

	snd_seq_client_info_alloca(&pClientInfo);
	snd_seq_port_info_alloca(&pPortInfo);
	snd_seq_client_info_set_client(pClientInfo, -1);

	while (snd_seq_query_next_client(m_pAlsaSeq, pClientInfo) >= 0) {
		int iAlsaClient = snd_seq_client_info_get_client(pClientInfo);
		if (iAlsaClient > 0 && iAlsaClient != m_iAlsaClient) {
			snd_seq_port_info_set_client(pPortInfo, iAlsaClient);
			snd_seq_port_info_set_port(pPortInfo, -1);
			while (snd_seq_query_next_port(m_pAlsaSeq, pPortInfo) >= 0) {
				unsigned int uiPortCapability
					= snd_seq_port_info_get_capability(pPortInfo);
				if (((uiPortCapability & uiPortFlags) == uiPortFlags) &&
					((uiPortCapability & SND_SEQ_PORT_CAP_NO_EXPORT) == 0)) {
					int iAlsaPort = snd_seq_port_info_get_port(pPortInfo);
					QString sItem = QString::number(iAlsaClient) + ':';
					sItem += snd_seq_client_info_get_name(pClientInfo);
					sItem += c_pszItemSep;
					sItem += QString::number(iAlsaPort) + ':';
					sItem += snd_seq_port_info_get_name(pPortInfo);
					list.append(sItem);
				}
			}
		}
	}

	return list;
}


// MIDI Input(readable) / Output(writable) device connects.
bool qxgeditMidiDevice::connectDeviceList (
	bool bReadable, const QStringList& list ) const
{
	if (m_pAlsaSeq == NULL)
		return false;

	if (list.isEmpty())
		return false;

	snd_seq_addr_t seq_addr;
	snd_seq_port_subscribe_t *pPortSubs;

	snd_seq_port_subscribe_alloca(&pPortSubs);

	snd_seq_client_info_t *pClientInfo;
	snd_seq_port_info_t   *pPortInfo;

	snd_seq_client_info_alloca(&pClientInfo);
	snd_seq_port_info_alloca(&pPortInfo);

	unsigned int uiPortFlags;
	if (bReadable)
		uiPortFlags = SND_SEQ_PORT_CAP_READ  | SND_SEQ_PORT_CAP_SUBS_READ;
	else
		uiPortFlags = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;

	int iConnects = 0;
	while (snd_seq_query_next_client(m_pAlsaSeq, pClientInfo) >= 0) {
		int iAlsaClient = snd_seq_client_info_get_client(pClientInfo);
		if (iAlsaClient > 0 && iAlsaClient != m_iAlsaClient) {
			QString sClientName = snd_seq_client_info_get_name(pClientInfo);
			snd_seq_port_info_set_client(pPortInfo, iAlsaClient);
			snd_seq_port_info_set_port(pPortInfo, -1);
			while (snd_seq_query_next_port(m_pAlsaSeq, pPortInfo) >= 0) {
				unsigned int uiPortCapability
					= snd_seq_port_info_get_capability(pPortInfo);
				if (((uiPortCapability & uiPortFlags) == uiPortFlags) &&
					((uiPortCapability & SND_SEQ_PORT_CAP_NO_EXPORT) == 0)) {
					int iAlsaPort = snd_seq_port_info_get_port(pPortInfo);
					QString sPortName = snd_seq_port_info_get_name(pPortInfo);
					QStringListIterator iter(list);
					while (iter.hasNext()) {
						const QString& sItem = iter.next();
						const QString& sClientItem
							= sItem.section(c_pszItemSep, 0, 0);
						const QString& sPortItem
							= sItem.section(c_pszItemSep, 1, 1);
						if (sClientName != sClientItem.section(':', 1, 1))
							continue;
						if (sPortName != sPortItem  .section(':', 1, 1))
							continue;
						if (bReadable) {
							seq_addr.client = iAlsaClient;
							seq_addr.port   = iAlsaPort;
							snd_seq_port_subscribe_set_sender(pPortSubs, &seq_addr);
							seq_addr.client = m_iAlsaClient;
							seq_addr.port   = m_iAlsaPort;
							snd_seq_port_subscribe_set_dest(pPortSubs, &seq_addr);
						} else {
							seq_addr.client = m_iAlsaClient;
							seq_addr.port   = m_iAlsaPort;
							snd_seq_port_subscribe_set_sender(pPortSubs, &seq_addr);
							seq_addr.client = iAlsaClient;
							seq_addr.port   = iAlsaPort;
							snd_seq_port_subscribe_set_dest(pPortSubs, &seq_addr);
						}
						if (snd_seq_subscribe_port(m_pAlsaSeq, pPortSubs) == 0)
							iConnects++;
					}
				}
			}
		}
	}

	return (iConnects > 0);
}


// end of qxgeditMidiDevice.cpp
