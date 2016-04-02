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

#ifndef SYNTHRENDERER_H_
#define SYNTHRENDERER_H_

#include <QObject>
#include <QReadWriteLock>
#include <drumstick.h>
#include <pulse/simple.h>
#include "eas.h"

using namespace drumstick;

class SynthRenderer : public QObject
{
    Q_OBJECT

public:
    explicit SynthRenderer(QObject *parent = 0);
    virtual ~SynthRenderer();

    void subscribe(const QString& portName);
    void stop();
    bool stopped();

    void initReverb(int reverb_type);
    void initChorus(int chorus_type);
    void setReverbWet(int amount);
    void setChorusLevel(int amount);

private:
    void initALSA();
    void initEAS();
    void initPulse();
    void writeMIDIData(SequencerEvent *ev);

public slots:
    void subscription(MidiPort* port, Subscription* subs);
    void sequencerEvent( SequencerEvent* ev );
    void run();

signals:
    void finished();

private:
    bool m_Stopped;
    QReadWriteLock m_mutex;

    /* Drumstick ALSA*/
    MidiClient* m_Client;
    MidiPort* m_Port;
    MidiCodec* m_codec;

    /* SONiVOX EAS */
    int m_sampleRate, m_bufferSize, m_channels;
    EAS_DATA_HANDLE m_easData;
    EAS_HANDLE m_easHandle;

    /* pulseaudio */
    pa_simple *m_handle;
};

#endif /*SYNTHRENDERER_H_*/
