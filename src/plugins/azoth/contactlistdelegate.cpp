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

#include "contactlistdelegate.h"
#include <QPainter>
#include <QPixmap>
#include <QApplication>
#include <QAbstractProxyModel>
#include <QTreeView>
#include <util/resourceloader.h>
#include "interfaces/iclentry.h"
#include "interfaces/isupportgeolocation.h"
#include "core.h"
#include "xmlsettingsmanager.h"
#include "util.h"

namespace LeechCraft
{
namespace Azoth
{
	const int CContactShift = 20;
	const int CPadding = 2;
	const int AvatarPaddingBottom = 2;

	ContactListDelegate::ContactListDelegate (QTreeView* parent)
	: QStyledItemDelegate (parent)
	, View_ (parent)
	{
		handleShowAvatarsChanged ();
		handleShowClientIconsChanged ();
		handleActivityIconsetChanged ();
		handleMoodIconsetChanged ();
		handleSystemIconsetChanged ();

		XmlSettingsManager::Instance ().RegisterObject ("ShowAvatars",
				this, "handleShowAvatarsChanged");
		XmlSettingsManager::Instance ().RegisterObject ("ShowClientIcons",
				this, "handleShowClientIconsChanged");
		XmlSettingsManager::Instance ().RegisterObject ("ActivityIcons",
				this, "handleActivityIconsetChanged");
		XmlSettingsManager::Instance ().RegisterObject ("MoodIcons",
				this, "handleMoodIconsetChanged");
		XmlSettingsManager::Instance ().RegisterObject ("SystemIcons",
				this, "handleSystemIconsetChanged");
	}

	void ContactListDelegate::paint (QPainter *painter,
			const QStyleOptionViewItem& sopt, const QModelIndex& index) const
	{
		QStyleOptionViewItemV4 o = sopt;
		Core::CLEntryType type = index.data (Core::CLREntryType).value<Core::CLEntryType> ();

		painter->save ();

		switch (type)
		{
		case Core::CLETAccount:
			DrawAccount (painter, o, index);
			break;
		case Core::CLETCategory:
			DrawCategory (painter, o, index);
			break;
		case Core::CLETContact:
			o.rect.adjust (-1.2 * View_->indentation (), 0, 0, 0);
			DrawContact (painter, o, index);
			break;
		}

		painter->restore ();
	}

	QSize ContactListDelegate::sizeHint (const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QSize size = QStyledItemDelegate::sizeHint (option, index);
		if (index.data (Core::CLREntryType).value<Core::CLEntryType> () == Core::CLETContact &&
				size.height () < 24)
			size.setHeight (24);
		return size;
	}

	void ContactListDelegate::DrawAccount (QPainter *painter,
			QStyleOptionViewItemV4 o, const QModelIndex& index) const
	{
		painter->save ();
		painter->setRenderHints (QPainter::HighQualityAntialiasing | QPainter::Antialiasing);
		
		QPainterPath rectPath;
		rectPath.addRoundedRect (o.rect, 8, 8);
		
		painter->fillPath (rectPath, o.palette.color (QPalette::Window));
		painter->setPen (o.palette.color (QPalette::WindowText));
		painter->drawPath (rectPath);
		
		painter->restore ();

		o.font.setBold (true);
		QStyledItemDelegate::paint (painter, o, index);
	}

	void ContactListDelegate::DrawCategory (QPainter *painter,
			QStyleOptionViewItemV4 o, const QModelIndex& index) const
	{
		const QRect& r = o.rect;

		const int textWidth = o.fontMetrics.width (index.data ().value<QString> () + " ");
		const int rem = r.width () - textWidth;

		const int visibleCount = index.model ()->rowCount (index);

		const QAbstractItemModel *model = index.model ();
		QModelIndex sourceIndex = index;
		while (const QAbstractProxyModel *proxyModel = qobject_cast<const QAbstractProxyModel*> (model))
		{
			model = proxyModel->sourceModel ();
			sourceIndex = proxyModel->mapToSource (sourceIndex);
		}

		const QString& str = QString (" %1/%2 ")
				.arg (visibleCount)
				.arg (model->rowCount (sourceIndex));
				
		painter->save ();
		
		painter->setRenderHints (QPainter::HighQualityAntialiasing | QPainter::Antialiasing);
		
		QPainterPath bgPath;
		bgPath.addRoundedRect (r.adjusted (-r.topLeft ().x (), 0, 0, 0), 6, 6);
		painter->drawPath (bgPath);
		
		if (rem >= o.fontMetrics.width (str))
		{
			if (o.state & QStyle::State_Selected)
				painter->setPen (o.palette.color (QPalette::HighlightedText));
			
			QFont font = painter->font ();
			font.setItalic (true);
			painter->setFont (font);

			const QRect numRect (r.left () + textWidth - 1, r.top () + CPadding,
					rem - 1, r.height () - 2 * CPadding);
			
			const QRect& br = painter->boundingRect (numRect,
					Qt::AlignVCenter | Qt::AlignRight, str).adjusted (0, 0, 1, 0);
			QPainterPath rectPath;
			rectPath.addRoundedRect (br, 4, 4);
			
			painter->fillPath (rectPath, o.palette.color (QPalette::Background));
			
			painter->drawText (numRect, Qt::AlignVCenter | Qt::AlignRight, str);

			painter->setPen (o.palette.color (QPalette::WindowText));
			painter->drawPath (rectPath);
		}
		
		painter->restore ();

		const int unread = index.data (Core::CLRUnreadMsgCount).toInt ();
		if (unread)
		{
			painter->save ();

			const QString& text = QString (" %1 :: ").arg (unread);

			QFont unreadFont = o.font;
			unreadFont.setBold (true);

			int unreadSpace = CPadding + QFontMetrics (unreadFont).width (text);

			painter->setFont (unreadFont);
			painter->drawText (r.left () + CPadding, r.top () + CPadding,
					unreadSpace, r.height () - 2 * CPadding,
					Qt::AlignVCenter | Qt::AlignLeft,
					text);

			painter->restore ();

			o.rect.setLeft (unreadSpace + o.rect.left ());
		}

		QStyledItemDelegate::paint (painter, o, index);
	}

	void ContactListDelegate::DrawContact (QPainter *painter,
			QStyleOptionViewItemV4 option, const QModelIndex& index) const
	{
		QObject *entryObj = index.data (Core::CLREntryObject).value<QObject*> ();
		ICLEntry *entry = qobject_cast<ICLEntry*> (entryObj);

		const bool isMUC = entry->GetEntryType () == ICLEntry::ETMUC;

		QStyle *style = option.widget ?
				option.widget->style () :
				QApplication::style ();

		const QRect& r = option.rect;
		const int sHeight = r.height ();
		const int iconSize = sHeight - 2 * CPadding;
		const int clientIconSize = (iconSize > 16) ? 16 : iconSize;

		const QIcon& stateIcon = index.data (Qt::DecorationRole).value<QIcon> ();
		QString name = index.data (Qt::DisplayRole).value<QString> ();
		const QString status = entry->GetStatus ().StatusString_.replace ('\n', ' ');
		const QImage& avatarImg = ShowAvatars_ ?
				Core::Instance ().GetAvatar (entry, iconSize - AvatarPaddingBottom) :
				QImage ();
		const int unreadNum = index.data (Core::CLRUnreadMsgCount).toInt ();
		const QString& unreadStr = unreadNum ?
				QString (" %1 :: ").arg (unreadNum) :
				QString ();
		if (!status.isEmpty ())
			name += " (" + status + ")";

		const bool selected = option.state & QStyle::State_Selected;
		const QColor fgColor = selected ?
				option.palette.color (QPalette::HighlightedText) :
				option.palette.color (QPalette::Text);

		QFont unreadFont;
		int unreadSpace = 0;
		if (unreadNum)
		{
			unreadFont = option.font;
			unreadFont.setBold (true);

			unreadSpace = CPadding + QFontMetrics (unreadFont).width (unreadStr);
		}

		const int textShift = 2 * CPadding + iconSize + unreadSpace;

		QList<QIcon> clientIcons = isMUC || !ShowClientIcons_ ?
				QList<QIcon> () :
				Core::Instance ().GetClientIconForEntry (entry).values ();
		if (entry->GetEntryType () == ICLEntry::ETPrivateChat)
		{
			const QByteArray& aff = index.data (Core::CLRAffiliation).toByteArray ();
			const QIcon& icon = Core::Instance ().GetAffIcon (aff);

			if (!icon.isNull ())
				clientIcons.prepend (icon);
		}
		const QStringList& vars = entry->Variants ();
		if (!vars.isEmpty ())
		{
			const QMap<QString, QVariant>& addInfo = entry->GetClientInfo (vars.first ());
			if (addInfo.contains ("user_activity"))
			{
				const QMap<QString, QVariant>& actInfo = addInfo ["user_activity"].toMap ();
				const QString& iconName = ActivityIconset_ + '/' +
						GetActivityIconName (actInfo ["general"].toString (),
								actInfo ["specific"].toString ());

				QIcon icon = ActivityIconCache_ [iconName];
				if (icon.isNull ())
					icon = QIcon (Core::Instance ()
							.GetResourceLoader (Core::RLTActivityIconLoader)->
									GetIconPath (iconName));

				if (!icon.isNull ())
				{
					clientIcons.prepend (icon);
					ActivityIconCache_ [iconName] = icon;
				}
			}
			if (addInfo.contains ("user_mood"))
			{
				const QMap<QString, QVariant>& moodInfo = addInfo ["user_mood"].toMap ();
				QString iconName = moodInfo ["mood"].toString ();
				iconName [0] = iconName.at (0).toUpper ();
				iconName.prepend (MoodIconset_ + '/');

				QIcon icon = MoodIconCache_ [iconName];
				if (icon.isNull ())
					icon = QIcon (Core::Instance ()
							.GetResourceLoader (Core::RLTMoodIconLoader)->
									GetIconPath (iconName));

				if (!icon.isNull ())
				{
					clientIcons.prepend (icon);
					MoodIconCache_ [iconName] = icon;
				}
			}
			if (addInfo.contains ("user_tune"))
				LoadSystemIcon ("/notification_roster_tune", clientIcons);
			
			ISupportGeolocation *geoloc =
					qobject_cast<ISupportGeolocation*> (entry->GetParentAccount ());
			if (geoloc)
			{
				const GeolocationInfo_t& info = geoloc->
						GetUserGeolocationInfo (entryObj, vars.value (0, QString ()));
				if (!info.isEmpty ())
					LoadSystemIcon ("/geolocation", clientIcons);
			}
		}

		const int clientsIconsWidth = clientIcons.isEmpty () ?
				0 :
				clientIcons.size () * (clientIconSize + CPadding);
		/* text for width is total width minus shift of the text from
		 * the left (textShift) minus space for avatar (if present) with
		 * paddings minus space for client icons and paddings between
		 * them: there are N-1 paddings inbetween if there are N icons.
		 */
		const int textWidth = r.width () - textShift -
				(isMUC || !ShowAvatars_ ? 0 : (clientIconSize + 2 * CPadding)) -
				clientsIconsWidth;

		QPixmap pixmap (r.size ());
		pixmap.fill (Qt::transparent);
		QPainter p (&pixmap);
		p.translate (-r.topLeft ());

		if (selected ||
				(option.state & QStyle::State_MouseOver))
			style->drawPrimitive (QStyle::PE_PanelItemViewItem,
					&option, &p, option.widget);

		p.setPen (fgColor);

		if (unreadNum)
		{
			p.setFont (unreadFont);
			p.drawText (r.left () + textShift - unreadSpace, r.top () + CPadding,
					textWidth, r.height () - 2 * CPadding,
					Qt::AlignVCenter | Qt::AlignLeft,
					unreadStr);
		}

		p.setFont (option.font);
		p.drawText (r.left () + textShift, r.top () + CPadding,
				textWidth, r.height () - 2 * CPadding,
				Qt::AlignVCenter | Qt::AlignLeft,
				option.fontMetrics.elidedText (name, Qt::ElideRight, textWidth));

		p.drawPixmap (r.topLeft () + QPoint (CPadding, CPadding),
				stateIcon.pixmap (iconSize, iconSize));

		if (!avatarImg.isNull ())
			p.drawPixmap (r.topLeft () + QPoint (textShift + textWidth + clientsIconsWidth + CPadding, CPadding),
					QPixmap::fromImage (avatarImg));

		int currentShift = textShift + textWidth + CPadding;

		Q_FOREACH (const QIcon& icon, clientIcons)
		{
			p.drawPixmap (r.topLeft () + QPoint (currentShift, CPadding),
					icon.pixmap (clientIconSize, clientIconSize));
			currentShift += clientIconSize + CPadding;
		}

		if (entry->GetEntryType () == ICLEntry::ETPrivateChat)
		{
			const QModelIndex& next = index.model ()->index (index.row () + 1, 0, index.parent ());
			if (next.isValid () &&
					next.data (Core::CLRRole) != index.data (Core::CLRRole))
			{
				p.setBrush (QColor (option.palette.color (QPalette::Text)));
				p.drawLine (r.bottomLeft (), r.bottomRight ());
			}
		}

		painter->drawPixmap (option.rect, pixmap);
	}
	
	void ContactListDelegate::LoadSystemIcon (const QString& name,
			QList<QIcon>& clientIcons) const
	{
		const QString& iconName = SystemIconset_ + name;
		QIcon icon = SystemIconCache_ [iconName];
		if (icon.isNull ())
			icon = QIcon (Core::Instance ()
					.GetResourceLoader (Core::RLTSystemIconLoader)->
							GetIconPath (iconName));
			
		if (!icon.isNull ())
		{
			clientIcons.prepend (icon);
			SystemIconCache_ [iconName] = icon;
		}
	}

	void ContactListDelegate::handleShowAvatarsChanged ()
	{
		ShowAvatars_ = XmlSettingsManager::Instance ()
				.property ("ShowAvatars").toBool ();
	}

	void ContactListDelegate::handleShowClientIconsChanged ()
	{
		ShowClientIcons_ = XmlSettingsManager::Instance ()
				.property ("ShowClientIcons").toBool ();
	}
	
	void ContactListDelegate::handleActivityIconsetChanged ()
	{
		ActivityIconCache_.clear ();
		
		ActivityIconset_ = XmlSettingsManager::Instance ()
				.property ("ActivityIcons").toString ();
	}
	
	void ContactListDelegate::handleMoodIconsetChanged ()
	{
		MoodIconCache_.clear ();
		
		MoodIconset_ = XmlSettingsManager::Instance ()
				.property ("MoodIcons").toString ();
	}
	
	void ContactListDelegate::handleSystemIconsetChanged ()
	{
		SystemIconCache_.clear ();
		
		SystemIconset_ = XmlSettingsManager::Instance ()
				.property ("SystemIcons").toString ();
	}
}
}
