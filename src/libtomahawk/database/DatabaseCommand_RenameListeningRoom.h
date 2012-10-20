/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2012,      Teo Mrnjavac <teo@kde.org>
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

#ifndef DATABASECOMMAND_RENAMELISTENINGROOM_H
#define DATABASECOMMAND_RENAMELISTENINGROOM_H

#include "Typedefs.h"
#include "DatabaseCommandLoggable.h"

#include "DllMacro.h"

class DatabaseImpl;

class DLLEXPORT DatabaseCommand_RenameListeningRoom : public DatabaseCommandLoggable
{
Q_OBJECT
Q_PROPERTY( QString listeningRoomGuid READ listeningRoomGuid WRITE setListeningRoomguid )
Q_PROPERTY( QString listeningRoomTitle READ listeningRoomTitle WRITE setListeningRoomTitle )

public:
    explicit DatabaseCommand_RenameListeningRoom( QObject* parent = 0 )
            : DatabaseCommandLoggable( parent )
    {}

    explicit DatabaseCommand_RenameListeningRoom( const Tomahawk::source_ptr& source,
                                                  const QString& listeningRoomGuid,
                                                  const QString& listeningRoomTitle );

    QString commandname() const { return "renamelisteningroom"; }

    virtual void exec( DatabaseImpl* lib );
    virtual void postCommitHook();
    virtual bool doesMutates() const { return false; }

    QString listeningRoomGuid() const { return m_listeningRoomGuid; }
    void setListeningRoomguid( const QString& s ) { m_listeningRoomGuid = s; }

    QString listeningRoomTitle() const { return m_listeningRoomTitle; }
    void setListeningRoomTitle( const QString& s ) { m_listeningRoomTitle = s; }

private:
    QString m_listeningRoomGuid;
    QString m_listeningRoomTitle;
};

#endif // DATABASECOMMAND_RENAMELISTENINGROOM_H
