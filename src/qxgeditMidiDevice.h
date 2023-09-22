// qxgeditMidiDevice.h
//
/****************************************************************************
   Copyright (C) 2005-2023, rncbc aka Rui Nuno Capela. All rights reserved.

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
#include <QStringList>



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

	// MIDI SysEx sender.
	void sendSysex(const QByteArray& sysex) const;
	void sendSysex(unsigned char *pSysex, unsigned short iSysex) const;

	// MIDI Input(readable) / Output(writable) device list
	QStringList inputs() const;
	QStringList outputs() const;

	// MIDI Input(readable) / Output(writable) connects.
	bool connectInputs(const QStringList& inputs) const;
	bool connectOutputs(const QStringList& outputs) const;

	// Emit received data signals.
	void emitReceiveRpn(unsigned char ch, unsigned short rpn, unsigned short val)
		{ emit receiveRpn(ch, rpn, val); }
	void emitReceiveNrpn(unsigned char ch, unsigned short nrpn, unsigned short val)
		{ emit receiveNrpn(ch, nrpn, val); }
	void emitReceiveSysex(const QByteArray& sysex)
		{ emit receiveSysex(sysex); }

	// Forward decl.
	class Impl;

signals:

	// Received data signals.
	void receiveRpn(unsigned char ch, unsigned short rpn, unsigned short val);
	void receiveNrpn(unsigned char ch, unsigned short nrpn, unsigned short val);
	void receiveSysex(const QByteArray& sysex);

private:

	// Name says it all.
	Impl *m_pImpl;

	// Pseudo-singleton reference.
	static qxgeditMidiDevice *g_pMidiDevice;
};


#endif	// __qxgeditMidiDevice_h


// end of qxgeditMidiDevice.h
