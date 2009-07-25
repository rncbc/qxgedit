// XGParamObserver.h
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

#ifndef __XGParamObserver_h
#define __XGParamObserver_h

// Forward declarations.
class XGParam;


//---------------------------------------------------------------------------
// XGParamObserver - Scalar parameter value control/view.

class XGParamObserver
{
public:

	// Constructor.
	XGParamObserver(XGParam *param);

	// Virtual destructor.
	virtual ~XGParamObserver();

	// Subject value accessor.
	void setParam(XGParam *param);
	XGParam *param() const;

	// Indirect value accessors.
	void setValue(unsigned short u);
	unsigned short value() const;

	// Busy flag predicate.
	bool isBusy() const;

	// Pure virtual view updater.
	virtual void update() = 0;

private:

	// Instance variables.
	XGParam *m_param;
};

#endif  // __XGParamObserver_h


// end of XGParamObserver.h
