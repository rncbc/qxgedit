// XGParamSysex.cpp
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

#include "XGParamSysex.h"
#include "XGParam.h"

#include <cstdio>


//-------------------------------------------------------------------------
// XG SysEx generic message.
//

// Constructor
XGSysex::XGSysex ( unsigned short size )
	: m_data(NULL), m_size(size)
{
	m_data = new unsigned char [m_size];
}

// Destructor
XGSysex::~XGSysex (void)
{
	delete m_data;
}


// Basic accessors.
unsigned char *XGSysex::data (void) const
{
	return m_data;
}

unsigned short XGSysex::size (void) const
{
	return m_size;
}


//-------------------------------------------------------------------------
// XG Parameter Change SysEx message.

// Constructor.
XGParamSysex::XGParamSysex ( XGParam *param )
	: XGSysex(8 + param->size())
{
	unsigned short i = 0;

	m_data[i++] = 0xf0;	// SysEx status (SOX)
	m_data[i++] = 0x43;	// Yamaha id.
	m_data[i++] = 0x10;	// Device no.
	m_data[i++] = 0x4c;	// XG Model id.

	m_data[i++] = param->high();
	m_data[i++] = param->mid();
	m_data[i++] = param->low();

	if (param->high() == 0x08 && param->low() == 0x09) // DETUNE (2byte,4 bit).
		param->set_data_value2(&m_data[i], param->value());
	else
		param->set_data_value(&m_data[i], param->value());
	i += param->size();

	// Coda...
	m_data[i] = 0xf7;		// SysEx status (EOX)
}


//-------------------------------------------------------------------------
// (QS300) USER VOICE Bulk Dump SysEx message.

// Constructor.
XGUserVoiceSysex::XGUserVoiceSysex ( unsigned short id )
	: XGSysex(0x188) // (size = 0x188 = 11 + 0x17d)
{
	XGParamMasterMap *pMasterMap = XGParamMasterMap::getInstance();
	if (pMasterMap == NULL) {
		::memset(m_data, 0, m_size);
		return;
	}

	unsigned short high = 0x11;
	unsigned short mid  = id;
	unsigned short low  = 0x00;

	unsigned short i = 0;

	m_data[i++] = 0xf0;	// SysEx status (SOX)
	m_data[i++] = 0x43;	// Yamaha id.
	m_data[i++] = 0x00;	// Device no.
	m_data[i++] = 0x4b;	// QS300 Model id.
	m_data[i++] = 0x02;	// Byte count MSB (= 0x17d >> 7).
	m_data[i++] = 0x7d;	// Byte count LSB (= 0x17d & 0x7f).

	m_data[i++] = high;
	m_data[i++] = mid;
	m_data[i++] = low;

	unsigned short i0 = i;
	while (i < m_size - 2) {
		low = i - i0;
		XGParam *param = pMasterMap->find_param(high, mid, low);
		if (param) {
			if (param->size() > 4) {
				XGDataParam *dataparam = static_cast<XGDataParam *> (param);
				::memcpy(&m_data[i], dataparam->data(), dataparam->size());
			}
			else
			if (high == 0x08 && low == 0x09) { // DETUNE (2byte, 4bit).
				param->set_data_value2(&m_data[i], param->value());
			}
			else {
				param->set_data_value(&m_data[i], param->value());
			}
			i += param->size();
		} else {
			m_data[i++] = 0x00;
		}
	}

	// Compute checksum...
	unsigned char cksum = 0;
	for (unsigned short j = 4; j < i; ++j) {
		cksum += m_data[j];
		cksum &= 0x7f;
	}
	m_data[i++] = 0x80 - cksum;

	// Coda...
	m_data[i] = 0xf7;		// SysEx status (EOX)
}



// end of XGParamSysex.cpp
