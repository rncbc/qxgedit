// qxgeditXGParamMap.h
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

#ifndef __qxgeditXGParamMap_h
#define __qxgeditXGParamMap_h

#include "XGParam.h"


//----------------------------------------------------------------------------
// qxgeditXGParamMap -- XGParam master map.
//
class qxgeditXGParamMap : public XGParamMasterMap
{
public:

	// Constructor.
	qxgeditXGParamMap();

	// Destructor.
	~qxgeditXGParamMap();

	// Direct parameter data access.
	unsigned short set_param_data(
		unsigned short high, unsigned short mid, unsigned short low,
		unsigned char *data);

	// All parameter reset (to default)
	void reset_all();

	// Part reset (to default)
	void reset_part(unsigned short iPart);

	// Drums reset (to default)
	void reset_drums(unsigned short iDrumSet);

	// user voice reset (to default)
	void reset_user(unsigned short iUser);

	// Send regular XG Parameter change SysEx message.
	void send_param(XGParam *pParam);

	// Send USER VOICE Bulk Dump SysEx message.
	void send_user(unsigned short iUser) const;

private:

	// Simple XGParam observer.
	class Observer : public XGParamObserver
	{
	public:
		// Constructor.
		Observer(XGParam *pParam);
	protected:
		// View updater (observer callback).
		void reset();
		void update();
	};

	// Local observer map.
	typedef QHash<XGParam *, Observer *> ObserverMap;

	// Instance variables.
	ObserverMap m_observers;
};

#endif	// __qxgeditXGParamMap_h

// end of qxgeditXGParamMap.h

