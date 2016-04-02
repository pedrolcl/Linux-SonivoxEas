/*
    Sonivox EAS Synthesizer for Qt applications
    Copyright (C) 2016, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHCONTROLLER_H
#define SYNTHCONTROLLER_H

#include <QObject>
#include <QThread>
#include "synthrenderer.h"

class SynthController : public QObject
{
    Q_OBJECT
public:
    explicit SynthController(QObject *parent = 0);
    virtual ~SynthController();
    SynthRenderer *renderer() const;

    void start();
    void stop();

signals:
    void finished();

private:
    QThread m_renderingThread;
    SynthRenderer *m_renderer;
};

#endif // SYNTHCONTROLLER_H
