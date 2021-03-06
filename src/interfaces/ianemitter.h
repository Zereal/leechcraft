/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2011  Georg Rudoy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#ifndef INTERFACES_IANEMITTER_H
#define INTERFACES_IANEMITTER_H
#include <QtPlugin>
#include <QVariant>
#include <QStringList>

namespace LeechCraft
{
	struct ANFieldData
	{
		QString ID_;
		QString Name_;
		QString Description_;
		QVariant::Type Type_;
		QStringList EventTypes_;
		
		ANFieldData ()
		: Type_ (QVariant::Invalid)
		{
		}
		
		ANFieldData (const QString& id,
				const QString& name,
				const QString& description,
				QVariant::Type type,
				QStringList events)
		: ID_ (id)
		, Name_ (name)
		, Description_ (description)
		, Type_ (type)
		, EventTypes_ (events)
		{
		}
	};
}

class IANEmitter
{
public:
	virtual ~IANEmitter () {}
	
	virtual QList<LeechCraft::ANFieldData> GetANFields () const = 0;
};

Q_DECLARE_INTERFACE (IANEmitter, "org.Deviant.LeechCraft.IANEmitter/1.0");
Q_DECLARE_METATYPE (LeechCraft::ANFieldData);

#endif
