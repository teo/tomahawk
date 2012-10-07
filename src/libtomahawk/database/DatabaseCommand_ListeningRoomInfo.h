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

public:
    explicit DatabaseCommand_ListeningRoomInfo( QObject* parent = 0 );
    explicit DatabaseCommand_ListeningRoomInfo( const Tomahawk::source_ptr& author,
                                                const Tomahawk::listeningroom_ptr& listeningRoom );
    virtual ~DatabaseCommand_ListeningRoomInfo() {}

    QString commandname() const { return "listeningroominfo"; }

    virtual void exec( DatabaseImpl* lib );
    virtual void postCommitHook();

    QVariant listeningRoomV() const;

    void setListeningRoomV( const QVariant& v )
    {
        m_v = v;
    }

protected:
    void setListeningRoom( const Tomahawk::listeningroom_ptr& listeningRoom );

    QVariant m_v;
    
private:
    Tomahawk::listeningroom_ptr m_listeningRoom;
};

#endif // DATABASECOMMAND_LISTENINGROOMINFO_H
