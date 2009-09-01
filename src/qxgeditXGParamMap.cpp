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
	qxgeditMidiDevice *pMidiDevice = qxgeditMidiDevice::getInstance();
	if (pMidiDevice == NULL)
		return;

	XGParam *pParam = param();
	unsigned char aSysex[]  = { 0xf0, 0x43, 0x10, 0x4c,
		0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7 };
	unsigned short iSysex = 8 + pParam->size();
	aSysex[4] = pParam->high();
	aSysex[5] = pParam->mid();
	aSysex[6] = pParam->low();
	pParam->set_data_value(&aSysex[7], pParam->value());
	aSysex[iSysex - 1] = 0xf7;

	pMidiDevice->sendSysex(aSysex, iSysex);

	// HACK: Special reset actions...
	if (pParam->high() == 0x00 && pParam->mid() == 0x00) {
		qxgeditXGParamMap *pParamMap
			= static_cast<qxgeditXGParamMap *> (
				XGParamMasterMap::getInstance());
		if (pParamMap) {
			switch (pParam->low()) {
			case 0x7d: // Drum Setup Reset
				pParamMap->reset_drums(pParam->value());
				break;
			case 0x7e: // XG System On
			case 0x7f: // All Parameter Reset
				pParamMap->reset_all();
				break;
			}
		}
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
	unsigned char high, unsigned char mid, unsigned char low,
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
		pParam->set_value(pParam->def(), iter.value());
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
			pParam->set_value(pParam->def()/*, iter.value()*/);
	}
#else
	XGParamMap::const_iterator iter = MULTIPART.constBegin();
	for (; iter != MULTIPART.constEnd(); ++iter) {
		XGParamSet *pParamSet = iter.value();
		if (pParamSet->contains(iPart)) {
			XGParam *pParam = pParamSet->value(iPart);
			if (pParam)
				pParam->set_value(pParam->def());
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
			pParam->set_value(pParam->def(), iter.value());
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
				pParam->set_value(pParam->def());
		}
	}
}


// end of qxgeditXGParamMap.cpp

