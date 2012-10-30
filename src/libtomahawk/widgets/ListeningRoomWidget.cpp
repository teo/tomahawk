/*
 *  Copyright 2012, Teo Mrnjavac <teo@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "ListeningRoomWidget.h"

#include "HeaderLabel.h"
#include "ListeningRoomHeader.h"
#include "ListeningRoomModel.h"
#include "playlist/TrackView.h"
#include "playlist/PlaylistLargeItemDelegate.h" //TODO: make nice delegate for rooms!
#include "playlist/PlayableProxyModel.h"
#include "Source.h"
#include "utils/TomahawkUtilsGui.h"

#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QBoxLayout>
#include <QtGui/QListView>

ListeningRoomWidget::ListeningRoomWidget( QWidget* parent )
    : QWidget( parent )
    , m_model( 0 )
{
    setLayout( new QVBoxLayout );

    m_header = new ListeningRoomHeader( this );
    m_body = new QWidget( this );

    layout()->addWidget( m_header );
    layout()->addWidget( m_body );
    TomahawkUtils::unmarginLayout( layout() );

    QVBoxLayout* bodyLayout = new QVBoxLayout;
    m_body->setLayout( bodyLayout );
    TomahawkUtils::unmarginLayout( bodyLayout );

    //TODO: replace with a "currently playing track" widget
    QLabel *placeholder = new QLabel( "Placeholder for\ncurrently playing track.", m_body );
    bodyLayout->addWidget( placeholder );
    placeholder->setFixedHeight( 80 );
    placeholder->setAlignment( Qt::AlignCenter );

    HeaderLabel* upcomingTracksLabel = new HeaderLabel( m_body );
    upcomingTracksLabel->setText( "Upcoming tracks" );
    bodyLayout->addWidget( upcomingTracksLabel );

    m_view = new TrackView( m_body );
    bodyLayout->addWidget( m_view );

    m_pixmap = QIcon( RESPATH "images/listeningroom.png" ).pixmap( 64 );

    PlaylistLargeItemDelegate* delegate =
            new PlaylistLargeItemDelegate( PlaylistLargeItemDelegate::LovedTracks,
                                           m_view,
                                           m_view->proxyModel() );
    connect( delegate, SIGNAL( updateIndex( QModelIndex ) ),
             m_view,   SLOT( update( QModelIndex ) ) );
    m_view->setItemDelegate( delegate );
    m_view->proxyModel()->setStyle( PlayableProxyModel::Large );
}


QString
ListeningRoomWidget::title() const
{
     return m_view->title();
}


QString
ListeningRoomWidget::description() const
{
    //TODO: implement!
    return m_view->description();
}


QPixmap
ListeningRoomWidget::pixmap() const
{
    return m_pixmap;
}


void
ListeningRoomWidget::setModel( ListeningRoomModel* model )
{
    Q_ASSERT( !m_model ); //TODO: does it ever happen that m_model is already assigned?
    if ( m_model )
        delete m_model;

    m_model = model;
    m_view->setPlayableModel( model );
    m_view->setSortingEnabled( false );

    m_header->setCaption( model->title() );
    m_header->setDescription( model->description() );
    m_header->setPixmap( m_pixmap );
    //TODO: hook something up to show listeners in header
    m_view->setEmptyTip( tr( "This room is currently empty.\n"
                             "Add some tracks to it and enjoy the music with your friends!" ) );

}


Tomahawk::playlistinterface_ptr
ListeningRoomWidget::playlistInterface() const
{
    return Tomahawk::playlistinterface_ptr(); //TODO: implement
}


