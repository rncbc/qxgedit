// XGParam.h
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

#ifndef __XGParam_h
#define __XGParam_h

#include <QHash>


#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// XG Effect table helpers.

typedef
struct _XGEffectParamItem
{
	unsigned char    id;                    // parameter index
	const char      *name;                  // parameter name.
	unsigned short   min;                   // minimum value.
	unsigned short   max;                   // maximum value.
	float          (*getv)(unsigned short); // convert to display value.
	unsigned short (*getu)(float);          // invert to native value.
	const char *   (*gets)(unsigned short); // enumerated string value.
	const char *   (*unit)();               // unit suffix label.

} XGEffectParamItem;

typedef
struct _XGEffectItem
{
	unsigned char      msb;                 // effect type MSB
	unsigned char      lsb;                 // effect type LSB
	const char        *name;                // effect type name
	XGEffectParamItem *params;              // effect type parameters
	unsigned short    *defs;                // effect type defaults

} XGEffectItem;

const XGEffectItem *REVERBEffectItem(unsigned short etype);
const XGEffectItem *CHORUSEffectItem(unsigned short etype);
const XGEffectItem *VARIATIONEffectItem(unsigned short etype);

unsigned short REVERBEffectDefault(unsigned short etype, unsigned char index);
unsigned short CHORUSEffectDefault(unsigned short etype, unsigned char index);
unsigned short VARIATIONEffectDefault(unsigned short etype, unsigned char index);


//-------------------------------------------------------------------------
// XG Parameter table helpers.

typedef
struct _XGParamItem
{
	unsigned char    id;	// id=low address.
	unsigned char    size;  // data size in bytes.
	unsigned short   min;   // minimum value; 0=REVERB, 1=CHORUS, 2=VARIATION.
	unsigned short   max;   // maximum value; parameter index (0..15)
	const char      *name;  // parameter name; NULL=depends on effect type.
	unsigned short   def;   // default value;
	float          (*getv)(unsigned short); // convert to display value.
	unsigned short (*getu)(float);          // invert to native value.
	const char *   (*gets)(unsigned short); // enumerated string value.
	const char *   (*unit)();               // unit suffix label.

} XGParamItem;


const XGParamItem *SYSTEMParamItem(unsigned char id);
const XGParamItem *EFFECTParamItem(unsigned char id);
const XGParamItem *MULTIPARTParamItem(unsigned char id);
const XGParamItem *DRUMSETUPParamItem(unsigned char id);

#ifdef __cplusplus
}
#endif

//-------------------------------------------------------------------------
// class XGParam - XG Generic parameter descriptor.

class XGParam
{
public:

	// Constructor.
	XGParam(unsigned char high, unsigned char mid, unsigned char low);

	// Address acessors.
	unsigned char high() const;
	unsigned char mid()  const;
	unsigned char low()  const;

	// Number of bytes needed to encode subject.
	unsigned char size() const;

	// Virtual accessors.
	virtual const char *name() const;
	virtual unsigned short min() const;
	virtual unsigned short max() const;
	virtual unsigned short def() const;
	virtual float getv(unsigned short u) const;
	virtual unsigned short getu(float v) const;
	virtual const char *gets(unsigned short u) const;
	virtual const char *unit() const;

	// Value accessors.
	void setValue(unsigned short c);
	unsigned short value() const;

protected:

	// Parameter descriptor.
	const XGParamItem *m_param;

private:

	// Parameter address.
	unsigned char m_high;
	unsigned char m_mid;
	unsigned char m_low;

	// Parameter state.
	unsigned short m_value;
};


//-------------------------------------------------------------------------
// class XGEffectParam - XG Effect parameter descriptor.

class XGEffectParam : public XGParam
{
public:

	// Constructor.
	XGEffectParam(unsigned char high, unsigned char mid, unsigned char low,
		unsigned short etype);

	// Sub-address accessors.
	unsigned short etype() const;

	// Virtual accessors.
	const char *name() const;
	unsigned short min() const;
	unsigned short max() const;
	unsigned short def() const;
	float getv(unsigned short u) const;
	unsigned short getu(float v) const;
	const char *gets(unsigned short u) const;
	const char *unit() const;

private:

	// Parameter sub-type.
	unsigned short m_etype;	

	// Parameter sub-descriptor.
	const XGEffectParamItem *m_eparam;
};


//-------------------------------------------------------------------------
// class XGParamKey - XG Parameter hash key.
//
struct XGParamKey
{
	// Constructor helper.
	XGParamKey(unsigned char hi, unsigned char mi, unsigned char lo)
		: high(hi), mid(mi), low(lo) {}
	XGParamKey(XGParam *param)
		: high(param->high()), mid(param->mid()), low(param->low()) {}

	// Hash key comparator.
	bool operator== ( const XGParamKey& key ) const
	{
		return (key.high == high)
			&& (key.mid  == mid)
			&& (key.low  == low);
	}

	// Key memberes.
	unsigned char high;
	unsigned char mid;
	unsigned char low;
};

// Hash key function
inline uint qHash ( const XGParamKey& key )
{
	return qHash(((key.high << 7) + key.mid) ^ key.low);
}


//-------------------------------------------------------------------------
// class XGParamMap - XG Parameter hash map.
//
class XGParamMap : public QHash<XGParamKey, XGParam *>
{
public:

	// Constructor.
	XGParamMap() : m_bAutoDelete(false) {}

	// Destructor.
	~XGParamMap()
	{
		if (m_bAutoDelete) {
			XGParamMap::const_iterator iter = XGParamMap::constBegin();
			for (; iter != XGParamMap::constEnd(); ++iter)
				delete iter.value();
		}
	}
	
	// Auto delete predicate.
	void setAutoDelete(bool bAutoDelete)
		{ m_bAutoDelete = bAutoDelete; }
	bool isAutoDelete() const
		{ return m_bAutoDelete; }

	// Append method
	void append(XGParam *param)
		{ insertMulti(XGParamKey(param), param); }

	// Map finders.
	XGParam *find(const XGParamKey& key, unsigned short etype = 0) const
	{
		XGParamMap::const_iterator iter = XGParamMap::constFind(key);
		while (iter != XGParamMap::constEnd() && iter.key() == key) {
			XGParam *param = iter.value();
			if (key.high == 0x02 && key.mid == 0x01) {
				XGEffectParam *eparam
					= static_cast<XGEffectParam *> (param);
				if (eparam->etype() == etype)
					return param;
			} 
			else return param;

		}
		return NULL;
	}

	XGParam *find(
		unsigned char high, unsigned char mid, unsigned char low,
		unsigned short etype = 0) const
		{ return find(XGParamKey(high, mid, low), etype); }

private:

	// Instance properties.
	bool m_bAutoDelete;
};


//-------------------------------------------------------------------------
// class XGParamList XG Parameter map list.
//

class XGParamList : public QHash<unsigned short, XGParamMap> {};


//-------------------------------------------------------------------------
// class XGParamMaster - XG Parameter master state database.
//

class XGParamMaster : public XGParamMap
{
public:

	// Constructor.
	XGParamMaster();

	// Parameter group-links.
	XGParamList REVERB;
	XGParamList CHORUS;
	XGParamList VARIATION;
	XGParamList MULTIPART;
	XGParamList DRUMSETUP;
};


#endif	// __XGParam_h

// end of XGParam.h
