/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
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

#ifndef DATABASECOMMAND_LISTENINGROOMS_H
#define DATABASECOMMAND_LISTENINGROOMS_H

#include <QObject>
#include <QVariantMap>

#include "database/DatabaseCommandLoggable.h"

#include "DllMacro.h"

class DLLEXPORT DatabaseCommand_ListeningRooms : public DatabaseCommandLoggable
{
    Q_OBJECT
    Q_PROPERTY( int action READ action WRITE setAction )
public:
    enum Action
    {
        RoomInfo = 1
    };

    explicit DatabaseCommand_ListeningRooms( QObject* parent = 0 )
        : DatabaseCommandLoggable( parent )
    {}

    virtual QString commandname() const { return "listeningrooms"; }

    virtual void exec( DatabaseImpl* );
    virtual void postCommitHook();

    virtual bool doesMutates() const { return false; }
    virtual bool singletonCmd() const { return false; }
    virtual bool localOnly() const { return false; }
    virtual bool groupable() const { return true; }
    
    int action() const { return m_action; }
    void setAction( int a ) { m_action = static_cast< Action >( a ); }

signals:
    void roomCreated(); //TODO: actually use this or something like it

private:
    Action m_action;
};

#endif // DATABASECOMMAND_LISTENINGROOMS_H
