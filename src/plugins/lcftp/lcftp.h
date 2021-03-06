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

#ifndef PLUGINS_LCFTP_LCFTP_H
#define PLUGINS_LCFTP_LCFTP_H
#include <memory>
#include <QObject>
#include <QTranslator>
#include <QModelIndex>
#include <interfaces/iinfo.h>
#include <interfaces/idownload.h>
#include <interfaces/ijobholder.h>
#include <interfaces/ihavetabs.h>
#include <interfaces/ihavesettings.h>
#include <interfaces/ientityhandler.h>
#include <xmlsettingsdialog/xmlsettingsdialog.h>

namespace LeechCraft
{
	namespace Plugins
	{
		namespace LCFTP
		{
			class TabManager;

			class LCFTP : public QObject
						, public IInfo
						, public IHaveTabs
						, public IJobHolder
						, public IDownload
						, public IHaveSettings
						, public IEntityHandler
			{
				Q_OBJECT
				Q_INTERFACES (IInfo IHaveTabs IJobHolder IDownload IEntityHandler IHaveSettings)

				std::auto_ptr<QTranslator> Translator_;
				boost::shared_ptr<LeechCraft::Util::XmlSettingsDialog> XmlSettingsDialog_;
				TabManager *TabManager_;
			public:
				void Init (ICoreProxy_ptr);
				void SecondInit ();
				void Release ();
				QByteArray GetUniqueID () const;
				QString GetName () const;
				QString GetInfo () const;
				QStringList Provides () const;
				QStringList Needs () const;
				QStringList Uses () const;
				void SetProvider (QObject*, const QString&);
				QIcon GetIcon () const;
				
				TabClasses_t GetTabClasses () const;
				void TabOpenRequested (const QByteArray&);

				QAbstractItemModel* GetRepresentation () const;

				qint64 GetDownloadSpeed () const;
				qint64 GetUploadSpeed () const;
				void StartAll ();
				void StopAll ();
				EntityTestHandleResult CouldDownload (const Entity&) const;
				int AddJob (Entity);
				void KillTask (int);

				EntityTestHandleResult CouldHandle (const Entity&) const;
				void Handle (Entity);

				boost::shared_ptr<LeechCraft::Util::XmlSettingsDialog> GetSettingsDialog () const;
			signals:
				void jobFinished (int);
				void jobRemoved (int);
				void jobError (int, IDownload::Error);
				void gotEntity (const LeechCraft::Entity&);

				void addNewTab (const QString&, QWidget*);
				void removeTab (QWidget*);
				void changeTabName (QWidget*, const QString&);
				void changeTabIcon (QWidget*, const QIcon&);
				void changeTooltip (QWidget*, QWidget*);
				void statusBarChanged (QWidget*, const QString&);
				void raiseTab (QWidget*);
			};
		};
	};
};

#endif

