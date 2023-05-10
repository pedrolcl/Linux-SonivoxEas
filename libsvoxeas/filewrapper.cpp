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

#include <QDebug>
#include "filewrapper.h"

static int
readAt(void *handle, void *buffer, int pos, int size) {
    return static_cast<FileWrapper*>(handle)->readAt(buffer, pos, size);
}

static int
size(void *handle) {
    return static_cast<FileWrapper*>(handle)->size();
}

FileWrapper::FileWrapper(const QString &path)
    : m_ok{false}
    , m_Base{0}
    , m_Length{0}
    , m_easFile{}
{
    //qDebug() << Q_FUNC_INFO << path;
    m_file.setFileName(path);
    m_ok = m_file.open(QIODevice::ReadOnly);
    if (m_ok) {
        //qDebug("FileWrapper. opened %s", path);
        m_Length = m_file.size();
        m_easFile.handle = this;
        m_easFile.readAt = ::readAt;
        m_easFile.size = ::size;
    }
}

FileWrapper::FileWrapper(const char *path)
    : FileWrapper(QString::fromLocal8Bit(path))
{
    //qDebug("FileWrapper(path=%s)", path);
}

FileWrapper::~FileWrapper() {
    //qDebug("~FileWrapper");
    m_file.close();
}

int
FileWrapper::readAt(void *buffer, int offset, int size) {
    //qDebug("readAt(%p, %d, %d)", buffer, offset, size);
    m_file.seek(offset);
    if (offset + size > m_Length) {
        size = m_Length - offset;
    }
    return m_file.read((char *)buffer, size);
}

int
FileWrapper::size() {
    //qDebug("size() = %d", int(mLength));
    return m_Length;
}

bool FileWrapper::ok() const
{
    return m_ok;
}

EAS_FILE_LOCATOR
FileWrapper::getLocator() {
    return &m_easFile;
}
