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

#include "PartyHeader.h"

#include "PartyWidget.h"
#include "SourceList.h"
#include "utils/TomahawkUtilsGui.h"

#include <QtGui/QBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>


PartyHeader::PartyHeader( PartyWidget* parent )
    : BasicHeader( parent )
    , m_buttonState( Disband ) //just so the first setting gets applied
{
    m_listenersWidget = new QWidget( this );
    m_mainLayout->addWidget( m_listenersWidget );
    QHBoxLayout* controlsLayout = new QHBoxLayout;
    m_verticalLayout->addLayout( controlsLayout );
    m_joinLeaveButton = new QPushButton( this );
    controlsLayout->addWidget( m_joinLeaveButton );
    controlsLayout->addStretch();

    m_buttonStrings[ Join ]    = tr( "Join", "Button for a listener to join a party" );
    m_buttonStrings[ Leave ]   = tr( "Leave", "Button for a listener to leave a party" );
    m_buttonStrings[ Disband ] = tr( "Disband", "Button for a DJ to disband a party" );

    m_buttonIcons[ Join ]    = QIcon( RESPATH "images/list-add.png" );
    m_buttonIcons[ Leave ]   = QIcon( RESPATH "images/list-remove.png" );
    m_buttonIcons[ Disband ] = QIcon( RESPATH "images/delete.png" );

    setButtonState( Join );

    connect( m_joinLeaveButton, SIGNAL( clicked() ),
             this, SLOT( onJoinLeaveButtonClicked() ) );

    QBoxLayout* listenersWidgetLayout = new QVBoxLayout;
    m_listenersWidget->setLayout( listenersWidgetLayout );
    m_avatarsLayout = new QHBoxLayout; //avatars go in here!
    m_avatarsLayout->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    listenersWidgetLayout->addLayout( m_avatarsLayout );
    m_unnamedListenersLabel = new QLabel( m_listenersWidget );
    QPalette pal = palette();
    pal.setColor( QPalette::Foreground, Qt::white );
    m_unnamedListenersLabel->setPalette( pal );
    QFont font = m_unnamedListenersLabel->font();
    font.setPointSize( TomahawkUtils::defaultFontSize() + 1 );
    m_unnamedListenersLabel->setFont( font );
    m_unnamedListenersLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

    listenersWidgetLayout->addWidget( m_unnamedListenersLabel );
//TODO: unmargin maybe?
}

PartyHeader::~PartyHeader()
{
}

void
PartyHeader::setListeners( const QStringList& listenerDbids )
{
    qDeleteAll( m_avatarLabels );
    m_avatarLabels.clear();
    m_unnamedListeners = 0;
    foreach ( const QString& dbid, listenerDbids )
    {
        const Tomahawk::source_ptr& s = SourceList::instance()->get( dbid );

        if ( s.isNull() ) //means we don't know this listener
        {
            ++m_unnamedListeners;
        }
        else
        {
            QLabel* avatar = new QLabel( m_listenersWidget );
            QPixmap pxmp = s->avatar( TomahawkUtils::RoundedCorners );
            if ( pxmp.isNull() )
            {
                if ( m_defaultAvatar.isNull() )
                    m_defaultAvatar = TomahawkUtils::createRoundedImage( QPixmap( RESPATH "images/user-avatar.png" ), QSize( 32, 32 ) );
                pxmp = m_defaultAvatar;
            }
            avatar->setPixmap( pxmp );
            avatar->setToolTip( s->friendlyName() );
            m_avatarLabels.insert( dbid, avatar );
        }
    }

    fillListeners();
}

void PartyHeader::setButtonState( PartyHeader::ButtonState state )
{
    if ( state == m_buttonState )
        return;

    m_buttonState = state;

    m_joinLeaveButton->setText( m_buttonStrings[ state ] );
    m_joinLeaveButton->setIcon( m_buttonIcons[ state ] );
}


void
PartyHeader::fillListeners()
{
    if ( m_unnamedListeners )
        m_unnamedListenersLabel->setText( tr( "and %n other listener(s).", "", m_unnamedListeners ) );
    else
        m_unnamedListenersLabel->setText( "" );
    foreach ( QLabel* avatar, m_avatarLabels )
    {
        m_avatarsLayout->addWidget( avatar );
    }
}


void
PartyHeader::onJoinLeaveButtonClicked()
{
    emit joinLeaveButtonClicked( m_buttonState );
}
