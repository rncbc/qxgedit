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

	// Special USERVOICE stuff...
	if (pParam->high() == 0x11) {
		pParamMap->send_user(pParamMap->USERVOICE.current_key());
	} else {
		pParamMap->send_param(pParam);
	}
}


//----------------------------------------------------------------------------
// qxgeditXGParamMap -- XGParam master map.
//

// Constructor.
qxgeditXGParamMap::qxgeditXGParamMap (void)
	: XGParamMasterMap()
{
	// Setup local observers...
	XGParamMasterMap::const_iterator iter
		= XGParamMasterMap::constBegin();
	for (; iter != XGParamMasterMap::constEnd(); ++iter) {
		XGParam *pParam = iter.value();
		m_observers.insert(pParam, new Observer(pParam));
	}
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


// Direct parameter data access.
unsigned short qxgeditXGParamMap::set_param_data (
	unsigned short high, unsigned short mid, unsigned short low,
	unsigned char *data )
{
	XGParam *pParam = find_param(high, mid, low);
	if (pParam == NULL)
		return 0;

	if (!m_observers.contains(pParam))
		return 0;

	m_observers.value(pParam)->set_value(pParam->data_value(data));

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
	unsigned short iSysex = 8 + pParam->size();
	unsigned char *pSysex = new unsigned char [iSysex];

	unsigned short i = 0;
	pSysex[i++] = 0xf0;	// SysEx status (SOX)
	pSysex[i++] = 0x43;	// Yamaha id.
	pSysex[i++] = 0x10;	// Device no.
	pSysex[i++] = 0x4c;	// XG Model id.

	// Regular XG Parameter change...
	pSysex[i++] = pParam->high();
	pSysex[i++] = pParam->mid();
	pSysex[i++] = pParam->low();
	pParam->set_data_value(&pSysex[i], pParam->value());
	i += pParam->size();

	// Coda...
	pSysex[i] = 0xf7;	// SysEx status (EOX)

	// Send it out...
	pMidiDevice->sendSysex(pSysex, iSysex);

	delete [] pSysex;

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

	// HACK: Flag dirty the main form...
	qxgeditMainForm *pMainForm = qxgeditMainForm::getInstance();
	if (pMainForm)
		pMainForm->contentsChanged();
}


// Send USER VOICE Bulk Dump SysEx message.
void qxgeditXGParamMap::send_user ( unsigned short iUser ) const
{
	qxgeditMidiDevice *pMidiDevice = qxgeditMidiDevice::getInstance();
	if (pMidiDevice == NULL)
		return;

	// USER VOICE (QS300) Bulk Dump...
	unsigned short high = 0x11;
	unsigned short mid  = iUser;
	unsigned short low  = 0x00;

	// Build the complete SysEx message...
	unsigned short iSysex = 0x188;	// (= 11 + 0x17d);
	unsigned char *pSysex = new unsigned char [iSysex];

	unsigned short i = 0;
	pSysex[i++] = 0xf0;	// SysEx status (SOX)
	pSysex[i++] = 0x43;	// Yamaha id.
	pSysex[i++] = 0x00;	// Device no.
	pSysex[i++] = 0x4b;	// QS300 Model id.
	pSysex[i++] = 0x02;	// Byte count MSB (= 0x17d >> 7).
	pSysex[i++] = 0x7d;	// Byte count LSB (= 0x17d & 0x7f).

	pSysex[i++] = high;
	pSysex[i++] = mid;
	pSysex[i++] = low;

	unsigned short i0 = i;
	while (i < iSysex - 2) {
		low = i - i0;
		XGParam *pParam = find_param(high, mid, low);
		if (pParam) {
			if (pParam->size() > 4) {
				XGDataParam *pDataParam	= static_cast<XGDataParam *> (pParam);
				::memcpy(&pSysex[i], pDataParam->data(), pDataParam->size());
			} else {
				pParam->set_data_value(&pSysex[i], pParam->value());
			}
			i += pParam->size();
		} else {
			pSysex[i++] = 0x00;
		}
	}

	// Compute checksum...
	unsigned char cksum = 0;
	for (unsigned short j = 4; j < i; ++j) {
		cksum += pSysex[j];
		cksum &= 0x7f;
	}
	pSysex[i++] = 0x80 - cksum;

	// Coda...
	pSysex[i] = 0xf7;	// SysEx status (EOX)

	// Send it out...
	pMidiDevice->sendSysex(pSysex, iSysex);

	delete [] pSysex;

	// HACK: Flag dirty the main form...
	qxgeditMainForm *pMainForm = qxgeditMainForm::getInstance();
	if (pMainForm)
		pMainForm->contentsChanged();
}


// end of qxgeditXGParamMap.cpp
