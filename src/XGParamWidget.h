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

#ifdef XGPARAM_WIDGET_MAP

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

#endif	// XGPARAM_WIDGET_MAP


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
		Observer(XGParam *param, XGParamWidget<W> *widget)
			: XGParamObserver(param), m_widget(widget) {}

	protected:
		// Observer resetter.
		void reset()  { m_widget->set_param(param()); }
		// Observer updater.
		void update() { m_widget->set_value(value()); }

	private:
		// Members.
		XGParamWidget<W> *m_widget;
	};

	// Constructor.
	XGParamWidget(QWidget *parent = NULL)
		: W(parent), m_param_map(NULL), m_param_id(0) {}

	// Virtual destructor.
	virtual ~XGParamWidget()
		{ clear_observers(); }

	// Pure virtual methods.
	virtual void set_param(XGParam *param) = 0;
	virtual void set_value(unsigned short u) = 0;

	// Setup.
	void set_param_map(XGParamMap *map, unsigned short id)
	{
		clear_observers();

		m_param_map = map;
		m_param_id  = id;

		XGParamSet *paramset = m_param_map->find_paramset(m_param_id);
		if (paramset == NULL)
			return;

		XGParamSet::const_iterator iter = paramset->constBegin();
		for (; iter != paramset->constEnd(); ++iter)
			m_observers.insert(iter.key(), new Observer(iter.value(), this));

#ifdef XGPARAM_WIDGET_MAP
		XGParamWidgetMap *pParamWidgetMap = XGParamWidgetMap::getInstance();
		if (pParamWidgetMap)
			pParamWidgetMap->add_widget(this, m_param_map, m_param_id);
#endif

		unsigned short key = m_param_map->current_key();
		if (paramset->contains(key)) {
			XGParam *param = paramset->value(key);
			if (param)
				param->notify_reset();
		}
		else
		if (m_param_map->key_param()) {
			XGParam *param = m_param_map->key_param();
			m_observers.insert(0, new Observer(param, this));
			param->notify_reset();
		}
	}

	XGParamMap *param_map() const
		{ return m_param_map; }
	unsigned short param_id() const
		{ return m_param_id; }

	// Observer accessor.
	Observer *observer() const
	{
		if (m_param_map == NULL)
			return NULL;

		unsigned short key = m_param_map->current_key();
		if (!m_observers.contains(key))
			return NULL;

		return static_cast<Observer *> (m_observers.value(key));
	}

protected:

	// Observers cleaner.
	void clear_observers()
	{
		QHash<unsigned short, XGParamObserver *>::const_iterator iter
			= m_observers.constBegin();
		for (; iter != m_observers.constEnd(); ++iter)
			delete iter.value();
		m_observers.clear();
	}

private:

	// Members.
	XGParamMap    *m_param_map;
	unsigned short m_param_id;

	QHash<unsigned short, XGParamObserver *> m_observers;
};


#endif	// __XGParamWidget_h

// end of XGParamWidget.h
