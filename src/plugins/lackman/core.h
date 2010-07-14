/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2010  Georg Rudoy
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

#ifndef PLUGINS_LACKMAN_CORE_H
#define PLUGINS_LACKMAN_CORE_H
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <QObject>
#include <interfaces/iinfo.h>
#include "repoinfo.h"

class QAbstractItemModel;
class QStandardItemModel;

namespace LeechCraft
{
	namespace Plugins
	{
		namespace LackMan
		{
			class RepoInfoFetcher;
			class ExternalResourceManager;
			class Storage;
			class PackagesModel;

			class Core : public QObject
			{
				Q_OBJECT

				ICoreProxy_ptr Proxy_;
				RepoInfoFetcher *RepoInfoFetcher_;
				ExternalResourceManager *ExternalResourceManager_;
				Storage *Storage_;
				PackagesModel *PluginsModel_;

				Core ();
			public:
				static Core& Instance ();

				void Release ();

				void SetProxy (ICoreProxy_ptr);
				ICoreProxy_ptr GetProxy () const;

				QAbstractItemModel* GetPluginsModel () const;

				DependencyList GetDependencies (int) const;
				QList<ListPackageInfo> GetDependencyFulfillers (const Dependency&) const;
				bool IsVersionOk (const QString& candidate, QString refVer) const;
				bool IsFulfilled (const Dependency&) const;
				QIcon GetIconForLPI (const ListPackageInfo&);

				void AddRepo (const QUrl&);
				void UpdateRepo (const QUrl&, const QStringList&);

				QStringList GetAllTags () const;
			private:
				InstalledDependencyInfoList GetSystemInstalledPackages () const;
				InstalledDependencyInfoList GetLackManInstalledPackages () const;
				InstalledDependencyInfoList GetAllInstalledPackages () const;
				void PopulatePluginsModel ();
				void HandleNewPackages (const PackageShortInfoList& shorts,
						int componentId, const QString& component, const QUrl& repoUrl);
			private slots:
				void handleInfoFetched (const RepoInfo&);
				void handleComponentFetched (const PackageShortInfoList&,
						const QString&, int);
				void handlePackageFetched (const PackageInfo&, int);
			signals:
				void delegateEntity (const LeechCraft::Entity&,
						int*, QObject**);
				void gotEntity (const LeechCraft::Entity&);
				void tagsUpdated ();
			};
		}
	}
}

#endif