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

#ifndef LISTENINGROOMMODEL_H
#define LISTENINGROOMMODEL_H

#include "playlist/PlayableModel.h"

class DLLEXPORT ListeningRoomModel : public PlayableModel
{
    Q_OBJECT

    typedef struct {
        int row;
        QPersistentModelIndex parent;
        Qt::DropAction action;
    } DropStorageData;

public:
    explicit ListeningRoomModel( QObject* parent = 0 );
    virtual ~ListeningRoomModel();

    QMimeData* mimeData( const QModelIndexList& indexes ) const;
    bool dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent );

    Tomahawk::listeningroom_ptr listeningRoom() const { return m_listeningRoom; }

public slots:    
    virtual void loadListeningRoom( const Tomahawk::listeningroom_ptr& room, bool loadEntries = true );

    void clear();

    virtual void appendEntries( const QList< Tomahawk::lrentry_ptr >& entries );

    void insertAlbums( const QList< Tomahawk::album_ptr >& albums, int row = 0 );
    void insertArtists( const QList< Tomahawk::artist_ptr >& artists, int row = 0 );
    void insertQueries( const QList< Tomahawk::query_ptr >& queries, int row = 0 );
    void insertEntriesFromView( const QList< Tomahawk::lrentry_ptr >& entries, int row = 0 );

    void removeIndex( const QModelIndex& index, bool moreToCome = false );

signals:
    void listeningRoomDeleted();
    void listeningRoomChanged();
    void listenersChanged();

protected:
    QList<Tomahawk::lrentry_ptr> listeningRoomEntries() const;

private slots:
    void parsedDroppedTracks( QList<Tomahawk::query_ptr> );
    void trackResolved( bool );
    void reload();
    void reloadRoomMetadata();
    void insertEntriesPrivate( const QList< Tomahawk::lrentry_ptr >& entries, int row = 0 );

private:
    void beginRoomChanges();
    void endRoomChanges();

    Tomahawk::listeningroom_ptr m_listeningRoom;
    
    bool m_isLoading;
    bool m_changesOngoing;
    QList< Tomahawk::Query* > m_waitingForResolved;

    int m_savedInsertPos;
    QList< Tomahawk::lrentry_ptr > m_savedInsertTracks;
    QList< Tomahawk::lrentry_ptr > m_savedRemoveTracks;

    DropStorageData m_dropStorage;
};

#endif // LISTENINGROOMMODEL_H
