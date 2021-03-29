/*
 * sk_plugin.h - Classical Three-Dimensional Artwork for Qt 5
 *
 * Copyright (c) 2017-2021 rncbc aka Rui Nuno Capela <rncbc@rncbc.org>
 * Copyright (c) 2007, 2008 Christoph Feck <christoph@maxiom.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef SKULPTURE_PLUGIN_H
#define SKULPTURE_PLUGIN_H 1

/*-----------------------------------------------------------------------*/

#include <QStylePlugin>

class SkulptureStylePlugin : public QStylePlugin
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "skulpture.json")
#endif
	public:
		QStringList keys() const;
		QStyle *create(const QString &key);
};


/*-----------------------------------------------------------------------*/


#endif // SKULPTURE_PLUGIN_H


