/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2010-2011, Leo Franchi <lfranchi@kde.org>
 *   Copyright 2010-2012, Jeff Mitchell <jeff@tomahawk-player.org>
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

#ifndef PARTY_H
#define PARTY_H

#include <QVariant>
#include <QSharedPointer>
#include <QQueue>
#include <qjson/qobjecthelper.h>

#include "Typedefs.h"
#include "DllMacro.h"

class SourceTreePopupDialog;
class DatabaseCommand_PartyInfo;
class PartyModel;

namespace Tomahawk
{


class DLLEXPORT Party : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString guid             READ guid              WRITE setGuid )
    Q_PROPERTY( uint createdon           READ createdOn         WRITE setCreatedOn )
    Q_PROPERTY( QVariantList listenerIds READ listenerIdsV      WRITE setListenerIdsV )
    Q_PROPERTY( int currentRow           READ currentRow        WRITE setCurrentRow )
    Q_PROPERTY( QString playlistGuid     READ playlistGuid      WRITE setPlaylistByGuid )

    friend class ::DatabaseCommand_PartyInfo;
    friend class ::PartyModel;

public:
    virtual ~Party();

    // User-callable named ctors. On a peer that isn't a host, these should not be used.
    static Tomahawk::party_ptr createFromPlaylist( const Tomahawk::playlist_ptr& playlist );

    static Tomahawk::party_ptr createNew( const QString& title,
                                          const QList<query_ptr>& queries = QList< Tomahawk::query_ptr >() );

    static Tomahawk::party_ptr load( const QString& guid );

    static void remove( const party_ptr& party );

    void updateFrom( const Tomahawk::party_ptr& other );

    source_ptr author() const;
    QString guid() const;
    uint createdOn() const;
    int currentRow() const;
    playlist_ptr playlist() const;

    // <IGNORE hack="true">
    // these need to exist and be public for the json serialization stuff
    // you SHOULD NOT call them.  They are used for an alternate CTOR method from json.
    // maybe friend QObjectHelper and make them private?
    explicit Party( const source_ptr& author );
    void setGuid( const QString& s );
    void setCreatedOn( uint createdOn );
    void setListenerIdsV( const QVariantList& v );
    QVariantList listenerIdsV() const;
    void setCurrentRow( int row );
    void setPlaylistByGuid( const QString& guid );
    QString playlistGuid() const;
    // </IGNORE>

    QStringList listenerIds() const;

    Tomahawk::playlistinterface_ptr playlistInterface();

signals:
    void created();

    void changed();

    void aboutToBeDeleted( const Tomahawk::party_ptr& lr );
    void deleted( const Tomahawk::party_ptr& lr );

    void listenersChanged();

public slots:
    void reportCreated( const Tomahawk::party_ptr& self );
    void reportDeleted( const Tomahawk::party_ptr& self );

    // Only ever used by the LR host. Listeners do not add or remove themselves here, they just
    // latch on/off and the host takes care of adding them here and notifying everybody.
    void addListener( const Tomahawk::source_ptr& listener );
    void removeListener( const Tomahawk::source_ptr& listener );
    void onListenerOffline();

    void setWeakSelf( QWeakPointer< Party > self );
private slots:
    // Only ever used by the LR host!
    void pushUpdate();

    void onDeleteResult( SourceTreePopupDialog* dialog );

private:
    explicit Party( const source_ptr& author,
                    const QString& guid,
                    const playlist_ptr& basePlaylist );


    Party();
    void init();

    QWeakPointer< Party > m_weakSelf;

    bool m_locallyChanged;
    bool m_deleted;

    source_ptr m_source;
    QString m_guid;
    uint m_createdOn;
    Tomahawk::playlist_ptr m_playlist;
    QStringList m_listenerIds;
    int m_currentRow;

    Tomahawk::playlistinterface_ptr m_playlistInterface;
};

} //namespace Tomahawk

Q_DECLARE_METATYPE( Tomahawk::party_ptr )

#endif // PARTY_H
