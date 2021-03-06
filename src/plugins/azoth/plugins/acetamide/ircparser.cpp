/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2010-2011  Oleg Linkin
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

#include "ircparser.h"
#include <boost/bind.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_loops.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <QTextCodec>
#include "ircaccount.h"
#include "ircserverhandler.h"

namespace LeechCraft
{
namespace Azoth
{
namespace Acetamide
{
	using namespace boost::spirit::classic;

	IrcParser::IrcParser (IrcServerHandler *sh)
	: ISH_ (sh)
	, ServerOptions_ (sh->GetServerOptions ())
	{
	}

	void IrcParser::AuthCommand ()
	{
		QString pass = ServerOptions_.ServerPassword_;
		if (!pass.isEmpty ())
		{
			QString passCmd = QString ("PASS " + pass + "\r\n");
			ISH_->SendCommand (passCmd);
		}

		UserCommand ();
		NickCommand (QStringList () << ISH_->GetNickName ());
	}

	void IrcParser::UserCommand ()
	{
		QString userCmd = QString ("USER " +
				ISH_->GetAccount ()->GetUserName () + " 8 * :" +
				ISH_->GetAccount ()->GetRealName () + "\r\n");
		ISH_->SendCommand (userCmd);
	}

	void IrcParser::NickCommand (const QStringList& nick)
	{
		const QString name = !nick.isEmpty () ?  EncodingList (nick).at (0)
				: QString ();
		QString nickCmd = QString ("NICK " + name + "\r\n");
		ISH_->SendCommand (nickCmd);
	}

	void IrcParser::JoinCommand (const QString& channel)
	{
		QString joinCmd = QString ("JOIN " + channel + "\r\n");
		ISH_->SendCommand (joinCmd);
	}

	void IrcParser::PrivMsgCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		const QString target = encodedCmd.first ();
		const QStringList msg = encodedCmd.mid (1);
		QString privmsgCmd = QString ("PRIVMSG " + target + " :" +
				msg.join (" ") + "\r\n");
		ISH_->SendCommand (privmsgCmd);
	}

	void IrcParser::PartCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		QString partCmd = QString ("PART " + encodedCmd.first () + " :" +
				QStringList (encodedCmd.mid (1)).join (" ") + "\r\n");
		ISH_->SendCommand (partCmd);
	}

	void IrcParser::PongCommand (const QStringList& msg)
	{
		QString pongCmd = QString ("PONG :" + EncodingList (msg).join (" ") + 
				"\r\n");
		ISH_->SendCommand (pongCmd);
	}

	void IrcParser::RawCommand (const QStringList& cmd)
	{
		QString rawCmd = EncodingList (cmd).join (" ") + "\r\n";
		ISH_->SendCommand (rawCmd);
	}

	void IrcParser::CTCPRequest (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		if (!encodedCmd.count ())
			return;
		QString ctcpCmd;
		if (encodedCmd.count () > 2)
			ctcpCmd = "PRIVMSG " + encodedCmd.first () + " :\001" +
					encodedCmd.at (1) + " " + QStringList (encodedCmd.mid (2)).join (" ") + 
					"\001\r\n";
		else
			ctcpCmd = "PRIVMSG " + encodedCmd.first () + " :\001" +
					encodedCmd.at (1) + "\001\r\n";
		ISH_->SendCommand (ctcpCmd);
	}

	void IrcParser::CTCPReply (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		if (!encodedCmd.count ())
			return;

		QString ctcpCmd = QString ("NOTICE " + encodedCmd.first () + " :" +
				encodedCmd.last () + "\r\n");
		ISH_->SendCommand (ctcpCmd);
	}

	void IrcParser::TopicCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		if (!encodedCmd.count ())
			return;

		QString topicCmd;
		if (encodedCmd.count () == 1)
			topicCmd = QString ("TOPIC " + encodedCmd.first () + "\r\n");
		else
			topicCmd = QString ("TOPIC " + encodedCmd.first () + " :" +
					QStringList (encodedCmd.mid (1)).join (" ") + "\r\n");
		ISH_->SendCommand (topicCmd);
	}

	void IrcParser::NamesCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		QString target = QString ();
		if (encodedCmd.count ())
			target =  encodedCmd.first ();

		QString namesCmd = QString ("NAMES " + target + "\r\n");
		ISH_->SendCommand (namesCmd);
	}

	void IrcParser::InviteCommand (const QStringList& cmd)
	{
		QString inviteCmd = QString ("INVITE " + EncodingList (cmd).join (" ") +
				"\r\n");
		ISH_->SendCommand (inviteCmd);
	}

	void IrcParser::KickCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		QString kickCmd = QString ("KICK " + encodedCmd.join (" ") + "\r\n");
		ISH_->SendCommand (kickCmd);
	}

	void IrcParser::OperCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		QString operCmd = QString ("OPER " + encodedCmd.at (0) + " :" +
				QStringList (encodedCmd.mid (1)).join (" ") + "\r\n");
		ISH_->SendCommand (operCmd);
	}

	void IrcParser::SQuitCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		QString squitCmd = QString ("SQUIT " + encodedCmd.first () + " :" +
				QStringList (encodedCmd.mid (1)).join (" ") + "\r\n");
		ISH_->SendCommand (squitCmd);
	}

	void IrcParser::MOTDCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		QString motdCmd;
		if (!encodedCmd.count ())
			motdCmd = QString ("MOTD\r\n");
		else
			motdCmd = QString ("MOTD " + encodedCmd.first () + "\r\n");

		ISH_->SendCommand (motdCmd);
	}

	void IrcParser::LusersCommand (const QStringList& cmd)
	{
		QString lusersCmd = QString ("LUSERS " + EncodingList (cmd).join (" ") +
				"\r\n");
		ISH_->SendCommand (lusersCmd);
	}

	void IrcParser::VersionCommand (const QStringList& cmd)
	{
		QString versionCmd = QString ("VERSION " + EncodingList (cmd).join (" ") +
				"\r\n");
		ISH_->SendCommand (versionCmd);
	}

	void IrcParser::StatsCommand (const QStringList& cmd)
	{
		QString statsCmd = QString ("STATS " + EncodingList (cmd).join (" ") + 
				"\r\n");
		ISH_->SendCommand (statsCmd);
	}

	void IrcParser::LinksCommand (const QStringList& cmd)
	{
		QString linksCmd = QString ("LINKS " + EncodingList (cmd).join (" ") + 
				"\r\n");
		ISH_->SendCommand (linksCmd);
	}

	void IrcParser::TimeCommand (const QStringList& cmd)
	{
		QString timeCmd = QString ("TIME " + EncodingList (cmd).join (" ") + 
				"\r\n");
		ISH_->SendCommand (timeCmd);
	}

	void IrcParser::ConnectCommand (const QStringList& cmd)
	{
		QString connectCmd = QString ("CONNECT " + EncodingList (cmd).join (" ") +
				"\r\n");
		ISH_->SendCommand (connectCmd);
	}

	void IrcParser::TraceCommand (const QStringList& cmd)
	{
		QString traceCmd = QString ("TRACE " + EncodingList (cmd).join (" ") + 
				"\r\n");
		ISH_->SendCommand (traceCmd);
	}

	void IrcParser::AdminCommand (const QStringList& cmd)
	{
		QString adminCmd = QString ("ADMIN " + EncodingList (cmd).join (" ") + 
				"\r\n");
		ISH_->SendCommand (adminCmd);
	}

	void IrcParser::InfoCommand (const QStringList& cmd)
	{
		QString infoCmd = QString ("INFO " + EncodingList (cmd).join (" ") + 
				"\r\n");
		ISH_->SendCommand (infoCmd);
	}

	void IrcParser::WhoCommand (const QStringList& cmd)
	{
		QString whoCmd = QString ("WHO " + EncodingList (cmd).join (" ") + 
				"\r\n");
		ISH_->SendCommand (whoCmd);
	}

	void IrcParser::WhoisCommand (const QStringList& cmd)
	{
		QString whoisCmd = QString ("WHOIS " + EncodingList (cmd).join (" ") + 
				"\r\n");
		ISH_->SendCommand (whoisCmd);
	}

	void IrcParser::WhowasCommand (const QStringList& cmd)
	{
		QString whowasCmd = QString ("WHOWAS " + EncodingList (cmd).join (" ") +
				"\r\n");
		ISH_->SendCommand (whowasCmd);
	}

	void IrcParser::KillCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		QString killCmd = QString ("KILL " + encodedCmd.first () + " :" +
				QStringList (encodedCmd.mid (1)).join (" ") + "\r\n");
		ISH_->SendCommand (killCmd);
	}

	void IrcParser::PingCommand (const QStringList& cmd)
	{
		QString pingCmd = QString ("PING " + EncodingList (cmd).join (" ") + 
				"\r\n");
		ISH_->SendCommand (pingCmd);
	}

	void IrcParser::AwayCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		QString awayCmd;
		if (encodedCmd.count ())
			awayCmd = QString ("AWAY :" + encodedCmd.join (" ") + "\r\n");
		else
			awayCmd = QString ("AWAY\r\n");
		ISH_->SendCommand (awayCmd);
	}

	void IrcParser::RehashCommand (const QStringList&)
	{
		QString rehashCmd = QString ("REHASH\r\n");
		ISH_->SendCommand (rehashCmd);
	}

	void IrcParser::DieCommand (const QStringList&)
	{
		QString dieCmd = QString ("DIE\r\n");
		ISH_->SendCommand (dieCmd);
	}

	void IrcParser::RestartCommand (const QStringList&)
	{
		QString dieCmd = QString ("RESTART\r\n");
		ISH_->SendCommand (dieCmd);
	}

	void IrcParser::SummonCommand (const QStringList& cmd)
	{
		QStringList encodedCmd = EncodingList (cmd);
		QString summonCmd = QString ("SUMMON " + encodedCmd.first () +
				QStringList (encodedCmd.mid (1)).join (" ") + "\r\n");
		ISH_->SendCommand (summonCmd);
	}

	void IrcParser::UsersCommand (const QStringList& cmd)
	{
		QString usersCmd = QString ("USERS " + EncodingList (cmd).first () + "\r\n");
		ISH_->SendCommand (usersCmd);
	}

	void IrcParser::UserhostCommand (const QStringList& cmd)
	{
		QString userhostCmd = QString ("USERHOST " + EncodingList (cmd).join (" ") +
				"\r\n");
		ISH_->SendCommand (userhostCmd);
	}

	void IrcParser::WallopsCommand (const QStringList& cmd)
	{
		QString wallopsCmd = QString ("WALLOPS :" + EncodingList (cmd).join (" ") +
				"\r\n");
		ISH_->SendCommand (wallopsCmd);
	}

	void IrcParser::IsonCommand (const QStringList& cmd)
	{
		QString isonCmd = QString ("ISON " + EncodingList (cmd).join (" ") + "\r\n");
		ISH_->SendCommand (isonCmd);
	}

	void IrcParser::QuitCommand (const QStringList& cmd)
	{
		QString quitCmd = QString ("QUIT :" + EncodingList (cmd).join (" ") + "\r\n");
		ISH_->SendCommand (quitCmd);
	}

	bool IrcParser::ParseMessage (const QByteArray& message)
	{
		QString msg;
		QTextCodec *codec = QTextCodec::codecForName (ISH_->
				GetServerOptions ().ServerEncoding_.toUtf8 ());
		msg = codec->toUnicode (message);

		IrcMessageOptions_.Command_.clear ();
		IrcMessageOptions_.Nick_.clear ();
		IrcMessageOptions_.Message_.clear ();
		IrcMessageOptions_.Parameters_.clear ();

		std::string nickStr;
		std::string commandStr;
		std::string msgStr;
		QList<std::string> opts;

		range<> ascii (char (0x01), char (0x7F));
		rule<> special = lexeme_d [ch_p ('[') | ']' | '\\' | '`' |
				'_' | '^' | '{' | '|' | '}'];
		rule<> shortname = *(alnum_p
				>> *(alnum_p || ch_p ('-'))
				>> *alnum_p);
		rule<> hostname = shortname
				>> *(ch_p ('.')
				>> shortname);
		rule<> nickname = (alpha_p | special)
				>> * (alnum_p | special | ch_p ('-'));
		rule<> user =  +(ascii - '\r' - '\n' - ' ' - '@' - '\0');
		rule<> host = lexeme_d [+(anychar_p - ' ')] ;
		rule<> nick = lexeme_d [nickname [assign_a (nickStr)]
				>> !(!(ch_p ('!')
				>> user)
				>> ch_p ('@')
				>> host)];
		rule<> nospcrlfcl = (anychar_p - '\0' - '\r' - '\n' -
				' ' - ':');
		rule<> lastParam = lexeme_d [ch_p (' ')
				>> !ch_p (':')
				>> (*(ch_p (':') | ch_p (' ') | nospcrlfcl))
					[assign_a (msgStr)]];
		rule<> firsParam = lexeme_d [ch_p (' ')
				>> (nospcrlfcl
				>> *(ch_p (':') | nospcrlfcl))
					[push_back_a (opts)]];
		rule<> params =  *firsParam
				>> !lastParam;
		rule<> command = longest_d [(+alpha_p) |
				(repeat_p (3) [digit_p])][assign_a (commandStr)];
		rule<> prefix = longest_d [hostname | nick];
		rule<> reply = (lexeme_d [!(ch_p (':')
				>> prefix >> ch_p (' '))]
				>> command
				>> !params
				>> eol_p);

		bool res = parse (msg.toUtf8 ().constData (), reply).full;

		if (!res)
		{
			qWarning () << "input string is not a valide IRC command"
					<< msg;
			return false;
		}
		else
		{
			IrcMessageOptions_.Nick_ =
					QString::fromUtf8 (nickStr.c_str ());
			IrcMessageOptions_.Command_ =
					QString::fromUtf8 (commandStr.c_str ()).toLower ();
			IrcMessageOptions_.Message_ =
					QString::fromUtf8 (msgStr.c_str ());
			IrcMessageOptions_.Parameters_ = opts;
		}

		return true;
	}

	IrcMessageOptions IrcParser::GetIrcMessageOptions () const
	{
		return IrcMessageOptions_;
	}

	QStringList IrcParser::EncodingList (const QStringList& list)
	{
		QTextCodec *codec = QTextCodec::codecForName (ISH_->
				GetServerOptions ().ServerEncoding_.toUtf8 ());
		QStringList encodedList;
		Q_FOREACH (const QString& str, list)
		{
			encodedList << codec->fromUnicode (str);
		}

		return encodedList;
	}

};
};
};
