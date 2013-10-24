/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2012-2013, Teo Mrnjavac <teo@kde.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DATABASECOMMAND_PARTYINFO_H
#define DATABASECOMMAND_PARTYINFO_H

#include "Typedefs.h"
#include "DatabaseCommandLoggable.h"
#include "qjson/qobjecthelper.h"

#include "DllMacro.h"

namespace Tomahawk
{

class DLLEXPORT DatabaseCommand_PartyInfo : public DatabaseCommandLoggable
{
    Q_OBJECT
    Q_PROPERTY( QVariant party      READ partyV WRITE setPartyV )
    Q_PROPERTY( QString partyGuid   READ partyGuid WRITE setPartyGuid )
    Q_PROPERTY( int action          READ action WRITE setAction )

public:
    //Rationale: dbcmd_lri is a singleton, so only the last of its type gets executed.
    //For this reason it must have Actions, and if the last action in a non-trivial queue
    //is Delete the LR is never even shown

    enum Action
    {
        Broadcast = 1,
        Disband
    };

    //Do not construct through this ctor
    explicit DatabaseCommand_PartyInfo( QObject* parent = 0 );

    //Named ctors, use these!
    static DatabaseCommand_PartyInfo* broadcastParty( const Tomahawk::source_ptr& author,
                                                      const Tomahawk::party_ptr& party );

    static DatabaseCommand_PartyInfo* disbandParty( const Tomahawk::source_ptr& author,
                                                    const QString& partyGuid );

    virtual ~DatabaseCommand_PartyInfo() {}

    QString commandname() const { return "partyinfo"; }

    virtual void exec( DatabaseImpl* lib );
    virtual void postCommitHook();
    virtual bool singletonCmd() const { return true; }

    QVariant partyV() const;
    void setPartyV( const QVariant& v ) { m_v = v; }

    QString partyGuid() const { return m_guid; }
    void setPartyGuid( const QString& guid ) { m_guid = guid; }

    int action() const { return m_action; }
    void setAction( int a ) { m_action = static_cast< Action >( a ); }

protected:
    void setParty( const Tomahawk::party_ptr& party );

    QVariant m_v;
    QString m_guid;

private:
    explicit DatabaseCommand_PartyInfo( Action action,
                                        const Tomahawk::source_ptr& author ); //used by named ctors

    Tomahawk::party_ptr m_party;
    Action m_action;
};

} //ns

#endif // DATABASECOMMAND_PARTYINFO_H
