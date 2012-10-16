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

#include "Typedefs.h"
#include "DllMacro.h"

class SourceTreePopupDialog;
class DatabaseCommand_ListeningRoomInfo;

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

struct ListeningRoomRevision
{
    QString revisionguid;
    QString oldrevisionguid;
    QList< lrentry_ptr > newlist;
    QList< lrentry_ptr > added;
    QList< lrentry_ptr > removed;
    bool applied; // false if conflict
};

struct ListeningRoomRevisionQueueItem
{
    QString newRev;
    QString oldRev;
    QList< lrentry_ptr > entries;
    bool applyToTip;

    ListeningRoomRevisionQueueItem( const QString& nRev,
                                    const QString& oRev,
                                    const QList< lrentry_ptr >& e,
                                    bool latest )
        : newRev( nRev )
        , oldRev( oRev)
        , entries( e )
        , applyToTip( latest )
    {}
};

class DLLEXPORT ListeningRoom : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString guid            READ guid               WRITE setGuid )
    Q_PROPERTY( QString currentrevision READ currentrevision    WRITE setCurrentrevision )
    Q_PROPERTY( QString title           READ title              WRITE setTitle )
    Q_PROPERTY( QString creator         READ creator            WRITE setCreator )
    Q_PROPERTY( uint createdon          READ createdOn          WRITE setCreatedOn )

    friend class ::DatabaseCommand_ListeningRoomInfo;

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

    source_ptr author() const       { return m_source; }
    QString currentrevision() const { return m_currentrevision; }
    QString title() const           { return m_title; }
    QString creator() const         { return m_creator; }
    QString guid() const            { return m_guid; }
    uint lastmodified() const       { return m_lastmodified; }
    uint createdOn() const          { return m_createdOn; }

    const QQueue< lrentry_ptr >& entries() { return m_entries; }

    void addEntry( const Tomahawk::query_ptr& query,
                   const QString& oldrev );
    void addEntries( const QList< Tomahawk::query_ptr >& queries,
                     const QString& oldrev );
    void insertEntries( const QList< Tomahawk::query_ptr >& queries,
                        const int position,
                        const QString& oldrev );

    // <IGNORE hack="true">
    // these need to exist and be public for the json serialization stuff
    // you SHOULD NOT call them.  They are used for an alternate CTOR method from json.
    // maybe friend QObjectHelper and make them private?
    explicit ListeningRoom( const source_ptr& author );
    void setCurrentrevision( const QString& s ) { m_currentrevision = s; }
    void setCreator( const QString& s )         { m_creator = s; }
    void setGuid( const QString& s )            { m_guid = s; }
    void setCreatedOn( uint createdOn )         { m_createdOn = createdOn; }
    void setTitle(const QString& title );
    // </IGNORE>


    QList< lrentry_ptr > entriesFromQueries( const QList< Tomahawk::query_ptr >& queries, bool clearFirst = false );

    Tomahawk::playlistinterface_ptr playlistInterface();

signals:
    void created();
    void revisionLoaded( Tomahawk::ListeningRoomRevision );

    void changed();
    void renamed( const QString& newTitle, const QString& oldTitle );

    void aboutToBeDeleted( const Tomahawk::listeningroom_ptr& lr );
    void deleted( const Tomahawk::listeningroom_ptr& lr );

public slots:
    // want to update the playlist from the model?
    // generate a newrev using uuid() and call this:
    void createNewRevision( const QString& newrev, const QString& oldrev, const QList< lrentry_ptr >& entries );

    // Want to update some metadata of a plentry_ptr? this gets you a new revision too.
    // entries should be <= entries(), with changed metadata.
    void updateEntries( const QString& newrev, const QString& oldrev, const QList< lrentry_ptr >& entries );

    void reportCreated( const Tomahawk::listeningroom_ptr& self );
    void reportDeleted( const Tomahawk::listeningroom_ptr& self );

    void resolve();

    void setWeakSelf( QWeakPointer< ListeningRoom > self ) { m_weakSelf = self; }
private slots:
    void onResultsFound( const QList< Tomahawk::result_ptr >& results );
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

    void checkRevisionQueue();
    QWeakPointer< ListeningRoom > m_weakSelf;

    source_ptr m_source;
    QString m_currentrevision;
    QString m_guid;
    QString m_title;
    QString m_creator;
    uint m_lastmodified;
    uint m_createdOn;
    QQueue< lrentry_ptr > m_entries;
    QList< lrentry_ptr > m_initEntries;

    Tomahawk::playlistinterface_ptr m_playlistInterface;
};

} //namespace Tomahawk

Q_DECLARE_METATYPE( QSharedPointer< Tomahawk::ListeningRoom > )
Q_DECLARE_METATYPE( QList< QSharedPointer< Tomahawk::ListeningRoomEntry > > )

#endif // LISTENINGROOM_H
