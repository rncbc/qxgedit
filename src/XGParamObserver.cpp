// XGParamObserver.cpp
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

#include "XGParam.h"


//---------------------------------------------------------------------------
// XGParamObserver - Scalar parameter value control/view.

// Constructor.
XGParamObserver::XGParamObserver ( XGParam *param )
	: m_param(param)
{
	if (m_param) m_param->attach(this);
}

// Virtual destructor.
XGParamObserver::~XGParamObserver (void)
{
	if (m_param) m_param->detach(this);
}


// Subject value accessor.
void XGParamObserver::set_param ( XGParam *param )
{
	if (m_param /* && param */)
		m_param->detach(this);

	m_param = param;

	if (m_param)
		m_param->attach(this);
}

XGParam *XGParamObserver::param (void) const
{
	return m_param;
}


// Indirect value accessors.
void XGParamObserver::set_value ( unsigned short u )
{
	if (m_param) m_param->set_value(u, this);
}

unsigned short XGParamObserver::value (void) const
{
	return (m_param ? m_param->value() : 0);
}


// Busy flag predicate.
bool XGParamObserver::busy (void) const
{
	return (m_param ? m_param->busy() : true);
}


// end of XGParamObserver.cpp
