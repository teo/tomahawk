/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2013, Christian Muehlhaeuser <muesli@tomahawk-player.org>
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

#ifndef MODEHEADER_H
#define MODEHEADER_H

#include <QWidget>

#include "DllMacro.h"

class QRadioButton;

class DLLEXPORT ModeHeader : public QWidget
{
    Q_OBJECT

public:
    ModeHeader( QWidget* parent );
    ~ModeHeader();

protected:
    void changeEvent( QEvent* e );

signals:
    void detailedClicked();
    void flatClicked();
    void gridClicked();

private:
    QWidget* m_parent;

    QRadioButton* m_radioCloud;
    QRadioButton* m_radioDetailed;
    QRadioButton* m_radioNormal;

};

#endif
