/*
    Sonivox EAS Synthesizer for Qt applications
    Copyright (C) 2016-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QCoreApplication>
#include <QObject>
#include <QReadLocker>
#include <QString>
#include <QTextStream>
#include <QVersionNumber>
#include <QWriteLocker>
#include <QtDebug>

#include <drumstick/sequencererror.h>
#include <pulse/error.h>
#include <pulse/simple.h>

#include "eas_chorus.h"
#include "eas_reverb.h"
#include "filewrapper.h"
#include "synthrenderer.h"

using namespace drumstick::ALSA;

SynthRenderer::SynthRenderer(int bufTime, QObject *parent) : QObject(parent),
    m_Stopped(true),
    m_isPlaying(false),
    m_bufferTime(bufTime)
{
    initALSA();
    initEAS();
    initPulse();  
}

void
SynthRenderer::initALSA()
{
    const char* errorstr = "Fatal error from the ALSA sequencer. "
        "This usually happens when the kernel doesn't have ALSA support, "
        "or the device node (/dev/snd/seq) doesn't exists, "
        "or the kernel module (snd_seq) is not loaded. "
        "Please check your ALSA/MIDI configuration.";
    try {
        m_Client = new MidiClient(this);
        m_Client->open();
        m_Client->setClientName("Sonivox EAS");
        connect(m_Client,
                &MidiClient::eventReceived,
                this,
                &SynthRenderer::sequencerEvent,
                Qt::UniqueConnection);
        m_Port = new MidiPort(this);
        m_Port->attach( m_Client );
        m_Port->setPortName("Synthesizer input");
        m_Port->setCapability( SND_SEQ_PORT_CAP_WRITE |
                               SND_SEQ_PORT_CAP_SUBS_WRITE );
        m_Port->setPortType( SND_SEQ_PORT_TYPE_APPLICATION |
                             SND_SEQ_PORT_TYPE_MIDI_GENERIC );
        connect(m_Port,
                &MidiPort::subscribed,
                this,
                &SynthRenderer::subscription,
                Qt::UniqueConnection);
        m_Port->subscribeFromAnnounce();
        m_codec = new MidiCodec(256);
        m_codec->enableRunningStatus(false);
    } catch (const SequencerError& ex) {
        qFatal("%s Returned error was: %s\n", errorstr, ex.what());
    } catch (...) {
        qFatal("%s\n", errorstr);
    }
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
        qFatal("EAS_Config returned null\n");
        return;
    }

    eas_res = EAS_Init(&dataHandle);
    if (eas_res != EAS_SUCCESS) {
      qFatal("EAS_Init error: %ld\n", eas_res);
      return;
    }

    if (!m_soundfont.isEmpty()) {
      FileWrapper dlsFile(m_soundfont);
      if (dlsFile.ok()) {
          eas_res = EAS_LoadDLSCollection(dataHandle, nullptr, dlsFile.getLocator());
          if (eas_res != EAS_SUCCESS) {
              qWarning() << QString("EAS_LoadDLSCollection(%1) error: %2").arg(m_soundfont).arg(eas_res);
          }
      } else {
          qWarning() << "Failed to open" << m_soundfont;
      }
    }

    eas_res = EAS_OpenMIDIStream(dataHandle, &handle, NULL);
    if (eas_res != EAS_SUCCESS) {
      qFatal("EAS_OpenMIDIStream error: %ld\n", eas_res);
      EAS_Shutdown(dataHandle);
      return;
    }

    m_easData = dataHandle;
    m_streamHandle = handle;
    assert(m_streamHandle != 0);
    m_sampleRate = easConfig->sampleRate;
    m_bufferSize = easConfig->mixBufferSize;
    m_channels = easConfig->numChannels;
    m_libVersion = easConfig->libVersion;
    qDebug() << Q_FUNC_INFO << "Sonivox library:" << libVersion() << "bufferSize:" << m_bufferSize
             << "sampleRate:" << m_sampleRate << "channels:" << m_channels;
}

void
SynthRenderer::initPulse()
{
    pa_sample_spec samplespec;
    pa_buffer_attr bufattr;
    int period_bytes = 0;
    char *server = 0;
    char *device = 0;
    int err;

    samplespec.format = PA_SAMPLE_S16LE;
    samplespec.channels = m_channels;
    samplespec.rate = m_sampleRate;

    period_bytes = pa_usec_to_bytes(m_bufferTime * 1000, &samplespec);
    qDebug() << "period_bytes:" << period_bytes;
    bufattr.maxlength = (int32_t)-1;
    bufattr.tlength = period_bytes;
    bufattr.minreq = (int32_t)-1;
    bufattr.prebuf = (int32_t)-1;
    bufattr.fragsize = (int32_t)-1;

    m_pulseHandle = pa_simple_new (server, "SonivoxEAS", PA_STREAM_PLAYBACK,
                    device, "Synthesizer output", &samplespec,
                    NULL, /* pa_channel_map */
                    &bufattr, &err);
    if (err != PA_OK || !m_pulseHandle)
    {
      qFatal("Failed to create PulseAudio connection. err:%d - %s", err, pa_strerror(err));
    }
    qDebug() << Q_FUNC_INFO << "latency:" << pa_simple_get_latency(m_pulseHandle, &err);
}

void
SynthRenderer::uninitEAS()
{
    EAS_RESULT eas_res;
    if (m_easData != 0 && m_streamHandle != 0) {
      eas_res = EAS_CloseMIDIStream(m_easData, m_streamHandle);
      if (eas_res != EAS_SUCCESS) {
          qWarning() << "EAS_CloseMIDIStream error: " << eas_res;
      }
      m_streamHandle = 0;
      eas_res = EAS_Shutdown(m_easData);
      if (eas_res != EAS_SUCCESS) {
          qWarning() << "EAS_Shutdown error: " << eas_res;
      }
      m_easData = 0;
    }
}

void SynthRenderer::uninitALSA()
{
    if (m_Port != nullptr) {
        m_Port->detach();
        delete m_Port;
        m_Port = nullptr;
    }
    if (m_Client != nullptr) {
        m_Client->close();
        delete m_Client;
        delete m_codec;
        m_Client = nullptr;
        m_codec = nullptr;
    }
}

void SynthRenderer::uninitPulse()
{
    pa_simple_free(m_pulseHandle);
}

QString SynthRenderer::libVersion() const
{
    quint8 v1, v2, v3, v4;
    v1 = (m_libVersion >> 24) & 0xff;
    v2 = (m_libVersion >> 16) & 0xff;
    v3 = (m_libVersion >> 8) & 0xff;
    v4 = m_libVersion & 0xff;
    QVersionNumber vn{v1, v2, v3, v4};
    return vn.toString();
}

QStringList SynthRenderer::alsaConnections() const
{
    QStringList items;
    QListIterator<PortInfo> it(m_Client->getAvailableInputs());
    while (it.hasNext()) {
        PortInfo p = it.next();
        items << QString("%1:%2").arg(p.getClientName()).arg(p.getPort());
    }
    return items;
}

SynthRenderer::~SynthRenderer()
{
    uninitALSA();
    uninitEAS();
    uninitPulse();
    qDebug() << Q_FUNC_INFO;
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
    auto *sender = subs->getSender();
    if (sender) {
        qDebug() << "Subscription made from " << sender->client << ":" << sender->port;
    }
    delete subs;
}

void
SynthRenderer::subscribe(const QString& portName)
{
    try {
        qDebug() << "Trying to subscribe" << portName.toLocal8Bit().data();
        m_Port->subscribeFrom(portName);
    } catch (const SequencerError& err) {
        qWarning() << "SequencerError exception. Error code: " << err.code()
                   << " (" << err.qstrError() << ")";
        qWarning() << "Location: " << err.location();
        throw err;
    }
}

void SynthRenderer::unsubscribe(const QString &portName)
{
    try {
        qDebug() << "Trying to unsubscribe" << portName.toLocal8Bit().data();
        m_Port->unsubscribeFrom(portName);
    } catch (const SequencerError &err) {
        qWarning() << "SequencerError exception. Error code: " << err.code() << " ("
                   << err.qstrError() << ")";
        qWarning() << "Location: " << err.location();
        throw err;
    }
}

void
SynthRenderer::run()
{
    int pa_err;
    unsigned char data[m_bufferSize * sizeof (EAS_PCM) * m_channels];
    qDebug() << Q_FUNC_INFO << "started";
    try {
        m_Client->setRealTimeInput(false);
        m_Client->startSequencerInput();
        m_Stopped = false;
        m_isPlaying = false;
        if (m_files.length() > 0) {
            preparePlayback();
        }
        while (!stopped()) {
            EAS_RESULT eas_res;
            EAS_I32 numGen = 0;
            size_t bytes = 0;
            QCoreApplication::sendPostedEvents();
            if (m_isPlaying) {
                int t = getPlaybackLocation();
                emit playbackTime(t);
            }
            if (m_easData != 0)
            {
                EAS_PCM *buffer = (EAS_PCM *) data;
                eas_res = EAS_Render(m_easData, buffer, m_bufferSize, &numGen);
                if (eas_res != EAS_SUCCESS) {
                    qWarning() << "EAS_Render error:" << eas_res;
                }
                bytes += (size_t) numGen * sizeof(EAS_PCM) * m_channels;
                // hand over to pulseaudio the rendered buffer
                if (pa_simple_write (m_pulseHandle, data, bytes, &pa_err) < 0)
                {
                    qWarning() << "Error writing to PulseAudio connection:" << pa_err;
                }
            }
            if (m_isPlaying && playbackCompleted()) {
                closePlayback();
                if (m_files.length() == 0) {
                    m_isPlaying = false;
                    emit playbackStopped();
                } else {
                    preparePlayback();
                }
            }
            //qDebug() << Q_FUNC_INFO << pa_simple_get_latency(m_pulseHandle, &pa_err);
        }
        if (m_isPlaying) {
            closePlayback();
        }
        m_Client->stopSequencerInput();
    } catch (const SequencerError& err) {
        qWarning() << "SequencerError exception. Error code: " << err.code()
                   << " (" << err.qstrError() << ")";
        qWarning() << "Location: " << err.location();
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

    if (m_easData != 0 && m_streamHandle != 0)
    {
        count = m_codec->decode((unsigned char *)&buffer, sizeof(buffer), ev->getHandle());
        if (count > 0) {
            //qDebug() << Q_FUNC_INFO << QByteArray((char *)&buffer, count).toHex();
            eas_res = EAS_WriteMIDIStream(m_easData, m_streamHandle, buffer, count);
            if (eas_res != EAS_SUCCESS) {
                qWarning() << "EAS_WriteMIDIStream error: " << eas_res;
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
            qWarning() << "EAS_SetParameter error:" << eas_res;
        }
    }
    eas_res = EAS_SetParameter(m_easData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_BYPASS, sw);
    if (eas_res != EAS_SUCCESS) {
        qWarning() << "EAS_SetParameter error: " << eas_res;
    }
    //qDebug() << Q_FUNC_INFO << reverb_type << sw;
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
            qWarning() << "EAS_SetParameter error:" << eas_res;
        }
    }
    eas_res = EAS_SetParameter(m_easData, EAS_MODULE_CHORUS, EAS_PARAM_CHORUS_BYPASS, sw);
    if (eas_res != EAS_SUCCESS) {
        qWarning() << "EAS_SetParameter error:" << eas_res;
    }
    //qDebug() << Q_FUNC_INFO << chorus_type << sw;
}

int SynthRenderer::reverbWet()
{
    EAS_I32 wet = 0;
    EAS_RESULT eas_res = EAS_GetParameter(m_easData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_WET, &wet);
    if (eas_res != EAS_SUCCESS) {
        qWarning() << "EAS_GetParameter error:" << eas_res;
    }
    //qDebug() << Q_FUNC_INFO << wet;
    return wet;
}

void
SynthRenderer::setReverbWet(int amount)
{
    EAS_RESULT eas_res = EAS_SetParameter(m_easData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_WET, (EAS_I32) amount);
    if (eas_res != EAS_SUCCESS) {
        qWarning() << "EAS_SetParameter error:" << eas_res;
    }
    //qDebug() << Q_FUNC_INFO << amount;
}

int SynthRenderer::chorusLevel()
{
    EAS_I32 level = 0;
    EAS_RESULT eas_res = EAS_GetParameter(m_easData,
                                          EAS_MODULE_CHORUS,
                                          EAS_PARAM_CHORUS_LEVEL,
                                          &level);
    if (eas_res != EAS_SUCCESS) {
        qWarning() << "EAS_GetParameter error:" << eas_res;
    }
    //qDebug() << Q_FUNC_INFO << level;
    return level;
}

void
SynthRenderer::setChorusLevel(int amount)
{
    EAS_RESULT eas_res = EAS_SetParameter(m_easData, EAS_MODULE_CHORUS, EAS_PARAM_CHORUS_LEVEL, (EAS_I32) amount);
    if (eas_res != EAS_SUCCESS) {
        qWarning() << "EAS_SetParameter error:" << eas_res;
    }
    //qDebug() << Q_FUNC_INFO << amount;
}

void SynthRenderer::initSoundfont(const QString &dlsFile)
{
    if (m_soundfont != dlsFile) {
        m_soundfont = dlsFile;
        uninitEAS();
        initEAS();
    }
}

void
SynthRenderer::playFile(const QString fileName)
{
    qDebug() << Q_FUNC_INFO << fileName;
    m_files.append(fileName);
}

void
SynthRenderer::preparePlayback()
{
    EAS_HANDLE handle;
    EAS_RESULT result;
    EAS_I32 playTime;

    m_currentFile = new FileWrapper(m_files.first());
    m_files.removeFirst();

    /* call EAS library to open file */
    if ((result = EAS_OpenFile(m_easData, m_currentFile->getLocator(), &handle)) != EAS_SUCCESS)
    {
        qWarning() << "EAS_OpenFile" << result;
        return;
    }

    /* prepare to play the file */
    if ((result = EAS_Prepare(m_easData, handle)) != EAS_SUCCESS)
    {
        qWarning() << "EAS_Prepare" << result;
        return;
    }

    /* get play length */
    if ((result = EAS_ParseMetaData(m_easData, handle, &playTime)) != EAS_SUCCESS)
    {
        qWarning() << "EAS_ParseMetaData. result=" << result;
        return;
    }
    else
    {
        qDebug() << "EAS_ParseMetaData. playTime=" << playTime;
    }

    qDebug() << Q_FUNC_INFO;
    m_midiFileHandle = handle;
    m_isPlaying = true;
}

bool
SynthRenderer::playbackCompleted()
{
    EAS_RESULT result;
    EAS_STATE state = EAS_STATE_EMPTY;
    if ((result = EAS_State(m_easData, m_midiFileHandle, &state)) != EAS_SUCCESS)
    {
        qWarning() << "EAS_State:" << result;
    }
    //qDebug() << Q_FUNC_INFO << state;
    /* is playback complete */
    return ((state == EAS_STATE_STOPPED) || (state == EAS_STATE_ERROR));
}

void
SynthRenderer::closePlayback()
{
    qDebug() << Q_FUNC_INFO;
    EAS_RESULT result = EAS_SUCCESS;
    /* close the input file */
    if ((result = EAS_CloseFile(m_easData, m_midiFileHandle)) != EAS_SUCCESS)
    {
        qWarning() << "EAS_CloseFile" << result;
    }
    m_midiFileHandle = 0;
    delete m_currentFile;
    m_currentFile = 0;
    m_isPlaying = false;
}

int
SynthRenderer::getPlaybackLocation()
{
    EAS_I32 playTime = 0;
    EAS_RESULT result = EAS_SUCCESS;
    /* get the current time */
    if ((result = EAS_GetLocation(m_easData, m_midiFileHandle, &playTime)) != EAS_SUCCESS)
    {
        qWarning() << "EAS_GetLocation" << result;
    }
    //qDebug() << Q_FUNC_INFO << playTime;
    return playTime;
}

void
SynthRenderer::startPlayback(const QString fileName)
{
    if (!stopped())
    {
        playFile(fileName);
        preparePlayback();
    }
}

void
SynthRenderer::stopPlayback()
{
    if (!stopped()) {
        closePlayback();
    }
}
