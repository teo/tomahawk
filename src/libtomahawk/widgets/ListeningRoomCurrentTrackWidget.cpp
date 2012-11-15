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

#include "ListeningRoomCurrentTrackWidget.h"

#include "ElidedLabel.h"
#include "ListeningRoomModel.h"
#include "PlayableItem.h"
#include "Album.h"
#include "Artist.h"
#include "utils/TomahawkUtilsGui.h"
#include "Source.h"
#include "Result.h"

#include <QtGui/QLabel>
#include <QtGui/QBoxLayout>


ListeningRoomCurrentTrackWidget::ListeningRoomCurrentTrackWidget( QWidget* parent )
    : QWidget( parent )
{
    QHBoxLayout* mainLayout = new QHBoxLayout;
    setLayout( mainLayout );

    m_albumArtLabel = new QLabel( this );
    mainLayout->addWidget( m_albumArtLabel );
    m_albumArtLabel->setFixedSize( 80, 80 );

    QVBoxLayout* vLayout = new QVBoxLayout;
    mainLayout->addLayout( vLayout );

    m_trackLabel = new ElidedLabel( this );
    m_trackLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    m_artistLabel = new ElidedLabel( this );
    m_artistLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    m_lovesLabel = new QLabel( this );
    m_lovesLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    QFont font = m_trackLabel->font();
    font.setBold( true );
    font.setPointSize( TomahawkUtils::defaultFontSize() + 2 );
    m_trackLabel->setFont( font );

    font = m_artistLabel->font();
    font.setBold( true );
    font.setPointSize( TomahawkUtils::defaultFontSize() - 1 );
    m_artistLabel->setFont( font );

    vLayout->addWidget( m_trackLabel );
    vLayout->addWidget( m_artistLabel );
    vLayout->addWidget( m_lovesLabel );

    m_trackLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    m_artistLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    m_lovesLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

    m_durationLabel = new QLabel( this );
    m_durationLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    m_avatarLabel = new QLabel( this );
    font = m_durationLabel->font();
    font.setBold( true );
    m_durationLabel->setFont( font );

    mainLayout->addWidget( m_durationLabel );
    mainLayout->addWidget( m_avatarLabel );

    TomahawkUtils::unmarginLayout( mainLayout );
    mainLayout->setMargin( 8 );
    mainLayout->setSpacing( 8 );

    m_albumArtPlaceholder.load( RESPATH "images/no-album-art-placeholder.png" );
    m_albumArtPlaceholder = m_albumArtPlaceholder.scaled( QSize( 80, 80 ), Qt::KeepAspectRatio, Qt::SmoothTransformation );
}


void
ListeningRoomCurrentTrackWidget::setItem( const QPersistentModelIndex& idx )
{
    int row = idx.row();
    const ListeningRoomModel* model = qobject_cast< const ListeningRoomModel* >( idx.model() );


    PlayableItem* item = model->itemFromIndex( idx );

    const Tomahawk::query_ptr q = item->query()->displayQuery();

    m_trackLabel->setText( q->track() );
    m_artistLabel->setText( q->artist() );

    if ( item->query()->coverLoaded() )
        m_albumArtLabel->setPixmap( item->query()->cover( QSize( 80, 80 ) ) );
    else
        m_albumArtLabel->setPixmap( m_albumArtPlaceholder );

    QString lovesString = item->query()->socialActionDescription( "Love", Tomahawk::Query::Detailed );
    m_lovesLabel->setText( lovesString );

    unsigned int duration = q->duration();

    const QPixmap sourceIcon = q->results().first()->sourceIcon( Tomahawk::Result::DropShadow, QSize( 32, 32 ) );
    m_avatarLabel->setPixmap( sourceIcon );

    if ( duration > 0 )
    {
        m_durationLabel->setText( TomahawkUtils::timeToString( duration ) );
    }
}
