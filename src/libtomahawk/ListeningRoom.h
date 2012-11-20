/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2010-2011, Leo Franchi <lfranchi@kde.org>
 *   Copyright 2010-2012, Jeff Mitchell <jeff@tomahawk-player.org>
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

#ifndef LISTENINGROOM_H
#define LISTENINGROOM_H

#include <QVariant>
#include <QSharedPointer>
#include <QQueue>
#include "qobjecthelper.h"

#include "Typedefs.h"
#include "DllMacro.h"

class SourceTreePopupDialog;
class DatabaseCommand_ListeningRoomInfo;
class ListeningRoomModel;

namespace Tomahawk
{

class DLLEXPORT ListeningRoomEntry : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString guid              READ guid         WRITE setGuid )
    Q_PROPERTY( unsigned int duration     READ duration     WRITE setDuration )
    Q_PROPERTY( unsigned int lastmodified READ lastmodified WRITE setLastmodified )
    Q_PROPERTY( QVariant query            READ queryVariant WRITE setQueryVariant )
    Q_PROPERTY( int score                 READ score        WRITE setScore )

public:
    ListeningRoomEntry() {}
    virtual ~ListeningRoomEntry() {}

    bool isValid() const { return !m_query.isNull(); }

    const Tomahawk::query_ptr& query() const { return m_query; }
    void setQuery( const Tomahawk::query_ptr& q ) { m_query = q; }

    QVariant queryVariant() const;
    void setQueryVariant( const QVariant& v );

    QString guid() const { return m_guid; }
    void setGuid( const QString& s ) { m_guid = s; }

    QString resultHint() const { return m_resulthint; }
    void setResultHint( const QString& s ) { m_resulthint= s; }

    unsigned int duration() const { return m_duration; }
    void setDuration( unsigned int i ) { m_duration = i; }

    unsigned int lastmodified() const { return m_lastmodified; }
    void setLastmodified( unsigned int i ) { m_lastmodified = i; }

    source_ptr lastSource() const { return m_lastsource; }
    void setLastSource( source_ptr s ) { m_lastsource = s; }

    int score() const { return m_score; }
    void setScore( int score ) { m_score = score; }

    //TODO: my vote?

private:
    QString             m_guid;
    Tomahawk::query_ptr m_query;
    unsigned int        m_duration;
    unsigned int        m_lastmodified;
    source_ptr          m_lastsource;
    QString             m_resulthint;
    int                 m_score;
    int                 m_myVote; //0=default, can be -1, 0, 1
};


class DLLEXPORT ListeningRoom : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString guid            READ guid               WRITE setGuid )
    Q_PROPERTY( QString title           READ title              WRITE setTitle )
    Q_PROPERTY( QString creator         READ creator            WRITE setCreator )
    Q_PROPERTY( uint createdon          READ createdOn          WRITE setCreatedOn )
    Q_PROPERTY( QVariantList entries    READ entriesV           WRITE setEntriesV )
    Q_PROPERTY( QVariantList listenerIds READ listenerIdsV      WRITE setListenerIdsV )
    Q_PROPERTY( int currentRow          READ currentRow         WRITE setCurrentRow )

    friend class ::DatabaseCommand_ListeningRoomInfo;
    friend class ::ListeningRoomModel;

public:
    virtual ~ListeningRoom();

    static Tomahawk::listeningroom_ptr load( const QString& guid );

    static Tomahawk::listeningroom_ptr create( const Tomahawk::source_ptr& author,
                                               const QString& guid,
                                               const QString& title,
                                               const QString& creator,
                                               const QList<query_ptr>& queries = QList< Tomahawk::query_ptr >() );

    static void remove( const listeningroom_ptr& room );
    void rename( const QString& title );

    void updateFrom( const Tomahawk::listeningroom_ptr& other );

    source_ptr author() const       { return m_source; }
    QString title() const           { return m_title; }
    QString creator() const         { return m_creator; }
    QString guid() const            { return m_guid; }
    uint lastmodified() const       { return m_lastmodified; }
    uint createdOn() const          { return m_createdOn; }
    int currentRow() const          { return m_currentRow; }

    const QList< Tomahawk::lrentry_ptr >& entries() const { return m_entries; }

    // Even though lrentry_ptr is an honest QMetaObject registered type, and even though a QList of
    // those can be a Q_PROPERTY, since a QList of pointers to QObjects is not a QObject it cannot
    // be QVariantified automagically with QObjectHelper for JSON serialization.
    // Trying to QVariantify it makes qobject2qvariant and the QJson serializer (wait for it...)
    // FAIL SILENTLY just for kicks.
    // For this reason, we must provide special getter/setters for a QVariantList converted version
    // of m_entries.
    QVariantList entriesV() const;

    void insertEntry( const Tomahawk::query_ptr& query, int position );
    void addEntries( const QList< Tomahawk::query_ptr >& queries );
    void insertEntries( const QList< Tomahawk::query_ptr >& queries,
                        const int position );

    void moveEntries( const QList< Tomahawk::lrentry_ptr >& entries, int position );

    void removeEntries( const QList< Tomahawk::lrentry_ptr >& entries );


    // <IGNORE hack="true">
    // these need to exist and be public for the json serialization stuff
    // you SHOULD NOT call them.  They are used for an alternate CTOR method from json.
    // maybe friend QObjectHelper and make them private?
    explicit ListeningRoom( const source_ptr& author );
    void setCreator( const QString& s )         { m_creator = s; }
    void setGuid( const QString& s )            { m_guid = s; }
    void setCreatedOn( uint createdOn )         { m_createdOn = createdOn; }
    void setTitle(const QString& title );
    void setEntriesV( const QVariantList& l );
    void setListenerIdsV( const QVariantList& v );
    QVariantList listenerIdsV() const;
    void setCurrentRow( int row )               { m_currentRow = row; }
    // </IGNORE>

    QStringList listenerIds() const { return m_listenerIds; }

    QList< lrentry_ptr > entriesFromQueries( const QList< Tomahawk::query_ptr >& queries, bool clearFirst = false );

    Tomahawk::playlistinterface_ptr playlistInterface();

signals:
    void created();

    void changed();
    void renamed( const QString& newTitle, const QString& oldTitle );

    void aboutToBeDeleted( const Tomahawk::listeningroom_ptr& lr );
    void deleted( const Tomahawk::listeningroom_ptr& lr );

    /// Notification for tracks being inserted at a specific point
    /// Contiguous range from startPosition
    void tracksInserted( const QList< Tomahawk::lrentry_ptr >& tracks, int startPosition );

    /// Notification for tracks being removed from the room
    void tracksRemoved( const QList< Tomahawk::lrentry_ptr >& tracks );

    /// Notification for tracks being moved in a room. List is of new tracks, and new position of first track
    /// Contiguous range from startPosition
    void tracksMoved( const QList< Tomahawk::lrentry_ptr >& tracks, int startPosition );

    void listenersChanged();

public slots:
    void reportCreated( const Tomahawk::listeningroom_ptr& self );
    void reportDeleted( const Tomahawk::listeningroom_ptr& self );

    // Only ever used by the LR host. Listeners do not add or remove themselves here, they just
    // latch on/off and the host takes care of adding them here and notifying everybody.
    void addListener( const Tomahawk::source_ptr& listener );
    void removeListener( const Tomahawk::source_ptr& listener );
    void onListenerOffline();

    void resolve();

    void setWeakSelf( QWeakPointer< ListeningRoom > self ) { m_weakSelf = self; }
private slots:
    // Only ever used by the LR host!
    void pushUpdate();

    void onResultsChanged();
    void onResolvingFinished();

    void onDeleteResult( SourceTreePopupDialog* dialog );

private:
    explicit ListeningRoom( const source_ptr& author,
                            const QString& guid,
                            const QString& title,
                            const QString& creator,
                            const QList< Tomahawk::lrentry_ptr >& entries = QList< Tomahawk::lrentry_ptr >() );


    ListeningRoom();
    void init();

    QWeakPointer< ListeningRoom > m_weakSelf;

    bool m_locallyChanged;
    bool m_deleted;

    source_ptr m_source;
    QString m_guid;
    QString m_title;
    QString m_creator;
    uint m_lastmodified;
    uint m_createdOn;
    QList< Tomahawk::lrentry_ptr > m_entries;
    QStringList m_listenerIds;
    int m_currentRow;

    Tomahawk::playlistinterface_ptr m_playlistInterface;
};

} //namespace Tomahawk

Q_DECLARE_METATYPE( Tomahawk::listeningroom_ptr )
Q_DECLARE_METATYPE( Tomahawk::lrentry_ptr )
Q_DECLARE_METATYPE( QList<Tomahawk::lrentry_ptr> )

#endif // LISTENINGROOM_H
