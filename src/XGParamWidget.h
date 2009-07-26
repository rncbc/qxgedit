// XGParamWidget.h
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

#ifndef __XGParamWidget_h
#define __XGParamWidget_h

#include "XGParam.h"
#include "XGParamObserver.h"


//-------------------------------------------------------------------------
// class XGParamInst - XGParam/Widget hash key instance.
//
class XGParamInst
{
public:

	// Constructor.
	XGParamInst(XGParamMap *map = NULL, unsigned short id = 0)
		: m_map(map), m_id(id) {}
	XGParamInst(const XGParamInst& inst)
		: m_map(inst.m_map), m_id(inst.m_id) {}

	// Accessors.
	XGParamMap *map() const
		{ return m_map; }
	unsigned short id() const
		{ return m_id; }
	
	// Hash key comparator.
	bool operator== ( const XGParamInst& inst ) const
		{ return (inst.map() == m_map) && (inst.id() == m_id); }

	// XG parameter indirection.
	XGParam *param() const
		{ return m_map->find_param(m_id); }

private:

	// Member variables.
	XGParamMap    *m_map;
	unsigned short m_id;
};


// XGParam/QWidget hash key function.
inline uint qHash ( const XGParamInst& inst )
{
	return qHash(inst.map()) ^ qHash(inst.id());
}


//-------------------------------------------------------------------------
// class XGParamWidgetMap - XGParam/Widget cross-map.
//
class QWidget;

class XGParamWidgetMap
{
public:

	// Constructor.
	XGParamWidgetMap();

	// Destructor.
	~XGParamWidgetMap();

	// Pseudo-singleton accessor.
	static XGParamWidgetMap *getInstance();

	// Add widget to map.
	void add_widget(QWidget *widget, XGParamMap *map, unsigned short id);
	void add_widget(QWidget *widget, XGParam *param);
	void add_widget(QWidget *widget, const XGParamKey& key);

	// State parameter lookup.
	XGParam *find_param(QWidget *widget) const;

	// Widget lookup.
	QWidget *find_widget(XGParamMap *map, unsigned id) const;
	QWidget *find_widget(XGParam *param) const;
	QWidget *find_widget(const XGParamKey& key) const;

private:

	// Instance parameters.
	QHash<QWidget *, XGParamInst> m_widget_map;
	QHash<XGParamInst, QWidget *> m_params_map;

	// Pseudo-singleton accessor.
	static XGParamWidgetMap *g_pParamWidgetMap;
};


//----------------------------------------------------------------------
// class XGParamWidget -- Template widget observer/visitor.
//

template <class W>
class XGParamWidget : public W
{
public:

	// Local observer.
	class Observer : public XGParamObserver
	{
	public:
		// Constructor.
		Observer(XGParam *param, W *widget)
			: XGParamObserver(param), m_widget(widget) {}

	protected:
		// Observer updater.
		void update() { m_widget->setValue(value()); }

	private:
		// Members.
		W *m_widget;
	};

	// Constructor.
	XGParamWidget(QWidget *parent = NULL)
		: W(parent) {}

	// Destructor.
	virtual ~XGParamWidget()
		{ clear_observers(); }

	// Setup.
	void set_param_map(XGParamMap *map, unsigned short id)
	{
		clear_observers();

		XGParamWidgetMap *pParamWidgetMap = XGParamWidgetMap::getInstance();
		if (pParamWidgetMap == NULL)
			return;

		XGParamSet *paramset = map->find_paramset(id);
		XGParamSet::const_iterator iter = paramset->constBegin();
		for (; iter != paramset->constEnd(); ++iter)
			m_observers.append(new Observer(iter.value(), this));

		pParamWidgetMap->add_widget(this, map, id);
	}

	// Observer accessor.
	const QList<Observer *>observers() const
		{ return m_observers; }

protected:

	// Observers cleaner.
	void clear_observers()
	{
		qDeleteAll(m_observers);
		m_observers.clear();
	}

private:

	// Members.
	QList<Observer *> m_observers;
};


#endif	// __XGParamWidget_h

// end of XGParamWidget.h
