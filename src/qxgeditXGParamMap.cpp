// qxgeditXGParamMap.cpp
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
#include "qxgeditXGParamMap.h"

#include "qxgeditMidiDevice.h"

#include "qxgeditMainForm.h"

#include "XGParamSysex.h"


//----------------------------------------------------------------------------
// qxgeditXGParamMap::Observer -- XGParam master map observer.

// Constructor.
qxgeditXGParamMap::Observer::Observer ( XGParam *pParam )
	: XGParamObserver(pParam)
{
}

// View updater (observer callback).
void qxgeditXGParamMap::Observer::reset (void)
{
}

void qxgeditXGParamMap::Observer::update (void)
{
	qxgeditXGParamMap *pParamMap
		= static_cast<qxgeditXGParamMap *> (
			XGParamMasterMap::getInstance());
	if (pParamMap == NULL)
		return;

	XGParam *pParam = param();
	if (pParam->high() == 0x11) {
		// Special USERVOICE bulk dump stuff...
		unsigned short iUser = pParamMap->USERVOICE.current_key(); 
		if (pParamMap->auto_send())
			pParamMap->send_user(iUser);
		pParamMap->set_user_dirty(iUser, true);
	} else {
		// regular XG Parameter change...
		pParamMap->send_param(pParam);
	}

	// HACK: Flag dirty the main form...
	qxgeditMainForm *pMainForm = qxgeditMainForm::getInstance();
	if (pMainForm)
		pMainForm->contentsChanged();
}


//----------------------------------------------------------------------------
// qxgeditXGParamMap -- XGParam master map.
//

// Constructor.
qxgeditXGParamMap::qxgeditXGParamMap (void)
	: XGParamMasterMap(), m_auto_send(false)
{
	// Setup local observers...
	XGParamMasterMap::const_iterator iter
		= XGParamMasterMap::constBegin();
	for (; iter != XGParamMasterMap::constEnd(); ++iter) {
		XGParam *pParam = iter.value();
		m_observers.insert(pParam, new Observer(pParam));
	}

	reset_user_dirty();
}


// Destructor.
qxgeditXGParamMap::~qxgeditXGParamMap (void)
{
	// Cleanup local observers...
	// qDeleteAll(m_observers);
	ObserverMap::const_iterator iter = m_observers.constBegin();
	for (; iter != m_observers.constEnd(); ++iter)
		delete iter.value();
	m_observers.clear();
}


// Direct SysEx data receiver.
bool qxgeditXGParamMap::set_sysex_data (
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
						high, mid, low + i, &data[9 + i],
							(bNotify && high != 0x11));
					if (n > 1)
						i += (n - 1);
				}
				ret = (i == (size - 6));
				// Deferred QS300 Bulk Dump feedback...
				if (bNotify && high == 0x11)
					send_user(mid);
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
unsigned short qxgeditXGParamMap::set_param_data (
	unsigned short high, unsigned short mid, unsigned short low,
	unsigned char *data, bool bNotify )
{
	XGParam *pParam = find_param(high, mid, low);
	if (pParam == NULL)
		return 0;

	if (!m_observers.contains(pParam))
		return 0;

	Observer *pObserver = (bNotify ? NULL : m_observers.value(pParam));
	if (pParam->size() > 4) {
		XGDataParam *pDataParam = static_cast<XGDataParam *> (pParam);
		pDataParam->set_data(data, pDataParam->size(), pObserver);
	} else {
		pParam->set_value(pParam->data_value(data), pObserver);
	}

	if (high == 0x11)
		set_user_dirty(mid, true);

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
void qxgeditXGParamMap::reset_all (void)
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGParamMap::reset_all()");
#endif

	ObserverMap::const_iterator iter = m_observers.constBegin();
	for (; iter != m_observers.constEnd(); ++iter) {
		XGParam *pParam = iter.key();
		pParam->reset(iter.value());
	}

	reset_user_dirty();
}


// Part reset (to default)
void qxgeditXGParamMap::reset_part ( unsigned short iPart )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGParamMap::reset_part(%u)", iPart);
#endif

#if 0
	ObserverMap::const_iterator iter = m_observers.constBegin();
	for (; iter != m_observers.constEnd(); ++iter) {
		XGParam *pParam = iter.key();
		if (pParam->high() == 0x08 && pParam->mid() == iPart)
			pParam->reset(/* iter.value() */);
	}
#else
	XGParamMap::const_iterator iter = MULTIPART.constBegin();
	for (; iter != MULTIPART.constEnd(); ++iter) {
		XGParamSet *pParamSet = iter.value();
		if (pParamSet->contains(iPart)) {
			XGParam *pParam = pParamSet->value(iPart);
			if (pParam)
				pParam->reset();
		}
	}
#endif
}


// Drums reset (to default)
void qxgeditXGParamMap::reset_drums ( unsigned short iDrumSet )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGParamMap::reset_drums(%u)", iDrumSet);
#endif

	unsigned short iHigh = 0x30 + iDrumSet;
	ObserverMap::const_iterator iter = m_observers.constBegin();
	for (; iter != m_observers.constEnd(); ++iter) {
		XGParam *pParam = iter.key();
		if (pParam->high() == iHigh)
			pParam->reset(iter.value());
	}
}


// user voice reset (to default)
void qxgeditXGParamMap::reset_user ( unsigned short iUser )
{
#ifdef CONFIG_DEBUG
	qDebug("qxgeditXGParamMap::reset_user(%u)", iUser);
#endif

	XGParamMap::const_iterator iter = USERVOICE.constBegin();
	for (; iter != USERVOICE.constEnd(); ++iter) {
		XGParamSet *pParamSet = iter.value();
		if (pParamSet->contains(iUser)) {
			XGParam *pParam = pParamSet->value(iUser);
			if (pParam)
				pParam->reset();
		}
	}

	set_user_dirty(iUser, false);
}


// Send regular XG Parameter change SysEx message.
void qxgeditXGParamMap::send_param ( XGParam *pParam )
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


// Send USER VOICE Bulk Dump SysEx message.
void qxgeditXGParamMap::send_user ( unsigned short iUser ) const
{
	qxgeditMidiDevice *pMidiDevice = qxgeditMidiDevice::getInstance();
	if (pMidiDevice == NULL)
		return;

	// Build the complete SysEx message...
	XGUserVoiceSysex sysex(iUser);
	// Send it out...
	pMidiDevice->sendSysex(sysex.data(), sysex.size());
}


// (QS300) USERVOICE dirty slot simple managing.
void qxgeditXGParamMap::reset_user_dirty (void)
{
	for (unsigned short i = 0; i < 32; ++i)
		m_user_dirty[i] = 0;
}

void qxgeditXGParamMap::set_user_dirty ( unsigned short iUser, bool bDirty )
{
	if (iUser < 32) {
		if (bDirty)
			m_user_dirty[iUser]++;
		else
			m_user_dirty[iUser] = 0;
	}
}

bool qxgeditXGParamMap::user_dirty ( unsigned short iUser ) const
{
	return (iUser < 32 && m_user_dirty[iUser] > 0);
}


// (QS300) USERVOICE bulk dump auto-send feature.
void qxgeditXGParamMap::set_auto_send ( bool bAuto )
{
	m_auto_send = bAuto;
}

bool qxgeditXGParamMap::auto_send (void) const
{
	return m_auto_send;
}


// end of qxgeditXGParamMap.cpp
