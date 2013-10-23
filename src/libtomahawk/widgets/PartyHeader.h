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

#ifndef PARTYHEADER_H
#define PARTYHEADER_H

#include "BasicHeader.h"

#include "Typedefs.h"

#include <QtCore/QHash>

class PartyWidget;
class QBoxLayout;
class QPushButton;

class PartyHeader : public BasicHeader
{
    Q_OBJECT
public:
    enum ButtonState
    {
        Join,
        Leave,
        Disband
    };

    explicit PartyHeader( PartyWidget* parent );
    virtual ~PartyHeader();

public slots:
    void setListeners( const QStringList& listenerDbids );
    void setButtonState( ButtonState state );

signals:
    void joinLeaveButtonClicked( PartyHeader::ButtonState );

private  slots:
    void onJoinLeaveButtonClicked();

private:
    void fillListeners();

    ButtonState m_buttonState;
    QMap< ButtonState, QIcon >   m_buttonIcons;
    QMap< ButtonState, QString > m_buttonStrings;

    QList< Tomahawk::source_ptr > m_listeners;
    QPixmap m_defaultAvatar;

    QWidget* m_listenersWidget;
    QMap< QString, QLabel* > m_avatarLabels;
    int m_unnamedListeners;
    QBoxLayout* m_avatarsLayout;
    QLabel* m_unnamedListenersLabel;

    QPushButton* m_joinLeaveButton;
};

#endif // PARTYHEADER_H
