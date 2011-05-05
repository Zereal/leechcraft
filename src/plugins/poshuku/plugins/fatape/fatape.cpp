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

#include "fatape.h"
#include <QIcon>

namespace LeechCraft
{
namespace Poshuku
{
namespace FatApe
{
	void Plugin::Init (ICoreProxy_ptr)
	{
	}
	
	void Plugin::SecondInit ()
	{
	}
	
	void Plugin::Release ()
	{
	}

	QByteArray Plugin::GetUniqueID () const
	{
		return "org.LeechCraft.Poshuku.FatApe";
	}
	
	QString Plugin::GetName () const
	{
		return "Poshuku FatApe";
	}

	QString Plugin::GetInfo () const
	{
		return tr ("GreaseMonkey support layer for the Poshuku browser.");
	}

	QIcon Plugin::GetIcon () const
	{
		return QIcon ();
	}

	QSet<QByteArray> Plugin::GetPluginClasses () const
	{
		QSet<QByteArray> result;
		result << "org.LeechCraft.Poshuku.Plugins/1.0";
		return result;
	}
}
}
}

Q_EXPORT_PLUGIN2 (leechcraft_poshuku_fatape, LeechCraft::Poshuku::FatApe::Plugin);