/*
    Sonivox EAS Synthesizer for Qt applications
    Copyright (C) 2016-2023, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef SYNTHRENDERER_H_
#define SYNTHRENDERER_H_

#include <QObject>
#include <QReadWriteLock>
#include <drumstick/alsaclient.h>
#include <drumstick/alsaport.h>
#include <drumstick/alsaevent.h>
#include <pulse/simple.h>
#include "eas.h"
#include "filewrapper.h"

class SynthRenderer : public QObject
{
    Q_OBJECT

public:
    explicit SynthRenderer(int bufTime, QObject *parent = 0);
    virtual ~SynthRenderer();

    void subscribe(const QString& portName);
    void stop();
    bool stopped();

    void initReverb(int reverb_type);
    void initChorus(int chorus_type);
    void setReverbWet(int amount);
    void setChorusLevel(int amount);
    void initSoundfont(const QString& dlsFile);

    void playFile(const QString fileName);
    void startPlayback(const QString fileName);
    void stopPlayback();

    void uninitALSA();

    void uninitPulse();

private:
    void initALSA();
    void initEAS();
    void uninitEAS();
    void initPulse();
    void writeMIDIData(drumstick::ALSA::SequencerEvent *ev);

    void preparePlayback();
    bool playbackCompleted();
    void closePlayback();
    int getPlaybackLocation();

public slots:
    void subscription(drumstick::ALSA::MidiPort* port, drumstick::ALSA::Subscription* subs);
    void sequencerEvent( drumstick::ALSA::SequencerEvent* ev );
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
    drumstick::ALSA::MidiClient* m_Client;
    drumstick::ALSA::MidiPort* m_Port;
    drumstick::ALSA::MidiCodec* m_codec;

    /* SONiVOX EAS */
    int m_sampleRate, m_bufferSize, m_channels;
    EAS_DATA_HANDLE m_easData;
    EAS_HANDLE m_streamHandle;
    EAS_HANDLE m_midiFileHandle;
    FileWrapper *m_currentFile;
    QString m_soundfont;

    /* pulseaudio */
    int m_bufferTime;
    pa_simple *m_pulseHandle;
};

#endif /*SYNTHRENDERER_H_*/
