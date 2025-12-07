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

#include "filewrapper.h"
#include <cstdio>

FileWrapper::FileWrapper(const QString &path)
    : FileWrapper(path.toLocal8Bit().data())
{}

FileWrapper::FileWrapper(const char *path)
    : m_ok{false}
    , m_easFile{}
{
    memset(&m_easFile, 0, sizeof(EAS_FILE));
    m_easFile.handle = fopen(path, "rb");
    m_ok = (m_easFile.handle != 0);
}

FileWrapper::~FileWrapper() {
    if (m_easFile.handle != 0) {
        fclose(reinterpret_cast<FILE *>(m_easFile.handle));
    }
}

bool FileWrapper::ok() const
{
    return m_ok;
}

EAS_FILE_LOCATOR
FileWrapper::getLocator() {
    return &m_easFile;
}
