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

#ifndef LISTENINGROOMMODEL_H
#define LISTENINGROOMMODEL_H

#include "playlist/PlayableModel.h"

class ListeningRoomModel : public PlayableModel
{
    Q_OBJECT
public:
    explicit ListeningRoomModel( QObject* parent = 0 );
    virtual ~ListeningRoomModel();

    Tomahawk::listeningroom_ptr listeningRoom() const { return m_listeningRoom; }

    virtual void loadListeningRoom( const Tomahawk::listeningroom_ptr& room, bool loadEntries = true );

signals:
    void listeningRoomDeleted();
    void listeningRoomChanged();

private:
    Tomahawk::listeningroom_ptr m_listeningRoom;
    
    bool m_isLoading;
};

#endif // LISTENINGROOMMODEL_H
