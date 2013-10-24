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

#include "PartyWidget.h"

#include "HeaderLabel.h"
#include "PartyModel.h"
#include "playlist/TrackView.h"
#include "playlist/PlaylistLargeItemDelegate.h" //TODO: make nice delegate for parties!
#include "playlist/PlayableProxyModel.h"
#include "playlist/PlayableItem.h"
#include "Source.h"
#include "utils/TomahawkUtilsGui.h"
#include "Typedefs.h"
#include "utils/Closure.h"
#include "database/Database.h"
#include "database/DatabaseImpl.h"
#include "LatchManager.h"
#include "widgets/PartyCurrentTrackWidget.h"
#include "Party.h"

#include <QtCore/QTimeLine>
#include <QtGui/QLabel>
#include <QtGui/QBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QPushButton>

#include "SourceList.h"

PartyWidget::PartyWidget( QWidget* parent )
    : QWidget( parent )
    , m_model( 0 )
    , m_drawerShown( false )
    , m_downArrow( QIcon( RESPATH "images/arrow-down-double.png" ) )
    , m_upArrow( QIcon( RESPATH "images/arrow-up-double.png" ) )
    , m_currentRow( -1 )
{
    QBoxLayout* mainLayout = new QVBoxLayout;
    setLayout( mainLayout );

    m_header = new PartyHeader( this );
    m_historyDrawer = new QWidget( this );
    m_body = new QWidget( this );

    mainLayout->addWidget( m_header );
    mainLayout->addWidget( m_historyDrawer );
    mainLayout->addWidget( m_body );

    m_historyDrawer->setMaximumHeight( 0 );

    TomahawkUtils::unmarginLayout( mainLayout );

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
    m_historyView->setReadOnly( true );
    connect ( m_historyView, SIGNAL( itemActivated( QModelIndex ) ),
              this, SLOT( onHistoryItemActivated( QModelIndex ) ) );

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

    m_currentTrackWidget = new PartyCurrentTrackWidget( m_body );
    bodyLayout->addWidget( m_currentTrackWidget );
    m_currentTrackWidget->setFixedHeight( 80 + 2*8 );

    HeaderLabel* upcomingTracksLabel = new HeaderLabel( m_body );
    upcomingTracksLabel->setText( tr( "Upcoming tracks" ) );
    upcomingTracksLabel->setStyleSheet( "border-bottom-left-radius: 16px;");
    bodyLayout->addWidget( upcomingTracksLabel );

    m_view = new TrackView( m_body );
    bodyLayout->addWidget( m_view );

    m_pixmap = QIcon( RESPATH "images/party.png" ).pixmap( 64 );

    PlaylistLargeItemDelegate* delegate =
            new PlaylistLargeItemDelegate( PlaylistLargeItemDelegate::LovedTracks,
                                           m_view,
                                           m_view->proxyModel() );
    connect( delegate, SIGNAL( updateIndex( QModelIndex ) ),
             m_view,   SLOT( update( QModelIndex ) ) );
    m_view->setItemDelegate( delegate );
    m_view->proxyModel()->setStyle( PlayableProxyModel::Large );
    connect( m_view, SIGNAL( itemActivated( QModelIndex ) ),
             this, SLOT( onMainViewItemActivated( QModelIndex ) ) );

    connect( m_header, SIGNAL( joinLeaveButtonClicked( PartyHeader::ButtonState ) ),
             this, SLOT( onJoinLeaveButtonClicked( PartyHeader::ButtonState ) ) );
}


QString
PartyWidget::title() const
{
     return m_view->title();
}


QString
PartyWidget::description() const
{
    //TODO: implement!
    return m_view->description();
}


QPixmap
PartyWidget::pixmap() const
{
    return m_pixmap;
}


void
PartyWidget::setModel( PlaylistModel* model )
{
    Q_ASSERT( !m_model ); //TODO: does it ever happen that m_model is already assigned?
    if ( m_model )
        delete m_model;

    m_model = model;

    m_historyView->setPlayableModel( model );
    m_historyView->setSortingEnabled( false );

    m_view->setPlayableModel( model );
    m_view->setSortingEnabled( false );
    if ( m_model->party()->author()->isLocal() )
    {
        m_view->setManualProgression( true );
    }
    else
    {
        m_view->setReadOnly( true );
    }

    m_header->setCaption( model->title() );
    m_header->setDescription( model->description() );
    m_header->setPixmap( m_pixmap );
    m_view->setEmptyTip( tr( "This party is currently empty.\n"
                             "Add some tracks to it and enjoy the music with your friends!" ) );

    connect( m_model, SIGNAL( listenersChanged() ),
             this, SLOT( onListenersChanged() ) );
    onListenersChanged();

    connect( m_model, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
             this, SLOT( onDataChanged( QModelIndex, QModelIndex ) ) );
}


void
PartyWidget::resizeEvent( QResizeEvent* e )
{
    QWidget::resizeEvent( e );
    if ( m_drawerShown )
        m_historyDrawer->setFixedHeight( height() * 0.3 );
}


void
PartyWidget::toggleHistoryDrawer()
{
    m_timeline->setEasingCurve( QEasingCurve::OutBack );
    m_timeline->setFrameRange( 0, height() * 0.3 );

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
PartyWidget::onAnimationStep( int step )
{
    m_drawerH = step;
    m_historyDrawer->setFixedHeight( m_drawerH );
}

void
PartyWidget::onAnimationFinished()
{
    if ( m_drawerShown )
        m_historyDrawer->setFixedHeight( m_drawerH );
    else
        m_historyDrawer->setFixedHeight( 0 );
    m_drawerH = -1;
}


void
PartyWidget::onListenersChanged()
{
    if ( m_model && !m_model->party().isNull() )
    {
        Tomahawk::party_ptr lr = m_model->party();

        m_header->setListeners( lr->listenerIds() );
        m_header->setDescription( m_model->description() );

        // If I'm the DJ
        if ( lr->author() == SourceList::instance()->getLocal() )
        {
            m_header->setButtonState( PartyHeader::Disband );
        }
        // If I'm one of the listeners
        else
        {
            // I need to ask the DatabaseImpl directly because Source::userName() answers just
            // "My Collection" instead of the dbid if the source is local.
            // This is probably a FIXME.
            if ( lr->listenerIds().contains( Tomahawk::Database::instance()->impl()->dbid() ) )
            {
                m_header->setButtonState( PartyHeader::Leave );
            }
            else
            {
                m_header->setButtonState( PartyHeader::Join );
            }
        }
    }
}


void
PartyWidget::onJoinLeaveButtonClicked( PartyHeader::ButtonState state )
{
    Tomahawk::LatchManager* lman = Tomahawk::LatchManager::instance();
    Tomahawk::source_ptr lrSource = m_model->party()->author();

    switch ( state )
    {
    case PartyHeader::Join:
        if ( lman->isLatched( lrSource ) )
            lman->catchUpRequest();
        else
            lman->latchRequest( lrSource );

        lman->latchModeChangeRequest( lrSource, true /*we always latch on realtime if we are a LR*/ );

        break;
    case PartyHeader::Leave:
        lman->unlatchRequest( lrSource );
        break;
    case PartyHeader::Disband:
        qDebug() << "Doing delete of party:" << m_model->party()->title();
        Tomahawk::Party::remove( m_model->party() );
    }
}


void
PartyWidget::onDataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    Q_UNUSED( topLeft );
    Q_UNUSED( bottomRight );

    if ( m_model->currentItem().row() != m_currentRow )
    {
        m_currentRow = ( m_model->currentItem() == QModelIndex() ) ? -1 : m_model->currentItem().row();
        m_view->proxyModel()->setFilterCutoff( PlayableProxyModel::ShowAfter, m_currentRow );
        m_historyView->proxyModel()->setFilterCutoff( PlayableProxyModel::ShowBefore, m_currentRow );
        if ( m_currentRow > -1 )
            m_currentTrackWidget->setItem( m_model->currentItem() );
    }
}


void
PartyWidget::onHistoryItemActivated( const QModelIndex& idx )
{
    Q_ASSERT( !m_model->party().isNull() );
    Q_ASSERT( !m_model->party()->author().isNull() );

    if ( m_model->party()->author()->isLocal() )
    {
        PlayableItem* item = m_model->itemFromIndex( m_historyView->proxyModel()->mapToSource( idx ) );
        if ( !item->lrentry().isNull() )
        {
            QList< Tomahawk::lrentry_ptr > entries;
            const Tomahawk::lrentry_ptr& lre = item->lrentry();
            entries.append( lre );
            m_model->insertEntriesFromView( entries, m_currentRow + 1 );
            playlistInterface()->nextResult();
        }
    }
    else
    {
        Tomahawk::LatchManager* lman = Tomahawk::LatchManager::instance();
        if ( !lman->isLatched( m_model->party()->author() ) )
        {
            onJoinLeaveButtonClicked( PartyHeader::Join );
        }
    }
}

void
PartyWidget::onMainViewItemActivated( const QModelIndex& idx )
{
    Q_ASSERT( !m_model->party().isNull() );
    Q_ASSERT( !m_model->party()->author().isNull() );

    if ( m_model->party()->author()->isLocal() )
    {
        PlayableItem* item = m_model->itemFromIndex( m_view->proxyModel()->mapToSource( idx ) );
        if ( !item->lrentry().isNull() )
        {
            QList< Tomahawk::lrentry_ptr > entries;
            const Tomahawk::lrentry_ptr& lre = item->lrentry();
            entries.append( lre );

            m_model->removeIndex( m_view->proxyModel()->mapToSource( idx ) );
            m_model->insertEntriesFromView( entries, m_currentRow + 1 );
            m_view->startPlayingFromStart();
        }
    }
    else
    {
        Tomahawk::LatchManager* lman = Tomahawk::LatchManager::instance();
        if ( !lman->isLatched( m_model->party()->author() ) )
        {
            onJoinLeaveButtonClicked( PartyHeader::Join );
        }
    }
}


Tomahawk::playlistinterface_ptr
PartyWidget::playlistInterface() const
{
    if ( !m_model )
    {
        return Tomahawk::playlistinterface_ptr();
    }
    else
    {
        return m_view->proxyModel()->playlistInterface();
    }
}


