/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2012, Teo Mrnjavac <teo@kde.org>
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

#ifndef DATABASECOMMAND_LISTENINGROOMINFO_H
#define DATABASECOMMAND_LISTENINGROOMINFO_H

#include "Typedefs.h"
#include "DatabaseCommandLoggable.h"
#include "qjson/qobjecthelper.h"

#include "DllMacro.h"

class DLLEXPORT DatabaseCommand_ListeningRoomInfo : public DatabaseCommandLoggable
{
    Q_OBJECT
    Q_PROPERTY( QVariant listeningRoom READ listeningRoomV WRITE setListeningRoomV )
    Q_PROPERTY( QString listeningRoomGuid READ listeningRoomGuid WRITE setListeningRoomGuid )
    Q_PROPERTY( int action READ action WRITE setAction )

public:
    //TODO: must use this same dbcmd to send delete messages as well.
    //Rationale: dbcmd_lri is a singleton, so only the last of its type gets executed.
    //For this reason it must have Actions, and if the last action in a non-trivial queue
    //is Delete the LR is never even shown

    enum Action
    {
        Info = 1,
        Disband
    };

    //Do not construct through this ctor
    explicit DatabaseCommand_ListeningRoomInfo( QObject* parent = 0 );

    //Named ctors, use these!
    static DatabaseCommand_ListeningRoomInfo* RoomInfo( const Tomahawk::source_ptr& author,
                                                        const Tomahawk::listeningroom_ptr& listeningRoom );

    static DatabaseCommand_ListeningRoomInfo* DisbandRoom( const Tomahawk::source_ptr& author,
                                                           const QString& roomGuid );

    virtual ~DatabaseCommand_ListeningRoomInfo() {}

    QString commandname() const { return "listeningroominfo"; }

    virtual void exec( DatabaseImpl* lib );
    virtual void postCommitHook();
    virtual bool singletonCmd() const { return true; }

    QVariant listeningRoomV() const;
    void setListeningRoomV( const QVariant& v ) { m_v = v; }

    QString listeningRoomGuid() const { return m_guid; }
    void setListeningRoomGuid( const QString& guid ) { m_guid = guid; }

    int action() const { return m_action; }
    void setAction( int a ) { m_action = static_cast< Action >( a ); }

protected:
    void setListeningRoom( const Tomahawk::listeningroom_ptr& listeningRoom );

    QVariant m_v;
    QString m_guid;

private:
    explicit DatabaseCommand_ListeningRoomInfo( Action action,
                                                const Tomahawk::source_ptr& author ); //used by named ctors

    Tomahawk::listeningroom_ptr m_listeningRoom;
    Action m_action;
};

#endif // DATABASECOMMAND_LISTENINGROOMINFO_H
