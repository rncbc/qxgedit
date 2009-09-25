// qxgeditXGMasterMap.cpp
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
#include "qxgeditXGMasterMap.h"

#include "qxgeditMidiDevice.h"

#include "qxgeditMainForm.h"

#include "XGParamSysex.h"


//----------------------------------------------------------------------------
// qxgeditXGMasterMap::Observer -- XGParam master map observer.

// Constructor.
qxgeditXGMasterMap::Observer::Observer ( XGParam *pParam )
	: XGParamObserver(pParam)
{
}

// View updater (observer callback).
void qxgeditXGMasterMap::Observer::reset (void)
{
}

void qxgeditXGMasterMap::Observer::update (void)
{
	qxgeditXGMasterMap *pMasterMap = qxgeditXGMasterMap::getInstance();
	if (pMasterMap == NULL)
		return;

	XGParam *pParam = param();

#ifdef CONFIG_DEBUG_0
	qDebug("qxgeditXGMasterMap::Observer[%p]::update() [%02x %02x %02x %s %u]",
		this, pParam->high(), pParam->mid(), pParam->low(),
		pParam->text().toUtf8().constData(), pParam->value());
#endif

	if (pParam->high() == 0x11) {
		// Special USERVOICE bulk dump stuff...
		unsigned short iUser = pMasterMap->USERVOICE.current_key(); 
		pMasterMap->set_user_dirty(iUser, true);
		if (pMasterMap->auto_send()) {
			pMasterMap->send_user(iUser);
			pMasterMap->set_user_dirty_1(iUser, false);
		}
	} else {
		// Regular XG Parameter change...
		pMasterMap->send_param(pParam);
	}

	// HACK: Flag dirty the main form...
	qxgeditMainForm *pMainForm = qxgeditMainForm::getInstance();
	if (pMainForm)
		pMainForm->contentsChanged();
}


//----------------------------------------------------------------------------
// qxgeditXGMasterMap -- XGParam master map.
//

// Constructor.
qxgeditXGMasterMap::qxgeditXGMasterMap (void)
	: XGParamMasterMap(), m_auto_send(false)
{
	// Setup local observers...
	XGParamMasterMap::const_iterator iter
		= XGParamMasterMap::constBegin();
	for (; iter != XGParamMasterMap::constEnd(); ++iter) {
		XGParam *pParam = iter.value();
		m_observers.insert(pParam, new Observer(pParam));
	}

	reset_part_dirty();
	reset_user_dirty();
}


// Destructor.
qxgeditXGMasterMap::~qxgeditXGMasterMap (void)
{
	// Cleanup local observers...
	// qDeleteAll(m_observers);
	ObserverMap::const_iterator iter = m_observers.constBegin();
	for (; iter != m_observers.constEnd(); ++iter)
		delete iter.value();
	m_observers.clear();
}


// Singleton (re)cast.
qxgeditXGMasterMap *qxgeditXGMasterMap::getInstance (void)
{
	return static_cast<qxgeditXGMasterMap *> (XGParamMasterMap::getInstance());
}


// Direct SysEx data receiver.
bool qxgeditXGMasterMap::set_sysex_data (
	unsigned char *data, unsigned short len, bool bNotify )
{
	 // SysEx (actually)...
	if (data[0] != 0xf0 || data[len - 1] != 0xf7)
		return false;

	// Yamaha ID...
	if (data[1] != 0x43)
		return false;

	bool ret = false;
	unsigned short i;

	unsigned char mode  = (data[2] & 0x70);
//	unsigned char devno = (data[2] & 0x0f);
	if (data[3] == 0x4c || data[3] == 0x4b) {
		// XG/QS300 Model ID...
		if (mode == 0x00) {
			// Native Bulk Dump...
			unsigned short size = (data[4] << 7) + data[5];
			unsigned char cksum = 0;
			for (i = 0; i < size + 5; ++i) {
				cksum += data[4 + i];
				cksum &= 0x7f;
			}
			if (data[9 + size] == 0x80 - cksum) {
				unsigned short high = data[6];
				unsigned short mid  = data[7];
				unsigned short low  = data[8];
				for (i = 0; i < size; ++i) {
					// Parameter Change...
					unsigned short n = set_param_data(
						high, mid, low + i, &data[9 + i], bNotify);
					if (n > 1)
						i += (n - 1);
				}
				ret = (i == size);
			}
		}
		else
		if (mode == 0x10) {
			// Parameter Change...
			unsigned short high = data[4];
			unsigned short mid  = data[5];
			unsigned short low  = data[6];
			ret = (set_param_data(high, mid, low, &data[7], bNotify) > 0);
		}
	}
	
	return ret;
}


// Direct parameter data access.
unsigned short qxgeditXGMasterMap::set_param_data (
	unsigned short high, unsigned short mid, unsigned short low,
	unsigned char *data, bool bNotify )
{
	XGParam *pParam = find_param(high, mid, low);
	if (pParam == NULL)
		return 0;

	if (!m_observers.contains(pParam))
		return 0;

	if (high == 0x08 && low >= 0x01 && 0x03 >= low) {
		set_part_dirty(mid, true);
		bNotify = false;
	}
	else
	if (high == 0x11) {
		set_user_dirty(mid, true);
		bNotify = false;
	}

	Observer *pObserver = (bNotify ? NULL : m_observers.value(pParam));
	if (pParam->size() > 4) {
		XGDataParam *pDataParam = static_cast<XGDataParam *> (pParam);
		pDataParam->set_data(data, pDataParam->size(), pObserver);
	} else {
		pParam->set_value(pParam->data_value(data), pObserver);
	}

#ifdef CONFIG_DEBUG
	fprintf(stderr, "< %02x %02x %02x",
		pParam->high(),
		pParam->mid(),
		pParam->low());
	const char *name = pParam->name();
	if (name) {
		unsigned short c = pParam->value();
		const char *s = pParam->gets(c);
		const char *u = pParam->unit();
		fprintf(stderr, " %s", pParam->text().toUtf8().constData());
		fprintf(stderr, " %g", pParam->getv(c));
		if (s) fprintf(stderr, " %s", s);
		if (u) fprintf(stderr, " %s", u);
	}
	fprintf(stderr, " >\n");
#endif

	return pParam->size();
}


// All parameter reset (to default)
void qxgeditXGMasterMap::reset_all (void)
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGMasterMap::reset_all()");
#endif

	ObserverMap::const_iterator iter = m_observers.constBegin();
	for (; iter != m_observers.constEnd(); ++iter) {
		XGParam *pParam = iter.key();
		pParam->reset(iter.value());
	}

	reset_part_dirty();
	reset_user_dirty();
}


// Part reset (to default)
void qxgeditXGMasterMap::reset_part ( unsigned short iPart )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGMasterMap::reset_part(%u)", iPart);
#endif

	XGParamMap::const_iterator iter = MULTIPART.constBegin();
	for (; iter != MULTIPART.constEnd(); ++iter) {
		XGParamSet *pParamSet = iter.value();
		if (pParamSet->contains(iPart)) {
			XGParam *pParam = pParamSet->value(iPart);
			if (pParam)
				pParam->reset();
		}
	}

	if (part_dirty(iPart)) {
		send_part(iPart);
		set_part_dirty(iPart, false);
	}
}


// Drums reset (to default)
void qxgeditXGMasterMap::reset_drums ( unsigned short iDrumSet )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGMasterMap::reset_drums(%u)", iDrumSet);
#endif

	unsigned short high = 0x30 + iDrumSet;
	ObserverMap::const_iterator iter = m_observers.constBegin();
	for (; iter != m_observers.constEnd(); ++iter) {
		XGParam *pParam = iter.key();
		if (pParam->high() == high)
			pParam->reset(iter.value());
	}
}


// user voice reset (to default)
void qxgeditXGMasterMap::reset_user ( unsigned short iUser )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGMasterMap::reset_user(%u)", iUser);
#endif

	// Suspend auto-send temporarily...
	bool bAuto = auto_send();
	set_auto_send(false);

	XGParamMap::const_iterator iter = USERVOICE.constBegin();
	for (; iter != USERVOICE.constEnd(); ++iter) {
		XGParamSet *pParamSet = iter.value();
		if (pParamSet->contains(iUser)) {
			XGParam *pParam = pParamSet->value(iUser);
			if (pParam)
				pParam->reset();
		}
	}

	if (user_dirty(iUser)) {
		send_user(iUser);
		set_user_dirty(iUser, false);
	}

	// Restore auto-send.
	set_auto_send(bAuto);
}


// Send regular XG Parameter change SysEx message.
void qxgeditXGMasterMap::send_param ( XGParam *pParam )
{
	if (pParam == NULL)
		return;

	qxgeditMidiDevice *pMidiDevice = qxgeditMidiDevice::getInstance();
	if (pMidiDevice == NULL)
		return;

	// Build the complete SysEx message...
	XGParamSysex sysex(pParam);
	// Send it out...
	pMidiDevice->sendSysex(sysex.data(), sysex.size());

	// HACK: Special reset actions...
	if (pParam->high() == 0x00 && pParam->mid() == 0x00) {
		switch (pParam->low()) {
		case 0x7d: // Drum Setup Reset
			reset_drums(pParam->value());
			break;
		case 0x7e: // XG System On
		case 0x7f: // All Parameter Reset
			reset_all();
			break;
		}
	}
}


// Send Multi Part Bank Select/Program Number SysEx messages.
void qxgeditXGMasterMap::send_part ( unsigned short iPart ) const
{
	qxgeditMidiDevice *pMidiDevice = qxgeditMidiDevice::getInstance();
	if (pMidiDevice == NULL)
		return;

	unsigned short high = 0x08;
	unsigned short mid  = iPart;

	for (unsigned short low = 0x01; low < 0x04; ++low) {
		XGParam *pParam = find_param(high, mid, low);
		if (pParam) {
			// Build the complete SysEx message...
			XGParamSysex sysex(pParam);
			// Send it out...
			pMidiDevice->sendSysex(sysex.data(), sysex.size());
		}
	}
}


// Send USER VOICE Bulk Dump SysEx message.
void qxgeditXGMasterMap::send_user ( unsigned short iUser ) const
{
	qxgeditMidiDevice *pMidiDevice = qxgeditMidiDevice::getInstance();
	if (pMidiDevice == NULL)
		return;

	// Build the complete SysEx message...
	XGUserVoiceSysex sysex(iUser);
	// Send it out...
	pMidiDevice->sendSysex(sysex.data(), sysex.size());
}


// MULTIPART dirty slot simple managing.
void qxgeditXGMasterMap::reset_part_dirty (void)
{
	for (unsigned short i = 0; i < 32; ++i)
		m_part_dirty[i] = 0;
}

void qxgeditXGMasterMap::set_part_dirty ( unsigned short iPart, bool bDirty )
{
	if (iPart < 16) {
		if (bDirty)
			m_part_dirty[iPart] |=  1;
		else
			m_part_dirty[iPart] &= ~1;
	}
}

bool qxgeditXGMasterMap::part_dirty ( unsigned short iPart ) const
{
	return (iPart < 16) && (m_part_dirty[iPart] != 0);
}


// (QS300) USERVOICE dirty slot simple managing.
void qxgeditXGMasterMap::reset_user_dirty (void)
{
	for (unsigned short i = 0; i < 32; ++i)
		m_user_dirty[i] = 0;
}

void qxgeditXGMasterMap::set_user_dirty ( unsigned short iUser, bool bDirty )
{
	if (iUser < 32) {
		if (bDirty)
			m_user_dirty[iUser] |=  7;
		else
			m_user_dirty[iUser] &= ~7;
	}
}

bool qxgeditXGMasterMap::user_dirty ( unsigned short iUser ) const
{
	return (iUser < 32) && (m_user_dirty[iUser] & 7);
}


void qxgeditXGMasterMap::set_user_dirty_1 ( unsigned short iUser, bool bDirty )
{
	if (iUser < 32) {
		if (bDirty)
			m_user_dirty[iUser] |=  1;
		else
			m_user_dirty[iUser] &= ~1;
	}
}

bool qxgeditXGMasterMap::user_dirty_1 ( unsigned short iUser ) const
{
	return (iUser < 32) && (m_user_dirty[iUser] & 1);
}


void qxgeditXGMasterMap::set_user_dirty_2 ( unsigned short iUser, bool bDirty )
{
	if (iUser < 32) {
		if (bDirty)
			m_user_dirty[iUser] |=  2;
		else
			m_user_dirty[iUser] &= ~2;
	}
}

bool qxgeditXGMasterMap::user_dirty_2 ( unsigned short iUser ) const
{
	return (iUser < 32) && (m_user_dirty[iUser] & 2);
}


// (QS300) USERVOICE bulk dump auto-send feature.
void qxgeditXGMasterMap::set_auto_send ( bool bAuto )
{
	m_auto_send = bAuto;
}

bool qxgeditXGMasterMap::auto_send (void) const
{
	return m_auto_send;
}


// Part randomize (from value/def)
void qxgeditXGMasterMap::randomize_part ( unsigned short iPart, int p )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGMasterMap::randomize_part(%u, %d)", iPart, p);
#endif

	XGParamMap::const_iterator iter = MULTIPART.constBegin();
	for (; iter != MULTIPART.constEnd(); ++iter) {
		XGParamSet *pParamSet = iter.value();
		if (pParamSet->contains(iPart)) {
			XGParam *pParam = pParamSet->value(iPart);
			if (pParam && pParam->low() > 0x04)
				pParam->randomize_value(p);
		}
	}

	if (part_dirty(iPart)) {
		send_part(iPart);
		set_part_dirty(iPart, false);
	}
}


// Drums randomize (from value/def)
void qxgeditXGMasterMap::randomize_drums (
	unsigned short iDrumSet, unsigned short iDrumKey, int p )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGMasterMap::randomize_drums(%u, %u, %d)", iDrumSet, iDrumKey, p);
#endif

	unsigned short key = (unsigned short) (iDrumSet << 7) + iDrumKey;
	XGParamMap::const_iterator iter = DRUMSETUP.constBegin();
	for (; iter != DRUMSETUP.constEnd(); ++iter) {
		XGParamSet *pParamSet = iter.value();
		if (pParamSet->contains(key)) {
			XGParam *pParam = pParamSet->value(key);
			if (pParam)
				pParam->randomize_value(p);
		}
	}
}


// User voice randomize (from value/def)
void qxgeditXGMasterMap::randomize_user ( unsigned short iUser, int p )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGMasterMap::randomize_user(%u, %d)", iUser, p);
#endif

	// Suspend auto-send temporarily...
	bool bAuto = auto_send();
	set_auto_send(false);

	unsigned short id0 = 0x3d + (USERVOICE.current_element() * 0x50);
	XGParamMap::const_iterator iter = USERVOICE.constBegin();
	for (; iter != USERVOICE.constEnd(); ++iter) {
		unsigned short id = iter.key();
		if (id < 0x3d)
			continue;
		if (USERVOICE.elements() > 0 && (id < id0 || id >= id0 + 0x50))
			continue;
		XGParamSet *pParamSet = iter.value();
		if (pParamSet->contains(iUser)) {
			XGParam *pParam = pParamSet->value(iUser);
			if (pParam)
				pParam->randomize_value(p);
		}
	}

	if (user_dirty(iUser)) {
		send_user(iUser);
		set_user_dirty_1(iUser, false);
	}

	// Restore auto-send.
	set_auto_send(bAuto);
}


// end of qxgeditXGMasterMap.cpp
