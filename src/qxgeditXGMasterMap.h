// qxgeditXGMasterMap.h
//
/****************************************************************************
   Copyright (C) 2005-2018, rncbc aka Rui Nuno Capela. All rights reserved.

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

#ifndef __qxgeditXGMasterMap_h
#define __qxgeditXGMasterMap_h

#include "XGParam.h"


//----------------------------------------------------------------------------
// qxgeditXGMasterMap -- XGParam master map.
//
class qxgeditXGMasterMap : public XGParamMasterMap
{
public:

	// Constructor.
	qxgeditXGMasterMap();

	// Destructor.
	~qxgeditXGMasterMap();

	// Singleton (re)cast.
	static qxgeditXGMasterMap *getInstance();

	// Direct RPN value receiver.
	bool set_rpn_value(unsigned char ch,
		unsigned short rpn, unsigned short val, bool bNotify = false);

	// Direct NRPN value receiver.
	bool set_nrpn_value(unsigned char ch,
		unsigned short nrpn, unsigned short val, bool bNotify = false);

	// Direct SysEx data receiver.
	bool set_sysex_data(
		unsigned char *data, unsigned short len, bool bNotify = false);

	// Direct parameter data access.
	unsigned short set_param_data(
		unsigned short high, unsigned short mid, unsigned short low,
		unsigned char *data, bool bNotify = false);
	unsigned short set_param_data(
		XGParam *pParam, unsigned char *data, bool bNotify = false);

	// All parameter reset (to default)
	void reset_all();

	// Part reset (to default)
	void reset_part(unsigned short iPart);

	// Drums reset (to default)
	void reset_drums(unsigned short iDrumSet);

	// User voice reset (to default)
	void reset_user(unsigned short iUser);

	// Send regular XG Parameter change SysEx message.
	void send_param(XGParam *pParam);

	// Send Multi Part Bank Select/Program Number SysEx messages.
	void send_part(unsigned short iPart) const;

	// Send (QS300) USERVOICE Bulk Dump SysEx message.
	void send_user(unsigned short iUser) const;

	// MULTPART dirty slot simple managers.
	void reset_part_dirty();
	void set_part_dirty(unsigned short iPart, bool bDirty);
	bool part_dirty(unsigned short iPart) const;

	// (QS300) USERVOICE dirty slot simple managers.
	void reset_user_dirty();
	void set_user_dirty(unsigned short iUser, bool bDirty);
	bool user_dirty(unsigned short iUser) const;

	void set_user_dirty_1(unsigned short iUser, bool bDirty);
	bool user_dirty_1(unsigned short iUser) const;

	void set_user_dirty_2(unsigned short iUser, bool bDirty);
	bool user_dirty_2(unsigned short iUser) const;

	// (QS300) USERVOICE bulk dump auto-send feature.
	void set_auto_send(bool bAuto);
	bool auto_send() const;

	// Part randomize (from value/def)
	void randomize_part(unsigned short iPart, float p = 20.0f);

	// Drums randomize (from value/def)
	void randomize_drums(
		unsigned short iDrumSet, unsigned short iDrumKey, float p = 20.0f);

	// User voice randomize (from value/def)
	void randomize_user(unsigned short iUser, float p = 20.0f);

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

	// Multi Part dirty flag array.
	int m_part_dirty[16];

	// QS300 User Voice dirty flag array.
	int m_user_dirty[32];

	// QS300 User Voice auto-send feature.
	bool m_auto_send;
};


#endif	// __qxgeditXGMasterMap_h

// end of qxgeditXGMasterMap.h

