// XGParamWidget.cpp
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

#include "XGParamWidget.h"

#ifdef XGPARAM_WIDGET_MAP

#include <QWidget>

#include <cstdio>

//-------------------------------------------------------------------------
// class XGParamQWidgetMap - XGParam/Widget cross-map.
//
// Pseudo-singleton reference.
XGParamWidgetMap *XGParamWidgetMap::g_pParamWidgetMap = NULL;

// Pseudo-singleton accessor (static).
XGParamWidgetMap *XGParamWidgetMap::getInstance (void)
{
	return g_pParamWidgetMap;
}


// Constructor.
XGParamWidgetMap::XGParamWidgetMap(void)
{
	// Pseudo-singleton set.
	g_pParamWidgetMap = this;
}


// Destructor.
XGParamWidgetMap::~XGParamWidgetMap(void)
{
	// Pseudo-singleton reset.
	g_pParamWidgetMap = NULL;
}


// Add widget to map.
void XGParamWidgetMap::add_widget (
	QWidget *widget, XGParamMap *map, unsigned short id )
{
	XGParamInst inst(map, id);
	m_widget_map.insert(widget, inst);
	m_params_map.insert(inst, widget);
}

void XGParamWidgetMap::add_widget (
	QWidget *widget, XGParam *param )
{
	XGParamMasterMap *pMasterMap = XGParamMasterMap::getInstance();
	if (pMasterMap == NULL)
		return;

	XGParamMap *map = pMasterMap->find_param_map(param);
	if (map == NULL)
		return;

	add_widget(widget, map, param->low());
}

void XGParamWidgetMap::add_widget (
	QWidget *widget, const XGParamKey& key )
{
	XGParamMasterMap *pMasterMap = XGParamMasterMap::getInstance();
	if (pMasterMap == NULL)
		return;

	XGParam *param = pMasterMap->find_param(key);
	if (param == NULL)
		return;

	add_widget(widget, param);
}



// State parameter lookup.
XGParam *XGParamWidgetMap::find_param ( QWidget *widget ) const
{
	XGParam *param = NULL;
	if (m_widget_map.contains(widget))
		param = m_widget_map.value(widget).param();
	return param;
}

// Widget lookup.
QWidget *XGParamWidgetMap::find_widget ( XGParamMap *map, unsigned id ) const
{
	QWidget *widget = NULL;
	XGParamInst inst(map, id);
	if (m_params_map.contains(inst))
		widget = m_params_map.value(inst);
	return widget;
}

QWidget *XGParamWidgetMap::find_widget ( XGParam *param ) const
{
	XGParamMasterMap *pMasterMap = XGParamMasterMap::getInstance();
	if (pMasterMap == NULL)
		return NULL;

	XGParamMap *map = pMasterMap->find_param_map(param);
	if (map == NULL)
		return NULL;

	return find_widget(map, param->low());
};

QWidget *XGParamWidgetMap::find_widget ( const XGParamKey& key ) const
{
	XGParamMasterMap *pMasterMap = XGParamMasterMap::getInstance();
	if (pMasterMap == NULL)
		return NULL;

	XGParam *param = pMasterMap->find_param(key);
	if (param == NULL)
		return NULL;

	return find_widget(param);
}

#endif	// XGPARAM_WIDGET_MAP


// end of XGParamWidget.cpp
