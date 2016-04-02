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

#include <QObject>
#include <QString>
#include <QCoreApplication>
#include <QTextStream>
#include <QtDebug>
#include <QReadLocker>
#include <QWriteLocker>
#include <eas_reverb.h>
#include <eas_chorus.h>
#include <pulse/simple.h>
#include "synthrenderer.h"

SynthRenderer::SynthRenderer(QObject *parent) : QObject(parent)
{
    initALSA();
    initEAS();
    initPulse();
}

void
SynthRenderer::initALSA()
{
    m_Client = new MidiClient(this);
    m_Client->open();
    m_Client->setClientName("Sonivox EAS");
    connect( m_Client, SIGNAL(eventReceived(SequencerEvent*)),
                       SLOT(sequencerEvent(SequencerEvent*)),
                       Qt::DirectConnection );
    m_Port = new MidiPort(this);
    m_Port->attach( m_Client );
    m_Port->setPortName("Synthesizer input");
    m_Port->setCapability( SND_SEQ_PORT_CAP_WRITE |
                           SND_SEQ_PORT_CAP_SUBS_WRITE );
    m_Port->setPortType( SND_SEQ_PORT_TYPE_APPLICATION |
                         SND_SEQ_PORT_TYPE_MIDI_GENERIC );
    connect( m_Port, SIGNAL(subscribed(MidiPort*,Subscription*)),
                     SLOT(subscription(MidiPort*,Subscription*)));
    m_Port->subscribeFromAnnounce();
    m_codec = new MidiCodec(256);
    m_codec->enableRunningStatus(false);
    qDebug() << Q_FUNC_INFO;
}

void
SynthRenderer::initEAS()
{
    /* SONiVOX EAS initialization */
    EAS_RESULT eas_res;
    EAS_DATA_HANDLE dataHandle;
    EAS_HANDLE handle;

    const S_EAS_LIB_CONFIG *easConfig = EAS_Config();
    if (easConfig == 0) {
        qDebug() << "EAS_Config returned null";
        return;
    }

    eas_res = EAS_Init(&dataHandle);
    if (eas_res != EAS_SUCCESS) {
      qDebug() << "EAS_Init error: " << eas_res;
      return;
    }

    eas_res = EAS_OpenMIDIStream(dataHandle, &handle, NULL);
    if (eas_res != EAS_SUCCESS) {
      qDebug() << "EAS_OpenMIDIStream error: " << eas_res;
      EAS_Shutdown(dataHandle);
      return;
    }

    m_easData = dataHandle;
    m_easHandle = handle;
    assert(m_easHandle != 0);
    m_sampleRate = easConfig->sampleRate;
    m_bufferSize = easConfig->mixBufferSize;
    m_channels = easConfig->numChannels;
    qDebug() << Q_FUNC_INFO << "EAS bufferSize=" << m_bufferSize << " sampleRate=" << m_sampleRate << " channels=" << m_channels;
}

void
SynthRenderer::initPulse()
{
    pa_sample_spec samplespec;
    pa_buffer_attr bufattr;
    int period_bytes;
    char *server = 0;
    char *device = 0;
    int err;

    samplespec.format = PA_SAMPLE_S16LE;
    samplespec.channels = m_channels;
    samplespec.rate = m_sampleRate;

    period_bytes = m_bufferSize * sizeof (EAS_PCM) * m_channels;
    bufattr.maxlength = (int32_t)-1;
    bufattr.tlength = period_bytes;
    bufattr.minreq = (int32_t)-1;
    bufattr.prebuf = (int32_t)-1; /* Just initialize to same value as tlength */
    bufattr.fragsize = (int32_t)-1; /* Not used */

    m_handle = pa_simple_new (server, "SonivoxEAS", PA_STREAM_PLAYBACK,
                    device, "Synthesizer", &samplespec,
                    NULL, /* pa_channel_map */
                    &bufattr,
                    &err);

    if (!m_handle)
    {
      qDebug() << "Failed to create PulseAudio connection";
    }
    qDebug() << Q_FUNC_INFO;
}

SynthRenderer::~SynthRenderer()
{
    m_Port->detach();
    delete m_Port;
    m_Client->close();
    delete m_Client;
    delete m_codec;

    qDebug() << Q_FUNC_INFO;

    EAS_RESULT eas_res;
    if (m_easData != 0 && m_easHandle != 0) {
      eas_res = EAS_CloseMIDIStream(m_easData, m_easHandle);
      if (eas_res != EAS_SUCCESS) {
          qDebug() << "EAS_CloseMIDIStream error: " << eas_res;
      }
      eas_res = EAS_Shutdown(m_easData);
      if (eas_res != EAS_SUCCESS) {
          qDebug() << "EAS_Shutdown error: " << eas_res;
      }
    }

    pa_simple_free(m_handle);
}

bool
SynthRenderer::stopped()
{
	QReadLocker locker(&m_mutex);
    return m_Stopped;
}

void
SynthRenderer::stop()
{
	QWriteLocker locker(&m_mutex);
    qDebug() << Q_FUNC_INFO;
    m_Stopped = true;
}

void
SynthRenderer::subscription(MidiPort*, Subscription* subs)
{
    qDebug() << "Subscription made from "
             << subs->getSender()->client << ":"
             << subs->getSender()->port;
}

void
SynthRenderer::subscribe(const QString& portName)
{
    try {
        qDebug() << "Trying to subscribe " << portName.toLocal8Bit().data();
        m_Port->subscribeFrom(portName);
    } catch (const SequencerError& err) {
        qDebug() << "SequencerError exception. Error code: " << err.code()
             << " (" << err.qstrError() << ")";
        qDebug() << "Location: " << err.location();
        throw err;
    }
}

void
SynthRenderer::run()
{
    int err;
    unsigned char data[1024];
    qDebug() << Q_FUNC_INFO << "started";
    try {
        m_Client->setRealTimeInput(false);
        m_Client->startSequencerInput();
        m_Stopped = false;
        while (!stopped()) {
            EAS_RESULT eas_res;
            EAS_I32 numGen = 0;
            size_t bytes = 0;
            if (m_easData != 0)
            {
                EAS_PCM *buffer = (EAS_PCM *) data;
                eas_res = EAS_Render(m_easData, buffer, m_bufferSize, &numGen);
                if (eas_res != EAS_SUCCESS) {
                    qDebug() << "EAS_Render error:" << eas_res;
                }
                bytes += (size_t) numGen * sizeof(EAS_PCM) * m_channels;
                // hand over to pulseaudio the rendered buffer
                if (pa_simple_write (m_handle, data, bytes, &err) < 0)
                {
                    qDebug() << "Error writing to PulseAudio connection.";
                }
            }
        }
        m_Client->stopSequencerInput();
    } catch (const SequencerError& err) {
        qDebug() << "SequencerError exception. Error code: " << err.code()
             << " (" << err.qstrError() << ")";
        qDebug() << "Location: " << err.location();
        throw err;
    }
    qDebug() << Q_FUNC_INFO << "ended";
    emit finished();
}

void
SynthRenderer::sequencerEvent(SequencerEvent *ev)
{
    switch (ev->getSequencerType()) {
    case SND_SEQ_EVENT_CHANPRESS:
    case SND_SEQ_EVENT_NOTEOFF:
    case SND_SEQ_EVENT_NOTEON:
    case SND_SEQ_EVENT_CONTROLLER:
    case SND_SEQ_EVENT_KEYPRESS:
    case SND_SEQ_EVENT_PGMCHANGE:
    case SND_SEQ_EVENT_PITCHBEND:
        writeMIDIData(ev);
        break;
    }
    delete ev;
}

void
SynthRenderer::writeMIDIData(SequencerEvent *ev)
{
    EAS_RESULT eas_res = EAS_ERROR_ALREADY_STOPPED;
    EAS_I32 count;
    EAS_U8 buffer[256];

    if (m_easData != 0 && m_easHandle != 0)
    {
        count = m_codec->decode((unsigned char *)&buffer, sizeof(buffer), ev->getHandle());
        if (count > 0) {
            eas_res = EAS_WriteMIDIStream(m_easData, m_easHandle, buffer, count);
            if (eas_res != EAS_SUCCESS) {
                qDebug() << "EAS_WriteMIDIStream error: " << eas_res;
            }
        }
    }
}

void
SynthRenderer::initReverb(int reverb_type)
{
    EAS_RESULT eas_res;
    EAS_BOOL sw = EAS_TRUE;
    if ( reverb_type >= EAS_PARAM_REVERB_LARGE_HALL && reverb_type <= EAS_PARAM_REVERB_ROOM ) {
        sw = EAS_FALSE;
        eas_res = EAS_SetParameter(m_easData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_PRESET, (EAS_I32) reverb_type);
        if (eas_res != EAS_SUCCESS) {
            qDebug() << "EAS_SetParameter error:" << eas_res;
        }
    }
    eas_res = EAS_SetParameter(m_easData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_BYPASS, sw);
    if (eas_res != EAS_SUCCESS) {
        qDebug() << "EAS_SetParameter error: " << eas_res;
    }
}

void
SynthRenderer::initChorus(int chorus_type)
{
    EAS_RESULT eas_res;
    EAS_BOOL sw = EAS_TRUE;
    if (chorus_type >= EAS_PARAM_CHORUS_PRESET1 && chorus_type <= EAS_PARAM_CHORUS_PRESET4 ) {
        sw = EAS_FALSE;
        eas_res = EAS_SetParameter(m_easData, EAS_MODULE_CHORUS, EAS_PARAM_CHORUS_PRESET, (EAS_I32) chorus_type);
        if (eas_res != EAS_SUCCESS) {
            qDebug() << "EAS_SetParameter error:" << eas_res;
        }
    }
    eas_res = EAS_SetParameter(m_easData, EAS_MODULE_CHORUS, EAS_PARAM_CHORUS_BYPASS, sw);
    if (eas_res != EAS_SUCCESS) {
        qDebug() << "EAS_SetParameter error:" << eas_res;
    }
}

void
SynthRenderer::setReverbWet(int amount)
{
    EAS_RESULT eas_res = EAS_SetParameter(m_easData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_WET, (EAS_I32) amount);
    if (eas_res != EAS_SUCCESS) {
        qDebug() << "EAS_SetParameter error:" << eas_res;
    }
}

void
SynthRenderer::setChorusLevel(int amount)
{
    EAS_RESULT eas_res = EAS_SetParameter(m_easData, EAS_MODULE_CHORUS, EAS_PARAM_CHORUS_LEVEL, (EAS_I32) amount);
    if (eas_res != EAS_SUCCESS) {
        qDebug() << "EAS_SetParameter error:" << eas_res;
    }
}
