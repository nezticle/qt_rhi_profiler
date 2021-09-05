/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "rhiprofilerparser.h"
#include "rhiprofilerglobals.h"

RhiProfilerParser::RhiProfilerParser(QObject *parent) : QObject(parent)
{

}

void RhiProfilerParser::feed(const QByteArray &line)
{
    const QList<QByteArray> elems = line.split(',');
    if (elems.count() < 4) {
        qWarning("Malformed line '%s'", line.constData());
        return;
    }
    bool ok = false;
    const int op = elems[0].toInt(&ok);
    if (!ok) {
        qWarning("Invalid op %s", elems[0].constData());
        return;
    }
    if (op < MIN_KNOWN_OP || op > MAX_KNOWN_OP) {
        qWarning("Unknown op %d", op);
        return;
    }

    Event e;
    e.op = QRhiProfiler::StreamOp(op);
    e.timestamp = elems[1].toLongLong();
    e.resource = elems[2].toULongLong();
    e.resourceName = elems[3];

    const int elemCount = elems.count();
    for (int i = 4; i < elemCount; i += 2) {
        if (i + 1 < elemCount && !elems[i].isEmpty() && !elems[i + 1].isEmpty()) {
            QByteArray key = elems[i];
            if (key.startsWith('F')) {
                key = key.mid(1);
                bool ok = false;
                const float value = elems[i + 1].toFloat(&ok);
                if (!ok) {
                    qWarning("Failed to parse float %s in line '%s'", elems[i + 1].constData(), line.constData());
                    continue;
                }
                Event::Param param;
                param.key = key;
                param.valueType = Event::Param::Float;
                param.floatValue = value;
                e.params.append(param);
            } else {
                const qint64 value = elems[i + 1].toLongLong();
                Event::Param param;
                param.key = key;
                param.valueType = Event::Param::Int64;
                param.intValue = value;
                e.params.append(param);
            }
        }
    }

    emit eventReceived(e);
}

const RhiProfilerParser::Event::Param *RhiProfilerParser::Event::param(const char *key) const {
    auto it = std::find_if(params.cbegin(), params.cend(), [key](const Param &p) {
        return !strcmp(p.key.constData(), key);
    });
    return it == params.cend() ? nullptr : &*it;
}
