Sonivox EAS for Linux and Qt
============================

This project is a Linux MIDI Synth library based on the Sonivox EAS Synthesizer published by Google on the Android Open Source Project.
It is a real time GM synthesizer without needing external soundfonts, using embedded samples instead. It consumes very little resources, so
it may be indicated in Linux projects for small embedded devices.

The library uses ALSA Sequencer MIDI input and PulseAudio output. Complete compile-time dependencies are:
* Qt5, http://www.qt.io/
* Drumstick 2, for ALSA MIDI input. http://sourceforge.net/projects/drumstick/
* PulseAudio, for audio output. http://www.freedesktop.org/wiki/Software/PulseAudio/

Just to clarify the Drumstick dependency: this project requires Drumstick::ALSA, but Drumstick does not depend on this project at all. There is a Drumstick::RT backend that includes the Sonivox synth as well, but both projects are independent regarding this synthesizer.

The project directory contains:
* cmdlnsynth: Command line sample program using the synthesizer library
* guisynth: GUI sample program using the synthesizer library
* libsvoxeas: The Linux synthesizer shared library, using ALSA Sequencer and PulseAudio
* sonivox: The AOSP source files, with a qmake project file to compile and test under QtCreator as a static library

Hacking
-------

Use your favorite IDE or text editor with the source files. My preference is QtCreator: https://www.qt.io/ide/
To build, test and debug you may also find QtCreator interesting. You may also use CMake (>= 3.0) to build the project instead of qmake.

Licenses
--------

Copyright (C) 2016-2021 Pedro López-Cabanillas.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.



This project includes code from the Sonivox EAS synthesizer, Copyright Sonic Network Inc. 2006. 
Sonivox EAS belongs to the Android Open Source Project. It has been forked from the official repositories:

https://android.googlesource.com/platform/external/sonivox/

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. 
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing 
permissions and limitations under the License.



The Android robot is reproduced or modified from work created and shared by Google and used according to terms described in the 
Creative Commons 3.0 Attribution License.
