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
//
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
//
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
//
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

	// Decode param value from raw data.
	unsigned short valueFromData(unsigned char *data) const;

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
//
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
class XGParamKey
{
public:

	// Constructor helper.
	XGParamKey(unsigned char high, unsigned char mid, unsigned char low)
		: m_high(high), m_mid(mid), m_low(low) {}
	XGParamKey(XGParam *param)
		: m_high(param->high()), m_mid(param->mid()), m_low(param->low()) {}

	// Key accessors.
	unsigned char high() const
		{ return m_high; }
	unsigned char mid() const
		{ return m_mid; }
	unsigned char low() const
		{ return m_low; }

	// Hash key comparator.
	bool operator== ( const XGParamKey& key ) const
	{
		return (key.high() == m_high)
			&& (key.mid()  == m_mid)
			&& (key.low()  == m_low);
	}

private:

	// Key members.
	unsigned char m_high;
	unsigned char m_mid;
	unsigned char m_low;
};


// Hash key function
inline uint qHash ( const XGParamKey& key )
{
	return qHash(((key.high() << 7) + key.mid()) ^ key.low());
}


//-------------------------------------------------------------------------
// class XGParamSet - XG Parameter map.
//

class XGParamSet : public QHash<unsigned short, XGParam *> {};


//-------------------------------------------------------------------------
// class XGParamMap - XG Parameter mapper.
//

class XGParamMap : public QHash<unsigned short, XGParamSet *>
{
public:

	// Constructor.
	XGParamMap();

	// Destructor.
	~XGParamMap();

	// Append method.
	void add_param(XGParam *param, unsigned short key);

	// Map finders.
	XGParam *find_param(unsigned short id);

	// Key param accessors.
	void set_key_param(XGParam *param);
	XGParam *key_param() const;

	// Key value accessors.
	void set_current_key(unsigned short key);
	unsigned short current_key () const;

protected:

	// Param set/factory method.
	XGParamSet *find_paramset(unsigned short id);

private:

	// Instance variables.
	XGParam *m_key_param;
	unsigned short m_key;
};


//-------------------------------------------------------------------------
// class XGParamMaster - XG Parameter master state database.
//

class XGParamMaster : public QHash<XGParamKey, XGParam *>
{
public:

	// Constructor.
	XGParamMaster();

	// Destructor.
	~XGParamMaster();

	// Parameter group-maps.
	XGParamMap SYSTEM;
	XGParamMap REVERB;
	XGParamMap CHORUS;
	XGParamMap VARIATION;
	XGParamMap MULTIPART;
	XGParamMap DRUMSETUP;

	// Master map finders.
	XGParam *find_param(
		unsigned char high,
		unsigned char mid,
		unsigned char low) const;

protected:

	// master append method
	void add_param(XGParam *param);

	XGParam *find_param(
		const XGParamKey& key,
		unsigned short etype = 0) const;
};


#endif	// __XGParam_h

// end of XGParam.h
