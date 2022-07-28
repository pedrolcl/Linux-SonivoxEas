/*
    Sonivox EAS Synthesizer for Qt applications
    Copyright (C) 2016-2022, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDebug>
#include "synthcontroller.h"
#include "synthrenderer.h"

SynthController::SynthController(int bufTime, QObject *parent) : QObject(parent)
{
    m_renderer = new SynthRenderer(bufTime);
    m_renderer->moveToThread(&m_renderingThread);
    connect(&m_renderingThread, &QThread::started,  m_renderer, &SynthRenderer::run);
    connect(&m_renderingThread, &QThread::finished, m_renderer, &QObject::deleteLater);
    connect(m_renderer, &SynthRenderer::finished, this, &SynthController::finished);
}

SynthController::~SynthController()
{
    qDebug() << Q_FUNC_INFO;
    if (m_renderingThread.isRunning()) {
        stop();
    }
}

void
SynthController::start()
{
    qDebug() << Q_FUNC_INFO;
    m_renderingThread.start(QThread::HighPriority);
}

void
SynthController::stop()
{
    qDebug() << Q_FUNC_INFO;
    m_renderer->stop();
    m_renderingThread.quit();
    m_renderingThread.wait();
}

SynthRenderer*
SynthController::renderer() const
{
    return m_renderer;
}
