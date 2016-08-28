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
#include <ao/ao.h>
#include "eas.h"
#include "filewrapper.h"

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

    void playFile(const QString fileName);
    void startPlayback(const QString fileName);
    void stopPlayback();

private:
    void initALSA();
    void initEAS();
    void initAudio();
    void writeMIDIData(SequencerEvent *ev);

    void preparePlayback();
    bool playbackCompleted();
    void closePlayback();
    int getPlaybackLocation();

public slots:
    void subscription(MidiPort* port, Subscription* subs);
    void sequencerEvent( SequencerEvent* ev );
    void run();

signals:
    void finished();
    void playbackStopped();
    void playbackTime(int time);

private:
    bool m_Stopped;
    bool m_isPlaying;

    QReadWriteLock m_mutex;
    QStringList m_files;

    /* Drumstick ALSA*/
    MidiClient* m_Client;
    MidiPort* m_Port;
    MidiCodec* m_codec;

    /* SONiVOX EAS */
    int m_sampleRate, m_bufferSize, m_channels;
    EAS_DATA_HANDLE m_easData;
    EAS_HANDLE m_streamHandle;
    EAS_HANDLE m_fileHandle;
    FileWrapper *m_currentFile;

    /* libao */
    ao_device *m_aoDevice;
};

#endif /*SYNTHRENDERER_H_*/
