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

#include "sdsession.h"
#include <boost/bind.hpp>
#include <QStandardItemModel>
#include <QDomElement>
#include <QtDebug>
#include <QXmppDiscoveryIq.h>
#include "glooxaccount.h"
#include "clientconnection.h"
#include "sdmodel.h"
#include "capsmanager.h"
#include "vcarddialog.h"
#include "formbuilder.h"
#include "util.h"

namespace LeechCraft
{
namespace Azoth
{
namespace Xoox
{
	SDSession::SDSession (GlooxAccount *account)
	: Model_ (new SDModel (this))
	, Account_ (account)
	{
		ID2Action_ ["view-vcard"] = boost::bind (&SDSession::ViewVCard, this, _1);
		ID2Action_ ["add-to-roster"] = boost::bind (&SDSession::AddToRoster, this, _1);
		ID2Action_ ["register"] = boost::bind (&SDSession::Register, this, _1);
	}
	
	namespace
	{
		template<typename T>
		QList<QStandardItem*> AppendRow (T *parent,
				const QStringList& strings,
				const QString& jid,
				const QString& node)
		{
			QList<QStandardItem*> items;
			Q_FOREACH (const QString& string, strings)
			{
				QStandardItem *item = new QStandardItem (string);
				items << item;
				item->setEditable (false);
			}
			items.at (0)->setData (jid, SDSession::DRJID);
			items.at (0)->setData (node, SDSession::DRNode);
			parent->appendRow (items);
			return items;
		}
	}
	
	void SDSession::SetQuery (const QString& query)
	{
		Model_->clear ();
		Model_->setHorizontalHeaderLabels (QStringList (tr ("Name")) << tr ("JID") << tr ("Node"));

		QList<QStandardItem*> items = AppendRow (Model_,
				QStringList (query) << query << "",
				query,
				"");
		JID2Node2Item_ [query] [""] = items.at (0);
		
		items.at (0)->setData (true, DRFetchedMore);

		Account_->GetClientConnection ()->RequestInfo (query,
				boost::bind (&SDSession::HandleInfo, this, _1));
		Account_->GetClientConnection ()->RequestItems (query,
				boost::bind (&SDSession::HandleItems, this, _1));
	}
	
	QAbstractItemModel* SDSession::GetRepresentationModel () const
	{
		return Model_;
	}
	
	QList<QPair<QByteArray, QString> > SDSession::GetActionsFor (const QModelIndex& index)
	{
		QList<QPair<QByteArray, QString> > result;
		if (!index.isValid ())
			return result;
		
		const QModelIndex& sibling = index.sibling (index.row (), CName);
		QStandardItem *item = Model_->itemFromIndex (sibling);
		const ItemInfo& info = Item2Info_ [item];

		if (info.Caps_.contains ("vcard-temp") &&
				!info.JID_.isEmpty ())
			result << QPair<QByteArray, QString> ("view-vcard", tr ("View VCard..."));
		if (!info.JID_.isEmpty ())
			result << QPair<QByteArray, QString> ("add-to-roster", tr ("Add to roster..."));
		if (info.Caps_.contains ("jabber:iq:register"))
			result << QPair<QByteArray, QString> ("register", tr ("Register..."));

		return result;
	}
	
	void SDSession::ExecuteAction (const QModelIndex& index, const QByteArray& id)
	{
		if (!index.isValid ())
			return;
		
		const QModelIndex& sibling = index.sibling (index.row (), CName);
		QStandardItem *item = Model_->itemFromIndex (sibling);
		const ItemInfo& info = Item2Info_ [item];
		
		if (!ID2Action_.contains (id))
		{
			qWarning () << Q_FUNC_INFO
					<< "unknown ID"
					<< id;
			return;
		}
		
		ID2Action_ [id] (info);
	}
	
	namespace
	{
		struct Appender
		{
			QStringList Strings_;
			
			Appender ()
			{
			}
			
			Appender& operator() (const QString& text, const QString& name)
			{
				if (!text.isEmpty ())
					Strings_ << name + ' ' + text;
				
				return *this;
			}
			
			QString operator() () const
			{
				return Strings_.join ("<br/>");
			}
		};
	}
	
	void SDSession::HandleInfo (const QXmppDiscoveryIq& iq)
	{
		QStandardItem *item = JID2Node2Item_ [iq.from ()] [iq.queryNode ()];
		if (!item)
		{
			qWarning () << Q_FUNC_INFO
					<< "no parent node for"
					<< iq.from ();
			return;
		}
		
		const QModelIndex& index = item->index ();
		const QModelIndex& sibling = index.sibling (index.row (), CName);
		QStandardItem *targetItem = Model_->itemFromIndex (sibling);
		
		if (iq.identities ().size () == 1)
		{
			const QXmppDiscoveryIq::Identity& id = iq.identities ().at (0);
			const QString& text = id.name ();
			if (!text.isEmpty ())
				targetItem->setText (text);
		}
		
		QString tooltip = "<strong>" + tr ("Identities:") + "</strong><ul>";
		Q_FOREACH (const QXmppDiscoveryIq::Identity& id, iq.identities ())
		{
			if (id.name ().isEmpty ())
				continue;

			tooltip += "<li>";
			tooltip += Appender ()
					(id.name (), tr ("Identity name:"))
					(id.category (), tr ("Category:"))
					(id.type (), tr ("Type:"))
					(id.language (), tr ("Language:"))
					();
			tooltip += "</li>";
		}
		tooltip += "</ul>";

		const QStringList& caps = Account_->GetClientConnection ()->
				GetCapsManager ()->GetCaps (iq.features ());
		if (!caps.isEmpty ())
		{
			tooltip += "<strong>" + tr ("Capabilities:");
			tooltip += "</strong><ul><li>";
			tooltip += caps.join ("</li><li>");
			tooltip += "</li></ul>";
		}

		targetItem->setToolTip (tooltip);
		
		ItemInfo info =
		{
			iq.features (),
			iq.identities (),
			iq.from ()
		};
		Item2Info_ [targetItem] = info;
	}
	
	void SDSession::HandleItems (const QXmppDiscoveryIq& iq)
	{
		QStandardItem *parentItem = JID2Node2Item_ [iq.from ()] [iq.queryNode ()];
		if (!parentItem)
		{
			qWarning () << Q_FUNC_INFO
					<< "no parent node for"
					<< iq.from ();
			return;
		}
		
		Q_FOREACH (const QXmppDiscoveryIq::Item& item, iq.items ())
		{
			QList<QStandardItem*> items = AppendRow (parentItem,
					QStringList (item.name ()) << item.jid () << item.node (),
					item.jid (),
					item.node ());
			JID2Node2Item_ [item.jid ()] [item.node ()] = items.at (0);
			
			Account_->GetClientConnection ()->RequestInfo (item.jid (),
					boost::bind (&SDSession::HandleInfo, this, _1), item.node ());
		}
	}
	
	void SDSession::QueryItem (QStandardItem *item)
	{
		item->setData (true, DRFetchedMore);

		const QString& jid = item->data (DRJID).toString ();
		const QString& node = item->data (DRNode).toString ();
		Account_->GetClientConnection ()->RequestItems (jid,
				boost::bind (&SDSession::HandleItems, this, _1), node);
	}
	
	void SDSession::ViewVCard (const SDSession::ItemInfo& info)
	{
		const QString& jid = info.JID_;
		if (jid.isEmpty ())
			return;
		
		VCardDialog *dia = new VCardDialog;
		dia->show ();
		Account_->GetClientConnection ()->FetchVCard (jid, dia);
	}
	
	void SDSession::AddToRoster (const SDSession::ItemInfo& info)
	{
		const QString& jid = info.JID_;
		if (jid.isEmpty ())
			return;
		
		Account_->AddEntry (jid, QString (), QStringList ());
	}
	
	void SDSession::Register (const SDSession::ItemInfo& info)
	{
		const QString& jid = info.JID_;
		if (jid.isEmpty ())
			return;
		
		QXmppIq iq;
		iq.setType (QXmppIq::Get);
		iq.setTo (jid);
		QXmppElement elem;
		elem.setTagName ("query");
		elem.setAttribute ("xmlns", "jabber:iq:register");
		iq.setExtensions (QXmppElementList (elem));

		Account_->GetClientConnection ()->SendPacketWCallback (iq, this, "handleRegistrationForm");
	}
	
	void SDSession::handleRegistrationForm (const QXmppIq& iq)
	{
		QXmppDataForm form;
		Q_FOREACH (const QXmppElement& elem, iq.extensions ())
		{
			if (elem.tagName () != "query" ||
					elem.attribute ("xmlns") != "jabber:iq:register")
				continue;
			
			const QXmppElement& x = elem.firstChildElement ("x");
			form.parse (XooxUtil::XmppElem2DomElem (x));
			if (!form.isNull ())
				break;
		}
		
		if (form.isNull ())
		{
			qWarning () << Q_FUNC_INFO
					<< "no form found, sorry";
			return;
		}
		
		FormBuilder builder;
		QWidget *widget = builder.CreateForm (form);
		std::auto_ptr<QDialog> dialog (new QDialog ());
		dialog->setWindowTitle (widget->windowTitle ());
		dialog->setLayout (new QVBoxLayout ());
		dialog->layout ()->addWidget (widget);
		QDialogButtonBox *box = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		dialog->layout ()->addWidget (box);
		connect (box,
				SIGNAL (accepted ()),
				dialog.get (),
				SLOT (accept ()));
		connect (box,
				SIGNAL (rejected ()),
				dialog.get (),
				SLOT (reject ()));
		
		if (dialog->exec () != QDialog::Accepted)
			return;
		
		form = builder.GetForm ();
		
		QXmppIq regIq;
		regIq.setType (QXmppIq::Set);
		regIq.setTo (iq.from ());
		QXmppElement elem;
		elem.setTagName ("query");
		elem.setAttribute ("xmlns", "jabber:iq:register");
		elem.appendChild (XooxUtil::Form2XmppElem (form));

		regIq.setExtensions (QXmppElementList (elem));
		
		Account_->GetClientConnection ()->GetClient ()->sendPacket (regIq);
		Account_->AddEntry (iq.from (), QString (), QStringList (tr ("Gateways")));
	}
}
}
}
