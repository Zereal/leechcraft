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

#include "notificationrule.h"
#include <QStringList>
#include <QtDebug>

namespace LeechCraft
{
namespace AdvancedNotifications
{
	AudioParams::AudioParams ()
	{
	}
		
	AudioParams::AudioParams (const QString& fname)
	: Filename_ (fname)
	{
	}

	NotificationRule::NotificationRule ()
	: Methods_ (NMNone)
	{
	}

	NotificationRule::NotificationRule (const QString& name,
			const QString& cat, const QStringList& types)
	: Name_ (name)
	, Category_ (cat)
	, Types_ (types)
	, Methods_ (NMNone)
	{
	}
	
	bool NotificationRule::IsNull () const
	{
		return Name_.isEmpty () ||
				Category_.isEmpty () ||
				Types_.isEmpty ();
	}
	
	QString NotificationRule::GetName () const
	{
		return Name_;
	}
	
	void NotificationRule::SetName (const QString& name)
	{
		Name_ = name;
	}
	
	QString NotificationRule::GetCategory () const
	{
		return Category_;
	}
	
	void NotificationRule::SetCategory (const QString& cat)
	{
		Category_ = cat;
	}
	
	QStringList NotificationRule::GetTypes () const
	{
		return Types_;
	}
	
	void NotificationRule::SetTypes (const QStringList& types)
	{
		Types_ = types;
	}
	
	NotificationMethods NotificationRule::GetMethods () const
	{
		return Methods_;
	}
	
	void NotificationRule::SetMethods (const NotificationMethods& methods)
	{
		Methods_ = methods;
	}
	
	FieldMatches_t NotificationRule::GetFieldMatches () const
	{
		return FieldMatches_;
	}
	
	VisualParams NotificationRule::GetVisualParams () const
	{
		return VisualParams_;
	}

	void NotificationRule::SetVisualParams (const VisualParams& params)
	{
		VisualParams_ = params;
	}
	
	AudioParams NotificationRule::GetAudioParams () const
	{
		return AudioParams_;
	}

	void NotificationRule::SetAudioParams (const AudioParams& params)
	{
		AudioParams_ = params;
	}
	
	TrayParams NotificationRule::GetTrayParams () const
	{
		return TrayParams_;
	}

	void NotificationRule::SetTrayParams (const TrayParams& params)
	{
		TrayParams_ = params;
	}
	
	void NotificationRule::SetFieldMatches (const FieldMatches_t& matches)
	{
		FieldMatches_ = matches;
	}
	
	void NotificationRule::Save (QDataStream& stream) const
	{
		stream << static_cast<quint8> (1)
			<< Name_
			<< Category_
			<< Types_
			<< static_cast<quint16> (Methods_)
			<< AudioParams_.Filename_
			<< static_cast<quint16> (FieldMatches_.size ());

		Q_FOREACH (const FieldMatch& match, FieldMatches_)
			match.Save (stream);
	}
	
	void NotificationRule::Load (QDataStream& stream)
	{
		quint8 version = 0;
		stream >> version;
		if (version != 1)
		{
			qWarning () << Q_FUNC_INFO
					<< "unknown version"
					<< version;
			return;
		}
		
		quint16 methods;
		stream >> Name_
			>> Category_
			>> Types_
			>> methods
			>> AudioParams_.Filename_;
		Methods_ = static_cast<NotificationMethods> (methods);
		
		quint16 numMatches = 0;
		stream >> numMatches;
		
		for (int i = 0; i < numMatches; ++i)
		{
			FieldMatch match;
			match.Load (stream);
			FieldMatches_ << match;
		}
	}
}
}
