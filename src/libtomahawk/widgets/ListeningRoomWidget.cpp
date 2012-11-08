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
#include "Typedefs.h"

#include <QtCore/QTimeLine>
#include <QtGui/QLabel>
#include <QtGui/QBoxLayout>
#include <QtGui/QListView>
#include <QtGui/QPushButton>

#include "SourceList.h"

ListeningRoomWidget::ListeningRoomWidget( QWidget* parent )
    : QWidget( parent )
    , m_model( 0 )
    , m_drawerShown( false )
    , m_downArrow( QIcon( RESPATH "images/arrow-down-double.png" ) )
    , m_upArrow( QIcon( RESPATH "images/arrow-up-double.png" ) )
{
    setLayout( new QVBoxLayout );

    m_header = new ListeningRoomHeader( this );
    m_historyDrawer = new QWidget( this );
    m_body = new QWidget( this );

    layout()->addWidget( m_header );
    layout()->addWidget( m_historyDrawer );
    layout()->addWidget( m_body );
    TomahawkUtils::unmarginLayout( layout() );

    // m_historyDrawer
    QVBoxLayout* historyLayout = new QVBoxLayout;
    m_historyDrawer->setLayout( historyLayout );
    TomahawkUtils::unmarginLayout( historyLayout );

    m_historyView = new TrackView( m_historyDrawer );
    historyLayout->addWidget( m_historyView );

    PlaylistLargeItemDelegate* historyDelegate =
            new PlaylistLargeItemDelegate( PlaylistLargeItemDelegate::LovedTracks,
                                           m_historyView,
                                           m_historyView->proxyModel() );
    connect( historyDelegate, SIGNAL( updateIndex( QModelIndex ) ),
             m_historyView,   SLOT( update( QModelIndex ) ) );
    m_historyView->setItemDelegate( historyDelegate );
    m_historyView->proxyModel()->setStyle( PlayableProxyModel::Large );
    m_historyDrawer->setFixedHeight( 0 );

    // m_body
    QVBoxLayout* bodyLayout = new QVBoxLayout;
    m_body->setLayout( bodyLayout );
    TomahawkUtils::unmarginLayout( bodyLayout );

    //TODO: replace with a "currently playing track" widget
    m_showTracksString = tr( "Show previous tracks" );
    m_hideTracksString = tr( "Hide previous tracks" );
    m_previousTracksButton = new QPushButton( m_body );
    m_previousTracksButton->setText( m_showTracksString );
    m_previousTracksButton->setIcon( m_downArrow );
    connect( m_previousTracksButton, SIGNAL( clicked() ),
             this, SLOT( toggleHistoryDrawer() ) );
    QFontMetrics fm = m_previousTracksButton->fontMetrics();
    int pixelsWide = qMax( fm.width( m_showTracksString ), fm.width( m_hideTracksString ) )
                     + m_previousTracksButton->iconSize().width();
    m_previousTracksButton->setFixedWidth( pixelsWide + 24 /*a bit of padding*/ );

    m_timeline = new QTimeLine( 250, this );
    m_timeline->setUpdateInterval( 5 );
    m_drawerH = -1;
    connect( m_timeline, SIGNAL( frameChanged( int ) ), SLOT( onAnimationStep( int ) ) );
    connect( m_timeline, SIGNAL( finished() ), SLOT( onAnimationFinished() ) );


    QHBoxLayout* previousTracksLayout = new QHBoxLayout;
    bodyLayout->addLayout( previousTracksLayout );
    previousTracksLayout->addStretch();
    previousTracksLayout->addWidget( m_previousTracksButton );

    QLabel *placeholder = new QLabel( "Placeholder for\ncurrently playing track.", m_body );
    bodyLayout->addWidget( placeholder );
    placeholder->setFixedHeight( 80 );
    placeholder->setAlignment( Qt::AlignCenter );

    HeaderLabel* upcomingTracksLabel = new HeaderLabel( m_body );
    upcomingTracksLabel->setText( tr( "Upcoming tracks" ) );
    upcomingTracksLabel->setStyleSheet( "border-bottom-left-radius: 16px;");
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

    //TODO: testing LR header, remove this in favor of getting dbids from RoomInfo
    QStringList dbids;
    foreach ( const Tomahawk::source_ptr& s, SourceList::instance()->sources( true ) )
    {
        dbids << s->userName();
    }
    m_header->setListeners( dbids );
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

    m_historyView->setPlayableModel( model );
    m_historyView->setSortingEnabled( false );

    m_view->setPlayableModel( model );
    m_view->setSortingEnabled( false );

    m_header->setCaption( model->title() );
    m_header->setDescription( model->description() );
    m_header->setPixmap( m_pixmap );
    //TODO: hook something up to show listeners in header
    m_view->setEmptyTip( tr( "This room is currently empty.\n"
                             "Add some tracks to it and enjoy the music with your friends!" ) );

}

void
ListeningRoomWidget::toggleHistoryDrawer()
{
    m_timeline->setEasingCurve( QEasingCurve::OutBack );
    m_timeline->setFrameRange( 0, m_historyDrawer->sizeHint().height() );

    if ( !m_drawerShown )
    {
        m_previousTracksButton->setText( m_hideTracksString );
        m_previousTracksButton->setIcon( m_upArrow );

        m_timeline->setDirection( QTimeLine::Forward );
        m_timeline->start();

        m_drawerShown = true;
    }
    else
    {
        m_previousTracksButton->setText( m_showTracksString );
        m_previousTracksButton->setIcon( m_downArrow );

        m_timeline->setDirection( QTimeLine::Backward );
        m_timeline->start();

        m_drawerShown = false;
    }
}

void
ListeningRoomWidget::onAnimationStep( int step )
{
    m_drawerH = step;
    m_historyDrawer->setFixedHeight( m_drawerH );
}

void
ListeningRoomWidget::onAnimationFinished()
{
    if ( m_drawerShown )
        m_historyDrawer->setFixedHeight( m_drawerH );
    else
        m_historyDrawer->setFixedHeight( 0 );
    m_drawerH = -1;
}


Tomahawk::playlistinterface_ptr
ListeningRoomWidget::playlistInterface() const
{
    return Tomahawk::playlistinterface_ptr(); //TODO: implement
}


