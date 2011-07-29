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

#ifndef PLUGINS_ADVANCEDNOTIFICATIONS_CORE_H
#define PLUGINS_ADVANCEDNOTIFICATIONS_CORE_H
#include <QObject>
#include <interfaces/iinfo.h>
#include "notificationrule.h"

namespace LeechCraft
{
namespace AdvancedNotifications
{
	class NotificationRulesWidget;

	class Core : public QObject
	{
		Q_OBJECT
		
		ICoreProxy_ptr Proxy_;
		
		NotificationRulesWidget *NRW_;
		
		Core ();
	public:
		static Core& Instance ();
		
		ICoreProxy_ptr GetProxy () const;
		void SetProxy (ICoreProxy_ptr);
		
		NotificationRulesWidget* GetNRW ();
		
		QList<NotificationRule> GetRules (const Entity&) const;
	};
}
}

#endif