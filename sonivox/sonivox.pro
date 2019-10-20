#----------------------------------
#
# Sonivox EAS library for QtCreator
#
#----------------------------------

QT -= core gui
QMAKE_LINK = $$QMAKE_LINK_C
QMAKE_CFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unused-value -Wno-unused-variable -Wno-unused-function

TARGET = sonivox
TEMPLATE = lib
VERSION = 3.6.10
CONFIG += staticlib \
    stdlib \
    warn_on

DEFINES += \
	UNIFIED_DEBUG_MESSAGES \
	EAS_WT_SYNTH \
#	_IMELODY_PARSER \
#	_RTTTL_PARSER \
#	_OTA_PARSER \
#	_XMF_PARSER \
	NUM_OUTPUT_CHANNELS=2 \
    _SAMPLE_RATE_22050 \
	MAX_SYNTH_VOICES=64 \
    _16_BIT_SAMPLES \
	_FILTER_ENABLED \
    DLS_SYNTHESIZER \
	_REVERB_ENABLED \
	_CHORUS_ENABLED

INCLUDEPATH += host_src lib_src

HEADERS += host_src/eas.h \
           host_src/eas_chorus.h \
           host_src/eas_reverb.h \
           host_src/eas_types.h

SOURCES += host_src/eas_config.c \
           host_src/eas_hostmm.c \
#           host_src/eas_main.c \
           host_src/eas_report.c \
           host_src/eas_wave.c \
           lib_src/eas_chorus.c \
           lib_src/eas_chorusdata.c \
           lib_src/eas_data.c \
           lib_src/eas_dlssynth.c \
           lib_src/eas_flog.c \
#           lib_src/eas_ima_tables.c \
#           lib_src/eas_imaadpcm.c \
#           lib_src/eas_imelody.c \
#           lib_src/eas_imelodydata.c \
           lib_src/eas_math.c \
           lib_src/eas_mdls.c \
           lib_src/eas_midi.c \
           lib_src/eas_mididata.c \
           lib_src/eas_mixbuf.c \
           lib_src/eas_mixer.c \
#           lib_src/eas_ota.c \
#           lib_src/eas_otadata.c \
           lib_src/eas_pan.c \
           lib_src/eas_pcm.c \
           lib_src/eas_pcmdata.c \
           lib_src/eas_public.c \
           lib_src/eas_reverb.c \
           lib_src/eas_reverbdata.c \
#           lib_src/eas_rtttl.c \
#           lib_src/eas_rtttldata.c \
           lib_src/eas_smf.c \
           lib_src/eas_smfdata.c \
           lib_src/eas_tcdata.c \
           lib_src/eas_tonecontrol.c \
           lib_src/eas_voicemgt.c \
 #          lib_src/eas_wavefile.c \
 #          lib_src/eas_wavefiledata.c \
           lib_src/eas_wtengine.c \
           lib_src/eas_wtsynth.c \
 #          lib_src/eas_xmf.c \
 #          lib_src/eas_xmfdata.c \
           lib_src/jet.c \
           lib_src/wt_22khz.c
