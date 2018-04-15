// XGParamSysex.h
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

#ifndef __XGParamSysex_h
#define __XGParamSysex_h

// Forward ddeclarations.
class XGParam;


//-------------------------------------------------------------------------
// XG SysEx generic message.
//

class XGSysex
{
public:

	// Constructor
	XGSysex(unsigned short size);

	// Destructor
	virtual ~XGSysex();

	// Basic accessors.
	unsigned char *data() const;
	unsigned short size() const;

protected:

	// Instance variables.
	unsigned char *m_data;
	unsigned short m_size;
};


//-------------------------------------------------------------------------
// XG Parameter Change SysEx message.

class XGParamSysex : public XGSysex
{
public:

	// Constructor.
	XGParamSysex(XGParam *param);
};


//-------------------------------------------------------------------------
// (QS300) USER VOICE Bulk Dump SysEx message.

class XGUserVoiceSysex : public XGSysex
{
public:

	// Constructor.
	XGUserVoiceSysex(unsigned short id);
};


#endif	// __XGParamSysex_h

// end of XGParamSysex.h

